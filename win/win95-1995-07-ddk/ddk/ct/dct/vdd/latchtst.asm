
	DOSSEG
	.MODEL SMALL
	.STACK 100h

	.CODE

	ASSUME CS:_TEXT
	ASSUME DS:_TEXT
	ASSUME ES:NOTHING

start	PROC NEAR
	mov	ax, cs
	mov	ds, ax
	mov	ax,0A000h
	mov	es,ax

; Put display into mode 10, set up DS, ES
	mov	ax,10h
	int	10h

	mov	al,00Fh
	mov	cl,0Fh
	mov	si,0
LatchLoop:
	push	ax
; Load the latches
	mov	dx,3C4h
	mov	al,2
	out	dx,al
	mov	al,cl
	inc	dx
	out	dx,al
	pop	ax
	push	ax
	mov	ES:[si],al
	mov	al,ES:[si]
	mov	al,0Fh
	out	dx,al

; Set the latch write mode, delay awhile, copy latched data across the line
	push	cx
	mov	dx,3CEh
	mov	al,5
	out	dx,al
	mov	al,1
	inc	dx
	out	dx,al

	mov	al,50
	xor	cx,cx
delayloop:
	loop	delayloop
	dec	al
	jnz	delayloop

; Copy latched data across line
	mov	di,si
	inc	di
	mov	cx,4*80 - 1
	add	si,cx
	inc	si
	rep stosb

;Restore write mode to 0
	mov	dx,3CEh
	mov	al,5
	out	dx,al
	xor	al,al
	inc	dx
	out	dx,al

; next loop
	pop	cx
	pop	ax
	xor	al,0FFh
	loop	latchloop


finis:
	mov	ax, 4C00h
	int	21h

start ENDP


	end start
