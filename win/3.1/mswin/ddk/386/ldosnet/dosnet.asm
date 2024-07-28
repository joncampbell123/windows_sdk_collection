PAGE 58,132
;******************************************************************************
TITLE DOSNET.ASM
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1988-1990
;
;   Title:	DOSNET.ASM - DOS NETWORK INT 21 Call manager device
;
;   Version:	2.01
;
;   Date:	09-Jun-1989
;
;   Author:	ARR
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   09-Jun-1989 ARR Move stuff from DOSMGR device
;
;==============================================================================

	.386p

;******************************************************************************
;
;  What this device does is manage DOS network drive connections in such a way
;	as to prevent the DOS internal drive state information structures
;	from containing corrupt (stale) information. It also prevents the
;	data structure state in the REDIRector from getting similarly corrupted.
;
;
;      o CONNECTIONS in effect when WIN386 starts are GLOBAL connections which
;	    cannot be broken while WIN386 is running.
;
;	      The DOS CDS structures are part of the "instance snapshot" which
;	      is restored ehrn WIN386 exits. If we allowed GLOBAL network
;	      connections to be broken or changed under WIN386 we would
;	      "restore" a connection which no longer exists (or is different)
;	      and corrupt the CDS structures.
;
;      o CONNECTIONS in effect in the SYSTEM VM when a new VM is created
;	    are INHERITED by the new VM. INHERITED connections cannot be
;	    broken in any VM including the SYS VM.
;
;	      When VMs are created, the SHELL device calls the
;	      DOSMGR_Copy_VM_Drive_State service so that new VMs have the same
;	      drive state as the current SYS VM drive state.
;
;	      The DOS REDIRector is driven by reference counts on its
;	      internal "connection" structures. These reference counts are
;	      not updated when VMs are created and destroyed. In order to
;	      prevent "corruption" of the REDIRector data structures,
;	      trying to delete one of these INHERITED connections is prevented.
;
;      o CONNECTIONS made by a particular VM are LOCAL to that VM because
;	    the DOS CDS structures, where the connection is indicated, are
;	    LOCAL to each VM.
;
;	      The DOS REDIRector is a GLOBAL object however. In order to
;	      prevent corrupting the REDIRectors data structures when a VM
;	      terminates the DOSNET device "net use /d"s any LOCAL uses
;	      that the VM has performed.
;
;      o Windows version 3 has a message called WM_FILESYSCHANGE which is
;	    sent when the filesystem is changed, this includes changes made
;	    in VMs other than the SYS VM. We do not want to send
;	    WM_FILESYSCHANGE messages on LOCAL network drives because Windows
;	    doesn't know anything about these drives.
;
;      o UNC uses (drive resource uses which are not associated with any
;	    drive) do not need to be managed by DOSNET. They behave GLOBALY.
;
;      o Printer uses do not need to be managed by DOSNET. They behave GLOBALY.
;
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;
; This DOSNET device DOES NOT HAVE one of these pieces. This is because it
;    doesn't need one. The interaction of applications with DOS REDIR networks
;    is via the standard DOS INT 21H calls (5Fh) and the mapping of them for
;    Protected Mode applications is handled by the DOSMGR device. In the case
;    of a network that has made extensions to the DOS INT 21H, or other API,
;    interfaces (such as the NOVELL network) this piece would be needed and
;    the DOSNET device is a logical place to put it (although it could be in
;    a separate device instead).
;
;      o API mapper to map APIs specific to this network from Protected Mode
;	    applications. Consult V86MMGR documentation and the VNETBIOS device
;	    sources.
;
;* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;
;******************************************************************************

;******************************************************************************
;			      I N C L U D E S
;******************************************************************************

	INCLUDE VMM.Inc
	INCLUDE Debug.Inc
	INCLUDE DNLocal.Inc
	INCLUDE shell.inc

	Create_DOSNET_Service_Table EQU TRUE

	INCLUDE DOSNET.Inc

;******************************************************************************

Declare_Virtual_Device DOSNET, 1, 0, DOSNET_Sys_Control, DOSNET_Device_ID, \
		       DOSNET_Init_Order

;******************************************************************************
;				 E X T E R N A L S
;******************************************************************************


;
VxD_CODE_SEG

VxD_CODE_ENDS

VxD_ICODE_SEG

VxD_ICODE_ENDS


VxD_DATA_SEG

Extrn	DNNetUseGlobalMsg:byte
Extrn	DNNetUseGlobalMsgDrive:byte
Extrn	DNNetUseInherMsg:byte
Extrn	DNNetUseInherMsgDrive:byte
Extrn	DNNetUseParentMsg:byte
Extrn	DNNetUseParentMsgDrive:byte
Extrn	DNDontDestroyMsg:byte
Extrn	DNDontDestroyMsgDrive:byte

ALIGN 4

;
; Control block offset of DOSNET section
;
	public DN_CB_Offset
DN_CB_Offset		dd	?

;
; Flag that indicates that the origin of this "NET USE /D" call is the DOSNET
;    device itself.
;
NetUseCleanUpFlg	db	0

VxD_DATA_ENDS


VxD_IDATA_SEG

VxD_IDATA_ENDS


VxD_ICODE_SEG

;******************************************************************************
;
;   NAME:
;	DOSNET_Sys_Critical_Init
;
;   DESCRIPTION:
;	Allocate the control block buffer
;
;   ENTRY:
;	EBX = VM1 handle
;
;   EXIT:
;       CF set if error
;
;   USES:
;	EAX,ECX,EDX,Flags
;
;------------------------------------------------------------------------------

