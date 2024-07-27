;
; Include file for kernel assembler source files.
;

.xlist
; ?DF = 1
include cmacros.inc
.list
include kdebug.inc

if1
outif   NOVELL,1,<Novell hacks enabled>
endif

; define misc. register pairs
;
regptr  esax,es,ax
regptr  esbx,es,bx
regptr  essi,es,si
regptr  esdi,es,di
regptr  dssi,ds,si
regptr  dsdi,ds,di
regptr  cssi,cs,si
regptr  csdi,cs,di
regptr  dxax,dx,ax
regptr  ssbx,ss,bx

; Define constants for interfacing to Open and Delete Pathname procedures
;
fReOpen equ     10000000b
fExist  equ     01000000b
fPrompt equ     00100000b
fCreate equ     00010000b
fCancel equ     00001000b
fVerify equ     00000100b
fDelete equ     00000010b
fParse  equ     00000001b

MaxFileLen      equ     80

OPENSTRUC       STRUC
opLen   db      ?
opDisk  db      ?
opXtra  dw      ?
opDate  dw      ?
opTime  dw      ?
opFile  db      ?
OPENSTRUC       ENDS

KBINFO          STRUC
kbRanges        DB      4 dup (0)  ; Far East ranges for KANJI
kbStateSize     DW      0       ; #bytes of state info maintained by TOASCII
kbNumFuncKeys   DW      0       ; How many function keys are on the keyboard
kbHasBreak      DW      0       ; true => keyboard supplies make and break
kbRate          DW      0       ; maximum rate of keyboard input events
KBINFO          ENDS


EXECBLOCK       struc
envseg          dw      ?               ; seg addr of environment
lpcmdline       dd      ?               ; pointer to asciz command line
lpfcb1          dd      ?               ; default fcb at 5C
lpfcb2          dd      ?               ; default fcb at 6C
EXECBLOCK       ends

SYSINITVAR      STRUC
dpbhead         DD      ?       ; Head of DPB-FAT list
sfthead         DD      ?       ; Head of SFT table list
pclockdev       DD      ?       ; pointer to clock device
pcondev         DD      ?       ; pointer to console device
SYSINITVAR      ENDS
;
; system file table
;
SFT             STRUC
sftLink         DD      ?
sftCount        DW      ?               ; number of entries
sftFile         DB      ?
SFT             ENDS

SFT_ENTRY2      STRUC
sf_ref_count2   DB      ?       ; number of processes sharing fcb
sf_mode2        DB      ?       ; mode of access
sf_attr2        DB      ?       ; attribute of file
sf_fcb          DB      24 dup (?)
sf_devid        DB      ?
SFT_ENTRY2      ENDS

SFT_ENTRY3      STRUC
sf_ref_count3   DW      ?       ; number of processes sharing fcb
sf_mode3        DW      ?       ; mode of access
sf_attr3        DB      ?       ; attribute of file
sf_flags        DW      ?
sf_devptr       DD      ?
                DB      22 dup (?)      ; this is 21 for DOS 3.1
sf_name         DB      ?
SFT_ENTRY3      ENDS

DPB     STRUC
dpb_drive       DB      ?       ; Logical drive # assoc with DPB (A=0,B=1,...)
DPB     ENDS

devid_device    EQU     0080H     ; true if a device
sf_isnet        EQU     8000H     ; true if network drive
