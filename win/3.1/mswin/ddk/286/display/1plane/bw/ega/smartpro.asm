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
; if SPECIAL_CASE_DEV_OBWNC
;	dev_obwnc_one_char
;	dev_obwnc_two_char
;	dev_obwnc_three_char
;	dev_obwnc_four_char
;	dev_obwnc_n_char
;	dev_obwnc_n_char
;	dev_obwnc_n_char
;	dev_obwnc_n_char
; endif
;
; if SPECIAL_CASE_DEV_TBNC
;	dev_tbnc_one_char
;	dev_tbnc_two_char
;	dev_tbnc_three_char
;	dev_tbnc_four_char
;	dev_tbnc_n_char
;	dev_tbnc_n_char
;	dev_tbnc_n_char
;	dev_tbnc_n_char
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
	include cmacros.inc
	include fontseg.inc
	include macros.mac
	include strblt.inc
	include gdidefs.inc
	.list

	externA SCREEN_W_BYTES


;	Special cases

SPECIAL_CASE_DEV_OBWNC =1	;Device, opaque black on white, non-clipped
SPECIAL_CASE_DEV_TBNC  =0	;Device, transparent black, non-clipped


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

name&_n_char_inner_loop:
	mov	bx,[bp][si].cchar	;;BX = offset of bits
	mov	al,[bx][di]
	mov	cl,[bp][si].wwidth	;;CL = width
	shl	ax,cl
	sub	si,4			;;--> next char
	dec	ch
	jnz	name&_n_char_inner_loop
	mov	bx,[bp][si].cchar	;;BX = offset of bits
	mov	al,[bx][di]
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
	mov	al,[si][bx]
	shl	ax,cl
	mov	al,ds:[si][bp]
	xchg	cl,ch
	shl	ax,cl
	xchg	cl,ch
	xchg	cx,ss_phases
	xchg	bx,dx
	mov	al,[si][bx]
	xchg	bx,dx
	shl	ax,cl
	lodsb
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
	mov	al,ds:[si][bp]
	mov	cl,dl
	shl	ax,cl
	mov	al,[si][bx]
	mov	cl,dh
	shl	ax,cl
	lodsb
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
;	process a destination character consisting of 3 source bytes
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
	mov	al,[bx][si]
	shl	ax,cl
	xchg	cl,ch
	lodsb
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
;	process a destination character consisting of 3 source bytes
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
;	tw_non_clipped is a macro for generating the character drawing
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
;	obwnc is a macro for generating the character drawing
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

sBegin	Code
assumes cs,Code

;-----------------------------------------------------------------------;
;
;	The following tables are used to dispatch the various
;	combinations of drawing required for foreground/background,
;	opaque/transparent, device/bitmap, clipped/non-clipped text
;
;-----------------------------------------------------------------------;

gen_nc	label	word
	dw	CodeOFFSET gen_nc_one_char
	dw	CodeOFFSET gen_nc_two_char
	dw	CodeOFFSET gen_nc_three_char
	dw	CodeOFFSET gen_nc_four_char
	dw	CodeOFFSET gen_nc_n_char
	dw	CodeOFFSET gen_nc_n_char
	dw	CodeOFFSET gen_nc_n_char
	dw	CodeOFFSET gen_nc_n_char

gen_cl	label	word
	dw	CodeOFFSET gen_cl_one_char
	dw	CodeOFFSET gen_cl_two_char
	dw	CodeOFFSET gen_cl_three_char
	dw	CodeOFFSET gen_cl_four_char
	dw	CodeOFFSET gen_cl_n_char
	dw	CodeOFFSET gen_cl_n_char
	dw	CodeOFFSET gen_cl_n_char
	dw	CodeOFFSET gen_cl_n_char



if SPECIAL_CASE_DEV_OBWNC eq 1	;Device, opaque black on white, non-clipped

dev_obwnc label word
	dw	CodeOFFSET dev_obwnc_one_char
	dw	CodeOFFSET dev_obwnc_two_char
	dw	CodeOFFSET dev_obwnc_three_char
	dw	CodeOFFSET dev_obwnc_four_char
;	dw	CodeOFFSET dev_obwnc_n_char
;	dw	CodeOFFSET dev_obwnc_n_char
;	dw	CodeOFFSET dev_obwnc_n_char
;	dw	CodeOFFSET dev_obwnc_n_char
	dw	CodeOFFSET gen_nc_n_char
	dw	CodeOFFSET gen_nc_n_char
	dw	CodeOFFSET gen_nc_n_char
	dw	CodeOFFSET gen_nc_n_char

