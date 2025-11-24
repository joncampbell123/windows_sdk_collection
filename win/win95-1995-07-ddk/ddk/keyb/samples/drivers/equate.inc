;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

	page	59,132

;**************************************************************************
; This file contains equates, data structures and definitions needed to
; access both the Standard BIOS (STD-BIOS) and the Extended BIOS (EX-BIOS)
; of Vectra using MASM 3.0. Depending on what part of the BIOS you are
; accessing you might not need all the equates.  The equates are organized
; as follows:
;
;  o Usefull macros.
;  o 80286 Support Macros and Equates.
;  o EX-BIOS Equates:
;      1) Generic Structures and equates used by all drivers.
;      2) Equates for Vector Addressed.
;      3) Function and SubFunction Equates common to all drivers.
;      4) Function and Subfunction Equates individual to drivers. These
;         are order by vector number.
;  o Industry Standard (STD-BIOS) Interrupt numbers and function equates.
;  o Industry Standard (STD-BIOS) Data Area
;
; The major reason for organizing the equate file in this form is that
; the programmer can extract only those equates that he needs to create
; a tailored equate file.
;**************************************************************************

;**************************************************************************
; Useful macros. 
;**************************************************************************
;  none at this time


;**************************************************************************
; 80286 Support macros and equates.
;**************************************************************************
;**************************************************************************
; The following macro is used to compensate for a bug in the 80286
; hardware interrupt system.  It seems that during a normal
; POPF instruction cycle interrupts are always enabled regardless
; of the state of the interrupt enable flag prior to the pop or after
; the pop. 
;**************************************************************************
POPPF       macro
            jmp   $+3
            iret
            push  cs
            call $-2
            endm

;**************************************************************************
; EX-BIOS support macros and equates.
;**************************************************************************

; Equates for EX-BIOS interrupt number and vector address.  
HPENTRY                 equ     6FH
F_HPENTRY               equ     HPENTRY

;**************************************************************************
;                       SYSTEMCALL [vector_address]
;**************************************************************************
systemcall macro vec
 ifnb <vec>
  mov     bp,vec
 endif
  int     HPENTRY
endm

;**************************************************************************
; HP_VECTOR_TABLE Entry
;**************************************************************************
HP_TABLE_ENTRY          struc   ;<1,2,3>
HP_ENTRY_IP             dw      0
HP_ENTRY_CS             dw      0
HP_ENTRY_DS             dw      0
HP_TABLE_ENTRY          ends
;**********************************************************************
; Structure of Data Header for HP's vectors.
;**********************************************************************
HP_SHEADER              STRUC  ;<1,2,3,4,5,6,7,8,9,0>
DH_ATR                  dw  0   ; Attribute
DH_NAME_INDEX           dw  0   ; Name index of driver.
DH_V_DEFAULT            dw  0   ; Driver vector position in HPtable
DH_P_CLASS              dw  0   ; Parent class
DH_C_CLASS              dw  0   ; Child class
DH_V_PARENT             dw  0   ; Vector used when the driver cannot handle
                                ; an F_ISR function call
DH_V_CHILD              dw  0   ; Vector used when the driver cannot handle
                                ; a regular function call
DH_MAJOR                db  0   ; Driver's major address if any.
DH_MINOR                db  0   ; Driver's minor address if any.
HP_SHEADER              ENDS
                        
;**********************************************************************
; DH_ATR bit record
;**********************************************************************
ATR_HP            equ  1000000000000000B ; 1- The Rest of header is valid
ATR_DEVCFG        equ  0100000000000000B ; 1- Present in DEVCONFG
ATR_ISR           equ  0010000000000000B ; 1- Replace My ISR   (Child)
ATR_ENTRY         equ  0001000000000000B ; 1- Replace My ENTRY (Parent)
ATR_RSVD          equ  0000000000000000B ; 0 - Available on allocation from HP
ATR_FREE          equ  0000001000000000B ; 1 - Available Vector
ATR_SRVC          equ  0000010000000000B ; 2 - Service Vector
ATR_LOG           equ  0000011000000000B ; 3 - Logical Device Start DEVCONFG CHA
ATR_IND           equ  0000100000000000B ; 4 - Filter, show driver (options)
ATR_BOT           equ  0000101000000000B ; 5 - End of Chain
ATR_ESC           equ  0000110000000000B ; 6 - Filter, Pass thru , No options
ATR_TYPE7         equ  0000111000000000B ; 7 - Available
ATR_TYPE_MASK     equ  0000111000000000B
ATR_INP           equ  0000000100000000B ;  1 - Chain ISR ( to Parents )
                                         ;  0 - Chain Entry ( Child)
ATR_SUBADD        equ  0000000000000000B
ATR_MAJOR         equ  0000000000100000B
ATR_MINOR         equ  0000000001000000B
ATR_MID           equ  0000000001100000B
ATR_PSHARE        equ  0000000000010000B
ATR_CSHARE        equ  0000000000001000B
ATR_ROM           equ  0000000000000100B
ATR_YIELD         equ  0000000000000010B ;
ATR_0             equ  0000000000000001B

;**********************************************************************
;  DH_Class
;**********************************************************************
CL_KBDFC           equ  1000000000000000B       ; 1 - HP Softkey Transaltor
CL_KBD             equ  0100000000000000B       ; 1 - Keyboard
CL_CCP             equ  0010000000000000B       ; 1 - Cursor Control pad
CL_CON             equ  0001000000000000B       ; 1 - Console Device
CL_BYTE            equ  0000100000000000B       ; 1 - PRN device
CL_COMM            equ  0000010000000000B       ; 1 - COMM device
CL_INTERFACE       equ  0000001000000000B       ; 1 - interface, HPHIL, HPIB
CL_FILT            equ  0000000100000000B       ; 1 - charachter filter
CL_BLK             equ  0000000010000000B       ; 1 - block device
CL_BOOT            equ  0000000001000000B       ; 1 - boot block device
CL_LGID            equ  0000000000100000B       ; 1 - logical physical gid
                                                ; e.g. ccp to gid translator
CL_PGID            equ  0000000000010000B       ; 1 - physical gid
CL_GID             equ  0000000000001000B       ; 1 - any graphics input device
CL_PTS             equ  0000000000000100B       ; 1 - physical touch screen
CL_ASCII           equ  0000000000000010B       ; 1 - ascii input device
CL_EXTEND          equ  0000000000000001B       ; 0 - set of classes above
                                                ; 1 - alternate class set
CL_ALL             equ  1111111111111111B       ; Member of all classes
CL_NULL            equ  0000000000000000B       ; Member No Classes

