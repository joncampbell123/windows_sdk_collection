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

PAGE 58,132
;******************************************************************************
TITLE vkdio.asm -
;******************************************************************************
;
;   Title:	vkdio.asm
;
;   Version:	1.00
;
;==============================================================================
	.386p


;******************************************************************************
;			      I N C L U D E S
;******************************************************************************

.XLIST
	INCLUDE VMM.INC
	INCLUDE Debug.INC
	INCLUDE VPICD.INC

	INCLUDE VKD.INC
	INCLUDE VKDSYS.INC
	INCLUDE OPTTEST.Inc
.LIST


IFNDEF DebugPorts
DebugPorts = 0	   ; no debugging
ENDIF


VxD_IDATA_SEG
EXTRN	Kbd_MCA_Passwd_Ini:BYTE
EXTRN	Kbd_8042_ReadCmd_Ini:BYTE
EXTRN	Kbd_8042_WriteCmd_Ini:BYTE
VxD_IDATA_ENDS


VxD_DATA_SEG

EXTRN	VKD_CB_Offset:DWORD
EXTRN	VKD_flags:DWORD
EXTRN	VKD_Kbd_Owner:DWORD
EXTRN	VKD_irq_Handle:DWORD

PUBLIC VKD_8042_owner
VKD_8042_owner	    dd	0		; VM handle, if VKDf_wait_cmd_res set


;------------------------------------------------------------------------------
;
;   Macros used to statically define the 8042 command table:
;
;	NoEntry 	 start [, end]
;	    reserve entries which may have specific code to virtualize, or
;	    are just sent if ever received
;
;	Read_8042_Data	 cmdnum, cmdbytes [, restrict]
;	    specify a command which returns data thru successive reads from 60h
;	    cmdbytes specifies # of bytes returned, VKD_NulTerminated specifies
;		that a nul-terminated string of bytes is returned
;	    restrict optionally restricts the command to the focus VM, or to be
;		ignored in all VMs, if blank, then command allowed in all VM's
;		(FOCUS or IGNORE)
;
;	Write_8042_Data  cmdnum, cmdbytes [, restrict]
;	    specify a command which gets additional data thru successive writes
;		to 60h
;	    cmdbytes specifies # of bytes required, VKD_NulTerminated specifies
;		that it reads thru a nul terminating byte
;	    restrict optionally restricts the command to the focus VM, or to be
;		ignored in all VMs, if blank, then command allowed in all VM's
;		(FOCUS or IGNORE)

NoEntry MACRO start, end
.errnz $ - offset cmd_table - start
IFB <end>
	db  VKD_not_defined
ELSE
	db  (end - start + 1) DUP(VKD_not_defined)
ENDIF
ENDM

Read_8042_Data MACRO cmdnum, cmdbytes, restrict
.errnz $ - offset cmd_table - cmdnum
.errnz cmdbytes GT VKD_NulTerminated	; max repeat count is kept in 5 bits

IFIDNI <restrict>, <FOCUS>
	db  VKD_Do_In_Focus_VM OR VKD_Read_Cmd + cmdbytes
ELSE
IFIDNI <restrict>, <IGNORE>
	db  VKD_Ignore_In_All_VMs OR VKD_Read_Cmd + cmdbytes
ELSE
	db  VKD_Do_In_All_VMs OR VKD_Read_Cmd + cmdbytes
ENDIF
ENDIF
ENDM

Write_8042_Data MACRO cmdnum, cmdbytes, restrict
.errnz $ - offset cmd_table - cmdnum
.errnz cmdbytes GT VKD_NulTerminated	; max repeat count is kept in 5 bits

IFIDNI <restrict>, <FOCUS>
	db  VKD_Do_In_Focus_VM OR VKD_Write_Cmd + cmdbytes
ELSE
IFIDNI <restrict>, <IGNORE>
	db  VKD_Ignore_In_All_VMs OR VKD_Write_Cmd + cmdbytes
ELSE
	db  VKD_Do_In_All_VMs OR VKD_Write_Cmd + cmdbytes
ENDIF
ENDIF
ENDM

;------------------------------------------------------------------------------
;
;   Macros used to dynamically modify the 8042 command table with code:
;
;	C_NoEntry	   cmdnum
;	    reserve an entry which may have specific code to virtualize, or
;	    is just sent if ever received
;
;	C_Read_8042_Data   cmdnum, cmdbytes [, restrict]
;	    specify a command which returns data thru successive reads from 60h
;	    cmdbytes specifies # of bytes returned, VKD_NulTerminated specifies
;		that a nul-terminated string of bytes is returned
;	    restrict optionally restricts the command to the focus VM, or to be
;		ignored in all VMs, if blank, then command allowed in all VM's
;		(FOCUS or IGNORE)
;
;	C_Write_8042_Data  cmdnum, cmdbytes [, restrict]
;	    specify a command which gets additional data thru successive writes
;		to 60h
;	    cmdbytes specifies # of bytes required, VKD_NulTerminated specifies
;		that it reads thru a nul terminating byte
;	    restrict optionally restricts the command to the focus VM, or to be
;		ignored in all VMs, if blank, then command allowed in all VM's
;		(FOCUS or IGNORE)

C_NoEntry MACRO cmdnum
	mov	[cmd_table+cmdnum], 0
ENDM

