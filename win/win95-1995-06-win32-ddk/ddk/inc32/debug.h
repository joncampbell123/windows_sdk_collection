/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

#define	NOBUGBUG	1

// XLATOFF

#if DEBLEVEL > DEBLEVELRETAIL

#ifndef Not_VxD

#ifdef WIN31COMPAT

#define Debug_Out(str)	{Out_Debug_String(str "\r\n"); _asm {int 1}}

#define	Trace_Out(str)	{Out_Debug_String(str "\r\n");}

#else // not WIN31COMPAT

#define	Debug_Out(str)	{_Debug_Out_Service(str "\r\n");}

#define	Trace_Out(str)	{_Trace_Out_Service(str "\r\n");}

#define	Debug_Printf	_Debug_Printf_Service

#endif // WIN31COMPAT

#define	Debug_OutC(cond, str)	{if(cond) {Debug_Out(str)}}
#define	Trace_OutC(cond, str)	{if(cond) {Trace_Out(str)}}

#endif // Not_VxD

#define	STATIC
#define	Trap()			{_asm {_emit 0xcc}}
#define	TrapC(cond)		{if(cond) {Trap()}}
#define	TrapFatal()		{_asm { \
				 _asm _emit 0xcc \
				 _asm _emit 0xeb \
				 _asm _emit 0xfd}}
#define	TrapFatalC(cond)	{if(cond) {TrapFatal()}}

#else // DEBLEVEL > DEBLEVELRETAIL

#ifndef Not_VxD

#define	Debug_Out(str)
#define	Trace_Out(str)
#define	Debug_OutC(cond, str)
#define	Trace_OutC(cond, str)
#define	Debug_Printf()

#endif // Not_VxD

#define	STATIC		static
#define	Trap()
#define	TrapC(cond)
#define	TrapFatal()
#define	TrapFatalC(cond)

#endif // DEBLEVEL > DEBLEVELRETAIL

#ifndef Not_VxD

#if DEBLEVEL >= DEBLEVELMAX
#define Trace_Out_DebMax(str)		Trace_Out(str)
#define Debug_Out_DebMax(str)		Debug_Out(str)
#define Trace_OutC_DebMax(cond, str)	Trace_OutC(cond, str)
#define Debug_OutC_DebMax(cond, str)	Debug_OutC(cond, str)
#define	Debug_Printf_DebMax		_Debug_Printf_Service
#define Assert_VM_Handle_DebMax(hvm)	Assert_VM_Handle(hvm)
#else
#define Trace_Out_DebMax(str)
#define Debug_Out_DebMax(str)
#define Trace_OutC_DebMax(cond, str)
#define Debug_OutC_DebMax(cond, str)
#define	Debug_Printf_DebMax()
#define Assert_VM_Handle_DebMax(hvm)
#endif

#ifdef DEBUG
#define	Queue_Out(sz, uleax, ulebx) Queue_Debug_String(sz "\r\n", uleax, ulebx)
#else
#define	Queue_Out(sz, uleax, ulebx)
#endif // DEBUG

#ifdef WIN31COMPAT
#define Assert_VM_Handle(hvm)	(VMM_TRUE)
#else
#define Assert_VM_Handle(hvm)	Debug_OutC(((struct cb_s *)hvm)->CB_Signature != VMCB_ID, "Assert_VM_Handle failed")
#endif

#ifdef WIN31COMPAT
#define Assert_Thread_Handle(ptcb)
#else
#define Assert_Thread_Handle(ptcb)	Debug_OutC(((struct tcb_s *)ptcb)->TCB_Signature != SCHED_OBJ_ID_THREAD, "Assert_Thread_Handle failed")
#endif

#endif // Not_VxD

#define	BUGBUG(d, id, note)

#define	IsDebugOnlyLoaded(pvar)	(((unsigned long)(pvar)) <= MAXSYSTEMLADDR)

// XLATON

