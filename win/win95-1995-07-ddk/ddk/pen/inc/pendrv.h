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

//=============================================================================
//
// File:  pendrv.h - Header file for Uni-pen driver
//
// Comment:  Contains the Structures and constants that the pen driver
//           uses to interface with the virtual pen driver and clients.
//
//=============================================================================


// Include Windows header file
#ifndef VxD
#include <windows.h>
#define NOPENAPPS
#include <penwin.h>
//#else
//#include "Basedef.H"
#endif


// some basic types
#ifdef CCODE
#ifdef VxD
typedef unsigned long DWORD;
typedef unsigned short WORD;
#endif
#endif

#ifdef CCODE
#ifdef VxD
typedef PDWORD LPDWORD;
typedef char *LPSTR;
#endif
//typedef unsigned long *LPDWORD;
//typedef char *LPSTR;
#else
typedef unsigned long FAR *LPDWORD;
typedef char FAR *LPSTR;
#endif

#ifdef VxD

#ifdef DEBUG
#define DBGM_TRACE		0x00000001
#define DBGM_WARNING	0x00000002
#define DBGM_ERROR		0x00000004
#define DBGM_RETURN 	0x00000008
#define DBGM_MISC		0x00000010
#define DBGM_PWARN1		0x00000020
#define DBGM_PWARN2		0x00000040
#define DBGM_MISC_HEX	0x00000080
#define DBGM_STRING 	0x00000100

#define DBG_TRACE(Str)			Dbg_Output(DBGM_TRACE,1,Str)
#define DBG_WARNING(Con,Str)	Dbg_Output(DBGM_WARNING,Con,Str)
#define DBG_ERROR(Con,Str)		Dbg_Output(DBGM_ERROR,Con,Str)
#define DBG_RETURN(Con,Str)		Dbg_Output(DBGM_RETURN,Con,Str)
#define DBG_MISC(Con,Str)		Dbg_Output(DBGM_MISC,Con,Str)
#define DBG_MISC_HEX(Con,Str)	Dbg_Output(DBGM_MISC_HEX,Con,Str)
#define DBG_RECORDMESSAGE(dwMsg) Dbg_RecordMessage(dwMsg)
#define DBG_STRING(Con,Str) 	Dbg_Output(DBGM_STRING,Con,Str)

#ifdef WIN31COMPAT
#define DBG_PWARN1(Con)
#define DBG_PWARN2(Con)
#else
#define DBG_PWARN1(Con) 		Dbg_Output(DBGM_PWARN1,Con,NULL)
#define DBG_PWARN2(Con) 		Dbg_Output(DBGM_PWARN2,Con,NULL)
#endif // WIN31COMPAT

#else  // don't expand the VxD macros
#define DBG_TRACE(Str)
#define DBG_WARNING(Con,Str)
#define DBG_ERROR(Con,Str)
#define DBG_RETURN(Con,Str)
#define DBG_MISC(Con,Str)
#define DBG_MISC_HEX(Con,Str)
#define DBG_RECORDMESSAGE(dwMsg)
#define DBG_PWARN1(Con)
#define DBG_PWARN2(Con)
#define DBG_STRING(Con,Str)
#endif


#else  // we're talking about the pen driver
#ifdef DEBUG

#define DBG_PARAM	  0x0001
#define DBG_RETURN	  0x0002
#define DBG_MISC	  0x0004
#define DBG_VDRVC	  0x0008
#define DBG_VDRVR	  0x0010
#define DBG_WARN_NULLS 0x0020
#define DBG_ALL (DBG_PARAM | DBG_RETURN | DBG_MISC | DBG_VDRVC | DBG_VDRVR | DBG_WARN_NULLS)

#define DBF_ALL (DBF_TRACE | DBF_WARNING | DBF_ERROR | DBF_FATAL)

void FAR PASCAL DebugInfo(WORD wType,LPSTR lpMsg,LPARAM lParam1,LPARAM lParam2,LPSTR lpStr,DWORD dwVMsg);
// Output macros

// for debug strings
typedef struct _MSGS {
    UINT wMsg;
    LPSTR lpStr;
} MSGS;

typedef MSGS FAR *LPMSGS;

// for debug strings
typedef struct _MSGSL {
	DWORD dwMsg;
    LPSTR lpStr;
} MSGSL;
typedef MSGSL FAR *LPMSGSL;

