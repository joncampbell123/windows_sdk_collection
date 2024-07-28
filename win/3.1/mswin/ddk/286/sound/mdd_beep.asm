?DF = 1                ; don't use default segments

.xlist
include cmacros.inc
include sound.inc
.list

externFP MyOpenSound
externFP GetModuleHandle
externFP GetCodeHandle

_TEXT   segment BYTE PUBLIC 'CODE'
_TEXT   ends

_DATA	segment BYTE PUBLIC 'DATA'
externB fReEnter
szSound db      "SOUND",0
_DATA	ends

createSeg   _BEEP,CODE,BYTE,PUBLIC,CODE

sBegin  CODE
        assumes cs,code

;-----------------------------------------------------------------------;
; OpenSound
;
;   This portion of OPENSOUND insures there is enough memory to
;   load the rest of the sound driver, if not, it returns an error.
;
; Entry:
;	none
;
; Returns:
;
; Registers Destroyed:
;
; History:
;  Sun 10-Sep-1989 10:29:28  -by-  David N. Weise  [davidw]
;
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

cProc   OpenSound,<PUBLIC,FAR>,<si,di>
cBegin
	mov	ax,offset _DATA:szSound
	cCall	GetModuleHandle,<ds,ax>
	mov	bx,1			; make sure seg #1 is loaded
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
	ret				; call MyOpenSound
ops1:
cEnd

;-----------------------------------------------------------------------;
; WEP
;
; The Windows exit procedure!
;
; Entry:
;	WORD
; Returns:
;
; Registers Destroyed:
;
; History:
;  Wed 13-Sep-1989 16:48:13  -by-  David N. Weise  [davidw]
; Wrote it!
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

cProc	WEP,<PUBLIC,FAR>
;	parmW	word_arg
cBegin nogen
	mov	ax,1
	ret	2
cEnd nogen


;-----------------------------------------------------------------------
;
; DoBeep
;
; Beeps the speaker.
;
; Entry:
;	none
;
; Returns:
;	nothing
;
; Registers Modified:
;	AX,BX,CX,DX
;
; History:
;  Mon 12-Dec-1988 11:10:27  -by-  David N. Weise  [davidw]
; Protect mode'ized it, and cleaned it up.
;
;  Sat 10-Oct-1987	     -by-  Bob Gunderson   [bobgu]
; This code was changed to fix a bug in the DoBeep routine.  We must let
; DoBeep know when the sound driver is actually making sound.  DoBeep
; will not screw with the speaker in this case.  The sound driver can,
; however do things while DoBeep has the speaker going.  This is not
; fatal and will simply result in some funny sounds.  Without this fix,
; DoBeep could go into continuous beep mode and never turn the speaker
; off.
;-----------------------------------------------------------------------

	assumes ds,nothing
	assumes es,nothing

cProc   DoBeep,<PUBLIC,FAR>
cBegin  nogen

	push	ds
	mov	ax,_DATA
	mov	ds,ax
	assumes ds,_DATA
	xor	ax,ax		; indicate failure to beep
	cmp	ds:fReEnter,al	; first time in ?
	pop	ds
	assumes ds,nothing
	jnz	BeepX		; no, quit

	mov	al,SQUARE	; select timer 2
	out	TMRCMD,al
	mov	ax,0533h	; divisor for 1000 hz
	pause
	out	TIMER2,al	; write timer 2 count - lsb
        mov     al,ah
	pause
	out	TIMER2,al	; write timer 2 count - msb
	jmp	short beep1

beep1:	cli
	in	al,SPEAKER	; get current setting of port
	or	al,SPKRON	; turn speaker on
	pause
	out	SPEAKER,al
	sti

	mov	cx,2400h	; wait awhile
beep2:	loop	beep2

	mov	cx,0633h	; new tone
        mov     al,cl
	out	TIMER2,al
        mov     al,ch
	pause
	out	TIMER2,al

	mov	cx,3400h	; wait again
beep3:	loop	beep3

; Just turn the speaker off.  We can do this now since we never get
; here if the sound driver was making a sound when we entered.

	cli
	in	al,SPEAKER	; get current setting of port
	and	al,NOT SPKRON	; turn speaker off
	pause
	out	SPEAKER,al
	sti
	mov	al,1		; indicate no errors

beepX:	ret	2

cEnd nogen

sEnd    CODE

        end
