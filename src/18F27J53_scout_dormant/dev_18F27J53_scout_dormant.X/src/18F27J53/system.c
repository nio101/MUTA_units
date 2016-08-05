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
#include "system.h"
#include "system_config.h"
#include "plib/rtcc.h"

// ---------------------------------------------------------------
// PIC18F27J53 Configuration

#pragma config WDTEN = OFF      // Watchdog Timer (Disabled - Controlled by SWDTEN bit)
#pragma config CFGPLLEN = OFF    // PLL Enable Configuration Bit (PLL Disabled)
#pragma config STVREN = OFF     // Stack Overflow/Underflow Reset (Disabled)
#pragma config XINST = OFF       // Extended Instruction Set (Disabled)
#pragma config CP0 = OFF        // Code Protect (Program memory is not code-protected)
#pragma config OSC = INTOSC     // internal oscillator without PLL
#pragma config CLKOEC = OFF     // EC Clock Out Enable Bit  (CLKO output disabled on the RA6 pin)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor (Disabled)
#pragma config IESO = OFF           // Internal External Oscillator Switch Over Mode (Disabled)
#pragma config RTCOSC = INTOSCREF   // RTCC Clock Select (RTCC uses INTRC)
#pragma config ADCSEL = BIT12       // 12 - Bit ADC Enabled

/*********************************************************************
 * Function:        void SYSTEM_Initialize( void )
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    Board is initialized for P2P usage
 *
 * Overview:        This function configures the board 
 *
 * Note:            This routine needs to be called before the function 
 *                  to initialize stack or any other function that
 *                  operates on the stack
 ********************************************************************/
