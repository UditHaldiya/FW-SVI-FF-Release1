/**
Copyright 2004-2005 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file key.c
    \brief BIOS routines for support of the keyboard buttons
    keyboard/display reconnect is also supported

    CPU: Any (with standard C compiler)

    OWNER: EP
    \n RCS: $Id$
    Created: 9/27/04 Author:  EP
*/

#include "config.h"
#include "mnwrap.h"
#include "mnassert.h"
#include "lpc2114io.h"
#include "bios_def.h"
#include "faultpublic.h"
#include "key.h"
#include "gpio.h"

typedef u32 rawkbd_t;

/* Type with all Keyboard and DI related variables */
typedef struct
{
    rawkbd_t key_debounced;
    rawkbd_t key_debounced_l;
    rawkbd_t key_latch;
    rawkbd_t key_up_transition;
    rawkbd_t key_down_transition;
    bool_t keyboard_locked ; /* 1=locked 0=keyboard usable */
    bool_t di_debounced; /* sampled and debounced state of DI pin. */
    bool_t di_latch;     /* latch to sample T-1 DI input state */
//    bool_t keyboard_connected; /* True if keyboard is recognized */
                           /* False if no connected keyboard is recognized */
    bool_t keyboard_reconnected ; /* TRUE= not connected to connected transitition detected */
                              /* FALSE= keyboard connected or disconnected */
    u8 latch_count;
}   KeyData_t;

/* Variable with all Keyboard and DI related variables */
static KeyData_t KeyData;

/**
    \brief poll the keyboard - called by A/D convert - but only if the LCD is NOT being updated

    \param none
*/
#define KBD_NUM_TEST_INPUTS 1U //up to 4, depending on paranoia level, but we agreed on at least 1.
#define KBD_NUM_BUTTONS 4U //3 functional and connected
#define KBD_NUM_READS (KBD_NUM_BUTTONS+KBD_NUM_TEST_INPUTS)
#define KBD_PIN 17 //kludge for missing HW definition
CONST_ASSERT((1U<<KBD_PIN)==GPIO1_SI_PB);

//While at it: we want to be able to shift all 8 bits comfortably; hence the shift direction
CONST_ASSERT(KBD_PIN > 8);
#define KBD_SHIFT_IN(keymask) ( ((keymask)>>1)|(GPIO1->IOPIN &  GPIO1_SI_PB))
#define KBD_DEBOUNCE_COUNT (3U)     // will yield 4=(3+1) matches

//After everything is shifted in: the first bit is farther out
#define KBD_TEST_MASK (((1U<<KBD_NUM_TEST_INPUTS) - 1U) << KBD_PIN)
#define KBD_MASK_KBDPRESENT (GPIO1_SI_PB>>KBD_NUM_TEST_INPUTS)
#define KBD_MASK_PUSHBUTTON_PLUS (GPIO1_SI_PB>>(KBD_NUM_TEST_INPUTS+1U))
#define KBD_MASK_PUSHBUTTON_STAR (GPIO1_SI_PB>>(KBD_NUM_TEST_INPUTS+2U))
#define KBD_MASK_PUSHBUTTON_MINUS (GPIO1_SI_PB>>(KBD_NUM_TEST_INPUTS+3U))

#define KBD_TEST_VALUE KBD_TEST_MASK //in active-low form, non-existent buttons are not pressed (pulled up)

//----------------------------------------------------------------------------------------------------------------------

/**
    \brief Initialize the keyboard driver

    \param none
*/
void bios_InitKeyboardDriver(void)
{
    KeyData.key_debounced        = KBD_MASK_PUSHBUTTON_PLUS|KBD_MASK_PUSHBUTTON_STAR|KBD_MASK_PUSHBUTTON_MINUS|KBD_TEST_MASK;  /* assumed state is present, not pressed, negative logic */
    KeyData.key_debounced_l      = KBD_MASK_PUSHBUTTON_PLUS|KBD_MASK_PUSHBUTTON_STAR|KBD_MASK_PUSHBUTTON_MINUS|KBD_TEST_MASK;
    KeyData.key_latch            = KBD_MASK_PUSHBUTTON_PLUS|KBD_MASK_PUSHBUTTON_STAR|KBD_MASK_PUSHBUTTON_MINUS|KBD_TEST_MASK;   /* assumed level is high */
    KeyData.key_up_transition    = 0U; /* assume no tranitions are recognized, positive logic */
    KeyData.key_down_transition  = 0U;
    KeyData.keyboard_locked      = false;
    KeyData.di_latch             = true; /* assumed level is high */
    KeyData.di_debounced         = true ;	/* assumed state is open */
//    KeyData.keyboard_connected   = true; /* assume a connceted keyboard */
    KeyData.keyboard_reconnected = false; /* recognize transition */
    return;
}

/** \brief A mandatory mopup interface
This version does nothing
*/
void key_Mopup(void)
{
}


