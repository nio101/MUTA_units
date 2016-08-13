/*
 * MUTA operator / basecamp - r1 - 01/2016
 * 
 * Main source code: copyright (2015-2016) Nicolas Barthe,
 * distributed as open source under the terms
 * of the GNU General Public License (see COPYING.txt)
 * 
 * USB and MIWI original source code: copyright Microchip.
 * 
*/

#include <xc.h>         // should compile with XC8 v1.35 + legacy PLIB v2.00
#include <stdint.h>
#include <string.h>
#include <timers.h>     // remember: you must install manually legacy plib to
#include <pwm.h>        // those since XC5 v1.35 doesn't include plib anymore
#include "18F27J53/system.h"
#include "18F27J53/system_config.h"
#include "18F27J53/symbol.h"
#include "usb_config.h"
#include "usb/usb.h"
#include "usb/usb_device_hid.h"
#include "app_led_usb_status.h"
#include "miwi/miwi_api.h"
#include "drv_mrf_miwi_89xa.h"
#include "../../../muta/muta_messages.h"

/* miwi config is configured through the following files:
      config_89xa.h: TX power, bitrate...
      miwi_config.h: Miwi role/options

the initial output power is determined statically
by the value of TX_POWER set in config_89xa.h.
Current config: 1dBm => 1.2mW

Remember: Miwi stack uses TIMER1
 + drv_mrf_miwi_89xa.c has been patched for the LED to reflect Miwi activity
 (see drv_mrf_miwi_89xa.c, line #1475) */

/* note: clock frequency and MK1/MK3 #define are in system_config.h */

/* config words start @0x01FFF8
let's put some const at the end of flash memory:
    model of the unit (00: operator, 01:scout, 02:enforcer...)
    + make of the unit (1 for mk1, 2 for mk2...) */
#ifdef MK1
const uint8_t myMODEL[2] @ 0x01FFF4 = { 0x00, 0x01 }; // operator mk1
#else #ifdef MK3
const uint8_t myMODEL[2] @ 0x01FFF4 = { 0x00, 0x03 }; // operator mk3
#endif
// Unique ID used to identify the unit
const uint8_t myUID[2] @ 0x01FFF6 = { 0xFF, 0x01 }; // UID, must be unique!
extern uint8_t myLongAddress[4];    // equal to myMODEL+myUID
                                    // done at the beginning of main()
/* this is the 64bit key that will be used for encryption/decryption
!!!it should be the same for every unit!!!
algo: XTEA64 + CCM_16 */
const unsigned char StaticSecurityKey[8] @ 0x01FEC = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07  };
unsigned char mySecurityKey[8];
/* note: mySecurityKey will be initialized with StaticSecurityKey
 * then mySecurityKey[2] and mySecurityKey[6] will be replaced by
 * random values generated for each network session by the PC-side python script
 * and returned through NETWORK_REGISTER */

/* Memory mapping of myMODEL, myUID and mySecurityKey:
    myMODEL[2] @ 0x01FFF4
    myUID[2] @ 0x01FFF6
    mySecurityKey[8] @ 0x01FEC        
=> allow the use of HEXMATE (command line utility that comes with MPLABX)
to modify those value without recompiling the firmware
ex.: HEXMATE source.hex -FILL=0xBEEF@0x1000 -Odest.hex */

// Possible channel numbers are from 0 to 31
// channels 0,4,8,12,16,20,24,28 (energy scan duration: ~22sec)
//#define MUTA_CHANNELS      0x11111111
#define MUTA_CHANNELS      0x00000001

// to use the internal RTCC as a timer (with frequency==multiples of 1min, default @1min)
#define DEFAULT_UPDATE_FREQUENCY    1
volatile bool timer_flag = false;
volatile uint8_t timer_count = DEFAULT_UPDATE_FREQUENCY;

// Dedicated Operator Variables / values
const char POWER_LABEL[] = "Pwr";   // Operator TX Power level
const char PANID_LABEL[] = "PAN";   // PANid associated with the operator
const char CHANNEL_LABEL[] = "Chn"; // Channel used by the operator
const char NOISE_LABEL[] = "Nse";   // Noise level measured on the channel
uint8_t m_power, noise_level, BestChannel;
const char UPTIME_LABEL[] = "Upt";  // Uptime (since the last reset)
volatile unsigned long m_uptime_seconds = 0;  // updated every minute
const char UPDATE_FREQUENCY_LABEL[] = "UpF";    // Update frequency
volatile uint8_t m_update_frequency = DEFAULT_UPDATE_FREQUENCY;
unsigned long m_failed_messages = 0;
unsigned long m_sent_messages = 0;

// TinyPack encoding stuff
uint8_t m_buffer[32];
uint8_t * p_buffer;
MUTA_VARIABLE m_var;

