;***    IOCTL.INC - IOCTL Definitions
;
;       SCCSID = @(#)ioctl.inc	8.2 86/11/17

;
; <<==++ Copyright (c) Microsoft Corporation  1986 ++==>>
;


;**     Device Driver COM Block format

IOGC    STRUC

IOGCDM  DB      size SRHEAD DUP(?)      ; fixed header
IOGCMI  DB      ?               ; minor function code
IOGCMA  DB      ?               ; major function code
IOGCSI  DW      ?               ; SI contents
IOGCDI  DW      ?               ; DI contents
IOGCAD  DD      ?               ; data address

IOGC    ENDS



;**     Major and Minor Codes
;       Category(Major) and Function(Minor) Codes
;       -----------------------------------------
;
;       The Major and Minor values are each contained in a byte.
;
;
;       Category Code:
;       (Major Code)
;
;               0... ....               - Microsoft Defined
;               1... ....               - Oem/User Defined
;               .xxx xxxx               - Code
;
;       WARNING:  When adding new category codes update the MAX_CAT_CODE equ.
;
;       Function Code
;       (Minor Code)
;
;               0... ....               - Return error if unsupported
;               1... ....               - Ignore if unsupported
;               .0.. ....               - Intercepted by DOS
;               .1.. ....               - Passed to driver
;               ..0. ....               - Function sends data/commands to device
;               ..1. ....               - Function querys data/info from device
;               ...x xxxx               - Subfunction
;
;
;       Note that the "sends/querys" data bit is intended only to
;       regularize the function set.  It plays no critical role; some
;       functions may contain elements of both command and query.  The
;       convention is that such commands are defined as "sends data".

IOFUNC_SUBFUNC  equ     1Fh             ; Function code subfunction
IOFUNC_READ     equ     20h             ; Read subfunction bit
IOFUNC_DRIVER   equ     40h             ; passed to driver
IOFUNC_IGNERR   equ     80h             ; ignore if unsupported
IOCAT_USERDEF   equ     80h             ; user defined category

IOMIN_SUBFUNC   equ     1fh             ; Minor code subfunction
IOMIN_READ      equ     20h             ; Read subfunction bit
IOMIN_DRIVER    equ     40h             ; Passed to driver
IOMIN_IGNERR    equ     80h             ; Ignore if unsupported


;**     Major and Minor Codes


IOC_SE  EQU      1              ; serial device control
        IOSW_BR EQU     41H             ; set baud rate
        IOSR_BR EQU     61H             ; return baud rate
        IOSW_LC equ     42h             ; set line control (stop bits, parity)
        IOSR_LC equ     62h             ; return line control
        IOSW_FI equ     03h             ; flush input (seen by DOS and driver)
        IOSW_FO equ     04h             ; flush output (seen by DOS and driver)
        IOSR_LS equ     65h             ; return current line status
        IOSW_MC equ     46h             ; return modem control
        IOSR_MC equ     66h             ; set modem control
        IOSR_MS equ     67h             ; return modem status
        IOSR_SI equ     68h             ; return # of input chars available
        IOSR_SO equ     69h             ; return # of queued output chars
        IOSW_TI equ     4ah             ; transmit immediate
        IOSW_TB equ     4bh             ; transmit "break"
        IOSW_XF equ     4ch             ; behave as if XOFF received
        IOSW_XN equ     4dh             ; behave as if XON received
        IOSW_XE equ     4eh             ; enable automatic XON/XOFF behaviour
        IOSW_XD equ     4fh             ; disable automatic XON/XOFF behaviour

IOC_TC  EQU      2              ; Terminal Control
        IOTW_KI EQU     03H             ; set keyboard intercept characters
        IOTR_KI EQU     23H             ; get keyboard intercept characters
        IOTW_VS EQU     44H             ; Set video state
        IOTR_VS EQU     64H             ; Return video state
        IOTW_SS EQU     45H             ; screen shift
        IOTR_SS EQU     66H             ; return screen shift status

IOC_SC  EQU      3              ; Screen Control
        IOSC_LS EQU     41H             ; Locate SIB
        IOSC_SS EQU     42H             ; save segment
        IOSC_RS EQU     43H             ; restore segment
        IOSC_EI EQU     44H             ; re-enable I/O
        IOSC_IS EQU     45H             ; initialize screen
        IOSC_RW EQU     47H             ; reset pop-up window
        IOSC_PW EQU     48H             ; establish pop-up window
        IOSC_DW EQU     49H             ; destroy pop-up window
        IOSC_DI EQU     4AH             ; disable I/O
        IOSC_VS EQU     4BH             ; Set/return video mode
        IOSC_SH EQU     4CH             ; Set/return screen shift
        IOSC_MS EQU     4DH             ; Set/return maximum screensegsize
        IOSC_CS EQU     4FH             ; Close Screen

IOC_KC  EQU      4              ; Keyboard Control
        IOKC_IM EQU     51H             ; Set Input Mode
        IOKC_LK EQU     41H             ; Locate KIB
        IOKC_SS EQU     42H             ; save segment
        IOKC_RS EQU     43H             ; restore segment
        IOKC_CK EQU     44H             ; change keyboard images
        IOKC_IK EQU     45H             ; initialize keyboard
        IOKC_SL EQU     06H             ; set console locus
        IOKC_RL EQU     07H             ; reset console locus
        IOKC_PK EQU     48H             ; establish pop-up keyboard
        IOKC_DK EQU     49H             ; drop pop-up keyboard
        IOKC_DI EQU     4AH             ; disable I/O
        IOKC_MS EQU     6DH             ; Return keyboard size.
        IOKC_KC EQU     6FH             ; Close Keyboard.

IOC_PC  EQU      5              ; printer Control
        IOLW_FC EQU     42H             ; Frame control (CPL, LPI)
        IOLR_FC EQU     62H             ;
        IOLW_RD EQU     43H             ; Re-direction
        IOLR_RD EQU     63H             ;
        IOLW_IR EQU     44H             ; Infinite re-try
        IOLR_IR EQU     64H             ;
        IOLW_AF EQU     48H             ; @@ Activate Font


IOC_LP  EQU      6              ; light pen
;;      <undefined>


IOC_MC  EQU      7              ; mouse Control
;;      <undefined>

;IOC_PA EQU     8               ; print arbitrater
;       IOPA_SP EQU     040H            ; Set arbitrator pushout buffer
;       IOPA_EN EQU     041H            ; Enable arbitration
;       IOPA_RG EQU     042H            ; Register process as spooler


IOC_DC  EQU     8               ; disk control
        IODC_LK EQU     00H             ; Lock drive
        IODC_UL EQU     01H             ; Unlock drive
        IODC_RM EQU     02H             ; Redetermine media
        IODC_SL EQU     03H             ; Set Logical Map
        IODC_BR EQU     20H             ; Block removable
        IODC_GL EQU     21H             ; Get Logical Map
        IODC_SP EQU     43H             ; Set device parameters
        IODC_WT EQU     44H             ; Write track
        IODC_FT EQU     45H             ; Format track
        IODC_GP EQU     63H             ; Get device parameters
        IODC_RT EQU     64H             ; Read track
        IODC_VT EQU     65H             ; Verify track


IOC_PD  EQU     9               ; @@ Physical Disk Control
        IOPD_LK EQU     00H             ; @@ Lock physical drive
        IOPD_UL EQU     01H             ; @@ Unlock physical drive


IOC_MO  EQU     10              ; Monitor control
        IOMO_RE EQU     40H             ; Register
        IOMO_IO EQU     41H             ; I/O

IOC_BC  EQU     11              ; Device buffer control
        IOBC_FI EQU     01H             ; flush input
        IOBC_FO EQU     02H             ; flush output

MAX_CAT_CODE    EQU     11      ; the largest legal category code
                                ; change this equ as new IOCTL
                                ; categories get added

;
; Command code definitions for certain functions
;

; Category 8, Function 43 - Set Device Parameters

        IODC_SP_DEV     EQU     01H     ; Set BPB for physical device
        IODC_SP_MEDIA   EQU     02H     ; Set fake BPB for medium
