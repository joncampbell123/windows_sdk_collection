       TITLE   VDD - Virtual Display Device for EGA   version 2.10  2/88
;******************************************************************************
;
; VDDTIO.ASM - Virtual EGA I/O Register emulation
;
;   Author: MDW
;
;   (C) Copyright MICROSOFT Corp. 1986 - 1991
;
;DESCRIPTION:
;   I/O trapping routines for video device.  Emulate I/O for non-attached
;   VMs and pass I/O through to the device for attached VMs.  Special handling
;   for VMs running in video memory but not attached to the physical display.
;   Also routines for enabling and disabling trapping as needed.
;
;   Made changes to make EGA registers readable for the CHIPS435 chip and
;	the ATI EGA Wonder 800 chip - this just made the VGA and EGA versions
;	equal as far as input processing of the standard EGA registers goes.
;
;******************************************************************************

	.386p

	INCLUDE VMM.inc
	INCLUDE OPTTEST.INC
	INCLUDE ega.inc
	INCLUDE vdd.inc
	INCLUDE debug.inc

IFDEF	VGA
	INCLUDE vga.inc
ENDIF

;******************************************************************************
; EXTRN routines
;
VxD_CODE_SEG
IFDEF	PEGA
	EXTRN	VDD_Unmap_PShdw:NEAR
	EXTRN	VDD_Map_PShdw:NEAR
	EXTRN	VDD_Alloc_PShdw:NEAR
ENDIF
	EXTRN	VDD_Mem_Disable:NEAR
	EXTRN	VDD_Mem_Valid:NEAR
VxD_CODE_ENDS


VxD_IDATA_SEG

IFDEF	VGAMONO
Begin_VxD_IO_Table VDD_MONO_IO_Table
; Treat Mono access (3B?) just like color (3D?)
	VxD_IO	3BAh,VIOT_3DA		    ; Feature controller
	VxD_IO	3B4h,VIOT_3D4
	VxD_IO	3B5h,VIOT_3D5		    ; CRT controller registers
End_VxD_IO_Table VDD_MONO_IO_Table
ENDIF

Begin_VxD_IO_Table VDD_IO_Table
	VxD_IO	3C0h,VIOT_3C0
	VxD_IO	3C1h,VIOT_3C1		    ; Attribute controller registers
	VxD_IO	3C2h,VIOT_3C2		    ; Miscellaneous register
	VxD_IO	3C3h,VIOT_Ignore
	VxD_IO	3C4h,VIOT_3C4
	VxD_IO	3C5h,VIOT_3C5		    ; Sequencer registers
	VxD_IO	3C6h,VIOT_3C6		    ; COMPAQ special, ignore if bckgrnd
IFDEF	VGA
	VxD_IO	3C7h,VIOT_3C7		    ; VGA DAC registers
	VxD_IO	3C8h,VIOT_3C8
	VxD_IO	3C9h,VIOT_3C9
ELSE
	VxD_IO	3C7h,VIOT_Ignore
IFDEF	VEGA
	VxD_IO	3C8h,VIOT_3C8
	VxD_IO	3C9h,VIOT_3C9		    ; Scratch registers(VEGA)
ELSE
	VxD_IO	3C8h,VIOT_Ignore
	VxD_IO	3C9h,VIOT_Ignore
ENDIF
ENDIF
	VxD_IO	3CAh,VIOT_3CA
IFDEF	ATIEGA
	VxD_IO	3CBh,VIOT_3CB
ELSE
	VxD_IO	3CBh,VIOT_Ignore
ENDIF
	VxD_IO	3CCh,VIOT_3CC		    ; Grx ctrl pos reg,Misc O/P Read Reg
IFDEF	ATIEGA
	VxD_IO	3CDh,VIOT_3CD
ELSE
	VxD_IO	3CDh,VIOT_Ignore
ENDIF
	VxD_IO	3CEh,VIOT_3CE
	VxD_IO	3CFh,VIOT_3CF		    ; Graphics controller registers
	VxD_IO	3D0h,VIOT_Ignore
	VxD_IO	3D1h,VIOT_Ignore
	VxD_IO	3D2h,VIOT_Ignore
	VxD_IO	3D3h,VIOT_Ignore
	VxD_IO	3D4h,VIOT_3D4
	VxD_IO	3D5h,VIOT_3D5		    ; CRT controller registers
	VxD_IO	3D6h,VIOT_Ignore
	VxD_IO	3D7h,VIOT_Ignore
IFDEF	PEGA
	VxD_IO	3D8h,VIOT_3D8		    ; Paradise status register
	VxD_IO	3D9h,VIOT_Ignore
	VxD_IO	3DAh,VIOT_3DA		    ; Feature controller
	VxD_IO	3DBh,VIOT_3DB		    ; Paradise mode, light pen reset
	VxD_IO	3DCh,VIOT_3DC		    ; Light pen set
	VxD_IO	3DDh,VIOT_3DD		    ; Paradise Plantronics register
	VxD_IO	3DEh,VIOT_3DE		    ; Paradise AT&T/Olivetti register
	VxD_IO	3DFh,VIOT_3DF		    ; Paradise Special
ELSE
	VxD_IO	3D8h,VIOT_Ignore
	VxD_IO	3D9h,VIOT_Ignore
	VxD_IO	3DAh,VIOT_3DA		    ; Feature controller
	VxD_IO	3DBh,VIOT_3DB		    ; Light pen reset
	VxD_IO	3DCh,VIOT_3DC		    ; Light pen set
	VxD_IO	3DDh,VIOT_Ignore
	VxD_IO	3DEh,VIOT_Ignore
	VxD_IO	3DFh,VIOT_Ignore
ENDIF
End_VxD_IO_Table VDD_IO_Table


IFDEF	VGA8514
Begin_VxD_IO_Table VDD_8514_IO_Table
;;;	VxD_IO	  100h, VIOT_8514_Null	    ; POS ports are not 8514 specific
;;;	VxD_IO	  101h, VIOT_8514_Null	    ;	allowing access should not
;;;	VxD_IO	  102h, VIOT_8514_Null	    ;	cause display problems...
	VxD_IO	  2E8h, VIOT_8514_Null
	VxD_IO	  2E9h, VIOT_8514_Null
	VxD_IO	  2EAh, VIOT_8514_Null
	VxD_IO	  2EBh, VIOT_8514_Null
	VxD_IO	  2ECh, VIOT_8514_Null
	VxD_IO	  2EDh, VIOT_8514_Null
	VxD_IO	  6E8h, VIOT_8514_Null
	VxD_IO	  6E9h, VIOT_8514_Null
	VxD_IO	 0AE8h, VIOT_8514_Null
	VxD_IO	 0AE9h, VIOT_8514_Null
	VxD_IO	 0EE8h, VIOT_8514_Null
	VxD_IO	 0EE9h, VIOT_8514_Null
	VxD_IO	 12E8h, VIOT_8514_Null
	VxD_IO	 12E9h, VIOT_8514_Null
	VxD_IO	 16E8h, VIOT_8514_Null
	VxD_IO	 16E9h, VIOT_8514_Null
	VxD_IO	 1AE8h, VIOT_8514_Null
	VxD_IO	 1AE9h, VIOT_8514_Null
	VxD_IO	 1EE8h, VIOT_8514_Null
	VxD_IO	 1EE9h, VIOT_8514_Null
	VxD_IO	 22E8h, VIOT_8514_Null
	VxD_IO	 22E9h, VIOT_8514_Null
	VxD_IO	 42E8h, VIOT_8514_Null
	VxD_IO	 42E9h, VIOT_8514_Null
	VxD_IO	 46E8h, VIOT_8514_Null
	VxD_IO	 46E9h, VIOT_8514_Null
	VxD_IO	 4AE8h, VIOT_8514_Null
	VxD_IO	 4AE9h, VIOT_8514_Null
	VxD_IO	 82E8h, VIOT_8514_Null
	VxD_IO	 82E9h, VIOT_8514_Null
	VxD_IO	 86E8h, VIOT_8514_Null
	VxD_IO	 86E9h, VIOT_8514_Null
	VxD_IO	 8AE8h, VIOT_8514_Null
	VxD_IO	 8AE9h, VIOT_8514_Null
	VxD_IO	 8EE8h, VIOT_8514_Null
	VxD_IO	 8EE9h, VIOT_8514_Null
	VxD_IO	 92E8h, VIOT_8514_Null
	VxD_IO	 92E9h, VIOT_8514_Null
	VxD_IO	 96E8h, VIOT_8514_Null
	VxD_IO	 96E9h, VIOT_8514_Null
	VxD_IO	 9AE8h, VIOT_8514_Null
	VxD_IO	 9AE9h, VIOT_8514_Null
	VxD_IO	 9EE8h, VIOT_8514_Null
	VxD_IO	 9EE9h, VIOT_8514_Null
	VxD_IO	0A2E8h, VIOT_8514_Null
	VxD_IO	0A2E9h, VIOT_8514_Null
	VxD_IO	0A6E8h, VIOT_8514_Null
	VxD_IO	0A6E9h, VIOT_8514_Null
	VxD_IO	0AAE8h, VIOT_8514_Null
	VxD_IO	0AAE9h, VIOT_8514_Null
	VxD_IO	0AEE8h, VIOT_8514_Null
	VxD_IO	0AEE9h, VIOT_8514_Null
	VxD_IO	0B2E8h, VIOT_8514_Null
	VxD_IO	0B2E9h, VIOT_8514_Null
	VxD_IO	0B6E8h, VIOT_8514_Null
	VxD_IO	0B6E9h, VIOT_8514_Null
	VxD_IO	0BAE8h, VIOT_8514_Null
	VxD_IO	0BAE9h, VIOT_8514_Null
	VxD_IO	0BEE8h, VIOT_8514_Null
	VxD_IO	0BEE9h, VIOT_8514_Null
	VxD_IO	0E2E8h, VIOT_8514_Null
	VxD_IO	0E2E9h, VIOT_8514_Null
End_VxD_IO_Table VDD_8514_IO_Table
ENDIF

IFDEF	ATIVGA
; ATI VGA extended registers
Begin_VxD_IO_Table ATI_IO_Table
	VxD_IO	  1CEh, VIOT_1CE
	VxD_IO	  1CFh, VIOT_1CF
End_VxD_IO_Table ATI_IO_Table
ENDIF
VxD_IDATA_ENDS


;******************************************************************************
; EXTRN and PUBLIC data
;
VxD_DATA_SEG
	Extern_vgVDD
EXTRN	VDD_CB_Off:DWORD

;*******
; The following table handles dispatch for changes to the Graphics controller
;   and sequencer.  When some of the registers are changed, the state of the
;   VDD also changes.
public	VDD_SG0_JTab
VDD_SG0_JTab	LABEL	DWORD
	DD	OFFSET32 VOT_VG_Fin	    ; Graphics ctlr G_SetRst(ignore)
	DD	OFFSET32 VOT_VG_SREna	    ; Graphics ctlr G_SREna
	DD	OFFSET32 VOT_VG_Fin	    ; Graphics ctlr G_CComp(ignore)
	DD	OFFSET32 VOT_VG_Func	    ; Graphics ctlr G_Func
	DD	OFFSET32 VOT_VG_MSlct	    ; Graphics ctlr G_MSlct
	DD	OFFSET32 VOT_VG_Mode	    ; Graphics ctlr G_Mode
	DD	OFFSET32 VOT_VG_Misc	    ; Graphics ctlr G_Misc
	DD	OFFSET32 VOT_VG_Fin	    ; Graphics ctlr G_DCare(ignore)
	DD	OFFSET32 VOT_VG_Mask	    ; Graphics ctlr G_Mask
IFDEF CTVGA
%OUT Routine reqd to handle changes to GR F7 Reg(CTVGA)
	DD	OFFSET32 VOT_VG_Fin	    ; Graphics ctlr F7 Reg NOP for now
ENDIF
IFDEF	PVGA
	DD	7 DUP (OFFSET32 VOT_VG_Fin) ; Graphics ctlr PVGA regs
ELSE
	DD	GL_Resvd DUP (OFFSET32 VOT_VG_Fin)  ; Graphics ctlr G_Resvd
