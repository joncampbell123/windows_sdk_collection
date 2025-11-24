; This module defines the startup procedure for a run-time code library.
;
if1
    ?WINLIBC = 0
    ?PASCAL = 0
    ifndef ?PASLIBW
	ifndef ?MLIBW
	    ifndef ?SLIBW
		?WINLIBC = 1
		ifndef ?MLIB
		    memS = 1
		    %out ! Compiling for WINLIBC.LIB
		else
		    memM = 1
		    %out ! Compiling for MWINLIBC.LIB/LWINLIBC.LIB
		endif
	    else
		memS = 1
		%out ! Compiling for SLIBW.LIB
	    endif
	else
	    memM = 1
	    %out ! Compiling for MLIBW.LIB/LLIBW.LIB
	endif
    else
	?PASCAL = 1
	memM = 1
	%out ! Compiling for PASLIBW.LIB
    endif
    ifndef ?OEMLIBC
	?LIBOEM = 0
    else
	?LIBOEM = 1
	%out ! Compiling for OEMLIBC.LIB
    endif
endif

	.xlist
	?DF  = 1;
	?PLM = 1;
	?WIN = 1;
	include cmacros.inc
	.list

;
; Define single code segment
;
createSeg   _TEXT,CODE,PARA,PUBLIC,CODE

;
; Define data segment order for data.  Para alignment.
;
createSeg   NULL,NULL,PARA,PUBLIC,BEGDATA,DGROUP
createSeg   _DATA,DATA,PARA,PUBLIC,DATA,DGROUP
createSeg   CDATA,CDATA,WORD,COMMON,DATA,DGROUP
createSeg   CONST,CONST,WORD,PUBLIC,CONST,DGROUP
createSeg   _BSS,_BSS,PARA,PUBLIC,BSS,DGROUP

createSeg   XIB,XIBSEG,WORD,PUBLIC,DATA,DGROUP
createSeg   XI, XISEG, WORD,PUBLIC,DATA,DGROUP
createSeg   XIE,XIESEG,WORD,PUBLIC,DATA,DGROUP
createSeg   XPB,XIBSEG,WORD,PUBLIC,DATA,DGROUP
createSeg   XP, XISEG, WORD,PUBLIC,DATA,DGROUP
createSeg   XPE,XIESEG,WORD,PUBLIC,DATA,DGROUP
createSeg   XCB,XCBSEG,WORD,PUBLIC,DATA,DGROUP
createSeg   XC, XCSEG, WORD,PUBLIC,DATA,DGROUP
createSeg   XCE,XCESEG,WORD,PUBLIC,DATA,DGROUP
defGrp	    DGROUP,DATA

sBegin	    NULL
	    DD	0
labelDP     <PUBLIC,rsrvptrs>
maxRsrvPtrs = 5
	    DW	maxRsrvPtrs
	    DW	maxRsrvPtrs DUP (0)
sEnd	    NULL

sBegin	    DATA

assumes DS,DATA

public	__acrtused
	__acrtused = 1

if SizeC
globalCP    __aaltstkovr,-1	;
endif


sEnd	    DATA

externFP    <FATALEXIT>
externFP    <init_lib>

sBegin	CODE
assumes CS,CODE

	PUBLIC	__chkstk,_chkstk,chkstk
__chkstk:
_chkstk:
chkstk:
	pop	bx
if sizeC
	pop	dx
endif
	sub	ax,sp
	neg	ax
chkstk1:
	mov	sp,ax
if sizeC
	push	dx
	push	bx
ccc	proc	far
	ret
ccc	endp
else
	jmp	bx
endif

labelNP <PUBLIC,__astkovr>
	mov	al,-1
	db	0BBh

labelNP <PUBLIC,__cintDIV>
	mov	al,-2
	db	0BBh

labelNP <PUBLIC,__fptrap>
	mov	al,-3

	cbw
	cCall	FATALEXIT,<ax>
sEnd	CODE


sBegin	CODE
assumes CS,CODE

	externFP	LocalInit

cProc __astart,<PUBLIC,FAR>
cBegin
	;;
	;; DS = automatic data segment.
	;; CX = size of heap.
	;; DI = module handle.
	;; ES:SI = address of command line (not used).
	;;
	cCall init_lib,<di,cx>
cEND	__astart

sEnd	CODE

end __astart