#define DBG_TRACE_PARAM(lp1,lp2)	(DebugInfo(DBF_TRACE|DBG_PARAM,glpMsg,lp1,lp2,NULL,NULL))
#define DBG_TRACE_RET(lp1)			(DebugInfo(DBF_TRACE|DBG_RETURN,glpMsg,lp1,NULL,NULL,NULL))
#define DBG_TRACE_MISC(lpStr)		(DebugInfo(DBF_TRACE|DBG_MISC,glpMsg,NULL,NULL,lpStr,NULL))

#define DBG_TRACE_VDRVC(dwVMsg) (DebugInfo(DBF_TRACE|DBG_VDRVC,glpMsg,NULL,NULL,NULL,dwVMsg))
#define DBG_TRACE_VDRVR(dwVMsg) (DebugInfo(DBF_TRACE|DBG_VDRVR,glpMsg,NULL,NULL,NULL,dwVMsg))

#define DBG_WARNING(lp1,lp2,lpStr)	(DebugInfo(DBF_WARNING,glpMsg,lp1,lp2,lpStr,NULL))
#define DBG_WARNING_NULLS(lp1,lp2,lpStr) (DebugInfo(DBF_WARNING|DBG_WARN_NULLS,glpMsg,lp1,lp2,lpStr,NULL))
#define DBG_ERROR(lp1,lp2,lpStr)	(DebugInfo(DBF_ERROR,glpMsg,lp1,lp2,lpStr,NULL))
#define DBG_FATAL(lp1,lp2,lpStr)	(DebugInfo(DBF_FATAL,glpMsg,lp1,lp2,lpStr,NULL))

#else  // don't expand the macros
#define DBG_TRACE_PARAM(lp1,lp2)
#define DBG_TRACE_RET(lp1)
#define DBG_TRACE_MISC(lpStr)
#define DBG_TRACE_VDRVC(dwVMsg)
#define DBG_TRACE_VDRVR(dwVMsg)
#define DBG_WARNING(lp1,lp2,lpStr)
#define DBG_WARNING_NULLS(lp1,lp2,lpStr)
#define DBG_ERROR(lp1,lp2,lpStr)
#define DBG_FATAL(lp1,lp2,lpStr)
#endif

#endif	// end debug macros.


// version numbers
#define WIN_VER_31	0x0000030A
#define WIN_VER_311 	0x0000030B
#define WIN_VER_40	0x00000400


// The following are constants that define different types of supported
// hardware.
// Warning: The same values are used in the registry too, so please be 
// sure to update those too.
#ifdef CPQ
#define TYPE_CPQCONCERTO		2
#endif // CPQ
#define TYPE_WACOMHD648A		6
#define TYPE_WACOMSD510C		8
#define TYPE_WACOMUD0608R		9
#define TYPE_UNKNOWN				11

#ifdef VxD
// Need to include some values from penwin.h for VpenD
#define PDK_OUTOFRANGE		0x4000
#define PDK_UP					0x0000
#define PDK_DOWN				0x0001
#define PDK_BARREL1			0x0002
#define PDK_BARREL2			0x0004
#define PDK_BARREL3			0x0008
#define PDK_SWITCHES			(PDK_DOWN|PDK_BARREL1|PDK_BARREL2|PDK_BARREL3)
#define PDK_INVERTED			0x0080

#define PHW_PRESSURE			0x00000001	// report pressure in OEMdata if avail
#define PHW_HEIGHT			0x00000002	// ditto height
#define PHW_ANGLEXY			0x00000004	// ditto xy angle
#define PHW_ANGLEZ			0x00000008	// ditto z angle
#define PHW_BARRELROTATION	0x00000010	// ditto barrel rotation
#define PHW_OEMSPECIFIC		0x00000020	// ditto OEM-specific value
#define PHW_PDK				0x00000040	// report per-point PDK_xx in OEM data
#define PHW_ALL				0x0000007F	// report everything

#ifdef DEBUG
#define HW_SEQUENTIAL		0x00000100
#define HW_PROFILE			0x00000200
#define HW_DEBUG				0x00000400
#endif // DEBUG

// Playback constants
#define PLAY_VERSION_10_DATA 0
#define PLAY_VERSION_20_DATA 1

// return values from pen driver
#define DRV_FAILURE			0x00000000
#define DRV_SUCCESS			0x00000001
#define DRV_BADPARAM1		0xFFFFFFFF
#define DRV_BADPARAM2		0xFFFFFFFE
#define DRV_BADSTRUCT		0xFFFFFFFD

