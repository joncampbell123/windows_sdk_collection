;**********************************************************************;
;**********************************************************************;
;								       ;
; File Name:       MESSAGE.INC 					       ;
;								       ;
; Program Name:    Token-Ring RIPL Boot ROM Device Driver              ;
;								       ;
; Companion Files: None						       ;
;								       ;
; Function:        Message Equests for all messages printed during     ;
;                  the initialization process.  		       ;	
;								       ;
; History:             						       ;
;								       ;
;     04/21/89  Mike Witkowski - Original Version 0.0		       ;
;     02/27/90  Mike Witkowski - Reworked for RIPL                     ;
;								       ;
;**********************************************************************;
;**********************************************************************;

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
; Text Macros                                                          ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
text_ninstalled EQU	<"MAC1001: Adapter Not Installed">
text_carderror	EQU	<"MAC1002: Adapter Hardware Error">
text_initerror	EQU	<"MAC1003: Adapter Initialization Error">
text_irqerror	EQU	<"MAC1004: Adapter IRQ Error">
text_dmaerror	EQU	<"MAC1005: Adapter DMA Error">
text_downerror	EQU	<"MAC1006: Adapter Download Error">
text_speederror	EQU	<"MAC1007: Adapter Ring Speed Error">
text_mediaerror	EQU	<"MAC1008: Adapter Media Type Error">
text_openerror	EQU	<"MAC1009: Adapter Open Error">
text_statuserr	EQU	<"Fatal Adapter Status Received">
text_beacon	EQU	<"Ring Beaconing">
