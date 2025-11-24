
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

	mov	ax,063Ch
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
	mov	es:[si],al
	mov	al,es:[si]
	mov	al,0Fh
	out	dx,al

; Set the latch write mode, delay awhile, copy latched data across the line
	mov	dx,3CEh
	mov	al,5
	out	dx,al
	mov	al,1
	inc	dx
	out	dx,al

	push	cx
	mov	al,50
delayloop2:
	loop	delayloop2
	dec	al
	jnz	delayloop2

; Copy latched data across line
	mov	di,si
	inc	di
	mov	cx,4*80 - 1
	add	si,cx
	inc	si
	rep stosb

;Set write mode to 0, XOR latch with data
	mov	dx,3CEh
	mov	al,5
	out	dx,al
	xor	al,al
	inc	dx
	out	dx,al
	dec	dx
	mov	al,3
	out	dx,al
	mov	al,18h
	inc	dx
	out	dx,al

; Set the S/R write vals, delay awhile, write data across the line
	pop	cx
	dec	dx
	mov	al,0
	out	dx,al
	mov	al,ah
	inc	dx
	out	dx,al
	dec	dx
	mov	al,1
	out	dx,al
	pop	ax
	push	ax
	inc	dx
	out	dx,al

	push	cx
	mov	ah,8
copyloop:
	mov	al,10
delayloop:
	loop	delayloop
	dec	al
	jnz	delayloop

; Copy latched, S/R data across line
	mov	di,si
	mov	cx,40
	add	si,cx
	mov	al,0FFh
	rep stosb
	dec	ah
	jnz	SHORT copyloop

;Restore write mode to 0, disable set/reset
	mov	dx,3CEh
	mov	al,5
	out	dx,al
	xor	al,al
	inc	dx
	out	dx,al
	dec	dx
	out	dx,al
	inc	dx
	out	dx,al
	dec	dx
	inc	al
	out	dx,al
	inc	dx
	dec	al
	out	dx,al
	dec	dx
	mov	al,3
	out	dx,al
	xor	al,al
	inc	dx
	out	dx,al


; next loop
	pop	cx
	pop	ax
	xor	ax,0FFFFh
	loop	latchloopjmp


finis:
	mov	ax, 4C00h
	int	21h

latchloopjmp:
	jmp	latchloop
start ENDP


	end start
