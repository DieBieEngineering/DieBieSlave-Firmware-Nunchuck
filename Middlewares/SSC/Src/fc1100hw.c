/**
\addtogroup FC1100HW FC1100 Hardware Access
@{
*/

/**
\file    fc1100hw.c
\author EthercatSSC@beckhoff.com
\brief Implementation
Hardware access implementation for FC1100 hardware access on Win32 platforms

\version 5.10

<br>Changes to version V5.0:<br>
V5.01 HW1: Invalid ESC access function was used<br>
<br>Changes to version V4.42:<br>
V5.0 ESC4: Save SM disable/Enable. Operation may be pending due to frame handling.
*/

/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#ifdef _WIN32
#include <stdio.h>
#include <tchar.h>
#include "TcHelper.h"
#endif

#include "ecat_def.h"
#if FC1100_HW

#define _FC1100_
#include "fc1100hw.h"
#undef _FC1100_
#include "ecatappl.h"

/*-----------------------------------------------------------------------------------------
------
------    Local Variables
------
-----------------------------------------------------------------------------------------*/

UINT16 * pFc1100Device;
HANDLE gDoneEvent;
HANDLE hTimerQueue;

TSYNCMAN    TmpSyncMan;

UINT16 TimerCnt;

/*-----------------------------------------------------------------------------------------
------
------    Local Functions
------
-----------------------------------------------------------------------------------------*/
BOOL InitSystemEvents(void);
void FreeTimerResources(void);


UINT16* GetFc1100Pointer(void)
{
    DWORD nError = TcHelperInstall();

    if (nError != 0)
    {
#ifdef _DEBUG
        DebugMessage("Failed to install mapping driver:  0x%X.\r\n", nError);
#endif
        return 0;
    }
    //get a handle on the first FC11xx fieldbus card installed
    pFc1100Device = (UINT16*)MapESCPointer(1);
#ifdef _DEBUG
    DebugMessage("Device pointer:  0x%X.\r\n", pFc1100Device);
    if(pFc1100Device == 0)
    {
        DWORD err = GetLastError();
        DebugMessage("Error:  0x%X.\r\n", err);
    }
#endif



    return pFc1100Device;
}

void FreeFc1100Handle(void)
{
    UnmapESCPointer();
    pFc1100Device = NULL;
    TcHelperRemove();
}

BOOL WINAPI ConsoleHandler(DWORD CEvent)
{
    switch(CEvent)
    {
    case CTRL_CLOSE_EVENT:
            bRunApplication = FALSE;
        break;
    }
    return TRUE;
}

VOID CALLBACK TimerRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{

#if ECAT_TIMER_INT
    ECAT_CheckTimer();
#endif

    TimerCnt++;

    SetEvent(gDoneEvent);
}

BOOL InitSystemEvents(void)
{
    BOOL Result =  FALSE;
    HANDLE hTimer = NULL;
    hTimerQueue = NULL;

    // Use an event object to track the TimerRoutine execution
    gDoneEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (NULL != gDoneEvent)
    {
        // Create the timer queue.
        hTimerQueue = CreateTimerQueue();
        if (NULL != hTimerQueue)
        {
            // Set a timer to call the timer routine in 10 seconds.
            if (CreateTimerQueueTimer( &hTimer, hTimerQueue, (WAITORTIMERCALLBACK)TimerRoutine, NULL , 1, 1, 0))
            {
                Result =  TRUE;
            }
        }
    }

    TimerCnt = 0;

    if(Result == TRUE)
        Result = (BOOL) SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE);


    return Result;
}

void FreeTimerResources(void)
{
    CloseHandle(gDoneEvent);

    if(hTimerQueue != NULL)
    {
        // Delete all timers in the timer queue.
        if (!DeleteTimerQueue(hTimerQueue))
        {
#ifdef _DEBUG
            DebugMessage("DeleteTimerQueue failed (%X)\n", GetLastError());
#endif
        }
    }

}

/*-----------------------------------------------------------------------------------------
------
------    Global Functions
------
-----------------------------------------------------------------------------------------*/

#ifdef _DEBUG
/////////////////////////////////////////////////////////////////////////////////////////
/**
\brief    Writes messages to the debugger output
*////////////////////////////////////////////////////////////////////////////////////////
void DebugMessage(const char* pszFormat, ...)
{
    va_list args;
    va_start(args, pszFormat);

    int nBuf;
    char szBuffer[512];

    nBuf = _vsnprintf(szBuffer, sizeof(szBuffer), pszFormat, args);

    OutputDebugString(szBuffer);
    va_end(args);
}
#endif 

/////////////////////////////////////////////////////////////////////////////////////////
/**
\return     0 if initialization was successful

 \brief    This function initialize the EtherCAT Slave Interface.
*////////////////////////////////////////////////////////////////////////////////////////

UINT16 HW_Init(void)
{
    /* the memory interface to the ESC, the ESC-interrupt and the ECAT-timer for the
       watchdog monitoring should be initialized here microcontroller specific*/

    pEsc = (MEM_ADDR ESCMEM *) GetFc1100Pointer();
    if(pEsc == NULL)
    {
        return 1;
    }
    if (InitSystemEvents() == FALSE)
    {
        return 1;
    }

    /* initialize the AL_Event Mask Register */
#if ESC_32BIT_ACCESS
    {
    UINT32 ResetValue = 0;
    HW_EscWriteDWord(ResetValue, ESC_AL_EVENTMASK_OFFSET);
    }
#else
    {
    UINT16 ResetValue = 0;
    HW_EscWriteWord(ResetValue, ESC_AL_EVENTMASK_OFFSET);
    }
#endif 

    return 0;
}

void HW_Release(void)
{
    FreeFc1100Handle();

    FreeTimerResources();
}

#if BOOTSTRAPMODE_SUPPORTED
/////////////////////////////////////////////////////////////////////////////////////////
/**

 \brief    This function resets the hardware
*////////////////////////////////////////////////////////////////////////////////////////

void     HW_RestartTarget(void)
{
}
#endif /* BOOTSTRAPMODE_SUPPORTED */

#if ESC_EEPROM_EMULATION
/////////////////////////////////////////////////////////////////////////////////////////
/**
\return     0 if reload was successful

 \brief    This function is called when the master has request an EEPROM reload during EEPROM emulation

*////////////////////////////////////////////////////////////////////////////////////////
UINT16 HW_EepromReload (void)
{
    return 0;
}
#endif



UINT16 HW_GetTimer(void)
{
    return TimerCnt;
}

void HW_ClearTimer(void)
{
    TimerCnt = 0;
}


#if UC_SET_ECAT_LED
void HW_SetLed(BOOL RunLed,BOOL ErrLed)
{

}
#endif //#if UC_SET_ECAT_LED

#endif //#if FC1100_HW

/** @} */