BeginProc DOSNET_Sys_Critical_Init

	VMMCall _Allocate_Device_CB_Area, <<SIZE DN_Control_Blk_Struc>, 0>
	test	eax, eax
	jnz	SHORT DN_CB_OK
	Debug_Out "DOSNET ERROR:  Could not alloc control block data area space"
	VMMcall Fatal_Memory_Error

DN_CB_OK:
	mov	[DN_CB_Offset], eax
	clc
	ret

EndProc DOSNET_Sys_Critical_Init



;******************************************************************************
;
;   DOSNET_Device_Init
;
;   DESCRIPTION:
;	Now we can simulate interrupts into the system VM. We will set
;	up the GLOBAL uses information and set up the INT 21 hook.
;
;   ENTRY:
;	EBX = SYS_VM_Handle
;
;   EXIT:
;	Carry set if problem
;
;   USES:
;	EAX,ECX,EDX,ESI,EDI,Flags
;
;==============================================================================

BeginProc DOSNET_Device_Init

    ;
    ; Install the DOS system call hook.
    ;
    ; NOTE: We only need to hook the V86 call. This indirectly hooks the
    ;	    Protected Mode call as well because after the API mapping of
    ;	    the INT 21 is performed, the INT 21 is reflected into V86
    ;	    mode which will trip this hook. One might tend to think:
    ;	    "Wouldn't it be more efficient to hook the PM INT 21 as well?"
    ;	    But there are some subtle interactions that would be harder to
    ;	    handle if it was done this way:
    ;
    ;	      The PM hook would get tripped, then the V86 hook would get
    ;		tripped. The V86 hook would have to "interact and exclude"
    ;		with the PM hook to prevent them from interfereing with each
    ;		other (hooking the same call twice, once in PM and once in
    ;		V86).
    ;
    ;	      The error simulation on failure of NET USE /D is done by actually
    ;		reflecting the call into DOS (in V86 mode) after "adjusting"
    ;		the parameters to cause an error. The PM hook would have to
    ;		reflect into V86 mode anyway to get this behavior.
    ;
	mov	eax, 21h
	mov	esi, offset32 DOSNET_Int_21
	VMMcall Hook_V86_Int_Chain
    ;
    ; Init the global Net Use list in the SYS VM
    ;
	VMMCall _Allocate_Temp_V86_Data_Area,<256,0>

	or	eax,eax
IFDEF DEBUG
	jnz	short DDID20
	debug_out "_Allocate_Temp_V86_Data_Area failed DOSNET_Device_Init"
DDID20:
ENDIF
	jz	DDIDone
	mov	esi,eax
	mov	edi,eax
	shr	edi,4			; EDI = segment

	VMMCall Get_SYS_VM_Handle

	mov	edx,ebx
	add	edx,[DN_CB_Offset]

	Push_Client_State

	VMMCall Begin_Nest_Exec

	mov	ebp, [ebx.CB_Client_Pointer]
	mov	[ebp.Client_DS], di
	mov	[ebp.Client_ES], di
	mov	[ebp.Client_SI], 0
	mov	[ebp.Client_DI], 128
	xor	ecx,ecx
DDIGetRedirLp:
	mov	[ebp.Client_BX], cx
	mov	[ebp.Client_AX], 5F02h
	mov	eax, 21h

	VMMCall Exec_Int

	test	[ebp.Client_EFLAGS],CF_Mask
	jnz	short DDIGetRedirDone
	cmp	[ebp.Client_BL], 4
	jnz	short DDISkipUse	; Ignore non-drive uses
	mov	eax,dword ptr [esi]
	or	al,al
	jz	short DDISkipUse	; Ignore UNC uses
	cmp	ah,':'
	jnz	short DDISkipUse	; Ignore non-drive uses
	or	al,20h
	sub	al,'a'
	jc	short DDISkipUse	; Ignore non-drive uses
	cmp	al,26
	jae	short DDISkipUse	; Ignore non-drive uses
	movzx	eax,al
	mov	[edx.DN_VM_LocalNetUseFlgs][eax],DN_LocUse_F_Global + DN_LocUse_F_InUse
DDISkipUse:
	inc	ecx
	jmp	short DDIGetRedirLp

DDIGetRedirDone:

	VMMCall End_Nest_Exec

	Pop_Client_State

	VMMCall _Free_Temp_V86_Data_Area

DDIDone:
	clc
	ret

EndProc DOSNET_Device_Init

VxD_ICODE_ENDS


VxD_CODE_SEG

BeginDoc
;******************************************************************************
;
;   DOSNET_Get_Version
;
;   DESCRIPTION:
;	Return DOSNET device version
;
;   ENTRY:
;	None
;
;   EXIT:
;	EAX = Version, Major in AH, Minor in AL
;	Carry clear
;
;   USES:
;	Flags,EAX
;
;==============================================================================
EndDoc

BeginProc DOSNET_Get_Version, SERVICE

	mov	eax,0300h
	clc
	ret

EndProc DOSNET_Get_Version

