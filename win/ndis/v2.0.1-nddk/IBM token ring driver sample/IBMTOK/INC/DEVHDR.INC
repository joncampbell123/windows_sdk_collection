;        SCCSID = @(#)devhdr.inc        8.2 87/01/29
;
; <<==++ Copyright (c) Microsoft Corporation  1986 ++==>>
;
;**     Device Table Record
;
;       Devices are described by a chain of these records

SysDev  STRUC
SDevNext        DD      ?       ; Pointer to next device header
SDevAtt         DW      ?       ; Attributes of the device
SDevStrat       DW      ?       ; Strategy entry point
SDevInt         DW      ?       ; Interrupt entry point
SDevName        DB      '        ' ; Name of device (only first byte used for block)
SDevProtCS      DW      ?       ; Protect-mode CS selecter of strategy entry pt
SDevProtDS      DW      ?       ; Protect-mode DS selecter
SDevRealCS      DW      ?       ; Real-mode CS segment of strategy entry pt
SDevRealDS      DW      ?       ; Real-mode DS segment
SysDev  ENDS

;*      Field aliases for use in loading block drivers

SDevCDPBs       equ     byte ptr SDevName+1
SDevDPBseg      equ     word ptr SDevProtCS
SDevBPBvec      equ     word ptr SDevProtDS

;**     Device Driver Type defininitions
;

DEV_CIN         EQU 0000000000000001B ;  0  2 5 Device is console in
DEV_COUT        EQU 0000000000000010B ;  1  2 5 Device is console out
DEV_NULL        EQU 0000000000000100B ;  2  2 5 Device is the Null device
DEV_CLOCK       EQU 0000000000001000B ;  3  2 5 Device is the clock device
DEV_SPEC        EQU 0000000000010000B ;  4  2   Devices can support INT 29h
DEV_ADD_ON      EQU 0000000000100000B ;  5      Device is add-on driver (BWS)
DEV_GIOCTL      EQU 0000000001000000B ;  6   3  Device supports generic ioctl
DEV_FCNLEV      EQU 0000001110000000B ; 9-7   5 Device function level
;                   0000010000000000B ; 10
DEV_30          EQU 0000100000000000B ; 11  2 5 Accepts Open/Close/Removable Media
;                   0001000000000000B ; 12
DEV_NON_IBM     EQU 0010000000000000B ; 13  2 5 Device is a non IBM device.
DEV_IOCTL       EQU 0100000000000000B ; 14  2   Device accepts IOCTL request
DEV_CHAR_DEV    EQU 1000000000000000B ; 15  2 5 Device is a character device


;
;       ***** BUGBUG *****
;
;   DOS 3.2 Screws us as it uses bit 6 to indicate generic ioctl support
;   for the time being those device drivers are SOL.

;       Level definitions for devices

;                   0000001110000000B   ; Device function level
DEVLEV_0    EQU     0000000000000000B   ; DOS 3.0 and before (NEEDS TO BE FIXED)
DEVLEV_1    EQU     0000000010000000B   ; DOS 5.0
DEVLEV_2    EQU     0000000000000000B
DEVLEV_3    EQU     0000000000000000B
DEVLEV_4    EQU     0000000000000000B
DEVLEV_5    EQU     0000000000000000B


;;;DEVLEV_2   EQU  DEV_30 OR 0000000001000000B ; Set/Get Map; Generic IOCTL.
;;;DEVLEV_3   EQU  DEV_30 OR 0000000010000000B ; Pause/Restart/GenIOCTL/VID
;;;; New device driver spec
;;;DEVLEV_4   EQU  DEV_30 OR 0000000000000000B ; DOS 5.0
;;;DEVLEV_5   EQU  DEV_30 OR 0000000100000000B ; For future use
;;;DEVLEV_6   EQU  DEV_30 OR 0000000101000000B ;   "
;;;DEVLEV_7   EQU  DEV_30 OR 0000000110000000B ;   "
;;;DEVLEV_8   EQU  DEV_30 OR 0000000111000000B ;   "