// Main functions / helpers to keep the main code simple & clean
void timer_reset();
void send_network_register();
void send_complete_update();
void send_writables_update();
void send_periodic_update();
void update_variable();
void wait_for_1_sec();

// USB stuff
void APP_DeviceCustomHIDInitialize();
unsigned char ReceivedDataBuffer[64];
unsigned char ToSendDataBuffer[64];
unsigned char* pUsbBuffer;
bool sendHIDBinary(uint8_t* buffer, uint8_t length);   // send binary via USB
bool sendHIDText(char* str);                           // send text via USB
volatile USB_HANDLE USBOutHandle;
volatile USB_HANDLE USBInHandle;

// to make the led gently pulse (using PWM), instead of a raw blinking mode
#define LED_max_brightness  235
#define LED_min_brightness  10
volatile uint16_t LED_brightness = LED_max_brightness;
volatile uint8_t LED_increasing = 0;
typedef enum
{
    LED_OFF,
    LED_FAST_PULSE,
    LED_SLOW_PULSE,
    LED_FULL
} LED_STATE;
volatile LED_STATE   status_led;
void update_LED();
volatile uint8_t repeat_full;

APP_STATE   myAppState;     // to track USB states (from USB demo source code)

// --------------
//      main
// --------------
void main(void)
{
    SYSTEM_Initialize(); // PIC hardware setup/configuration
    
    //while(true)
    //{}
    
    myAppState = STATE_INIT;

    bool result;
    // set the unique LongAddress as the result of the unique myMODEL+myUID
    myLongAddress[0] = myMODEL[0];
    myLongAddress[1] = myMODEL[1];
    myLongAddress[2] = myUID[0];
    myLongAddress[3] = myUID[1];
    
    // initialize the security key
    for (int x=0; x<8; x++)
        mySecurityKey[x] = StaticSecurityKey[x];

    /* LED STATUS:
          LED FAST PULSE: Init/USB/Connection in progress
          LED SLOW : Ready
          LED OFF: network not found/problem encountered - waiting 1min before retry/reset
          BLINK: Receiving/sending data */
    status_led = LED_FAST_PULSE;
    update_LED();
    
    // USB initialization
    USBDeviceInit();
    /* WARNING! USBDeviceAttach will enable high and low interrupts
     * using a custom USBEnableInterrupts() in usb_hal_pic18.h
     * USB interrupts will be handled in low priority mode */
    USBDeviceAttach();
    while(1)
    {
        /* If the USB device isn't configured yet, we can't really do anything
         * else since we don't have a host to talk to.  So jump back to the
         * top of the while loop. */
        if( USBGetDeviceState() < CONFIGURED_STATE )
            continue;
        if( USBIsDeviceSuspended() == true )
            continue;
        myAppState = STATE_USB_READY;
        break;
    }
    // USB is ready here
    strcpy(ToSendDataBuffer, "I|RESET");
    sendHIDText(ToSendDataBuffer);
    __delay_ms(20);
    
    // A) DO A NETWORK_REGISTER THROUGH USB
    //=====================================
    send_network_register();
    while(1)
    {
        //Is there a new USB HID message from PC?
        if(HIDRxHandleBusy(USBOutHandle) == false)
        {
            switch (ReceivedDataBuffer[0]) // 'G' for go command
            {
                case PC_MESSAGE_SEND:
                    if (ReceivedDataBuffer[1] != 0x00 || ReceivedDataBuffer[2] != 0x00 \
                            || ReceivedDataBuffer[3] != 4 ||ReceivedDataBuffer[4] != MUTA_NETWORK_REGISTER \
                            || ReceivedDataBuffer[5] != MUTA_BOOL_TRUE)
                    {               // message is not as expected, not for operator, or operator not authorized
                        RESET();    // =>  RESET!, it will reset the PC script too
                    }
                    mySecurityKey[2] = ReceivedDataBuffer[6];
                    mySecurityKey[6] = ReceivedDataBuffer[7];
                    break;
                case PC_RESET:
                    RESET();
                    break;
                default:
                    // ignore other messages
                    break;
            }
            // free the buffer for the next USB message
            USBOutHandle = HIDRxPacket(CUSTOM_DEVICE_HID_EP, (uint8_t*)&ReceivedDataBuffer, 64);
            break;
        }
    }

    // B) CREATE THE MIWI NETWORK
    //===========================
    MiApp_ProtocolInit(false);  // init the Miwi stack
    m_power = TX_POWER;
    MiMAC_SetPower(m_power);   // set static power level
    
    // noise scan
    strcpy(ToSendDataBuffer, "I|scanning channels for noise...");
    sendHIDText(ToSendDataBuffer);            
    __delay_ms(20);
      
    // noise scan on the 8 selected channels takes 24 sec with '14'
    // to speed up dev phase, let's use '8' and 1 channel only
    BestChannel = MiApp_NoiseDetection(MUTA_CHANNELS, 8, NOISE_DETECT_ENERGY, &noise_level);
    
    sprintf(ToSendDataBuffer, "I|best channel: %i (noise == %i)", BestChannel, noise_level);
    sendHIDText(ToSendDataBuffer);
    __delay_ms(20);
    
    if( MiApp_SetChannel(BestChannel) == false )
    {
        sprintf(ToSendDataBuffer, "I|Error: unable to set channel %i! (T_T)", BestChannel);
        sendHIDText(ToSendDataBuffer);
        __delay_ms(20);
        return;
    }
	    
    sprintf(ToSendDataBuffer, "I|creating network...");
    sendHIDText(ToSendDataBuffer);
    __delay_ms(20);

    MiApp_StartConnection(START_CONN_DIRECT, 0, 0);
    MiApp_ConnectionMode(DISABLE_ALL_CONN);  // only for PAN/coordinators
        
    sprintf(ToSendDataBuffer, "I|PANID:%02x%02x Ch:%02d",myPANID.v[1],myPANID.v[0],BestChannel);
    sendHIDText(ToSendDataBuffer);
    __delay_ms(20);

    sprintf(ToSendDataBuffer, "I|operator's address: %02x%02x", myShortAddress.v[1], myShortAddress.v[0]);
    sendHIDText(ToSendDataBuffer);
    __delay_ms(20);
    
    sprintf(ToSendDataBuffer, "I|status: ready!");
    sendHIDText(ToSendDataBuffer);
    __delay_ms(20);
    // network is set up, but we are not responding to network scans
    
    // C) SEND A COMPLETE UPDATE
    //==========================
    send_complete_update();
    
    // Ok, init done, we can accept connections now
    MiApp_ConnectionMode(ENABLE_ALL_CONN);  // only for PAN/coordinators
    
    sprintf(ToSendDataBuffer, "I|accepting scans & connections");
    sendHIDText(ToSendDataBuffer);
    __delay_ms(20);
    
    myAppState = STATE_READY;
    status_led = LED_SLOW_PULSE;
    update_LED();
    
    m_uptime_seconds = 0;
    timer_reset();
    
    // D) MAIN LOOP
    //=============
    while(1)
    {
        uint8_t shortaddress[2];    // short address of sender
        uint8_t power;  // for POWER_TEST
        uint8_t rssi;
        uint8_t uid[2]; // for NETWORK_REGISTER
        uint8_t size;

        // D1) MIWI STUFF
        //===============
        MiWiTasks();    // should be called as often as possible
        
        // D2) INCOMING MIWI MESSAGES
        //===========================
        if(MiApp_MessageAvailable()) // we've received a Miwi message
        {
            shortaddress[0] = rxMessage.SourceAddress[0];
            shortaddress[1] = rxMessage.SourceAddress[1];
            switch(rxMessage.Payload[0])
            {
                case MUTA_POWER_TEST:
                    rssi = rxMessage.PacketRSSI;
                    power = rxMessage.Payload[1];
                    MiApp_DiscardMessage();
                    // power test -> answer with power+rssi
                    MiApp_FlushTx();
                    MiApp_WriteData(MUTA_POWER_TEST);
                    MiApp_WriteData(power);
                    MiApp_WriteData(rssi);
                    // send the message back using the short address + ENCRYPT payload
                    if (MiApp_UnicastAddress(&shortaddress, false, true) == true)
                    {
                        sprintf(ToSendDataBuffer, "I|%02x%02x:POWER_TEST: OK, %s, RSSI=%i", shortaddress[1], shortaddress[0], dBm_to_mW(power), rssi);
                        sendHIDText(ToSendDataBuffer);
                        //__delay_ms(20);
                    }
                    else
                    {
                        sprintf(ToSendDataBuffer, "I|%02x%02x:POWER_TEST: error sending back, %s, RSSI=%i", shortaddress[1], shortaddress[0], dBm_to_mW(power), rssi);
                        sendHIDText(ToSendDataBuffer);
                        //__delay_ms(20);
                    }
                    break;
                
                case MUTA_NETWORK_REGISTER:
                    // we received a network register, pass it to the PC script
                    p_buffer = &m_buffer[0];
                    *(p_buffer++) = PC_MESSAGE_RECEIVED;
                    *(p_buffer++) = shortaddress[0];
                    *(p_buffer++) = shortaddress[1];
                    size = rxMessage.PayloadSize;
                    memcpy(p_buffer, &rxMessage.Payload[0], size); // original msg
                    p_buffer += size;
                    // send to PC via USB in binary
                    sendHIDBinary(&m_buffer[0], (p_buffer-&m_buffer[0]));
                    MiApp_DiscardMessage();
                    break;      
                    
                case MUTA_PING: // PING message, sending back the same payload
                    MiApp_FlushTx();
                    size = rxMessage.PayloadSize;
                    memcpy(&TxBuffer[TxData], &rxMessage.Payload[0], size); // send back the same payload
                    TxData = TxData + size;
                    MiApp_DiscardMessage();
                    // send the message back using the short address + ENCRYPTION
                    if (MiApp_UnicastAddress(&shortaddress, false, true) == true)
                    {
                        sprintf(ToSendDataBuffer, "I|%02x%02x:PING successful: payload_size=%i", shortaddress[1], shortaddress[0], size);
                        sendHIDText(ToSendDataBuffer);
                        //__delay_ms(20);
                    }
                    else
                    {
                        sprintf(ToSendDataBuffer, "I|%02x%02x:PING error sending back: payload_size=%i", shortaddress[1], shortaddress[0], size);
                        sendHIDText(ToSendDataBuffer);
                        //__delay_ms(20);
                    }
                    break;
                
                case MUTA_UPDATE:
                    // we received an update, pass it to the PC script
                    p_buffer = &m_buffer[0];
                    *(p_buffer++) = PC_MESSAGE_RECEIVED;
                    *(p_buffer++) = shortaddress[0];
                    *(p_buffer++) = shortaddress[1];
                    size = rxMessage.PayloadSize;
                    memcpy(p_buffer, &rxMessage.Payload[0], size); // original msg
                    p_buffer += size;
                    // send to PC via USB in binary
                    sendHIDBinary(&m_buffer[0], (p_buffer-&m_buffer[0]));
                    MiApp_DiscardMessage();
                    break;      
                    
                default:
                    /* should not happen! */
                    sprintf(ToSendDataBuffer, "I|%02x%02x: %i command not recognized!", shortaddress[1], shortaddress[0], rxMessage.Payload[0]);
                    sendHIDText(ToSendDataBuffer);                    
            }   
        }
        
        // D3) INCOMING USB MESSAGES
        //==========================
        else if(HIDRxHandleBusy(USBOutHandle) == false)
        {   // we've received a command from the PC via USB
            switch (ReceivedDataBuffer[0])
            {
                case PC_RESET:  // used by the script to force reset
                    RESET();
                    break;
                case PC_MESSAGE_SEND:
                    // we have a message to send via Miwi
                    shortaddress[0] = ReceivedDataBuffer[1];
                    shortaddress[1] = ReceivedDataBuffer[2];
                    size = ReceivedDataBuffer[3]; // payload is in ReceivedDataBuffer[4:]
                    if ((shortaddress[0] == 0x00) && (shortaddress[1] == 0x00))
                    {   // if it's an update for me (PAN operator), just decode it and apply changes
                        // note: only UPDATE is sent directly from PC to operator
                        if ((ReceivedDataBuffer[4] == MUTA_UPDATE) && (size > 2))
                        {   // if it is an update message + message not empty
                            bool last_update = (ReceivedDataBuffer[5] == MUTA_BOOL_TRUE);
                            // decode the values
                            p_buffer = &ReceivedDataBuffer[6];
                            while ((p_buffer - &ReceivedDataBuffer[6]) < (size - 2))
                            {   // decode the next value and move p_buffer accordingly
                                m_var = decode_variable(p_buffer);
                                p_buffer += m_var.size;
                                // take the update order into account
                                /*sprintf(ToSendDataBuffer, "I|0000:update/value decoded:%s size=%i", m_var.label, m_var.size);
                                sendHIDText(ToSendDataBuffer);
                                __delay_ms(20);*/
                                update_variable();
                            }
                            if (last_update)
                            {   // all values decoded, answer if last_update
                                // send back a partial update
                                send_writables_update();
                            }
                        }
                    }
                    else
                    {   // if it's for somebody else, just send it
                        MiApp_FlushTx();
                        memcpy(&TxBuffer[TxData], &ReceivedDataBuffer[4], size); // send the payload
                        TxData = TxData + size;
                        if (ReceivedDataBuffer[4] == MUTA_NETWORK_REGISTER)
                        // send the message using the short address + NO ENCRYPTION
                            MiApp_UnicastAddress(&shortaddress, false, false) == true;
                        else // send the message using the short address + ENCRYPTION
                            MiApp_UnicastAddress(&shortaddress, false, true);
                        // take the update order into account
                        /*sprintf(ToSendDataBuffer, "I|%02x%02x: sending msg, payload:%i", shortaddress[1], shortaddress[0], size);
                        sendHIDText(ToSendDataBuffer);
                        __delay_ms(20);*/
                    }
                    break;
                default:
                    // ignore invalid messages
                    break;
            }
            // free the buffer for the next USB message
            USBOutHandle = HIDRxPacket(CUSTOM_DEVICE_HID_EP, (uint8_t*)&ReceivedDataBuffer, 64);
        }
        
        // D4) PERIODICAL UPDATE
        //======================
        else if (timer_flag == true)
        {
            send_periodic_update();
            timer_flag = false; // timer_count has been reset in the interrupt routine
        }
    }
}



