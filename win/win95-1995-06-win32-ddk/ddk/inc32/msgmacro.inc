;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

;******************************************************************************
;
;   Title:      msgmacro.inc - Macros for localizable strings
;
;   Version:    1.00
;
;==============================================================================

MESSAGE_VERSION EQU 100h

;------------------------------------------------------------------------------
;
;   Symbols whose names begin with `@MSG' are for private msgmacro.inc use.
;
;------------------------------------------------------------------------------
;
; Each message class (init, pageable, locked, etc.) has its own message anchor
; block, which in turn tells how to access all the messages of that class.
;
; Message numbers themselves are one-based (not zero-based) indexes into the
; offset array appended to the anchor block.  There is an extra phantom
; message offset at the end which gives the total size of the message data.
;

@MSGANCHORBLOCK struc
@msgab_cMsg     dd      ?       ; Number of messages in this table
@msgab_pMsgs    dd      ?       ; Pointer to message data
@msgab_ver      dd      ?       ; Version stamp (MESSAGE_VERSION)
@msgab_rgOfs    dw      0 dup(?); Variable-sized array of offsets
@MSGANCHORBLOCK ends

@MSG_CLASSNAMES TEXTEQU <>      ; Names of classes used
@MSG_CLASSLAST TEXTEQU <'?'>    ; The last defined class

;==============================================================================
;
; The generic macro-making engine
;
;   NOTE! SPECIAL HACK FOR LOCKED MESSAGES!  Since locked messages have
;   no mask, we cannot use a bit test to see if a message number is a
;   locked message.  So we make sure that "L" goes at the end of the
;   class list by always prepending non-locked class identifiers and
;   always appending the locked class identifier.  Yes, it's gross.
;
;   NOTE! also that we must test CREATE_MESSAGES at evaluation time
;   rather than at definition time, because some people put the
;   `CREATE_MESSAGES=1' line *between* the `include msgmacro.inc'
;   and the `include mymsg.inc' lines...
;
;==============================================================================


@MSG_SEG MACRO t:req
        t SEGMENT DWORD PUBLIC FLAT 'MCODE'
ENDM

@MSG_ENDS MACRO t:req
        t ENDS
ENDM

;
; Create a new message class.
;   c = class identifier (single character)
;   f = bitmask used to identify messages of this class
;  nm = full name (used in PUSH_xx_SPRINTF et al)
;
@MSG_CREATE_MESSAGE_CLASS MACRO c:req, fMask:req, nm:req

        externdef @MSG_&c&AB:@MSGANCHORBLOCK    ; The anchor block
        externdef @MSG_&c&DATA:byte             ; The data itself

        @MSG_&c&CLASSMASK = fMask

  VxD_&c&MESSAGE_SEG MACRO

    IFDEF CREATE_MESSAGES

      IFNDEF BLD_COFF

        @MSG_TABLE_SEG  TEXTEQU <@MSG_SEG  _&c&MSGTABLE>
        @MSG_TABLE_ENDS TEXTEQU <@MSG_ENDS _&c&MSGTABLE>

        @MSG_DATA_SEG   TEXTEQU <@MSG_SEG  _&c&MSGDATA>
        @MSG_DATA_ENDS  TEXTEQU <@MSG_ENDS _&c&MSGDATA>

      ELSE

        @MSG_TABLE_SEG  TEXTEQU <@MSG_SEG  _&c&MSG$1>
        @MSG_TABLE_ENDS TEXTEQU <@MSG_ENDS _&c&MSG$1>

        @MSG_DATA_SEG   TEXTEQU <@MSG_SEG  _&c&MSG$2>
        @MSG_DATA_ENDS  TEXTEQU <@MSG_ENDS _&c&MSG$2>

      ENDIF

        @MSG_DATASTART  TEXTEQU <@MSG_&c&DATA>

      IFNDEF BLD_COFF
        @MSG_SEG  _&c&MSGTABLE          ; Create the anchor block
      ELSE
        @MSG_SEG  _&c&MSG$1             ; Create the anchor block
      ENDIF
        @MSG_&c&AB LABEL @MSGANCHORBLOCK
        ;  DO NOT UNDER ANY CIRCUMSTANCES EDIT THE FOLLOWING LINE
        ;  UNLESS YOU WANT TO SPEND A HALF HOUR CHASING DOWN A MASM BUG!
@MSGANCHORBLOCK { (@MSG_&c&TABLEEND - $ - SIZE @MSGANCHORBLOCK)/2, offset32 @MSG_&c&DATA, MESSAGE_VERSION }
      IFNDEF BLD_COFF
        @MSG_ENDS _&c&MSGTABLE
      ELSE
        @MSG_ENDS _&c&MSG$1
      ENDIF

      IFNDEF BLD_COFF
        @MSG_SEG  _&c&MSGDATA
        @MSG_&c&DATA LABEL BYTE         ; The start-of-data marker
        @MSG_ENDS _&c&MSGDATA

	_&c&MGROUP GROUP _&c&MSGTABLE, _&c&MSGDATA
      ELSE
        @MSG_SEG  _&c&MSG$2
        @MSG_&c&DATA LABEL BYTE         ; The start-of-data marker
        @MSG_ENDS _&c&MSG$2
      ENDIF

    ENDIF                               ; CREATE_MESSAGES

        @MSG_CUR = @MSG_&c&CLASSMASK    ; Current message number

    IF fMask
        @MSG_CLASSNAMES CATSTR <&c&>, @MSG_CLASSNAMES
    ELSE
        @MSG_CLASSNAMES CATSTR @MSG_CLASSNAMES, <&c&>
    ENDIF
        @MSG_CLASSLAST CatStr <'>, \
                @SubStr(%@MSG_CLASSNAMES, @SizeStr(%@MSG_CLASSNAMES), 1), <'>

  ENDM                                  ; VxD_&c&MESSAGE_SEG

  VxD_&c&MESSAGE_ENDS MACRO

    IFDEF CREATE_MESSAGES

        @MSG_DATA_SEG
        @MSG_&c&DATAEND LABEL BYTE      ; The end-of-data marker
        @MSG_DATA_ENDS

        @MSG_TABLE_SEG                  ; Terminate the offset table
        @MSG_&c&TABLEEND LABEL WORD
                dw      @MSG_&c&DATAEND - @MSG_&c&DATA ; add phantom
        @MSG_TABLE_ENDS

    ENDIF

  ENDM                                  ; VxD_&c&MESSAGE_ENDS

  ; msgnum may be a constant or a register.
  ; If it is a register, the register will be changed.


  PUSH_&nm&_SPRINTF MACRO msgnum:REQ, params:VARARG
        @MSG_PUSH_SPRINTF <&c&>, <msgnum>, <params>
  ENDM

  POP_&nm&_SPRINTF TEXTEQU <POP_SPRINTF>

ENDM


;==============================================================================
;
; Declare our various message classes
;
;==============================================================================

@MSG_CLASSMASK  =       0C000h
@MSG_MSGMASK    =       03FFFh

@MSG_CREATE_MESSAGE_CLASS I, 8000h, INIT
@MSG_CREATE_MESSAGE_CLASS L, 0000h, LOCKED
@MSG_CREATE_MESSAGE_CLASS P, 4000h, PAGEABLE

VxD_MESSAGE_SEG  TEXTEQU <VxD_LMESSAGE_SEG> ; For backwards compatibility
VxD_MESSAGE_ENDS TEXTEQU <VxD_LMESSAGE_ENDS>

;==============================================================================
;
; Macros for declaring messages
;
;==============================================================================

LONG_MESSAGE_START MACRO   name

    @MSG_CUR    =       @MSG_CUR + 1
    &name       =       @MSG_CUR

    IFDEF CREATE_MESSAGES

        @MSG_DATA_SEG
        name&_ME$ label byte
        @MSG_DATA_ENDS

        @MSG_TABLE_SEG
                DW      name&_ME$ - @MSG_DATASTART
        @MSG_TABLE_ENDS

    ENDIF

ENDM

LONG_MESSAGE MACRO  string
    IFDEF CREATE_MESSAGES
        @MSG_DATA_SEG
            db string
        @MSG_DATA_ENDS
    ENDIF                               ; !CREATE_MESSAGES
ENDM

LONG_MESSAGE_END MACRO name
    IFDEF CREATE_MESSAGES
        @MSG_DATA_SEG
            db  0
        @MSG_DATA_ENDS
    ENDIF                               ; !CREATE_MESSAGES
ENDM

DECLARE_MESSAGE MACRO   name, string
    LONG_MESSAGE_START <name>
    LONG_MESSAGE <string>
    LONG_MESSAGE_END <name>
ENDM

;BUGBUG temporary, should put somewhere separate to be compressed
SQZD_MSG_START  EQU LONG_MESSAGE_START
SQZD_MSG        EQU LONG_MESSAGE
SQZD_MSG_END    EQU LONG_MESSAGE_END

;==============================================================================
;
; Macros for accessing messages
;
;==============================================================================


; @MSG_GetOneRegClass: Internal helper macro for GET_MESSAGE_PTR
;
;   c = message class to try
;   msgno = register containing message number
;   reg = register to receive pointer
;   l = label to jump to when done

@MSG_GetOneRegClass MACRO c, msgno, reg, l
    LOCAL skip
    IF '&c&' EQ @MSG_CLASSLAST          ; Last class, skip the test

        movzx   reg, @MSG_&c&AB.@msgab_rgOfs[msgno*2-2-2*@MSG_&c&CLASSMASK] ; Get offset
        add     reg, @MSG_&c&AB.@msgab_pMsgs            ; Add bias
                                                        ; Fall through
    ELSE
        TestReg msgno, @MSG_&c&CLASSMASK                ; Is it ours?
        jz      short skip                              ; N: Skip
        movzx   reg, @MSG_&c&AB.@msgab_rgOfs[msgno*2-2-2*@MSG_&c&CLASSMASK] ; Y: Get offset
        add     reg, @MSG_&c&AB.@msgab_pMsgs            ;    Add bias
        jmp     l                                       ;    Done
      skip:
    ENDIF
ENDM

; @MSG_GetOneConstClass: Internal helper macro for GET_MESSAGE_PTR
;
;   c = message class to try
;   msgnum = constant message number
;   reg = target register
;   @MSG_regstr = optional `+reg*2'

