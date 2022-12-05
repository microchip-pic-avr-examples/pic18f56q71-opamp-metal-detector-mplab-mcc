 /*
 * MAIN Generated Driver File
 * 
 * @file main.c
 * 
 * @defgroup main MAIN
 * 
 * @brief This is the generated driver implementation file for the MAIN driver.
 *
 * @version MAIN Driver Version 1.0.0
*/

/*
© [2022] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/
#include "mcc_generated_files/system/system.h"

/*
    Main application
*/

#define Delay_100ms               100
#define Delay_1us                 1

#define CALIB_STEPS_NO            50          // Choose a number of steps for the calibration of the threshold
#define CALIB_ERROR               5
#define FIRST_ACQ                 0

void app_ChargingPulseGen(void);
void app_PrepareEnvelope(void);
uint16_t app_ThrsldTimeMeas(void);
void app_CheckEnvelopeThrsld(uint16_t period);
void app_ResetTimer(void);
uint16_t app_ThrsldMeasDefaultValue(void);

uint16_t Measured_Threshold_Metal;

int main(void)
{
    SYSTEM_Initialize();                                             
    
    uint16_t measuredPeriod;
    
    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts 
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global Interrupts 
    // Use the following macros to: 

    // Enable the Global High Interrupts 
    //INTERRUPT_GlobalInterruptHighEnable(); 

    // Disable the Global High Interrupts 
    //INTERRUPT_GlobalInterruptHighDisable(); 

    // Enable the Global Low Interrupts 
    //INTERRUPT_GlobalInterruptLowEnable(); 

    // Disable the Global Low Interrupts 
    //INTERRUPT_GlobalInterruptLowDisable(); 

    Measured_Threshold_Metal = app_ThrsldMeasDefaultValue();     /* Set the threshold for metal detection, LED will toggle until the calibration is fulfilled */
    
    while(1)
    {
        app_PrepareEnvelope();                                   /* Discharge / Charge the external capacitor, generate PWM pulse */    
       
        measuredPeriod = app_ThrsldTimeMeas();                   /* Store time between top and threshold set for envelope */ 

        app_CheckEnvelopeThrsld(measuredPeriod);                 /* Check value and toggle LED */    

        app_ResetTimer();                                        /* Reset timer counter */     
        
        __delay_ms(Delay_100ms);                                 /* Delay between cycles */
    }    
}

void app_PrepareEnvelope(void)
{
    OPA1CON4bits.PTRES = 1;                                      /* Discharge the external capacitor */   
    app_ChargingPulseGen();                                      /* Generate PWM pulse */
    OPA1CON4bits.PTRES = 0;                                      /* Charge the external capacitor */
}

void app_ChargingPulseGen(void)
{
    T2CONbits.ON = 1;                                            /* Start 1 PWM pulse */   
    while(!PIR3bits.TMR2IF);                                     /* Wait for the entire PWM pulse, ignore first peak of LC signal */
    PIR3bits.TMR2IF = 0;                                         /* Clear timer flag */
}

uint16_t app_ThrsldTimeMeas(void)
{
    __delay_us(Delay_1us);                                       /* Prevent false spikes while charging external capacitor */
    while(T1GCONbits.GVAL);                                      /* Wait for envelope to drop below threshold */
    return Timer1_Read();                                        /* Return the measured period until threshold is reached */
}

void app_CheckEnvelopeThrsld(uint16_t period)
{
    if(period < Measured_Threshold_Metal)                        /* Compare the measured period with no metal detected threshold */
        LED_SetLow();                                            /* If period is lower than threshold, set LED on */
    else
        LED_SetHigh();                                           /* If period is higher than threshold, set LED off */
}

void app_ResetTimer(void)
{
    Timer1_Write(0);                                             /* Reset the timer counter by writing 0 */
}

uint16_t app_ThrsldMeasDefaultValue(void)
{   
    __delay_ms(Delay_100ms);                                     /* Used for system initialization */
    
    uint16_t thrshld = 0;
    uint8_t calibCounter = FIRST_ACQ;
    uint16_t measPer;
    
    while (calibCounter <= CALIB_STEPS_NO)                       /* Number of iterations */
    {
        LED_Toggle();                                            /* Toggle the LED while calibration is in progress*/
        
        app_PrepareEnvelope();                                   /* PWM Pulse generation */
        measPer = app_ThrsldTimeMeas();                
        app_ResetTimer();
        
        switch (calibCounter)                                    /* Dismiss first read */
        {
            case FIRST_ACQ: 
                break;
            default: 
                thrshld += measPer;
                break;
        }

        calibCounter++;
        __delay_ms(Delay_100ms);
    }
    
    thrshld = (thrshld / CALIB_STEPS_NO) - CALIB_ERROR;          /* Average value of the Threshold when no metal is in the proximity of the LC circuit */
    
    return thrshld; 
}