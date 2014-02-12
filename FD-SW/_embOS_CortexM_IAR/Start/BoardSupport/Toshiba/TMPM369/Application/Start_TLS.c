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
File    : Start_TLS.c
Purpose : Sample application to demonstrate usage of TLS
          TLS support is CPU and compiler specific and may not be
          implemented in all OS ports.

NOTE    : Usage of thread local storage from different tasks requires
          thread safe system libraries, because the TLS is located
          on the heap memory and is dynamically created during runtime.

          xmtx.c from the Setup folder has to be added to the project
                 to support thread safe libraries.

--------  END-OF-HEADER  ---------------------------------------------
*/

#include "RTOS.h"
#include <errno.h>

OS_STACKPTR int StackHP[128], StackLP[128];  /* Task stacks         */
OS_STACKPTR int StackMP[128];                /* Task stacks         */
OS_TASK TCBHP, TCBLP;                        /* Task-control-blocks */
OS_TASK TCBMP;                               /* Task-control-blocks */

/*********************************************************************
*
*       HPTask() with thread local storage
*/
static void HPTask(void) {
  OS_TLS_Init();       // Initialize TLS for this task
  while (errno != 0) {
    // errno is local to this task, we should not arrive here
  }
  errno = 3;           // Simulate a task specific error
  while (1) {
    OS_Delay (10);
      while (errno != 3) {
        // errno is local to this task, we should not arrive here
      }
  }
}

/*********************************************************************
*
*       MPTask() with thread local storage
*/
static void MPTask(void) {
  OS_TLS_Init();       // Initialize TLS for this task
  while (errno != 0) {
    // errno is local to this task, we should not arrive here
  }
  errno = 2;           // Simulate a task specific error
  while (1) {
    OS_Delay (10);
    while (errno != 2) {
      // errno is local to this task, we should not arrive here
    }
  }
}

/*********************************************************************
*
*       LPTask() without thread local storage
*/
static void LPTask(void) {  // This task does not use TLS
  while (errno != 1) {
    // errno is not local to this task, we expect the global value
    // set in main() and should not arrive here
  }
  while (1) {
    OS_Delay (50);
  }
}

/*********************************************************************
*
*       main()
*
*********************************************************************/

int main(void) {
  OS_IncDI();                      /* Initially disable interrupts  */
  errno = 1;                       /* Simulate an error             */
  OS_InitKern();                   /* Initialize OS                 */
  OS_InitHW();                     /* Initialize Hardware for OS    */
  /* You need to create at least one task here !                    */
  OS_CREATETASK(&TCBHP, "HP Task", HPTask, 100, StackHP);
  OS_CREATETASK(&TCBMP, "MP Task", MPTask,  70, StackMP);
  OS_CREATETASK(&TCBLP, "LP Task", LPTask,  50, StackLP);
  OS_Start();                      /* Start multitasking            */
  return 0;
}

