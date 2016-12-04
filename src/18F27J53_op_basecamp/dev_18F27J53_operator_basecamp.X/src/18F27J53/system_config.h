/********************************************************************
 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the "Company") for its PIC(R) Microcontroller is intended and
 supplied to you, the Company's customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *******************************************************************/

#ifndef _SYSTEM_CONFIG_H
    #define _SYSTEM_CONFIG_H
 
#include "miwi_config.h"        //Include miwi application layer configuration file
#include "miwi_config_mesh.h"   //Include protocol layer configuration file
#include "config_89xa.h"        //Transceiver configuration file
 
// Define clock frequency @16MHz
#define _XTAL_FREQ 16000000

// because led output has changed on mk3+ models
#define MK1
//#define MK3

// switches ?

//#define SW1             1
//#define SW2             2	
    
// There are three ways to use NVM to store data: External EPROM, Data EEPROM and
// programming space, with following definitions:
//      #define USE_EXTERNAL_EEPROM
//      #define USE_DATA_EEPROM
//      #define USE_PROGRAMMING_SPACE
// Each demo board has defined the method of using NVM, as
// required by Network Freezer feature.
//#define USE_EXTERNAL_EEPROM

//#define SUPPORT_TWO_SPI

// Define EEPROM_SHARE_SPI if external EEPROM shares the SPI
// bus with RF transceiver
//#define EEPROM_SHARE_SPI

//MRF89XA DEFINITIONS

// nCSDATA on RC6
#define Data_nCS            LATCbits.LATC6
#define Data_nCS_TRIS       TRISCbits.TRISC6

// nCSCONF on RB3
#define Config_nCS          LATBbits.LATB3
#define Config_nCS_TRIS     TRISBbits.TRISB3

// IRQ0 on RB0/INT0
#define IRQ0_INT_PIN        PORTBbits.RB0
#define IRQ0_INT_TRIS       TRISBbits.TRISB0
#define PHY_IRQ0            INTCONbits.INT0IF
#define PHY_IRQ0_En         INTCONbits.INT0IE

// IRQ1 on RC2/RP13
#define IRQ1_INT_PIN        PORTCbits.RC2
#define IRQ1_INT_TRIS       TRISCbits.TRISC2
#define PHY_IRQ1            INTCON3bits.INT1IF
#define PHY_IRQ1_En         INTCON3bits.INT1IE
#define PHY_IRQ1_IP         INTCON3bits.INT1IP

// nRESET on RA5
#define PHY_RESETn          LATAbits.LATA5
#define PHY_RESETn_TRIS     TRISAbits.TRISA5

// SPI2 Pin Definitions

// SDI on RC1/RP12
#define SPI_SDI             PORTCbits.RC1
#define SDI_TRIS            TRISCbits.TRISC1
//#define SPI_SDI             PORTAbits.RA1
//#define SDI_TRIS            TRISAbits.TRISA1

// SDO on RB1/RP4
#define SPI_SDO             LATBbits.LATB1
#define SDO_TRIS            TRISBbits.TRISB1

// SCK on RB2/RP5
#define SPI_SCK             LATBbits.LATB2
#define SCK_TRIS            TRISBbits.TRISB2

// Switch and LED Pin Definitions
//#define LED0                LATCbits.LATC6
//#define LED0_TRIS           TRISCbits.TRISC6

/*
#define LED0_LAT        LATCbits.LATC6
#define LED0_PORT       PORTCbits.RC6
#define LED0_TRIS       TRISCbits.TRISC6

#define LED1_LAT        LATCbits.LATC7
#define LED1_PORT       PORTCbits.RC7
#define LED1_TRIS       TRISCbits.TRISC7

#define LED2_LAT        LATBbits.LATB0
#define LED2_PORT       PORTBbits.RB0
#define LED2_TRIS       TRISBbits.TRISB0
*/
// USB status LED
//#define LED_USB_DEVICE_STATE                    LED0

// not useful anymore, must be removed
#define BUTTON_USB_DEVICE_HID_CUSTOM            BUTTON_NONE

#endif
