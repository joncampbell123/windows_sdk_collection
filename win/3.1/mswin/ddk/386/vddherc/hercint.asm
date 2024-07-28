       title   VDD - Virtual Display Device for HERC   version 0.00  08/31/87
;******************************************************************************
;
;VDD - Virtual Display Device
;
;   Author: Marc Wilson
;
;   (C) Copyright MICROSOFT Corp. 1986, 1987
;
;   October, 1986
;   May 15,  1987
;
;DESCRIPTION:
;
;COMPILE:masm386    -t -Mx -Ddevtest -I..\INCLUDE vddint.asm,,,;
;
;******************************************************************************

	.386p

	INCLUDE VMM.INC
	INCLUDE VDD.INC
	INCLUDE HERC.INC
	INCLUDE DEBUG.INC


;******************************************************************************
; EXTRN routines and data
;
VxD_CODE_SEG
	EXTRN	VDD_Mem_Virtual:NEAR
	EXTRN	VDD_Mem_I10Done:NEAR
	EXTRN	VDD_Mem_Physical:NEAR

VxD_CODE_ENDS

VxD_DATA_SEG
	EXTRN	vgVDD:BYTE
	EXTRN	VDD_CB_Off:DWORD
	EXTRN	VDD_Focus_VM:DWORD
	PUBLIC	VDD_I10_OldVector
VDD_I10_OldVector   dd	    ?

VxD_DATA_ENDS

VxD_CODE_SEG

;******************************************************************************
; VDD_Int_10
;
;DESCRIPTION: Emulate WRITE TTY (Speed hack) and do some state
;	changes for mode change INT 10h's.
;
;ENTRY: EBX = current VM handle
;	EBP = client register pointer
;
;EXIT:	CF = 1 indicates interrupt is to be reflected, else it has been emulated
;
;USES:	Flags, EAX, ECX, EDX
;
BeginProc   VDD_Int_10,PUBLIC
	test	[vgVDD.Vid_Flags],fVid_Msg  ; Q: Special message mode?
	jnz	VI10ExitEmNo		    ;	Y: Don't emulate
	mov	eax,[ebp.Client_EAX]
	cmp	ah,0Eh			    ; Q: Write TTY function code?
	jnz	VI10NotTTY		    ;	N: No TTY to emulate
	mov	edi,ebx
	add	edi,[VDD_CB_Off]
	test	[edi.VDD_PIF],fVidTxtEmulate; Q: INT 10 Emulate ON?
	jz	SHORT VI10ExitEmNo		    ;	N: Don't emulate

	cmp	[edi.VDD_Mode],HERC_MODE_Text ; Q: Normal text mode?
	jnz	SHORT VI10ExitEmNo	    ;	N: not emulated
;	cmp	BYTE PTR ds:[449h], 3	    ; Q: Really mode 3?
;	jne	SHORT VI10ExitEmNo	    ; N: not emulated
	cmp	al,7			    ; Q: Bell?
	jz	SHORT VI10ExitEmNo	    ;	Y: not emulated
	mov	ecx,eax 		    ; CL = character
	mov	eax,[ebx.CB_High_Linear]
	mov	eax,DWORD PTR 10h*4[eax]
	cmp	eax,[VDD_I10_OldVector]     ; Q: Has anybody hooked INT 10?
	jne	SHORT VI10ExitEmNo	    ;	Y: not emulated
	cld
	mov	esi,0B0000h
	mov	edi,esi 		    ; EDI = VRAM address
	movzx	eax,WORD PTR DS:[44Eh]	    ; EAX = offset to active page
	add	esi,eax 		    ; ESI = address of active video page

; Emulate TTY out for character in CL
	movzx	edx,BYTE PTR DS:[462h]	    ; EDX = active page
	movzx	edx,WORD PTR DS:[450h+edx*2] ; DH=row, DL=column

	cmp	cl,0Dh			    ; Q: Carriage return?
	jz	SHORT VI10Return	    ;	Y: set column to zero
	cmp	cl,08h			    ; Q: Backspace?
	jz	SHORT VI10BackSp	    ;	Y: back up one col.(unless on edge)
	cmp	cl,0Ah			    ; Q: Linefeed?
	jz	short VI10LineFd	    ;	Y: increment row or scroll

