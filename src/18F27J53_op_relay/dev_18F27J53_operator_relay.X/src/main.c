/*
 * MUTA operator / relay - r1 - 01/2016
 * 
 * Main source code: copyright (2015-2016) Nicolas Barthe,
 * distributed as open source under the terms
 * of the GNU General Public License (see COPYING.txt)
 * 
 * USB and MIWI original code: copyright Microchip.
 * 
*/

#include <xc.h>         // should compile with XC8 v1.35 + legacy PLIB v2.00
#include <stdint.h>
#include <string.h>
#include <i2c.h>
#include <rtcc.h>
#include <timers.h>     // remember: you must install manually legacy plib to
#include <pwm.h>        // those since XC5 v1.35 doesn't include plib anymore
#include <math.h>
#include "18F27J53/system.h"
#include "18F27J53/system_config.h"
#include "18F27J53/symbol.h"
#include "miwi/miwi_api.h"
#include "miwi/drv_mrf_miwi_89xa.h"
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

/* config words start @0x01FFF8
let's put some const at the end of flash memory:
    model of the unit (00: operator, 01:scout, 02:enforcer...)
    + make of the unit (1 for mk1, 2 for mk2...) */
const uint8_t myMODEL[2] @ 0x01FFF4 = { 0x00, 0x03 }; // op mk3
// Unique ID used to identify the unit
const uint8_t myUID[2] @ 0x01FFF6 = { 0xFF, 0x03 }; // uid
/* those info are mapped @ 0x01FFF4 and 0x01FFF6, and can then be modified
 * without recompiling the firmware, if needed (using the proper tool) */
extern uint8_t myLongAddress[4];    // equal to myMODEL+myUID
                                    // done at the beginning of main()

// this is the 64bit key that will be used for encryption/decryption
// !!!it should be the same for every unit!!!
// algo: XTEA64 + CCM_16
// the key is mapped @ 0x01FEC and can then be modified without recompiling
// the firmware, if needed (using the proper tool)
const unsigned char StaticSecurityKey[8] @ 0x01FEC = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07  };
unsigned char mySecurityKey[8];
/* note: mySecurityKey will be initialized with StaticSecurityKey
 * then mySecurityKey[2] and mySecurityKey[6] will be replaced by
 * random values generated for each network session, and returned through NETWORK_REGISTER */

/* Memory mapping of myMODEL, myUID and mySecurityKey:
    myMODEL[2] @ 0x01FFF4
    myUID[2] @ 0x01FFF6
    mySecurityKey[8] @ 0x01FEC        
=> allow the use of HEXMATE (command line utility that comes with MPLABX)
to modify those value without recompiling the firmware
ex.: HEXMATE source.hex -FILL=0xBEEF@0x1000 -Odest.hex */
// Possible channel numbers are from 0 to 31
// channels 0,4,8,12,16,20,24,28 (~22sec max))
//#define MUTA_CHANNELS      0x11111111
#define MUTA_CHANNELS      0x00000001

#define MINIMAL_RSSI    33
#define MUTA_PANID   0x1337

#define MAX_CONSECUTIVE_FAILS   3

// to use the internal RTCC as a timer (with frequency==multiples of 1min, default @1min)
#define DEFAULT_UPDATE_FREQUENCY    1
volatile bool timer_flag = false;
volatile uint8_t timer_count = DEFAULT_UPDATE_FREQUENCY;

// Variables / values
const char POWER_LABEL[] = "Pwr";
uint8_t m_power;
const char UPTIME_LABEL[] = "Upt";
volatile unsigned long m_uptime_seconds = 0;  // updated every minute
const char UPDATE_FREQUENCY_LABEL[] = "UpF";
volatile uint8_t m_update_frequency = DEFAULT_UPDATE_FREQUENCY;
const char FAILEDMSG_LABEL[] = "FlM";
uint8_t consecutive_fails = 0;
unsigned long m_failed_messages = 0;
unsigned long m_sent_messages = 0;
        
