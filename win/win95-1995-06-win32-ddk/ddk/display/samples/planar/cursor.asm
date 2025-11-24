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

title   Exported cursor functions Windows 3.1 VGA/EGA Display drivers
.286c

.xlist
include CMACROS.INC
include WINDEFS.INC
include MFLAGS.INC
include CURSOR.INC
.list

subttl  Data Area for Set Cursor Routines
page +
sBegin  Data
globalW CURSOR_X_COORDINATE,0
globalW CURSOR_Y_COORDINATE,0
globalW UNDONE_X_COORDINATE,0
globalW UNDONE_Y_COORDINATE,0
globalB CURSOR_STATUS,IS_VISIBLE+IS_NULL
globalW CURSOR_X_SIZE,0
globalW CURSOR_Y_SIZE,0
globalW X_HOT_SPOT,0
globalW Y_HOT_SPOT,0
globalW wInhibitCount,0
externW Qsize                           ;MouseBlur Queue size.
sEnd    Data

IS_EXCLUDED     equ 000000001b          ;Cursor is excluded.
IS_BUSY         equ 000000010b          ;Critical section.
IS_NULL         equ 000000100b          ;Cursor is null.
HAS_MOVED       equ 000001000b          ;Cursor has moved.
IS_VISIBLE      equ 000010000b          ;Cursor is visible.

subttl  The SetCursor Code
page +
sBegin  Code
assumes cs,Code
assumes ds,Data

externNP        CursorOff
externNP        LoadCursor
externNP        DrawCursor
externNP        ExcludeTest
externNP        EraseTail
externNP        unexclude

;--------------------------------------------------------------------------
; SetCursor
;--------------------------------------------------------------------------
cProc   SetCursor, <PUBLIC, FAR>, <si, di>
	parmD   lpCursorShape
	localB  bStatus

	org     $+1
_cstods label   word
	org     $-1
	public  _cstods

cBegin
	mov     cl,IS_BUSY              ;Tell MoveCursor and CheckCursor
	xchg    CURSOR_STATUS,cl        ;to not do anything.
	mov     bStatus,cl

	cmp     seg_lpCursorShape,0     ;is this a null pointer?
	jne     short SC_DrawACursor    ;no, go load and draw a cursor.
	test    bStatus,IS_NULL         ;is cursor already null?
	jnz     short SC_End            ;yep, we're done.
	or      bStatus,IS_NULL         ;Indicate cursor is null.
	call    CursorOff               ;erase the old cursor.
	and     bStatus,not IS_VISIBLE  ;Show the cursor is not visible.
	jmp     short SC_End            ;and we're done.

SC_DrawACursor:
	mov     ax,Qsize                ;If multiple trails are on 
	cmp     ax,1                    ;the screen, then turn them
	jz      short @f                ;off before redefining the cursor.
	call    CursorOff
@@:     and     bStatus,not IS_NULL     ;Show non-null cursor.
	push    ds                      ;save DS = Data
	mov     ax,ds                   ;make ES --> Data
	mov     es,ax                   
	lds     si,lpCursorShape        ;DS:SI points at our cursor
	lodsw                           ;get the X-hotspot
	mov     es:X_HOT_SPOT,ax        ;save it.
	lodsw                           ;get the Y-hotspot 
	mov     es:Y_HOT_SPOT,ax        ;save it.
	lodsw                           ;get the size in X
	mov     es:CURSOR_X_SIZE,ax     ;save it
	lodsw                           ;get the size in Y
	mov     es:CURSOR_Y_SIZE,ax     ;save it
	mov     di,ax                   ;this is size of our cursor
	add     si,4                    ;point at the cursor bits
@@:     call    LoadCursor              ;Load the new cursor image.
	pop     ds
	test    bStatus,IS_EXCLUDED     ;don't draw the cursor if 
	jnz     short SC_End            ;excluded.
	call    DrawCursor              ;Draw the new cursor.   
	or      bStatus,IS_VISIBLE      ;Show the cursor is visible.
SC_End:                                          
	mov     cl,bStatus
	xchg    cl, CURSOR_STATUS
	mov     ax,1                    ;return success code
cEnd     

;--------------------------------------------------------------------------
; CheckCursor
;--------------------------------------------------------------------------
cProc   CheckCursor, <FAR,PUBLIC>, <si,di>
cBegin   
	mov     cl, IS_BUSY
	xchg    CURSOR_STATUS, cl       ;get the state of the flag
					;upon entry to this routine, and
					;disable MoveCursor activity.

	;Should we try to bring the cursor back?

	test    cl,IS_BUSY              ;Are we exclusive?
	jnz     short CC_Exit           ;no, get out.

	test    cl,HAS_MOVED                    ;Has the cursor moved?
	jz      short @f                        ;If so, 
	mov     ax,UNDONE_X_COORDINATE          ;update with the most
	mov     CURSOR_X_COORDINATE,ax          ;recent position.
	mov     ax,UNDONE_Y_COORDINATE
	mov     CURSOR_Y_COORDINATE,ax
	and     cl,not HAS_MOVED
