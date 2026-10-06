;  DISKTEST.COM source

;   To make this, you must be in real mode.
;   
;   masm disktest;
;   link disktest;
;   exe2bin disktest.exe disktest.com
;   del disktest.exe
;


Code	segment

	mov ax, 201h	; Read one sector
	mov bx, offset buffer	; to buffer
	mov cx, 1	; from track 0 sector 1
	mov dx, 0	; from drive 0 head 0
	push ds		; mov es,ds
	pop  es		; es:bx -> data buffer
	int  13h  	; execute read
	int  20h  	; exit

buffer	db 512 dup (?)  ; read buffer

Code	ends

	end