/* ASM
BUGBUG	macro	d, id, note
endm


;***	IsDebugOnlyLoaded
;
;	ENTRY	lab - label to jump if NOT loaded if not blank
;
;	EXIT:	jumps to label, if given, if debug only segment is NOT loaded
;		ZF is clear if debug-only segment IS loaded
;
;	USES:	flags
;
;	The code we generate is
;
;	    test    [magic_address], magic_operand
;	ifnb <lab>
;	    jz	    lab
;	endif
;
;	where magic_address is the address of magic_operand.  Essentially,
;	we are `test'ing an immediate constant with itself.
;
;	If the debug-only segment is not loaded, the magic_operand
;	will be zero, because _DBOSTART is placed at exactly
;	MAXSYSTEMLADDR + 1 in the no-debug-only case.
;
;	If the debug-only segment is loaded, the magic_operand will
;	be nonzero because it will be offset of the actual debug-only
;	segment (relative to MAXSYSTEMLADDR + 1).

IsDebugOnlyLoaded	macro	lab
	local	var, magic
_DBOSTART segment
var	label	byte	
_DBOSTART ends
	db	0F7h, 05h		; test memory absolute with immediate
	dd	OFFSET32 magic		; magic_address
magic	dd	OFFSET32 var - (MAXSYSTEMLADDR + 1) ; magic_operand
ifnb <lab>
	jz	lab
endif
	endm


;**	DPublic - Make a public symbol for debugging
;
;	A lot of debuggers only work with public symbols.  This macro
;	allows us to declare them public in debug mode but doesn't
;	litter the distributed .OBJ files with symbols which
;		1) waste space, and
;		2) facilitate reverse engineering of DOS

DPublic MACRO	arg
if DEBLEVEL GT DEBLEVELRETAIL
	public	arg
endif
        ENDM


;******************************************************************************
;
;   Assumes_Fall_Through
;
;   DESCRIPTION:
;	Used for debugging purposes only.  It will generate an error if
;	the IP <> the specified label.
;
;   PARAMETERS:
;	Label_Name = Name of label to fall-through to
;
;------------------------------------------------------------------------------

Assumes_Fall_Through MACRO L
ifndef MASM6
IF2
 IFDEF profileall
  IF (?prolog_&L - $) GT 3
  %OUT ERROR:  Fall through to &L invalid
  .ERR
  ENDIF
 ELSE
  IF (L - $) GT 3
  %OUT ERROR:  Fall through to &L invalid
  .ERR
  ENDIF
 ENDIF
ENDIF
else
 IFDEF profileall
.errnz ((?prolog_&L - $) GT 3), <ERROR: Fall through to &L invalid>
 ELSE
.errnz ((L - $) GT 3), <ERROR: Fall through to &L invalid>
 ENDIF
endif    ; not MASM6

     ENDM


ifndef Not_VxD

;******************************************************************************
;
;   Assert_VM_Handle
;
;   PARAMETERS:
;	Handle_Register = Register that contains a VM handle
;	MinDebugLevel   = Validate only if debug level is this level or higher
;			  (default = DEBLEVELNORMAL)
;	fUsesFlags      = the symbol "USES_FLAGS" if the macro is permitted
;			  to damage flags
;
;   ASSUMES:
;	Debug_Test_Valid_Handle does not destroy any registers or flags
;	unless USES_FLAGS is set, in which case flags are modified
;
;   EXIT:
;	NOTHING MODIFIED (not even flags)
;	unless USES_FLAGS is set, in which case flags are modified
;
;------------------------------------------------------------------------------
;
; Optimized for the case R = ebx, since that is by far the most common case.
;
; If DEBLEVELRETAIL: Do nothing
; If DEBLEVELNORMAL: Expand in-line for fast validation
; If DEBLEVELMAX:    Call into VMM for full validation
;

??avh_parse_one_arg macro arg
    ifidni <arg>, <USES_FLAGS>
	??_fUsesFlagsPushfd equ <>	    ; Don't need to preserve flags
	??_fUsesFlagsPopfd equ <>
    elseifnb <arg>
	??_debLevel = arg
    endif
endm

??avh_parse_args macro DL, fUSES_FLAGS
	??_fUsesFlagsPushfd equ <pushfd>    ; Preserve flags by default
	??_fUsesFlagsPopfd equ <popfd>
	??_debLevel = DEBLEVELNORMAL	    ; Default deblevel
	??avh_parse_one_arg <DL>
	??avh_parse_one_arg <fUSES_FLAGS>
endm

Assert_VM_Handle MACRO R, DL, fUSES_FLAGS
	LOCAL l1

IF DEBLEVEL GT DEBLEVELRETAIL

	??avh_parse_args <DL>, <fUSES_FLAGS>

IF DEBLEVEL GE ??_debLevel

IFNDEF WIN31COMPAT
IF DEBLEVEL LT DEBLEVELMAX
	??_fUsesFlagsPushfd
	cmp	[R].CB_Signature, VMCB_ID
	je	SHORT l1
ENDIF
ENDIF

IFDIFI <ebx>,<R>	
	push	ebx
	mov	ebx, R
ENDIF
	VMMCall	Debug_Test_Valid_Handle
IFDIFI <ebx>, <R>
	pop	ebx
ENDIF

IFNDEF WIN31COMPAT
IF DEBLEVEL LT DEBLEVELMAX
l1:
	??_fUsesFlagsPopfd
ENDIF
ENDIF

ENDIF

ENDIF
	ENDM

;******************************************************************************
;
;   Assert_Thread_Handle
;
;   PARAMETERS:
;       Handle_Register = Register that contains a thread handle
;	MinDebugLevel   = Validate only if debug level is this level or higher
;			  (default = DEBLEVELNORMAL)
;	fUsesFlags      = the symbol "USES_FLAGS" if the macro is permitted
;			  to damage flags
;
;   ASSUMES:
;       Debug_Test_Valid_Thread_Handle does not destroy any registers or flags
;	unless USES_FLAGS is set, in which case flags are modified
;
;   EXIT:
;	NOTHING MODIFIED (not even flags)
;	unless USES_FLAGS is set, in which case flags are modified
;
;------------------------------------------------------------------------------
;
; Optimized for the case R = edi, since that is by far the most common case.
;
; If DEBLEVELRETAIL: Do nothing
; If DEBLEVELNORMAL: Expand in-line for fast validation
; If DEBLEVELMAX:    Call into VMM for full validation

Assert_Thread_Handle MACRO R, DL, fUSES_FLAGS
	LOCAL l1

IF DEBLEVEL GT DEBLEVELRETAIL

	??avh_parse_args <DL>, <fUSES_FLAGS>

IF DEBLEVEL GE ??_debLevel

IF DEBLEVEL LT DEBLEVELMAX
	??_fUsesFlagsPushfd
	cmp	dword ptr [R.TCB_Signature], SCHED_OBJ_ID_THREAD
	je	SHORT l1
ENDIF
			
IFDIFI <edi>,<R>
        push    edi
        mov     edi, R
ENDIF
        VMMCall Debug_Test_Valid_Thread_Handle
IFDIFI <edi>,<R>
        pop     edi
ENDIF

IF DEBLEVEL LT DEBLEVELMAX
l1:
	??_fUsesFlagsPopfd
ENDIF

ENDIF

ENDIF
	ENDM

;******************************************************************************
;
;   Assert_Cur_Thread_Handle (Register)
;
;   DESCRIPTION: Verifies that the register contains the current thread handle
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================
;
; Optimized for the case R = edi, since that is by far the most common case.

Assert_Cur_Thread_Handle MACRO R, DL
	LOCAL myDebLevel
	LOCAL	OK

IF DEBLEVEL GT DEBLEVELRETAIL

IFB <DL>
	myDebLevel EQU DEBLEVELNORMAL
ELSE
	myDebLevel EQU <DL>
ENDIF

IF DEBLEVEL GE myDebLevel

IFDIFI <edi>,<R>
        push    edi
        mov     edi, R
ENDIF
        VMMCall Debug_Test_Cur_Thread
IFDIFI <edi>,<R>
        pop     edi
ENDIF

ENDIF

ENDIF
	ENDM

;******************************************************************************
;
;   Debug_Printf
;
;   ENTRY:	fmt - format string
;		args - printf arguments enclosed with <>
;		dl - optional DEBLEVEL
;
;   USES: 	NONE
;
;------------------------------------------------------------------------------

Debug_Printf	macro	fmt, args, dl
	local	fmtlab, myDebLevel

ife ?_DBOCODE
    ??_fDoit = VMM_TRUE
else
    ??_fDoit = FALSE
endif

if DEBLEVEL GT DEBLEVELRETAIL

ifb <dl>
	myDebLevel EQU <DEBLEVELNORMAL>
else
	myDebLevel EQU <dl>
endif

if DEBLEVEL GE myDebLevel 
    ??_fDoit = VMM_TRUE
endif

endif

if ??_fDoit

ife ?_DBOCODE

VxD_DEBUG_ONLY_DATA_SEG
fmtlab	db	fmt, 0
VxD_DEBUG_ONLY_DATA_ENDS

else

ifdef VMMSYS

VMM_LOCKED_DATA_SEG
fmtlab	db	fmt, 0
VMM_LOCKED_DATA_ENDS

else

VxD_LOCKED_DATA_SEG
fmtlab	db	fmt, 0
VxD_LOCKED_DATA_ENDS

endif ; VMMSYS

endif ; ?_DBOCODE

	ifb <args>
	VMMCall _Debug_Printf_Service, <(OFFSET32 fmtlab), esp>
	else
	VMMCall _Debug_Printf_Service, <(OFFSET32 fmtlab), esp, args>
	endif

endif ; ??_fDoit

	endm

;******************************************************************************
;
;   Trace_Out / Debug_Out common code
;
;------------------------------------------------------------------------------

CHECK_EOL MACRO f, x, ln
	ifdifi <x>,<noeol>
	ifdifi <x>,<no_eol>
	ifdifi <x>,<nocrlf>
	ifdifi <x>,</noeol>
		%OUT Line ln: Unknown symbol (x) in f, taken as NOEOL
	endif
	endif
	endif
	endif

ENDM

??_Gen_String macro lbl:req, str:req
	ife ?_ICODE
	    ??_segName textequ <_IDATA>
	elseife ?_PCODE
	    ??_segName textequ <_PDATA>
	elseife ?_SCODE
	    ??_segName textequ <_SDATA>
	elseife ?_DBOCODE
	    ??_segName textequ <_DBODATA>
	else
	    ??_segName textequ <_LDATA>	; Default is locked data
	endif

	??_segName segment
	    lbl	db	str
	    ife ??_nocrlf
		db	0dh,0ah
	    endif
		db	0
	??_segName ends
endm


??Trace_Debug_Helper macro typ, str, arg1, arg2
	local	string

    ife ?_DBOCODE
	??_fDoit = VMM_TRUE
    else
	??_fDoit = 0
    endif

    if (DEBLEVEL GT DEBLEVELRETAIL) OR ??_fDoit

	??_nocrlf = 0
	??_debLevel = DEBLEVELNORMAL

	irp x, <arg1, arg2>
	    ifnb <x>
		if ((.TYPE x) AND 20h) GT 0	;; symbol defined
		    ??_debLevel = x
		else
		    Check_EOL <typ>, <x>, %(@Line)
		    ??_nocrlf = 1
		endif
	    endif
	endm

	if DEBLEVEL GE ??_debLevel
	    ??_fDoit = VMM_TRUE
	endif

    endif

    if ??_fDoit

	irpc c, str
	    ifidn <c>, <">
		??_is_string = 1
	    else
	    ifidni <c>, <'>
		??_is_string = 1
	    else
		??_is_string = 0
	    endif
	    endif
	    exitm
	endm

	if ??_is_string 		; Put the string in the approp segment
	    ??_Gen_String string, <str>
	    ??_debug_out_str textequ <OFFSET32 string>
	else
	    ??_debug_out_str textequ <str>
	endif

	ifdef WIN31COMPAT
		pushfd
		pushad
		mov	esi, ??_debug_out_str
		VMMCall Out_Debug_String
	    ifidni <typ>, <Debug_Out>
		VMMCall Test_Debug_Installed
		jz	SHORT $+4
		int	1
	    endif
		popad
		popfd
	else
		push	??_debug_out_str
	    ifidni <typ>, <Debug_Out>
		VMMCall _Debug_Out_Service
	    else
		VMMCall _Trace_Out_Service
	    endif
	endif ; WIN31COMPAT

    endif ; ??_fDoit

	endm

;******************************************************************************
;
;   Trace_Out
;
;------------------------------------------------------------------------------

;       Create the following macros:
;
;	Trace_OutS, Trace_OutNS, Trace_OutC, Trace_OutNC, Trace_OutA,
;	Trace_OutNA, Trace_OutAE, Trace_OutNAE, Trace_OutB, Trace_OutNB,
;	Trace_OutBE, Trace_OutNBE, Trace_OutE, Trace_OutNE, Trace_OutZ,
;	Trace_OutNZ, Trace_OutG, Trace_OutNG, Trace_OutGE, Trace_OutNGE,
;	Trace_OutL, Trace_OutNL, Trace_OutLE, Trace_OutNLE, Trace_OutO,
;	Trace_OutNO

irp     cond,<S,C,A,AE,B,BE,E,Z,G,GE,L,LE,O>

Trace_Out&cond macro str, arg1, arg2
	?trace_out <str>,jn&cond, <arg1>, <arg2>
	endm

Trace_OutN&cond macro str, arg1, arg2
	?trace_out <str>,j&cond, <arg1>, <arg2>
	endm

endm

Trace_Out MACRO str, arg1, arg2
	??Trace_Debug_Helper <Trace_Out>, <str>, <arg1>, <arg2>
endm

Trace_OutECXZ   macro str, arg1, arg2
	local	l1,l2
if (DEBLEVEL GT DEBLEVELRETAIL) or (?_DBOCODE eq 0)
	jecxz	l1
	jmp	short l2
l1:	Trace_Out <str>, <arg1>, <arg2>
l2:
endif
	endm

Trace_OutECXNZ macro str, arg1, arg2
	?trace_out <str>,jecxz, <arg1>, <arg2>
	endm

Trace_OutEAXz macro str, arg1, arg2
	local	l1
if (DEBLEVEL GT DEBLEVELRETAIL) or (?_DBOCODE eq 0)
	or	eax,eax
	jnz	short l1
	Trace_Out <str>, <arg1>, <arg2>
l1:
endif
	endm

Trace_OutEAXnz macro str, arg1, arg2
	local	l1
if (DEBLEVEL GT DEBLEVELRETAIL) or (?_DBOCODE eq 0)
	or	eax,eax
	jz	short l1
	Trace_Out <str>, <arg1>, <arg2>
l1:
endif
	endm

?trace_out macro str, jmpop, arg1, arg2
	Local	nomsg
if (DEBLEVEL GT DEBLEVELRETAIL) or (?_DBOCODE eq 0)
	jmpop	short nomsg
	Trace_Out <str>,<arg1>,<arg2>
nomsg:
endif
	endm


;******************************************************************************
;
;   Debug_Out
;
;------------------------------------------------------------------------------

;	Create the following macros:
;
;	Debug_OutS, Debug_OutNS, Debug_OutC, Debug_OutNC, Debug_OutA,
;	Debug_OutNA, Debug_OutAE, Debug_OutNAE, Debug_OutB, Debug_OutNB,
;	Debug_OutBE, Debug_OutNBE, Debug_OutE, Debug_OutNE, Debug_OutZ,
;	Debug_OutNZ, Debug_OutG, Debug_OutNG, Debug_OutGE, Debug_OutNGE,
;	Debug_OutL, Debug_OutNL, Debug_OutLE, Debug_OutNLE, Debug_OutO,
;	Debug_OutNO


irp	cond,<S,C,A,AE,B,BE,E,Z,G,GE,L,LE,O>

Debug_Out&cond &macro str, arg1
	?debug_out <str>,jn&cond,<arg1>
	&endm

Debug_OutN&cond &macro str, arg1
	?debug_out <str>,j&cond,<arg1>
	&endm

endm

Debug_Out MACRO str, arg1, arg2
	??Trace_Debug_Helper <Debug_Out>, <str>, <arg1>, <arg2>
endm

Debug_OutECXZ	macro str, arg1
	local	l1,l2
if DEBLEVEL GT DEBLEVELRETAIL
	jecxz	l1
	jmp	short l2
l1:	Debug_Out <str>, <arg1>
l2:
endif
	endm

Debug_OutECXNZ macro str, arg1
	?debug_out <str>,jecxz, <arg1>
	endm

Debug_OutEAXz macro str, arg1
	local	l1
if DEBLEVEL GT DEBLEVELRETAIL
	or	eax,eax
	jnz	short l1
	Debug_Out <str>, <arg1>
l1:
endif
	endm

Debug_OutEAXnz macro str, arg1
	local	l1
if DEBLEVEL GT DEBLEVELRETAIL
	or	eax,eax
	jz	short l1
	Debug_Out <str>, <arg1>
l1:
endif
	endm

?debug_out macro str,jmpop, arg1
	Local	nomsg
if DEBLEVEL GT DEBLEVELRETAIL
	jmpop	short nomsg
	Debug_Out <str>, <arg1>
nomsg:
endif
	endm


;******************************************************************************
;
;   Queue_Out
;
;------------------------------------------------------------------------------

Queue_Out MACRO S, V1, V2, DL
	LOCAL	Str_Off
	LOCAL MyDebLevel

IF DEBLEVEL GT DEBLEVELRETAIL

IFB <DL>
	myDebLevel = DEBLEVELNORMAL
ELSE
	myDebLevel = DL
ENDIF

IF DEBLEVEL GE myDebLevel

_LDATA SEGMENT
Str_Off db S, 0dh,0ah, 0
_LDATA ENDS

	push	esi
IFNB <V1>
    IF	TYPE V1 GT 0
	push	dword ptr V1
    ELSE
	push	V1
    ENDIF
ELSE
	push	eax		; dummy value1
ENDIF
IFNB <V2>
    IF	TYPE V2 GT 0
	push	dword ptr V2
    ELSE
	push	V2
    ENDIF
ELSE
	push	ebx		; dummy value2
ENDIF
	mov	esi, OFFSET32 Str_Off
	VMMCall Queue_Debug_String
	pop	esi
ENDIF

ENDIF
	ENDM


;******************************************************************************
;
;   Mono_Out
;
;------------------------------------------------------------------------------

Mono_Out MACRO S, nocrlf
	LOCAL	Str_Off
IF DEBLEVEL GT DEBLEVELRETAIL
_LDATA SEGMENT
Str_Off db	S
IFB <nocrlf>
	db	0dh,0ah
ENDIF
	db	0
_LDATA ENDS

	pushfd
	pushad
	mov	esi, OFFSET32 Str_Off
	VMMCall Out_Mono_String
	popad
	popfd
ENDIF
	ENDM


;******************************************************************************
;
;   Mono_Out_At
;
;------------------------------------------------------------------------------

Mono_Out_At MACRO Row, Col, S, nocrlf
	LOCAL	Str_Off
IF DEBLEVEL GT DEBLEVELRETAIL
_LDATA SEGMENT
Str_Off db	S
IFB <nocrlf>
	db	0dh,0ah
ENDIF
	db	0
_LDATA ENDS

	pushfd
	pushad
	mov	dx, (Row SHL 8)+Col
	VMMCall Set_Mono_Cur_Pos
	mov	esi, OFFSET32 Str_Off
	VMMCall Out_Mono_String
	popad
	popfd
ENDIF
	ENDM




;******************************************************************************
;
;   Assert_Ints_Disabled
;
;------------------------------------------------------------------------------

Assert_Ints_Disabled MACRO DL

IFB <DL>
	??_debLevel = DEBLEVELNORMAL
ELSE
	??_debLevel = DL
ENDIF

IF DEBLEVEL GE ??_debLevel

ifndef ??_aidMessage
	_LDATA segment
	??_aidMessage	db	"ERROR:  Ints enabled at Assert_Ints_Disabled"
			db	0Dh, 0Ah, 0
	_LDATA ends
endif
	pushfd
	test	byte ptr [esp+1], IF_Mask SHR 8
    .if	!ZERO?
	push	OFFSET32 ??_aidMessage
	VMMCall _Debug_Out_Service
    .endif
	popfd
ENDIF
	ENDM


;******************************************************************************
;
;   Assert_Ints_Enabled
;
;------------------------------------------------------------------------------

Assert_Ints_Enabled MACRO DL

IFB <DL>
	??_debLevel = DEBLEVELNORMAL
ELSE
	??_debLevel = DL
ENDIF

IF DEBLEVEL GE ??_debLevel

ifndef ??_aieMessage
	_LDATA segment
	??_aieMessage	db	"ERROR:  Ints disabled at Assert_Ints_Enabled"
			db	0Dh, 0Ah, 0
	_LDATA ends
endif
	pushfd
	test	byte ptr [esp+1], IF_Mask SHR 8
    .if	ZERO?
	push	OFFSET32 ??_aieMessage
	VMMCall _Debug_Out_Service
    .endif
	popfd
ENDIF
	ENDM

;******************************************************************************
;
;   Assert_Cur_VM_Handle (Register)
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================
;
; Optimized for the case R = ebx, since that is by far the most common case.

Assert_Cur_VM_Handle MACRO R, DL
	LOCAL	OK
	LOCAL myDebLevel

IF DEBLEVEL GT DEBLEVELRETAIL

IFB <DL>
	myDebLevel = DEBLEVELNORMAL
ELSE
	myDebLevel = DL
ENDIF

IF DEBLEVEL GE myDebLevel

IFDIFI <ebx>,<R>
	push	ebx
	mov	ebx, R
ENDIF
	VMMCall Debug_Test_Cur_VM
IFDIFI <ebx>,<R>
	pop	ebx
ENDIF

ENDIF

ENDIF
	ENDM


Assert_Client_Ptr MACRO Reg, DL
	LOCAL myDebLevel
IF DEBLEVEL GT DEBLEVELRETAIL

IFB <DL>
	myDebLevel = DEBLEVELNORMAL
ELSE
	myDebLevel = DL
ENDIF

IF DEBLEVEL GE myDebLevel
IFDIFI <ebp>,<Reg>
	push	ebp
	mov	ebp, Reg
ENDIF
	VMMCall Validate_Client_Ptr
IFDIFI <ebp>,<Reg>
	pop	ebp
ENDIF
ENDIF

ENDIF
	ENDM

endif ;* Not_VxD *


;******************************************************************************
;
;	TRAP
;
;------------------------------------------------------------------------------


;	Create the following macros:
;
;	TRAPs, TRAPns, TRAPc, TRAPnc, TRAPa, TRAPna, TRAPae, TRAPnae,
;	TRAPb, TRAPnb, TRAPbe, TRAPnbe, TRAPe, TRAPne, TRAPz, TRAPnz,
;	TRAPg, TRAPng, TRAPge, TRAPnge, TRAPl, TRAPnl, TRAPle, TRAPnle,
;	TRAPo, TRAPno
;
;	TRAPFATALs, TRAPFATALns, TRAPFATALc, TRAPFATALnc, TRAPFATALa,
;	TRAPFATALna, TRAPFATALae, TRAPFATALnae, TRAPFATALb, TRAPFATALnb,
;	TRAPFATALbe, TRAPFATALnbe, TRAPFATALe, TRAPFATALne, TRAPFATALz,
;	TRAPFATALnz, TRAPFATALg, TRAPFATALng, TRAPFATALge, TRAPFATALnge,
;	TRAPFATALl, TRAPFATALnl, TRAPFATALle, TRAPFATALnle, TRAPFATALo,
;	TRAPFATALno

irp	cond,<s,c,a,ae,b,be,e,z,g,ge,l,le,o>

TRAP&cond &macro
	?trap	jn&cond
	&endm

TRAPn&cond &macro
	?trap	j&cond
	&endm

TRAPFATAL&cond &macro
	?trap	jn&cond, FATAL
	&endm

TRAPFATALn&cond &macro
	?trap	j&cond, FATAL
	&endm
endm

TRAP	macro
if DEBLEVEL GT DEBLEVELRETAIL
	int	3
endif
	endm

TRAPFATAL	macro
	local	l
if DEBLEVEL GT DEBLEVELRETAIL
l:	int	3
	jmp	short l
endif
	endm

TRAPecxz macro
	local	l1,l2
if DEBLEVEL GT DEBLEVELRETAIL
	jecxz	l1
	jmp	short l2
l1:	int	3
l2:
endif
	endm

TRAPecxnz macro
	?trap	jecxz
	endm

?trap	macro	jmpop, fatal
	Local	l, n
if DEBLEVEL GT DEBLEVELRETAIL
	jmpop	short n
l:	int	3
ifnb	<fatal>
	jmp	short l
endif
n:
endif
	endm

ifndef Not_VxD

;******************************************************************************


Dump_Struc_Head MACRO
if DEBLEVEL GT DEBLEVELRETAIL
	Trace_Out "  Base    Address   Offs     Value  Field name"
ENDIF
	ENDM

Dump_Struc MACRO Base, X
if DEBLEVEL GT DEBLEVELRETAIL
	pushfd
	pushad
	lea	esi, [Base]
	mov	ecx, X
	lea	edx, [esi+ecx]

IF SIZE X EQ 6
	mov	bx, WORD PTR [edx+4]
	mov	eax, DWORD PTR [edx]
	Trace_Out "#ESI  #EDX  #CX  #BX:#EAX  &X"
ELSE
IF SIZE X EQ 4
	mov	eax, DWORD PTR [edx]
	Trace_Out "#ESI  #EDX  #CX  #EAX  &X"
ELSE
IF SIZE X EQ 2
	mov	ax, WORD PTR [edx]
	Trace_Out "#ESI  #EDX  #CX      #AX  &X"
ELSE
	mov	al, BYTE PTR [edx]
	Trace_Out "#ESI  #EDX  #CX        #AL  &X"
ENDIF
ENDIF
ENDIF

	popad
	popfd
ENDIF
	ENDM

BeginDoc
;******************************************************************************
;
;   Begin_Touch_1st_Meg / End_Touch_1st_Meg
;
;   DESCRIPTION:
;	These macros should be used by VxDs that need to touch memory in
;	the 1st megabyte.  For example, if a VxD wanted to examine interrupt
;	vector 21h, the code would look like this:
;
;	    Begin_Touch_1st_Meg
;	    mov     eax, DWORD PTR ds:[21h*4]
;	    End_Touch_1st_Meg
;
;	When building retail VxDs, these macros generate no code.  When
;	building debug VxDs these macros generate calls to Enable_Touch_1st_Meg
;	and Disable_Touch_1st_Meg.
;
;   ENTRY:
;	None
;
;   EXIT:
;	None
;
;   USES:
;	Nothing
;
;==============================================================================
EndDoc

Begin_Touch_1st_Meg MACRO DL
	LOCAL myDebLevel
IF DEBLEVEL GT DEBLEVELRETAIL
IFB <DL>
	myDebLevel = DEBLEVELMAX
ELSE
	myDebLevel = DL
ENDIF
IF DEBLEVEL GE myDebLevel
	VMMCall Enable_Touch_1st_Meg
ENDIF
ENDIF
	ENDM

;------------------------------------------------------------------------------

End_Touch_1st_Meg MACRO DL
	LOCAL myDebLevel
IF DEBLEVEL GT DEBLEVELRETAIL
IFB <DL>
	myDebLevel = DEBLEVELMAX
ELSE
	myDebLevel = DL
ENDIF
IF DEBLEVEL GE myDebLevel
	VMMCall Disable_Touch_1st_Meg
ENDIF
ENDIF
	ENDM

endif ;* Not_VxD *
*/

