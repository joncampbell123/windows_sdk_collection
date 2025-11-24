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

/*****************************************************************************
 *
 *  Title:      SHELL.H - Public services for SHELL
 *
 *  Version:    4.00
 *
 *****************************************************************************/

#ifndef _SHELL_H
#define _SHELL_H

#ifndef Not_VxD

/*XLATOFF*/
#define SHELL_Service   Declare_Service
#pragma warning (disable:4003)      // turn off not enough params warning
/*XLATON*/

/*MACROS*/
Begin_Service_Table(SHELL)

SHELL_Service   (SHELL_Get_Version, LOCAL)
SHELL_Service   (SHELL_Resolve_Contention, LOCAL)
SHELL_Service   (SHELL_Event, LOCAL)
SHELL_Service   (SHELL_SYSMODAL_Message, LOCAL)
SHELL_Service   (SHELL_Message, LOCAL)
/*ENDMACROS*/

#ifndef WIN30COMPAT
/*
 *  Following specific to SHELL version >= 3.10
 */
/*MACROS*/
SHELL_Service   (SHELL_GetVMInfo, LOCAL)
/*ENDMACROS*/

#endif  // WIN30COMPAT

#ifndef WIN31COMPAT
/*
 *  Following specific to SHELL version >= 4.00
 */
/*MACROS*/
SHELL_Service   (_SHELL_PostMessage, LOCAL)
SHELL_Service   (_SHELL_ShellExecute)
SHELL_Service   (_SHELL_PostShellMessage)
SHELL_Service   (SHELL_DispatchRing0AppyEvents, LOCAL)  /* INIT ONLY */
SHELL_Service   (SHELL_Hook_Properties, LOCAL)
SHELL_Service   (SHELL_Unhook_Properties, LOCAL)
SHELL_Service   (SHELL_Update_User_Activity, LOCAL)
SHELL_Service   (_SHELL_QueryAppyTimeAvailable, LOCAL)
SHELL_Service   (_SHELL_CallAtAppyTime, LOCAL)
SHELL_Service   (_SHELL_CancelAppyTimeEvent)
SHELL_Service   (_SHELL_BroadcastSystemMessage, LOCAL)
SHELL_Service   (_SHELL_HookSystemBroadcast, LOCAL)
SHELL_Service   (_SHELL_UnhookSystemBroadcast, LOCAL)
SHELL_Service   (_SHELL_LocalAllocEx, LOCAL)
SHELL_Service   (_SHELL_LocalFree, LOCAL)
SHELL_Service   (_SHELL_LoadLibrary, LOCAL)
SHELL_Service   (_SHELL_FreeLibrary, LOCAL)
SHELL_Service   (_SHELL_GetProcAddress, LOCAL)
SHELL_Service   (_SHELL_CallDll, LOCAL)
SHELL_Service	(_SHELL_SuggestSingleMSDOSMode)
SHELL_Service	(SHELL_CheckHotkeyAllowed)
SHELL_Service	(_SHELL_GetDOSAppInfo)
/*ENDMACROS*/

#endif  // WIN31COMPAT

/*MACROS*/
End_Service_Table(SHELL)
/*ENDMACROS*/

#ifndef WIN31COMPAT
// HookSystemBroadCast call order definitions
#define SBH_CALL_ORDER_DEFAULT      0
#define SBH_CALL_ORDER_REGISTRY     0x30000000
#define SBH_CALL_ORDER_CONFIGMG     0x40000000

#endif  // WIN31COMPAT

//  BeginDoc
//
//  Windows Message Box Flags used on SHELL_SYSMODAL_Message and
//  SHELL_Message services
//
#define MB_OK                   0x00
#define MB_OKCANCEL             0x01
#define MB_ABORTRETRYIGNORE     0x02
#define MB_YESNOCANCEL          0x03
#define MB_YESNO                0x04
#define MB_RETRYCANCEL          0x05
#define MB_ICONHAND             0x10
#define MB_ICONEXCLAMATION      0x30
#define MB_ICONASTERISK         0x40
#define MB_DEFBUTTON1           0x00
#define MB_DEFBUTTON2           0x100
#define MB_DEFBUTTON3           0x200
#define MB_APPLMODAL            0x00
#define MB_SYSTEMMODAL          0x1000
#define MB_NOFOCUS              0x8000
/*
 *  Following bits are WIN386 SHELL extensions
 */
