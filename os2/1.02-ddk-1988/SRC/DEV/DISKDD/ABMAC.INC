;
; Copyright (c) Microsoft Corporation,  1988
;
BREAK	macro	subtitle
	subttl	subtitle
	page
	endm
.xcref	BREAK

ERRNZ	macro	expr
	.errnz	expr
	endm
.xcref	ERRNZ

SaveReg macro	reglist
	IRP reg,<reglist>
	   push    reg
	endm
	endm
.xcref	SaveReg

RestoreReg  macro   reglist
	    IRP reg,<reglist>
	       pop reg
	    endm
	    endm
.xcref	RestoreReg

return	macro x
	local a
a:
	ret
	ret_l = a
	endm
.xcref	return

condret macro	cc,ncc
	local	a
	ifdef	ret_l			;; if ret_l is defined
	if (($ - ret_l) le 126) and ($ gt ret_l)
		a:  j&cc    ret_l	;; a: j<CC> to ret_l
		ret_&cc = a		;; define ret_<CC> to be a:
		exitm
	endif
	endif
	ifdef	ret_&cc 		;; if ret_<CC> defined
	if (($ - ret_&cc) le 126) and ($ gt ret_&cc)
		a:  j&cc    ret_&cc	;;     a: j<CC> to ret_<CC>
		ret_&cc = a		;; define ret_<CC> to be a:
		exitm
	endif
	endif
	j&ncc	a			;; j<NCC> a:
	return				;; return
	a:				;; a:
	ret_&cc = ret_l 		;; define ret_<CC> to be ret_l
endm
.xcref	condret

RETZ	macro
	condret z,nz
	endm
.xcref	retz

RETNZ	macro
	condret nz,z
	endm
.xcref	retnz

RETC	macro
	condret c,nc
	endm
.xcref	retc

RETNC	macro
	condret nc,c
	endm
.xcref	retnc

LJE	macro l
	LJ JE JNE l
	endm

LJNE	macro l
	LJ jne JE l
	endm

LJZ	macro l
	LJE l
	endm

LJNZ	macro l
	LJNE l
	endm

LJC	macro l
	LJ jc JNC l
	endm

LJNC	macro l
	LJ jnc JC l
	endm

LJA	macro l
	LJ ja JNA l
	endm

LJNA	macro l
	LJ jna JA l
	endm

LJB	macro l
	LJ jb JNB l
	endm

LJNB	macro l
	LJ jnb JB l
	endm

LJS	macro l
	LJ js JNS l
	endm

LJNS	macro l
	LJ jns JS l
	endm

LJAE	macro l
	LJ jae JNAE l
	endm

LJBE	macro l
	LJ jbe JNBE l
	endm

LJL	macro l
	LJ jl JNL l
	endm

LJG	macro l
	LJ jg JNG l
	endm

LJLE	macro l
	LJ jle JNLE l
	endm

;;*	 LJ - generate long conditional jump
;;
;;	 if target preceeds us and is in range just use a short jump
;;	 else use a long jump
;;
;;	 LJ <direct jmp>,<skip jmp>,<label>

LJ	macro dirop,idirop,l
	local a
	if ((.TYPE l) xor 20h) and 0A0h
	  idirop a			      ;; not defined or is external
	  jmp l
a:
	else				      ;; is local definied
	  if (($-l) lt 124) and ($ gt l)
	    dirop l			      ;; is before and within range
	  else
	    idirop a			      ;; out of range or forward
	    jmp l
a:
	  endif
	endif
	endm

.xcref	LJE, LJNE, LJZ, LJNZ, LJC, LJNC, LJA, LJNA, LJ
.xcref	LJB, LJNB, LJS, LJNS, LJAE, LJBE, LJG, LJL, LJLE

ENABLE	macro
	jmp  $+2		; timing delay
	sti			; enable processor external interrupts
	endm

DISABLE macro
	cli
	endm			; disable processor external interrupts

DEVHLP	macro	function
	mov	dl,function
	call	dword ptr [DevHlpFunc]
	endm

IOWait	macro
	local	l
	jmp	short	l
l	label	near
	endm

DebugPublic macro label
	public label
	endm

INT3	macro
  if abdebug
	INT 03H
  endif
	endm

ListLevel macro list,bug
  if bug
    if1
      %out Warning: debuging code is being generated
    endif
  endif
  ifidn <list>,<1>		       ;; .list and .xlist
	  .list 		       ;; .lall .xall .sall
	  .sfcond		       ;; .lfcond .sfconf (.tfcond)
	  .sall
  else
    ifidn <list>,<2>
	  .list
	  .sfcond
	  .xall
    else
      ifidn <list>,<3>
	  .list
	  .lfcond
	  .lall
	  if1
	    %out listing level = 3
	  endif
      else
	  .xlist
	  .sfcond
	  .sall
      endif
    endif
  endif
	endm

;;
;;  Define MACRO to call DOS Request Command and Result
;;
TRACEREQUEST  macro    ;; trace requested DOS command on entry
  if abdebug	       ;; placed in ABDSK1.ASM
  if abtrace
    extrn ReqTrace:near
    call ReqTrace
  endif
  endif
	 endm

TRACERESULT1  macro    ;; trace requested DOS command and result on complete.
  if abdebug	       ;; placed in ABSUBR.ASM
  if abtrace
    call RsltTrace
  endif
  endif
	 endm

TRACERESULT2  macro    ;; trace requested DOS command and result on complete.
  if abdebug	       ;; placed in ABDSK1.ASM
  if abtrace
    extrn RsltTrace:near
    call RsltTrace
  endif
  endif
	 endm
