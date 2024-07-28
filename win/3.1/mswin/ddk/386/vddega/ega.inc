;******************************************************************************
;
; EGA.INC    Include file for Virtual Display Devices for EGA/VGA/8514
;
;	Author: MDW
;	Copyright (c) Microsoft 1986-1991
;	January, 1989
;
;DESCRIPTION:
;	This defines constants and data structures for EGA/VGA/8514
;       Virtual Display Devices(VDD).
;
;******************************************************************************

	INCLUDE VMDAEGA.INC		; Grabber interface definitions

;**************************************
; Specific video adaptor conditional assembly map to generic conditionals
;
ISVGA	MACRO
IFNDEF VGA
VGA equ -1
ENDIF
ENDM

ISVGAMONO	MACRO
IFNDEF VGAMONO
VGAMONO equ -1
ENDIF
ENDM

ISEGA	MACRO
IFNDEF EGA
EGA equ -1
ENDIF
ENDM

;***************************************
; Flavors of VGA
IFDEF	VGA8514 			; 8514 gets VGA also
IF2
%OUT	8514 support
ENDIF
ISVGA
ISVGAMONO
ENDIF

IFDEF	CTVGA 			        ; Chips and Technologies 82C441 "VGA"
IF2
%OUT	CHIPS 441 VGA support
ENDIF
ISVGA
ENDIF

IFDEF	V7VGA				; Video 7 VGA?
IF2
%OUT	Video 7 support
ENDIF
ISVGA
ENDIF

IFDEF	CLVGA				; Video 7 VGA?
IFNDEF	V7VGA
.ERR	Cirrus Logic support needs Video 7 support
ENDIF
IF2
%OUT	Cirrus Logic support (VEGA VGA, ACER and others)
ENDIF
ISVGA
ENDIF

IFDEF	VGAMONO 			; Monochrome VGA
IF2
%OUT	Monochrome VGA support
ENDIF
ISVGA
ENDIF

IFDEF	PVGA				; Paradise VGA
IF2
%OUT	Paradise VGA support
ENDIF
ISVGA
ENDIF

IFDEF	IBMVGA				; True blue
IF2
%OUT	IBM VGA support
ENDIF
ISVGA
ENDIF

IFDEF	TLVGA				; Tseng Labs VGA
IF2
%OUT	Tseng Labs VGA support
ENDIF
ISVGA
ENDIF

IFDEF	GENVGA				 ; GENOA SuperVGA
IF2
%OUT	GENOA SuperVGA support
ENDIF
ISVGA
ENDIF

;***************************************
; Default VGA
IFDEF	VGA
IFNDEF	VGA8514 			; 8514 and CHIPs are special VDD's
IFNDEF	CTVGA
GENVGA		equ	-1		; Turn on GENOA SuperVGA code
CLVGA		equ	-1		; Turn on Cirrus Logic code
V7VGA		equ	-1		; Turn on Video 7 code
TLVGA		equ	-1		; Turn on Tseng Labs code
PVGA		equ	-1		; Turn on Paradise VGA code
VGAMONO 	equ	-1		; Turn on VGA support for mode 7 and F
ENDIF
ENDIF	; NOT CTVGA
ENDIF	; NOT VGA8514

; Flavors of EGA
IFDEF	VEGA				; Video 7 EGA
IF2
%OUT	VEGA support
ENDIF
ISEGA
ENDIF
IFDEF	PEGA				; Paradise EGA(1 and 2 support)
IF2
%OUT	PEGA support
ENDIF
ISEGA
ENDIF
IFDEF	ATIEGA				; ATI EGA(800 and 800+ support)
IF2
%OUT	ATI EGA support
ENDIF
ISEGA
ENDIF

; Default EGA
IFDEF		EGA
PEGA		equ	-1		; Turn on Paradise EGA code
VEGA		equ	-1		; Turn on Video 7 VEGA code
ATIEGA		equ	-1		; Turn on ATI EGA codee
ENDIF

;***************
;  Video display equates
;	...Colr are for CGA and EGA color modes
;	...Mono are for EGA monochrome modes
;
pIndx6845Colr	equ	3D4h		; 6845 index register(or EGA CRTC)
pIndx6845Mono	equ	3B4h
pData6845Colr	equ	3D5h		; 6845 data register(or EGA CRTC)
pData6845Mono	equ	3B5h

pStatColr	equ	3DAh		; Status register
pStatMono	equ	3BAh
fStatEna	equ	00000001b	;   Video RAM access OK for processor
fStatLPTr	equ	00000010b	;   Light pen triggered
fStatLPSw	equ	00000100b	;   Light pen switch is "on"
fStatDots	equ	00001000b	;   Video dot stream in alpha modes
CGA_Stat_IVal	equ	fstatEna+fStatDots  ; Initial value for pseudo status

pFeatColr	equ	pStatColr	; Feature control
pFeatMono	equ	pStatMono
pFeatVGAIn	equ	3CAh		; VGA feature control read
fFeatFC0	equ	00000001b	;   FC0
fFeatFC1	equ	00000010b	;   FC1


pLPen1Colr	equ	3DBh		; Light pen latch clear
pLPen1Mono	equ	3BBh
pLPen2Colr	equ	3DCh		; Light pen latch set
pLPen2Mono	equ	3BCh

pModeColr	equ	3D8h		; mode register NOT USED FOR EGA
pModeMono	equ	3B8h		;
; Note that 80x25 and Blink bits are only bits used for monochrome
fMod8025	equ	00000001b	;   80x25 alpha mode
fModMedRes	equ	00000010b	;   graphics mode
fModBW		equ	00000100b	;   B & W mode
fModVidEna	equ	00001000b	;   video enable
fModHiRes	equ	00010000b	;   640 x 200 B & W graphics
fModBlink	equ	00100000b	;   Blink bit(or intensity) in alpha
CGA_Mod_40BW	equ	00101100b	;   40x25 alpha B & W
CGA_Mod_40Clr	equ	00101000b	;   40x25 alpha color
CGA_Mod_80BW	equ	00101101b	;   80x25 alpha B & W
CGA_Mod_80Clr	equ	00101001b	;   80x25 alpha color
CGA_Mod_LoBW	equ	00001110b	;   320x200 B & W Graphics
CGA_Mod_LoClr	equ	00001010b	;   320x200 Color Graphics
CGA_Mod_HiBW	equ	00011110b	;   640x200 B & W Graphics
Mon_Mod_80	equ	00100001b	;   80x25 Monochrome

