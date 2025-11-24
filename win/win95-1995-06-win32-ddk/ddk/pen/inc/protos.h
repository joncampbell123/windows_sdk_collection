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

// Prototype list for routines that are shared between files ------------------
// TimerCB.c
DWORD cCall_Installable_Driver(DWORD dwVMHandle,DWORD dwClientRegStruc,DWORD dwInfo);
void cTimer_Callback_Entry_Point(void);

//APIProc.c
DWORD cVpenD_API_Proc(DWORD dwMsg, DWORD lParam1, DWORD lParam2);

#ifdef DEBUG
// Dbg_util.c
void Dbg_Hardware(P_HARDWAREINFO pHwInfo);
void Dbg_PenInfo(LPDRV_PENINFO pPenInfo);
void Dbg_PenPacket(LPDRV_PENPACKET pPP);
void Dbg_ActiveDevice(P_VpenD_Register pAD);
void Dbg_DumpByteBuffer(P_HARDWAREINFO pHwInfo, PBYTE pBuffer);
#endif

//APIProcs.c
DWORD cVpenD_API_Load(DWORD lParam1,DWORD lParam2);
DWORD cVpenD_API_Register(DWORD lParam1,DWORD lParam2);
DWORD cVpenD_API_Free(DWORD lParam1,DWORD lParam2);
DWORD cVpenD_API_Enable(DWORD lParam1,DWORD lParam2);
DWORD cVpenD_API_Disable(DWORD lParam1,DWORD lParam2);

// initutil.c
DWORD cLoad_VpenD(DWORD lParam1);
void cVpenD_Device_Load(void);

// hwint.c
//void _cdecl cVpenD_Hw_Int(void);
DWORD cVpenD_Hw_Int(void);

//Miscutil.c
DWORD cConvertToFlat(DWORD dwptr);
void cVpenD_Device_Exit(void);
DWORD cVpenD_Unregister(DWORD lParam1,DWORD lParam2);


// OEM public routines --------------------------------------------------------
// oemhwint.c
DWORD cOEM_Hw_Int(LPDRV_PENPACKET lppp);
DWORD Wacom_Build_Pen_Packet(LPDRV_PENPACKET lppp);
DWORD Wacom_Build_Pen_Packet_UD(LPDRV_PENPACKET lppp);
DWORD Grid_Build_Pen_Packet(LPDRV_PENPACKET lppp);


// OEMAPI.c
DWORD cOEM_API_Proc(DWORD dwMsg, DWORD lParam1, DWORD lParam2);

//OEMInit.c
DWORD OEM_API_PIInit(DWORD lParam1,DWORD lParam2);
DWORD OEM_API_HWInit(DWORD lParam1,DWORD lParam2);
DWORD OEM_API_CommandInit(DWORD lParam1);

//End-Of-File
