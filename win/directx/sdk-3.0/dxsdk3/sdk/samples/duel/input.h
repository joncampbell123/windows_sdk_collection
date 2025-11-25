/*==========================================================================
 *
 *  Copyright (C) 1995-1996 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       input.h
 *  Content:    input routines include file
 *
 *
 ***************************************************************************/

#ifndef _INPUT_H
#define _INPUT_H

#include <dinput.h>
#include "duel.h"

BOOL            InitInput(void);
BOOL CALLBACK   DI_EnumDevProc(LPDIDEVICEINSTANCE lpdidi, LPVOID lpv);
void            DI_ReadKeys(void);
void            CleanupInput(void);
BOOL                    ReacquireInputDevices(void);


#endif // _INPUT_H



