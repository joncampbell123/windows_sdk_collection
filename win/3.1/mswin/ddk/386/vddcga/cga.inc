;******************************************************************************
;
;CGA.INC    Definitions for CGA Virtual Display Device
;
;	Author: MDW
;	Copyright (c) Microsoft 1986-1989
;	October, 1986
;
;DESCRIPTION:
;	This defines constants and the data structures private to the CGA
;	Virtual Display Device(VDD). It includes definitions of data
;	structures, I/O ports and flag bits used by the VDD.
;
;******************************************************************************

INCLUDE VMDACGA.INC			; VMDA structure definitions

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
CGA_I_Addr1	EQU 449h		; Misc variables
CGA_I_Len1	EQU 1Eh

;***********
;  CGA video display equates
;
pIndx6845Colr	EQU	3D4h		; 6845 index register
pData6845Colr	EQU	3D5h		; 6845 data register

pStatColr	EQU	3DAh		; Status register
fStatEna	EQU	00000001b	;   Video RAM access OK for processor
fStatLPTr	EQU	00000010b	;   Light pen triggered
fStatLPSw	EQU	00000100b	;   Light pen switch is "on"
fStatDots	EQU	00001000b	;   Video dot stream in alpha modes
CGA_Stat_IVal	EQU	fstatEna+fStatDots  ; Initial value for pseudo status


pLPen1Colr	EQU	3DBh		; Light pen latch clear
pLPen1Mono	EQU	3BBh
pLPen2Colr	EQU	3DCh		; Light pen latch set
pLPen2Mono	EQU	3BCh

pModeColr	EQU	3D8h		; mode register NOT USED FOR EGA
; Note that 80x25 and Blink bits are only bits used for monochrome
fMod8025	EQU	00000001b	;   80x25 alpha mode
fModMedRes	EQU	00000010b	;   graphics mode
fModBW		EQU	00000100b	;   B & W mode
fModVidEna	EQU	00001000b	;   video enable
fModHiRes	EQU	00010000b	;   640 x 200 B & W graphics
fModBlink	EQU	00100000b	;   Blink bit(or intensity) in alpha
CGA_Mod_40BW	EQU	00101100b	;   40x25 alpha B & W
CGA_Mod_40Clr	EQU	00101000b	;   40x25 alpha color
CGA_Mod_80BW	EQU	00101101b	;   80x25 alpha B & W
CGA_Mod_80Clr	EQU	00101001b	;   80x25 alpha color
CGA_Mod_LoBW	EQU	00001110b	;   320x200 B & W Graphics
CGA_Mod_LoClr	EQU	00001010b	;   320x200 Color Graphics
CGA_Mod_HiBW	EQU	00011110b	;   640x200 B & W Graphics
Mon_Mod_80	EQU	00100001b	;   80x25 Monochrome
CGA_Mod_Mask	EQU	00010111b	;   Required bits for mode check

;Note low resolution(160 x 100) mode uses 40x25 alpha mode above

pColrCGA	EQU	3D9h		; CGA color register
fColrBack	EQU	00001111b	;   Background/border color
fColrBPalAlpha	EQU	00010000b	;   Background palette in Alpha modes
fColrFPalLoClr	EQU	00100000b	;   Foreground palette in med res mode
CGA_Colr_80Clr	EQU	00000000b	;   80x25 alpha color default

pMode2		EQU	03DEh		; ATT/Olivetti mode 2
pMode3		EQU	03DFh		; ATT/Olivetti mode 3
pStat2		EQU	pMode3		; ATT/Olivetti status 2

; Page counts for various modes
; Used in place of VPTMax because it does not vary with mode
CGA_Pages	EQU	4		; 4 Pages (16k) on a typical CGA
CGA_Mask	EQU	(1 SHL CGA_Pages) - 1
IDC_Pages	EQU	8		; 8 Pages on IDC in 640x400 mode
IDC_Mask	EQU	(1 SHL IDC_Pages) - 1
ATT_Pages	EQU	8		; 8 pages on ATT in 640x400 mode
IDC_FPages	EQU	2		; 2 Pages for IDC text font RAM

;*******
; VDD data structures
;
; VDD global variables
;
vgVDD_Struc	STRUC
Vid_VM_Handle	DD	?		; Handle of VM
Vid_Flags	DW	?		; global flags
Vid_Type	DB	?		; 0 = CGA 1 = IDC 2 = ATT
Vid_Mode	DB	?		; Display Mode: 0 = 640x200 1 = 640x400
vgVDD_Struc	ENDS

