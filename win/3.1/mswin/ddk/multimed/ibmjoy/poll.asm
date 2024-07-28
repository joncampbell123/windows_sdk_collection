;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   poll.asm
;
;   Copyright (c) 1991-1992 Microsoft Corporation.  All Rights Reserved.
;
;   This module is a device driver for the IBM game adapter board.
;   The card MUST be configured at 201H
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        .286

        .xlist
        include cmacros.inc                   
        include windows.inc
        include mmsystem.inc
        include mmddk.inc
        .list

?PLM=1  ; Pascal calling convention
?WIN=1  ; Windows prolog/epilog code

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   external functions
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        externFP LocalInit           ; in KERNEL
        externNP LibMain             ; C code to do DLL init

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   equates and structure definitions
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

JOYPORT       equ 0201H             ; port address

JOY1_XY_MASK  equ <0001h + 0200h>   ; mask for joystick 1 X, and Y port bits
JOY2_XY_MASK  equ <0004h + 0800h>   ; mask for joystick 2 X, and Y port bits
JOY2_X_MASK   equ <0004h>           ; mask for joystick 2 X port bits
JOY2_Y_MASK   equ <0800h>           ; mask for joystick 2 Y port bits

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   data segment
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

sBegin DATA

; stuff defined in C module

externW     _JoyCal
externW     _gwXpos
externW     _gwYpos
externW     _gwButtons
externD     _gdwTimeout

_wXpos_hi   dw  ?
_wYpos_hi   dw  ?

sEnd DATA

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   code segment
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

sBegin CODE

        assumes cs, CODE
        assumes ds, DATA

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL
;
; @api void | Poll | Polls the device to find its position and 
;     the state of the buttons etc.
;
; @rdesc AX == 0 iff unpluged, AX != 0 otherwise.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
cProc Poll <PUBLIC> <si, di>

    parmW   id
    parmW   zOnly
    localW  wClear                  

    cBegin

    ; first we'll get the button state

    mov     dx, JOYPORT              ; get whole port state
    in      al, dx
    mov     cl, 4                    ; shift 4 bits for J1 button state
    cmp     id, 0
    jz      @f
    add     cl, 2                    ; shift 6 bits for J2 button state
@@:
    shr     al, cl
    and     ax, 3                    ; off all other bits to make sure
    xor     ax, 3                    ; invert sense (port gives reverse logic)
    mov     _gwButtons, ax           ; save it
    
    ; now we'll poll to get the position information

if 1
    ; Note that in general you should avoid disabling interrupts whenever
    ; possible.  If you don't have a good reason to do this, then don't.
    ; In this case, the joysticks aren't auto-triggering and latched,
    ; and things happen too quickly for timers, so we spin in a loop and
    ; wait for bits to flip. The code will operate with interrupts enabled,
    ; but with glitches in reported position due to interrupts serviced 
    ; while spinning in the timing loop. Normally, the timing loop is
    ; quick, but if you poll an unplugged joystick the system will
    ; halt until we reach _gdwTimeout.

    pushf       ; remember previous interrupt state
    cli
endif

    ; We poll the analog ports that we are interested in twice, once to
    ; make sure that the monostable vibrators have stabilized since the
    ; last time they were polled and once to actually read the positions.

    mov     wClear, 1                ; loop once to make sure bits are stable

real_poll:
    mov     dx, JOYPORT              ; get port state
    mov     si, offset _JoyCal       ; get calibration data
    
    mov     bx, JOY1_XY_MASK         ; set mask for x and y poll on J1
    cmp     id, 0                    ; Q: J1
    jz      @f                       ;  Y: use first struct in JoyCal
    cmp     zOnly, 0                 ; J2 - Q: poll z
    jnz     @f                       ;  Y: use first struct in JoyCal
    
    add     si, (size JOYCALIBRATE)  ;  N: use second struct in JoyCal
    mov     bx, JOY2_XY_MASK         ; set mask for x and y poll on J2

@@:
    cmp     zOnly, 0
    jz      poll_setxy
    jl      poll_setz_y

poll_setz_x:                         ; poll z position on x
    mov     ax, [si].jcal_wZbase
    neg     ax
    cwd
    mov     _gwXpos, ax              ; initialize the X position counter
    mov     _wXpos_hi, dx            ; used to check for roll-over error
    mov     cx, [si].jcal_wZdelta    ; initialize calibration registers
    mov     bx, JOY2_X_MASK          ; set mask for x poll only on J2
    jmp     poll_timers

poll_setz_y:                         ; poll z position on y
    mov     ax, [si].jcal_wZbase
    neg     ax
    cwd
    mov     _gwYpos, ax              ; initialize the Y position counter
    mov     _wYpos_hi, dx            ; used to check for roll-over error
    mov     di, [si].jcal_wZdelta    ; initialize calibration registers
    mov     bx, JOY2_Y_MASK          ; set mask for y poll only on J2
    jmp     poll_timers

