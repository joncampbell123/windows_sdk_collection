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

	page	,132
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; SetSpeed.asm
;
; Sets 'typematic' speed of AT-type keyboards (type code 3 or 4 in this
; driver).
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

include keyboard.inc

if1
	%out .. setspeed enabled

endif

sBegin DATA
        extrn   KeyType:byte    ; keyboard type
        extrn   PCType:byte

sEnd DATA

createSeg _SETSP, SETSP, BYTE, PUBLIC, CODE
sBegin	SETSP        ; Beginning of code segment
assumes	CS,SETSP
assumes	DS,DATA

.386
;-----------------------------------------------------------------------;
; set_typematic_and_delay                                               ;
; Arguments:                                                            ;
;       rate_of_speed   an integer value of which the lowest 5 bits define
;                       the desired rate of repeat.  OR -1 for inquiring
;                       speed-setting capability
; Returns:                                                              ;
;       if rate_of_speed == -1
;               AX = -1 if not speed capable 
;               AX = 0 if yes capable
;       if rate_of_speed != -1
;               AX = speed actually set on keyboard
;               AX = -1 if unsuccessful                                         ;
; Alters:                                                               ;
;       AX,BX,CX,DX                                                     ;
; Notes:
;       Checking of parameter range is now done.
; Calls:                                                                ;
;       send_to_keyboard                                                ;
; History:                                                              ;
;  March 2, 1988        by      Ron Gery
; brought this over from speedkey
;                                                                       ;
;  Mon Nov 18 20:53:39 1985    -by-    Wes Rupel    [wesleyr]           ;
; Wrote it!                                                             ;
;-----------------------------------------------------------------------;
        assume  es:nothing

; keyboard type definitions (as used in init. code):

KB_RT      	equ	4	; RT = Enhanced = 101/102 keyboard
KB_AT		equ	3	; AT = 84 or 86 key keyboard

; table for getting the keyboard setting based on user input in 0-31 range,
; where 0 is slowest and 31 is fastest


rate_table      db      31              ;0
                db      31              ;1
                db      31              ;2
                db      26              ;3
                db      23              ;4
                db      20              ;5
                db      18              ;6
                db      17              ;7
                db      15              ;8
                db      13              ;9
                db      12              ;10
                db      11              ;11
                db      10              ;12
                db      9               ;13
                db      9               ;14
                db      8               ;15
                db      7               ;16
                db      6               ;17
                db      6               ;18
                db      5               ;19
                db      4               ;20
                db      4               ;21
                db      3               ;22
                db      3               ;23
                db      2               ;24
                db      2               ;25
                db      1               ;26
                db      1               ;27
                db      1               ;28
                db      0               ;29
                db      0               ;30
                db      0               ;31

cProc   SetSpeed,<LOADDS, FAR,PASCAL,PUBLIC>
 parmW  rate_of_speed
cBegin

	cmp	PCType, IBMPS280ID	; PS-2 mod 80?
	je	SS_good_system
	cmp     PCType, IBMATID         ; works only on AT-type system
	jne     SS_error_return
;	cmp     KeyType, KB_RT          ; must have enhanced keyboard, or
;	je      SS_good_system
;	cmp     KeyType, KB_AT          ; 84(IBM)/86(Olivetti) key AT keyboard.
;	jne     SS_error_return

SS_good_system:
        mov     dx,rate_of_speed        ; get parameter from stack.

        inc     dx                      ; is it -1 (capability inquire)
	jnz	do_some_work		; no, we need to set this
	xor	ax,ax			; yes, we are capable
	jmp	short SS_end		; and return

do_some_work:
        dec     dx                      ; restore to original
	and	dx,001Fh		; keep the 5 low bits for key repeat

        mov     al,0F3h
        cli
        call    send_to_keyboard
        jcxz    SS_sti_error_return     ; error, but don't forget sti

        lea     bx,rate_table           ; --> rate table
        add     bx,dx                   ; --> table entry we want
        mov     al, byte ptr cs:[bx]    ; AL = rate we want

	mov	bx, rate_of_speed
	and	bl, 060h		; keep the hi 3 bits of the byte
	or	al, bl

        call    send_to_keyboard

SS_sti_error_return:
        sti
        jcxz    SS_error_return

SS_normal_return:
	mov	ax, rate_of_speed	; return what was passed in
        jmp     short SS_end

SS_error_return:
        mov     ax,-1                   ; error return

SS_end:
cEnd

;-----------------------------------------------------------------------;
; send_to_keyboard                                                      ;
;                                                                       ;
; Arguments:                                                            ;
;       AL = data to send.                                              ;
; Returns:                                                              ;
;       CX = 0  if  send failed.                                        ;
; Alters:                                                               ;
;       AX                                                              ;
; Calls:                                                                ;
;       nothing                                                         ;
; History:                                                              ;
;  Wed 30-Nov-1988 22:31:38  -by-  David N. Weise  [davidw]		;
; Made it work in protect mode windows as well.  Gee it's great to      ;
; back in some old DSR code!!						;
;!!! I think the idea about waiting for the ACK is to make sure that	;
;!!! we aren't speaking to the controller too soon again.               ;
;
;  April 8, 1988	by	Ron Gery
; take out check for 'ACK' after send.  we know it's all legal.
;
;  March 2, 1988        by      Ron Gery
; brought this over from speedkey
;                                                                       ;
;  Mon Nov 18 20:56:05 1985    -by-    Wes Rupel    [wesleyr]           ;
; Wrote it! (inspired by Tech Journal, vol.3, no.5, p39).               ;
;-----------------------------------------------------------------------;

        public  send_to_keyboard

send_to_keyboard        proc    near
        sub     cx,cx
        mov     ah,al
wait_for_controller_to_listen:
	in	al,kb_status
        and     al,02h
        loopnz  wait_for_controller_to_listen
        jcxz    send_failed
        mov     al,ah
	out	kb_data,al
        sub     cx,cx
wait_for_controller_to_read_data:
        jmp     short $+2                 ; Delays between io instructions. 
        jmp     short $+2                 ; Fixs compaq keyboard problem

	in	al,kb_status
        and     al,02h
        loopnz  wait_for_controller_to_read_data
        jcxz    send_failed
        sub     cx,cx
wait_for_acknowledge:
	in	al,kb_status
        and     al,01h
        loopz   wait_for_acknowledge
        jcxz    send_failed

        mov     cx,100
        ;
        ; We need to let some time elapse before we try to read the data
        ; because of a problem running this code in the DOS box under
        ; OS/2. The wait here needed to be longer than several standard
        ; jmp $'s so I used the loop. The bug described here was only
        ; observed on PS/2 model 55sx and model 70 machines.
        ;
        ; Mikecole.
        ;
wait_for_data:
        loop    wait_for_data

	in	al,kb_data
        inc     cx              ; can't return cx=0 if we get here !!
;*      cmp     al,0FAh	 	; take out the check for 'ACK'
;*      jz      send_confirmed
;*      sub     cx,cx
send_confirmed:
send_failed:
        ret                   
send_to_keyboard        endp

.286p
sEnd  SETSP

        end
