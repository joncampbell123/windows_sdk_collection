	page	,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	SMARTPRO.ASM
;
;   This module contains the routines for outputting proportion
;   and fixed pitch characters.
;
; Created: 24-Jul-1987
; Author:  Walt Moore [waltm]
;
; Copyright (c) 1984-1987 Microsoft Corporation
;
; Exported Functions:	None
;
; Public Functions:
;	gen_nc_one_char
;	gen_nc_two_char
;	gen_nc_three_char
;	gen_nc_four_char
;	gen_nc_n_char
;	gen_nc_n_char
;	gen_nc_n_char
;	gen_nc_n_char
;	gen_cl_one_char
;	gen_cl_two_char
;	gen_cl_three_char
;	gen_cl_four_char
;	gen_cl_n_char
;	gen_cl_n_char
;	gen_cl_n_char
;	gen_cl_n_char
;
;
; if SPECIAL_CASE_DEV_ONC
;	dev_onc_one_char
;	dev_onc_two_char
;	dev_onc_three_char
;	dev_onc_four_char
; endif
;
; if SPECIAL_CASE_DEV_TNC
;	dev_tnc_one_char
;	dev_tnc_two_char
;	dev_tnc_three_char
;	dev_tnc_four_char
; endif
;
; if SPECIAL_CASE_DEV_TC
;	dev_tc_one_char
;	dev_tc_two_char
;	dev_tc_three_char
;	dev_tc_four_char
; endif
;
; if SPECIAL_CASE_BM_OBWNC
;	bm_obwnc_one_char
;	bm_obwnc_two_char
;	bm_obwnc_three_char
;	bm_obwnc_four_char
; endif
;
; if SPECIAL_CASE_BM_OWBNC
;	bm_owbnc_one_char
;	bm_owbnc_two_char
;	bm_owbnc_three_char
;	bm_owbnc_four_char
; endif
;
; if SPECIAL_CASE_BM_TBNC
;	bm_tbnc_one_char
;	bm_tbnc_two_char
;	bm_tbnc_three_char
;	bm_tbnc_four_char
; endif
;
; if SPECIAL_CASE_BM_TWNC
;	bm_twnc_one_char
;	bm_twnc_two_char
;	bm_twnc_three_char
;	bm_twnc_four_char
; endif
;
; Public Data:		none
;
; General Description:
;
; Restrictions:
;
;-----------------------------------------------------------------------;
	.xlist
	include macros.mac
	include cmacros.inc
	include gdidefs.inc
	include rlfntseg.inc
	include	display.inc
	include	ega.inc
	include	egamem.inc
	include rlstrblt.inc
	.list

	externA SCREEN_W_BYTES
	externA ScreenSelector
	externNP set_ega_opaque_mode


;	Special cases

SPECIAL_CASE_BM_OBBNC = 0	;Bitmap, opaque, black on black, non-clipped
SPECIAL_CASE_BM_OBWNC = 1	;Bitmap, opaque, black on white, non-clipped
SPECIAL_CASE_BM_OWBNC = 0	;Bitmap, opaque, white on black, non_clipped
SPECIAL_CASE_BM_OWWNC = 0	;Bitmap, opaque, white on white, non-clipped
SPECIAL_CASE_BM_TBNC  = 0	;Bitmap, transparent black, non-clipped
SPECIAL_CASE_BM_TWNC  = 0	;Bitmap, transparent white, non-clipped

SPECIAL_CASE_DEV_ONC  = 1	;Device, opaque, non-clipped
SPECIAL_CASE_DEV_TNC  = 1	;Device, transparent, non-clipped
;SPECIAL_CASE_DEV_OC  = 0	;Device, opaque, clipped (see notes below)
SPECIAL_CASE_DEV_TC   = 0	;Device, transparent, clipped

TRANSPARENT_SPEEDUP = 1		;Speed up for transparant mode.
RALPHS_SPEEDUP = 1		;RalphL's speed up suggestion.

;-----------------------------------------------------------------------;
;	The following equates are used to index into the buffer
;	of character widths, phases (x location), and offsets
;	of the bits within the font.
;-----------------------------------------------------------------------;

base	equ	0

wwidth	equ	byte ptr 0
pphase	equ	byte ptr 1
cchar	equ	word ptr -2

wwidth1 equ	byte ptr base - 0
pphase1 equ	byte ptr wwidth1 + 1
cchar1	equ	word ptr wwidth1 - 2

wwidth2 equ	byte ptr base - 4
pphase2 equ	byte ptr wwidth2 + 1
cchar2	equ	word ptr wwidth2 - 2

wwidth3 equ	byte ptr base - 8
pphase3 equ	byte ptr wwidth3 + 1
cchar3	equ	word ptr wwidth3 - 2

wwidth4 equ	byte ptr base - 12
pphase4 equ	byte ptr wwidth4 + 1
cchar4	equ	word ptr wwidth4 - 2
page

;--------------------------------Macro----------------------------------;
; upd_bm
;
;	upd_bm is the macro used for generating the destination update
;	code for a bitmap.
;
;	Usage:
;		upd_bm	ll
;	Where
;		ll	is the macro to invoke for the looping logic
;-----------------------------------------------------------------------;

upd_bm	macro	ll
if DRAW_ADJUST				;;If last logic operation defined
	dec	di			;;  did a stosb, adjust for it
endif
	add	di,ss_next_scan
	ll
	endm


;--------------------------------Macro----------------------------------;
; upd_dev
;
;	upd_dev is the macro used for generating the destination update
;	code for the physical device
;
;	Usage:
;		upd_dev ll
;	Where
;		ll	is the macro to invoke for the looping logic
;-----------------------------------------------------------------------;

upd_dev	macro	ll
	add	di,SCREEN_W_BYTES-DRAW_ADJUST
	ll
	endm
page

;--------------------------------Macro----------------------------------;
; n_char
;
;	n_char is a macro for generating the code required to process
;	a destination character consisting of 5,6,7 or 8 source bytes
;
;	Usage:
;		n_char	name,output,update,setup
;	Where
;
;		name	the name of the procedure
;		output	the macro which will generate the required
;			output logic
;		update	the macro which will generate the required
;			destination update logic
;		setup	if this parameter is given as "inline", then
;			setup code will be generated inline, else a
;			subroutine will be called to perform the
;			initialization.  This subroutine will be
;			created if not defined.
;
;	The macro will generate the loop_logic macro which is the
;	macro used by all update macros for generating looping logic.
;	It is defined as a macro so that devices which are interleaved
;	can make multiple copies of it, possibly removing a jump.
;-----------------------------------------------------------------------;

n_char	macro	name,output,update,setup
	local	genflag

	assumes ds,nothing		;;Set the assumptions now
	assumes es,nothing
	assumes ss,StrStuff

;-----------------------------------------------------------------------;
; n character looping logic macro
;-----------------------------------------------------------------------;

loop_logic	&macro
	inc	dx			;;Next scan of font
	dec	ss_height		;;Loop until all chars output
	jnz	name&_n_char_outer_loop
	pop	bp			;;BP = frame pointer
	sub	di,cell_adjust
	ret
	&endm

;-----------------------------------------------------------------------;
; n character long looping logic macro
;-----------------------------------------------------------------------;

long_loop_logic	&macro
	inc	dx			;;Next scan of font
	dec	ss_height		;;Loop until all chars output
	jz	name&_n_char_exit
	jmp	name&_n_char_outer_loop
name&_n_char_exit:
	pop	bp			;;BP = frame pointer
	sub	di,cell_adjust
	ret
	&endm

;-----------------------------------------------------------------------;
; n character setup logic
;-----------------------------------------------------------------------;
public	name&_n_char			;;make public
genflag=0				;;assume out_of_line, already generated
ifidn <setup>,<inline>			;;if in_line
name&_n_char proc near			;;  define procedure
  genflag = 01b 			;;  show code must be generated
else					;;else
  ifndef n_char_setup			;;  if procedure not defined
    genflag = 11b			;;    show proc must also be generated
  else					;;  else
    ife $-n_char_setup			;;    a hack since it is defined on
      genflag = 11b			;;    pass 2 regardless of if we have
    endif				;;    generated it
  endif 				;;
endif					;;
if genflag				;;if to generate setup
  if genflag and 10b			;;  if generating procedural version
public	n_char_setup			;;    make public for debugging
n_char_setup proc near			;;    define start of setup proc
  endif 				;;  endif
	mov	ss_height,ax		;;  save # scans in character
  if genflag and 10b			;;  if generating setup procedure
	pop	ax			;;    get return address
  endif 				;;  endif
	mov	ss_num_chars,cl 	;;  save # of characters - 1
	push	bp			;;  save frame pointer
	mov	bp,dx			;;  set buffer pointer
	xor	dx,dx			;;  index into font scan
  if genflag and 10b			;;  if generating setup procedure
	jmp	ax			;;    dispatch to caller
n_char_setup endp			;;    terminate the procedure
name&_n_char proc near			;;    define actual procedure
	call	n_char_setup		;;    call setup code
  endif 				;;  endif
else					;;else
name&_n_char proc near			;;    define actual procedure
	call	n_char_setup		;;    call setup code
