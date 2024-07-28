;******************************************************************************
;
;HERC.INC    Definitions for HERC Virtual Display Device
;
;	Author: MDW
;	Copyright (c) Microsoft 1986-1989
;
;DESCRIPTION:
;	This defines constants and the data structures private to the HERC
;	Virtual Display Device(VDD). It includes definitions of data
;	structures, I/O ports and flag bits used by the VDD.
;
;******************************************************************************

INCLUDE VMDAHERC.INC			; VMDA structure definitions

;***************
; Priority boost for System VM when grabber is rendering VM into a window.
;
VDD_Pri_Device_Boost EQU Low_Pri_Device_Boost

;**************************************
; Message mode screen dimensions
;
VDD_Msg_ScrnWid EQU 80			; 80 columns
VDD_Msg_ScrnHgt EQU 25			; 25 rows

;*******
; Instance Map equates
;
HERC_I_Addr1	EQU 449h		; Misc variables
HERC_I_Len1	EQU 1Eh

Assert_VDD_CTL_PTR macro Ptr_Register

IFDEF DEBUG
IFNDEF VDD_Validate_DX_Ctrl
	EXTRN VDD_Validate_DX_Ctrl:near
ENDIF
	push	edx
	mov	edx, Ptr_Register
	call	VDD_Validate_DX_Ctrl
	pop	edx
ENDIF
	ENDM

;***********
;  Video display equates

pIndx6845Mono	equ	3B4h
pData6845Mono	equ	3B5h
pStatMono	equ	3BAh

fStatHSync	equ	00000001b		;Video RAM access OK for processor
fStatDots	equ	00001000b		;Video dot stream in alpha modes
fStatVSync	equ	10000000b		;Vertical Retrace 
HERC_Stat_IVal	equ	fstatVSync+fStatDots+fStatHSync
			;Initial value for pseudo status

pFeatMono	equ	pStatMono
pResetLtPen	equ	3BBh
pSetLtPen	equ	3B9h
pGrphCntrl	equ	3BFh
pModeMono	equ	3B8h		;

; Note that 80x25 and Blink bits are only bits used for monochrome
fModMedRes	equ	00000010b	;   graphics mode
fModVidEna	equ	00001000b	;   video enable
fModBlink	equ	00100000b	;   Blink bit(or intensity) in alpha
fModPage1	equ	10000000b	;   page 1 graphics

Mon_Mod_80	equ	00001000b	;   80x25 Monochrome
HERC_Mod_Mask	equ	10001010b	;   Required bits for mode check

HERC_RamFont_Mask equ	00000001b	; xMode Ram font mode bit
HERC_48KMod_Mask equ	00000100b	;   xMode 48k Ram font mode bit


; Page counts for various modes
; Used in place of VPTMax because it does not vary with mode

HERC_Pages	equ	16	                ; 16 Pages on HERC (64k)
HERC_Mask	EQU	(1 SHL HERC_Pages) - 1

;*******
; VDD data structures
;
; VDD global variables
;
vgVDD_Struc	struc
Vid_VM_Handle	DD	?		; Handle of VM
Vid_Flags	DW	?		; global flags
Vid_cb_Line	DB	?		; Bytes per line
Vid_Type	DB	?		; 0 = GB102, 1 = GB112
Vid_Mode	DB	?		; Display Mode: 0 = 640x200 1 = 640x400
Vid_IndxMax	DB	?		; Max valid Index register number
vgVDD_Struc	ends

;
; VDD global flag definitions
;
fVid_DOff	EQU	0004h		; Display is off
fVid_DOffBit	EQU	2
fVid_Msg	EQU	0800h		; Doing system message
fVid_MsgBit	EQU	11
fVid_MsgI	EQU	1000h		; Initializing for system message
fVid_MsgIBit	EQU	12
fVid_MsgA	EQU	2000h		; System message mode active
fVid_MsgABit	EQU	13
fVid_MsgSttSaved EQU    4000h           ; SysVM state saved as Msg state
fVid_MsgSttSavedBit EQU 14              

;
;	Vid_Type Values
;
Vid_Type_HERC102   equ	   0
Vid_Type_HERC112   equ	   1

