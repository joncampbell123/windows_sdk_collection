;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

;----------------------------------------------------------------------------
; MACROS.INC
;----------------------------------------------------------------------------

;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
fptr	equ	fword ptr
wptr	equ	word  ptr
bptr    equ     byte  ptr

;----------------------------------------------------------------------------
; S T R U C S
;----------------------------------------------------------------------------

;----------------------------------------------------------------------------
; The following structure should be used to access high and low
; words of a DWORD.  This means that "word ptr foo[2]" -> "foo.hi".
;----------------------------------------------------------------------------
LONG    struc
lo      dw      ?
hi      dw      ?
LONG    ends

FARPOINTER      struc
off     dw      ?
sel     dw      ?
FARPOINTER      ends

FPOINTER        struc
foff    dd      ?
fsel    dw      ?
FPOINTER        ends

;----------------------------------------------------------------------------
; M A C R O S
;----------------------------------------------------------------------------

;----------------------------------------------------------------------------
; IO_Delay
; Delay between I/O to ports.
;----------------------------------------------------------------------------
IO_Delay    macro
jmp $+2
endm

;----------------------------------------------------------------------------
; PushReg
; Pushes index register, its current value, and the value of the
; data register at reg+1.
;----------------------------------------------------------------------------
PushReg	macro
	push	dx
	in	al,dx
	mov	bl,al	
	inc	dx
	in	al,dx
	dec	dx
	mov	bh,al
	push	bx
	inc	cx
	endm

;----------------------------------------------------------------------------
; PopReg
; Restores the value of an index register and the value of the data register
; at reg+1.
;----------------------------------------------------------------------------
PopReg	macro
	pop	bx
	pop	dx
	mov	al,bl
	out	dx,al
	inc	dx
	mov	al,bh
	out	dx,al
	dec	dx
	dec	cx
	endm

;----------------------------------------------------------------------------
; ?CHKSTKPROC
;
; Private Stack Checking Macro
;
; ?CHKSTKPROC will be invoked by the CMACROS for any procedure
; with local variables if both ?CHKSTK and ?CHKSTKPROC were
; defined prior to including the CMACROS.
;
; The macro has one parameter, which is the number of bytes
; to allocate, which is supplied by the CMACROS.
;
; The usage of this macro is defined by the CMACROS.  There is no
; user control over the macro.	Register usage will be as defined
; by the routine my_check_stack.
;
; Calls:
;	my_check_stack
; History:
;	Fri 16-Jan-1987 16:49:03 -by-  Walt Moore [waltm]
;	Initial version
;----------------------------------------------------------------------------
ifdef	?CHKSTK 			;;Only define macro if
ifdef	?CHKSTKPROC			;;  private stack checking
ifndef	?CHKSTKNAME			;;If user name differs from default
extrn	my_check_stack:near		;;Procedure to do the checking
endif
?CHKSTKPROC macro s			;;Actual macro text
	mov	ax,s			;;my_check_stack takes requested space
ifdef	?CHKSTKNAME
	?CHKSTKNAME
else
	call	my_check_stack		;;  in AX
endif
endm
endif
endif

;----------------------------------------------------------------------------
; odd	--- macro for odd alignment, counterpart for masm's "even".
;----------------------------------------------------------------------------
odd	macro
	ife (offset $) AND 1
	nop
	endif
	endm

;----------------------------------------------------------------------------
; pushem
; popem
;
; Allows giving a list of registers to push/pop on a single line.
; Also allows easy verificaton that pushes and pops are balanced
; because arguements are given in the same order:
;
;	pushem	ax,bx,cx   goes with
;	popem	ax,bx,cx
;----------------------------------------------------------------------------
pushem	macro	r1,r2,r3,r4,r5,r6,r7,r8,r9,rA,rB,rC,rD,rE,rF,r10,r11,r12
	irp	x,<r1,r2,r3,r4,r5,r6,r7,r8,r9,rA,rB,rC,rD,rE,rF,r10,r11,r12>
	ifnb	<x>
	push	x
	endif
	endm
	endm

popem	macro	r1,r2,r3,r4,r5,r6,r7,r8,r9,rA,rB,rC,rD,rE,rF,r10,r11,r12
	irp	x,<r12,r11,r10,rF,rE,rD,rC,rB,rA,r9,r8,r7,r6,r5,r4,r3,r2,r1>
	ifnb	<x>
	pop	x
	endif
	endm
	endm

;----------------------------------------------------------------------------
; smov
;
; smove moves the contents of one segment register into another
; segment register.
;
; usage
;
;	smov	destREG,srcREG
;----------------------------------------------------------------------------
smov	macro	x,y
	push	y
	pop	x
	endm

