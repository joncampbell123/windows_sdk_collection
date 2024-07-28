TITLE vddstate.asm -
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1986-1991
;
;   Title:	vddstate.asm - This module keeps track of a VM's video state
;			       and reports that state to VMDOSAPP.
;
;   Author:	MDW
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;
;==============================================================================
	.386p

	INCLUDE VMM.INC
	INCLUDE OPTTEST.INC
	INCLUDE VDD.INC
	INCLUDE EGA.INC
	INCLUDE DEBUG.INC

;******************************************************************************
; EXTRN and PUBLIC data
;
VxD_CODE_SEG
	EXTRN	VDD_Mem_UpdOff:NEAR
	EXTRN	VDD_SetCopyMask:NEAR
	EXTRN	VDD_Mem_ModPag:NEAR
	EXTRN	VDD_Mem_ACopy:NEAR
	EXTRN	VDD_Save2:NEAR
IFDEF VGA
	EXTRN	VDD_SaveCtlr:NEAR
ENDIF
VxD_CODE_ENDS


VxD_DATA_SEG
	Extern_vgVDD
	EXTRN	VDD_SG0_JTab:DWORD

IFDEF VGA
	EXTRN	VDD_PhysA0000:DWORD
	EXTRN	VDD_2EGA_Start:DWORD
	EXTRN	VDD_Phys2EGA:DWORD
ENDIF


Txt_Rect3_Off	dd	160*25
Txt_Rect2_Off	dd	160*12
Txt_Rect1_Off	dd	160*3
Txt_Rect0_Off	dd	0

; Table of page masks for pages that VMDOSAPP wants to know about
PUBLIC	VDD_Mod_MskTab
VDD_Mod_MskTab	LABEL DWORD
	DD	000000001h		; mode 0, 40x25 alpha, 2 bytes/char
	DD	000000001h		; mode 1, 40x25 alpha, 2 bytes/char
	DD	000000001h		; mode 2, 80x25 alpha, 2 bytes/char
	DD	000000001h		; mode 3, 80x25 alpha, 2 bytes/char
	DD	00000000Fh		; mode 4, 320x200x2 graphics
	DD	00000000Fh		; mode 5, 320x200x2 graphics
	DD	00000000Fh		; mode 6, 640x200x1 graphics
	DD	000000000h		; mode 7 not supported
	DD	000000000h		; mode 8 not supported
	DD	000000000h		; mode 9 not supported
	DD	000000000h		; mode A not supported
	DD	000000000h		; mode B not supported
	DD	000000000h		; mode C not supported
	DD	00F0F0F0Fh		; mode D, 320x200 graphics, 4 32k planes
	DD	00F0F0F0Fh		; mode E, 640x200 graphics, 4 32k planes
	DD	000000000h		; mode F, not supported
	DD	07F7F7F7Fh		; mode 10,640*350 graphics, 4 28k planes
	DD	0000000FFh		; mode 11, 640x480 graphics, 1 38k pln
	DD	0FFFFFFFFh		; mode 12, 640x480 graphics, 4 38k plns
	DD	0000000FFh		; mode 13, 320x200x8 graphics, 1 64k pln

PUBLIC	VDD_Mod_MskTab2
VDD_Mod_MskTab2 LABEL DWORD
	DD	000000000h		; mode 0, 40x25 alpha, 2 bytes/char
	DD	000000000h		; mode 1, 40x25 alpha, 2 bytes/char
	DD	000000000h		; mode 2, 80x25 alpha, 2 bytes/char
	DD	000000000h		; mode 3, 80x25 alpha, 2 bytes/char
	DD	000000000h		; mode 4, 320x200x2 graphics
	DD	000000000h		; mode 5, 320x200x2 graphics
	DD	000000000h		; mode 6, 640x200x1 graphics
	DD	000000000h		; mode 7 not supported
	DD	000000000h		; mode 8 not supported
	DD	000000000h		; mode 9 not supported
	DD	000000000h		; mode A not supported
	DD	000000000h		; mode B not supported
	DD	000000000h		; mode C not supported
	DD	000000000h		; mode D, 320x200 graphics, 4 32k planes
	DD	000000000h		; mode E, 640x200 graphics, 4 32k planes
	DD	000000000h		; mode F, not supported
	DD	000000000h		; mode 10,640*350 graphics, 4 28k planes
	DD	000000003h		; mode 11, 640x480 graphics, 1 38k pln
	DD	003030303h		; mode 12, 640x480 graphics, 4 38k plns
	DD	0000000FFh		; mode 13, 320x200x8 graphics, 1 64k pln

VxD_DATA_ENDS

VxD_CODE_SEG

;******************************************************************************
;VDD_State_VMCreate
;
;DESCRIPTION:
;	Initializes current controller state for new VM.
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD CB ptr
;
;EXIT:	none
;
;USES:	Flags, EAX, ECDX, EDX, ESI
;
BeginProc VDD_State_VMCreate, PUBLIC

;*******
; Initialize EGA state structure with values from System VM
;
	VMMCall Test_Sys_VM_Handle
	jnz	SHORT copy_sys_state

	mov	esi, ebx
	add	esi,[VDD_CB_Off]		; ESI = SYS VM VDD ptr
IFDEF	VGA
	call	VDD_SaveCtlr
ENDIF	;VGA
	jmp	SHORT skip_sys_copy

copy_sys_state:
	push	ebx
	VMMCall Get_Sys_VM_Handle
	mov	esi,ebx
	pop	ebx
	add	esi,[VDD_CB_Off]		; ESI = SYS VM VDD ptr

	push	esi
	push	edi
	lea	edi,[edi.VDD_Stt]
.ERRNZ	EGA_State_struc MOD 4
	mov	ecx,size EGA_State_struc/4
	push	edi
	push	ecx
	lea	esi,[esi.VDD_Stt]		; current state of system VM
        cld
	rep movsd				;   is copied to new VM state
	pop	ecx
	pop	esi
	pop	edi
	push	edi
	lea	edi,[edi.VDD_SttCopy]		;   and copy of current state
	rep movsd
	pop	edi
	pop	esi

skip_sys_copy:

;*******
; Initialize I/O trap table for when VM is running w/o display
;
	mov	[edi.VDD_Routine],OFFSET32 VDD_SG0_JTab


IFDEF	PEGA

;*******
; Initialize virtualized VRTC and HRTC counters
;
	mov	al,VDD_VERT_PERIOD
	mov	[edi.VDD_VertCnt],al
	mov	al,VDD_HORIZ_PERIOD
	mov	[edi.VDD_HorizCnt],al

;*******
; Initialize PEGA specific CRTC state
;
	xor	eax,eax
	mov	DWORD PTR [edi.VDD_Stt.VC_PCUnUMsk],eax
	mov	DWORD PTR [edi.VDD_Stt.VC_PCUnLMsk],eax
	mov	DWORD PTR [edi.VDD_Stt.VC_PCScUMsk],eax
	mov	DWORD PTR [edi.VDD_Stt.VC_PCScLMsk],eax

	lea	eax,[edi.VDD_Stt.CRTC]
	mov	[edi.VDD_Cur_CRTC],eax
	mov	[edi.VDD_Stt.V_PSttCnt],-2
ENDIF


IFDEF VGA
IFDEF VGA8514
%OUT 2ND HALF OF VIDEO MEMORY FOR NOW!
	mov	edx,0A8000h			; Assume 2nd EGA at A80000h
	mov	[VDD_2EGA_Start],edx		; Set start addr of 2nd EGA
	sub	edx,0A0000h
	add	edx,[VDD_PhysA0000]
	mov	[VDD_Phys2EGA],edx
;	SetFlag [Vid_Flags],fVid_450		;   Y: Allow 2nd half EGA
ELSE
	xor	eax,eax
IFDEF	CTVGA
; Initialize CTVGA specific registers
	mov	BYTE PTR [edi.VDD_Stt.G_CTCtl],al
	mov	BYTE PTR [edi.VDD_Stt.C_CT400],al
ENDIF
	dec	eax
	mov	[edi.VDD_STT.V_VGAEna],al	; VGA enabled

	cmp	esi,edi 			; Creating SYS VM?
	jz	SHORT VMCM_003			;   Y: All done

	cmp	[VDD_2EGA_Start],0		; Q: Initialized already?
	jnz	SHORT VMCM_003			;   Y: Don't do it again
	mov	edx,0A8000h			; Assume 2nd EGA at A80000h
;*******
; Set up some global vars based on System VM's state when another VM starts up
;
;	test	[esi.VDD_Stt.V_Misc],80h	; Q: 350 rows windows driver?
;	jz	SHORT VN_SYS_SetMode		;   Y: Only uses 128k RAM
; Following is old code to support special 450 line windows display driver
;	mov	al,[esi.VDD_Stt.V_Misc]
;	cmp	al,0E3h 			; Q: mode 12 windows driver?
;	jnz	SHORT VN_SYS_SetMode		;   N: 2nd EGA starts at A8000
;	ClrFlag [Vid_Flags], fVid_450
;	mov	al,[esi.VDD_Stt.C_VDspEnd]	; Get vert. display end
;	cmp	al,0c2h 			; Q: Mode 12 short screen?
;	ja	SHORT VN_SYS_2EGA		;   N: inhibit 2nd EGA
;	SetFlag [Vid_Flags],fVid_450		;   Y: Allow 2nd half EGA
;	mov	edx,0A9000h			;      2nd EGA starts at A9000
;	jmp	SHORT VN_SYS_SetMode
;VN_SYS_2EGA:
IFDEF	VGA
	SetFlag [esi.VDD_Flags],fVDD_256	; SYS VM needs 256k RAM