; Put character in display memory
	mov	eax,0A0h		    ; Line Length
	mul	dh			    ; EAX = offset to start of line
	add	eax,esi 		    ; ESI = addr of start of line
	movzx	ebx,dl
	shl	ebx,1			    ; EAX = offset in line to column
	add	eax,ebx 		    ; ESI = addr of char to modify
	mov	[eax],cl		    ; Save character
	inc	dl
	cmp	dl,BYTE PTR DS:[44Ah]
	jnz	short VI10SetPos
	xor	dl,dl
VI10LineFd:
	cmp	dh,24
	jz	short VI10Scroll
	inc	dh
VI10SetPos:
	call	VDD_Dspy_SetPosn
VI10ExitEmYes:
	clc				    ; I emulated this INT 10h
	ret

VI10ExitEmNo:
	stc				    ; I did not emulate this INT 10h
	ret
VI10Return:
	xor	dl,dl
	jmp	short VI10SetPos
VI10BackSp:
	or	dl,dl
	jz	VI10SetPos
	dec	dl
	jmp	VI10SetPos
VI10Scroll:
	call	VDD_Dspy_SetPosn
	mov	edi,esi 		    ; EDI = addr of start of video page
	mov	eax,0A0h		    ; EAX = bytes per line
	lea	esi,[edi][eax]		    ; ESI = start of 2nd line
	mul	dh			    ; EAX = number of bytes to move
	or	eax,eax 		    ; Q: Any lines to scroll?
	jz	short VI10ClrLine	    ;	N: Just clear bottom line
	mov	ecx,eax
; NOTE: this must be byte move because 386 locks out DMA during word or dword
;	    moves to video(byte wide) memory
	rep movsb			    ;	Y: Scroll the video memory
VI10ClrLine:
	mov	eax,[edi]		    ; Get char on last line
	mov	al,20h			    ; Make it a blank(save attribute)
	mov	ecx,0A0h/2
	rep stosw			    ; Clear the line with blanks
	jmp	VI10ExitEmYes		    ; Output emulated, exit
VDD_Dspy_SetPosn:
	movzx	ecx,BYTE PTR DS:[462h]	    ; ECX = active page

; *** Called Entry Point: ***
;   ENTRY:   DX = row, column
;	    ECX = active page number
;	    ESI = address of start of video page
;	    EDI = address of start of video memory
;   EXIT:
;
;   USES:   flags, EAX, ECX
VDD_Dspy_SetPosn2:
	mov	WORD PTR DS:[450h+ecx*2],dx ; save row/column
	mov	eax,0A0h
	mul	dh			    ; EAX = offset to start of line
	add	eax,esi 		    ; EAX = addr of start of line
	movzx	ecx,dl
	shl	ecx,1
	add	eax,ecx 		    ; EAX = addr of char to modify
	sub	eax,edi 		    ; EAX = offset from start of VRAM
	shr	eax,1
	xchg	ah,al
	mov	ecx,ebx
	VMMCall Get_Cur_VM_Handle
	xchg	ecx,ebx
	add	ecx,[VDD_CB_Off]
	mov	WORD PTR [ecx.VDD_Stt.V_CAddrH],ax
	sub	ecx,[VDD_CB_Off]
	cmp	ecx,[vgVDD.Vid_VM_Handle]   ; Q: Does this VM have real HERC?
	jz	short VI10SPPhys	    ;	Y: set port values
	ret				    ;	N: Don't do output
VI10SPPhys:
ifdef OLD
	push	edx
	push	eax
	mov	edx,pIndx6845Mono
	mov	al,0Fh
	out	dx,al			    ; Select cursor low addr reg
	inc	edx
	mov	al,ah
	out	dx,al
	dec	edx
	mov	al,0Eh
	out	dx,al			    ; Select cursor high addr reg
	inc	edx
	pop	eax
	out	dx,al
	pop	edx