C_Read_8042_Data MACRO cmdnum, cmdbytes, restrict
	mov	[cmd_table+cmdnum], cmdbytes
IFIDNI <restrict>, <FOCUS>
	or	[cmd_table+cmdnum], VKD_Do_In_Focus_VM OR VKD_Read_Cmd
ELSE
IFIDNI <restrict>, <IGNORE>
	or	[cmd_table+cmdnum], VKD_Ignore_In_All_VMs OR VKD_Read_Cmd
ELSE
	or	[cmd_table+cmdnum], VKD_Do_In_All_VMs OR VKD_Read_Cmd
ENDIF
ENDIF
ENDM

C_Write_8042_Data MACRO cmdnum, cmdbytes, restrict
	mov	[cmd_table+cmdnum], cmdbytes
IFIDNI <restrict>, <FOCUS>
	or	[cmd_table+cmdnum], VKD_Do_In_Focus_VM OR VKD_Write_Cmd
ELSE
IFIDNI <restrict>, <IGNORE>
	or	[cmd_table+cmdnum], VKD_Ignore_In_All_VMs OR VKD_Write_Cmd
ELSE
	or	[cmd_table+cmdnum], VKD_Do_In_All_VMs OR VKD_Write_Cmd
ENDIF
ENDIF
ENDM

;------------------------------------------------------------------------------
PUBLIC VKD_8042_cmd_table
VKD_8042_cmd_table label byte
cmd_table	   label   byte
	NoEntry 	    0, 1Fh
	NoEntry 	    020h	; read command byte
	Read_8042_Data	    021h, 1
	Read_8042_Data	    022h, 1
	Read_8042_Data	    023h, 1
	Read_8042_Data	    024h, 1
	Read_8042_Data	    025h, 1
	Read_8042_Data	    026h, 1
	Read_8042_Data	    027h, 1
	Read_8042_Data	    028h, 1
	Read_8042_Data	    029h, 1
	Read_8042_Data	    02Ah, 1
	Read_8042_Data	    02Bh, 1
	Read_8042_Data	    02Ch, 1
	Read_8042_Data	    02Dh, 1
	Read_8042_Data	    02Eh, 1
	Read_8042_Data	    02Fh, 1
	Read_8042_Data	    030h, 1
	Read_8042_Data	    031h, 1
	Read_8042_Data	    032h, 1
	Read_8042_Data	    033h, 1
	Read_8042_Data	    034h, 1
	Read_8042_Data	    035h, 1
	Read_8042_Data	    036h, 1
	Read_8042_Data	    037h, 1
	Read_8042_Data	    038h, 1
	Read_8042_Data	    039h, 1
	Read_8042_Data	    03Ah, 1
	Read_8042_Data	    03Bh, 1
	Read_8042_Data	    03Ch, 1
	Read_8042_Data	    03Dh, 1
	Read_8042_Data	    03Eh, 1
	Read_8042_Data	    03Fh, 1
	NoEntry 	    040h, 05Fh
	NoEntry 	    060h	; write command byte
	Write_8042_Data     061h, 1
	Write_8042_Data     062h, 1
	Write_8042_Data     063h, 1
	Write_8042_Data     064h, 1
	Write_8042_Data     065h, 1
	Write_8042_Data     066h, 1
	Write_8042_Data     067h, 1
	Write_8042_Data     068h, 1
	Write_8042_Data     069h, 1
	Write_8042_Data     06Ah, 1
	Write_8042_Data     06Bh, 1
	Write_8042_Data     06Ch, 1
	Write_8042_Data     06Dh, 1
	Write_8042_Data     06Eh, 1
	Write_8042_Data     06Fh, 1
	Write_8042_Data     070h, 1
	Write_8042_Data     071h, 1
	Write_8042_Data     072h, 1
	Write_8042_Data     073h, 1
	Write_8042_Data     074h, 1
	Write_8042_Data     075h, 1
	Write_8042_Data     076h, 1
	Write_8042_Data     077h, 1
	Write_8042_Data     078h, 1
	Write_8042_Data     079h, 1
	Write_8042_Data     07Ah, 1
	Write_8042_Data     07Bh, 1
	Write_8042_Data     07Ch, 1
	Write_8042_Data     07Dh, 1
	Write_8042_Data     07Eh, 1
	Write_8042_Data     07Fh, 1
	NoEntry 	    080h, 0A9h
	Read_8042_Data	    0AAh, 1
	Read_8042_Data	    0ABh, 1
	NoEntry 	    0ACh
	NoEntry 	    0ADh	; disable keyboard
	NoEntry 	    0AEh	; enable keyboard
	NoEntry 	    0AFh, 0BFh
	Read_8042_Data	    0C0h, 1
	NoEntry 	    0C1h, 0CFh
	NoEntry 	    0D0h	; read output port
	NoEntry 	    0D1h	; write output port
	NoEntry 	    0D2h, 0DFh
	Read_8042_Data	    0E0h, 1
	NoEntry 	    0E1h, 0EFh

    ;
    ; The pulse entries have NoEntry for anybody who tries to pulse
    ; lines 0 (reboot) or 1 (A20).  Attempts to pulse line 1 will
    ; merely be ignored.  Attempting to pulse line 0 will close the VM.
    ;
	NoEntry 	    0F0h	; pulse 3210
	NoEntry 	    0F1h	; pulse 321
	NoEntry 	    0F2h	; pulse 32 0
	Write_8042_Data     0F3h, 0	; pulse 32
	NoEntry 	    0F4h	; pulse 3 10
	NoEntry 	    0F5h	; pulse 3 1
	NoEntry 	    0F6h	; pulse 3  0
	Write_8042_Data     0F7h, 0	; pulse 3
	NoEntry 	    0F8h	; pulse  210
	NoEntry 	    0F9h	; pulse  21
	NoEntry 	    0FAh	; pulse  2 0
	Write_8042_Data     0FBh, 0	; pulse  2
	NoEntry 	    0FCh	; pulse   10
	NoEntry 	    0FDh	; pulse   1
	NoEntry 	    0FEh	; pulse    0
	Write_8042_Data     0FFh, 0	; pulse


