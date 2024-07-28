       TITLE   VDD - Virtual Display Device for EGA   version 2.30  1/88
;******************************************************************************
;
;VDDINT - Virtual Display Device INT 10h handling
;
;   Author: MDW
;
;   (C) Copyright MICROSOFT Corp. 1986-1991
;
;   January 1988
;
;DESCRIPTION:
;	This module handles the INT 10h interrupt.  It emulates the write TTY,
;	function 0Eh and does some additional processing during mode changes,
;	function 0 and function 11h(not sub function 30h).
;
;COMPILE:
;..\..\tools\masm5 -t -Mx -DEGA -DDEBUG -I..\..\INCLUDE -p vddint.asm;
;
;******************************************************************************

	.386p

	INCLUDE VMM.Inc
	INCLUDE Opttest.Inc
	INCLUDE vdd.inc
	INCLUDE ega.inc
	INCLUDE debug.inc


;******************************************************************************
; EXTRN routines and data
;
VxD_CODE_SEG
	EXTRN	VDD_IO_SetTrap:NEAR
	EXTRN	VDD_Detach2:NEAR
	EXTRN	VDD_Mem_I10Done:NEAR
	EXTRN	VDD_Mem_Physical:NEAR
	EXTRN	VDD_Mem_Disable:NEAR
	EXTRN	VDD_Resume_Bkgnd_VMs:NEAR
	EXTRN	VDD_Clr_VM_Time_Out:NEAR
	EXTRN	VDD_Set_VM_Time_Out:NEAR
VxD_CODE_ENDS

VxD_DATA_SEG
	Extern_vgVDD
	EXTRN	VDD_CB_Off:DWORD
	EXTRN	VDD_Msg_VM:DWORD
	PUBLIC	VDD_I10_OldVector
	PUBLIC	VDD_Stub_I10_Vector
VDD_I10_OldVector   dd	    0
VDD_Stub_I10_Vector dd	    ?

	PUBLIC	VDD_I2F_Next_CS
	PUBLIC	VDD_I2F_Next_EIP
VDD_I2F_Next_CS     dd	    ?
VDD_I2F_Next_EIP    dd	    ?

	PUBLIC VDD_V86_tmp_data_area
VDD_V86_tmp_data_area	dd  ?		; address of 17 byte buffer down in
					; low global memory, used for INT 10h
					; AX=1002h calls done from display
					; driver during INT 2F notifications
;*******
; INT 10h protect mode selector:offset
;
	PUBLIC	Vid_I10_Next_CS
	PUBLIC	Vid_I10_Next_EIP
Vid_I10_Next_CS     DD	    ?
Vid_I10_Next_EIP    DD	    ?

VxD_DATA_ENDS

VxD_CODE_SEG

        PUBLIC  VDD_I10_Mode_Change_Start
        PUBLIC  VDD_I10_Mode_Change_End

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
	cmp	[VDD_Msg_VM], 0 	    ; Q: Message mode?
	jnz	VI10ExitEmNo		    ;	Y: Don't emulate
	mov	eax,[ebp.Client_EAX]
	cmp	ah,0Eh			    ; Q: Write TTY function code?
	jnz	VI10NotTTY		    ;	N: No TTY to emulate
	mov	edi,ebx
	add	edi,[VDD_CB_Off]
	TestMem [edi.VDD_PIF],fVidTxtEmulate; Q: INT 10 Emulate ON?
	jz	VI10ExitEmNo		    ;	N: Don't emulate
	TestMem [edi.VDD_Flags],fVDD_MEna   ; Q: Is memory enabled?
	jz	VI10ExitEmNo		    ;	N: Don't emulate
	cmp	[edi.VDD_ModeEGA],3	    ; Q: Mode 3?
	jnz	VI10ExitEmNo		    ;	N: not emulated
	cmp	al,7			    ; Q: Bell?
	jz	VI10ExitEmNo		    ;	Y: not emulated
	mov	ecx,eax 		    ; CL = character
	cmp	BYTE PTR ds:[449h], 3	    ; Q: Really mode 3?
	jne	VI10ExitEmNo	            ;	N: not emulated
	mov	eax,DWORD PTR ds:[10h*4]
	cmp	eax,[VDD_Stub_I10_Vector]   ; Q: Has anybody hooked INT 10?
	jne	VI10ExitEmNo	            ;	Y: not emulated
	cld
	mov	esi,0B8000h
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
	cmp	dh,BYTE PTR DS:[484h]
	jz	short VI10Scroll
	inc	dh
VI10SetPos:
	call	VDD_Dspy_SetPosn
VI10ExitEmYes:
	clc				    ; I emulated this INT 10h
	ret

