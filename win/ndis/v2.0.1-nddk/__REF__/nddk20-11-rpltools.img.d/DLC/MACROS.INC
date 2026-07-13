;**********************************************************************;
;**********************************************************************;
;								       ;
; File Name:       MACROS.INC                    		       ;
;								       ;
; Program Name:    RIPL DLC Boot ROM Module             	       ;
;								       ;
; Companion Files: None						       ;
;								       ;
; Function:        This module contains all the general purpose macro  ;
;		   definitions for the macro used throughout the       ;
;		   source code.                                        ;
;                                                                      ;     
; History:             						       ;
;								       ;
;     02/20/90  Mike Witkowski - Original Version 0.0		       ;
;								       ;
;**********************************************************************;
;**********************************************************************;


;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;                    ASSEMBLY OUTPUT ORIENTED MACROS		       ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

PAGE_BREAK	MACRO parm_title
		SUBTTL parm_title
		PAGE +
                ENDM
		.XCREF PAGE_BREAK


;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;                           STRUCTURE MACROS                           ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

ptr_struct	STRUC
offp		DW	?
segp		DW	?
ptr_struct	ENDS


lword_struct	STRUC	  	; Intel long word structure
lo		DW	?
hi		DW	?
lword_struct	ENDS


mword_struct	STRUC		; Motorola long word structure
Mhi		DW	?
Mlo		DW	?
mword_struct	ENDS


string_struct	STRUC		; String structure 
STR_Len		DW	?
STR_str		DB	?
string_struct	ENDS


;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;                           LABELLING MACROS                           ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

;----------------------------------------------------------------------;
; MLABEL - creates a label for the name given, and assigns the type    ;
;          to the label.  Also, if the publik parameter has been de-   ;
;          fined, then declare the label name as PUBLIC.  We also      ;
;          provide all the necessary equates to use this macro here.   ;
;----------------------------------------------------------------------;

MNEAR	  	EQU	<NEAR>
MBYTE  		EQU	<BYTE>
MWORD	  	EQU	<WORD>
MDWORD 		EQU	<DWORD>
MPUBLIC		EQU	<PUBLIC>

MLABEL		MACRO	name, type, publik
		IFNB	<publik>
                
                PUBLIC	name
		ENDIF
name		LABEL	type			
		ENDM


;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;                           OPERATIONAL MACROS                         ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

;----------------------------------------------------------------------;
; PUSHREG - pushes and arbitrary number of registers onto the stack.   ;
;----------------------------------------------------------------------;
PUSHREG		MACRO	r1, r2, r3, r4, r5, r6, r7, r8, r9, r10 
		IFNB	<r1>
		push	r1
		PUSHREG	r2, r3, r4, r5, r6, r7, r8, r9, r10
		ENDIF
		ENDM


;----------------------------------------------------------------------;
; POPREG - pops and arbitrary number of registers onto the stack.      ;
;----------------------------------------------------------------------;
POPREG		MACRO	r1, r2, r3, r4, r5, r6, r7, r8, r9, r10 
		IFNB	<r1>
		POPREG	r2, r3, r4, r5, r6, r7, r8, r9, r10
		pop	r1
		ENDIF
		ENDM


;----------------------------------------------------------------------;
; STKFRAME - sets up the BP register to look at a standard stack frame ;
;            from which all standard stack frame structures may be     ;
;            referenced off of.                                        ;
;----------------------------------------------------------------------;
STKFRAME	MACRO
		push	bp		; Save old BP register value
		mov	bp, sp		; Make BP stack frame pointer
		ENDM


;----------------------------------------------------------------------;
; STKFRMEND - marks the end of the use of a stack frame by restoring   ;
;             the old BP register value from the stack.                ;
;----------------------------------------------------------------------;
STKFRMEND	MACRO
		pop	bp		; Restore old BP register value
		ENDM
