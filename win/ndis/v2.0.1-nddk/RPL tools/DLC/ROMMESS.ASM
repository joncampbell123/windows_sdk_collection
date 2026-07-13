		PAGE  60, 132 
		TITLE RIPL DLC Boot ROM Module - MESSAGE.ASM
;**********************************************************************;
;**********************************************************************;
;								       ;
; File Name:       MESSAGE.ASM 					       ;
;								       ;
; Program Name:    RIPL DLC Boot ROM Module             	       ;
;								       ;
; Companion Files: None						       ;
;								       ;
; Function:        Data declarations for messages printed during the   ;
;                  RIPL process.                                       ;
;								       ;
; History:             						       ;
;								       ;
;     02/26/90  Mike Witkowski - Original Version 0.0		       ;
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

START_DATA

PAGE_BREAK <Message Definitions>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
; Message Definitions                                                  ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
MLABEL <sz_banner>, %MWORD, %MPUBLIC
		DW	size_banner - 1
                DB      text_banner, CR

MLABEL <sz_time>, %MWORD, %MPUBLIC
		DW	size_time - 1
                DB      text_time, CR

MLABEL <sz_drvinit>, %MWORD, %MPUBLIC
		DW	size_drvinit - 1
                DB      text_drvinit, CR

MLABEL <sz_descript>, %MWORD, %MPUBLIC
		DW	size_descript - 1
                DB      text_descript, CR

MLABEL <sz_fretry>, %MWORD, %MPUBLIC
		DW	size_fretry + 1
text_fretry     fretry_struc <>
                DB      CR

MLABEL <sz_sretry>, %MWORD, %MPUBLIC
		DW	size_sretry + 1
text_sretry     sretry_struc <>
                DB      CR

MLABEL <sz_seqnce>, %MWORD, %MPUBLIC
		DW	size_seqnce + 1
text_seqnce     seqnce_struc <>
                DB      CR

MLABEL <sz_blank>, %MWORD, %MPUBLIC
		DW	size_blank - 1
                DB      text_blank, CR

MLABEL <sz_adpok>, %MWORD, %MPUBLIC
		DW	size_adpok - 1
                DB      text_adpok, CR

END_DATA

	END
