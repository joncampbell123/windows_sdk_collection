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

;***	VRedir service definitions

VRedir_Device_ID	equ	0481h

Begin_Service_Table VRedir

VRedir_Service	VRedir_Get_Version, LOCAL
VRedir_Service	VRedir_Register, LOCAL
VRedir_Service	VRedir_MakeMailSlot, LOCAL
VRedir_Service	VRedir_DeleteMailSlot, LOCAL
VRedir_Service	VRedir_ServerEnum, LOCAL
End_Service_Table VRedir


;**	VRedir_Get_Version - Get version number
;
;	The version number returned during development will be bumped
;	any time something in the VRedir service interface changes.
;	When we ship, it will get set to something like 30Ah.
;
; Entry	None
; Exit	EAX = version number (3.10 for this release)
;	'C' clear
; Uses	EAX, Flags


;;VRDVERSION	equ	030Ah
VRDVERSION	equ	0004h	; Sub versions while building

;**	VRedir_Register - register a filesystem provider
;
;	A filesystem provider calls this service to register a mount
;	routine with the redirector.
;
;	This procedure uses the C6 386 _cdecl calling sequence
;
; Entry	TOS = Entrypoint for providers mount routine
; Exit	EAX = error code, 0 for success
;	caller clears argument from stack
; Uses	C Registers


;**	MountFS - filesystem provider mount routine
;
;	This procedure uses the C6 386 _cdecl calling sequence
;
; Entry	TOS = pointer to IOReq
;	ir_flags - 0 (indicates MOUNT)
;	ir_gi.g_ptr - flat address of BDD (from BlockDev)
;	ir_pos - first sector (physical sector # of logical sector 0)
;	ir_drv - drive to mount (0=A:, 1=B:, ...)
;
; Exit	EAX = 0 if success, non-zero if error
;	if success,
;		ir_pr = pointer to provider specific information
;		ir_gi = flat address of entry point table
;			dword	number of entries in table
;			dword	NetDelete
;			dword	NetDir
;			dword	NetFileAttributes
;			dword	NetFileInfo
;			dword	NetFlush
;			dword	NetGetDiskInfo
;			dword	NetOpen
;			dword	NetRename
;			dword	NetSearch
;			dword	ShutDown
;
; Uses	C Registers  (EAX, ECX, EDX may be stomped)


;**	VRedir_MakeMailSlot - Make a MailSlot
;
;	A VxD calls this service to make a MailSlot.
;
;	This procedure uses the C6 386 _cdecl calling sequence
;
;	The read procedure is declared as follows:
;		int _cdecl ReadProc(void *pdata, uint len, uchar lana);
;
; Entry	TOS   = Procedure to be called for mailslot reads
;	TOS+4 = pointer ASCIIZ mailslot name (without the "\MAILSLOT\")
;	TOS+8 = pointer to INT to receive the mailslot handle
; Exit	EAX = error code, 0 for success
;	caller clears argument from stack
; Uses	C Registers


;**	VRedir_DeleteMailSlot - Delete a MailSlot
;
;	A VxD calls this service to delete a MailSlot.
;
;	This procedure uses the C6 386 _cdecl calling sequence
;
; Entry	TOS   = mailslot handle received from VRedir_MakeMailSlot
; Exit	EAX = error code, 0 for success
;	caller clears argument from stack
; Uses	C Registers


;**	VRedir_ServerEnum - Remote a ServerEnum call
;
;	The browser VxD calls this service to make a ServerEnum
;	call to a remote server.
;
;	struct separms {
;		char 	*se_domains;	// Must be set to zero
;		ulong	se_type;	// Service type
;		ushort 	*se_totent;	// flat pointer to total entries var
;		ushort 	*se_entread;	// flat pointer to entries read var
;		ushort	se_buflen;	// length of buffer
;		char 	*se_buf;	// flat pointer to buffer
;		short	se_level;	// 0 or 1 valid
;		char 	*se_server;	// ASCIIZ of the form \\SERVERNAME
;	};	/* separms */
;
;	This procedure uses the C6 386 _cdecl calling sequence
;
; Entry	TOS   = pointer to separms struct
;	TOS+4 = lana number
;
; Exit	EAX = error code, 0 for success
;	caller clears argument from stack
; Uses	C Registers


