;/**[f******************************************************************
; * trig.a - 
; *
; * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
; * Copyright (C) 1989-1992  Microsoft Corporation.
; * Company confidential.
; *
; **f]*****************************************************************/

; peterwo - 12-17-91  bug fix and simplify this using a little trig.

; basically, we have a table of sines spanning angles from 0 to 90 degs.
; our task is to convert any sin or cos of any angle to
; a sin of an angle from 0 to 90 degs.

; how to do this:
; cos(phi) = sin(phi + 90)      //  (a) convert cosines to sines

; sin(phi) = - sin(-phi)        //  (b) convert negative angles to positive

; sin(phi) = sin(phi % 360)     //  (c) convert angle to range (0 < phi < 360)

; if (180 < phi < 360)
;    sin(phi) = - sin(phi - 180)  //  (d) convert angle to range 
;                                 //        (0 < phi < 180)

; if (90 < phi < 180)
;    sin(phi) = sin(180 - phi)  //  (e) convert angle to range (0 < phi < 90)





	.xlist
	include cmacros.inc
	.list


	ExternFP Scale



createSeg _TRIG,nres,byte,public,CODE
sBegin	nres
assumes cs,nres
assumes ds,nothing


	include vecttext.h		; C and MASM inc file
	include vttable.h		; C and MASM inc file


cProc	RCos,<FAR,PUBLIC>

	parmw	R
	parmw	Angle

cBegin

        mov     ax,Angle
        add     ax, 900                 ; rule (a)
	jmp	short ComputeSin
cEnd    <nogen>


cProc	RSin,<FAR,PUBLIC>

	parmw	R
	parmw	Angle

cBegin

        mov     ax,Angle
ComputeSin:
; separate sign from Angle
        cwd                             ; save sign
        xor     ax,dx                   ; take absolute value
        sub     ax,dx                   ;

        mov     bx,dx                   ; save sign
        xor     dx,dx                   ; clear out high word
        mov     cx,3600                 ; divide by period of sin/cos
        div     cx                      ; get modulo in dx
        mov     ax,dx                   ; ax now contains a postive angle
                                        ; between 0 and 360 degrees
        cmp     ax,1800
        jle     LessThan180
        sub     ax,1800                 ; rule (d)
        xor     bx,0FFFFh               ; reverse sign

LessThan180:
        cmp     ax,900
        jle     LessThan90
        sub     ax,1800                 ; rule (e)
        neg     ax

LessThan90:                             ; ax is now between 0 and 90 degrees
        mov     dx,bx                   ; RTable expects the sign in dx


;	RTable - ...
;
;	Compute (R * vttable[index]) / (10000)
;
;	Currently:
;		dx = negate flag  0 or FFFF
;		ax = index



	push	dx			;Save negation flag

	ifdef	INTERPOLATEDANGLES	;If interpolation

	push	di			;Interpolation
	cwd				;Compute Index/10
	mov	cx,10
	div	cx
	mov	bx,ax
	shl	bx,1
	mov	di,vttable[bx]		;Get base value
	mov	ax,vttable+2[bx]
	sub	ax,di
	cCall	Scale,<dx,ax,cx>
	add	ax,di
	mov	cx,10000
	cCall	Scale,<R,ax,cx>
	pop	di

	else

	mov	bx,ax
	shl	bx,1			;Make it a word index
	mov	cx,10000
	cCall	Scale,<R,vttable[bx],cx>

	endif


	pop	dx			;Negate result if needed
	xor	ax,dx
	sub	ax,dx



cEnd	RSin

sEnd	nres
end
