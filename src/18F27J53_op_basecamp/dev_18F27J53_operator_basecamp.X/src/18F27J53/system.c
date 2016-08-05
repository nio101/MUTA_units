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

// ---------------------------------------------------------------
// PIC18F27J53 Configuration

#pragma config WDTEN = OFF      // Watchdog Timer (Disabled - Controlled by SWDTEN bit)
#pragma config PLLDIV = 2       // PLL Prescaler Selection (Divide by 2 (8 MHz oscillator input))
#pragma config CFGPLLEN = ON    // PLL Enable Configuration Bit (PLL Enabled)
#pragma config STVREN = OFF     // Stack Overflow/Underflow Reset (Disabled)
#pragma config XINST = OFF       // Extended Instruction Set (Enabled)
#pragma config CPUDIV = OSC3_PLL3  // CPU Clock = 48MHz/3 => 16MHz
#pragma config CP0 = OFF        // Code Protect (Program memory is not code-protected)
#pragma config OSC = INTOSCPLL  // Oscillator (INTOSCPLL)
#pragma config CLKOEC = OFF     // EC Clock Out Enable Bit  (CLKO output disabled on the RA6 pin)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor (Disabled)
#pragma config IESO = OFF       // Internal External Oscillator Switch Over Mode (Disabled)
#pragma config RTCOSC = INTOSCREF   // RTCC Clock Select (RTCC uses INTRC)
#pragma config LS48MHZ = SYS48X8    // Low Speed USB mode with 48 MHz system clock bit (System clock at 48 MHz USB CLKEN divide-by is set to 8)


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
    // System is online... at 8MHz since PLL is not on...
    // Turn PLL ON and wait for the proper delay to switch to 48MHz
    unsigned int pll_startup_counter = 600;
    OSCTUNEbits.PLLEN = 1;  //Enable the PLL and wait 2+ms until the PLL
                            //locks before enabling USB module
    while(pll_startup_counter--);

    // Enable the USB status LED and turn it off
    //LED_Enable(LED_USB_DEVICE_STATE);
    //LED_On(LED_USB_DEVICE_STATE);

    // configure IO as OUTPUT => 0uA mesuré en sleep !!!
    TRISA = 0;
    TRISB = 0;
    TRISC = 0; 
    
    LATCbits.LATC0 = 1;     // turn off the LED
    
    // turn RTCC and RTCC/alarm ON
    //RtccInitClock();   // config says RTCC should use INTRC
                         // RtccInitClock enables timer1 crystal use!
    RTCCFG = 0x0;
    RTCCAL = 0x00;  // signed value to calibrate RTCC on INTOSC
    /*if(mRtccIsOn()) // - not necessary!
    {
       if(!mRtccIsWrEn())
       {
           RtccWrOn();
       }
       mRtccOff();
    }*/
    mRtccWrOff();
    RtccWrOn(); //write enable the rtcc registers
    mRtccOn(); //enable the rtcc module
    mRtccAlrmEnable(); //enable the rtcc alarm
    RTCCFGbits.RTCPTR0 = 0; //RTCVALH/L will point to min/seconds values
    RTCCFGbits.RTCPTR1 = 0;
    RTCVALHbits.RTCVALH = 0x00; // write 00 to min & sec
    RTCVALLbits.RTCVALL = 0x00;
    // RTCC alarm setup
    ALRMCFGbits.AMASK = 0b0011; // alarm every minute
    //ALRMCFGbits.AMASK = 0b0010; // alarm every 10sec
    ALRMCFGbits.CHIME = 1;      // chime ON, alarm is repeated indefinitly
    ALRMCFGbits.ALRMEN = 1;     // Alarm ON
    mRtccWrOff();   // write disable rtcc regs

    // setup RTCC interrupt
    IPR3bits.RTCCIP = 0; // low priority for RTCC
    PIR3bits.RTCCIF = 0;  // clear RTCC interrupt flag
    PIE3bits.RTCCIE = 1;  // RTCC interrupt enable

    /*******************************************************************/
    // Configure PPS Related Pins
    /*******************************************************************/
    // Unlock to config PPS
    EECON2 = 0x55;
    EECON2 = 0xAA;
    PPSCONbits.IOLOCK = 0;

    // inputs
    RPINR1 = 13;                // Mapping IRQ1 to RC2(RP13)
    RPINR21 = 12;               // Mapping SDI2 to RC1(RP12)    
    RPINR22 = 5;                // Mapping SCK2IN to RB2(RP5)

    // outputs
    RPOR4 = 10;                 // Mapping SDO2 to RB1(RP4)
    RPOR5 = 11;                 // Mapping SCK2OUT to RB2(RP5)
