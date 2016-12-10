/*
 * MUTA scout / dormant - r2 (MUTA v01) - 12/2016
 * 
 * Main source code: copyright (2015-2016) Nicolas Barthe,
 * distributed as open source under the terms
 * of the GNU General Public License (see COPYING.txt)
 * 
 * USB and MIWI original code: copyright Microchip.
 * 
*/

#include <xc.h>         // should compile with XC8 v1.35 + legacy PLIB v2.00
#include <stdint.h>     // remember: you must install manually legacy plib
#include <i2c.h>        // those since XC8 v1.35 doesn't include plib anymore
#include <rtcc.h>
#include <timers.h>
#include <pwm.h>
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

*/
/* config words start @0x01FFF8
let's put some const at the end of flash memory:
    model of the unit (00: operator, 01:scout, 02:enforcer...)
    + make of the unit (1 for mk1, 2 for mk2...) */
const uint8_t myMUTA_version @ 0x01FFF2 = 0x01; // MUTA protocol version
const uint8_t myMODEL @ 0x01FFF4 = 0x01; // scout
// Unique ID used to identify the unit
const uint8_t myUID[2] @ 0x01FFF6 = { 0x00, 0x04 };   // uid
/* those info are mapped @ 0x01FFF4 and 0x01FFF6, and can then be modified
 * without recompiling the firmware, if needed (using the proper tool) */
extern uint8_t myLongAddress[4];    // equal to myMODEL+myUID
                                    // done at the beginning of main()

// this is the 64bit key that will be used for encryption/decryption
// !!!it should be the same for every unit!!!
// algo: XTEA64 + CCM_16
// the key is mapped @ 0x01FEC and can then be modified without recompiling
// the firmware, if needed (using the proper tool)
const unsigned char StaticSecurityKey[8] @ 0x01FFEC = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07  };
unsigned char mySecurityKey[8];
/* note: mySecurityKey will be initialized with StaticSecurityKey
 * then mySecurityKey[2] and mySecurityKey[6] will be replaced by
 * random values generated for each network session, and returned through NETWORK_REGISTER */

/* Memory mapping of myMODEL, myUID and mySecurityKey:
    myMODEL[2] @ 0x01FFF4
    myUID[2] @ 0x01FFF6
    mySecurityKey[8] @ 0x01FFEC        
=> allow the use of HEXMATE (command line utility that comes with MPLABX)
to modify those value without recompiling the firmware
ex.: HEXMATE source.hex -FILL=0xBEEF@0x1000 -Odest.hex */
// Possible channel numbers are from 0 to 31
// channels 0,4,8,12,16,20,24,28 (~22sec max))
#define MUTA_CHANNELS      0x11111111
//#define MUTA_CHANNELS      0x01010101   // channel 0,8,16,24 only
//#define MUTA_CHANNELS      0x00000001   // channel 0 only

#define MINIMAL_RSSI     35
#define MUTA_PANID   0x1337

#define MAX_CONSECUTIVE_FAILS   3

// to use the internal RTCC as a timer
// with frequency==multiples of 1min, default @1min
#define DEFAULT_UPDATE_FREQUENCY    1
volatile bool timer_flag = false;
volatile uint8_t timer_count = DEFAULT_UPDATE_FREQUENCY;

// Variables / values
const char POWER_LABEL[] = "Pwr";
uint8_t m_power;
const char UPTIME_LABEL[] = "Upt";
volatile float m_uptime_days = 0;   // updated every minute
const char UPDATE_FREQUENCY_LABEL[] = "UpF";
volatile uint8_t m_update_frequency = DEFAULT_UPDATE_FREQUENCY;
const char FAILEDMSG_LABEL[] = "FlM";
uint8_t consecutive_fails = 0;
unsigned long m_failed_messages = 0;
unsigned long m_sent_messages = 0;
const char BATTERY_VOLTAGE_LABEL[] = "VBa";
float m_battery_voltage = 0.0;
const char LIGHT_LEVEL_LABEL[] = "Lit";
uint8_t m_light_level = 0;
const char TEMPERATURE_LABEL[] = "Tmp";
float m_temperature = 0.0;

