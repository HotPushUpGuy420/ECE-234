// .. "Copyright (c) 2008 Robert B. Reese, Bryan A. Jones, J. W. Bruce ("AUTHORS")"
//    All rights reserved.
//    (R. Reese, reese_AT_ece.msstate.edu, Mississippi State University)
//    (B. A. Jones, bjones_AT_ece.msstate.edu, Mississippi State University)
//    (J. W. Bruce, jwbruce_AT_ece.msstate.edu, Mississippi State University)
//
//    Permission to use, copy, modify, and distribute this software and its
//    documentation for any purpose, without fee, and without written agreement is
//    hereby granted, provided that the above copyright notice, the following
//    two paragraphs and the authors appear in all copies of this software.
//
//    IN NO EVENT SHALL THE "AUTHORS" BE LIABLE TO ANY PARTY FOR
//    DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
//    OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE "AUTHORS"
//    HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//    THE "AUTHORS" SPECIFICALLY DISCLAIMS ANY WARRANTIES,
//    INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
//    AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
//    ON AN "AS IS" BASIS, AND THE "AUTHORS" HAS NO OBLIGATION TO
//    PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
//
//    Please maintain this header in its entirety when copying/modifying
//    these files.
//
// ***********
// app_mailC.c
// ***********
#define ESOS_USE_IRQS

// INCLUDEs go here  (First include the main esos.h file)
//      After that, the user can include what they need
#include    "esos.h"
#ifdef __linux
  #include    "esos_pc.h"
  #include    "esos_pc_stdio.h"
  // INCLUDE these so our printf and other PC hacks work
  #include <stdio.h>
  #include <sys/select.h>
  #include <termios.h>
  #include <unistd.h>
#else
  #include    "esos_pic24.h"
  #include    "esos_pic24_rs232.h"
  #include <stdio.h>
#endif

#ifdef __linux
  uint8_t	LED1 = TRUE;
  #define CONFIG_LED1()
#else
    #if (HARDWARE_PLATFORM == EMBEDDED_F14)
      #define CONFIG_LED1()         CONFIG_RF4_AS_DIG_OUTPUT()
      #define LED1                  _LATF4
    #else
      #define CONFIG_LED1()         do{CONFIG_RB15_AS_DIG_OUTPUT();ENABLE_RB15_OPENDRAIN();}while(0)
      #define LED1                  _LATB15
  #endif
#endif

/*
 * PROTOTYPEs go here
 *
 */
void reverseString(char *psz_s1, char *psz_s2);
uint32_t    randomNumInRange(uint32_t u32_lo, uint32_t u32_hi);

ESOS_USER_TASK(sender_C0);
ESOS_USER_TASK(sender_C1);
ESOS_USER_TASK(recipient_C);

// GLOBALs go here
//  Generally, the user-created semaphores will be defined/allocated here

// timer globals
uint32_t        u32_myT1Count = 0;

struct stTask*    pst_MyTasks[3];

/*
 * Simulate the timer ISR found on a MCU
 *   The PC doesn't have a timer ISR, so this task will periodically
 *   call the timer services callback instead.
 *   USED ONLY FOR DEVELOPMENT AND TESTING ON PC.
 *   Real MCU hardware doesn't need this task
 */
ESOS_USER_TASK( __simulated_isr ) {
  ESOS_TASK_BEGIN();
  while (TRUE) {
    // call the ESOS timer services callback just like a real H/W ISR would
    __esos_tmrSvcsExecute();
    ESOS_TASK_WAIT_TICKS(  1 );

  } // endof while(TRUE)
  ESOS_TASK_END();
} // end child_task


/************************************************************************
 * User supplied functions
 ************************************************************************
 */

/*
 * Returns a random number with a value between the two arguments.
 *
 * /todo  Yes, I know this routine is cheesy.  But, it works and
 * I am in a really big hurry right now.
 */