VxD_DATA_ENDS


VxD_ICODE_SEG

;******************************************************************************
;
;   Force_Upper_Case
;
;   DESCRIPTION:
;	This routine takes an ASCII character in AL and, if it is a lower
;	case letter, converts it to upper case.  (copied from initinfo.asm)
;
;   ENTRY:
;	AL = ASCII character
;
;   EXIT:
;	AL = upper case letter or other ASCII character(unchanged)
;
;   USES:
;
;==============================================================================
BeginProc Force_Upper_Case

	cmp	al, "a"
	jb	SHORT FUC_Done
	cmp	al, "z"
	ja	SHORT FUC_Done
	sub	al, "a"-"A"
FUC_Done:
	ret

EndProc Force_Upper_Case

;******************************************************************************
;
;   VKD_skip_white_space
;
;   DESCRIPTION:    Scan past characters <= ' '
;
;   ENTRY:	    ESI -> starting location in string
;
;   EXIT:	    AL = last char read
;		    ESI -> next char to be read
;		    If Z flag clear
;			non-white space char found
;		    Else
;			end of string (nul) found
;
;   USES:	    AL, ESI, Flags
;
;==============================================================================
BeginProc VKD_skip_white_space

	cld
skip_white_space:
	lodsb
	or	al, al
	jz	short skip_done
	cmp	al, ' '
	jbe	skip_white_space
	or	al, al
skip_done:
	ret

EndProc VKD_skip_white_space


;******************************************************************************
;
;   VKD_process_cmd_ini
;
;   DESCRIPTION:
;
;   ENTRY:	    EDX = command string
;		    ECX = VKD_Read_Cmd or VKD_Write_Cmd
;		    EDI -> INI key string
;
;   EXIT:	    nothing
;
;   USES:	    Flags
;
;==============================================================================
BeginProc VKD_process_cmd_ini

	pushad
	or	cl, VKD_Do_In_All_VMs
	cmp	byte ptr [edx], 0
	je	DEBFAR cmd_ini_exit
	VMMCall Convert_Hex_String
	cmp	eax, 256
IFDEF DEBUG
	jb	short ci_D01
	Trace_Out 'VKD: Cmd value too large in INI line "', /noeol
ci_D00:
	mov	esi, edi
	VMMCall Out_Debug_String
	Trace_Out '=', /noeol
	mov	esi, [esp.Pushad_EDX]
	VMMCall Out_Debug_String
	Trace_Out '"'
	jmp	cmd_ini_exit
ci_D01:
ELSE
	jae	short cmd_ini_exit
ENDIF
	mov	ebx, eax
	mov	esi, edx
	call	VKD_skip_white_space
	jz	short ci_done
	cmp	al, ','
IFDEF DEBUG
	je	short ci_D03
ci_D02:
	Trace_Out 'VKD: Invalid INI line "', /noeol
	jmp	ci_D00
ci_D03:
ELSE
	jne	short cmd_ini_exit
ENDIF

	call	VKD_skip_white_space
	jz	short ci_done
	cmp	al, '-'
	jne	short ci_get_count
	or	cl, VKD_NulTerminated
	jmp	short ci_find_separator
ci_get_count:
	cmp	al, ','
	je	short ci_chk_restrictions
	mov	edx, esi
	dec	edx
	VMMCall Convert_Decimal_String
	cmp	eax, VKD_NulTerminated
IFDEF DEBUG
	jb	short ci_D04
	Trace_Out 'VKD: Count too large in INI line "', /noeol
	jmp	ci_D00
ci_D04:
ELSE
	jae	short cmd_ini_exit
ENDIF
	or	cl, al
	mov	esi, edx

ci_find_separator:
	call	VKD_skip_white_space
	jz	short ci_done
	cmp	al, ','
IFDEF DEBUG
	jne	ci_D02
ELSE
	jne	short cmd_ini_exit
ENDIF

ci_chk_restrictions:
	call	VKD_skip_white_space
	jz	short ci_done
	call	Force_Upper_Case
	cmp	al, 'F'
	je	short ci_focus_only
	cmp	al, 'N'
	jne	short ci_done
	and	cl, NOT VKD_Type_Mask
	or	cl, VKD_Ignore_In_All_VMs
	jmp	short ci_done
ci_focus_only:
	and	cl, NOT VKD_Type_Mask
	or	cl, VKD_Do_In_Focus_VM

ci_done:
	mov	[ebx+cmd_table], cl
IFDEF DEBUG
	cmp	[vkd_debug_io], VMM_TRUE
	jne	short cmd_ini_exit
	Trace_Out 'VKD 8042 command declared #bl set to #cl'
