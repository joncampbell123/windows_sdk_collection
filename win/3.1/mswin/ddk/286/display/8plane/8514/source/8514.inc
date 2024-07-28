;For a grey scale 8514 driver (256 grey scales), change the following equate
;from 0 to any non-zero value and remake the driver:

GreyScale	equ	0
;NUMCOLORS	 equ	 256
;BITSPIXEL	 equ	 8
NUMRESERVED	equ	16

;Following is a list of function equates for the 8514:

TransparentMode equ     03h
ReplaceMode     equ     07h
ReplaceOpaque   equ     0707h
ANDMode         equ     0ch
ANDOpaque       equ     0c0ch
XORMode         equ     05h
XOROpaque       equ     0505h
ORMode          equ     0bh
OROpaque        equ     0b0bh
NOTMode         equ     0
NOTOpaque       equ     0
Black           equ     01h
BlackOpaque     equ     0101h


X_SIZE                  equ     1024
Y_SIZE                  equ     768
CURSOR_SAVE_X           equ     0
CURSOR_SAVE_Y           equ     768
CURSOR_AND_INVISIBLE_X  equ     CURSOR_SAVE_X+64
CURSOR_XOR_INVISIBLE_X  equ     CURSOR_AND_INVISIBLE_X+64

;Following are some port equates:

STATUS_PORT                     equ     9ae8h
CLIP_PORT                       equ     0bee8h
MODE_PORT                       equ     0bee8h
RECT_HEIGHT_PORT                equ     0bee8h
RECT_WIDTH_PORT                 equ     96e8h
Srcx_PORT                       equ     86e8h
Srcy_PORT                       equ     82e8h
Dstx_PORT                       equ     8ee8h
Dsty_PORT                       equ     8ae8h
FUNCTION_0_PORT                 equ     0b6e8h
FUNCTION_1_PORT                 equ     0bae8h
WRITE_ENABLE_PORT               equ     0aae8h
READ_ENABLE_PORT                equ     0aee8h
COMMAND_FLAG_PORT               equ     9ae8h
PATTERN_DEFINE_PORT             equ     0e2e8h
COLOUR_0_PORT                   equ     0a2e8h
COLOUR_1_PORT                   equ     0a6e8h
PATTERN_0_PORT                  equ     0bee8h
PATTERN_1_PORT                  equ     0bee8h
K1_PORT                         equ     8ae8h
K2_PORT                         equ     8ee8h
ERROR_TERM_PORT                 equ     92e8h
USER_DEFINED_PATTERN_MODE       equ     0a080h
BLOCK_MOVE_MODE                 equ     0a0c0h

;Following are some equates used by the BoardBlt family:

ReadFlag                equ     01h
WriteFlag               equ     0feh
InvisibleFlag           equ     02h
MemManFlagY             equ     04h
ResetMemManFlagY        equ     0fbh
SrcPresent              equ     10h
PatternPresent          equ     20h
DstPresent              equ     40h
TrailingNotPresent      equ     80h  

;Here are equates related to FIFO space:

ONE_WORD                equ     80h
TWO_WORDS               equ     40h
THREE_WORDS             equ     20h
FOUR_WORDS              equ     10h
FIVE_WORDS              equ     08h
SIX_WORDS               equ     04h
SEVEN_WORDS		equ	02h
EIGHT_WORDS		equ	01h

;Following are the FIFO checking macros:

GetDataCheckStatus  macro
local	DataStatusReCheck
DataStatusReCheck:
	mov	dx, STATUS_PORT
	in	ax, dx
	and	ah, 01h
	jz	DataStatusReCheck
	endm

MakeEmptyFIFO           macro
local   StatusReCheck
StatusReCheck:
	mov	dx,STATUS_PORT+1
	in	al,dx
	and	al,02h
	jnz	StatusReCheck
	endm

CheckFIFOSpace          macro   SpaceNeeded
local   SpaceReCheck
SpaceReCheck:
	mov	dx,STATUS_PORT
	in	ax,dx
	and	al,SpaceNeeded
	jnz	SpaceReCheck
	endm

;Following are for the multi-res feature (charlkin 4/10/91)
MULTIRES_120DPI         equ     0h
MULTIRES_96DPI          equ     1h