void bios_HandleDigitalInput(void)
{
    u32 key_temp;
    u8_least keybit;

    if (lcd_IsLCDSelected())
    {   // Check if LCD is busy - exit if so.
        return;
    }

    key_StartDIPower();                /* at first start power for DI (charge DI input capacitor) */

    /* latch the state at the inputs of the keyboard shifting register (S0=High)*/
    lcd_SelectExtLCD();            /* S0=high(inverter) to shift out data bits */

    //AK: Shift in all bits (active low)
    key_temp = 0U;
    for(keybit=KBD_NUM_READS; keybit>0U; --keybit)
    {
        lcd_SetKeyShift_LO();          /* clk low->high (inverter) first data bit is avialable */

        /* So=low(inverter) for shifing out data */
        lcd_SetKeyShift_HI();
        lcd_DeSelectExtLCD();              // Set BOTH Select and RS HIGH

        key_temp = KBD_SHIFT_IN(key_temp);
    }

    if((key_temp & KBD_TEST_MASK) != KBD_TEST_VALUE)
    {
        //keyboard fault: log it but continue assuming no key pressed
        error_SetFault(FAULT_KEYPAD);
        bios_InitKeyboardDriver();
        KeyData.keyboard_reconnected = true; //to deal with a wiggly insertion of the LCD/keypad module
    }
    //Include connectedness in the debouncing stuff
    else if (key_temp == KeyData.key_latch)
    {
        if(KeyData.latch_count == 0)
        {
            //So, this is the new (or the old) stable value;
            //edges are mismatches with the previous
            u32 edge = key_temp ^ KeyData.key_debounced_l;

            //see if keypad is reconnected (was high, now low)
            if((~key_temp & edge & KBD_MASK_KBDPRESENT) != 0u)
            {
	            KeyData.keyboard_reconnected = true;
            }

            //Transitions are in normal logic (1= active)
            KeyData.key_down_transition |= ~key_temp & edge;
            KeyData.key_up_transition |= key_temp & edge;
            KeyData.key_debounced = key_temp;
            KeyData.key_debounced_l = key_temp;

        }
        else
        {
            --KeyData.latch_count;
        }
    }
    else
    {
        //The keys are bouncing; consider none pressed (active low!), but preserve last known connectedness (need it?)
        // We do not need to clear the Debounced value - it will be cleared automatically
        // by the debouncing code above. Just let it do the work.

        // KeyData.key_debounced |= KBD_MASK_PUSHBUTTON_PLUS|KBD_MASK_PUSHBUTTON_STAR|KBD_MASK_PUSHBUTTON_MINUS|KBD_TEST_MASK;
        KeyData.key_latch = key_temp;
        KeyData.latch_count = KBD_DEBOUNCE_COUNT;
    }

#if 0
    /* shift the states of the pushbuttons to a temporary variable *************************/
    if (GPIO1_SI_PB !=(GPIO1->IOPIN &  GPIO1_SI_PB))
    {
        key_temp = PUSHBUTTON_MINUS;
    }
    GPIO1->IOCLR = GPIO1_RS_LCD; /* latch Data bit */
    GPIO1->IOSET = GPIO1_RS_LCD;
    if (GPIO1_SI_PB !=(GPIO1->IOPIN &  GPIO1_SI_PB))
    {
        key_temp = key_temp | PUSHBUTTON_STAR;
    }
    GPIO1->IOCLR = GPIO1_RS_LCD; /* latch Data bit */
    GPIO1->IOSET = GPIO1_RS_LCD;
    if (GPIO1_SI_PB !=(GPIO1->IOPIN &  GPIO1_SI_PB))
    {
        key_temp = key_temp | PUSHBUTTON_PLUS;
    }
    /* Debounce input keys. If input levels are same as in last call, then input level is stable */
    if (key_temp == KeyData.key_latch)
    {
        KeyData.key_debounced = key_temp ;
    }
    KeyData.key_latch = key_temp ; /* sample key levels for next cycle */
    /* Check for key transitions ************************************************************/
    /* Bits are only updated if current state of key=true -> only not pressed to pressed recognized */
    KeyData.key_down_transition = (KeyData.key_down_transition | (KeyData.key_debounced &  (KeyData.key_debounced ^ KeyData.key_debounced_l)));
    /* Bits are only updated if current state of key=false -> only not pressed to pressed recognized */
    KeyData.key_up_transition = (KeyData.key_up_transition | (~(KeyData.key_debounced) &  (KeyData.key_debounced ^ KeyData.key_debounced_l)));
    /* sample debounced key states for next cylce */
    KeyData.key_debounced_l = KeyData.key_debounced ;
    /* Check state of connected Keyboard (connected or not) ***********************************/
    /* shift states to the uc */
    GPIO1->IOCLR = GPIO1_RS_LCD; /* latch Data bit */
    GPIO1->IOSET = GPIO1_RS_LCD;
    if (GPIO1_SI_PB == (GPIO1->IOPIN &  GPIO1_SI_PB))
    {   /* input is High, so no LCD interface is connected */
        KeyData.keyboard_connected = false ;
    }
    else
    {   /* input is low, LCD interface is connected */
 	    if (false == KeyData.keyboard_connected)
	    {   /* not connected in last state and connected now -> LCD interface reconnected */
	        KeyData.keyboard_reconnected = true ;
	        KeyData.keyboard_connected = true ;
	    }
	}
#endif

    /* DI input line is charged now */
    /* Debounce DI input line *****************************************************************/
    if (key_GetDIState())
    {   /* switch open recognized */
        if (true == KeyData.di_latch) /* If current state is same as in last cycle save debounced state */
        {
            KeyData.di_debounced = true ; /* debounced state is "open" */
        }
        KeyData.di_latch = true; /* sample level */
    }
    else
    {   /* switch closed recognized */
        if (false == KeyData.di_latch) /* If current state is same as in last cycle save debounced state */
        {
            KeyData.di_debounced = false ; /* debounced state is "closed" */
        }
        KeyData.di_latch = false; /* sample level */
    }

    lcd_DeSelectExtLCD();          /* Disable CS LCD */
    key_StopDIPower();             /* turn off power for DI */

}