#define MB_ASAP                 0x80000000  // Message is timely, put
                                            //  up as soon as possible
/*
 * The following flag bits are 3.10 extensions
 */
#define MB_NOWINDOW             0x40000000  // DO NOT switch to Windows
                                            // to do this message, do it
                                            // in full screen mode always.
                                            // ONLY FOR SHELL_SYSMODAL_Message

#define MB_HANGSYS              0x20000000  // Hang with ints disabled.
                                            // This is for especially
                                            // severe errors (like parity
                                            // errors).
                                            // ONLY FOR SHELL_SYSMODAL_Message

#ifndef WIN31COMPAT

/*
 * The following flag bits are 4.0 extensions
 */

/*
 * MB_APPYTIME, if passed to SHELL_Sysmodal_Message, means that the service
 * is being called at 'Appy-time and that a GUI dialog box should be attempted.
 *
 * It is an error to pass the bit to SHELL_Message.
 */
#define MB_APPYTIME             0x10000000

#endif /* WIN31COMPAT */

/*
 *  Windows Message Box Return codes used on SHELL_SYSMODAL_Message and
 *  SHELL_Message services
 */
#define IDOK                    1
#define IDCANCEL                2
#define IDABORT                 3
#define IDRETRY                 4
#define IDIGNORE                5
#define IDYES                   6
#define IDNO                    7

//  EndDoc

/*
 *  On SHELL_Event, the wParam in EAX occupies the low 16 bits of EAX. The High 16
 *  bits contain extra flags
 */
#define SE_WP_PrtScBoost        0x00100000      // Boost the SYS VM during Print screen
#define SE_WP_PrtScBoostBit     20              //
                                                //
#define SE_WP_DispUpdBoost      0x00200000      // Boost the SYS VM during updates
#define SE_WP_DispUpdBoostBit   21

#ifndef WIN31COMPAT
/*
 *  Scheduling values for SHELL_PostMessage.
 *
 *  These bits go into the upper word of the uMsg argument.
 *
 *  All bits not explicitly mentioned are reserved and must be zero.
 */
#define SPM_UM_DoNotWaitForCrit     0x00020000
#define SPM_UM_DoNotWaitForCritBit  17

#define SPM_UM_AlwaysSchedule       0x00080000
#define SPM_UM_AlwaysScheduleBit    19

#endif /* WIN31COMPAT */

//  BeginDoc

/*
 *  Bits of returned EAX flags for SHELL_GetVMInfo service
 */
#define SGVMI_Windowed    0x00000004            // Is Windowed
#define SGVMI_ALTTABdis   0x00000020            // Alt+Tab is reserved
#define SGVMI_ALTESCdis   0x00000040            // Alt+Esc is reserved
#define SGVMI_ALTSPACEdis 0x00000080            // Alt+Space is reserved
#define SGVMI_ALTENTERdis 0x00000100            // Alt+Enter is reserved
#define SGVMI_ALTPRTSCdis 0x00000200            // Alt+PrtSc is reserved
#define SGVMI_PRTSCdis    0x00000400            // PrtSc is reserved
#define SGVMI_CTRLESCdis  0x00000800            // Ctrl+Esc is reserved
#define SGVMI_HasHotKey   0x00004000            // Has a shortcut key
#define SGVMI_Polling     0x00001000            // Polling detection Enab
#define SGVMI_FastPaste   0x00020000            // Allow Fast paste Enab
#define SGVMI_NoHMA       0x00002000            // No HMA
#define SGVMI_XMS_Lock    0x00010000            // XMS Hands Locked
#define SGVMI_EMS_Lock    0x00008000            // EMS Hands Locked
#define SGVMI_V86_Lock    0x00040000            // V86 Memory Locked
#define SGVMI_ClsExit     0x40000000            // Close on Exit Enab