VI10NotTTY:
	cmp	ah,2			    ; Q: Set cursor posn?
	je	VI10SetCurs		    ;	Y: Go do it
	or	ah,ah			    ; Q: Mode setting?
	jz	SHORT VI10_mode 	    ;	 Y: notify VDD and reflect
	cmp	ah,11h			    ; Q: Mode setting(via char set chg)?
	jnz	SHORT VI10ExitEmNo	    ;	N: reflect
	cmp	al,30h			    ; Q: Get font info?
	jz	SHORT VI10ExitEmNo	    ;	Y: reflect
VI10_Mode:
	mov	edi,ebx
	add	edi,[VDD_CB_Off]
	TestMem [edi.VDD_Flags],fVDD_ModeSet; Q: Already doing mode set?
	jnz	SHORT VI10ExitEmNo	    ;	Y: Just reflect
	call	VDD_I10_Mode_Change_Start
	mov	eax,5000
	mov	esi, OFFSET32 VDD_I10_Mode_Change_End
	VMMCall Call_When_VM_Returns

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
;   USES:   flags, EAX, ECX, ESI
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
	mov	WORD PTR [ecx.VDD_Stt.C_CAddrH],ax
IFDEF PEGA
        push    esi
        mov     esi,[ecx.VDD_Cur_CRTC]
        mov     WORD PTR [esi.C_CAddrH],ax
        pop     esi
ENDIF
	sub	ecx,[VDD_CB_Off]
	cmp	ecx,[Vid_VM_Handle]	    ; Q: Does this VM have real EGA?
	jz	short VI10SPPhys	    ;	Y: set port values
	ret				    ;	N: Don't do output
VI10SPPhys:
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

VI10SetCurs:
	movzx	ecx,[ebp.Client_BH]	    ; ECX = output page
	cmp	cl,BYTE PTR ds:[462h]	    ; Q: active page?
	jne	VI10ExitEmNo		    ;	N: reflect
	mov	eax,DWORD PTR ds:[10h*4]
	cmp	eax,[VDD_Stub_I10_Vector]   ; Q: Has anybody hooked INT 10?

	jne	VI10ExitEmNo		    ;	Y: not emulated
	mov	edi,ebx
	add	edi,[VDD_CB_Off]
	TestMem [edi.VDD_PIF],fVidTxtEmulate; Q: INT 10 Emulate ON?
	jz	VI10ExitEmNo		    ;	N: Don't emulate
	cmp	[edi.VDD_ModeEGA],3	    ; Q: Mode 3?
	jnz	VI10ExitEmNo		    ; N: not emulated
	cmp	BYTE PTR ds:[449h], 3	    ; Q: Really mode 3?
	jne	VI10ExitEmNo		    ; N: not emulated
	mov	esi,0B8000h
	mov	edi,esi
	movzx	eax,WORD PTR ds:[44Eh]	    ; EAX = offset to active page
	add	esi,eax 		    ; ESI = address of active video page
	mov	edx,[ebp.Client_EDX]	    ; DH,DL = cursor posn desired
	call	VDD_Dspy_SetPosn2
	jmp	VI10ExitEmYes		    ; Call emulated, exit
EndProc     VDD_Int_10


;******************************************************************************
;
;   VDD_I10_Mode_Switch
;
;   DESCRIPTION:    V86 Breakpoint handler that receives notification from V86
;		    stub when an INT 10 mode switch is starting.
;
;   ENTRY:
;	EBX = Current VM Handle
;	EBP -> Client register structure
;
;   EXIT:
;
;   USES:
;
;==============================================================================
BeginProc VDD_I10_Mode_Switch

	Assert_Cur_VM_Handle ebx
	Assert_Client_Ptr ebp
	inc	[ebp.Client_IP] 	    ; inc past the brkpt
; This entry point is used by the INT 10h trapping code
VDD_I10_Mode_Change_Start:

	TestMem [Vid_Flags], fVid_SysVMI    ; Q: SYS VM initialized?
	jz	VI10_00 		    ;	N: ignore the mode switch

	mov	edi,ebx
	add	edi,[VDD_CB_Off]

;
; Call VDD_Mem_Physical for attached VM's or VDD_Mem_Disable for unattached VM's,
; to make sure that any random null pages that have been mapped are unmapped
; again.  This is to fix the case where a VM in graphics mode touches text mode
; memory pages, which causes null pages to be mapped.  Then when the VM
; switches back to text mode, we miss seeing page faults to get physical memory
; mapped in again, and thus end up with garbage that isn't cleaned up in
; physical memory.
;
	cmp	ebx, [Vid_VM_Handle]	    ;Q: attached VM?
	jne	short VI10_disable_mem	    ;	N: disable all VRAM for the VM
	call	VDD_Mem_Physical	    ;	Y: map phys which unmaps memory
	jmp	short VI10_continue	    ;	   not physically enabled

VI10_disable_mem:
	call	VDD_Mem_Disable

