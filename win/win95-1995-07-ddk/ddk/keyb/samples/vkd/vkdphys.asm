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
TITLE vkdphys.asm -
;******************************************************************************
;
;   Title:	vkdphys.asm - physical keyboard services
;
;   Version:	1.00
;
;==============================================================================

	.386p



;******************************************************************************
;			      I N C L U D E S
;******************************************************************************

.xlist
	INCLUDE VMM.Inc
	INCLUDE VPICD.Inc
	INCLUDE Debug.Inc
	INCLUDE VTD.Inc
	INCLUDE VKD.Inc
	INCLUDE VKDSYS.Inc
	INCLUDE	SHELL.INC
	INCLUDE OPTTEST.Inc
.list


;******************************************************************************
;			    L O C A L	D A T A
;******************************************************************************

VxD_LOCKED_DATA_SEG

EXTRN	VKD_irq_Handle:DWORD
EXTRN	VKD_CB_Offset:DWORD
EXTRN	VKD_Kbd_Owner:DWORD
EXTRN	VKD_flags:DWORD
EXTRN	VKD_8042_owner:DWORD
EXTRN	NewShell:BYTE

ack_timeoutH	dd  0
ack_response	label dword
ack_res_state	db  0
ack_res_data	db  0
		dw  0

;******************************************************************************
;
; ack handling bits  (used in ack_res_state)
;
ACK_ring0_seq	    equ 80h
ACK_opt_rdy	    equ 40h
ACK_opt_rdy_bit     equ 6


KeyBuf_Size equ 128

	align 4
keybuf_tail dd	0		    ; position of last scan code entered
keybuf_head dd	0		    ; position of next scan code to remove
keybuf	    db	KeyBuf_Size DUP(?)

VxD_LOCKED_DATA_ENDS


VxD_CODE_SEG

EXTRN	VKD_VM_Service_Phys:NEAR
EXTRN	VKD_8042_Response:NEAR

VxD_CODE_ENDS


VxD_LOCKED_CODE_SEG

EXTRN	Put_8042_Byte:NEAR

BeginDoc
;******************************************************************************
;
; VKD_Filter_Keyboard_Input
;
; DESCRIPTION:
;		This service can be hooked by a VxD which wishes to
;		modify or reject keyboard input.
; ENTRY:
;		CL = scan code.
; EXIT:
;		Carry Clear => continue processing, CL = new scan code
;		Carry set => Reject keyboard input.
; USES:
;		FLAGS, CL
;==============================================================================
EndDoc
BeginProc VKD_Filter_Keyboard_Input,Async_Service,High_Freq
	clc
	ret
EndProc VKD_Filter_Keyboard_Input

;******************************************************************************
;			I N T E R R U P T   H A N D L E R
;******************************************************************************

;******************************************************************************
;
;   VKD_Int_09
;
;   DESCRIPTION:
;
;   ENTRY:
;	EAX = IRQ Handle
;	EBX = Handle of current VM
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc VKD_Int_09, PUBLIC, High_Freq

	xor	ecx, ecx
	dec	ch

vi_09_poll_Busy:
	in	al, pstat_Kybd		; get status
	test	al, fKBS_DAV		; Q: Data avail ?
	jnz	@F			;    Y:
	test	al, fKBS_Bsy		;    N: Q: System busy ?
	loopnz	vi_09_poll_Busy		;          Y: Search again
@@:
	mov	cl, al

	mov	eax, [VKD_irq_Handle]
	VxDCall VPICD_Phys_EOI		; EOI the physical interrupt

	cmp	[NewShell],1
	jne	vi_09_oldshell

	VxDCall	SHELL_Update_User_Activity	; tell shell about it

