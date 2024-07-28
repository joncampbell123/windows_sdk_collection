       title   VDD - Virtual Display Device for CGA   version 0.00  08/31/87
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
	INCLUDE CGA.INC
	INCLUDE DEBUG.INC

;******************************************************************************
; EXTRN routines and data
;
VxD_CODE_SEG
	EXTRN	VDD_IO_SetTrap:NEAR
	EXTRN	VDD_Mem_Virtual:NEAR
	EXTRN	VDD_Mem_Physical:NEAR
	EXTRN	VDD_Mem_I10Done:NEAR
	EXTRN	VDD_Get_Mode:NEAR
        
VxD_CODE_ENDS

VxD_DATA_SEG
	EXTRN	vgVDD:BYTE
	EXTRN	VDD_CB_Off:DWORD
	EXTRN	VDD_Focus_VM:DWORD
	PUBLIC	VDD_I10_OldVector
VDD_I10_OldVector   dd	    ?

VxD_DATA_ENDS

VxD_CODE_SEG

;**************************************************************************
;
; Reflect? - return ZF set if I/R should be reflected
;
; ENTRY: EBX = VM Handle
;
; EXIT: ZF set if I/R should be reflected
;       Cleared otherwise
;
;       If ZF clear, ESI = address of active video page 
;                    EDI = VRAM address
; Pseudo Code:
;
;    Setup EDI = CB ptr
;    if (PIF.Txt emulation off || VDD_Mode != 3 || not really in Mode 3
;                || int 10 hooked by s'one else)
;        Set ZF;         ; set flag for reflecting the I/R
;    else    {
;        setup ESI, EDI;
;        Clear ZF;       ; Set flag for INT 10 emulation
;    }
;
; USES: EDI,EAX,ESI
;
BeginProc Reflect? 

	mov	edi,ebx
	add	edi,[VDD_CB_Off]
	test	[edi.VDD_PIF],fVidTxtEmulate; Q: INT 10 Emulate ON?
	jz	SHORT R_SetZF               ;	N: Don't emulate
        call    VDD_Get_Mode
	cmp	[edi.VDD_Mode],3	    ; Q: Mode 3?
	jne	SHORT R_SetZF   	    ;	N: not emulated
	cmp	BYTE PTR ds:[449h], 3	    ; Q: Really mode 3?
	jne	SHORT R_SetZF               ;   N: not emulated
	mov	eax,[ebx.CB_High_Linear]
	mov	eax,DWORD PTR 10h*4[eax]
	cmp	eax,[VDD_I10_OldVector]     ; Q: Has anybody hooked INT 10?
	jne	SHORT R_SetZF               ;	Y: not emulated
	mov	esi,0B8000h
	mov	edi,esi 		    ; EDI = VRAM address
	movzx	eax,WORD PTR DS:[44Eh]	    ; EAX = offset to active page
	add	esi,eax 		    ; ESI = address of active video page
        
        mov     eax, 1
        or      eax,eax
        ret
R_SetZF:
        xor     eax,eax
        ret

EndProc Reflect?

;****************************************************************************
;
;   VDD_Dspy_SetPosn
;
;   compute cursor addr in AX, update VDD_Stt cursor addr reg values
;   Also do physical IO if VM is attached.
;
;
;   ENTRY:  DX  = <row, column>
;	    ESI = address of start of video page
;	    EDI = address of start of video memory
;   EXIT:   
;           
;
; 
;   USES:   flags, EAX, ECX
BeginProc VDD_Dspy_SetPosn
	movzx	ecx,BYTE PTR DS:[462h]	    ; ECX = active page
	mov	WORD PTR DS:[450h+ecx*2],dx ; save row/column
	mov	eax,0A0h                    ; Line Length 
	mul	dh			    ; EAX = offset to start of line
	add	eax,esi 		    ; EAX = addr of start of line
	movzx	ecx,dl
	shl	ecx,1                       ; ECX = offset in line to column 
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
	cmp	ecx,[vgVDD.Vid_VM_Handle]   ; Q: Attached VM?
	jz	short VDS_Phys	            ;	Y: set port values
	ret				    ;	N: Don't do output
VDS_Phys:
	push	edx
	push	eax
	mov	edx,pIndx6845Colr
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
	ret
EndProc VDD_Dspy_SetPosn


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
;
; Pseudo code:
;
;    if (Special Msg mode)
;        Reflect;
;    else if (AH != 0Eh)   {    /* convoluted logic for speed?? */
;        if (AH == 02)
;           Set Cursor Pos fn;
;        else if (AH == 0)
;           Set mode fn;
;        else
;           Reflect;
;    }
;    else if (AH == 0Eh)     
;        Write TTY fn;
;    
;
; Cursor Pos Fn:
;    if (BH != Active page)
;        Reflect;
;    Call Reflect?              ; Reflect because of other reasons?
;    jz Reflect;    
;
;    Call VDD_Dspy_SetPosn
;    jmp EmYes;
;
; Set Mode Fn:
;    Setup edi
;    if (already doing a mode set)
;        reflect;
;    VDD_LastMode = -1;
;    Set fVDD_ModeSet in VDD_Flags;
;    call VDD_IO_SetTrap;
;    Adjust_Exec_Priority
;    Setup call back on IRET to VDD_I10_Ret;
;    Reflect;
;
;
; Write TTY Fn:
;    Call Reflect?              ; Reflect because of other reasons?
;    jz Reflect;
;    Emulate TTY out for char in CL
;    if (CL == command char)    {
;        if (CL == BELL)
;            Reflect;
;        else 
;            Handle CR, LF, BS;
;    }
;    else    {
;        Put char in display memory;
;        Compute new Col. Handle LF if Col > maxcol, Scroll if Row > MaxRow
;    }
;    
;    ; Scrolling calls VDD_Dsp_SetPosn, skip calling again
;
;    if (No Scrolling occurred)
;        call VDD_Dsp_SetPosn;
;    jmp EmYes;
;
;
BeginProc   VDD_Int_10,PUBLIC

	test	[vgVDD.Vid_Flags],fVid_Msg  ; Q: Special message mode?
	jnz	SHORT VI10ExitEmNo	    ;	Y: Don't emulate
	mov	eax,[ebp.Client_EAX]
	cmp	ah,0Eh			    ; Q: Write TTY function code?
	jne	VI10NotTTY		    ;	N: No TTY to emulate

WriteTTY:
	mov	ecx,eax 		    ; CL = character
        call    Reflect?                    ; Q: Reflect the I/R?
        jz      SHORT VI10ExitEmNo          ;   Y: 
	cmp	cl,7			    ; Q: Bell?
	je	SHORT VI10ExitEmNo          ;	Y: not emulated
; Emulate TTY out for character in CL
	movzx	edx,BYTE PTR DS:[462h]	    ; EDX = active page
	movzx	edx,WORD PTR DS:[450h+edx*2] ; DH=row, DL=column

	cmp	cl,0Dh			    ; Q: Carriage return?
	je	SHORT VI10Return	    ;	Y: set column to zero
	cmp	cl,08h			    ; Q: Backspace?
	je	SHORT VI10BackSp	    ;	Y: back up one col.(unless on edge)
	cmp	cl,0Ah			    ; Q: Linefeed?
	je	short VI10LineFd	    ;	Y: increment row or scroll

; Put character in display memory
	mov	eax,0A0h		    ; Line Length
	mul	dh			    ; EAX = offset to start of line
	add	eax,esi 		    ; ESI = addr of start of line
	movzx	ebx,dl
	shl	ebx,1			    ; EBX = offset in line to column
	add	eax,ebx 		    ; ESI = addr of char to modify
	mov	[eax],cl		    ; Save character
	inc	dl
	cmp	dl,BYTE PTR DS:[44Ah]
	jne	short VI10SetPos
	xor	dl,dl
VI10LineFd:
	cmp	dh,24
	je	short VI10Scroll
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
	cld
	rep movsb			    ;	Y: Scroll the video memory
VI10ClrLine:
	mov	eax,[edi]		    ; Get char on last line
	mov	al,20h			    ; Make it a blank(save attribute)
	mov	ecx,0A0h/2
	cld
	rep stosw			    ; Clear the line with blanks
	jmp	VI10ExitEmYes		    ; Output emulated, exit

VI10SetCurs:
	movzx	ecx,[ebp.Client_BH]	    ; ECX = output page
	cmp	cl,BYTE PTR ds:[462h]	    ; Q: active page?
	jne	VI10ExitEmNo		    ;	N: reflect
        call    Reflect?                    ; Reflect the I/R?
        jz      VI10ExitEmNo                ; reflect the I/R 
        mov     dx,[ebp.Client_DX]
	call	VDD_Dspy_SetPosn
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
	call	VDD_IO_SetTrap
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
	call	VDD_IO_SetTrap
	mov	eax, - High_Pri_Device_Boost
	VMMCall Adjust_Exec_Priority

	cmp	[VDD_Focus_VM],ebx
	je	VDD_Mem_Physical
	jmp	VDD_Mem_Virtual
        
EndProc     VDD_I10_Ret

VxD_CODE_ENDS

	END
