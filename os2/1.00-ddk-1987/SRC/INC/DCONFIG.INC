;      SCCSID = @(#)dconfig.inc	5.1 86/11/10

;
; <<==++ Copyright (c) Microsoft Corporation  1986 ++==>>
;


;       Disk/Diskette Configuration Data Table

ROMcfg          struc
ROMlink         dw   ?                ; link to next table entry
ROMcyls         dw   ?                ; maximum number of cylinders
ROMsecptrk      dw   ?                ; maximum sectors per track
ROMdevnbr       db   ?                ; device number
ROMheads        dw   ?                ; number of heads
ROMwpf          dw   ?                ; write precompensation factor
ROMflags        dw   ?                ; flag byte
;ROMfixed       equ  0001h            ;   0=diskette, 1=fixed disk
;ROMchangeline  equ  0002h            ;   change line is supported
ROMcfg          ends

;       Flag bits

ROMfixed        equ  0001h            ;   0=diskette, 1=fixed disk
ROMchangeline   equ  0002h            ;   change line is supported