/* XLATON */
/******************************************************************************
 *
 *   TrashThis
 *
 *  DESCRIPTION:
 *
 *	In DEBUG, trashes a register or variable to ensure that the
 *	caller is not relying on its value being preserved, or containing
 *	any particular value.  Does nothing in RETAIL.
 *
 *	Procedures that use the C calling convention should trash the
 *	ECX and EDX registers (and possibly also EAX) on exit, as well
 *	as all the stack parameters.
 *
 *  PARAMETERS:
 *
 *  C:
 *	TrashThis1(a) - destroy variable `a'
 *	TrashThis2(a,b) - destroy variables `a' and `b'
 *	 ...
 *
 *  Asm:
 *	l - list of registers or variables to munge
 *	    More efficient code is generated if the first element of `l'
 *	    is a register, because that register will be used to trash
 *	    the other values.
 *
 *****************************************************************************/

/* XLATOFF */

#ifdef	DEBUG
#define TrashThis1(a) (*(PULONG)&(a) |= 0xFFFFFF80)
#define TrashThis2(a,b) (TrashThis1(a), TrashThis1(b))
#define TrashThis3(a,b,c) (TrashThis1(a), TrashThis2(b,c))
#define TrashThis4(a,b,c,d) (TrashThis1(a), TrashThis3(b,c,d))
#else
#define TrashThis1(a)
#define TrashThis2(a,b)
#define TrashThis3(a,b,c)
#define TrashThis4(a,b,c,d)
#endif