/* send a NETWORK_REGISTER */
void send_network_register()
{
    // fill in temp buffer
    p_buffer = &m_buffer[0];
    *(p_buffer++) = PC_MESSAGE_RECEIVED;
    *(p_buffer++) = 0x00;   // it's coming from the PAN operator, (me)
    *(p_buffer++) = 0x00;
    //*(p_buffer++) = 0xff;   // fake RSSI
    *(p_buffer++) = MUTA_NETWORK_REGISTER;
    memcpy(p_buffer, &myLongAddress[0], 4); // UID
    p_buffer += 4;
#ifdef ENABLE_SLEEP // sleeping device or not ?
    *(p_buffer++) = MUTA_BOOL_TRUE;
#else
    *(p_buffer++) = MUTA_BOOL_FALSE;
#endif
    // send temp buffer to PC via USB in binary
    sendHIDBinary(&m_buffer[0], (p_buffer-&m_buffer[0]));
}



/* send an update message with all the variables/values */
void send_complete_update()
{
    uint8_t tmp;
    p_buffer = m_buffer;    // encode the UPDATE message in a temporary buffer
    *(p_buffer++) = PC_MESSAGE_RECEIVED;
    *(p_buffer++) = 0x00;   // it's coming from the PAN operator, (me)
    *(p_buffer++) = 0x00;
    *(p_buffer++) = MUTA_UPDATE;
    *(p_buffer++) = MUTA_BOOL_FALSE;    // ack_required
    // encode units variables/values now
    memcpy(m_var.label, POWER_LABEL, 3);
    m_var.unit = MUTA_NO_UNIT;
    m_var.value_byte1 = m_power;
    m_var.writable = true;
    p_buffer += encode_uint8_variable(p_buffer, m_var);
    memcpy(m_var.label, CHANNEL_LABEL, 3);
    m_var.unit = MUTA_NO_UNIT;
    m_var.value_byte1 = BestChannel;
    m_var.writable = false;
    p_buffer += encode_uint8_variable(p_buffer, m_var);
    memcpy(m_var.label, NOISE_LABEL, 3);
    m_var.unit = MUTA_NO_UNIT;
    m_var.value_byte1 = noise_level;
    m_var.writable = false;
    p_buffer += encode_uint8_variable(p_buffer, m_var);
    // send temp buffer to PC via USB in binary
    sendHIDBinary(m_buffer, (p_buffer-m_buffer));
    __delay_ms(20);

    p_buffer = m_buffer;    // encode the UPDATE message in a temporary buffer
    *(p_buffer++) = PC_MESSAGE_RECEIVED;
    *(p_buffer++) = 0x00;   // it's coming from the PAN operator, (me)
    *(p_buffer++) = 0x00;
    *(p_buffer++) = MUTA_UPDATE;
    *(p_buffer++) = MUTA_BOOL_TRUE;    // ack_required
    // encode units variables/values now
    // send the uptime
    memcpy(m_var.label, UPTIME_LABEL, 3);
    if (m_uptime_seconds < 60)  // send uptime as seconds
    {
        m_var.unit = MUTA_SECONDS_UNIT;
        m_var.value_byte1 = m_uptime_seconds;
        m_var.writable = false;
        p_buffer += encode_uint8_variable(p_buffer, m_var);        
    }
    else if (m_uptime_seconds < 3600)   // send uptime as minutes
    {
        m_var.unit = MUTA_MINUTES_UNIT;
        m_var.value_byte1 = m_uptime_seconds / 60;
        m_var.writable = false;
        p_buffer += encode_uint8_variable(p_buffer, m_var);        
    }
    else if (m_uptime_seconds < (86400))    // send uptime as hours
    {
        m_var.unit = MUTA_HOURS_UNIT;
        tmp = m_uptime_seconds / 3600;
        m_var.value_byte1 = tmp / 256;
        m_var.value_byte2 = tmp - (m_var.value_byte1 * 256);
        m_var.writable = false;
        p_buffer += encode_uint16_variable(p_buffer, m_var);        
    }
    else
    {   // send uptime as days
        m_var.unit = MUTA_NO_UNIT;
        tmp = m_uptime_seconds / 86400;
        m_var.value_byte1 = tmp / 256;
        m_var.value_byte2 = tmp - (m_var.value_byte1 * 256);
        m_var.writable = false;
        p_buffer += encode_uint16_variable(p_buffer, m_var);        
    }
    // send the update frequency
    memcpy(m_var.label, UPDATE_FREQUENCY_LABEL, 3);
    m_var.unit = MUTA_MINUTES_UNIT;
    m_var.value_byte1 = m_update_frequency;
    m_var.writable = true;
    p_buffer += encode_uint8_variable(p_buffer, m_var);
    // send the message
    sendHIDBinary(m_buffer, (p_buffer-m_buffer));
    __delay_ms(20);
}