endif					;;endif ;setup code

;-----------------------------------------------------------------------;
; n character compilation logic
;-----------------------------------------------------------------------;

name&_n_char_outer_loop:
	xor	si,si			;;SI = char index
	mov	ch,ss_num_chars 	;;Get # of characters - 1
	xchg	dx,di			;;Index to next font scan in DI
	xor	ax,ax		

name&_n_char_inner_loop:
	mov	bx,[bp][si].cchar	;;BX = offset of bits
	or	al,[bx][di]
	mov	cl,[bp][si].wwidth	;;CL = width
	rol	ax,cl
	sub	si,4			;;--> next char
	dec	ch
	jnz	name&_n_char_inner_loop
	mov	bx,[bp][si].cchar	;;BX = offset of bits
	or	al,[bx][di]
	mov	cl,[bp][si].pphase	;;CL = phase
	shr	ax,cl
	xchg	dx,di			;;DI = dest ptr
	output	<update>		;;Macro to do whatever for outputting
name&_n_char  endp
	endm
page

;--------------------------------Macro----------------------------------;
; four_char
;
;	four_char is a macro for generating the code required to
;	process a destination character consisting of 4 source bytes
;
;	Usage:
;		four_char  name,output,update,setup
;	Where
;
;		name	the name of the procedure
;		output	the macro which will generate the required
;			output logic
;		update	the macro which will generate the required
;			destination update logic
;		setup	if this parameter is given as "inline", then
;			setup code will be generated inline, else a
;			subroutine will be called to perform the
;			initialization.  This subroutine will be
;			created if not defined.
;
;	The macro will generate the loop_logic macro which is the
;	macro used by all update macros for generating looping logic.
;	It is defined as a macro so that devices which are interleaved
;	can make multiple copies of it, possibly removing a jump.
;-----------------------------------------------------------------------;

four_char macro name,output,update,setup
	local	genflag

	assumes ds,nothing		;;Set the assumptions now
	assumes es,nothing
	assumes ss,StrStuff

;-----------------------------------------------------------------------;
; four character looping logic macro
;-----------------------------------------------------------------------;

loop_logic	&Macro
	dec	ss_height
	jnz	name&_four_char_loop
	pop	bp			;;BP = frame pointer
	sub	di,cell_adjust
	ret
	&endm

;-----------------------------------------------------------------------;
; four character long looping logic macro
;-----------------------------------------------------------------------;

long_loop_logic	&Macro
	dec	ss_height
	jz	name&_four_char_exit
	jmp	name&_four_char_loop
name&_four_char_exit:
	pop	bp			;;BP = frame pointer
	sub	di,cell_adjust
	ret
	&endm

;-----------------------------------------------------------------------;
; four character setup logic
;-----------------------------------------------------------------------;

public	name&_four_char 		;;make public
genflag=0				;;assume out_of_line, already generated
ifidn <setup>,<inline>			;;if in_line
name&_four_char proc near		;;  define procedure
  genflag = 01b 			;;  show code must be generated
else					;;else
  ifndef four_char_setup		;;  if procedure not defined
    genflag = 11b			;;    show proc must also be generated
  else					;;  else
    ife $-four_char_setup		;;    a hack since it is defined on
      genflag = 11b			;;    pass 2 regardless of if we have
    endif				;;    generated it
  endif
endif
if genflag				;;if to generate setup
  if genflag and 10b			;;  if generating procedural version
public	four_char_setup 		;;    make public for debugging
four_char_setup proc near		;;    define start of setup proc
  endif 				;;  endif
	mov	ss_height,ax		;;  save # scans in character
  if genflag and 10b			;;  if generating setup procedure
	pop	ax			;;    get return address
  endif 				;;  endif
	push	bp			;;  save frame pointer
	mov	bp,dx			;;  --> buffer
	mov	dl,[bp].wwidth3 	;;
	mov	dh,[bp].pphase4 	;;
	mov	ss_phases,dx		;;
	mov	cl,[bp].wwidth1 	;;
	mov	ch,[bp].wwidth2 	;;
	mov	si,[bp].cchar4		;;  4th character so we can lodsb
	mov	bx,[bp].cchar1		;;
	mov	dx,[bp].cchar3		;;
	mov	bp,[bp].cchar2		;;
	sub	bx,si			;;  compute deltas
	sub	bp,si			;;
	sub	dx,si			;;
  if genflag and 10b			;;  if generating setup procedure
	jmp	ax			;;    dispatch to caller
four_char_setup endp			;;    terminate the procedure
name&_four_char proc near		;;    define actual procedure
	call	four_char_setup 	;;    call setup code
  endif 				;;  endif
else					;;else
name&_four_char proc near		;;    define actual procedure
	call	four_char_setup 	;;    call setup code
endif					;;endif ;setup code

;-----------------------------------------------------------------------;
; four character compilation logic
;-----------------------------------------------------------------------;
name&_four_char_loop:
	xor	ax,ax
	mov	al,[si][bx]
	rol	ax,cl
	or	al,ds:[si][bp]
	xchg	cl,ch
	rol	ax,cl
	xchg	cl,ch
	xchg	cx,ss_phases
	xchg	bx,dx
	or	al,[si][bx]
	xchg	bx,dx
	rol	ax,cl
	or	al,ds:[si]
	inc	si
	xchg	cl,ch
	shr	ax,cl
	xchg	cl,ch
	xchg	cx,ss_phases
	output	<update>		;;Macro to do whatever for outputting
name&_four_char endp
	endm
page

;--------------------------------Macro----------------------------------;
; three_char
;
;	three_char is a macro for generating the code required to
;	process a destination character consisting of 3 source bytes
;
;	Usage:
;		three_char  name,output,update,setup
;	Where
;
;		name	the name of the procedure
;		output	the macro which will generate the required
;			output logic
;		update	the macro which will generate the required
;			destination update logic
;		setup	if this parameter is given as "inline", then
;			setup code will be generated inline, else a
;			subroutine will be called to perform the
;			initialization.  This subroutine will be
;			created if not defined.
;
;	The macro will generate the loop_logic macro which is the
;	macro used by all update macros for generating looping logic.
;	It is defined as a macro so that devices which are interleaved
;	can make multiple copies of it, possibly removing a jump.
;-----------------------------------------------------------------------;

three_char macro name,output,update,setup
	local	genflag

	assumes ds,nothing		;;Set the assumptions now
	assumes es,nothing
	assumes ss,StrStuff

;-----------------------------------------------------------------------;
; three character looping logic macro
;-----------------------------------------------------------------------;

loop_logic	&macro
	dec	ss_height
	jnz	name&_three_char_loop
	pop	bp
	sub	di,cell_adjust
	ret
	&endm

;-----------------------------------------------------------------------;
; three character long looping logic macro
;-----------------------------------------------------------------------;

long_loop_logic	&macro
	dec	ss_height
	jz	name&_three_char_exit
	jmp	name&_three_char_loop
name&_three_char_exit:
	pop	bp
	sub	di,cell_adjust
	ret
	&endm

;-----------------------------------------------------------------------;
; three character setup logic
;-----------------------------------------------------------------------;

public	name&_three_char		;;make public
genflag=0				;;assume out_of_line, already generated
ifidn <setup>,<inline>			;;if in_line
name&_three_char proc near		;;  define procedure
  genflag = 01b 			;;  show code must be generated
else					;;else
  ifndef three_char_setup		;;  if procedure not defined
    genflag = 11b			;;    show proc must also be generated
  else					;;  else
    ife $-three_char_setup		;;    a hack since it is defined on
      genflag = 11b			;;    pass 2 regardless of if we have
    endif				;;    generated it
  endif
endif
if genflag				;;if to generate setup
  if genflag and 10b			;;  if generating procedural version
public	three_char_setup		;;    make public for debugging
three_char_setup proc near		;;    define start of setup proc
  endif 				;;  endif
	mov	ss_height,ax		;;  save # scans in character
  if genflag and 10b			;;  if generating setup procedure
	pop	ax			;;    get return address
  endif 				;;  endif
	push	bp
	mov	bp,dx			;;  BP = buffer
	mov	dl,[bp].wwidth1 	;;
	mov	dh,[bp].wwidth2 	;;
	mov	ch,[bp].pphase3 	;;
	mov	si,[bp].cchar3		;;
	mov	bx,[bp].cchar2		;;
	mov	bp,[bp].cchar1		;;
	sub	bx,si			;;
	sub	bp,si			;;
  if genflag and 10b			;;  if generating setup procedure
	jmp	ax			;;    dispatch to caller
three_char_setup endp			;;    terminate the procedure
name&_three_char proc near		;;    define actual procedure
	call	three_char_setup	;;    call setup code
  endif 				;;  endif
else					;;else
name&_three_char proc near		;;    define actual procedure
	call	three_char_setup	;;    call setup code
endif					;;endif ;setup code

;-----------------------------------------------------------------------;
; three character compilation logic
;-----------------------------------------------------------------------;