;***************************************************************************
; Vector Addresses
;***************************************************************************
V_SCOPY                 equ     0000H           ; Copyright Notice
V_DOLITTLE              equ     0006H           ; Nop Routine
V_PNULL                 equ     000CH           ; No Device 
V_SYSTEM                equ     0012H           ; System Intrinsics
V_SINPUT                equ     002AH           ; Input Inquire routines
V_SQWERTY               equ     0036H           ; Qwerty KBD Translator
V_PSOFTKEY              equ     003CH           ; HP f1-f8 Translator
V_PFUNCTION             equ     0042H           ; IBM F1-F10 Translator
V_PNUM_PAD              equ     0048H           ; Numeric Pad Translator
V_SPCCP                 equ     004EH           ; CCP Translator Driver
V_PVIDEO                equ     0054H           ; Video Intrinsics
V_STRACK                equ     005AH           ; Common cursor control funcs.
V_EVENT_TOUCH           equ     0060H           ; Touch Event Intercept
V_EVENT_TABLET          equ     0066H           ; Tablet Event Intercept
V_EVENT_POINTER         equ     006CH           ; Pointer Event Intercept
V_LCCP_CURSOR           equ     008AH           ; CCP to Cursor Always Filter (Default)
V_RAW                   equ     0090H           ; CCP+Softkey RAW Mode Filter
V_LCCP_NUMPAD           equ     0096H           ; CCP to Numeric Pad Filter
V_OFF                   equ     009CH           ; CCP+Softkey Off Filter
V_LCCP_GID              equ     00A2H           ; CCP to GID Filter ( Not Implemented)
V_LFUNCTION             equ     00A8H           ; Softkey (f1-f8) to Function
                                                ;  key (F1-F8) Filter (Default)
V_L8041                 equ     00AEH           ; 8041 Interface
V_PGID_CCP              equ     00B4H           ; Graphic to CCP Filter
V_LTABLET               equ     00BAH           ; Tablet driver
V_LPOINTER              equ     00C0H           ; Pointer driver
V_LTOUCH                equ     00C6H           ; Touch driver
V_LHPMOUSE              equ     00CCH           ; Microsoft/Mouse System's
                                                ;   Compatible Driver
V_LNULL                 equ     0108H           ; No Driver
V_HPHIL                 equ     0114H           ; HPHIL Driver


;~~ VVR 11/02/87
V_SCANDOOR		equ	016EH		; Scan Door driver	

;************************************************************************
; V_SCANDOOR (016EH) Function and subfunction codes.
;************************************************************************
; F_ISR, F_SYSTEM and F_IO_CONTROL are functions common to all drivers.
; Subfunction under F_STATE_IOCTL particular to this driver.
;
F_STATE_IOCTL		equ	0008H	; Get and set the state

SF_GET_STATE		equ	0000H	; 
SF_SET_STATE		equ	0002H	; 

;~~ VVR 11/02/87
;*********************************************************************
; Common Function Codes for HP Routines.
;*********************************************************************
F_ISR                   equ     00H*2           ; Interrupt service call
F_SYSTEM                equ     01H*2           ; System func. call,
                                                ;   Subfunction required
F_IO_CONTROL            equ     02H*2           ; Device/Driver Dependent
                                                ;   Functions
F_ITF_TO_ENVOY          equ     F_IO_CONTROL    ; Translation function for
                                                ;   SITF_ENVOY service.
F_PUT_BYTE              equ     03H*2           ; Write one byte of data:
                                                ;   Byte is in AL
F_GET_BYTE              equ     04H*2           ; Read a byte of data:
                                                ;   Byte returned in AL
F_PUT_BUFFER            equ     05H*2           ; Write a buffer of data,
                                                ;   ES,DI pointer, CX count
F_GET_BUFFER            equ     06H*2           ; Read a buffer if data,
                                                ;   ES,DI pointer, CX count
F_PUT_WORD              equ     07H*2           ; Write word of data, BX data
F_GET_WORD              equ     08H*2           ; Read word of data, BX data
F_GETTRM_BUFFER         equ    009H*2           ; Reads buffer of data,
                                                ;   ES,DI pointer, CX count
                                                ; BH upper bound, BL lower bound
F_PUT_BLOCK             equ     F_PUT_BUFFER    ;used for disk applications
F_GET_BLOCK             equ     F_GET_BUFFER

;*********************************************************************
; Common Subfunction codes of the F_SYSTEM Function.
;*********************************************************************
SF_INIT                 equ     00H*2   ;Initialize command
SF_START                equ     01H*2   ;Secondary Init--initialize dependent
                                        ; upon other drivers/data structures         
SF_REPORT_STATE         equ     02H*2   ;Reports state of driver
SF_VERSION_DESC         equ     03H*2   ;Report version and option describe
                                        ;  record
SF_DEF_ATTR             equ     04H*2   ;Reports default Configuration
                                        ;   (Baud Rate)
SF_GET_ATTR             equ     05H*2   ;Reports Current Configuration
                                        ;   ES,DI pointer
SF_SET_ATTR             equ     06H*2   ;Sets Next Configuration ES,DI, CX
SF_OPEN                 equ     07H*2   ;Reserve Driver for exclusive access
SF_CLOSE                equ     08H*2   ;Release  "   from    "        "
SF_TIMEOUT              equ     09H*2   ;Notify Driver Timeout Occurred
SF_INTERVAL             equ     0AH*2   ;Notify Driver Interval Occurred
SF_TEST                 equ     0BH*2   ;Test Function

;*********************************************************************
; Common Subfunction Codes for the F_IO_CONTROL function.
;*********************************************************************
SF_LOCK                 equ     00H*2   ; Lock Device for exclusive access
SF_UNLOCK               equ     01H*2   ; Unlock Device for exclusivce access

;*********************************************************************
; HP Routines Return Status: Sucessful codes are positive and failure
; are negative.
;*********************************************************************
RS_BREAK         equ    00CH            ; Break -- IFC
RS_DATA_NREADY   equ    00AH            ; RS232 Data Not Ready=>Retry Operation
RS_OVERRUN       equ    008H            ; RS232 Port Data Overrun =>Retry Operation
RS_DONE          equ    006H            ; indicates all done return child
RS_NOT_SERVICED  equ    004H            ; indicates a chained ISR--not handled
RS_UNSUPPORTED   equ    002H            ; indicates function is NOPed/not valid
                                        ; for this driver
RS_SUCCESSFUL    equ    F_ISR           ; indicates executed just fine
RS_SERVICED      equ    rs_SUCCESSFUL   ; indicates chained ISR done
RS_PASSTHRU      equ    rs_SERVICED     ; indicates launch data to parent
;************************************************************************
RS_FAIL          equ    0FEH            ; To be used with hardware failure
RS_TIMEOUT       equ    0FCH            ; to indicate remote device timeout
RS_BAD_PARAMETER equ    0FAH            ; to indicate a bad parameter
RS_BUSY          equ    0F8H            ; to indicate driver/device is busy
RS_NO_VECTOR     equ    0F6H            ; out of hp_VT vectors
RS_OFFLINE       equ    0F4H            ; device is offline
RS_OUT_OF_PAPER  equ    0F2H            ; out of paper on printer device
RS_PARITY        equ    0F0H            ; parity error in transmission
RS_FRAME         equ    0EEH            ; framing error