vi_09_oldshell:

	test	cl, fKBS_DAV		;Q: any data available ?
	jz	i9_exit			;   N: ignore int

	in	al, pdata_Kybd		; get data byte from 8042
	mov	cl, al			; save scan code read

	VK_HK_Queue_Out "VKD_Int_09 key #al"

	mov	esi, [VKD_CB_Offset]

	TestMem [VKD_flags], VKDf_wait_cmd_res
	jnz	short Handle_8042_res

	cmp	cl, KBD_Rsp_Ack 	;Q: ACK byte?
	je	short Handle_Ack	;   Y: handle ACK specially

	VxDCall	VKD_Filter_Keyboard_Input ; give a chance to other VxDs to
					; modify/reject the input.
	jc	i9_exit			; no input to be queued.

	mov	eax, [keybuf_tail]
	inc	eax
	and	eax, KeyBuf_Size - 1	; wrap around for circular Q
	cmp	eax, [keybuf_head]	;Q: buffer full?
	je	short buf_full		;   Y: ignore scan code
	mov	[keybuf_tail], eax	;   N: update tail index & store code
	mov	[eax+keybuf], cl

buf_full:
	mov	edx, [VKD_Kbd_Owner]	; pass focus VM handle to server
	add	edx, esi
	cmp	[edx.eventHandle], 0	;Q: already requested?
	jnz	short i9_exit		;  Y: don't request again

	mov	esi, OFFSET32 VKD_VM_Service_Phys
	VMMCall Schedule_Global_Event
	mov	[edx.eventHandle], esi
	jmp	short i9_exit

Handle_Ack:
	mov	edx, OFFSET32 ack_response
.errnz ack_res_state - ack_response
	cmp	byte ptr [edx], 0	;Q: in ring 0 ack sequence?
	jz	short reflect_data	;   N: just pass the ack to VM
	xor	al, al
	xchg	al, [edx+1]		; get response in ax
.errnz ack_res_data - ack_response - 1
	btr	dword ptr [edx], ACK_opt_rdy_bit ;Q: option byte to send?
	jnc	short end_ack_seq	;	    N: clear res state
	call	VKD_Send_Data
	jmp	short i9_exit

Handle_8042_res:
	mov	edx, [VKD_8042_owner]
	add	esi, edx

	call	Put_8042_Byte
	mov	ch, [esi._8042_expected]
	cmp	ch, VKD_NulTerminated	    ;Q: waiting for nul?
	jne	short no_nul		    ;	N:
	or	cl, cl			    ;	Y: Q: nul found?
	jz	short sch_event
	inc	ch
no_nul:
	dec	ch
	mov	[esi._8042_expected], ch
	jnz	short i9_exit

sch_event:
	mov	al, KBD_Ctl_Ena 	    ; re-enable keyboard
	call	VKD_Send_Cmd
	ClrFlag [VKD_flags], VKDf_wait_cmd_res
reflect_8042_byte:
	mov	esi, OFFSET32 VKD_8042_Response
	sti
	VMMCall	Call_Global_Event

i9_exit:
	clc
	ret

end_ack_seq:
	mov	[ack_res_state], 0	; indicate last ack received
	jmp	i9_exit

reflect_data:
	mov	edx, [VKD_Kbd_Owner]
	add	esi, edx
	call	Put_8042_Byte
	jmp	reflect_8042_byte

EndProc VKD_Int_09

VxD_LOCKED_CODE_ENDS


VxD_CODE_SEG

;******************************************************************************
;
;   Kybd_Wait_Out   Wait till the keyboard controller receives data/cmd
;
;   DESCRIPTION:    Waits for input buffer full status port to clear or timeout
;
;   ENTRY:	    none
;
;   EXIT:	    none
;
;   USES:	    Flags
;
;   ASSUMES:
;	The time spent waiting for output buffer to clear is indeterminate.
;	The assumption is made that it will clear before a tight loop executed
;	64k times can finish.
;
;==============================================================================
BeginProc Kybd_Wait_Out

	push	eax
	push	ecx
	mov	eax, [VKD_irq_Handle]
	or	eax, eax
	jz	short not_hooked1
	VxDCall VPICD_Physically_Mask

not_hooked1:
	mov	ecx, 10000h
Kybd_WO_Lp:
	in	al, pstat_Kybd
	test	al, fKBS_Bsy		; Is buffer still full?
	loopdnz Kybd_WO_Lp		;   Yes, wait till empty or timeout