BeginDoc
;******************************************************************************
;
;   DOSNET_Do_PSP_Adjust
;
;   DESCRIPTION:
;	This service allows the DOSMGR device to ask whether it should perform
;	adjustments to try and initially give each VM in the system a different
;	starting DOS PSP address. This needs to be done on networks (such as
;	MSNET) that use the PSP address as part of an ID to uniquely identify
;	different processes talking to the SERVER (this effects the behavior
;	of DOS SHARE on the SERVER end). On a network which uses DOS PSP
;	addresses as part of an ID, WIN386 can cause the ID to be non-unique
;	since thare are now multiple VMs which all can have an app in them
;	which has the same PSP address. If the PSP adjust is enabled by this
;	service, the DOSMGR device causes each VM to start at a different
;	paragraph address (VMID is the basis of the adjustment value) and
;	thus have a different PSP address. This has the cost of wasting some
;	memory with the benifit of making the PSP addresses different in all
;	the VMs.
;
;	For a network which does not use DOS PSP addresses for anything, or
;	one which is WIN386 aware and uses the Get VMID INT 2F service of
;	WIN386 to deal with this problem, a return of Carry SET, EAX !=0
;	is appropriate.
;
;	NOTE: That the uniqueness of PSPs is NOT GUARANTEED by this. It will
;	      deal with the case on most configurations, but on some it will
;	      not. The only absolutely correct solution is to make the network
;	      software WIN386 aware and work the VMID into the network ID,
;	      in addition to the PSP address, by using the Get VMID INT 2F
;	      service of WIN386.
;
;   ENTRY:
;	None
;
;   EXIT:
;	Carry Set
;	    DO NOT do PSP adjustment
;	    EAX == 0 if SYSTEM.INI override of this is allowed
;	    EAX != 0 if SYSTEM.INI override of this is NOT allowed
;	Carry Clear
;	    DO PSP adjustment
;	    EAX == 0 if SYSTEM.INI override of this is allowed
;	    EAX != 0 if SYSTEM.INI override of this is NOT allowed
;
;   NOTE: The behavior of DOSMGR if the DOSNET device is not loaded is a
;	  return of carry SET, EAX = 0.
;
;   USES:
;	Flags,EAX
;
;==============================================================================
EndDoc

BeginProc DOSNET_Do_PSP_Adjust, SERVICE

	xor	eax,eax 	; SYSTEM.INI override allowed (also clears carry)
;;;	   clc			    ; DO PSP adjust
	ret

EndProc DOSNET_Do_PSP_Adjust

BeginDoc
;******************************************************************************
;
;   DOSNET_Send_FILESYSCHANGE
;
;   DESCRIPTION:
;	It is incorrect to send the WM_FILESYSCHANGE message to the Windows
;	shell on drives which Windows knows nothing about. This routine
;	tells the caller whether this is a "local to this VM" drive or
;	not. The WM_FILESYSCHANGE message allows Windows applications
;	to be told when a change is made to the file volume name space so
;	that they can update portions of their display which may be showing
;	that part of the file volume.
;
;	Sending a WM_FILESYSCHANGE on a drive incorrectly can result in
;	all sorts of misbehavior. If the indicated drive letter is invalid
;	in the SYS VM, the Windows app may get an error it is not expecting.
;	If the indicated drive letter actually mapps a different file system
;	volume in the SYS VM, all sorts of other unexpected errors may occur.
;
;   NOTE: This is a DOSNET device service and cannot be implemented directly
;	  in another device. Another device that also wishes to effect the
;	  WM_FILESYSCHANGE behavior must hook this service. Note also that
;	  the DOSNET device does not install if there is no REDIR, so a device
;	  which wants to hook this service must ship with a modified DOSNET
;	  device which ALWAYS loads (real mode init code removed).
;
;   ENTRY:
;	EAX (AL) = Drive number (0 = A)
;	EBX is VM Handle of VM involved
;
;   EXIT:
;	Carry Set
;	    WM_FILESYSCHANGE should NOT be sent on this drive
;	Carry Clear
;	    WM_FILESYSCHANGE SHOULD be sent on this drive
;
;   USES:
;	FLAGS
;
;==============================================================================
EndDoc

BeginProc DOSNET_Send_FILESYSCHANGE, SERVICE

	push	ebx

	Assert_VM_Handle ebx

	VMMCall Test_SYS_VM_Handle

	clc
	je	short DNSFSCDone	; Always should on SYS VM
	cmp	al,26
	jae	short DNSFSCDone	; Carry clear if jump
	add	ebx,[DN_CB_Offset]
	test	[ebx.DN_VM_LocalNetUseFlgs][eax],DN_LocUse_F_InUse
	jz	short DNSFSCDone	; Carry clear if jump
	test	[ebx.DN_VM_LocalNetUseFlgs][eax],DN_LocUse_F_Global + DN_LocUse_F_Inherit
	jnz	short DNSFSCDone	; Carry clear if jump
	stc				; Drive is a local use drive
DNSFSCDone:
	pop	ebx
	ret

EndProc DOSNET_Send_FILESYSCHANGE


;******************************************************************************
;
;   DOSNET_Sys_Control
;
;   DESCRIPTION:
;	Dispatch routine for the sys control calls
;
;   ENTRY:
;	Call specific
;
;   EXIT:
;	Call specific
;
;   USES:
;	Call specific
;
;==============================================================================

