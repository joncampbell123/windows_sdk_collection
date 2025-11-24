;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

;***    IFSMgr service definitions

ifndef  IFSMgr_Device_ID
  IFSMgr_Device_ID equ 00040h
else
  .errnz IFSMgr_Device_ID - 00040h
endif

Begin_Service_Table IFSMgr

IFSMgr_Service  IFSMgr_Get_Version
IFSMgr_Service  IFSMgr_RegisterMount
IFSMgr_Service  IFSMgr_RegisterNet
IFSMgr_Service  IFSMgr_RegisterMailSlot
IFSMgr_Service  IFSMgr_Attach
IFSMgr_Service  IFSMgr_Detach
IFSMgr_Service  IFSMgr_Get_NetTime
IFSMgr_Service  IFSMgr_Get_DOSTime
IFSMgr_Service  IFSMgr_SetupConnection
IFSMgr_Service  IFSMgr_DerefConnection
IFSMgr_Service  IFSMgr_ServerDOSCall
IFSMgr_Service  IFSMgr_CompleteAsync
IFSMgr_Service  IFSMgr_RegisterHeap
IFSMgr_Service  IFSMgr_GetHeap
IFSMgr_Service  IFSMgr_RetHeap
IFSMgr_Service  IFSMgr_CheckHeap
IFSMgr_Service  IFSMgr_CheckHeapItem
IFSMgr_Service  IFSMgr_FillHeapSpare
IFSMgr_Service  IFSMgr_Block
IFSMgr_Service  IFSMgr_Wakeup
IFSMgr_Service  IFSMgr_Yield
IFSMgr_Service  IFSMgr_SchedEvent
IFSMgr_Service  IFSMgr_QueueEvent
IFSMgr_Service  IFSMgr_KillEvent
IFSMgr_Service  IFSMgr_FreeIOReq
IFSMgr_Service  IFSMgr_MakeMailSlot
IFSMgr_Service  IFSMgr_DeleteMailSlot
IFSMgr_Service  IFSMgr_WriteMailSlot
IFSMgr_Service  IFSMgr_PopUp
IFSMgr_Service  IFSMgr_printf
IFSMgr_Service  IFSMgr_AssertFailed
IFSMgr_Service  IFSMgr_LogEntry
IFSMgr_Service  IFSMgr_DebugMenu
IFSMgr_Service  IFSMgr_DebugVars
IFSMgr_Service  IFSMgr_GetDebugString
IFSMgr_Service  IFSMgr_GetDebugHexNum
IFSMgr_Service  IFSMgr_NetFunction
IFSMgr_Service  IFSMgr_DoDelAllUses
IFSMgr_Service  IFSMgr_SetErrString
IFSMgr_Service  IFSMgr_GetErrString
IFSMgr_Service  IFSMgr_SetReqHook
IFSMgr_Service  IFSMgr_SetPathHook
IFSMgr_Service  IFSMgr_UseAdd
IFSMgr_Service  IFSMgr_UseDel
IFSMgr_Service  IFSMgr_InitUseAdd
IFSMgr_Service  IFSMgr_ChangeDir
IFSMgr_Service  IFSMgr_DelAllUses
IFSMgr_Service  IFSMgr_CDROM_Attach
IFSMgr_Service  IFSMgr_CDROM_Detach
IFSMgr_Service  IFSMgr_Win32DupHandle
IFSMgr_Service  IFSMgr_Ring0_FileIO
IFSMgr_Service  IFSMgr_Win32_Get_Ring0_Handle
IFSMgr_Service  IFSMgr_Get_Drive_Info
IFSMgr_Service  IFSMgr_Ring0GetDriveInfo
IFSMgr_Service  IFSMgr_BlockNoEvents
IFSMgr_Service  IFSMgr_NetToDosTime
IFSMgr_Service  IFSMgr_DosToNetTime
IFSMgr_Service  IFSMgr_DosToWin32Time
IFSMgr_Service  IFSMgr_Win32ToDosTime
IFSMgr_Service  IFSMgr_NetToWin32Time
IFSMgr_Service  IFSMgr_Win32ToNetTime
IFSMgr_Service  IFSMgr_MetaMatch
IFSMgr_Service  IFSMgr_TransMatch
IFSMgr_Service  IFSMgr_CallProvider
IFSMgr_Service  UniToBCS
IFSMgr_Service  UniToBCSPath
IFSMgr_Service  BCSToUni
IFSMgr_Service  UniToUpper
IFSMgr_Service  UniCharToOEM
IFSMgr_Service  CreateBasis
IFSMgr_Service  MatchBasisName
IFSMgr_Service  AppendBasisTail
IFSMgr_Service  FcbToShort
IFSMgr_Service  ShortToFcb
IFSMgr_Service  IFSMgr_ParsePath
IFSMgr_Service  Query_PhysLock
IFSMgr_Service  _VolFlush
IFSMgr_Service  NotifyVolumeArrival
IFSMgr_Service  NotifyVolumeRemoval
IFSMgr_Service  QueryVolumeRemoval
IFSMgr_Service  IFSMgr_FSDUnmountCFSD
IFSMgr_Service  IFSMgr_GetConversionTablePtrs
IFSMgr_Service  IFSMgr_CheckAccessConflict
IFSMgr_Service  IFSMgr_LockFile
IFSMgr_Service  IFSMgr_UnlockFile
IFSMgr_Service  IFSMgr_RemoveLocks
IFSMgr_Service  IFSMgr_CheckLocks
IFSMgr_Service  IFSMgr_CountLocks
IFSMgr_Service  IFSMgr_ReassignLockFileInst
IFSMgr_Service  IFSMgr_UnassignLockList
IFSMgr_Service  IFSMgr_MountChildVolume
IFSMgr_Service  IFSMgr_UnmountChildVolume
IFSMgr_Service  IFSMgr_SwapDrives
IFSMgr_Service  IFSMgr_FSDMapFHtoIOREQ
IFSMgr_Service  IFSMgr_FSDParsePath
IFSMgr_Service  IFSMgr_FSDAttachSFT
IFSMgr_Service  IFSMgr_GetTimeZoneBias
IFSMgr_Service  IFSMgr_PNPEvent
IFSMgr_Service  IFSMgr_RegisterCFSD
IFSMgr_Service  IFSMgr_Win32MapExtendedHandleToSFT
IFSMgr_Service  IFSMgr_DbgSetFileHandleLimit
IFSMgr_Service  IFSMgr_Win32MapSFTToExtendedHandle
IFSMgr_Service  IFSMgr_FSDGetCurrentDrive
IFSMgr_Service  IFSMgr_InstallFileSystemApiHook
IFSMgr_Service  IFSMgr_RemoveFileSystemApiHook
IFSMgr_Service  IFSMgr_RunScheduledEvents
IFSMgr_Service	IFSMgr_CheckDelResource
IFSMgr_Service	IFSMgr_Win32GetVMCurdir
IFSMgr_Service	IFSMgr_SetupFailedConnection
IFSMgr_Service	_GetMappedErr
IFSMgr_Service	ShortToLossyFcb
IFSMgr_Service	IFSMgr_GetLockState
IFSMgr_Service	BcsToBcs
IFSMgr_Service	IFSMgr_SetLoopback
IFSMgr_Service	IFSMgr_ClearLoopback
IFSMgr_Service	IFSMgr_ParseOneElement
IFSMgr_Service	BcsToBcsUpper
End_Service_Table IFSMgr