name&_three_char_loop:
	xor	ax,ax
	mov	al,ds:[si][bp]
	mov	cl,dl
	rol	ax,cl
	or	al,[si][bx]
	mov	cl,dh
	rol	ax,cl
	or	al,ds:[si]
	inc	si
	mov	cl,ch
	shr	ax,cl
	output	<update>		;;Macro to do whatever for outputting
name&_three_char endp
	endm
page

;--------------------------------Macro----------------------------------;
; two_char
;
;	two_char is a macro for generating the code required to
;	process a destination character consisting of 2 source bytes
;
;	Usage:
;		two_char  name,output,update,setup
;	Where
;
;		name	the name of the procedure
;		output	the macro which will generate the required
;			output logic
;		update	the macro which will generate the required
;			destination update logic
;		setup	if this parameter is given as "inline", then
;			setup code will be generated inline, else a
;			subroutine will be called to perform the
;			initialization.  This subroutine will be
;			created if not defined.
;
;	The macro will generate the loop_logic macro which is the
;	macro used by all update macros for generating looping logic.
;	It is defined as a macro so that devices which are interleaved
;	can make multiple copies of it, possibly removing a jump.
;-----------------------------------------------------------------------;

two_char macro name,output,update,setup
	local	genflag

	assumes ds,nothing		;;Set the assumptions now
	assumes es,nothing
	assumes ss,StrStuff

;-----------------------------------------------------------------------;
; two character looping logic macro
;-----------------------------------------------------------------------;

loop_logic	&macro
	dec	dx
	jnz	name&_two_char_loop
	sub	di,cell_adjust
	ret
	&endm

;-----------------------------------------------------------------------;
; two character long looping logic macro
;-----------------------------------------------------------------------;

long_loop_logic	&macro
	dec	dx
	jz	name&_two_char_exit
	jmp	name&_two_char_loop
name&_two_char_exit:
	sub	di,cell_adjust
	ret
	&endm

;-----------------------------------------------------------------------;
; two character setup logic
;-----------------------------------------------------------------------;

public	name&_two_char			;;make public
genflag=0				;;assume out_of_line, already generated
ifidn <setup>,<inline>			;;if in_line
name&_two_char proc near		;;  define procedure
  genflag = 01b 			;;  show code must be generated
else					;;else
  ifndef two_char_setup 		;;  if procedure not defined
    genflag = 11b			;;    show proc must also be generated
  else					;;  else
    ife $-two_char_setup		;;    a hack since it is defined on
      genflag = 11b			;;    pass 2 regardless of if we have
    endif				;;    generated it
  endif
endif
if genflag				;;if to generate setup
  if genflag and 10b			;;  if generating procedural version
public	two_char_setup			;;    make public for debugging
two_char_setup proc near		;;    define start of setup proc
  endif 				;;  endif
	xchg	bp,dx			;;  BP = buffer, DX = font height
	mov	cl,[bp].wwidth1 	;;
	mov	ch,[bp].pphase2 	;;
	mov	bx,[bp].cchar1		;;
	mov	si,[bp].cchar2		;;
	mov	bp,dx			;;  restore frame pointer
	xchg	ax,dx			;;  set DX = font height
	sub	bx,si			;;  delta between the characters
  if genflag and 10b			;;  if generating setup procedure
	ret				;;    dispatch to caller
two_char_setup endp			;;    terminate the procedure
name&_two_char proc near		;;    define actual procedure
	call	two_char_setup		;;    call setup code
  endif 				;;  endif
else					;;else
name&_two_char proc near		;;    define actual procedure
	call	two_char_setup		;;    call setup code
endif					;;endif ;setup code

;-----------------------------------------------------------------------;
; two character compilation logic
;-----------------------------------------------------------------------;

name&_two_char_loop:
	xor	ax,ax
	mov	al,[bx][si]
	rol	ax,cl
	xchg	cl,ch
	or	al,ds:[si]
	inc	si
	shr	ax,cl
	xchg	cl,ch
	output	<update>		;;Macro to do whatever for outputting
name&_two_char endp
	endm
page

;--------------------------------Macro----------------------------------;
; one_char
;
;	one_char is a macro for generating the code required to
;	process a destination character consisting of 1 source byte
;
;	Usage:
;		one_char  name,output,update,setup
;	Where
;
;		name	the name of the procedure
;		output	the macro which will generate the required
;			output logic
;		update	the macro which will generate the required
;			destination update logic.
;		setup	if this parameter is given as "inline", then
;			setup code will be generated inline, else a
;			subroutine will be called to perform the
;			initialization.  This subroutine will be
;			created if not defined.
;
;	The macro will generate the loop_logic macro which is the
;	macro used by all update macros for generating looping logic.
;	It is defined as a macro so that devices which are interleaved
;	can make multiple copies of it, possibly removing a jump.
;-----------------------------------------------------------------------;

one_char macro name,output,update,setup
	local	genflag

	assumes ds,nothing		;;Set the assumptions now
	assumes es,nothing
	assumes ss,StrStuff

;-----------------------------------------------------------------------;
; one character looping logic macro
;-----------------------------------------------------------------------;

loop_logic	&macro
	dec	dx
	jnz	name&_one_char_loop
	sub	di,cell_adjust
	ret
	&endm

;-----------------------------------------------------------------------;
; one character long looping logic macro
;-----------------------------------------------------------------------;

long_loop_logic	&macro
	dec	dx
	jz	name&_one_char_exit
	jmp	name&_one_char_loop
name&_one_char_exit:
	sub	di,cell_adjust
	ret
	&endm

;-----------------------------------------------------------------------;
; one character setup logic
;-----------------------------------------------------------------------;

public	name&_one_char			;;make public
genflag=0				;;assume out_of_line, already generated
ifidn <setup>,<inline>			;;if in_line
name&_one_char proc near		;;  define procedure
  genflag = 01b 			;;  show code must be generated
else					;;else
  ifndef one_char_setup 		;;  if procedure not defined
    genflag = 11b			;;    show proc must also be generated
  else					;;  else
    ife $-one_char_setup		;;    a hack since it is defined on
      genflag = 11b			;;    pass 2 regardless of if we have
    endif				;;    generated it
  endif
endif
if genflag				;;if to generate setup
  if genflag and 10b			;;  if generating procedural version
public	one_char_setup			;;    make public for debugging
one_char_setup proc near		;;    define start of setup proc
  endif 				;;  endif
	xchg	dx,bp			;;  BP --> character buffer
	mov	si,[bp].cchar1		;;  DS:SI = char1
	mov	cl,[bp].pphase1 	;;
	xchg	dx,bp			;;  BP --> frame
	xchg	ax,dx			;;  DX = clipped_font_height
  if genflag and 10b			;;  if generating setup procedure
	ret				;;    dispatch to caller
one_char_setup endp			;;    terminate the procedure
name&_one_char proc near		;;    define actual procedure
	call	one_char_setup		;;    call setup code
  endif 				;;  endif
else					;;else
name&_one_char proc near		;;    define actual procedure
	call	one_char_setup		;;    call setup code
endif					;;endif ;setup code

;-----------------------------------------------------------------------;
; one character compilation logic
;-----------------------------------------------------------------------;

name&_one_char_loop:
	lodsb				;;char1
	shr	al,cl
	output	<update>
name&_one_char endp
	endm
page

;--------------------------------Macro----------------------------------;
; clipped_output
;
;	clipped_output is the macro passed to the "x" character macros
;	when the default subroutines are to be called for outputing a
;	clipped character
;
;	Usage:
;		clipped_output ll
;	Where
;		ll	is the macro to be invoked for generating
;			the looping logic.
;-----------------------------------------------------------------------;

clipped_output macro x
	call	ss_draw_clipped
	x
	endm


;--------------------------------Macro----------------------------------;
; non_clipped_output
;
;	non_clipped_output is the macro passed to the "x" character
;	macros when the default subroutines are to be called for
;	outputing a non-clipped character
;
;	Usage:
;		non_clipped_output ll
;	Where
;		ll	is the macro to be invoked for generating
;			the looping logic.
;-----------------------------------------------------------------------;

non_clipped_output macro x
	call	ss_draw
	x
	endm


;--------------------------------Macro----------------------------------;
; ega_tnc
;
;	ega_tnc is a macro for generating the character drawing
;	logic for transparent mode, non-clipped, ignoring colors, which
;	only makes sense with hardware support of colors, as with the
;	EGA.
;
;	Usage:
;		ega_tnc update
;	Where
;		update	the macro which will generate the required
;			destination update logic.
;-----------------------------------------------------------------------;

ega_tnc	macro	update
if TRANSPARENT_SPEEDUP	
	or	ax,ax
	jz	@f
endif
	push	dx
	mov	dx,EGA_BASE + GRAF_DATA
	out	dx,al
	xchg	al,es:[di]
	pop	dx
@@:
DRAW_ADJUST = 0				;stosb is not used
	update
	endm


;--------------------------------Macro----------------------------------;
; ega_tc
;
;	ega_tc is a macro for generating the character drawing
;	logic for transparent mode, clipped, ignoring colors, which
;	only makes sense with hardware support of colors, as with the
;	EGA.
;
;	Usage:
;		ega_tc update
;	Where
;		update	the macro which will generate the required
;			destination update logic.
;-----------------------------------------------------------------------;

