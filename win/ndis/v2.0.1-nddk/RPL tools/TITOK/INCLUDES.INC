;**********************************************************************;
;**********************************************************************;
;								       ;
; File Name:       INCLUDES.INC                  		       ;
;								       ;
; Program Name:    TRNDD (Token-Ring Network Device Driver)	       ;
;								       ;
; Companion Files: None						       ;
;								       ;
; Function:        This module contains all the assembler statements   ;
;		   to include all the TRNDD include files.  This       ;
;		   module should be the only included file in all the  ;
;                  TRNDD assembly files.                               ;     
;                                                                      ;     
; History:             						       ;
;								       ;
;     03/13/89  Mike Witkowski - Original Version 0.0		       ;
;								       ;
;**********************************************************************;
;**********************************************************************;


		INCLUDE keys.inc        ; General macro definitions               
		INCLUDE macros.inc      ; General macro definitions               
		INCLUDE adapter.inc     ; General macro definitions               
		INCLUDE	drvseg.inc	; Segment order defintions
       		INCLUDE tmsstrct.inc	; Adapter specific structures
       		INCLUDE ripl.inc	; RIPL specific structures
		INCLUDE macstrct.inc    ; MAC/NDI internal structures
		INCLUDE message.inc	; Message text includes
