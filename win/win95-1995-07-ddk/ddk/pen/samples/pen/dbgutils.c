/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

/* DBGUTILS.c: 

	This file contains all debugging code for the pen driver.

*/

#ifdef DEBUG
#include <windows.h>
#define PENDRIVER      // Include only pen driver code from penwin.h
#define NOPENAPPS
#include <penwin.h>

#include "PenDrv.h"

// List of all virtual driver messages that are supported.
MSGSL VDriverMessages[]=
{
        {VPEND_LOAD,                    "Pen:VPEND_LOAD"},
        {VPEND_ENABLE,                  "Pen:VPEND_ENABLE"},
        {VPEND_DISABLE,                 "Pen:VPEND_DISABLE"},
        {VPEND_LOAD,                    "Pen:VPEND_LOAD"},
        {VPEND_FREE,                    "Pen:VPEND_FREE"},
        {VPEND_SETSAMPLINGRATE,         "Pen:VPEND_SETSAMPLINGRATE"},
        {VPEND_GETSAMPLINGRATE,         "Pen:VPEND_GETSAMPLINGRATE"},
        {VPEND_SETSAMPLINGDIST,         "Pen:VPEND_SETSAMPLINGDIST"},
        {VPEND_GETSAMPLINGDIST,         "Pen:VPEND_GETSAMPLINGDIST"},
        {VPEND_GETCALIBRATION,          "Pen:VPEND_GETCALIBRATION"},
        {VPEND_SETCALIBRATION,          "Pen:VPEND_SETCALIBRATION"},
        {VPEND_PENPLAYSTART,            "Pen:VPEND_PENPLAYSTART"},
        {VPEND_PENPLAYSTOP,             "Pen:VPEND_PENPLAYSTOP"},
        {VPEND_PENPLAYBACK,             "Pen:VPEND_PENPLAYBACK"},
        {VPEND_GETPENINFO,              "Pen:VPEND_GETPENINFO"},
        {VPEND_OEM,                     "Pen:VPEND_OEM"},
        {0,                             NULL}
} ;


// format strings for different types of messages
LPSTR lpFormatParam="%s %x:%x, %x:%x";
LPSTR lpFormatRet="%s returning %x:%x";
LPSTR lpFormatMisc="%s %s";
LPSTR lpFormatWarn="%s %x:%x, %x:%x : %s";
LPSTR lpFormatVdrvC="%s Calling Virtual Pen Driver : %s";
LPSTR lpFormatVdrvR="%s Back From Virtual Pen Driver : %s";

// main type of flag for debug output (see DebugOutput and Windows.h)
WORD gwFlags=DBF_DRIVER;

