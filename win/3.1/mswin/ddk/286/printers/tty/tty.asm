;/*
; +--------------------------------------------------------------------------+
; |                                                                          |
; |  Copyright (C) 1983-1990 by Microsoft Inc.                               |
; |  Modificado en 1986 por Jaime Garza V. FRALC Consultores (W 1.03)        |
; |  Modificado en 1988 por Jaime Garza V. FRALC Consultores (W 2.03)        |
; |  Modificado en 1988 por Armando Rodriguez M. FRALC Consultores (W 3.00)  |
; |                                                                          |
; +--------------------------------------------------------------------------+
;*/

	title	Routinas de incializacion de Generic

	if1
	%out	TTY.ASM
	endif
	page	,132
; +--------------------------------------------------------------------------+
;	Microsoft history
;	05 dec 89	peterbe		Modified WEP() to close com1: if
;					debug code is used.
; +--------------------------------------------------------------------------+


; This file contains GDI information for the different kinds of paper
; supported by the driver.  It also contains WEP().
;
; Este archivo contiene la informacion GDIINFO para los diferentes
; tipos de papel soportado por el driver.
;

; Definir la porcion de gdidefs.inc que se necesita
;
; Define the necessary part of gdidefs.inc

incDevice = 1

    include cmacros.inc
    include gdidefs.inc


?WIN=1

sBegin	data
assumes	ds,data
sEnd	data

EXTERNFP <LocalInit>

ifdef DEBUG
EXTERNFP <DBGclose>
endif
EXTRN  PSEUDOINIT:FAR

sBegin	code

	public	OffsetClipRect

assumes	cs,code

;       funcion creada por Jaime Garza V.
;       para inicializar el local heap de
;       la libreria a fin de poder usar
;       dialog boxes mas complicados

PUBLIC LoadLib

cProc	LoadLib,<FAR,PUBLIC,NODATA>,<si,di>
cBegin

	Xor ax,ax
	Jcxz	LoadError   ; cantidad definida en HEAPSIZE
	Push	ax
	Push	ax
	Push	cx	    ; LocalInit(0,0,cx);
	Call	LocalInit   ; regresa valor adecuado en ax
	or	ax,ax	    ; test for failure
	Jz  LoadError
	Push	di	    ; hInstance
	Call	PSEUDOINIT
	Mov ax,	1

LoadError:

cEnd

;   Funcion por J.G. requerida en Windows 3.xx
;   para finalizar la libreria. En C seria:
;
;   void FAR PASCAL WEP(int bSystemExit)
;   {
;       /* do nothing */
;   }
;
;   Pero para ahorrar bytes en asembler es:

cProc WEP,<PUBLIC,FAR>

ifdef DEBUG
	; the debug version uses WEP() to close the handle for com1:
	; on exit.
	ParmW	bSystemExit
cBegin
if1
%out .. DEBUG version of WEP()
endif

	test	bSystemExit, 0ffh
	jnz	WEPexit
	cCall	DBGclose

WEPexit:

cEnd

else	; not DEBUG
if1
%out .. normal version of WEP()
endif

;   Se recibe un parametro que se ignora
;   ParmW bSystemExit

cBegin nogen

    RET	2

cEnd nogen
endif


;       return:  (ax) > 0 si el rectangulo resultante no esta vacio
;			.. if the resulting rectangle isn't empty

cProc	OffsetClipRect,<NEAR,PUBLIC>

	ParmD	    lpRect
	ParmW	    Xoffset
	ParmW	    Yoffset

cBegin
	push	    ds
	lds	    bx,lpRect		;cargar rectangulo
	mov	    cx,Xoffset
	mov	    dx,Yoffset
	xor	    ax,ax

	sub	    left[bx],cx
	jns	    shift1
	mov	    left[bx],ax
shift1:
	sub	    top[bx],dx
	jns	    shift2
	mov	    top[bx],ax

shift2:
	mov	    ax,right[bx]
	sub	    ax,cx
	mov	    right[bx],ax
	sub	    ax,left[bx]
	jle	    done
	sub	    bottom[bx],dx
	mov	    ax,bottom[bx]
	sub	    ax,top[bx]
done:
	pop	    ds

cEnd	OffsetRect

cProc	Copy, <FAR, PUBLIC>, <si,di>

	parmd	dst		;apuntado long destination
	parmd	src		;apuntador long source
	parmw	cnt		;contador de bytes

cBegin	Copy
	push	ds
	mov	cx,cnt
	les	di,dst
	lds	si,src
	cld
	rep	movsb
	pop	ds
cEnd	Copy

;	SetByteValue: sets 'cnt' bytes starting at 'dst' to 'val'

cProc	SetByteValue, <FAR, PUBLIC>, <si,di>

        parmd   dst             ;long destination pointer
	parmb	val		;value to set to
        parmw   cnt             ;cnt of bytes

cBegin	SetByteValue
        mov     cx,cnt
        les     di,dst
	mov	al,val
        cld
	rep	stosb
cEnd	SetByteValue


sEnd	code

end	LoadLib