;******************************************************************************
; Function Number Equates for the EX-BIOS routines and its Data Structures.
;******************************************************************************

;*********************************************************************
; V_SYSTEM (0012H) function codes.
;*********************************************************************
F_INS_BASEHPVT          equ     0004H
F_INS_XCHGFIX           equ     0006H
F_INS_XCHGRSVD          equ     0008H
F_INS_XCHGFREE          equ     000AH
F_INS_FIXOWNDS          equ     000CH
F_INS_FIXGETDS          equ     000EH
F_INS_FIXGLBDS          equ     0010H
F_INS_FREEOWNDS         equ     0012H
F_INS_FREEGETDS         equ     0014H
F_INS_FREEGLBDS         equ     0016H
F_INS_FIND              equ     0018H
F_INS_FINDALL           equ     001AH
F_INS_NXTLOGBLK         equ     001CH
F_RAM_GET               equ     001EH
F_RAM_RET               equ     0020H
F_CMOS_GET              equ     0022H
F_CMOS_RET              equ     0024H
F_CMOS_HPDFLTS          equ     0026H
F_CMOS_IBMDFLTS         equ     0028H
F_YIELD                 equ     002AH
F_INS_ADR               equ     002CH
F_RESERVE_STRING3       equ     002EH
F_SND_CLICK_ENABLE      equ     0030H
F_SND_CLICK_DISABLE     equ     0032H
F_SND_CLICK             equ     0034H
F_SND_BEEP_ENABLE       equ     0036H
F_SND_BEEP_DISABLE      equ     0038H
F_SND_BEEP              equ     003AH
F_SND_SET_BEEP          equ     003CH
F_SND_TONE              equ     003EH
F_STR_GET_FREE_INDEX    equ     0040H
F_STR_DEL_BUCKET        equ     0042H
F_STR_PUT_BUCKET        equ     0044H
F_STR_GET_STRING        equ     0046H
F_STR_GET_INDEX         equ     0048H

;**********************************************************************
;  String Bucket Header. This structure is usefull if using the
; following V_SYSTEM functions: F_STR_DEL_BUCKET and F_STR_PUT_BUCKET.
;**********************************************************************
STR_HEADER              STRUC
STR_NXT_HDR             dd      (?)
STR_UPPER_BOUND         dw      (?)
STR_LOWER_BOUND         dw      (?)
STR_LIST_PTR            dd      (?)
STR_SEGMENT             dw      (?)
STR_HEADER              ENDS

;**********************************************************************
;  V_SYSTEM Global Data Segment
;**********************************************************************
HP_GLB_HEADER           STRUC
T_HP_HEADER             dw      (?)
T_USED_AND_RESERVED     dw      6 dup (?)
T_HP_LAST_DS            dw      (?)
T_HP_MAX_DS             dw      (?)
T_HP_NXT_VCTR           dw      (?)
T_SND_FLAG              db      (?) ;
T_SND_CLICK_COUNT       db      (?) ;
T_SND_CLICK_DURA        db      (?) ;
T_SND_CLICK_VOLUME      db      (?) ;
T_SND_BEEP_CYCLE        dw      (?) ;
T_SND_BEEP_DURA         dw      (?) ;
T_SND_BEEP_COUNT        db      (?) ;
                        db      (?) ;  1 reserved byte for volume
T_STR_NEXT_INDEX        dw      (?)
T_STR_ROOT              dd      (?)
T_STR_VCT_HDR           db      size STR_HEADER dup (?) ; Area vector's name and
T_STR_MSG_HDR           db      size STR_HEADER dup (?) ; ROM message strings
T_8259_FLAGS            db      (?)
                        db      31 dup (?) ; reserve 2 paragraph
HP_GLB_HEADER           ENDS

;*********************************************************************
; V_SINPUT (2AH) Function and subfunction codes.
;*********************************************************************
; F_ISR, F_SYSTEM and F_IO_CONTROL are functions common to all drivers.
F_INQUIRE               equ     0006H
F_INQUIRE_ALL           equ     0008H ; Reports ID's of devices
F_INQUIRE_FIRST         equ     000AH ; Reports (V_HPHIL1) offset
F_REPORT_ENTRY          equ     000CH ; Reports entrypoint of (V_PGID)

;
; Extra subfunctions under F_IO_CONTROL.
;
SF_DEF_LINKS            equ     0000H ; Sets default cofiguration
SF_GET_LINKS            equ     0002H ; Reports current configuration
SF_SET_LINKS            equ     0004H ; Sets Next Configuation

;**********************************************************************
; VDI CLASSES. 
;**********************************************************************
VDI_CL_NULL             equ     0
VDI_CL_LOCATOR          equ     1
VDI_CL_VALUATOR         equ     2
VDI_CL_STROKE           equ     3
VDI_CL_CHOICE           equ     4
VDI_CL_STRING           equ     5

;*********************************************************************
; V_SVIDEO (54H) sub function codes. Use this subfunctions when calling 
; the video driver directly.   of Sub of ???? ABE
;*********************************************************************
SF_VID_ID_HP            equ     0
SF_VID_GET_INFO         equ     2
SF_VID_SET_INFO         equ     4
SF_VID_MOD_INFO         equ     6
SF_VID_GET_RES          equ     8
SF_VID_SET_MODE         equ     10

;***********************************************************************
; V_SVIDEO (54H) EX-BIOS Data Structures.
;***********************************************************************
VID_BLOCK_SIZE          equ     0027H
VIDEO_DATA              struc
VID_ATR                 dw      ?
VID_NAME_INDEX          dw      ?
VID_V_DEFAULT           dw      ?
VID_PRIMARY             db      ?       ; Which display is primary:
VID_SECONDARY           db      ?       ; Which adapter is seconday (see above).
VID_FOUND_ROM           db      ?       ; Flag set to true if rom found and init'ed
VID_IDS                 db      4 DUP (?) ; Room for four possible ID's
VID_STATUS              db      4 DUP (?) ; Room for all status registers.
VID_EXT_STATUS          db      4 DUP (?) ; Room for extended status registers.
VID_PARM_BLOCK          db      VID_BLOCK_SIZE DUP (?)  ; Place to save parameters
VID_LAST_IBM_MODE       db      (?)
VID_EXT_MODE            db      (?)
VID_PADDING             db      2H DUP (?)
VIDEO_DATA              ends