void SYSTEM_Initialize(void)
{    
    // No PLL
    PLLEN = 0;

    // 8MHz from INTOSC
    OSCCONbits.IRCF = 0b111;

    // no analog pin used, all digital
    ANCON0 = 0xFF;
    ANCON1 = 0x1F;

    // configure IO as OUTPUT => 0uA mesuré en sleep !!!
    TRISA = 0;
    TRISB = 0;
    TRISC = 0;    

    LATCbits.LATC0 = 1;     // turn on the LED!

    UPUEN = 0;  // USB internal pullup disabled
    UCONbits.USBEN = 0; // USB disabled    

    RCONbits.IPEN = 1;  // enable interrupt priorities management
    
    // RTCC alarm setup
    ALRMCFGbits.AMASK = 0b0011; // alarm every minute
    //ALRMCFGbits.AMASK = 0b0010; // alarm every 10sec
    ALRMCFGbits.CHIME = 1;      // chime ON, alarm is repeated indefinitly
    ALRMCFGbits.ALRMEN = 1;     // Alarm ON

    // turn RTCC and RTCC/alarm ON
    //RtccInitClock();   // config says RTCC should use INTRC
                         // RtccInitClock enables timer1 crystal use!
    RTCCFG = 0x0;
    RTCCAL = 0x00;
    if(mRtccIsOn())
    {
       if(!mRtccIsWrEn())
       {
           RtccWrOn();
       }
       mRtccOff();
    }
    mRtccWrOff();
    RtccWrOn(); //write enable the rtcc registers
    mRtccOn(); //enable the rtcc module
    mRtccAlrmEnable(); //enable the rtcc alarm
    RTCCFGbits.RTCPTR0 = 0; //RTCVALH/L will point to min/seconds values
    RTCCFGbits.RTCPTR1 = 0;
    RTCVALHbits.RTCVALH = 0x00; // write 00 to min & sec
    RTCVALLbits.RTCVALL = 0x00;
    mRtccWrOff();   // write disable to rtcc regs    

    // setup RTCC interrupt
    IPR3bits.RTCCIP = 0; // low priority for RTCC
    PIR3bits.RTCCIF = 0;  // clear RTCC interrupt flag
    PIE3bits.RTCCIE = 1;  // RTCC interrupt enable    

    //I2C

    // SDA1 & SCL1 must be defined as inputs in TRIS
    TRISBbits.TRISB4 = 1;
    TRISBbits.TRISB5 = 1;

    SSP1ADD = 19;   // for 100kHz I2C if FOsc@8MHz    

    //--------------------------------------
    // ADC

    // turn off the output ref voltage+I2C temp supply
    LATAbits.LATA6 = 0;

    //ADCON0bits.ADON = 1;    // ADC is on - must be far from GO
    ADCON0bits.ADON = 0;    // ADC is off

    // set RA0/AN0 as input
    TRISAbits.TRISA0 = 1;
    // set RA0/AN0 in analog mode
    ANCON0bits.PCFG0 = 0;

    // set RA1/AN1 as input
    TRISAbits.TRISA1 = 1;
    // set RA1/AN1 in analog mode
    ANCON0bits.PCFG1 = 0;

    ANCON1bits.VBGEN = 0;   //Internal band gap voltage reference turned off
    //ANCON1bits.VBGEN = 1;   //Internal band gap voltage reference turned ON
    //__delay_ms(10);

    // setup ANCON0
    ADCON0bits.VCFG0 = 0;   // Vss for v- ref
    ADCON0bits.VCFG1 = 0;   // Vdd for v+ ref
    //ADCON0bits.CHS = 0b1111;     // VBE
    ADCON1bits.ADCS = 0b001;    // TAD = 8*TOSC == 1usec @ 8MHz
    //ADCON1bits.ACQT = 0b010;  // 4*TAD = 4usec for acqu time > 2.5us recom.
    ADCON1bits.ACQT = 0b111;    // max TAD, the more, the better... :)
    ADCON1bits.ADFM = 1;        // right justified result    

    /*******************************************************************/
    // Configure PPS Related Pins
    /*******************************************************************/
    // Unlock to config PPS
    EECON2 = 0x55;
    EECON2 = 0xAA;
    PPSCONbits.IOLOCK = 0;

    // inputs
    RPINR1 = 13;    // Mapping IRQ1 to RC2(RP13)
    RPINR21 = 12;   // Mapping SDI2 to RC1(RP12)    
    RPINR22 = 5;    // Mapping SCK2IN to RB2(RP5)

    // outputs
    RPOR4 = 10;     // Mapping SDO2 to RB1(RP4)
    RPOR5 = 11;     // Mapping SCK2OUT to RB2(RP5)
    //RPOR11 = 14;  // RC0/RP11: ICCP1 output (for pwm led)

    // Lock System
    EECON2 = 0x55;
    EECON2 = 0xAA;
    PPSCONbits.IOLOCK = 1;

    /*******************************************************************/
    // Config RF Radio
    /*******************************************************************/

    //RCONbits.IPEN = 1;  // enable interrupt priorities management

    /*******************************************************************/
    // Config MRF89XA Pins
    /*******************************************************************/

    Data_nCS = 1;
    Config_nCS = 1;
    
    Data_nCS_TRIS = 0; 
    Config_nCS_TRIS = 0;   

    // INT0/INT1 config pin as input
    IRQ0_INT_TRIS = 1;
    IRQ1_INT_TRIS = 1;
    
    // Config IRQ1 Edge = Rising
    INTCON2bits.INTEDG1 = 1;
    
    // Config IRQ0 Edge = Rising
    INTCON2bits.INTEDG0 = 1;
    
    PHY_IRQ0 = 0;           // MRF89XA
    PHY_IRQ0_En = 1;        // INT0 enable (INT0:always high priority)

    PHY_IRQ1 = 0;           // MRF89XA
    PHY_IRQ1_IP = 1;        // INT1 => High Priority interrupt
    PHY_IRQ1_En = 1;        // MRF89XA

    /*******************************************************************/
    // Configure SPI2
    /*******************************************************************/
            
    SDI_TRIS = 1;
    SDO_TRIS = 0;
    SCK_TRIS = 0;
    
    SSP2STAT = 0xC0; // 1100 0000
    SSP2CON1 = 0b00101010;  // Fosc/8 => SPI@1MHz / this enables SSP1 too
    //SSP2CON1 = 0x21; // 0010 0001 - Fosc / 16 => SPI@1MHz if FOSC@16MHz
    //SSP2CON1 = 0x22;  // Fosc / 64

    PHY_RESETn = 0;         // No reset
    PHY_RESETn_TRIS = 1;    // !?! should be an output here: reset unused !?!

    /*******************************************************************/
    // Enable System Interupts
    /*******************************************************************/
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;    
}


