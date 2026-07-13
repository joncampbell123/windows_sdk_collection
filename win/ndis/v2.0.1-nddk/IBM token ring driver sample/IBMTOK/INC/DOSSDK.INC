;****************************************************************************;
;*                                                                          *;
;*      (C) Copyright 1988,1989 by 3Com Corporation                         *;
;*                                                                          *;
;*      This IBM Token Ring NDIS Driver source is the property of Microsoft *;
;*      Corporation and is provided only as an example of an NDIS MAC       *;
;*      driver for use with Microsoft's OS/2 LAN Manager, DOS LAN Manager,  *;
;*      or LM/X products.                                                   *;
;*                                                                          *;
;****************************************************************************;

;------ DOSSDK.INC ----------------------------------------------------------;
;                                                                            ;
;       Constants defined in the Microsoft DDK include files for OS/2 but    ;
;       needed also for DOS.                                                 ;
;                                                                            ;
;----------------------------------------------------------------------------;

DEV_CHAR_DEV    EQU             1000000000000000B       ; char device
DEV_IOCTL       EQU             0100000000000000B       ; ioctl support

STERR   EQU             8000h   ; Bit 15 - Error
STDON   EQU             0100h   ; Bit 8 - Done

;
;**     Device Driver Request Packet Defintions
;

PktMax          equ     18

Packet          struc
PktLen          db      ?               ; length in bytes of packet
PktUnit         db      ?               ; subunit number of block device
PktCmd          db      ?               ; command code
PktStatus       dw      ?               ; status word
PktDOSLink      dd      ?               ; reserved
PktDevLink      dd      ?               ; device multiple-request link
PktData         db      PktMax dup (?)  ; data pertaing to specific packet
Packet          ends

CMDInit         equ     0                       ; INIT command
InitcUnit       equ     byte ptr PktData        ; number of units returned
InitpEnd        equ     dword ptr PktData+1     ; pointer to free mem after dev
InitEcode       equ     word ptr PktData+1      ; @@ size of code segment
InitEdata       equ     word ptr PktData+3      ; @@ size of data segment

;
;**     Device Driver Header
;

SysDev          struc
SDevNext        DD      ?       ; Pointer to next device header
SDevAtt         DW      ?       ; Attributes of the device
SDevStrat       DW      ?       ; Strategy entry point
SDevInt         DW      ?       ; Interrupt entry point
SDevName        DB  8 dup (' ') ; Name of device
SysDev          ends