ENDIF

VN_SYS_SetMode:
	mov	[VDD_2EGA_Start],edx		; Set start addr of 2nd EGA
	sub	edx,0A0000h
	add	edx,[VDD_PhysA0000]
	mov	[VDD_Phys2EGA],edx
VMCM_003:
ENDIF
ENDIF
	clc					; Return no error(CF=0)
	ret
EndProc VDD_State_VMCreate

;******************************************************************************
;VDD_State_GetStt      Build controller current state message
;
;DESCRIPTION:
;	Builds current controller state message at ESI
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD CB ptr
;	ESI = Place to build message
;	CF = 1, use VDD_Stt; CF = 0, use VDD_SttCopy
;
;EXIT:
;
;USES:	Flags, EAX, EDX
;
BeginProc VDD_State_GetStt, PUBLIC

Assert_VDD_ptrs ebx,edi

; Assume geting grabbed video state(CF = 0)
	lea	edx,[edi.VDD_SttCopy]		    ; EDX = grabbed ctlr state
	mov	ecx,esi
	lea	esi,[edi.VDD_CPg]
	movzx	eax,[esi.VPH_Mode]
	jnc	SHORT VSGS_0

; Get current VM video state
	call	VDD_Get_Mode			    ; AL = current mode
	lea	esi,[edi.VDD_Pg]		    ; ESI = VPH struc ptr
IFDEF	PEGA
	mov	edx,[edi.VDD_Cur_CRTC]
ELSE
	lea	edx,[edi.VDD_Stt.CRTC]
ENDIF
	stc
VSGS_0:
	pushfd
	call	VDD_Scrn_Size			    ; Set Rows, Cols, PgSz
	mov	[ecx.VDA_EGA_Mode],al		    ; Set current mode
	mov	al,[esi.VPH_Rows]
	mov	[ecx.VDA_EGA_Rows],al		    ; Set rows/page(text mode)
	mov	al,[edi.VDD_Stt.A_OvScn]	    ; AL = overscan color
	mov	[ecx.VDA_EGA_Colr],al		    ; Set border color
	mov	esi,ecx
	popfd
	jnc	SHORT VSGS_Palett		    ; Grab ignores cursor stuff

; Note that grab ignores these
	call	VDD_Get_EGA_Posn		    ; Get position
	mov	[esi.VDA_EGA_CurX],cx		    ; Set cursor X position
	mov	[esi.VDA_EGA_CurY],ax		    ; Set cursor Y position

; The following calculates the cursor start and end position based on
;	the values output to the registers. Note that the complicated
;	algorithm takes into account several different "features" of
;	varying EGA hardware, including how the cursor appears if END=START
;	and scan lines beyond the max scan line.

	TestMem [edi.VDD_Flags],fVDD_Hide	    ; Q: cursor hidden?
	jnz	short VSGS_None 		    ;	Y: return no cursor
	movzx	eax,[edx.C_CStart]		    ; AX = start scan line
	and	al,1Fh				    ; mask out other bits
	movzx	ecx,[edx.C_CEnd]		    ; CL = end scan line
	and	cl,1Fh				    ; mask out other bits
	mov	ch,[edx.C_MaxScan]		    ; CH = max scan line
	and	ch,0Fh
	cmp	al,ch				    ; Q: start beyond max?
	ja	short VSGS_None 		    ;	Y: set to null cursor
VSGS_2:
	cmp	al,cl				    ; Q: start = end?
	jnz	SHORT VSGS_3			    ;	N: continue
	inc	cl				    ;	Y: one line cursor
VSGS_3:
	or	cl,cl				    ; Q: is end at 0
	jnz	SHORT VSGS_4			    ;	N: continue
	mov	cl,ch				    ;	Y: make end max+1
	inc	cl
VSGS_4:
	cmp	al,cl				    ; Q: start before end?
	jb	SHORT VSGS_4a			    ;	Y: save values
	xchg	al,cl				    ;	N: No split crsr support
VSGS_4a:
	cmp	al,ch				    ; Q: start beyond max?
	jbe	SHORT VSGS_CType		    ;	N: Good value
	mov	cl,ch				    ;	Y: Give VM big block
	inc	cl
	xor	al,al
	jmp	short VSGS_CType

VSGS_None:
        mov     ax,1                    ; make Cursor Begin > Cursor End
        mov     cx,0

VSGS_CType:
	mov	[esi.VDA_EGA_CurBeg],ax 	    ; Set start scan line
	mov	ch,ah				    ; zero CH
	mov	[esi].VDA_EGA_CurEnd,cx 	    ; Set end scan line

VSGS_Palett:
; Return the palette
IFDEF PEGA
        lea     edx,[edi.VDD_Stt]
ENDIF
	mov	eax,DWORD PTR [edx.A_Pal]	    ; Pass palette registers
	mov	DWORD PTR [esi.VDA_EGA_Pal],eax
	mov	eax,DWORD PTR [edx.A_Pal+4]
	mov	DWORD PTR [esi.VDA_EGA_Pal+4],eax
	mov	eax,DWORD PTR [edx.A_Pal+8]
	mov	DWORD PTR [esi.VDA_EGA_Pal+8],eax
	mov	eax,DWORD PTR [edx.A_Pal+12]
	mov	DWORD PTR [esi.VDA_EGA_Pal+12],eax

IFDEF VGA
; check for GEM Ventura special DAC 0 entry
        mov     eax,DWORD PTR [edi.VDD_DAC]
        or      eax,eax
        jz      SHORT VSGS_NotGEM
; DAC zero entry is white instead of black,unusual but used by GEM Ventura
; return flag to indicate first DAC entry is nonzero for GEM Ventura, 
; grabber needs to use white for palette entry 0
	SetFlag [esi.VDA_EGA_Flags],fVDA_V_InvDAC
VSGS_NotGEM:
ENDIF

; Return the screen ON/OFF flag
	and	[esi.VDA_EGA_Flags],NOT fVDA_V_ScOff
	test	[edx.A_Indx],fVAI_ScOn		    ; Q: Is screen off?
	jnz	SHORT VSGS_6			    ;	N: proceed
	SetFlag [esi.VDA_EGA_Flags],fVDA_V_ScOff    ;	Y: Set screen off flag
VSGS_6:
	TestMem [edi.VDD_Flags],fVDD_MInit	    ; Q: VMInit done yet?
        jnz     SHORT VSGS_MainMemCheck             ;   Y: continue
	SetFlag [esi.VDA_EGA_Flags],fVDA_V_ScOff    ;	N: Set screen off flag
        jmp     SHORT VSGS_7
VSGS_MainMemCheck:        
	TestMem [edi.VDD_EFlags],fVDE_NoMain	    ; Q: No main mem?
        jz      SHORT VSGS_7                        ;   N: continue
	SetFlag [esi.VDA_EGA_Flags],fVDA_V_ScOff    ;	Y: Set screen off flag

VSGS_7:
; Return the horizontal cursor tracking flag
	and	[esi.VDA_EGA_Flags],NOT fVDA_V_HCurTrk
	TestMem [edi.VDD_Flags],fVDD_HCurTrk	    ; Q: HCurTrk ON?
	jz	SHORT VSGS_8			    ;	N: proceed
	SetFlag [esi.VDA_EGA_Flags],fVDA_V_HCurTrk  ;	Y: Set HCurTrk flag
VSGS_8:
	ret

EndProc VDD_State_GetStt

;******************************************************************************
;VDD_State_CtlrChgs	 Build controller change state by comparing prev. with cur.
;
;DESCRIPTION:
;	Compares the fields in the VM's VDD_Stt and VDDSttCopy and
;	sets bits in the message flag word pointed to by ESI. If only the cursor
;	position or size changed, just the cursor flag bit is set. If any
;	additional changes are detected, the controller change flag bit is set.
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD CB ptr
;	ESI = Place to build message
;
;EXIT:	EAX = VDD_Mod_Flag bits to pass to VMDOSAPP
;
;USES:	Flags, EAX
;
;ASSUMES:
;	VDD_ModeEGA is valid(VDD_Get_Mode has been called), this is called
;	only for windowed VMs.
;
BeginProc VDD_State_CtlrChgs, PUBLIC

Assert_VDD_ptrs ebx,edi
; First check whether screen is turned off
	and	[esi.VDD_Mod_Flag],NOT fVDD_M_ScOff ; Clear screen off flag

	TestMem [edi.VDD_Flags],fVDD_MInit	    ; Q: memory initialized?
	jz	SHORT VCC_Off			    ;	N: report screen off
	TestMem [edi.VDD_Flags],fVDD_ModeSet	    ; Q: Mode change ongoing?
	jnz	SHORT VCC_NoChg 		    ;	Y: report screen off
	test	[edi.VDD_Stt.A_Indx],fVAI_ScOn
	jnz	SHORT VCC_0
