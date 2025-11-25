/*===========================================================================*\
|
|  File:        cgtimer.cpp
|
|  Description: 
|       
|-----------------------------------------------------------------------------
|
|  Copyright (C) 1995-1996 Microsoft Corporation.  All Rights Reserved.
|
|  Written by Moss Bay Engineering, Inc. under contract to Microsoft Corporation
|
\*===========================================================================*/

/**************************************************************************

    (C) Copyright 1995-1996 Microsoft Corp.  All rights reserved.

    You have a royalty-free right to use, modify, reproduce and 
    distribute the Sample Files (and/or any modified version) in 
    any way you find useful, provided that you agree that 
    Microsoft has no warranty obligations or liability for any 
    Sample Application Files which are modified. 

    we do not recomend you base your game on IKlowns, start with one of
    the other simpler sample apps in the GDK

 **************************************************************************/

#include <windows.h>
#include "CGTimer.h"

static void TimerThread(void);
extern CGameTimer * Timer;
static CRITICAL_SECTION cs;

CGameTimer::CGameTimer(void)
{
#if 0
    DWORD id;
    // initialize our critical section dude
    InitializeCriticalSection(&cs);

    EnterCriticalSection(&cs);
    // create our Event item
    Event = CreateEvent (
        NULL,
        FALSE,  // auto-reset
        FALSE,  // initially not-signalled
        NULL
        );

    // create our timer thread
    Time = 0;   // start at time-slice zero
    SetRate(60);    // sixty Hertz
    
    thread = CreateThread(
        NULL,
            0,
        (LPTHREAD_START_ROUTINE) TimerThread,
        NULL,
        0,
        &id
        );  
//  SetThreadPriority( thread, THREAD_PRIORITY_HIGHEST );
    LeaveCriticalSection(&cs);
#endif
}

CGameTimer::~CGameTimer(void)
{
#if 0
    // kill the thread
    TerminateThread(thread,0);
    // remove the Event
    CloseHandle(Event);
    // and the CS
    DeleteCriticalSection(&cs);
#endif
}

static void TimerThread(void)
{
    long mylasttime = timeGetTime();
    int x;

    while (1)
    {
        // wait till it's time to wake up

        x = timeGetTime() - mylasttime;
        while (x < Timer->rate)
        {
            Sleep(0);
            x = timeGetTime() - mylasttime;
        }
        mylasttime = timeGetTime();

        // increment our notion of time
        EnterCriticalSection(&cs);
        if (!Timer->paused)
        {
            ++Timer->Time;
            SetEvent(Timer->Event);                     
        }   // let the other thread go now...

        LeaveCriticalSection(&cs);
    }
}

void    CGameTimer::Pause(void) 
{
//  EnterCriticalSection(&cs);
    paused = TRUE;
//  LeaveCriticalSection(&cs);
};
    
void    CGameTimer::Resume(void) 
{
//  EnterCriticalSection(&cs);
    paused = FALSE;
//  LeaveCriticalSection(&cs);
};

void CGameTimer::SetRate ( int hertz )
{
    if (hertz)
        rate = 1000 / hertz;
}

int CGameTimer::GetRate (void)
{
    if (rate)
    {
        return(1000 / rate);
    }
    else
    {
        return(0);      
    }   
}