@MSG_GetOneConstClass MACRO c, msgnum, reg
    IF (msgnum AND @MSG_CLASSMASK) EQ @MSG_&c&CLASSMASK
%       movzx   reg, @MSG_&c&AB.@msgab_rgOfs[(msgnum AND @MSG_MSGMASK)*2-2 @MSG_regstr]
        add     reg, @MSG_&c&AB.@msgab_pMsgs            ; Add bias
    ENDIF
ENDM

;
; GET_MESSAGE_PTR msgnum, reg, msgreg
;
;   msgnum is the message number to retrieve, which can be a message constant
;   or a register.  Using a register is discouraged, however, because it
;   causes bloated code to be generated.
;
;   reg is the register to receive the result.
;
;   msgreg (optional) is a register which will be added to msgnum to produce
;   the actual message number.  If you want to pass a variable as msgnum,
;   you should instead pass a constant as msgnum and the offset from msgnum
;   as the msgreg.
;
; Valid input
;
;   GET_MESSAGE_PTR MSG_X, esi          - returns pointer to message X
;   GET_MESSAGE_PTR MSG_X, esi, eax     - returns pointer to message X+EAX
;   GET_MESSAGE_PTR eax,   esi          - returns pointer to message EAX
;                                         (larger than others)

GET_MESSAGE_PTR MACRO   msgnum:REQ, reg:REQ, msgreg
    LOCAL Done

    IF (OPATTR(msgnum)) AND 010h ; if msg number is in a register with no hint
        % forc c, @MSG_CLASSNAMES
                @MSG_GetOneRegClass c, msgnum, reg, Done
        endm
      Done:
    ELSE                        ; We have a hint; exploit it!
        IFB <msgreg>
                @MSG_regstr TEXTEQU <>
        ELSE
                @MSG_regstr TEXTEQU <+&msgreg*2>
        ENDIF

        % forc c, @MSG_CLASSNAMES
             @MSG_GetOneConstClass c, <msgnum>, reg
        endm
    ENDIF