// TinyPack encoding stuff
uint8_t m_buffer[32];
uint8_t * p_buffer;
MUTA_VARIABLE m_var;

// Main functions / helpers to keep the main code simple & clean
bool send_complete_update();
bool send_writables_update();
bool send_periodic_update();
void update_variable();
void timer_reset();
void wait_for_1_sec();

// to make the led pulse
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

APP_STATE   myAppState; // track App status


// --------------
//      main
// --------------
void main() {
    SYSTEM_Initialize(); // PIC hardware setup/configuration
    
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
          LED FAST PULSE: Init/Connection in progress
          LED SLOW : Ready
          LED OFF: problem encountered - waiting 1min before retry/reset
          BLINK: Receiving/sending data */
    status_led = LED_FAST_PULSE;
    update_LED();   

    MiApp_ProtocolInit(false);  // init the Miwi stack
    MiMAC_SetPower(TX_POWER);   // use max power for scan process
    
    // A) SEARCH FOR A NETWORK WITH PANID == MUTA_PANID
    //=================================================
    volatile uint8_t scanresult;
    while(1)
    {
        scanresult = MiApp_SearchConnection(14, MUTA_CHANNELS);
        if (scanresult == 0)
        {
            // no candidate found, let's wait 1min
            // before retrying
            status_led = LED_OFF;
            update_LED();
            timer_reset();
            while(!timer_flag)
            {}
        }
        else
        {
            break;
        }
    }
    uint8_t best_RSSI = 0;
    uint8_t best_candidate;
    for(uint8_t j = 0; j < scanresult; j++)
    {   
        if (ActiveScanResults[j].RSSIValue >= best_RSSI && ActiveScanResults[j].PANID.Val == MUTA_PANID)
        {
            best_RSSI = ActiveScanResults[j].RSSIValue;
            best_candidate = j;
        }
    }
    
    // B) JOIN THE NETWORK
    //====================
    uint8_t Status = MiApp_EstablishConnection(best_candidate, CONN_MODE_DIRECT);
    if(Status == 0xFF)
    {  
        // joining network failed!
        // let's wait 1min before reset
        myAppState = STATE_PROBLEM;
        status_led = LED_OFF;
        update_LED();
        timer_reset();
        while(!timer_flag)
        {}
        RESET();
    }
    MiApp_ConnectionMode(DISABLE_ALL_CONN);  // no scan or join for now
        
    // C) NETWORK_REGISTER
    //====================
#ifdef ENABLE_SLEEP // sleeping device or not ?
    result = do_NETWORK_REGISTER(&myLongAddress[0],MUTA_BOOL_TRUE);
#else
    result = do_NETWORK_REGISTER(&myLongAddress[0],MUTA_BOOL_FALSE);
#endif
    if (!result)
    {
        // auth failed!
        // wait 1min & reset
        myAppState = STATE_PROBLEM;
        status_led = LED_OFF;
        update_LED();
        timer_reset();
        while(!timer_flag)
        {}
        RESET();
    }
    
    // force security key update on
    security_force_key_update();
    
    myAppState = STATE_READY;
    status_led = LED_SLOW_PULSE;
    update_LED();
    
    // D) POWER LEVEL DYNAMIC ADAPTATION
    // =================================
    m_power = adapt_power_level(TX_POWER, MINIMAL_RSSI);
    
    // E) RESET UPTIME, TIMER, & SEND UPDATE
    // =====================================
    m_uptime_seconds = 0;
    while (!send_complete_update())
    {
        // failed attempt
        consecutive_fails++;
        if (consecutive_fails >= MAX_CONSECUTIVE_FAILS)
        {
            // wait 1min & reset!
            myAppState = STATE_PROBLEM;
            status_led = LED_OFF;
            update_LED();
            timer_reset();
            while(!timer_flag)
            {}
            RESET();
        }
        wait_for_1_sec(); // or not ?
    }
    consecutive_fails = 0;
    timer_reset();
    
    // F) MAIN LOOP: ANSWERS TO POWER_TEST, PING, AND PERIODICAL UPDATES
    // =================================================================
    MiApp_ConnectionMode(ENABLE_ALL_CONN);  // other units can scan & join
    
    uint8_t shortaddress[2];    // short address of sender
    uint8_t power;  // for POWER_TEST
    uint8_t rssi;
    uint8_t size;
    
    while(1) {        
        // F1) MIWI STUFF
        // ==============
        MiWiTasks();
        // F2) MIWI MESSAGES
        // =================
        if(MiApp_MessageAvailable())
        {
            shortaddress[0] = rxMessage.SourceAddress[0];
            shortaddress[1] = rxMessage.SourceAddress[1];
            size = rxMessage.PayloadSize;
            switch(rxMessage.Payload[0])
            {
                case MUTA_POWER_TEST:
                    rssi = rxMessage.PacketRSSI;
                    power = rxMessage.Payload[1];
                    MiApp_DiscardMessage();
                    // response
                    MiApp_FlushTx();
                    MiApp_WriteData(MUTA_POWER_TEST);
                    MiApp_WriteData(power);
                    MiApp_WriteData(rssi);
                    // send the message back using the short address + ENCRYPT payload
                    MiApp_UnicastAddress(&shortaddress, false, true);
                    break;
                                    
                case MUTA_PING: // PING message, sending back the same payload
                    MiApp_FlushTx();
                    memcpy(&TxBuffer[TxData], &rxMessage.Payload[0], size); // send back the same payload
                    TxData = TxData + size;
                    MiApp_DiscardMessage();
                    // send the message back using the short address + ENCRYPT payload
                    MiApp_UnicastAddress(&shortaddress, false, true);
                    break;
                
                case MUTA_UPDATE: // UPDATE message
                    // if it's for me, coming from PAN operator, and not empty,
                    // then process it
                    if ((shortaddress[0] == 0x00) && (shortaddress[1] == 0x00) && (size > 2))
                    {
                        bool ack_required = rxMessage.Payload[1];
                        // decode the values
                        p_buffer = &rxMessage.Payload[2];
                        while ((p_buffer - &rxMessage.Payload[2]) < (size - 2))
                        {   // decode the next value and move p_buffer accordingly
                            m_var = decode_variable(p_buffer);
                            p_buffer += m_var.size;
                            // take the update order into account
                            update_variable();
                        }
                        MiApp_DiscardMessage();
                        if (ack_required)
                        {   // all values decoded, answer if last_update
                            while (!send_writables_update())
                            {
                                // failed attempt
                                consecutive_fails++;
                                if (consecutive_fails >= MAX_CONSECUTIVE_FAILS)
                                {
                                    // wait 1min & reset!
                                    myAppState = STATE_PROBLEM;
                                    status_led = LED_OFF;
                                    update_LED();
                                    timer_reset();
                                    while(!timer_flag)
                                    {}
                                    RESET();
                                }
                                wait_for_1_sec(); // or not ?
                            }
                            consecutive_fails = 0;
                        }
                    }
                    else
                        MiApp_DiscardMessage();
                    
                default:
                    MiApp_DiscardMessage();
            }
        }
        // F3) PERIODIC UPDATES
        // ====================
        else if (timer_flag == true)
        {
            while (!send_periodic_update())
            {
                // failed attempt
                consecutive_fails++;
                if (consecutive_fails >= MAX_CONSECUTIVE_FAILS)
                {
                    // wait 1min & reset!
                    myAppState = STATE_PROBLEM;
                    status_led = LED_OFF;
                    update_LED();
                    timer_reset();
                    while(!timer_flag)
                    {}
                    RESET();
                }
                wait_for_1_sec(); // or not ?
            }
            consecutive_fails = 0;
            timer_flag = false; // timer_count has been reset already
        }
    };
}


