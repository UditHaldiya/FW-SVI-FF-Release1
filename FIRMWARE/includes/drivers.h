/*****************************************************************************
*  File drivers.h                        
*  MNCB - Header for peripheral driver functions
*                                                   
*******************************************************************************
*  WRITTEN :                                     
*      June 2004
*  REVISION HISTORY:                                  
*
*
*
******************************************************************************/
                 
/*****************************************************************************/
/*                                                                            */
/*    Copyright 2004 - 2004 as an unpublished trade secret.                         */
/*                                                                           */
/*    This document and all information herein is the property of            */
/*    Valve & Controls Division of Dresser Industries,Inc. It is             */
/*    confidential and must not be made public or copied and is              */
/*    subject to return upon demand.                                         */
/*                                                                           */
/*****************************************************************************/
#ifndef DRIVERS_H
#define DRIVERS_H

#define DASetValues PWM_Write

#define USE_2_5   0 
#define ORIG      0 

#define BUTTON1 (1u << 25)
#define BUTTON2 (1u << 24)
#define BUTTON3 (1u << 23)

#define GREEN_BOARD_BUTTON_MASK (BUTTON1 | BUTTON2 | BUTTON3)

// defines and prototypes for internal A/D converter
#define AD0     (1u << 0)
#define AD1     (1u << 1)
#define AD2     (1u << 2)
#define AD3     (1u << 3)

#define ADC_IP_PRESSURE AD0
#define ADC_IP_CURRENT  AD1
#define ADC_SETPOINT    AD2
#define ADC_POT         AD3

extern void ADCI_Init       (void);
extern u32  ADCI_Read       (u32 channel);
// end defines and prototypes for internal A/D converter


extern void DAC_Write       (u32 value);
extern void SPI_Init        (void);
void ADC_SPI_Init(void);
void ADC_PollCycle(void);

extern void FRAM_Init(void);
extern u32 FRAM_WriteString(const u8 * data,u32 address,u32 length);
extern u32 FRAM_ReadString (u8 * data,u32 address,u32 length);
extern void PWM_Write       (u32 value);
extern void PWM_WriteX      (u32 value);
extern void PWM_Write2      (u32 value);
extern void PWM_Init        (void);
extern void LCD_WriteString (const u8 * data,u32 address,u32 length);

extern void BTN_Poll        (void);
extern u32  BTN_Read        (void);
extern u32  BTN_Peek        (void);
extern void I2C_Init        (void);
extern void I2C_Test        (void);
#endif