;************************************************************************
; V_L8041 (00AEH) Function and subfunction codes.
;************************************************************************
; F_ISR, F_SYSTEM and F_IO_CONTROL are functions common to all drivers.
; Subfunction under F_IO_CONTROL particular to this driver.
SF_SVC_REQ              equ     0000H   ; HP8041 service request.
SF_STRT_LTMO            equ     0002H   ; Start Long Time Out.
SF_STRT_MTMO            equ     0004H   ; Start Medium Time Out.
SF_STRT_STMO            equ     0006H   ; Start Short Time Out.
SF_STOP_TMO             equ     0008H   ; Stop Time Out.
SF_CREAT_INTR           equ     000AH   ; Create Interval Entry.
SF_DELET_INTR           equ     000CH   ; Delete Interval Entry.
SF_ENABL_INTR           equ     000EH   ; Enable Interval.
SF_DISBL_INTR           equ     0010H   ; Disable Interval.
SF_SET_RAMSW            equ     0012H   ; Set RAM Switch to 1.
SF_CLR_RAMSW            equ     0014H   ; Clear RAM Switch to 0.
SF_SET_CRTSW            equ     0016H   ; Set CRT Switch to 1.
SF_CLR_CRTSW            equ     0018H   ; Clear CRT Switch to 0.
SF_PASS_THRU            equ     001AH   ; Pass Data Byte to HP8041.
SF_HP                   equ     001CH   ; Report Existance of HP Extensions.
SF_DEF_KBATTR           equ     001EH   ; Report Default Typematic Rate/Delay.
SF_GET_KBATTR           equ     0020H   ; Report Current Typematic Rate/Delay.
SF_SET_KBATTR           equ     0022H   ; Set Typematic Rate/Delay.
SF_DEF_MAP              equ     0024H   ; Report Default Keyboard Mappings.
SF_GET_MAP              equ     0026H   ; Report Current Keyboard Mappings.
SF_SET_MAP              equ     0028H   ; Set Keyboard Mappings.
SF_SET_XLAT             equ     002AH   ; Set Keyboard Translator.
SF_DESCRIBE             equ     002CH   ; Report Keyboard HPHIL address and ID.
SF_RESET_DEF            equ     002EH   ; Reset keyboard typematic, mappings
                                        ; and translators.

;**********************************************************************
; V_L8041 Data structures.
;**********************************************************************
SVC_DATA        STRUC
SVC_HEADR       dw      8 dup (?)       ; hp headr
SVC_STATUS      db      0               ; general status byte
SVC_INTR_STAT   db      0               ; active interval status
SVC_INTR_CNT    db      0               ; number of interval entries created
SVC_INTR_TBL    db      8 dup (?)       ; vector numbers for interval usrs
SVC_TMO_VEC     db      0               ; vector number to call for timeout
SVC_HEAD        db      0               ; queue head index
SVC_TAIL        db      0               ; queue tail index
SVC_QUEUE       db      128 dup (?)     ; scan code queue
SVC_ATTR_TBL    db      4 dup (?)       ; table of keyboard rate/delays
SVC_MAP_TBL     dw      5 dup (?)       ; table of ds's for default
                                        ; keyboard translators
SVC_DATA        ENDS

;**********************************************************************
; V_LGID Fucntion Codes. This is a common driver for: V_LTABLET, 
; V_LPOINTER and V_LTOUCH.
;**********************************************************************
F_SAMPLE                equ     06
; F_IO_CONTROL:
SF_TRACK_ON             equ     4
SF_TRACK_OFF            equ     6
SF_CREATE_EVENT         equ     8
SF_EVENT_ON             equ     0Ah
SF_EVENT_OFF            equ     0Ch
SF_CLIPPING_ON          equ     0Eh
SF_CLIPPING_OFF         equ     10h
;**********************************************************************
; V_LGID LD_DEVICE_STATE
;**********************************************************************
EVENT_ENABLED           equ     10h
TRACK_ENABLED           equ     08h
CLIP_ENABLED            equ     04h
BUTTON_ERROR            equ     02H
ISR_IN_PROGRESS         equ     01H

;*********************************************************************
; V_PGID Data Structures
;*********************************************************************
DESCRIBE        STRUC
                db      size HP_SHEADER dup (?) ; this data is always offset by
D_SOURCE        db      ?       ; 7-4 (high nibble) contains the GID type
                                ; 3-0 (low nibble) is the address of the device
D_HPHIL_ID      db      ?       ; device id byte returned by an HPHIL device
D_DESC_MASK     db      ?       ; describe header from HPHIL device
D_IO_MASK       db      ?       ; I/O descriptor byte from device
D_XDESC_MASK    db      ?       ; extended describe byte from device
D_MAX_AXIS      db      ?       ; maximum number of axis reported
D_CLASS         db      ?       ; device class
                                ; 7-4 (high nibble) contains current class
                                ; 3-0 (low nibble) contain the default class
D_PROMPTS       db      ?       ; number of buttons/prompts
                                ; 7-4 (high nibble) is the number of prompts
                                ; 3-0 (low nibble) is the number of buttons
D_RESERVED      db      ?       ; reserved for future
D_BURST_LEN     db      ?       ; maximum burst length output to a device
                                ; if devices supports more than 255 bytes then
                                ; 255 bytes is the default maximum
D_WR_REG        db      ?       ; number of write registers supported by a device
D_RD_REG        db      ?       ; number of read registers supported by a device
D_TRANSITION    db      ?       ; transitions reported per button
D_STATE         db      ?       ; current state of buttons
D_RESOLUTION    dw      ?       ; counts / cm (m) returned by HPHIL device
D_SIZE_X        dw      ?       ; Maximum count of in units of resolution
D_SIZE_Y        dw      ?       ;
D_ABS_X         dw      ?       ; data reported from device
D_ABS_Y         dw      ?       ; that reports absolute data
D_REL_X         dw      ?       ; data reported from device
D_REL_Y         dw      ?       ; that is relitive
D_ACCUM_X       dw      ?       ; these are used to accumulate scaling
D_ACCUM_Y       dw      ?       ; remainder
DESCRIBE        ENDS

DESCRIBE_SIZE   	equ     size DESCRIBE
D_CCP_STATE             equ     D_STATE + 1
D_SIZE                  equ     D_SIZE_X
D_SAMPLE_ABSOLUTE       equ     D_ABS_X
D_SAMPLE_RELATIVE       equ     D_REL_X
D_REMAINDER_ACCUM       equ     D_ACCUM_X
D_BUFFER                equ     D_SIZE_X        ; offset where buffer begins
D_CLASS_CURRENT 	equ     0F0H
D_CLASS_DEFAULT 	equ     00FH
; The field LD_SOURCE uses the following to access the defined nibbles
D_ADDR_MASK             equ     00FH
D_TYPE_MASK             equ     0F0H
;*********************************************************************
; V_LGID Data Structures
;*********************************************************************
LDESCRIBE       STRUC
                db      size HP_SHEADER dup (?) ; this data is always offset by
LD_SOURCE       db      ?       ; 7-4 (high nibble) contains the GID type
                                ; 3-0 reserved
LD_HPHIL_ID     db      ?       ; device id byte returned by an HPHIL device
LD_DEVICE_STATE dw      ?       ; status bits for logical device
LD_INDEX        db      ?       ; vector index of invoking driver
LD_MAX_AXIS     db      ?       ; maximum number of axis reported
LD_CLASS        db      ?       ; device class
                                ; 7-4 (high nibble) contains current class
                                ; 3-0 (low nibble) contain the default class
LD_PROMPTS      db      ?       ; number of buttons/prompts
                                ; 7-4 (high nibble) is the number of prompts
                                ; 3-0 (low nibble) is the number of buttons