/**
    \brief return the stae of the pushbuttons
    After call of function, the "pressed" flags are reset.

    \param[out] bool_t - state of digital input - true == open; false closed
     : bitcoded pushbuttons PUSHBUTTON_STAR, PUSHBUTTON_MINUS, PUSHBUTTON_PLUS
                  return value = pressed (down and released) state of pushbuttons
                  If bit = 1 (down and released)
                  pUpDown = state of pushbuttons
                  0 = pushbutton is up
                  1 = pushbutton is down
*/
u8 bios_ReadPushButtonStatus(u8 *pUpdown)
{
    u8 retval;
    u8 keystate;
    u32 keystate_raw;
    u32 edge_raw;

    MN_ASSERT(NULL != pUpdown);
    keystate = 0U;
    retval = 0U;
    if (KeyData.keyboard_locked)
    {   /* with keyboard locked pushbutton state is "up" and no edges are recognized */
    }
    else
    {   /* acquire pushbutton levels */
        keystate_raw = KeyData.key_debounced ;
        /* acquire pending edges */
        MN_ASSERT(!(*pUpdown > PUSHBUTTON_RELEASE_EDGE));
        MN_ENTER_CRITICAL();
            switch (*pUpdown)
            {
                case PUSHBUTTON_DOWN_EDGE:
                    edge_raw = KeyData.key_down_transition;
                    KeyData.key_down_transition = 0U;
                    break;

                case PUSHBUTTON_RELEASE_EDGE:
                default:
                    edge_raw = KeyData.key_up_transition;
                    KeyData.key_up_transition = 0U;
                    break;

            }
        MN_EXIT_CRITICAL();
        if((keystate_raw & KBD_MASK_PUSHBUTTON_PLUS)==0U)
        {
            keystate = PUSHBUTTON_PLUS;
        }
        if((keystate_raw & KBD_MASK_PUSHBUTTON_MINUS)==0U)
        {
            keystate |= PUSHBUTTON_MINUS;
        }
        if((keystate_raw & KBD_MASK_PUSHBUTTON_STAR)==0U)
        {
            keystate |= PUSHBUTTON_STAR;
        }
        if((edge_raw & KBD_MASK_PUSHBUTTON_PLUS)!=0U)
        {
            retval = PUSHBUTTON_PLUS;
        }
        if((edge_raw & KBD_MASK_PUSHBUTTON_MINUS)!=0U)
        {
            retval |= PUSHBUTTON_MINUS;
        }
        if((edge_raw & KBD_MASK_PUSHBUTTON_STAR)!=0U)
        {
            retval |= PUSHBUTTON_STAR;
        }

    } /* end of if keyboard locked */
    *pUpdown = keystate ;
    return (retval);
}

/**
    \brief return the debounced state of the digital input

    \param[out] bool_t - state of digital input - true == open; false closed
*/
bool_t bios_ReadDiState(void)
{
    return(KeyData.di_debounced);
}

/**
    \brief set the lock state of the keyboard

    \param[in] bool_t lock - lock state - flase == locked
*/
void bios_LockKeyboard(bool_t lock)
{
    KeyData.keyboard_locked = lock; /* sample state */
    if (false == lock)
    {   /* reset pending keyboard flags, if enabled again */
        KeyData.key_up_transition = (u8)0;
        KeyData.key_down_transition = (u8)0;
    }
    return;
}

/**
    \brief return the state of keyboard connection

    \param[out] bool_t - true keyboard removed and now reconnected
*/
bool_t bios_KeyboardReconnected(void)
{
    bool_t Kcon;

    MN_ENTER_CRITICAL();
        Kcon = KeyData.keyboard_reconnected;   /* sample marker */
        KeyData.keyboard_reconnected = false ; /* reset marker */
    MN_EXIT_CRITICAL();
    return(Kcon);
}

/* End of the source */
