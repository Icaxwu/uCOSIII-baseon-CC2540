/*
*********************************************************************************************************
*                                                uC/OS-III
*                                          The Real-Time Kernel
*
*
*                           (c) Copyright 2009-2010; Micrium, Inc.; Weston, FL
*                    All rights reserved.  Protected by international copyright laws.
*
*                                           ARM Cortex-M3 Port
*
* File    : OS_CPU_C.C
* Version : V3.02.01
* By      : JJL
*           BAN
*
* LICENSING TERMS:
* ---------------
*           uC/OS-III is provided in source form for FREE short-term evaluation, for educational use or 
*           for peaceful research.  If you plan or intend to use uC/OS-III in a commercial application/
*           product then, you need to contact Micrium to properly license uC/OS-III for its use in your 
*           application/product.   We provide ALL the source code for your convenience and to help you 
*           experience uC/OS-III.  The fact that the source is provided does NOT mean that you can use 
*           it commercially without paying a licensing fee.
*
*           Knowledge of the source code may NOT be used to develop a similar product.
*
*           Please help us continue to provide the embedded community with the finest software available.
*           Your honesty is greatly appreciated.
*
*           You can contact us at www.micrium.com, or by phone at +1 (954) 217-2036.
*
* For       : ARMv7M Cortex-M3
* Mode      : Thumb2
* Toolchain : IAR EWARM
*********************************************************************************************************
*/

#define   OS_CPU_GLOBALS

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *os_cpu_c__c = "$Id: $";
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <os.h>

/*
*********************************************************************************************************
*                                           IDLE TASK HOOK
*
* Description: This function is called by the idle task.  This hook has been added to allow you to do
*              such things as STOP the CPU to conserve power.
*
* Arguments  : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/

void  OSIdleTaskHook (void)
{
    
#if OS_CFG_APP_HOOKS_EN > 0u
    if (OS_AppIdleTaskHookPtr != (OS_APP_HOOK_VOID)0) {
        (*OS_AppIdleTaskHookPtr)();
    }
#endif
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       OS INITIALIZATION HOOK
*
* Description: This function is called by OSInit() at the beginning of OSInit().
*
* Arguments  : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/

void  OSInitHook (void)
{
    OS_CPU_ExceptStkBase = (CPU_STK *)(OSCfg_ISRStkBasePtr + OSCfg_ISRStkSize - 1u);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         STATISTIC TASK HOOK
*
* Description: This function is called every second by uC/OS-III's statistics task.  This allows your
*              application to add functionality to the statistics task.
*
* Arguments  : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/

void  OSStatTaskHook (void)
{
#if OS_CFG_APP_HOOKS_EN > 0u
    if (OS_AppStatTaskHookPtr != (OS_APP_HOOK_VOID)0) {
        (*OS_AppStatTaskHookPtr)();
    }
#endif
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          TASK CREATION HOOK
*
* Description: This function is called when a task is created.
*
* Arguments  : p_tcb        Pointer to the task control block of the task being created.
*
* Note(s)    : None.
*********************************************************************************************************
*/