IFDEF DEBUG
	jz	short kwo_no_timeout
	Trace_Out 'VKD:  wait for busy bit clear timed out'
kwo_no_timeout:
ENDIF
	mov	eax, [VKD_irq_Handle]
	or	eax, eax
	jz	short not_hooked2
	VxDCall VPICD_Physically_Unmask

not_hooked2:
	pop	ecx
	pop	eax
	ret

EndProc Kybd_Wait_Out


;******************************************************************************
;
;   VKD_Send_Data
;
;   DESCRIPTION:    send a byte of data to the keyboard data port
;
;   ENTRY:	    AL byte to send
;
;   EXIT:
;
;   USES:
;
;==============================================================================
BeginProc VKD_Send_Data

	call	Kybd_Wait_Out		; Wait for previous command to clear
	out	pdata_Kybd, al		; send keyboard command byte
	ret

EndProc VKD_Send_Data


;******************************************************************************
;
;   VKD_Send_Cmd
;
;   DESCRIPTION:    send a byte of data to the keyboard command port
;
;   ENTRY:	    AL byte to send
;
;   EXIT:
;
;   USES:
;
;==============================================================================
BeginProc VKD_Send_Cmd

	call	Kybd_Wait_Out		    ; Wait for previous command to clear
	out	pcmd_Kybd, al
	ret

EndProc VKD_Send_Cmd


;******************************************************************************
;   P U B L I C   S E R V I C E S
;******************************************************************************

;******************************************************************************
;
;   VKD_Buf_Empty
;
;   DESCRIPTION:    determine if the input buffer is empty
;
;   ENTRY:
;
;   EXIT:	    Z flag set, if buffer is empty
;
;   USES:	    EAX
;
;==============================================================================

BeginProc VKD_Buf_Empty, PUBLIC

	mov	eax, [keybuf_head]
	cmp	eax, [keybuf_tail]	;Q: buffer empty?
	ret

EndProc VKD_Buf_Empty


;******************************************************************************
;
;   VKD_Get_Byte
;
;   DESCRIPTION:    get next available byte from input queue, if possible
;
;   ENTRY:	    nothing
;
;   EXIT:	    al byte from queue
;		    Carry flag set, if queue empty
;
;   USES:	    EAX, Carry flag
;
;==============================================================================

BeginProc VKD_Get_Byte, PUBLIC, High_Freq

	stc
	pushfd
	cli
	mov	eax, [keybuf_head]
	cmp	eax, [keybuf_tail]	;Q: buffer empty?
	je	short gb_exit		;   Y: ret
	inc	eax
	and	eax, KeyBuf_Size - 1	; wrap around for circular Q
	mov	[keybuf_head], eax
	mov	al, [eax+keybuf]	; get byte from buffer
	and	dword ptr [esp], NOT CF_Mask	; clear carry
gb_exit:
	popfd
	ret

EndProc VKD_Get_Byte


;******************************************************************************
;
;   VKD_Put_Byte
;
;   DESCRIPTION:    put a byte into the input queue, if possible
;
;   ENTRY:	    AL is byte to input
;
;   EXIT:	    Carry flag set, if queue full
;
;   USES:	    nothing
;
;==============================================================================

BeginProc VKD_Put_Byte, PUBLIC, Async_Service

	pushfd
	push	ebx
	cli
	mov	ebx, [keybuf_tail]
	inc	ebx
	and	ebx, KeyBuf_Size - 1	; wrap around for circular Q
	cmp	ebx, [keybuf_head]	;Q: buffer full?
	je	short pb_buf_full	;   Y:
	mov	[keybuf_tail], ebx	;   N: update tail index & store code
	mov	[ebx+keybuf], al
	pop	ebx
	popfd
	clc
	jmp	short pb_exit
pb_buf_full:
	pop	ebx
	popfd
	stc
pb_exit:
	ret

EndProc VKD_Put_Byte


VxD_Code_Ends

VxD_Pageable_Code_Seg