;----------------------------------------------------------------------------
; abs_ax
;	takes absolute value of AX
; Entry:
;	AX	= integer
; Returns:
;	AX	= abs(AX)
;----------------------------------------------------------------------------
abs_ax	macro
	cwd
	xor	ax,dx
	sub	ax,dx
	endm

;----------------------------------------------------------------------------
; min_ax
;	returns min of AX and REG
; Entry:
;	AX	= integer
;	REG	= general purpose register containing an integer
; Returns:
;	AX	= min(AX,REG)
;----------------------------------------------------------------------------
min_ax  macro   REG
ifdif <REG>,<0>
        sub     ax,REG
endif
	cwd
	and	ax,dx
ifdif <REG>,<0>
        add     ax,REG
endif
	endm

;----------------------------------------------------------------------------
; max_ax
;	returns max of AX and REG
; Entry:
;	AX	= integer
;	REG	= general purpose register containing an integer
; Returns:
;	AX	= max(AX, REG)
;----------------------------------------------------------------------------
max_ax  macro   REG
ifdif <REG>,<0>
        sub     ax,REG
endif
	cwd
	not	dx
        and     ax,dx
ifdif <REG>,<0>
        add     ax,REG
endif
	endm

;----------------------------------------------------------------------------
; jmpnext
; jmpnext stop
;
; jmpnext is used in the generation of fall through chains.  It
; generates the opcode used to swallow the next two bytes of object
; code (cmp ax,immediate word), and performs error checking to
; ensure that only two bytes of object code exist between any
; use of jmpnext.
;
; The chain is terminated by use of an optional parameter to jmpnext.
; If this optional field is non-blank, the chain is terminated.
;
; usage
;
;	dog:
;	      mov    al,34
;	     jmpnext
;
;	foo:
;	     mov     al,0
;	     jmpnext
;
;	bar:
;	     mov     al,1
;	     jmpnext stop			;End of the chain
;----------------------------------------------------------------------------
??ji	=	0			;;Initial index value

jmpnext macro e
jn %??ji,%(??ji+1),e			;;Set next label
endm

jn macro i,j,e
.sall
??ji&i:
.xall
ifb <e> 				;;If not the end of the chain
	db	03Dh			;;cmp ax, next two bytes
errn$	??ji&j,+2			;;next lable must be two bytes away
endif
??ji=j					;;increment counter
endm

;----------------------------------------------------------------------------
; missing_code
;
; missing_code is a macro which will display a message on the screen
; at assembly time.  It is used to flag code sequences which have not
; been completed.
;
; usage
;
;	missing_code	<text>
;----------------------------------------------------------------------------
missing_code	macro	x
	if1
	??_out	<&! Missing Code &!  x>
	endif
	endm
;----------------------------------------------------------------------------
; LMHtoP
;
; Converts a Local Memory Handle to a local pointer.
;
; Entry:
; 	reg1[,reg2]
; Returns:
; 	reg1 = pointer
;----------------------------------------------------------------------------
LMHtoP macro r1,r2		;; Local Movable Handle to pointer
ifnb <r2>
	mov	r1,[r2]
else
	mov	r1,[r1]
endif
endm

;----------------------------------------------------------------------------
; DOUT
;
; Writes some debugging info to the standard aux device.
;----------------------------------------------------------------------------
ifdef DEBUG
        extrn OutputDebugString:FAR

DOUT    macro   text
        local   string_buffer

_DATA segment
string_buffer label byte
        db      "&text&",13,10,0
_DATA ends

        pusha
        push    es
        push    DataBASE
        push    DataOFFSET string_buffer
        call    OutputDebugString
        pop     es
        popa
        endm
else

DOUT    macro   text
        endm
endif
;----------------------------------------------------------------------------
; WriteAux
; WriteDebug
;
; Writes some debugging info to the standard aux device.
;----------------------------------------------------------------------------
WriteAux    macro x
ifdef DEBUGAUX
            DOUT <x>
endif
            endm

WriteDebug  macro x
            DOUT <x>
            endm

;----------------------------------------------------------------------------
; REPSTOSB Dst
;
; store <cx> copies of al at Dst, aliging destination on WORD writes
;
;   Dst         destination, must be of the form SEL:[di] (default is es:[di])
;
; Entry:
;   Dst         -> points to dest buffer
;   al          byte to write
;   cx          count bytes
;
; NOTE this code assumes the direction flag is set to FORWARD
;
; Returns:
;
; History:
;       Sun 31-Jul-1989  -by-  ToddLa
;	Wrote it.
;----------------------------------------------------------------------------
?REPSTOSB macro Dst
        local   l1
        local   l2
        local   l3

        mov     ah,al               ; make sure ah == al
        test    di,1
        jz      short l1
        stos    byte ptr Dst
        dec     cx
