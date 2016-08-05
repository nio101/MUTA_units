#include "muta_messages.h"

/*
#define testbit(var, bit)       ((var) & (1 << (bit)))
#define setbit(var, bit)        ((var) |= (1 << (bit)))
#define clearbit(var, bit)      ((var) &= ~(1 << (bit)))
*/

uint8_t last_rssi = 0;
extern unsigned long m_failed_messages;
extern unsigned long m_sent_messages;

// Power_lvl -> text description
const char* dBm_to_mW(uint8_t power)
{
    switch(power)
    {
        case TX_POWER_13_DB:
            return TX_POWER_13_DB_MW;
        case TX_POWER_10_DB:
            return TX_POWER_10_DB_MW;
        case TX_POWER_7_DB:
            return TX_POWER_7_DB_MW;
        case TX_POWER_4_DB:
            return TX_POWER_4_DB_MW;
        case TX_POWER_1_DB:
            return TX_POWER_1_DB_MW;
        case TX_POWER_N_2_DB:
            return TX_POWER_N_2_DB_MW;
        case TX_POWER_N_5_DB:
            return TX_POWER_N_5_DB_MW;
        case TX_POWER_N_8_DB:
            return TX_POWER_N_8_DB_MW;
        default:
            return TX_POWER_INVALID;
    }
}

// TinyPack encoding

// Remember: 3 variables max per 21-byte-max-length payload!

uint8_t encode_uint8_variable(uint8_t* buffer, MUTA_VARIABLE var)
{
    *(buffer++) = var.label[0];
    *(buffer++) = var.label[1];
    *(buffer++) = var.label[2];
    var.type = MUTA_UINT8_TYPE << 1;
    if (var.writable)
    {
        var.type = var.type + 1;
    }
    var.type = var.type << 4;
    var.type = var.type + var.unit;
    *(buffer++) = var.type;
    *(buffer++) = var.value_byte1;
    return 5;
}

uint8_t encode_uint16_variable(uint8_t* buffer, MUTA_VARIABLE var)
{
    *(buffer++) = var.label[0];
    *(buffer++) = var.label[1];
    *(buffer++) = var.label[2];
    var.type = MUTA_UINT16_TYPE << 1;
    if (var.writable)
    {
        var.type = var.type + 1;
    }
    var.type = var.type << 4;
    var.type = var.type + var.unit;
    *(buffer++) = var.type;
    *(buffer++) = var.value_byte2;
    *(buffer++) = var.value_byte1;
    return 6;
}

uint8_t encode_ufixed16_variable(uint8_t* buffer, MUTA_VARIABLE var)
{
    *(buffer++) = var.label[0];
    *(buffer++) = var.label[1];
    *(buffer++) = var.label[2];
    var.type = MUTA_UFIXED16_TYPE << 1;
    if (var.writable)
    {
        var.type = var.type + 1;
    }
    var.type = var.type << 4;
    var.type = var.type + var.unit;
    *(buffer++) = var.type;
    *(buffer++) = var.value_byte2;
    *(buffer++) = var.value_byte1;
    return 6;
}

uint8_t encode_boolean_variable(uint8_t* buffer, MUTA_VARIABLE var)
{
    *(buffer++) = var.label[0];
    *(buffer++) = var.label[1];
    *(buffer++) = var.label[2];
    var.type = MUTA_BOOLEAN_TYPE << 1;
    if (var.writable)
    {
        var.type = var.type + 1;
    }
    var.type = var.type << 4;
    var.type = var.type + var.unit;
    *(buffer++) = var.type;
    if (var.value_byte1 == MUTA_BOOL_TRUE)
    {
        *(buffer++) = MUTA_BOOL_TRUE;
    } else {
        *(buffer++) = MUTA_BOOL_FALSE;
    }
    return 5;
}

// TinyPack decoding

MUTA_VARIABLE decode_variable(uint8_t* p_buffer)
{
    MUTA_VARIABLE var;
    memcpy(var.label, p_buffer, 3);
    p_buffer += 3;
    
    var.unit = *p_buffer & 0b00000111;
    if (*p_buffer & 0b00010000)
        var.writable = true;
    else
        var.writable = false;
    var.type = (*p_buffer++ & 0b11100000) >> 5;
    var.value_byte1 = *p_buffer++;
    if ((var.type == MUTA_UINT16_TYPE) || (var.type == MUTA_UFIXED16_TYPE))
    {
        var.value_byte2 = *p_buffer++;
        var.size = 6;
    }
    else
        var.size = 5;
    return var;
}

// Helpers to keep the main code clean
//====================================

bool do_NETWORK_REGISTER(uint8_t* p_uid, bool sleeping_device)
{
    uint8_t shortaddress[2] = { 0x00, 0x00 };   // PAN coordinator
    MiApp_FlushTx();
    MiApp_WriteData(MUTA_NETWORK_REGISTER);
    memcpy(&TxBuffer[TxData], p_uid, 4); // UID
    TxData = TxData + 4;
    MiApp_WriteData(sleeping_device);
    bool result = MiApp_UnicastAddress(&shortaddress[0], false, false);
    if (!result)
    {
        m_failed_messages++;
        return false;
    }
    else
    {
        unsigned int i;
        result = false;
        for (i=0; i<100; i++)   // check for an answer with 10ms interval
        {                       // with a timeout @1sec
            __delay_ms(10);
            if(MiApp_MessageAvailable())
            {
                // message received
                last_rssi = rxMessage.PacketRSSI;
                if (rxMessage.Payload[0] != MUTA_NETWORK_REGISTER)
                {
                    MiApp_DiscardMessage();
                }
                else
                {
                    result = (rxMessage.Payload[1] == MUTA_BOOL_TRUE);
                    mySecurityKey[2] = rxMessage.Payload[2];
                    mySecurityKey[6] = rxMessage.Payload[3];
                    MiApp_DiscardMessage();
                    break;
                }
            }
        }
        // result is true here only if answer brings authorized == true
        if (!result)
            m_failed_messages++;
        else
            m_sent_messages++;
        return result;
    }
}