/* XLATON */

/* ASM
IFDEF	DEBUG
TrashThis Macro	l:vararg
    ?TrashThisVal equ <0FFFFFF80h>
    ?TrashThisValSet = 0
    for r, <l>
	or	r, ?TrashThisVal	; Mangle the value
      ife ?TrashThisValSet
        if (OPATTR r) and 10h		; If is a register
	    ?TrashThisVal equ <r>	; use it to mangle others
	    ?TrashThisValSet = 1
	endif
      endif
    endm
endm
ELSE
TrashThis Macro	l:vararg
endm
ENDIF

*/



#ifndef Not_VxD

/*XLATOFF*/
#if DEBLEVEL >= DEBLEVELMAX

#define	Begin_Touch_1st_Meg()	Enable_Touch_1st_Meg()
#define	End_Touch_1st_Meg()	Disable_Touch_1st_Meg()

#else

#define	Begin_Touch_1st_Meg()
#define	End_Touch_1st_Meg()

#endif
/*XLATON*/

/*
 *  Macros for the _Debug_Flags_Service service.
 *
 *  Place ENTER_NOBLOCK at the beginning of a section of code
 *  that must not block and EXIT_NOBLOCK.  Between the two, any
 *  code that might block (e.g., accessing pageable code or data,
 *  blocking on a semaphore) will trigger an assertion failure.
 *
 *  You can use the Assert_Might_Block macro to force the above
 *  check to be made.
 *
 *  Assert_Not_Nest_Exec checks that the current thread is not
 *  in nested execution.
 *
 */