ega_tc	macro	update
	and	al,ss_clip_mask
if TRANSPARENT_SPEEDUP	
	jz	@f
endif	
	push	dx
	mov	dx,EGA_BASE + GRAF_DATA
	out	dx,al
	xchg	al,es:[di]
	pop	dx
@@:
DRAW_ADJUST = 0				;stosb is not used
	update
	endm

;--------------------------------Macro----------------------------------;
; ega_onc
;
;	ega_onc is a macro for generating the character drawing
;	logic for opaque mode, non-clipped, ignoring colors, which
;	only makes sense with hardware support of colors, as with the
;	EGA.
;
;	Usage:
;		ega_onc update
;	Where
;		update	the macro which will generate the required
;			destination update logic.
;-----------------------------------------------------------------------;

ega_onc	macro	update
	stosb
DRAW_ADJUST	= 1			;;STOSB is used
	update
	endm


;--------------------------------Macro----------------------------------;
; owwc
;
;	owwc  is a macro for generating the character drawing
;	logic for opaque mode, white text, white background, clipped.
;
;	Usage:
;		owwc update
;	Where
;		update	the macro which will generate the required
;			destination update logic.
;-----------------------------------------------------------------------;

owwc	macro	update
	mov	al,ss_clip_mask
	or	es:[di],al
DRAW_ADJUST	= 0			;;STOSB isn't used
	update
	endm


;--------------------------------Macro----------------------------------;
; twc
;
;	twc is a macro for generating the character drawing
;	logic for transparent mode, white text, clipped.
;
;	Usage:
;		twc update
;	Where
;		update	the macro which will generate the required
;			destination update logic.
;-----------------------------------------------------------------------;

twc	macro	update
	and	al,ss_clip_mask
	or	es:[di],al
DRAW_ADJUST	= 0			;;STOSB isn't used
	update
	endm


;--------------------------------Macro----------------------------------;
; twnc
;
;	twnc is a macro for generating the character drawing
;	logic for transparent mode, white text, non-clipped.
;
;	Usage:
;		twnc update
;	Where
;		update	the macro which will generate the required
;			destination update logic.
;-----------------------------------------------------------------------;

twnc	macro	update
	or	es:[di],al
DRAW_ADJUST	= 0			;;STOSB isn't used
	update
	endm


;--------------------------------Macro----------------------------------;
; obbc
;
;	obbc is a macro for generating the character drawing
;	logic for opaque mode, black text, black background, clipped.
;
;	Usage:
;		obbc update
;	Where
;		update	the macro which will generate the required
;			destination update logic.
;-----------------------------------------------------------------------;

obbc	macro	update
	mov	al,ss_clip_mask
	not	al
	and	es:[di],al
DRAW_ADJUST	= 0			;;STOSB isn't used
	update
	endm


;--------------------------------Macro----------------------------------;
; tbc
;
;	tbc is a macro for generating the character drawing
;	logic for transparent mode, black text, clipped.
;
;	Usage:
;		tbc update
;	Where
;		update	the macro which will generate the required
;			destination update logic.
;-----------------------------------------------------------------------;

tbc	macro	update
	and	al,ss_clip_mask
	not	al
	and	es:[di],al
DRAW_ADJUST	= 0			;;STOSB isn't used
	update
	endm


;--------------------------------Macro----------------------------------;
; tbnc
;
;	tbnc is a macro for generating the character drawing
;	logic for transparent mode, black text, non-clipped.
;
;	Usage:
;		tbnc update
;	Where
;		update	the macro which will generate the required
;			destination update logic.
;-----------------------------------------------------------------------;

tbnc	macro	update
	not	al
	and	es:[di],al
DRAW_ADJUST	= 0			;;STOSB isn't used
	update
	endm


;--------------------------------Macro----------------------------------;
; obwc
;
;	obwc is a macro for generating the character drawing
;	logic for opaque mode, black text, white background, clipped.
;
;	Usage:
;		obwc update
;	Where
;		update	the macro which will generate the required
;			destination update logic.
;-----------------------------------------------------------------------;

obwc	macro	update
	not	al
	mov	ah,ss_clip_mask
	and	al,ah
	not	ah
	and	ah,es:[di]
	or	al,ah
	stosb
DRAW_ADJUST	= 1			;;STOSB is used
	update
	endm


;--------------------------------Macro----------------------------------;
; owbc
;
;	owbc is a macro for generating the character drawing
;	logic for opaque mode, white text, black background, clipped.
;
;	Usage:
;		owbc update
;	Where
;		update	the macro which will generate the required
;			destination update logic.
;-----------------------------------------------------------------------;

owbc	macro	update
	mov	ah,ss_clip_mask
	and	al,ah
	not	ah
	and	ah,es:[di]
	or	al,ah
	stosb
DRAW_ADJUST	= 1			;;STOSB is used
	update
	endm


;--------------------------------Macro----------------------------------;
; obwnc
;
;	obwnc is a macro for generating the character drawing
;	logic for opaque mode, black text, white background, non-clipped.
;
;	Usage:
;		obwnc update
;	Where
;		update	the macro which will generate the required
;			destination update logic.
;-----------------------------------------------------------------------;

obwnc	macro	update
	not	al
	stosb
DRAW_ADJUST	= 1			;;STOSB is used
	update
	endm


;--------------------------------Macro----------------------------------;
; owbnc
;
;	owbnc is a macro for generating the character drawing
;	logic for opaque mode, white text, black background, non-clipped.
;
;	Usage:
;		owbnc update
;	Where
;		update	the macro which will generate the required
;			destination update logic.
;-----------------------------------------------------------------------;

owbnc	macro	update
	stosb
DRAW_ADJUST	= 1			;;STOSB is used
	update
	endm


;--------------------------------Macro----------------------------------;
; obbnc
;
;	obbnc is a macro for generating the character drawing
;	logic for opaque mode, black text, black background, non-clipped.
;
;	Usage:
;		obbnc update
;	Where
;		update	the macro which will generate the required
;			destination update logic.
;-----------------------------------------------------------------------;

obbnc	macro	update
	mov	byte ptr es:[di],0
DRAW_ADJUST	= 0			;;STOSB isn't used
	update
	endm


;--------------------------------Macro----------------------------------;
; owwnc
;
;	owwnc is a macro for generating the character drawing
;	logic for opaque mode, white text, white background, non-clipped.
;
;	Usage:
;		owwnc update
;	Where
;		update	the macro which will generate the required
;			destination update logic.
;-----------------------------------------------------------------------;

owwnc	macro	update
	mov	byte ptr es:[di],0FFh
DRAW_ADJUST	= 0			;;STOSB isn't used
	update
	endm
page

createSeg _REAL,rCode,word,public,CODE
sBegin	rCode
assumes cs,rCode

;-----------------------------------------------------------------------;
;
;	The following tables are used to dispatch the various
;	combinations of drawing required for foreground/background,
;	opaque/transparent, device/bitmap, clipped/non-clipped text
;
;-----------------------------------------------------------------------;


special_case_clip_tables	label	word
	dw	rCodeOFFSET gen_cl
	dw	rCodeOFFSET gen_cl
	dw	rCodeOFFSET gen_cl
	dw	rCodeOFFSET gen_cl
	dw	rCodeOFFSET gen_cl
	dw	rCodeOFFSET gen_cl
	dw	rCodeOFFSET gen_cl
	dw	rCodeOFFSET gen_cl
if SPECIAL_CASE_DEV_TC eq 1
	dw	rCodeOFFSET dev_tc
	dw	rCodeOFFSET dev_tc
	dw	rCodeOFFSET dev_tc
	dw	rCodeOFFSET dev_tc
else
	dw	rCodeOFFSET gen_cl
	dw	rCodeOFFSET gen_cl
	dw	rCodeOFFSET gen_cl
	dw	rCodeOFFSET gen_cl
endif
	dw	rCodeOFFSET dev_oc
	dw	rCodeOFFSET dev_oc
	dw	rCodeOFFSET dev_oc
	dw	rCodeOFFSET dev_oc


special_case_non_clip_tables	label	word
if SPECIAL_CASE_BM_TBNC eq 1
	dw	rCodeOFFSET bm_tbnc
	dw	rCodeOFFSET bm_tbnc
else
	dw	rCodeOFFSET gen_nc
	dw	rCodeOFFSET gen_nc
endif
if SPECIAL_CASE_BM_TWNC eq 1
	dw	rCodeOFFSET bm_twnc
	dw	rCodeOFFSET bm_twnc
else
	dw	rCodeOFFSET gen_nc
	dw	rCodeOFFSET gen_nc
endif
if SPECIAL_CASE_BM_OBBNC eq 1
	dw	rCodeOFFSET bm_obbnc
else
	dw	rCodeOFFSET gen_nc
endif
if SPECIAL_CASE_BM_OBWNC eq 1
	dw	rCodeOFFSET bm_obwnc
else
	dw	rCodeOFFSET gen_nc
endif
if SPECIAL_CASE_BM_OWBNC eq 1
	dw	rCodeOFFSET bm_owbnc
else
	dw	rCodeOFFSET gen_nc
