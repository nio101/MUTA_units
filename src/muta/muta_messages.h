// Header file for
// MUTA custom messages and types
//
// Copyright (2015-2016) Nicolas Barthe,
// distributed as open source under the terms
// of the GNU General Public License (see COPYING.txt)

// See DOCS.txt for more info

// Compatible with: MUTA v01

#include <stdint.h>
#include <string.h>
#include "miwi/drv_mrf_miwi_89xa.h"
#include "system_config.h"

// used to keep track of the rssi of the last received message
extern uint8_t last_rssi;

// Power_lvl // text & value
uint8_t mW_ufixed16_to_dBm(uint8_t value1, uint8_t value2);
void dBm_to_mW_ufixed16(uint8_t power, uint8_t* value1, uint8_t* value2);
const char* dBm_to_mW(uint8_t power);
const char TX_POWER_13_DB_MW[] = "20mW";
const char TX_POWER_10_DB_MW[] = "10mW";
const char TX_POWER_7_DB_MW[] = "5mW";
const char TX_POWER_4_DB_MW[] = "2.5mW";
const char TX_POWER_1_DB_MW[] = "1.25mW";
const char TX_POWER_N_2_DB_MW[] = "0.6mW";
const char TX_POWER_N_5_DB_MW[] = "0.3mW";
const char TX_POWER_N_8_DB_MW[] = "0.15mW";
const char TX_POWER_INVALID[] = "0.0mW";

// Messages commands sent/received from PC
// 'I'
#define PC_MESSAGE_INFO         0x49
// 'M'
#define PC_MESSAGE_RECEIVED     0x4d
// 'S'
#define PC_MESSAGE_SEND         0x53
// 'X'
#define PC_RESET                0x58

// Messages exchanged on network
#define MUTA_POWER_TEST          01
#define MUTA_NETWORK_REGISTER    02
#define MUTA_PING                03
#define MUTA_UPDATE              04

// TinyPack encoding/decoding
// types
#define MUTA_UINT8_TYPE     0
#define MUTA_UINT16_TYPE    1
#define MUTA_UFIXED16_TYPE  2
#define MUTA_BOOLEAN_TYPE   3
// units
#define MUTA_NO_UNIT        0
#define MUTA_DEGREES_UNIT   1
#define MUTA_VOLT_UNIT      2
#define MUTA_PERCENT_UNIT   3
#define MUTA_MINUTES_UNIT   4
#define MUTA_SECONDS_UNIT   5
#define MUTA_HOURS_UNIT     6
#define MUTA_DAYS_UNIT      7
#define MUTA_MWATT_UNIT     8
// boolean values
#define MUTA_BOOL_FALSE     0xf0
#define MUTA_BOOL_TRUE      0xff

// variable struct used to encode/decode variables/values
typedef struct {
    char label[3];
    uint8_t type;
    uint8_t value_byte1;
    uint8_t value_byte2;
    uint8_t unit;
    bool writable;
    bool _signed;
    uint8_t size;
} MUTA_VARIABLE;

// label compare helper
bool labelcmp(uint8_t* p1, uint8_t* p2);

// encoding functions
uint8_t encode_uint8_variable(uint8_t* buffer, MUTA_VARIABLE var);
uint8_t encode_uint16_variable(uint8_t* buffer, MUTA_VARIABLE var);
uint8_t encode_ufixed16_variable(uint8_t* buffer, MUTA_VARIABLE var);
uint8_t encode_boolean_variable(uint8_t* buffer, MUTA_VARIABLE var);

// decoding function
MUTA_VARIABLE decode_variable(uint8_t* p_buffer);

// high level helpers to keep the main code clean
bool do_NETWORK_REGISTER(uint8_t* p_uid, bool sleeping_device);
uint8_t do_POWER_TEST(uint8_t power_lvl);
bool do_PING(uint8_t* p_short_addr);
uint8_t adapt_power_level(uint8_t power, uint8_t minimal_rssi);
bool do_UPDATE(uint8_t* p_buff, uint8_t size, bool ack_required);