/* ASM

if DEBLEVEL GT DEBLEVELRETAIL

ENTER_NOBLOCK MACRO
	push	DFS_ENTER_NOBLOCK
	VMMCall _Debug_Flags_Service
	ENDM

EXIT_NOBLOCK MACRO
	push	DFS_EXIT_NOBLOCK
	VMMCall _Debug_Flags_Service
	ENDM

Assert_CLD MACRO
	pushfd
	test	dword ptr [esp], DF_MASK
	Debug_OutNZ "Direction flag is not clear."
	popfd
	ENDM

Assert_Might_Block MACRO
	push	DFS_TEST_BLOCK
	VMMCall _Debug_Flags_Service
	ENDM

Assert_Not_Nest_Exec MACRO
	push	DFS_TEST_NEST_EXEC
	VMMCall _Debug_Flags_Service
	ENDM

ELSE					; in retail, everything is a NOP

ENTER_NOBLOCK EQU <>
EXIT_NOBLOCK EQU <>
Assert_CLD EQU <>
Assert_Might_Block EQU <>
Assert_Not_Nest_Exec EQU <>

ENDIF ; DEBLEVEL GT DEBLEVELRETAIL

*/

//XLATOFF
#if DEBLEVEL > DEBLEVELRETAIL

#define ENTER_NOBLOCK() 	_Debug_Flags_Service(DFS_ENTER_NOBLOCK)
#define EXIT_NOBLOCK() 		_Debug_Flags_Service(DFS_EXIT_NOBLOCK)
#define Assert_CLD()		_Debug_Flags_Service(DFS_TEST_CLD)
#define Assert_Might_Block()	_Debug_Flags_Service(DFS_TEST_BLOCK)
#define Assert_Not_Nest_Exec()  _Debug_Flags_Service(DFS_TEST_NEST_EXEC)

