/*********************************************************************
*               SEGGER MICROCONTROLLER GmbH & Co KG                  *
*       Solutions for real time microcontroller applications         *
**********************************************************************
*                                                                    *
*       (c) 1995 - 2012  SEGGER Microcontroller GmbH & Co KG         *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
*       embOS * Real time operating system for microcontrollers      *
*                                                                    *
*                                                                    *
*       Please note:                                                 *
*                                                                    *
*       Knowledge of this file may under no circumstances            *
*       be used to write a similar product or a real-time            *
*       operating system for in-house use.                           *
*                                                                    *
*       Thank you for your fairness !                                *
*                                                                    *
**********************************************************************
*                                                                    *
*       OS version: 3.86d                                            *
*                                                                    *
**********************************************************************

----------------------------------------------------------------------
File    : BSP.c
Purpose : BSP for Toshiba Topas TMPM369 eval board
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "BSP.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define PORTC     (*(volatile unsigned int*)(0x400C0200)) // Port C register
#define PORTC_CR  (*(volatile unsigned int*)(0x400C0204)) // Port C control register
#define PORTC_UP  (*(volatile unsigned int*)(0x400C020C)) // Port C pull-up control register

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/

/*********************************************************************
*
*       BSP_Init()
*/
void BSP_Init(void) {
  PORTC_CR = 0xFF; // Enable outputs
  PORTC_UP = 0xFF; // Enable Pullups
}

/*********************************************************************
*
*       LED switching routines
*       LEDs are switched on with low level on port lines
*/
void BSP_SetLED(int Index) {
  if (Index < 8) {
    PORTC |= (1uL << Index);
  }
}

void BSP_ClrLED(int Index) {
  if (Index < 8) {
    PORTC &= ~(1uL << Index);
  }
}

void BSP_ToggleLED(int Index) {
  if (Index < 8) {
    PORTC ^= (1uL << Index);
  }
}

/****** End Of File *************************************************/
