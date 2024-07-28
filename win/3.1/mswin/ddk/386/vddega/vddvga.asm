       TITLE   VDD - Virtual Display Device for VGA   version 2.10  2/88
;******************************************************************************
;
; VDDVGA - VGA routines for VDD
;
;   Author: MDW
;
;   (C) Copyright MICROSOFT Corp. 1986-1991
;
;   February, 1988
;
;DESCRIPTION:
;	This module has the VGA specific routines for VDD
;
;COMPILE:
;..\..\tools\masm5 -t -Mx -DEGA -DDEBUG -I..\..\INCLUDE -p vddvga.asm;
;
;******************************************************************************


	.386p

	INCLUDE VMM.inc
	INCLUDE OPTTEST.INC
	INCLUDE ega.inc
	INCLUDE debug.inc

IFDEF	VGA
	INCLUDE vga.inc

;******************************************************************************
; EXTRN routines
;
VxD_CODE_SEG
	EXTRN	VDD_Get_Mode:NEAR
	EXTRN	VDD_Mem_Chg:NEAR
VxD_CODE_ENDS

;******************************************************************************
; EXTRN data
;
VxD_DATA_SEG
	Extern_vgVDD
	EXTRN	VDD_CB_Off:DWORD

VxD_DATA_ENDS


VxD_CODE_SEG

;******************************************************************************
;
;   VDD_Init_DAC
;
;   DESCRIPTION:    initialize the DAC table with <index, 0, 0, 0> for each
;		    of the 256 entries
;
;   ENTRY:	    EDI = VDD CB ptr
;
;   EXIT:	    table is initialized
;
;   USES:	    flags
;
;==============================================================================

BeginProc VDD_Init_DAC, PUBLIC

	push	ecx
	push	esi
	mov	ecx, 256

	xor	esi, esi
init_dac_lp:
	mov	[esi*4][edi.VDD_DAC], esi
	inc	esi
	loop	init_dac_lp

	lea	esi, [edi.VDD_DAC]
	mov	[edi.VDD_PEL_Addr], esi
	mov	[edi.VDD_PEL_Idx], 1
	mov	[edi.VDD_PEL_Mode], DAC_Read_Mode

	pop	esi
	pop	ecx
	ret

EndProc VDD_Init_DAC


;******************************************************************************
;VDD_SaveCtlr	    Save any controller state not trapped
;
;DESCRIPTION: Saves the state of the VGA by reading the registers.  Registers
;	    that are trapped are not saved here. Note that background VMs and
;	    windowed VM are trapped and VM1 has a constant state. Note that the
;	    VDD_VGA_Savexxxx routines must be called in specific order.
;	    Specifically, CRTC, Graphics, Sequencer, Misc and Attribute.
;
;ENTRY: [VM_HandleRun] = VM currently running in VGA
;	Carry set for complete save
;	NOT Carry for
;
;EXIT:	VGA index registers restored, VDD_Stt structure modified
;
;USES:	none
;
;CALLS: VDD_VGA_Savexxxx routines
;
;ASSUMES:
;
BeginProc VDD_SaveCtlr, PUBLIC

	pushad
	pushfd
	mov	ebx,[Vid_VM_HandleRun]
	Queue_Out "VDD_SaveCtlr #ebx"
	Assert_VM_Handle ebx
	cmp	ebx,[Vid_VM_Handle]		; Q: Attached VM?
IFDEF	TLVGA
	jnz	VSV_Exit			;   N: Backgrnd VMs are trapped
ELSE
	jnz	SHORT VSV_Exit			;   N: Backgrnd VMs are trapped
ENDIF
	mov	edi,ebx
	add	edi,[VDD_CB_Off]
	cld

; Make sure video enabled
	mov	edx,pVGAEna
IFDEF	TLVGA
; Tseng labs has video subsystem enable on two diff ports depending
;	on value in feature control.  Also has another bit to worry about.

	mov	ah,fVGAEna
	TestMem [Vid_Flags],fVid_TLVGA
	jz	SHORT VSC_NotTLEnaRd
	mov	edx,pFeatRead
	in	al,dx
	and	al,3
	cmp	al,3
	mov	edx,pVGAEna
	jnz	SHORT VSC_NotTLEnaRd
	or	ah,8
	mov	edx,46E8h			; Other VGA Enable port
VSC_NotTLEnaRd:
	push	edx
	in	al,dx
	IO_Delay
	mov	[edi.VDD_STT.V_VGAEna],al
	or	al,ah				; Make sure enable is true
ELSE
	push	edx
	in	al,dx
	IO_Delay
	mov	[edi.VDD_STT.V_VGAEna],al
	or	al,fVGAEna			; Make sure enable is true
ENDIF
	out	dx,al
	IO_Delay

	call	VDD_VGA_SaveMisc		; Save Feat, Misc etc.

	Set3DxPorts

	call	VDD_VGA_SaveCRTC		; Save CRTC
	call	VDD_VGA_SaveGrp 		; Save Graphics controller
	call	VDD_VGA_SaveSeq 		; Save Sequencer
	mov	[edi.VDD_ModeEGA],-1

	TestMem [Vid_Flags], fVid_DOff		;Q: display off?
	jz	SHORT VSV_Partial		;   N: Partial save
	call	VDD_VGA_SaveDAC 		;   Y: Save DAC &
	call	VDD_VGA_SaveAttr		;      Attribute controller
IFDEF	V7VGA
	call	V7_SaveExtRegs
ENDIF
IFDEF	ATIVGA
	call	ATi_SaveExtRegs
ENDIF

VSV_Partial:
	call	VDD_Get_Mode

	Rest3?xPorts [edi.VDD_Stt.V_Misc]	; Restore 3Dx/3Bx addressing

