;***************************************************************************
;                                                                          *
;   Copyright (C) 1983,1984 by Microsoft Inc.                              *
;                                                                          *
;***************************************************************************

;***************************************************************************
;                                                                          *
;   Header file for IBM/PC timer device driver.  Contains all machine      *
;   specific constant and data structure definitions.                      *
;                                                                          *
;***************************************************************************

.xlist
ifdef DOS5
.286p
include infoseg.inc
else
?NODATA=1
endif

include cmacros.inc
include windefs.inc
.list


; Approximate value of timer tic in milliseconds * 1000.
;
ifdef DOS5
res_low     EQU 1000
res_high    EQU 0
else
res_low     EQU 54925
res_high    EQU 0
endif

; equates for disk status
REMOVEABLE  equ 2
FIXED       equ 3

; Location of Single Diskette indicator
SingleDrive equ 504H

ifdef DOS5

; Timer structure
tiblock     STRUC
tifNotBusy  DB  ?       ; -1 means not used.
tiproc      DD  ?       ; far proc to call.
tihandle    DW  ?       ; timer handle [for DOS 5]
tiblock     ENDS

else

; Timer structure
tiblock     STRUC
tirate      DW  ?       ; -1 means not used, 0 means call as fast as possible
ticount     DW  ?       ; -1 means end of table.
tiproc      DD  ?       ; far proc to call.
tiblock     ENDS

endif

;
; macro for retf.
;
retf    macro   cb
        ifnb    <cb>
        db      0cah
        dw      (cb)
        else
        db      0cbh
        endif
        endm