VCC_Off:
	SetFlag [esi.VDD_Mod_Flag],fVDD_M_ScOff     ; Set screen off flag
	test	[edi.VDD_SttCopy.A_Indx],fVAI_ScOn  ; Q: Screen off change?
	jnz	SHORT VCC_Chg_Ctl		    ;	Y: return ctlr chged
	jmp	SHORT VCC_1
VCC_0:
	test	[edi.VDD_SttCopy.A_Indx],fVAI_ScOn  ; Q: Screen off change?
	jz	SHORT VCC_Chg_Ctl		    ;	Y: return ctlr chged
VCC_1:
        cmp     [edi.VDD_LastMode],-1
        jz      SHORT VCC_Chg_Ctl
	mov	al,[edi.VDD_ModeEGA]
	test	al,-1				    ; Q: Mode change?
	js	SHORT VCC_Chg_Ctl		    ;	Y: return ctlr changed
; Also check the "43 line mode" register, for which the mode is the same
	mov	al,[edi.VDD_Stt.C_MaxScan]
	cmp	al,[edi.VDD_SttCopy.C_MaxScan]	    ; Q: Mode change?
	jne	SHORT VCC_Chg_Ctl		    ;	Y: return ctlr changed

; Check for change in display offset
	mov	ax,WORD PTR [edi.VDD_Stt.C_AddrH]
	cmp	ax,WORD PTR [edi.VDD_SttCopy.C_AddrH] ; Q: Offset change?
	jne	SHORT VCC_Chg_Ctl		    ;	Y: return ctlr changed

; Check for change in attributes
	mov	eax,DWORD PTR [edi.VDD_Stt.A_Pal]
	cmp	eax,DWORD PTR [edi.VDD_SttCopy.A_Pal] ; Q: Palette chg?
	jne	SHORT VCC_Chg_Ctl		    ;	Y: return ctlr changed
	mov	eax,DWORD PTR [edi.VDD_Stt.A_Pal+4]
	cmp	eax,DWORD PTR [edi.VDD_SttCopy.A_Pal+4] ; Q: Palette chg?
	jne	SHORT VCC_Chg_Ctl		    ;	Y: return ctlr changed
	mov	eax,DWORD PTR [edi.VDD_Stt.A_Pal+8]
	cmp	eax,DWORD PTR [edi.VDD_SttCopy.A_Pal+8]  ; Q: Palette chg?
	jne	SHORT VCC_Chg_Ctl		    ;	Y: return ctlr changed
	mov	eax,DWORD PTR [edi.VDD_Stt.A_Pal+12]
	cmp	eax,DWORD PTR [edi.VDD_SttCopy.A_Pal+12] ; Q: Palette chg?
	jne	SHORT VCC_Chg_Ctl		    ;	Y: return ctlr changed

	call	Vid_Chk_Cursor
	jne	SHORT VCC_Chg_Curs		    ;	Y: return cursor changed
VCC_NoChg:
	xor	eax,eax
	ret

; CRTC cursor size or address changed
VCC_Chg_Curs:
	mov	eax,fVDD_M_Curs 		    ; Set cursor changed flag
	clc
	ret
; CRTC register affecting display changed
VCC_Chg_Ctl:
	call	VDD_Get_Mode			    ; EAX = VM's video mode
	cmp	al,0Bh
	jz	SHORT VCC_NoChg 		    ; No changes for mode B
	mov	eax,fVDD_M_Ctlr+fVDD_M_Curs	    ; Set ctrlr chg flag
	clc
	ret
EndProc VDD_State_CtlrChgs

;******************************************************************************
;
; VDD_State_MemChgs
;
;DESCRIPTION:
;	This routine builds a state change structure at the location pointed
;	to by ESI. It returns the length in ECX
;
;	A single change at 0,0 is stored as 0,0,1,1
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD CB ptr
;	ESI = Place to build change structure
;
;EXIT:	ECX = length of memory change structure pointed to by ESI
;	Memory change structure possibly modified to reflect changes
;
;USES:	Flags, EAX, ECX, EDX
;
;ASSUMES:
;	Message, except for controller and cursor flag bits is zero
;	Mod_List will not be looked at unless fVDD_M_VRAM flag is set
;	any change box (scroll or modified) is specified by the row
;	and col. OUTSIDE the box. This is the way VMDOSAPP was spec'd
;
BeginProc VDD_State_MemChgs, PUBLIC

Assert_VDD_ptrs ebx,edi
	push	ebp				; EBP and EBX are scratch
	push	ebx
	call	VDD_Mem_UpdOff			; Update display offset

;*******
; Initialize Mod structure: no mem changes, length = 4 bytes
	mov	[esi.VDD_Mod_count], 0
	mov	ecx,4

	TestMem [edi.VDD_Flags],fVDD_MInit	; Q: memory initialized?
	jz	SHORT VMC_Exit			;   N: not valid just now
	call	VDD_Get_Mode			; EAX = video mode
	cmp	al,0Bh
	jz	SHORT VMC_Exit			; mode not valid just now
	cmp	al,7				; Q: EGA Graphics mode?
IFDEF	VGAMONO
	ja	VMC_EGAGraphics 		;   Y: Do graphics processing
	je	SHORT VMC_Text			;   N: Do Text mode
ELSE
	jae	VMC_EGAGraphics 		;   Y: Do graphics processing
ENDIF
	cmp	al,3				; Q: Text mode?
	ja	SHORT VMC_CGAGraphics		;   N: Do CGA graphics
VMC_Text:
        call    VDD_SetCopyMask                 ;   Y: Get copy pages mask
        mov     [edi.VDD_CPg.VPH_PgAccMsk],ecx  ; displayed pages are accessed
        push    eax
        call    VDD_Mem_ACopy                   ; alloc copy mem if necessary
        pop     eax
	mov	ecx,esi
	lea	esi,[edi.VDD_Pg]
	call	VDD_Mod_Text			; Build text changes, if any
VMC_Exit:
	pop	ebx
	pop	ebp
	ret
VMC_NoCGAChg:
	pop	eax
	pop	ecx
	jmp	VMC_Exit
;*******
; Build CGA graphics mode change structure
VMC_CGAGraphics:
	push	ecx
	push	eax
	cmp	[edi.VDD_CPg.VPH_Mode],al	; Q: Did mode change?
	mov	al,0Fh
	jnz	SHORT VMC_CGA_01		;   Y: Return all pages modified
	call	VDD_Mem_ModPag
	and	eax,0Fh
	jz	SHORT VMC_NoCGAChg
	or	[edi.VDD_CPg.VPH_PgAccMsk],eax
	push	eax
	mov	al,[edi.VDD_ModeEGA]
	call	VDD_Mem_ACopy
	pop	eax
VMC_CGA_01:
	mov	ah,al
	shr	al,2
	and	ah,03h
	or	al,ah
	mov	ecx,4096/(320/8)
	test	al,2				; Q: Just one page modified?
	jz	SHORT VMC_CGA_02		;   Y: return one page of lines
	mov	ecx,200 			;   N: return whole screen
VMC_CGA_02:
	SetFlag [esi.VDD_Mod_Flag],<fVDD_M_Type_Rect+fVDD_M_VRAM>
	xor	eax,eax
	mov	[esi.VDD_Mod_List.R_Left],ax
	mov	[esi.VDD_Mod_List.R_Top],ax
	inc	eax
	mov	[esi.VDD_Mod_Count],ax
	pop	eax
	cmp	al,5
	mov	eax,640
	ja	SHORT VMC_CGA_03
	mov	eax,320
VMC_CGA_03:
	mov	[esi.VDD_Mod_List.R_Right],ax
	mov	[esi.VDD_Mod_List.R_Botm],cx
	pop	ecx
	add	ecx,SIZE Rect
	jmp	VMC_Exit

;*******
; Build EGA graphics mode change structure
VMC_EGAGraphics:
	call	VDD_Mem_ModPag			; Q: Any changes?
	or	al,al
	jz	VMC_Exit			;   N: Exit, no changes
	SetFlag [esi.VDD_Mod_Flag],<fVDD_M_Type_Page+fVDD_M_VRAM>
	mov	[esi.VDD_Mod_List],al
	mov	[esi.VDD_Mod_Count],7		; Bits for 7 pages
	inc	ecx
	cmp	ebx,[Vid_VM_Handle2]		; Q: This VM attached 2nd EGA?
	jnz	VMC_Exit			;   N: No save
	pushad
	call	VDD_Save2			;   Y: Save current state
	popad
	jmp	VMC_Exit
EndProc VDD_State_MemChgs

;******************************************************************************
;VDD_State_Query    Determine if state of device is changed
;
;DESCRIPTION:
;
;ENTRY: EBX = VM Handle
;	EDI = VDD CB ptr
;
;EXIT:	CF = 1 indicates change detected
;
;USES:	Flags
;
BeginProc VDD_State_Query,PUBLIC

	push	eax
Assert_VDD_ptrs ebx,edi
	TestMem [edi.VDD_Flags],fVDD_Win	; Q: Windowed VM?
	jz	SHORT VSQ_Exit			;   N: report no change

