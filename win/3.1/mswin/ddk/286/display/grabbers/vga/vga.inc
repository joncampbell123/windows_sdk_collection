
;
; NAME
;	EGA.INC
;
; DESCRIPTION
;	This file defines various VGA-specific constants and equates for
;	port addresses, register indices, and VGA BIOS video data structures.
;
; AUTHOR
;	Jim Christy
;
; HISTORY
;	1.00	060187	jhc	Epoch
;
;	1.10	071588	vvr	Added VGA specific font info data structure.
;	1.11	062989	vvr	added feature control register output
;
;
; Define EGA register ports
;

SEQ_ADDR	=	003C4h
SEQ_DATA	=	003C5h
CRTC_ADDR	=	003D4h			;will need to patch for mono
CRTC_DATA	=	003D5h			;will need to patch for mono
GRAPH_ADDR	=	003CEh
GRAPH_DATA	=	003CFh
GRAPH_1_POS	=	003CCh
GRAPH_2_POS	=	003CAh
ATTR_ADDR	=	003C0h
MISC_OUTPUT	=	003C2h
INPUT_STATUS_0	=	003C2h
INPUT_STATUS_1	=	003DAh			;will need to patch for mono

MISC_READ_R	=	003CCh			; for VGA 	
FEAT_CONTROL 	=	003CAh			; for VGA
FEAT_CONTROL_R	=	003DAh

;
; Define EGA register indices
;

S_RESET 	=	000h
S_CLOCK 	=	001h
S_MAP		=	002h
S_CGEN		=	003h
S_MEM		=	004h

C_HRZ_TOT	=	000h
C_HRZ_DSP	=	001h
C_STRT_HRZ_BLK	=	002h
C_END_HRZ_BLK	=	003h
C_STRT_HRZ_SYN	=	004h
C_END_HRZ_SYN	=	005h
C_VRT_TOT	=	006h
C_OVERFLOW	=	007h
C_PRE_ROW	=	008h
C_MAX_SCAN_LN	=	009h
C_CRSR_START	=	00Ah
C_CRSR_END	=	00Bh
C_STRT_HGH	=	00Ch
C_STRT_LOW	=	00Dh
C_CRSR_LOC_HGH	=	00Eh
C_CRSR_LOC_LOW	=	00Fh
C_VRT_SYN_STRT	=	010h
C_LGHT_PEN_HGH	=	010h
C_VRT_SYN_END	=	011h
C_LGHT_PEN_LOW	=	011h
C_VRT_DSP_END	=	012h
C_OFFSET	=	013h
C_UNDERLN_LOC	=	014h
C_STRT_VRT_BLK	=	015h
C_END_VRT_BLK	=	016h
C_MODE_CTL	=	017h
C_LN_COMP	=	018h

G_SET_RESET	=	000h
G_ENBL_SET	=	001h
G_CLR_COMP	=	002h
G_DATA_ROT	=	003h
G_READ_MAP	=	004h
G_MODE		=	005h
G_MISC		=	006h
G_COLOR 	=	007h
G_BIT_MASK	=	008h

A_MODE		=	010h
A_OVERSC	=	011h
A_CPLANE	=	012h
A_HPEL		=	013h


;
; Define structure of VGA BIOS video data area (extensions to standard BIOS
; video data areas).
;

VgaBiosData	struc
  ebdRows	db	?
  ebdPoints	dw	?
  ebdInfo	db	?
  ebdInfo3	db	?
  ebdFlags	db	?
  ebdDCC	db	?	
VgaBiosData	ends


;
; Define structure of an entry in the VGA video parameter table pointed to
; by the first long pointer in the table pointed to by the VGA SavePtr.
;

VgaVideoParms	struc
  evpCharsX	db	?
  evpCharsY	db	?
  evpCellY	db	?
  evpScrLen	dw	?
  evpSeqRegs	db	04	dup (?)
  evpMiscReg	db	?
  evpCrtcRegs	db	25	dup (?)
  evpAttrRegs	db	20	dup (?)
  evpGraphRegs	db	09	dup (?)
VgaVideoParms	ends


;
; Define the FontInfo data structure and constants returned by the ContextInfo
; subfunction FontInfo
;

FontInfo	struc
  fibank0	db	?
  fibank1	db	?
  fibank2	db	?
  fibank3	db	?
  fiBank4	db	?
  fibank5	db	?
  fibank6	db	?
  fiBank7	db	?
FontInfo	ends

FI_CUSTOM	=	000h
FI_EMPTY	=	0FFh
FI_MAXINDEX	=	8

NUM_SEQ_REGS	=	05h
NUM_GRAPH_REGS	=	09h
NUM_CRTC_REGS	=	018h
NUM_ATTR_REGS	=	015h