ENDIF

cmd_ini_exit:
	popad
	ret

EndProc VKD_process_cmd_ini


;******************************************************************************
;
;   VKD_IO_Init
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================
BeginProc VKD_IO_Init

;
; hook keyboard I/O ports
;
	mov	edx, pcmd_Kybd
	mov	esi, OFFSET32 VKD_Trap_CommandR
	VMMCall Install_IO_Handler

	mov	edx, pdata_Kybd
	mov	esi, OFFSET32 VKD_Trap_DataR
	VMMCall Install_IO_Handler

;
; check machine specific INI switches
;
	VMMCall Get_Machine_Info
	xor	eax, eax
	TestReg ebx, GMIF_MCA				; Q: Micro channel?
	jz	SHORT not_MCA				;   N:
	dec	eax

not_MCA:
	xor	esi, esi
	mov	edi, OFFSET32 Kbd_MCA_Passwd_Ini
	VMMCall Get_Profile_Boolean
	or	eax, eax		    ;Q: switch = TRUE?
	jz	short not_MCA_PssWd	    ;	N:

; define additional 8042 commands for handling password security

	Trace_Out "including support for PS/2 compatible password security"

	C_Read_8042_Data  KBD_Ctl_TstPass, 1
	C_Write_8042_Data KBD_Ctl_LdPass,  VKD_NulTerminated, FOCUS
	C_Write_8042_Data KBD_Ctl_EnPass,  0,		      FOCUS

not_MCA_PssWd:

IFDEF DEBUG
VxD_IDATA_SEG
vkd_debug_io_ini    db 'KYBDIODEBUG', 0
VxD_IDATA_ENDS
VxD_DATA_SEG
vkd_debug_io	db 0
VxD_DATA_ENDS

	mov	eax, FALSE
	mov	edi, OFFSET32 vkd_debug_io_ini
	VMMCall Get_Profile_Boolean
	mov	[vkd_debug_io], al
ENDIF

	mov	edi, OFFSET32 Kbd_8042_ReadCmd_Ini
	VMMCall Get_Profile_String
next_read_cmd:
	jc	short no_more_read_cmds
	mov	ecx, VKD_Read_Cmd
	call	VKD_process_cmd_ini
	VMMCall Get_Next_Profile_String
	jmp	next_read_cmd

no_more_read_cmds:
	mov	edi, OFFSET32 Kbd_8042_WriteCmd_Ini
	VMMCall Get_Profile_String
next_write_cmd:
	jc	short no_more_write_cmds
	mov	ecx, VKD_Write_Cmd
	call	VKD_process_cmd_ini
	VMMCall Get_Next_Profile_String
	jmp	next_write_cmd

no_more_write_cmds:

	ret

EndProc VKD_IO_Init

VxD_ICODE_ENDS


VxD_CODE_SEG

EXTRN	VKD_Send_Cmd:NEAR
EXTRN	VKD_Send_Data:NEAR
EXTRN	VKD_Set_LEDs:NEAR
EXTRN	VKD_Set_Key_Rate:NEAR
EXTRN	Simulate_VM_INT9:NEAR

EXTRN	VAD_Read_Command:NEAR
EXTRN	VAD_Write_Command:NEAR
EXTRN	VAD_Read_Data:NEAR
EXTRN	VKD_Alt_Timeout:NEAR

;******************************************************************************
;			I / O	E M U L A T I O N
;******************************************************************************

;******************************************************************************
;
;   VKD_Trap_CommandR
;
;   DESCRIPTION:    virtualize the I/O done to the keyboard's command port
;
;   ENTRY:	    AL = Byte to be output (if output)
;		    EBX = VM handle
;		    EDX = Port address
;		    ECX = 0 for byte input, 4 for byte output, others emulated
;
;   EXIT:	    AL = byte input (if input)
;
;   USES:	    Everything
;
;==============================================================================

BeginProc VKD_Trap_CommandR, PUBLIC, High_Freq

	mov	esi, ebx		; set esi to point to VKD data in
	add	esi, [VKD_CB_Offset]	; the VM's control block
	Dispatch_Byte_IO Fall_Through, <SHORT VKD_Out_64>

VKD_In_64:  ; get status

	call	VAD_Read_Command		; Q: PS/2 type mouse status?
	jc	cmdr_exit		;    Y: returned AL

	in	al, pstat_Kybd			; read keyboard status
	and	al, fKBS_Sys OR fKBS_UnLock
	or	al, [esi._8042_status]
cmdr_exit:
	ret


VKD_Out_64:

IF DebugPorts GE 2
	Trace_Out 'O#al',/noeol
ENDIF

	movzx	eax, al
	mov	ah, [eax+cmd_table]
	or	ah, ah			;Q: table entry or command?
	jz	chk_special		;   N: check for special command

	mov	[esi._8042_last_cmd], al
	mov	dl, ah
	and	dl, VKD_Cnt_Mask
	mov	[esi._8042_expected], dl

	mov	dl, ah
	and	dl, VKD_Type_Mask
	cmp	dl, VKD_Ignore_In_All_VMs
	je	short simulate_ignore
	cmp	dl, VKD_Do_In_All_VMs
	je	short do_cmd
	cmp	ebx, [VKD_Kbd_owner]
	jne	short simulate_ignore