;
; VDD Flag bit definitions
;	VDD_Flags is in the per-process area accessed via VM_Handle
;
; Bits 6 thru 15 are not currently
fVDD_NoMain	equ	0000000000000001b   ; Out of main, display not accurate
fVDD_NoMainBit	equ	0
fVDD_NoCopy	equ	0000000000000010b   ; Out of copy, can't update window
fVDD_NoCopyBit	equ	1
fVDD_MMsg	equ	0000000000000100b   ; User told about out of main
fVDD_MMsgBit	equ	2
fVDD_CMsg	equ	0000000000001000b   ; User told about out of copy
fVDD_CMsgBit	equ	3
fVDD_Max	equ	0000000000010000b   ; Max video memory requested
fVDD_MaxBit	equ	4
fVDD_ATT	equ	0000000000100000b   ; Max video memory requested
fVDD_ATTBit	equ	5


;***************
; Hercules definitions

; HERCULES controller state
HERC_State_struc     struc
					; 16 data registers in 6845
V_HTotal	db	?		; [0]
V_HDisp 	db	?		; [1]
V_HSync 	db	?		; [2]
V_HSyncWid	db	?		; [3]
V_VTotal	db	?		; [4]
V_VAdjust	db	?		; [5]
V_VDisp 	db	?		; [6]
V_Sync		db	?		; [7]
V_Interl	db	?		; [8]
V_MaxScan	db	?		; [9]
V_CStart	db	?		; [10]
V_CEnd		db	?		; [11]
V_AddrH 	db	?		; [12]
V_AddrL 	db	?		; [13]
V_CAddrH	db	?		; [14]
V_CAddrL	db	?		; [15]

; Any changes to above is reported to VMDOSAPP

V_LPenH 	db	?		; [16]
V_LPenL 	db	?		; [17] Last reg on GB102
V_Unused	db	?		; [18] unused
		db	?		; [19] unused
; Any changes to xMode is also reported to VMDOSAPP
V_xMode 	db	?		; [20] xModeReg of GB112
V_Score 	db	?		; [21] ScoreReg of GB112
V_Strike	db	?		; [22] StrikeReg of GB112
		db  32-23 DUP(?)	; [23-31] allow for 20h CRT regs

;*******

V_Mode		db	?		; Value output to mode register
V_Cntrl		db	?
V_PageBase	dw	?		;Video RAM page base B800 for page 1.

V_VertCnt	db	?		; Hold Vertical Retrace Counter
V_HorizCnt	db	?		; Toggle Horizontal Retrace Counter

V_Stat		db	?		; Pseudo status byte value
V_Indx		db	?		; 6845 index register
HERC_State_struc     ends

.ERRNZ size HERC_State_Struc mod 4



;***************
; Page handle structure for video memory(40 kB max)
VPHMax		EQU	16  ; 16 page table entries(64Kb) per save area
.ERRE	(VDA_Mem_DPagOff-VDA_Mem_PgMap) EQ VPHMax

VDD_PgHndl_Struc STRUC
VPH_Mode	DB	?		    ; Mode of saved memory
VPH_PgCnt	DB	?		    ; Video pages alloc'd count
VPH_LLen	DW	?		    ; Txt mode line len (= 2 * columns)
VPH_Rows	DB	?		    ; Text mode rows
		DB	?,?,?		    ; Unused
VPH_PgSz	DD	?		    ; Bytes in one displayed page
VPH_PgAllMsk	DD	?		    ; Video pages alloc'd mask
VPH_PgAccMsk	DD	?		    ; Pages accessed mask
VPH_hMem	DD	?		    ; Memory handle
VPH_MState	DB	SIZE VDA_Mem_State DUP (?)
VDD_PgHndl_Struc ENDS