endif
if SPECIAL_CASE_BM_OWWNC eq 1
	dw	rCodeOFFSET bm_owwnc
else
	dw	rCodeOFFSET gen_nc
endif
if SPECIAL_CASE_DEV_TNC eq 1
	dw	rCodeOFFSET dev_tnc
	dw	rCodeOFFSET dev_tnc
	dw	rCodeOFFSET dev_tnc
	dw	rCodeOFFSET dev_tnc
else
	dw	rCodeOFFSET gen_nc
	dw	rCodeOFFSET gen_nc
	dw	rCodeOFFSET gen_nc
	dw	rCodeOFFSET gen_nc
endif
if SPECIAL_CASE_DEV_ONC eq 1
	dw	rCodeOFFSET dev_onc
	dw	rCodeOFFSET dev_onc
	dw	rCodeOFFSET dev_onc
	dw	rCodeOFFSET dev_onc
else
	dw	rCodeOFFSET gen_nc
	dw	rCodeOFFSET gen_nc
	dw	rCodeOFFSET gen_nc
	dw	rCodeOFFSET gen_nc
endif


gen_nc	label	word
	dw	rCodeOFFSET gen_nc_one_char
	dw	rCodeOFFSET gen_nc_two_char
	dw	rCodeOFFSET gen_nc_three_char
	dw	rCodeOFFSET gen_nc_four_char
	dw	rCodeOFFSET gen_nc_n_char
	dw	rCodeOFFSET gen_nc_n_char
	dw	rCodeOFFSET gen_nc_n_char
	dw	rCodeOFFSET gen_nc_n_char

gen_cl	label	word
	dw	rCodeOFFSET gen_cl_one_char
	dw	rCodeOFFSET gen_cl_two_char
	dw	rCodeOFFSET gen_cl_three_char
	dw	rCodeOFFSET gen_cl_four_char
	dw	rCodeOFFSET gen_cl_n_char
	dw	rCodeOFFSET gen_cl_n_char
	dw	rCodeOFFSET gen_cl_n_char
	dw	rCodeOFFSET gen_cl_n_char

;	This is a difficult enough mode of output that it is always
;	"special-cased".  It actually goes through the generic handlers,
;	but the entries in the first table below set up two passes
;	through the functions in the second table.

dev_oc label word
	dw	rCodeOFFSET ega_oc_outer_loop_prop
	dw	rCodeOFFSET ega_oc_outer_loop_prop
	dw	rCodeOFFSET ega_oc_outer_loop_prop
	dw	rCodeOFFSET ega_oc_outer_loop_prop
	dw	rCodeOFFSET ega_oc_outer_loop_prop
	dw	rCodeOFFSET ega_oc_outer_loop_prop
	dw	rCodeOFFSET ega_oc_outer_loop_prop
	dw	rCodeOFFSET ega_oc_outer_loop_prop

	public	ega_oc
ega_oc label word
	dw	rCodeOFFSET gen_cl_one_char
	dw	rCodeOFFSET gen_cl_two_char
	dw	rCodeOFFSET gen_cl_three_char
	dw	rCodeOFFSET gen_cl_four_char
	dw	rCodeOFFSET gen_cl_n_char
	dw	rCodeOFFSET gen_cl_n_char
	dw	rCodeOFFSET gen_cl_n_char
	dw	rCodeOFFSET gen_cl_n_char

if SPECIAL_CASE_BM_OBWNC eq 1
bm_obwnc label word
	dw	rCodeOFFSET bm_obwnc_one_char
	dw	rCodeOFFSET bm_obwnc_two_char
	dw	rCodeOFFSET bm_obwnc_three_char
	dw	rCodeOFFSET bm_obwnc_four_char
	dw	rCodeOFFSET gen_nc_n_char
	dw	rCodeOFFSET gen_nc_n_char
	dw	rCodeOFFSET gen_nc_n_char
	dw	rCodeOFFSET gen_nc_n_char
endif

if SPECIAL_CASE_BM_OWBNC eq 1
bm_owbnc label word
	dw	rCodeOFFSET bm_owbnc_one_char
	dw	rCodeOFFSET bm_owbnc_two_char
	dw	rCodeOFFSET bm_owbnc_three_char
	dw	rCodeOFFSET bm_owbnc_four_char
	dw	rCodeOFFSET gen_nc_n_char
	dw	rCodeOFFSET gen_nc_n_char
	dw	rCodeOFFSET gen_nc_n_char
	dw	rCodeOFFSET gen_nc_n_char
endif

if SPECIAL_CASE_BM_TBNC eq 1
bm_tbnc label word
	dw	rCodeOFFSET bm_tbnc_one_char
	dw	rCodeOFFSET bm_tbnc_two_char
	dw	rCodeOFFSET bm_tbnc_three_char
	dw	rCodeOFFSET bm_tbnc_four_char
	dw	rCodeOFFSET gen_nc_n_char
	dw	rCodeOFFSET gen_nc_n_char
	dw	rCodeOFFSET gen_nc_n_char
	dw	rCodeOFFSET gen_nc_n_char
endif

if SPECIAL_CASE_BM_TWNC eq 1
bm_twnc label word
	dw	rCodeOFFSET bm_twnc_one_char
	dw	rCodeOFFSET bm_twnc_two_char
	dw	rCodeOFFSET bm_twnc_three_char
	dw	rCodeOFFSET bm_twnc_four_char
	dw	rCodeOFFSET gen_nc_n_char
	dw	rCodeOFFSET gen_nc_n_char
	dw	rCodeOFFSET gen_nc_n_char
	dw	rCodeOFFSET gen_nc_n_char
endif

if SPECIAL_CASE_DEV_ONC eq 1	;Device, opaque, non-clipped
dev_onc label word
	dw	rCodeOFFSET dev_onc_one_char
	dw	rCodeOFFSET dev_onc_two_char
	dw	rCodeOFFSET dev_onc_three_char
	dw	rCodeOFFSET dev_onc_four_char
	dw	rCodeOFFSET gen_nc_n_char
	dw	rCodeOFFSET gen_nc_n_char
	dw	rCodeOFFSET gen_nc_n_char
	dw	rCodeOFFSET gen_nc_n_char
endif

if SPECIAL_CASE_DEV_TNC eq 1	;Device, transparent, non-clipped
dev_tnc label	word
	dw	rCodeOFFSET dev_tnc_one_char
	dw	rCodeOFFSET dev_tnc_two_char
	dw	rCodeOFFSET dev_tnc_three_char
	dw	rCodeOFFSET dev_tnc_four_char
	dw	rCodeOFFSET gen_nc_n_char
	dw	rCodeOFFSET gen_nc_n_char
	dw	rCodeOFFSET gen_nc_n_char
	dw	rCodeOFFSET gen_nc_n_char
endif

if SPECIAL_CASE_DEV_TC eq 1	;Device, transparent, clipped
dev_tc label	word
	dw	rCodeOFFSET dev_tc_one_char
	dw	rCodeOFFSET dev_tc_two_char
	dw	rCodeOFFSET dev_tc_three_char
	dw	rCodeOFFSET dev_tc_four_char
	dw	rCodeOFFSET gen_cl_n_char
	dw	rCodeOFFSET gen_cl_n_char
	dw	rCodeOFFSET gen_cl_n_char
	dw	rCodeOFFSET gen_cl_n_char
endif


;-----------------------------------------------------------------------;
;
;	Define the drawing logic tables.  These tables are used to
;	fetch the address of the output function for the combination
;	of foreground/background colors, opaque/transparent mode,
;	and device/bitmap.
;
;	The tables are indexed as follows:
;
;	  DEV/BM O/T FG BK  0;
;	    |	  |   |  |
;	    |	  |   |   ----------  background color (0/1)
;	    |	  |   |
;	    |	  |    -------------  foreground color (0/1)
;	    |	  |
;	    |	   -----------------  0 if transparent, 1 if opaque
;	    |
;	     ------------------------ 0 if a bitmap, 1 if the device
;-----------------------------------------------------------------------;

MASK_DEVBM	equ	00010000b
MASK_OT		equ	00001000b
MASK_FG		equ	00000100b
MASK_BK		equ	00000010b

MASK_DEV_ONC	equ	MASK_DEVBM or MASK_OT	;MASK_BK, MASK_FG wildcards
MASK_DEV_TNC	equ	MASK_DEVBM		;MASK_BK, MASK_FG wildcards
MASK_DEV_OC	equ	MASK_DEVBM or MASK_OT	;MASK_BK, MASK_FG wildcards
MASK_DEV_TC	equ	MASK_DEVBM		;MASK_BK, MASK_FG wildcards

MASK_BM_OBWNC	equ	MASK_OT or MASK_BK
MASK_BM_OWBNC	equ	MASK_OT or MASK_FG
MASK_BM_TBNC	equ	0			;MASK_BK is a wildcard
MASK_BM_TWNC	equ	MASK_FG			;MASK_BK is a wildcard

