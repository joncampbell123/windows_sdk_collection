	.xlist
	include cmacros.inc
	.list


	ExternFP Scale



createSeg _TRIG,nres,byte,public,CODE
sBegin	nres
assumes cs,nres
assumes ds,nothing


	include vecttext.h
	include vttable.inc



cProc	RSin,<FAR,PUBLIC>

	parmw	R
	parmw	Angle

cBegin

	xor	dx,dx			;Don't want result negated
	mov	ax,Angle
	mov	bx,ax			;If Angle <= 900
	sub	bx,900			;  return RTable(R,Angle)
	jle	RCos20

	sub	ax,1800 		;If Angle <= 1800
	jle	RCos10			;  return RTable(R,1800-Angle)

	dec	dx			;Want result negated
	sub	bx,1800 		;If Angle <= 2700
	jle	RCos20			;  return -RTable(R,Angle-1800)
	sub	ax,1800 		;Return -RTable(R,3600-Angle)
	jmp	short RCos10


cEnd	<nogen>






cProc	RCos,<FAR,PUBLIC>

	parmw	R
	parmw	Angle

cBegin

	xor	dx,dx			;Don't want result negated
	mov	ax,Angle		;If Angle <= 900
	sub	ax,900			;  return RTable(R,900-Angle)
	jle	RCos10

	dec	dx			;Want result negated
	mov	bx,ax			;If Angle <= 1800
	sub	bx,900			;  return -RTable(R,Angle-900)
	jle	RCos20

	sub	ax,1800 		;If Angle < 2700
	jl	RCos10			;  return -RTable(R,2700-Angle)

	inc	dx			;Don't want result negated
	jmp	short RCos20		;  return RTable(R,Angle-2700)


RCos10:
	neg	ax

RCos20:
;	jmp	RTable
	errn$	RTable




;	RTable - ...
;
;	Compute (R * vttable[index]) / (10000)
;
;	Currently:
;		dx = negate flag
;		ax = index


RTable:

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



cEnd	RCos

sEnd	nres
end