/* send an update message with only the variables/values that can be modified */
void send_writables_update()
{   uint8_t tmp;
    p_buffer = m_buffer;    // encode the UPDATE message in a temporary buffer
    *(p_buffer++) = PC_MESSAGE_RECEIVED;
    *(p_buffer++) = 0x00;   // it's coming from the PAN operator, (me)
    *(p_buffer++) = 0x00;
    *(p_buffer++) = MUTA_UPDATE;
    *(p_buffer++) = MUTA_BOOL_TRUE;    // last_one
    // encode units variables/values now
    // send power level
    memcpy(m_var.label, POWER_LABEL, 3);
    m_var.unit = MUTA_NO_UNIT;
    m_var.value_byte1 = m_power;
    m_var.writable = true;
    p_buffer += encode_uint8_variable(p_buffer, m_var);
    // send the update frequency
    memcpy(m_var.label, UPDATE_FREQUENCY_LABEL, 3);
    m_var.unit = MUTA_MINUTES_UNIT;
    m_var.value_byte1 = m_update_frequency;
    m_var.writable = true;
    p_buffer += encode_uint8_variable(p_buffer, m_var);
    // send temp buffer to PC via USB in binary
    sendHIDBinary(m_buffer, (p_buffer-m_buffer));
    __delay_ms(20);
}


