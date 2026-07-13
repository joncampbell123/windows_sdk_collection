		PAGE  60, 132 
		TITLE RIPL DLC Boot ROM Module - ROMTOP.ASM
;**********************************************************************;
;**********************************************************************;
;								       ;
; File Name:       ROMTOP.ASM					       ;
;								       ;
; Program Name:    RIPL DLC Boot ROM Module             	       ;
;								       ;
; Companion Files: None						       ;
;								       ;
; Function:        Contains standard Boot ROM header: ROM stamp, and   ;
;                  size of ROM.                                        ;
;								       ;
;								       ;
; History:             						       ;
;								       ;
;     02/20/90  Mike Witkowski - Original Version 0.0		       ;
;								       ;
;**********************************************************************;
;**********************************************************************;

;-------------------------------------;
; Include all general companion files ;
;-------------------------------------;
 					   
		.XLIST
	       	INCLUDE includes.inc  	
		.LIST

;--------------------------;
; External Data References ;
;--------------------------;

START_HEADER

BootROMID	DB	55h, 0AAh
ROMPages        DB	16		; Set 8K ROM size

END_HEADER

	END