/* send an update with all the variables */
bool send_complete_update()
{
    uint16_t tmp;
    // FIRST PART OF THE UPDATE
    p_buffer = m_buffer;    // encode the variables/values in a temporary buffer
    // encode units variables/values now
    // send the power level
    memcpy(m_var.label, POWER_LABEL, 3);
    m_var.unit = MUTA_NO_UNIT;
    m_var.value_byte1 = m_power;
    m_var.writable = true;
    p_buffer += encode_uint8_variable(p_buffer, m_var);
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
    if (!do_UPDATE(m_buffer, p_buffer-m_buffer, false))
        return false;
    // SECOND PART OF THE UPDATE
    p_buffer = m_buffer;    // encode the variables/values in a temporary buffer
    // send the failed msg percentage
    memcpy(m_var.label, FAILEDMSG_LABEL, 3);
    m_var.unit = MUTA_PERCENT_UNIT;
    float tmp2 = (float)(m_failed_messages*100)/(float)(m_failed_messages + m_sent_messages); // 33.33
    m_var.value_byte1 = (uint8_t)floor(tmp2);  // integer part
    m_var.value_byte2 = (uint8_t)floor(((tmp2 - floor(tmp2))*100));  // +2 digits
    m_var.writable = false;
    p_buffer += encode_ufixed16_variable(p_buffer, m_var);
    // send the message
    return do_UPDATE(m_buffer, p_buffer-m_buffer, true);
}