bool Pwr_updated = false;
bool UpF_updated = false;

// TinyPack encoding stuff
uint8_t m_buffer[32];
uint8_t * p_buffer;
MUTA_VARIABLE m_var;

// Main functions / helpers to keep the main code simple & clean
bool process_update_answer();
void read_sensors();
bool send_initial_update();
bool send_writables_update();
bool send_periodical_update();
void update_variable();
void timer_reset();
void RTCC_reset();
void wait_for_1_sec();
void inline sleep_and_reset();

// temp sensor I2C address
#define temp_slave_address 0b10010000

// declare the scout as a sleeping unit
#define SLEEPING_SCOUT

APP_STATE   myAppState; // track App status


// ------------------
//      main loop
// ------------------
void main() {

    SYSTEM_Initialize(); // PIC hardware setup/configuration    
    
    RTCC_reset();
    timer_reset();
        
    myAppState = STATE_INIT;

    bool result;
    // set the unique LongAddress as the result of the unique myMODEL+myUID
    myLongAddress[0] = myMUTA_version;
    myLongAddress[1] = myMODEL;
    myLongAddress[2] = myUID[0];
    myLongAddress[3] = myUID[1];
    
    // initialize the security key
    for (int x=0; x<8; x++)
        mySecurityKey[x] = StaticSecurityKey[x];
    
    MiApp_ProtocolInit(false);  // init the Miwi stack
    MiMAC_SetPower(TX_POWER);   // use max power for scan process
    
    // A) SEARCH FOR A NETWORK WITH PANID == MUTA_PANID
    //=================================================
    volatile uint8_t scanresult;
    while(1)
    {
        scanresult = MiApp_SearchConnection(8, MUTA_CHANNELS);
        if (scanresult == 0)
        {
            // no candidate found, sleep & try again + increase the 
            // default tryout frequency by 1mn, up to 10min
            if (m_update_frequency<10)
            {
                m_update_frequency++;
            }
            LATCbits.LATC0 = 0;  // turn led off
            timer_reset();
            MiApp_TransceiverPowerState(POWER_STATE_SLEEP);
#ifdef SLEEPING_SCOUT
            while (!timer_flag)
            {
                OSCCONbits.IDLEN = 0;   // specs says it should be used just before SLEEP())
                SLEEP();        // now we are sleeping            
            }
#else
            while(!timer_flag)
            {}
#endif
            MiApp_TransceiverPowerState(POWER_STATE_WAKEUP);// wake up the MRF89
            //if(MiApp_MessageAvailable()) // flush - not required here:
            //    MiApp_DiscardMessage();  // Miwi is not initialized!
            //MiWiTasks();
        }
        else
        {
            // restore the default update frequency
            m_update_frequency = DEFAULT_UPDATE_FREQUENCY;
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
        // let's sleep 1 cycle before resetting
        myAppState = STATE_PROBLEM;
        sleep_and_reset();
    }
    MiApp_ConnectionMode(DISABLE_ALL_CONN);  // don't answer to scan or join requests
    
    // C) NETWORK_REGISTER
    //====================
#ifdef SLEEPING_SCOUT
    /* I don't use the sleeping feature of Miwi
     * - I couldn't get it to work! - but we don't need it anyway */
    result = do_NETWORK_REGISTER(&myLongAddress[0],MUTA_BOOL_TRUE);
#else
    result = do_NETWORK_REGISTER(&myLongAddress[0],MUTA_BOOL_FALSE);
#endif
    if (!result)
    {
        // auth failed!
        // let's sleep 1 cycle before resetting
        myAppState = STATE_PROBLEM;
        sleep_and_reset();
    }
    
    // force security key update on
    security_force_key_update();
    
    myAppState = STATE_READY;
    
    // D) POWER LEVEL DYNAMIC ADAPTATION
    // =================================
    m_power = adapt_power_level(TX_POWER, MINIMAL_RSSI);
    
    // E) RESET UPTIME, READ SENSORS, SEND INITIAL UPDATE
    // ==================================================
    m_uptime_days = 0;

    RTCC_reset();   // to resync it to every min from now
    
    read_sensors();  
    while (!send_initial_update())
    {
        // another failed attempt
        consecutive_fails++;
        if (consecutive_fails >= MAX_CONSECUTIVE_FAILS)
        {
            // let's sleep 1 cycle before resetting
            myAppState = STATE_PROBLEM;
            sleep_and_reset();
        }
        wait_for_1_sec();
    }
    consecutive_fails = 0;  // here we have received an UPDATE as answer to send_initial_update()
    
    // F) MAIN LOOP
    // ============
    while (true)
    {
        // F1) PROCESS RECEIVED UPDATE
        // ===========================
        process_update_answer();
                
        // F2) SLEEP/READ/SEND UPDATE
        // ==========================
        LATCbits.LATC0 = 0;  // turn led off
        MiApp_TransceiverPowerState(POWER_STATE_SLEEP);
        timer_reset();
#ifdef SLEEPING_SCOUT
            while (!timer_flag)
            {
                OSCCONbits.IDLEN = 0;   // specs says it should be used just before SLEEP())
                SLEEP();        // now we are sleeping            
            }
#else
            while(!timer_flag)
            {}
#endif
        LATCbits.LATC0 = 1;  // turn led on
        read_sensors();
        MiApp_TransceiverPowerState(POWER_STATE_WAKEUP);    // wake up the MRF89
        if(MiApp_MessageAvailable()) // flush
            MiApp_DiscardMessage();
        MiWiTasks();
        while (!send_periodical_update())
        {
            // failed attempt
            consecutive_fails++;
            if (consecutive_fails >= MAX_CONSECUTIVE_FAILS)
            {
                // let's sleep 1 cycle before resetting
                myAppState = STATE_PROBLEM;
                sleep_and_reset();
            }
            wait_for_1_sec();
        }
        consecutive_fails = 0;  // here we have received an UPDATE as answer to send_periodical_update()
    }
}


/* read update answers from PAN operator until timeout, update message empty or ack_required with variables update
 * if variables update, send_writables and wait for an update message as answer (whatever the content, the process will stop here)
 * return true if everything is normal, false if error sending/error receiving (timeout)
 */

bool process_update_answer()    // TODO
{
    uint8_t shortaddress[2];    // short address of sender
    uint8_t result = false;
    uint8_t i;

    // faire une boucle while avec sorties: update vide(true), timeout (false) ou ack_required => send_writables => attente réponse ou timeout
    // réponse => ok, on s'arrête là (true) et on discarde le message reçu (pas d'enchainement d'update requests), timeout => false
    
    while(true)
    {
        for (i=0; i<200; i++)   // check for an answer with a 10ms interval check
        {                       // within a 2sec timeframe (sec for all the update messages)
            __delay_ms(10);
            if(MiApp_MessageAvailable())
            {
                shortaddress[0] = rxMessage.SourceAddress[0];
                shortaddress[1] = rxMessage.SourceAddress[1];
                uint8_t size = rxMessage.PayloadSize;
                switch(rxMessage.Payload[0])
                {                
                    case MUTA_UPDATE: // UPDATE message
                        // if it's for me, coming from PAN operator, and empty,
                        // then stop here & return true, it's an ack
                        if ((shortaddress[0] == 0x00) && (shortaddress[1] == 0x00) && (size == 2))
                        {
                            return true;
                        }
                        // else if it's for me, coming from PAN operator, and not empty,
                        // then process it, there are updates to do
                        else if ((shortaddress[0] == 0x00) && (shortaddress[1] == 0x00) && (size > 2))
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
                                        // sleep 1min: reset!
                                        myAppState = STATE_PROBLEM;
                                        LATCbits.LATC0 = 0;  // turn led off
                                        MiApp_TransceiverPowerState(POWER_STATE_SLEEP);
                                        timer_reset();
                                        PIR3bits.RTCCIF = 0;  // clear RTCC interrupt flag
                                        //OSCCONbits.IDLEN = 0;   // specs says it should be used just before SLEEP())
                                        SLEEP();    // now we are sleeping
                                        RESET();    // now we don't
                                    }
                                    wait_for_1_sec(); // or not ?
                                }
                                consecutive_fails = 0;
                                return true;
                            }
                        }
                        else
                            MiApp_DiscardMessage();

                    default:
                        MiApp_DiscardMessage();
                }
            }
        }
    }
    return result;   // will be false if timeout
}