;**     IFSMgr_Get_Version - Get version number
;
;       This service is always valid.
;
; Entry None
; Exit  EAX = version number (3.10 for this release)
;       'C' clear
; Uses  EAX, Flags


IFSMGRVERSION   equ     022h

;**     IFSMgr_RegisterMount - register a filesystem provider
;
;       A filesystem provider calls this service to register a mount
;       routine with the redirector.
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
; Entry TOS = Entrypoint for providers mount routine
;       TOS + 4 = Version of IFSMGR provider was built for
;       TOS + 8 = 1 if default FSD is requesting mount
;               = 0 if normal FSD is requesting mount
; Exit  EAX = Provider ID (-1 if error)
;       caller clears argument from stack
; Uses  C Registers


;**     MountFS - filesystem provider mount routine
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
; Entry TOS = pointer to IOReq
;       ir_flags - 0 (indicates MOUNT)
;       ir_gi.g_ptr - flat address of BDD (from BlockDev)
;       ir_pos - first sector (physical sector # of logical sector 0)
;       ir_drv - drive to mount (0=A:, 1=B:, ...)
;
; Exit  EAX = 0 if success, non-zero if error
;       if success,
;               ir_pr = pointer to provider specific information
;               ir_gi = flat address of entry point table
;                       dword   number of entries in table
;                       dword   NetDelete
;                       dword   NetDir
;                       dword   NetFileAttributes
;                       dword   NetFileInfo
;                       dword   NetFlush
;                       dword   NetGetDiskInfo
;                       dword   NetOpen
;                       dword   NetRename
;                       dword   NetSearch
;                       dword   ShutDown
;
; Uses  C Registers  (EAX, ECX, EDX may be stomped)


;**     IFSMgr_RegisterNet - register a network provider
;
;       A filesystem provider calls this service to register a
;       connect routine with the redirector.
;
;       Versioning:
;       The FSD should first call IFSMgr_Get_Version.  If the
;       version returned by the IFS Manager is older than the
;       FSD is prepared to handle, the FSD should no come up.
;
;       The version number the FSD uses when calling IFSMgr_RegisterNet
;       should be IFSMGRVERSION for the newest version of the IFS Manager
;       that the FSD knows how to handle.  If the IFS Manager does not
;       support that version, it will return an error for this call.
;
; Entry TOS     = Entrypoint for providers connect routine
;       TOS + 4 = FSD version number
; Exit  EAX = Provider ID (-1 if error)
;       caller clears argument from stack
; Uses  C Registers


;l IFSMgr_Get_Version.  If the
;       version returned by the IFS Manager is older than the
;       FSD is prepared to handle, the FSD should no come up.
;
;       The version number the FSD uses when calling IFSMgr_RegisterNet
;       should be IFSMGRVERSION for the newest version of the IFS Manager
;       that the FSD knows how to handle.  If the IFS Manager does not
;       support that version, it will return an error for this call.
;
; Entry TOS     = Entrypoint for providers mailslot handling routine
;       TOS + 4 = FSD version number
; Exit  EAX = Provider ID (-1 if error)
;       caller clears argument from stack
; Uses  C Registers

;**     MailSlotFunction - Handle a mailslot request
;
; Entry TOS     - pointer to IOReq
;                 ir_flags = 0 - Mailslot just openned
;                   ir_options - number of mailslots now openned
;                 ir_flags = 1 - Mailslot closed
;                   ir_options - number of mailslots still openned
;                 ir_flags = 2 - Write remote 2nd class mailslot
;                   ir_ppath - Unicode string containing \MAILSLOT\<path>
;                   ir_ppath2 - 16 character remote server name
;                   ir_length - length of users buffer
;                   ir_options - priority of message
;                   ir_timeout - timeout

;**     Connect - network provider connect routine
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
; Entry TOS = pointer to IOReq


;**     IFSMgr_Attach
;**     IFSMgr_Detach

;**     IFSMgr_InitUseAdd
;**     IFSMgr_UseAdd
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
;       The use_info_2 structure is of the form:
;
;       typedef struct _LM_GUID {
;               unsigned short  guid_uid;         /* LM10 style user id */
;               unsigned long   guid_serial;      /* user record serial number */
;               unsigned char   guid_rsvd[10];    /* pad out to 16 bytes for now */
;       } LM_GUID;
;
;       struct use_info_2 {
; -->           char            ui2_local[DEVLEN+1];
;               char            ui2_pad_1;
; -->           char FAR *      ui2_remote;
; -->           char FAR *      ui2_password;
;               unsigned short  ui2_status;
; -->           short           ui2_asg_type;
;               unsigned short  ui2_refcount;
;               unsigned short  ui2_usecount;
; -->           unsigned short  ui2_res_type;
;               uar FAR *       ui2_remote;
; -->           char FAR *      ui2_password;
;               unsigned short  ui2_status;
; -->           short           ui2_asg_type;
;               unsigned short  ui2_refcount;
;               unsigned short  ui2_usecount;
; -->           unsigned short  ui2_res_type;
;               unsigned short  ui2_flags;
;               unsigned short  ui2_usrclass;
;               void FAR *      ui2_dirname;
;               struct _LM_GUID ui2_dfs_id;
;       };      /* use_info_2 */
;
;       This is the netuse_info form:
;
;       netuse_info {
;               void            *nu_data;
;               int             nu_info; 
;               unsigned long   nu_flags;
;               unsigned long   nu_rsvd;        
;       };      /* netuse_info */

;
; The fields marked with --> should be filled in. All other fields should
; be set to zero except ui2_res_type which should be set to USE_RES_UNC
;
; For IFSMgr_InitUseAdd (connection will be set up disconnected)
; Entry TOS     = pointer to use_info_2 structure (pointers must be flat)
;       TOS + 4 = Provider ID (may be -1 for all providers)
; Exit  EAX     = error code (0 for success)
;
;       The FSD may call this api with a NULL ptr for the IOReq, in this 
;       case a ptr to a filled in netuse_info struct must be passed as 
;       the third parameter.
;
; For IFSMgr_UseAdd
; Entry TOS     = pointer to IOReq
;           ir_data = pointer to use_info_2 structure (pointers must be flat)
;           ir_flags = 1 if connection should be set up disconnected, 0 otherwise
;       TOS + 4 = Provider ID (may be -1 for all providers)
;       TOS + 8 = ptr to netuse_info struct if ptr to IOReq is null otherwise pass null.
; Exit  ir_error set properly (0 for success)
;

;**     IFSMgr_UseDel
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
;       The FSD may call this api with a NULL ptr for the IOReq, in this 
;       case a ptr to a filled in netuse_info struct must be passed as 
;       the third parameter.
;
; Entry TOS     = pointer to IOReq
;           ir_data = pointer to name (A:, B:, ..., LPT1, ..., \\server\share, ...)
;           ir_flags =force level (0-3)
;       TOS + 4 = Provider ID (may be -1 for all providers)
;       TOS + 8 = ptr to netuse_info struct if ptr to IOReq is null otherwise pass null.
; Exit  ir_error = error code (0 for success)


;**     IFSMgr_Get_NetTime - Get current time in net format
;
;       This service is not valid until IFSMgr performs VNSInitComplete.
;
; Entry None
; Exit  EAX = nunber of seconds since 1/1/70
; Uses  C registers  (eax, ecx, edx, flags)


;**     IFSMgr_Get_DOSTime - Get current time in DOS format
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
;       Returns date/time in DOS format.
;       dos format:
;               ___________________________
;               | hours | minutes | seconds|
;               | 5 bit |  6 bits | 5 bits+|
;               |__________________________|
;               ___________________________
;               | years*|  months |  days  |
;               | 7 bit |  4 bits | 5 bits |
;               |__________________________|
;
;       (*) years from 1980
;       (+) 2 seconds units
;
; Entry none
; Exit  eax = high word is date word in dos format
;             low word is time word in dos format
; Uses  C registers  (eax, ecx, edx, flags)


;**     IFSMgr_SetupConnection
;**     IFSMgr_DerefConnection

;**     IFSMgr_ServerDOSCall
;
;       This routine checks to see if a server DOS call is being directed
;       to a drive/handle which is being managed by the redir.  Due to the
;       variety of DOS calls that can be routed through this interface, the
;       client register state is saved and the the DPL info is loaded into
;       the client register structure and the call is routed through the
;       preamble for the requested function.  If the call is not to be
;       intercepted, then the client register state is restored and the
;       call is passed on.  If the call is intercepted, the saved client
;       state is discarded and the call is processed through the normal
;       code paths.
;
;       Entry   ebx = handle of current VM
;               ecx = function number
;               esi = pointer to DPL32 structure
;               ebp = pointer to client registers
;       Exit    'C' clear if request taken
;               'C' set if not.
;       Uses    eax, ebx, ecx, edx, edi, esi, flags


;*      DPL - DOS Parameter List
;
;       Parameter list structure for the "Server DOS Call".

DPL32   STRUC
DPL32_EAX       DD      ?       ; EAX register
DPL32_EBX       DD      ?       ; EBX register
DPL32_ECX       DD      ?       ; ECX register
DPL32_EDX       DD      ?       ; EDX register
DPL32_ESI       DD      ?       ; ESI register
DPL32_EDI       DD      ?       ; EDI register
DPL32_EBP       DD      ?       ; EBP register
DPL32_UID       DW      ?       ; User (Machine) ID (0 = local macine)
DPL32_PID       DW      ?       ; Process ID (0 = local user PID)
DPL32_DTA       DD      ?       ; Flat pointer to DTA (instead of CurDTA)
DPL32_FLAGS     DB      ?       ; Control flags
DPL32_PAD       DB 3 DUP (?)    ; pad to dword boundry
DPL32   ENDS

DPL32W  STRUC
DPL32_AX        DW      ?       ; AX register
                DW      ?
DPL32_BX        DW      ?       ; BX register
                DW      ?
DPL32_CX        DW      ?       ; CX register
                DW      ?
DPL32_DX        DW      ?       ; DX register
                DW      ?
DPL32_SI        DW      ?       ; SI register
                DW      ?
DPL32_DI        DW      ?       ; DI register
                DW      ?
DPL32_BP        DW      ?       ; BP register
                DW      ?
DPL32_PID32     DD      ?       ; 32-bit process ID
DPL32W  ENDS

DPL32B  STRUC
DPL32_AL        DB      ?       ; AL register
DPL32_AH        DB      ?       ; AH register
                DW      ?
DPL32_BL        DB      ?       ; BL register
DPL32_BH        DB      ?       ; BH register
                DW      ?
DPL32_CL        DB      ?       ; CL register
DPL32_CH        DB      ?       ; CH register
                DW      ?
DPL32_DL        DB      ?       ; DL register
DPL32_DH        DB      ?       ; DH register
DPL32B  ENDS

;low 2 bits of DPL32_FLAGS specify character set as follows:

DPL32_USE_ANSI		equ	0
DPL32_USE_OEM		equ	1
DPL32_USE_UNICODE       equ     2

DPL32_8_3_MATCHING      equ     4

;**     IFSMgr_CompleteAsync

;**     IFSMgr_RegisterHeap - Register as a user of the net heap
;
;       This service is not valid until IFSMgr performs SysCriticalInit.
;
;       This call allows a user of the net heap to register a
;       scavenger routine for use by the heap manager.
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
; Entry TOS     - address of scavenger routine (0 if none)
;       TOS+4   - debug flag.  (0 if VNetSup_RetHeap called to free heap)
; Exit  None
; Uses  C registers  (eax, ecx, edx, flags)


;**     IFSMgr_GetHeap - Allocate a chunk of the net heap
;
;       This service is not valid until IFSMgr performs SysCriticalInit.
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
; Entry TOS - Size required
; Exit  EAX - address of heap chunk.  0 if failure
; Uses  C registers  (eax, ecx, edx, flags)


;**     IFSMgr_RetHeap - Free a chunk of net heap
;
;       This service is not valid until IFSMgr performs SysCriticalInit.
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
; Entry TOS - address of heap chunk
; Exit  None
; Uses  C registers  (eax, ecx, edx, flags)


;**     IFSMgr_CheckHeap - check heap consistancy
;
;       This service is not valid until IFSMgr performs SysCriticalInit.
;
;       This routine uses the C6 386 _cdecl calling convention
;
;       NOTE:   On a non-debug build this routine will return 0 without
;               doing anything.
;
; Entry TOS   - DWORD pointer to heap item (0 for none)
;       TOS+4 - DWORD pointer to ASCIIZ filename
;       TOS+8 - DWORD line number
; Exit  0 if no error found, non-zero if problem hit.
; Uses  C registers  (eax, ecx, edx, flags)


;**     IFSMgr_CheckHeapItem - check heap item consistancy
;
;       This service is not valid until IFSMgr performs SysCriticalInit.
;
;       This routine uses the C6 386 _cdecl calling convention
;
;       NOTE:   On a non-debug build this routine will return 0 without
;               doing anything.
;
;       NOTE:   This call is not valid on freed heap items if
;               VNetSup_RetHeap is not used to free the heap item.
;
; Entry TOS   - DWORD pointer to heap item
;       TOS+4 - DWORD pointer to ASCIIZ filename
;       TOS+8 - DWORD line number
;       TOS+12- DWORD   0 if item is expected to be free
;                       1 if item is expected to be busy
;                       2 if don't know
; Exit  0 if no error found, non-zero if problem hit.
; Uses  C registers  (eax, ecx, edx, flags)


;**     IFSMgr_FillHeapSpare
;**     IFSMgr_Block
;**     IFSMgr_Wakeup
;**     IFSMgr_Yield
;**     IFSMgr_SchedEvent
;**     IFSMgr_QueueEvent
;**     IFSMgr_KillEvent
;**     IFSMgr_FreeIOReq

;**     IFSMgr_MakeMailSlot - Make a MailSlot
;
;       A VxD calls this service to make a MailSlot.
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
;       The read procedure is declared as follows:
;               int _cdecl ReadProc(void *pdata, uint len, uchar lana);
;
; Entry TOS   = Procedure to be called for mailslot reads
;       TOS+4 = pointer ASCIIZ mailslot name (without the "\MAILSLOT\")
;       TOS+8 = pointer to INT to receive the mailslot handle
; Exit  EAX = error code, 0 for success
;       caller clears argument from stack
; Uses  C Registers


;**     IFSMgr_DeleteMailSlot - Delete a MailSlot
;
;       A VxD calls this service to delete a MailSlot.
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
; Entry TOS   = mailslot handle received from IFSMgr_MakeMailSlot
; Exit  EAX = error code, 0 for success
;       caller clears argument from stack
; Uses  C Registers

;**     IFSMgr_WriteMailslot
;
;       A VxD calls this service to delete a MailSlot.
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
; Entry TOS     = pointer to ASCIIZ name (without the "\MAILSLOT\"
;       TOS+4   = pointer to data
;       TOS+8   = length of data
;       TOS+12  = network number
;       TOS+16  = pointer to remote name data was received from (or 0 if local)
;                 immediately followed by local name.  both names are 16 bytes long
;       TOS+17  = pointer to transport dependant info
; Exit  none
; Uses  C Registers

;**     IFSMgr_PopUp
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
;       This service is called to put up a popup.
;
; Entry TOS     - VMHandle
;       TOS+4   - Message Box Flags (MB_SystemModal will be set by VNetSup)
;       TOS+8   - Message number
;       TOS+12  - Device ID
; Exit  EAX     - return from SHELL_SYSMODAL_Message
; Uses  C registers  (eax, ecx, edx, flags)


;**     IFSMgr_printf - printf to the debug console
;
;       This service is not valid until IFSMgr performs DeviceInit.
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
;       NOTE:   On a non-debug build this routine will return without
;               doing anything.
;
; Entry TOS   - DWORD pointer to ASCIIZ format string
;       TOS+n - parameters as required
; Exit  None
; Uses  C registers  (eax, ecx, edx, flags)


;**     IFSMgr_AssertFailed - Print an AssertFailed message
;
;       This service is not valid until IFSMgr performs DeviceInit.
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
;       NOTE:   On a non-debug build this routine will return without
;               doing anything.
;
; Entry TOS   - DWORD pointer to ASCIIZ message
;       TOS+4 - DWORD pointer to ASCIIZ filename
;       TOS+8 - DWORD line number
; Exit  None
; Uses  C registers  (eax, ecx, edx, flags)


;**     IFSMgr_LogEntry - Make a log entry in the net trace log
;
;       This service is not valid until IFSMgr performs SysCriticalInit.
;
;       This procedure uses the C6 _cdecl calling sequence
;
;       NOTE:   On a non-debug build this routine will return without
;               doing anything.
;
; Entry TOS    - DWORD Pointer to string of 4 chars to put in log
;       TOS+4  - DWORD Info1 (first dword stored in log)
;       TOS+8  - DWORD Info2 (second dword stored in log)
;       TOS+12 - DWORD Stack Depth for return address (number of procedures)
;       TOS+16 - DWORD Mask (only low word is significant)
; Exit  None
; Uses  C registers  (eax, ecx, edx, flags)


;**     IFSMgr_DebugMenu - print a menu and return addr of handler routine
;
;       This service is not valid until IFSMgr performs SysCriticalInit.
;
;       This routine uses the C6 386 _cdecl calling convention
;
;       NOTE:   On a non-debug build this routine will return 0 without
;               doing anything.
;
;       For 'C' declarations, include vnsmenu.h
;
; Entry TOS   - DWORD pointer to menu table with last item set to 0.
; Exit  EAX   - address of debug routine to call (0 if exit requested)
; Uses  C registers  (eax, ecx, edx, flags)

;**     DebugRoutine - handle a debug display request
;
;       This routine uses the C6 386 _cdecl calling convention
;
;       For 'C' declarations, include vnsmenu.h
;
; Entry TOS   - DWORD pointer to remainder of command line (or 0)
; Exit  EAX   - 0 if debug should continue, non-zero for immediate exit
;               to debugger.
; Uses  C registers  (eax, ecx, edx, flags)

IFSDbgMenu      struc
        IFSM_string     dd ?    ; menu string
        IFSMM_pmh       dd ?    ; address of handler routine
IFSDbgMenu ends


;**     IFSMgr_DebugVars
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
;       This service is called to get the address of IFSMgr debug vars
;
; Entry none
; Exit  EAX     - 0 if non debug build
;                 pointer to VNSDebugVars if debug
; Uses  C registers  (eax, ecx, edx, flags)

;**     IFSMgr_GetDebugString
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
;       NOTE:   On a non-debug build this routine will return without
;               doing anything.
;
; Entry TOS     - pointer to prompt string (without CR/LF)
;       TOS+4   - pointer to buffer
;       TOS+8   - length of buffer
; Exit  EAX     - exit code: 0 if ESC hit
;                            1 if string put in buffer
; Uses  C registers  (eax, ecx, edx, flags)

;**     IFSMgr_GetDebugHexNum
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
;       NOTE:   On a non-debug build this routine will return without
;               doing anything.
;
; Entry TOS     - pointer to prompt string (without CR/LF)
;               - pointer to dword to receive hex value
; Exit  EAX     - exit code: 0 if ESC hit
;                            1 if string put in buffer
; Uses  C registers  (eax, ecx, edx, flags)


;**     IFSMgr_NetFunction
;
;       This routine is used by the ifsmgr to export int21 func 5f calls
;       and int2f function 11 calls that are not supported directly by the
;       ifsmgr.  To handle these calls, an FSD should hook this service
;       and process the requests.  If the FSD doesn't understand a request
;       it should pass the call on to the original address.
;       A FSD gets a hook ID by calling IFSMgr_GetReqHookID.  A custom preamble
;       (set by calling IFSMgr_SetReqHook) puts the hook ID in ESI.  This hook
;       ID is then passed to NetFunction.
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
; Entry TOS     - ptr to ioreq structure
;       TOS + 4 - ptr to user register structure
;       TOS + 8 - hook ID (0 for all FSDs)
; Exit  none
;
; Uses  C registers  (eax, ecx, edx, flags)

;**     IFSMgr_DoDelAllUses
;
;       This service is called by a net FSD to remove all redirections to
;       drives managed by that FSD.
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
; Entry TOS     - ptr to ioreq structure
; Exit  none
;
; Uses  C registers  (eax, ecx, edx, flags)

;**     IFSMgr_SetErrString
;
;       This routine accepts error strings from VxDs and stores
;       them for use when handling Int2F function 5 (GetErrorText).
;       It also handle strings for use with IFSMgr_Popup
;
;       NOTE:   The memory containing the error message must be left
;               in memory.  The IFS Manager keeps pointers to the
;               strings rather than copying them.
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
; Entry TOS     - ptr to ASCIIZ error message
;       TOS + 4 - Error code (only the low word used)
;       TOS + 8 - table indicator, 0 = error code table, 1 = device name table
; Exit  EAX     - error code
;                       NOTICE: ERROR_ACCESS_DENIED means the error code
;                               is already in the table)


;**     IFSMgr_GetErrString
;
;       Given an error code, this routine will return a pointer to
;       an error message if it is in the table.
;
; Entry TOS     - Error code (only the low word used)
;       TOS + 4 - table indicator, 0 = error code table, 1 = device name table
; Exit  EAX     - pointer to error message (0 if not available)


;**     IFSMgr_SetReqHook - Set a custom preamble on the IFSManager Int21 Hook
;
;       FSDs that need to handle Int21 Functions not normally handled by
;       the IFSManager can use IFSMgr_SetReqHook to hook those functions.
;       The FSD supplies a function number and the address of a preamble
;       routine.  When the IFSManager detects an Int21 with the given function
;       number, it will route it to the preamble to see if the request should
;       be hooked.
;
;       This service returns the address of the old preamble.  If the preamble
;       does not hook the request, it must jump the old preamble.
;
;       The entry/exit conditions for preambles are:
;       Entry   EBX     - Handle of VM
;               ECX     - Function (value of users AH)
;               EBP     - Client register structure
;               ESI     - 0
;       Exit if request is to be hooked
;               Carry Clear
;               EBX     - Handle of VM
;               EBP     - Client register structure
;               ESI     - 0 or FSD ID
;       Exit if request is not to be hooked
;               jump to the old preamble with the entry conditions preserved
;       Only EBX and EBP need to be preserved, ESI must be set properly
;
; Entry TOS     - High word = Interrupt level (must be 21h)
;                 Low word = Function number (value to look for in AH)
;       TOS + 4 - Entry point of preamble
; Exit  EAX     - 0 if error
;                 Previous preamble routine (preambles must chain) if success

;**     IFSMgr_SetPathHook - Set a hook to identify custom path prefixs
;
;       FSDs that look for special path prefixs to locate remote resources
;       can use IFSMgr_SetPathHook to give IFSMgr the entrypoint of a routine
;       that can identify a special path prefix.
;
;       The entry/exit conditions for the path checker are:
;       Entry   ESI     - ASCIIZ path   (NOTE: ESI must be preserved)
;       Exit    Path recognized - return with:
;                       'C' clear - Path recognized
;                       EAX - length of portion of path that is specific to FSD
;                       EDX - Provider ID of FSD claiming path
;                       'Z' - Set if device check should be skipped in preamble
;                             Clear if device check should be performed in preamble
;               Path not recognized - pass request it on next path checker.
;       Uses    EAX, EBC, ECX, EDX, EDI
;
; Entry TOS     - Entry point of path checker
; Exit  EAX     - 0 if error
;                 Previous path checker (path checkers must chain) if success


;**     IFSMgr_ChangeDir - Change Directory
;
;       FSDs that need to cause a change the current path on a given drive
;
;       Entry   TOS - Pointer to IOReq
;                       ir_data - pointer to ASCIIZ path string
;                       ir_drv - set to default drive (needed if path string
;                                does not contain drive letter)
;               TOS+4 - VM to change directory for.  0 for current VM.
;       Exit    ir_error set appropriately


;**     IFSMgr_CDROM_Attach - Special service for CDFS to force a mount on a
;       drive that is passed in. The VRP for the volume that is mounted on
;       the drive is returned to the caller of the service. The caller can
;       use this VRP to pass i/o requests down to the IO subsystem. The CDFS
;       needs to handle direct ioctls from DOS applications on driveletters 
;       for which it has no volume parameters at all. If a volume is already
;       mounted on the drive, the IFS manager returns the VRP for that volume
;       without forcing a mount. Each call to attach increments the drive 
;       useage count.
;
;       NOTE: This service works only under Chicago. It just returns success
;       under Snowball.
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
;       The entry/exit conditions for this service are:
; Entry TOS     - Driveletter (0-based)
;       TOS + 4 - Pointer to dword to contain the VRP pointer
;
; Exit  EAX     - 0 if success. VRP pointer updated in variable.
;               - Errorcode if failure.
;
; Uses  C registers  (eax, ecx, edx, flags)


;**     IFSMgr_CDROM_Detach - Special service provided to the CDFS to indicate
;       that it does not need to use the drive anymore. The drive useage count
;       is decremented for every detach operation. If the drive useage count
;       goes down to zero, the volume resource associated with the drive is 
;       automatically freed.
;
;       NOTE: This service works only under Chicago.
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
;       The entry/exit conditions for this service are:
; Entry TOS     - Driveletter (0-based)
;       TOS + 4 - VRP pointer of drive returned on IFSMgr_CDROM_Attach
;
; Exit  None
;
; Uses  C registers  (eax, ecx, edx, flags)

;**     IFSMgr_Win32DupHandle - This service is provided for the Win32 VxD 
;       to do an inter-process dup. This service works for both extended 
;       handles and normal DOS handles.
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
;	int Win32DupHandle(pid_t srcpid, pid_t duppid, ulong *phandle, 
;					uchar flag,ulong *fReturnFlags )
;
; Entry TOS     = process id of process issuing dup (must be current process)
;       TOS+4   = process id of process handle is to be dup'ed into
;       TOS+8   = ptr handle to be dup'ed (returns dup'ed handle)
;       TOS+12  = flag to indicate kind of dup - normal or for memory-mapping
;	TOS+16	= global Netware handle if running on real-mode NETX
;	TOS+20	= ptr to variable to contain return flags (defined below)
;
; Exit  EAX     = 0 if success
;			TOS+8 contains new handle
;			TOS+16 contains return flags
;               = errorcode if failure
;
; Uses  C registers  (eax, ecx, edx, flags)

; Return flag values for Win32DupHandle

WDUP_RMM_DRIVE		equ	0001h	; file handle mapped to RMM drive
WDUP_NETWARE_HANDLE	equ	0002h	; file handle belongs to Netware


;**     IFSMgr_Get_Drive_Info - this service returns information about a
;       given volume.
;
; Entry
;       al      = volume (A=0)
;
; Exit
;   (carry clear)
;       eax     = flags
;               FDRV_INT13      ; volume lives on an int13 drive
;               FDRV_FASTDISK   ; volume is on a drive for which a
;                               ; fastdisk driver is present
;               FDRV_COMP       ; compression driver detected on this volume
;               FDRV_RMM        ; volume can be accessed through RMM
;               FDRV_USE_RMM    ; volume must be accessed through RMM
;               FDRV_DOS        ; DBP exists for this volume... DOS
;                               ; knows about it
;               FDRV_COMPHOST   ; host volume for compression driver (dblspace)
;               FDRV_N

;**     Query_PhysLock - service to determined if current process has the
;                        int13 physical lock.
;
; Entry
;       eax     = int 13 unit number (0-7) or (80h-87h)
;
; Exit
;       NC if the current process has the int 13 lock
;       CY if the current process does not have the int 13 lock
;
; Uses ALL except ebx, edi
;

;**     _VolFlush - volume based flush/invalidate 
;
; Entry TOS     = logical unit number to flush/invalidate
;       TOS+4   = flags
;                 0                     ; just flush the cache
;                 VOL_DISCARD_CACHE     ; flush and invalidate the cache
;                 VOL_REMOUNT           ; flush and invalidate the cache
;					; & remount the vol (recompute
;					; the FAT "geometry" variables)
;
; Exit  (eax) = non-zero if error on flush/invalidate
;
; Uses  C registers  (eax, ecx, edx, flags)
;

;**     NotifyVolumeArrival - called when a new volume appears in the system.
;       Updates the CDSs in all the VMs and updates IFSMGR data structs to
;       indicate this is a new supported volume
;
; Entry TOS = logical unit number for new volume
;
; Exit  None
;
; Uses  eax, ecx, edx, flags
;

;**     NotifyVolumeRemoval - called when a new volume is removed.
;       Updates the CDSs in all the VMs and updates IFSMGR data structs to
;       indicate this is no longer a supported volume
;
; Entry TOS = logical unit number for new volume
;
; Exit  None
;
; Uses  eax, ecx, edx, flags
;

;**     QueryVolumeRemoval - called when a new volume is to be removed.
;       Checks if there are any open files on he volume. If yes closes them
;       after querying the user. Then unmounts the FSD. 
;
; Entry TOS = logical unit number for new volume
;
; Exit  (eax) = 0 if successfull non-zero otherwise
;
; Uses  eax, ecx, edx, flags
;

;**     IFSMgr_FSDUnmountCFSD - called by a CFSD to unmount its resource. The
;       CFSD passes in the volume handle previously passed to it on its mount
;       and is called back to do the actual unmount operation. 
;
; Entry TOS = IFS manager volume handle
;
; Exit  (eax) = 0 if successfull non-zero otherwise
;
; Uses  C registers (eax, ecx, edx, flags)


;**     IFSMgr_GetConversionTablePtrs - called to obtain a ptr to a table
;       of conversion tables ptrs within the IFSmgr.  The ptrs reference
;       AVL-tree-based run-length encoded structs for converting 
;               UNICODE->ANSI
;               UNICODE->OEM
;               ANSI->UNICODE
;               OEM->UNICODE
;               UNICODE->UPPER (delta table)
;               UNICODE->UPPER (conversion table)
;       These tables are referenced by the routines MapUniToBCS and
;       MapBCSToUni UniToUpper implemented in unicode.asm.
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
; Entry nothing
;
; Exit  ptr to following structure
;               dword   count of ptrs in following table
;               dword   flat-ptr to UniToWinAnsiTable (ring-0 data)
;               dword   flat-ptr to UniToOemTable (ring-0 data)
;               dword   flat-ptr WinAnsiToUniTable (ring-0 data)
;               dword   flat-ptr to OemToUniTable (ring-0 data)
;               dword   flat-ptr to UniToUpperDelta (ring-0 data)
;               dword   flat-ptr to UniToUpperTable (ring-0 data)
; Uses  eax
;


;;* See Ifs.h for function prototypes for:
;
;       IFSMgr_NetToDosTime
;       IFSMgr_DosToNetTime
;       IFSMgr_DosToWin32Time
;       IFSMgr_Win32ToDosTime
;       IFSMgr_NetToWin32Time
;       IFSMgr_Win32ToNetTime
;       IFSMgr_MetaMatch
;       IFSMgr_TransMatch
;       UniToBCS
;       UniToBCSPath
;       BCSToUni
;       UniToUpper


;**     IFSMgr_CheckAccessConflict - check if desired access can be granted
;
;       This function validates the desired access mode and
;       determines if the desired access to the file can be
;       granted.
;
;       int IFSMgr_CheckAccessConflict (fSoftCompatDisable, pf, pfn, cookie)
;
;       PARAMETERS
;               softCompatDisable
;                       - IN boolean which defeats "soft" share
;                         semantics if they are enabled.  The "soft"
;                         share semantics are in effect IFF the
;                         fSoftCompat flag is TRUE.
;                         Non-zero specifies "hard" share rules.
;               pf      - IN pointer to file mode descriptor
;                         specifying desired access
;               pfn     - IN pointer to enumeration function
;               cookie  - IN cookie for enumeration function
;       RETURN
;               0       - desired mode is compatable
;               (other) - error code


;**     IFSMgr_LockFile - lock a range of bytes in a file
;
;       This function is called to lock a range of bytes in
;       a file.  Locks may not overlap; a lock may not be of
;       of zero length.
;
;       int IFSMgr_LockFile (pp, offmin, cb, owner, fileinst)
;
;       PARAMETERS
;               pp                      - IN pointer to file cookie
;               offmin          - IN starting file offset of the lock
;               cb                      - IN length of the locked region
;               owner           - IN owner of the lock
;               fileinst        - IN open instance to which lock belongs
;               pp                      - OUT file cookie filled in
;       RETURN
;               0                       - Success
;               (other)         - error code


;**     IFSMgr_UnlockFile - unlock a range of bytes in a file
;
;       This function is called to unlock a range of bytes in
;       a file.  Locks may not overlap; a lock may not be of
;       of zero length; a lock may not extend beyond the
;       maximum file size; an unlock must match a lock exactly.
;
;       int IFSMgr_UnlockFile (pp, offmin, cb, owner, fileinst)
;
;       PARAMETERS
;               pp                      - IN pointer to file cookie
;               offmin          - IN starting file offset of the lock
;               cb                      - IN length of the locked region
;               owner           - IN owner of the lock
;               fileinst        - IN open file instance
;               pp                      - OUT file cookie possibly set to NULL
;       RETURN
;               0                       - Success
;               (other)         - error code


;**     IFSMgr_RemoveLocks - remove all locks for a given owner
;
;       This function is called to remove all locks from a file
;       for a specified owner.
;
;       If the special owner id 0 is passed in, then all locks on
;       the file are removed.
;
;       If the file instance is NULL, then locks are removed for
;       all instances.  So, to remove all locks, pass in owner 0
;       and file instance NULL.
;       
;       void IFSMgr_RemoveLocks (pp, owner, fileinst)
;
;       PARAMETERS
;               pp                      - IN pointer to file cookie
;               owner           - IN owner of the lock
;               fileinst        - IN open file instance
;               pp                      - OUT file cookie possibly set to NULL
;       RETURN
;               NONE


;**     IFSMgr_CheckLocks - check if an i/o operation violates a lock
;
;       This function is called to check if i/o over a
;       specified range would violate any locks on the file.
;       A range length of zero is interpreted to mean the
;       file is going to be truncated at the given offset;
;       any locks at or above the given offset will result in a
;       lock violation.
;
;       int IFSMgr_CheckLocks (p, offmin, cb, owner, fileinst)
;
;       PARAMETERS
;               p                       - file cookie
;               offmin          - starting file offset of the lock
;               cb                      - length of the locked region
;               owner           - owner of the lock
;               fileinst        - open file instance
;       RETURN
;               0                       - Success
;               (other)         - error code


;**     IFSMgr_CountLocks - count locks for a given file instance
;
;       This function is called to count all locks on a file
;       for a specified instance.
;
;       int IFSMgr_CountLocks (p, fileinst)
;
;       PARAMETERS
;               p                       - IN file cookie
;               fileinst        - IN open file instance
;       RETURN
;               int                     - count of locks for instance


;**     IFSMgr_ReassignLockInst - reassign file instances for existing lock list
;
;       This function is called to replace existing 'file instance' fields
;       in the lock list with a new 'file instance'.  This is done upon exit
;       from a level 4 exclusive lock when the IFS manager re-opens existing
;       files.  Typically, the FSD (at least VFAT) will create a new 'file
;       instance' (i.e. SFT) upon opening a file.  The IFS has saved the
;       'file instance' that was previously associated with the file and
;       passes it to the FSD when the open is done.  The FSD, after
;       obtaining the new file instance value, calls this service to
;       replace old file instances in the lock list with the new file
;       instance.  The owner of the lock must be marked as 'inactive' before
;       this substitution will occur.  After the substitution is made, the
;       'inactive' mark is removed from the owner.  This prevents erroneous
;       substitution is cases where the old file instance may happen to match
;       a new file instance that was previously substituted into the lock list.
;
;       int IFSMgr_ReassignLockFileInst (plkList, oldFileInst, newFileInst)
;
;       PARAMETERS
;               plkList         - IN pointer to lock list
;               oldFileInst     - IN previous open file instance
;               newFileInst     - IN new open file instance
;       RETURN
;               0                       - Success


;**     IFSMgr_UnassignLockList - mark all owners in this list 'inactive'
;
;       int IFSMgr_UnassignLockFileInst (plkList)
;
;       PARAMETERS
;               plkList         - IN pointer to lock list
;       RETURN
;               0                       - Success


;**     IFSMgr_MountChildVolume - mount a child volume
;
;       A child volume is a volume that is physically related to
;       some other volume, known as the parent.  For example, some
;       on-the-fly data compression schemes represent the compressed
;       volume as a file on an uncompressed volume.  In this example,
;       the uncompressed volume is the parent, and the compressed
;       volume is the child.
;
;       int IFSMgr_MountChildVolume (pvrp, driveno)
;
;       PARAMETERS
;               pvrp            - IN child volume VRP pointer
;               driveno         - IN child volume drive number (0 = A:, ...)
;       RETURN
;               0                       - Success
;               (other)         - error code


;**     IFSMgr_UnmountChildVolume - unmount a child volume
;
;       This service is provided to allow the unmounting of a child
;       volume on a specified drive.
;
;       We unmount the specified volume on the specified drive and
;       destroy the resource.  Why do we go to all this trouble?
;       For removable child volumes which are mounted automatically,
;       it is important to make the set of related volumes (i.e.,
;       the parent and its children) behave as a unit so that the
;       physical media volume tracks properly.  Thus, under normal
;       circumstances, the notification that a volume is going away
;       causes all of its relations to disappear as well.  This is
;       exactly what we *don't* want to happen in the case where
;       the user wishes to destroy one volume in a set.  By killing
;       the target here, before the volume removal notication is
;       received, we insure that there will be no way for the
;       volume removal to affect the other related volumes.  Sounds
;       complicated?  You bet it is!
;
;       int IFSMgr_UnmountChildVolume (pvrp, driveno)
;
;       PARAMETERS
;               pvrp            - IN child volume VRP pointer
;               driveno         - IN child volume drive number (0 = A:, ...)
;       RETURN
;               0                       - Success
;               (other)         - error code


;**     IFSMgr_SwapDrives - swap a parent and child drive
;
;       This service is provided to allow the drive letter
;       of a child volume and its parent to be swapped.
;
;       int IFSMgr_SwapDrives (pvrp, drive1, drive2)
;
;       PARAMETERS
;               pvrp            - IN VRP pointer
;               drive1          - IN first drive number (0 = A:, ...)
;               drive2          - IN second drive number (0 = A:, ...)
;       RETURN
;               0                       - Success
;               (other)         - error code


;**     IFSMgr_PNPEvent - report PNP events
;
;       int IFSMgr_PNPEvent( dword msg, dword res, dword flags )
;
;       PARAMETERS
;               msg             - IN one of:
;                                       DBT_DEVICEARRIVAL
;                                       DBT_DEVICEQUERYREMOVE
;                                       DBT_DEVICEQUERYREMOVEFAILED
;                                       DBT_DEVICEREMOVEPENDING
;                                       DBT_DEVICEREMOVECOMPLETE
;                                       DBT_DEVICETYPESPECIFIC
;                                       DBT_DEVICEDEVNODESCHANGED
;
;               volid           - IN Volume ID (A=0) r resource handle
;               flags           - IN
;                                   one of PNPT_x to specify type:
;                                       PNPT_VOLUME
;                                       PNPT_NET
;                                   'or'd with 0 or more:
;                                       (if PNPT_VOLUME)
;                                       DBTF_MEDIA
;                                       DBTF_NET
;                                       (if PNPT_NET)
;                                       DBTF_RESOURCE
;                                       DBTF_XPORT
;                                       
;       RETURN
;               one of PNPE_x

;**     IFSMgr_RegisterCFSD - register a mountable filesystem provider
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
; Entry TOS     = Entrypoint for providers mount routine
;       TOS + 4 = Version of IFSMGR provider was built for
;       TOS + 8 = pointer to device name strings( in unicode) registered
;
; Exit  Provider ID (-1 if error)
;
; Uses  C Registers

;**     IFSMgr_Win32MapExtendedHandleToSFT - map an extended handle to a SFT
;
;       This service allocates a free SFT and maps an extended handle to this
;       SFT. It returns the index to the SFT back to the caller. This api is
;       for the purpose of redirection for Win32 apps when they spawn DOS apps.
;       The SFT and the extended handle track one another from this point on.
;       This service MUST be called in the context of the DOS VM the SFT is
;       going to be in.
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
;       int IFSMgr_Win32MapExtendedHandleToSFT( ulong ExtHandle, ushort *psfn );
;
; Entry TOS     = Extended handle to be mapped
;       TOS + 4 = PSP extended handle is on
;       TOS + 8 = Pointer to variable to contain the SFN
;
; Exit  EAX = 0 if no error, *psfn contains the SFN
;       EAX = non-zero errorcode if error
;
; Uses  C Registers

;**     IFSMgr_DbgSetFileHandleLimit - set limit for extended handles.
;
;       This procedure sets the maximum number of extended handles available
;       in the system. The limit is rounded up to the nearest multiple of 256.
;       This is a debug-only service and is provided mainly for testing
;       purposes to exhaust the available handles quicker. On a retail build,
;       it returns without doing anything.
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
;       int IFSMgr_DbgSetFileHandleLimit( unsigned long MaxHandles );
;
; Entry TOS     = Handle limit to be set
;
; Exit  EAX = -1 if the limit passed in was invalid
;           = 0 if the limit was successfully set
;
; Uses  C Registers

;**     IFSMgr_Win32MapSFTToExtendedHandle - map a SFT to an extended handle
;
;       This service maps an already existing SFT handle to an extended
;       handle. This service will work only for SFT handles that are on
;       drives supported by the IFS manager.
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
;       int IFSMgr_Win32MapSFTToExtendedHandle( ushort DOSHandle, ulong *pExtHandle );
;
; Entry TOS     = DOS handle to be mapped
;       TOS + 4 = Pointer to variable to contain the extended handle
;
; Exit  EAX = 0 if no error, *pExtHandle contains the extended handle
;       EAX = non-zero errorcode if error
;
; Uses  C Registers

;**     IFSMgr_InstallFileSystemApiHook - install a filesystem api hook
;
;       This service installs a filesystem api hook for the caller. This
;       hook is between the IFS manager and a FSD. So, the hooker gets to see
;       any calls that the IFS manager makes to FSDs.
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
;       ppIFSFileHookFunc IFSMgr_InstallFileSystemApiHook( pIFSFileHookFunc HookFunc )
;
; Entry TOS     = Address of function that is to be installed as the hook
;
; Exit  EAX = Pointer to variable containing the address of the previous
;               hooker in this chain.
;
; Uses  C registers

;**     IFSMgr_RemoveFileSystemApiHook - remove previously installed api hook
;
;       This service removes a previously installed filesystem api hook. The
;       hook can be removed dynamically at any time even if there have been
;       other hookers installed on top of this hook.
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
;       int IFSMgr_RemoveFileSystemApiHook( pIFSFileHookFunc HookFunc )
;
; Entry TOS     = Address of function that is to be installed as the hook
;
; Exit  EAX = 0 if success, non-zero for failure (invalid hook function)
;
; Uses  C registers

;**     IFSMgr_RunScheduledEvents - run events if any have been scheduled
;
;       Checks to see if any scheduled global events are waiting and performs
;       a nested ResumeExec if so.
;
; Entry nothing
;
; Exit  carry set if no events were found, clear otherwise
;

;**	IFSMgr_CheckDelResource - check/delete all connections for resource.
;
;	This service deletes all active connections on the requested shell
;	resource. A force level is passed in so that the appropriate checks are
;	made before blowing the resource away. The FSD is called via its
;	disconnect entry point to disconnect the resource. There is also a flag
;	passed in so that this service can be called only to check if the
;	resource can be successfully deleted without really deleting it.
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
;	int IFSMgr_CheckDelResource( pshres psr, int force, int fDelFlag );	
;
; Entry	TOS	= pointer to IFS resource handle passed in on connect
;	TOS+4	= force level to use (0-4)
;	TOS+8	= delete flag
;			non-zero = delete resource
;			0 = just check for deletion
;
; Exit		Returns 0 if success, errorcode if failure

;**	IFSMgr_Win32GetVMCurdir - return current directory for given drive
;
;	This service returns the current directory path for a given drive in the
; 	current VM context. If the drive is owned by the IFS, we use our IfsCDS
; 	structure for the current directory. Otherwise, we look in the real-mode
; 	CDS and extract the current directory for the drive.
;
;       This procedure uses the C6 386 _cdecl calling sequence
;
;	int IFSMgr_Win32GetVMCurdir( int wDrive, void far *pCurdirBuf );
;
; Entry	TOS = 0-based drive number
;	TOS+4 = ptr to buffer to contain curdir
;
; Exit	EAX = 0 if successful
;	  pCurdirBuf contains curdir. Note that the path returned does not have
;	  "D:\" i.e. a curdir of root will be a null path. The path is always
;	  returned in the OEM character set for DOS consistency reasons.
;	EAX = errorcode, if failure

;**	IFSMgr_SetupFailedConnection - api call out on the connect operation
;
;	This service is used by the ifs manager to export a connect operation
;	when it has been failed by all registered net providers. Any VxD can
;	hook this service and then establish a fake connection.
;
;	This procedure uses the C6 386 _cdecl calling sequence
;
;	int IFSMgr_SetupFailedConnection( pioreq pir, unsigned long *ProId );
;
; Entry	TOS = pointer to ioreq structure(filled in for connect operation)
;	TOS+4 = pointer to variable to contain provider id if connection done
;
; Exit	EAX = 0 if successful, errorcode if failure.

;**     _GetMappedErr - returns the mapped error from the ext. error
;
; Entry TOS     = int21 function number
;       TOS+4   = Extended Error Code
;
; Exit  (eax) = Mapped error code
;
; Uses  C registers  (eax, ecx, edx, flags)
;


;***	IFSMgr_GetLockState - get lock state for specified drive
;
;	This service is provided to allow file system clients
;	(e.g., Compressed Volume Managers) to query if the
;	specified drive is locked.
;
;	This service uses the C calling convention.
;
;	ENTRY	driveno - volume drive number (0 = A:, ...)
;		pdwtype - pointer to DWORD in which to return type
;		pdwflags - pointer to DWORD in which to return flags
;		pdwowner - pointer to DWORD in which to return owning thread
;
;		NULL may be passed in for any of the above pointer
;		values if the caller does not wish to see the
;		corresponding return value.
;
;	RETURN	int - error code (0 if successful)
;
;	USES	EAX, ECX, EDX, Flags

;**	IFSMgr_Win32_Get_Ring0_Handle
;
;	This routine returns a ring 0 handle for a given extended handle. There
;	are certain conditions under which the ring 0 handle is not returned
;	because we must get an int 21h to send the i/o request down as follows:
;		1. This handle is not an extended handle.
;		2. We are in Win31 filesystem mode.
;		3. There is an app in the VM that has hooked int 21h.
;	If none of these conditions are triggered, the ring 0 handle is
;	returned for the next read/write operation. This service also returns
;	the current position of the file since the caller does not have any
;	idea of the current file position.
;
;	Entry	EBX = extended handle
;
;	Exit	CY clear --> handle mapped successfully,
;			EBX = ring 0 handle
;			EDX = current file position
;		CY set 	 --> handle was not mapped
;			EBX is preserved in this case
;
;	Uses	EAX, ECX

;**	IFSMgr_SetLoopback
;
;	This routine is intended to be called by a server to add a loopback
;	path to ifsmgr's loopback path list.
;
;	This routine uses the __cdecl calling convention.
;
;	ENTRY	pszUNCPath   - ansi string of the form \\<server>\<share>
;		pszLocalPath - ansi string of the form <drive>:<path>
;
;	RETURN	nothing
;
;	USES	EAX, ECX, EDX, Flags

;**	IFSMgr_ClearLoopback
;
;	This routine is intended to be called by a server to remove a
;	loopback path from ifsmgr's loopback path list.
;
;	This routine uses the __cdecl calling convention.
;
;	ENTRY	pszUNCPath   - ansi string of the form \\<server>\<share>
;
;	RETURN	nothing

;**	IFSMgr_ParseOneElement
;
;	This routine is intended to be called on a single path element i.e 
;	a filename or directory name, path separators are not allowed. It
;	parses this name and returns the parse flags for it in the
;	FILE_FLAG_* format defined in ifs.h. Callers can use this service to
;	determine whether the name passed in is a shortname or LFN.
;
;	This routine uses the __cdecl calling convention
;
;	int IFSMgr_ParseOneElement( string_t pstr );
;
;	Entry	pstr = ptr to path element in unicode format
;
;	Exit	EAX = parse flags, if no error the following flags are returned:
;			FILE_FLAG_IS_LFN = path element is LFN
;			FILE_FLAG_HAS_STAR = path element contains '*' wildcard
;			FILE_FLAG_WILDCARDS = path element contains wildcards
;			FILE_FLAG_HAS_DOT = path element contains a dot char
;		EAX = -1 if there was a parse error
;
;	Uses	C registers