Begin_Control_Dispatch DOSNET_Sys

	Control_Dispatch Sys_Critical_Init, DOSNET_Sys_Critical_Init
	Control_Dispatch Device_Init,	    DOSNET_Device_Init
	Control_Dispatch Destroy_VM,	    DOSNET_Destroy_VM
	Control_Dispatch VM_Terminate,	    DOSNET_VM_Terminate
	Control_Dispatch Sys_VM_Terminate,  DOSNET_VM_Terminate
	Control_Dispatch Create_VM,	    DOSNET_Create_VM
	Control_Dispatch Query_Destroy,     DOSNET_Query_Destroy
IFDEF DEBUG
	Control_Dispatch Debug_Query,	    DOSNET_Dump_Debug_Info
ENDIF

End_Control_Dispatch DOSNET_Sys


;******************************************************************************
;
;   DOSNET_Query_Destroy
;
;   DESCRIPTION:
;	OK to destroy running VM? Answer NO if any local uses outstanding.
;
;   ENTRY:
;	EBX = VM Handle
;
;   EXIT:
;	Carry Set if not OK (outstanding net uses)
;
;   USES:
;	FLAGS,EAX,EDX,ECX
;
;==============================================================================

BeginProc DOSNET_Query_Destroy

	mov	edx,ebx
	add	edx,[DN_CB_Offset]
	mov	ecx,25
DNQDChkLp:
	test	[edx.DN_VM_LocalNetUseFlgs][ecx],DN_LocUse_F_InUse
	jz	short DNQDChkSkip	; Not net
	test	[edx.DN_VM_LocalNetUseFlgs][ecx],DN_LocUse_F_Global
	jnz	short DNQDChkSkip	; Is global, OK
	test	[edx.DN_VM_LocalNetUseFlgs][ecx],DN_LocUse_F_Inherit
	jnz	short DNQDChkSkip	; Is Inherit, OK
    ;
    ; ECX drive is a local use.
    ;
	mov	al,cl
	add	al,'A'
	mov	ecx,offset32 DNDontDestroyMsg
	mov	[DNDontDestroyMsgDrive],al
	xor	edi,edi 		; Standard caption
	xor	esi,esi 		; No call back
	mov	eax,MB_OK + MB_ICONEXCLAMATION + MB_SYSTEMMODAL

	VMMCall Test_SYS_VM_Handle

	jz	short DNQD_Modal

	VxdCall SHELL_Message

	jmp	short DNQD_MsgDone

DNQD_Modal:
	debug_out "Odd case DOSNET_Query_Destroy on SYS VM????"

	VxdCall SHELL_SYSMODAL_Message

DNQD_MsgDone:

	stc
	jmp	short DNQDDone

DNQDChkSkip:
	jecxz	DNQDDoneOK
	dec	ecx
	jmp	DEBFAR DNQDChkLp

DNQDDoneOK:
	clc
DNQDDone:
	ret

EndProc DOSNET_Query_Destroy

;******************************************************************************
;
;   DOSNET_Destroy_VM
;
;   DESCRIPTION:
;	Clean up any outstanding net use inheritance counts.
;	We need this here because a VM which crashes or is destroyed
;	will not go through a normal termination sequence.
;
;   ENTRY:
;	EBX = VM Handle
;
;   EXIT:
;	Carry clear
;
;   USES:
;	FLAGS,EAX,EDX
;
;==============================================================================

BeginProc DOSNET_Destroy_VM

	mov	edx,ebx
	add	edx,[DN_CB_Offset]
	mov	ecx,25
DDVMUndoLp:
	test	[edx.DN_VM_LocalNetUseFlgs][ecx],DN_LocUse_F_InUse
	jz	DEBFAR DDVMUndoSkip
	test	[edx.DN_VM_LocalNetUseFlgs][ecx],DN_LocUse_F_Global
	jnz	DEBFAR DDVMUndoSkip
	test	[edx.DN_VM_LocalNetUseFlgs][ecx],DN_LocUse_F_Inherit
IFDEF DEBUG
	jnz	short DDVMD20
	debug_out "Stranded local use drive #CL DOSNET_Destroy_VM"
DDVMD20:
ENDIF
	jz	short DDVMUndoSkip
	push	ebx

	VMMCall Get_SYS_VM_Handle

	mov	eax,ebx
	add	eax,[DN_CB_Offset]
	pop	ebx
IFDEF DEBUG
	test	[eax.DN_VM_LocalNetUseFlgs][ecx],DN_LocUse_F_InUse
	jnz	short DDVMD05
	debug_out "Inher net use back link to SYS VM invalid 1 DOSNET_Destroy_VM"
DDVMD05:
	test	[eax.DN_VM_LocalNetUseFlgs][ecx],DN_LocUse_F_Inherit + DN_LocUse_F_Global
	jz	short DDVMD07
	debug_out "Inher net use back link to SYS VM invalid 2 DOSNET_Destroy_VM"
DDVMD07:
ENDIF
	cmp	[eax.DN_VM_LocalNetInhRefCnt][ecx],0
IFDEF DEBUG
	jnz	short DDVMD10
	debug_out "Attempt to dec drive #CL inherit count through 0 DOSNET_Destroy_VM"
DDVMD10:
ENDIF
	jz	short DDVMUndoSkip
	dec	[eax.DN_VM_LocalNetInhRefCnt][ecx]
DDVMUndoSkip:
	jecxz	DDVMDone
	dec	ecx
	jmp	DEBFAR DDVMUndoLp

DDVMDone:
	clc
	ret

EndProc DOSNET_Destroy_VM


