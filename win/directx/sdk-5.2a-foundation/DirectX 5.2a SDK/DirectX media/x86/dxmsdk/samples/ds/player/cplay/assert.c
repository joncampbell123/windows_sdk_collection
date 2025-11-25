//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1997  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;

#include "stdwin.h"

//
// DbgAssert
//
// Displays a message box if the condition evaluated to FALSE
//
void DbgAssert(const char *pCondition, const char *pFileName, int iLine)
{
    int MsgId;
    char szInfo[1024];

    wsprintf(szInfo, TEXT("%s \nAt line %d of %s"),pCondition, iLine, pFileName);
    MsgId = MessageBox(NULL, szInfo, TEXT("ASSERT Failed"),
                           MB_SYSTEMMODAL |
                           MB_ICONHAND |
                           MB_ABORTRETRYIGNORE);
    switch (MsgId)
    {
        case IDABORT:           // Kill the application

            FatalAppExit(FALSE, TEXT("Application terminated"));
            break;

        case IDRETRY:           // Break into the debugger
            DebugBreak();
            break;

        case IDIGNORE:          // Ignore assertion continue executing
            break;
    }
}

