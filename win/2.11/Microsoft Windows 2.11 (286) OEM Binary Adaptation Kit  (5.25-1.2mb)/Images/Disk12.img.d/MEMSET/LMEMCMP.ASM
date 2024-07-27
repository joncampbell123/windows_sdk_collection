		title	lmemcmp - byte string comparer
		subttl	Copyright (C) 1985 Aldus Corporation
		page	60,132

; abstract
;
; this module contains a c-language callable routine for comparing two blocks
; of bytes.
;
;  $Revision:   3.0  $
;  $Date:   17 Oct 1987 10:46:08  $
;  $Author:   TCJ  $
;  $Log:   I:/pm2/wut/vcs/lmemcmp.av  $
;
;   Rev 3.0   17 Oct 1987 10:46:08   TCJ
;Initial revision.
;
		page

; system includes
.xlist
memS	equ		1		;select small model
?PLM = 0				;no plm conventions
?WIN = 0				;no need to do windows calling conventions--no calls
                include cmacros.inc
.list
		page

sBegin	CODE
assumes	CS, CODE

cProc	lmemcmp,<PUBLIC>,<ds,si,es,di>
	parmD	first
	parmD	second
	parmW	cnt

cBegin
	xor		ax,ax		;set up for equality return
	mov		cx,cnt		;get number of bytes
	jcxz	$2			;get out if zero
	lds		si,first	;get first address
	les		di,second	;get second address
	cld					;say "compare 'em forward"
	repe	cmpsb		;compare 'em while equal
	je		$2			;equal, return 0
	jb		$1			;below, return -1
	inc		ax 			;above, return 1
	inc		ax			; (created the hard way)
$1:	dec 	ax			;first < second, return -1
$2:

cEnd
sEnd
end