ENDIF
	DD	OFFSET32 VOT_VS_Fin	    ; Sequencer S_Rst(ignore)
	DD	OFFSET32 VOT_VS_Fin	    ; Sequencer S_ClMode(ignore)
	DD	OFFSET32 VOT_VS_Mask	    ; Sequencer S_Mask
	DD	OFFSET32 VOT_VS_Fin	    ; Sequencer S_ChMap(ignore)
	DD	OFFSET32 VOT_VS_MMode	    ; Sequencer S_MMode
	DD	SL_Resvd DUP (OFFSET32 VOT_VS_Fin)  ; Sequencer S_Resvd

;*******
; The following table handles dispatch for changes to the Graphics controller
;   and sequencer.  When some of the registers are changed, the state of the
;   VDD also changes. This table is used when the VM is attached to the 2nd EGA
;
public	VDD_SG2_JTab
VDD_SG2_JTab	LABEL	DWORD
	DD	OFFSET32 VOT_2G_Doit	    ; Graphics ctlr G_SetRst
	DD	OFFSET32 VOT_2G_DoIt	    ; Graphics ctlr G_SREna
	DD	OFFSET32 VOT_2G_Doit	    ; Graphics ctlr G_CComp
	DD	OFFSET32 VOT_2G_DoIt	    ; Graphics ctlr G_Func
	DD	OFFSET32 VOT_2G_DoIt	    ; Graphics ctlr G_MSlct
	DD	OFFSET32 VOT_2G_Mode	    ; Graphics ctlr G_Mode
	DD	OFFSET32 VOT_2G_Misc	    ; Graphics ctlr G_Misc
	DD	OFFSET32 VOT_2G_Doit	    ; Graphics ctlr G_DCare
	DD	OFFSET32 VOT_2G_DoIt	    ; Graphics ctlr G_Mask
IFDEF	PVGA
	DD	3 DUP (OFFSET32 VOT_2G_DoIt) ; Grap ctlr PVGA regs
	DD	4 DUP (OFFSET32 VOT_VG_FIN) ; Grap ctlr PVGA regs
ELSE
	DD	GL_Resvd DUP (OFFSET32 VOT_2G_DoIt) ; Grap ctlr G_Resvd
ENDIF
	DD	OFFSET32 VOT_VS_Fin	    ; Sequencer S_Rst(ignore)
	DD	OFFSET32 VOT_VS_Fin	    ; Sequencer S_ClMode(ignore)
	DD	OFFSET32 VOT_2S_Mask	    ; Sequencer S_Mask
	DD	OFFSET32 VOT_VS_Fin	    ; Sequencer S_ChMap(ignore)
	DD	OFFSET32 VOT_2S_MMode	    ; Sequencer S_MMode
	DD	SL_Resvd DUP (OFFSET32 VOT_VS_Fin)  ; Sequencer S_Resvd

;*******
; The following table handles dispatch for changes to the Graphics controller
;   and sequencer when attached to the 2nd EGA, but registers have not been
;   restored yet.  All output is just saved.
;
public	VDD_SGx_JTab
VDD_SGx_JTab	LABEL DWORD
	DD	OFFSET32 VOT_VG_Fin	  ; Graphics ctlr G_SetRst
	DD	OFFSET32 VOT_VG_Fin	  ; Graphics ctlr G_SREna
	DD	OFFSET32 VOT_VG_Fin	  ; Graphics ctlr G_CComp
	DD	OFFSET32 VOT_VG_Fin	  ; Graphics ctlr G_Func
	DD	OFFSET32 VOT_VG_Fin	  ; Graphics ctlr G_MSlct
	DD	OFFSET32 VOT_VG_Fin	  ; Graphics ctlr G_Mode
	DD	OFFSET32 VOT_VG_Fin	  ; Graphics ctlr G_Misc
	DD	OFFSET32 VOT_VG_Fin	  ; Graphics ctlr G_DCare
	DD	OFFSET32 VOT_VG_Fin	  ; Graphics ctlr G_Mask
IFDEF	PVGA
	DD	7 DUP (OFFSET32 VOT_VG_FIN) ; Grap ctlr PVGA regs
ELSE
	DD	GL_Resvd DUP (OFFSET32 VOT_VG_Fin)  ; Grap ctlr G_Resvd
ENDIF
PUBLIC Seq_Rtn_Off
Seq_Rtn_Off EQU     $-VDD_SGx_JTab
	DD	OFFSET32 VOT_VS_Fin	  ; Sequencer S_Rst(ignore)
	DD	OFFSET32 VOT_VS_Fin	  ; Sequencer S_ClMode(ignore)
	DD	OFFSET32 VOT_VS_Fin	  ; Sequencer S_Mask
	DD	OFFSET32 VOT_VS_Fin	  ; Sequencer S_ChMap(ignore)
	DD	OFFSET32 VOT_VS_Fin	  ; Sequencer S_MMode
	DD	SL_Resvd DUP (OFFSET32 VOT_VS_Fin)  ; Sequencer S_Resvd

VxD_DATA_ENDS

VxD_ICODE_SEG

;******************************************************************************
; VDD_IO_Init
;
;DESCRIPTION:
;	Sets up trapping for VDD I/O ports
;
;ENTRY: EBX = SYS VM handle
;	EDI = VDD CB ptr for SYS VM
;
;EXIT:	CF = 1 if error
;
;USES:	Flags
;
BeginProc VDD_IO_Init,PUBLIC

	pushad
	mov	edi, OFFSET32 VDD_IO_Table	; Table of ports
	VMMCall Install_Mult_IO_Handlers	; Install the port traps
	jc	SHORT VIOI_Exit

IFDEF	VGAMONO
	TestMem [Vid_Flags],fVid_Mono		;Q: VDD handles MONO mem?
	jz	SHORT VDD_IO_I_nomono		;   N: don't hook mono I/O
	mov	edi, OFFSET32 VDD_MONO_IO_Table
	VMMCall Install_Mult_IO_Handlers	; Install the port traps
	jc	SHORT VIOI_Exit
VDD_IO_I_nomono:
ENDIF

IFDEF	VGA8514
; globally 8514 I/O ports are always trapped, but we disable trapping in the
; SYS VM, so Windows can have free reign
;
	mov	edi, OFFSET32 VDD_8514_IO_Table ; Table of ports
	VMMCall Install_Mult_IO_Handlers	; Install the port traps
	jc	SHORT VIOI_Exit

	movzx	ecx, [edi.VxD_IO_Ports]
	add	edi, SIZE VxD_IOT_Hdr
VIOI_disable_SYS_trapping:
	movzx	edx, [edi.VxD_IO_Port]
	add	edi, SIZE VxD_IO_Struc
	VMMCall Disable_Local_Trapping
	loop	VIOI_disable_SYS_trapping
ENDIF	;VGA8514

	clc					; return with no error
VIOI_Exit:
	popad
	ret
EndProc VDD_IO_Init

VxD_ICODE_ENDS


VxD_CODE_SEG

;******************************************************************************
; VDD_IO_SetTrap
;
;DESCRIPTION:
;	Sets up trapping for VDD I/O ports when VM type or focus changes
;
;ENTRY: EBX = VM handle
;	EDI = VM's VDD CB ptr
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, EDX
;
BeginProc VDD_IO_SetTrap,PUBLIC

Assert_VDD_ptrs ebx,edi
	mov	edx,3C0h
	TestMem [edi.VDD_Flags],fVDD_NoTrap	; Q: Special VM type?
	jnz	SHORT VIOST_Disable_Local	;   Y: No trapping

IFDEF	VGA
	TestMem [edi.VDD_Flags],fVDD_ModeSet	; Q: Doing mode set?
	jnz	SHORT VIOST_Enable_Local	;   Y: trap all ports

	cmp	ebx,[Vid_VM_Handle]		; Q: VM not attached?
	jnz	SHORT VIOST_Enable_Local	;   Y: Do trapping

; check the PIF NoTrap bits for Attached VM

mVidNoTrapPIF EQU fVidNoTrpTxt+fVidNoTrpLRGrfx+fVidNoTrpHRGrfx
	TestMem [edi.VDD_PIF],mVidNoTrapPIF	; Q: No trapping enabled?
	jz	SHORT VIOST_Enable_Local	;   N: trap all ports

	cmp	[edi.VDD_ModeEGA],7		; Q: Text mode?
	je	SHORT VIOST_00			;   Y: check relevant PIF bit
	cmp	[edi.VDD_ModeEGA],3		; Q: Text mode?
	ja	SHORT VIOST_01			;   N: continue
VIOST_00:
	TestMem [edi.VDD_PIF],fVidNoTrpTxt	; Q: No trapping enabled?
	jnz	SHORT VIOST_Disable_Local	;   Y: No trapping
	jmp	SHORT VIOST_Enable_Local	;   N: Do trapping
VIOST_01:
	cmp	[edi.VDD_ModeEGA],6		; Q: Lo Res mode?
	ja	SHORT VIOST_02			;   N: continue
	TestMem [edi.VDD_PIF],fVidNoTrpLRGrfx	; Q: No trapping enabled?
	jnz	SHORT VIOST_Disable_Local	;   Y: No trapping
	jmp	SHORT VIOST_Enable_Local	;   N: Do trapping
VIOST_02:
	TestMem [edi.VDD_PIF],fVidNoTrpHRGrfx	; Q: No trapping enabled?
	jnz	SHORT VIOST_Disable_Local	;   Y: No trapping
; fall through to enable trapping, put a jump for safety
	jmp	SHORT VIOST_Enable_Local	;   N: Do trapping
        
ELSE
	cmp	ebx,[Vid_VM_Handle]		; Q: Attached VM?
	jnz	SHORT VIOST_Enable_Local	;   N: Do trapping
mVidNoTrapPIF EQU fVidNoTrpTxt+fVidNoTrpLRGrfx+fVidNoTrpHRGrfx
	TestMem [edi.VDD_PIF],mVidNoTrapPIF	;   Y: Q: No trapping enabled?
	jz	SHORT VIOST_Enable_Local	;	N: trap all ports
	TestMem [edi.VDD_Flags],fVDD_ModeSet	; Q: Doing mode set?
	jnz	SHORT VIOST_Enable_Local	;   Y: trap all ports
	cmp	[edi.VDD_ModeEGA],3		; Q: Text mode?
	ja	SHORT VIOST_00			;   N: continue
	TestMem [edi.VDD_PIF],fVidNoTrpTxt	; Q: No trapping enabled?
	jnz	SHORT VIOST_Disable_Local	;   Y: No trapping
	jmp	SHORT VIOST_Enable_Local	;   N: Do trapping
VIOST_00:
	cmp	[edi.VDD_ModeEGA],6		; Q: Lo Res mode?
	ja	SHORT VIOST_01			;   N: continue
	TestMem [edi.VDD_PIF],fVidNoTrpLRGrfx	; Q: No trapping enabled?
	jnz	SHORT VIOST_Disable_Local	;   Y: No trapping
	jmp	SHORT VIOST_Enable_Local	;   Y: Do trapping
VIOST_01:
	TestMem [edi.VDD_PIF],fVidNoTrpHRGrfx	; Q: No trapping enabled?
	jnz	SHORT VIOST_Disable_Local	;   Y: No trapping
;;	jmp	SHORT VIOST_Enable_Local	;   Y: Do trapping
ENDIF

; Enable local trapping for VM
VIOST_Enable_Local:
	bts	[edi.VDD_Flags],fVDD_IOTBit	; Q: Already trapped?
	jc	SHORT VIOST_Exit		;   Y: All done
VIOST_EL0:					;   N: enable local traps
	VMMCall Enable_Local_Trapping
	inc	edx
	cmp	edx,3E0h
	jb	VIOST_EL0
VIOST_Exit:
	ret

; Disable local trapping for VM
VIOST_Disable_Local:
	btr	[edi.VDD_Flags],fVDD_IOTBit	; Q: Already trapped?
	jnc	SHORT VIOST_Exit		;   N: All done
VIOST_DL0:					;   N: enable local traps
	VMMCall Disable_Local_Trapping
VIOST_DL1:
	inc	edx
IFDEF	VEGA
	cmp	edx,3C8h			; Don't disable trapping
	jb	VIOST_DL0			;   on 3C8 and 3C9
	cmp	edx,3C9h
	jbe	VIOST_DL1