do_cmd:
	mov	ecx, 10000h
do_cmd_lp:
	TestMem [VKD_flags], VKDf_wait_cmd_res	;Q: currently waiting?
IFDEF DEBUG
	jz	short not_waiting
	Debug_Out 'waiting for previous 8042 cmd to finish'
spin_8042_busy:
	TestMem [VKD_flags], VKDf_wait_cmd_res	;Q: currently waiting?
	loopnz	spin_8042_busy
not_waiting:
ELSE
	loopnz	do_cmd_lp
ENDIF
	mov	[VKD_8042_owner], ebx
	cmp	[esi._8042_expected], 0 	;Q: 0 bytes read or written?
	je	short send_direct		;   Y: just send commmand
	test	ah, VKD_Write_Cmd		;Q: writing data to 8042?
	jnz	short writing_data		;   Y:

	SetFlag [VKD_flags], VKDf_wait_cmd_res
	push	eax
	mov	al, KBD_Ctl_Dis
	call	VKD_Send_Cmd
	or	[esi._8042_status], fKBS_Bsy
	pop	eax
send_direct:
	call	VKD_Send_Cmd			; send physical command
						; queue responses in VKD_INT_09
						; re-enable keyboard and
						; send responses into the VM
	ret
;;;;	jmp	DEBFAR cmd_exit

writing_data:					; queue data in VKD_Trap_DataR
	SetFlag [esi.kbdState], KBS_8042Writing ; send physical command
	jmp	DEBFAR cmd_exit 		; send queued data




simulate_ignore:
IFDEF DEBUG
	cmp	[vkd_debug_io], VMM_TRUE
	jne	short D00_no_trace
	Trace_Out 'VKD: ignoring #al'
D00_no_trace:
ENDIF

	test	ah, VKD_Write_Cmd		;Q: writing data to 8042?
	jnz	short ignore_writing_data	;   Y:

	xor	al, al
	movzx	ecx, [esi._8042_expected]
	cmp	cl, VKD_NulTerminated
	jne	short si_queue_nuls
	mov	cl, 1
si_queue_nuls:
	xchg	ecx, eax
	call	Put_8042_Byte
	xchg	ecx, eax
	loop	si_queue_nuls
	mov	edx, ebx
	CallRet VKD_8042_Response

ignore_writing_data:
	SetFlag [esi.kbdState], KBS_8042Ignoring
	jmp	short cmd_exit

;
; handle special commands here:
;
chk_special:
	cmp	al, KBD_Ctl_Dis 	;Q: disable keyboard command?
	jne	short cmd_l1		;   N:
					;   Y: include disabled bit in cmd byte
	or	[esi._8042_cmd_byte], fKBC_Dis
	jmp	short cmd_exit
cmd_l1:
	cmp	al, KBD_Ctl_Ena 	;Q: enable keyboard command?
	jne	short cmd_l2		;   N:
					;   Y: exclude disabled bit in cmd byte
	and	[esi._8042_cmd_byte], NOT fKBC_Dis
	jmp	short cmd_exit
cmd_l2:
	cmp	al, KBD_Ctl_RdCmd
	jne	short cmd_l3
	mov	al, [esi._8042_cmd_byte]    ; virtualize read command byte
	call	VKD_8042_Server
	jmp	short cmd_exit

cmd_l3:
	cmp	al, KBD_Ctl_WrCmd
	jne	short cmd_l4
	SetFlag [esi.kbdState], KBS_WrCmd_Cmd
	jmp	short cmd_exit

%OUT virtualize RdOut

cmd_l4:
	cmp	al, KBD_Ctl_WrOut
	jne	short cmd_l5
	SetFlag [esi.kbdState], KBS_WrOut_Cmd
	jmp	short cmd_exit

cmd_l5:
	cmp	al, KBD_Ctl_Pulse	;Q: pulse ouput port line cmd?
	jb	short cmd_not_pulse	;   N:
	or	al, 2			;   Y: don't allow pulsing A20
	test	al, 1			;   Q: pulsing reset line?
	jnz	send_direct		;      N: let it go
	jmp	VKD_Trap_CommandR_Reboot ;     Y: bozo is trying to reboot

cmd_not_pulse:

cmd_unknown:

	call	VAD_Write_Command	;Q: PS/2 type mouse cmds?
	jz	short cmd_exit		;   Y:

	cmp	ebx, [VKD_Kbd_Owner]	;Q: owner?
IFDEF DEBUG
	cmp	[vkd_debug_io], VMM_TRUE
	jne	send_direct
	Trace_Out 'VKD: unknown command #al'
	jmp	send_direct
ELSE
	je	send_direct		;   Y:
ENDIF

	Trace_Out '8042 command (#al) for non-owner'

cmd_exit:
	ret

EndProc VKD_Trap_CommandR

BeginProc VKD_Trap_CommandR_Reboot, RARE

	Trace_Out "VM #ebx trying to reboot system will be closed"
	pushad
	xor	ecx, ecx		; no flags for Close
	mov	eax, 5000		; let's say, oh, 5 seconds
	VMMCall	Close_VM		; die!
	popad
	jmp	simulate_ignore

EndProc VKD_Trap_CommandR_Reboot