endif


if SPECIAL_CASE_DEV_TBNC eq 1	;Device, transparent black, non-clipped

dev_tbnc label	word
	dw	CodeOFFSET dev_tbnc_one_char
	dw	CodeOFFSET dev_tbnc_two_char
	dw	CodeOFFSET dev_tbnc_three_char
	dw	CodeOFFSET dev_tbnc_four_char
;	dw	CodeOFFSET dev_tbnc_n_char
;	dw	CodeOFFSET dev_tbnc_n_char
;	dw	CodeOFFSET dev_tbnc_n_char
;	dw	CodeOFFSET dev_tbnc_n_char
	dw	CodeOFFSET gen_nc_n_char
	dw	CodeOFFSET gen_nc_n_char
	dw	CodeOFFSET gen_nc_n_char
	dw	CodeOFFSET gen_nc_n_char
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

clipped_drawing_functions label word

	dw	CodeOFFSET  bm_trans_black_clip
	dw	CodeOFFSET  bm_trans_black_clip
	dw	CodeOFFSET  bm_trans_white_clip
	dw	CodeOFFSET  bm_trans_white_clip
	dw	CodeOFFSET  bm_opaque_black_on_black_clip
	dw	CodeOFFSET  bm_opaque_black_on_white_clip
	dw	CodeOFFSET  bm_opaque_white_on_black_clip
	dw	CodeOFFSET  bm_opaque_white_on_white_clip
	dw	CodeOFFSET dev_trans_black_clip
	dw	CodeOFFSET dev_trans_black_clip
	dw	CodeOFFSET dev_trans_white_clip
	dw	CodeOFFSET dev_trans_white_clip
	dw	CodeOFFSET dev_opaque_black_on_black_clip
	dw	CodeOFFSET dev_opaque_black_on_white_clip
	dw	CodeOFFSET dev_opaque_white_on_black_clip
	dw	CodeOFFSET dev_opaque_white_on_white_clip


non_clipped_drawing_functions label word

	dw	CodeOFFSET  bm_trans_black
	dw	CodeOFFSET  bm_trans_black
	dw	CodeOFFSET  bm_trans_white
	dw	CodeOFFSET  bm_trans_white
	dw	CodeOFFSET  bm_opaque_black_on_black
	dw	CodeOFFSET  bm_opaque_black_on_white
	dw	CodeOFFSET  bm_opaque_white_on_black
	dw	CodeOFFSET  bm_opaque_white_on_white
INDEX_DEV_TBNC	= offset $ - offset non_clipped_drawing_functions
	dw	CodeOFFSET dev_trans_black
INDEX_DEV_TBNC_ALT = offset $ - offset non_clipped_drawing_functions
	dw	CodeOFFSET dev_trans_black
	dw	CodeOFFSET dev_trans_white
	dw	CodeOFFSET dev_trans_white
	dw	CodeOFFSET dev_opaque_black_on_black
INDEX_DEV_OBWNC = offset $ - offset non_clipped_drawing_functions
	dw	CodeOFFSET dev_opaque_black_on_white
	dw	CodeOFFSET dev_opaque_white_on_black
	dw	CodeOFFSET dev_opaque_white_on_white
page

define_frame	SmartPro
cBegin	nogen
cEnd	nogen

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
;	Created.
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
;	Created.
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
;	Created.
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
;	Created.
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
;	Created.
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
; dev_opaque_white_on_white_clip
; dev_trans_white_clip
; dev_trans_white
;
;   Standard device drawing functions for:
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
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

dev_opaque_white_on_white_clip:
	mov	al,0FFh

dev_trans_white_clip:
	and	al,ss_clip_mask

dev_trans_white:
	or	es:[di],al

DRAW_ADJUST	= 0			;STOSB isn't used
	upd_dev <ret>			;Will generate the ret
page

;--------------------------Private-Routine------------------------------;
; dev_opaque_black_on_black_clip
; dev_trans_black_clip
; dev_trans_black
;
;   Standard device drawing functions for:
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
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

dev_opaque_black_on_black_clip:
	mov	al,0FFh

dev_trans_black_clip:
	and	al,ss_clip_mask

dev_trans_black:
	not	al
	and	es:[di],al

DRAW_ADJUST	= 0			;STOSB isn't used
	upd_dev <ret>			;Will generate the ret
page

;--------------------------Private-Routine------------------------------;
; dev_opaque_black_on_white
; dev_opaque_white_on_black
;
;   Standard device drawing functions for:
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
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