#else  // back to pen driver
#ifdef DEBUG
#define DBG_SEQUENTIAL		0x0100
#define DBG_PROFILE			0x0200
#define DBG_DEBUG				0x0400
#endif // DEBUG

#endif // VxD

// return values from pen driver
#define DRV_NOTSERVICED 	0x00000002
#define DRV_SERVICED			0x00000003

// The following values will be used by the VpenD_API_Proc and the associated
// structures.
//								   dx:ax

#define VPEND_DRIVER			0x01000000
#define VPEND_DISABLE			0x00000001 | VPEND_DRIVER
#define VPEND_LOAD				0x00000002 | VPEND_DRIVER
#define VPEND_FREE				0x00000004 | VPEND_DRIVER
#define VPEND_SETSAMPLINGRATE	0x00000008 | VPEND_DRIVER
#define VPEND_SETSAMPLINGDIST	0x00000010 | VPEND_DRIVER
#define VPEND_GETSAMPLINGRATE	0x00000020 | VPEND_DRIVER
#define VPEND_GETCALIBRATION	0x00000040 | VPEND_DRIVER
#define VPEND_PENPLAYSTART		0x00000080 | VPEND_DRIVER
#define VPEND_PENPLAYSTOP		0x00000100 | VPEND_DRIVER
#define VPEND_ENABLE			0x00000200 | VPEND_DRIVER
#define VPEND_SETCALIBRATION	0x00000400 | VPEND_DRIVER
#define VPEND_PENPLAYBACK		0x00000800 | VPEND_DRIVER
#define VPEND_GETPENINFO		0x00001000 | VPEND_DRIVER
#define VPEND_GETSAMPLINGDIST	0x00002000 | VPEND_DRIVER

#define VPEND_PIINIT			0x00004000 | VPEND_DRIVER
#define VPEND_HWINIT			0x00008000 | VPEND_DRIVER
#define VPEND_COMMANDINIT		0x00010000 | VPEND_DRIVER

#define VPEND_SPEAKUP			0x00020000 | VPEND_DRIVER
#define VPEND_SHUTUP			0x00040000 | VPEND_DRIVER

#define VpenD_lParam1_ptr		0x04000000
#define VpenD_lParam2_ptr		0x08000000

#define VPEND_OEM				0x80000000

#ifdef VxD
// note that penwin.h has these marked as longs!
#define PDC_INTEGRATED	0x0001
#define PDC_PROXIMITY	0x0002
#define PDC_RANGE			0x0004
#define PDC_INVERT		0x0008
#define PDC_RELATIVE		0x0010
#define PDC_BARREL1		0x0020
#define PDC_BARREL2		0x0040
#define PDC_BARREL3		0x0080
#endif // VxD


#ifdef VxD
// OEM return codes when servicing asynchronous interrupts
#define FF_UNKNOWN			0x00000000
#define FF_SUCCESS			0x00000001
#define FF_COM_BASE			0x00000002
#define FF_OVERRUN			0x00000004
#define FF_RECORD_ERROR 	0x00000008
#define FF_EXPECTING_SYNC	0x00000010
#define FF_INCOMPLETE		0x00000020
//#define FF_COMPLETE		0x00000040
#define FF_ERROR				0x00000080
#define FF_PENPACKET			0x00000100
#define FF_OORPENPACKET		0x00000200
#define FF_DATANOTREADY 	0x00000400
#define FF_OEMSPECIFIC		0x80000000

// for ReadINI
#define LONGINT 1
#define HEXINT	2
#define BOOLTYPE 3

// PDT_ values that match what is found in penwin.h
#define PDT_NULL		0
#define PDT_PRESSURE	1
#define PDT_HEIGHT		2
#define PDT_ANGLEXY		3
#define PDT_ANGLEZ		4
#define PDT_BARRELROTATION	5
#define PDT_OEMSPECIFIC		16

#ifdef DEBUG
#define PDT_SEQUENTIAL	17
#define PDT_TIMING		18
#define PDT_PROFILE		19
#define PDT_DEBUG		20
#endif

#endif