;***************
; HERC control block definitions
CB_VD_Struc	STRUC
VDD_Flags	DD  ?			; VDD flags
VDD_EFlags	DD  ?			; VDD Event and Error flags
VDD_Mode	DB  ?			; Mode of display
VDD_LastMode	DB  ?			; Mode of display
VDD_PIF 	DW  ?			; VDD PIF bits
VDD_DirtyPages	DD  ?			; Dirty pages mask
VDD_AccPages	DD  ?			; Pages written (for updating windows)
VDD_Stt 	DB  SIZE HERC_State_Struc dup (?) ; Controller state structure
VDD_SttCopy	DB  SIZE HERC_State_Struc dup (?) ; Controller state copy
VDD_Pg		DB  SIZE VDD_PgHndl_Struc dup (?) ; VM's video pages
VDD_CPg 	DB  SIZE VDD_PgHndl_Struc dup (?) ; VM's video pages copy
VDD_Upd_Start	DD  ?			; Start time of window update
VDD_ModTime	DD  ?			; System time at last video mod
VDD_Time_Out_Handle DD	?		; Display update timeout handle
VDD_Get_Mem_Count dd    ?               ; Match calls to VDD_Get_Mem/VDD_Free_Mem
                                        ; VDD_Unlock_APP unfreezes the App only
                                        ; if this count is Zero.
CB_VD_Struc	ENDS

VDD_Def_Update_Time EQU 50		; Default window update time = 50ms


;***********
; VDD_Flags bit definitions
;
fVDD_Win	equ	00000001h	    ; VM is displayed in window
fVDD_WinBit	equ	0
fVDD_HCurTrk	equ	00000002h	    ; Track cursor horizontally
fVDD_HCurTrkBit equ	1
fVDD_Grab	equ	00000004h	    ; Grab event pending
fVDD_GrabBit	equ	2
fVDD_Rest	equ	00000008h	    ; Video restore event pending
fVDD_RestBit	equ	3
fVDD_ModeSet	equ	00000010h	    ; Video in INT 10 mode set
fVDD_ModeSetBit equ	4
fVDD_Save	equ	00000020h	    ; We have EGA state to restore
fVDD_SaveBit	equ	5
fVDD_6	        equ	00000040h	    ; unused
fVDD_6bit       equ	6		    ; 
fVDD_Md3	EQU	00000080h	    ; ATT 3rd mode byte written
fVDD_Md3Bit	EQU	7
fVDD_Sus	equ	00000400h	    ; VM suspended
fVDD_SusBit	equ	10
fVDD_Upd	equ	00000800h	    ; Update event pending
fVDD_UpdBit	equ	11
fVDD_ForcedUpd 	equ	00008000h	    ; Force window update after VMInit
fVDD_ForcedUpdBit	equ	15
fVDD_Rst	equ	00010000h	    ; Restore attached VM pending
fVDD_RstBit	equ	16
fVDD_Boost	equ	01000000h	    ; SYS VM boosted for memory read
fVDD_BoostBit	equ	24
fVDD_Hide	equ	08000000h	    ; Hide cursor in window
fVDD_HideBit	equ	27
fVDD_MInit	equ	80000000h	    ; Display mem initialized
fVDD_MInitBit	equ	31

;***************
; VDD_EFlags definitions, error and error msg status
;	Note that Msg bits are error bits shifted left 8
;
fVDE_NoMain	equ	00000100h	    ; Out of main, display not accurate
fVDE_NoMainBit	equ	8
fVDE_NoCopy	equ	00000200h	    ; Out of copy, window not accurate
fVDE_NoCopyBit	equ	9
fVDE_NoMMsg	equ	00010000h	    ; User told out of main
fVDE_NoMMsgBit	equ	16
fVDE_NoCMsg	equ	00020000h	    ; User told out of copy
fVDE_NoCMsgBit	equ	17


Assert_VDD_ptrs MACRO R_Handle,R_VDDptr
LOCAL	AVP_Ok
IFNDEF	VDD_CB_Off
EXTRN	VDD_CB_Off:DWORD
ENDIF
Assert_VM_Handle    R_Handle
	pushfd
	push	R_VDDptr
	sub	R_VDDptr,[VDD_CB_Off]
	cmp	R_Handle,R_VDDptr
	pop	R_VDDptr
	je	short AVP_Ok
Debug_Out   "VDD ptr not correct, #&R_VDDptr"
AVP_Ok:
	popfd
	ENDM