ENDIF
	cmp	edx,3E0h
	jb	VIOST_DL0
	ret

EndProc VDD_IO_SetTrap

;******************************************************************************
; VIOT_3xx		 Trap I/O for ports 3C0 .. 3DF
;
;DESCRIPTION:
;	Handles EGA I/O port emulation.
;
;ENTRY: EBX = VM handle
;	EAX/AX/AL = DWORD/WORD/BYTE to output to port.
;	EDX = port address for I/O.
;	ECX = I/O length/direction descriptor
;	SS:EBP = Client stack
;
;EXIT:	EAX/AX/AL = emulated input value from port.
;
;USES:	Flags, ECX, EDX, ESI, EDI
;

;***************
;   Labeling conventions:
;	VIOT_nnn	    Video I/O trap entry point for port nnn
;	VIOT_tt_nnn	    Video Input/Output Trap routine where:
;	    tt =    In - input
;	       =    VI - virtual input
;	       =    PI - physical input
;	       =    Out - output
;	       =    PO - physical output
;	       =    VO - virtual output

;***************
;   This Macro separates out input and virtual output
;	Physical BYTE output falls through.
;	INVOCATION:
;	    IOPort = I/O port number
;	    InpPort = Input port name (prefixed by "VIOT_In_")
;	    VOPort = Output port name (prefixed by "VIOT_VO_")
;	ASSUMES: If input port rtn not given, do byte input
;	    If output port name not given, fall through
.LALL
VIOT_Entry MACRO IOPort, InpPort, FarJmpI, VOPort, FarJmpVO
LOCAL xxxyzx,xxxyzz

IF1
IFNDEF	VIOT_In_Undef
BeginProc VIOT_In_Undef,High_Freq		;; Input undefined
VIOT_In_Undef&IOPort EQU VIOT_In_Undef
	in	al,dx
	ret
EndProc VIOT_In_Undef
ENDIF
ENDIF

IF2
IFDEF	VIOT_In_Undef&IOPort
BeginProc VIOT_In_Undef,High_Freq		;; Input undefined
	in	al,dx
	ret
EndProc VIOT_In_Undef
ENDIF
ENDIF

BeginProc VIOT_&IOPort,High_Freq		;; Entry point for I/O trap
	mov	edi,ebx
	add	edi,[VDD_CB_Off]		;; ESI = VDD control blk ptr
	Emulate_Non_Byte_IO			;; Return ZF=0 if byte input
IFB	<InpPort>				;; Q: Input routine defined?
	jnz	VIOT_In_Undef			;;   N: Do real input
ELSE
IFNB	<FarJmpI>
	jnz	VIOT_In_&InpPort		;;   Y: Branch to handle input
ELSE
	jnz	SHORT VIOT_In_&InpPort
ENDIF
ENDIF
IFNB	<VOPort>				;; Q: Virtual out rtn defined?
	cmp	ebx,[Vid_VM_Handle]
IFNB	<FarJmpVO>
	jnz	VIOT_VO_&VOPort 		;;   Y: if virtual out, do it
ELSE
	jnz	SHORT VIOT_VO_&VOPort
ENDIF						;;   N: just drop thru to phys
ENDIF
	ENDM

;***************
;   This Macro terminates an I/O trap procedure
;	INVOCATION:
;	    IOPort = I/O port number
VIOT_End   MACRO IOPort
EndProc VIOT_&IOPort
	ENDM

;***************
;   This Macro separates out virtual input(physical input falls through)
;	INVOCATION:
;	    IOName = I/O port name
;	    FarJmpV = if present, do far jump to VIOT_VI_xxx label
VIOT_Input MACRO IOName, FarJmpV, NullPort
VIOT_In_&IOName:
IFB	<NullPort>
	cmp	ebx,[Vid_VM_Handle]
IFNB	<FarJmpV>
	jnz	VIOT_VI_&IOName
ELSE
	jnz	SHORT VIOT_VI_&IOName
ENDIF
ENDIF
	ENDM

;***************
;   This Macro exits from a virtual or physical port trap, optionally doing out
;	INVOCATION:
;	    IOName = I/O port number
;	    DoOut = if present, do byte output
;	    NoPSttClr = if present, do not reset Paradise status counter
VIOT_Out_Exit MACRO IOName, DoOut, NoPSttClr
IFNB	<DoOut>
	out	dx,al
ENDIF
IFB	<NoPSttClr>
IFDEF	PEGA
	mov	[edi.VDD_Stt.V_PSttCnt],-2
ENDIF
ENDIF
	ret
	ENDM

;***************
;   This Macro exits from a physical or virtual port input trap
;	INVOCATION:
;	    IOName = I/O port name
;	    InFlag = if present, do input from port
VIOT_In_Exit MACRO IOName, DoInput, NoPSttClr
IFNB	<DoInput>
	in	al,dx
ENDIF
IFB	<NoPSttClr>
IFDEF	PEGA
	mov	[edi.VDD_Stt.V_PSttCnt],-2
ENDIF
ENDIF
	ret
	ENDM

;*********************************************************************
;
;   The I/O port trap routines

;***************    Undefined ports
;
VIOT_Entry Ignore
	VIOT_Out_Exit	Ignore, DoOut, NoPSttClr
VIOT_End Ignore


IFDEF	VGA8514
BeginProc VIOT_8514_Null, High_Freq

	Dispatch_Byte_IO Fall_Through, <SHORT VIOT_8514_Out>

VIOT_8514_In:
	xor	al, al

VIOT_8514_Out:
	ret

EndProc VIOT_8514_Null
ENDIF

IFDEF	ATIVGA
;***************    ATI VGA extension index register
;
BeginProc VIOT_1CE
	TestMem [Vid_Flags],fVid_ATiVGA 	    ; Q: ATI EGA 800?
	jnz	VIOT_ATI_1CE_OUT
;
;	ClrFlag [edi.VDD_Flags], fVDD_ATIEna
	out	dx,al
	ret
;
VIOT_ATI_1CE_OUT:
VIOT_Entry	x1CE,ATIV_Indx,,ATIV_Indx
; Physical output
	
	out	dx,al
;
VIOT_VO_ATIV_Indx:
; Virtual output
	cmp	al,0a0h
	jb	VIOT_ATI_1ce_Ret
	cmp	al,0bfh
	ja	VIOT_ATI_1ce_Ret	
;
	SetFlag [edi.VDD_Flags],fVDD_ATIEna
	mov	[edi.VDD_Stt.ATI_Indx],al
VIOT_ATI_1ce_Ret:
	ret

; Physical input
VIOT_Input ATIV_Indx,,
	ClrFlag [edi.VDD_Flags], fVDD_ATIEna
	in	al,dx
	ret
VIOT_VI_ATIV_Indx:
; Virtual input
	ClrFlag [edi.VDD_Flags], fVDD_ATIEna
	mov	al,[edi.VDD_Stt.ATI_Indx]
	ret
VIOT_End x1CE
VIOT_End 1CE

;***************    ATI VGA extension data register
;
BeginProc VIOT_1CF
	TestMem [Vid_Flags],fVid_ATiVGA 	    ; Q: ATI EGA?
	jnz	VIOT_ATI_1CF_OUT
;
VIO_1CF_OTHERS_OUT:
	ClrFlag [edi.VDD_Flags], fVDD_ATIEna
	out	dx,al
	ret
;
VIOT_ATI_1CF_OUT:
VIOT_Entry	x1CF,ATIV_Data,,ATIV_Data
; Physical output
	TestMem [edi.VDD_Flags],fVDD_ATIEna
	jz	VIO_1CF_OTHERS_OUT
	push	edx
	push	eax
	mov	ah,al	
	mov	al,[edi.VDD_Stt.ATI_Indx]
	out	dx,ax
	pop	eax
	pop	edx

	ClrFlag [edi.VDD_Flags], fVDD_ATIEna
	ret

VIOT_VO_ATIV_Data:
; Virtual output
	
	TestMem [edi.VDD_Flags],fVDD_ATIEna
	jz	VIOT_1CF_OTHERS_OUT
	ClrFlag [edi.VDD_Flags], fVDD_ATIEna
	movzx	ecx,[edi.VDD_Stt.ATI_Indx]
	mov	[edi.VDD_Stt.ATI_Regs][ecx - a0h],al
	ret
;
;
VIOT_Input ATIV_Data,,
; Physical input
VIOT_ATI_1CF_IN:
	ClrFlag [edi.VDD_Flags], fVDD_ATIEna
	in	al,dx
	ret

VIOT_VI_ATIV_Data:
; Virtual input
	TestMem [edi.VDD_Flags],fVDD_ATIEna
	jz	VIOT_ATI_1CF_OUT
	movzx	ecx,[edi.VDD_Stt.ATI_Indx]
	ClrFlag [edi.VDD_Flags], fVDD_ATIEna
	mov	al,[edi.VDD_Stt.ATI_Regs][ecx-0a0h]
	ret
VIOT_End x1CF
VIOT_End 1CF

ENDIF

;***************    Attribute controller(port 3C0 and port 3C1)
;
VIOT_Entry	3C0,3C0,,Attr
	jmp	short VIOT_PHYSO_3C0
VIOT_In_3C0:
; Note that if trapping is disabled and the index register is written to
;   and then trapping is enabled and the index is read, the value will
;   be bogus.  We don't know of any software that depends on this.
	mov	al, [edi.VDD_Stt.A_Indx]
	and	al, NOT fVAI_Indx
	VIOT_In_Exit 3C0

IFDEF EGA
VIOT_Entry	3C1,3C1,FAR,Attr,
ELSE
VIOT_Entry	3C1,3C1,,Attr,
ENDIF
VIOT_PHYSO_3C0:
	out	dx,al
VIOT_VO_Attr:
	TestMem [edi.VDD_Stt.A_Indx],fVAI_Indx	    ; Q: Setting index?
	jnz	SHORT VIOT_AIndx		    ;	Y: go set it
	movzx	ecx,[edi.VDD_Stt.A_Indx]	    ;	N: it's data reg output
	test	cl, 00010000b			    ; Q: Setting palette?
	jnz	short VIOT_VOA0 		    ;	N: Access OK
	test	cl, fVAI_ScOn			    ;	Y: Q: Display on?
	jnz	SHORT VIOT_ASv1 		    ;	    Y: ignore
VIOT_VOA0:
	and	ecx, NOT fVAI_ScOn
IFDEF	PEGA
	test	[edi.VDD_Stt.V_PSttCnt],-1	    ; Q: Paradise special?
	jns	SHORT VIOT_ASv_PSpecial 	    ;	Y: go process
VIOT_ASv0:
	TestMem [edi.VDD_Stt.V_Flag_PEGA],fVPa_PALokd ; Q: Attribute ctlr locked?
	jnz	SHORT VIOT_ASv1 		    ;	Y: ignore value
ENDIF
	cmp	ecx,A_IMax			    ; Q: index valid?
	ja	SHORT VIOT_ASv1 		    ;	N: ignore
	mov	[edi.VDD_Stt.A_Pal][ecx],al	    ; Save the Attr reg. value
VIOT_ASv1:
	or	[edi.VDD_Stt.A_Indx],fVAI_Indx	    ; Attr ready for index
	jmp	SHORT VIOT_ASv2

VIOT_AIndx:
	mov	[edi.VDD_Stt.A_Indx],al
VIOT_ASv2:
	VIOT_Out_Exit	Attr


IFDEF	PEGA
VIOT_ASv_PSpecial:
;;;Debug_Out "PEGA Attr OUT #DX,#AL"
	cmp	edx,xA_PALok			    ; Q: PEGA lock attr reg?
	jnz	SHORT VIOT_ASv0 		    ;	N: treat normally
	SetFlag [edi.VDD_Stt.V_Flag_PEGA],fVPa_PALok ;	Y: Indicate reg written
	push	eax
	and	al,mPEna
	cmp	al,vPDis			    ; Q: Unlock Attr timing?
	pop	eax
	jnz	SHORT VIOT_ASv_PUnlk		    ;	Y: All done
	SetFlag [edi.VDD_Stt.V_Flag_PEGA],fVPa_PALokd ; N: Indicate locked
	SetFlag [Vid_Flags],fVid_PALokd
	jmp	VIOT_ASv1