/* send an update message with only the variables/values that vary periodically */
void send_periodic_update()
{   uint8_t tmp;
    p_buffer = m_buffer;    // encode the UPDATE message in a temporary buffer
    *(p_buffer++) = PC_MESSAGE_RECEIVED;
    *(p_buffer++) = 0x00;   // it's coming from the PAN operator, (me)
    *(p_buffer++) = 0x00;
    *(p_buffer++) = MUTA_UPDATE;
    *(p_buffer++) = MUTA_BOOL_TRUE;    // last_one
    // encode units variables/values now
    // send the uptime
    memcpy(m_var.label, UPTIME_LABEL, 3);
    if (m_uptime_seconds < 60)  // send uptime as seconds
    {
        m_var.unit = MUTA_SECONDS_UNIT;
        m_var.value_byte1 = m_uptime_seconds;
        m_var.writable = false;
        p_buffer += encode_uint8_variable(p_buffer, m_var);        
    }
    else if (m_uptime_seconds < 3600)   // send uptime as minutes
    {
        m_var.unit = MUTA_MINUTES_UNIT;
        m_var.value_byte1 = m_uptime_seconds / 60;
        m_var.writable = false;
        p_buffer += encode_uint8_variable(p_buffer, m_var);        
    }
    else if (m_uptime_seconds < (86400))    // send uptime as hours
    {
        m_var.unit = MUTA_HOURS_UNIT;
        tmp = m_uptime_seconds / 3600;
        m_var.value_byte1 = tmp / 256;
        m_var.value_byte2 = tmp - (m_var.value_byte1 * 256);
        m_var.writable = false;
        p_buffer += encode_uint16_variable(p_buffer, m_var);        
    }
    else
    {   // send uptime as days
        m_var.unit = MUTA_NO_UNIT;
        tmp = m_uptime_seconds / 86400;
        m_var.value_byte1 = tmp / 256;
        m_var.value_byte2 = tmp - (m_var.value_byte1 * 256);
        m_var.writable = false;
        p_buffer += encode_uint16_variable(p_buffer, m_var);        
    }
    // send temp buffer to PC via USB in binary
    sendHIDBinary(m_buffer, (p_buffer-m_buffer));
    __delay_ms(20);
}



