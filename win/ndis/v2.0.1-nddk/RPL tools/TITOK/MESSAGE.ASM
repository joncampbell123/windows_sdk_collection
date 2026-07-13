		PAGE  60, 132 
		TITLE Token-Ring RIPL Boot ROM Driver - MESSAGE.ASM
;**********************************************************************;
;**********************************************************************;
;								       ;
; File Name:       MESSAGE.ASM 					       ;
;								       ;
; Program Name:    Token-Ring RIPL Boot ROM Device Driver              ;
;								       ;
; Companion Files: None						       ;
;								       ;
; Function:        Data declarations for all messages printed during   ;
;                  the initialization process.                         ;
;								       ;
; History:             						       ;
;								       ;
;     03/16/89  Mike Witkowski - Original Version 0.0		       ;
;     02/27/90  Mike Witkowski - Reworked for RIPL                     ;
;								       ;
;**********************************************************************;
;**********************************************************************;

;-------------------------------------;
; Include all general companion files ;
;-------------------------------------;
 					   
		.XLIST
	       	INCLUDE includes.inc  	
		.LIST

CR		EQU	0dh 		; Carriage Return
LF		EQU	0ah		; Line Feed
TM		EQU	0		; Line Terminator

START_DATA

PAGE_BREAK <Message Definitions>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
; Message Definitions                                                  ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

MSG_ninstalled	DB	text_ninstalled, CR, TM	
MSG_carderror	DB	text_carderror, CR, TM
MSG_initerror	DB	text_initerror, CR, TM
MSG_irqerror	DB	text_irqerror, CR, TM 
MSG_dmaerror	DB	text_dmaerror, CR, TM 
MSG_downerror	DB	text_downerror, CR, TM 
MSG_mediaerror	DB	text_mediaerror, CR, TM 
MSG_speederror	DB	text_speederror, CR, TM 
MSG_openerror	DB	text_openerror, CR, TM 

MLABEL <MSG_statuserr>, %MBYTE, %MPUBLIC
       	        DB	text_statuserr, CR, TM 
MLABEL <MSG_beacon>, %MBYTE, %MPUBLIC
                DB	text_beacon, CR, TM 


PAGE_BREAK <Message Table Definitions>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
; MSG_cardinit - table defining the offsets to all the card initiali-  ;
;                zation messages.                                      ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
MLABEL <gbl_message>, %MWORD, %MPUBLIC
		DW	OFFSET DGROUP:MSG_ninstalled
		DW	OFFSET DGROUP:MSG_carderror
		DW	OFFSET DGROUP:MSG_initerror
		DW	OFFSET DGROUP:MSG_carderror
		DW	OFFSET DGROUP:MSG_irqerror
		DW	OFFSET DGROUP:MSG_dmaerror
		DW	OFFSET DGROUP:MSG_downerror
		DW	OFFSET DGROUP:MSG_mediaerror
		DW	OFFSET DGROUP:MSG_speederror
		DW	OFFSET DGROUP:MSG_initerror
		DW	OFFSET DGROUP:MSG_openerror
		DW	OFFSET DGROUP:MSG_openerror
		DW	0
		DW	OFFSET DGROUP:MSG_carderror
		DW	OFFSET DGROUP:MSG_irqerror

END_DATA

	END
