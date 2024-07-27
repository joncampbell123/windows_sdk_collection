;-------------------------------------------------------------------
;
; TYY.INC -- Global equates for WinOldAp
;
;-------------------------------------------------------------------
MaxCol		equ	80
MaxLine 	equ	25
Col80		equ	MaxCol-1
Line25		equ	MaxLine-1
MaxBufLines	equ	2
ICONTICK	equ	183
NORMALICON	equ	1

msgMax		equ	60	; maximum length of out of memory message
msgNoSwap	equ	10	; the bozo deleted the swap file
msgNoClip	equ	9	; Not enough memory for clipboard
msgNoMem	equ	8	; index of out of memory message (Cannot change)
msgNoDisk	equ	5	; out of swap disk space
msgActive	equ	4
msgBadDir	equ	3
msgNoExec	equ	2
msgNeed 	equ	1

;
; OldApState constants
;
SF_NOT_STARTED	equ	0	; The app has not been exec'ed
SF_RUNNING	equ	1	; The app is running
SF_TERMINATED	equ	2	; The app has been exited
SF_CLOSED	equ	3	; The app has exited and closed

;
; fSwitch constants. Setting fSwitch to SW_SWITCH will
; cause a switch in bad applications.
;
SW_CLEAR	equ	00000000b
SW_SWITCH	equ	00000001b
SW_TABTYPE	equ	00000010b
SW_ESCTYPE	equ	00000000b
SW_SHIFT	equ	00000100b

SW_ESC		equ	SW_SWITCH or SW_ESCTYPE
SW_SHIFTESC	equ	SW_SHIFT or SW_SWITCH or SW_ESCTYPE
SW_TAB		equ	SW_SWITCH or SW_TABTYPE

;
; fHotKey contants
;
HK_CLEAR	equ	0
HK_MODE 	equ	2
HK_NEXT 	equ	4
HK_PREV 	equ	6

;
; fSelect constants
;
FS_NOTACTIVE	equ	0
FS_ACTIVE	equ	1
FS_MARK 	equ	2

;
; fUserLock constants
;
UL_ON		equ	001b
UL_OFF		equ	0

cEsc		equ	1bh
StartEdit	equ	0
CancelEdit	equ	1bh
;!! HP RCP 031787
;cmCopy 	 equ	 0FFF0H
;cmPaste	 equ	 0FFF1H
;cmSelect	 equ	 0FFF2H
;cmScroll	 equ	 0FFF3H


cmSelect	equ	2050
cmCopy		equ	2051
cmPaste 	equ	2052
cmScroll	equ	2053

; Bad app menu ID's
;
cmbRestore	equ	2054
cmbMove 	equ	2055
cmbSize 	equ	2056
cmbMinimize	equ	2057
cmbMaximize	equ	2058
cmbClose	equ	2060
;***

IDSOEMText	equ	00F00H

; Title strings
;
maxTS		equ	15		; Title strings cannot be more than
tsMark		equ	101h
tsScroll	equ	102h
tsInactive	equ	103h


;
; flags and location used for RT keyboard identification
;
OEM_KBRT	equ	00010000b
KBFlagRT	equ	0496H

;
; Vectra EX-BIOS call flags
;
F16_SET_XLATORS equ	6F07H

;
; flags for pifBehavior combined with pifMsFlags
;
fScreen 	equ	10000000b
fForeground	equ	01000000b
f8087		equ	00100000b
fKeyboard	equ	00010000b
fNoGrab 	equ	00001000b
fNoSwitch	equ	00000100b
fGraphics	equ	00000010b
fResident	equ	00000001b

fCOM1		equ	10000000b
fCOM2		equ	01000000b
fDestroy	equ	00010000b

BadBits equ (fScreen+fKeyboard+fResident+fNoSwitch)
UglyBits equ	(fKeyboard+fNoSwitch+fResident)

; flags for pifFlags
;
fParam		equ	01000000b
fSwap		equ	00100000b

; flags for fHook
;
MemHook 	equ	00000001b	; memory hooks in
KeyHook 	equ	00000010b	; keyboard hooks in
VidHook 	equ	00000100b	; video hooks in
SwiHook 	equ	00001000b	; switcher hooks in
ClipHook	equ	00010000b	; clipboard hooks in
MsHook		equ	00100000b	; INT 33 mouse hook in
int23Hook 	equ	01000000b	; Winoldap int 23 

NotMemHook	equ	11111110b
NotKeyHook	equ	11111101b
NotVidHook	equ	11111011b
NotSwiHook	equ	11110111b
NotClipHook	equ	11101111b
NotMsHook	equ	11011111b
NotInt23Hook	equ	10111111b

RS232_base	equ	400H

;
; We only look at the first few bytes of the TDB
;
TDB		STRUC
TDB_next	DW  ?		; next task in queue
TDB_taskSP	DW  ?		; Saved SS:SP for this task
TDB_taskSS	DW  ?		;
TDB		ENDS

TASKREG STRUC
TASKDX	DW	?
TASKBX	DW	?
TASKES	DW	?
TASKCX	DW	?
TASKAX	DW	?
TASKDI	DW	?
TASKSI	DW	?
TASKDS	DW	?
TASKBP	DW	?
TASKREG ENDS