;******************************************************************************
;
;   DOSNET_VM_Terminate
;
;   DESCRIPTION:
;	Clean up any outstanding local net uses and un-inherit inherited drives
;
;   ENTRY:
;	EBX = VM Handle
;
;   EXIT:
;	Carry clear
;
;   USES:
;	FLAGS,EAX,EDX
;
;==============================================================================

BeginProc DOSNET_VM_Terminate

	mov	edx,ebx
	add	edx,[DN_CB_Offset]
	mov	ecx,25
DVMTUndoLp:
	test	[edx.DN_VM_LocalNetUseFlgs][ecx],DN_LocUse_F_InUse
	jz	DEBFAR DVMTUndoSkip	; Not net, ignore
	test	[edx.DN_VM_LocalNetUseFlgs][ecx],DN_LocUse_F_Global
	jnz	DEBFAR DVMTUndoSkip	; Global, ignore
	test	[edx.DN_VM_LocalNetUseFlgs][ecx],DN_LocUse_F_Inherit
	jz	DEBFAR DVMTUndoUse	; Local use, go undo it
    ;
    ; ECX drive is an inherited drive from SYS VM
    ;
	push	ebx

	VMMCall Get_SYS_VM_Handle

	mov	eax,ebx
	add	eax,[DN_CB_Offset]
	pop	ebx
	cmp	eax,edx
IFDEF DEBUG
	jne	short DNVTD03
	debug_out "Inher set in SYS VM DOSNET_VM_Terminate"
DNVTD03:
ENDIF
	je	short DVMTUndoSkipZap
IFDEF DEBUG
	test	[eax.DN_VM_LocalNetUseFlgs][ecx],DN_LocUse_F_InUse
	jnz	short DNVTD05
	debug_out "Inher net use back link to SYS VM invalid 1 DOSNET_VM_Terminate"
DNVTD05:
	test	[eax.DN_VM_LocalNetUseFlgs][ecx],DN_LocUse_F_Inherit + DN_LocUse_F_Global
	jz	short DNVTD07
	debug_out "Inher net use back link to SYS VM invalid 2 DOSNET_VM_Terminate"
DNVTD07:
ENDIF
	cmp	[eax.DN_VM_LocalNetInhRefCnt][ecx],0
IFDEF DEBUG
	jnz	short DVMTD10
	debug_out "Attempt to dec drive #CL inherit count through 0 DOSNET_VM_Terminate"
DVMTD10:
ENDIF
	jz	short DVMTUndoSkipZap
	dec	[eax.DN_VM_LocalNetInhRefCnt][ecx]
DVMTUndoSkipZap:
	mov	[edx.DN_VM_LocalNetUseFlgs][ecx],0
DVMTUndoSkip:
	jecxz	DVMTDone
	dec	ecx
	jmp	DEBFAR DVMTUndoLp

DVMTUndoUse:
	inc	[NetUseCleanUpFlg]	; Flag that this use /d is from DOSNET
	mov	eax,ecx
	add	al,'A'
	mov	ah,':'
	push	eax
	mov	esi,esp
	mov	eax,5F04h		; "net use /d" call

	VxDInt	21h

IFDEF DEBUG
	jnc	short DVMTD40
	debug_out "Net UnUse clean up attempt failed error #EAX drv @esp"
DVMTD40:
ENDIF
	mov	[NetUseCleanUpFlg],0	; Clear "from DOSNET" flag
	pop	eax
	jmp	short DVMTUndoSkipZap

DVMTDone:
	clc
	ret

EndProc DOSNET_VM_Terminate


;******************************************************************************
;
;   DOSNET_Create_VM
;
;   DESCRIPTION:
;	Initialize the use arrays setting up the GLOBAL and INHERITed drives
;	for the new VM
;
;   ENTRY:
;	EBX = VM Handle
;
;   EXIT:
;	Carry clear
;
;   USES:
;	All but segs, EBP
;
;==============================================================================

BeginProc DOSNET_Create_VM

	push	ebx

	VMMCall Get_SYS_VM_Handle

	mov	esi,ebx
	pop	edi
	cmp	esi,edi
	je	short DCVMDone		; Skip SYS VM
	add	esi,[DN_CB_Offset]
	add	edi,[DN_CB_Offset]
	mov	ecx,25
DCVMSetupLp:
	test	[esi.DN_VM_LocalNetUseFlgs][ecx],DN_LocUse_F_InUse
	jz	short DCVMSetupSkip	; Not a net drive
IFDEF DEBUG
	test	[esi.DN_VM_LocalNetUseFlgs][ecx],DN_LocUse_F_Inherit
	jz	short DCVMD30
	debug_out "DN_LocUse_F_Inherit set in SYS VM drive #CL DOSNET_Create_VM"
DCVMD30:
	mov	al,[esi.DN_VM_LocalNetUseFlgs][ecx]
	and	al,DN_LocUse_F_Inherit + DN_LocUse_F_Global
	cmp	al,DN_LocUse_F_Inherit + DN_LocUse_F_Global
	jne	short DCVMD40
	debug_out "DN_LocUse_F_Inherit + Global set in SYS VM drive #CL DOSNET_Create_VM"
DCVMD40:
ENDIF
	test	[esi.DN_VM_LocalNetUseFlgs][ecx],DN_LocUse_F_Global
	jnz	short DCVMSetGlobal
	mov	[edi.DN_VM_LocalNetUseFlgs][ecx],DN_LocUse_F_InUse + DN_LocUse_F_Inherit
	inc	[esi.DN_VM_LocalNetInhRefCnt][ecx]