poll_setxy:                          ; poll x and y positions
    mov     ax, [si].jcal_wXbase
    neg     ax
    cwd
    mov     _gwXpos, ax              ; initialize the X position counter
    mov     _wXpos_hi, dx            ; used to check for roll-over error

    mov     ax, [si].jcal_wYbase
    neg     ax
    cwd
    mov     _gwYpos, ax              ; initialize the Y position counter
    mov     _wYpos_hi, dx            ; used to check for roll-over error

    mov     cx, [si].jcal_wXdelta    ; initialize calibration registers
    mov     di, [si].jcal_wYdelta

poll_timers:
    mov     dx, JOYPORT              ; get port state
    cmp     wClear, 1
    je      poll_skipout             ; only start timer on second (real) poll
    xor     ax, ax
    out     dx, al                   ; start the timers (output a zero byte)

; bl  =   mask which represents X position bit state
; bh  =   mask which represents Y position bit state

poll_skipout:
    mov     ah, bl
    or      ah, bh                   ; ah = bit mask for both X and Y

; When reading joystick port, the bit for a pot starts at 1, and when
; the resistance value for a counter is greater than or equal to the
; pot's, the bit goes to zero, and stays there.

poll_loop:
    in      al, dx                   ; get position info
    and     al, ah                   ; only watch bits of interest
    jz      poll_done                ; counters hit zero?

poll_x_test:
    test    al, bl            ; is X bit still high (i.e. increment X counter)?
    jz      poll_y_test

; X potentiometer is still high, so keep counting up
    add     _gwXpos, cx
    adc     _wXpos_hi, 0

    push    ax                          ; save these while we check timeout
    push    dx

    mov     ax, _gwXpos                 ; test to see if we timed out
    mov     dx, _wXpos_hi
    cmp     word ptr _gdwTimeout+2, dx
    jg      poll_y_test_pop             ; make sure that this is a SIGNED
    jl      poll_x_overflow_pop         ; compare! extreme can be negative!
    cmp     word ptr _gdwTimeout, ax
    jae     poll_y_test_pop

poll_x_overflow_pop:          ; x (32bits) has overflowed gdwTimeout
    pop     dx
    pop     ax

poll_x_overflow:              ; x (32bits) has overflowed gdwTimeout
    xor     ax, ax            ; call this joystick unplugged
    jmp     poll_sti

poll_y_test_pop:
    pop     dx
    pop     ax

poll_y_test:
    test    al, bh            ; is Y bit still high (i.e. increment Y counter)?
    jz      poll_loop

; Y potentiometer is still high, so keep counting up
    add     _gwYpos, di
    adc     _wYpos_hi, 0

    push    ax                          ; save these while we check timeout
    push    dx

    mov     ax, _gwYpos                 ; test to see if we timed out
    mov     dx, _wYpos_hi
    cmp     word ptr _gdwTimeout+2, dx
    jg      poll_loop_pop               ; make sure that this is a SIGNED
    jl      poll_y_overflow_pop         ; compare! extreme can be negative!
    cmp     word ptr _gdwTimeout, ax
    jae     poll_loop_pop

poll_y_overflow_pop:          ; y (32bits) has overflowed gdwTimeout
    pop     dx
    pop     ax

poll_y_overflow:              ; y (32bits) has overflowed gdwTimeout
    xor     ax, ax            ; call this joystick unplugged
    jmp     poll_sti

poll_loop_pop:
    pop     dx
    pop     ax
    jmp     poll_loop

poll_done:                    ; end of poll loop
    dec     wClear                  
    jnz     poll_sti          ; stop if this was the second (real) poll 
    jmp     real_poll         ; otherwise go back and do the real one

poll_sti:

if 1
    pop     bx                ; flags register (from pushf)
    test    bx, 0200h         ; were interrupts enabled before?
    jz      poll_fix_x
    sti                       ; if so, reenable them
endif

poll_fix_x:
    xor     bx, bx
    cmp     _wXpos_hi, bx
    jz      poll_fix_y
    jl      @f
    dec     bx
@@:
    mov     _gwXpos, bx       ; minimum value is 0, maximum is 0xffff

poll_fix_y:
    xor     bx, bx
    cmp     _wYpos_hi, bx
    jz      poll_fix_done
    jl      @f
    dec     bx
@@:
    mov     _gwYpos, bx       ; minimum value is 0, maximum is 0xffff

poll_fix_done:
    mov     al, ah            ; ax = 0 for unplugged joystick
    xor     ah, ah            ; and nonzero for good read

    cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL
;
; @asm LibEntry | Called when DLL is loaded.
;
; @reg  CX | Size of heap.
;
; @reg  DI | Module handle.
;
; @reg  DS | Automatic data segment.
;
; @reg  ES:SI | Address of command line.
;
; @rdesc AX is TRUE if the load is successful and FALSE otherwise.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
cProc LibEntry <FAR, PUBLIC, NODATA>, <>

        cBegin

        ; push frame for LibMain (hModule, wDataSeg, cbHeap, lpszCmdLine)

        push di                 ; hInstance
        push ds                 ; data segment
        push cx                 ; heap size
        push es                 ; command line seg
        push si                 ; command line off

        ; init the local heap (if one is declared in the .def file)

        jcxz no_heap

        cCall LocalInit, <0, 0, cx>

no_heap:
        cCall LibMain

        ; ax now has return value from LibMain

        cEnd

sEnd CODE

        end LibEntry