LD_RESERVED     db      ?       ; reserved for future
LD_RES2         db      ?
LD_RES3         db      ?
LD_RES4         db      ?
LD_TRANSITION   db      ?       ; transitions reported per button
LD_STATE        db      ?       ; current state of buttons
LD_RESOLUTION   dw      ?       ; counts / cm (m) returned by HPHIL device
LD_SIZE_X       dw      ?       ; Maximum count of in units of resolution
LD_SIZE_Y       dw      ?       ;
LD_ABS_X        dw      ?       ; data reported from device
LD_ABS_Y        dw      ?       ; that reports absolute data
LD_REL_X        dw      ?       ; data reported from device
LD_REL_Y        dw      ?       ; that is relitive
LD_ACCUM_X      dw      ?       ; these are used to accumulate scaling
LD_ACCUM_Y      dw      ?       ; remainders
LDESCRIBE       ENDS
LDESCRIBE_SIZE 		equ     size LDESCRIBE

LD_SIZE                 equ     LD_SIZE_X
LD_SAMPLE_ABSOLUTE      equ     LD_ABS_X
LD_SAMPLE_RELATIVE      equ     LD_REL_X
LD_REMAINDER_ACCUM      equ     LD_ACCUM_X
LD_BUFFER               equ     LD_RESOLUTION   ; offset where buffer begins
; the following mask are used in the field LD_CLASS
LD_CLASS_CURRENT        equ     0F0H
LD_CLASS_DEFAULT        equ     00FH
; The field LD_SOURCE uses the following to access the defined nibbles
LD_RES_MASK             equ     00FH
LD_TYPE_MASK            equ     0F0H
;
; Physical Scan Code Sets
;
T_NO_KEY		equ	00H	; should never occur
T_KC_R1			equ	01H	; reserved see HPHIL ERS
T_KC_ASCII		equ	02H	; ascii data
T_KC_R3			equ	03H	; reserved see HPHIL ERS
T_KC_ITF		equ	04H	; ITF scancode reported by HP150 ITF keyboard,
					; HP200	ITF keyboard, barcode reader in scancode mode
					; ( Button reports are in this set )
T_KC_R5			equ	05H	; reserved see HPHIL ERS
T_KC_WILD		equ	06H	; wild card set, device dependent, used by BUTTON PAD
T_KC_HPHIL_ENVOY	equ	07H	; reported by ENVOY Keyboard
;
; Logical Scan Code Sets
;
T_KC_IBM_AT		equ	08H
T_KC_BUTTON           	equ	09H	; reported by the physical driver to the logical driver
					; PGID translates T_KC_ITF to T_KC_BUTTON and filters
					; any other scancode out of the data stream
T_KC_IBM_PC		equ	0AH
T_KC_HP_SOFTKEY		equ	0BH
T_KC_IS_FUNCTION	equ	0CH
T_KC_HP_CCP		equ	0DH
T_KC_QWERTY		equ	0EH
T_KC_NUMPAD		equ	0FH
;
; how a string data type is formed
;

T_STRING		equ	010H
;T_STRING  00x1 ttttB  indicates a string of data bytes of type
;                      defined by the lower nybble 'tttt'.  The state
;                      information, naturally, only applies to the
;                      first byte of data as it is can be modified by each
;                      subsequent byte of data.
;

T_STATE			equ	020H
;T_STATE   001x ttttB  indicates the character type indicated
;                      in 'tttt' field has the current logical state
;                      of the keyboard appended onto it.
;
T_GID			equ	40H	; used to set and test for gid types
T_REL08			equ	40H	;  normal mouse type data
T_REL16			equ	41H
T_ABS08			equ	42H	;  normal TOUCH SCREEN Data
T_ABS16			equ	43H	;  normal TABLET data type
;
; logical types of GID data
;
T_MOUSE			equ	44H	;  specially formed data
T_TS			equ	45H	;  Specially formed data  ( 0..80 x 0..25 range - default)
T_GIDA			equ	46H	;  Specially windowed data ( 640 x 200 range - default )
T_TABLET		equ	46H	;  ???? Do not use this type on new code
T_GIDR			equ	47H	;  specially windowed data ( 640 x 200 range - default )
T_UNKNOWN		equ	4FH	;  Unknown data type.


;
; The folowing types are used in the HPHIL driver to map to the GID types
;
GID_R08			equ	00H
GID_R16			equ	01H
GID_A08			equ	02H
GID_A16			equ	03H

GID_UNDEF		equ	0FH	; this undefined type is used because
					; the character input devices at the 
					; current time do not have graphics input
					; and this tells the HPHIL driver that
					; the data type to return is determined
					; by the scancodes the device returns

CLASS_KBD        equ      00H
CLASS_TS         equ      01H
CLASS_ASCII      equ      02H
CLASS_BINARY     equ      03H
CLASS_MOUSE      equ      04H
CLASS_GIDCCP     equ      05H
CLASS_TABLET     equ      06H
CLASS_JOY        equ      07H
CLASS_UNDEF8     equ      08H
CLASS_PADDLE     equ      09H
CLASS_THUMB      equ      0AH
CLASS_TRACKBALL  equ      0BH
CLASS_KEYPAD     equ      0CH
CLASS_UNDEFD     equ      0DH
CLASS_UNDEFE     equ      0EH
CLASS_UNDEFF     equ      0FH

;************************************************************************
; V_LHPMOUSE (00CCH) Function and subfunction codes.
;************************************************************************
; F_ISR, F_SYSTEM and F_IO_CONTROL are functions common to all drivers.
; Subfunction under F_IO_CONTROL particular to this driver.
SF_MOUSE_COM            equ     0000H   ; This function is used during the 
                                        ; reinit call from DOS.  It is used
                                        ; to set up INT 33H.  This is done  
                                        ; because DOS takes INT 33H when it
                                        ; is initialized.
SF_MOUSE_OVERRIDE       equ     0002H   ; This function is used to force the
                                        ; V_LHPMOUSE driver to install INT 33
                                        ; even when the mouse is not present.
                                        ; This allows a programmer to map
                                        ; devices to the V_LHPMOUSE driver if
                                        ; a mouse is not present.

;************************************************************************
; V_STRACK (05AH) Function and subfunction codes.
;************************************************************************
F_TRACK_INIT            equ     0004H   ; used to perform a soft initialization
                                        ; of the tracking driver.
F_TRACK_ON              equ     0006H   ; enables tracking
F_TRACK_OFF             equ     0008H   ; disables tracking
F_DEF_MASKS             equ     000AH   ; define masks for sprite
F_SET_LIMITS_X          equ     000CH   ; define X limit of screen coordinates
F_SET_LIMITS_Y          equ     000EH   ; define Y limits
F_PUT_SPRITE            equ     0010H   ; Used to put the sprite on the screen
F_REMOVE_SPRITE         equ     0012H   ; removes the sprite from the screen

