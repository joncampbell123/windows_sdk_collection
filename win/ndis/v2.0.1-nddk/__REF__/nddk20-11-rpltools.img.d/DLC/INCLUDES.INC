;**********************************************************************;
;**********************************************************************;
;								       ;
; File Name:       INCLUDES.INC                  		       ;
;								       ;
; Program Name:    RIPL DLC Boot ROM Module             	       ;
;								       ;
; Companion Files: None						       ;
;								       ;
; Function:        This module contains all the assembler statements   ;
;		   to include all the include files.  This module      ;
;		   module should be the only included file in all the  ;
;                  source assembly files.                              ;     
;                                                                      ;     
; History:             						       ;
;								       ;
;     02/20/90  Mike Witkowski - Original Version 0.0		       ;
;								       ;
;**********************************************************************;
;**********************************************************************;


		INCLUDE keys.inc        ; General macro definitions               
		INCLUDE macros.inc      ; General macro definitions               
		INCLUDE	drvseg.inc	; Segment order defintions
		INCLUDE ripl.inc        ; RIPL frame format defintions
		INCLUDE message.inc     ; ROM Messages
