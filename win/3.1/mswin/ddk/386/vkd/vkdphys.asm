PAGE 58,132
;******************************************************************************
TITLE vkdphys.asm -
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1988
;
;   Title:	vkdphys.asm - physical keyboard services
;
;   Version:	1.00
;
;   Date:	05-Aug-1988
;
;   Author:	RAP
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   05-Aug-1988 RAP
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
	INCLUDE VKD.Inc
	INCLUDE VKDSYS.Inc
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

ack_timeoutH	dd  0
ack_response	dw  0
last_cmd	db  0

;******************************************************************************
;
; ack handling bits  (used in high byte of ack_response)
;
ACK_opt_rdy	    equ 80h
ACK_received	    equ 0Fh


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

	in	al, pstat_Kybd		; get status
	mov	cl, al

	mov	eax, [VKD_irq_Handle]
	VxDCall VPICD_Phys_EOI		; EOI the physical interrupt

	test	cl, fKBS_DAV		;Q: any data available?
	jz	i9_exit 		;  N: ignore int

	in	al, pdata_Kybd		; get data byte from 8042
	mov	cl, al			; save scan code read

	VK_HK_Queue_Out "VKD_Int_09 key #al"

	TestMem [VKD_flags], VKDf_wait_cmd_res
	jnz	short Handle_8042_res

	cmp	cl, KBD_Rsp_Ack 	;Q: ACK byte?
	je	short Handle_Ack	;   Y: handle ACK specially

	mov	eax, [keybuf_tail]
	inc	eax
	and	eax, KeyBuf_Size - 1	; wrap around for circular Q
	cmp	eax, [keybuf_head]	;Q: buffer full?
	je	short buf_full		;   Y: ignore scan code
	mov	[keybuf_tail], eax	;   N: update tail index & store code
	mov	[eax+keybuf], cl

buf_full:
	mov	edx, [VKD_Kbd_Owner]	; pass focus VM handle to server
	add	edx, [VKD_CB_Offset]
	cmp	[edx.eventHandle], 0	;Q: already requested?
	jnz	short i9_exit		;  Y: don't request again

	mov	esi, OFFSET32 VKD_VM_Service_Phys
	VMMCall Schedule_Global_Event
	mov	[edx.eventHandle], esi
	jmp	short i9_exit

Handle_Ack:
	xor	ax, ax			; get response in ax, and clear response
	xchg	ax, [ack_response]	; for next ack
	test	ah, ACK_opt_rdy 	;Q: option byte to send?
	jz	short ack_recvd 	;   N: just ignore ack
	call	VKD_Send_Data
	jmp	short i9_exit
ack_recvd:				; set high byte of ack_response to
					; indicate that the last ack was received
	mov	byte ptr [ack_response+1], ACK_received
	jmp	short i9_exit

Handle_8042_res:
	mov	edx, [VKD_8042_owner]
	mov	esi, edx
	add	esi, [VKD_CB_Offset]

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
	mov	esi, OFFSET32 VKD_8042_Response
	sti
	VMMJmp	Call_Global_Event

i9_exit:
	ret

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
	mov	[last_cmd], al
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
	and	[esp], NOT CF_Mask	; clear carry
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

BeginProc VKD_Put_Byte, PUBLIC

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

	mov	ecx, 10000h
vsl_8042_busy:
	cmp	byte ptr [ack_response+1], ACK_received ;Q: waiting for ack response?
	loopdnz vsl_8042_busy		;		     Y: loop

	mov	ah, ACK_opt_rdy 	; set high bit to indicate option byte
	mov	[ack_response], ax	; to be sent
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

	mov	ecx, 10000h
vskr_8042_busy:
	cmp	byte ptr [ack_response+1], ACK_received ;Q: waiting for ack response?
	loopdnz vskr_8042_busy		 ;		    Y: loop

	mov	ah, ACK_opt_rdy 	; set high bit to indicate option byte
	mov	[ack_response], ax	; to be sent
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
	cmp	byte ptr [ack_response+1], ACK_received
	je	short skip_enable
	Trace_Out 'VKD: wait for ACK timed out'
	mov	[ack_response], 0
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
;   ENTRY:	    nothing
;
;   EXIT:	    nothing
;
;   USES:	    EAX, EBX, Flags
;
;==============================================================================

BeginProc Dump_KeyBuf

	mov	ebx, [keybuf_head]
dkb_lp:
	cmp	ebx, [keybuf_tail]	;Q: buffer empty?
	je	short dkb_exit		;   Y: ret
	inc	ebx
	and	ebx, KeyBuf_Size - 1	; wrap around for circular Q
	mov	al, [ebx+keybuf]	; get byte from buffer
	Trace_Out ' #al' /noeol
	jmp	dkb_lp
dkb_exit:
	Trace_Out ' '
	ret

EndProc Dump_KeyBuf

ENDIF

VxD_CODE_ENDS

	END
