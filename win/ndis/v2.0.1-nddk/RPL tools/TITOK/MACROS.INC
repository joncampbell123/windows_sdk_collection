;**********************************************************************;
;**********************************************************************;
;								       ;
; File Name:       MACROS.INC                    		       ;
;								       ;
; Program Name:    Token-Ring RIPL Boot ROM Device Driver              ;
;								       ;
; Companion Files: None						       ;
;								       ;
; Function:        This module contains all the general purpose macro  ;
;		   definitions for the macro used throughout the       ;
;		   TRNDD source code.  The macros defined include the  ;
;                  following:                                          ;     
;                                                                      ;     
;                - PAGE_BREAK: causes a page break and subtitle change ;     
;                - ptr_struct: structure for offset/segment separation ;     
;                                                                      ;     
; History:             						       ;
;								       ;
;     03/10/89  Mike Witkowski - Original Version 0.0		       ;
;     04/11/89  Mike Witkowski - Reworked for RIPL       	       ;
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
; IFCALL - calls adapter specific routines.                            ; 
;----------------------------------------------------------------------;
IFCALL		MACRO	prefix
                IFDEF   TYPE_3C603
                CALLIT  prefix, <3c603>
                ENDIF
                IFDEF   TYPE_M8110
                CALLIT  prefix, <m8110>
                ENDIF
                IFDEF   TYPE_P1345
                CALLIT  prefix, <p1345>
                ENDIF
                IFDEF   TYPE_WDTCPLUS
                CALLIT  prefix, <wdtcplus>
                ENDIF
		ENDM

CALLIT		MACRO	prefix, suffix
                call    prefix&suffix
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


;----------------------------------------------------------------------;
; OPTMOVE - moves data at the fastest rate possible using CPU type,    ;
;           data alignment, and move size as considerations.  OPTMOVE  ;
;           is used mainly in cases where data alignment is not known. ;
;	    Since this macro is used mainly in the receive data trans- ;
;           fer case, the macro default aligns the move on the source  ;
;           operand (si) since this has the greatest likelihood of     ;
;           being DWORD aligned in the 386.                            ;	
;----------------------------------------------------------------------;
OPTMOVE		MACRO	
		shr	cx, 1		  ; Make BYTE length WORD length
		rep	movsw		  ; Move WORDs
		rcl	cx, 1		  ; Get remaining byte count
		rep	movsb		  ; Move byte if there
		ENDM


;----------------------------------------------------------------------;
; SHORT JUMP MACROS						       ;
;----------------------------------------------------------------------;
SJE		MACRO	jump
		SJ	je, jump
		ENDM

SJNE		MACRO	jump
		SJ	jne, jump
		ENDM

SJZ		MACRO	jump
		SJ	jz, jump
		ENDM

SJNZ		MACRO	jump
		SJ	jnz, jump
		ENDM

SJC		MACRO	jump
		SJ	jc, jump
		ENDM

SJNC		MACRO	jump
		SJ	jnc, jump
		ENDM

SJB		MACRO	jump
		SJ	jb, jump
		ENDM

SJNB		MACRO	jump
		SJ	jnb, jump
		ENDM

SJA		MACRO	jump
		SJ	ja, jump
		ENDM

SJNA		MACRO	jump
		SJ	jna, jump
		ENDM

SJBE		MACRO	jump
		SJ	jbe, jump
		ENDM

SJNBE		MACRO	jump
		SJ	jnbe, jump
		ENDM

SJAE		MACRO	jump
		SJ	jae, jump
		ENDM

SJNAE		MACRO	jump
		SJ	jnae, jump
		ENDM

SJG		MACRO	jump
		SJ	jg, jump
		ENDM

SJNG		MACRO	jump
		SJ	jng, jump
		ENDM

SJGE		MACRO	jump
		SJ	jge, jump
		ENDM

SJNGE		MACRO	jump
		SJ	jnge, jump
		ENDM

SJL		MACRO	jump
		SJ	jl, jump
		ENDM

SJNL		MACRO	jump
		SJ	jnl, jump
		ENDM

SJLE		MACRO	jump
		SJ	jle, jump
		ENDM

SJNLE		MACRO	jump
		SJ	jnle, jump
		ENDM

SJS		MACRO	jump
		SJ	js, jump
		ENDM