; Restore VM's video subsystem enable register
	pop	edx
	mov	al, [edi.VDD_STT.V_VGAEna]
	out	dx,al
	IO_Delay

VSV_Exit:
	popfd
	popad
	ret
EndProc VDD_SaveCtlr

;*******
; Attribute controller
;   NOTE: ASSUME 3Dx addressing!!

BeginProc VDD_VGA_SaveAttr
	mov	esi,pAttrEGA
	mov	edx,pStatColr
	in	al,dx				; Set toggle to index register
	IO_Delay
	xchg	edx,esi
	TestMem [Vid_Flags],fVid_DOff		; Q: Globally turned off?
	jnz	SHORT VSV_A0			;   Y: don't read the index
	TestMem [edi.VDD_Flags],fVDD_AxRd	; Q: Index saved and mod'd?
	jnz	SHORT VSV_A0			;   Y: Don't read it again
	in	al,dx				; Read Attribute index register
	IO_Delay
	and	[edi.VDD_Stt.A_Indx],fVAI_Indx
	and	al,not fVAI_Indx
	or	[edi.VDD_Stt.A_Indx],al 	;	and save it
VSV_A0:
	push	edi
	mov	ecx,A_Color-A_Pal+1
ifdef	CTVGA
	dec	ecx				; no 14h for CTVGA
endif
	lea	edi,[edi.VDD_Stt.A_Pal]
	xor	ah,ah

; AH  = Attribute index
; ECX = number of registers
; EDX = Sequencer port
; ESI = Status port
; EDI = Sequencer state structure address
        cld
VSV_A_Loop:
	xchg	edx,esi
	in	al,dx				; Set toggle to index register
	IO_Delay
	xchg	edx,esi
	test	al,8
	jz	SHORT VSV_A_Loop		; Only read during VRTC
	mov	al,ah
	out	dx,al				; Select a register
	IO_Delay
	inc	edx
	in	al,dx				; Read it
	IO_Delay
	dec	edx
	stosb					; Save it
	inc	ah
	loop	VSV_A_Loop
	xchg	edx,esi
	in	al,dx
	IO_Delay
	xchg	edx,esi
	pop	edi
	mov	al,[edi.VDD_Stt.A_Indx]
	and	al,0FFh-fVAI_Indx
	TestMem [Vid_Flags],fVid_DOff		; Q: Is video off?
	jz	SHORT VSV_A1			;   N: Continue
	and	al,not fVAI_ScOn		;   Y: Keep it off
VSV_A1:
	out	dx,al				; Restore index register
	IO_Delay
	test	[edi.VDD_Stt.A_Indx],fVAI_Indx	; Q: 3C0 = access data?
	jz	SHORT VSV_A2			;   Y: all done
	xchg	edx,esi 			;   N: reset toggle to index
	in	al,dx
	IO_Delay
VSV_A2:
	ret
EndProc VDD_VGA_SaveAttr

BeginProc VDD_VGA_SaveMisc
IFDEF	CTVGA
;*******
; Enable CTVGA special register read mode
IFDEF	VGAMONO
%OUT Need some special logic to detect 3Bx addressing - bit to detect it
%OUT relies on using 3Dx or 3Bx addressing of CRTC...
ENDIF
	mov	edx,pIndx6845Colr
	mov	al,17h
	out	dx,al
	IO_Delay
	inc	edx
	in	al,dx
	IO_Delay
	push	eax
	or	al,10h
	out	dx,al				; Enable extended read mode
	IO_Delay
	dec	edx
	mov	al,xC_CT400
	out	dx,al
	IO_Delay
	inc	edx
	in	al,dx
	IO_Delay
        mov     [edi.VDD_Stt.C_CT400],al
	push	eax
	and	al,7Fh				; Read Misc and Feat regs
	out	dx,al
	IO_Delay
	dec	edx

;*******
; Miscellaneous register
	mov	al,xC_CTMiscRead
	out	dx,al
	IO_Delay
	inc	edx
	in	al,dx
	IO_Delay
	mov	[edi.VDD_Stt.V_Misc],al 	; Save Misc reg
	dec	edx

;*******
; Feature control register
	mov	al,xC_CTFeatRead
	out	dx,al
	IO_Delay
	inc	edx
	in	al,dx
	IO_Delay
	mov	[edi.VDD_Stt.V_Feat],al 	;	and save it

;*******
; Graphics control register
	mov	edx,pGrpIndx
	mov	al,xG_CTCtl
	out	dx,al
	IO_Delay
	inc	edx
	in	al,dx
	IO_Delay
	mov	[edi.VDD_Stt.xG_CTCtl],al	; Save Graphics control reg
	dec	edx
	mov	al,[edi.VDD_Stt.G_Indx]
	out	dx,al				; Restore Graphics ctlr index
	IO_Delay

;*******
; Restore the CRTC
	mov	edx,pIndx6845Colr
	mov	al,xC_CT400
	out	dx,al
	IO_Delay
	inc	edx
	pop	eax
	out	dx,al				; Restore 400 line register
	IO_Delay
	dec	edx
	mov	al,17h
	out	dx,al
	IO_Delay
	inc	edx
	pop	eax
	out	dx,al				; Restore reg 17h
	IO_Delay
	dec	edx
	mov	al,[edi.VDD_Stt.C_Indx] 	; Restore index
	out	dx,al
	IO_Delay
	ret
ELSE
;*******
; Miscellaneous register
	mov	edx,pMiscRead
	in	al,dx				; Read the Misc. register
	IO_Delay
	mov	[edi.VDD_Stt.V_Misc],al 	;	and save it

;*******
; Feature control register
	mov	edx,pFeatRead
	in	al,dx				; Read the Feature register
	IO_Delay
	mov	[edi.VDD_Stt.V_Feat],al 	;	and save it
	ret