%OUT commented out conditionals in VDD_State_Query
;	TestMem [edi.VDD_Event_Flags],fVDE_NoMain ; Q: Valid state?
;	jnz	SHORT VSQ_Exit			;   N: report no change
;	TestMem [edi.VDD_Flags],fVDD_MEna	; Q: Valid state?
;	jz	short VSQ_Exit			;   N: report no change

	TestMem [edi.VDD_Flags],fVDD_UpdAll2
	jnz	short VSQ_Change

	call	VDD_Mem_UpdOff			; Update display offset
	call	VDD_Mem_ModPag			; check video displayed pages
if2
%out only report changes to currently displayed pages
endif
	jnz	SHORT VSQ_Change		; CF=0 means no change
	call	Vid_Chk_Cursor			; ZF = 0 if cursor changed
	jnz	SHORT VSQ_Change
	test	[edi.VDD_LastMode],-1		; Q: Change in display mode?
	js	SHORT VSQ_Change		;   Y: Report controller change

; Finally, check to see if screen was turned off or on
	test	[edi.VDD_Stt.A_Indx],fVAI_SCOn
	jnz	SHORT VSQ_1
	test	[edi.VDD_SttCopy.A_Indx],fVAI_ScOn ; Q: Screen off change?
	jnz	SHORT VSQ_Change		;	    Y: return ctlr chged
	jmp	SHORT VSQ_Exit
VSQ_1:
	test	[edi.VDD_SttCopy.A_Indx],fVAI_ScOn ; Q: Screen off change?
	jnz	SHORT VSQ_Exit			;	    N: return no chg
VSQ_Change:
	pop	eax
	stc
	ret

VSQ_Exit:
	pop	eax
	clc
	ret

EndProc VDD_State_Query


%OUT Check this out for MONO
;******************************************************************************
;VDD_Mod_Text
;
;DESCRIPTION:
;	Modifies change structure to indicate rectangles of changed text.
;	Assumes the VDD_Scrn_Size has already been called for current mode,
;	setting VDD_Pg.VPH_LLen and VPH_PgSiz.
;
;ENTRY: EAX = mode
;	EDI = VDD CB ptr
;	ESI = VDD_Pg ptr
;	ECX = change structure ptr
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, EDX
;
BeginProc VDD_Mod_Text

	push	ebx
	push	ebp
	push	ecx

; assume no scroll modfs
	ClrFlag [edi.VDD_Flags], fVDD_ScrollModf
	TestMem [edi.VDD_Flags],fVDD_UpdAll2	; Q: Force a full screen Upd?
	jnz	SHORT VMT_AllChgd		;   Y: Report all changed

	call	VDD_Mem_ModPag			; EAX = mod'd pages mask
; EAX = mod'd pages normalized to first displayed page
	and	eax,0Fh 			; Q: Displayed pages dirty?
	jz	SHORT VMT_Exit			;   N: No changes
	call	Setup_Start_Count
	jz	SHORT VMT_Exit			; Disp'd pgs not mod'd, no chgs

; EBP = offset of first mod'd page from first displayed page
; ECX = bytes of displayed memory left to end from first mod'd page
;
	cmp	[edi.VDD_CPg.VPH_MState.VDA_Mem_Addr],0; Q: Copy mem alloc'd?
	jz	SHORT VMT_AllChgd		;   N: Report all changed

	mov	edx,[esp]			; EDX = chg struc ptr
	call	VMT_Comp			; Build change structure entries

        call    VMT_Check_Scroll		; Check for scrolling
; Set up return
VMT_Exit:
	pop	ecx
	pop	ebp
	pop	ebx
	cmp	[ecx.VDD_Mod_Count],0
	jz	SHORT VMT_NoChange
	ret
VMT_NoChange:
	xor	eax,eax
	mov	[edi.VDD_AccPages],eax
	mov	[edi.VDD_AccPages2],eax
	ret

; Here for low memory situations, return entire screen changed
VMT_AllChgd:
	mov	edx,[esp]
	xor	eax,eax
        mov     [edx.VDD_Mod_Flag],fVDD_M_VRAM+fVDD_M_Curs+fVDD_M_Type_Rect
	mov	[edx.VDD_Mod_List.R_Left],ax
	mov	[edx.VDD_Mod_List.R_Top],ax
	inc	eax
	mov	[edx.VDD_Mod_Count],ax
	movzx	eax,[esi.VPH_LLen]
	shr	eax,1				; Text is 2 bytes per column
	mov	[edx.VDD_Mod_List.R_Right],ax
	movzx	eax,[esi.VPH_Rows]
	mov	[edx.VDD_Mod_List.R_Botm],ax
	jmp	VMT_Exit