void inline sleep_and_reset()
{
    LATCbits.LATC0 = 0;  // turn led off
    timer_reset();
    MiApp_TransceiverPowerState(POWER_STATE_SLEEP);
#ifdef SLEEPING_SCOUT
    while (!timer_flag)
    {
        OSCCONbits.IDLEN = 0;   // specs says it should be used just before SLEEP())
        SLEEP();        // now we are sleeping            
    }
#else
    while(!timer_flag)
    {}
#endif
    MiApp_TransceiverPowerState(POWER_STATE_WAKEUP);// wake up the MRF89
    RESET();    // it's groundhog day!
}


/* read the sensors values and update the corresponding variables */

void read_sensors()
{
    // measure the external voltage ref & the temperature
    ADCON0bits.ADON = 1;    // ADC is on - must be far from GO
    LATAbits.LATA6 = 1;     // turn on the output ref voltage+I2Ctemp supply
    __delay_ms(20);         // let it rise - use MRF89 waking up instead

    // start with a calibration, even after each sleep
    ADCON1bits.ADCAL = 1;   // calibration mode ON
    ADCON0bits.GO = 1;      // start the automatic acq+conversion
    while (ADCON0bits.GO == 1)  {}  // wait for the ADC to finish its job
    ADCON1bits.ADCAL = 0;   // calibration mode OFF

    // read the voltage ref
    ADCON0bits.CHS = 0;     // select AN0 for ADC        
    ADCON0bits.GO = 1;              // start the automatic acq+conversion
    while (ADCON0bits.GO == 1)  {}  // wait for the ADC to finish its job

    uint16_t adc_result = (ADRESHbits.ADRESH << 8) + ADRESLbits.ADRESL;
    m_battery_voltage = (4096F / adc_result) * 1.24F;

    // read the photodiode voltage
    ADCON0bits.CHS = 1;     // select AN1 for ADC
    //__delay_ms(10);
    ADCON0bits.GO = 1;              // start the automatic acq+conversion
    while (ADCON0bits.GO == 1)  {}  // wait for the ADC to finish its job

    adc_result = (ADRESHbits.ADRESH << 8) + ADRESLbits.ADRESL;  // 0 for absolute dark, 74 for full sunlight
    m_light_level = (uint8_t)((adc_result / 74F) * 100F); // as a percentage
    if (m_light_level > 100)
    {
        m_light_level = 100;
    }

    //int tmp=0;

    // read temp
    __delay_ms(90); // to let the sensor read the temp
    OpenI2C(MASTER, SLEW_OFF);

    uint8_t t0, t1;

    // ask for register #0
    IdleI2C();                         // Wait until the bus is idle
    StartI2C();                        // Send START condition
    IdleI2C();                         // Wait for the end of the START condition
    WriteI2C( temp_slave_address & 0xfe );  // Send address with R/W cleared for write
    IdleI2C();                         // Wait for ACK
    WriteI2C( 0x00 );                  // Write register pointer
    IdleI2C();                         // Wait for ACK
    StopI2C();                         // Hang up, send STOP condition

    // read register #0
    IdleI2C();                         // Wait until the bus is idle
    StartI2C();                        // Send START condition
    IdleI2C();                         // Wait for the end of the START condition
    WriteI2C( temp_slave_address | 0x01 );  // Send address with R/W set for read
    IdleI2C();                         // Wait for ACK
    t1 = ReadI2C();               // Read first byte of data
    AckI2C();                          // Send ACK
    t0 = ReadI2C();               // Read nth byte of data
    NotAckI2C();                       // Send NACK
    StopI2C();                         // Hang up, send STOP condition

    uint16_t value = (t1<<8) + t0;
    value = (value>>4);
    m_temperature = ((float)value)*0.0625F;

    ADCON0bits.ADON = 0;  // ADC is off
    CloseI2C();           // shutdown I2C
    LATAbits.LATA6 = 0;   // turn off the sensors power supply
}