l1:
        shr     cx,1
        rep     stos word ptr Dst
        adc     cl,cl
        rep     stos byte ptr Dst

        endm

REPSTOSB macro Dst
ifb <Dst>
        ?REPSTOSB es:[di]
else
        ?REPSTOSB Dst
endif
        endm

;----------------------------------------------------------------------------
; REPMOVSB Dst, Src, alignR
;
; copy <cx> bytes from Src to Dst, aliging destination or source
; on WORD writes
;
; Entry:
;   Dst         destination, must be of the form SEL:[edi] (default is es:[edi])
;   Src         source,      must be of the form SEL:[esi] (default is ds:[esi])
;   alignR      register to align   esi or edi (default is edi)
;   cx          count bytes
;
; NOTE this code assumes the direction flag is set to FORWARD
;
; Returns:
;
; History:
;       Sun 31-Jul-1989  -by-  ToddLa
;	Wrote it.
;----------------------------------------------------------------------------
?REPMOVSB  macro Dst, Src, alignR
        local   l1
        local   l2
        local   l3

        test    alignR,1
        jz      short l1
        movs    byte ptr Dst, byte ptr Src
        dec     cx
l1:     shr     cx,1
        rep     movs    word ptr Dst, word ptr Src
        adc     cl,cl
        rep     movs    byte ptr Dst, byte ptr Src

        endm

REPMOVSB  macro Dst, Src, alignR

ifb <Dst>
        ?REPMOVSB es:[di],ds:[si],di
        exitm
endif

ifb <Src>
        ?REPMOVSB Dst,ds:[si],di
        exitm
endif

ifb <alignR>
        ?REPMOVSB Dst,Src,di
        exitm
endif
        ?REPMOVSB Dst,Src,alignR

        endm

;----------------------------------------------------------------------------
; REPMOVSBXLAT Dst, Src, alignR, XlatTable
;
; copy <cx> bytes from Src to Dst, aliging destination or source
; on WORD writes
;
; Entry:
;   Dst         destination, must be of the form SEL:[di] (default is es:[di])
;   Src         source,      must be of the form SEL:[si] (default is ds:[si])
;   alignR      register to align   si or di (default is di)
;   XlatTable   byte translate table
;
; NOTE this code assumes the direction flag is set to FORWARD
;
; Returns:
;
; History:
;       Sun 31-Jul-1989  -by-  ToddLa
;	Wrote it.
;----------------------------------------------------------------------------
?REPMOVSBXLAT  macro Dst, Src, alignR, XlatTable
        local   l1, l2, l3, l4

        jcxz    short l4
        test    alignR,1
        jz      short l1
        lods    byte ptr Src
        xlat    XlatTable
        stos    byte ptr Dst
        dec     cx

l1:     shr     cx,1
        jz      short l3

l2:     lods    word ptr Src
        xchg    al,ah
        xlat    XlatTable
        xchg    al,ah
        xlat    XlatTable
        stos    word ptr Dst
        loop    l2

l3:     jnc     short l4
        lods    byte ptr Src
        xlat    XlatTable
        stos    byte ptr Dst
l4:
        endm

REPMOVSBXLAT  macro Dst, Src, alignR, XlatTable

ifb <Dst>
        ?REPMOVSBXLAT es:[di],ds:[si]
        exitm
endif

ifb <Src>
        ?REPMOVSBXLAT Dst,ds:[si]
        exitm
endif

ifb <alignR>
        ?REPMOVSBXLAT Dst,Src,di
        exitm
endif
        ?REPMOVSBXLAT Dst,Src,alignR,XlatTable
        endm

;----------------------------------------------------------------------------
; Public Label
; example:
;   PLABEL sort
;
; generates:
;   sort:
;   public sort
;----------------------------------------------------------------------------
PLABEL	macro n
n:
public n
endm

;----------------------------------------------------------------------------
; Public Proc
; example:
;   PPROC sort near
;
; generates:
;   proc sort near
;   public sort
;----------------------------------------------------------------------------
PPROC	macro n,nf
n proc nf
public n
endm

;----------------------------------------------------------------------------
; BiModal Label
; example:
;   BLABEL sort
;
; generates:
;   ifdef _286
;   sort_286:		
;   else
;   sort_386:		
;   endif
;----------------------------------------------------------------------------
BLABEL	macro n
ifdef _286
n&_286:
else
n&_386:
endif
endm
