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

/* DRVRPROC.c: 

	Installable driver entry point. Processes messages for pen driver.

	Routines contained in module:
	  DriverProc
	  CallVirtualPenDriver
	  CallPenUIDLL
	  RecordTabletEvent
*/

#include <windows.h>
#define PENDRIVER    //Include only pen driver code from penwin.h.
#define NOPENAPPS
#include <penwin.h>
#include "PenDrv.h"

// variables that can be touched at interrupt time
extern DRV_PENINFO vpiPenInfo;
extern DRV_PENPACKET PenPacketBuffer[MAX_BUFFER_SIZE];

// This buffer should be adjusted if the OEM will be using it.
extern char szOEMBuffer[];
extern DWORD vdwDataType;
extern DWORD vdwNumberOfPenPackets;
extern DWORD vdwOffsetIntoBuffer;
extern DWORD vdwIndexIntoBuffer;

extern int viActingLikeMouse;

extern UINT viNumberOfHookRoutines;
extern HOOKROUTINE vrglpfnProcessTabletEvents[];


// non-interrupt time variables
VpenD_Func vlpfnVpenD_API_Entry_Point; // pen driver variable

DRV_CALBSTRUCT gcbCalibrate;

// The dwPlayFlags variable tells whether the driver should play
// back 1.0 or 2.0 pen pack data.
DWORD dwPlayFlags=PLAY_VERSION_10_DATA;

// file global variables
#define ERROR_DURING_ENABLE  0x0001
#define IN_SETUP_MODE        0x0002

WORD wMiscFlags=0;

const char szDriverName[]="Microsoft Pen Driver 2.00";
const char szPenDLL[]="penui.dll";
const char szConfigDialog[]="ConfigDialog";

#ifdef DEBUG
MSGS DriverMessages[]=
{
        {DRV_LOAD,               "PEN:DRV_LOAD"},
        {DRV_FREE,               "PEN:DRV_FREE"},
        {DRV_ENABLE,             "PEN:DRV_ENABLE"},
        {DRV_DISABLE,            "PEN:DRV_DISABLE"},
        {DRV_INSTALL,            "PEN:DRV_INSTALL"},
        {DRV_REMOVE,             "PEN:DRV_REMOVE"},
        {DRV_EXITSESSION,        "PEN:DRV_EXITSESSION"},
        {DRV_EXITAPPLICATION,    "PEN:DRV_EXITAPPLICATION"},
        {DRV_POWER,              "PEN:DRV_POWER"},
        {DRV_SetEntryPoints,     "PEN:DRV_SetEntryPoints"},
        {DRV_RemoveEntryPoints,  "PEN:DRV_RemoveEntryPoints"},
        {DRV_SetPenSamplingRate, "PEN:DRV_SetPenSamplingRate"},
        {DRV_SetPenSamplingDist, "PEN:DRV_SetPenSamplingDist"},
        {DRV_GetCalibration,     "PEN:DRV_GetCalibration"},
        {DRV_SetCalibration,     "PEN:DRV_SetCalibration"},
        {DRV_PenPlayStart,       "PEN:DRV_PenPlayStart"},
        {DRV_PenPlayBack,        "PEN:DRV_PenPlayBack"},
        {DRV_PenPlayStop,        "PEN:DRV_PenPlayStop"},
        {DRV_GetVersion,         "PEN:DRV_GetVersion"},
        {DRV_GetPenInfo,         "PEN:DRV_GetPenInfo"},
        {DRV_OPEN,               "PEN:DRV_OPEN"},
        {DRV_CLOSE,              "PEN:DRV_CLOSE"},
        {DRV_Query,              "PEN:DRV_Query"},
        {DRV_QUERYCONFIGURE,     "PEN:DRV_QUERYCONFIGURE"},
        {DRV_CONFIGURE,          "PEN:DRV_CONFIGURE"},
        {DRV_GetName,            "PEN:DRV_GetName"},
        {0,NULL}
};

LPSTR glpMsg;
LPSTR glplParam1Null="Expected lParam1 to be NULL!";
LPSTR glplParam2Null="Expected lParam2 to be NULL!";
LPSTR glplParamBoth="Expected both lParam's to be NULL!";
#endif