;******************************************************************************
;
;   VKD_Trap_DataR
;
;   DESCRIPTION:    virtualize the I/O done to the keyboard's data port
;
;   ENTRY:	    AL = Byte to be output (if output)
;		    EBX = VM handle
;		    EDX = Port address
;		    ECX = 0 for byte input, 4 for byte output, others emulated
;
;   EXIT:	    AL = Byte input (if input)
;
;   USES:	    Everything
;
;==============================================================================

BeginProc VKD_Trap_DataR, PUBLIC, High_Freq

	mov	esi, ebx		; set esi to point to VKD data in
	add	esi, [VKD_CB_Offset]	; the VM's control block
	Dispatch_Byte_IO Fall_Through, <SHORT VKD_Out_60>

VKD_In_60:
	call	VAD_Read_Data		; Q: PS/2 mouse data ?
	jc	data_exit		;    Y: returned AL
					;    N: read keyboard data
	mov	al, [esi._8042_data]
	mov	[esi._8042_status], 0

	ClrFlag [esi.kbdState], KBS_ACK
;
; clear int request, if it hasn't been simulated yet
;
	push	eax
	mov	eax, [VKD_irq_Handle]
	VxDCall VPICD_Get_Status
	TestReg ecx, VPICD_Stat_In_Service  ;Q: EOI pending?
	jnz	short @F		    ;	Y: leave int requested
	VxDCall VPICD_Clear_Int_Request     ;	N: clear int request
	TestMem	[esi.kbdState], KBS_sim_alt_hld ; Q: Is alt key down ?
	jz	@F				;    N:
	push	esi
	mov	eax,[esi.AltKeyDelay]
	mov	edx,esi
	mov	esi,OFFSET32 VKD_Alt_Timeout	; reset it.
	VMMCall	Set_VM_Time_Out
	pop	esi
@@:
	pop	eax

IF DebugPorts GE 1
	Trace_Out 'i#al',/noeol
ENDIF
	call	_8042_buf_empty
	jz	short data_exit
	mov	edx, ebx
	CallRet Simulate_VM_INT9	; check about simulating another
					; 8042 response byte
data_exit:
	ret


VKD_Out_60:
IF DebugPorts GE 2
	Trace_Out 'o#al',/noeol
ENDIF
	mov	ecx, [esi.kbdState]

	TestReg ecx, KBS_8042Writing	;Q: waiting for 8042 data?
	jnz	DEBFAR queue_data	;   Y:

	TestReg ecx, KBS_8042Ignoring	;Q: waiting for 8042 data?
	jnz	ignore_data		;   Y:

	TestReg ecx, KBS_WrCmd_Cmd	;Q: waiting for KBD_Ctl_WrCmd data byte?
	jnz	short record_command_byte  ;Y:

	TestReg ecx, KBS_WrOut_Cmd	;Q: waiting for KBD_Ctl_WrOut data byte?
	jnz	short send_WrOut_byte	;   Y:

	TestReg ecx, KBS_Cmd_hold	;Q: special command state?
	jz	not_option		;   N:

	btr	ecx, KBS_LED_Cmd_bit	;Q: special LED command state?
	jnc	short not_led_cmd	;   N:
	test	al, 80h 		;Q: new command byte?
	jnz	abort_cmd		;   Y:
	cmp	ebx, [VKD_Kbd_Owner]	;Q: focus VM?
	jne	send_ack		;   N:
	call	VKD_Set_LEDs		;   Y: set LED's with value in al
	jmp	send_ack

not_led_cmd:				; must have been special "set repeat
					; rate" command state
	test	al, 80h 		;Q: new command byte?
	jnz	abort_cmd		;   Y:
	call	VKD_Set_Key_Rate	; set rates with values in al
	and	cl, NOT KBS_Rpt_Cmd
.errnz 0FFFFFF00h AND KBS_Rpt_Cmd
	jmp	send_ack

record_command_byte:
	ClrFlag [esi.kbdState], KBS_WrCmd_Cmd
IFDEF DEBUG
	test	al, fKBC_PCCnvrtBit
	jz	short unsupported

%OUT test for other unsupported command byte bits

	jmp	short wrcmd_ok
unsupported:
	Debug_Out 'VKD:  unsupported command byte #al - ignoring'
	jmp	data_exit
wrcmd_ok:
ENDIF
	mov	[esi._8042_cmd_byte], al

%OUT may want to physically update command byte for some virtual bit changes
;;;;	    and     al, fKBC_Sys OR fKBC_OverLock

	jmp	data_exit

send_WrOut_byte:
	ClrFlag [esi.kbdState], KBS_WrOut_Cmd
	or	al, 2			; don't allow changing A20
	push	eax
	mov	al, KBD_Ctl_WrOut
	call	VKD_Send_Cmd
	pop	eax
	call	VKD_Send_Data
	jmp	data_exit

queue_data:
	mov	cl, al
	call	Put_8042_Byte		; queue data
	mov	ch, [esi._8042_expected]
	cmp	ch, VKD_NulTerminated	;Q: waiting for nul?
	jne	short qd_no_nul 	;   N:
	or	cl, cl			;   Y: Q: nul found?
	jz	short qd_send_cmd
	inc	ch
qd_no_nul:
	dec	ch
	mov	[esi._8042_expected], ch
	jnz	data_exit		; jump if not last byte
qd_send_cmd:
	call	VKD_send_queued_command
	ClrFlag [esi.kbdState], KBS_8042Writing
	jmp	data_exit