DCVMSetupSkip:
	jecxz	DCVMDone
	dec	ecx
	jmp	short DCVMSetupLp

DCVMDone:
	clc
	ret

DCVMSetGlobal:
	mov	[edi.DN_VM_LocalNetUseFlgs][ecx],DN_LocUse_F_InUse + DN_LocUse_F_Global
	jmp	short DCVMSetupSkip

EndProc DOSNET_Create_VM


;******************************************************************************
;
;   DOSNET_Int_21
;
;   DESCRIPTION:
;	This procedure handles the processing of NET USE and NET USE /d
;
;   ENTRY:
;	EBX = VM Handle making INT 21h call
;
;   EXIT:
;	None
;
;   USES:
;	All but segs, EBP
;
;==============================================================================

;
; This structure is allocated and associated with a specific NET USE
;   call. It indicates what the call is doing.
;
DN_I21_TailStruc STRUC

NDrive		dd	?	; 0 = A drive number
DriveAddr	dd	?	; V86 address of NET USE string
OrigDrive	db	?	; Original byte at DriveAddr
RETFlag 	db	0	;  3 is NETUSE ret
				;  4 is NETUNUSE ret
DN_I21_TailStruc ENDS


BeginProc DOSNET_Int_21, High_Freq

	cmp	[ebp.Client_AH],5Fh	; NET USE call?
	jne	DN_I21_Reflect		; No, ignore
DN_I21_NETUSE:
	cmp	[ebp.Client_AL],04h	; Cancel Redirection?
	je	DEBFAR NetUseBreak	; Yes
	cmp	[ebp.Client_AL],03h	; Make Redirection?
	jne	DN_I21_Reflect		; No, ignore
NetUseMake:
	cmp	[ebp.Client_BL],04h	; File device make?
	jne	DN_I21_Reflect		; No, ignore
	movzx	esi,[ebp.Client_DS]	; Get pointer to drive
	shl	esi,4
	movzx	eax,[ebp.Client_SI]
	add	esi,eax
	add	esi,[ebx.CB_High_Linear]
	cmp	byte ptr [esi],0	; UNC use?
	je	DN_I21_Reflect		; Yes, Ignore UNC
	cmp	word ptr [esi+1],":"	; Valid Drive use?
	jne	DN_I21_Reflect		; No, Ignore random drivel
	movzx	eax,byte ptr [esi]	; Get drive letter
	or	al,20h			; Make lower case
	sub	al,'a'			; Convert to number
	jc	DN_I21_Reflect		; Ignore, invalid drive
	cmp	al,26
	jae	DN_I21_Reflect		; Ignore, invalid drive
	push	eax			; Save drive #
	call	DOSNET_AllocInfoStruc	; Allocate DN_I21_TailStruc for call
	pop	eax			; drive #
IFDEF DEBUG
	jnc	short NUD10
	debug_out "DOSNET_AllocInfoStruc failed on 5F03h call, can't track drive #AL"
NUD10:
ENDIF
	jc	DN_I21_Reflect
	mov	[edx.NDrive],eax
	mov	[edx.RETFlag],3 	; NET USE make type
	jmp	DN_I21_SetCB

NetUseBreak:
	cmp	[NetUseCleanUpFlg],0	; From DOSNET device?
	jne	DN_I21_Reflect		; Yes, let it through
	movzx	esi,[ebp.Client_DS]
	shl	esi,4
	movzx	eax,[ebp.Client_SI]
	add	esi,eax
	add	esi,[ebx.CB_High_Linear]
	cmp	word ptr [esi],"//"	; UNC?
	je	DN_I21_Reflect		; Yes, Ignore UNC
	cmp	word ptr [esi],"\\"	; UNC?
	je	DN_I21_Reflect		; Yes, Ignore UNC
	cmp	word ptr [esi],"\/"	; UNC?
	je	DN_I21_Reflect		; Yes, Ignore UNC
	cmp	word ptr [esi],"/\"     ; UNC?
	je	DN_I21_Reflect		; Yes, Ignore UNC
	cmp	word ptr [esi+1],":"	; Drive type?
	jne	DN_I21_Reflect		; No, Ignore char dev type
	movzx	eax,byte ptr [esi]	; Get drive letter
	or	al,20h			; To lower case
	sub	al,'a'			; To number
	jc	DN_I21_Reflect		; Ignore, invalid drive
	cmp	al,26
	jae	DN_I21_Reflect		; Ignore, invalid drive
	mov	ecx,ebx
	add	ecx,[DN_CB_Offset]
	test	[ecx.DN_VM_LocalNetUseFlgs][eax],DN_LocUse_F_InUse
	jz	DN_I21_Reflect		; Ignore, uninteresting drives
	test	[ecx.DN_VM_LocalNetUseFlgs][eax],DN_LocUse_F_Global
	jnz	short DN_I21_FailBreakGlobal
	test	[ecx.DN_VM_LocalNetUseFlgs][eax],DN_LocUse_F_Inherit
	jnz	short DN_I21_FailBreakInherit
	cmp	[ecx.DN_VM_LocalNetInhRefCnt][eax],0
	jnz	short DN_I21_FailBreakParent
	push	eax			; Save drive
	push	ecx			; and CB pointer
	call	DOSNET_AllocInfoStruc	; Allocate DN_I21_TailStruc for call
	pop	ecx
	pop	eax
	jc	short BrkBlast
	mov	[edx.NDrive],eax
	mov	[edx.RETFlag],4 	; Net use /D type
	jmp	DN_I21_SetCB