SJNS		MACRO	jump
		SJ	jns, jump
		ENDM

SJO		MACRO	jump
		SJ	jo, jump
		ENDM

SJNO		MACRO	jump
		SJ	jno, jump
		ENDM

SJP		MACRO	jump
		SJ	jp, jump
		ENDM

SJPE		MACRO	jump
		SJ	jpe, jump
		ENDM

SJNP		MACRO	jump
		SJ	jnp, jump
		ENDM

SJPO		MACRO	jump
		SJ	jpo, jump
		ENDM

SJMP		MACRO	jump
		jmp  	SHORT jump
		ENDM



;----------------------------------------------------------------------;
; LONG JUMP MACROS						       ;
;----------------------------------------------------------------------;
LJE		MACRO	jump
		LJ	je, SJNE, jump
		ENDM

LJNE		MACRO	jump
		LJ	jne, SJE, jump
		ENDM

LJZ		MACRO	jump
		LJ	jz, SJNZ, jump
		ENDM

LJNZ		MACRO	jump
		LJ	jnz, SJZ, jump
		ENDM

LJC		MACRO	jump
		LJ	jc, SJNC, jump
		ENDM

LJNC		MACRO	jump
		LJ	jnc, SJC, jump
		ENDM

LJB		MACRO	jump
		LJ	jb, SJNB, jump
		ENDM

LJNB		MACRO	jump
		LJ	jnb, SJB, jump
		ENDM

LJA		MACRO	jump
		LJ	ja, SJNA, jump
		ENDM

LJNA		MACRO	jump
		LJ	jna, SJA, jump
		ENDM

LJBE		MACRO	jump
		LJ	jbe, SJNBE, jump
		ENDM

LJNBE		MACRO	jump
		LJ	jnbe, SJBE, jump
		ENDM

LJAE		MACRO	jump
		LJ	jae, SJNAE, jump
		ENDM

LJNAE		MACRO	jump
		LJ	jnae, SJAE, jump
		ENDM

LJG		MACRO	jump
		LJ	jg, SJNG, jump
		ENDM

LJNG		MACRO	jump
		LJ	jng, SJG, jump
		ENDM

LJGE		MACRO	jump
		LJ	jge, SJNGE, jump
		ENDM

LJNGE		MACRO	jump
		LJ	jnge, SJGE, jump
		ENDM

LJL		MACRO	jump
		LJ	jl, SJNL, jump
		ENDM

LJNL		MACRO	jump
		LJ	jnl, SJL, jump
		ENDM

LJLE		MACRO	jump
		LJ	jle, SJNLE, jump
		ENDM

LJNLE		MACRO	jump
		LJ	jnle, SJLE, jump
		ENDM

LJS		MACRO	jump
		LJ	js, SJNS, jump
		ENDM

LJNS		MACRO	jump
		LJ	jns, SJS, jump
		ENDM

LJO		MACRO	jump
		LJ	jo, SJNO, jump
		ENDM

LJNO		MACRO	jump
		LJ	jno, SJO, jump
		ENDM

LJP		MACRO	jump
		LJ	jp, SJNP, jump
		ENDM

LJPE		MACRO	jump
		LJ	jpe, SJPO, jump
		ENDM

LJNP		MACRO	jump
		LJ	jnp, SJP, jump
		ENDM

LJPO		MACRO	jump
		LJ	jpo, SJPE, jump
		ENDM

LJMP		MACRO	jump
		jmp  	jump
		ENDM


;----------------------------------------------------------------------;
; SJ -  Performs a SHORT jump if in mode386, else performs a normal    ;
;	conditional jump.					       ;
;----------------------------------------------------------------------;
SJ		MACRO	jtype, jump
		IFDEF mode386
		jtype	SHORT jump
		ELSE
		jtype	jump
		ENDIF
		ENDM


;----------------------------------------------------------------------;
; LJ -  Performs long jump.  In 386 mode, just perform conditional     ;
;       jump.  In non-386 mode, take the opposite conditional to a     ;
;       short local label and do a long jump to the actual label.      ;
;----------------------------------------------------------------------;
LJ		MACRO	jtype, ojtype, jump
		LOCAL	nlabel
		IFDEF	mode386
		jtype	jump
		ELSE
		ojtype	nlabel
		jmp	jump
nlabel: 
		ENDIF
		ENDM