// mouse constants...
// Definitions of the status word passed to the event_proc
// in the AX register.	The registers passed to the event_proc
// are as follows:
//
//	AX = status as defined below
//	BX = delta X or normalized abs
//	CX = delta Y or normalized abs
//	DX = number of buttons
//
// If the SF_ABSOLUTE bit is clear in the status word, then the
// BX and CX are reporting relative motion from the last reported
// position.  If this bit is set, then BX and CX contain normalized
// absolute coordinates between 0 and +65535, which will be mapped
// by the event_proc onto the display surface

#define SF_MOVEMENT 0x0001		//Movement occured
#define SF_B1_DOWN	0x0002		//Button 1 (SW1) changed to down
#define SF_B1_UP	0x0004		//Button 1 (SW1) changed to up
#define SF_B2_DOWN	0x0008		//Button 2 (SW3) changed to down
#define SF_B2_UP	0x0010		//Button 2 (SW3) changed to up
#define SF_ABSOLUTE 0x8000		//BX,CX are normalized absolute coords


//-----------------------------------------------------------------------------

#ifdef VxD
#define MAXOEMDATAWORDS	6
#endif

struct tagDRV_OEMPENINFO {
	DWORD wPdt;		  //Pen Data Type
	DWORD wValueMax;  //Largest returnable value by device
	DWORD wDistinct;  //number of distinct readings possible
} ;

typedef struct tagDRV_OEMPENINFO DRV_OEMPENINFO;

#ifdef CCODE
typedef DRV_OEMPENINFO *LPDRV_OEMPENINFO;
#else
typedef DRV_OEMPENINFO FAR *LPDRV_OEMPENINFO;
#endif

typedef struct tagDRV_PENINFO {
	DWORD cxRawWidth;		//width of tablet .001in & max X
	DWORD cyRawHeight;		//height of tablet .001in & max Y
	DWORD wDistinctWidth;
	DWORD wDistinctHeight;
	DWORD nSamplingRate;
	DWORD nSamplingDist;
	DWORD dwSamplingRateMin; // minimum sampling rate value
	DWORD dwSamplingRateMax; // maximum sampling rate value
   DWORD fuOEM;

	DWORD lPdc;
	DWORD cPens;
	DWORD cbOemData;			//width of OEM data packet
	DRV_OEMPENINFO rgoempeninfo[MAXOEMDATAWORDS];
   DWORD rgdwReserved[8];
} DRV_PENINFO;

#ifdef VxD
typedef DRV_PENINFO *LPDRV_PENINFO;
#else
typedef DRV_PENINFO FAR *LPDRV_PENINFO;
#endif

typedef struct DRV_CalbStruct {
	DWORD dwOffsetX;
	DWORD dwOffsetY;
	DWORD dwDistinctWidth;
	DWORD dwDistinctHeight;
} DRV_CALBSTRUCT;

#ifdef VxD
typedef DRV_CALBSTRUCT *PDRV_CALBSTRUCT;
#else
typedef DRV_CALBSTRUCT FAR *LPDRV_CALBSTRUCT;
#endif

//#ifdef VxD
struct tagDRV_PENPACKET  {
	WORD wTabletX;				//	dw	0	; X in tablet coordinates
	WORD wTabletY;				//	dw	0	; Y in tablet coordinates
	WORD wPDK;					//	dw	0	; various status bits for packet
	WORD rgwOemData[MAXOEMDATAWORDS]; // OEM info like pressure
	DWORD ddTimeStamp;			//	dd	0
#ifdef DEBUG
	DWORD ddSequential;			//	dd	0
	DWORD ddProfile;			//	dd	0
	DWORD ddDebug;				//	dd	0
#endif
} ;

typedef struct tagDRV_PENPACKET DRV_PENPACKET;

#ifndef VxD
typedef DRV_PENPACKET FAR *LPDRV_PENPACKET;
#else

#ifdef CCODE
typedef DRV_PENPACKET *LPDRV_PENPACKET;
#else
typedef DRV_PENPACKET FAR *LPDRV_PENPACKET;
#endif

#endif

struct tagvpend_init {
   LPDWORD			lpDataType;
   LPDWORD			lpNumberOfPenPackets;
   LPDWORD			lpOffsetIntoBuffer;
   LPDWORD			lpIndexIntoBuffer;
   LPDRV_PENPACKET	lpPenPacketBuffer;
   LPDRV_PENINFO	lpPenInfo;
   LPSTR			lpOEMBuffer;
   LPDWORD			lpfnEntryPoint;
} ;

typedef struct tagvpend_init VPEND_INIT;