BrkBlast:
	debug_out "DOSNET_AllocInfoStruc failed on 5F04h call, freeing drive #AL"
	mov	[ecx.DN_VM_LocalNetUseFlgs][eax],0
	jmp	DN_I21_Reflect


;
; Can't NET USE /D GLOBAL drives
;
DN_I21_FailBreakGlobal:
	mov	ecx,offset32 DNNetUseGlobalMsg
	mov	edi,offset32 DNNetUseGlobalMsgDrive
	jmp	short DN_I21_FailBreak

;
; Can't NET USE /D drives inherited from SYS VM
;
DN_I21_FailBreakInherit:
	mov	ecx,offset32 DNNetUseInherMsg
	mov	edi,offset32 DNNetUseInherMsgDrive
	jmp	short DN_I21_FailBreak

;
; Can't NET USE /D drives passed to other VMs
;
DN_I21_FailBreakParent:
	mov	ecx,offset32 DNNetUseParentMsg
	mov	edi,offset32 DNNetUseParentMsgDrive
DN_I21_FailBreak:
	mov	al,byte ptr [esi]	; Drive letter
	and	al,NOT 20h		; To upper case
	mov	byte ptr [edi],al	; Set drive letter in message
	push	esi
	xor	edi,edi 		; Standard caption
	xor	esi,esi 		; No call back
	mov	eax,MB_OK + MB_ICONEXCLAMATION + MB_SYSTEMMODAL

	VMMCall Test_SYS_VM_Handle

	jz	short DN_I21_Modal

	VxdCall SHELL_Message

	jmp	short DN_I21_MsgDone

DN_I21_Modal:

	VxdCall SHELL_SYSMODAL_Message

DN_I21_MsgDone:
	call	DOSNET_AllocInfoStruc
	pop	esi
	jc	short FailBrkAsIs
	mov	al,byte ptr [esi]	; Get drive letter
	mov	byte ptr [esi],'['	; Change to invalid drive to simulate
					;   correct DOS error
	mov	[edx.NDrive],0FFFFFFFFh ; Flag that we are failing this
	mov	[edx.DriveAddr],esi	; Save user address
	mov	[edx.OrigDrive],al	; And original byte
	mov	[edx.RETFlag],4 	; NET USE /D type
	jmp	short DN_I21_SetCB


FailBrkAsIs:
	mov	[ebp.Client_AX],15	; Error Invalid Drive
	or	[ebp.Client_EFLAGS],CF_Mask
	clc
	ret

;
; Set the call back to catch when the INT 21H is complete
;
DN_I21_SetCB:
	mov	eax,10000		; 10 second time out
	mov	esi,offset32 DN_I21_CallBack

	VMMCall Call_When_VM_Returns

DN_I21_Reflect:
	stc
	ret

DN_I21_FreeReflect:

	VMMCall _HeapFree,<edx,0>

	jmp	short DN_I21_Reflect


EndProc DOSNET_Int_21

;**
;
; DOSNET_AllocInfoStruc - Allocate DN_I21_TailStruc
;
; ENTRY:
;	None
;
; EXIT:
;	Carry Set
;	    Failed
;	Carry Clear
;	    EDX -> DN_I21_TailStruc
;
; USES:
;	EAX,ECX,EDX,FLAGS
;
BeginProc DOSNET_AllocInfoStruc

	VMMCall _HeapAllocate,<<(SIZE DN_I21_TailStruc)>,HeapZeroInit>

	or	eax,eax
IFDEF DEBUG
	jnz	short DN_I21D30
	trace_out "HeapAllocate Failed DOSNET_Int_21"
DN_I21D30:
ENDIF
	stc
	jz	short DN_AIFDone
	mov	edx,eax
	clc
DN_AIFDone:
	ret

EndProc DOSNET_AllocInfoStruc


;**
;
; DN_I21_CallBack - Perform back end processing of INT 21 call
;
; ENTRY:
;	EBX = VM Handle
;	EDX -> DN_I21_TailStruc for call
;
; EXIT:
;	None
;
; USES:
;	EAX,ECX,EDX,FLAGS
;
BeginProc DN_I21_CallBack

IFDEF DEBUG
	jnc	short DN_I21D10
	trace_out "Int 21 timed out DN_I21_CallBack"
DN_I21D10:
ENDIF
	jc	short DN_I21_CallBackFree
	cmp	[edx.RETFlag],4 		; NET USE /D?
	je	short SetNetUnUseRet		; Yes
	test	[ebp.Client_EFLAGS], CF_Mask	; Call worked?
	jnz	short DN_I21_CallBackFree	; No, done
	cmp	[edx.RETFlag],3 		; NET USE?
	je	short SetNetUseRet		; Yes
	debug_out "DN_I21_CallBack odd RETFlag value"
DN_I21_CallWorked:
DN_I21_CallBackFree:

	VMMCall _HeapFree,<edx,0>

	ret

SetNetUnUseRet:
	mov	eax,[edx.NDrive]
	inc	eax				; Special "Set a Failure" case?
	jz	short NetUnUseRetMyFail 	; Yes, go fix back up
	dec	eax
	test	[ebp.Client_EFLAGS], CF_Mask	; Worked?
	jnz	short DN_I21_CallBackFree	; No, done
    ;
    ; NET USE /D Worked
    ;	Clear the flags, no longer a net drive
    ;
	mov	ecx,ebx
	add	ecx,[DN_CB_Offset]
	mov	[ecx.DN_VM_LocalNetUseFlgs][eax],0
	jmp	short DN_I21_CallBackFree