VIOT_ASv_PUnlk:
	and	[edi.VDD_Stt.V_Flag_PEGA],NOT fVPa_PALokd ;  Assume unlock Attr
	ClrFlag [Vid_Flags], fVid_PALokd
	jmp	VIOT_ASv1
ENDIF

VIOT_In_3C1:
	movzx	eax, [edi.VDD_Stt.A_Indx]
	and	al, (NOT fVAI_ScOn) AND A_IMsk	    ; make sure index valid
	mov	al, [edi.VDD_Stt.A_Pal][eax]	    ; Get the Attr reg. value
	VIOT_In_Exit 3C1

VIOT_End 3C1
VIOT_End 3C0

;***************    Miscellaneous register
;
VIOT_Entry	3C2,Stat0,,Misc
	out	dx,al
VIOT_VO_Misc:
IFDEF	PEGA
	test	[edi.VDD_Stt.V_PSttCnt],-1	    ; Q: Paradise special?
	jns	SHORT VIOT_M_PSpecial		    ;	Y: go process
ENDIF
	mov	[edi.vdd_Stt.V_Misc],al 	    ; Save Misc output value

VIOT_MSv_Ret:
	VIOT_Out_Exit	Misc

IFDEF	PEGA
VIOT_M_PSpecial:
;;;Debug_Out "PEGA Misc OUT #DX,#AL"
	mov	[edi.VDD_Stt.VC_PMisc],al	    ; Save Paradise Misc value
	SetFlag [edi.VDD_Stt.V_Flag_PEGA],fVPa_PMisc ; indicate PMisc active
	jmp	SHORT VIOT_MSv_Ret
ENDIF

;***************    Status 0 register
;
	VIOT_Input	Stat0
	VIOT_In_Exit	Stat0, DoInput
VIOT_VI_Stat0:
	mov	al,[edi.vdd_Stt.V_Stat0]
	xor	al,fstatDots+fstatEna		    ; Toggle dot stream bit
	mov	[edi.VDD_Stt.V_Stat0],al	    ; Save value of pseudo status
	VIOT_In_Exit
VIOT_End 3C2

;***************    Sequencer index
;
VIOT_Entry	3C4,SeqI,,SeqI
	out	dx,al
VIOT_VO_SeqI:
	mov	[edi.VDD_Stt.S_Indx],al
VIOT_Out_SeqIRet:
	VIOT_Out_Exit	SeqI

	VIOT_Input SeqI
	VIOT_In_Exit SeqI, DoInput
VIOT_VI_SeqI:
	mov	al, [edi.VDD_Stt.S_Indx]	    ; get virtual value
	VIOT_In_Exit

VIOT_End 3C4

;***************    Sequencer data
;
IFDEF V7VGA
VIOT_Entry	3C5,Seq,FAR,Seq,FAR
        movzx   ecx,[edi.VDD_Stt.S_Indx]
	cmp	cl,6				    ; Q: Extensions enable reg?
	je	short VIOT_OSeq_EnableExt	    ;  Y: save state of reg
	or	cl,cl				    ; Q: Reinitializing?
	jz	SHORT VIOT_OSeq_ModChgAL	    ;	Y: Indicate mode change
	js	SHORT VIOT_Out_V7ExtReg 	    ;  N: see if ext. are enabled
VIOT_OSeq_SaveVal:
	cmp	cl,S_IMax			    ; Q: IBM range?
	ja	SHORT VIOT_Out_SeqRet		    ;  N: ignore it
	mov	[edi.VDD_Stt.S_Rst][ecx],al	    ; N: Save value and exit
VIOT_Out_SeqRet:
	out	dx,al
VIOT_Out_SeqNoOut:
	VIOT_Out_Exit	Seq
VIOT_OSeq_ModChgAL:
	mov	[edi.VDD_ModeEGA],-1		    ; Invalidate mode
	jmp	VIOT_OSeq_SaveVal		    ; Save new value and exit

; Save extended register to save area
VIOT_Out_V7ExtReg:
	TestMem [Vid_Flags],<fVid_V7VGA+fVid_CLVGA> ; Q: Video 7 or Cirrus Logic?
	jz	SHORT VIOT_OSeq_SaveVal 	    ;  N: ignore the value
	push	eax
	dec	edx
	mov	al,6
	out	dx,al
	inc	edx
	in	al,dx
	mov	[edi.VDD_Stt.S_Rst.6],al	    ; Save ext. enable value
	mov	al,cl
	dec	edx
	out	dx,al
	inc	edx
	pop	eax
	cmp	[edi.VDD_Stt.S_Rst.6],1 	    ; Q: Ext. enabled?
IFDEF	DEBUG
	je	short @F
	Debug_Out " EXTENDED DISABLED!!"
@@:
ENDIF
	jne	VIOT_Out_SeqNoOut		    ;  N: ignore the value
	cmp	cl,083h
	jz	SHORT VIOT_Out_SetAttr
;;;	cmp	cl,0FFh
;;;	jnz	SHORT VIOT_VS_SavePhys		    ; NO!! this messes up some
;;;	or	al,010h 			    ;	video 7 adapters
VIOT_VS_SavePhys:
        mov     [edi.VDD_Stt.VDD_V7ER][ecx-80H],al
	jmp	VIOT_Out_SeqRet
VIOT_Out_SetAttr:
	xor	al,80h
	mov	[edi.VDD_Stt.A_Indx],al
	xor	al,80h
	jmp	VIOT_VS_SavePhys

; Clear/set extension enable flag.
; V7 Extensions are enabled by writing an 0EAh to SR 6 and are
; disabled by writing an 0AEh to SR 6. V7 hardware returns a 1 in SR 6
; if extensions are enabled. It returns a 0 if extensions are disabled.
; This routine transforms the output value to that returned by the hardware
; ECX = 6
VIOT_OSeq_EnableExt:
	TestMem [Vid_Flags],<fVid_V7VGA+fVid_CLVGA> ; Q: Video 7 or Cirrus Logic?
	jz	VIOT_OSeq_SaveVal		    ;  N: ignore the value
	xor	cl,cl				    ;  Y: set disable value
	cmp	al,0eah 			    ; Q: Enable extensions?
	jne	SHORT VIOT_OSeq_EnableExt_10	    ;  N: see if disable command
	inc	cl
	mov	[edi.VDD_Stt.S_Rst.6],cl
	jmp	VIOT_Out_SeqRet
VIOT_OSeq_EnableExt_10:
	cmp	al,0aeh 			    ; Q: Disable extensions?
	jne	VIOT_Out_SeqRet 		    ;  N: ignore
	mov	[edi.VDD_Stt.S_Rst.6],cl
	jmp	VIOT_Out_SeqRet

; SEQUENCER "IN AL,DX" EMULATION
	VIOT_Input Seq
	VIOT_In_Exit Seq, DoInput
VIOT_VI_Seq:
        movzx   ecx, [edi.VDD_Stt.S_Indx]
	or	cl,cl
	js	SHORT VIOT_VI_V7EXT
	mov	al, [edi.VDD_Stt.S_Rst][ecx]	      ; get virtual value
VIOT_VI_Ex:
	VIOT_In_Exit
VIOT_VI_V7EXT:
	mov	al, [edi.VDD_Stt.VDD_V7ER][ecx-80H]   ; get virtual value
	cmp	cx,083h
	jnz	SHORT VIOT_VI_Ex
	mov	al,[edi.VDD_Stt.A_Indx]
	xor	al,fVAI_Indx
	jmp	short VIOT_VI_Ex

; SEQUENCER "OUT DX,AL" EMULATION
; Virtual output of Sequencer data register
VIOT_VO_Seq:
        movzx   ecx,[edi.VDD_Stt.S_Indx]
	cmp	cl,6				    ; Q: Extensions enable reg?
	je	SHORT VIOT_VOSeq_EnableExt	    ;  Y: save state of reg
	or	cl,cl				    ; Q: Reinitializing?
	js	SHORT VIOT_VO_V7ExtReg		    ;  N: see if ext. are enabled
VIOT_VOSeq_SaveVal:
	cmp	cl,S_IMax			    ; Q: IBM range?
	ja	SHORT VIOT_VSex 		    ;  N: ignore it
	cmp	[edi.VDD_Stt.S_Rst][ecx],al	    ; Q: Value change?
	jz	SHORT VIOT_VSEx 		    ;	N: all done
;*******
; Process change in Sequencer register
;   AL	= value
;   EBX = VM handle
;   ECX = index(register number)
;   EDX = port address
;   EDI = VDD control block structure ptr
	mov	esi,[edi.VDD_Routine]		      ; EDX = addr of dispatch table
	call	DWORD PTR DS:[esi.Seq_Rtn_Off][ecx*4] ; call port chg handler
VIOT_VSEx:
	VIOT_Out_Exit  VSeq

; Save extended register to save area
VIOT_VO_V7ExtReg:
	TestMem [Vid_Flags],<fVid_V7VGA+fVid_CLVGA> ; Q: Video 7 or Cirrus Logic?
	jz	SHORT VIOT_VOSeq_SaveVal	    ;  N: No special stuff
	cmp	[edi.VDD_Stt.S_Rst.6],1 	    ; Q: Ext. enabled?
	jne	SHORT VIOT_VSEx 		    ;  N: ignore the value
	cmp	cl,083h
	jz	SHORT VIOT_VO_SetAttr
;;;	cmp	cl,0FFh
;;;	jnz	SHORT VIOT_VS_Save		    ; NO!! this messes up some
;;;	or	al,010h 			    ;	video 7 adapters
VIOT_VS_Save:
        mov     [edi.VDD_Stt.VDD_V7ER][ecx-80H],al
	jmp	SHORT VIOT_VSEx
VIOT_VO_SetAttr:
	xor	al,fVAI_Indx
	mov	[edi.VDD_Stt.A_Indx],al
	xor	al,fVAI_Indx
	jmp	VIOT_VS_Save
; Clear/set extension enable flag.
; V7 Extensions are enabled by writing an 0EAh to SR 6 and are
; disabled by writing an 0AEh to SR 6. V7 hardware returns a 1 in SR 6
; if extensions are enabled. It returns a 0 if extensions are disabled.
; This routine transforms the output value to that returned by the hardware
; ECX = 6
VIOT_VOSeq_EnableExt:
	TestMem [Vid_Flags],<fVid_V7VGA+fVid_CLVGA> ; Q: Video 7 or Cirrus Logic?
	jz	SHORT VIOT_VOSeq_SaveVal	    ;  N: No special stuff
	xor	cl,cl				    ;  Y: set disable value
	cmp	al,0eah 			    ; Q: Enable extensions?
	jne	SHORT VIOT_VOSeq_EnableExt_10	     ;	N: see if disable command
	inc	cl
	mov	[edi.VDD_Stt.S_Rst.6],cl
	jmp	VIOT_VSEx			    ;  N: ignore
VIOT_VOSeq_EnableExt_10:
	cmp	al,0aeh 			    ; Q: Disable extensions?
	jne	VIOT_VSEx			    ;  N: ignore
	mov	[edi.VDD_Stt.S_Rst.6],cl
	jmp	VIOT_VSEx
VIOT_End 3C5

ELSE

VIOT_Entry	3C5,Seq,,Seq
	movzx	ecx,[edi.VDD_Stt.S_Indx]
	cmp	cl,S_IMax			    ; Q: Valid index?
	ja	SHORT VIOT_Out_SeqRet		    ;	N: ignore the value
	or	ecx,ecx 			    ; Q: Reinitializing?
	jz	SHORT VIOT_OSeq_ModChgAL	    ;	Y: Indicate mode change
VIOT_OSeq_SaveAL:
	mov	[edi.VDD_Stt.S_Rst][ecx],al	    ; N: Save value and exit
VIOT_Out_SeqRet:
	VIOT_Out_Exit	Seq, DoOutput
VIOT_OSeq_ModChgAL:
	mov	[edi.VDD_ModeEGA],-1		    ; Invalidate mode
	jmp	VIOT_OSeq_SaveAL		    ; Save new value and exit