//*****************************************************************************
//
// Function: DebugInfo
//
//	Comment:	This function is used only if the driver is built for debugging.
//		It uses DebugOutput to display debug strings that can be viewed
//		in either DBWin.exe or off of the com port.	DBWin should be used
//		to set the viewing options.  This routine provides output for
//		all four types of debug messages:
//
//		Traces:		miscellaneous information and code paths
//		Warnings:	bogus information that shouldn't be there
//		Errors:		bad parameters, asserts
//		Fatal Errors:   very bad problems
//
//			This routine is always called through the debug macros DBG_<macro>.
//
//	Params:
//		wType:		A series of flags that describe debug level and format to
//				use.  (See the Debug Macros for cleaner description.)
//		lpMsg:		Pointer to main driver message (aka DRV_LOAD).
//		lParam1:	Data to be displayed in hex format.
//		lParam2:	Data to be displayed in hex format.
//		lpStr:		Miscellaneous string.  Use wsprintf to fill buffer for random data.
//		wVMsg:		Message value of virtual pen driver messages.
//
//	Return: void
//
//*****************************************************************************
void FAR PASCAL DebugInfo(WORD wType,
						  LPSTR lpMsg,
						  LPARAM lParam1,LPARAM lParam2,
						  LPSTR lpStr,
						  DWORD dwVMsg)
{
	//The debug flags and some special case flags.
	WORD wMainType=(wType & DBF_ALL);
	WORD wSubType=(wType & DBG_ALL);
	WORD wFinalFlags=gwFlags | wMainType;

	LPSTR lpVMsg;

	//If calling the virtual pen driver, look up the message string.
	if(dwVMsg)
	{
		LPMSGSL lpVDrvrMsgs=VDriverMessages;

		dwVMsg&=(~(VPEND_OEM | VpenD_lParam1_ptr | VpenD_lParam2_ptr));

		while(lpVDrvrMsgs->lpStr!=NULL)
		{
			if(lpVDrvrMsgs->dwMsg==dwVMsg)
				break;
			else
				lpVDrvrMsgs++;
		}
		// for new debug macro
		lpVMsg=lpVDrvrMsgs->lpStr;
		if(lpVMsg==NULL)
			lpVMsg="Unknown message!";
	}

	switch(wMainType)
	{
		case DBF_TRACE:
			{
			switch(wSubType)
				{
				// Output will look like:
				//
				// t PEN <DRV_Message> <HIWORD(lParam1):LOWORD(LParam1)>, <HIWORD(lParam2):LOWORD(LParam2)>
				case DBG_PARAM:
					DebugOutput(wFinalFlags,
								(LPSTR)lpFormatParam,
								(LPSTR)lpMsg,
								HIWORD(lParam1),LOWORD(lParam1),
								HIWORD(lParam2),LOWORD(lParam2));
					break;

				// Output will look like:
				//
				// t PEN <DRV_Message> returning <HIWORD(lParam1):LOWORD(lParam1)
				case DBG_RETURN:
					DebugOutput(wFinalFlags,
								(LPSTR)lpFormatRet,
								(LPSTR)lpMsg,
								HIWORD(lParam1),LOWORD(lParam1));
					break;

				// Output will look like:
				//
				// t PEN <DRV_Message> <string>
				case DBG_MISC:
					DebugOutput(wFinalFlags,(LPSTR)lpFormatMisc,(LPSTR)lpMsg,(LPSTR)lpStr);
					break;

				// Output will look like:
				//
				// t PEN <DRV_Message> Calling Virtual Pen Driver : <VpenD_Msg>
				case DBG_VDRVC:
					DebugOutput(wFinalFlags,(LPSTR)lpFormatVdrvC,(LPSTR)lpMsg,(LPSTR)lpVMsg);
					break;

				// Output will look like:
				//
				// t PEN <DRV_Message> Back From Virtual Pen Driver : <VpenD_Msg>
				case DBG_VDRVR:
					DebugOutput(wFinalFlags,(LPSTR)lpFormatVdrvR,(LPSTR)lpMsg,(LPSTR)lpVMsg);
					break;
				}
			}
			break;

		// Output will look like the following for the next three types:
		//
		// t PEN <DRV_Message> <HIWORD(lParam1):LOWORD(LParam1)>, <HIWORD(lParam2):LOWORD(LParam2)>	<string>
		case DBF_WARNING:
			{
			switch(wSubType)
				{
				case DBG_WARN_NULLS:
					if( (lParam1!=NULL) || (lParam2!=NULL) )
						DebugOutput(wFinalFlags,
									(LPSTR)lpFormatWarn,
									(LPSTR)lpMsg,
									HIWORD(lParam1),LOWORD(lParam1),
									HIWORD(lParam2),LOWORD(lParam2),
									(LPSTR)lpStr);
					break;
				default:
					DebugOutput(wFinalFlags,
								(LPSTR)lpFormatWarn,
								(LPSTR)lpMsg,
								HIWORD(lParam1),LOWORD(lParam1),
								HIWORD(lParam2),LOWORD(lParam2),
								(LPSTR)lpStr);
					break;
				}
			}
			break;
		// Output will look like the following for the next three types:
		//
		// t PEN <DRV_Message> <HIWORD(lParam1):LOWORD(LParam1)>, <HIWORD(lParam2):LOWORD(LParam2)>	<string>
		case DBF_ERROR:
		case DBF_FATAL:
			{
				DebugOutput(wFinalFlags,
							(LPSTR)lpFormatWarn,
							(LPSTR)lpMsg,
							HIWORD(lParam1),LOWORD(lParam1),
							HIWORD(lParam2),LOWORD(lParam2),
							(LPSTR)lpStr);
			}
			break;
		default:
			break;
	}
}


#endif

//End-Of-File