/*
 *  Bits of returned ECX flags for SHELL_GetVMInfo service
 *  Currently no bits defined.
 */

#endif /* Not_VxD */
//  EndDoc

#ifndef WIN31COMPAT

#ifndef Not_VxD

/* Local Memory Flags */
#define LMEM_FIXED          0x0000
#define LMEM_ZEROINIT       0x0040

#define LPTR                (LMEM_FIXED | LMEM_ZEROINIT)

#define LMEM_STRING     0x00010000
#define LMEM_OEM2ANSI   0x00020000

/* Flags returned by SHELL_QueryAppyTimeAvailable */
#define QAAFL_APPYAVAIL	0x00000001	/* 'Appy-time server exists */
#define QAAFL_APPYNOW	0x00000002	/* It is now 'Appy-time */

/* Flags for SHELL_CallAtAppyTime */
#define CAAFL_RING0     0x00000001 /* Does not require GUI services */
#define CAAFL_TIMEOUT   0x00000002 /* Time out the event */

#endif /* Not_VxD */

/******************************************************************************
 *
 * Structure used for SHELL_ShellExecute, q.v.
 *
 *****************************************************************************/

/* H2INCSWITCHES -t */

typedef struct _SHEXPACKET {    /* shex */

    DWORD   dwTotalSize;/* Size of packet, including all baggage */
    DWORD   dwSize;     /* Size of SHEXPACKET */
    DWORD   ibOp;       /* Operation to perform */
    DWORD   ibFile;     /* File to open */
    DWORD   ibParams;   /* Optional parameters */
    DWORD   ibDir;      /* Directory in which to launch */
    DWORD   dwReserved;	/* Must be zero */
    DWORD   nCmdShow;   /* How to show the window */

/*  char    rgchBaggage[]; */	/* Arbitrary baggage appended here */

} SHEXPACKET;

/* H2INCSWITCHES -t- */




/* EQUATES FOR _SHELL_GetDOSAppInfo */
// Function IDs
#define SGDAIF_COMPARENAME  0
#define SGDAIF_TESTAPPBIT   1
#define SGDAIF_LASTVALID    1	    // Used internally


// Return values
#define SGDAIR_SUCCESS	    0
#define SGDAIR_NOMATCH	    1
#define SGDAIR_BADFUNCTION  2



#endif /* WIN31COMPAT */

/*XLATOFF*/

#ifndef Not_VxD

typedef DWORD   APPY_HANDLE;
typedef VOID    (* __cdecl APPY_CALLBACK)(DWORD dwRefData);

#define BSM_ERROR_SOMEONE_FAILED    0
#define BSM_ERROR_SUCCESS       1
#define BSM_ERROR_OUT_OF_MEMORY     0xFFFFFFFF

typedef DWORD   SYSBHOOK_HANDLE;
typedef BOOL    (* __cdecl SYSBHOOK_CALLBACK)(DWORD wMessage, DWORD wParam, DWORD lParam, DWORD dwRefData);

#pragma warning (disable:4035)      // turn off no return code warning

DWORD VXDINLINE
SHELL_SYSMODAL_Message(HVM hvm, DWORD dwMBFlags, PCHAR pszMessage, PCHAR pszCaption)
{
    _asm    mov ebx, [hvm]
    _asm    mov eax, [dwMBFlags]
    _asm    or  eax, MB_SYSTEMMODAL
    _asm    mov ecx, [pszMessage]
    _asm    mov edi, [pszCaption]
    VxDCall(SHELL_SYSMODAL_Message);
}

#ifndef WANTVXDWRAPS

#ifndef WIN31COMPAT