dev_opaque_black_on_white:
	not	al

dev_opaque_white_on_black:
	stosb

DRAW_ADJUST	= 1			;STOSB is used
	upd_dev <ret>			;Will generate the ret
page

;--------------------------Private-Routine------------------------------;
; dev_opaque_black_on_white_clip
; dev_opaque_white_on_black_clip
;
;   Standard device drawing functions for:
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
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

dev_opaque_black_on_white_clip:
	not	al

dev_opaque_white_on_black_clip:
	mov	ah,ss_clip_mask
	and	al,ah
	not	ah
	and	ah,es:[di]
	or	al,ah
	stosb

DRAW_ADJUST	= 1			;STOSB is used
	upd_dev <ret>			;Will generate the ret
page

;--------------------------Private-Routine------------------------------;
; dev_opaque_white_on_white
; dev_opaque_black_on_black
;
;   Standard device drawing functions for:
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
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

dev_opaque_white_on_white:
	mov	al,0FFh
	jmpnext

dev_opaque_black_on_black:
	xor	al,al
	jmpnext stop
	stosb

DRAW_ADJUST	= 1			;STOSB is used
	upd_dev <ret>			;Will generate the ret
page

;--------------------------Private-Routine------------------------------;
; preset_pro_text
;
;  any frame variables and stack locations (in the StrStuff segment)
;  required to output text with the current attributes is set.
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
;	DX,SI,DI,BP,DS,ES
; Registers Destroyed:
;	AX,BX,CX,FLAGS
; Calls:
;	None
; History:
;	Fri 24-Jul-1987 13:21:18 -by-  Walt Moore [waltm]
;	Created.
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

;	Accumulate foreground/background colors, opaque/transparent mode,
;	and device/bitmap to determine which drawing functions will be
;	used.

	and	bx,IS_OPAQUE		;BL = 0000000 O/T
	errnz	IS_OPAQUE-00000001
	mov	cx,colors
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


;	All clipped text will use the generic handlers.  Since the "n"
;	character case is passed to the default handler, we must set that
;	address also.


	mov	ax,clipped_drawing_functions[bx]
	mov	ss_draw_clipped,ax
	mov	clipped_table,CodeOFFSET gen_cl
	mov	ax,next_scan
	mov	ss_next_scan,ax
	mov	ax,non_clipped_drawing_functions[bx]
	mov	ss_draw,ax


if SPECIAL_CASE_DEV_OBWNC eq 1
	mov	ax,CodeOFFSET dev_obwnc ;If this is the correct special case
	cmp	bl,INDEX_DEV_OBWNC	;  then set dispatcher for it
	je	preset_pro_text_finish
endif


if SPECIAL_CASE_DEV_TBNC eq 1
	mov	ax,CodeOFFSET dev_tbnc	;If this is the correct special case
	cmp	bl,INDEX_DEV_TBNC	;  then set dispatcher for it
	je	preset_pro_text_finish
	cmp	bl,INDEX_DEV_TBNC_ALT	;  then set dispatcher for it
	je	preset_pro_text_finish
endif
	mov	ax,CodeOFFSET gen_nc	;Generic handler will be used

preset_pro_text_finish:
	mov	non_clipped_table,ax	;Save non-clipped dispatch table
	ret

preset_pro_text endp
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


if SPECIAL_CASE_DEV_OBWNC eq 1	;Device, opaque black on white, non-clipped

	one_char	<dev_obwnc>,<obwnc>,<upd_dev loop_logic>,<inline>
	two_char	<dev_obwnc>,<obwnc>,<upd_dev loop_logic>,<inline>
	three_char	<dev_obwnc>,<obwnc>,<upd_dev loop_logic>,<inline>
	four_char	<dev_obwnc>,<obwnc>,<upd_dev loop_logic>,<sub>
;	n_char		<dev_obwnc>,<obwnc>,<upd_dev loop_logic>,<sub>
endif


if SPECIAL_CASE_DEV_TBNC eq 1	;Device, transparent black, non-clipped

	one_char	<dev_tbnc>,<tbnc>,<upd_dev loop_logic>,<sub>
	two_char	<dev_tbnc>,<tbnc>,<upd_dev loop_logic>,<sub>
	three_char	<dev_tbnc>,<tbnc>,<upd_dev loop_logic>,<sub>
	four_char	<dev_tbnc>,<tbnc>,<upd_dev loop_logic>,<sub>
;	n_char		<dev_tbnc>,<tbnc>,<upd_dev loop_logic>,<sub>
endif


sEnd	Code
end