uint32_t    randomNumInRange(uint32_t u32_lo, uint32_t u32_hi) {
  uint32_t  u32_d1, u32_d2, u32_d4, u32_ret;
  UINT32  U32_temp;

  while (TRUE) {
    u32_d4 = esos_GetRandomUint32();
    u32_ret = u32_lo + u32_d4;
    if (u32_ret <= u32_hi) return u32_ret;

    U32_temp._uint32 = u32_d4;
    u32_d2 = U32_temp.u16LoWord ^ U32_temp.u16HiWord;
    u32_ret = u32_lo + u32_d2;
    if (u32_ret <= u32_hi) return u32_ret;

    u32_d1 = U32_temp.u8LoLsb ^ U32_temp.u8LoMsb ^ U32_temp.u8HiLsb ^ U32_temp.u8HiMsb;
    u32_ret = u32_lo + u32_d1;
    if (u32_ret <= u32_hi) return u32_ret;
  } //endwhile
} //end randomNumInRange


// user-created timer callback
ESOS_USER_TIMER( swTimerCounter ) {
  u32_myT1Count++;
} //endof swTimerCounter

// user-created timer callback
ESOS_USER_TIMER( swTimerLED ) {
  LED1 = !LED1;
#ifdef __linux
  printf("\a");
  fflush(stdout);
#endif
} //endof swTimerLED


/**********************************************************
**
**     a few tasks to send mail messages to each other
**
************************************************************/

ESOS_USER_TASK( sender_C0 ) {
  uint32_t                      u32_rnd;
  static  uint8_t               u8_cnt;
  static  ESOS_TASK_HANDLE    hTask;
  static  MAILMESSAGE         st_Message;

  ESOS_TASK_BEGIN();
  u8_cnt=0;
  // store handle to our recipient ESOS task
  hTask = esos_GetTaskHandle( recipient_C );

  while (TRUE) {

    // create/fill in the local copy of the message
    // we will send a single uint8 as data payload
    ESOS_TASK_MAKE_MSG_UINT8(st_Message, u8_cnt);

    // simulate a task that does other things if the
    // receipient task mailbox is full

    if (ESOS_TASK_MAILBOX_GOT_AT_LEAST_DATA_BYTES( hTask, sizeof(uint8_t) ) ) {
      // ESOS task mailboxes are stored locally in each task.
      // but we already now the recipient task has enough space
      // in their mailbox, so send message
      printf("C0 sending MESSAGE %d\n", u8_cnt);
      ESOS_TASK_SEND_MESSAGE(hTask, &st_Message);
    } else {
      printf("C0 doing useful work instead of mailing. Discarding MESSAGE %d.\n", u8_cnt );
    }

    u8_cnt++;
    if (u8_cnt>50) u8_cnt=0;

    // wait a random amount of time between sending mail messages
    u32_rnd = 1+(0x0F & esos_GetRandomUint32());
    u32_rnd <<= 7;
    ESOS_TASK_WAIT_TICKS( u32_rnd);

  } // endof while(TRUE)
  ESOS_TASK_END();
} // end sender_C0()

ESOS_USER_TASK( sender_C1 ) {
  static    uint32_t        u32_rnd;
  static  uint8_t         u8_cnt;
  static  ESOS_TASK_HANDLE    hTask;
  static  MAILMESSAGE       st_Message;

  ESOS_TASK_BEGIN();
  u8_cnt= 100;
  hTask = esos_GetTaskHandle( recipient_C );

  while (TRUE) {
    ESOS_TASK_MAKE_MSG_UINT8(st_Message, u8_cnt);               // create message locally
    ESOS_TASK_WAIT_ON_TASKS_MAILBOX_HAS_AT_LEAST(hTask, sizeof(uint8_t));     // wait until recipient has mailbox space

    // get a random number
    u32_rnd = 1+(0x0F & esos_GetRandomUint32());
    // create a random message that wants delivery confirmation (1-in-4 chance)
    if ( (u32_rnd % 4) == 0 ) {
      st_Message.u8_flags |= ESOS_MAILMESSAGE_REQUEST_ACK;
      printf("C1 sending MESSAGE with ACK request %d\n", u8_cnt);
      ESOS_TASK_WAIT_ON_DELIVERY(hTask, &st_Message);
    } else {
      printf("C1 sending MESSAGE %d\n", u8_cnt );
      ESOS_TASK_SEND_MESSAGE(hTask, &st_Message);
    }

    u8_cnt++;
    if (u8_cnt>150) u8_cnt=100;

    u32_rnd <<= 7;
    ESOS_TASK_WAIT_TICKS( u32_rnd);

  } // endof while(TRUE)
  ESOS_TASK_END();
} // end sender_C1()