/******************************************************************************
BeginDoc
 *
 *  BeginCProc
 *
 *  DESCRIPTION:
 *	This macro inserts inline assembly code to validate the current
 *	machine state, in the same manner that BeginProc does for assembly.
 *
 *	The following flags are supplied by default.
 *
 *	    DFS_LOG
 *	    DFS_TEST_CLD
 *
 *	You may combine any of the following flags.
 *
 *	   -DFS_LOG		// note minus sign!
 *	    DFS_NEVER_REENTER
 *	    DFS_TEST_REENTER
 *	   -DFS_TEST_CLD	// note minus sign!
 *	    DFS_NOT_SWAPPING
 *	    DFS_TEST_BLOCK
 *
 *  EXAMPLES:
 *
 *	BeginCProc(0);			// use the defaults
 *
 *	BeginCProc(DFS_TEST_REENTER);	// also check VMM reentrancy
 *
 *	BeginCProc(-DFS_LOG);		// don't log this procedure
 *
 *	BeginCProc(DFS_TEST_REENTER-DFS_LOG);
 *					// check VMM reentrancy, but don't log
 *
EndDoc
 *****************************************************************************/


#define BeginCProc(f) _Debug_Flags_Service(f+DFS_LOG+DFS_TEST_CLD)

#else					// in retail, everything is a NOP

#define ENTER_NOBLOCK()
#define EXIT_NOBLOCK()
#define Assert_CLD()
#define Assert_Might_Block()
#define Assert_Not_Nested_Exec()
#define BeginCProc(f)

#endif
//XLATON

#ifdef Begin_Service_Table		// define only if vmm.h is included

//XLATOFF
#define	DEBUG_Service	Declare_Service
#pragma warning (disable:4003)		// turn off not enough params warning
//XLATON

//MACROS
Begin_Service_Table(DEBUG)

DEBUG_Service	(DEBUG_Get_Version, LOCAL)
DEBUG_Service	(DEBUG_Fault, LOCAL)
DEBUG_Service	(DEBUG_CheckFault, LOCAL)
DEBUG_Service	(_DEBUG_LoadSyms)

End_Service_Table(DEBUG)
//ENDMACROS

//XLATOFF
#pragma warning (default:4003)		// turn on not enough params warning
//XLATON

#endif // Begin_Service_Table

#endif // Not_VxD