uint8_t do_POWER_TEST(uint8_t power_lvl)
{
    uint8_t shortaddress[2];   // PAN coordinator
    MiApp_FlushTx();
    MiApp_WriteData(MUTA_POWER_TEST);
    MiApp_WriteData(power_lvl);
    if (myShortAddress.v[0] == 0x00) // it's a coordinator or an operator
    {
        if (myShortAddress.v[1] == 0x00) // PAN operator, no POWER_TEST
        {
            return 0;
        }
        shortaddress[0] = 0x00; // coordinators send it to PAN operator
        shortaddress[1] = 0x00;
    }
    else    // it's an end unit
    {
        shortaddress[0] = 0x00; // points to the parent 0403 -> 0400
        shortaddress[1] = myShortAddress.v[1];
    }
    bool result = MiApp_UnicastAddress(&shortaddress[0], false, true);
    if (!result)
    {
        m_failed_messages++;
        return false;
    }
    else
    {
        unsigned int i;
        uint8_t rssi = 0;
        for (i=0; i<100; i++)   // check for an answer with 10ms interval
        {                       // with a timeout @1sec
            __delay_ms(10);
            if(MiApp_MessageAvailable())
            {
                // message received
                last_rssi = rxMessage.PacketRSSI;
                if (rxMessage.Payload[0] != MUTA_POWER_TEST || rxMessage.Payload[1] != power_lvl)
                {
                    MiApp_DiscardMessage();
                }
                else
                {
                    rssi = (rxMessage.Payload[2]);
                    MiApp_DiscardMessage();
                    break;
                }
            }
        }
        // rssi == returned value or 0 if timeout
        if (rssi == 0)
            m_failed_messages++;
        else
            m_sent_messages++;
        return rssi;
    }
}

uint8_t adapt_power_level(uint8_t power, uint8_t minimal_rssi)
{
    uint8_t m_power = TX_POWER; // let's begin with the max value given for TX_POWER
    uint8_t last_good_power = m_power;
    uint8_t rssi;
    while(1)
    {
        MiMAC_SetPower(m_power);
        __delay_ms(20);
        rssi = do_POWER_TEST(m_power);
        if (rssi < minimal_rssi)
        {
            break;
        }
        else
        {
            last_good_power = m_power;
            if (m_power == TX_POWER_N_8_DB)
            {
                break;  // can't go further, we're at the min power level
            }
            else
            {
                m_power = m_power + 1;
            }
        }
    }
    m_power = last_good_power;
    MiMAC_SetPower(m_power);
    return m_power;
}

bool do_PING(uint8_t* p_short_addr)
{   // send a dummy max size encrypted payload and receive it back unchanged
    uint8_t buffer[21]; 
    uint8_t* p_buffer = buffer;
    *(p_buffer++) = MUTA_PING;
    for (uint8_t x=0; x<20; x++)
        *(p_buffer++) = x;
    MiApp_FlushTx();
    memcpy(&TxBuffer[TxData], buffer, 21); // UID
    TxData = TxData + 21;
    bool result = MiApp_UnicastAddress(p_short_addr, false, true);
    if (!result)
    {
        m_failed_messages++;
        return false;
    }
    else
    {
        unsigned int i;
        for (i=0; i<100; i++)   // check for an answer with 10ms interval
        {                       // with a timeout @1sec
            __delay_ms(10);
            if(MiApp_MessageAvailable())
            {
                // message received, check the payload
                last_rssi = rxMessage.PacketRSSI;
                if (rxMessage.Payload[0] != MUTA_PING)
                {
                    MiApp_DiscardMessage();
                }
                else
                {
                    for (uint8_t x=0; x<20; x++)
                        if (rxMessage.Payload[x+1] != x)
                        {
                            MiApp_DiscardMessage();
                            return false;
                        }
                    MiApp_DiscardMessage();
                    m_sent_messages++;
                    return true;
                }
            }
        }
        m_failed_messages++;
        return false;   // timeout
    }
}

bool do_UPDATE(uint8_t* p_buff, uint8_t size, bool ack_required)
{
    uint8_t shortaddress[2] = { 0x00, 0x00 };   // PAN coordinator
    MiApp_FlushTx();
    MiApp_WriteData(MUTA_UPDATE);
    if (ack_required)
        MiApp_WriteData(MUTA_BOOL_TRUE);
    else
        MiApp_WriteData(MUTA_BOOL_FALSE);
    memcpy(&TxBuffer[TxData], p_buff, size); // encoded variables
    TxData = TxData + size;
    bool result = MiApp_UnicastAddress(&shortaddress[0], false, true);
    if (!result)
    {
        m_failed_messages++;
        return false;
    }
    if (ack_required)
    {
        unsigned int i;
        result = false;
        for (i=0; i<100; i++)   // check for an answer with 10ms interval
        {                       // with a timeout @1sec
            __delay_ms(10);
            if(MiApp_MessageAvailable())
            {
                // message received
                last_rssi = rxMessage.PacketRSSI;
                if (rxMessage.Payload[0] != MUTA_UPDATE)
                {
                    MiApp_DiscardMessage();
                }
                else
                    result = true;
                    // don't discard, the message, just check for an answer,
                    // it will then be processed in the main loop                    
                break;
            }
        }
        if (!result)
            m_failed_messages++;
        else
            m_sent_messages++;
        return result;
    }
    else
    {
        m_sent_messages++;
        return true;
    }
}