ENDIF
EndProc VDD_VGA_SaveMisc

;*******					; Read CRTC data registers
; CRT controller
;   NOTE: ASSUME 3Dx addressing!!
BeginProc VDD_VGA_SaveCRTC
	mov	edx,pIndx6845Colr
	in	al,dx				; Read CRTC index register
	IO_Delay
	mov	[edi.VDD_Stt.C_Indx],al 	;	and save it
	and	[edi.VDD_Stt.C_Indx],C_IMsk	; Mask to index bits
IFDEF	CTVGA
	test	al,20h				; Q: Was it CTVGA special reg?
	jz	SHORT VSVC_0			;   N: continue
	or	[edi.VDD_Stt.C_Indx],0F0h	; Add in more bits
VSVC_0:
	mov	al,17h
	out	dx,al
	IO_Delay
	inc	edx
	in	al,dx
	IO_Delay
	push	eax
	or	al,10h
	out	dx,al				; Enable extended read mode
	IO_Delay
	dec	edx
	mov	al,xC_CT400
	out	dx,al
	IO_Delay
	inc	edx
	in	al,dx
	IO_Delay
	mov	[edi.VDD_Stt.C_CT400],al	; Save 400 line register
	or	al,80h				; Read CRTC cursor posn regs
	out	dx,al
	IO_Delay
	dec	edx
ENDIF
	push	edi
	mov	ecx,C_LnCmp-C_HTotal+1
	lea	edi,[edi.VDD_Stt.C_HTotal]
	xor	ah,ah

; AH  = CRTC index
; ECX = number of registers
; EDX = CRTC port
; EDI = CRTC state structure address
        cld
VSV_C_Loop:
	mov	al,ah
	out	dx,al				; Select a register
	IO_Delay
	inc	edx
	in	al,dx				; Read it
	IO_Delay
	dec	edx
	stosb					; Save it
	inc	ah
	loop	VSV_C_Loop
	pop	edi
IFDEF	CTVGA
        mov	al, 0FEh
        out	dx,al
	IO_Delay
        inc	edx
        in	al,dx
	IO_Delay
        mov	[edi.VDD_Stt.C_CTTempFE],al
        dec	edx

        mov	al,xC_CT400
        out	dx,al
	IO_Delay
        inc	edx
        mov	al,[edi.VDD_Stt.C_CT400] 	
        out	dx,al				; Restore 400 line register
	IO_Delay
        dec	edx
        mov	al,17h
        out	dx,al
	IO_Delay
        inc	edx
        pop	eax
        out	dx,al				; Restore reg 17h
	IO_Delay
        mov	[edi.VDD_Stt.C_Mode],al 	;   and put correct val in data
        dec	edx
ENDIF
	mov	al,[edi.VDD_Stt.C_Indx]
	out	dx,al				; Restore index register
	IO_Delay
	ret
EndProc VDD_VGA_SaveCRTC

;*******					; Read CRTC data registers
; Graphics controller
BeginProc VDD_VGA_SaveGrp
	mov	edx,pGrpIndx
	in	al,dx				; Read Graphics controller index
	IO_Delay
	mov	[edi.VDD_Stt.G_Indx],al 	;	and save it
	and	[edi.VDD_Stt.G_Indx],G_IMsk	; Mask to index bits
IFDEF	CTVGA
	test	al,20h				; Q: Was it CTVGA special reg?
	jz	SHORT VSV_G_IBM			;   N: continue
	or	[edi.VDD_Stt.G_Indx],0F0h	; Add in more bits
VSV_G_IBM:
ENDIF
	mov	ecx,G_Mask-G_SetRst+1
IFDEF	PVGA
	TestMem [Vid_Flags], fVid_PVGA		; Q: Paradise VGA?
	jz	SHORT VSVG0			;   N: read IBM regs only
; Note that count is one less since we are reading GRP.15 here, not in loop
	mov	ecx,G_PVGA5-G_SetRst		;   Y: read Paradise regs also
	mov	al,cl				;	Read GRP.15 first
	out	dx,al
	IO_Delay
	inc	edx
	in	al,dx
	IO_Delay
	mov	[edi.VDD_Stt.G_PVGA5],al	; Save value of Paradise VGA 5
	and	al,07
	ClrFlag [edi.VDD_Flags],fVDD_PVGA_Ena
	cmp	al,5
	jnz	SHORT VSVG00
	SetFlag [edi.VDD_Flags],fVDD_PVGA_Ena
VSVG00:
	mov	al,5
	out	dx,al
	IO_Delay
	dec	edx
VSVG0:
ENDIF
	push	edi
	lea	edi,[edi.VDD_Stt.G_SetRst]
	xor	ah,ah

; AH  = Graphics controller index
; ECX = number of registers
; EDX = Graphics controller port
; EDI = Graphics controller state structure address
        cld
VSV_G_Loop:
	mov	al,ah
	out	dx,al				; Select a register
	IO_Delay
	inc	edx
	in	al,dx				; Read it
	IO_Delay
	dec	edx
	stosb					; Save it
	inc	ah
	loop	VSV_G_Loop
	pop	edi

IFDEF	PVGA
	TestMem [Vid_Flags], fVid_PVGA		; Q: Paradise VGA?
	jz	SHORT VSVG1			;   N: read IBM regs only
; Restore the LOCK register
	mov	al,G_PVGA5-G_SetRst
	out	dx,al
	IO_Delay
	inc	edx
	mov	al,[edi.VDD_Stt.G_PVGA5]	; Restore Paradise VGA 5
	out	dx,al
	IO_Delay
	dec	edx
VSVG1:
ENDIF