@@:     test    cl,IS_NULL              ;is cursor null?
	jnz     short CC_Exit           ;No need to draw it.

	test    cl,IS_VISIBLE           ;Have we drawn the cursor before?
	jnz     short CC_CheckTail      ;Yes, maybe remove the tail.

	or      cl,IS_VISIBLE           ;Indicate it is visible.
	push    cx
	call    DrawCursor              ;Bring the cursor back. 
	pop     cx

	test    cl,IS_EXCLUDED          ;Is cursor excluded?
	jz      short CC_Exit           ;no. Get out.
	call    ExcludeTest             ;Did we draw in the cursor exclusion
	jnc     short CC_Exit           ;region?
	push    cx                      ;Yes, remove the cursor.
	call    CursorOff       
	pop     cx
	and     cl,not IS_VISIBLE       ;Show that it is not visible.   
CC_Exit:
	xchg    cl,CURSOR_STATUS        ;get back the original state of
					;the draw busy flag
cEnd

CC_CheckTail:
	mov     ax,Qsize                ;Nothing special happens
	cmp     ax,1                    ;when the queue size is 1.
	je      short CC_Exit
	push    cx
	call    EraseTail
	pop     cx
	jmp     short CC_Exit

;--------------------------------------------------------------------------
; MoveCursor
;--------------------------------------------------------------------------
cProc   MoveCursor, <PUBLIC, FAR>, <si, di>
	parmW   absX
	parmW   absY
cBegin
	mov     cl,IS_BUSY              ;set draw busy and get current
	xchg    cl,CURSOR_STATUS        ;status into CL
	mov     ax,absX                 ;get X-coordinate
	mov     bx,absY                 ;get Y-coordinate
	or      ax,ax                   ;exceeding low X-limit?
	jg      MC_CHECK_HIGH_X_LIMIT   ;nope, go on
	xor     ax,ax                   ;fix cursor position to 0

MC_CHECK_HIGH_X_LIMIT:
	cmp     ax,SCREEN_WIDTH         ;exceeding high X-limit?
	jb      MC_CHECK_LOW_Y_LIMIT    ;nope, go on
	mov     ax,SCREEN_WIDTH-1       ;fix cursor position

MC_CHECK_LOW_Y_LIMIT:
	or      bx,bx                   ;exceeding low Y-limit?
	jg      MC_CHECK_HIGH_Y_LIMIT   ;nope, go on
	mov     bx,1                    ;fix cursor position

MC_CHECK_HIGH_Y_LIMIT:
	cmp     bx,SCREEN_HEIGHT        ;exceeding high Y-limit?
	jb      MC_MOVE_CURSOR          ;nope, go do the cursor
	mov     bx,SCREEN_HEIGHT-1      ;fix cursor position

MC_MOVE_CURSOR:                  
	test    cl,IS_BUSY              ;is it safe to move the cursor?
	jnz     DONT_MOVE_CURSOR        ;no, get out.

	mov     CURSOR_X_COORDINATE,ax  ;save this location
	mov     CURSOR_Y_COORDINATE,bx

	test    cl,IS_NULL              ;Is the cursor null?
	jnz     MOVE_CURSOR_EXIT        ;yes, nothing to draw.

	push    cx                      ;Preserve status across the call.
	call    DrawCursor
	pop     cx
	or      cl,IS_VISIBLE           ;Show we have drawn the cursor.

	test    cl,IS_EXCLUDED
	jz      short MOVE_CURSOR_EXIT

	call    ExcludeTest             ;Have we drawn in the exclusion region?
	jnc     short @f                ;No, we are done.
	push    cx                      ;Yes, remove the cursor.
	call    CursorOff       
	pop     cx
	and     cl,not IS_VISIBLE       ;Show that it is not visible.
@@:     jmp     short MOVE_CURSOR_EXIT

DONT_MOVE_CURSOR:
	or      cl,HAS_MOVED            ;Indicate the cursor has moved.
	mov     UNDONE_X_COORDINATE,ax  ;save this location
	mov     UNDONE_Y_COORDINATE,bx

MOVE_CURSOR_EXIT:                                                       
	xchg    cl,CURSOR_STATUS        ;restore the original state of
					;the cursor status flag
cEnd

if 0
;--------------------------------------------------------------------------
; October 31, 1991 - Removed this code because it causes the cursor to
; disapper (and/or blink) during GP fault recovery.  This code does 
; give a bit of a performance win, however, not a big win.
;--------------------------------------------------------------------------
; ORDINAL 105.
; InhibitCursorRedraw  -- User calls this when it expects that several
; back-to-back driver operations are going to occur.  If the driver
; has to exclude the cursor, it won't bring it back unless wInhibitCount is
; zero. 
; >0: Inhibit the cursor
;  0: UnInhibit the cursor
; <0: Reset the cursor
;--------------------------------------------------------------------------
cProc   InhibitCursorRedraw, <FAR,PUBLIC>
	parmW   wInhibit
cBegin   
	cmp     wInhibit,0      ;Is the command to Uninhibit the cursor?
	jz      short ICR_Uninhibit ;Zero means yes. 
	jl      short ICR_Reset ;negative means reset.
ICR_Inhibit:
	inc     wInhibitCount   ;Positive...Just increment wInhibitCount.
	jmp     ICR_Exit

ICR_Reset:
	xor     ax,ax
	mov     wInhibitCount,ax
	jmp     short @f
ICR_Uninhibit:
	dec     wInhibitCount   ;no. Decrement wInhibitCount and if we
	jnz     short ICR_Exit  ;go to zero, go unexclude the cursor.
@@:     cCall   unexclude
ICR_Exit:
cEnd    
endif

sEnd            Code
end