;************************************************************************
; V_HPHIL (0114H) Function and subfunction codes.
;************************************************************************
; F_ISR, F_SYSTEM and F_IO_CONTROL are functions common to all drivers.
; Subfunction under F_IO_CONTROL particular to this driver.
;
SF_CRV_CRV_MAJ_MIN      equ     0004    ; This is used to set a default
                                        ; major and minor addresses
SF_CRV_RECONFIGURE      equ     0006    ; Funtion id used to force the HPHIL
                                        ; link to reconfigure the devices
SF_CRV_WR_PROMPTS       equ     0008    ; Used to write a prompt to a device
SF_CRV_WR_ACK           equ     000Ah   ; Used to write an acknowledge to a
                                        ; device
SF_CRV_REPEAT           equ     000Ch   ; Function is used to set a 30 Hz or
                                        ; 60 Hz repeat for keyboards
SF_CRV_DISABLE_REPEAT   equ     000Eh   ; Used to cancel the repeat rates in 
                                        ; keyboards
SF_CRV_SELF_TEST        equ     0010    ; Used to issue a selftest command
                                        ; to a physical device
SF_CRV_REPORT_STATUS    equ     0012    ; Used to get the status information 
                                        ; that an HPHIL device might wish to
                                        ; report.  For specific information
                                        ; on what is reported, see the specs
                                        ; for the device.
SF_CRV_REPORT_NAME      equ     0014    ; This function is used to return the
                                        ; ascii name that a device has.
SF_ENVOY_REPEAT         equ     0016    ; Used to set the keyboard repeat
                                        ; and delay rates
SF_ENVOY_LED            equ     0018    ; Used to set the keyboard LEDs

;**********************************************************************
; V_HPHIL Data Structure - do not tread lightly in this data structure
;**********************************************************************
HPHIL_DATA              struc
                        db      size HP_SHEADER DUP (?) ; dirver header
CRV_STATUS              dw      ?
CRV_ENVOY_STATUS        db      ?     ; status bits used to support
                                      ; the envoy keyboard
CRV_CURRENT_STATE       db      ?     ; current configuration address
CRV_ADDRESS             db      ?     ; configuration address
CRV_POLL_ADDRESS        db      ?     ; address of device whose poll
CRV_POLL_HEADER         db      ?     ; poll header of data being proc
CRV_MAJOR               db      ?     ; default major address
CRV_MINOR               db      ?     ; default minor address
CRV_EXPECTED_CMD        db      ?     ; command expected to return
CRV_KBD_ADDRESS         db      ?     ; address of the keyboard
CRV_MAX_DEVICES         db      ?     ; status byte of itf keycode tra
CRV_KC_STATE            db      ?     ; state of itf kc translation
CRV_POLL_RECORD         db      32 DUP (?)      ; poll record buffer
RQ_ENV_REPEAT           db      ?     ; envoy special command for repe
RQ_ENV_DELAY            db      ?     ; envoy special command for dela
RQ_ENV_LED              db      ?     ; envoy special command for led'
RQ_RequEST              db      ?
RQ_REGISTER             db      ?
RQ_ADDRESS              db      ?
RQ_DATA                 db      ?
RQ_MAX_BURST            dw      ?
RQ_COUNT                dw      ?
RQ_OFFSET               dw      ?
RQ_SEGMENT              dw      ?
RQ_SPECIAL              db      ?
RQ_TIMEOUT              db      ?
HPHIL_DATA      ends

;***********************************************************************
; HPHIL Extended Header DH_CLASS
;***********************************************************************
CLASS_KBD        equ      00H
CLASS_TS         equ      01H
CLASS_ASCII      equ      02H
CLASS_BINARY     equ      03H
CLASS_MOUSE      equ      04H
CLASS_GIDCCP     equ      05H
CLASS_TABLET     equ      06H
CLASS_JOY        equ      07H
CLASS_UNDEF8     equ      08H
CLASS_PADDLE     equ      09H
CLASS_THUMB      equ      0AH
CLASS_TRACKBALL  equ      0BH
CLASS_KEYPAD     equ      0CH
CLASS_UNDEFD     equ      0DH
CLASS_UNDEFE     equ      0EH
CLASS_UNDEFF     equ      0FH


;**************************************************************************
; Industry Standard  equates.
;**************************************************************************

INT_DIVIDE_ZERO		equ	00H
INT_SINGLE_STEP		equ	01H
INT_NMI			equ     02H
INT_BREAKPOINT		equ	03H
INT_OVERFLOW		equ	04H
;**************************************************************************
; Print Screen Service
;**************************************************************************
INT_PRINT_SCREEN	equ	05H
;**************************************************************************
; 8259 Master Interrupt Controller Hardware Interrupts
;**************************************************************************

INT_IRQ0_TIMER		equ     08H								  
INT_IRQ1_KBD_ISR	equ     09H								  
INT_IRQ2 		equ     0AH								  
INT_IRQ3_SERIAL2 	equ     0BH								  
INT_IRQ4_SERIAL1	equ     0CH								  
INT_IRQ5_PRN2		equ     0DH								  
INT_IRQ6_FLOPPY		equ     0EH								  
INT_IRQ7_PRN1		equ     0FH								  

;**************************************************************************
;  Interrupt 10H,  Video Support Functions
;**************************************************************************
INT_VIDEO               equ     10H     ;Video Functions Interrupt

F10_SET_MODE            equ     00H     ;Set Video Mode
F10_SET_CURSIZE         equ     01H     ;Set Cursor Size
F10_SET_CURPOS          equ     02H     ;Set Cursor Position
F10_RD_CURPOS           equ     03H     ;Read Cursor Position
F10_RD_PENPOS           equ     04H     ;Read Light-Pen Position
F10_SET_PAGE            equ     05H     ;Set Active Display Page
F10_SCROLL_UP           equ     06H     ;Scroll Rectangle Up
F10_SCROLL_DN           equ     07H     ;Scroll Rectangle Down
F10_RD_CHARATR          equ     08H     ;Read Character and Attribute at
                                        ;Cursor Position
F10_WR_CHARATR          equ     09H     ;Write Character and Attribute at
                                        ;Cursor Position
F10_WR_CHARCUR          equ     0AH     ;Write Character at Cursor Position
F10_SET_PALLET          equ     0BH     ;Set Color Pallet
F10_WR_PIXEL            equ     0CH     ;Write Pixel Dot  
F10_RD_PIXEL            equ     0DH     ;Read Pixel Dot
F10_WR_CHARTEL          equ     0EH     ;Teletype Character Write
F10_GET_STMODE          equ     0FH     ;Get Video State and Mode
        ; Function codes 10H - 12H are reserved
F10_WRS_CHAR            equ     13H     ;Write Character String
F10_INQUIRE             equ     6F00H   ;EX-BIOS present
F10_GET_INFO            equ     6F01H   ;Get video parameters
F10_SET_INFO            equ     6F02H   ;Sets video parameter
F10_MOD_INFO            equ     6F03H   ;Modifies video parameters
F10_GET_RES             equ     6F04H   ;Reports video resoultion
F10_SET_RES             equ     6F05H   ;Sets video resolution
F10_XSET_MODE           equ     6F06H   ;Sets extended video mode