; Virtual output of Sequencer data register
VIOT_VO_Seq:
	movzx	ecx,[edi.VDD_Stt.S_Indx]
	cmp	cl,S_IMax			    ; Q: Valid index?
	ja	SHORT VIOT_VSEx 		    ;	N: Exit
	cmp	[edi.VDD_Stt.S_Rst][ecx],al	    ; Q: Value change?
	jz	SHORT VIOT_VSEx 		    ;	N: all done
;*******
; Process change in Sequencer register
;   AL	= value
;   EBX = VM handle
;   ECX = index(register number)
;   EDX = port address
;   EDI = VDD control block structure ptr
	mov	esi,[edi.VDD_Routine]		    ; EDX = addr of dispatch table
	call	DWORD PTR DS:[esi.Seq_Rtn_Off][ecx*4] ; call port chg handler
VIOT_VSEx:
	VIOT_Out_Exit  VSeq

	VIOT_Input Seq
	VIOT_In_Exit Seq, DoInput
VIOT_VI_Seq:
	movzx	ecx, [edi.VDD_Stt.S_Indx]
	mov	al, [edi.VDD_Stt.S_Rst][ecx]	    ; get virtual value
	VIOT_In_Exit

VIOT_End 3C5
ENDIF ;V7VGA

;***************    Miscellaneous register
;
VIOT_Entry	3C6,Undef,Far,CPQ
	out	dx,al
VIOT_VO_CPQ:
	VIOT_Out_Exit	CPQ
VIOT_End 3C6

IFDEF	VGA
;
; IN:	DAC State Register
; OUT:	Read mode PEL address
;
VIOT_Entry	3C7,3C7,,3C7
	out	dx,al
VIOT_VO_3C7:
	movzx	eax, al
	shl	eax, 2
	lea	edx, [edi.VDD_DAC]
	add	eax, edx		; eax points to start of PEL entry
	mov	[edi.VDD_PEL_Addr], eax
	mov	[edi.VDD_PEL_Idx], 1
	mov	[edi.VDD_PEL_Mode], DAC_Read_Mode
	VIOT_Out_Exit	3C7

	VIOT_Input	3C7
	VIOT_In_Exit	3C7, DoInput
VIOT_VI_3C7:
	mov	al, [edi.VDD_PEL_Mode]
	VIOT_In_Exit	3C7

VIOT_End 3C7

;
; IN:	Write mode PEL address
; OUT:	Write mode PEL address
;
VIOT_Entry	3C8,3C8,,3C8
	out	dx,al
VIOT_VO_3C8:
	movzx	eax, al
	shl	eax, 2
	lea	edx, [edi.VDD_DAC]
	add	eax, edx		; eax points to start of PEL entry
	mov	[edi.VDD_PEL_Addr], eax
	mov	[edi.VDD_PEL_Idx], 1
	mov	[edi.VDD_PEL_Mode], DAC_Write_Mode
	VIOT_Out_Exit	3C8

	VIOT_Input	3C8
	VIOT_In_Exit	3C8, DoInput
VIOT_VI_3C8:
	mov	eax, [edi.VDD_PEL_Addr]
	lea	edx, [edi.VDD_DAC]
	sub	eax, edx
	shr	eax, 2			; al = PEL index
	VIOT_In_Exit	3C8

VIOT_End 3C8


;
; IN:	Read current PEL data byte
; OUT:	Write current PEL data byte
;
VIOT_Entry	3C9,3C9,,3C9
	out	dx, al
VIOT_VO_3C9:
	mov	esi, [edi.VDD_PEL_Addr]
	movzx	edx, [edi.VDD_PEL_Idx]
	cmp	[edi.VDD_PEL_Mode], DAC_Read_Mode ;Q: read mode?
IFDEF DEBUG
	jne	short VIOT_3C9_write_ok
	Debug_Out 'VDD: write DAC data in read mode (port 3C9)'
	jmp	short VIOT_update_DAC_ptr
VIOT_3C9_write_ok:
ELSE
	je	short VIOT_update_DAC_ptr	  ;   Y: skip write update
ENDIF
	mov	[esi+edx], al
	jmp	short VIOT_update_DAC_ptr

	VIOT_Input	3C9
	VIOT_In_Exit	3C9, DoInput
VIOT_VI_3C9:
	mov	esi, [edi.VDD_PEL_Addr]
	movzx	edx, [edi.VDD_PEL_Idx]
	mov	al, [esi+edx]
VIOT_update_DAC_ptr:
	cmp	edx, 3			;Q: last byte in PEL entry?
	jb	short VIOT_3C9_not_last ;   N:
	xor	edx, edx		;   Y: INC to next entry
	add	esi, 4
        push    eax                     ; don't stomp on the return value in AL
	lea	eax, [edi.VDD_PEL_Addr]
	cmp	esi, eax		;Q: end of DAC table?
        pop     eax
	jb	short VIOT_3C9_no_wrap	;   N:
	lea	esi, [edi.VDD_DAC]	;   Y: wrap to start of table
VIOT_3C9_no_wrap:
	mov	[edi.VDD_PEL_Addr], esi
VIOT_3C9_not_last:
	inc	edx
	mov	[edi.VDD_PEL_Idx], dl
	VIOT_In_Exit	3C9

VIOT_End 3C9

ENDIF	;VGA

IFDEF	VEGA
;***************    VEGA specific scratch RAM index register
;
;	The scratch RAM is treated as global.  The value of the index
;	register, however, is restored whenever the data registers are
;	read or written.  This should satisfy usage of scratch RAM as
;	initialization information.
;
VIOT_Entry	3C8,VgRAMI,,VgRAMI
	out	dx,al
VIOT_VO_VgRAMI:
	mov	[edi.VDD_VgRAMI],al
	VIOT_Out_Exit

VIOT_In_VgRAMI:
	in	al,dx
	mov	[edi.VDD_VgRAMI],al
	VIOT_In_Exit	VgRAMI
VIOT_End 3C8

;***************    VEGA specific scratch RAM data register
;
VIOT_Entry	3C9,VgRAM
	push	eax
	mov	al, [edi.VDD_VgRAMI]
	dec	edx
	out	dx,al
	IO_Delay
	inc	edx
	pop	eax
	VIOT_Out_Exit	VgRAM, DoOutput


; VEGA scratch RAM input
	VIOT_Input	VgRAM
VIOT_VI_VgRAM:
	mov	al, [edi.VDD_VgRAMI]
	dec	edx
	out	dx,al
	IO_Delay
	inc	edx
	VIOT_In_Exit	VgRAM, DoInput
VIOT_End 3C9
ENDIF	;VEGA

;***************    Graphics controller 2 position
;			    Constant - never saved/restored
VIOT_Entry	3CA,,,GPos2
	out	dx,al
VIOT_VO_GPos2:
	VIOT_Out_Exit	GPos2
VIOT_End 3CA

IFDEF ATIEGA
;***************    ATI EGA extension index register
;
BeginProc VIOT_3CB
	TestMem [Vid_Flags],fVid_ATI800 	    ; Q: ATI EGA 800?
	jz	VIOT_Ignore			    ;	N: just exit
VIOT_Entry	x3CB,ATI_Indx,,ATI_Indx
; Physical output
	out	dx,al

VIOT_VO_ATI_Indx:
; Virtual output
	ClrFlag [edi.VDD_Flags], fVDD_ATIEna
	mov	[edi.VDD_Stt.ATI_Indx],al
VIOT_ATI_Ret:
	ret

; Physical input
VIOT_Input ATI_Indx,,
	ClrFlag [edi.VDD_Flags], fVDD_ATIEna
	in	al,dx
	ret
VIOT_VI_ATI_Indx:
; Virtual input
	ClrFlag [edi.VDD_Flags], fVDD_ATIEna
	mov	al,[edi.VDD_Stt.ATI_Indx]
	ret
VIOT_End x3CB
VIOT_End 3CB

;***************    ATI EGA extension data register
;
BeginProc VIOT_3CD
	TestMem [Vid_Flags],fVid_ATI800 	    ; Q: ATI EGA?
	jz	VIOT_Ignore			    ;	N: Do physical
VIOT_Entry	x3CD,ATI_Data,,ATI_Data
; Physical output
	cli
	dec	edx
	dec	edx
	push	eax
	mov	al,[edi.VDD_Stt.ATI_Indx]
	out	dx,al
	IO_Delay
	inc	edx
	inc	edx
	in	al,dx
	IO_Delay
	pop	eax
	out	dx,al
	sti

VIOT_VO_ATI_Data:
; Virtual output
	btr	[edi.VDD_Flags],fVDD_ATIEnaBit
	jnc	SHORT VIOT_ATI_Out_Ignore
	movzx	ecx,[edi.VDD_Stt.ATI_Indx]
	cmp	cl,2				    ; Q: unchangeable register?
	je	SHORT VIOT_ATI_Out_Ret		    ;	Y: don't save
	cmp	cl,0Ah				    ; Q: unchangeable register?
	je	SHORT VIOT_ATI_Out_Ret		    ;	Y: don't save
	cmp	cl,ATI_RegCount
	jae	SHORT VIOT_ATI_Out_Ignore
	mov	[edi.VDD_Stt.ATI_Regs][ecx],al
IFDEF	DEBUG
	jmp	VIOT_ATI_Ret
VIOT_ATI_Out_Ignore:
	Debug_Out "ATI data output ignored, index: #CX"
ELSE
VIOT_ATI_Out_Ignore:
ENDIF
VIOT_ATI_Out_Ret:
	ret
VIOT_Input ATI_Data,,
; Physical input
VIOT_In_Data:
	SetFlag [edi.VDD_Flags],fVDD_ATIEna
	in	al,dx
	ret
; Some registers, just return physical value on read
VIOT_ATI_In_Phys:
	dec	edx
	dec	edx
	out	dx,al
	IO_Delay
	inc	edx
	inc	edx
	jmp	VIOT_In_Data

VIOT_VI_ATI_Data:
; Virtual input
	SetFlag [edi.VDD_Flags],fVDD_ATIEna
	movzx	eax,[edi.VDD_Stt.ATI_Indx]
	cmp	al,ATI_RegCount
	jae	VIOT_ATI_In_Phys
	cmp	al,02h
	je	VIOT_ATI_In_Phys		    ; Read reg 2 physical
	jb	SHORT VIOT_VI_Getit		    ; Virtualize 0,1
	cmp	al,3
	je	VIOT_ATI_In_Phys		    ; Read reg 3 physical
	cmp	al,0Ah
	jb	VIOT_ATI_In_Phys		    ; Read reg 4-9 physical
	jne	SHORT VIOT_VI_GetIt
	mov	al,03h				    ; Reg A is read of reg 3
VIOT_VI_GetIt:
	mov	al, [edi.VDD_Stt.ATI_Regs][eax]
	ret
VIOT_End x3CD
VIOT_End 3CD
ENDIF

;***************    Graphics controller 1 position
;			    Constant - never saved/restored
VIOT_Entry	3CC,MiscOP_Read,,GPos1
	out	dx,al
        ret

        VIOT_Input MiscOP_Read,,
        in      al,dx
        ret
VIOT_VI_MiscOP_Read:
        mov     al,[edi.VDD_Stt.V_Misc]
        ret

VIOT_VO_GPos1:
	VIOT_Out_Exit	GPos1
VIOT_End 3CC

;***************    Graphics controller index register
;
VIOT_Entry	3CE,GrpI,,GrpI
	out	dx,al
VIOT_VO_GrpI:
	mov	[edi.VDD_Stt.G_Indx],al
	VIOT_Out_Exit	GrpI

;*******
; Graphics controller index input
	VIOT_Input GrpI
	VIOT_In_Exit GrpI, DoInput
VIOT_VI_GrpI:
	mov	al, [edi.VDD_Stt.G_Indx]	    ; get virtual value
	VIOT_In_Exit

VIOT_End 3CE

;***************    Graphics controller data register
;
VIOT_Entry	3CF,Grp,,Grp
	out	dx,al
	movzx	ecx,[edi.VDD_Stt.G_Indx]
IFDEF   CTVGA
        cmp     cl,xG_CTCtl
        jne     SHORT VIOT_Grp00
        mov     cl,09                   ; Grp F7 reg's index is 09h
        jmp     SHORT VIOT_rp_1