; Restore the index
	mov	al,[edi.VDD_Stt.G_Indx]
	out	dx,al				; Restore index register
	IO_Delay
	ret
EndProc VDD_VGA_SaveGrp

;*******					; Read CRTC data registers
; Sequencer
BeginProc VDD_VGA_SaveSeq
	mov	edx,pSeqIndx
	in	al,dx				; Read CRTC index register
	IO_Delay
	and	al,S_IMsk
	mov	[edi.VDD_Stt.S_Indx],al 	;	and save it
	push	edi
	mov	ecx,S_MMode-S_Rst+1
	lea	edi,[edi.VDD_Stt.S_Rst]
	xor	ah,ah

; AH  = Sequencer index
; ECX = number of registers
; EDX = Sequencer port
; EDI = Sequencer state structure address
        cld
VSV_S_Loop:
	mov	al,ah
	out	dx,al				; Select a register
	IO_Delay
	inc	edx
	in	al,dx				; Read it
	IO_Delay
	dec	edx
	stosb					; Save it
	inc	ah
	loop	VSV_S_Loop
	pop	edi
	mov	al,[edi.VDD_Stt.S_Indx]
	out	dx,al				; Restore index register
	IO_Delay
	ret
EndProc VDD_VGA_SaveSeq

;******
; DAC registers
;
BeginProc VDD_VGA_SaveDAC

	push	edi
	lea	edi, [edi.VDD_DAC]
	xor	ecx,ecx
IFDEF VGA8514
	mov	ebx,[Vid_VM_HandleRun]
	VMMCall Test_Sys_VM_Handle
	mov	edx, p8514DACRindx
	je	short VVSDAC_1
ENDIF

	mov	edx,pDACRindx
VVSDAC_1:
	mov	al,cl
        cld
	stosb
	out	dx,al				; Set index then read data
	IO_Delay
	add	edx, 2				; mov	  edx,pDACData
.errnz pDACData - pDACRindx - 2
IFDEF VGA8514
.errnz p8514DACData - p8514DACRindx - 2
ENDIF
	in	al,dx				; Get red
	IO_Delay
	stosb
	in	al,dx				; Get green
	IO_Delay
	stosb
	in	al,dx				; Get blue
	IO_Delay
	stosb
	sub	edx, 2				; mov	  edx,pDACRindx
	inc	cl
	jnz	VVSDAC_1
VVS_exit:
	pop	edi
	ret
EndProc VDD_VGA_SaveDAC

;******************************************************************************
;
;   VDD_Rest_DAC
;
;   DESCRIPTION:    Restore DAC palette
;
;   ENTRY:	    EBX = VM handle
;		    EDI = VDD CB ptr
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc VDD_Rest_DAC, PUBLIC

Assert_VDD_ptrs ebx,edi, Pseudo_OK
	push	esi
	lea	esi, [edi.VDD_DAC]
	mov	ecx,256 			; 256 DAC registers
IFDEF VGA8514
	VMMCall Test_Sys_VM_Handle
	mov	edx, p8514DACWindx
	je	short VVRDAC_1
ENDIF
	mov	edx,pDACWindx
VVRDAC_1:
        cld
	lodsd
	out	dx,al				; Set index then data
	IO_Delay
	inc	edx				; mov	  edx,pDACData
.errnz pDACData - pDACWindx - 1
IFDEF VGA8514
.errnz p8514DACData - p8514DACWindx - 1
ENDIF
	shr	eax,8
	out	dx,al				; Output red
	IO_Delay
	shr	eax,8
	out	dx,al				; Output green
	IO_Delay
	shr	eax,8
	out	dx,al				; Output blue
	IO_Delay
	dec	edx				; mov	  edx,pDACWindx
	loopd	VVRDAC_1
VVRDAC_nochng:
	pop	esi
	ret

EndProc VDD_Rest_DAC

IFDEF V7VGA
;******************************************************************************
;
; V7_RestCRTC
;	Restore Video Seven CRTC extended registers. This changes only those
;	extended registers which deal with the CRTC.
;
; Entry Conditions:
;	EDI points to the VM control block
;	ESI = EDI.VDD_Stt
;
; Exit Conditions:
;	V7 extended registers restored.
;	No cpu registers modified
;
;******************************************************************************
BeginProc V7_RestCRTC, PUBLIC
Assert_VDD_ptrs ebx,edi, Pseudo_OK

	TestMem [Vid_Flags],fVid_V7VGA
	jz	V7RCRTC_Exit

	push	eax
	push	edx
	push	esi
;
; now, save the state of the extended registers access bit
;
        mov     edx,pSeqIndx
	mov	al,6
	out	dx,al
	inc	edx
	in	al,dx
	dec	edx
        cmp     al,1                    ; Q: Ext. enabled?
	je	SHORT V7RCRTC_10	;  Y: Don't need to enable again
	mov	ax,0ea06h		; (value to enable it)
	out	dx,ax			;  N: enable it
V7RCRTC_10:

;
; start the restore...
;
	lea	esi,[esi.VDD_V7ER]
	mov	ax,0100h		; Do all extended stuff
	out	dx,ax			;  in sync. reset
IF 0
;
; VEGA VGA SUPPORT
;
	mov	ax,04afh
	out	dx,ax
;
        mov     al,085h                 ; timing control
	out	dx,al
	inc	dx
	in	al,dx
	and	al,NOT 11010111b
	mov	ah,[esi+0adh-80h]
	and	ah,11010111b
	or	al,ah
	out	dx,al
	dec	dx
;
        mov     al,0adh                 ; 256 color mode page select
	out	dx,al
	inc	dx
	in	al,dx
	and	al,NOT 11000000b
	mov	ah,[esi+0adh-80h]
	and	ah,11000000b
	or	al,ah
	out	dx,al
	dec	dx