;******************************************************************************
;
;   VKD_Enable_Kbd
;
;   DESCRIPTION:    send command to 8042 to enable the keyboard
;
;   ENTRY:	    nothing
;
;   EXIT:	    nothing
;
;   USES:	    nothing
;
;==============================================================================

BeginProc VKD_Enable_Kbd, PUBLIC

	push	eax
	mov	al, KBD_Ctl_Ena
	call	VKD_Send_Cmd		; send keyboard command byte
	pop	eax
	ret

EndProc VKD_Enable_Kbd

;******************************************************************************
;
;   VKD_Disable_Kbd
;
;   DESCRIPTION:    send command to 8042 to disable the keyboard
;
;   ENTRY:	    nothing
;
;   EXIT:	    nothing
;
;   USES:	    nothing
;
;==============================================================================

BeginProc VKD_Disable_Kbd, PUBLIC

	push	eax
	mov	al, KBD_Ctl_Dis
	call	VKD_Send_Cmd		; send keyboard command byte
	pop	eax
	ret

EndProc VKD_Disable_Kbd

VxD_Pageable_Code_Ends

VxD_Code_Seg

;******************************************************************************
;
;   VKD_Clear_Data_Buf
;
;   DESCRIPTION:    clear the keyboard data buffer port
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc VKD_Clear_Data_Buf, PUBLIC

	push	eax
clr_lp:
	in	al, pstat_Kybd		; get status
	test	al, fKBS_DAV		;Q: any data available?
	jz	short clr_exit		;  N: all done

	in	al, pdata_Kybd		; get data byte for 8042
	jmp	clr_lp

clr_exit:
	pop	eax
	ret

EndProc VKD_Clear_Data_Buf


;******************************************************************************
;
;   VKD_Poll_ACK
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
BeginProc VKD_Poll_ACK

	push	eax
	VxdCall VTD_Get_Real_Time
	mov	esi, eax
vsl_8042_busy:
	cmp	[ack_res_state], 0	;Q: waiting for ack response?
	je	short @F		;   N: it happened
	VxdCall VTD_Get_Real_Time
	sub	eax, esi
	cmp	eax, 1000000		;Q: ACK taken longer than .8 sec?
	jb	vsl_8042_busy		;   N: loop
@@:
	pop	eax
	ret

EndProc VKD_Poll_ACK


;******************************************************************************
;
;   VKD_Set_LEDs
;
;   DESCRIPTION:
;
;   ENTRY:	    AL = LED status to send to the keyboard
;
;   EXIT:
;
;   USES:	    flags
;
;==============================================================================

BeginProc VKD_Set_LEDs, PUBLIC

	pushad

	call	VKD_Poll_ACK
	mov	[ack_res_data], al
	mov	[ack_res_state], ACK_ring0_seq + ACK_opt_rdy ; set flag bits
					; to indicate option byte to be sent
	mov	al, KBD_Cmd_LED
	call	VKD_Send_Data

	xor	esi, esi
	xchg	esi, [ack_timeoutH]
	VMMCall Cancel_Time_Out
	mov	eax, 1000		; 1 sec timeout
	xor	edx, edx
	mov	esi, OFFSET32 VKD_ACK_Timeout
	VMMCall Set_Global_Time_Out
	mov	[ack_timeoutH], esi
	popad
	ret

EndProc VKD_Set_LEDs


;******************************************************************************
;
;   VKD_Set_Key_Rate
;
;   DESCRIPTION:    set the keyboard repeat & delay rates
;
;   ENTRY:	    al is repeat & delay values to set
;
;			0ddrrrrr	dd    = 2 bits of delay rate
;					rrrrr = 5 bits of repeat rate
;
;   EXIT:
;
;   USES:	    flags
;
;==============================================================================