ENDM

;==============================================================================
;
; Macros for sprintf'ing messages
;
;==============================================================================

; @MSG_Lsprintf_Helper: Internal helper macro for LOCALIZED_SPRINTF
;
;   c = message class to try
;   msgnum = constant message number

@MSG_Lsprintf_Helper MACRO c, msgnum
    IF (msgnum AND @MSG_CLASSMASK) EQ @MSG_&c&CLASSMASK
        @MSG_callparams EQU <offset32 @MSG_&c&AB>
    ENDIF
ENDM

;
; LOCALIZED_SPRINTF
;
;   Usage:  LOCALIZED_SPRINTF buf, msgno, parms
;
;           buf   = where to put the output string
;           msgno = message number (must be a constant)
;           parms = optional parameters to the sprintf
;
;
;   Returns: None
;
;   Uses:   Flags

LOCALIZED_SPRINTF MACRO buf:REQ, msgnum:REQ, parms:VARARG

        @MSG_callparams textequ <ERROR: Bad message number>
        % forc c, @MSG_CLASSNAMES
             @MSG_Lsprintf_Helper c, <msgnum>
        endm

        @MSG_callparams CATSTR <buf, >, @MSG_callparams, <, (msgnum) AND @MSG_MSGMASK, >, <parms>
        VMMCall _LocalizeSprintf, <%@MSG_callparams>

ENDM

;
; PUSH_SPRINTF
;
;   Usage:  PUSH_SPRINTF msgno, parms
;
;           msgno = message number
;           parms = optional parameters to the sprintf
;
;   The message number may be a constant (in which case it can be a
;   message from any class), or it may be held in a register (in
;   which case it *MUST* be a message ordinal from the LOCKED message
;   class).
;
;   If you want to pass a message number in a register, and the message
;   number is *NOT* for a locked message, then you must call one of the
;   PUSH_XXX_SPRINTF macros, such as PUSH_INIT_SPRINTF.
;
;   Returns: EAX -> output buffer (allocated from the stack)
;
;   Uses:   EAX, ESP, Flags
;
;   WARNING!  This services changes ESP by an unpredictable amount.
;   Any ESP-based parameters and/or local variables will be inaccessible
;   until you do a POP_SPRINTF.

