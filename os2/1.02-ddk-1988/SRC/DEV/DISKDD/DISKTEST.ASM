;
; Copyright (c) Microsoft Corporation,  1988
;
; DISKTEST.COM source

;   To make this, you must be in real mode, then run disktest.mak

	DOSSEG
	.MODEL SMALL
	.DATA
buffer	db 512 dup (?)
	.CODE
	ASSUME es:DGROUP
	mov ax, DGROUP	; set ax for int13h call
	mov es, ax
	mov ax, 201h	; Read one sector
	mov bx, offset DGROUP:buffer  ; to buffer
	mov cx, 1	; from track 0 sector 1
	mov dx, 0	; from drive 0 head 0
			; es:bx -> data buffer
	int  13h  	; execute read
	mov  ah, 4ch	; exit with return code
	mov  al, 1
	int  21h
	END