ENDIF
;
; Restore only the CRTC stuff...
;
	mov	eax,0a4h		; clock select
	mov	ah,[esi+eax-80h]
	out	dx,ax
	mov	ax,0a5h 		; some crtc reg...
	mov	ah,[esi+eax-80h]
	out	dx,ax
	mov	ax,0e1h 		; some crtc reg...
	mov	ah,[esi+eax-80h]
	out	dx,ax
        mov     ax,0f8h                 ; extended clock control
	mov	ah,[esi+eax-80h]
	out	dx,ax
	mov	ax,0fdh 		; extended timing select
	mov	ah,[esi+eax-80h]
	out	dx,ax
;
; register e0 is messy, it has to be combined with the current state
;
	mov	al,0e0h
	out	dx,al
	inc	dx
	in	al,dx
	mov	ah,[esi+0e0h-80h]	; al=ctlr, ah=crtc
	and	ax,025dah		; crtc=00100101, ctlr=11011010
	or	al,ah			; merged...
	out	dx,al
	dec	dx
;
; register f6 is messy, it has to be combined with the current state
;
	mov	al,0f6h
	out	dx,al
	inc	dx
	in	al,dx
	mov	ah,[esi+0f6h-80h]	; al=ctlr, ah=crtc
	and	ax,0f00fh		; crtc=11110000, ctlr=00001111
	or	al,ah			; merged...
	out	dx,al
	dec	dx
;
; register fc is messy, it has to be combined with the current state
;
	mov	al,0fch 		; compatibility control register
	out	dx,al
	inc	dx
	in	al,dx
	mov	ah,[esi+0fch-80h]	; al=ctlr, ah=crtc
	and	ax,0cd32h		; crtc=11001101, ctlr=00110010
	or	al,ah			; merged...
	out	dx,al
	dec	dx
;
; register ff is messy, it has to be combined with the current state
;
	mov	al,0ffh 		; extensions to 1mb BSR
	out	dx,al
	inc	dx
	in	al,dx
	mov	ah,[esi+0ffh-80h]	; al=ctlr, ah=crtc
	and	ax,0609fh		; crtc=01100000, ctlr=10011111
	or	al,ah			; merged...
;;;	or	al,10h			; bit 4 ALWAYS fixed (NO!!)
	out	dx,al
	dec	dx
;
        mov     ax,0300h
	out	dx,ax			; End sync. reset
;
	cmp	[edi.VDD_Stt.S_Rst.6],1 ; Q: Ext. enabled?
	mov	ax,0ea06h		; (value to enable it)
	je	SHORT V7RCRTC_40	;  Y: enable it then
	mov	ax,0ae06h		;     disable value
V7RCRTC_40:
	out	dx,ax
;
	pop	esi
	pop	edx
	pop	eax
;
V7RCRTC_Exit:
	ret

EndProc V7_RestCRTC


;******************************************************************************
;
; V7_RestCTLR
;	Restore cpu access to Video Seven VRAM. This changes only those
;	extended registers which deal with memory access.
;
; Entry Conditions:
;	EDI points to the VM control block
;
; Exit Conditions:
;	V7 extended registers restored.
;	No cpu registers modified
;
;******************************************************************************
BeginProc V7_RestCTLR, PUBLIC
Assert_VDD_ptrs ebx,edi, Pseudo_OK

	TestMem [Vid_Flags],fVid_V7VGA
	jz	V7RCTLR_Exit

	push	eax
	push	ecx
	push	edx
	push	esi
;
; now, save the state of the extended registers access bit
;
        mov     edx,pSeqIndx
	mov	al,6
	out	dx,al
	inc	edx
	in	al,dx
	dec	edx
        cmp     al,1                    ; Q: Ext. enabled?
	je	SHORT V7RCTLR_10	;  Y: Don't need to enable again
	mov	ax,0ea06h		; (value to enable it)
	out	dx,ax			;  N: enable it
V7RCTLR_10:

;
; start the restore...
;
	lea	esi,[edi.VDD_Stt.VDD_V7ER]
	mov	ecx,80h

IF 0
;
; VEGA VGA SUPPORT
;
	mov	ax,04afh
	out	dx,ax
;
	mov	eax,081h		; graphics pos 1
	mov	ah,[esi+81h-80h]
	out	dx,ax
	inc	eax			; graphics pos 2
	mov	ah,[esi+82h-80h]
	out	dx,ax
;
        mov     al,0adh                 ; 256 color mode page select
	out	dx,al
	inc	dx
	in	al,dx
	and	al,NOT 00110000b
	mov	ah,[esi+0adh-80h]
	and	ah,00110000b
	or	al,ah
	out	dx,al
	dec	dx
ENDIF
;
; we will restore all, except the CRTC registers
;
	mov	ah,080h
;
        cld
V7ReCT_20:
	lodsb
;
; skip the CRTC exceptions
;
	cmp	ah,083h
	jz	SHORT V7ReCT_skip
        cmp     ah,0a4h
	jz	SHORT V7ReCT_skip
	cmp	ah,0a5h
	jz	SHORT V7ReCT_skip
	cmp	ah,0e0h
	jz	SHORT V7ReCT_special1
	cmp	ah,0e1h
	jz	SHORT V7ReCT_skip
	cmp	ah,0e8h
	jz	SHORT V7ReCT_skip
        cmp     ah,0f6h
	jz	SHORT V7ReCT_special2
	cmp	ah,0f8h
	jz	SHORT V7ReCT_skip
	cmp	ah,0fch
	jz	SHORT V7ReCT_special3
	cmp	ah,0fdh
	jz	SHORT V7ReCT_skip
	cmp	ah,0FFh
	jz	SHORT V7ReCT_Special4
