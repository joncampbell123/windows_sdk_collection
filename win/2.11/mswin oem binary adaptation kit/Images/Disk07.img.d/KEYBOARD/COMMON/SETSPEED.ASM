;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; SetSpeed.asm
;
; Sets 'typematic' speed of AT-type keyboards (type code 3 or 4 in this
; driver).
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; History: (plb[Olivetti] = peterbel[Microsoft])
;
; 19 apr 88	peterbe	84/86 keyboard is type 3, not type 2!
; 
; 21 mar 88     plb     This version has fixed delay of 500 MS (1)
; 18 mar 88     plb     Now  check for only IBMATID and keyboard type,
;                       but for ALL calls to SetSpeed. Took out delays
;                       I put in yesterday.
; 17 mar 88     plb     Updated to include 'speed capable' check.
;                       Added check for IBMPS2ID, IBMATID
; 15 mar 88     plb     extracted from USA KEYBOARD.ASM
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

include keyboard.inc
include olikbd.inc

        ExternFP        GetProfileInt

sBegin DATA
        extrn   KeyType:byte    ; keyboard type
        extrn   PCType:byte

sEnd DATA

sBegin  CODE        ; Beginning of code segment
assumes CS,CODE
assumes DS,DATA

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

cProc   SetSpeed,<FAR,PASCAL,PUBLIC>
 parmW  rate_of_speed
cBegin
	cmp     PCType, IBMATID         ; works only on AT-type system
        jne     SS_error_return
        cmp     KeyType, KB_RT          ; must have enhanced keyboard, or
        je      SS_good_system
        cmp     KeyType, KB_AT          ; 84(IBM)/86(Olivetti) key AT keyboard.
        jne     SS_error_return

SS_good_system:
        mov     dx,rate_of_speed        ; get parameter from stack.

        inc     dx                      ; is it -1 (capability inquire)
        jz      SS_normal_return        ; yes... return capable

        dec     dx                      ; restore to original

        jns     SS_not_low              ; is the speed negative?
        xor     dx,dx                   ; minimum speed
        jmp     short SS_set_the_speed

SS_not_low:
        cmp     dx,31
        jle     SS_set_the_speed
        mov     dx,31                   ; maximum speed

SS_set_the_speed:

        mov     al,0F3h
        cli
        call    send_to_keyboard
        jcxz    SS_sti_error_return     ; error, but don't forget sti

        lea     bx,rate_table           ; --> rate table
        add     bx,dx                   ; --> table entry we want
        mov     al, byte ptr cs:[bx]    ; AL = rate we want
                        
        or      al, 20h                 ; or in delay value

        call    send_to_keyboard

SS_sti_error_return:
        sti
        jcxz    SS_error_return

SS_normal_return:
        mov     ax,dx                   ; DX = speed value we used
        jmp     short SS_end

SS_error_return:
        mov     ax,-1                   ; error return

SS_end:
cEnd

        public  send_to_keyboard
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
;  April 8, 1988	by	Ron Gery
; take out check for 'ACK' after send.  we know it's all legal.
;  March 2, 1988        by      Ron Gery
; brought this over from speedkey
;                                                                       ;
;  Mon Nov 18 20:56:05 1985    -by-    Wes Rupel    [wesleyr]           ;
; Wrote it! (inspired by Tech Journal, vol.3, no.5, p39).               ;
;-----------------------------------------------------------------------;

send_to_keyboard        proc    near
        sub     cx,cx
        mov     ah,al
wait_for_controller_to_listen:
        in      al,64h
        and     al,02h
        loopnz  wait_for_controller_to_listen
        jcxz    send_failed
        mov     al,ah
        out     60h,al
        sub     cx,cx
wait_for_controller_to_read_data:
        in      al,64h
        and     al,02h
        loopnz  wait_for_controller_to_read_data
        jcxz    send_failed
        sub     cx,cx
wait_for_acknowledge:
        in      al,64h
        and     al,01h
        loopz   wait_for_acknowledge
        jcxz    send_failed
        in      al,60h
;*      cmp     al,0FAh	 	; take out the check for 'ACK'
;*      jz      send_confirmed
;*      sub     cx,cx
send_confirmed:
send_failed:
        ret                   
send_to_keyboard        endp


sEnd  CODE

        end