#ifdef VxD
typedef VPEND_INIT *LPVPEND_INIT;
#else
typedef VPEND_INIT FAR *LPVPEND_INIT;
#endif

#ifdef VxD
struct tag_VpenD_Register  {
	DWORD Device_VM_Handle; 	// old VPenD_Owner
	DWORD ibThis;
	DWORD ibThis_offset;
	DWORD ibNext;
	DWORD ibNext_offset;
	DWORD ibCurrent;
	DWORD iNumberOfPenPackets;
	DWORD wPDKLast;
// The following pointers get initialized from the VpenD_Init structure and
// are stored here for per instance useage.
	DWORD		pDataType;
	DWORD		piNumberOfPenPackets;
	DWORD		piThis_Offset;
	DWORD		piThis;
	LPDRV_PENPACKET pPenPacketBuffer;
	LPDRV_PENINFO	pPenInfo;
	DWORD		pOEMBuffer;
	DWORD		pfnEntryPoint;
} ;


typedef struct tag_VpenD_Register _VpenD_Register;
typedef _VpenD_Register *P_VpenD_Register;
#endif

// valid data types for pen driver include
#define DT_PENPACKET		0x00000001
#define DT_TIMERTICK		0x00000002
#define DT_OORPENPACKET 	0x00000004
#define DT_UPDATEPENINFO	0x00000008
#define DT_OEM				0x00000400


#ifdef VxD
typedef struct tag_HARDWAREINFO /*_hardwareinfo*/ {
	DWORD ddCom_Port;
	DWORD com_base;
	DWORD ddIRQ_Number;
	DWORD ddPauseTime;
	DWORD ddReserved;

	DWORD ddOrientation;
	DRV_CALBSTRUCT calibrate;
	DWORD ddBufferZoneX;
	DWORD ddBufferZoneY;

	DWORD dwHardwarePacketSize;
	DWORD dwComPortSettings;	// contains divisor,parity,data,stop
	DWORD dwHwFlags;
	DWORD ddHardwareType;

// Now for some system information that the OEM will not have to deal with.
	DWORD ddTimerTickRate;
	DWORD ddDelayTime;

	DWORD VpenD_IRQ_Handle;
	DWORD SystemVMHandle;
} _HARDWAREINFO, *P_HARDWAREINFO;
#endif


#define HW_CALIBRATE   0x00010000
#define HW_BUFFERZONE  0x00020000
#define HW_PUSHMODEL   0x00040000
#define HW_TIMERMODEL  0x00080000
#define HW_SERIAL	   0x00100000
#define HW_BIOS 	   0x00200000

#ifdef VxD
// for version 1.0 compatibility on playback
typedef struct tagPENPACKET	{
	WORD wTabletX;					 // x in raw coords
	WORD wTabletY;					 // ditto y
	WORD wPDK;						 // state bits
	WORD rgwOemData[MAXOEMDATAWORDS];	// OEM-specific data
} PENPACKET, *PPENPACKET;
#endif



// MORE MISC stuff here...
#ifndef VxD

// multiplex stuff
#define GET_VPEND_API_ENTRY_POINT 1684h
#define VPenD_Device_ID 25h
#define MULTIPLEX_INTERRUPT  2Fh

// PenPacketBuffer declaration
#define MAX_BUFFER_SIZE 32

// Hook routine constants and declaration ************************************
//
// The following is the declaration of the hookroutines that will be called
// at interrupt time.  This driver will support MAXHOOKS number of hook
// routines to call.
//
#define MAXHOOKS	4

// The defined return values for the hook routines are:
#define HR_STOP 	0L
#define HR_CONTINUE	1L

typedef DWORD (CALLBACK *HOOKROUTINE)(WORD wHookID, WORD wEventType,WPARAM wParam, LPARAM lParam);
typedef HOOKROUTINE FAR *LPFNHOOKROUTINE;
// ***************************************************************************

// dialog stuff
// typedef for routines exported from pendll.dll
typedef DWORD (FAR PASCAL *FN_CONFIGDIALOG)(HDRVR hDriver,HANDLE hParent);

// in interupt.c
void _cdecl _interrupt _loadds _export Deal_With_Pen_Packets(void);

// oemui.c
//DWORD NEAR PASCAL OEMDriverProc(DWORD dwDrvID, LPDWORD lpdwState, HDRVR hDriver,UINT wMsg, LPARAM lParam1, LPARAM lParam2);

