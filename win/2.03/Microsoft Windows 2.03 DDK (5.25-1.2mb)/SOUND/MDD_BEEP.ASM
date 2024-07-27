?DF = 1                ; don't use default segments
.xlist
include cmacros.inc
.list
erofm           EQU     -2      ;/* out of memory */
externFP        MyOpenSound
externFP        <GetModuleHandle,GetCodeHandle>

_TEXT   segment BYTE PUBLIC 'CODE'
_TEXT   ends


createSeg   _BEEP,CODE,BYTE,PUBLIC,CODE

sBegin  CODE
        assumes cs,code

szSound db      "SOUND",0

;-----------------------------------------------------------------------
;
;   This portion of OPENSOUND insures there is enough memory to load the
;   rest of the sound driver, if not, it returns an error
;
cProc   OpenSound,<PUBLIC,FAR>,<si,di>
cBegin
        mov     ax,codeOffset szSound
        regptr  csax,cs,ax
        cCall   GetModuleHandle,<csax>
        mov     bx,1                            ; make sure seg #1 is loaded
        cCall   GetCodeHandle,<ax,bx>
        mov     cx,ax
        mov     ax,erOFM
        jcxz    ops1
        push    cs                      ; push return address
        mov     ax,codeOffset ops1
        push    ax

        push    cx
        mov     ax,offset _TEXT:MyOpenSound
        push    ax
        ret                     ; call MyOpenSound
ops1:
cEnd
;-----------------------------------------------------------------------
;
;       Name:           beep - Beep the speaker
;
;       Call:           beep(int)
;
;       Purpose:        Make some noise
;
;       Entry:          Type of beep, currently ignored
;
;       Exit:           none
;
;       Modified:       AX,BX,CX,DX
;
;-----------------------------------------------------------------------


    PUBLIC  fReEnter
fReEnter        db      0

; This code was changed to fix a bug in the DoBeep routine.  We must let
; DoBeep know when the sound driver is actually making sound.  DoBeep
; will not screw with the speaker in this case.  The sound driver can,
; however do things while DoBeep has the speaker going.  This is not
; fatal and will simply result in some funny sounds.  Without this fix,
; DoBeep could go into continuous beep mode and never turn the speaker
; off. (Sat 10-Oct-1987 : bobgu)


cProc   DoBeep,<PUBLIC,FAR>
cBegin  nogen
        xor     ax,ax                   ; indicate failure to beep

        inc     cs:[fReEnter]
	cmp	cs:[fReEnter],1 	; first time in ?
        jnz     BeepX                   ; no, quit

        mov     al,0B6H         ; select timer 2
        out     43h,al
        mov     ax,0533H        ; divisor for 1000 hz
        out     42h,al          ; write timer 2 count - lsb
        mov     al,ah
	out	42h,al		; write timer 2 count - msb
	jmp	short beep1

beep1:
	cli
	in	al,61h		; get current setting of port
        or      al,00000011b    ; turn speaker on
        out     61h,al
	sti

        mov     cx,2400H        ; wait awhile
beep2:
	loop	beep2

        mov     cx,0633H        ; new tone
        mov     al,cl
        out     42H,al
        mov     al,ch
        out     42H,al

        mov     cx,3400H        ; wait again
beep3:
	loop	beep3

; Just turn the speaker off.  We can do this now since we never get
; here if the sound driver was making a sound when we entered.
	cli
	in	al,61h		; get current setting of port
	and	al,11111100b	; turn speaker off
        out     61h,al
	sti

        mov     al,1            ; indicate no errors
beepX:
	dec	cs:[fReEnter]
        ret     2
cEnd    nogen
sEnd    CODE
        end