PUSH_SPRINTF MACRO msgnum:REQ, params:VARARG

        @MSG_numvars = 2

  IFNB <params>
    FOR i, <params>
        @MSG_numvars = @MSG_numvars + 1
    ENDM
  ENDIF
  @MSG_numvarbyteshi = (@MSG_numvars*4) SHL 16

  IF (OPATTR(msgnum)) AND 010h        ; if msg number is in a register
        add     msgnum, @MSG_numvarbyteshi
        @MSG_callparams CATSTR <offset32 @MSG_LAB, msgnum, >, <params>
  ELSE
    @MSG_callparams textequ <ERROR: Bad message number>
    % forc c, @MSG_CLASSNAMES
         @MSG_Lsprintf_Helper c, <msgnum>
    endm
    @MSG_callparams CATSTR @MSG_callparams, \
                    <, @MSG_numvarbyteshi + (msgnum AND @MSG_MSGMASK), >, <params>
  ENDIF

      @MSG_PushCParams <%@MSG_callparams>
      VMMCall _LocalizeStackSprintf

ENDM


;
; @MSG_PUSH_SPRINTF is a common worker for the PUSH_XXX_SPRINTF macros.
;

@MSG_PUSH_SPRINTF MACRO c:req, msgnum:REQ, params

        @MSG_numvars = 2
  IFNB <params>
    FOR i, <params>
        @MSG_numvars = @MSG_numvars + 1
    ENDM
  ENDIF
  @MSG_numvarbyteshi = (@MSG_numvars*4) SHL 16

  IF (OPATTR(msgnum)) AND 010h        ; if msg number is in a register
        add     msgnum, @MSG_numvarbyteshi-@MSG_&c&CLASSMASK
        @MSG_callparams CATSTR <offset32 @MSG_&c&AB, msgnum, >, <params>
  ELSE
      .erre (msgnum AND @MSG_CLASSMASK) EQ @MSG_&c&CLASSMASK
        @MSG_callparams CATSTR <offset32 @MSG_&c&AB, @MSG_numvarbyteshi + msgnum -@MSG_&c&CLASSMASK, >, <params>
  ENDIF
      @MSG_PushCParams <%@MSG_callparams>
      VMMCall _LocalizeStackSprintf

ENDM

;
; POP_SPRINTF
;
;   Usage:  POP_SPRINTF
;
;       This cleans the stack after a previous PUSH_SPRINTF or
;       PUSH_XXX_SPRINTF call.
;
;   Returns: None
;
;   Uses:   Flags

POP_SPRINTF MACRO
;BUGBUG add signature and assertion for debug
                add   esp, [esp]            ; Last item in block pushed is the size of the data block
                ENDM


PUSHED_SPRINTF_LENGTH MACRO reg:REQ
;BUGBUG add signature and assertion for debug
                mov   &reg, [esp+4]         ; Next to last item pushed is the length of the string
                ENDM


FATAL_ERROR_MSG MACRO MsgNum:REQ, ExitFlags
        GET_MESSAGE_PTR <&MsgNum>, esi
IFB <ExitFlags>
        xor     eax, eax
ELSE
        mov     eax, ExitFlags
ENDIF
        VMMcall Fatal_Error_Handler
        ENDM


FATAL_ERROR_SQZD_MSG MACRO MsgNum:REQ, ExitFlags
        PUSH_SPRINTF <&MsgNum>
        xchg    esi, eax
IFB <ExitFlags>
        xor     eax, eax
ELSE
        mov     eax, ExitFlags
ENDIF
        VMMcall Fatal_Error_Handler
        ENDM

;***    PushCParams
;
;       Processes argument list

@MSG_PushCParams macro arglst
        LOCAL   count
        LOCAL   count2

        count = 0                     ;; number of dwords on stack (global)
        FOR     x,<arglst>
                ifnb <x>
                    count = count + 1
                    @MSG_MakePush <x>,%count
                endif
        ENDM

        count2 = count
        REPEAT  count
                @MSG_DoPush <?AM>,%count2
                count2 = count2 - 1
        ENDM
        ENDM


; Makes a macro that will push argment when invoke - used by cCall only

@MSG_MakePush   macro   name, num
        .xcref
        .xcref  ?AM&num
        .cref
        ?AM&num &macro
                push    name
                &endm
        endm
        .xcref  @MSG_MakePush

; Concatenates, invokes and purges a macro name - used by PushCParams

@MSG_DoPush macro   name1, name2
        name1&name2
        purge   name1&name2
        endm
        .xcref  @MSG_DoPush