#ifdef MK1
    RPOR17 = 14;                // RC6/RP17: ICCP1 output (mk1 only)
#else #ifdef MK3
    RPOR11 = 14;                // RC0/RP11: ICCP1 output (mk3 and +)
#endif

    // Lock System
    EECON2 = 0x55;
    EECON2 = 0xAA;
    PPSCONbits.IOLOCK = 1;

    /*******************************************************************/
    // AN0 & AN1 Analog Pins others Digital Pins
    /*******************************************************************/
    //ANCON0 = 0xFC;
    //ANCON1 = 0x1F;

    // no analog pin used, all digital
    ANCON0 = 0xFF;
    ANCON1 = 0x1F;

    // RTCC Off
    RTCEN = 0;
    RTCOE = 0;
    
    /*******************************************************************/
    // Config RF Radio
    /*******************************************************************/

    RCONbits.IPEN = 1;  // enable interrupt priorities management

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
    SSP2CON1 = 0x21; // 0010 0001 - Fosc / 16 => SPI@1MHz if FOSC@16MHz
    //SSP2CON1 = 0x22;  // Fosc / 64

    PHY_RESETn = 0;         // No reset
    PHY_RESETn_TRIS = 1;    // !?! should be an output here: reset unused !?!

    /*******************************************************************/
    // Enable System Interupts
    /*******************************************************************/
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1; 
    
    /*******************************************************************/
    // Configure LED PWM & timer
    /*******************************************************************/   
    
    // setup CCP1 PWM to drive led on RC0/RP11
    // PPS: already assigned CCP1 to RC0/RP11
#ifdef MK1
    TRISCbits.TRISC6 = 1; // RC6 => input (PWM LED OFF) (mk1 only)
#else #ifdef MK3
    TRISCbits.TRISC0 = 1; // RC0 => input (PWM LED OFF) (mk3 and +)
#endif    

    CCPTMRS0 = 0x00; //select timer resources, Timer2 for CCP1 (and /2/3)

    PR2 = 0xF9; // load with PWM period value (fixed at 1 kHz)
    CCP1CONbits.DC1B = 0b00;    // least significant bits of duty cycle
    CCP1CONbits.CCP1M = 0b1100; // PWM mode
    CCPR1L = 0x00; // eight high bits of duty cycle

    T2CONbits.T2CKPS = 0b10; // timer2 prescale: /16
    T2CONbits.TMR2ON = 0b1;  // timer2 ON

#ifdef MK1
    TRISCbits.TRISC6 = 0; // RC6 => output (PWM LED ON) (mk1 only)
#else #ifdef MK3
    TRISCbits.TRISC0 = 0; // RC0 => output (PWM LED ON) (mk3 and +)
#endif    
    // duty cycle is zero here so the LED *should* be OFF

    // setup timer4 to update the LED brightness every 5ms (via interrupt)
    T4CONbits.T4OUTPS = 0b1111; // :16 postscaler
    T4CONbits.T4CKPS = 0b11;    // :16 prescaler
    PR4 = 234;              // period for an interrupt every 15ms @16MHz
    IPR3bits.TMR4IP = 0;    // interrupt for timer4 is low priority
    PIR3bits.TMR4IF = 0;    // clear flag
    PIE3bits.TMR4IE = 1;    // enable interrupt    
    T4CONbits.TMR4ON = 1; // timer4 is ON
}