ignore_data:
	mov	cl, [esi._8042_expected]
	cmp	cl, VKD_NulTerminated	;Q: waiting for nul?
	jne	short id_no_nul 	;   N:
	or	cl, cl			;   Y: Q: nul found?
	jz	short id_cmd_done
	inc	cl
id_no_nul:
	dec	cl
	mov	[esi._8042_expected], al
	jnz	data_exit
id_cmd_done:
	ClrFlag [esi.kbdState], KBS_8042Ignoring
	jmp	data_exit

abort_cmd:
	and	cl, NOT (KBS_LED_Cmd + KBS_Rpt_Cmd + KBS_ACK)
					; abort special command and start
					; processing new one
.errnz 0FFFFFF00h AND (KBS_LED_Cmd + KBS_Rpt_Cmd + KBS_ACK)

not_option:
IFNDEF No_Gravis_Phoenix_Support
	cmp	al, KBD_Cmd_Gravis	;Q: Gravis prefix?
	je	short gravis_prefix
	TestMem [esi.kbdState], KBS_gravis_seq	;Q: programming gravis?
	jnz	short bad_kbd_cmd		;   Y: skip command processing
ENDIF
	cmp	al, KBD_Cmd_LED 	;Q: Set LED's?
	jb	short bad_kbd_cmd	;   N: bad command
	ja	short od_1		;   N: a different command
	or	ecx, KBS_LED_Cmd	;   Y: set special command state
	jmp	short send_ack		;      send ACK to VM

od_1:	cmp	al, KBD_Cmd_Rpt 	;Q: Set Repeat/Delay rates?
	jne	short od_2		;   N:
	or	ecx, KBS_Rpt_Cmd	;   Y: set special command state
	jmp	short send_ack		;      send ACK to VM

od_2:	cmp	al, KBD_Cmd_Ech 	;Q: Echo?
	jne	short od_3		;   N:
	mov	al, KBD_Rsp_Ech 	;   Y: send echo response
	jmp	short kick_server

od_3:	cmp	al, KBD_Cmd_Rsn 	;Q: Resend?
	jne	short send_ack		;   N: just send ACK for ignored functions
					;      and functions that the kbd treats
					;      as NOP's
	mov	al, [esi._8042_last_data] ; Y: re-send last byte
	jmp	short kick_server

KBS8042flgs equ KBS_ACK+KBS_LED_Cmd+KBS_Rpt_Cmd+KBS_WrCmd_Cmd+KBS_WrOut_Cmd+KBS_8042Writing+KBS_8042Ignoring

IFNDEF No_Gravis_Phoenix_Support
gravis_prefix:
	SetFlag [esi.kbdState], KBS_gravis_prefix
	jmp	short pass_data
ENDIF

bad_kbd_cmd:
IFNDEF No_Gravis_Phoenix_Support
	btr	[esi.kbdState], KBS_gravis_prefix_bit	;Q: last byte a prefix?
	jnc	short pass_data 			;   N:
	cmp	al, KBD_Gravis_Start	;Q: start programming seq?
	je	short @F		;   Y: set flag and send data
    ;
    ; I'm making an assumption here.  I've observed their sequence to be:
    ; send 93/5, 93/1 commands, then stream data followed by a 23h byte and
    ; some other byte (probably a checksum), and finally a 93/3 command to
    ; test the programming.  I assume that once in the programming sequence
    ; any 93/x command, where x <= 5, will cause an end to the programming
    ; sequence.  If they only ever end with a 93/3 sequence, then my assumption
    ; will still hold.	The remaining unknown is still whether they can have
    ; a 93 byte in their programming sequence.	If it can only be the final
    ; "checksum" byte, then this code is still fine, because the sequence would
    ; be: 23 93 93 3.  The first 93 would set the prefix flag, the 2nd 93 would
    ; simply set the prefix flag again, and the 3 would get both the seq flag
    ; and the prefix flags cleared.	RAP  2/95
    ;
	cmp	al, KBD_Gravis_Clear	;Q: end programming seq?
	ja	short pass_data 	;   N:
	ClrFlag [esi.kbdState], KBS_gravis_seq ;Y: clear the seq flag and
	jmp	short pass_data 	;	    send command byte
@@:
	SetFlag [esi.kbdState], KBS_gravis_seq
pass_data:
ENDIF
	ClrFlag [esi.kbdState], KBS8042flgs	; clear 8042 state flags
	cmp	ebx, [VKD_Kbd_Owner]	;Q: owner?
	jne	short ignore_data_byte	;   N:
	call	VKD_Send_Data
ignore_data_byte:
	ret

send_ack:
	or	ecx, KBS_ACK
	mov	al, KBD_Rsp_Ack
kick_server:
	mov	[esi.kbdState], ecx
	CallRet SHORT VKD_8042_Server

EndProc VKD_Trap_DataR

;******************************************************************************
;
;   _8042_Buf_Empty
;
;   DESCRIPTION:    Determine if 8042 response buffer is empty
;
;   ENTRY:	    ESI -> VKD data in CB
;
;   EXIT:	    Z flag set if TRUE
;
;   USES:	    flags
;
;==============================================================================
BeginProc _8042_Buf_Empty

	push	eax
	mov	al, [esi._8042_out_head]
	cmp	al, [esi._8042_out_tail]
	pop	eax
	ret