;******************************************************************************
;
; Setup_Start_Count - Depending on the dirty pages mask in EAX
;			 setup EBP = Start of search offset, ECX = search len
;
; For 25 line mode:
;   Offset = Offset within the displayed page and PgSz = VPH_PgSz. Both are
;	calculated by VDD_Scrn_Size.
;   Note that discontiguous changes are assumed contiguous (i.e. the
;	in between page is included in the compare).
;
;    Mask | EBP,start |  ECX,count  (if negative, no changes)
;   ------|-----------|-------
;   0001B | Offset    | MIN(PgSz,(4k-Offset))
;   0010B | 4K	      | MIN((PgSz+Offset-4K),(8k-4k))
;   0011B | Offset    | MIN(PgSz,(8K-Offset))
;   0100B | 8K	      | MIN(PgSz+Offset-8K,(12k-8k))
;   0101B | Offset    | MIN(PgSz,(12k-Offset)
;   0110B | 4K	      | MIN(PgSz+Offset-4K,(12k-4k))
;   0111B | Offset    | MIN(PgSz,(12k-Offset)
;   1000B | 12k       | MIN(PgSz+Offset-12K,(16k-12k))
;   1001B | Offset    | MIN(PgSz,(16k-Offset))
;   1010B | 4K	      | MIN((PgSz+Offset-4K),(16k-4k))
;   1011B | Offset    | MIN(PgSz,(16K-Offset))
;   1100B | 8K	      | MIN((PgSz+Offset-8K),(16k-8k))
;   1101B | Offset    | MIN(PgSz,(16k-Offset)
;   1110B | 4K	      | MIN((PgSz+Offset-4K),(16k-4k))
;   1111B | Offset    | MIN(PgSz,(16k-Offset)
;
; ENTRY: EAX = mask for dirty pages
;
; EXIT: Set ZF if displayed pages not mod'd
;	EBP = offset in the copy mem of the first possible mod'd byte
;	    =	offset of first mod'd char from first displayed page
;	ECX = count to end of mod'd displayed memory from first mod'd byte
;
IFDEF	DEBUG
PUBLIC Setup_Start_Count
ENDIF
Setup_Start_Count:
	and	eax,0Fh
	jz	SHORT SSC_Exit
	mov	ebp,[esi.VPH_MState.VDA_Mem_DPagOff]
	mov	ecx,[esi.VPH_PgSz]
	bsr	edx,eax
	inc	edx			    ; EDX = most sig bit number + 1
	shl	edx,12			    ; EDX = 4k * (most sig bit + 1)
	bsf	eax,eax 		    ; EAX = least sig bit number
	or	eax,eax 		    ; Q: Bit 0 set?
	jnz	SHORT SSC_NotFirst	    ;	N: 1st displayed page not mod'd

; Modification is contiguous from first displayed page
	sub	edx,ebp 		    ; EDX = last mod'd page - offset
	cmp	ecx,edx 		    ; ECX = MIN(EDX,ECX)
	jb	SHORT SSC_Exit
	mov	ecx,edx
SSC_Exit:
	ret

; First displayed page not modified,
SSC_NotFirst:
	shl	eax,12
	xchg	ebp,eax 		    ; EBP = 4k * (least sig bit - 1)
	add	ecx,eax 		    ; ECX = PgSz+Offset
	sub	ecx,ebp 		    ; ECX = PgSz+Offset-start
	jb	SHORT SSC_NoChanges	    ; Changes not displayed
	sub	edx,ebp 		    ; EDX = MSB*4k-(LSB-1)*4k
	cmp	ecx,edx 		    ; ECX = MIN(EDX,ECX)
	jb	SHORT SSC_Exit
	mov	ecx,edx
	ret
SSC_NoChanges:
	xor	eax,eax
	ret

;******************************************************************************
;
; VMT_Comp
;
;DESCRIPTION:
;	Compare current and copy state for one page, generate change buffer,
;	which consists of pairs of offsets to start and end of change
;	relative to first mod'd page. Note that display offset is always
;	zero for the copy memory
;
;ENTRY: EDX = change structure pointer
;	EDI = VDD CB ptr
;	EBP = offset of first mod'd page from first displayed page
;	ECX = bytes to end of displayed video memory from [EBP]
;
;EXIT:	
;
;USES:	Flags, EAX, EDX
;
IFDEF	DEBUG
PUBLIC VMT_Comp
ENDIF
VMT_Comp:
	push	edi
	push	esi
	push	ecx
	mov	ebx,edi 			; EBX = VDD CB ptr

;*******
; Search from end to first modified display memory
	movzx	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_DispPag]
	movzx	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_PgMap][esi]
	shl	esi,12
	add	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_Addr]
					; ESI = addr of first displayed char
	add	esi,ebp 		; ESI = address of first possible diff
	add	esi,ecx
	sub	esi,4			; ESI = last dword disp'd mem

	mov	edi,[ebx.VDD_CPg.VPH_MState.VDA_Mem_Addr]
					; EDI = addr of first displayed char
	add	edi,ebp 		; EDI = address of first possible diff
	add	edi,ecx
	sub	edi,4			; EDI = last dword disp'd mem

	shr	ecx,2
	std
	repe cmpsd			; ECX = last chg offset from EBP
	cld
	jz	VMTC_Done 		;   Exit if no changes
	shl	ecx,2
	add	ecx,ebp
	sub	ecx,[ebx.VDD_Pg.VPH_MState.VDA_Mem_DPagOff]

; ECX = last change offset from start of display
;
; The VMTC_CheckRect routine stores the rect in the change structure
	mov	eax,[Txt_Rect3_Off]	; EAX = Rect offset from ESI,EDI
	call	VMTC_CheckRect		; Check rectangle for changes
	jz	VMTC_Done 	        ; No more changes

	mov	eax,[Txt_Rect2_Off]	; EAX = Rect offset from ESI,EDI
	call	VMTC_CheckRect		; Check rectangle for changes
	jz	SHORT VMTC_Combine

	mov	eax,[Txt_Rect1_Off]	; EAX = Rect offset from ESI,EDI
	call	VMTC_CheckRect		; Check rectangle for changes
	jz	SHORT VMTC_Combine

	mov	eax,[Txt_Rect0_Off]	; EAX = Rect offset from ESI,EDI
	call	VMTC_CheckRect		; Check rectangle for changes

IFDEF  DEBUG
       jz      SHORT VMTC_D00
Debug_Out   "VDD: VMTC_CheckRect non zero at end of compare"
VMTC_D00:
ENDIF
    ;
    ; Combine rectangles if they are sufficiently close.
    ; Atmost there can be four rects, one each from Rect3,Rect2,Rect1,Rect0.
    ; 
    ;   |-------------|Rect0 - Lines 0-2
    ;   |             |
    ;   |-------------|
    ;   |             |Rect1 - Lines 3-11
    ;   |             |
    ;   |-------------|
    ;   |             |Rect2 - Lines 11-24
    ;   |             |
    ;   |             |
    ;   |             |
    ;   |-------------|
    ;   |             |Rect3 - Lines 25-43/52
    ;   |             |
    ;   |             |
    ;   \\           \\
    ;
    ; The rects are stored such that the top rect comes last,
    ; viz. Rect3/Rect2/Rect1/Rect0
    ; 
    ; if (only one rect)
    ;     continue;
    ; else if (two rects)
    ;     Try to combine the two;
    ; else if (three rects)     
    ;     Call CombineThree
    ; else if (four rects)     {
    ;     if (Combine Last Two)
    ;         dec Count, Call CombineThree;
    ;     else if (CombineThree)
    ;         Shift 4th rect up;
    ; }
    ;
    ;CombineThree()
    ;
    ; if (Combine last Two)
    ;     dec Count, CombineTwo;
    ; else if (CombineTwo)
    ;     Shift 3rd rect up 1 place
    ;           
VMTC_Combine:
	cmp	[edx.VDD_Mod_Count],1           
        jbe     SHORT VMTC_Done                 ; need atleast two rects
	cmp	[edx.VDD_Mod_Count],2
        je      SHORT VMTC_TwoRects             ; only two rects to combine
	cmp	[edx.VDD_Mod_Count],3
        je      SHORT VMTC_ThreeRects           ; Three rects to combine
VMTC_FourRects:                                 ; Four rects to combine
        push    edx
	lea	edx,[edx.VDD_Mod_List][8*2]     ; EDX = 3rd rect
        call    VMTC_CombineRect          ; Q: Combined 3rd and 4th?         
        pop     edx
        jz      SHORT VMTC_CombineAndShift; N: combine top three and shift 4th
        dec     [edx.VDD_Mod_Count]       ; Y: dec count 
VMTC_ThreeRects:
        call    VMTC_CombineThree
        jmp     SHORT VMTC_Done

VMTC_TwoRects:
        push    edx
	lea	edx,[edx.VDD_Mod_List]  ; EDX = 1st rect
        call    VMTC_CombineRect        ; Q: Combined 1st and 2nd?        
        pop     edx
        jz      SHORT VMTC_Done         ;  N: couldn't combine
        dec     [edx.VDD_Mod_Count]     ;  Y: Dec count
        jmp     SHORT VMTC_Done

VMTC_CombineAndShift:
        call    VMTC_CombineThree       ; Try to combine top three rects
        push    ebx
        xor     eax,eax
        mov     ax,[edx.VDD_Mod_Count]
        dec     ax                      ; Zero based index
        shl     eax,3                   ; multiply by 8
        lea     ebx,[edx.VDD_Mod_List]  
        add     ebx,eax                 ;copy 4th rect on to this one

	mov	ax,[edx.VDD_Mod_List.R_Left][3*8]
	mov	[ebx.R_Left],ax
	mov	ax,[edx.VDD_Mod_List.R_Right][3*8]
	mov	[ebx.R_Right],ax
	mov	ax,[edx.VDD_Mod_List.R_Top][3*8]
	mov	[ebx.R_Top],ax
	mov	ax,[edx.VDD_Mod_List.R_Botm][3*8]
	mov	[ebx.R_Botm],ax
        pop     ebx           

VMTC_Done:
	pop	ecx
	pop	esi
	pop	edi
	ret

;*****************************************************************************
;
; VMTC_CombineRect 
;
; ENTRY: EDX = Ptr to rect in the VDD_Mod_List
;
; EXIT: Combine Rect at EDX+8 into rect at EDX if possible.
;       ZF set if rects could not be combined.
;
;       Two rectangles are combined if there is 1 or less number of lines 
;       seperating them. VDD_Mod_Count not modified, caller should modify 
;       this if necessary.
;
;*****************************************************************************
IFDEF	DEBUG
PUBLIC VMTC_CombineRect
ENDIF
VMTC_CombineRect:
	mov	ax,[edx.R_Top]
	sub     ax,[edx.R_Botm][1*8]
        cmp     ax,2
        jg      SHORT VCR_Fail          ; WARNING! This has to be JG, not JA
                                        ; to account for AX being negative!
        mov     ax,[edx.R_Top][1*8]
        mov     [edx.R_Top],ax
        mov     [edx.R_left],0          ; Set Left=0,Right=80 for combined rects
        mov     [edx.R_Right],80
        mov     eax,1
        or      eax,eax
        ret
VCR_Fail:
        xor     eax,eax
        ret

;*****************************************************************************
;
; VMTC_CombineThree - try to combine the three rects in VDD_Mod_List  
;                     and adjust count.
; 
;ENTRY: EDX = change structure pointer
;
;
;                
;*****************************************************************************
IFDEF	DEBUG
public VMTC_CombineThree
ENDIF
VMTC_CombineThree:
        push    edx
	lea	edx,[edx.VDD_Mod_List][8]       ; EDX = 2nd rect
        call    VMTC_CombineRect                ; Q: Combine Last Two?     
        pop     edx
        jz      SHORT VCC_FirstTwo              ; N: try to combine first two
        dec     [edx.VDD_Mod_Count]             ; Y: dec count

        push    edx
	lea	edx,[edx.VDD_Mod_List]          ; EDX = 1st rect
        call    VMTC_CombineRect                ; Q: Combine top two?
        pop     edx
        jz      SHORT VCC_Done                  ;  N: couldn't combine
        dec     [edx.VDD_Mod_Count]             ;  Y: dec count
        jmp     SHORT VCC_Done
    ;
    ; Try Combining the first two rects
    ; if successful shift the 3rd up
    ;                
VCC_FirstTwo:
        push    edx
	lea	edx,[edx.VDD_Mod_List]          ; EDX = 1st rect
        call    VMTC_CombineRect                ; Q: Combine first two?
        pop     edx
        jz      SHORT VCC_Done                  ;  N: couldn't combine
        dec     [edx.VDD_Mod_Count]             ;  Y: dec count
    ;                
    ;   Shift 3rd rect up
    ;                
	mov	ax,[edx.VDD_Mod_List.R_Left][2*8]
	mov	[edx.VDD_Mod_List.R_Left][1*8],ax
	mov	ax,[edx.VDD_Mod_List.R_Right][2*8]
	mov	[edx.VDD_Mod_List.R_Right][1*8],ax
	mov	ax,[edx.VDD_Mod_List.R_Top][2*8]
	mov	[edx.VDD_Mod_List.R_Top][1*8],ax
	mov	ax,[edx.VDD_Mod_List.R_Botm][2*8]
	mov	[edx.VDD_Mod_List.R_Botm][1*8],ax

VCC_Done:
        ret
        
;******************************************************************************
;
; VMTC_CheckRect
;
;DESCRIPTION: Emits a rectangle if modification is found between [EAX] and
;	[ECX]
;
;ENTRY: EDX = change structure pointer
;	EBX = VDD CB ptr
;	ECX = offset from display start to last modified DWORD
;	EAX = offset from display start to start of current rectangle
;
;EXIT:	ZF = 1 indicates no more changes
;	ECX = offset from display start of last change before current rect
;
;ASSUMES: Text mode only
;	The rectangles are not overlapping and rectangles with
;	the larger valued offset are processed before those with a
;	smaller offset.
;	The offset from the mod'd page to the change is linear. That is,
;	all intervening pages are allocated.
;
;USES:	Flags, EAX, ECX, EDX, ESI, EDI, EBP
;
IFDEF	DEBUG
PUBLIC VMTC_CheckRect
ENDIF
VMTC_CheckRect:

	cmp	ecx,eax 			; Q: Chg end in this rectangle?
	jb	VMTC_RectExit			;   N: check next
						;   Y: Save end and find start
	movzx	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_DispPag]
	movzx	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_PgMap][esi]
	shl	esi,12
	add	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_Addr]
	add	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_DPagOff]
	add	esi,eax 			; ESI = main mem start addr

	mov	edi,[ebx.VDD_CPg.VPH_MState.VDA_Mem_Addr]
	add	edi,[ebx.VDD_CPg.VPH_MState.VDA_Mem_DPagOff]
	add	edi,eax 			; EDI = copy mem start addr
	push	esi
	push	edi
	push	eax				; Save rect start offset
	push	ecx				; Save offset to last change-4

