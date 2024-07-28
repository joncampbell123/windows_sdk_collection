RD_MASK_ENABLE		equ	0aee8h
WR_MASK_ENABLE		equ	0aae8h
MAJ_AXIS_PCNT		equ	096e8h
MISC_REG		equ	0bee8h
CUR_X_POS		equ	086e8h
CUR_Y_POS		equ	082e8h
COMMAND_REG		equ	09ae8h
STATUS_REG		equ	09ae8h	; use to read a word
Q_STATUS_REG		equ	09ae8h	; use to read a byte
CMD_STATUS_REG		equ	09ae9h	; use to read upper byte
PIX_TRANS_REG		equ	0e2e8h
FRGD_MIX_REG		equ	0bae8h
BKGD_MIX_REG		equ	0b6e8h

CMD_ACTIVE		equ	002h
DATA_AVAIL		equ	001h
NO8514			equ	040h
PLANE8			equ	080h

WRITCMD 		equ	1h
PLANAR			equ	2h
LSTPELNULL		equ	4h
STROKE_ALG		equ	8h
DRAWCMD 		equ	010h
INCX			equ	020h
YMAJAXIS		equ	040h
INCY			equ	080h
PC_TRANS		equ	0100h
BIT16			equ	0200h
BYTE_SWAP		equ	01000h

NO_FCN			equ	0h
LINE_DRAW		equ	02000h
FILL_X_RECT		equ	04000h
FILL_Y_RECT		equ	06000h
FILL_RECT		equ	08000h
AXIAL_LINE		equ	0a000h
COPY_RECT		equ	0c000h
HANG			equ	0e000h

NOT_DEST		equ	0h
ZERO			equ	1h
ONE			equ	2h
DEST			equ	3h
NOT_SRC 		equ	4h
SRC_XOR_DEST		equ	5h
NOT_OP			equ	6h
SRC			equ	7h

B_CLR_ACTIVE		equ	0h
F_CLR_ACTIVE		equ	020h
PTRANS_ACTIVE		equ	040h
ALL_PLANE_COPY		equ	060h