else
	push	edx			; save edx
	push	ebx			; save ebx
	mov	ebx,eax			; save cursor address
	mov	edx,pIndx6845Mono	; Index Register Address
	mov	al,0Fh			; cursor low addr index
	out	dx,ax		        ; write cursor low addr register
	mov	al,0Eh			; cursor hi addr index
	mov	ah,bl		        ; cursor high addr value
	out	dx,ax			; write cursor high addr reg
	mov	eax,ebx			; restore eax
	pop	ebx			; restore ebx
	pop	edx			; restore edx

endif
	ret
VI10SetCurs:
	movzx	ecx,[ebp.Client_BH]	    ; ECX = output page
	cmp	cl,BYTE PTR ds:[462h]	    ; Q: active page?
	jne	VI10ExitEmNo		    ;	N: reflect
	mov	eax,[ebx.CB_High_Linear]
	mov	eax,DWORD PTR 10h*4[eax]
	cmp	eax,[VDD_I10_OldVector]     ; Q: Has anybody hooked INT 10?
	jne	VI10ExitEmNo		    ;	Y: not emulated
	mov	edi,ebx
	add	edi,[VDD_CB_Off]
	test	[edi.VDD_PIF],fVidTxtEmulate; Q: INT 10 Emulate ON?
	jz	VI10ExitEmNo		    ;	N: Don't emulate
	cmp	[edi.VDD_Mode],HERC_MODE_Text ; Q: Normal text mode?  
	jnz	VI10ExitEmNo		    ; N: not emulated
;	cmp	BYTE PTR ds:[449h], 3	    ; Q: Really mode 3?
;	jne	VI10ExitEmNo		    ; N: not emulated
	mov	esi,0B0000h
	mov	edi,esi
	movzx	eax,WORD PTR ds:[44Eh]	    ; EAX = offset to active page
	add	esi,eax 		    ; ESI = address of active video page
	call	VDD_Dspy_SetPosn2
	jmp	VI10ExitEmYes		    ; Call emulated, exit
VI10NotTTY:
	cmp	ah,2			    ; Q: Set cursor posn?
	je	VI10SetCurs		    ;	Y: Go do it
	or	ah,ah			    ; Q: Mode setting?
	jnz	VI10ExitEmNo		    ;	N: reflect
VI10Mode:
	mov	edi,ebx
	add	edi,[VDD_CB_Off]
	test	[edi.VDD_Flags],fVDD_ModeSet; Q: Already doing mode set?
	jnz	VI10ExitEmNo		    ;	Y: Just reflect
	mov	[edi.VDD_LastMode],-1	    ; Indicate mode change for window
	or	[edi.VDD_Flags],fVDD_ModeSet
	mov	eax, High_Pri_Device_Boost
	VMMCall Adjust_Exec_Priority
	mov	eax,2000		    ; 2 second timeout
	mov	edx,edi
	mov	esi,OFFSET32 VDD_I10_Ret    ; Procedure to call on IRET
	VMMCall Call_When_VM_Returns
	jmp	VI10ExitEmNo		    ; Go do mode set
EndProc     VDD_Int_10



;******************************************************************************
; VDD_I10_Ret	 Return after completion of INT 10 in VM
;
;DESCRIPTION: Return from break after INT 10 mode set call.  The break point
;	    was put on the stack in the VDD_Int_10 routine.
;
;ENTRY: EBX = current VM handle
;	EBP = client register pointer
;	EDX = VM VDD ptr
;	CF = 1 indicates timeout
;
;EXIT:
;
;USES:	all regs except EBP
;
BeginProc   VDD_I10_Ret,PUBLIC

IFDEF	DEBUG
	jnc	SHORT VI10_NotTimeout
Debug_Out   "Mode set timeout for VM #EBX"
VI10_NotTimeout:
ENDIF
	mov	edi,edx
        call    VDD_Mem_I10Done
	and	[edi.VDD_Flags],NOT fVDD_ModeSet; No longer critical region
	mov	eax, - High_Pri_Device_Boost
	VMMCall Adjust_Exec_Priority

	cmp	[VDD_Focus_VM],ebx
	jz	VDD_Mem_Physical
	jmp	VDD_Mem_Virtual
        ret        
EndProc     VDD_I10_Ret

VxD_CODE_ENDS

	END