/* update the variables */
void update_variable()
{   // m_var handles all the update info received, now process them
    // and update the corresponding variable/value accordingly
    if (strcmp(POWER_LABEL, m_var.label) == 0)   // new value for power
    {
        m_power = m_var.value_byte1;
        if ((m_power >= 1) && (m_power <= 7))
            MiMAC_SetPower(m_power);
    }
    else if ((strcmp(UPDATE_FREQUENCY_LABEL, m_var.label) == 0) \
            && (m_var.unit == MUTA_MINUTES_UNIT) && (m_var.type == MUTA_UINT8_TYPE))
    {
        if (m_var.value_byte1 == 0) // special test/debug mode: timer every 10sec
        {
            m_update_frequency = 0;
            RtccWrOn(); //write enable the rtcc registers
            RTCCFGbits.RTCPTR0 = 0; //RTCVALH/L will point to min/seconds values
            RTCCFGbits.RTCPTR1 = 0;
            RTCVALHbits.RTCVALH = 0x00; // write 00 to min & sec
            RTCVALLbits.RTCVALL = 0x00;
            ALRMCFGbits.AMASK = 0b0010; // alarm every 10sec
            mRtccWrOff();   // write disable rtcc regs
            timer_count = 1;
            timer_flag = false;   
        }
        else
        {
            m_update_frequency = m_var.value_byte1;
            RtccWrOn(); //write enable the rtcc registers
            RTCCFGbits.RTCPTR0 = 0; //RTCVALH/L will point to min/seconds values
            RTCCFGbits.RTCPTR1 = 0;
            RTCVALHbits.RTCVALH = 0x00; // write 00 to min & sec
            RTCVALLbits.RTCVALL = 0x00;
            ALRMCFGbits.AMASK = 0b0011; // alarm every minute
            mRtccWrOff();   // write disable rtcc regs
            timer_count = m_update_frequency;
            timer_flag = false;
        }
    }
}