/* send all the variables */
bool send_initial_update()
{
    uint16_t tmp;
    // FIRST PART OF THE UPDATE
    p_buffer = m_buffer;    // encode the variables/values in a temporary buffer
    // encode units variables/values now
    // send the battery voltage
    memcpy(m_var.label, BATTERY_VOLTAGE_LABEL, 3);
    m_var.unit = MUTA_VOLT_UNIT;
    m_var.value_byte1 = (uint8_t)floor(m_battery_voltage);  // integer part
    m_var.value_byte2 = (uint8_t)floor(((m_battery_voltage - floor(m_battery_voltage))*100));  // +2 digits
    m_var.writable = false;
    p_buffer += encode_ufixed16_variable(p_buffer, m_var);
    // send the light level
    memcpy(m_var.label, LIGHT_LEVEL_LABEL, 3);
    m_var.unit = MUTA_PERCENT_UNIT;
    m_var.value_byte1 = m_light_level;
    m_var.writable = false;
    p_buffer += encode_uint8_variable(p_buffer, m_var);
    // send the temperature
    memcpy(m_var.label, TEMPERATURE_LABEL, 3);
    m_var.unit = MUTA_DEGREES_UNIT;
    m_var.value_byte1 = (uint8_t)floor(m_temperature);  // integer part
    m_var.value_byte2 = (uint8_t)floor(((m_temperature - floor(m_temperature))*100));  // +2 digits
    m_var.writable = false;
    p_buffer += encode_ufixed16_variable(p_buffer, m_var);
    // send the message
    if (!do_UPDATE(m_buffer, p_buffer-m_buffer, false))
        return false;

    // SECOND PART OF THE UPDATE
    p_buffer = m_buffer;    // encode the variables/values in a temporary buffer
    // send the power level
    memcpy(m_var.label, POWER_LABEL, 3);
    m_var.unit = MUTA_MWATT_UNIT;
    dBm_to_mW_ufixed16(m_power, &(m_var.value_byte1), &(m_var.value_byte2));
    m_var.writable = true;
    p_buffer += encode_ufixed16_variable(p_buffer, m_var);
    // send the uptime as a fraction of days
    memcpy(m_var.label, UPTIME_LABEL, 3);
    m_var.unit = MUTA_DAYS_UNIT;
    m_var.value_byte1 = (uint8_t)floor(m_uptime_days);  // integer part
    m_var.value_byte2 = (uint8_t)floor(((m_uptime_days - floor(m_uptime_days))*100));  // +2 digits
    m_var.writable = false;
    p_buffer += encode_ufixed16_variable(p_buffer, m_var);
    // send the update frequency
    memcpy(m_var.label, UPDATE_FREQUENCY_LABEL, 3);
    m_var.unit = MUTA_MINUTES_UNIT;
    m_var.value_byte1 = m_update_frequency;
    m_var.writable = true;
    p_buffer += encode_uint8_variable(p_buffer, m_var);
    // send the message
    if (!do_UPDATE(m_buffer, p_buffer-m_buffer, false))
        return false;
    
    // THIRD PART OF THE UPDATE
    // send the failed msg percentage
    p_buffer = m_buffer;    // encode the variables/values in a temporary buffer
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


/* send all the writables variables */
bool send_writables_update()
{
    uint16_t tmp;
    p_buffer = m_buffer;    // encode the variables/values in a temporary buffer
    // encode units variables/values now
    if  (Pwr_updated)
    {
        // send the power level
        memcpy(m_var.label, POWER_LABEL, 3);
        m_var.unit = MUTA_MWATT_UNIT;
        dBm_to_mW_ufixed16(m_power, &(m_var.value_byte1), &(m_var.value_byte2));
        m_var.writable = true;
        p_buffer += encode_ufixed16_variable(p_buffer, m_var);  
        Pwr_updated = false;
    }
    if (UpF_updated)
    {
        // send the update frequency
        memcpy(m_var.label, UPDATE_FREQUENCY_LABEL, 3);
        m_var.unit = MUTA_MINUTES_UNIT;
        m_var.value_byte1 = m_update_frequency;
        m_var.writable = true;
        p_buffer += encode_uint8_variable(p_buffer, m_var);
        UpF_updated = false;
    }
    // send the message
    return do_UPDATE(m_buffer, p_buffer-m_buffer, true);
}


/* send all the periodically updated variables */
bool send_periodical_update()
{
    uint16_t tmp;
    // FIRST PART OF THE UPDATE
    p_buffer = m_buffer;    // encode the variables/values in a temporary buffer
    // encode units variables/values now
    // send the battery voltage
    memcpy(m_var.label, BATTERY_VOLTAGE_LABEL, 3);
    m_var.unit = MUTA_VOLT_UNIT;
    m_var.value_byte1 = (uint8_t)floor(m_battery_voltage);  // integer part
    m_var.value_byte2 = (uint8_t)floor(((m_battery_voltage - floor(m_battery_voltage))*100));  // +2 digits
    m_var.writable = false;
    p_buffer += encode_ufixed16_variable(p_buffer, m_var);
    // send the light level
    memcpy(m_var.label, LIGHT_LEVEL_LABEL, 3);
    m_var.unit = MUTA_PERCENT_UNIT;
    m_var.value_byte1 = m_light_level;
    m_var.writable = false;
    p_buffer += encode_uint8_variable(p_buffer, m_var);
    // send the temperature
    memcpy(m_var.label, TEMPERATURE_LABEL, 3);
    m_var.unit = MUTA_DEGREES_UNIT;
    m_var.value_byte1 = (uint8_t)floor(m_temperature);  // integer part
    m_var.value_byte2 = (uint8_t)floor(((m_temperature - floor(m_temperature))*100));  // +2 digits
    m_var.writable = false;
    p_buffer += encode_ufixed16_variable(p_buffer, m_var);    
    // send the message
    if (!do_UPDATE(m_buffer, p_buffer-m_buffer, false))
        return false;

    // SECOND PART OF THE UPDATE
    p_buffer = m_buffer;    // encode the variables/values in a temporary buffer
    // send the uptime as a fraction of days
    memcpy(m_var.label, UPTIME_LABEL, 3);
    m_var.unit = MUTA_DAYS_UNIT;
    m_var.value_byte1 = (uint8_t)floor(m_uptime_days);  // integer part
    m_var.value_byte2 = (uint8_t)floor(((m_uptime_days - floor(m_uptime_days))*100));  // +2 digits
    m_var.writable = false;
    p_buffer += encode_ufixed16_variable(p_buffer, m_var);
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


/* process an update request received from PAN operator */
void update_variable()
{
    if (labelcmp(m_var.label, (uint8_t*)POWER_LABEL) == 1)   // new value for power
    {
        m_power = mW_ufixed16_to_dBm(m_var.value_byte1, m_var.value_byte2);
        MiMAC_SetPower(m_power);
        Pwr_updated = true;
    }
    else if ((labelcmp(m_var.label, (uint8_t*)UPDATE_FREQUENCY_LABEL) == 1) \
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
        UpF_updated = true;
    }
}

void RTCC_reset()
{
    RtccWrOn(); //write enable the rtcc registers
    RTCCFGbits.RTCPTR0 = 0; //RTCVALH/L will point to min/seconds values
    RTCCFGbits.RTCPTR1 = 0;
    RTCVALHbits.RTCVALH = 0x00; // write 00 to min & sec
    RTCVALLbits.RTCVALL = 0x00;
    mRtccWrOff();   // write disable rtcc regs    
}

/* reset the timer */
void timer_reset()
{
    if (m_update_frequency == 0) // special debug mode:every 10sec
        timer_count = 1;
    else
        timer_count = m_update_frequency;
    timer_flag = false;
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
    //RTCC
    if(PIR3bits.RTCCIF) // RTCC every minute interrupt
    {
        m_uptime_days = m_uptime_days + 0.000671296F; //== 58 seconds in days
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