VI10_continue:
	movzx	eax, [edi.VDD_ModeEGA]

	TestMem [edi.VDD_Flags],fVDD_ModeSet; Q: Already doing mode set?
	jnz	SHORT VI10_exit 	    ;	Y: Just reflect
	push	eax
	mov	[edi.VDD_LastMode],-1	    ; Indicate mode change for window
	cmp	[Vid_VM_Handle2],ebx	    ; Q: 2nd EGA attached VM?
	jnz	short VI10_Not2 	    ;	N: Continue
	call	VDD_Detach2		    ;	Y: Detach from 2nd EGA
VI10_Not2:
	TestMem [edi.VDD_Flags],fVDD_NoTrap
	jz	SHORT VI10_TrapEnable
	SetFlag [edi.VDD_Flags],fVDD_NTSave
	ClrFlag [edi.VDD_Flags],fVDD_NoTrap
VI10_TrapEnable:
	SetFlag [edi.VDD_Flags],fVDD_ModeSet
	call	VDD_IO_SetTrap
	mov	eax, High_Pri_Device_Boost
	VMMCall Adjust_Exec_Priority

	pop	eax
; Assume RAM not saved and clear "Needs 256k mem" flag
	ClrFlag [edi.VDD_Flags],<fVDD_MdSvRam+fVDD_old256>
	VMMCall Test_Sys_VM_Handle		; Q: SYS VM?
	jz	SHORT VI10_exit 		;   Y: Don't do special stuff

	btr	[edi.VDD_Flags],fVDD_256Bit
	jnc	SHORT VI10_not256
	SetFlag [edi.VDD_Flags],fVDD_old256
VI10_not256:
	btr	[edi.VDD_Flags],fVDD_SaveBit ;Q: Have we saved memory?
	jnc	SHORT VI10_exit 	    ;	N: Then don't care what happens
					    ;	    with existing memory
; NOTE that when doing a 11xx font change xx never has sign bit set and
;   so the RAM is not saved.
        mov     ax,[ebp.Client_AX]
        cmp     ah,11h
        je      SHORT VI10_SetSvRAMBit
	test	al,80h			    ; Q: Save RAM during mode set?
	jz	SHORT VI10_exit 	    ;	N: Go do mode set
VI10_SetSvRAMBit:
	SetFlag [edi.VDD_Flags],<fVDD_MdSvRam+fVDD_Save> ;Y: Remember RAM saved

VI10_exit:
	TestMem [edi.VDD_Flags],fVDD_Win	; Q: Running in a window?
        jz      SHORT VI10_00
	TestMem [edi.VDD_Flags],fVDD_ModeChange
        jnz     SHORT VI10_00
	call	VDD_Clr_VM_Time_Out		; Clear VM timeout
	SetFlag [edi.VDD_Flags],fVDD_ModeChange ; Delay mode change
VI10_00:
	ret

EndProc VDD_I10_Mode_Switch

;******************************************************************************
;
;   VDD_I10_Mode_Sw_Done
;
;   DESCRIPTION:    V86 Breakpoint handler that receives notification from V86
;		    stub when an INT 10 mode switch has completed.
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================
BeginProc VDD_I10_Mode_Sw_Done
	Assert_Client_Ptr ebp

	inc	[ebp.Client_IP] 		; inc past the brkpt

; This entry point is from call back if mode change is detected from
; INT 10 trap instead of breakpoint.
VDD_I10_Mode_Change_End:
	TestMem [Vid_Flags], fVid_SysVMI	; Q: SYS VM initialized?
	jz	SHORT VIR_exit			;   N: ignore the mode switch

	VMMCall Get_Cur_VM_Handle
	mov	edi,ebx
	add	edi,[VDD_CB_Off]

	btr	[edi.VDD_Flags],fVDD_ModeSetBit ; No longer critical region
	jnc	SHORT VIR_01			; Skip unboost if already done

	call	VDD_Set_VM_Time_Out		; Set long time out

	TestMem [edi.VDD_Flags],fVDD_NTSave
	jz	SHORT VIR_00
	ClrFlag [edi.VDD_Flags], fVDD_NTSave
	SetFlag [edi.VDD_Flags], fVDD_NoTrap
VIR_00:
	mov	eax, - High_Pri_Device_Boost
	VMMCall Adjust_Exec_Priority
VIR_01:
	call	VDD_IO_SetTrap
	call	VDD_Mem_I10Done 		; Adjust memory for new mode
	TestMem [edi.VDD_Flags],fVDD_256	;Q: 256 bit set now?
	jnz	SHORT VIR_exit			;   Y:
	TestMem [edi.VDD_Flags],fVDD_old256	;Q: 256 bit set before mode sw?
	jz	SHORT VIR_exit			;   N:
	call	VDD_Resume_Bkgnd_VMs		;   Y: resume any VM's which
						;      were suspended because
						;      of this VM
