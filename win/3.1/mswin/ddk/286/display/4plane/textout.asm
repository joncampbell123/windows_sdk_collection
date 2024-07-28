;----------------------------------------------------------------------------;
; This file redirects the ExtTextOut and StrBlt calls to appropriate real or ;
; protected mode routine.						     ;
;----------------------------------------------------------------------------;


	.xlist
	include	cmacros.inc
	.list

sBegin	Data
sEnd	Data

	public	ExtTextOut
	public	StrBlt

	
sBegin	Code

	externD	ExtTextOutFunction
	externD	StrBltFunction
	

ExtTextOut  proc  far
	assumes	cs,Code
	assumes	ds,nothing
	assumes	es,nothing
if 0
	cmp	cs:DoCount,0
	jne	@f
	jmp	dword ptr [ExtTextOutFunction]
public	DoCount
DoCount	dw	0
@@:	
	push	bp
	mov	bp,sp
	push	DoCount
DoAgain:
	push	[bp+44]
	push	[bp+42]			;parmD	lp_device		;Destination device
	push	[bp+40]			;parmW	x			;Left origin of string
	push	[bp+38]			;parmW	y			;Top  origin of string
	push	[bp+36]
	push	[bp+34]			;parmD	lp_clip_rect		;Clipping rectangle
	push	[bp+32]
	push	[bp+30]			;parmD	lp_string		;The string itself
	push	[bp+28]			;parmW	count			;Number of characters in the string
	push	[bp+26]
	push	[bp+24]			;parmD	lp_font 		;Font to use
	push	[bp+22]
	push	[bp+20]			;parmD	lp_draw_mode		;Drawmode structure to use
	push	[bp+18]
	push	[bp+16]			;parmD	lp_xform		;Current text transform
	push	[bp+14]
	push	[bp+12]			;parmD	lp_dx			;Widths for the characters
	push	[bp+10]
	push	[bp+8]			;parmD	lp_opaque_rect		;Opaquing rectangle
	push	[bp+6]			;parmW	eto_options
	call	dword ptr [ExtTextOutFunction]
	dec	word ptr [bp-2]		;DoCount
	jnz	DoAgain
	pop	bp
	pop	bp
endif
	jmp	dword ptr [ExtTextOutFunction]

ExtTextOut  endp


StrBlt	proc	far

	jmp	dword ptr [StrBltFunction]

StrBlt	endp

sEnd 	Code

end