;
V7ReCT_Restore:
	xchg	al,ah			; restore the data...
	out	dx,ax
	xchg	al,ah
;
V7ReCT_skip:
        inc     ah
	loop	V7ReCT_20
;
	cmp	[edi.VDD_Stt.S_Rst.6],1 ; Q: Ext. enabled?
	mov	ax,0ea06h		; (value to enable it)
	je	SHORT V7RCTLR_30	;  Y: enable it then
	mov	ax,0ae06h		;     disable value
V7RCTLR_30:
	out	dx,ax
;
	pop	esi
	pop	edx
	pop	ecx
	pop	eax
V7RCTLR_Exit:
	ret

V7ReCT_special1:
;
; register e0
;
	xchg	ah,al			; ah=ctlr value, al=index
	out	dx,al			; setup the port
	inc	dx
	in	al,dx
	and	ax,0da25h		; crtc=00100101, ctlr=11011010
	or	al,ah
	out	dx,al
	dec	dx
	mov	ah,0e0h
	jmp	short V7ReCT_skip

V7ReCT_special2:
;
; register f6
;
        xchg    ah,al                   ; ah=ctlr value, al=index
	out	dx,al			; setup the port
	inc	dx
	in	al,dx
	and	ax,0ff0h		; crtc=11110000, ctlr=00001111
	or	al,ah
	out	dx,al
	dec	dx
	mov	ah,0f6h
	jmp	short	V7ReCT_skip
;
V7ReCT_special3:
;
; register fc
;
	xchg	ah,al			; ah=ctlr value, al=index
	out	dx,al			; setup the port
	inc	dx
	in	al,dx
	and	ax,032cdh		; crtc=11001101, ctlr=00110010
	or	al,ah
	out	dx,al
	dec	dx
	mov	ah,0fch
	jmp	short	V7ReCT_skip

V7ReCT_special4:
;
; register ff
;
	xchg	ah,al			; ah=ctlr value, al=index
	out	dx,al			; setup the port
	inc	dx
	in	al,dx			; ah=ctlr value, al=CRTC value
	and	ax,09F60h		; ctlr=10011111, crtc=01100000
	or	al,ah
;;;	or	al,10h			; bit 4 is ALWAYS fixed (NO!!!)
	out	dx,al
	dec	dx
	mov	ah,0ffh
	jmp	short	V7ReCT_skip

EndProc V7_RestCTLR

IF 0
;******************************************************************************
;
; V7_RestLatches
;	Restore latches
;
; Entry Conditions:
;	EDI points to the VM control block
;
; Exit Conditions:
;	Latches restored
;	No cpu registers modified
;
;******************************************************************************
BeginProc V7_RestLatches, PUBLIC
Assert_VDD_ptrs ebx,edi, Pseudo_OK

	TestMem [Vid_Flags],fVid_V7VGA
	jz	V7RLtch_Exit

Debug_Out "Restore latches for VM #EBX"
	push	eax
	push	ecx
	push	edx
	push	esi
;
; now, save the state of the extended registers access bit
;
        mov     edx,pSeqIndx
	mov	al,6
	out	dx,al
	inc	edx
	in	al,dx
	dec	edx
        cmp     al,1                    ; Q: Ext. enabled?
	je	SHORT V7ReLT_10 	;  Y: No enable necessary
	mov	ax,0ea06h		; (value to enable it)
	out	dx,ax			;  N: enable it, save
;
V7ReLT_10:
;
; Restore latches
;
	mov	al,0a3h
	mov	ecx,4
;
V7ReLT_30:
	mov	ah,[edi.VDD_Latches][ecx-1]
	out	dx,ax
	dec	al
	loop	V7ReLT_30
;
	cmp	[edi.VDD_Stt.S_Rst.6],1 ; Q: Ext. enabled?
	mov	ax,0ea06h		; (value to enable it)
	je	SHORT V7ReLT_10 	;  Y: save it for later
	mov	ax,0ae06h		;     disable value
;
V7ReLT_10:
	out	dx,ax

	pop	esi
	pop	edx
	pop	ecx
	pop	eax

V7RLtch_Exit:
	ret
EndProc V7_RestLatches
ENDIF

;******************************************************************************
;
; V7_ExtRegs_PreSR
;	Set up the extended registers memory addressing for saving/restoring
;	the video RAM
;
; Entry Conditions:
;	EDI = VDD CB ptr for VM we are saving or restoring
;
; Exit Conditions:
;	V7 extended registers cleared
;	No cpu registers modified except flags
;
;******************************************************************************
BeginProc V7_ExtRegs_PreSR, PUBLIC

IFDEF DEBUG
	push	ebx
	mov	ebx, edi
	sub	ebx, [VDD_CB_Off]
Assert_VDD_ptrs ebx,edi,Pseudo_OK
	pop	ebx
ENDIF

	TestMem [Vid_Flags],fVid_V7VGA
	jz	SHORT V7ER_PS_Exit

	push	eax
        push    edx
;
	mov	edx,pSeqIndx
	mov	ax,0ea06h
	out	dx,ax				; Enable extensions

	IO_Delay
	mov	ax,000f6h
	out	dx,ax				; Bank select = 0
	IO_Delay
	mov	al,0FFh
	out	dx,al				; Bank select = enabled
	IO_Delay
	inc	edx
	in	al,dx
	IO_Delay
	mov	ah, [edi.VDD_Stt.VDD_V7ER+7Fh]
	and	ah, 10h 			; VM's 256K Bank enable bit
	and	al, NOT 10h			; clear current setting
	or	al, ah				; or in VM's setting
	out	dx,al
	dec	edx
	IO_Delay
	mov	ax,00feh
	out	dx,ax
	IO_Delay
	mov	ax,00fch
	out	dx,ax