clipped_drawing_functions label word

	dw	rCodeOFFSET  bm_trans_color
	dw	rCodeOFFSET  bm_trans_color
	dw	rCodeOFFSET  bm_trans_color
	dw	rCodeOFFSET  bm_trans_color
	dw	rCodeOFFSET  bm_opaque_color
	dw	rCodeOFFSET  bm_opaque_color
	dw	rCodeOFFSET  bm_opaque_color
	dw	rCodeOFFSET  bm_opaque_color
	dw	rCodeOFFSET ega_trans_clip
	dw	rCodeOFFSET ega_trans_clip
	dw	rCodeOFFSET ega_trans_clip
	dw	rCodeOFFSET ega_trans_clip
	dw	0			;special case for opaque-clipped
	dw	0
	dw	0
	dw	0


non_clipped_drawing_functions label word

	dw	rCodeOFFSET  bm_trans_color
	dw	rCodeOFFSET  bm_trans_color
	dw	rCodeOFFSET  bm_trans_color
	dw	rCodeOFFSET  bm_trans_color
	dw	rCodeOFFSET  bm_opaque_color
	dw	rCodeOFFSET  bm_opaque_color
	dw	rCodeOFFSET  bm_opaque_color
	dw	rCodeOFFSET  bm_opaque_color
	dw	rCodeOFFSET ega_trans
	dw	rCodeOFFSET ega_trans
	dw	rCodeOFFSET ega_trans
	dw	rCodeOFFSET ega_trans
	dw	rCodeOFFSET ega_opaque_non_clip
	dw	rCodeOFFSET ega_opaque_non_clip
	dw	rCodeOFFSET ega_opaque_non_clip
	dw	rCodeOFFSET ega_opaque_non_clip


mono_non_clipped_drawing_functions	label word

	dw	rCodeOFFSET  bm_trans_black
	dw	rCodeOFFSET  bm_trans_black
	dw	rCodeOFFSET  bm_trans_white
	dw	rCodeOFFSET  bm_trans_white
	dw	rCodeOFFSET  bm_opaque_black_on_black
	dw	rCodeOFFSET  bm_opaque_black_on_white
	dw	rCodeOFFSET  bm_opaque_white_on_black
	dw	rCodeOFFSET  bm_opaque_white_on_white
page

define_frame	SmartPro
cBegin	nogen
cEnd	nogen

;--------------------------Private-Routine------------------------------;
; ega_oc_outer_loop_prop
;
;   Overall control logic for opaque clipped text to the EGA.
;
;   For proportional text, this function stands in as bit gathering
;   function (xxx_one_char, xxx_two_char, etc.). For fixed pitch text,
;   this function supersedes the clipped bit gathering functions
;   (xxx_one_in_first, xxx_two_in_last, etc.).  In either case, it sets
;   up two passes through the appropriate generic bit gathering functions.
;
;   Since the drawing function changes between passes, ss_draw_clipped
;   must be set as needed here.
;
; Entry:
;	DS:     =  Font bits segment
;	ES:DI  --> destination (ES = ScreenSelector)
;	DX	=  pointer to frame data
;	CX	=  # of source characters - 1
;	AX	=  Visible height
; Returns:
;	ES:DI  --> destination
; Error Returns:
;	None
; Registers Preserved:
;	CX,DX,DI,DS,ES
; Registers Destroyed:
;	AX,BX,SI,BP
; Calls:
;	None
; History:
;	Sun 22-Aug-1987 00:28:00 -by-  Bob Grudem [bobgru]
;	Wrote it.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-rCode------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

	public	ega_oc_outer_loop_prop

ega_oc_outer_loop_prop	proc	near

	mov	bx,cx			;create dispatch table index
	shl	bx,1
;	add	bx,rCodeOFFSET ega_oc
	mov	ss_draw_clipped,rCodeOFFSET ega_opaque_clip_1


	push	ax			;save registers for second pass
	push	bx
	push	cx
	push	dx
	push	di
	push	bp			;prop code sometimes destroys this

	push	ax			;save regs destroyed inside call
	push	dx
	call	setup_ega_opaque_clip_magic
	pop	dx
	pop	ax
	jc	@f			;skip 1st pass if planes are disabled.
	call	ega_oc[bx]		;do first pass
@@:
	mov	dx,EGA_BASE + SEQ_DATA
	mov	al,bptr colors[BACKGROUND]
	out	dx,al

	pop	bp
	pop	di			;restore registers for second pass
	pop	dx
	pop	cx
	pop	bx
;	pop	ax			;let's use ax before restoring


;	The next block of code determines if the second pass is
;	necessary.
;
;	Note that the EGA has been programmed to allow writing only to
;	the planes composing the background color. Since we're writing
;	opaque text, all planes in the background color also in the
;	foreground have been set by the first pass.  The second
;	pass cleans up the background planes not in the foreground
;	color.  If there aren't any, then skip the second pass.

;	push	ax			;already saved at top of stack
	mov	ax,ss_colors
	xor	al,ah			;determine mismatching bits
	and	al,ah			;isolate those in the bk color
	pop	ax
	jz	ega_oc_outer_loop_skip_pass_2



;	Set the return address variable in case we're fixed pitch.
;	The first "errnz" is commented out because these subroutines are
;	linked in from smartpro.asm.  The errnz should be alive there.

	sub	ss_draw_clipped,2
	errnz	<rCodeOFFSET ega_opaque_clip_1 - rCodeOFFSET ega_opaque_clip_2 - 2>
	call	ega_oc[bx]		;do second pass

ega_oc_outer_loop_chk_inner:
	test	excel,RES_EGA_INNER	;is there a middle part to string?
	jz	ega_oc_outer_loop_exit	;if no, skip EGA register setup
	call	set_ega_opaque_mode	;else, prepare EGA for full opaque
	and	excel,not RES_EGA_INNER	;prevent this at end of string

ega_oc_outer_loop_exit:
	ret


;	Update ES:DI as would have been done by second pass, then check
;	to see if EGA reprogramming is necessary for a non-clipped section
;	of opaque text.

ega_oc_outer_loop_skip_pass_2:
	inc	di			;update di for whoever follows
	jmp	short ega_oc_outer_loop_chk_inner


ega_oc_outer_loop_prop	endp
	page

;--------------------------Private-Routine------------------------------;
;   ega_trans_clip
;   ega_trans
;
;   Drawing routines for EGA, transparent mode, clipped and non-clipped.
;
; Entry:
;	AL = character to output
;	ES:DI --> destination byte
; Returns:
;	ES:DI --> same byte, next scan
; Error Returns:
;	None
; Registers Preserved:
;	BX,CX,DX,SI,DI,DS,ES,BP
; Registers Destroyed:
;	AX
; Calls:
;	None
; History:
;	Tue 25-Aug-1987 17:00:00 -by-  Bob Grudem [bobgru]
;	Wrote it.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

ega_trans_clip:
	and	al,ss_clip_mask

ega_trans:
	push	dx
	mov	dx,EGA_BASE + GRAF_DATA
	out	dx,al
	xchg	al,es:[di]
	pop	dx
DRAW_ADJUST = 0				;stosb is not used
	upd_dev	<ret>			;will generate ret
page

;--------------------------Private-Routine------------------------------;
; bm_opaque_white_on_white_clip
; bm_trans_white_clip
; bm_trans_white
;
;   Standard bitmap drawing functions for:
;
;	opaque, white on white, clipped
;	transparent, white, clipped
;	transparent, white, non-clipped
;
; Entry:
;	AL = character to output
;	ES:DI --> destination byte
; Returns:
;	ES:DI --> same byte, next scan
; Error Returns:
;	None
; Registers Preserved:
;	BX,CX,DX,SI,DI,DS,ES,BP
; Registers Destroyed:
;	AX
; Calls:
;	None
; History:
;	Wed 22-Jul-1987 17:54:46 -by-  Walt Moore [waltm]
;	Wrote it.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

bm_opaque_white_on_white_clip:
	mov	al,0FFh

bm_trans_white_clip:
	and	al,ss_clip_mask

bm_trans_white:
	or	es:[di],al

DRAW_ADJUST	= 0			;STOSB isn't used
	upd_bm	<ret>			;Will generate the ret
page

;--------------------------Private-Routine------------------------------;
; bm_opaque_black_on_black_clip
; bm_trans_black_clip
; bm_trans_black
;
;   Standard bitmap drawing functions for:
;
;	opaque, black on black, clipped
;	transparent, black, clipped
;	transparent, black, non-clipped
;
; Entry:
;	AL = character to output
;	ES:DI --> destination byte
; Returns:
;	ES:DI --> same byte, next scan
; Error Returns:
;	None
; Registers Preserved:
;	BX,CX,DX,SI,DI,DS,ES,BP
; Registers Destroyed:
;	AX
; Calls:
;	None
; History:
;	Wed 22-Jul-1987 17:54:46 -by-  Walt Moore [waltm]
;	Wrote it.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

bm_opaque_black_on_black_clip:
	mov	al,0FFh

bm_trans_black_clip:
	and	al,ss_clip_mask

bm_trans_black:
	not	al
	and	es:[di],al

DRAW_ADJUST	= 0			;STOSB isn't used
	upd_bm	<ret>			;Will generate the ret
page