;**************************************************************************
; Interrupt 11H,  Equipment Determination Function
;**************************************************************************
INT_equIPMENT           equ     11H     ;Equipment Determination Interrupt

;**************************************************************************
;  Interrupt 12H, Report System Memory Size
;**************************************************************************
INT_MEM_SIZE            equ     12H     ;Memory Size Interrupt

;**************************************************************************
; Interrupt 13H, Internal Disc Support Functions (Flexible and Hard discs)
;**************************************************************************
INT_DISC                equ     13H     ;Hard Disc Functions Interrupt

F13_RESET_DISC          equ     00H     ;Reset Hard Disc
F13_RD_LSTATUS          equ     01H     ;Read Status of Last Operation
F13_RD_SECTORS          equ     02H     ;Read Sectors
F13_WR_SECTORS          equ     03H     ;Write Sectors
F13_VR_SECTORS          equ     04H     ;Verify Sectors
F13_FORMAT_FLEX         equ     05H     ;Format Diskette Track
        ; Function code 06H is reserved
F13_FORMAT_HDISC        equ     07H     ;Format Hard Disc
F13_GET_HPARMS          equ     08H     ;Get Hard Disc Parameters
        ; Function codes 09H - 0BH are reserved
F13_TRACK_SEEK          equ     0CH     ;Seek to Track
F13_ALT_RESET           equ     0DH     ;Alternate Hard Disc Reset
        ; Function codes 0EH - 014H are reserved
F13_GET_DASD            equ     15H     ;Read Disc Type (DASD)  
F13_CHG_STATUS          equ     16H     ;Get Disc Change Line Status
F13_SET_DASD            equ     17H     ;Set Disc Type for Flexible Disc
                                        ; Formatting(DASD)

;**************************************************************************
; Interrupt 14H,  Serial Communications Functions 
;**************************************************************************
INT_SERIAL              equ     14H     ;Serial Communications driver 

F14_INIT                equ     00H     ;Initialize Serial Port Parameters
F14_XMIT                equ     01H     ;Send Out One Character 
F14_RECV                equ     02H     ;Receive One Character
F14_STATUS              equ     03H     ;Get Serial Port Status               

F14_INQUIRE             equ     6F00H   ;Reports if EX-BIOS Functions
                                        ; are present
F14_EXINIT              equ     6F01H   ;Initializes serial port
                                        ; (19.2 Kbaud)   
F14_PUT_BUFFER          equ     6F02H   ;Writes a buffer of data
F14_GET_BUFFER          equ     6F03H   ;Reads a buffer of data
F14_TRM_BUFFER          equ     6F04H   ;Reads a buffer of data,
                                        ;terminates on specified condition.

;**************************************************************************
; Interrupt 15H,  System Control Functions (Processor Mode Switch, Extended
;                                           Memory Functions, ETC.)
;**************************************************************************
INT_SYSTEM              equ     15H     ;System/Cassette Functions Interrupt

        ; function codes 0 - 3 for Cassette Handling are Unsupported
F15_DEVICE_OPEN         equ     80H     ;Device Open
F15_DEVICE_CLOSE        equ     81H     ;Device Close
F15_PROG_TERM           equ     82H     ;Program Termination
F15_WAIT_EVENT          equ     83H     ;Event Wait
F15_JOYSTICK            equ     84H     ;Joystick Support
F15_SYS_REQ             equ     85H     ;System Request Key Pressed
F15_WAIT                equ     86H     ;Wait Fixed Amount of Time
F15_BLOCK_MOVE          equ     87H     ;Move Block of Memory to/from
                                        ;Extended Memory
F15_GET_XMEM_SIZE       equ     88H     ;Get Extended Memory Size
F15_ENTER_PROT          equ     89H     ;Switch to Protected Mode
F15_DEV_BUSY            equ     90H     ;Device Busy Loop
F15_INT_COMPLETE        equ     91H     ;Set Interrupt Completed Flag

;**************************************************************************
; Interrupt 16H, Keyboard Support Functions
;**************************************************************************
INT_KBD                 equ     16H     ;Keyboard Driver

F16_GET_KEY             equ     00H     ;Read keycode from Keyboard Buffer
F16_STATUS              equ     01H     ;Report Status of keyboard buffer
F16_KEY_STATE           equ     02H     ;Get Key Modifier Status
F16_INQUIRE             equ     6F00H   ;Inquire EX-BIOS Functions present
F16_DEF_ATTR            equ     6F01H   ;Reports default values for repeat
                                        ;rates and delay time before repeat.
F16_GET_ATTR            equ     6F02H   ;Reports current repeat rates and
                                        ;delay time.
F16_SET_ATTR            equ     6F03H   ;Replaces current repeat rates and
                                        ;delay time.
F16_DEF_MAPPING         equ     6F04H   ;Reports default HP-System vector
                                        ;entries for keyboard translator
                                        ;drivers.
F16_GET_MAPPING         equ     6F05H   ;Reports current HP-System vector
                                        ; entries for keyboard translator
                                        ; drivers.
F16_SET_MAPPING         equ     6F06H   ;Replaces current HP-System vector
                                        ; entries for keyboard translator
                                        ; drivers.
F16_SET_TRANSLATORS     equ     6F07H   ;Switches either the cursor control
                                        ; pad translator or the HP Softkey
                                        ; translator functions.
F16_KBD                 equ     6F08H   ;Reports keyboard HPHIL address and
                                        ; Identification.
F16_KBD_RESET           equ     6F09H   ;Resets logical keyboard structure
                                        ; to defaults.

;**************************************************************************
; Interrupt 17H, Printer Support Functions
;**************************************************************************
INT_PRINTER             equ     17H     ;Printer Port Support Interrupt

F17_PUT_CHAR            equ     00H     ;Send Printer One Byte
F17_GET_CHAR            equ     01H     ;Initialize Printer
F17_STATUS              equ     02H     ;Get Printer Status

F17_INQUIRE             equ     6F00H   ;Reports EX-BIOS functions exists
F17_READ_STATUS         equ     6F01H   ;Reports the status of a printer
                                        ; port read buffer.
F17_PUT_BUFFER          equ     6F02H   ;Writes a buffer of data to the
                                        ; printer port.
F17_GET_BUFFER          equ     6F03H   ;Reads a buffer of data.
F17_TRM_BUFFER          equ     6F04H   ;Reads a buffer of data from the
                                        ; port, terminates on specified
                                        ; condition.
;**************************************************************************
;  BASIC Entyrypoint -- Executes as an IRET
;**************************************************************************
			 		
INT_BASIC		equ	18H	; Active resident Language			  
;**************************************************************************
; Reboot System
;**************************************************************************
INT_BOOT		equ     19H	; Reboot System
;**************************************************************************
; Interrupt 1AH,  Real-Time Clock Support Functions
;**************************************************************************
INT_CLOCK               equ     1AH     ; Clock Functions Interrupt