void  OSTaskCreateHook (OS_TCB  *p_tcb)
{
#if OS_CFG_APP_HOOKS_EN > 0u
    if (OS_AppTaskCreateHookPtr != (OS_APP_HOOK_TCB)0) {
        (*OS_AppTaskCreateHookPtr)(p_tcb);
    }
#else
    (void)p_tcb;                                            /* Prevent compiler warning                               */
#endif
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                           TASK DELETION HOOK
*
* Description: This function is called when a task is deleted.
*
* Arguments  : p_tcb        Pointer to the task control block of the task being deleted.
*
* Note(s)    : None.
*********************************************************************************************************
*/

void  OSTaskDelHook (OS_TCB  *p_tcb)
{
#if OS_CFG_APP_HOOKS_EN > 0u
    if (OS_AppTaskDelHookPtr != (OS_APP_HOOK_TCB)0) {
        (*OS_AppTaskDelHookPtr)(p_tcb);
    }
#else
    (void)p_tcb;                                            /* Prevent compiler warning                               */
#endif
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                            TASK RETURN HOOK
*
* Description: This function is called if a task accidentally returns.  In other words, a task should
*              either be an infinite loop or delete itself when done.
*
* Arguments  : p_tcb        Pointer to the task control block of the task that is returning.
*
* Note(s)    : None.
*********************************************************************************************************
*/

void  OSTaskReturnHook (OS_TCB  *p_tcb)
{
#if OS_CFG_APP_HOOKS_EN > 0u
    if (OS_AppTaskReturnHookPtr != (OS_APP_HOOK_TCB)0) {
        (*OS_AppTaskReturnHookPtr)(p_tcb);
    }
#else
    (void)p_tcb;                                            /* Prevent compiler warning                               */
#endif
}


/*$PAGE*/
/*
**********************************************************************************************************
*                                       INITIALIZE A TASK'S STACK
*
* Description: This function is called by OS_Task_Create() or OSTaskCreateExt() to initialize the stack
*              frame of the task being created. This function is highly processor specific.
*
* Arguments  : p_task       Pointer to the task entry point address.
*
*              p_arg        Pointer to a user supplied data area that will be passed to the task
*                               when the task first executes.
*
*              p_stk_base   Pointer to the base address of the stack.
*
*              stk_size     Size of the stack, in number of CPU_STK elements.
*
*              opt          Options used to alter the behavior of OS_Task_StkInit().
*                            (see OS.H for OS_TASK_OPT_xxx).
*
* Returns    : Always returns the location of the new top-of-stack' once the processor registers have
*              been placed on the stack in the proper order.
*
* Note(s)    : 1) Interrupts are enabled when task starts executing.
*
*              2) All tasks run in Thread mode, using process stack.
**********************************************************************************************************
*/

CPU_STK  *OSTaskStkInit (OS_TASK_PTR    p_task,
                         void          *p_arg,
                         CPU_STK       *p_stk_base,
                         CPU_STK       *p_stk_limit,
                         CPU_STK_SIZE   stk_size,
                         OS_OPT         opt)
{
    CPU_STK  *p_stk;


    (void)opt;                                              /* Prevent compiler warning                               */

    p_stk = &p_stk_base[stk_size];                          /* Load stack pointer                                     */
                                                            /* Registers stacked as if auto-saved on exception        */
    *--p_stk = (CPU_STK)((CPU_INT16U)p_task >> 8);          /* Entry Point                                                   */
    *--p_stk = (CPU_STK)((CPU_INT16U)p_task & 0xFF);        
    *--p_stk = (CPU_STK)0xaa;                        /* A                                               */
    *--p_stk = (CPU_STK)0xbb;                        /* B                                                    */
    *--p_stk = (CPU_STK)0x1b;                        /* VB                                                     */
    *--p_stk = (CPU_STK)0x00;                        /* PSW                                                     */
    *--p_stk = (CPU_STK)0x00;                        /* DPTR                                                     */
    *--p_stk = (CPU_STK)0x00;                        
                                                      /* Remaining registers saved on process stack             */
    *--p_stk = (CPU_STK)0x07u;                        /* R7                                                    */
    *--p_stk = (CPU_STK)0x06u;                        /* R6                                                    */
    *--p_stk = (CPU_STK)0x05u;                        /* R5                                                    */
    *--p_stk = (CPU_STK)0x04u;                        /* R4                                                    */
    *--p_stk = (CPU_STK)0x70u;                        /* V7                                                     */
    *--p_stk = (CPU_STK)0x60u;                        /* V6                                                    */
    *--p_stk = (CPU_STK)0x50u;                        /* V5                                                     */
    *--p_stk = (CPU_STK)0x40u;                        /* V4                                                     */
    *--p_stk = (CPU_STK)0x30u;                        /* V3                                                       */
    *--p_stk = (CPU_STK)0x20u;                        /* V2                                                       */
    *--p_stk = (CPU_STK)0x10u;                        /* V1                                                       */
    *--p_stk = (CPU_STK)0x00u;                        /* V0                                                       */
    *--p_stk = (CPU_STK)((CPU_INT16U)p_arg >> 8);     /* R3                                                         */
    *--p_stk = (CPU_STK)((CPU_INT16U)p_arg & 0xFF);   /* R2                                                         */
    *--p_stk = (CPU_STK)0x01u;                        /* R1                                                         */
    *--p_stk = (CPU_STK)0x00u;                        /* R0                                                         */
    
    
    
    return (p_stk);    
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                           TASK SWITCH HOOK
*
* Description: This function is called when a task switch is performed.  This allows you to perform other
*              operations during a context switch.
*
* Arguments  : None.
*
* Note(s)    : 1) Interrupts are disabled during this call.
*              2) It is assumed that the global pointer 'OSTCBHighRdyPtr' points to the TCB of the task
*                 that will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCurPtr' points
*                 to the task being switched out (i.e. the preempted task).
*********************************************************************************************************
*/

void  OSTaskSwHook (void)
{
#if OS_CFG_TASK_PROFILE_EN > 0u
    CPU_TS  ts;
#endif
#ifdef  CPU_CFG_INT_DIS_MEAS_EN
    CPU_TS  int_dis_time;
#endif



#if OS_CFG_APP_HOOKS_EN > 0u
    if (OS_AppTaskSwHookPtr != (OS_APP_HOOK_VOID)0) {
        (*OS_AppTaskSwHookPtr)();
    }
#endif

#if OS_CFG_TASK_PROFILE_EN > 0u
    ts = OS_TS_GET();
    if (OSTCBCurPtr != OSTCBHighRdyPtr) {
        OSTCBCurPtr->CyclesDelta  = ts - OSTCBCurPtr->CyclesStart;
        OSTCBCurPtr->CyclesTotal += (OS_CYCLES)OSTCBCurPtr->CyclesDelta;
    }

    OSTCBHighRdyPtr->CyclesStart = ts;
#endif

#ifdef  CPU_CFG_INT_DIS_MEAS_EN
    int_dis_time = CPU_IntDisMeasMaxCurReset();             /* Keep track of per-task interrupt disable time          */
    if (OSTCBCurPtr->IntDisTimeMax < int_dis_time) {
        OSTCBCurPtr->IntDisTimeMax = int_dis_time;
    }
#endif

#if OS_CFG_SCHED_LOCK_TIME_MEAS_EN > 0u
                                                            /* Keep track of per-task scheduler lock time             */
    if (OSTCBCurPtr->SchedLockTimeMax < OSSchedLockTimeMaxCur) {
        OSTCBCurPtr->SchedLockTimeMax = OSSchedLockTimeMaxCur;
    }
    OSSchedLockTimeMaxCur = (CPU_TS)0;                      /* Reset the per-task value                               */
#endif
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                              TICK HOOK
*
* Description: This function is called every tick.
*
* Arguments  : None.
*
* Note(s)    : 1) This function is assumed to be called from the Tick ISR.
*********************************************************************************************************
*/

void  OSTimeTickHook (void)
{
#if OS_CFG_APP_HOOKS_EN > 0u
    if (OS_AppTimeTickHookPtr != (OS_APP_HOOK_VOID)0) {
        (*OS_AppTimeTickHookPtr)();
    }
#endif
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          SYS TICK HANDLER
*
* Description: Handle the system tick (SysTick) interrupt, which is used to generate the uC/OS-II tick
*              interrupt.
*
* Arguments  : None.
*
* Note(s)    : 1) This function MUST be placed on entry 15 of the Cortex-M3 vector table.
*********************************************************************************************************
*/

void  OS_CPU_SysTickHandler (void)
{
    P1_1 = ~P1_1;
#if 1
    CPU_SR_ALLOC();


    CPU_CRITICAL_ENTER();
    OSIntNestingCtr++;                                      /* Tell uC/OS-III that we are starting an ISR             */
    CPU_CRITICAL_EXIT();
    
    OSTimeTick();                                           /* Call uC/OS-III's OSTimeTick()                          */

    OSIntExit();                                            /* Tell uC/OS-III that we are leaving the ISR             */
#endif  
    //P1_0 = 0;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         INITIALIZE SYS TICK
*
* Description: Initialize the SysTick.
*
* Arguments  : cnts         Number of SysTick counts between two OS tick interrupts.
*
* Note(s)    : 1) This function MUST be called after OSStart() & after processor initialization.
*********************************************************************************************************
*/
void  OS_CPU_SysTickInit (CPU_INT32U  cnts)
{   
   CPU_INT32U sleepTimer = 0; 
   sleepTimer |= ST0; 
   sleepTimer |= (CPU_INT32U)ST1 <<  8; 
   sleepTimer |= (CPU_INT32U)ST2 << 16; 
   sleepTimer += cnts; 
   ST2 = (CPU_INT08U)(sleepTimer >> 16); 
   ST1 = (CPU_INT08U)(sleepTimer >> 8); 
   ST0 = (CPU_INT08U) sleepTimer; 
   
    STIE = 1;   //SleepTimerinterrupt enable
    STIF = 0;   //SleepTimerinterrupt flag 还没处理的
}