BeginProc VKD_Set_Key_Rate, PUBLIC

	pushad

	call	VKD_Poll_ACK
	mov	[ack_res_data], al
	mov	[ack_res_state], ACK_ring0_seq + ACK_opt_rdy ; set flag bits
					; to indicate option byte to be sent
	mov	al, KBD_Cmd_Rpt
	call	VKD_Send_Data

	xor	esi, esi
	xchg	esi, [ack_timeoutH]
	VMMCall Cancel_Time_Out
	mov	eax, 1000		; 1 sec timeout
	xor	edx, edx
	mov	esi, OFFSET32 VKD_ACK_Timeout
	VMMCall Set_Global_Time_Out
	mov	[ack_timeoutH], esi
	popad
	ret

EndProc VKD_Set_Key_Rate


;******************************************************************************
;
;   VKD_ACK_Timeout
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
BeginProc VKD_ACK_Timeout

	mov	[ack_timeoutH], 0
	cmp	[ack_res_state], 0
	je	short skip_enable
	Trace_Out 'VKD: wait for ACK timed out'
	xor	eax, eax
	mov	[ack_response], eax
	mov	al, KBD_Cmd_Enb 	    ; try sending an enable to the
	call	VKD_Send_Data		    ; keyboard
skip_enable:
	ret

EndProc VKD_ACK_Timeout


IFDEF Support_Reboot

VxD_DATA_SEG
EXTRN VKD_attempt_reboot:BYTE
VxD_DATA_ENDS

;******************************************************************************
;
;   VKD_Reboot
;
;   DESCRIPTION:    try to reboot the system using the system dependent way
;		    by trying to toggle the reset line through the 8042
;
;   ENTRY:
;
;   EXIT:	    System will reset, or return to caller
;
;   USES:
;
;==============================================================================
BeginProc VKD_Reboot, PUBLIC

	cmp	[VKD_attempt_reboot], 0
	je	short reboot_exit

	VMMCall _MapPhysToLinear,<472h,2,0>
	inc	eax
IFDEF	DEBUG
	jnz	SHORT VR_GotPhysAddr
Debug_Out "VKD: Cannot address reboot parameter at 472h"
VR_GotPhysAddr:
ENDIF
	jz	SHORT VR_NoWarm
	dec	eax
	mov	word ptr [eax], 1234h	    ; specify warm boot
VR_NoWarm:
%OUT	THIS SHOULD BE PARAMETERIZED in WIN.INI!!! 2.1 had at least one test!
	mov	al, KBD_Ctl_Pulse OR 1110b  ; toggle the reset line
	call	VKD_Send_Cmd

reboot_exit:
	clc
	ret				    ; failed, if we got this far

EndProc VKD_Reboot

ENDIF


IFDEF DEBUG

;******************************************************************************
;
;   Dump_KeyBuf
;
;   DESCRIPTION:    Debug code to dump the global keyboard buffer
;
;   From keybuf_head to keybuf_tail are global keys not yet handled.
;   From keybuf_tail to keybuf_head are global keys already handled.
;
;   ENTRY:	    nothing
;
;   EXIT:	    nothing
;
;   USES:	    EAX, EBX, Flags
;
;==============================================================================

BeginProc Dump_KeyBuf

	Trace_Out "global keybuf =" /noeol
	mov	ebx, [keybuf_head]
dkb_lp1:
	cmp	ebx, [keybuf_tail]	;Q: buffer empty?
	je	short dkb_exit1		;   Y: ret
	inc	ebx
	and	ebx, KeyBuf_Size - 1	; wrap around for circular Q
	mov	al, [ebx+keybuf]	; get byte from buffer
	Trace_Out ' #al' /noeol
	jmp	dkb_lp1
dkb_exit1:
	Trace_Out ' '

	Trace_Out "recent global keybuf =" /noeol
	mov	ebx, [keybuf_tail]
dkb_lp2:
	mov	al, [ebx+keybuf]	; get byte from buffer
	Trace_Out ' #al' /noeol
	inc	ebx
	and	ebx, KeyBuf_Size - 1	; wrap around for circular Q
	cmp	ebx, [keybuf_head]	;Q: buffer empty?
	jne	short dkb_lp2		;   N: keep going
	Trace_Out ' '

	ret

EndProc Dump_KeyBuf

ENDIF

VxD_CODE_ENDS

	END
