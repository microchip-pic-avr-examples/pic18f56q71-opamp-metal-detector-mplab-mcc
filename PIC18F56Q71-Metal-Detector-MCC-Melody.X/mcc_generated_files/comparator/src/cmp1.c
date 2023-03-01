/**
 * CMP1 Generated Driver File.
 * 
 * @file cmp1.c
 * 
 * @ingroup cmp1
 * 
 * @brief This file contains the API implementation for the CMP1 driver.
 *
 * @version CMP1 Driver Version 2.12.0
*/
 /*
© [2023] Microchip Technology Inc. and its subsidiaries.

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

 /**
   Section: Included Files
 */

#include <xc.h>
#include "../cmp1.h"
#include "../../system/interrupt.h"

/**
  Section: CMP1 APIs
*/

void CMP1_Initialize(void) 
{
    // CHYS 10mV Comparator hysteresis; CON enabled; CPOL not inverted; CSYNC asynchronous; 
    CM1CON0 = 0x82;

    // CINTN no_intFlag; CINTP no_intFlag; CSP 30ns hi_speed; 
    CM1CON1 = 0x0;
    
    // CNCH CIN2-; 
    CM1NCH = 0x2;
    
    // CPCH DAC1_OUT; 
    CM1PCH = 0x3;    
}

bool CMP1_GetOutputStatus(void) 
{
  return CM1CON0bits.C1OUT; /* Note : As per datasheet, CxOUT bit in CMOUT register (if relevant) 
                                                   is mirror copy of CMxCON0.OUT */
}

/**
 * @ingroup cmp1
 * @brief Implements the Interrupt Service Routine (ISR) for the CMP interrupt when vectored interrupt is enabled.
 * @param None.
 * @return None.
 */
void __interrupt(irq(CMP1),base(8)) CMP1_ISR()
{
    // Clear the CMP1 interrupt flag
    PIR1bits.C1IF = 0;
    
}