VIOT_Grp00:
ENDIF
	cmp	cl,G_IMax			    ; Q: Valid index?
	ja	SHORT VIOT_GrpEx		    ;	N: ignore the value
IFDEF	PVGA
	push	eax
	jnz	SHORT V3CF_O3
	and	eax,7
	xor	al,5				    ; Q: Unlocking PVGA?
	jnz	SHORT V3CF_O1			    ;	N: its locked
	SetFlag [edi.VDD_Flags],fVDD_PVGA_Ena
	jmp	SHORT V3CF_O3
V3CF_O1:
	ClrFlag [edi.VDD_Flags], fVDD_PVGA_Ena
V3CF_O3:
	pop	eax
ELSE
IFDEF DEBUG
	cmp	cl, 8
	jbe	short VIOT_rp_1
	Trace_Out "Grp ctlr weird index: OUT #dx,#al  IDX=#cl"
ENDIF
VIOT_rp_1:
ENDIF
	mov	[edi.VDD_Stt.G_SetRst][ecx],al	    ; Save new value
VIOT_GrpEx:
	VIOT_Out_Exit	Grp

; Virtual output of Graphics controller data register
VIOT_VO_Grp:

	movzx	ecx,[edi.VDD_Stt.G_Indx]
IFDEF   CTVGA
        cmp     cl,xG_CTCtl
        jne     SHORT VIOT_Grp01
        mov     cl,09                   ; Grp F7 reg's index is 09h
        jmp     SHORT VIOT_rp_2
VIOT_Grp01:
ENDIF
	cmp	cl,G_IMax			    ; Q: Valid index?
	ja	SHORT VIOT_GrpEx		    ;	N: Exit
VIOT_rp_2:
	cmp	[edi.VDD_Stt.G_SetRst][ecx],al	    ; Q: Did register chg?
	jz	SHORT VIOT_GrpEx		    ;	N: All done
;*******
; Process change in Graphics ctrlr register
;   AL	= value
;   EBX = VM handle
;   ECX = index(register number)
;   EDX = port address
;   ESI = VDD control block structure ptr
	mov	esi,[edi.VDD_Routine]		    ; EBP = addr of dsptch table
	call	DWORD PTR DS:[esi][ecx*4]	    ; Execute routine
	jmp	VIOT_GrpEx

;*******
; Graphics controller data input
	VIOT_Input Grp
	VIOT_In_Exit Grp, DoInput
VIOT_VI_Grp:
	movzx	ecx, [edi.VDD_Stt.G_Indx]
	mov	al, [edi.VDD_Stt.G_SetRst][ecx]     ; get virtual value
	VIOT_In_Exit

VIOT_End 3CF

;***************     CRT controller index register
;
VIOT_Entry	3D4,CRTI,,CRTI
	out	dx,al
VIOT_VO_CRTI:
	mov	[edi.VDD_Stt.C_Indx],al 	    ; Save curr index reg value
	VIOT_Out_Exit	CRTI

;*******
; CRTC index input
	VIOT_Input CRTI
	VIOT_In_Exit CRTI, DoInput
VIOT_VI_CRTI:
	mov	al, [edi.VDD_Stt.C_Indx]	    ; get virtual value
	VIOT_In_Exit

VIOT_End 3D4

;***************     CRT controller data register
;
IFDEF VGA
VIOT_Entry	3D5,CRT,,CRT
ELSE
VIOT_Entry	3D5,CRT,FAR,CRT
ENDIF
	out	dx,al
VIOT_VO_CRT:
	movzx	ecx,[edi.VDD_Stt.C_Indx]	    ; ECX = index
IFDEF   CTVGA
        cmp     cl,xC_CT400
        jne     SHORT VIOT_CRT01
        mov     cl,25                             ; xC_CT400 - index is 25
        jmp     SHORT VIOT_CSv0
VIOT_CRT01:
        cmp     cl, 0FEh
        jne     SHORT VIOT_CRT02
        mov     cl,26
        jmp     SHORT VIOT_CSv0
VIOT_CRT02:
ENDIF

IFDEF	PEGA
	xor	esi,esi
	bts	esi,ecx 			    ; ESI = index mask
	test	[edi.VDD_Stt.V_PSttCnt],-1	    ; Q: Paradise special?
	jns	SHORT VIOT_CSv_PSpecial 	    ;	Y: go process
ENDIF
VIOT_VO_CRT0:
	cmp	cl,C_IMax			    ; Q: Valid index?
	ja	SHORT VIOT_CSvEx		    ;	N: ignore the value
VIOT_CSv0:
IFDEF	PEGA
	TestMem [edi.VDD_Stt.V_Flag_PEGA],<fVPa_PCScrmd+fVPa_PCLokd>
						    ; Q: Lokd,Scrmd?
	jnz	VIOT_CSv_PExt			    ;	Y: save in diff struc
						    ; N: save unlkd, unscmbld
	or	DWORD PTR [edi.VDD_Stt.VC_PCUnUMsk],esi

	not	esi				    ; Invalidate other values
	and	DWORD PTR [edi.VDD_Stt.VC_PCUnLMsk],esi
	and	DWORD PTR [edi.VDD_Stt.VC_PCScUMsk],esi
	and	DWORD PTR [edi.VDD_Stt.VC_PCScLMsk],esi
	mov	esi,[edi.VDD_Cur_CRTC]
	mov	[esi][ecx],al
ENDIF
	mov	[edi.VDD_Stt.CRTC][ecx],al
VIOT_CSvSaved:
	mov	al,BYTE PTR CModeTab[ecx]	    ; If change in mode, flag it
	or	[edi.VDD_ModeEGA],al

VIOT_CSvEx:
	VIOT_Out_Exit	CRT

CModeTab    LABEL BYTE				    ; -1 for each reg that
	DB  -1,-1,-1,-1,-1,-1,-1,-1		    ;	    affects the mode
	DB  -1,-1, 0, 0,-1,-1, 0, 0		    ; 0 for reg that doesn't
	DB  -1,-1,-1,-1,-1,-1,-1,-1,-1
IFDEF	TLVGA
	DB  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
ELSE
IFDEF   CTVGA
        DB  -1,0
ENDIF
	DB  CL_Resvd DUP (0)
ENDIF
.errnz	($-CModeTab)-(C_IMax+1)



IFDEF	PEGA
VIOT_CSv_PSpecial:
;;;Debug_Out "PEGA CRTC OUT #DX,#AL, Index=#CX"
	cmp	cl,xC_PNew			    ; Q: Paradise special?
	jz	SHORT VIOT_CSv_Par1		    ;	Y: Process it
	cmp	cl,xC_HTime			    ; Q: Paradise special?
	jz	SHORT VIOT_CSv_Par1H		    ;	Y: Process it
	cmp	cl,xC_PScrm			    ; Q: Paradise special?
	jz	SHORT VIOT_CSv_Scrm		    ;	Y: Process it
	jmp	VIOT_CSv0			    ; Weird, treat normally
VIOT_CSv_Par1:
	mov	[edi.VDD_Stt.VC_PNew],al	    ; Save Paradise new reg.
	SetFlag [edi.VDD_Stt.V_Flag_PEGA],fVPa_PNew ;	indicate value output
VIOT_CSv1P:
	call	VDD_CRTC_PChg
	jmp	VIOT_CSvEx
VIOT_CSv_Par1H:
	SetFlag [edi.VDD_Stt.V_Flag_PEGA],fVPa_PCLok ; Indicate HTLF output to
	push	eax
	and	al,mPEna
	cmp	al,vPDis			    ; Q: HTLF active?
	pop	eax
	jnz	SHORT VIOT_CSv_HTLFDis		    ;	N: just return
	SetFlag [edi.VDD_Stt.V_Flag_PEGA],fVPa_PCLokd ;   Y: indicate HTLF active
	SetFlag [Vid_Flags],fVid_PCLokd
	jmp	VIOT_CSv1P
VIOT_CSv_HTLFDis:
	and	[edi.VDD_Stt.V_Flag_PEGA],NOT fVPa_PCLokd ; HTLF not active
	ClrFlag [Vid_Flags], fVid_PCLokd
	jmp	VIOT_CSv1P
VIOT_CSv_Scrm:
	SetFlag [edi.VDD_Stt.V_Flag_PEGA],fVPa_PScrm ; indicate PScrm active
	push	eax
	and	al,mPEna
	cmp	al,vPDis			    ; Q: Scrambler enabled?
	pop	eax
	jz	SHORT VIOT_CSv_ScrmDis		    ;	N: Disable scrambler
	SetFlag [edi.VDD_Stt.V_Flag_PEGA],fVPa_PCScrmd ; Scrambler enabled
	SetFlag [Vid_Flags],fVid_PScrmd
	jmp	VIOT_CSv1P
VIOT_CSv_ScrmDis:
	and	[edi.VDD_Stt.V_Flag_PEGA],NOT fVPa_PCScrmd ; Scrmb not enabled
	ClrFlag [Vid_Flags], fVid_PScrmd
	jmp	VIOT_CSv1P
; Paradise CRTC save in locked and/or scrambled CRTC
VIOT_CSv_PExt:
	bt	DWORD PTR [edi.VDD_Stt.VC_PCRegMsk],ecx ; Q: Register enabled?
	jnc	VIOT_CSvEx			    ;	N: exit
	TestMem [edi.VDD_Stt.V_Flag_PEGA],fVPa_PCScrmd ; Q: Scrambled?
	jnz	SHORT VIOT_CSv_PScrmd		    ;	Y: save scrambled struc
; Unscrambled, locked
;;;Debug_Out "Changing CRTC to UnL"
	or	DWORD PTR [edi.VDD_Stt.VC_PCUnLMsk],esi ; N: save in PCUnL
	not	esi
	and	DWORD PTR [edi.VDD_Stt.VC_PCScUMsk],esi
	and	DWORD PTR [edi.VDD_Stt.VC_PCScLMsk],esi
	push	edx
	lea	edx,[edi.VDD_Stt.VC_PCUnL][ecx]     ; EDX = place to save value
	call	VDD_Save_PCRTC
	pop	edx
	jmp	VIOT_CSvSaved
VIOT_CSv_PScrmd:
	TestMem [edi.VDD_Stt.V_Flag_PEGA],fVPa_PCLokd ; Q: Locked?
	jnz	SHORT VIOT_CSv_PScrL		    ;	Y: save scrmbld, locked
; Scrambled, unlocked
;;;Debug_Out "Changing CRTC to ScU"
	or	DWORD PTR [edi.VDD_Stt.VC_PCScUMsk],esi ; indicate reg exists
	not	esi
	and	DWORD PTR [edi.VDD_Stt.VC_PCUnLMsk],esi
	and	DWORD PTR [edi.VDD_Stt.VC_PCScLMsk],esi
	push	edx
	lea	edx,[edi.VDD_Stt.VC_PCScU][ecx]     ; EDX = place to save value
	call	VDD_Save_PCRTC
	pop	edx
	jmp	VIOT_CSvSaved
VIOT_CSv_PScrL:
; Scrambled, locked
;;;Debug_Out "Changing CRTC to ScL"
	or	DWORD PTR [edi.VDD_Stt.VC_PCScLMsk],esi ; indicate reg exists
	not	esi
	and	DWORD PTR [edi.VDD_Stt.VC_PCUnLMsk],esi
	push	edx
	lea	edx,[edi.VDD_Stt.VC_PCScL][ecx]     ; EDX = place to save value
	call	VDD_Save_PCRTC
	pop	edx
	jmp	VIOT_CSvSaved

;*******
; VDD_Save_PCRTC	save CRTC value in secondary CRTC
;
;	AL = output byte
;	CL = index
;	EBX = Control block ptr
;	EDX = secondary CRTC struc ptr
;
VDD_Save_PCRTC:
	push	eax
	cmp	cl,7
	jz	SHORT VDD_SvOvflw
	mov	[edx],al
	cmp	cl,C_MaxScan			    ; If val affects cursor
	jb	SHORT VDD_SvPCEx
	cmp	cl,C_CAddrL			    ;	then save in base CRTC
	ja	SHORT VDD_SvPCEx