void timer_reset()
{
    /*RtccWrOn(); //write enable the rtcc registers
    RTCCFGbits.RTCPTR0 = 0; //RTCVALH/L will point to min/seconds values
    RTCCFGbits.RTCPTR1 = 0;
    RTCVALHbits.RTCVALH = 0x00; // write 00 to min & sec
    RTCVALLbits.RTCVALL = 0x00;
    mRtccWrOff();   // write disable rtcc regs
    */
    if (m_update_frequency == 0) // special debug mode:every 10sec
        timer_count = 1;
    else
        timer_count = m_update_frequency;
    timer_flag = false;
}

/** USB FUNCTIONS ***************************************************/

/* Initializes the Custom HID demo code */
void APP_DeviceCustomHIDInitialize()
{
    //initialize the variable holding the handle for the last
    // transmission
    USBInHandle = 0;
    //enable the HID endpoint
    USBEnableEndpoint(CUSTOM_DEVICE_HID_EP, USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
    //Re-arm the OUT endpoint for the next packet
    USBOutHandle = (volatile USB_HANDLE)HIDRxPacket(CUSTOM_DEVICE_HID_EP,(uint8_t*)&ReceivedDataBuffer,64);
}


/* send HID binary data */
bool sendHIDBinary(char* buffer, uint8_t length)
{ 
    if ((USBGetDeviceState() < CONFIGURED_STATE)||(USBIsDeviceSuspended() == true))
    {
        return false;
    }
    while (1)
    {
        if(!HIDTxHandleBusy(USBInHandle))
        {
            memcpy(&ToSendDataBuffer[0], buffer, length);
            USBInHandle = HIDTxPacket(CUSTOM_DEVICE_HID_EP, &ToSendDataBuffer[0],length);
            return true;
        }
    }
    return true;    // to prevent compiler warning
}


/* send HID text data */
bool sendHIDText(char* str)
{
    if ((USBGetDeviceState() < CONFIGURED_STATE)||(USBIsDeviceSuspended() == true))
    {
        return false;
    }
    while (1)
    {
        if(!HIDTxHandleBusy(USBInHandle))
        {
            uint8_t* p = &ToSendDataBuffer[0];
            int size = 0;
            uint8_t c;
            while ( c = *str++ )
            {
                *p = c;
                p++;
                size++;
            }
            USBInHandle = HIDTxPacket(CUSTOM_DEVICE_HID_EP, &ToSendDataBuffer[0],size);
            return true;
        }
    }
    return true;    // to prevent compiler warning
}


/* update the LED pulsing pattern */
void update_LED()
{
    switch(status_led)
    {
        case LED_OFF:
            // duty cycle == 0
            CCP1CONbits.DC1B0 = 0; //set low bit
            CCP1CONbits.DC1B1 = 0;  //set second lowest
            CCPR1L = 0; //set highest eight
            LED_increasing = 1;
            break;

        case LED_FULL:
            // duty cycle == 1023
            CCP1CONbits.DC1B0 = 1023 & 1; //set low bit
            CCP1CONbits.DC1B1 = (1023 >> 1) & 1;  //set second lowest
            CCPR1L = (1023 >> 2); //set highest eight
            repeat_full = 4;
            break;

        case LED_FAST_PULSE:
            T4CONbits.T4CKPS = 0b01;    // :4 prescaler
            // push max duty cycle value for LED PWM
            CCP1CONbits.DC1B0 = LED_max_brightness & 1; //set low bit
            CCP1CONbits.DC1B1 = (LED_max_brightness >> 1) & 1;  //set second lowest
            CCPR1L = (LED_max_brightness >> 2); //set highest eight
            LED_increasing = 0;
            break;

        case LED_SLOW_PULSE:
            T4CONbits.T4CKPS = 0b11;    // :16 prescaler
            // push max duty cycle value for LED PWM
            CCP1CONbits.DC1B0 = LED_max_brightness & 1; //set low bit
            CCP1CONbits.DC1B1 = (LED_max_brightness >> 1) & 1;  //set second lowest
            CCPR1L = (LED_max_brightness >> 2); //set highest eight
            LED_increasing = 0;
            break;
    }
}


/* Note: Microchip USB stack has been modded here to use USB interrupts
   in low priority: usb_hal_pic18.h / line 444 */
// and MiWi file drv_mrf_miwi_89xa.c was patched too to have one single
// interrupt handler function:MIWIInterruptHandler()

// High priority interrupt handler
void interrupt SYS_InterruptHigh(void)
{
    MIWIInterruptHandler();
}


// Low priority interrupt handler
// The USB stack uses low priority interrupts
void interrupt low_priority SYS_InterruptLow(void)
{
    USBDeviceTasks();
    if (PIR3bits.TMR4IF)    // interrupt for LED pulse, every 5ms@16MHz
    {
        PIR3bits.TMR4IF = 0;    // clear flag
        if ((status_led == LED_SLOW_PULSE)||(status_led == LED_FAST_PULSE))
        {
            if (LED_increasing == 1)
            {
                LED_brightness = LED_brightness + 2;
                if (LED_brightness >= LED_max_brightness)
                {
                    LED_brightness = LED_max_brightness;
                    LED_increasing = 0;
                }
            }
            else
            {
                LED_brightness = LED_brightness - 2;
                if (LED_brightness <= LED_min_brightness)
                {
                    LED_brightness = LED_min_brightness;
                    LED_increasing = 1;
                }
            }
            // push new duty cycle value for LED PWM
            CCP1CONbits.DC1B0 = LED_brightness & 1; //set low bit
            CCP1CONbits.DC1B1 = (LED_brightness >> 1) & 1;  //set second lowest
            CCPR1L = (LED_brightness >> 2); //set highest eight
        }
        if (status_led == LED_FULL)
        {
            repeat_full = repeat_full - 1;
            if (repeat_full == 0)
            {
                if (myAppState == STATE_INIT)
                    status_led = LED_FAST_PULSE;
                else if (myAppState == STATE_USB_READY)
                    status_led = LED_FAST_PULSE;
                else if (myAppState == STATE_READY)
                    status_led = LED_SLOW_PULSE;
                else if (myAppState == STATE_PROBLEM)
                    status_led = LED_OFF;
                update_LED();
            }
        }
    }
    else if(PIR3bits.RTCCIF) // RTCC every minute interrupt
    {
        m_uptime_seconds = m_uptime_seconds + 64;
        timer_count = timer_count - 1;
        if (timer_count == 0)
        {
            timer_flag = true;
            if (m_update_frequency == 0) // special debug mode:every 10sec
                timer_count = 1;
            else
                timer_count = m_update_frequency;
        }
        PIR3bits.RTCCIF = 0;
    }
}


// USB event handler
bool USER_USB_CALLBACK_EVENT_HANDLER(USB_EVENT event, void *pdata, uint16_t size)
{
    switch((int)event)
    {
        case EVENT_TRANSFER:
            break;

        case EVENT_SOF:
            /* We are using the SOF as a timer to time the LED indicator.  Call
             * the LED update function here. */
            APP_LEDUpdateUSBStatus();
            break;

        case EVENT_SUSPEND:
            /* Update the LED status for the suspend event. */
            APP_LEDUpdateUSBStatus();
            break;

        case EVENT_RESUME:
            /* Update the LED status for the resume event. */
            APP_LEDUpdateUSBStatus();
            break;

        case EVENT_CONFIGURED:
            /* When the device is configured, we can (re)initialize the demo
             * code. */
            APP_DeviceCustomHIDInitialize();
            break;

        case EVENT_SET_DESCRIPTOR:
            break;

        case EVENT_EP0_REQUEST:
            /* We have received a non-standard USB request.  The HID driver
             * needs to check to see if the request was for it. */
            USBCheckHIDRequest();
            break;

        case EVENT_BUS_ERROR:
            break;

        case EVENT_TRANSFER_TERMINATED:
            break;

        default:
            break;
    }
    return true;
}

void wait_for_1_sec()
{
    unsigned int i;
    for (i=0; i<100; i++)
    {
        __delay_ms(10);
    }
}