VIR_exit:
	ret

EndProc VDD_I10_Mode_Sw_Done


;******************************************************************************
;
;   VDD_Int_10_PM
;
;   DESCRIPTION:    Ignore block set pallete registers, AH=10h, AL=2, when
;		    done during processing of the INT 2F foreground notify.
;
;   ENTRY:	    EBX = VM Handle
;		    Client_AH = function
;		    Client_AL = sub-function
;
;			For Client_AX = 1002h:
;
;			    Client_ES:DX -> 17 byte table
;
;   EXIT:	    none
;
;   USES:	    Preserves Seg registers
;
;==============================================================================
BeginProc VDD_Int_10_PM

	mov	eax, [ebp.Client_EAX]
	cmp	ax, 1002h
	je	short VI10_PM_chk
VI10_PM_Reflect:
	mov	ecx, [Vid_I10_Next_CS]
	mov	edx, [Vid_I10_Next_EIP]
	VMMjmp	Simulate_Far_Jmp

VI10_PM_chk:
	VMMCall Test_Sys_VM_Handle
	jnz	VI10_PM_Reflect
	mov	edi, [VDD_V86_tmp_data_area]
	Client_Ptr_Flat esi, ES, DX
	mov	ecx, 17
	cld
	rep	movsb
;
; Perform same int 10h in V86 mode
;
	Push_Client_State
	VMMCall Begin_Nest_V86_Exec
	sub	edi, 17
	mov	eax, edi
	and	edi, 0Fh
	mov	[ebp.Client_DX], di
	shr	eax, 4
	mov	[ebp.Client_ES], ax
	mov	eax, 10h
	VMMCall Exec_Int
	VMMCall End_Nest_Exec
	Pop_Client_State
	VMMjmp	Simulate_Iret		    ; fake pmode iret

EndProc VDD_Int_10_PM


;******************************************************************************
;
;   VDD_PM_Int_2F
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

BeginProc VDD_PM_Int_2F, High_Freq, PUBLIC

	call	VDD_Int_2F
	jnc	SHORT VI2F_PM_Eat_It
	mov	ecx, [VDD_I2F_Next_CS]
	mov	edx, [VDD_I2F_Next_EIP]
	VMMjmp	Simulate_Far_Jmp

VI2F_PM_Eat_It:
	VMMjmp	Simulate_Iret

EndProc VDD_PM_Int_2F


;******************************************************************************
; VDD_Int_2F
;
;DESCRIPTION: Display critical section and enable/disable trapping.
;
;ENTRY: EBX = VM handle
;	EBP = client register pointer
;
;EXIT:	CF = 1 indicates interrupt is to be reflected, else it has been emulated
;
;USES:	Flags
;
BeginProc   VDD_Int_2F,PUBLIC

	mov	eax,[ebp.Client_EAX]
	cmp	ah,40h			    ; Q:My Int 2F?
	jz	SHORT VI2F_00		    ;	Y: go check subfunction
	stc
	ret				    ;	N: return, CF=1
VI2F_00:
IF 0
	cmp	ax,4040h
	jz	SHORT VI2F_Debug
ENDIF
	or	al,al
	je	SHORT VI2F_EnableIOTrap
	cmp	al,3
	je	SHORT VI2F_EnterCrit
	cmp	al,4
	je	SHORT VI2F_ExitCrit
	cmp	al,7
	je	SHORT VI2F_DisableIOTrap
	stc
	ret
VI2F_DisableIOTrap:
	mov	edi,ebx
	add	edi,[VDD_CB_Off]
	ClrFlag [edi.VDD_Flags], fVDD_NoTrap
	jmp	SHORT VI2F_SetTrap
VI2F_EnableIOTrap:
	dec	al
	mov	[ebp.Client_AL],al
	mov	edi,ebx
	add	edi,[VDD_CB_Off]
	SetFlag [edi.VDD_Flags],fVDD_NoTrap
VI2F_SetTrap:
	call	VDD_IO_SetTrap
	clc
	ret
VI2F_EnterCrit:
	bts	[Vid_Flags],fVid_CritBit
	jc	SHORT VI2F_Emulated
	mov	eax, Critical_Section_Boost
	VMMCall Adjust_Exec_Priority
VI2F_Emulated:
	clc
	ret
VI2F_ExitCrit:
	btr	[Vid_Flags],fVid_CritBit
	jnc	SHORT VI2F_Emulated
	mov	eax, -Critical_Section_Boost
	VMMCall Adjust_Exec_Priority
	clc
	ret

IF   0
VI2F_Debug:

	pushad
; Temp testing code goes here. Invoked by INT 2F, AX = 4040h from VM.
	stc
	popad
	ret
ENDIF
EndProc   VDD_Int_2F
VxD_CODE_ENDS

	END