;--------------------------Private-Routine------------------------------;
; bm_opaque_black_on_white
; bm_opaque_white_on_black
;
;   Standard bitmap drawing functions for:
;
;	opaque, black on white, non-clipped
;	opaque, white on black, non-clipped
;
; Entry:
;	AL = character to output
;	ES:DI --> destination byte
; Returns:
;	ES:DI --> same byte, next scan
; Error Returns:
;	None
; Registers Preserved:
;	BX,CX,DX,SI,DI,DS,ES,BP
; Registers Destroyed:
;	AX
; Calls:
;	None
; History:
;	Wed 22-Jul-1987 17:54:46 -by-  Walt Moore [waltm]
;	Wrote it.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

bm_opaque_black_on_white:
	not	al

bm_opaque_white_on_black:
	stosb

DRAW_ADJUST	= 1			;STOSB is used
	upd_bm	<ret>			;Will generate the ret
page

;--------------------------Private-Routine------------------------------;
; bm_opaque_black_on_white_clip
; bm_opaque_white_on_black_clip
;
;   Standard bitmap drawing functions for:
;
;	opaque, black on white, clipped
;	opaque, white on black, clipped
;
; Entry:
;	AL = character to output
;	ES:DI --> destination byte
; Returns:
;	ES:DI --> same byte, next scan
; Error Returns:
;	None
; Registers Preserved:
;	BX,CX,DX,SI,DI,DS,ES,BP
; Registers Destroyed:
;	AX
; Calls:
;	None
; History:
;	Wed 22-Jul-1987 17:54:46 -by-  Walt Moore [waltm]
;	Wrote it.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

bm_opaque_black_on_white_clip:
	not	al

bm_opaque_white_on_black_clip:
	mov	ah,ss_clip_mask
	and	al,ah
	not	ah
	and	ah,es:[di]
	or	al,ah
	stosb

DRAW_ADJUST	= 1			;STOSB is used
	upd_bm	<ret>			;Will generate the ret
page

;--------------------------Private-Routine------------------------------;
; bm_opaque_white_on_white
; bm_opaque_black_on_black
;
;   Standard bitmap drawing functions for:
;
;	opaque, white on white, non-clipped
;	opaque, black on black, non-clipped
;
; Entry:
;	AL = character to output
;	ES:DI --> destination byte
; Returns:
;	ES:DI --> same byte, next scan
; Error Returns:
;	None
; Registers Preserved:
;	BX,CX,DX,SI,DI,DS,ES,BP
; Registers Destroyed:
;	AX
; Calls:
;	None
; History:
;	Wed 22-Jul-1987 17:54:46 -by-  Walt Moore [waltm]
;	Wrote it.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

bm_opaque_white_on_white:
	mov	al,0FFh
	jmpnext

bm_opaque_black_on_black:
	xor	al,al
	jmpnext stop
	stosb

DRAW_ADJUST	= 1			;STOSB is used
	upd_bm	<ret>			;Will generate the ret
page

;--------------------------Private-Routine------------------------------;
; bm_trans_color_fix
; bm_trans_color
;
;   Standard bitmap drawing function for:
;
;	transparent, color
;
;  Entry points are for the fixed width code and proportional width code,
;  respectively.
;
; Entry (fixed):
;	AL = character to output
;	DX = return address
;	ES:DI --> destination byte
; Entry (prop):
;	AL = character to output
;	ES:DI --> destination byte
; Returns:
;	ES:DI --> same byte, next scan
; Error Returns:
;	None
; Registers Preserved:
;	BX,CX,DX,SI,DI,DS,ES,BP
; Registers Destroyed:
;	AX
; Calls:
;	None
; History:
;	Tue 25-Aug-1987 17:00:00 -by-  Bob Grudem [bobgru]
;	Added fixed pitch entry point to consolidate it with the
;	proportional width case.
;	Late spring, 1987        -by-  Tony Pisculli [tonyp]
;	Wrote it.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

	public	bm_trans_color_fix
	public	bm_trans_color

bm_trans_color_fix:
	push	dx			;put return address on stack

bm_trans_color:
	push	ax
	push	cx
	push	di

	xchg	bp,ss_p_frame		;get frame pointer

	mov	ch,ss_clip_mask
	or	ch,ch
	mov	ah,al
	jz	bm_trans_color_no_clip

	mov	ah,ch
	and	ah,al

bm_trans_color_no_clip:
	not	ah
	mov	cl,num_planes
	mov	ch,byte ptr ss_colors[FOREGROUND]

bm_trans_color_loop:
	and	es:[di],ah
	ror	ch,1			;see if this color plane is used
	jnc	bm_trans_color_set_byte
	or	es:[di],al

bm_trans_color_set_byte:
	add	di,next_plane
	dec	cl
	jnz	bm_trans_color_loop

	xchg	bp,ss_p_frame		;restore bp and ss_p_frame

	pop	di
	pop	cx
	pop	ax

DRAW_ADJUST = 0
	upd_bm <ret>			;Will generate the ret
page

;--------------------------Private-Routine------------------------------;
; bm_opaque_color_fix
; bm_opaque_color
;
;   Standard bitmap drawing function for:
;
;	opaque, color
;
; Entry:
;	AL = character to output
;	ES:DI --> destination byte
; Returns:
;	ES:DI --> same byte, next scan
; Error Returns:
;	None
; Registers Preserved:
;	BX,CX,DX,SI,DI,DS,ES,BP
; Registers Destroyed:
;	AX
; Calls:
;	None
; History:
;	Tue 25-Aug-1987 17:00:00 -by-  Bob Grudem [bobgru]
;	Added fixed pitch entry point to consolidate it with the
;	proportional width case.
;	Late spring, 1987        -by-  Tony Pisculli [tonyp]
;	Wrote it.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

	public	bm_opaque_color_fix
	public	bm_opaque_color
bm_opaque_color_fix:
	push	dx			;put return address on stack

bm_opaque_color:

	push	ax
	push	cx
	push	dx
	push	di

	xchg	bp,ss_p_frame		;get frame pointer

	mov	ch,ss_clip_mask
	mov	dh,num_planes
	mov	dl,special_bm_opaque_color

bm_opaque_color_partial:
	shr	dl,1			;set C to inversion mask
	sbb	ah,ah
	and	ah,al			;AH = 1 where we want NOT background
	shr	dl,1			;set C to background color
	sbb	cl,cl			;AH = background color (00 or FF)
	xor	cl,ah			;AH = destination byte
	mov	ah,es:[di]
	xor	ah,cl
	and	ah,ch
	xor	es:[di],ah		;output byte to color plane
	add	di,next_plane		;point to next color plane
	dec	dh
	jnz	bm_opaque_color_partial	;handle next color plane

	xchg	bp,ss_p_frame		;restore bp and ss_p_frame

	pop	di
	pop	dx
	pop	cx
	pop	ax

DRAW_ADJUST = 0
	upd_bm <ret>			;Will generate the ret
page

;--------------------------Private-Routine------------------------------;
; ega_opaque_non_clip:
;
;   Standard device drawing functions for:
;
;	EGA opaque mode (color combination does not matter)
;
; Entry:
;	AL = character to output
;	ES:DI --> destination byte
; Returns:
;	ES:DI --> same byte, next scan
; Error Returns:
;	None
; Registers Preserved:
;	BX,CX,DX,SI,DI,DS,ES,BP
; Registers Destroyed:
;	AX
; Calls:
;	None
; History:
;	Wed 22-Jul-1987 17:54:46 -by-  Walt Moore [waltm]
;	Wrote it.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

	public	ega_opaque_non_clip
ega_opaque_non_clip:
	stosb

DRAW_ADJUST	= 1			;STOSB is used
	upd_dev <ret>			;Will generate the ret
page

;--------------------------Private-Routine------------------------------;
;   ega_opaque_clip_2
;   ega_opaque_clip_1
;
;   Device drawing functions for:
;
;	opaque, clipped
;
;   Two passes are required for this mode of text output.  Label
;   ega_opaque_clip_x is the drawing function for pass x.
;
; Entry:
;	AL = character to output
;	ES:DI --> destination byte
; Returns:
;	ES:DI --> same byte, next scan
; Error Returns:
;	None
; Registers Preserved:
;	BX,CX,DX,SI,DI,DS,ES,BP
; Registers Destroyed:
;	AX
; Calls:
;	None
; History:
;	Wed 22-Jul-1987 17:54:46 -by-  Walt Moore [waltm]
;	Wrote it.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

	public	ega_opaque_clip_2
	public	ega_opaque_clip_1
ega_opaque_clip_2:
	not	al
ega_opaque_clip_1:
	xchg	al,es:[di]

DRAW_ADJUST	= 0			;STOSB is not used
	upd_dev <ret>			;Will generate the ret
page

;--------------------------Private-Routine------------------------------;
; preset_pro_text
;
;  Set any frame variables and stack locations (in the StrStuff segment)
;  required to output text with the current attributes.
;
; Entry:
;	BL = accel
;	AH = excel
;	SS = StrStuff
; Returns:
;	None
; Error Returns:
;	None
; Registers Preserved:
;	DX,BP,DS,ES
; Registers Destroyed:
;	AX,BX,CX,SI,DI,FLAGS
; Calls:
;	None
; History:
;	Tue 25-Aug-1987 17:00:00 -by-  Bob Grudem [bobgru]
;	Rewrote assignment of bit-gathering and drawing functions.
;	Fri 24-Jul-1987 13:21:18 -by-  Walt Moore [waltm]
;	Wrote it.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

	public	preset_pro_text