EndProc _8042_Buf_Empty


;******************************************************************************
;
;   Get_8042_Byte
;
;   DESCRIPTION:    get 8042 byte queued in VM's CB
;
;   ENTRY:	    ESI -> VKD data in CB
;
;   EXIT:	    IF Carry clear THEN
;		      AL = byte
;		    ELSE
;		      no bytes available
;
;   USES:	    EAX, flags
;
;==============================================================================
BeginProc Get_8042_Byte

	movzx	eax, [esi._8042_out_head]
	cmp	al, [esi._8042_out_tail]    ;Q: buffer empty?
	stc
	je	short g8b_exit		    ;	Y: ret
	inc	eax
	and	al, _8042_OutBuf_Size - 1   ; wrap around for circular Q
	mov	[esi._8042_out_head], al
	mov	al, [eax+esi._8042_outbuf]  ; get byte from buffer
	clc
g8b_exit:
	ret

EndProc Get_8042_Byte


;******************************************************************************
;
;   Put_8042_Byte
;
;   DESCRIPTION:    place byte in 8042 buffer in VM's CB
;
;   ENTRY:	    ESI -> VKD data in CB
;		    CL = byte to queue
;
;   EXIT:	    nothing
;
;   USES:	    EAX, Flags
;
;==============================================================================
BeginProc Put_8042_Byte

	movzx	eax, [esi._8042_out_tail]
	inc	eax
	and	al, _8042_OutBuf_Size - 1   ; wrap around for circular Q
	cmp	al, [esi._8042_out_head]    ;Q: buffer full?
	je	short _8042_buf_full	    ;	Y: ignore scan code
	mov	[esi._8042_out_tail], al    ;	N: update tail index & store code
	mov	[eax+esi._8042_outbuf], cl
_8042_buf_full:
	ret

EndProc Put_8042_Byte


;******************************************************************************
;
;   VKD_8042_Response
;
;   DESCRIPTION:
;
;   ENTRY:	    EDX = VM Handle
;
;   EXIT:
;
;   USES:
;
;==============================================================================
BeginProc VKD_8042_Response

	mov	ebx, edx
	mov	esi, edx
	add	esi, [VKD_CB_Offset]

	call	Get_8042_Byte

	and	[esi._8042_status], NOT fKBS_Bsy
	call	VKD_8042_Server
	ret

EndProc VKD_8042_Response


;******************************************************************************
;
;   VKD_8042_Server
;
;   DESCRIPTION:    attempt to simulate hardware interrupts that are a result
;		    of port I/O
;
;   ENTRY:	    EBX = Handle of current VM
;		    ESI = pointer to VKD data in VM's control block
;		    AL = byte to output to VM
;
;   EXIT:
;
;   USES:
;
;   DESIGN:
;	get pic status
;	IF NOT IRQ_In_Service THEN   (* no EOI pending *)
;	  IF port empty THEN
;	    IF byte in queue THEN
;	      move byte from queue into port
;	      update status  (* port not empty *)
;	    ELSE
;	      RETURN	(* no reason to int VM *)
;	    END
;	  END;
;	  IF keyboard enabled THEN
;	    request int
;	  END
;	END
;
;==============================================================================

BeginProc VKD_8042_Server, High_Freq

	call	VKD_OutputByte		    ; setup for next port read
	or	[esi._8042_status], fKBS_Cmd ; data from cmd
IF DebugPorts GE 3
	Trace_Out 'L#al',/noeol
ENDIF
	test	[esi._8042_cmd_byte], fKBC_Int	;Q: 8042 int disabled?
	jz	short server2_exit	    ;	Y: don't check EOI state

	mov	eax, [VKD_irq_Handle]
	VxDCall VPICD_Get_Status

	TestReg ecx, VPICD_Stat_In_Service  ;Q: EOI pending?
	jnz	short server2_exit	    ;	Y: can't send more yet!
	VxDCall VPICD_Set_Int_Request	    ; request a VM int
IF DebugPorts GE 3
	Trace_Out ' '
	Trace_Out 'c',/noeol
ENDIF

server2_exit:
	ret

EndProc VKD_8042_Server

;******************************************************************************
;
;   VKD_OutputByte
;
;   DESCRIPTION:    place byte in virtual 8042 data register & set status
;
;   ENTRY:	    AL is byte to put in data register
;		    ESI points to VKD data in VM's control block
;
;   EXIT:	    nothing
;
;   USES:	    nothing
;
;==============================================================================
BeginProc VKD_OutputByte

	mov	[esi._8042_status], fKBS_DAV ; data available & not read
	mov	[esi._8042_data], al	    ; scan code to read
	mov	[esi._8042_last_data], al   ; scan code to read
	ret

EndProc VKD_OutputByte


;******************************************************************************
;
;   VKD_send_queued_command
;
;   DESCRIPTION:
;
;   ENTRY:	    ESI -> VKD data in VM's CB
;
;   EXIT:
;
;   USES:
;
;==============================================================================
BeginProc VKD_send_queued_command

	mov	al, [esi._8042_last_cmd]
	call	VKD_Send_Cmd
sqc_next_byte:
	call	Get_8042_Byte
	jc	short sqc_exit
	call	VKD_Send_Data
	jmp	sqc_next_byte
sqc_exit:
	ret

EndProc VKD_send_queued_command


VxD_CODE_ENDS

END