APPY_HANDLE VXDINLINE
SHELL_CallAtAppyTime(APPY_CALLBACK pfnAppyCallBack, DWORD dwRefData, DWORD flAppy, ...)
{
    _asm    push    flAppy[4]
    _asm    push    flAppy
    _asm    push    dwRefData
    _asm    push    pfnAppyCallBack
    Touch_Register(eax)
    Touch_Register(ecx)
    Touch_Register(edx)
    VxDCall(_SHELL_CallAtAppyTime);
    _asm    add esp, 4*4
}

VOID VXDINLINE
SHELL_CancelAppyTimeEvent(APPY_HANDLE hAppy)
{
    _asm    push    hAppy
    Touch_Register(eax)
    Touch_Register(ecx)
    Touch_Register(edx)
    VxDCall(_SHELL_CancelAppyTimeEvent);
    _asm    add esp, 4
}

BOOL VXDINLINE
SHELL_QueryAppyTimeAvailable(VOID)
{
    Touch_Register(ecx)
    Touch_Register(edx)
    VxDCall(_SHELL_QueryAppyTimeAvailable)
}

DWORD VXDINLINE
SHELL_LocalAllocEx(DWORD fl, DWORD cb, PVOID lpvBuf)
{
    _asm    push    lpvBuf
    _asm    push    cb
    _asm    push    fl
    Touch_Register(eax)
    Touch_Register(ecx)
    Touch_Register(edx)
    VxDCall(_SHELL_LocalAllocEx);
    _asm    add esp, 3*4
}

DWORD VXDINLINE
SHELL_LocalFree(DWORD hdata)
{
    _asm    push    hdata
    Touch_Register(eax)
    Touch_Register(ecx)
    Touch_Register(edx)
    VxDCall(_SHELL_LocalFree);
    _asm    add esp, 1*4
}

DWORD VXDINLINE
SHELL_CallDll(PCHAR lpszDll, PCHAR lpszProcName, DWORD cbArgs, PVOID lpvArgs)
{
    _asm    push    lpvArgs
    _asm    push    cbArgs
    _asm    push    lpszProcName
    _asm    push    lpszDll
    Touch_Register(eax)
    Touch_Register(ecx)
    Touch_Register(edx)
    VxDCall(_SHELL_CallDll);
    _asm    add esp, 4*4
}

DWORD VXDINLINE
SHELL_BroadcastSystemMessage(DWORD dwFlags, PDWORD lpdwRecipients, DWORD uMsg, DWORD wparam, DWORD lparam)
{
    _asm    push    lparam
    _asm    push    wparam
    _asm    push    uMsg
    _asm    push    lpdwRecipients
    _asm    push    dwFlags
    Touch_Register(eax)
    Touch_Register(ecx)
    Touch_Register(edx)
    VxDCall(_SHELL_BroadcastSystemMessage);
    _asm    add esp, 5*4
}

SYSBHOOK_HANDLE VXDINLINE
SHELL_HookSystemBroadcast(SYSBHOOK_CALLBACK pfnSysBHookCallBack, DWORD dwRefData, DWORD dwCallOrder)
{
        _asm    push    dwCallOrder
        _asm    push    dwRefData
        _asm    push    pfnSysBHookCallBack
        Touch_Register(eax)
    Touch_Register(ecx)
    Touch_Register(edx)
        VxDCall(_SHELL_HookSystemBroadcast);
        _asm    add     esp, 3*4
}

VOID VXDINLINE
SHELL_UnhookSystemBroadcast(SYSBHOOK_HANDLE SysBHookHandle)
{
        _asm    push    SysBHookHandle
        Touch_Register(eax)
    Touch_Register(ecx)
    Touch_Register(edx)
        VxDCall(_SHELL_UnhookSystemBroadcast);
        _asm    add     esp, 1*4
}

#endif // WIN31COMPAT

#endif // WANTVXDWRAPS

#pragma warning (default:4035)      // turn on no return code warning

#endif // Not_VxD

/*XLATON*/

#endif // _SHELL_H
