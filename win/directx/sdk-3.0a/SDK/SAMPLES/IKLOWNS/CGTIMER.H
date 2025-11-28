/*===========================================================================*\
|
|  File:        cgtimer.h
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

class CGameTimer {

public:
    volatile int paused;
    int rate;
    HANDLE  thread;
    HANDLE  Event;
    volatile int    Time;

    CGameTimer (void);
    ~CGameTimer (void);

    void    SetRate(int hertz);
    int     GetRate(void);
    void    Pause(void);
    void    Resume(void);
};