//TASK that doesn't check mail very often
ESOS_USER_TASK( recipient_C ) {
  uint32_t                u32_rnd;
  static MAILMESSAGE    stMsg;
  static ESOS_TASK_HANDLE           hSenderC0, hSenderC1;

  ESOS_TASK_BEGIN();
  hSenderC0 = esos_GetTaskHandle( sender_C0);
  hSenderC1 = esos_GetTaskHandle( sender_C1);
  while (TRUE) {

    // create a random delay to simulate being "busy"
    u32_rnd = 1+(0x0F & esos_GetRandomUint32());
    u32_rnd <<= 10;
    ESOS_TASK_WAIT_TICKS( u32_rnd );

    // check for incoming mail
    ESOS_TASK_WAIT_FOR_MAIL();

    // keep reading mail messages until they are all processed
    while ( ESOS_TASK_IVE_GOT_MAIL() ) {
      // make local copy of message (frees up mailbox space)
      ESOS_TASK_GET_NEXT_MESSAGE( &stMsg );
      printf("Got a message from ");
      if (ESOS_IS_TASK_SENDER( hSenderC0, stMsg)) {
        printf("sender_C0");
      } else if (ESOS_IS_TASK_SENDER( hSenderC1, stMsg)) {
        printf("sender_C1");
      } else {
        printf("UNKNOWN");
      }
      printf (" containing %d          enroute time = %ld ms\n", stMsg.au8_Contents[0], esos_GetSystemTick()-ESOS_GET_MSG_POSTMARK(stMsg) );
    } //endof while()
  } // endof while(TRUE)
  ESOS_TASK_END();
} // end recipient_C()



/******************************************************************************
 * Function:        void user_init(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        user_init is a centralized initialization routine where
 *                  the user can setup their application.   It is called
 *                  automagically by ES_OS during the operating system
 *                  initialization.
 *
 * Note:            The user should set up any state machines and init
 *                  all application variables.  They can also turn on
 *                  any needed peripherals here.
 *
 *                  The user SHALL NOT mess with the interrupt hardware
 *                  directly!!!  The ES_OS must be aware of the interrupts
 *                  and provides osXXXXXXX functions for the user to use.
 *                  Using these ES_OS-provided functions, the user may
 *                  (and probably should) initialize, register, and enable
 *                  interrupts in this routine.
 *
 *                  Furthermore, the user should register AT LEAST one
 *                  user application task here (via esos_RegisterTask) or
 *                  the ES_OS scheduler will have nothing to schedule
 *                  to run when this function returns.
 *
 *****************************************************************************/
void user_init(void) {

  CONFIG_LED1();
  /*
   * Now, let's get down and dirty with ESOS and our user tasks
   *
   *   Once tasks are registered, they will start executing in
   *   the ESOS scheduler.
   */

  // register our little ESOS task to mimic MCU's TIMER T1 IRQ which kicks off
  // the ESOS S/W timers when they expire
  esos_RegisterTask( __simulated_isr );

  /* ====================================================================
   * REGISTER SOME USER TASKS
   * ====================================================================
   */
  esos_RegisterTimer( swTimerLED, 500 );
  esos_RegisterTask( sender_C0 );
  esos_RegisterTask( sender_C1 );
  esos_RegisterTask( recipient_C );

  __esos_unsafe_PutString( HELLO_MSG );
} // end user_init()