;
        pop     edx
	pop	eax
V7ER_PS_Exit:
        ret
EndProc V7_ExtRegs_PreSR

;******************************************************************************
;
; V7_SaveExtRegs
;	Save the cpu access to Video Seven VRAM. This reads only those
;	extended registers which deal with memory access.
;
; Entry Conditions:
;	EDI points to the VM control block
;
; Exit Conditions:
;	V7 extended registers saved.
;	No cpu registers modified except flags
;
;******************************************************************************
BeginProc V7_SaveExtRegs, PUBLIC
Assert_VDD_ptrs ebx,edi, Pseudo_OK
;;; save V7VGA ext regs on CLVGA too
	TestMem [Vid_Flags],<fVid_V7VGA+fVid_CLVGA> ; Q: Video 7 or Cirrus Logic?
	jz	short V7SCTLR_Exit

	push	eax
	push	ecx
	push	edx
	push	esi
	push	edi

;
; now, save the state of the extended registers access bit
;
	mov	edx,pSeqIndx
	mov	al,6
	out	dx,al
	inc	edx
	in	al,dx
	dec	edx
	mov	[edi.VDD_Stt.S_Rst.6],al; Save enable value
        cmp     al,1                    ; Q: Ext. enabled?
	mov	ax,0ea06h		; (value to enable it)
	je	SHORT V7SaCT_10 	;  Y: save it for later
	out	dx,ax			;  N: enable it, save
	mov	ax,0ae06h		;     disable value
;
V7SaCT_10:
	push	eax			; save for later restore
;
; start the restore...
;
	lea	esi,[edi.VDD_Stt.VDD_V7ER]
	mov	ecx,80h

	mov	ax,0100h		; Do all extended stuff
	out	dx,ax			;  in sync. reset
IF 0
;
; VEGA VGA SUPPORT
;
	mov	ax,04afh
	out	dx,ax
;
	mov	al,081h 		; graphics pos 1
	out	dx,ax
	inc	edx
	IO_Delay
        in      al,dx
	mov	[esi+81h-80h],al
	dec	edx
	mov	al,080h 		; graphics pos 2
	out	dx,ax
	inc	edx
	IO_Delay
        in      al,dx
	mov	[esi+82h-80h],al
	dec	edx
;
        mov     al,0adh                 ; 256 color mode page select
	out	dx,al
	inc	edx
	IO_Delay
        in      al,dx
	and	al,NOT 00110000b
	mov	ah,[esi+0adh-80h]
	and	ah,00110000b
	or	al,ah
	mov	[esi+0adh-80h],al
	dec	edx
ENDIF
;
	mov	edi,esi 		; edi point directly to our storage
;
; we will save all, except the CRTC registers
;
	mov	ah,080h
;
        cld
V7SaCT_20:
	xchg	ah,al
	out	dx,al
	xchg	ah,al
	inc	edx
	IO_Delay
        in      al,dx
	stosb				; save the value
	dec	edx
	inc	ah
	loop	V7SaCT_20
;;;	dec	edi
;;;	mov	al,10h
;;;	or	[edi],al		; Make sure bank select is set
;
        mov     ax,0300h
	out	dx,ax			; End sync. reset
	IO_Delay
;
	pop	eax			; Restore enable/disable state
        out     dx,ax
	IO_Delay
;
	pop	edi
        mov     al,[edi.VDD_Stt.S_Indx]
	out	dx,al			; Restore index register
;
        pop     esi
	pop	edx
	pop	ecx
	pop	eax
V7SCTLR_Exit:
	ret
EndProc V7_SaveExtRegs
ENDIF

IFDEF	ATIVGA
ATiVGA_extended_reg	EQU	01ceh
;
;******************************************************************************
;
; ATi_RestCRTC
;	Restore ATi VGAWONDER extended registers. This changes only 
;	affect the video sync timing 
;
; Entry Conditions:
;	EDI points to the VM control block
;	ESI = EDI.VDD_Stt
;
; Exit Conditions:
;	ATi extended registers restored.
;	No cpu registers modified
;
;******************************************************************************
BeginProc ATi_RestCRTC, PUBLIC
Assert_VDD_ptrs ebx,edi, Pseudo_OK

	TestMem [Vid_Flags],fVid_ATiVGA
	jz	ATiRCRTC_Exit

	push	eax
	push	edx
;
	cli
	mov	edx,pSeqIndx
	mov	ax,100h
	out	dx,ax			;sync reset
;
	mov	edx,ATiVGA_extended_reg
	mov	al,0b2h
	mov	ah,[esi.ATI_Reg+012h]	;no meaning for V4 and above
	out	dx,ax			;clock select 1 for Merlyn 2
;
	mov	al,0b5h
	mov	ah,[esi.ATI_Reg+015h]	;I/O ???, safer to restore in
	out	dx,ax			;in case of timing problem
;
	mov	al,0b8h
	mov	ah,[esi.ATI_Reg+018h]
	out	dx,ax			;clock divider 
;
	mov	al,0b9h
	mov	ah,[esi.ATI_Reg+019h]
	and	ah,07fh			;handle difference in Merlyn 2
	out	dx,ax			;for clock chip only
;
	mov	al,0beh
	mov	ah,[esi.ATI_Reg+01eh]	;for Merlyn 4 and above
	out	dx,ax
;
	mov	edx,03cch
	in	al,dx	
	mov	edx,03c2h		;toggle clock select information into
	out	dx,al			;the clock chip, to make it effective 
;
	mov	edx,pSeqIndx
	mov	ax,300h
	out	dx,ax			;end sync reset
	sti
;
	pop	edx
	pop	eax
