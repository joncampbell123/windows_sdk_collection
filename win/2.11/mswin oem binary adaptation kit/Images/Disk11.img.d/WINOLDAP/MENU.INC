;***************************************************************
;
;MenuInfo structure  -	Internal
;
;***************************************************************

MenuInfo	STRUC
	ID	dw	?		;menu/title ID
	flags	dw	?		;style flags
	item	dw	?		;menu/macro ID (subordinate)
	text	db	31 dup(?)	;text string to be displayed
	mnem	db	0		;mnemonic or search char
	mnempos db	0		;position in string of search char
	twidth	db	0		;the width of the text
	handle	dw	?		;menu handle for menu titles
					;menu title ID for popup items
	next	dw	0		;pointer to next of this type
MenuInfo	ENDS




;******************************************************************
;
;MenuPoke structure  -	External
;
;******************************************************************

MenuPoke	STRUC
	wIDChangeItem	dw	?
	wIDNewItem	dw	?
	wMenuID 	dw	?
	wMacro		dw	?
	wChange 	dw	?
	wItem		db	32 dup(?)
MenuPoke	ENDS


;--------------------------------------------------------------------
;GENERAL EQUATES
Winoldap	equ	1	;winoldap must be defined for include files
MenuItemSize	equ	size MenuInfo
MenuPokeSize	equ	size MenuPoke

LowMenuID	equ	2048
HighMenuID	equ	3071


MaxMenuWidth	equ	30

NO_ITEM 	equ	-1	;No menuitem is selected
;--------------------------------------------------------------------
;MENU FLAGS


MF_CHANGE	= 0080h
MF_INSERT	= 0000h
MF_APPEND	= 0100h
MF_DELETE	= 0200h
MF_REMOVE	= 1000h
MF_BYPOSITION	= 0400h
MF_BYCOMMAND	= 0000h
MF_GRAYED	= 0001h
MF_DISABLED	= 0002h
MF_ENABLED	= 0000h
MF_CHECKED	= 0008h
MF_UNCHECKED	= 0000h
MF_STRING	= 0000h
MF_POPUP	= 0010h
MF_SEPARATOR	= 0800h

;currently Unsupported

MF_BITMAP	= 0004h
MF_MENUBREAK	= 0040h
MF_MENUBARBREAK = 0020h

;--------------------------------------------------------------------
;FLAG MASKS

COMMAND_MASK	= MF_CHANGE+MF_INSERT+MF_APPEND+MF_DELETE
STYLE_MASK	= MF_GRAYED+MF_DISABLED+MF_ENABLED+MF_REMOVE
DATA_MASK	= MF_STRING+MF_POPUP+MF_SEPARATOR
CHECK_MASK	= MF_CHECKED+MF_UNCHECKED
HOW_MASK	= MF_BYPOSITION+MF_BYCOMMAND
SUPPORT_MASK	= COMMAND_MASK+STYLE_MASK+HOW_MASK+CHECK_MASK+DATA_MASK


;-------------------------------------------------------------------
;DEFAULT DATA

SystemMenuID	= 2048	;default menu ID for the system menu
SYSTEM_CHAR	= 32	;space bar
FIRST_1X_CM_POS = 5	;first avail control menu pos in ver 1.x
FIRST_2X_CM_POS = 7	;first avail control menu pos in ver 2.x


;-----------------------------------------------------------------------
;MENU EXECUTION FLAGS

PM_ABORT	= 0
PM_EXEC 	= 2
PM_RIGHT	= 4
PM_LEFT 	= 6
PM_SELECT	= 8


;--------------------------------------------------------------------
; ACTIONS flags
AF_NOP		= 00h
AF_EXEC 	= 01h
AF_ABORT	= 02h
AF_RELEASE	= 04h
AF_UNSELECT	= 08h
AF_SELECTITEM	= 10h
AF_SELECTTITLE	= 20h
AF_DISPLAY	= 40h
AF_HILITEFIRST	= 80h

AF_EXECUTE	= AF_EXEC or AF_RELEASE
AF_PULLMENU	= AF_DISPLAY or AF_RELEASE
AF_CYCLEMENU	= AF_DISPLAY or AF_HILITEFIRST or AF_RELEASE
AF_CYCLETITLE	= AF_UNSELECT or AF_SELECTTITLE
AF_CYCLEITEM	= AF_UNSELECT or AF_SELECTITEM
AF_ESCAPE	= AF_ABORT or AF_RELEASE

;--------------------------------------------------------------------
;MENU TITLE

MT_CLEAR	= 0
MT_BAR		= 2
MT_SELECT	= 4

;------------------------------------------------------------------------
;BAD APP MENU DATA
CHECK_CHAR	= 175			;double right arrow
VERT_BAR	= 179
HORZ_BAR	= 196
UL_CORNER	= 218
UR_CORNER	= 191
LL_CORNER	= 192
LR_CORNER	= 217
