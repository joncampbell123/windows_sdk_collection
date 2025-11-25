/*===========================================================================*\
|
|  File:        cginput.cpp
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
#include <mmsystem.h>
#ifdef __WATCOMC__
#include <mem.h>
#else
#include <memory.h>
#endif

#include "cginput.h"

CGameInput::CGameInput (void)
{
    JOYINFO joypos;
    memset(&caps, 0, sizeof(JOYCAPS) * 16);
    memset(&joyThere, 0, sizeof(int) *  16);

    for (int x=0; x<QJoystick(); x++)
    {
        if (joyGetPos(JOYSTICKID1+x, &joypos) == JOYERR_NOERROR)
        {
            // there *is* a joystick 'x' installed!
            ++joyThere[x];
            joyGetDevCaps(JOYSTICKID1+x, &(caps[x]), sizeof(JOYCAPS));
        }
    }
    Flush();
    UpdateJoystick();
}

CGameInput::~CGameInput(void)
{
}

void CGameInput::Flush(void)
{
    // Do things this way because, ahem... SetKeyboardState() doesn't appear
    // to work on Win'95...
    for (int x=0; x<256; x++)
    {
        GetKeyboard(x);
    }
}


//--------------------------------------------------

int CGameInput::QKeyboard(void)
{
    // if there is a keyboard, returns 1 else 0
    return(GetKeyboardType(1));
}


int CGameInput::QMouse(void)
{
    // if no mouse, 0 else number of buttons on mouse
    return(GetSystemMetrics(SM_CMOUSEBUTTONS)); 
}


int CGameInput::QJoystick(void)
{
    // if no joystick(s), returns 0 else number of joysticks attached.
    return(joyGetNumDevs());    
}

int CGameInput::GetKeyboard(int key)
{
    // returns 0 if the key has been depressed, else returns 1 and sets key to code recd.
    return (GetAsyncKeyState(key));
}

int CGameInput::QKeyDepressed(int numkeys, int *keyarray)
{
    int x;

    // tells if keys in keyarray are currently depressed.  Returns 0 if not, 1 if all
    if (!numkeys || !keyarray) 
        return(0);

    for (x=0; x<numkeys ; x++)
    {
        // mask off top bit
        if ((GetAsyncKeyState(keyarray[x])) == 0)
            return(0);          
    }
    return(1);  
}

int CGameInput::GetMouse(int& xpos, int&ypos, int& buttons)
{
    // returns 0 if no mouse action to report; else, 1 and fills in params
    int button1, button2;
    POINT pt;

    if (!GetCursorPos(&pt))
        return(0);
    
    xpos = pt.x;
    ypos = pt.y;
    buttons = 0;

    button1 = GetAsyncKeyState(VK_LBUTTON);
    button2 = GetAsyncKeyState(VK_RBUTTON);
    if (button1)
        buttons |= 1;
    if (button2)
        buttons |= 2;

    return(1);  
}

int normalize (int val, int minval, int maxval)
{
    // error detection:
    if ((maxval-minval) == 0)
    {
        return(0);      
    }

    // zero-base:
    val -= minval;

    // normalize to 0..200:
    val = (200L * val) / (maxval-minval);

    // shift to -100 .. 100:
    val -= 100;

    return(val);    
}

void CGameInput::UpdateJoystick(void)
{
    for (int x=0; x<16; x++)
    {
        if (joyThere[x])
            joyGetPos(JOYSTICKID1 + x, &(cached_joyinfo[x]));
    }
}

int CGameInput::GetJoystick(int joynum, JOYINFO * joypos)
{
    if ((joynum >= 16) || (joynum <= 0) || !joyThere[joynum-1])
        return(0);      

    memcpy(joypos, &(cached_joyinfo[joynum-1]), sizeof(JOYINFO));

    // normalize the joypos to -100,0,100 scale....
    joypos->wXpos = normalize(joypos->wXpos, caps[joynum-1].wXmin, caps[joynum-1].wXmax);
    joypos->wYpos = normalize(joypos->wYpos, caps[joynum-1].wYmin, caps[joynum-1].wYmax);
    joypos->wZpos = normalize(joypos->wZpos, caps[joynum-1].wZmin, caps[joynum-1].wZmax);

    return(1);  
}