;Note low resolution(160 x 100) mode uses 40x25 alpha mode above

;***************
;  CGA video display equates
;
pColrCGA	equ	3D9h		; CGA color register
fColrBack	equ	00001111b	;   Background/border color
fColrBPalAlpha	equ	00010000b	;   Background palette in Alpha modes
fColrFPalLoClr	equ	00100000b	;   Foreground palette in med res mode
CGA_Colr_80Clr	equ	00000000b	;   80x25 alpha color default

;***************
; EGA Video display equates
;
pMiscEGA	equ	3C2h		; Miscellaneous output
pMiscVGAIn	equ	3CCh		; VGA read Misc. output
fMiscPNum	equ	00000001b	;   3Dx port numbers(vs. 3Bx)
fMiscREna	equ	00000010b	;   Ram enable
fMiscDot	equ	00001100b	;   Dot clock select
fMiscOSrc	equ	00010000b	;   Output source
fMiscPage	equ	00100000b	;   Page bit for odd/even mode
fMiscHPol	equ	01000000b	;   Horizontal retrace polarity
fMiscVPol	equ	10000000b	;   Vertical retrace polarity

pStt0EGA	equ	pMiscEga	; Miscellaneous status
fStt0Swit	equ	00010000b	;   Switch sense(addr'd by dot clk sel)
fStt0FCI0	equ	00100000b	;   Feature card input 0
fStt0FCI1	equ	01000000b	;   Feature card input 1
fStt0VRTC	equ	10000000b	;   VRTC

pAttrEGA	equ	3C0h		; Attribute(palette) address/data
mAttrIndx	equ	00011111b	;   Attribute register address mask
fVAI_Indx	equ	10000000b	    ; sign bit of Attribute index is
fVAI_IndxBit	equ	7		    ;	flag indicating port is index
fVAI_ScOn	equ	00100000b	    ; Indicates screen is on
fVAI_ScOnBit	equ	5
mAIndx		equ	01Fh		    ; Just the index bits


pSeqIndx	equ	3C4h		; Sequencer address
pSeqData	equ	3C5h		; Sequencer data
fSeq0ARst	equ	00000001b	;   0:Async reset
fSeq0SRst	equ	00000010b	;   0:Sync reset
fSeq1DPCh	equ	00000001b	;   1:Dots per character
fSeq1BWid	equ	00000010b	;   1:Band Width
fSeq1ShLd	equ	00000100b	;   1:Shift load(0=every char, 1=skip)
fSeq1DClk	equ	00001000b	;   1:Dot clock(=1 if halved)
mSeq2WMsk	equ	00001111b	;   2:Write mask
mSeq3ChrB	equ	00000011b	;   3:Char map B select(attr bit 3 = 0)
mSeq3ChrA	equ	00001100b	;   3:Char map A select(attr bit 3 = 1)
fSeq4Alph	equ	00000001b	;   4:Alpha mode(char gen enabled)
fSeq4ExtM	equ	00000010b	;   4:Extended memory installed
fSeq4SqAd	equ	00000100b	;   4:Seq'l mem addr'ing(vs. odd/even)
IFDEF VGA
fSeq4Chain4	equ	00001000b	;   4:Chain 4 (double odd/even) for
					;     256 color (mode 13)
ENDIF

pGrp1Pos	equ	3CCh		; Graphics posn 1(=0)
pGrp2Pos	equ	3CAh		; Graphics posn 2(=1)
pGrpIndx	equ	3CEh		; Graphics controller address
pGrpData	equ	3CFh		; Graphics controller data
fGrp0StV0	equ	00000001b	;   0:Value for plane 0 if enabled
fGrp0StV1	equ	00000010b	;   0:Value for plane 1 if enabled
fGrp0StV2	equ	00000100b	;   0:Value for plane 2 if enabled
fGrp0StV3	equ	00001000b	;   0:Value for plane 3 if enabled
fGrp1Ena0	equ	00000001b	;   1:Enable for plane 0
fGrp1Ena1	equ	00000010b	;   1:Enable for plane 1
fGrp1Ena2	equ	00000100b	;   1:Enable for plane 2
fGrp1Ena3	equ	00001000b	;   1:Enable for plane 3
mGrp2Colr	equ	00001111b	;   2:Color compare value
mGrp3RCnt	equ	00000111b	;   3:Rotate left count for mode 0
mGrp3Func	equ	00011000b	;   3:Function for modes 0 and 2
mGrp4RMsk	equ	00000111b	;   4:Read map select
mGrp5WMod	equ	00000011b	;   5:Write mode
fGrp5Test	equ	00000100b	;   5:Test condition
fGrp5RMod	equ	00001000b	;   5:Read mode(0=plane,1=compare)
fGrp5SqAd	equ	00010000b	;   5:Seq'l mem addr'ing(vs. odd/even)
fGrp5CGA	equ	00100000b	;   5:CGA 4 color mode addressing
fGrp6Char	equ	00000001b	;   6:Char or graphics
fGrp6Chain	equ	00000010b	;   6:Chain odd maps after even
mGrp6Addr	equ	00001100b	;   6:Processor view of VRAM start addr
					;	00=A000 for 128kb
					;	01=A000 for 64kb
					;	10=B000 for 32kb
					;	11=B800 for 32kb
mGrp7Colr	equ	00001111b	;   7:Color don't care for read compare
					;   8:Mask: use latch value vs. data

pCMode		equ	3C6h		; Control mode(Compaq specific)

IFDEF		PEGA
pPReg		equ	3DBh		; Paradise register
pShdw		equ	3DFh		; Shadow enable(Paradise specific)
fShdwEna	equ	40h		;   shadow enabled bit
fShdwEnaBit	equ	5
pPStt		equ	3D8h		; Unlock special features/status port
vPSNorm 	equ	0FFh		;   PShdw mem not active
vPShdw		equ	29h		;   PShdw mem active

mPEna		equ	0Fh		; PEGA Enable mask
vPDis		equ	0Ah		;   PEGA disable/lock value
vPDisOut	equ	0AAh		;   PEGA disable/lock value for output
vPEna		equ	05h		;   PEGA enable/unlock value
vPEnaOut	equ	055h		;   PEGA enable/unlock value for output
xA_PALok	equ	11h		; Attr ctlr lock enable index
xC_PScrm	equ	1Fh		; CRTC scrambler enable index
xC_HTime	equ	0		; CRTC Horiz timings lock enable index

xC_PNew 	equ	1Ah		; CRT ctlr new paradise reg index
ENDIF

;****************************************************************************
; Virtual Display Device(VDD) equates
;


;***************
; Timeout period to check for video updates when VM is windowed
;
VDD_Def_Update_Time EQU 50		; Default window update time = 50ms

;***************
; Priority boost for System VM when grabber is rendering VM into a window.
;
VDD_Pri_Device_Boost EQU Low_Pri_Device_Boost

;**************************************
; BIOS instance definitions
;
EGA_I_Addr1 EQU 449h
EGA_I_Len1  EQU 1Eh
EGA_I_Addr2 EQU 484h
EGA_I_Len2  EQU 7
EGA_I_Addr3 EQU 4A8h
EGA_I_Len3  EQU 4
EGA_I_Addr4 EQU 410h
EGA_I_Len4  EQU 2

;**************************************
; Message mode screen dimensions
;
VDD_Msg_ScrnWid EQU 80			; 80 columns
VDD_Msg_ScrnHgt EQU 25			; 25 rows


;***************
; Instance Map equates
;   VxD_IM_Len	defines the number of instance map ranges to reserve
;   VxD_IMV_Len defines the number of bytes those ranges represent
;
VDD_IM_Len	equ	4		; 4 ranges of RAM in instance data
VDD_IMV_Len	equ	1*4+1Eh+5+1	; 1 pointer, 1Eh bytes, 5 bytes, 1 byte

;***************
; Init values for V_VertCnt and V_HorizCnt
VDD_VERT_PERIOD equ	12		; Must be at lease 11 for Wordstar 2000
VDD_HORIZ_PERIOD equ	255		; Trial Value must not be too big

;****************
; Delay window update by this time to avoid screen size bounce 
; during a 43 line mode change
; Related to the fModeChange in VDD_Flags.
; At the beg. of a mode change this bit is set,
; at the end of a mode change we set a long time out and,
; at the end of this time out we clear this bit in VDD_VM_Time_Out. 
;
; when this bit is set - vdd_get_mod returns no changes,
;                      - vdd_VMIdle skips setting a timeout 
;                      - vdd_clr_vm_time_out does not clear any time out.
;                    
; 
;
VDD_ModeChangeDelay     equ     0100    ; long time out during a mode change

;****************************************************************************
; Virtual Display Device(VDD) structure definitions
;

;***************
; Page handle structure for video memory(up to 64k * 4 planes)
VPHMax		equ	64*4/4		; 64 pages(256Kb)
.ERRE		VPHMax EQ 64		; Change mask and algorithm if changes

VDD_PgHndl_struc struc
VPH_Mode	db	?		; Mode of saved memory
VPH_PgCnt	db	?		; Video pages alloc'd count
VPH_LLen	dw	?		; Text mode line len (= 2 * columns)
VPH_Rows	db	?		; Text mode rows
		db	?,?,?		; Unused
VPH_PgSz	dd	?		; Bytes in one displayed page
VPH_PgAllMsk	dd	?,?		; Video pages alloc'd mask, 1 byte/plane
VPH_PgAccMsk	dd	?,?		; Pages accessed mask
VPH_hMem	dd	?		; Memory handle
VPH_MState	db	SIZE VDA_Mem_State DUP (?)
VDD_PgHndl_struc ends
.ERRE	(VDA_Mem_DPagOff-VDA_Mem_PgMap) EQ VPHMax

VPH_Pg2AllMsk	EQU	DWORD PTR VPH_PgAllMsk+4    ; 2nd 32k alloc'd mask
VPH_Pg2AccMsk	EQU	DWORD PTR VPH_PgAccMsk+4    ; 2nd 32k accessed mask

; Note that the memory is saved 1st 32k plane 0, plane 1, plane 2, plane 3
;				2nd 32k plane 0, plane 1, plane 2, plane 3

;****************
; Controller state structure definitions
IFDEF VEGA
iMax_VgRAM	equ 16			; Number of VEGA scratch registers
ENDIF

;****************
; CRTC registers structure definition
;
SL_Resvd	equ	11

IFDEF	VGA
AL_Resvd	equ	11
ELSE
AL_Resvd	equ	12
ENDIF
IFDEF CTVGA
        CL_Resvd	equ	7-2     ; 2 extended CRTC regs for CTVGA
ELSE
IFNDEF TLVGA
	CL_Resvd	equ	7
ENDIF
ENDIF

IFDEF CTVGA
        GL_Resvd	equ	7-1     ; 1 extended Graphics Reg for CTVGA
ELSE
IFDEF PVGA
	GL_Resvd	equ	0	; 7 extended Graphics regs for PVGA
ELSE
        GL_Resvd	equ	7
ENDIF
ENDIF
VL_Resvd	equ	1
VDDL_Resvd	equ	128

IFDEF ATIEGA
pATI_Indx	equ	3CBh
pATI_Data	equ	3CDh
ATI_RegCount	equ	1Fh
ENDIF


CRTC_Struc	struc
C_HTotal	db	?		; Horizontal total
C_HDisp 	db	?		; Horizontal Displacement
C_HBlnkStrt	db	?		; Horizontal Blank start
C_HBlnkEnd	db	?		; Horizontal Blank end
C_HRTCStrt	db	?		; Horizontal retrace start
C_HRTCEnd	db	?		; Horizontal retrace end
C_VTotal	db	?		; Vertical total
C_Ovflw 	db	?		; 8th bit for following
C_RowScn	db	?		; First row after VRTC
C_MaxScan	db	?		; Max scan line
C_CStart	db	?		; Cursor start scan line
C_CEnd		db	?		; Cursor end scan line
C_AddrH 	db	?		; VRAM address high byte
C_AddrL 	db	?		; VRAM address low byte
C_CAddrH	db	?		; Cursor address high byte
C_CAddrL	db	?		; Cursor address low byte
; Note following two registers have read values as noted
C_VRTCStrt	db	?		; Vertical retrace start (write only)
					; Light pen address high byte(read only)
C_VRTCEnd	db	?		; Vertical retrace end(write only)
					; Light pen address low byte(read only)
C_VDspEnd	db	?		; Vertical display end line
C_LnOff 	db	?		; Offset to next line
C_UndrLn	db	?		; Underline
C_VBlnkStrt	db	?		; Vertical blanking start
C_VBlnkEnd	db	?		; Vertical blanking end
C_Mode		db	?		; Mode
C_LnCmp 	db	?		; Line compare for MA clearing

IFDEF   CTVGA
C_CT400         db      ?
C_CTTempFE      db      ?
ENDIF

IFDEF	TLVGA
		db	2 DUP (?)	; Unused
C_XZ_Strt	db	?		; X Zoom Start
C_XZ_End	db	?		; X Zoom End
C_YZ_StrtLo	db	?		; Y Zoom Start Low
C_YZ_EndLo	db	?		; Y Zoom End Low
C_YZ_StEndHi	db	?		; Y Zoom Start/End Hi
C_Z_StLo	db	?		; Zoom Start Lo
C_Z_StMid	db	?		; Zoom Start Mid
		db	?		; Unused
C_Extd_Strt	db	?		; Extended start address
C_Compat	db	?		; Compatibility
C_OvflwHi	db	?		; More overflow bits
		db	2 DUP (?)	; Unused - DWORD boundary
ELSE
C_Resvd 	db	CL_Resvd dup (?); Future expansion
ENDIF

CRTC_Struc	ends

;****************
; EGA controller state
EGA_State_struc     struc

; CRT controller(must be first in EGA state struc so can access using CRTC
;   structure definitions
CRTC		db	size CRTC_Struc dup (?)

; Miscellaneous
V_Misc		db	?		; Miscellaneous output

; Attribute controller
A_Pal		db	16 dup (?)	; Color palette
A_Mode		db	?		; Mode
A_OvScn 	db	?		; Over scan color
A_PlEna 	db	?		; Plane enable to attr reg
A_HPan		db	?		; Horizontal panning
IFDEF	VGA
A_Color 	db	?
ENDIF
A_Resvd 	db	AL_Resvd dup (?); Future expansion

; Sequencer
S_Rst		db	?		; Reset
S_ClMode	db	?		; Clocking mode
S_Mask		db	?		; Map mask
S_ChMap 	db	?		; Char map select
S_MMode 	db	?		; Memory mode
S_Resvd 	db	SL_Resvd dup (?); Future expansion

; Graphics controller
G_SetRst	db	?		; Set/Reset on write
G_SREna 	db	?		; Set/Reset enable
G_CComp 	db	?		; Color compare
G_Func		db	?		; Data rotate and function
G_MSlct 	db	?		; Read Map select
G_Mode		db	?		; Mode
G_Misc		db	?		; Miscellaneous
G_DCare 	db	?		; Color don't care on compare reads
G_Mask		db	?		; Mask for data/latches on write
IFDEF	PVGA
G_PVGA0A	db	?		; Address A
G_PVGA0B	db	?		; Address B
G_PVGA1 	db	?		; Memory size
G_PVGA2 	db	?		; Video select
G_PVGA3 	db	?		; CRT control
G_PVGA4 	db	?		; Video control
G_PVGA5 	db	?		; Enable features if out 3CF,0X5h
ELSE
IFDEF   CTVGA
G_CTCtl           db      ?
ENDIF
G_Resvd 	db	GL_Resvd dup (?); Future expansion
ENDIF

; Index registers
A_Indx		db	?		; Attribute controller index register
S_Indx		db	?		; Sequencer index register
C_Indx		db	?		; CRT controller index register
G_Indx		db	?		; Graphics controller index register

; Miscellaneous registers
G_Pos1		db	?		; Graphics 1 position(must be 0)
G_Pos2		db	?		; Graphics 1 position(must be 1)
V_Feat		db	?		; Feature connector output
V_VGAEna	db	?		; VGA - subsystem enable

; Status(input) registers
V_Stat0 	db	?		; Status 0
V_Stat1 	db	?		; Status 1
V_Resvd 	db	VL_Resvd dup (?); padding

IFDEF	PEGA
V_Flag_PEGA	dw  ?			; Paradise state flags

V_PSttCnt	db  ?			; Paradise status access cnt
		db  ?			; Pad to DWORD

; Must keep 4 sets of CRTC regs for PEGA, perturbations of locked and scrmbld
; The following are flags which indicate which regs are enabled/written
VC_PCRegMsk	dd  ?			; Enable mask for locked CRTC registers
VC_PCUnUMsk	dd  ?			; Unscrambled, unlkd regs written mask
VC_PCUnLMsk	dd  ?			; Unscrambled, locked regs written mask
VC_PCScUMsk	dd  ?			; Scrambled, unlocked regs written mask
VC_PCScLMsk	dd  ?			; Scrambled, locked regs written mask
VC_PShdw	db  ?			; Paradise shadow enable reg
VC_PReg 	db  ?			; Paradise register
VC_PNew 	db  ?			; Paradise New register
VC_PMisc	db  ?			; Paradise Misc reg command
VC_PCUnL	db  32 DUP (?)		; Unscrambled, locked CRTC values
VC_PCScU	db  32 DUP (?)		; Scrambled, unlocked CRTC vals
VC_PCScL	db  32 DUP (?)		; Scrambled, locked CRTC vals
ENDIF
IFDEF	V7VGA
VDD_V7Flag	db	?		; Bit 0 = save 256K always, Bit 1 = save 512K always
VDD_V7VSEna	db	?		; 3C3H - Video subsystem enable
VDD_V7PPM	db	?		; 3C6H - Palette Pixel Mask
		db	0
VDD_V7ER        db      80h dup (?)     ; Save area for Video 7 ext. regs.
ENDIF

IFDEF	ATIEGA
; ATI extensions index and data registers
ATI_Regs	db	ATI_RegCount dup (?)
ATI_Indx	db	?
ENDIF

EGA_State_struc 	ends
; Make sure CRTC_Struc fits into above save areas
IFDEF	PEGA
.ERRE	    (SIZE CRTC_Struc) LE 32
ENDIF

.ERRNZ	CRTC

;****************
; Control Block (CB) structure definition
;
CB_EGA_Struc	 struc
VDD_Flags	dd  ?			; VDD flags
VDD_EFlags	dd  ?			; VDD Event flags
VDD_Latches	db  ?,?,?,?		; EGA latch for each plane
VDD_DirtyPages	dd  ?			; Dirty video memory state
VDD_DirtyPages2 dd  ?			;   Extended pages
VDD_AccPages	dd  ?			; Pages written (for updating windows)
VDD_AccPages2	dd  ?			;   Extended pages
VDD_Stt 	db  size EGA_State_struc dup (?) ; Controller state structure
VDD_SttCopy	db  size EGA_State_struc dup (?) ; Controller state copy

IFDEF		VGA
; VDD_SaveMsgStt copies from VDD_DAC thru VDD_DAC_Pad2, keep contiguous
VDD_DAC 	dd  256 DUP(?)		; DAC save space
VDD_PEL_Addr	dd  ?			; address within VDD_DAC array
VDD_PEL_Idx	db  ?			; byte index within PEL dword entry (1-3)
VDD_PEL_Mode	db  ?			; 0 - read mode, 3 - write mode
VDD_DAC_Pad2	db  2 DUP (?)		; padding
ENDIF

IFDEF		PEGA
; Place to save CRTC values written (based on Locked, Scrambled or not)
VDD_Cur_CRTC	dd  ?			; Current CRTC regs ptr
; PEGA scratch pad memory handle and address
VDD_hPMem	dd  ?			; Paradise scratch mem handle
VDD_PMemPtr	dd  ?			; Paradise scratch mem address
ENDIF

VDD_ModeEGA	db  ?			; Mode of controller state
VDD_LastMode	db  ?			; Last mode VMDA aware of
VDD_MMode	db  ?			; Mode according to VRAM
VDD_VertCnt	db  ?			; Hold VRTC Counter
VDD_HorizCnt	db  ?			; Toggle HRTC Counter
VDD_Pg		db  size VDD_PgHndl_struc dup (?) ; VM's video pages
VDD_CPg 	db  size VDD_PgHndl_struc dup (?) ; VM's video pages copy
VDD_PIF 	dw  ?			; VDD PIF bits
VDD_ModTime	dd  ?			; System time at last video mod
VDD_Time_Out_Handle dd	?		; Display update timeout handle
VDD_Routine	dd  ?			; I/O trap rtns table addr
VDD_Upd_Start	dd  ?			; Start time of window update

IFDEF		VEGA
VDD_VgRAM	db  iMax_VgRAM DUP (?)	; VEGA scratch registers
VDD_VgRAMI	db  ?			; VEGA scratch register index
		db  ?,?,?		; padding
ENDIF
VDD_Get_Mem_Count dd    ?               ; Match calls to VDD_Get_Mem/VDD_Free_Mem
                                        ; VDD_UnLock_APP unfreezes the App only
					; if this count is Zero.
VDD_Update_Hit_Cnt  db	?		; hack to delay next timeout if getting too
					; many consecutive update events
		db  3 DUP(?)		; padding to dword

CB_EGA_Struc	 ends

;******* Special video mode values
; When mem mapped virtually, this holds video mode: 0-6
; Otherwise it holds one of the following values.
VDD_MMPhys	EQU 0FFh		;   Physical or undefined (virtual)
VDD_MM2nd	EQU 0FEh		;   Remapped to 2nd half of physical


;***************
; VDD Flag bit definitions
;
fVDD_Win	equ	00000001h	    ; VM is displayed in window
fVDD_WinBit	equ	0
fVDD_HCurTrk	equ	00000002h	    ; Track cursor horizontally
fVDD_HCurTrkBit equ	1
fVDD_Grab	equ	00000004h	    ; Grab event pending
fVDD_GrabBit	equ	2
fVDD_ModeChange equ	00000008h	    ; indicate mode change in progress
fVDD_ModeChangeBit equ	3
fVDD_ModeSet	equ	00000010h	    ; Video in INT 10 mode set
fVDD_ModeSetBit equ	4
fVDD_Save	equ	00000020h	    ; We have EGA state to restore
fVDD_SaveBit	equ	5
fVDD_MEna	equ	00000040h	    ; Video memory enabled
fVDD_MEnaBit	equ	6
fVDD_Font	equ	00000080h	    ; Memory mapped for font load
fVDD_FontBit	equ	7
fVDD_256	equ	00000100h	    ; This VM needs 256k video RAM
fVDD_256Bit	equ	8
fVDD_MdSvRAM	equ	00000200h	    ; RAM not destroyed during mode chg
fVDD_MdSvRAMBit equ	9
fVDD_old256	equ	00000400h	    ; Set in VDD_I10_Mode_Switch
fVDD_old256Bit	equ	10		    ;	if fVDD_256 set
fVDD_Upd	equ	00000800h	    ; Update event pending
fVDD_UpdBit	equ	11
fVDD_MonoMem	equ	00001000h	    ; Monochrome mem currently mapped
fVDD_MonoMemBit equ	12
fVDD_WaitAtt	equ	00002000h	    ; VDD suspended VM until attached
fVDD_WaitAttBit equ	13
fVDD_PVGA_Ena	equ	00004000h	    ; Paradise VGA regs are enabled
fVDD_PVGA_EnaBit equ	14
fVDD_ForcedUpd 	equ	00008000h	    ; Force window update after VMInit
fVDD_ForcedUpdBit	equ	15
fVDD_SuspEvent	equ	00010000h	    ; Suspend VM Event has been queued
fVDD_SuspEventBit	equ	16
fVDD_ATIEna	equ	00020000h	    ; ATI register is enabled
fVDD_ATIEnaBit	equ	17
fVDD_UpdAll1	equ	00040000h	    ; see notes in VDD_Clear_Mod
fVDD_UpdAll1Bit	equ	18
fVDD_UpdAll2 	equ	00080000h	    ; see notes in VDD_Clear_Mod
fVDD_UpdAll2Bit	equ	19
fVDD_AXRd	equ	00100000h	    ; Attribute index read(VGA)
fVDD_AXRdBit	equ	20
fVDD_NoTrap	equ	00200000h	    ; Don't trap I/O ports(use INT 2F)
fVDD_NoTrapBit	equ	21
fVDD_IOT	equ	00400000h	    ; I/O ports presently trapped
fVDD_IOTBit	equ	22
fVDD_NTSave	equ	00800000h	    ; NotTrap save during mode changes
fVDD_NTSaveBit	equ	23
fVDD_Boost	equ	01000000h	    ; SYS VM boosted for memory read
fVDD_BoostBit	equ	24
fVDD_25_unused  equ	02000000h	    
fVDD_25_Bit     equ	25
fVDD_26_unused  equ     04000000h
fVDD_26_Bit     equ     26
fVDD_Hide	equ	08000000h	    ; Hide cursor in window
fVDD_HideBit	equ	27
fVDD_28_unused  equ     10000000h
fVDD_28_Bit     equ     28
fVDD_29_unused  equ     20000000h
fVDD_29_Bit     equ     29
fVDD_ScrollModf equ	40000000h	    ; Previous get_mod  returned scroll event
fVDD_ScrollModfBit	equ	30          ; see notes in VDD_Clear_Mod
fVDD_MInit	equ	80000000h	    ; Display mem initialized
fVDD_MInitBit	equ	31

;***************
; VDD_EFlags definitions, error and error msg status
;	Note that Msg bits are error bits shifted left 8
;
fVDE_Crit	equ	00000001h	    ; Save during VDD critical section
fVDE_CritBit	equ	0
fVDE_NoMain	equ	00000100h	    ; Out of main, display not accurate
fVDE_NoMainBit	equ	8
fVDE_NoCopy	equ	00000200h	    ; Out of copy, window not accurate
fVDE_NoCopyBit	equ	9
fVDE_NoFGrnd	equ	00000400h	    ; Cannot run bkgrnd with fgrnd VM
fVDE_NoFGrndBit equ	10
fVDE_NoWind	equ	00000800h	    ; Cannot window this VM
fVDE_NoWindBit	equ	11
fVDE_Excl	equ	00001000h	    ; Background high Res excluded
fVDE_ExclBit	equ	12
fVDE_NoSupMode	equ	00002000h	    ; Mode not supported
fVDE_NoSupBit	equ	13
fVDE_NoMMsg	equ	00010000h	    ; User told out of main
fVDE_NoMMsgBit	equ	16
fVDE_NoCMsg	equ	00020000h	    ; User told out of copy
fVDE_NoCMsgBit	equ	17
fVDE_NoFGMsg	equ	00040000h	    ; User told cannot run with fgrnd
fVDE_NoFGMsgBit equ	18
fVDE_NoWMsg	equ	00080000h	    ; User told cannot window
fVDE_NoWMsgBit	equ	19
fVDE_ExclMsg	equ	00100000h	    ; User told VM exclusive
fVDE_ExclMsgBit equ	20
fVDE_NoSMsg	equ	00002000h	    ; User told mode not supported
fVDE_NoSMsgBit	equ	21

; VDD_Flag_PEGA: VM specific flags for state of PEGA chip
fVPa_PShdw	equ	0001h		;   Paradise shadow memory is enabled
fVPa_PShdwBit	equ	0
fVPa_PReg	equ	0002h		;   Paradise reg written to
fVPa_PRegBit	equ	1
fVPa_PNew	equ	0004h		;   New paradise reg written to
fVPa_PNewBit	equ	2
fVPa_PCLok	equ	0008h		;   Paradise CRTC lock reg written to
fVPa_PCLokBit	equ	3
fVPa_PALok	equ	0010h		;   Paradise Attr lock reg written to
fVPa_PALokBit	equ	4
fVPa_PScrm	equ	0020h		;   Paradise CRTC Scramble written to
fVPa_PScrmBit	equ	5
fVPa_PMisc	equ	0040h		;   Paradise Misc mode reg written to
fVPa_PMiscBit	equ	6
fVPa_PCLokd	equ	0080h		;   Paradise CRTC locked
fVPa_PCLokdBit	equ	7
fVPa_PALokd	equ	0100h		;   Paradise Attr locked
fVPa_PALokdBit	equ	8
fVPa_PCScrmd	equ	0200h		;   Paradise CRTC Scramble enabled
fVPa_PCScrmdBit equ	9

;***************
; The following equates and .ERRNZ make sure that the I/O trap routines
;   correctly do not write beyond the end of the save area reserved for
;   each of the controllers.
A_IMax		equ	A_Resvd-A_Pal+AL_Resvd-1
A_IMsk		equ	1Fh
.ERRNZ		A_IMax-A_IMsk
.ERRNZ		mAIndx-A_IMsk
IFDEF	PVGA
G_IMax		equ	G_PVGA5-G_SetRst
ELSE
G_IMax		equ	G_Resvd-G_SetRst+GL_Resvd-1
ENDIF
G_IMsk		equ	0Fh
.ERRNZ		G_IMax-G_IMsk
S_IMax		equ	S_Resvd-S_Rst+SL_Resvd-1
S_IMsk		equ	0Fh
.ERRNZ		S_IMax-S_IMsk
IFNDEF	TLVGA
C_IMax		equ	C_Resvd-C_HTotal+CL_Resvd-1
C_IMsk		equ	1Fh
.ERRNZ		C_IMax-C_IMsk
ELSE
C_IMax		equ	C_OvflwHi-C_HTotal+2
C_IMsk		equ	3Fh
.ERRNZ		C_IMax GE C_IMsk
ENDIF

;***************
; VDD global variables
;
Extern_vgVDD MACRO
EXTRN	Vid_VM_Handle:dword
EXTRN	Vid_VM_Handle2:dword
EXTRN	Vid_VM_HandleRun:dword
EXTRN	Vid_Flags:dword
ENDM

;
; VDD global flag definitions
;
fVid_SysVMI	equ	0001h		; VDD_VMInit called for SYS VM
fVid_SysVMI_Bit equ	0
fVid_TLVGA	equ	0002h		; Running on Tseng Labs VGA (more regs!)
fVid_TLVGABit	equ	1
fVid_PVGA	equ	0004h		; Running on Paradise VGA (more regs)
fVid_PVGABit	equ	2
fVid_PEGA	equ	0008h		; Running on PEGA board(Shadow RAM)
fVid_PEGABit	equ	3
fVid_DOff	equ	0010h		; Display is off(during save/restore)
fVid_DOffBit	equ	4
fVid_PCLokd	equ	0020h		; CRTC timing locked(PEGA)
fVid_PCLokdBit	equ	5
fVid_PALokd	equ	0040h		; Attr values locked(PEGA)
fVid_PALokdBit	equ	6
fVid_PScrmd	equ	0080h		; Attr values locked(PEGA)
fVid_PScrmdBit	equ	7
fVid_Mono	equ	0100h		; VDD supports Mono VGA
fVid_MonoBit	equ	8		;
IFDEF	    CHIPTECH
fVid_CTVGA	equ	0200h		; CHIPS VGA(82c441) controller
fVid_CTVGABit	equ	9
ENDIF
fVid_Crit	equ	0400h		; In video critical section
fVid_CritBit	equ	10
fVid_PageSwapInst equ	0800h		; Set if PageSwap device installed
fVid_PageSwapInstBit equ	11
fVid_MsgR	equ	1000h		; System message mode in Run state
fVid_MsgRBit	equ	12
fVid_MsgA	equ	2000h		; System message mode active
fVid_MsgABit	equ	13
fVid_V7VGA	equ	4000h		; Running on Video 7 VGA (more regs!!!)
fVid_V7VGABit	equ	14
fVid_CLVGA	equ	8000h		; Running on Cirrus Logic VGA
fVid_CLVGABit	equ	15
fVid_PVGARom	equ	10000h		; Paradise VGA ROM (Exclude C600-C7FF)
fVid_PVGARomBit equ	16
fVid_ATI800	equ	20000h		; ATI EGA 800 or 800+ adapter
fVid_ATI800Bit	equ	17
fVid_ATIWon	equ	40000h		; ATI EGA Wonder adapter
fVid_ATIWonBit	equ	18
fVid_GENOA	equ	80000h		; GENOA SuperVGA (Exclude C600-C7FF)
fVid_GENOABit	equ	19
fVid_8514	equ	100000h 	; IBM 8514
fVid_8514Bit	equ	20
fVid_ResvB0	equ	200000h 	; Reserve memory from B0 to B7
fVid_ResvB0Bit	equ	21
fVid_RsrvC6C7	equ	400000h 	; Reserve C6 & C7 as ROM without a signature
fVid_RsrvC6C7Bit equ	22
fVid_SuprsMsg	equ	800000h 	; Don't display error dialogs
fVid_SuprsMsgBit equ	23


; Mask for hardware with ROM without ROM signature at C6 and C7 pages
; Because "IFDEF xxxx OR yyyy" doesn't work
RSRVRomC6C7Msk EQU fVid_TLVGA+fVid_V7VGA+fVid_PVGARom+fVid_GENOA+fVid_8514

;*******
; Macros for dispatching to display dependent routines(VGA, EGA, CGA)
;	Address of vectors for the routines is set at initialization
;
;CallVDD macro	 RtnName
;    extrn   VDD_ProcTab:dword
;    push    eax
;    mov     eax,[VDD_ProcTab]
;    call    dword ptr [RtnName.eax]
;    pop     eax
;    endm
;
;JumpVDD macro	 RtnName
;;    extrn   VDD_ProcTab:dword
;    push    eax
;    mov     eax,[VDD_ProcTab]
;    mov     eax,[RtnName.eax]
;    xchg    eax,[esp]
;    ret
;    endm
;
;******
; Macro for defining the vector tables
;
;DefProc macro	 TableName,RtnName,EntryPoint
;.errnz      ($-TableName)-RtnName
;    dd      OFFSET32 EntryPoint
;    endm
;
;******
; Definitions for routine vector offsets
;VP_RState   equ     0				 ; Restore complete ctrlr state
;VP_PreSRAM  equ     VP_RState+4		 ; Set up for memory save/restore
;VP_RestIndx equ     VP_PreSRAM+4		 ; Restore index registers
;VP_RestCtlr equ     VP_RestIndx+4		 ; Restore the regs
;VP_RestCtl2 equ     VP_RestCtlr+4		 ; Restore 2nd EGA regs

;***************
; Special definitions of the memory used by WINDOWS DISPLAY.DRV
;	see EGAMEM.INC
;
;***************
; VM1's VRAM save area, last 16 bytes of the last displayed 4k page
;
SYS_VRAM_Addr	equ	0A7FF0h
SYS_VRAM_Len	equ	10h

SYS_VRAM_struc	    struc
current_brush	    db	  8 dup (?)	    ;8 bytes for a brush
tonys_bar_n_grill   db	  ?		    ;1 byte for a solid color
shadow_mem_status   db	  ?		    ;Status flags for upper 16/32K of mem
known_word	    dw	  ?		    ;Required locations for state
shadowed_mode	    db	  ?		    ;  detection code
plane_index	    db	  ?
enable_test	    db	  ?
saved_latches	    db	  ?
SYS_VRAM_struc	    ends

; When using the second half of the EGA, we clear the Shadow memory exists
;   bit and set the shadow_trashed bit if the shadow_in_use bit is set

shadow_exists	equ	00000001b	;Shadow memory exists
shadow_exists_Bit equ	0
shadow_in_use	equ	00000010b	;Shadow contains valid image
shadow_trashed	equ	00000100b	;Shadow contents have been destroyed

;*****************************************************************************
; The below macros assume EDX=3xx and EBX = VM handle

IFDEF	PEGA
; If CRTC has been locked, unlock it
PEGACUnlock  macro
	local	notLokd
	btr	[Vid_Flags],fVid_PCLokdBit
	jnc	SHORT notLokd
	mov	dl,(pIndx6845Colr and 0FFh)
	mov	al,xC_HTime
	out	dx,al
	IO_Delay
	mov	dl,(pPStt AND 0FFh)
	in	al,dx
	IO_Delay
	in	al,dx
	IO_Delay
	mov	dl,(pData6845Colr and 0FFh)
	mov	al,vPEnaOut
	out	dx,al
	IO_Delay
notLokd:
	endm

; Lock CRTC timing
PEGACLock  macro
	or	[Vid_Flags],fVid_PCLokd
	mov	dl,(pIndx6845Colr and 0FFh)
	mov	al,xC_HTime
	out	dx,al
	IO_Delay
	mov	dl,(pPStt AND 0FFh)
	in	al,dx
	IO_Delay
	in	al,dx
	IO_Delay
	mov	dl,(pData6845Colr and 0FFh)
	mov	al,vPDisOut
	out	dx,al
	IO_Delay
	endm

; If Attr has been locked, unlock it
PEGAAUnlock  macro
	local	notLokd
	btr	[Vid_Flags],fVid_PALokdBit
	jnc	SHORT notLokd
	mov	dl,(pStatColr AND 0FFh)
	in	al,dx
	IO_Delay
	mov	al,xA_PALok
	mov	dl,(pAttrEGA AND 0FFh)
	out	dx,al
	IO_Delay
	mov	dl,(pPStt AND 0FFh)
	in	al,dx
	IO_Delay
	in	al,dx
	IO_Delay
	mov	dl,(pAttrEGA AND 0FFh)
	mov	al,vPEnaOut
	out	dx,al
	IO_Delay
notLokd:
	endm

; Lock the Attribute controller
PEGAALock  macro
	or	[Vid_Flags],fVid_PALokd
	mov	dl,(pStatColr AND 0FFh)
	in	al,dx
	IO_Delay
	mov	al,xA_PALok
	mov	dl,(pAttrEGA AND 0FFh)
	out	dx,al
	IO_Delay
	mov	dl,(pPStt AND 0FFh)
	in	al,dx
	IO_Delay
	in	al,dx
	IO_Delay
	mov	dl,(pAttrEGA AND 0FFh)
	mov	al,vPDisOut
	out	dx,al
	IO_Delay
	endm


PEGACUnscram   macro
	local	notScramd
	btr	[Vid_Flags],fVid_PScrmdBit
	jnc	SHORT notScramd
	mov	eax,xC_PScrm
	mov	dl,(pIndx6845Colr and 0FFh)
	out	dx,al
	IO_Delay
	mov	dl,(pPStt AND 0FFh)
	in	al,dx
	IO_Delay
	in	al,dx
	IO_Delay
	mov	dl,(pData6845Colr and 0FFh)
	mov	al,vPDisOut
	out	dx,al
	IO_Delay
notScramd:
	endm

PEGACScram   macro
	or	[Vid_Flags],fVid_PScrmd
	mov	eax,xC_PScrm
	mov	dl,(pIndx6845Colr and 0FFh)
	out	dx,al
	IO_Delay
	mov	dl,(pPStt AND 0FFh)
	in	al,dx
	IO_Delay
	in	al,dx
	IO_Delay
	mov	dl,(pData6845Colr and 0FFh)
	mov	al,vPEnaOut
	out	dx,al
	IO_Delay
	endm
ENDIF

; Macro for setting 3Dx port addressing for mono supporting VDDs
Set3DxPorts MACRO
IFDEF	VGAMONO
; Make sure using 3Dx addressing
	mov	al,[edi.VDD_Stt.V_Misc]
	or	al,1
	mov	edx,pMiscEGA
	out	dx,al
	IO_Delay
ENDIF
ENDM

; Macro for restoring 3Dx or 3Bx port addressing for mono supporting VDDs
Rest3?xPorts	MACRO	MiscReg
IFDEF	VGAMONO
; Restore VM's Misc reg, 3?x addressing
	mov	al,MiscReg
	mov	edx,pMiscEGA
	out	dx,al
	IO_Delay
ENDIF
ENDM

Assert_VDD_ptrs MACRO R_Handle,R_VDDptr,Pseudo_OK
LOCAL	AVP_Ok1, AVP_Ok2
IFNDEF	VDD_CB_Off
EXTRN	VDD_CB_Off:DWORD
ENDIF
IFNB <Pseudo_OK>
IFNDEF	VDD_Msg_Pseudo_VM
EXTRN	VDD_Msg_Pseudo_VM:DWORD
ENDIF
	cmp	ebx, VDD_Msg_Pseudo_VM
	je	SHORT AVP_Ok1
ENDIF
Assert_VM_Handle    R_Handle
AVP_Ok1:
	pushfd
	push	R_VDDptr
	sub	R_VDDptr,[VDD_CB_Off]
	cmp	R_Handle,R_VDDptr
	pop	R_VDDptr
	je	short AVP_Ok2
Debug_Out   "VDD ptr not correct, #&R_VDDptr"
AVP_Ok2:
	popfd
	ENDM

    .LIST