;
ATiRCRTC_Exit:
	ret

EndProc ATi_RestCRTC


;******************************************************************************
;
; ATi_RestCTLR
;	Restore ATi VGAWONDER extended registers. This changes only 
;	affect the video memory organization 
;
; Entry Conditions:
;	EDI points to the VM control block
;
; Exit Conditions:
;	ATi extended registers restored.
;	No cpu registers modified
;
;******************************************************************************
BeginProc ATi_RestCTLR, PUBLIC
Assert_VDD_ptrs ebx,edi, Pseudo_OK

	TestMem [Vid_Flags],fVid_ATiVGA
	jz	ATiRCTLR_Exit

	push	eax
	push	ecx
	push	edx
	push	esi
;
	cli
	mov	edx,pSeqIndx
	mov	ax,100h
	out	dx,ax			;sync reset
;
	mov	edx,ATiVGA_extended_reg
	mov	al,0b0h
	mov	ah,[edi.VDD_Stt.ATI_Reg+010h]
	out	dx,ax			
;
	mov	al,0b1h
	mov	ah,[edi.VDD_Stt.ATI_Reg+11h]
	out	dx,ax
;
	mov	al,0b3h
	mov	ah,[edi.VDD_Stt.ATI_Reg+13h]
	out	dx,ax
;
	mov	al,0b6h
	mov	ah,[edi.VDD_Stt.ATI_Reg+16h]
	out	dx,ax
;
	mov	edx,pSeqIndx
	mov	ax,300h
	out	dx,ax			;end sync reset
	sti
;
;
	pop	esi
	pop	edx
	pop	ecx
	pop	eax
ATiRCTLR_Exit:
	ret


EndProc ATi_RestCTLR


;******************************************************************************
;
; ATi_ExtRegs_PreSR
;	Set up the extended registers memory addressing for saving/restoring
;	the video RAM
;
; Entry Conditions:
;	EDI = VDD CB ptr for VM we are saving or restoring
;
; Exit Conditions:
;	ATi extended registers cleared
;	No cpu registers modified except flags
;
;******************************************************************************
BeginProc ATi_ExtRegs_PreSR, PUBLIC


	TestMem [Vid_Flags],fVid_ATiVGA
	jz	SHORT ATiER_PS_Exit

	push	eax
        push    edx
;
	cli
	mov	edx,pSeqIndx
	mov	ax,100h
	out	dx,ax			;sync reset
;
	mov	ax,0604h
	out	dx,ax			; make sure no extension is on
;
	mov	edx,03dah
	in	al,dx
;
	mov	edx,03c0h
	mov	al,030h
	out	dx,al
	mov	al,1
	out	dx,al			;no 256 colors mode
;
	mov	edx,ATiVGA_extended_reg
	mov	al,0b1h			;extended register B1
	out	dx,al
	inc	edx
	in	al,dx
;;;	mov	al,[edi.VDD_Stt.ATI_Reg+11h]	; This instead of IN?
	mov	ah,al
	and	ah,087h
	mov	al,0b1h
	dec	edx
	out	dx,ax
;
	mov	al,0b6h			;extended register B6
	out	dx,al
	inc	edx
;;;	mov	al,[edi.VDD_Stt.ATI_Reg+16h]	; This instead of IN?
	in	al,dx
	mov	ah,al
	and	ah,0e7h
	mov	al,0b6h
	dec	edx
	out	dx,ax
;
	mov	al,0b3h			;extended register B3
	out	dx,al
	inc	edx
; Might want to get value in attached VM's data structure rather than reading
;	from the hardware here.
;;;	mov	al,[edi.VDD_Stt.ATI_Reg+13h]	; This instead of IN?
	in	al,dx
	mov	ah,al
	and	ah,0bfh			;Merlyn 4/5, bit4 hi memory enabled
	mov	al,0b3h			;should not have problem
	dec	edx
	out	dx,ax
;
	mov	al,0b0h			;extended register B0
	out	dx,al
	inc	edx
	in	al,dx
;;;	mov	al,[edi.VDD_Stt.ATI_Reg+10h]	; This instead of IN?
	mov	ah,al
	and	ah,0c1h			;should be OK if save and restore
	mov	al,0b0h			;are consistent.
	dec	edx
	out	dx,ax
;
	mov	edx,pSeqIndx
	mov	ax,300h
	out	dx,ax				;end sync reset
	sti
;
	pop	edx
	pop	eax
ATiER_PS_Exit:
        ret
EndProc ATi_ExtRegs_PreSR

;******************************************************************************
;
; ATi_SaveExtRegs
;	Save all the ATI VGAWONDER extended registers.
;
; Entry Conditions:
;	EDI points to the VM control block
;
; Exit Conditions:
;	ATi extended registers saved.
;	No cpu registers modified except flags
;
;******************************************************************************
BeginProc ATi_SaveExtRegs, PUBLIC
Assert_VDD_ptrs ebx,edi, Pseudo_OK
	TestMem [Vid_Flags],fVid_ATiVGA
	jz	ATiSCTLR_Exit

	push	eax
	push	edx
	push	edi
;
	mov	ah,0b0h
	mov	edx,ATiVGA_extended_reg
;
ATiVGA_save_xlp:
	mov	al,ah
	out	dx,al
	inc	edx
	in	al,dx
	dec	edx
	mov	[edi.VDD_Stt.ATI_Reg+10h],al
	inc	ah
	inc	edi
	cmp	ah,0bfh
	jb	ATiVGA_save_xlp
;
	pop	edi
	pop	edx
	pop	eax
ATiSCTLR_Exit:
	ret
EndProc ATi_SaveExtRegs

ENDIF

VxD_CODE_ENDS

ENDIF	; IBMVGA OR CTVGA

	END