LRESULT CALLBACK _export _loadds DriverProc(DWORD dwDrvID, HDRVR hDriver,
				UINT wMsg, LPARAM lParam1, LPARAM lParam2)
{
   //
   // Assume every message is successfully serviced.
   //
	DWORD dwRet=DRV_SUCCESS;

#ifdef DEBUG

	LPMSGS lpDrvrMsgs=DriverMessages;
	char szDbg[180];

	while(lpDrvrMsgs->lpStr!=NULL)
		{
	   if(lpDrvrMsgs->wMsg==wMsg)
			break;
		else
			lpDrvrMsgs++;
      }

	// for new debug macro
	if(lpDrvrMsgs->lpStr!=NULL)
		glpMsg=lpDrvrMsgs->lpStr;
	else
		glpMsg="Unknown Driver Message";

#endif

	switch(wMsg)
	{
		case DRV_LOAD:
		{
			VPEND_INIT VpenD_Init_Struct;

			DBG_TRACE_PARAM(lParam1,lParam2);
			DBG_WARNING_NULLS(lParam1,lParam2,glplParamBoth);

			// Set any necessary global variables.
			viActingLikeMouse=-1;

#ifdef DEBUG
            DBG_TRACE_MISC((LPSTR)"Calling multiplex interrupt");
#endif

			_asm {
				xor	di, di
				mov	ax, GET_VPEND_API_ENTRY_POINT		; 1684h
				mov	bx, VPenD_Device_ID	;for the Virtual Pen Driver
				int	MULTIPLEX_INTERRUPT	; ENHANCED_WINDOWS_INT			; 2fh

				mov	word ptr vlpfnVpenD_API_Entry_Point+2,es;
				mov	word ptr vlpfnVpenD_API_Entry_Point,di
				}
#ifdef DEBUG
			wsprintf((LPSTR)szDbg,"Virtual Pen Driver entry point: %x:%x",
					 HIWORD((DWORD)vlpfnVpenD_API_Entry_Point),
					 LOWORD((DWORD)vlpfnVpenD_API_Entry_Point));
			DBG_TRACE_MISC((LPSTR)szDbg);
#endif

			// Check registry for VxD before calling for it.

			if(vlpfnVpenD_API_Entry_Point==NULL)
			{
			    DBG_ERROR(0L,0L,(LPSTR)"Virtual Pen Driver not present - very bad!");
				return DRV_FAILURE;
			}

			// need to be able to call this code dynamically...

			// This next block of code tells VpenD that
			// there is a pen driver present and gives it some
			// information.
			VpenD_Init_Struct.lpDataType=(LPDWORD)&vdwDataType;
			VpenD_Init_Struct.lpPenInfo=(LPDRV_PENINFO)&vpiPenInfo;
			VpenD_Init_Struct.lpfnEntryPoint=(LPDWORD)(FARPROC)Deal_With_Pen_Packets;
			VpenD_Init_Struct.lpOEMBuffer=(LPSTR)&szOEMBuffer;
			VpenD_Init_Struct.lpPenPacketBuffer=(LPDRV_PENPACKET)&PenPacketBuffer;
			VpenD_Init_Struct.lpNumberOfPenPackets=(LPDWORD)&vdwNumberOfPenPackets;
			VpenD_Init_Struct.lpOffsetIntoBuffer=(LPDWORD)&vdwOffsetIntoBuffer;
			VpenD_Init_Struct.lpIndexIntoBuffer=(LPDWORD)&vdwIndexIntoBuffer;

/*
    If there was a problem at load time with booting the virtual pen driver,
    then a positive number other then 1 is returned.  So, need to look for
    0, 1 or a postive number here.

    If 0 then error (no virtual pen driver around)
    else if 1 then everything is fine
    else a positive number so need to run in setup mode.
*/

			{
			long lRet;

			// need to check return value and gracefully fail if necessary
			if( (lRet=CallVirtualPenDriver(VPEND_LOAD | VpenD_lParam1_ptr,
				(LPARAM)(LPVPEND_INIT)&VpenD_Init_Struct,
				(LPARAM)(DWORD)NULL)) == 0 )
				{
				DBG_WARNING(0L,0L,"Virtual Pen Driver Failed to Load");
				dwRet=(LRESULT)DRV_FAILURE;
			   }
			else
				{
				if(lRet!=1)
					{
					// need setup mode
					wMiscFlags|=IN_SETUP_MODE;
					return (LRESULT)DRV_SUCCESS;
					}
			   }
			}

#ifdef DEBUG
			wsprintf((LPSTR)szDbg,
"PENINFO: pi.cxRW=%li, pi.cyRH=%li, pi.wDW=%li, pi.wDH=%li, \
pi.nSR=%li, pi.nSD=%li, pi.cPens=%lu, pi.fuOEM=%lx \
pi.SRMin=%li, pi.SRMax=%li",
					vpiPenInfo.cxRawWidth,
					vpiPenInfo.cyRawHeight,
					vpiPenInfo.wDistinctWidth,
					vpiPenInfo.wDistinctHeight,
					vpiPenInfo.nSamplingRate,
					vpiPenInfo.nSamplingDist,
					vpiPenInfo.cPens,
					vpiPenInfo.fuOEM,
					vpiPenInfo.dwSamplingRateMin,
					vpiPenInfo.dwSamplingRateMax);

			DBG_TRACE_MISC((LPSTR)szDbg);
#endif

			if(vpiPenInfo.lPdc & PDC_INTEGRATED)
				{
				// Need to update calibration structure for future.
				CallVirtualPenDriver(VPEND_GETCALIBRATION | VpenD_lParam1_ptr,
						 (LPARAM)(LPDRV_CALBSTRUCT)&gcbCalibrate,
						 (LPARAM)(DWORD)NULL);

#ifdef DEBUG
				wsprintf((LPSTR)szDbg,
						"CALBSTRUCT: cb.X=%li, cb.Y=%li, cb.DW=%li, cb.DH=%li",
						gcbCalibrate.dwOffsetX,
						gcbCalibrate.dwOffsetY,
						gcbCalibrate.dwDistinctWidth,
						gcbCalibrate.dwDistinctHeight);

				DBG_TRACE_MISC((LPSTR)szDbg);
            }
         else
            {
            DBG_TRACE_MISC("PDC_INTEGRATED is not set in vpiPenInfo.lPdc");
#endif	
				}

			// Now that there is a pen info structure, determine
			// how many tablet units the pen will have to move before
			// moving one unit on the screen.  This will be used when
			// the driver acts like a mouse.
			// Cannot use globals due to dynamic display resolution changes!!!
			// viUnitsUntilMoveX=(WORD)vpiPenInfo.cxRawWidth/vicxScreen;
         		// viUnitsUntilMoveY=(WORD)vpiPenInfo.cyRawHeight/vicyScreen;
		}
		break;

		case DRV_FREE:
		{
			// Tell VpenD that the driver is being unloaded,
			// so completely clean up pointers and other
                        // driver-related information.
			DBG_TRACE_PARAM(lParam1,lParam2);
			DBG_WARNING_NULLS(lParam1,lParam2,glplParamBoth);

			CallVirtualPenDriver(VPEND_FREE,(LPARAM)NULL,(LPARAM)NULL);
		}
		break;

		// Notify the virtual driver that it's time to start sending
		// pen packets.
		case DRV_ENABLE:
		{
			// Tell VpenD to start sending interrupts.
			DBG_TRACE_PARAM(lParam1,lParam2);
			DBG_WARNING_NULLS(lParam1,lParam2,glplParamBoth);

			if(!CallVirtualPenDriver(VPEND_ENABLE,(LPARAM)NULL,(LPARAM)NULL))
			{
				DBG_WARNING(0L,0L,"Virtual Pen Driver failed to Enable");
				wMiscFlags|=ERROR_DURING_ENABLE;
				dwRet=(LRESULT)DRV_FAILURE;
			}

			if(wMiscFlags & IN_SETUP_MODE)
			{
				//MessageBox(GetFocus(),"DRV_ENABLE calling setup dialog",
				//		   "Pen Driver",MB_OK);
				dwRet=CallPenUIDLL(hDriver,GetFocus(),NULL);
			}
		}
		break;


      		// Notify the virtual driver to send no more pen packets.
		case DRV_DISABLE:
		{
			DBG_TRACE_PARAM(lParam1,lParam2);
			DBG_WARNING_NULLS(lParam1,lParam2,glplParamBoth);

			if(!(wMiscFlags & ERROR_DURING_ENABLE))
				CallVirtualPenDriver(VPEND_DISABLE,(LPARAM)NULL,(LPARAM)NULL);
		}
		break;

		case DRV_REMOVE:
		case DRV_INSTALL:
			DBG_TRACE_PARAM(lParam1,lParam2);
			DBG_WARNING_NULLS(lParam1,lParam2,glplParamBoth);
			DBG_TRACE_MISC((LPSTR)"This message not supported!");
			break;

		case DRV_POWER:
		case DRV_EXITSESSION:
			DBG_TRACE_PARAM(lParam1,lParam2);
			DBG_WARNING_NULLS(lParam1,lParam2,glplParamBoth);
			DBG_TRACE_MISC((LPSTR)"This message not supported!");

			break;

		case DRV_EXITAPPLICATION:
#ifdef DEBUG
			DBG_TRACE_PARAM(lParam1,lParam2);
			if(lParam1!=1L)
				DBG_WARNING(lParam1,lParam2,(LPSTR)"Expected lParam1==1!");
			if(lParam2!=NULL)
				DBG_WARNING(lParam1,lParam2,glplParam2Null);
			DBG_TRACE_MISC((LPSTR)"No need to support this message!");
#endif
			break;

		case DRV_SetEntryPoints:
			{
			// assume that there will be an error...
			dwRet=DRV_FAILURE;

			DBG_TRACE_PARAM(lParam1,lParam2);

			if(IsBadCodePtr((FARPROC)lParam2))
			{
				DBG_ERROR(lParam1,lParam2,(LPSTR)"Invalid Code Pointer : lParam2");
				return DRV_BADPARAM2;
			}

			// Need to check lParam1 to verify that the proper flags are passed
			// in.	Compare to PENREG_flags...
			if( !(lParam1&(PENREG_WILLHANDLEMOUSE|PENREG_DEFAULT)) )
			{
				DBG_ERROR(lParam1,lParam2,(LPSTR)"Invalid Flags in : lParam1");
				return DRV_BADPARAM1;
			}

			if(viNumberOfHookRoutines<MAXHOOKS)
			{
				int iDest;

				// Walk the register list looking for an opening.
				//
				for(iDest=0;iDest<MAXHOOKS;iDest++)
				{
					if(!vrglpfnProcessTabletEvents[iDest])	// If location NULL, found one!
					{
						// Clear interrupts so another "hook" can be added.
						_asm cli

						vrglpfnProcessTabletEvents[iDest]=(HOOKROUTINE)lParam2;

						// If the calling DLL plans to pass the mouse moves
						// to USER, then it will register that fact.

						if( (viActingLikeMouse==-1) &&
							((DWORD)lParam1&PENREG_WILLHANDLEMOUSE) )
							viActingLikeMouse=iDest;

						_asm sti

						// increment the number of hook routines
						viNumberOfHookRoutines++;

						// Found a location for the caller so return a
						// one-based number that will be used at remove time.
						dwRet=(DWORD)(iDest+1);

						DBG_TRACE_MISC("wHookID is the return value from this message");
						break;	// Out of For loop, so don't need to look anymore.
					}
				}
#ifdef DEBUG
				if( dwRet == DRV_FAILURE )
					DBG_WARNING(0L,0L,(LPSTR)"No location left registering!");
#endif
			}
#ifdef DEBUG
			else
				DBG_WARNING(0L,0L,(LPSTR)"All Registration locations taken!");
#endif
		}
		break;

		case DRV_RemoveEntryPoints:
		{
			// Assume a failure.
			dwRet=DRV_FAILURE;
			DBG_TRACE_PARAM(lParam1,lParam2);

			// Need to check lParam1 to verify that the proper flags are passed
			// in.	Compare to PENREG_flags...
			if( !(lParam1&(PENREG_WILLHANDLEMOUSE|PENREG_DEFAULT)) )
			{
				DBG_ERROR(lParam1,lParam2,(LPSTR)"Invalid flags in : lParam1");
				return DRV_BADPARAM1;
			}


			if( (lParam2<1) || (lParam2>MAXHOOKS) )
			{
				DBG_ERROR(lParam1,lParam2,(LPSTR)"HookID out of range! : lParam2");
				return DRV_BADPARAM2;
			}
#ifdef DEBUG
			wsprintf((LPSTR)szDbg,
					 "Removing HookID : %u",
					 (int)lParam2);
			DBG_TRACE_MISC((LPSTR)szDbg);
#endif

			// make zero-based
			lParam2--;

			// Is there something at this location to free?
			if(vrglpfnProcessTabletEvents[(int)lParam2]!=NULL)
			{
				// clear interrupts so hook can be removed
				_asm cli

				// If the calling DLL was handling the mouse, then pen driver needs
				// to start handling it.

				if(viActingLikeMouse==(int)lParam2)
					viActingLikeMouse=-1;

				vrglpfnProcessTabletEvents[(int)lParam2]=NULL;

				_asm sti

				// decrement the number of hook routines
				viNumberOfHookRoutines--;

				// no errors...
				dwRet=DRV_SUCCESS;
			}
#ifdef DEBUG
			if( (dwRet==DRV_SUCCESS) )
				DBG_TRACE_MISC((LPSTR)"HookID removed successfully");
			if( (dwRet!=DRV_SUCCESS) )
				DBG_ERROR(0L,0L,(LPSTR)"HookID removed successfully");
#endif
		}
		break;

		// Sampling rate and sampling distance values will be passed to the virtual
		// pen driver. If the hardware supports it, then great. If not, it doesn't
                // matter.
		case DRV_SetPenSamplingRate:
		{
			DWORD dwOldRate=vpiPenInfo.nSamplingRate;
#ifdef DEBUG
			WORD wNewRate=LOWORD(lParam1);
			DBG_TRACE_PARAM(lParam1,lParam2);
			if(lParam2!=NULL)
				DBG_WARNING(lParam1,lParam2,glplParam2Null);
			if((wNewRate<1) || (wNewRate>200))
				DBG_WARNING(lParam1,lParam2,(LPSTR)"Rate outside expected bounds : lParam1");
#endif
			// Tell VpenD to change sampling rate if possible.  Remember that
			// the documentation says that the desired sampling rate will be in the
			// low word of lParam1.
			// The value returned will be the new sampling rate.
			if( CallVirtualPenDriver(VPEND_SETSAMPLINGRATE,
						 MAKELONG(LOWORD(lParam1),0),
						 (LPARAM)NULL) )
			{
				// sampling rate changed, notify clients
				RecordTabletEvent(DT_UPDATEPENINFO,wMsg);
#ifdef DEBUG
				wsprintf((LPSTR)szDbg,
						 "NewRate=%li, OldRate=%li",
						 dwRet,dwOldRate);
				DBG_TRACE_MISC((LPSTR)szDbg);
#endif
				// return old rate
				dwRet=dwOldRate;
			}
		}
		break;

	  // This message is new and trivial.
		case DRV_GetPenSamplingRate:
		{
			DBG_TRACE_PARAM(lParam1,lParam2);
			DBG_WARNING_NULLS(lParam1,lParam2,glplParamBoth);

			dwRet=MAKELONG(vpiPenInfo.nSamplingRate,0);
		}
		break;

		case DRV_SetPenSamplingDist:
		{
			WORD wNewDist=LOWORD(lParam1);
#ifdef DEBUG
			DBG_TRACE_PARAM(lParam1,lParam2);
			if(lParam2!=NULL)
				DBG_WARNING(lParam1,lParam2,glplParam2Null);
#endif
			// Tell the driver to change the sampling distance if
			// possible.  Remember that the low word of lParam1 will contain
			// the desired value.
			// The old value is returned.
			dwRet=CallVirtualPenDriver(VPEND_SETSAMPLINGDIST,
						 MAKELONG(wNewDist,0),
						 (LPARAM)NULL);

			vpiPenInfo.nSamplingDist=MAKELONG(wNewDist,0);
#ifdef DEBUG
			wsprintf((LPSTR)szDbg,"Old Dist=%u, New Dist=%u",
						 LOWORD(dwRet),wNewDist);
			DBG_TRACE_MISC((LPSTR)szDbg);
#endif
			RecordTabletEvent(DT_UPDATEPENINFO,wMsg);
		}
		break;

		case DRV_GetCalibration:
		{
			LPCALBSTRUCT lpcbTmp=(LPCALBSTRUCT)lParam1;

			DBG_TRACE_PARAM(lParam1,lParam2);

			// Check to ensure that the CALBSTRUCT in lParam1 is valid.
			if( IsBadWritePtr( (LPVOID)lParam1,(int)sizeof(CALBSTRUCT) ))
			{
				DBG_ERROR(lParam1,lParam2,(LPSTR)"Bad write pointer : :lParam1");
				return DRV_BADPARAM1;
			}

#ifdef DEBUG
			if(lParam2!=NULL)
				DBG_WARNING(lParam1,lParam2,glplParam2Null);

			wsprintf((LPSTR)szDbg,"PI.fuOEM=%lx, PI.lPdc=%lx",
					 vpiPenInfo.fuOEM,vpiPenInfo.lPdc);
			DBG_TRACE_MISC((LPSTR)szDbg);
#endif
			if(vpiPenInfo.lPdc & PDC_INTEGRATED)
			{
				// Extract calibration values from the calibration structure.
				lpcbTmp->wOffsetX=(WORD)gcbCalibrate.dwOffsetX;
				lpcbTmp->wOffsetY=(WORD)gcbCalibrate.dwOffsetY;
				lpcbTmp->wDistinctWidth=(WORD)gcbCalibrate.dwDistinctWidth;
				lpcbTmp->wDistinctHeight=(WORD)gcbCalibrate.dwDistinctHeight;
				dwRet=DRV_SUCCESS;
			}
			else  // This hardware doesn't support calibration!
				dwRet=DRV_FAILURE;
		}
		break;

		case DRV_SetCalibration:
		{
			LPCALBSTRUCT lpcbTmp=(LPCALBSTRUCT)lParam1;

			DBG_TRACE_PARAM(lParam1,lParam2);

			// Check to ensure that the CALBSTRUCT in lParam1 is valid.
			if( IsBadReadPtr( (LPVOID)lParam1,(int)sizeof(CALBSTRUCT) ))
			{
				DBG_ERROR(lParam1,lParam2,(LPSTR)"Bad read pointer : :lParam1");
				return DRV_BADPARAM1;
			}
#ifdef DEBUG
			if(lParam2!=NULL)
				DBG_WARNING(lParam1,lParam2,glplParam2Null);
#endif
			if(vpiPenInfo.lPdc & PDC_INTEGRATED)
			{
				// Can't have a tablet with distinct width or height of zero.
				if( (lpcbTmp->wDistinctWidth==0) ||
					(lpcbTmp->wDistinctHeight==0) )
					dwRet=DRV_BADSTRUCT;
				else
				{
					// Convert the calibration structure and then call VpenD.
					gcbCalibrate.dwOffsetX=(DWORD)lpcbTmp->wOffsetX;
					gcbCalibrate.dwOffsetY=(DWORD)lpcbTmp->wOffsetY;
					gcbCalibrate.dwDistinctWidth=(DWORD)lpcbTmp->wDistinctWidth;
					gcbCalibrate.dwDistinctHeight=(DWORD)lpcbTmp->wDistinctHeight;
					dwRet=CallVirtualPenDriver(VPEND_SETCALIBRATION | VpenD_lParam1_ptr,
											 (LPARAM)(LPDRV_CALBSTRUCT)&gcbCalibrate,
											 (LPARAM)(DWORD)NULL);
					if(dwRet)
					{
						// Virtual driver always returns TRUE.
						RecordTabletEvent(DT_UPDATEPENINFO,wMsg);
						// Update the global PENINFO structure.
						vpiPenInfo.wDistinctWidth=(DWORD)lpcbTmp->wDistinctWidth;
						vpiPenInfo.wDistinctHeight=(DWORD)lpcbTmp->wDistinctHeight;
					}
				}
			}
			else // This hardware doesn't support calibration!
				dwRet=DRV_FAILURE;
		}
		break;

// The following three messages -- DRV_PenPlayStart, DRV_PenPlayBack, and DRV_PenPlayStop --
// will be reflected to the virtual driver.

    case DRV_PenPlayStart:
    {
        DBG_TRACE_PARAM(lParam1,lParam2);

        if (IsBadWritePtr((LPVOID)lParam1,(int)sizeof(DWORD)))
        {
            DBG_ERROR(lParam1,lParam2,(LPSTR)"DRV_PenPlayStart bad lParam1");
            return DRV_BADPARAM1;
        }

        if ((lParam2 != PLAY_VERSION_10_DATA) &&
            (lParam2 != PLAY_VERSION_20_DATA))
        {
            DBG_ERROR(lParam1,lParam2,(LPSTR)"DRV_PenPlayStart bad lParam2");
            return DRV_BADPARAM2;
        }

        dwPlayFlags = lParam2;

        dwRet=CallVirtualPenDriver(VPEND_PENPLAYSTART | VpenD_lParam1_ptr,
                                   (LPARAM)(DWORD)lParam1,
                                   (LPARAM)(DWORD)lParam2);
    }
    break;

    case DRV_PenPlayBack:
    {
        DWORD dwSizeTemp;

        DBG_TRACE_PARAM(lParam1,lParam2);

        // Check to see if lParam2 (which is the count of packets) is within
        // a valid range.

        if ((lParam2 & 0xFFFF0000) || (lParam2 == 0))
        {
            DBG_ERROR(lParam1,lParam2,(LPSTR)"DRV_PenPlayBack bad lParam2");
            return DRV_BADPARAM2;
        }

        // Check to ensure that the PENPACKETS in lParam1 are really there.
        // Check to see if the size is big enough for all the data to be
        // there if this is VERSION_10DATA.  Don't know if it's version
        // 1 or 2 so checking for 1 is better than nothing.

        dwSizeTemp = (dwPlayFlags == PLAY_VERSION_10_DATA) ?
                     sizeof(PENPACKET) :
                     sizeof(DRV_PENPACKET);

        if (IsBadReadPtr((LPVOID)lParam1,
                         (int)(dwSizeTemp * lParam2)
                        )
           )
        {
            DBG_ERROR(lParam1,lParam2,(LPSTR)"DRV_PenPlayBack bad lParam1");
            return DRV_BADPARAM1;
        }

        dwRet=CallVirtualPenDriver(VPEND_PENPLAYBACK | VpenD_lParam1_ptr,
                                   (LPARAM)(DWORD)lParam1,
                                   (LPARAM)(DWORD)lParam2);
    }
    break;

    case DRV_PenPlayStop:
    {
        DBG_TRACE_PARAM(lParam1,lParam2);

        dwRet=CallVirtualPenDriver(VPEND_PENPLAYSTOP,
                                   (LPARAM)(DWORD)NULL,
                                   (LPARAM)(DWORD)NULL);
    }
    break;

		case DRV_GetVersion:
		{
			DBG_TRACE_PARAM(lParam1,lParam2);
			DBG_WARNING_NULLS(lParam1,lParam2,glplParamBoth);

			dwRet=MAKELONG(0x0002,sizeof(DRV_PENPACKET));
		}
		break;

		case DRV_GetPenInfo:
		{
			int i;
			LPPENINFO lppi=(LPPENINFO)lParam1;

			DBG_TRACE_PARAM(lParam1,lParam2);
#ifdef DEBUG
			if(lParam2!=NULL)
				DBG_WARNING(lParam1,lParam2,glplParam2Null);
#endif
			// Check to ensure that the peninfo pointer is valid.
			if( IsBadWritePtr( (LPVOID)lParam1,(int)sizeof(PENINFO) ))
			{
				DBG_ERROR(lParam1,lParam2,(LPSTR)"Bad write pointer : :lParam1");
				return DRV_BADPARAM1;
			}

			// Need to copy individual components because of size difference!
			lppi->cxRawWidth=(UINT)vpiPenInfo.cxRawWidth;
			lppi->cyRawHeight=(UINT)vpiPenInfo.cyRawHeight;
			lppi->wDistinctWidth=(UINT)vpiPenInfo.wDistinctWidth;
			lppi->wDistinctHeight=(UINT)vpiPenInfo.wDistinctHeight;

			lppi->nSamplingRate=(int)vpiPenInfo.nSamplingRate;
			lppi->nSamplingDist=(int)vpiPenInfo.nSamplingDist;

			lppi->lPdc=(LONG)vpiPenInfo.lPdc;
			lppi->cPens=(int)vpiPenInfo.cPens;

			lppi->cbOemData=(int)vpiPenInfo.cbOemData;

			// copy OEM data... and reserved words...
			for(i=0;i<MAXOEMDATAWORDS;i++)
			{
				lppi->rgoempeninfo[i].wPdt=(UINT)vpiPenInfo.rgoempeninfo[i].wPdt;
				lppi->rgoempeninfo[i].wValueMax=(UINT)vpiPenInfo.rgoempeninfo[i].wValueMax;
				lppi->rgoempeninfo[i].wDistinct=(UINT)vpiPenInfo.rgoempeninfo[i].wDistinct;
			}

			for(i=0;i<7;i++)
				lppi->rgwReserved[i]=(UINT)vpiPenInfo.rgdwReserved[i];

			lppi->fuOEM=(UINT)vpiPenInfo.fuOEM;

			dwRet=DRV_SUCCESS;
		}
		break;

// The following messages get serviced by OEMDriverProc so just fall through.

		case DRV_OPEN:
			DBG_TRACE_PARAM(lParam1,lParam2);

#ifdef DEBUG
			if(lParam2 != NULL)
				DBG_WARNING(lParam1,lParam2,glplParam2Null);
#endif

			break;

		case DRV_CLOSE:
			DBG_TRACE_PARAM(lParam1,lParam2);
			DBG_WARNING_NULLS(lParam1,lParam2,glplParamBoth);
			break;

		case DRV_QUERYCONFIGURE:
			DBG_TRACE_PARAM(lParam1,lParam2);
			DBG_WARNING_NULLS(lParam1,lParam2,glplParamBoth);
			break;

		case DRV_CONFIGURE:
	   {
			DBG_TRACE_PARAM(lParam1,lParam2);
			if( !IsWindow(LOWORD(lParam1)) )
			{
				DBG_ERROR(lParam1,lParam2,(LPSTR)"Bad window handle: :lParam1");
				return DRV_BADPARAM1;
			}

			// need to pass in the hardware info...
			CallPenUIDLL(hDriver,LOWORD(lParam1),NULL);
	   }
		break;

		// For now, plug in a name for the driver, but the OEM will get the chance
		// to modify this when OEMUI.C is called.
		case DRV_GetName:
		{
			int iLen=lstrlen((LPSTR)szDriverName);
			DBG_TRACE_PARAM(lParam1,lParam2);
			if( ((int)lParam1<iLen) )
			{
				DBG_ERROR(lParam1,lParam2,(LPSTR)"Bad Buffer Size: lParam1");
				return DRV_BADPARAM1;
			}

			// validate parameters...
			if( (IsBadWritePtr((LPVOID)lParam2,(int)lParam1) ) )
			{
				DBG_ERROR(lParam1,lParam2,(LPSTR)"Bad buffer pointer : lParam2");
				return DRV_BADPARAM2;
			}
			// buffer's good, copy string...
			lstrcpy((LPSTR)lParam2,(LPSTR)&szDriverName);

			// Return the number of characters copied not including the NULL!
			dwRet=(LRESULT)iLen;
			
	 }
	 break;

		case DRV_Query:
		{
			DBG_TRACE_PARAM(lParam1,lParam2);

			switch(LOWORD(lParam1))
			{
				case DRV_Calibrate:
				case DRV_GetCalibration:
				case DRV_SetCalibration:
					if(vpiPenInfo.lPdc & PDC_INTEGRATED)
						dwRet=DRV_SUCCESS;
					else
						dwRet=DRV_FAILURE;
					break;

				case DRV_OPEN:
				case DRV_CLOSE:
				case DRV_LOAD:
				case DRV_FREE:
				case DRV_ENABLE:
				case DRV_DISABLE:
				case DRV_INSTALL:
				case DRV_REMOVE:
				case DRV_SetEntryPoints:
				case DRV_RemoveEntryPoints:
				case DRV_SetPenSamplingRate:
				case DRV_GetPenSamplingRate:
				case DRV_SetPenSamplingDist:
				case DRV_PenPlayStart:
				case DRV_PenPlayBack:
				case DRV_PenPlayStop:
				case DRV_GetVersion:
				case DRV_GetPenInfo:
				case DRV_GetName:
				case DRV_Query:
				case DRV_QUERYCONFIGURE:
				case DRV_CONFIGURE:
					dwRet=DRV_SUCCESS;
					break;
				default:
					DBG_TRACE_MISC("Unknown DRV_Query message");
					dwRet=DRV_FAILURE;
					break;
			}
		}
		break;

		default:
			return DefDriverProc(dwDrvID,hDriver,wMsg,lParam1,lParam2);
		break;
	} // end of main switch

		DBG_TRACE_RET(dwRet);
		return (LRESULT)dwRet;
}