NetUnUseRetMyFail:
	mov	esi,[edx.DriveAddr]
	mov	al,[edx.OrigDrive]
	mov	byte ptr [esi],al		; Restore proper user drive
	jmp	short DN_I21_CallBackFree

SetNetUseRet:
	mov	eax,[edx.NDrive]
	mov	ecx,ebx
	add	ecx,[DN_CB_Offset]
IFDEF DEBUG
	cmp	[ecx.DN_VM_LocalNetUseFlgs][eax],0
	je	short NUD20
	debug_out "5F03 call worked, but DN_VM_LocalNetUseFlgs != 0"
NUD20:
ENDIF
	mov	[ecx.DN_VM_LocalNetUseFlgs][eax],DN_LocUse_F_InUse
	mov	[ecx.DN_VM_LocalNetInhRefCnt][eax],0
	jmp	DN_I21_CallBackFree

EndProc DN_I21_CallBack

;****************************************************************************
;
; Querry Debug code
;

IFDEF DEBUG

BeginProc DOSNET_Dump_Debug_Info

	pushad
	mov	al,[NetUseCleanUpFlg]
	trace_out "NetUseCleanUpFlg = #AL"
	mov	edi,[DN_CB_Offset]
	VMMcall Get_Cur_VM_Handle
Dump_Next_VM:
	call	DOSNET_Dump_VM
	VMMcall Get_Next_VM_Handle
	VMMcall Test_Cur_VM_Handle
	je	SHORT DOSNET_DDI_Exit
	Trace_Out "[ESC] to exit, any other key for next VM: ", NO_EOL
	VMMcall In_Debug_Chr
	Trace_Out " "
	jnz	Dump_Next_VM

DOSNET_DDI_Exit:
	popad
	clc
	ret

EndProc DOSNET_Dump_Debug_Info

_DATA SEGMENT

Drive_Strng	db	"    Drive "
ThisDrv 	db	"A:, Inher Ref count #DL, FLAGS - ",0

_DATA ENDS

BeginProc DOSNET_Dump_VM

	trace_out "VM #EBX"
	push	ebx
	add	ebx,edi
	xor	eax,eax
	mov	[ThisDrv],'A'
	mov	ecx,26
ShowDrvs:
	mov	dl,[ebx.DN_VM_LocalNetInhRefCnt][eax]
	pushfd
	pushad
	mov	esi, OFFSET32 Drive_Strng
	VMMcall Out_Debug_String
	popad
	popfd
	test	[ebx.DN_VM_LocalNetUseFlgs][eax],DN_LocUse_F_InUse
	jz	short ChkInh
	trace_out "INUSE ",NO_EOL
ChkInh:
	test	[ebx.DN_VM_LocalNetUseFlgs][eax],DN_LocUse_F_Inherit
	jz	short ChkGlb
	trace_out "INHERIT ",NO_EOL
ChkGlb:
	test	[ebx.DN_VM_LocalNetUseFlgs][eax],DN_LocUse_F_Global
	jz	short FlgsDn
	trace_out "GLOBAL ",NO_EOL
FlgsDn:
	trace_out " "
	inc	[ThisDrv]
	inc	eax
	loop	ShowDrvs
	pop	ebx
	ret

EndProc DOSNET_Dump_VM

ENDIF
;
;****************************************************************************

VxD_CODE_ENDS

;******************************************************************************
;******************************************************************************
;
; Real mode initialization code
;
; DOSNET does not install if not in REDIR configuration.
;
;******************************************************************************

VxD_REAL_INIT_SEG

;
; Don't load if DOS is not in the REDIRector configuration
;
BeginProc DOSNET_init

	mov	ax,3000h		; Get version
	int	21h
	xchg	ah,al
	cmp	ax,030Ah		; At least 3.10?
	jb	short DOSNET_DontLoad	; No, no network stuff
    ;
    ; If the Get Redirection list entry DOS call returns an invalid
    ;	function error, the DOS REDIR is not loaded.
    ;
	mov	ax,5F02H		; Get Redirection list entry
	xor	bx,bx			; Index 0, Also Clears carry
	push	cs
	pop	ds
	push	cs
	pop	es
	mov	si,offset RMbuffer	; Point at a buffer (don't really care
	mov	di,offset RMbuffer	;   about its contents)
	int	21h
	jnc	short DOSNET_Load	; Call worked, REDIR is loaded
    ;
    ; We got an error. Invalid function is the interesting one. Any other error
    ;	(probably "no more files" (no redirections)) indicates the REDIR is
    ;	around.
    ;
	cmp	ax,1			; Invalid function?
	je	short DOSNET_DontLoad	; Yes, network not started
DOSNET_Load:
	mov	ax, Device_Load_Ok
DOSNET_LdDone:
	xor	bx, bx
	xor	edx, edx
	xor	si, si
	ret

DOSNET_DontLoad:
	mov	ax, Abort_Device_Load + No_Fail_Message
	jmp	short DOSNET_LdDone

RMbuffer db	128 dup (0)

EndProc DOSNET_init

VxD_REAL_INIT_ENDS

	END	DOSNET_Init