preset_pro_text proc	near

	mov	ss_p_frame,bp
	mov	cx,next_scan
	mov	ss_next_scan,cx
	mov	cx,colors
	mov	ss_colors,cx

;	Accumulate foreground/background colors, opaque/transparent mode,
;	and device/bitmap to determine which drawing functions will be
;	used.

	and	bx,IS_OPAQUE		;BL = 0000000 O/T
	errnz	IS_OPAQUE-00000001
;	mov	cx,colors
	shr	cl,1
	rcl	bl,1			;BL = 000000 O/T FG
	errnz	FOREGROUND
	shr	ch,1			;BL = 00000 O/T FG BK
	rcl	bl,1
	errnz	BACKGROUND-FOREGROUND-1
	and	ah,IS_DEVICE
	or	bl,ah			;BL = 0000 DEV/BM O/T FG BK
	errnz	IS_DEVICE-00001000b
	shl	bx,1			;BX = 00000000 000 DEV/BM O/T FG BK 0


;	Now that we have an index into our tables of drawing functions,
;	find out which table to use.  We use special tables for the
;	nonclipped cases of device and monochrome bitmaps.  For color
;	bitmaps, and all clipped cases, we'll get the drawing function
;	from the general case table.

	mov	ax,non_clipped_drawing_functions[bx]

	test	bl,MASK_DEVBM
	errnz	<MASK_DEVBM and 0FF00h>
	jnz	preset_pro_text_lookup_func
	cmp	num_planes,1
	jne	preset_pro_text_standard_func

	mov	ax,mono_non_clipped_drawing_functions[bx]

preset_pro_text_lookup_func:
	mov	si,special_case_non_clip_tables[bx]
	mov	di,special_case_clip_tables[bx]

preset_pro_text_have_func:
	mov	non_clipped_table,si
	mov	clipped_table,di
	mov	ss_draw,ax

	mov	ax,clipped_drawing_functions[bx]
	mov	ss_draw_clipped,ax

	ret


preset_pro_text_standard_func:
	mov	si,rCodeOFFSET gen_nc
	mov	di,rCodeOFFSET gen_cl
	jmp	short preset_pro_text_have_func

preset_pro_text endp
	page

	public	setup_ega_opaque_clip_magic
;--------------------------------------------------------------------------;
; setup_ega_opaque_clip_magic
;
; Entry:
;	BP	= frame pointer
; Returns:
;	none
; Error Returns:
;	none
; Registers Destroyed:
;	AX, DX
; Registers Preserved:
;	BX, CX, SI, DI, BP, DS, ES
; Calls:
;	none
; History:
;  Sat Apr 18, 1987 10:47:53p	-by-  Tony Pisculli	[tonyp]
; wrote it
;--------------------------------------------------------------------------;

setup_ega_opaque_clip_magic	proc near

	mov	dx,EGA_BASE + GRAF_ADDR
	mov	ax,DR_SET shl 8 + GRAF_DATA_ROT
	out16	dx,ax

	mov	ah,ss_clip_mask
	mov	al,GRAF_BIT_MASK
	out16	dx,ax

	mov	ah,bptr ss_colors[FOREGROUND]
	mov	al,GRAF_SET_RESET
	out16	dx,ax

	xor	ah,bptr ss_colors[BACKGROUND]
	not	ah
	mov	al,GRAF_ENAB_SR
	out16	dx,ax

	mov	dx,EGA_BASE + SEQ_DATA
	or	ah,bptr ss_colors[FOREGROUND]
	and	ah,0fh
	jz	@f
	mov	al,ah
	out	dx,al
	clc
	ret

@@:
	stc
	ret

setup_ega_opaque_clip_magic	endp
	page

;-----------------------------------------------------------------------;
;
;	The following cases are the general purpose routines for
;	outputting the text.  They will call through the stack
;	locations ss_draw and ss_draw_clipped to perform the
;	actual output operations.
;
;	Following the general purpose routines are any special
;	case routines.
;
;-----------------------------------------------------------------------;

	one_char	<gen_nc>,<non_clipped_output>,<loop_logic>,<sub>
	two_char	<gen_nc>,<non_clipped_output>,<loop_logic>,<sub>
	three_char	<gen_nc>,<non_clipped_output>,<loop_logic>,<sub>
	four_char	<gen_nc>,<non_clipped_output>,<loop_logic>,<sub>
	n_char		<gen_nc>,<non_clipped_output>,<loop_logic>,<sub>

	one_char	<gen_cl>,<clipped_output>,<loop_logic>,<sub>
	two_char	<gen_cl>,<clipped_output>,<loop_logic>,<sub>
	three_char	<gen_cl>,<clipped_output>,<loop_logic>,<sub>
	four_char	<gen_cl>,<clipped_output>,<loop_logic>,<sub>
	n_char		<gen_cl>,<clipped_output>,<loop_logic>,<sub>

if SPECIAL_CASE_DEV_ONC eq 1
	one_char	<dev_onc>,<ega_onc>,<upd_dev loop_logic>,<inline>
	two_char	<dev_onc>,<ega_onc>,<upd_dev loop_logic>,<inline>
	three_char	<dev_onc>,<ega_onc>,<upd_dev loop_logic>,<inline>
	four_char	<dev_onc>,<ega_onc>,<upd_dev loop_logic>,<sub>
;	n_char		<dev_onc>,<ega_onc>,<upd_dev loop_logic>,<sub>
endif

if SPECIAL_CASE_DEV_TNC eq 1
	one_char	<dev_tnc>,<ega_tnc>,<upd_dev loop_logic>,<inline>
	two_char	<dev_tnc>,<ega_tnc>,<upd_dev loop_logic>,<inline>
	three_char	<dev_tnc>,<ega_tnc>,<upd_dev loop_logic>,<inline>
	four_char	<dev_tnc>,<ega_tnc>,<upd_dev loop_logic>,<sub>
;	n_char		<dev_tnc>,<ega_tnc>,<upd_dev loop_logic>,<sub>
endif

if SPECIAL_CASE_DEV_TC eq 1
	one_char	<dev_tc>,<ega_tc>,<upd_dev loop_logic>,<sub>
	two_char	<dev_tc>,<ega_tc>,<upd_dev loop_logic>,<sub>
	three_char	<dev_tc>,<ega_tc>,<upd_dev loop_logic>,<sub>
	four_char	<dev_tc>,<ega_tc>,<upd_dev loop_logic>,<sub>
;	n_char		<dev_tc>,<ega_tc>,<upd_dev loop_logic>,<sub>
endif

if SPECIAL_CASE_BM_OBWNC eq 1
	one_char	<bm_obwnc>,<obwnc>,<upd_bm loop_logic>,<sub>
	two_char	<bm_obwnc>,<obwnc>,<upd_bm loop_logic>,<sub>
	three_char	<bm_obwnc>,<obwnc>,<upd_bm loop_logic>,<sub>
	four_char	<bm_obwnc>,<obwnc>,<upd_bm loop_logic>,<sub>
;	n_char		<bm_obwnc>,<obwnc>,<upd_bm loop_logic>,<sub>
endif

if SPECIAL_CASE_BM_OWBNC eq 1
	one_char	<bm_owbnc>,<owbnc>,<upd_bm loop_logic>,<sub>
	two_char	<bm_owbnc>,<owbnc>,<upd_bm loop_logic>,<sub>
	three_char	<bm_owbnc>,<owbnc>,<upd_bm loop_logic>,<sub>
	four_char	<bm_owbnc>,<owbnc>,<upd_bm loop_logic>,<sub>
;	n_char		<bm_owbnc>,<owbnc>,<upd_bm loop_logic>,<sub>
endif

if SPECIAL_CASE_BM_TBNC eq 1
	one_char	<bm_tbnc>,<tbnc>,<upd_bm loop_logic>,<sub>
	two_char	<bm_tbnc>,<tbnc>,<upd_bm loop_logic>,<sub>
	three_char	<bm_tbnc>,<tbnc>,<upd_bm loop_logic>,<sub>
	four_char	<bm_tbnc>,<tbnc>,<upd_bm loop_logic>,<sub>
;	n_char		<bm_tbnc>,<tbnc>,<upd_bm loop_logic>,<sub>
endif

if SPECIAL_CASE_BM_TWNC eq 1
	one_char	<bm_twnc>,<twnc>,<upd_bm loop_logic>,<sub>
	two_char	<bm_twnc>,<twnc>,<upd_bm loop_logic>,<sub>
	three_char	<bm_twnc>,<twnc>,<upd_bm loop_logic>,<sub>
	four_char	<bm_twnc>,<twnc>,<upd_bm loop_logic>,<sub>
;	n_char		<bm_twnc>,<twnc>,<upd_bm loop_logic>,<sub>
endif
page
sEnd	rCode
	end