long NEAR PASCAL CallVirtualPenDriver(DWORD dwFlags,LPARAM lParam1, LPARAM lParam2)
{
	DWORD dwRet=(DWORD)0;
	WORD lowFlags=LOWORD(dwFlags);
	WORD hiFlags=HIWORD(dwFlags);
	WORD lowP1=LOWORD(lParam1);
	WORD hiP1 =HIWORD(lParam1);
	WORD lowP2=LOWORD(lParam2);
	WORD hiP2 =HIWORD(lParam2);

	DBG_TRACE_VDRVC(dwFlags);
	if(vlpfnVpenD_API_Entry_Point)
	{
		_asm {
			mov ax,lowFlags
			mov dx,hiFlags

			mov cx,hiP1
			mov si,lowP1

			mov bx,hiP2
			mov di,lowP2
			}
		dwRet=(DWORD)(long)vlpfnVpenD_API_Entry_Point();
	}
#ifdef DEBUG
	else
	{
		DBG_ERROR(0L,0L,(LPSTR)"Blocked! Invalid Virtual Pen Driver entry point");
	}
#endif
	DBG_TRACE_VDRVR(dwFlags);
	return dwRet;
}


DWORD NEAR PASCAL CallPenUIDLL(HDRVR hDriver,HWND hParent,BOOL fInvoke)
{
	HINSTANCE hLib;
	DWORD dwRet=DRV_FAILURE;  // assume failure

	// Load UI DLL and call the ConfigDialog routine.
	if( (hLib=LoadLibrary((LPSTR)szPenDLL)) > HINSTANCE_ERROR )
		{
		FN_CONFIGDIALOG fnConfigDialog;

		if( fnConfigDialog = (FN_CONFIGDIALOG)GetProcAddress(hLib,(LPSTR)szConfigDialog) )
			{
			dwRet=fnConfigDialog(hDriver,hParent);
			}
#ifdef DEBUG
		else
			{
			DBG_ERROR(0L,0L,"ConfigDialog routine not exported from penui.dll");
			}
#endif
		FreeLibrary(hLib);
		}
#ifdef DEBUG
	else
		{
		DBG_ERROR(0L,0L,"Could not load penui.dll");
		}
#endif
	return dwRet;
}


int CALLBACK _loadds _export WEP(int nExitType)
{
	DBG_TRACE_MISC((LPSTR)"WEP called, returning TRUE");
	return 1;
}

void NEAR PASCAL RecordTabletEvent(WORD wEventType,WPARAM wMsg)
{
	LPFNHOOKROUTINE lpFunction=&vrglpfnProcessTabletEvents[0];
	int i;
	for(i=0;i<MAXHOOKS;i++)
	{
		if(*lpFunction)
		{
			_asm cli
			(*lpFunction)((WORD)i+1,wEventType,wMsg,0L);
			_asm sti
		}
		lpFunction++;
	}
}

