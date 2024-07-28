	title	dlgopena.asm
;*******************************************************************************
;*									       *
;*  MODULE	: DLGOPENA.ASM						       *
;*									       *
;*  DESCRIPTION : Assembly language helper routines for DLGOPEN.C	       *
;*									       *
;*  FUNCTIONS	: chdir ()  - change to specified asciiz directory.	       *
;*									       *
;*******************************************************************************
?WIN = 1

?PLM=1	    ; PASCAL Calling convention is DEFAULT
?WIN=1	    ; Windows calling convention
?386=0	    ; Use 386 code?
include cmacros.inc

;*********************************************************************
;* The following structure should be used to access high and low
;* words of a DWORD.  This means that "word ptr foo[2]" -> "foo.hi".
;*********************************************************************

LONG    struc
lo      dw      ?
hi      dw      ?
LONG    ends

FARPOINTER      struc
off     dw      ?
sel     dw      ?
FARPOINTER      ends

;*********************************************************************
;               DATA SEGMENT DECLARATIONS
;*********************************************************************

ifndef SEGNAME
    SEGNAME equ <TEXT>
endif

if ?386
    .386p
    createSeg _%SEGNAME, CodeSeg, word, use16, CODE
else
    .286p
    createSeg _%SEGNAME, CodeSeg, word, public, CODE
endif

sBegin	DATA
sEnd	DATA

sBegin	CodeSeg

assumes CS,CodeSeg
assumes DS,DATA

;****************************************************************************
;*									    *
;*  FUNCTION   : chdir (p)						    *
;*									    *
;*  PURPOSE    : Change to asciiz directory specified in p		    *
;*									    *
;*  RETURNS    : 1 - Success						    *
;*		 0 - Error						    *
;*									    *
;****************************************************************************

cProc	chdir,<PUBLIC,FAR,PASCAL>,<ds>
	parmD p
cBegin
	lds	dx,p
	mov	bx,dx

	cmp	BYTE PTR ds:[bx+1],':'
        jnz     chdnod                  ; No drive
        mov     dl,ds:[bx]
        or      dl,20h
        sub     dl,'a'

	mov	ah,0eh			; Set current drive
	int	21h

	mov	ah,19h			; Get current drive
	int	21h

	cmp	al,dl
	jne	chderror

	lds	dx,p
	add	dx,2
	mov	bx,dx
	cmp	BYTE PTR ds:[bx],0	; If path name is "" we are there
	jz	chdok
chdnod:
	mov	ah,3bh
	int	21h
	jc	chderror
chdok:
	mov	ax,1
chdexit:
cEnd
chderror:
	xor	ax,ax
	jmp	short chdexit

sEnd	CodeSeg

end
