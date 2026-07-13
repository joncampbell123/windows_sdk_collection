;**********************************************************************;
;**********************************************************************;
;								       ;
; File Name:       KEYS.INC                      		       ;
;								       ;
; Program Name:    TRNDD (Token-Ring Network Device Driver)	       ;
;								       ;
; Companion Files: None						       ;
;								       ;
; Function:        This module contains all the very important         ;
;		   directives that control the way the source code is  ;
;		   assembled.  The most important definition in this   ;
;                  file is the CPU's mode of operation.                ;     
;                                                                      ;     
; History:             						       ;
;								       ;
;     03/14/89  Mike Witkowski - Original Version 0.0		       ;
;								       ;
;**********************************************************************;
;**********************************************************************;


		.8086

MAX_ADPS        EQU     4               ; Maxmimum number od configurations

;TYPE_WDTCPLUS   EQU     1               ; Set type = WD TokenCard Master
TYPE_3C603       EQU     1               ; Set type = 3COM 3C603


IFDEF TYPE_P1345
TYPE_PWD        EQU     1               ; Define Proteon and WD
ELSE
IFDEF TYPE_WDTCPLUS
TYPE_PWD        EQU     1               ; Define Proteon and WD
ENDIF
ENDIF

IFDEF TYPE_P1390
CHIP_EAGLE      EQU     1               ; Define Eagle chip type
ENDIF