/* send an update with all the writables variables */
bool send_writables_update()
{
    uint16_t tmp;
    // FIRST PART OF THE UPDATE
    p_buffer = m_buffer;    // encode the variables/values in a temporary buffer
    // encode units variables/values now
    // send the power level
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
    // send the message
    return do_UPDATE(m_buffer, p_buffer-m_buffer, true);
}


/* send an update with all the variables that evolve periodically */
bool send_periodic_update()
{
    uint16_t tmp;
    p_buffer = m_buffer;
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
    // send the failed msg percentage
    memcpy(m_var.label, FAILEDMSG_LABEL, 3);
    m_var.unit = MUTA_PERCENT_UNIT;
    float tmp2 = (float)(m_failed_messages*100)/(float)(m_failed_messages + m_sent_messages); // 33.33
    m_var.value_byte1 = (uint8_t)floor(tmp2);  // integer part
    m_var.value_byte2 = (uint8_t)floor(((tmp2 - floor(tmp2))*100));  // +2 digits
    m_var.writable = false;
    p_buffer += encode_ufixed16_variable(p_buffer, m_var);
    // send the message
    return do_UPDATE(m_buffer, p_buffer-m_buffer, true);    
}


/* handle an update order received for a variable */
void update_variable()
{
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


/* reset the timer */
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
            break;

        case LED_FULL:
            // duty cycle == 1023
            CCP1CONbits.DC1B0 = 1023 & 1; //set low bit
            CCP1CONbits.DC1B1 = (1023 >> 1) & 1;  //set second lowest
            CCPR1L = (1023 >> 2); //set highest eight
            repeat_full = 2;
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


// High priority interrupt handler
void interrupt SYS_InterruptHigh(void)
{
    MIWIInterruptHandler();
}


// Low priority interrupt handler
// The USB stack uses low priority interrupts
void interrupt low_priority SYS_InterruptLow(void)
{
    if (PIR3bits.TMR4IF)    // interrupt for LED pulse, every 10ms@8MHz
    {
        PIR3bits.TMR4IF = 0;    // clear flag
        if ((status_led == LED_SLOW_PULSE)||(status_led == LED_FAST_PULSE))
        {
            if (LED_increasing == 1)
            {
                LED_brightness = LED_brightness + 4;
                if (LED_brightness >= LED_max_brightness)
                {
                    LED_brightness = LED_max_brightness;
                    LED_increasing = 0;
                }
            }
            else
            {
                LED_brightness = LED_brightness - 4;
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


void wait_for_1_sec()
{
    unsigned int i;
    for (i=0; i<100; i++)
    {
        __delay_ms(10);
    }
}