; Now, while we have the registers, put the end of the rect in the structure
	mov	eax,ecx 			; EAX = offset from start
	movzx	ecx,[edx.VDD_Mod_Count] 	; ECX = rectangle index
	push	edx
	xor	edx,edx
	div	[ebx.VDD_Pg.VPH_LLen]		; AX = line #, DX = col*2
	add	edx,4				; DWORD accuracy!
	mov	ebp,eax 			; EBP = mod'd line #
	inc	eax				; One line below change
	shr	edx,1				; DX = column
	xchg	ebx,[esp]
	mov	[ebx.VDD_Mod_List.R_Botm][ecx*8],ax
	mov	[ebx.VDD_Mod_List.R_Right][ecx*8],dx
	pop	edx
	xchg	ebx,edx

; Compare forward from start of rect to first change or up to last change
	mov	ecx,[esp]
	mov	eax,[esp+4]
	shr	eax,2
	shr	ecx,2
	sub	ecx,eax
	inc	ecx
	cld
	repe cmpsd				; Find first chg in Rect
	shl	ecx,2				; Convert to bytes
; Store start of new rectangle in VDD_Mod structure
	pop	eax
	sub	eax,ecx 			; EAX = offset from start
	movzx	ecx,[edx.VDD_Mod_Count] 	; ECX = rectangle index
	mov	esi,edx
	xor	edx,edx
	div	[ebx.VDD_Pg.VPH_LLen]		; AX = line #,
	shr	edx,1				; DX = column
	mov	[esi.VDD_Mod_List.R_Top][ecx*8],ax
	cmp	eax,ebp 			; Q: One line rectangle?
	jz	SHORT VMTC_OneLine		;	Y: cur vals are right
	movzx	edx,[ebx.VDD_Pg.VPH_LLen]
	shr	edx,1				;	N: right = llen/2 = col
	mov	[esi.VDD_Mod_List.R_Right][ecx*8],dx
	xor	edx,edx 			;	    and left = 0
VMTC_OneLine:
	mov	[esi.VDD_Mod_List.R_Left][ecx*8],dx
IFDEF RECTDEBUG
	push	eax
	push	edx
	mov	dx,[esi.VDD_Mod_List.R_Right][ecx*8]
	mov	ax,[esi.VDD_Mod_List.R_Botm][ecx*8]
Queue_Out "                 to #AX row, #BX column",EAX,EDX
	pop	edx
	pop	eax
Queue_Out "VDD:Text changes at #AX row, #BX column" EAX,EDX
ENDIF
	inc	ecx
	mov	[esi.VDD_Mod_Count],cx		; Update rectangle count
	mov	edx,esi
	SetFlag [edx.VDD_Mod_Flag],<fVDD_M_Type_Rect+fVDD_M_VRAM>

; Find last change in next previous rectangle
	pop	ecx		; ECX = this rect offset
	shr	ecx,2		;   is len to start of mod'd dwords
	mov	eax,ecx
	pop	edi
	pop	esi
	jz	SHORT VMTC_RectExit
        sub     edi,4           ; last dword in the previous rect - copy mem
        sub     esi,4           ; last dword in the previous rect - main mem
	std
	repe cmpsd		; Find last chg, next rect
	cld
        pushfd                  ; safety - if ECX = 0
	shl	ecx,2		; Convert to bytes
        popfd
VMTC_RectExit:
	ret

;*****************************************************************************
;
; VMT_Check_Scroll
;
; Scan the VDD_Mod_List for rects which can be replaced by a Scroll rect and
; a smaller update rect.
; 
; For each rect in the VDD_Mod_List {
;    if (Rect > 1/3 of screen size) 
;       Try replacing it by a scroll rect and a smaller update rect.
; }         
;            
; ENTRY: EBX = VM CB ptr
;        EDX = Ch struct ptr
;
; USES: EAX,ECX
IFDEF	DEBUG
public VMT_Check_Scroll
ENDIF

VMT_Check_Scroll:
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi     
        push    edi

    ;
    ; Setup ESI = display start, EDI = Copy page start
    ;
	movzx	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_DispPag]
	movzx	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_PgMap][esi]
	shl	esi,12
	add	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_DPagOff]
	add	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_Addr]
	mov	edi,[ebx.VDD_CPg.VPH_MState.VDA_Mem_Addr]
	add	edi,[ebx.VDD_CPg.VPH_MState.VDA_Mem_DPagOff]

        movzx   ecx,[edx.VDD_Mod_Count]
        jecxz   SHORT VCS_Exit
        mov     eax,edx                 ; EAX = Ch Struct ptr
        lea     edx,[edx.VDD_Mod_List]  ; EDX = First rect in the list
VCS_NextRect:               
    ;
    ; Rect greater than 1/3 screen size?
    ;
        push    ecx
        push    eax
        mov     ecx,25/3
        cmp     [ebx.VDD_Pg.VPH_Rows],25
        jbe     SHORT VCS_00
        mov     ecx,43/3                ; use 43/3 for 52 line mode also
VCS_00:
        mov     ax,[edx.R_Botm]
        sub     ax,[edx.R_Top]
;        inc     ax
        cmp     ax,cx                   ; Q: Rect > 1/3 Screen Size?
        pop     eax
        pop     ecx
        jb      SHORT VCS_01            ;    N: Not worth scrolling
    ;
    ; Make ESI,EDI point to start of rect
    ;
	push	esi
	push	edi
        push    eax
        push    edx
        movzx   eax,[edx.R_Top]
        mul     [ebx.VDD_Pg.VPH_LLen]
        add     esi,eax                 
        add     edi,eax                 
        pop     edx
        pop     eax
        call    ConvertToScroll
	pop	edi
	pop	esi
VCS_01:
        add     edx,8                   ; point to next rect
        loopd   SHORT VCS_NextRect
VCS_Exit:
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        ret

;*****************************************************************************
;
; ConvertToScroll:
;       convert an update rect to a scroll rect + an update rect
;
; ENTRY:
;       EAX = ptr to Ch struct
;       EBX = VM CB ptr
;       EDX = ptr to a rect in the VDD_Mod_List
;       ESI = start of rect in displayed memory
;       EDI = start of rect in copy memory
;
; EXIT: If successful, converts the update rect to a scroll rect and adds
;       an update rect at the end of the VDD_Mod_List.
;
IFDEF	DEBUG
public ConvertToScroll
ENDIF
ConvertToScroll:

        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        
        mov     ecx,7                   ; check upto scrolling UP by 7 lines