// drvrproc.c
// The API entry point in the virtual pen driver should look something like
// the following.  Unfortunately, we can use this because we need to stuff
// values directly into the regsiters.
// ax contains message
// ds:si contains pointer to VPEND_INIT structure
typedef long (CALLBACK *VpenD_Func)(void);

long NEAR PASCAL CallVirtualPenDriver(DWORD dwFlags,LPARAM lParam1, LPARAM lParam2);
DWORD NEAR PASCAL CallPenUIDLL(HDRVR hDriver, HWND hParent, BOOL fInvoke);
void NEAR PASCAL RecordTabletEvent(WORD wEventType,WPARAM wMsg);

#endif


#ifdef VxD

#define TYPE_NUL		0x00000001
#define TYPE_HEX		0x00000002
#define TYPE_DEC		0x00000004
#define TYPE_MODEL_STR	0x00000008
#define TYPE_FIXED		0x00000010	// OEM marks value as not-modifiable
#define TYPE_MOD		0x00000020	// OEM marks value as modifiable
#define TYPE_OFF		0x00000040
#define TYPE_PRE		0x00010000
#define TYPE_HEI		0x00020000
#define TYPE_AXY		0x00040000
#define TYPE_ANZ		0x00080000
#define TYPE_BLR		0x00100000
#define TYPE_CMD		0x00200000
#define TYPE_END		0x80000000

#define TYPE_SWITCH (TYPE_NUL | TYPE_HEX | TYPE_DEC | TYPE_MODEL_STR | \
TYPE_PRE | TYPE_HEI | TYPE_AXY | TYPE_ANZ | TYPE_BLR | TYPE_OFF)
#define TYPE_COMMAND (TYPE_CMD | TYPE_DEC)

#define TYPE_MOD_DEC	(TYPE_MOD | TYPE_DEC)
#define TYPE_MOD_HEX	(TYPE_MOD | TYPE_HEX)
#define TYPE_FIX_INT	(TYPE_FIXED | TYPE_DEC)
#define TYPE_FIX_HEX	(TYPE_FIXED | TYPE_HEX)
#define TYPE_MOD_STR	(TYPE_MOD | TYPE_MODEL_STR)
#define TYPE_MOD_CMD	(TYPE_MOD | TYPE_CMD)
#define TYPE_MOD_OFF	(TYPE_MOD | TYPE_OFF)

typedef struct tag_ini_stuff {
	char *pSection;
	char *pKeyWord;
	DWORD dwProfileType;

} _ini_stuff, *p_ini_stuff;

// Command array defines and structure.

// These numbers represent the location where the tablet command will live
// in the command structure.  If new commands are added they can just be added
// to the end of the list.

// These first messages are sent to the hardware at enable time.
// The messages will all be sent in this order starting with zero.

// reset command
#define API_RESET			0

#define API_BITFORMAT		1  // Binary command
#define API_STREAM			2
#define API_EVENOOR 		3
#define API_9600			4
#define API_19200			5

#define API_MEASUREMENT 	6
#define API_FORMAT			7
#define API_RESOLUTION		8
#define API_PORTRAIT		9
#define API_REL_ABS 		10 //places tablet in absolute or relative mode

#define API_PRESSUREON		11
#define API_PRESSURELEVEL	12
#define API_TILT			13
#define API_EXTRADATA		14
#define API_DATAFORMAT		15

// finally, start sending packets
#define API_TRANSMITON		16
#define API_SPACE1			17 //If OEM needs another command
#define API_SPACE2			18 //If OEM needs another command
#define API_SPACE3			19 //If OEM needs another command
#define API_SETUP_LAST		20 //If OEM needs another command

// is this needed?
#define API_RATE			21

// These messages will be sent at disable time
#define API_SHUTDOWN		22
#define API_TRANSMITOFF 	23
#define API_PRESSUREOFF 	24

// Misc messages
#define API_START			25
#define API_STOP			26
#define API_SENDONE 		27
#define API_SPEAKUP 		28
#define API_SHUTUP			29

// The amount of time to pasue after a reset
#define API_PAUSETIME		30

#define API_NUMBER			31


typedef struct tag_commands {
   // This table doesn't accept commands longer then 10 characters.
	char CommandStr[10];
} COMMANDS, *PCOMMANDS;


typedef struct RateTable {
	DWORD dwRate;
	char CommandStr[10];
} RATETABLE, *PRATETABLE;

#endif
