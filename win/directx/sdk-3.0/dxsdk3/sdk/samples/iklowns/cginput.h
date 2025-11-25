/*===========================================================================*\
|
|  File:        cginput.h
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

/*
    The CGameInput class serves as an abstraction layer on top of the
    various input devices which may be attached to the machine.
*/

#define MOUSE_BUTTON_1  1
#define MOUSE_BUTTON_2  2

class CGameInput {
private:
    int joyThere[16]; // 1  if joystick is plugged in, 0 otherwise
    JOYCAPS caps [16];
    JOYINFO cached_joyinfo[16];

public:
    /*
        Constructor/destructor.  Allocate any threads or other resources
        required by the object:
    */
    CGameInput(void);
    ~CGameInput(void);

    void Flush(void);   // remove input (clean up...)

    /*
        HW Query functions: determine whether or not a device exists.
        Return 0 if no such device, > 0 (depending on device) if it does.
    */
    int QKeyboard(void);    // 0= no keyboard, 1= exists
    int QMouse(void);       // 0=no, otherwise num buttons on mouse
    int QJoystick(void);    // 0=none, else number of joysticks

    /*
        Input functions: anyone wishing for input needs to call these:
    */
    int GetKeyboard(int yourkey);   // ret: 0=no key available
    //  pass number of keys to check, then array of VKEY codes for keys 
    //  to check.  If any key is *not* depressed, returns 0; else 1
    int QKeyDepressed(int numkeys, int *keyarray);
    //  buttons is bitmap of MOUSE_... returns 0 if no mouse input avail
    int GetMouse(int &xpos, int& ypos, int& buttons);
    //  joystick needs number of joystick to query
    int GetJoystick(int joynum, JOYINFO *joypos);
    void UpdateJoystick(void);
};