CompareLoop:
        push    ecx
    ;
    ; Count Dwords for comparing.
    ; count = [# of lines in updrect-(7-ECX+1)] * VPH_LLen bytes
    ;
        movzx   eax,[edx.R_Botm]
        sub     ax,[edx.R_Top]
        add     eax,ecx         
        sub     eax,8

        push    edx
        mul     [ebx.VDD_Pg.VPH_LLen]
        pop     edx                     ; EAX = # of bytes in the update rect
        shr     eax,2                   ; EAX = dword count
        mov     ecx,eax
    ;
    ; point to update rect shifted by one more line(in copy pages)
    ;
        movzx   eax,[ebx.VDD_Pg.VPH_LLen]
        add     edi,eax
        cld
    ;
    ; compare update rect with copy rect shifted by (7-ECX+1) lines
    ;
        push    esi
        push    edi
        repe cmpsd
        pop     edi
    ;
    ; compute # of lines in the scroll rect
    ;
        mov     eax,esi
        pop     esi
        push    esi
        sub     eax,esi                 ; EAX = width of scrollrect in bytes
        push    edx
	xor	edx,edx
	div	[ebx.VDD_Pg.VPH_LLen]	 
        pop     edx                     ; AX = # of lines in Scrollrect
    ;
    ; Scroll rect big enough?
    ;
        mov     cx,[edx.R_Botm]
        sub     cx,[edx.R_Top]
        inc     cx
        shr     cx,1
        cmp     ax,cx                   ; Q: Scroll rect >= 1/2 of Update Rect
        jl      CTS_NextScroll          ;   N: 
public FoundScrollRect
FoundScrollRect:
IFDEF RECTDEBUG
        push    eax
        push    ebx
        mov     ax,[edx.R_Top]
        mov     bx,[edx.R_Botm]
Queue_Out "Given UpdRect - Top=#AX,Bot=#BX",EAX,EBX
        pop     ebx
        pop     eax
ENDIF
        pop     esi
        pop     ecx
	SetFlag [ebx.VDD_Flags],fVDD_ScrollModf
    ;
    ; ScrCnt = 7 - ECX + 1
    ;
        sub     ecx,7
        neg     ecx
        inc     ecx
    ;
    ; Found a sufficiently big scroll rect
    ;
    ; EDX -> Scroll Rect
    ; AX = # of lines in the scrollrect
    ; CX = # of lines to be scrolled
    ; For the new smaller update rect at the end of the list,
    ;   Top = Top of old Update Rect + AX + # of lines to be scrolled
    ;   Bottom  = Bottom of old UpdateRect
    ; For the Scroll Rect
    ;   Bottom = Top of new update rect
    ;
        push    esi
        mov     esi,[esp+6*4]            ; ESI = ptr to ch Struct
    ;
    ; Add the smaller update rect at the end of the list
    ;
        push    ecx
        movzx   ecx,[esi.VDD_Mod_Count]
        inc     [esi.VDD_Mod_Count]         ; one more rect to update
        lea     esi,[esi.VDD_Mod_List][ecx*8]; ESI = end of list
        pop     ecx
    ;
    ; ESI -> New smaller update rect
    ;
        mov     [esi.R_Left],0
        mov     [esi.R_Right],80

        mov     bx,[edx.R_Botm]
        mov     [esi.R_Botm],bx          ; Set Bottom
    ;
    ; For the new UpdateRect,
    ;   Top = Top of old UpdateRect + ScrollRect lines + # of lines to be scrolled
    ;   Ensure Top < Bottom
        add     ax,[edx.R_Top]
        add     ax,cx

        cmp     ax,[esi.R_Botm]
        jb      SHORT CTS_00
        mov     ax,[esi.R_Botm]
        dec     ax
CTS_00:
        mov     [esi.R_Top],ax

IFDEF RECTDEBUG
        push    eax
        push    ebx
        mov     ax,[esi.R_Top]
        mov     bx,[esi.R_Botm]
Queue_Out "New UpdRect - Top=#AX,Bot=#BX",EAX,EBX
        pop     ebx
        pop     eax
ENDIF

    ;
    ; Set bottom of the Scroll Rect = Top of new update rect
    ;
        mov     bx,[esi.R_Top]
        mov     [edx.R_Botm],bx         
        pop     esi
    ;
    ; Save old Update rect values in registers
    ;
        mov     ax,[edx.R_Right]
        mov     bx,[edx.R_Botm]
        mov     si,[edx.R_Left]
        mov     di,[edx.R_Top]
IFDEF RECTDEBUG
Queue_Out "ScrollRect - Top=#AX,Bot=#BX,",EDI,EBX
Queue_Out "             Count=#AX",ECX
ENDIF
    ;
    ; Set parameters for a scroll rect.
    ;
        mov     [edx.ScrCnt],cl
        mov     [edx.ScrFlgs],Scr_M_Scroll+Scr_M_FullWid+Scr_M_Up
        mov     [edx.ScrRgt],al
        mov     [edx.ScrBot],bl
        xchg    si,ax
        xchg    di,bx
        mov     [edx.ScrLft],al
        mov     [edx.ScrTop],bl
        mov     [edx.ScrFch],' '
        mov     [edx.ScrFatt],07h

        jmp     SHORT CTS_Exit

CTS_NextScroll:
        pop     esi
        pop     ecx
        dec     ecx
        jnz     CompareLoop
                
CTS_Exit:
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        ret

EndProc VDD_Mod_Text

;******************************************************************************
;VDD_State_Update
;
;DESCRIPTION:
;	Copies the fields in the VM's VDD_Stt to VDDSttCopy
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD control block data ptr
;
;EXIT:
;
;USES:	Flags, ECX
;
;CALLS:
;
BeginProc VDD_State_Update, PUBLIC

Assert_VDD_ptrs ebx,edi
	pushfd

	push	esi
	push	edi
	lea	esi,[edi.VDD_Stt]		; ESI = current state ptr
	lea	edi,[edi.VDD_SttCopy]		; EDI = copy of state ptr
	mov	ecx,size EGA_State_struc/4	; ECX = size of state
.ERRNZ size EGA_State_struc mod 4
	cld
	rep movsd
	pop	edi
	pop	esi
	mov	cl,[edi.VDD_ModeEGA]
	mov	[edi.VDD_LastMode],cl
	ClrFlag [edi.VDD_Flags],fVDD_HCurTrk	; Cancel horiz cursor track
	popfd
	ret
EndProc VDD_State_Update

;******************************************************************************
;Vid_Chk_Cursor 	Determine if Cursor changed state
;
;DESCRIPTION:
;
;ENTRY: EBX = VM handle
;	EDI = VDD CB ptr
;
;EXIT:	ZF = 0 indicates change detected
;
;USES:	Flags, EAX
;
BeginProc Vid_Chk_Cursor

Assert_VDD_ptrs ebx,edi
; Check for change in cursor
	mov	ax,WORD PTR [edi.VDD_Stt.C_CAddrH]
	cmp	ax,WORD PTR [edi.VDD_SttCopy.C_CAddrH] ; Q: Curs off chg?
	jne	SHORT VDD_CC_Change		;   Y: return cursor changed
	mov	ax,WORD PTR [edi.VDD_Stt.C_CStart]
	cmp	ax,WORD PTR [edi.VDD_SttCopy.C_CStart] ; Q: Curs scan chg?
VDD_CC_Change:
	ret
EndProc Vid_Chk_Cursor



;******************************************************************************
;VDD_Scrn_Size	    Text size calculations
;
;DESCRIPTION:
;	Set value for VPH_PgSz, VPH_LLen and VPH_Rows.	Although VPH_LLen
;	is a word value, the below code makes use of the fact that it
;	never greater than 255.
;
;ENTRY: EAX = mode(text)
;	EDX = CRTC structure pointer
;	ESI = VDD CB VPH structure ptr (either main mem or copy mem)
;
;EXIT:	none
;
;USES:	Flags
;
BeginProc VDD_Scrn_Size, PUBLIC

Assert_VDD_ptrs ebx,edi
	push	eax
	push	ecx
	push	edx
IFDEF	VGAMONO
	cmp	al,7
	je	SHORT VSS_Text
ENDIF
	cmp	al,3
	ja	SHORT VSS_Grap
VSS_Text:
	cmp	al,2
	mov	al,80*2
	jae	SHORT VSS_1
	mov	al,40*2
VSS_1:
	mov	[esi.VPH_LLen],ax		; MUST BE BYTE VALUE
	movzx	ecx,[edx.C_MaxScan]		; FOR BELOW CODE TO WORK!!
	movzx	eax,[edx.C_VDspEnd]
	and	cl,01Fh
	inc	cl
IFDEF	VGA
	bt	DWORD PTR [edx.C_Ovflw],6
	rcl	ah,1
ENDIF
	bt	DWORD PTR [edx.C_Ovflw],1
	rcl	ah,1
	inc	eax
	xor	edx,edx 			    ; use DX:AX to be safe
	div	cx
	mov	[esi.VPH_Rows],al
	mov	cl, BYTE PTR [esi.VPH_LLen]	    ; VPH_LLen < 256 !!
	mul	cl				    ; VPH_LLen < 256 !!
	mov	[esi.VPH_PgSz],eax
VSS_Ex:
	pop	edx
	pop	ecx
	pop	eax
	ret
VSS_Grap:
	cmp	al,6
	ja	SHORT VSS_EGA
	mov	[esi.VPH_LLen],80		    ; 80 byte lines
	mov	[esi.VPH_Rows],200		    ; 200 scan lines
	mov	[esi.VPH_PgSz],8192+(80*100)	    ; odd scans offset by 8192
	jmp	VSS_Ex
VSS_EGA:
	mov	[esi.VPH_LLen],0		    ; Undefined
	mov	[esi.VPH_Rows],0		    ; Undefined
	mov	ecx,VSS_PgSz-(4*0Dh)[eax*4]
	mov	[esi.VPH_PgSz],ecx
	jmp	VSS_Ex
; Table of page sizes for modes greater than D
IFDEF	VGAMONO
VSS_PgSz    DD	(320*200)/8,(640*200)/8,(640*350)/8,(640*350)/8
ELSE
VSS_PgSz    DD	(320*200)/8,(640*200)/8,0,(640*350)/8
ENDIF
IFDEF	VGA
	    DD	(640*450)/8,(640*450)/8,320*200
ENDIF
EndProc VDD_Scrn_Size

;******************************************************************************
;VDD_Get_Mode	    Determine current mode
;
;DESCRIPTION:
;	Determine current mode from controller state
;
;ENTRY: EBX = control block ptr
;
;EXIT:	EAX = mode
;	[EBX.VDD_ModeEGA] = mode
;
;USES:	Flags
;
;ASSUMES:
;	Modes 0,2,8-A are invalid(0 and 2 are coerced to 1 and 3)
;
BeginProc VDD_Get_Mode, PUBLIC

IFDEF	EGA
%OUT	can we use EDI here instead?
	push	ebx
	add	ebx,[VDD_CB_Off]
	movzx	eax,[ebx.VDD_ModeEGA]		; Get last mode
	or	al,al				; Q: Mode already set?
	js	short VDD_GM_1			;   N: Go figure out mode
	pop	ebx
	ret

VDD_GM_1:
	push	edx
IFDEF	PEGA
	mov	edx,[ebx.VDD_Cur_CRTC]		; EDX = CRTC ptr
ELSE
	lea	edx,[ebx.VDD_Stt.CRTC]		; EDX = CRTC ptr
ENDIF
	test	[ebx.VDD_STT.G_Misc],1		; Q: Text?
	jz	short VDD_GM_Txt		;   Y: Go handle text modes
	test	[ebx.VDD_STT.G_Mode],020h 	; Q: Odd/Even mode?
	jnz	short VDD_GM_4			;   Y: May be CGA graphics mode
VDD_GM00:
	movzx	eax,[edx.C_VDspEnd]
	bt	dword ptr [edx.C_Ovflw],1
	rcl	ah,1				; EAX = lines displayed
	inc	eax
	cmp	eax,350 			; Q: 350 lines?
	mov	ah,0
	je	short VDD_GM_10 		;   Y: mode 10h
; Must be either 200 lines or 200 lines doubled ;   N: mode 6, D or E
	test	[edx.C_Mode],1			; Q: Mode 6 mem addressing?
	jz	short VDD_GM_6			;   Y: Is mode 6
	cmp	[edx.C_HDisp],40		; Q: 320 PELS per line?
	jbe	short VDD_GM_D			;   Y: must be mode D
	mov	al,0Eh				;   N: mode E
	jmp	short VDD_GM_Ex
VDD_GM_Txt:
	cmp	[edx.C_HDisp],40		; Q: 320 PELS per line?
	mov	al,1
	jbe	short VDD_GM_T0 		;   Y: must be mode 1
	mov	al,3				;   N: must be mode 3
VDD_GM_T0:
	test	[ebx.VDD_Stt.S_Mask],4		; Q: Writing plane 2
	jz	short VDD_GM_Ex 		;   N: normal text mode
	mov	al,0Bh				;   Y: must be loading font
VDD_GM_Ex:
	mov	[ebx.VDD_ModeEGA],al		; Save the mode.
	pop	edx
	pop	ebx
	ret
VDD_GM_4:
        test    [ebx.VDD_Stt.S_ClMode],08h      ; Q: Dot clock divided by 2?
        jz      SHORT VDD_GM00                  ;   N: cannot be mode 4/5
	mov	al,4
	jmp	short VDD_GM_Ex
VDD_GM_6:
	mov	al,6
	jmp	short VDD_GM_Ex
VDD_GM_D:
	mov	al,0Dh
	jmp	VDD_GM_Ex
VDD_GM_E:
	mov	al,0Eh
	jmp	VDD_GM_Ex
VDD_GM_10:
	mov	al,10h
	jmp	VDD_GM_Ex
ENDIF

IFDEF VGA

	push	ebx
	add	ebx,[VDD_CB_Off]
	movzx	eax,[ebx.VDD_ModeEGA]		; Get last mode
	or	al,al				; Q: Mode already set?
	js	SHORT VDD_GEM_1 		;   N: Go figure out mode
	pop	ebx
	ret

VDD_GEM_1:

	push	edx
	lea	edx,[ebx.VDD_Stt.CRTC]		; EDX = CRTC ptr
	test	[edx.G_Misc],1			; Q: Text?
	jz	SHORT VDD_GM_Txt		;   Y: Go handle text modes
	test	[ebx.VDD_STT.G_Mode],020h 	; Q: Odd/Even mode?
	jnz	SHORT VDD_GM_4			;   Y: May be CGA graphics mode
VDD_GM00:
ifdef	CTVGA
	and	[edx.C_Ovflw],1fh		; zero unused bits
endif
	movzx	eax,[edx.C_VDspEnd]
	bt	DWORD PTR [edx.C_Ovflw],6
	rcl	ah,1				; EAX = lines displayed
	bt	DWORD PTR [edx.C_Ovflw],1
	rcl	ah,1				; EAX = lines displayed
	inc	eax
	cmp	eax,480 			; Q: 480 lines?
	je	SHORT VDD_GM_VGA		;   Y: mode 11h or 12h
	cmp	eax,350 			; Q: 350 lines?
	mov	ah,0
	je	SHORT VDD_GM_10 		;   Y: mode 10h
	mov	al,13h
ifdef	CTVGA
	test	[ebx.VDD_Stt.G_CTCtl], 08h	; GRF7 bit 3
else
        test    [edx.S_MMode],08h               ; Q: Mode 13h special bit?
;	test	[edx.A_Mode],40h		; Q: Mode 13h special bit?
endif
	jnz	SHORT VDD_GM_Ex 		;   Y: return 13h
	test	[edx.C_Mode],1			; Q: Mode 6 mem addressing?
	mov	al,6
	jz	SHORT VDD_GM_Ex 		;   Y: Is mode 6
	cmp	[edx.C_HDisp],40		; Q: 320 PELS per line?
	mov	al,0Dh
	jbe	short VDD_GM_Ex 		;   Y: must be mode D
VDD_GM_E:
	mov	al,0Eh				;   N: mode E
	jmp	SHORT VDD_GM_Ex
VDD_GM_Txt:
	cmp	[edx.C_HDisp],40		; Q: 320 PELS per line?
	mov	al,1
	jbe	SHORT VDD_GM_T0 		;   Y: must be mode 1
	mov	al,7				;   N: must be mode 3 or 7
IFDEF	VGAMONO
	test	[edx.V_Misc],1			; Q: Monochrome?
	jz	SHORT VDD_GM_T0 		;   Y: Is mode 7
	mov	al,3				;   N: Is mode 3
ENDIF
VDD_GM_T0:
	test	[edx.S_Mask],4			; Q: Writing plane 2?
	jz	SHORT VDD_GM_Ex 		;   N: normal text mode
	mov	al,0Bh				;   Y: must be loading font
VDD_GM_Ex:
	mov	[ebx.VDD_ModeEGA],al		; Save the mode.
	pop	edx
	pop	ebx
	ret
VDD_GM_4:
        test    [edx.S_ClMode],08h              ; Q: Dot clock divided by 2?
        jz      SHORT VDD_GM00                  ;   N: cannot be mode 4/5
	mov	al,4
	jmp	SHORT VDD_GM_Ex
VDD_GM_10:
IFDEF	VGAMONO
	test	[edx.V_Misc],1			; Q: Monochrome?
	jz	SHORT VDD_GM_0F 		;   Y: Must be mode F
ENDIF
	mov	al,10h
	jmp	VDD_GM_Ex
VDD_GM_VGA:
	mov	eax,12h
	cmp	DWORD PTR [edx.A_Pal],03F3F3F00h
	jnz	VDD_GM_Ex
	cmp	DWORD PTR [edx.A_Pal+4],03F3F3F3Fh
	jnz	VDD_GM_Ex
	cmp	DWORD PTR [edx.A_Pal+8],03F3F3F3Fh
	jnz	VDD_GM_Ex
	cmp	DWORD PTR [edx.A_Pal+12],03F3F3F3Fh
	jnz	VDD_GM_Ex
;hack for softkick/Ventura publisher which uses a mode 11 palette in mode 12
        test    [edx.G_Misc],02h       ; Q: Odd planes chained to even planes?
        jnz     VDD_GM_Ex              ;   Y: mode 12
        
	mov	al,11h
	jmp	VDD_GM_Ex
VDD_GM_0F:
	mov	al,0Fh
	jmp	VDD_GM_Ex

ENDIF

EndProc VDD_Get_Mode


;******************************************************************************
;VDD_Get_EGA_Posn	Return current character position
;
;DESCRIPTION:
;	Determine current position from controller state
;
;ENTRY: EDX = VDD_Stt ptr
;
;EXIT:	ax = Current Line for cursor
;	cx = Current Offset within line
;
;USES:	Flags, EAX, ECX
;
;CALLS:
;
;ASSUMES:
;	EGA
;
BeginProc VDD_Get_EGA_Posn

	push	edx
	movzx	ecx,[edx.C_AddrL]
	mov	ch,[edx.C_AddrH]
	movzx	eax,[edx.C_CAddrL]
	mov	ah,[edx.C_CAddrH]
	sub	eax,ecx 			; EAX = byte offset into cur page
	movzx	ecx,[edx.C_HDisp]
	inc	cl
	xor	edx,edx
	div	cx				; AX = line number, DX = offset
	mov	ecx,edx
	pop	edx
	ret
EndProc VDD_Get_EGA_Posn


VxD_CODE_ENDS
	END