F1A_RD_CLK_CNT          equ     00H     ;Read Current Clock Count
F1A_SET_CLK_CNT         equ     01H     ;Set Current Clock Count
F1A_GET_RTC             equ     02H     ;Read Real-Time Clock
F1A_SET_RTC             equ     03H     ;Set Real-Time Clock
F1A_GET_DATE            equ     04H     ;Read Date from Real-Time Clock
F1A_SET_DATE            equ     05H     ;Set Date in Real-Time Clock
F1A_SET_ALARM           equ     06H     ;Set Alarm
F1A_RESET_ALARM         equ     07H     ;Reset Alarm                
;**************************************************************************
; Interrupt 1BH,  Break Key
;**************************************************************************
INT_BREAK_EVENT		equ     1BH
;**************************************************************************
;  Timer Tick Event
;**************************************************************************
INT_TIMER_TICK		equ	1CH
;**************************************************************************
; Video Parameters
;**************************************************************************
INT_VIDEO_PARMS		equ	1DH
;**************************************************************************
; Floppy Parameters
;**************************************************************************
INT_FLOPPY_PARMS	equ	1EH
;**************************************************************************
; Graphics Characters Table
;**************************************************************************
INT_GRAPHICS_CHAR	equ	1FH

;**************************************************************************
; Interrupt 33H,  HP Mouse Support (MS-Mouse Emulation)
; Full AX register used for function code
;**************************************************************************
INT_HPMOUSE             equ     33H
F33_MSE_INSTALL         equ     0000H   ; Mouse installed flag and reset hardware.
F33_MSE_ENABLE          equ     0001H   ; Put cursor on screen.
F33_MSE_DISABLE         equ     0002H   ; Turn off cursor.
F33_MSE_REPORT_DATA     equ     0003H   ; Get positional data and button info
F33_MSE_PUT_CURSOR      equ     0004H   ; Positon the cursor.
F33_MSE_REPORT_PRESS    equ     0005H   ; Report button press status
F33_MSE_REPORT_RELEASE  equ     0006H   ; Report button release information.
F33_MSE_SET_HORIZ       equ     0007H   ; Set minimum and maximum horizontal
                                        ; values.
F33_MSE_SET_VERT        equ     0008H   ; Set min and max vertical values.
F33_MSE_GRAPH_CURSOR    equ     0009H   ; Define the graphics cursor.
F33_MSE_TEXT_CURSOR     equ     000AH   ; Define the text cursor.
F33_MSE_MOTION          equ     000BH   ; Report motion counters.
F33_MSE_SET_USR         equ     000CH   ; Define user subroutine call.
F33_MSE_ENABLE_LIGHT    equ     000DH   ; Enable light pen emulation mode.
F33_MSE_DISABLE_LIGHT   equ     000EH   ; Disable  light pen emulation.
F33_MSE_RATIO           equ     000FH   ; Set pixel movement ratio.
F33_MSE_COND_OFF        equ     0010H   ; Define area to conditionally turn
                                        ; display off.  Not used.
F33_MSE_XTEND_GCSR      equ     0012H   ; Extended sprite graphics entry point.
F33_MSE_SPEED           equ     0013H   ; Set mouse doubling speed factor.

F33_INQUIRE             equ     6F00H   ; Returns "HP" in bx if HPMOUSE driver
                                        ; is being used.
;**************************************************************************
; When there is a fixed disc installed, direct entrypoint to floppy
;**************************************************************************
INT_FLOPPY_DIRECT	equ	40H
;**************************************************************************
; Fixed Disc Parameters
;**************************************************************************
INT_HDISC_PARMS		equ	41H
;**************************************************************************
; Real Time Clock Event vector
;**************************************************************************
INT_RTC_EVENT		equ	4AH
;**************************************************************************
; 8259 EX-BIOS Slave Interrupt Controller
;**************************************************************************
INT_SVC_REQUEST		equ	68H
INT_8041_OBF		equ	69H
INT_HPHIL		equ	6CH
INT_HPENTRY		equ	HPENTRY
;**************************************************************************
; 8259 Slave Interrupt Controller Interrupts
;**************************************************************************

IRQ8_RTC		equ	70H
IRQ9_REDIRECT		equ	71H
IRQ10			equ	72H
IRQ11			equ	73H
IRQ12			equ	74H
IRQ13_287		equ	75H
IRQ14_HDISC		equ	76H
IRQ15			equ	77H

;******************************************************************************
;  use this macro to setup the MS-DOS driver header required at the top of
;  any installable device driver
;******************************************************************************
msd_header 	macro    ATT,STRATEGY_ENTRY,ISR_ENTRY,STRING
		dd        -1		;mark as last driver in list
  		dw        ATT
		dw        STRATEGY_ENTRY
		dw        ISR_ENTRY
		db        STRING
		db        14 dup (?)	; Pad so it is paragraph aligned. (~~AA)
	        endm
page
;******************************************************************************
; the following structures are used to access MS-DOS driver command blocks
;******************************************************************************
MSD_REQ_HEADER	struc		;00; structure for access to MS driver cmds
MSD_CMDLEN	db	?	;00; length of cmd in bytes including data @ end
MSD_UNIT	db	?	;01; unit number for command
MSD_CMD		db	?	;02; command code
MSD_STATUS	dw	?	;03; filler with completion status before return
		db 8 dup (?)    ;  ; area reserved for DOS
MSD_MEDIA	db	?	;13; most cmds have this defined in the data area
MSD_TRANS	dw	?	;14;
		dw	?	;16;
MSD_COUNT	dw	?	;18;
MSD_START	dw	?	;20;
MSD_REQ_HEADER  ends
;******************************************************************************
; Access to the data area of the INIT driver command
;******************************************************************************
MSD_INIT_CMD	struc
		db	13 dup (?)   ;first cover header area
MSD_UNIT_COUNT	db	?	; 0B ;number of units service by this driver
MSD_END_OFFSET	dw	?	; 0C ;offset of end of code
MSD_END_SEG	dw	?	; 0E ;segment address of end of code
MSD_BPB_OFFSET	dw	?	; 12 ;
MSD_BPB_SEG	dw	?	; 14 ;seg:offset of BPB list for units attached
MSD_1ST_UNIT	db	?	; 16 ;tells driver letter of first unit
MSD_INIT_CMD	ends
;******************************************************************************
; Access to the data area for INPUT or OUTPUT driver commands
;******************************************************************************
MSD_IO_CMD	struc
		db	14 dup (?) ;Media byte defined in header
MSD_XFER_OFFSET	dw	?
MSD_XFER_SEG	dw	?	;full address of buffer for data transfer
MSD_XFER_COUNT	dw	?	;could be bytes or block count
MSD_1ST_BLK	dw	?	;address of first block to read or write
MSD_VERR_SEG	dw	?	;pointer to volume id if err code => 0Fh
MSD_IO_CMD	ends