; Most locked regs are thrown away. However, if scrambler is enabled, then
;   register zero is saved also(affects scrambler).  In this case, we do not
;   copy it to the base CRTC registers.
;	TestMem [edi.VDD_Stt.V_Flag_PEGA],fVPa_PCLokd
;	jz	SHORT VDD_SvPC0
;	or	cl,cl
;	jz	SHORT VDD_SvPCEx
;VDD_SvPC0:
	mov	[edi.VDD_Stt.CRTC][ecx],al
VDD_SvPCEx:
	pop	eax
	ret
;Must merge overflow according to value in VDD_PNew
VDD_SvOvflw:
	TestMem [edi.VDD_Stt.V_Flag_PEGA],fVPa_PCLokd ; Q: Overflow locked?
	jz	SHORT VDD_SvOv2 		    ;	N: just save value
	test	[edi.VDD_Stt.VC_PNew],2 	    ; Q: Overflow locked?
	jnz	SHORT VDD_SvOv2 		    ;	N: just save value
	TestMem [edi.VDD_Stt.V_Flag_PEGA],fVPa_PCScrmd ; Q: scrambled?
	jnz	SHORT VDD_SvOv0 		    ;	Y: merge scrmbld val
	mov	ah,[edi.VDD_Stt.CRTC][ecx]	    ;	N: try merge unscrmbld
	test	DWORD PTR [edi.VDD_Stt.VC_PCUnUMsk],esi ; Q: Exist?
	jnz	SHORT VDD_SvOv1 		    ;	    Y: proceed
VDD_SvOv0:
	mov	ah,[edi.VDD_Stt.VC_PCScU][ecx]	    ;	    N: try merge scrmbld
	test	DWORD PTR [edi.VDD_Stt.VC_PCScUMsk],esi ; Q: Exist?
	jnz	SHORT VDD_SvOv1 		    ;	    Y: proceed
	mov	ah,[edi.VDD_Stt.CRTC][ecx]	    ;	N: try merge unscrmbld
	test	DWORD PTR [edi.VDD_Stt.VC_PCUnUMsk],esi ; Q: Exist?
	jnz	SHORT VDD_SvOv1 		    ;	    Y: proceed
	mov	ah,[edx]			    ; Fail: merge with this val
VDD_SvOv1:
	and	ah,0Eh				    ; Save locked bits
	and	al,0F2h 			    ; Get unlocked bits
	or	al,ah				    ; combine locked and unlkd
VDD_SvOv2:
	mov	[edx],al
	jmp	SHORT VDD_SvPCEx
ENDIF

;*******
; CRTC data input
	VIOT_Input CRT
	VIOT_In_Exit CRT, DoInput
VIOT_VI_CRT:
	movzx	ecx, [edi.VDD_Stt.C_Indx]
;;; IFNDEF VGA	    ALLOW READ OF ALL EGA REGISTERS!
; Allow reading some of the EGA registers, 0Ch to 11h only
;;;	cmp	cx,0Ch
;;;	jb	SHORT VIOT_VI_CRT_WriteOnly
;;;	cmp	cx,011h
;;;	ja	SHORT VIOT_VI_CRT_WriteOnly
;;;	jmp	SHORT VIOT_VI_CRT_OK
;;;VIOT_VI_CRT_WriteOnly:
;;;	in	al,dx
;;;	ret
;;;VIOT_VI_CRT_OK:
;;; ENDIF
	mov	al, [edi.VDD_Stt.CRTC][ecx]	    ; get virtual value
	VIOT_In_Exit

VIOT_End 3D5

;***************    Feature controller output/Status 1 input
;
VIOT_Entry	3DA,Stat1,,Feat
	out	dx,al
VIOT_VO_Feat:
	VIOT_Out_Exit	Feat

;***************    Status 1 input
;
	VIOT_Input	Stat1
	or	[edi.VDD_Stt.A_Indx],fVAI_Indx	    ; Attr ready for index
; Even for VM's with focus, we need to fake the status because some software
;   will end up looping on this forever since interrupts and scheduling of
;   VMs may make the software not ever detect the start of VRTC.  Note that
;   this algorithm has been tuned to work with all known popular software.
;;;;	VIOT_In_Exit	Stat1, DoInput
VIOT_VI_Stat1:
; N Times On, M Times Toggle Algorithm:
	mov	al,[edi.VDD_Stt.V_Stat1]	    ; Get value of pseudo status
	cmp	[edi.VDD_VertCnt],0		    ; Q: In Simulated VRTC?
	je	short VIVS_NotVert		    ;	N: continue
	dec	[edi.VDD_VertCnt]		    ;	Y: count down retrace
	jmp	SHORT VIVS_Merge
VIVS_NotVert:
	dec	[edi.VDD_HorizCnt]		    ; count down horiz
	jz	short VIVS_EndHoriz		    ; End of period
; Note Most General Algorithm would have duration counter for horizontal here
	xor	al,fstatDots+fstatEna		    ; Toggle dot stream bit
	jmp	SHORT VIVS_SaveMerge
VIVS_EndHoriz:
	mov	[edi.VDD_VertCnt],VDD_VERT_PERIOD
	mov	[edi.VDD_HorizCnt],VDD_HORIZ_PERIOD
	or	al,fstatDots+fstatEna		    ; Retrace Active Bits ON
VIVS_SaveMerge:
	mov	[edi.VDD_Stt.V_Stat1],al	    ; Save val of pseudo status
VIVS_Merge:
	mov	cl,al
	in	al,dx
	and	al,not CGA_Stat_IVal
	or	al,cl
	or	[edi.VDD_Stt.A_Indx],fVAI_Indx	    ; Attr ready for index
	VIOT_In_Exit
VIOT_End 3DA

IFDEF	PEGA
;***************    Paradise status register
;
VIOT_Entry	3D8,PStt,,PStt
	out	dx,al
VIOT_VO_PStt:
	VIOT_Out_Exit	PStt

; Paradise status input
	VIOT_Input	PStt
	inc	[edi.VDD_Stt.V_PSttCnt]
	VIOT_In_Exit	,DoInput,NoPSttClr
VIOT_VI_PStt:
	inc	[edi.VDD_Stt.V_PSttCnt]
	test	[edi.VDD_Stt.VC_PShdw], fShdwEna
	mov	al,vPSNorm
	jz	SHORT VIOT_VI_PSt0
	mov	al,vPShdw
VIOT_VI_PSt0:
	VIOT_In_Exit	,,NoPSttClr
VIOT_End 3D8

;***************    Paradise register/Light pen reset
;
VIOT_Entry	3DB,,,PLPenR
	out	dx,al
VIOT_VO_PLPenR:
	test	[edi.VDD_Stt.V_PSttCnt],-1
	jns	SHORT VIOT_VOPRg
	VIOT_Out_Exit	LPenR
VIOT_VOPRg:
;;;Debug_Out "PEGA OUT #DX,#AL"
	mov	[edi.VDD_Stt.VC_PReg],al
	SetFlag [edi.VDD_Stt.V_Flag_PEGA],fVPa_PReg
	VIOT_Out_Exit	PReg
VIOT_End 3DB

;***************    Light pen set
;
VIOT_Entry	3DC,,,LPenS
	out	dx,al
VIOT_VO_LPenS:
	VIOT_Out_Exit	LPenS
VIOT_End 3DC

;***************    Paradise Plantronics register
;
VIOT_Entry	3DD,,,PPlr
	out	dx,al
VIOT_VO_PPlr:
	VIOT_Out_Exit	PPlr
VIOT_End 3DD

;***************    Paradise AT&T/Olivetti register
;
VIOT_Entry	3DE,,,PATT
	out	dx,al
VIOT_VO_PATT:
	VIOT_Out_Exit	PATT
VIOT_End 3DE

;***************    Paradise Special
;
VIOT_Entry	3DF,PShdw,FAR,PShdw,FAR
; Physical output
VIOT_PO_PShdw:
	out	dx,al
	test	[edi.VDD_Stt.V_PSttCnt],-1	    ; Q: Reg enabled?
	js	SHORT VIOT_PO_PShdwDone 	    ;	N: Quit
	mov	[edi.VDD_STT.VC_PShdw],al
	test	al,fShdwEna                     ; Q: Enable Shadow RAM? 
	jz	SHORT VIOT_PO_PShDisable        ;   N: Disable if present
	bts	[Vid_Flags],fVid_PEGABit
	jc	SHORT VIOT_PO_Allocd
IFDEF	DEBUG
	VMMCall Test_Sys_VM_Handle
	jz	SHORT VIOT_Alloc_PShdw
Debug_Out "First PShdw access not sys VM!!"
VIOT_Alloc_PShdw:
ENDIF
	push	eax
	push	edx
	call	VDD_Alloc_PShdw
	pop	edx
	pop	eax
VIOT_PO_Allocd:
	test	[edi.VDD_Stt.G_Misc],08h	    ; Q: Address at A000h?
	jz	SHORT VIOT_PO_PShdwDone 	    ;	Y: Memory already there
	test	al,fShdwEna			    ; Q: PEGA shadow mem enable?
	jnz	SHORT VIOT_PO_PShMapIt		    ;	Y: allow access to hdwre
VIOT_PO_PShdwDone:
	VIOT_Out_Exit PShdw

; disable shadow memory if present
VIOT_PO_PShDisable:
	btr	[edi.VDD_Stt.V_Flag_PEGA],fVPa_PShdwBit
        jnc     SHORT VIOT_PO_PShdwDone
	xor	eax,eax
	VMMCall _ModifyPageBits,<ebx,0A0h,1,<NOT P_AVAIL>,eax,PG_HOOKED,eax>
        jmp     SHORT VIOT_PO_PShdwDone

; Make sure have access to physical memory
VIOT_PO_PShMapIt:
	push	eax
	push	edx
	mov	edx,0A0h

	SetFlag [edi.VDD_Stt.V_Flag_PEGA],fVPa_PShdw
	VMMCall _PhysIntoV86,<edx,ebx,edx,1,0>
	pop	edx
	pop	eax
	jmp	VIOT_PO_PShdwDone

; Virtual output
VIOT_VO_PShdw:
	test	[edi.VDD_Stt.V_PSttCnt],-1	    ; Q: Reg enabled?
	js	SHORT VIOT_VO_PShdwDone 	    ;	N: Quit
	test	al,fShdwEna
	jz	short VIOT_VO_PShUnmap
	call	VDD_Map_PShdw			    ; Map the shadow mem
	jmp	short VIOT_VO_PShdwEx
VIOT_VO_PShUnmap:
	call	VDD_Unmap_PShdw 		    ; Unmap the shadow mem
VIOT_VO_PShdwEx:
	mov	[edi.VDD_STT.VC_PShdw],al
VIOT_VO_PShdwDone:
	VIOT_Out_Exit	PShdw
VIOT_End 3DF

	VIOT_Input	PShdw
	VIOT_In_Exit	PShdw, DoInput
VIOT_VI_PShdw:
	mov	al,[edi.VDD_Stt.VC_PShdw]
	VIOT_In_Exit
ELSE

;***************    Light pen reset
;
VIOT_Entry	3DB,,,LPenR
	out	dx,al
VIOT_VO_LPenR:
	VIOT_Out_Exit	LPenR
VIOT_End 3DB

;***************    Light pen set
;
VIOT_Entry	3DC,,,LPenS
	out	dx,al
VIOT_VO_LPenS:
	VIOT_Out_Exit	LPenS
VIOT_End 3DC
ENDIF	;PEGA

;*****************************************************************************
; VOT_Vx_xxxx	   Output trap on Sequencer and Graphics controller ports
;
;DESCRIPTION:
;	These routines are invoked when a VM with a virtual display attempts
;	to modify certain sequencer or graphics controller registers.  The
;	modifications may force the VDD into a different state.
;	If a state transition is detected, all of the video pages are marked
;	not present.  When a page fault occurs, the actual state is determined
;	and the memory and trap dispatch table is set up at that point.
;
;ENTRY: EBX = handle of VM that attempted to do output
;	AL = byte to output to port.
;	ECX = Sequencer or Graphics controller index
;	EDX = I/O port address
;
;EXIT:	none
;
;USES:	flags, ESI
;
;*******
; Sequencer registers we don't care about
BeginProc VOT_VS_Fin,High_Freq

	mov	[edi.VDD_Stt.S_Rst][ecx],al	    ; Save new value
	ret
