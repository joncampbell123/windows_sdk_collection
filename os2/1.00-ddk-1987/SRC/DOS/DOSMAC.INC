;
; Macro file for MSDOS.
;
;	SCCSID = @(#)dosmac.inc	8.10 87/03/25

;
; <<==++ Copyright (c) Microsoft Corporation  1986 ++==>>
;

;

TRUE	EQU 0FFFFh
FALSE	EQU 0

;	Check if all (or most) the checking flags are to be enabled.

IFDEF	ALLSTRICT
	MISCSTRICT	=	1
	VMSTRICT	=	1
	DEVSTRICT	=	1
	VOLPTRCHK	=	1
	DEVSTRICT	=	1
	INTERRPRT	=	1
ENDIF

;*  Values for ?farcode, set by FARCODE macro to indicate which
;   code segment is being assembled.

FC_DosCode	EQU	0		; DosCode
FC_InitCode	EQU	1		; DosInitCode
FC_HighCode	EQU	2		; DosHighCode
FC_High2Code	EQU	3		; DosHigh2Code
FC_Ring3Code	EQU	4		; DosRing3Code
FC_InitRMCode	EQU	5		; DosInitRMCode

SUBTTL BREAK a listing into pages and give new subtitles
PAGE
BREAK	MACRO	subtitle
	SUBTTL	subtitle
	PAGE
ENDM
.xcref	BREAK

BREAK	<ASMVAR - handle assembly variables once and for all>

AsmVars Macro	varlist
IRP var,<varlist>
AsmVar	var
ENDM
ENDM

AsmVar	Macro	var
IFNDEF	var
var = FALSE
ENDIF
ENDM

;
; call a procedure that may be external.  The call will be short.
;
invoke	MACRO	name
.xcref
IF2
IFNDEF name
	EXTRN	name:NEAR
ENDIF
ENDIF
.cref
	CALL	name
ENDM
.xcref	invoke


PAGE
;
; jump to a label that may be external.  The jump will be near.
;
transfer    MACRO   name
.xcref
IF2
IFNDEF name
	EXTRN	name:NEAR
ENDIF
ENDIF
.cref
	JUMP	name
ENDM
.xcref	transfer


;
; get a short code address in a word
;

CSOFFSET EQU OFFSET DosCode:

short_addr  MACRO   name
IFDIF	<name>,<?>
.xcref
IF2
IFNDEF name
	EXTRN	name:NEAR
ENDIF
ENDIF
.cref
	DW  CSOFFSET name
ELSE
	DW  ?
ENDIF
ENDM
.xcref	short_addr


;
; get a long address in a dword
;
long_addr   MACRO   name
.xcref
IF2
IFNDEF name
	EXTRN	name:NEAR
ENDIF
ENDIF
.cref
	DD  name
ENDM
.xcref	long_addr

;	Declare a Procedure:
;	Parameters:
;	  name: procedure_name
;	  distance: near,far,hybrid,faronly
;	  scope: local,farlocal,nearlocal
;	  treg: scratch register
;		If non-blank, this register is used to optimizize far calls
;		to hybrid procedures, at the expense of near calls.
;		By default, near calls are the more efficient of the two.
;	  abase: the number of bytes to skip for calculating the ArgVar
;		 offsets. The default (if this is blank) depends on the
;		 "distance" variable. "NEAR" is 4 and "FAR" is 6.

.xcref	?frame
.xcref	?aframe
.xcref	?abase
.xcref	?stackdepth
.xcref	?initstack
?frame	    =  0	; initial
?aframe     =  0	; initial
?abase	    =  0	; initial
?stackdepth =  0	; initial stack size
?initstack  =  0	; initial stack size
?local	    =  0	; local proc flag
?near	    =  0	; near proc flag
?faronly    =  0	; faronly proc flag
?oldfaronly =  0	; outer scope faronly proc flag
?depth	    =  0	; procedure nesting level

Procedure macro   name,distance,scope,treg,abase
    if2
	if ?depth gt 1
	    %out Nesting too deep in Procedure name
	    .err
	endif
    endif
    ?depth	= ?depth + 1
    ?oldfaronly = ?faronly	;; save previous value
    ?frame	= 0
    ?aframe	= 0
    ?faronly	= 0		;; default to near/hybrid
    ?near	= 1		;; default to near
    ?initstack	= ?stackdepth	;; beginning of procedure
    ?local	= 0		;; default to public
    ifnb <scope>
	?local = 1		;; local if non-blank scope
	if2
	    ?x = 0
	    irp x,<LOCAL,local,farlocal,nearlocal>
		ifidn <x>,<scope>
		    ?x = 1
		endif
	    endm
	    ife ?x
		%out Bad scope_arg scope in Procedure name
		;;.err
	    endif
	endif
    endif
    ifnb <distance>		;; if nonblank, is hybrid, near, faronly or far
	if2
	    ?x = 0
	    irp x,<HYBRID,hybrid,NEAR,near,FAR,far,FARONLY,faronly>
		ifidn <x>,<distance>
		    ?x = 1
		endif
	    endm
	    ife ?x
		%out Bad distance_arg distance in Procedure name
		;;.err
	    endif
	endif
	irp x,<HYBRID,hybrid>
	    ?local = 0
	    ifidn <scope>,<local>
		?local = 1			;; local if scope is 'local'
	    endif
	    ifidn <scope>,<LOCAL>
		?local = 1			;; local if scope is 'LOCAL'
	    endif
	    ifidn <distance>,<x>
		ifb <treg>
		    ifidn <scope>,<nearlocal>
			GENHYBRID name		;; generate far public thunk
			?local = 1		;; but near label is local
		    else
			GENHYBRID name,<scope>	;; generate far thunk
		    endif
		else
		    ifidn <scope>,<farlocal>
			Entry name		;; generate near public label
			?local = 1		;; but far label is local
		    else
			Entry name,,<scope>
		    endif
		    pop  treg
		    push cs
		    push treg
		    ?faronly = 1	;; pretend to be faronly
		    ?near    = 0	;; not near
		endif
	    endif
	endm
	irp x,<FAR,far>
	    ifidn <distance>,<x>
		?near = 0		;; is far
	    endif
	endm
	irp x,<FARONLY,faronly>
	    ifidn <distance>,<x>
		?faronly = 1	;; is faronly
		?near	 = 0	;; not near
	    endif
	endm
    endif
ifdef NOLOCAL			;; if all symbols should be public
    ?local = 0
endif
    ife ?local
	if ?faronly
	    public hy_&name
	else
	    public name
	endif
    endif
    if ?near
	?abase = 2+2		;; remember the pushed BP
	name proc near		;; is near or hybrid
    else
	?abase = 4+2		;; remember the pushed BP
	if ?faronly
	    hy_&name proc far	;; is faronly
	else
	    name proc distance	;; else must be far
	endif
    endif
    ifnb <abase>		;; if abase is not blank, then use it
	?abase = abase
    endif
endm
.xcref	procedure

;
; end a procedure and check that stack depth is preserved
;
EndProc macro	name, chk
    ifdif <chk>,<NoCheck>	;; check the stack size
	if2
	    if ?initstack ne ?stackdepth ;; is it different?
		%out ***** Possible stack size error in name *****
	    endif
	endif
    endif
    if ?faronly
	hy_&name endp		;; is faronly
    else
	name endp			;; else near, far or hybrid
    endif
    ife ?depth
	if2
	    %out EndProc without matching Procedure name
	    .err
	endif
    else
	?depth = ?depth - 1
	?faronly = ?oldfaronly	;; restore previous value
    endif
endm
.xcref	EndProc


;**	EXTRNFAR - generate an EXTRN for the hybrid symbol if it
;	has not yet been defined, and the skip argument is blank.

EXTRNFAR macro	name,skip
ifb <skip>
    ifndef hy_&name
	extrn	hy_&name:far
    endif
endif
endm


; generate a far pointer to a hybrid procedure

DDFAR	macro name
	dd	hy_&name
endm


; move the offset of a hybrid procedure into a register or memory

MOVFAROFFSET macro target,name,group
ifb <group>
	mov	target,offset cs:hy_&name
else
	mov	target,offset group:hy_&name
endif
endm


; generate an intersegment call or jmp to a hybrid procedure

CALLFAR macro	name
	call	far ptr hy_&name
endm

JMPFAR	macro	name
	jmp	far ptr hy_&name
endm


; generate a hybrid procedure linkage procedure

GENHYBRID macro name,local
ifb <local>			;; if local symbol not requested
	public	hy_&name
endif
ifdef NOLOCAL			;; or if all symbols should be public
	public	hy_&name
endif
hy_&name proc far
	call	name
	ret
hy_&name endp
endm


; generate a near/far/hybrid entry into a procedure

Entry macro name,distance,scope
local a
a = 0				;; assume near label
ifnb <scope>
    if2
	?x = 0
	irp x,<local,LOCAL>
	    ifidn <x>,<scope>
		?x = 1
	    endif
	endm
	ife ?x
	    %out Bad scope_arg scope in Entry name
	    ;;.err
	endif
    endif
endif
ifnb <distance>
    if2
	?x = 0
	irp x,<faronly,hybrid,HYBRID>
	    ifidn <x>,<distance>
		?x = 1
	    endif
	endm
	ife ?x
	    %out Bad distance_arg distance in Entry name
	    ;;.err
	endif
    endif
    ifidn <distance>,<faronly>	;; was it far only?
	a = 1			;; then generate far label
    else			;; else must be hybrid
	jmp short name		;; previous callers skip hybrid linkage
	GENHYBRID name,<scope>
    endif
endif
if a				;; generate far label?
    ifb <scope> 		;; if local symbol not requested
	public hy_&name
    endif
    ifdef NOLOCAL		;; or if all symbols should be public
	public	hy_&name
    endif
    hy_&name:
else				;; else generate near label
    ifb <scope> 		;; if local symbol not requested
	public name
    endif
    ifdef NOLOCAL		;; or if all symbols should be public
	public	name
    endif
    name:
endif
endm
.xcref	Entry

BREAK <ERROR - store an error code then jump to a label>

error macro code
.xcref
	mov	al,code
	transfer Sys_Ret_Err_AL
.cref
ENDM
.xcref	error

BREAK <JUMP - real jump that links up shortwise>
;
; given a label <lbl> either 2 byte jump to another label <lbl>_J
; if it is near enough or 3 byte jump to <lbl>
;

jump	macro lbl
	local a
.xcref

	ifndef lbl&_J		;; is this the first invocation
a:  JMP lbl
	ELSE
	IF (lbl&_J GE $) OR ($-lbl&_J GT 126)
a:  JMP lbl		;; is the jump too far away?
	ELSE
a:  JMP lbl&_J		;; do the short one...
	ENDIF
	ENDIF
	lbl&_J = a
.cref
endm
.xcref	jump

BREAK <RETURN - return from a function>

return	macro x
	local a
.xcref
a:
	RET
ret_l = a
.cref
endm
.xcref	return

BREAK <CONDRET - conditional return>

condret macro	cc,ncc
	local	a
.xcref
.xcref a
.cref
	ifdef	ret_l		;; if ret_l is defined
	if (($ - ret_l) le 126) and ($ gt ret_l)
					;;     if ret_l is near enough then
		a:  j&cc    ret_l	;;     a: j<CC> to ret_l
		ret_&cc = a	    ;;	   define ret_<CC> to be a:
		exitm
	endif
	endif
	ifdef	ret_&cc 	;; if ret_<CC> defined
	if (($ - ret_&cc) le 126) and ($ gt ret_&cc)
					;;     if ret_<CC> is near enough
		a:  j&cc    ret_&cc	;;     a: j<CC> to ret_<CC>
		ret_&cc = a	    ;;	   define ret_<CC> to be a:
		exitm
	endif
	endif
	j&ncc	a		;; j<NCC> a:
	return		    ;; return
	a:		    ;; a:
	ret_&cc = ret_l 	;; define ret_<CC> to be ret_l
endm
.xcref	condret

BREAK <RETZ - return if zero, links up shortwise if necessary>

retz	macro
	condret z,nz
endm
.xcref	retz

BREAK <RETNZ - return if not zero, links up shortwise if necessary>

retnz	macro
	condret nz,z
endm
.xcref	retnz

BREAK <RETC - return if carry set, links up shortwise if necessary>

retc	macro
	condret c,nc
endm
.xcref	retc

BREAK <RETNC - return if not carry, links up shortwise if necessary>

retnc	macro
	condret nc,c
endm
.xcref	retnc

BREAK <CONTEXT - set the DOS context to a particular register>

Dos3Mac MACRO name
IF2
	%out	Warning - DOS 3 macro name called
ENDIF
ENDM


SegContext macro    srl,gr,noassume,base,grpnam
IFB <gr>			;; push and pop if no general register
    IRP sr,<srl>		;; for each register
	PUSH	base		;; push base
	POP	sr		;; pop into the register
	IFB <noassume>		;; if ASSUME not inhibited
	    ASSUME	sr:grpnam
	ENDIF
    ENDM
ELSE				;; else if we have a helper register
    MOV     gr,base		;; load helper with base
    IRP sr,<srl>		;; for each register in list
	MOV	sr,gr		;; load the register from the helper
	IFB <noassume>		;; if ASSUME not inhibited
	    ASSUME	sr:grpnam
	ENDIF
    ENDM
ENDIF
endm
.xcref	SegContext


DosContext macro    srl,gr,noassume
LOCAL i
    IFNB <gr>				;; if intermediate general register
	mov	gr,DosGroup
	IRP sr,<srl>
	    mov     sr,gr
	ENDM
    ELSE
	IRP sr,<srl>			;; for sr = *first* segreg in list
	    i = 0
	    IRP r,<srl> 		;;  for r = each segreg in list
		i = i + 1
		IF i EQ 1		;; if r is first in list
		    push    DosGroup	;; do push immediate
		ELSE			;; else r is second (or later) in list
		    push    sr		;; and sr contains desired value
		ENDIF
		pop	r
	    ENDM
	    EXITM			;; quit after sr = first in list
	ENDM
    ENDIF
    IFB <noassume>
	IRP sr,<srl>
	    ASSUME  sr:DosGroup
	ENDM
    ENDIF
endm
.xcref	DosContext


DosHighContext macro   srl,gr,noassume
	SegContext   <srl>,gr,noassume,DosHighData,DosHighData
endm
.xcref	DosHighContext


TaskContext macro   srl,gr,noassume
	SegContext   <srl>,gr,noassume,SS,TaskArea
endm
.xcref	TaskContext


DosInitContext macro   srl,gr,noassume
	SegContext   <srl>,gr,noassume,DosInitData,DosInitData
endm
.xcref	DosInitContext


BREAK <SaveReg/RestoreReg - save or restore a set of registers>

SaveReg MACRO	reglist 	;; push those registers
IRP reg,<reglist>
	?stackdepth = ?stackdepth + 1
	PUSH	reg
ENDM
ENDM
.xcref	SaveReg

RestoreReg  MACRO   reglist	;; pop those registers
IRP reg,<reglist>
	?stackdepth = ?stackdepth - 1
	POP reg
ENDM
ENDM
.xcref	RestoreReg

BREAK <Critical section macros>

ifdef INTERRPRT
	IntErrF = 0ffffh
endif

AsmVars <Debug,IntErrF,ShareF,Redirector>

BREAK	<DOSAssume - validate assumes>

DOSAssume   Macro   reg,reglist,message
local a,b
IFIDN <reg>,<CS>
	$temp	=   1
ELSE
	IFIDN <reg>,<SS>
		$temp	=   0
	ELSE
		IFIDN <reg>,<DOS>
			IF Redirector
				$temp = 8
			ELSE
				$temp = 1
			ENDIF
		ELSE
			%out ***** Invalid DOS register reg in DOSAssume *****
		ENDIF
	ENDIF
ENDIF
IF Debug
	IRP r,<reglist>
		IFIDN <r>,<DS>
			$temp = $temp OR 2
		ELSE
			IFIDN <r>,<ES>
				$temp = $temp OR 4
			ELSE
				%out ***** Invalid register r in DOSAssume *****
			ENDIF
		ENDIF
	ENDM
	PUSH	AX
	MOV AX,$temp
	PUSH	AX
IF ShareF OR Redirector
	MOV AX,OFFSET b
ELSE
	MOV AX,OFFSET DosGroup:b
ENDIF
	PUSH	AX
	Invoke	SegCheck
	POP AX
IF ShareF OR Redirector
	JMP SHORT a
b   DB	message,0
a:
ELSE
Table segment
b   DB	message,0
Table ends
ENDIF
ENDIF
IRP r,<reglist>
	IF $temp AND 1
		IF Redirector
			ASSUME	r:NETWRK
		ELSE
			ASSUME	r:DosGroup
		ENDIF
	ELSE
		IF $temp AND 8
			ASSUME	r:DosGroup
		ELSE
			ASSUME	r:TaskArea
		ENDIF
	ENDIF
ENDM
ENDM

BREAK	<ASSERT - make assertions about registers>
;*  Assert - sanity checks (contolled by DEBUG switch)
;
;   kind:   one of Z, NZ, ISBUF, ISCURBUF, ISSFT, ISVPB (case is
;	    significant)
;   objs:   register pair which should point to given structure
;	    for IS???; an expression for Z or NZ
;   message: message printed if assertion fails; should identify
;	    the calling routine

IF Debug
Assert	MACRO	kind, objs, message
	LOCAL	a,b
	IFIDN	<kind>,<Z>
	CMP objs,0
	JZ  a
	DBPRT	<>,<>,<message>
a:
	ELSE
	IFIDN	<kind>,<NZ>
	CMP objs,0
	JNZ a
	DBPRT	<>,<>,<message>
a:
	ELSE
		IFIDN	<kind>,<ISDPB>
%out ISDPB referenced... Needs to be examined (someday) and changed to ISVPB
	EXITM
		ENDIF
	PUSH	AX
		IRP obj,<objs>
	PUSH	obj
		ENDM
		IF ShareF
	MOV	AX,OFFSET b
		ELSE
	MOV	AX,OFFSET DosGroup:b
		ENDIF
	PUSH	AX
		IFIDN	<kind>,<ISBUF>
	Invoke	BUFCheck
		ENDIF
		IFIDN	<kind>,<ISCURBUF>
	Invoke	CURBUFCheck
		ENDIF
		IFIDN	<kind>,<ISSFT>
	Invoke	SFTCheck
		ENDIF
		IFIDN	<kind>,<ISVPB>
	Invoke	xVPBCheck
		ENDIF
	POP	AX
		IF ShareF
	JMP SHORT a
b   DB	message,0
a:
		ELSE
Table	segment
b   DB	message,0
Table	ends
		ENDIF
	ENDIF
	ENDIF
ENDM
ELSE
Assert	Macro
ENDM
ENDIF

BREAK	<CallInstall - hook to installable pieces>

CallInstall MACRO   name,save,restore
	IFNB	<save>
		SaveReg <save>
	ENDIF

	push	name			;; index of routine to call
	call	Redir_Dispatcher	;; go dispatch to redirector

	ifnb	<restore>
		RestoreReg  <restore>
	endif
ENDM

;   The Redir_Dispatcher routine is common low code and will
;   merely perform an indirect call through the redirector-supplied
;   address


BREAK	<CallPCache - hook to installable PathCacher, distinct from redir.>

CallPCache MACRO name

	push	name
	call	PathCache_Dispatcher

	endm

;   The PathCache_Dispatcher is a jump-stub, which must be called far
;   because it may had off to the PathCacher, which resides in a separate
;   segment when installed.  When the PathCacher is not installed, all
;   that happens is a clear-carry and a far ret.


BREAK <LJcc - Long Conditional Jumps>

LJE macro l
 LJ JE JNE l
endm

LJNE macro l
 LJ jne JE l
endm

LJZ macro l
 LJE l
endm

LJNZ macro l
 LJNE l
endm

LJC macro l
 LJ jc JNC l
endm

LJNC macro l
 LJ jnc JC l
endm

LJA macro l
 LJ ja JNA l
endm

LJNA macro l
 LJ jna JA l
endm

LJB macro l
 LJ jb JNB l
endm

LJNB macro l
 LJ jnb JB l
endm

LJS macro l
 LJ js JNS l
endm

LJNS macro l
 LJ jns JS l
endm

LJAE macro l
 LJ jae JNAE l
endm

LJBE macro l
 LJ jbe JNBE l
endm

LJL macro l
 LJ jl JNL l
endm

LJG macro l
 LJ jg JNG l
endm

LJLE macro l
 LJ jle JNLE l
endm

DLJE macro l
 DLJ JE JNE l
endm

DLJNE macro l
 DLJ jne JE l
endm

DLJZ macro l
 DLJE l
endm

DLJNZ macro l
 DLJNE l
endm

DLJC macro l
 DLJ jc JNC l
endm

DLJNC macro l
 DLJ jnc JC l
endm

DLJA macro l
 DLJ ja JNA l
endm

DLJNA macro l
 DLJ jna JA l
endm

DLJB macro l
 DLJ jb JNB l
endm

DLJNB macro l
 DLJ jnb JB l
endm

DLJS macro l
 DLJ js JNS l
endm

DLJNS macro l
 DLJ jns JS l
endm

DLJAE macro l
 DLJ jae JNAE l
endm

DLJBE macro l
 DLJ jbe JNBE l
endm

DLJG macro l
 DLJ jg JNG l
endm

DLJL macro l
 DLJ jl JNL l
endm

DLJLE macro l
 DLJ jle JNLE l
endm


;*	LJ - generate long conditional jump
;
;	if target preceeds us and is in range just use a short jump
;	else use a long jump
;
;	LJ <direct jmp>,<skip jmp>,<label>

LJ MACRO dirop,idirop,l
 local a
 IF ((.TYPE l) XOR 20h) AND 0A0h
  idirop a			       ;; not defined or is external
  jmp l
a:
 ELSE				       ;; is local definied
  IF (($-l) LT 124) AND ($ GT l)
   dirop l			      ;; is before and within range
  ELSE
   idirop a			      ;; is out of range or forward (pass 2)
   jmp l
a:
  ENDIF
 ENDIF
ENDM


;*	DLJ - generate debug long conditional jump
;
;	If DEBUG is defined then we generate a long jump, else a short
;	one.
;
;	DLJ <direct jmp>,<skip jmp>,<label>

DLJ MACRO dirop,idirop,l
 local a
 IF Debug
  idirop a
  jmp l
a:
 ELSE
  dirop l
 ENDIF
ENDM


;*	DLJMP - generate debug long jump
;
;	If Debug is defined then generate long jump, else a short one.
;
;	DLJMP <label>

DLJMP macro lbl
 IF Debug
  jump lbl
 ELSE
  jmp short lbl
 ENDIF
ENDM


.xcref	LJE, LJNE, LJZ, LJNZ, LJC, LJNC, LJA, LJNA
.xcref	LJB, LJNB, LJS, LJNS, LJAE, LJBE, LJG, LJL, LJLE
.xcref	DLJE, DLJNE, DLJZ, DLJNZ, DLJC, DLJNC, DLJA, DLJNA
.xcref	DLJB, DLJNB, DLJS, DLJNS, DLJAE, DLJBE, DLJG, DLJL, DLJLE
.xcref	LJ,DLJ


BREAK <NEW MACROS FOR 4.0>

ReferTask MACRO sym,len
 TaskArea segment

 IFIDN <len>,<WORD>
 EXTRN &sym:WORD
 ELSE
 IFIDN <len>,<DWORD>
 EXTRN &sym:DWORD
 ELSE
 EXTRN &sym:BYTE
 ENDIF
 ENDIF

 TaskArea ENDS
ENDM

ReferInitGlobal MACRO sym,len
 InitData segment

 IFIDN <len>,<WORD>
 EXTRN &sym:WORD
 ELSE
 IFIDN <len>,<DWORD>
 EXTRN &sym:DWORD
 ELSE
 EXTRN &sym:BYTE
 ENDIF
 ENDIF

 InitData ENDS
ENDM


ReferGlobal MACRO sym,len
 CONSTANTS SEGMENT

 IFIDN <len>,<WORD>
 EXTRN &sym:WORD
 ELSE
 IFIDN <len>,<DWORD>
 EXTRN &sym:DWORD
 ELSE
 IFIDN <len>,<QWORD>
 EXTRN &sym:QWORD
 ELSE
 EXTRN &sym:BYTE
 ENDIF
 ENDIF
 ENDIF

 CONSTANTS ENDS
ENDM



ReferHighGlobal MACRO sym,symtype
HIGHDATA SEGMENT

EXTRN &sym:&symtype

HIGHDATA ENDS
ENDM

	?farcode = FC_DosCode	; default to near code

;**	NEARCODE/FARCODE macros - used to control model of calls
;	used in macros (InternalError, DBPRT, PMONLY, etc.).
;	These macros should be invoked just *prior* to every code
;	segment declaration, and OUTSIDE of all segments.

NEARCODE macro
	?farcode = FC_DosCode	; near code segment
	assume cs:DosCode
endm

FARCODE macro	segname,NoIntErr,NoProtReal,NoDPRINTF
	EXTRNFAR IntErr,<NoIntErr>
	EXTRNFAR IntErrNull,<NoIntErr>
ifdef MODECHECK
	EXTRNFAR TaskOnly,<NoProtReal>
	EXTRNFAR ProtTaskOnly,<NoProtReal>
	EXTRNFAR ProtOnly,<NoProtReal>
	EXTRNFAR RealTaskOnly,<NoProtReal>
	EXTRNFAR RealOnly,<NoProtReal>
	EXTRNFAR RealProcTaskOnly,<NoProtReal>
endif
if Debug
	EXTRNFAR DPRINTF,<NoDPRINTF>
	EXTRNFAR dbtest,<NoDPRINTF>
endif
ifidn	<segname>,<INIT>
	assume cs:DosInitCode
	?farcode = FC_InitCode	    ; Indicate which far code segment
else
ifidn	<segname>,<HIGH>
	assume cs:DosHighCode
	?farcode = FC_HighCode	    ; Indicate which far code segment
else
ifidn	<segname>,<HIGH2>
	assume cs:DosHigh2Code
	?farcode = FC_High2Code     ; Indicate which far code segment
else
ifidn	<segname>,<RING3>
	assume cs:DosRing3Code
	?farcode = FC_Ring3Code     ; Indicate which far code segment
else
ifidn	<segname>,<INITRM>
	assume cs:DosInitRMCode
	?farcode = FC_InitRMCode    ; Indicate which far code segment
else
if2
	%out FARCODE: bad segment name: segname
	.ERR
endif
endif
endif
endif
endif
endif
endm


;**	GENCALL - generate a call of the appropriate model
GENCALL macro	target
if ?farcode
	CALLFAR target
else
	invoke	target
endif
endm


;**	Internal error detected in system
;	Also used by the bios!!

ifndef DataOffset			;; if not defined by bios
DataOffset equ	offset DosGroup:	;; use DosGroup
endif

InternalError	macro	arg,lab
local a
if IntErrF or Debug
	push	DataOffset a
	GENCALL IntErr
else
	GENCALL IntErrNull		;; no string passed
endif
ifdef	?iecnt				;; if ModName macro was invoked
	IntErrLabel %?iecnt,lab 	;; generate a public label
	?iecnt = ?iecnt + 1		;; and bump sequential label number
endif
if IntErrF or Debug
IntErrMsg segment
a	db	'&arg',0dh,0ah,'$'
IntErrMsg ends
endif
ENDM

ModName macro	mn
    ?iecnt = 0
    IntErrLabel macro n,lab
	ifnb <lab>
	    public ie_&mn&&n&&_&&lab
	ie_&mn&&n&&_&&lab:
	else
	    public ie_&mn&&n
	ie_&mn&&n:
	endif
    endm
endm


;**	User error detected in system

UserError   MACRO   arg
local a
	mov	ax,offset DosGroup:a
	GENCALL UsrErr
Table segment
a	db	'&arg',0dh,0ah,'$'
Table ends
ENDM


BREAK	<Stack frame manipulators>

LocalVar macro	name,length,lowname,highname,pad
local a
    ?pad = 0
    ?nopad = 0
    ifidn <pad>,<PAD>
	?pad = 1
    endif
    ifidn <pad>,<NOPAD>
	?nopad = 1
    endif

    ifidn <length>,<BYTE>
	?frame =  ?frame + 1
	a = ?frame
	name EQU  (BYTE PTR [BP-a])
    else
    ifidn <length>,<DBYTE>
	if ?frame and 1
	    ?frame = ?frame + 1
	endif
	?frame =  ?frame + 2
	a = ?frame
	name EQU  (WORD PTR [BP-a])
	ifb <lowname>
	    name&l EQU	(BYTE PTR [BP-a])
	else
	    lowname EQU  (BYTE PTR [BP-a])
	endif
	ifb <highname>
	    name&h EQU	(BYTE PTR [BP-a+1])
	else
	    highname EQU  (BYTE PTR [BP-a+1])
	endif
    else
    ifidn <length>,<WORD>
	ife ?nopad
	    if ?frame and 1
		?frame = ?frame + 1
	    endif
	endif
	?frame =  ?frame + 2
	a = ?frame
	name EQU  (WORD PTR [BP-a])
    else
    ifidn <length>,<DWORD>
	if ?frame and 1
	    ?frame = ?frame + 1
	endif
	?frame =  ?frame + 4
	a = ?frame
	name EQU  (DWORD PTR [BP-a])
	name&l EQU  (WORD PTR [BP-a])
	name&h EQU  (WORD PTR [BP-a+2])
    else
	?frame =  ?frame + length
	if ?pad
	    if ?frame and 1
		?frame = ?frame + 1
	    endif
	endif
	a = ?frame
	name EQU  (BYTE PTR [BP-a])
    endif	;; DWORD
    endif	;; WORD
    endif	;; DBYTE
    endif	;; BYTE
endm

EnterProc   macro varlist
local a
    if ?frame and 1
	?frame = ?frame + 1		;; make final frame even
    endif
    if ?frame
	ifb <varlist>			;; if no LocalVars initialized
	    enter ?frame,0		;; all done
	else
	    push  bp			;; else set up bp frame
	    mov   bp,sp
	    a = ?frame
	    irp var,<varlist>		;; and push each value
		if2
		    ife a
			%out EnterProc: too many args
			.err
		    endif
		endif
		push  var
		a = a - 2
	    endm
	    if a			;; if any left over, adjust sp
		if a gt 4
		    sub  sp,a		;; takes 3 bytes
		else
		    push  ax
		    if a eq 4
			push  ax	;; if a==4, takes 2 bytes
		    endif
		endif
	    endif
	endif
    else
	push bp
	mov  bp,sp
    endif
endm

LeaveProc   macro
    leave
endm

ArgVar	macro	name,length,lowname,highname
local a
    a = ?aframe + ?abase
    ifidn <length>,<BYTE>
	?aframe =  ?aframe + 1
	name EQU  (BYTE PTR [BP+a])
    else
    ifidn <length>,<DBYTE>
	?aframe =  ?aframe + 2
	name EQU  (WORD PTR [BP+a])
	ifb <lowname>
	    name&l EQU	(BYTE PTR [BP+a])
	else
	    lowname EQU  (BYTE PTR [BP+a])
	endif
	ifb <highname>
	    name&h EQU	(BYTE PTR [BP+a+1])
	else
	    highname EQU  (BYTE PTR [BP+a+1])
	endif
    else
    ifidn <length>,<WORD>
	?aframe =  ?aframe + 2
	name EQU  (WORD PTR [BP+a])
    else
    ifidn <length>,<DWORD>
	?aframe =  ?aframe + 4
	name EQU  (DWORD PTR [BP+a])
	name&l EQU  (WORD PTR [BP+a])
	name&h EQU  (WORD PTR [BP+a+2])
    else
	?aframe =  ?aframe + length
	name EQU  (BYTE PTR [BP+a])
    endif	;; DWORD
    endif	;; WORD
    endif	;; DBYTE
    endif	;; BYTE
endm

BREAK	<ERRNZ - Conditional Error>


;** ERRNZ - generate assembly error if arg != 0
;

ERRNZ	MACRO	expr
.errnz	expr
ENDM



;*  POPFF - special version of pop flags for i286
;
;   This avoids the pitfalls of the 286 POPF instruction
;   which will sometimes allow an interrupt to come in
;   even if IF is off in both the old and new flags.

POPFF	macro
local	a
	push	cs
if ?farcode
	push	offset cs:a
	iret
a:
else
	invoke	PopFlags
endif
endm


;**	RMONLY	- verify that the processor is in real mode
RMONLY	macro	task
	RMPM	Real,<task>
endm


;**	PMONLY	- verify that the processor is in protected mode
PMONLY	macro	task
	RMPM	Prot,<task>
endm


;**	DUALMODE - document that the processor may be in either mode
DUALMODE macro	task
	RMPM	<>,<task>
endm


;**	RMPROCONLY  - verify that the currently running process is the
;		      real mode process.  Presumed to be at task time.
RMPROCONLY	macro
	RMPM	RealProc,TASKTIME
endm


;**	RMPM - RMONLY/PMONLY support
RMPM	macro	rmpm,task
ifdef MODECHECK
ifnb <task>
    if2
	ifdif <task>,<TASKTIME>
	    %out RMONLY/PMONLY/DUALMODE/RMPROCONLY: bad macro arg: task
	    .ERR
	endif
    endif
    ifb <rmpm>
	GENCALL TaskOnly
    else
	GENCALL rmpm&TaskOnly
    endif
else
    ifnb <rmpm>
	GENCALL rmpm&Only
    endif
endif
endif
endm


;*  callgate - allow ring3 code in doscode call api
;
;   Kernel entry looks just like a normal api call

callgate MACRO	gate
ifndef GDT_&gate
	extrn	GDT_&gate:BYTE
endif
	db	9ah				; far ret
	dw	0,GSEL GDT_&gate
ENDM


;**	IODelay - delay long enough for the bus to catch up to the cpu
IODelay macro
	jmp	short $+2
endm