;	Vid_Type Values
Vid_Type_CGA	EQU	0
Vid_Type_IDC	EQU	1
Vid_Type_ATT	EQU	2

;	Vid_Mode Values
Vid_Mode_200	EQU	0
Vid_Mode_400	EQU	1


;
; VDD global flag definitions
;
fVid_PVC	EQU	0002h		; Adaptor is PVC subtype of ATT
fVid_PVCBit	EQU	1
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
fVDD_6Bit       equ	6		    
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
; CGA definitions

; CGA controller state
CGA_State_Struc STRUC
V_Mode		DB	?		    ; Value output to mode register
V_Colr		DB	?		    ; Value output to color select reg
V_HTotal	DB	?		    ; 18 data registers in 6845
V_HDisp 	DB	?
V_HSync 	DB	?
V_HSyncWid	DB	?
V_VTotal	DB	?
V_VAdjust	DB	?
V_VDisp 	DB	?
V_Sync		DB	?
V_Interl	DB	?
V_MaxScan	DB	?
V_CStart	DB	?
V_CEnd		DB	?
V_AddrH 	DB	?
V_AddrL 	DB	?
V_CAddrH	DB	?
V_CAddrL	DB	?
V_Mode2 	DB	?		    ; Extended mode reg(ATT/Olivetti)
V_Mode3 	DB	?		    ; Extended mode reg(ATT/Olivetti)
; Any changes to above is reported to VMDOSAPP
;
V_LPenH 	DB	?
V_LPenL 	DB	?
;
V_VertCnt	DB	?		    ; Vertical Retrace Counter
V_HorizCnt	DB	?		    ; Toggle Horiz Retrace Counter
V_Stat		DB	?		    ; Pseudo status byte value
V_Indx		DB	?		    ; 6845 index register

;	COMPAQ IDC Registers
;	Note order is important (See cgatio.asm)
I_MMode 	DB	?		    ; port 13C6
; I_Environ	DB	?		    ; port 17C6 (read only)
; I_DType	DB	?		    ; port 1BC6 (read only)
I_EMSel 	DB	?		    ; port 23C6
I_SBlank	DB	?		    ; port 27C6
I_UCtrl 	DB	?		    ; port 2BC6

		DB	?,?		    ; padding
CGA_State_Struc ENDS

fMModDspInt	EQU	00001000b	    ; IDC Master Mode Display
fMModExtBl	EQU	00010000b	    ; Master Mode Ext Monitor Blanked
fMModIntBl	EQU	00100000b	    ; Master Mode Int Monitor Blanked
fEMSelEMod	EQU	00000001b	    ; Extended Mode Enabled (640x400)
fEMSelFont	EQU	00001000b	    ; Ext Mode Select Font Ram Accessed


;***************
; Page handle structure for video memory(40 kB max)
VPHMax		EQU	8  ; 8 page table entries(32Kb) per save area
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
; CGA control block definitions
CB_VD_Struc	STRUC
VDD_Flags	DD  ?			; VDD flags
VDD_EFlags	DD  ?			; VDD Event and Error flags
VDD_Mode	DB  ?			; Mode of display
VDD_LastMode	DB  ?			; Mode of display
VDD_PIF 	DW  ?			; VDD PIF bits
VDD_DirtyPages	DD  ?			; Dirty pages mask
VDD_AccPages	DD  ?			; Pages written (for updating windows)
VDD_Stt 	DB  SIZE CGA_State_Struc dup (?) ; Controller state structure
VDD_SttCopy	DB  SIZE CGA_State_Struc dup (?) ; Controller state copy
VDD_Pg		DB  SIZE VDD_PgHndl_Struc dup (?) ; VM's video pages
VDD_CPg 	DB  SIZE VDD_PgHndl_Struc dup (?) ; VM's video pages copy
VDD_hFont	DD  ?			; Handle of font memory
VDD_AddrFont	DD  ?			; Address of font memory
VDD_Upd_Start	DD  ?			; Start time of window update
VDD_ModTime	DD  ?			; System time at last video mod
VDD_Time_Out_Handle DD	?		; Display update timeout handle
VDD_Get_Mem_Count dd    ?               ; Match calls to VDD_Get_Mem/VDD_Free_Mem
                                        ; VDD_UnLock_APP unfreezes the App only
                                        ; if this count is Zero.
CB_VD_Struc	ENDS

VDD_Def_Update_Time EQU 50		; Default window update time = 50ms


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