EndProc VOT_VS_Fin

;*******
; Sequencer write plane mask
BeginProc VOT_VS_Mask,High_Freq

	push	eax
	mov	ah,0Fh
	jmp	SHORT VOT_VS_ChkBits
EndProc VOT_VS_Mask

;*******
; Sequencer memory mode
BeginProc VOT_VS_MMode,High_Freq

	push	eax
	mov	ah,6
;	jmp	VOT_VS_ChkBits
IF2
.errnz	$-VOT_VS_ChkBits
ENDIF

VOT_VS_ChkBits:
	and	al,ah
	and	ah,[edi.VDD_Stt.S_Rst][ecx]
	cmp	ah,al				    ; Q: signficant bits change?
	pop	eax
	jz	SHORT VOT_VS_Done		    ;	N: no state change

	mov	[edi.VDD_ModeEGA],-1		    ;	Y: Invalidate mode
	mov	[edi.VDD_Stt.S_Rst][ecx],al	    ; Save new value
	jmp	VDD_Mem_Valid			    ; Disable mem if not valid
VOT_VS_Done:
	mov	[edi.VDD_Stt.S_Rst][ecx],al	    ; Save new value
	ret
EndProc VOT_VS_MMode

;*******
; Graphics controller don't care values - save and return
BeginProc VOT_VG_Fin,High_Freq

	mov	[edi.VDD_Stt.G_SetRst][ecx],al
	ret
EndProc VOT_VG_Fin

;*******
; Graphics controller Set/Reset enable
BeginProc VOT_VG_SREna,High_Freq

	mov	[edi.VDD_Stt.G_SetRst][ecx],al	    ; Save new value
	test	al,0Fh				    ; Q: Set/Reset enabled?
	jnz	VDD_Mem_Disable 		    ;	Y: Force VM to 2nd EGA
	ret
EndProc VOT_VG_SREna

;*******
; Graphics controller data rotate and logical operations
BeginProc VOT_VG_Func,High_Freq

	mov	[edi.VDD_Stt.G_SetRst][ecx],al	    ; Save new value
	test	al,1Fh				    ; Q: Any raster OPs?
	jnz	VDD_Mem_Disable 		    ;	Y: Force VM to 2nd EGA
	ret
EndProc VOT_VG_Func

;*******
; Graphics controller data write mask(CPU data vs. latch)
BeginProc VOT_VG_Mask,High_Freq

	mov	[edi.VDD_Stt.G_SetRst][ecx],al	    ; Save new value
	cmp	al,0FFh 			    ; Q: Any bits masked?
	jnz	VDD_Mem_Disable 		    ;	Y: Force VM to 2nd EGA
	ret
EndProc VOT_VG_Mask

;*******
; Graphics controller data rotate and logical operations
BeginProc VOT_VG_Mode,High_Freq

	mov	[edi.VDD_Stt.G_SetRst][ecx],al	    ; Save new value
	test	al,1Bh				    ; Q: Read & write mode 0?
	jnz	VDD_Mem_Disable 		    ;	N: Force VM to 2nd EGA
	ret
EndPRoc VOT_VG_Mode

;*******
; Graphics controller read plane select
BeginProc VOT_VG_MSlct,High_Freq

	push	eax
	mov	ah,01fh 			    ; Check sig bits
	jmp	SHORT VOT_VG_ChkBits
EndProc VOT_VG_MSlct

;*******
; Graphics controller VRAM start and odd/even chaining
BeginProc VOT_VG_Misc,High_Freq

	push	eax
	mov	ah,0Eh				    ; Check sig bits
;	jmp	SHORT VOT_VG_ChkBits
IF2
.errnz	$-VOT_VG_ChkBits
ENDIF

VOT_VG_ChkBits:
	and	al,ah
	and	ah,[edi.VDD_Stt.G_SetRst][ecx]
	cmp	ah,al				    ; Q: signficant bits change?
	pop	eax
	mov	[edi.VDD_Stt.G_SetRst][ecx],al
	jnz	VDD_Mem_Valid			    ;	Y: disbl mem if invalid
	ret
EndProc VOT_VG_Misc


;*****************************************************************************
; VOT_2x_xxxx	   Output trap on Sequencer and Graphics controller ports
;
;DESCRIPTION:
;	These routines are invoked when a VM with the "2nd EGA" attempts
;	to modify certain sequencer or graphics controller registers.
;	They allow certain otuputs to occur to the physical device, preserving
;	certain bits and do not allow other outputs to occur. In any case,
;	all values are saved.  Note that the memory at A0000 is mapped to
;	physical A8000 memory when the VM is installed in the "2nd EGA"
;
;ENTRY: EBX = handle of VM that attempted to do output
;	AL = byte to output to port.
;	ECX = Sequencer or Graphics controller index
;	EDX = I/O port address
;
;EXIT:
;
;USES:	flags
;
;CALLS: none
;
;ASSUMES:
;

;*******
; Sequencer write plane mask, allow all bits through
BeginProc VOT_2S_Mask,High_Freq

	xchg	ecx,eax
	dec	edx
	out	dx,al				    ; Output index
	inc	edx
	xchg	eax,ecx
	out	dx,al
	mov	[edi.VDD_Stt.S_Rst][ecx],al	    ; Save new value
	ret
EndProc VOT_2S_Mask

;*******
; Sequencer memory mode
BeginProc VOT_2S_MMode,High_Freq

	xchg	ecx,eax 			    ; AL = index, CL = value
	dec	edx
	out	dx,al				    ; Output index register
	inc	edx
	xchg	ecx,eax 			    ; ECX = index, AL = value
	mov	[edi.VDD_Stt.S_Rst][ecx],al	    ; Save new value
	mov	esi,[Vid_VM_Handle]
	add	esi,[VDD_CB_Off]
	and	al,0FEh 			    ; strip CRT font select bit
	mov	cl,[esi.VDD_Stt.S_MMode]
	and	cl,1
	or	al,cl				    ; Add bit from focus VM
	out	dx,al				    ; Output data register
	ret
EndProc VOT_2S_MMode

;*******
; Graphics controller Set/Reset, Set/Reset enable, Color Compare, Func,
;	Read plane select, Color Don't care, Bit mask
BeginProc VOT_2G_DoIt,High_Freq

	xchg	ecx,eax 			    ; AL = index, CL = value
	dec	edx
	out	dx,al				    ; Output index register
	inc	edx
	xchg	ecx,eax
	mov	[edi.VDD_Stt.G_SetRst][ecx],al	    ; Save new value
	out	dx,al				    ; Output data register
	ret
EndProc VOT_2G_DoIt

;*******
; Graphics controller VRAM start and chaining - preserve text/graphic bit
BeginProc VOT_2G_Misc,High_Freq

	xchg	ecx,eax 			    ; AL = index, CL = value
	dec	edx
	out	dx,al				    ; Output index register
	inc	edx
	xchg	ecx,eax
	mov	[edi.VDD_Stt.G_SetRst][ecx],cl	    ; Save new value
	mov	esi,[Vid_VM_Handle]
	add	esi,[VDD_CB_Off]
	and	cl,0FEh 			    ; strip CRT txt/graphics bit
	mov	al,[esi.VDD_Stt.G_Misc]
	and	al,1				    ; Add in bit from focus VM
	or	al,cl
	out	dx,al
	jmp	VDD_Mem_Valid			    ; Disable mem if not valid
EndProc VOT_2G_Misc

;*******
; Graphics controller mode - preserve CRTC odd/even bits bit
BeginProc VOT_2G_Mode,High_Freq

	xchg	ecx,eax 			    ; AL = index, CL = value
	dec	edx
	out	dx,al				    ; Output index register
	inc	edx
	xchg	ecx,eax
	mov	[edi.VDD_Stt.G_SetRst][ecx],al	    ; Save new value
	mov	esi,[Vid_VM_Handle]
	add	esi,[VDD_CB_Off]
	and	al,0DFh 			    ; strip odd/even bits bit
	mov	cl,[esi.VDD_Stt.G_Mode]
	and	cl,20h				    ; Add in bit from focus VM
	or	al,cl
	out	dx,al
	ret
EndProc VOT_2G_Mode

IFDEF	PEGA
;*****************************************************************************
; VDD_CRTC_PChg     Change trapping state based on values of PEGA regs
;
;DESCRIPTION: Called whenever the "New Paradise register", the scrambler
;	    register or the Horizontal Timing lock register change.  Sets up
;	    VDD_RegMsk for special handling of specific CRTC registers. Also
;	    sets a PTR to the "current CRTC" regs.
;
;ENTRY: EBX = control block
;
;EXIT:
;
;USES:	flags
;
;CALLS: none
;
;ASSUMES:
;
;*******
; Masks to determine which CRTC regs need extra processing
; Overflow reg is 7(always trap), Horizontal timing is 0,2,3,4,5 and 14h
;   vertical timing is 6, 10h, 11h, 15h, 16h
;   For scrambled modes, we always have to save register zero for restore
;	because, even if it is locked, register zero affects the scrambled
;	values. Note that 10h, 11h, 15h and 16h are scrambled.
VDD_PUU_RegMsk	    EQU     -1			       ; Unscrmbld,Unlocked
VDD_PSU_RegMsk	    EQU     -1			       ; Scrmbled, unlocked
VDD_PUL_RegMsk0     EQU     not 00073007Dh	       ; Lokd(PNew=00b)
VDD_PUL_RegMsk1     EQU     not 00010003Dh	       ; Lokd(PNew=01b)
VDD_PUL_RegMsk2     EQU     not 00073007Dh	       ; Lokd(PNew=10b)
VDD_PUL_RegMsk3     EQU     not 00010003Dh	       ; Lokd(PNew=11b)
.ERRE	VDD_PUL_RegMsk0 EQ VDD_PUL_RegMsk2
.ERRE	VDD_PUL_RegMsk1 EQ VDD_PUL_RegMsk3

BeginProc VDD_CRTC_PChg
;;;Debug_Out "PEGA CRTC_PChg"

	pushad
	mov	edx,edi 			    ; EDX = VDD CB ptr
	lea	edi,[edx.VDD_Stt.CRTC]
	mov	eax,VDD_PUU_RegMsk		    ; unscrmbld,unlkd CRTC mask
	TestMem [edx.VDD_Stt.V_Flag_PEGA],<fVPa_PCLokd+fVPa_PCScrmd>
	jz	SHORT VDD_PCP2
	lea	edi,[edx.VDD_Stt.VC_PCScU]
	mov	eax,VDD_PSU_RegMsk		    ; scrmbld unlokd CRTC mask
	TestMem [edx.VDD_Stt.V_Flag_PEGA],fVPa_PCLokd
	jz	SHORT VDD_PCP1
; When locked, don't save locked regs...
	mov	eax,VDD_PUL_RegMsk0		    ; unscrmbld,unlkd CRTC mask
	test	[edx.VDD_Stt.VC_PNew],1 	    ; Q: PNew = 00b or 10b?
	jz	SHORT VDD_PCP0			    ;	Y: save mask
	mov	eax,VDD_PUL_RegMsk1		    ;	N: PNew = 11b or 01b
VDD_PCP0:
	lea	edi,[edx.VDD_Stt.VC_PCUnL]
	TestMem [edx.VDD_Stt.V_Flag_PEGA],fVPa_PCScrmd ; Q: Scrambled?
	jz	SHORT VDD_PCP1			    ;	N: all done
	or	al,1				    ;	Y: Save CRTC0
	lea	edi,[edx.VDD_Stt.VC_PCScL]
; Set current CRTC with unlocked values(for mode detection...)
VDD_PCP1:
	push	edi
	lea	esi,[edx.VDD_Stt.CRTC]
	mov	ecx,size CRTC_Struc/4
	cld
	rep movsd
	pop	edi
; Set locked registers mask
VDD_PCP2:
	mov	DWORD PTR [edx.VDD_Stt.VC_PCRegMsk],eax
	mov	[edx.VDD_Cur_CRTC],edi
	popad
	ret
EndProc VDD_CRTC_PChg
ENDIF

VxD_CODE_ENDS

	END


