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
;       Thirs 21-Dec-1990 14:00:00 -by-  Ray Patrick [raypat]
;	Modified -- Added performance enhancements as follows:
;         1) Added self-modifying code in output routines which codes on the 
;            fly rol <immediate> and shr <immediate> opcodes into output 
;	     routines. This eliminates logic to move shift count into 
;            cl register.
;         2) Transparent mode only -- Added short circuit code to avoid
;            screen memory write when bit pattern is all zero.
;         
;       Fri 22-Oct-1990 14:00:00 -by-  Ray Patrick [raypat]
;	Modified -- for TrueType.
;         Replaced "mov" instructions with "or" in output routines
;         to allow overlapped characters (ala TrueType).  Also,
;         replaced "shl" with "rol" instructions in output routines
;         to allow negative widths of each character ORed in.  This
;         *greatly* simplfies the TrueType stack building logic.

;       Thu 06-Apr-1989 10:20:10 -by-  Amit Chatterjee [amitc]
;	Modified.
;          This TextOut function has 80386 soecific code in it, so
;          will non run on 8086 or 80286 protected mode. So now all
;          the text function is being put in a separate fixed segment
;          with the 8086 TextOut appearing in another fixed segment and
;          one of these two segments chosen at enable time.	
;		. Moved code to _PROTECT segment
;		. prefixed a 'p_' to all public labels
;
;	Fri 27-Jan-1989 14:46:50 -by-  Amit Chatterjee [amitc]
;       Modified code to support >64k fonts. 
;                . The header in the font segment now has 6 byte entries
;                  per character, with the last 4 bytes being a 32 bit 
;                  pointer to the bits in the same segment.	
;		
;		 . At this point 16 bit code and 16 bit data is still being
;                  used, however where ever necessary we have used the 
;		   extended register set and the address override to take
;		   advantage of 32 bit code and data capabilities.
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
	include fontseg.inc
	include	display.inc
	include	ega.inc
	include	egamem.inc
	include strblt.inc
	.list

	externA SCREEN_W_BYTES
	externA ScreenSelector
	externNP p_set_ega_opaque_mode

;	Special cases

SPECIAL_CASE_BM_OBBNC = 0	;Bitmap, opaque, black on black, non-clipped
SPECIAL_CASE_BM_OBWNC = 1	;Bitmap, opaque, black on white, non-clipped
SPECIAL_CASE_BM_OWBNC = 0	;Bitmap, opaque, white on black, non_clipped
SPECIAL_CASE_BM_OWWNC = 0	;Bitmap, opaque, white on white, non-clipped
SPECIAL_CASE_BM_TBNC  = 0	;Bitmap, transparent black, non-clipped
SPECIAL_CASE_BM_TWNC  = 0	;Bitmap, transparent white, non-clipped

SPECIAL_CASE_DEV_ONC  = 1	;Device, opaque, non-clipped
SPECIAL_CASE_DEV_TNC  = 1	;Device, transparent, non-clipped
;SPECIAL_CASE_DEV_OC  = 1	;Device, opaque, clipped (see notes below)
SPECIAL_CASE_DEV_TC   = 1	;Device, transparent, clipped

; Speed-ups

SELF_MODIFY = 1			;Self modifying code speed up.
TRANSPARENT_SPEEDUP = 1		;Speed up for transparant mode.

;-----------------------------------------------------------------------;
;	The following equates are used to index into the buffer
;	of character widths, phases (x location), and offsets
;	of the bits within the font.
;-----------------------------------------------------------------------;
base	equ	0

wwidth	equ	byte ptr 0
pphase	equ	byte ptr 1
cchar	equ	dword ptr -4

wwidth1 equ	byte ptr base - 0
pphase1 equ	byte ptr wwidth1 + 1
cchar1	equ	dword ptr wwidth1 - 4

wwidth2 equ	byte ptr base - 6
pphase2 equ	byte ptr wwidth2 + 1
cchar2	equ	dword ptr wwidth2 - 4

wwidth3 equ	byte ptr base - 12
pphase3 equ	byte ptr wwidth3 + 1
cchar3	equ	dword ptr wwidth3 - 4

wwidth4 equ	byte ptr base - 18
pphase4 equ	byte ptr wwidth4 + 1
cchar4	equ	dword ptr wwidth4 - 4

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
	sub	di,DRAW_ADJUST		;;extra adjust for stosb or stosw
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

;----------------------------------------------------------------------------;

page

;----------------------------------------------------------------------------;
; The bit gathering code generating macros are defined next. These logic of  ;
; the code is quite general in the sense that it can be used to gather 8 or  ;
; 16 bits (actually can gather upto 24 bits). The gathered bits will either  ;
; be in AL (for 8 bit gathering) or in AX (for 16 bit gathering). However the;
; 'UPDATE' will mostly be for 16 bit output. Now clipped case would always   ;
; output 8 bytes. However for non-clipped cases if we have an odd number of  ;
; inner bytes we will have to gather 8 bits for the last byte, but we will   ;
; force this into the clip case with a clip mask of 0ffh. As the update      ;
; will normally convert 16 bits at a time, CELL_ADJUST will have the appropr-;
; -ate decrement. But for clipped cases CELL_ADJUST needs to be 1 more. To   ;
; take care of this we have added one more parameter to the macro,namely,    ;
; DIDEC. DIDEC will be set to non_blank only for the macros for the clipped_ ;
; -cases. If it is non blank, we will force a DEC DI instruction.	     ;
;----------------------------------------------------------------------------;

;--------------------------------Macro----------------------------------;
; n_char
;
;	n_char is a macro for generating the code required to process
;	a destination character consisting of 5,6,7 or 8 source bytes
;
;	Usage:
;		n_char	name,output,update,setup,didec
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
;		didec	if not blank, it expands into dec di, which
;			is needed for 8 bit update code
;
;	The macro will generate the loop_logic macro which is the
;	macro used by all update macros for generating looping logic.
;	It is defined as a macro so that devices which are interleaved
;	can make multiple copies of it, possibly removing a jump.
;-----------------------------------------------------------------------;

n_char	macro	name,output,update,setup,didec
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
	jnz	short name&_n_char_outer_loop
	pop	bp			;;BP = frame pointer
	sub	di,cell_adjust
	&endm

;-----------------------------------------------------------------------;
; n character long looping logic macro
;-----------------------------------------------------------------------;

long_loop_logic	&macro
	inc	dx			;;Next scan of font
	dec	ss_height		;;Loop until all chars output
	jz	short name&_n_char_exit
	jmp	name&_n_char_outer_loop
name&_n_char_exit:
	pop	bp			;;BP = frame pointer
	sub	di,cell_adjust
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
  ifndef p_n_char_setup			;;  if procedure not defined
    genflag = 11b			;;    show proc must also be generated
  else					;;  else
    ife $-p_n_char_setup			;;    a hack since it is defined on
      genflag = 11b			;;    pass 2 regardless of if we have
    endif				;;    generated it
  endif 				;;
endif					;;
if genflag				;;if to generate setup
  if genflag and 10b			;;  if generating procedural version
public	p_n_char_setup			;;    make public for debugging
p_n_char_setup proc near	    	;;    define start of setup proc
  endif 				;;  endif
	mov	ss_height,ax		;;  save # scans in character
  if genflag and 10b			;;  if generating setup procedure
	pop	ax			;;    get return address
  endif 				;;  endif
  	mov	ch,ss_boldflag		;;get the bold flag
	and	ch,1			;;isolate just bold flag
	shl	cl,1			;;make room for bold flag
	or	cl,ch			;;n can't be > 127, or in bold flag 
	mov	ss_num_chars,cl 	;;  save # of characters - 1
	push	bp			;;  save frame pointer
	mov	bp,dx			;;  set buffer pointer
	xor	dx,dx			;;  index into font scan
	movzx	edi,di
  if genflag and 10b			;;  if generating setup procedure
	jmp	ax			;;    dispatch to caller
p_n_char_setup endp			;;    terminate the procedure
name&_n_char proc near			;;    define actual procedure
	call	p_n_char_setup		;;    call setup code
  endif 				;;  endif
else					;;else
name&_n_char proc near			;;    define actual procedure
	call	p_n_char_setup		;;    call setup code
endif					;;endif ;setup code

;-----------------------------------------------------------------------;
; n character compilation logic
;-----------------------------------------------------------------------;

name&_n_char_outer_loop:
	xor	eax,eax
	xor	si,si			;;SI = char index
	mov	ch,ss_num_chars 	;;Get # of characters - 1
	shr	ch,1			;;throw bold flag out, get count
	xchg	dx,di			;;Index to next font scan in DI
name&_n_char_inner_loop:
	mov	ebx,[bp][si].cchar	;;EBX = offset of bits
	or	al,[ebx][edi]
	mov	cl,[bp][si].wwidth	;;CL = width
	rol	eax,cl
	sub	si,6			;;--> next char
	dec	ch
	jnz	name&_n_char_inner_loop
	mov	ebx,[bp][si].cchar	;;BX = offset of bits
	or	al,[ebx][edi]
name&_n_char_output:
	mov	cl,[bp][si].pphase	;;CL = phase
	shr	eax,cl
	xchg	dx,di			;;DI = dest ptr
	output	<update>		;;Macro to do whatever for outputting
ifnb	<didec>
	dec	di			;;for 8 bit update
endif
	ret

name&_n_char  endp
	endm


;--------------------------------Macro----------------------------------;
; five_char
;
;	five_char is a macro for generating the code required to process
;	a destination character consisting of 5,6,7 or 8 source bytes
;
;	Usage:
;		five_char	name,output,update,setup,didec
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
;		didec	if not blank, it expands into dec di, which
;			is needed for 8 bit update code
;
;	The macro will generate the loop_logic macro which is the
;	macro used by all update macros for generating looping logic.
;	It is defined as a macro so that devices which are interleaved
;	can make multiple copies of it, possibly removing a jump.
;-----------------------------------------------------------------------;

five_char	macro	name,output,update,setup,didec
	local	genflag

	assumes ds,nothing		;;Set the assumptions now
	assumes es,nothing
	assumes ss,StrStuff

;-----------------------------------------------------------------------;
; 5 character looping logic macro
;-----------------------------------------------------------------------;

loop_logic	&macro
	inc	dx			;;Next scan of font
	dec	si			;;Loop until all chars output
	jnz	short name&_five_char_outer_loop
	pop	bp			;;BP = frame pointer
	sub	di,cell_adjust
	&endm

;-----------------------------------------------------------------------;
; 5 character long looping logic macro
;-----------------------------------------------------------------------;

long_loop_logic	&macro
	inc	dx			;;Next scan of font
	dec	si			;;Loop until all chars output
	jz	short name&_five_char_exit
	jmp	name&_five_char_outer_loop
name&_five_char_exit:
	pop	bp			;;BP = frame pointer
	sub	di,cell_adjust
	&endm

;-----------------------------------------------------------------------;
; 5 character setup logic
;-----------------------------------------------------------------------;
public	name&_five_char			;;make public
genflag=0				;;assume out_of_line, already generated
ifidn <setup>,<inline>			;;if in_line
name&_five_char proc near			;;  define procedure
  genflag = 01b 			;;  show code must be generated
else					;;else
  ifndef p_five_char_setup		;;  if procedure not defined
    genflag = 11b			;;    show proc must also be generated
  else					;;  else
    ife $-p_five_char_setup		;;    a hack since it is defined on
      genflag = 11b			;;    pass 2 regardless of if we have
    endif				;;    generated it
  endif 				;;
endif					;;
if genflag				;;if to generate setup
  if genflag and 10b			;;  if generating procedural version
public	p_five_char_setup  		;;    make public for debugging
p_five_char_setup proc near  		;;    define start of setup proc
  endif 				;;  endif
	mov	si,ax			;;  save # scans in character
  if genflag and 10b			;;  if generating setup procedure
	pop	ax			;;    get return address
  endif 				;;  endif
	push	bp			;;  save frame pointer
	mov	bp,dx			;;  set buffer pointer
	xor	dx,dx			;;  index into font scan
	movzx	edi,di			;;clear out high word
  if genflag and 10b			;;  if generating setup procedure
	jmp	ax			;;    dispatch to caller
p_five_char_setup endp			;;    terminate the procedure
name&_five_char proc near	      	;;    define actual procedure
	call	p_five_char_setup     	;;    call setup code
  endif 				;;  endif
else					;;else
name&_five_char proc near		;;    define actual procedure
	call	p_five_char_setup      	;;    call setup code
endif					;;endif ;setup code

;-----------------------------------------------------------------------;
; 5 character compilation logic
;-----------------------------------------------------------------------;

name&_five_char_outer_loop:
	xor	eax,eax
	xchg	dx,di			;;Index to next font scan in DI

	mov	ebx,[bp][0].cchar	;;BX = offset of bits
	or	al,[ebx][edi]
	mov	cl,[bp][0].wwidth	;;CL = width
	rol	eax,cl

	mov	ebx,[bp][-6].cchar	;;BX = offset of bits
	or	al,[ebx][edi]
	mov	cl,[bp][-6].wwidth	;;CL = width
	rol	eax,cl

	mov	ebx,[bp][-12].cchar	;;BX = offset of bits
	or	al,[ebx][edi]
	mov	cl,[bp][-12].wwidth	;;CL = width
	rol	eax,cl

	mov	ebx,[bp][-18].cchar	;;BX = offset of bits
	or	al,[ebx][edi]
	mov	cl,[bp][-18].wwidth	;;CL = width
	rol	eax,cl

	mov	ebx,[bp][-24].cchar	;;BX = offset of bits
	or	al,[ebx][edi]
	mov	cl,[bp][-24].pphase	;;CL = phase
name&_five_char_output:
	shr	eax,cl
	xchg	dx,di			;;DI = dest ptr
	output	<update>		;;Macro to do whatever for outputting
ifnb	<didec>
	dec	di			;;for 8 bit update
endif
	ret

name&_five_char  endp
	endm

page

;--------------------------------Macro----------------------------------;
; four_char
;
;	four_char is a macro for generating the code required to
;	process a destination character consisting of 4 source bytes
;
;	Usage:
;		four_char  name,output,update,setup,didec
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
;		didec	if not blank, it expands into dec di, which
;			is needed for 8 bit update code
;
;	The macro will generate the loop_logic macro which is the
;	macro used by all update macros for generating looping logic.
;	It is defined as a macro so that devices which are interleaved
;	can make multiple copies of it, possibly removing a jump.
;-----------------------------------------------------------------------;

four_char macro name,output,update,setup,didec
	local	genflag

	assumes ds,nothing		;;Set the assumptions now
	assumes es,nothing
	assumes ss,StrStuff

;-----------------------------------------------------------------------;
; four character looping logic macro
;
; the no-of-scans-to-converted has been cached in HIWORD of EDI
;-----------------------------------------------------------------------;

loop_logic	&Macro
	sub	edi,10000h		;;subtract from the HIWORD
	jnc	short name&_four_char_loop    ;;still more scans to convert
	pop	ebp			;;EBP = frame pointer
	sub	di,cell_adjust		;;adjust di for the next char
	&endm

;-----------------------------------------------------------------------;
; four character long looping logic macro
;
; the no-of-scans-to-converted has been cached in HIWORD of EDI
;-----------------------------------------------------------------------;

long_loop_logic	&Macro
	sub	edi,10000h		;;subtract 1 from the HIWORD
	jc	short name&_four_char_exit
	jmp	name&_four_char_loop
name&_four_char_exit:
	pop	ebp			;;EBP = frame pointer
	sub	di,cell_adjust
	&endm

;-----------------------------------------------------------------------;
; four character setup logic
;
; we will use the HIWORD of EDI to cache the number of scans to convert
; and the HIWORD of ECX to cache some of the phases
;-----------------------------------------------------------------------;

public	name&_four_char 		;;make public
genflag=0				;;assume out_of_line, already generated
ifidn <setup>,<inline>			;;if in_line
name&_four_char proc near		;;  define procedure
  genflag = 01b 			;;  show code must be generated
else					;;else
  ifndef p_four_char_setup		;;  if procedure not defined
    genflag = 11b			;;    show proc must also be generated
  else					;;  else
    ife $-p_four_char_setup		;;    a hack since it is defined on
      genflag = 11b			;;    pass 2 regardless of if we have
    endif				;;    generated it
  endif
endif
if genflag				;;if to generate setup
  if genflag and 10b			;;  if generating procedural version
public	p_four_char_setup 		;;    make public for debugging
p_four_char_setup proc near		;;    define start of setup proc
  endif 				;;  endif
	rol	edi,16			;; save DI in HIWORD
	mov	di,ax			;; load # scans to convert
	dec	di			;;we will decrement till cary in loop
	rol	edi,16			;; cache away # and restore DI
  if genflag and 10b			;;  if generating setup procedure
	xor	eax,eax
	pop	ax			;;    get return address
  endif 				;;  endif
	push	ebp			;;  save frame pointer
	mov	bp,dx			;;  --> buffer

if SELF_MODIFY				;;Oh no!! Self modifying code!! run!!
  if genflag and 10b			;;  if generating setup procedure
	mov	cl,[bp].wwidth1		;;
	mov	byte ptr fs:[eax+8],cl  ;;  
	mov	cl,[bp].wwidth2		;;
	mov	byte ptr fs:[eax+16],cl
	mov	cl,[bp].wwidth3		;;
	mov	byte ptr fs:[eax+24],cl
	mov	cl,[bp].pphase4		;;
	mov	byte ptr fs:[eax+32],cl
  else	
	mov	cl,[bp].wwidth1		;;
	mov	byte ptr fs:name&_four_char_width1+3,cl
	mov	cl,[bp].wwidth2		;;
	mov	byte ptr fs:name&_four_char_width2+3,cl
	mov	cl,[bp].wwidth3		;;
	mov	byte ptr fs:name&_four_char_width3+3,cl
	mov	cl,[bp].pphase4		;;
	mov	byte ptr fs:name&_four_char_phase4+3,cl
  endif
else
	mov	cl,[bp].wwidth3 	;;
	mov	ch,[bp].pphase4 	;;
	rol	ecx,16
	mov	cl,[bp].wwidth1		;;
	mov	ch,[bp].wwidth2
endif
	mov	ebx,[bp].cchar4
	mov	esi,[bp].cchar1		;;

	mov	edx,[bp].cchar3		;;
	mov	ebp,[bp].cchar2		;;
	sub	ebx,esi			;;  compute deltas
	sub	ebp,esi			;;
	sub	edx,esi			;;  for [esi][edx] addressing
	dec	ebx			;;  Allows lodsb at top of loop.
	dec	ebp
	dec	edx
  if genflag and 10b			;;  if generating setup procedure
	jmp	ax			;;    dispatch to caller
p_four_char_setup endp			;;    terminate the procedure
name&_four_char proc near		;;    define actual procedure
	call	p_four_char_setup 	;;    call setup code
  endif 				;;  endif
else					;;else
name&_four_char proc near		;;    define actual procedure
	call	p_four_char_setup 	;;    call setup code
endif					;;endif ;setup code

;-----------------------------------------------------------------------;
; four character compilation logic
;-----------------------------------------------------------------------;

name&_four_char_loop:
	xor	eax,eax
	lods	byte ptr ds:[esi]
ife SELF_MODIFY
	rol	eax,cl
	or	al,ds:[esi][ebp]
	xchg	cl,ch
	rol	eax,cl
	xchg	cl,ch
	rol	ecx,16			;; get the cached phases in CX
	or	al,ds:[esi][edx]
	rol	eax,cl
	or	al,[esi][ebx]
	xchg	cl,ch
	shr	eax,cl
	xchg	cl,ch
	rol	ecx,16			;; cache back the phases
else
  name&_four_char_width1:
  	rol	eax,4			;;count patched by setup code.
	or	al,ds:[esi][ebp]
  name&_four_char_width2:
  	rol	eax,4			;;count patched by setup code.
	or	al,ds:[esi][edx]
  name&_four_char_width3:
  	rol	eax,4			;;count patched by setup code.
	or	al,ds:[esi][ebx]
  name&_four_char_phase4:
  	shr	eax,4			;;count patched by setup code.
endif
	output	<update>		;;Macro to do whatever for outputting
ifnb	<didec>
	dec	di			;;extra adjust for 8 bit code
endif
	ret

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
;		three_char  name,output,update,setup,didec
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
;		didec	if not blank, it expands into dec di, which
;			is needed for 8 bit update code
;
;	The macro will generate the loop_logic macro which is the
;	macro used by all update macros for generating looping logic.
;	It is defined as a macro so that devices which are interleaved
;	can make multiple copies of it, possibly removing a jump.
;-----------------------------------------------------------------------;

three_char macro name,output,update,setup,didec
	local	genflag

	assumes ds,nothing		;;Set the assumptions now
	assumes es,nothing
	assumes ss,StrStuff

;-----------------------------------------------------------------------;
; three character looping logic macro
;
; the number of scans to convert is cached in the HIWORD of ECX
;-----------------------------------------------------------------------;

loop_logic	&macro
	sub	ecx,10000h		;;subtract 1 from HIWORD
	jnc	short name&_three_char_loop   ;;still more to convert
	pop	ebp			;;was pushed by setup macro
	sub	di,cell_adjust
	&endm

;-----------------------------------------------------------------------;
; three character long looping logic macro
;
; the number of scans to convert is cached in the HIWORD of ECX
;-----------------------------------------------------------------------;

long_loop_logic	&macro
	sub	ecx,10000h		;;subtract 1 from the HIWORD
	jc	short name&_three_char_exit
	jmp	name&_three_char_loop
name&_three_char_exit:
	pop	ebp
	sub	di,cell_adjust
	&endm

;-----------------------------------------------------------------------;
; three character setup logic
; the number of scans to convert will be cached into hiword of ECX
;-----------------------------------------------------------------------;

public	name&_three_char		;;make public
genflag=0				;;assume out_of_line, already generated
ifidn <setup>,<inline>			;;if in_line
name&_three_char proc near		;;  define procedure
  genflag = 01b 			;;  show code must be generated
else					;;else
  ifndef p_three_char_setup		;;  if procedure not defined
    genflag = 11b			;;    show proc must also be generated
  else					;;  else
    ife $-p_three_char_setup		;;    a hack since it is defined on
      genflag = 11b			;;    pass 2 regardless of if we have
    endif				;;    generated it
  endif
endif
if genflag				;;if to generate setup
  if genflag and 10b			;;  if generating procedural version
public	p_three_char_setup		;;    make public for debugging
p_three_char_setup proc near		;;    define start of setup proc
  endif 				;;  endif
	mov	cx,ax
	dec	cx			;;in loop we test for carry not 0
	rol	ecx,16			;; cache scan count into hiword ECX
  if genflag and 10b			;;  if generating setup procedure
	xor	eax,eax
	pop	ax			;;    get return address
  endif 				;;  endif
	push	ebp			;;loop_logic macro pops EBP on exit
	mov	bp,dx			;;  BP = buffer

	mov	dl,[bp].wwidth1 	;;
	mov	dh,[bp].wwidth2 	;;
	mov	ch,[bp].pphase3 	;;
if SELF_MODIFY
  if genflag and 10b			;;  if generating setup procedure
	mov	byte ptr fs:[eax+8],dl  ;;  Self-modify the rol code.
	mov	byte ptr fs:[eax+16],dh
	mov	byte ptr fs:[eax+24],ch
  else	
	mov	byte ptr fs:name&_three_char_width1+3,dl
	mov	byte ptr fs:name&_three_char_width2+3,dh
	mov	byte ptr fs:name&_three_char_phase3+3,ch
  endif
endif
	mov	esi,[bp].cchar1		;;
	mov	ebx,[bp].cchar2 	;;
	mov	ebp,[bp].cchar3 	;;

	sub	ebx,esi			;;
	sub	ebp,esi			;;
	dec	ebx
	dec	ebp
  if genflag and 10b			;;  if generating setup procedure
	jmp	ax			;;    dispatch to caller
p_three_char_setup endp			;;    terminate the procedure
name&_three_char proc near		;;    define actual procedure
	call	p_three_char_setup	;;    call setup code
  endif 				;;  endif
else					;;else
name&_three_char proc near		;;    define actual procedure
	call	p_three_char_setup	;;    call setup code
endif					;;endif ;setup code

;-----------------------------------------------------------------------;
; three character compilation logic
;-----------------------------------------------------------------------;

name&_three_char_loop:
	xor	eax,eax
	lods	byte ptr ds:[esi]
if SELF_MODIFY
  name&_three_char_width1:
   	rol	eax,4			;;count patched by setup code.
	or	al,[esi][ebx]
  name&_three_char_width2:
   	rol	eax,4			;;count patched by setup code.
	or	al,ds:[esi][ebp]
  name&_three_char_phase3:
   	shr	eax,4			;;count patched by setup code.
else
	mov	cl,dl
	rol	eax,cl
	or	al,[esi][ebx]
	mov	cl,dh
	rol	eax,cl
	or	al,ds:[esi][ebp]
	mov	cl,ch
	shr	eax,cl
endif
	output	<update>		;;Macro to do whatever for outputting
ifnb	<didec>
	dec	di			;;extra adjust for 8 bit gathering
endif
	ret

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
;		two_char  name,output,update,setup,didec
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
;		didec	if not blank, it expands into dec di, which
;			is needed for 8 bit update code
;
;	The macro will generate the loop_logic macro which is the
;	macro used by all update macros for generating looping logic.
;	It is defined as a macro so that devices which are interleaved
;	can make multiple copies of it, possibly removing a jump.
;-----------------------------------------------------------------------;

two_char macro name,output,update,setup,didec
	local	genflag

	assumes ds,nothing		;;Set the assumptions now
	assumes es,nothing
	assumes ss,StrStuff

;-----------------------------------------------------------------------;
; two character looping logic macro
;-----------------------------------------------------------------------;

loop_logic	&macro
	dec	dx
	jnz	short name&_two_char_loop
	sub	di,cell_adjust
	&endm

;-----------------------------------------------------------------------;
; two character long looping logic macro
;-----------------------------------------------------------------------;

long_loop_logic	&macro
	dec	dx
	jz	short name&_two_char_exit
	jmp	name&_two_char_loop
name&_two_char_exit:
	sub	di,cell_adjust
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
  ifndef p_two_char_setup 		;;  if procedure not defined
    genflag = 11b			;;    show proc must also be generated
  else					;;  else
    ife $-p_two_char_setup		;;    a hack since it is defined on
      genflag = 11b			;;    pass 2 regardless of if we have
    endif				;;    generated it
  endif
endif
if genflag				;;if to generate setup
  if genflag and 10b			;;  if generating procedural version
public	p_two_char_setup		;;    make public for debugging
p_two_char_setup proc near		;;    define start of setup proc
  endif 				;;  endif
	xchg	bp,dx			;;  BP = buffer, DX = font height
ife SELF_MODIFY	
	mov	cl,[bp].wwidth1 	;;
	mov	ch,[bp].pphase2 	;;
else
  if genflag and 10b			;;  if generating setup procedure
	xor	ebx,ebx
	mov	bx,sp
	mov	bx,ss:[ebx]		;;  Get return address (passively).
	mov	cl,[bp].wwidth1 	;;
	mov	byte ptr fs:[ebx+8],cl  ;;  Self-modify the rol code.
	mov	cl,[bp].pphase2 	;;
	mov	byte ptr fs:[ebx+16],cl
  else	
	mov	cl,[bp].wwidth1 	;;
	mov	byte ptr fs:name&_two_char_width1+3,cl
	mov	cl,[bp].pphase2
	mov	byte ptr fs:name&_two_char_phase2+3,cl
  endif
endif
	mov	esi,[bp].cchar1		;;
	mov	ebx,[bp].cchar2		;;
	mov	bp,dx			;;  restore frame pointer
	xchg	ax,dx			;;  set DX = font height

	sub	ebx,esi			;;  delta between the characters
	dec	ebx
  if genflag and 10b			;;  if generating setup procedure
	ret				;;    dispatch to caller
p_two_char_setup endp			;;    terminate the procedure
name&_two_char proc near		;;    define actual procedure
	call	p_two_char_setup	;;    call setup code
  endif 				;;  endif
else					;;else
name&_two_char proc near		;;    define actual procedure
	call	p_two_char_setup	;;    call setup code
endif					;;endif ;setup code

;-----------------------------------------------------------------------;
; two character compilation logic
;-----------------------------------------------------------------------;
name&_two_char_loop:
	xor	eax,eax
	lods	byte ptr ds:[esi]
Ife SELF_MODIFY	
	rol	eax,cl		      ; shift left by width
	xchg	cl,ch
	or	al,[ebx][esi]         ; or in next pattern.
	shr	eax,cl
	xchg	cl,ch
else
  name&_two_char_width1:
  	rol	eax,4			;;count patched by setup code.
	or	al,ds:[esi][ebx]
  name&_two_char_phase2:
  	shr	eax,4			;;count patched by setup code.
endif
	output	<update>		;;Macro to do whatever for outputting
ifnb	<didec>
	dec	di			;;extra update for 8 bit gathering code
endif
	ret

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
;		one_char  name,output,update,setup,didec
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
;		didec	if not blank, it expands into dec di, which
;			is needed for 8 bit update code
;
;	The macro will generate the loop_logic macro which is the
;	macro used by all update macros for generating looping logic.
;	It is defined as a macro so that devices which are interleaved
;	can make multiple copies of it, possibly removing a jump.
;-----------------------------------------------------------------------;

one_char macro name,output,update,setup,didec
	local	genflag

	assumes ds,nothing		;;Set the assumptions now
	assumes es,nothing
	assumes ss,StrStuff

;-----------------------------------------------------------------------;
; one character looping logic macro
;-----------------------------------------------------------------------;

loop_logic	&macro
	dec	dx
	jnz	short name&_one_char_loop
	sub	di,cell_adjust
	&endm

;-----------------------------------------------------------------------;
; one character long looping logic macro
;-----------------------------------------------------------------------;

long_loop_logic	&macro
	dec	dx
	jz	short name&_one_char_exit
	jmp	name&_one_char_loop
name&_one_char_exit:
	sub	di,cell_adjust
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
  ifndef p_one_char_setup 		;;  if procedure not defined
    genflag = 11b			;;    show proc must also be generated
  else					;;  else
    ife $-p_one_char_setup		;;    a hack since it is defined on
      genflag = 11b			;;    pass 2 regardless of if we have
    endif				;;    generated it
  endif
endif
if genflag				;;if to generate setup
  if genflag and 10b			;;  if generating procedural version
public	p_one_char_setup      		;;    make public for debugging
p_one_char_setup proc near		;;    define start of setup proc
  endif 				;;  endif
	xchg	dx,bp			;;  BP --> character buffer
	mov	esi,[bp].cchar1		;;  DS:SI = char1
	mov	cl,[bp].pphase1 	;;
	xchg	dx,bp			;;  BP --> frame
	xchg	ax,dx			;;  DX = clipped_font_height
  if genflag and 10b			;;  if generating setup procedure
	ret				;;    dispatch to caller
p_one_char_setup endp			;;    terminate the procedure
name&_one_char proc near		;;    define actual procedure
	call	p_one_char_setup	;;    call setup code
  endif 				;;  endif
else					;;else
name&_one_char proc near		;;    define actual procedure
	call	p_one_char_setup     	;;    call setup code
endif					;;endif ;setup code

;-----------------------------------------------------------------------;
; one character compilation logic
;-----------------------------------------------------------------------;
	xor	eax,eax
name&_one_char_loop:
	lods	byte ptr ds:[esi]
name&_1_char_output:
	shr	eax,cl
	output	<update>
ifnb	<didec>
	dec	di			;;extra update for 8 bit gathering code
endif
	ret

name&_one_char endp
	endm
page

;****************************************************************************;
; Same set of macros as above, but now to do boldening.		             ;
;****************************************************************************;

;--------------------------------Macro----------------------------------;
; bold_n_char
;
;	n_char is a macro for generating the code required to process
;	a destination character consisting of 5,6,7 or 8 source bytes
;
;	Usage:
;		n_char	name,output,update,setup,didec
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
;		didec	if not blank, it expands into dec di, which
;			is needed for 8 bit update code
;
;	The macro will generate the loop_logic macro which is the
;	macro used by all update macros for generating looping logic.
;	It is defined as a macro so that devices which are interleaved
;	can make multiple copies of it, possibly removing a jump.
;-----------------------------------------------------------------------;

bold_n_char	macro	name,output,update,setup,didec
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
	jnz	short name&_bold_n_char_outer_loop
	pop	bp			;;BP = frame pointer
	sub	di,cell_adjust
	&endm

;-----------------------------------------------------------------------;
; n character long looping logic macro
;-----------------------------------------------------------------------;

long_loop_logic	&macro
	inc	dx			;;Next scan of font
	dec	ss_height		;;Loop until all chars output
	jz	short name&_bold_n_char_exit
	jmp	name&_bold_n_char_outer_loop
name&_bold_n_char_exit:
	pop	bp			;;BP = frame pointer
	sub	di,cell_adjust
	&endm

;-----------------------------------------------------------------------;
; n character setup logic
;-----------------------------------------------------------------------;
public	name&_bold_n_char			;;make public
genflag=0				;;assume out_of_line, already generated
ifidn <setup>,<inline>			;;if in_line
name&_bold_n_char proc near			;;  define procedure
  genflag = 01b 			;;  show code must be generated
else					;;else
  ifndef p_bold_n_char_setup			;;  if procedure not defined
    genflag = 11b			;;    show proc must also be generated
  else					;;  else
    ife $-p_bold_n_char_setup			;;    a hack since it is defined on
      genflag = 11b			;;    pass 2 regardless of if we have
    endif				;;    generated it
  endif 				;;
endif					;;
if genflag				;;if to generate setup
  if genflag and 10b			;;  if generating procedural version
public	p_bold_n_char_setup			;;    make public for debugging
p_bold_n_char_setup proc near	    	;;    define start of setup proc
  endif 				;;  endif
	mov	ss_height,ax		;;  save # scans in character
  if genflag and 10b			;;  if generating setup procedure
	pop	ax			;;    get return address
  endif 				;;  endif
  	mov	ch,ss_boldflag		;;get the bold flag
	and	ch,1			;;isolate just bold flag
	shl	cl,1			;;make room for bold flag
	or	cl,ch			;;n can't be > 127, or in bold flag 
	mov	ss_num_chars,cl 	;;  save # of characters - 1
	push	bp			;;  save frame pointer
	mov	bp,dx			;;  set buffer pointer
	xor	dx,dx			;;  index into font scan
	movzx	edi,di
  if genflag and 10b			;;  if generating setup procedure
	jmp	ax			;;    dispatch to caller
p_bold_n_char_setup endp			;;    terminate the procedure
name&_bold_n_char proc near			;;    define actual procedure
	call	p_bold_n_char_setup		;;    call setup code
  endif 				;;  endif
else					;;else
name&_bold_n_char proc near			;;    define actual procedure
	call	p_bold_n_char_setup		;;    call setup code
endif					;;endif ;setup code

;-----------------------------------------------------------------------;
; n character compilation logic
;-----------------------------------------------------------------------;

name&_bold_n_char_outer_loop:
	xor	eax,eax
	xor	si,si			;;SI = char index
	mov	ch,ss_num_chars 	;;Get # of characters - 1
	shr	ch,1			;;throw bold flag out, get count
	xchg	dx,di			;;Index to next font scan in DI
name&_bold_n_char_inner_loop:
	mov	ebx,[bp][si].cchar	;;EBX = offset of bits
	or	al,[ebx][edi]
	mov	cl,[bp][si].wwidth	;;CL = width
	rol	eax,cl
	sub	si,6			;;--> next char
	dec	ch
	jnz	name&_bold_n_char_inner_loop
	mov	ebx,[bp][si].cchar	;;BX = offset of bits
	or	al,[ebx][edi]
	mov	ebx,eax			;;now bold it
	shr	ebx,1
	or	eax,ebx

name&_bold_n_char_output:
	mov	cl,[bp][si].pphase	;;CL = phase
	shr	eax,cl
	xchg	dx,di			;;DI = dest ptr
	output	<update>		;;Macro to do whatever for outputting
ifnb	<didec>
	dec	di			;;for 8 bit update
endif
	ret

name&_bold_n_char  endp
	endm


;--------------------------------Macro----------------------------------;
; bold_5_char
;
;	bold_5_char is a macro for generating the code required to process
;	a destination character consisting of 5,6,7 or 8 source bytes
;
;	Usage:
;		bold_5_char	name,output,update,setup,didec
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
;		didec	if not blank, it expands into dec di, which
;			is needed for 8 bit update code
;
;	The macro will generate the loop_logic macro which is the
;	macro used by all update macros for generating looping logic.
;	It is defined as a macro so that devices which are interleaved
;	can make multiple copies of it, possibly removing a jump.
;-----------------------------------------------------------------------;

bold_5_char	macro	name,output,update,setup,didec
	local	genflag

	assumes ds,nothing		;;Set the assumptions now
	assumes es,nothing
	assumes ss,StrStuff

;-----------------------------------------------------------------------;
; 5 character looping logic macro
;-----------------------------------------------------------------------;

loop_logic	&macro
	inc	dx			;;Next scan of font
	dec	si			;;Loop until all chars output
	jnz	short name&_bold_5_char_outer_loop
	pop	bp			;;BP = frame pointer
	sub	di,cell_adjust
	&endm

;-----------------------------------------------------------------------;
; 5 character long looping logic macro
;-----------------------------------------------------------------------;

long_loop_logic	&macro
	inc	dx			;;Next scan of font
	dec	si			;;Loop until all chars output
	jz	short name&_bold_5_char_exit
	jmp	name&_bold_5_char_outer_loop
name&_bold_5_char_exit:
	pop	bp			;;BP = frame pointer
	sub	di,cell_adjust
	&endm

;-----------------------------------------------------------------------;
; 5 character setup logic
;-----------------------------------------------------------------------;
public	name&_bold_5_char			;;make public
genflag=0				;;assume out_of_line, already generated
ifidn <setup>,<inline>			;;if in_line
name&_bold_5_char proc near			;;  define procedure
  genflag = 01b 			;;  show code must be generated
else					;;else
  ifndef p_bold_5_char_setup		;;  if procedure not defined
    genflag = 11b			;;    show proc must also be generated
  else					;;  else
    ife $-p_bold_5_char_setup		;;    a hack since it is defined on
      genflag = 11b			;;    pass 2 regardless of if we have
    endif				;;    generated it
  endif 				;;
endif					;;
if genflag				;;if to generate setup
  if genflag and 10b			;;  if generating procedural version
public	p_bold_5_char_setup  		;;    make public for debugging
p_bold_5_char_setup proc near  		;;    define start of setup proc
  endif 				;;  endif
	mov	si,ax			;;  save # scans in character
  if genflag and 10b			;;  if generating setup procedure
	pop	ax			;;    get return address
  endif 				;;  endif
	push	bp			;;  save frame pointer
	mov	bp,dx			;;  set buffer pointer
	xor	dx,dx			;;  index into font scan
	movzx	edi,di			;;clear out high word
  if genflag and 10b			;;  if generating setup procedure
	jmp	ax			;;    dispatch to caller
p_bold_5_char_setup endp			;;    terminate the procedure
name&_bold_5_char proc near	      	;;    define actual procedure
	call	p_bold_5_char_setup     	;;    call setup code
  endif 				;;  endif
else					;;else
name&_bold_5_char proc near		;;    define actual procedure
	call	p_bold_5_char_setup      	;;    call setup code
endif					;;endif ;setup code

;-----------------------------------------------------------------------;
; 5 character compilation logic
;-----------------------------------------------------------------------;

name&_bold_5_char_outer_loop:
	xor	eax,eax
	xchg	dx,di			;;Index to next font scan in DI

	mov	ebx,[bp][0].cchar	;;BX = offset of bits
	or	al,[ebx][edi]
	mov	cl,[bp][0].wwidth	;;CL = width
	rol	eax,cl

	mov	ebx,[bp][-6].cchar	;;BX = offset of bits
	or	al,[ebx][edi]
	mov	cl,[bp][-6].wwidth	;;CL = width
	rol	eax,cl

	mov	ebx,[bp][-12].cchar	;;BX = offset of bits
	or	al,[ebx][edi]
	mov	cl,[bp][-12].wwidth	;;CL = width
	rol	eax,cl

	mov	ebx,[bp][-18].cchar	;;BX = offset of bits
	or	al,[ebx][edi]
	mov	cl,[bp][-18].wwidth	;;CL = width
	rol	eax,cl

	mov	ebx,[bp][-24].cchar	;;BX = offset of bits
	or	al,[ebx][edi]
	mov	cl,[bp][-24].pphase	;;CL = phase
	mov	ebx,eax
	shr	ebx,1			;;bold it
	or	eax,ebx
name&_bold_5_char_output:
	shr	eax,cl
	xchg	dx,di			;;DI = dest ptr
	output	<update>		;;Macro to do whatever for outputting
ifnb	<didec>
	dec	di			;;for 8 bit update
endif
	ret

name&_bold_5_char  endp
	endm


page

;--------------------------------Macro----------------------------------;
; bold_4_char
;
;	bold_4_char is a macro for generating the code required to
;	process a destination character consisting of 4 source bytes
;
;	Usage:
;		bold_4_char  name,output,update,setup,didec
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
;		didec	if not blank, it expands into dec di, which
;			is needed for 8 bit update code
;
;	The macro will generate the loop_logic macro which is the
;	macro used by all update macros for generating looping logic.
;	It is defined as a macro so that devices which are interleaved
;	can make multiple copies of it, possibly removing a jump.
;-----------------------------------------------------------------------;

bold_4_char macro name,output,update,setup,didec
	local	genflag

	assumes ds,nothing		;;Set the assumptions now
	assumes es,nothing
	assumes ss,StrStuff

;-----------------------------------------------------------------------;
; four character looping logic macro
;
; the no-of-scans-to-converted has been cached in HIWORD of EDI
;-----------------------------------------------------------------------;

loop_logic	&Macro
	sub	edi,10000h		;;subtract from the HIWORD
	jnc	short name&_bold_4_char_loop    ;;still more scans to convert
	pop	ebp			;;EBP = frame pointer
	sub	di,cell_adjust		;;adjust di for the next char
	&endm

;-----------------------------------------------------------------------;
; four character long looping logic macro
;
; the no-of-scans-to-converted has been cached in HIWORD of EDI
;-----------------------------------------------------------------------;

long_loop_logic	&Macro
	sub	edi,10000h		;;subtract 1 from the HIWORD
	jc	short name&_bold_4_char_exit
	jmp	name&_bold_4_char_loop
name&_bold_4_char_exit:
	pop	ebp			;;EBP = frame pointer
	sub	di,cell_adjust
	&endm

;-----------------------------------------------------------------------;
; four character setup logic
;
; we will use the HIWORD of EDI to cache the number of scans to convert
; and the HIWORD of ECX to cache some of the phases
;-----------------------------------------------------------------------;

public	name&_bold_4_char 		;;make public
genflag=0				;;assume out_of_line, already generated
ifidn <setup>,<inline>			;;if in_line
name&_bold_4_char proc near		;;  define procedure
  genflag = 01b 			;;  show code must be generated
else					;;else
  ifndef p_bold_4_char_setup		;;  if procedure not defined
    genflag = 11b			;;    show proc must also be generated
  else					;;  else
    ife $-p_bold_4_char_setup		;;    a hack since it is defined on
      genflag = 11b			;;    pass 2 regardless of if we have
    endif				;;    generated it
  endif
endif
if genflag				;;if to generate setup
  if genflag and 10b			;;  if generating procedural version
public	p_bold_4_char_setup 		;;    make public for debugging
p_bold_4_char_setup proc near		;;    define start of setup proc
  endif 				;;  endif
	rol	edi,16			;; save DI in HIWORD
	mov	di,ax			;; load # scans to convert
	dec	di			;;we will decrement till cary in loop
	rol	edi,16			;; cache away # and restore DI
  if genflag and 10b			;;  if generating setup procedure
	pop	ax			;;    get return address
  endif 				;;  endif
	push	ebp			;;  save frame pointer
	mov	bp,dx			;;  --> buffer
	mov	cl,[bp].wwidth3 	;;
	mov	ch,[bp].pphase4 	;;
	rol	ecx,16
	mov	cl,[bp].wwidth1		;;
	mov	ch,[bp].wwidth2
	xor	esi,esi			;; will use [esi][edx] address mode
	mov	esi,[bp].cchar1		;;  1st character so we can lodsb
	mov	ebx,[bp].cchar4		;;
	xor	edx,edx			;; will use [edx] address mode
	mov	edx,[bp].cchar3 	;;
	mov	ebp,[bp].cchar2 	;;
	sub	ebx,esi			;;  compute deltas
	sub	ebp,esi			;;
	sub	edx,esi			;;  for [esi][edx] addressing
	dec	ebx			;; Allows lodsb at top of loop.
	dec	ebp
	dec	edx
  if genflag and 10b			;;  if generating setup procedure
	jmp	ax			;;    dispatch to caller
p_bold_4_char_setup endp			;;    terminate the procedure
name&_bold_4_char proc near		;;    define actual procedure
	call	p_bold_4_char_setup 	;;    call setup code
  endif 				;;  endif
else					;;else
name&_bold_4_char proc near		;;    define actual procedure
	call	p_bold_4_char_setup 	;;    call setup code
endif					;;endif ;setup code

;-----------------------------------------------------------------------;
; four character compilation logic
;-----------------------------------------------------------------------;

name&_bold_4_char_loop:
	xor	eax,eax
	lods	byte ptr ds:[esi]	;; char1
	rol	eax,cl
	or	al,ds:[esi][ebp]	;; char2
	xchg	cl,ch
	rol	eax,cl
	xchg	cl,ch
	rol	ecx,16			;; get the cached phases in CX
	or	al,ds:[esi][edx]	;; char3
	rol	eax,cl
	or	al,[esi][ebx]		;; char4
	push	ebx    			;;bold it
	mov	ebx,eax
	shr	ebx,1
	or	eax,ebx
	pop	ebx 
	xchg	cl,ch
	shr	eax,cl
	xchg	cl,ch
	rol	ecx,16			;; cache back the phases
	output	<update>		;;Macro to do whatever for outputting
ifnb	<didec>
	dec	di			;;extra adjust for 8 bit code
endif
	ret

name&_bold_4_char endp
	endm
page

;--------------------------------Macro----------------------------------;
; bold_3_char
;
;	bold_3_char is a macro for generating the code required to
;	process a destination character consisting of 3 source bytes
;
;	Usage:
;		bold_3_char  name,output,update,setup,didec
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
;		didec	if not blank, it expands into dec di, which
;			is needed for 8 bit update code
;
;	The macro will generate the loop_logic macro which is the
;	macro used by all update macros for generating looping logic.
;	It is defined as a macro so that devices which are interleaved
;	can make multiple copies of it, possibly removing a jump.
;-----------------------------------------------------------------------;

bold_3_char macro name,output,update,setup,didec
	local	genflag

	assumes ds,nothing		;;Set the assumptions now
	assumes es,nothing
	assumes ss,StrStuff

;-----------------------------------------------------------------------;
; three character looping logic macro
;
; the number of scans to convert is cached in the HIWORD of ECX
;-----------------------------------------------------------------------;

loop_logic	&macro
	sub	ecx,10000h		;;subtract 1 from HIWORD
	jnc	short name&_bold_3_char_loop   ;;still more to convert
	pop	ebp			;;was pushed by setup macro
	sub	di,cell_adjust
	&endm

;-----------------------------------------------------------------------;
; three character long looping logic macro
;
; the number of scans to convert is cached in the HIWORD of ECX
;-----------------------------------------------------------------------;

long_loop_logic	&macro
	sub	ecx,10000h		;;subtract 1 from the HIWORD
	jc	short name&_bold_3_char_exit
	jmp	name&_bold_3_char_loop
name&_bold_3_char_exit:
	pop	ebp
	sub	di,cell_adjust
	&endm

;-----------------------------------------------------------------------;
; three character setup logic
; the number of scans to convert will be cached into hiword of ECX
;-----------------------------------------------------------------------;

public	name&_bold_3_char		;;make public
genflag=0				;;assume out_of_line, already generated
ifidn <setup>,<inline>			;;if in_line
name&_bold_3_char proc near		;;  define procedure
  genflag = 01b 			;;  show code must be generated
else					;;else
  ifndef p_bold_3_char_setup		;;  if procedure not defined
    genflag = 11b			;;    show proc must also be generated
  else					;;  else
    ife $-p_bold_3_char_setup		;;    a hack since it is defined on
      genflag = 11b			;;    pass 2 regardless of if we have
    endif				;;    generated it
  endif
endif
if genflag				;;if to generate setup
  if genflag and 10b			;;  if generating procedural version
public	p_bold_3_char_setup		;;    make public for debugging
p_bold_3_char_setup proc near		;;    define start of setup proc
  endif 				;;  endif
	mov	cx,ax
	dec	cx			;;in loop we test for carry not 0
	rol	ecx,16			;; cache scan count into hiword ECX
  if genflag and 10b			;;  if generating setup procedure
	pop	ax			;;    get return address
  endif 				;;  endif
	push	ebp			;;loop_logic macro pops EBP on exit
	mov	bp,dx			;;  BP = buffer
	mov	dl,[bp].wwidth1 	;;
	mov	dh,[bp].wwidth2 	;;
	mov	ch,[bp].pphase3 	;;
	mov	esi,[bp].cchar1		;;
	mov	ebx,[bp].cchar2		;;
	mov	ebp,[bp].cchar3		;;
	sub	ebx,esi			;;
	sub	ebp,esi			;;
	dec	ebx			;; Allow lodsb at top of loop.
	dec	ebp
  if genflag and 10b			;;  if generating setup procedure
	jmp	ax			;;    dispatch to caller
p_bold_3_char_setup endp			;;    terminate the procedure
name&_bold_3_char proc near		;;    define actual procedure
	call	p_bold_3_char_setup	;;    call setup code
  endif 				;;  endif
else					;;else
name&_bold_3_char proc near		;;    define actual procedure
	call	p_bold_3_char_setup	;;    call setup code
endif					;;endif ;setup code

;-----------------------------------------------------------------------;
; three character compilation logic
;-----------------------------------------------------------------------;

name&_bold_3_char_loop:
	xor	eax,eax
	mov	cl,dl
	lods	byte ptr ds:[esi]	;;char1
	rol	eax,cl
	or	al,[esi][ebx]		;;char2
	mov	cl,dh
	rol	eax,cl
	or	al,ds:[esi][ebp]
	push	ebx
	mov	ebx,eax			;;bold it
	shr	ebx,1
	or	eax,ebx
	pop	ebx
name&_3_char_output:
	mov	cl,ch
	shr	eax,cl
	output	<update>		;;Macro to do whatever for outputting
ifnb	<didec>
	dec	di			;;extra adjust for 8 bit gathering
endif
	ret

name&_bold_3_char endp
	endm
page

;--------------------------------Macro----------------------------------;
; bold_2_char
;
;	bold_2_char is a macro for generating the code required to
;	process a destination character consisting of 2 source bytes
;
;	Usage:
;		bold_2_char  name,output,update,setup,didec
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
;		didec	if not blank, it expands into dec di, which
;			is needed for 8 bit update code
;
;	The macro will generate the loop_logic macro which is the
;	macro used by all update macros for generating looping logic.
;	It is defined as a macro so that devices which are interleaved
;	can make multiple copies of it, possibly removing a jump.
;-----------------------------------------------------------------------;

bold_2_char macro name,output,update,setup,didec
	local	genflag

	assumes ds,nothing		;;Set the assumptions now
	assumes es,nothing
	assumes ss,StrStuff

;-----------------------------------------------------------------------;
; two character looping logic macro
;-----------------------------------------------------------------------;

loop_logic	&macro
	dec	dx
	jnz	short name&_bold_2_char_loop
	sub	di,cell_adjust
	&endm

;-----------------------------------------------------------------------;
; two character long looping logic macro
;-----------------------------------------------------------------------;

long_loop_logic	&macro
	dec	dx
	jz	short name&_bold_2_char_exit
	jmp	name&_bold_2_char_loop
name&_bold_2_char_exit:
	sub	di,cell_adjust
	&endm

;-----------------------------------------------------------------------;
; two character setup logic
;-----------------------------------------------------------------------;

public	name&_bold_2_char			;;make public
genflag=0				;;assume out_of_line, already generated
ifidn <setup>,<inline>			;;if in_line
name&_bold_2_char proc near		;;  define procedure
  genflag = 01b 			;;  show code must be generated
else					;;else
  ifndef p_bold_2_char_setup 		;;  if procedure not defined
    genflag = 11b			;;    show proc must also be generated
  else					;;  else
    ife $-p_bold_2_char_setup		;;    a hack since it is defined on
      genflag = 11b			;;    pass 2 regardless of if we have
    endif				;;    generated it
  endif
endif
if genflag				;;if to generate setup
  if genflag and 10b			;;  if generating procedural version
public	p_bold_2_char_setup		;;    make public for debugging
p_bold_2_char_setup proc near		;;    define start of setup proc
  endif 				;;  endif
	xchg	bp,dx			;;  BP = buffer, DX = font height
	mov	cl,[bp].wwidth1 	;;
	mov	ch,[bp].pphase2 	;;
	mov	ebx,[bp].cchar2 	;;
	mov	esi,[bp].cchar1		;;
	mov	bp,dx			;;  restore frame pointer
	xchg	ax,dx			;;  set DX = font height
	sub	ebx,esi			;;  delta between the characters
	dec	ebx
  if genflag and 10b			;;  if generating setup procedure
	ret				;;    dispatch to caller
p_bold_2_char_setup endp			;;    terminate the procedure
name&_bold_2_char proc near		;;    define actual procedure
	call	p_bold_2_char_setup	;;    call setup code
  endif 				;;  endif
else					;;else
name&_bold_2_char proc near		;;    define actual procedure
	call	p_bold_2_char_setup	;;    call setup code
endif					;;endif ;setup code

;-----------------------------------------------------------------------;
; two character compilation logic
;-----------------------------------------------------------------------;

name&_bold_2_char_loop:
	xor	eax,eax
	lods	byte ptr ds:[esi]
	rol	eax,cl
	xchg	cl,ch
	or	al,[esi][ebx]
	push	ebx	  		;;bold it
	mov	ebx,eax
	shr	ebx,1
	or	eax,ebx
	pop	ebx
name&_2_char_output:
	shr	eax,cl
	xchg	cl,ch
	output	<update>		;;Macro to do whatever for outputting
ifnb	<didec>
	dec	di			;;extra update for 8 bit gathering code
endif
	ret

name&_bold_2_char endp
	endm
page

;--------------------------------Macro----------------------------------;
; bold_1_char
;
;	bold_1_char is a macro for generating the code required to
;	process a destination character consisting of 1 source byte
;
;	Usage:
;		bold_1_char  name,output,update,setup,didec
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
;		didec	if not blank, it expands into dec di, which
;			is needed for 8 bit update code
;
;	The macro will generate the loop_logic macro which is the
;	macro used by all update macros for generating looping logic.
;	It is defined as a macro so that devices which are interleaved
;	can make multiple copies of it, possibly removing a jump.
;-----------------------------------------------------------------------;

bold_1_char macro name,output,update,setup,didec
	local	genflag

	assumes ds,nothing		;;Set the assumptions now
	assumes es,nothing
	assumes ss,StrStuff

;-----------------------------------------------------------------------;
; one character looping logic macro
;-----------------------------------------------------------------------;

loop_logic	&macro
	dec	dx
	jnz	short name&_bold_1_char_loop
	sub	di,cell_adjust
	&endm

;-----------------------------------------------------------------------;
; one character long looping logic macro
;-----------------------------------------------------------------------;

long_loop_logic	&macro
	dec	dx
	jz	short name&_bold_1_char_exit
	jmp	name&_bold_1_char_loop
name&_bold_1_char_exit:
	sub	di,cell_adjust
	&endm

;-----------------------------------------------------------------------;
; one character setup logic
;-----------------------------------------------------------------------;

public	name&_bold_1_char			;;make public
genflag=0				;;assume out_of_line, already generated
ifidn <setup>,<inline>			;;if in_line
name&_bold_1_char proc near		;;  define procedure
  genflag = 01b 			;;  show code must be generated
else					;;else
  ifndef p_bold_1_char_setup 		;;  if procedure not defined
    genflag = 11b			;;    show proc must also be generated
  else					;;  else
    ife $-p_bold_1_char_setup		;;    a hack since it is defined on
      genflag = 11b			;;    pass 2 regardless of if we have
    endif				;;    generated it
  endif
endif
if genflag				;;if to generate setup
  if genflag and 10b			;;  if generating procedural version
public	p_bold_1_char_setup      		;;    make public for debugging
p_bold_1_char_setup proc near		;;    define start of setup proc
  endif 				;;  endif
	xchg	dx,bp			;;  BP --> character buffer
	mov	esi,[bp].cchar1		;;  DS:SI = char1
	mov	cl,[bp].pphase1 	;;
	xchg	dx,bp			;;  BP --> frame
	xchg	ax,dx			;;  DX = clipped_font_height
  if genflag and 10b			;;  if generating setup procedure
	ret				;;    dispatch to caller
p_bold_1_char_setup endp			;;    terminate the procedure
name&_bold_1_char proc near		;;    define actual procedure
	call	p_bold_1_char_setup	;;    call setup code
  endif 				;;  endif
else					;;else
name&_bold_1_char proc near		;;    define actual procedure
	call	p_bold_1_char_setup     	;;    call setup code
endif					;;endif ;setup code

;-----------------------------------------------------------------------;
; one character compilation logic
;-----------------------------------------------------------------------;
	xor	eax,eax
	xor	ebx,ebx
name&_bold_1_char_loop:
	lods	byte ptr ds:[esi]	;;char1
	mov	ebx,eax
	shr	bl,1
	or	al,bl
name&_1_char_output:
	shr	al,cl
	output	<update>
ifnb	<didec>
	dec	di			;;extra update for 8 bit gathering code
endif
	ret


name&_bold_1_char endp
	endm
page

;****************************************************************************;
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
	xchg	al,es:[di+1]
	mov	al,ah
	out	dx,al
	xchg	ah,es:[di]
	pop	dx
@@:
DRAW_ADJUST = 0					;DI has not been incremented
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
	xchg	ah,al
	mov	es:[di],ax
DRAW_ADJUST	= 0			;;STOSW is not used.
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
	or	es:[di],ah
	or	es:[di+1],al
DRAW_ADJUST	= 0			;;DI has not been inc'ed
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
	not	ax
	and	es:[di],ah
	and	es:[di+1],al
DRAW_ADJUST	= 0			;;DI has not been inced
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
	not	ax
	xchg	ah,al
	mov	es:[di],ax
DRAW_ADJUST	= 0			;;STOSW is not used
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
	xchg	ah,al
	mov	es:[di],ax	
DRAW_ADJUST	= 0			;;STOSW is used
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
	mov	word ptr es:[di],0
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
	mov	word ptr es:[di],0FFFFh
DRAW_ADJUST	= 0			;;STOSB isn't used
	update
	endm
page

;--------------------------------Macro----------------------------------;
; mapc
;
;	mapc is a macro for generating the character drawing
;	logic onto plane memory basically to synthesize italic font for
;       the particular string.
;
;	Usage:
;		mapc update
;	Where
;		update	the macro which will generate the required
;			destination update logic.
;-----------------------------------------------------------------------;

mapc	macro	update
	and	al,ss_clip_mask			;;combine with mask
	stosb					;;write it to map
DRAW_ADJUST = 1					;;stosb used
	update					;;update DI
	endm

;--------------------------------Macro----------------------------------;
; mapnc
;
;	mapnc is a macro for generating the character drawing
;	logic onto plane memory basically to synthesize italic font for
;       the particular string. This is for nonclipped bytes
;
;	Usage:
;		mapnc update
;	Where
;		update	the macro which will generate the required
;			destination update logic.
;-----------------------------------------------------------------------;

mapnc	macro	update
	xchg	al,ah			;;first byte was in ah
	stosw				;;write synthesized byte
DRAW_ADJUST = 2				;;stosw used
	update				;;update destination pointer
	endm

;-----------------------------------------------------------------------;
createSeg _PROTECT,pCode,word,public,CODE
sBegin	pCode
assumes cs,pCode

	.386p

;-----------------------------------------------------------------------;
;
;	The following tables are used to dispatch the various
;	combinations of drawing required for foreground/background,
;	opaque/transparent, device/bitmap, clipped/non-clipped text
;
;-----------------------------------------------------------------------;


special_case_clip_tables	label	word
	dw	pCodeOFFSET gen_cl
	dw	pCodeOFFSET gen_cl
	dw	pCodeOFFSET gen_cl
	dw	pCodeOFFSET gen_cl
	dw	pCodeOFFSET gen_cl
	dw	pCodeOFFSET gen_cl
	dw	pCodeOFFSET gen_cl
	dw	pCodeOFFSET gen_cl
if SPECIAL_CASE_DEV_TC eq 1
	dw	pCodeOFFSET dev_tc
	dw	pCodeOFFSET dev_tc
	dw	pCodeOFFSET dev_tc
	dw	pCodeOFFSET dev_tc
else
	dw	pCodeOFFSET gen_cl
	dw	pCodeOFFSET gen_cl
	dw	pCodeOFFSET gen_cl
	dw	pCodeOFFSET gen_cl
endif
	dw	pCodeOFFSET dev_oc
	dw	pCodeOFFSET dev_oc
	dw	pCodeOFFSET dev_oc
	dw	pCodeOFFSET dev_oc


bold_special_case_clip_tables	label	word
	dw	pCodeOFFSET bold_gen_cl
	dw	pCodeOFFSET bold_gen_cl
	dw	pCodeOFFSET bold_gen_cl
	dw	pCodeOFFSET bold_gen_cl
	dw	pCodeOFFSET bold_gen_cl
	dw	pCodeOFFSET bold_gen_cl
	dw	pCodeOFFSET bold_gen_cl
	dw	pCodeOFFSET bold_gen_cl
	dw	pCodeOFFSET bold_gen_cl
	dw	pCodeOFFSET bold_gen_cl
	dw	pCodeOFFSET bold_gen_cl
	dw	pCodeOFFSET bold_gen_cl
	dw	pCodeOFFSET dev_oc
	dw	pCodeOFFSET dev_oc
	dw	pCodeOFFSET dev_oc
	dw	pCodeOFFSET dev_oc



special_case_non_clip_tables	label	word
if SPECIAL_CASE_BM_TBNC eq 1
	dw	pCodeOFFSET bm_tbnc
	dw	pCodeOFFSET bm_tbnc
else
	dw	pCodeOFFSET gen_nc
	dw	pCodeOFFSET gen_nc
endif
if SPECIAL_CASE_BM_TWNC eq 1
	dw	pCodeOFFSET bm_twnc
	dw	pCodeOFFSET bm_twnc
else
	dw	pCodeOFFSET gen_nc
	dw	pCodeOFFSET gen_nc
endif
if SPECIAL_CASE_BM_OBBNC eq 1
	dw	pCodeOFFSET bm_obbnc
else
	dw	pCodeOFFSET gen_nc
endif
if SPECIAL_CASE_BM_OBWNC eq 1
	dw	pCodeOFFSET bm_obwnc
else
	dw	pCodeOFFSET gen_nc
endif
if SPECIAL_CASE_BM_OWBNC eq 1
	dw	pCodeOFFSET bm_owbnc
else
	dw	pCodeOFFSET gen_nc
endif
if SPECIAL_CASE_BM_OWWNC eq 1
	dw	pCodeOFFSET bm_owwnc
else
	dw	pCodeOFFSET gen_nc
endif
if SPECIAL_CASE_DEV_TNC eq 1
	dw	pCodeOFFSET dev_tnc
	dw	pCodeOFFSET dev_tnc
	dw	pCodeOFFSET dev_tnc
	dw	pCodeOFFSET dev_tnc
else
	dw	pCodeOFFSET gen_nc
	dw	pCodeOFFSET gen_nc
	dw	pCodeOFFSET gen_nc
	dw	pCodeOFFSET gen_nc
endif
if SPECIAL_CASE_DEV_ONC eq 1
	dw	pCodeOFFSET dev_onc
	dw	pCodeOFFSET dev_onc
	dw	pCodeOFFSET dev_onc
	dw	pCodeOFFSET dev_onc
else
	dw	pCodeOFFSET gen_nc
	dw	pCodeOFFSET gen_nc
	dw	pCodeOFFSET gen_nc
	dw	pCodeOFFSET gen_nc
endif

bold_special_case_non_clip_tables	label	word
	dw	pCodeOFFSET bold_gen_nc
	dw	pCodeOFFSET bold_gen_nc
	dw	pCodeOFFSET bold_gen_nc
	dw	pCodeOFFSET bold_gen_nc
	dw	pCodeOFFSET bold_gen_nc
	dw	pCodeOFFSET bold_gen_nc
	dw	pCodeOFFSET bold_gen_nc
	dw	pCodeOFFSET bold_gen_nc
	dw	pCodeOFFSET bold_gen_nc
	dw	pCodeOFFSET bold_gen_nc
	dw	pCodeOFFSET bold_gen_nc
	dw	pCodeOFFSET bold_gen_nc
	dw	pCodeOFFSET bold_gen_nc
	dw	pCodeOFFSET bold_gen_nc
	dw	pCodeOFFSET bold_gen_nc
	dw	pCodeOFFSET bold_gen_nc


gen_nc	label	word
	dw	pCodeOFFSET p_gen_nc_one_char
	dw	pCodeOFFSET p_gen_nc_two_char
	dw	pCodeOFFSET p_gen_nc_three_char
	dw	pCodeOFFSET p_gen_nc_four_char
	dw	pCodeOFFSET p_gen_nc_five_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char

bold_gen_nc	label	word
	dw	pCodeOFFSET p_b_gen_nc_bold_1_char
	dw	pCodeOFFSET p_b_gen_nc_bold_2_char
	dw	pCodeOFFSET p_b_gen_nc_bold_3_char
	dw	pCodeOFFSET p_b_gen_nc_bold_4_char
	dw	pCodeOFFSET p_b_gen_nc_bold_5_char
	dw	pCodeOFFSET p_b_gen_nc_bold_n_char
	dw	pCodeOFFSET p_b_gen_nc_bold_n_char
	dw	pCodeOFFSET p_b_gen_nc_bold_n_char
	dw	pCodeOFFSET p_b_gen_nc_bold_n_char
	dw	pCodeOFFSET p_b_gen_nc_bold_n_char
	dw	pCodeOFFSET p_b_gen_nc_bold_n_char
	dw	pCodeOFFSET p_b_gen_nc_bold_n_char
	dw	pCodeOFFSET p_b_gen_nc_bold_n_char
	dw	pCodeOFFSET p_b_gen_nc_bold_n_char
	dw	pCodeOFFSET p_b_gen_nc_bold_n_char
	dw	pCodeOFFSET p_b_gen_nc_bold_n_char


gen_cl	label	word
	dw	pCodeOFFSET p_gen_cl_one_char
	dw	pCodeOFFSET p_gen_cl_two_char
	dw	pCodeOFFSET p_gen_cl_three_char
	dw	pCodeOFFSET p_gen_cl_four_char
	dw	pCodeOFFSET p_gen_cl_five_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char

bold_gen_cl	label	word

	dw	pCodeOFFSET p_b_gen_cl_bold_1_char
	dw	pCodeOFFSET p_b_gen_cl_bold_2_char
	dw	pCodeOFFSET p_b_gen_cl_bold_3_char
	dw	pCodeOFFSET p_b_gen_cl_bold_4_char
	dw	pCodeOFFSET p_b_gen_cl_bold_5_char
	dw	pCodeOFFSET p_b_gen_cl_bold_n_char
	dw	pCodeOFFSET p_b_gen_cl_bold_n_char
	dw	pCodeOFFSET p_b_gen_cl_bold_n_char
	dw	pCodeOFFSET p_b_gen_cl_bold_n_char
	dw	pCodeOFFSET p_b_gen_cl_bold_n_char
	dw	pCodeOFFSET p_b_gen_cl_bold_n_char
	dw	pCodeOFFSET p_b_gen_cl_bold_n_char
	dw	pCodeOFFSET p_b_gen_cl_bold_n_char
	dw	pCodeOFFSET p_b_gen_cl_bold_n_char
	dw	pCodeOFFSET p_b_gen_cl_bold_n_char
	dw	pCodeOFFSET p_b_gen_cl_bold_n_char

;	This is a difficult enough mode of output that it is always
;	"special-cased".  It actually goes through the generic handlers,
;	but the entries in the first table below set up two passes
;	through the functions in the second table.

dev_oc label word
	dw	pCodeOFFSET p_ega_oc_outer_loop_prop
	dw	pCodeOFFSET p_ega_oc_outer_loop_prop
	dw	pCodeOFFSET p_ega_oc_outer_loop_prop
	dw	pCodeOFFSET p_ega_oc_outer_loop_prop
	dw	pCodeOFFSET p_ega_oc_outer_loop_prop
	dw	pCodeOFFSET p_ega_oc_outer_loop_prop
	dw	pCodeOFFSET p_ega_oc_outer_loop_prop
	dw	pCodeOFFSET p_ega_oc_outer_loop_prop
	dw	pCodeOFFSET p_ega_oc_outer_loop_prop
	dw	pCodeOFFSET p_ega_oc_outer_loop_prop
	dw	pCodeOFFSET p_ega_oc_outer_loop_prop
	dw	pCodeOFFSET p_ega_oc_outer_loop_prop
	dw	pCodeOFFSET p_ega_oc_outer_loop_prop
	dw	pCodeOFFSET p_ega_oc_outer_loop_prop
	dw	pCodeOFFSET p_ega_oc_outer_loop_prop
	dw	pCodeOFFSET p_ega_oc_outer_loop_prop

	public	p_ega_oc
p_ega_oc label word
	dw	pCodeOFFSET p_gen_cl_one_char
	dw	pCodeOFFSET p_gen_cl_two_char
	dw	pCodeOFFSET p_gen_cl_three_char
	dw	pCodeOFFSET p_gen_cl_four_char
	dw	pCodeOFFSET p_gen_cl_five_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char

	public	p_ega_oc
p_bold_ega_oc label word
	dw	pCodeOFFSET p_b_gen_cl_bold_1_char
	dw	pCodeOFFSET p_b_gen_cl_bold_2_char
	dw	pCodeOFFSET p_b_gen_cl_bold_3_char
	dw	pCodeOFFSET p_b_gen_cl_bold_4_char
	dw	pCodeOFFSET p_b_gen_cl_bold_5_char
	dw	pCodeOFFSET p_b_gen_cl_bold_n_char
	dw	pCodeOFFSET p_b_gen_cl_bold_n_char
	dw	pCodeOFFSET p_b_gen_cl_bold_n_char
	dw	pCodeOFFSET p_b_gen_cl_bold_n_char
	dw	pCodeOFFSET p_b_gen_cl_bold_n_char
	dw	pCodeOFFSET p_b_gen_cl_bold_n_char
	dw	pCodeOFFSET p_b_gen_cl_bold_n_char
	dw	pCodeOFFSET p_b_gen_cl_bold_n_char
	dw	pCodeOFFSET p_b_gen_cl_bold_n_char
	dw	pCodeOFFSET p_b_gen_cl_bold_n_char
	dw	pCodeOFFSET p_b_gen_cl_bold_n_char


if SPECIAL_CASE_BM_OBWNC eq 1
bm_obwnc label word
	dw	pCodeOFFSET p_bm_obwnc_one_char
	dw	pCodeOFFSET p_bm_obwnc_two_char
	dw	pCodeOFFSET p_bm_obwnc_three_char
	dw	pCodeOFFSET p_bm_obwnc_four_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
endif

if SPECIAL_CASE_BM_OWBNC eq 1
bm_owbnc label word
	dw	pCodeOFFSET p_bm_owbnc_one_char
	dw	pCodeOFFSET p_bm_owbnc_two_char
	dw	pCodeOFFSET p_bm_owbnc_three_char
	dw	pCodeOFFSET p_bm_owbnc_four_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
endif

if SPECIAL_CASE_BM_TBNC eq 1
bm_tbnc label word
	dw	pCodeOFFSET p_bm_tbnc_one_char
	dw	pCodeOFFSET p_bm_tbnc_two_char
	dw	pCodeOFFSET p_bm_tbnc_three_char
	dw	pCodeOFFSET p_bm_tbnc_four_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
endif

if SPECIAL_CASE_BM_TWNC eq 1
bm_twnc label word
	dw	pCodeOFFSET p_bm_twnc_one_char
	dw	pCodeOFFSET p_bm_twnc_two_char
	dw	pCodeOFFSET p_bm_twnc_three_char
	dw	pCodeOFFSET p_bm_twnc_four_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
endif

if SPECIAL_CASE_DEV_ONC eq 1	;Device, opaque, non-clipped
dev_onc label word
	dw	pCodeOFFSET p_dev_onc_one_char
	dw	pCodeOFFSET p_dev_onc_two_char
	dw	pCodeOFFSET p_dev_onc_three_char
	dw	pCodeOFFSET p_dev_onc_four_char
	dw	pCodeOFFSET p_dev_onc_five_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
endif

if SPECIAL_CASE_DEV_TNC eq 1	;Device, transparent, non-clipped
dev_tnc label	word
	dw	pCodeOFFSET p_dev_tnc_one_char
	dw	pCodeOFFSET p_dev_tnc_two_char
	dw	pCodeOFFSET p_dev_tnc_three_char
	dw	pCodeOFFSET p_dev_tnc_four_char
	dw	pCodeOFFSET p_gen_nc_five_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
	dw	pCodeOFFSET p_gen_nc_n_char
endif

if SPECIAL_CASE_DEV_TC eq 1	;Device, transparent, clipped
dev_tc label	word
	dw	pCodeOFFSET p_dev_tc_one_char
	dw	pCodeOFFSET p_dev_tc_two_char
	dw	pCodeOFFSET p_dev_tc_three_char
	dw	pCodeOFFSET p_dev_tc_four_char
	dw	pCodeOFFSET p_gen_cl_five_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char
	dw	pCodeOFFSET p_gen_cl_n_char
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

	dw	pCodeOFFSET  p_bm_trans_color
	dw	pCodeOFFSET  p_bm_trans_color
	dw	pCodeOFFSET  p_bm_trans_color
	dw	pCodeOFFSET  p_bm_trans_color
	dw	pCodeOFFSET  p_bm_opaque_color
	dw	pCodeOFFSET  p_bm_opaque_color
	dw	pCodeOFFSET  p_bm_opaque_color
	dw	pCodeOFFSET  p_bm_opaque_color
	dw	pCodeOFFSET p_ega_trans_clip
	dw	pCodeOFFSET p_ega_trans_clip
	dw	pCodeOFFSET p_ega_trans_clip
	dw	pCodeOFFSET p_ega_trans_clip
	dw	0			;special case for opaque-clipped
	dw	0
	dw	0
	dw	0


non_clipped_drawing_functions label word

	dw	pCodeOFFSET  p_bm_trans_color_nc
	dw	pCodeOFFSET  p_bm_trans_color_nc
	dw	pCodeOFFSET  p_bm_trans_color_nc
	dw	pCodeOFFSET  p_bm_trans_color_nc
	dw	pCodeOFFSET  p_bm_opaque_color_nc
	dw	pCodeOFFSET  p_bm_opaque_color_nc
	dw	pCodeOFFSET  p_bm_opaque_color_nc
	dw	pCodeOFFSET  p_bm_opaque_color_nc
	dw	pCodeOFFSET p_ega_trans
	dw	pCodeOFFSET p_ega_trans
	dw	pCodeOFFSET p_ega_trans
	dw	pCodeOFFSET p_ega_trans
	dw	pCodeOFFSET p_ega_opaque_non_clip
	dw	pCodeOFFSET p_ega_opaque_non_clip
	dw	pCodeOFFSET p_ega_opaque_non_clip
	dw	pCodeOFFSET p_ega_opaque_non_clip


mono_non_clipped_drawing_functions	label word

	dw	pCodeOFFSET	p_bm_trans_black
	dw	pCodeOFFSET	p_bm_trans_black
	dw	pCodeOFFSET	p_bm_trans_white
	dw	pCodeOFFSET	p_bm_trans_white
	dw	pCodeOFFSET	p_bm_opaque_black_on_black
	dw	pCodeOFFSET	p_bm_opaque_black_on_white
	dw	pCodeOFFSET	p_bm_opaque_white_on_black
	dw	pCodeOFFSET	p_bm_opaque_white_on_white
page

define_frame	PSmartPro
cBegin	nogen
cEnd	nogen

;--------------------------Private-Routine------------------------------;
; p_ega_oc_outer_loop_prop
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
	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

	public	p_ega_oc_outer_loop_prop

p_ega_oc_outer_loop_prop	proc	near
	mov	bx,cx			;create dispatch table index
	shl	bx,1
	mov	ss_draw_clipped,pCodeOFFSET p_ega_opaque_clip_1
	push	ax			;save registers for second pass
	push	bx
	push	cx
	push	dx
	push	di
	push	bp			;prop code sometimes destroys this
	push	ax			;save regs destroyed inside call
	push	dx
	call	p_setup_ega_opaque_clip_magic
	pop	dx
	pop	ax
	jc	@f			;Planes are disabled. skip 1st pass.

	cmp	fontweight,0		;normal text ?
	jz	p_ega_oc_normal_1	     	;no.
	call	p_bold_ega_oc[bx]	;bold text
	jmp	short @f
p_ega_oc_normal_1:
	call	p_ega_oc[bx]		;do first pass
@@:
	mov	dx,EGA_BASE + SEQ_DATA
	mov	al,bptr colors[BACKGROUND]
	out	dx,al
	pop	bp
	pop	di			;restore registers for second pass
	pop	dx
	pop	cx
	pop	bx

;	The next block of code determines if the second pass is
;	necessary.
;
;	Note that the EGA has been programmed to allow writing only to
;	the planes composing the background color. Since we're writing
;	opaque text, all planes in the background color also in the
;	foreground have been set by the first pass.  The second
;	pass cleans up the background planes not in the foreground
;	color.  If there aren't any, then skip the second pass.

	mov	ax,ss_colors
	xor	al,ah			;determine mismatching bits
	and	al,ah			;isolate those in the bk color
	pop	ax
	jz	p_ega_oc_outer_loop_skip_pass_2

;	Set the return address variable in case we're fixed pitch.
;	The first "errnz" is commented out because these subroutines are
;	linked in from smartpro.asm.  The errnz should be alive there.

	sub	ss_draw_clipped,2
	errnz	<pCodeOFFSET p_ega_opaque_clip_1 - pCodeOFFSET p_ega_opaque_clip_2 - 2>

	cmp	fontweight,0		;normal text ?
	jz	p_ega_oc_normal_2	     	;no.
	call	p_bold_ega_oc[bx]	;bold text
	jmp	short @f
p_ega_oc_normal_2:
	call	p_ega_oc[bx]		;do first pass
@@:
p_ega_oc_outer_loop_chk_inner:
	test	excel,RES_EGA_INNER	;is there a middle part to string?
	jz	p_ega_oc_outer_loop_exit	;if no, skip EGA register setup
	call	p_set_ega_opaque_mode	;else, prepare EGA for full opaque
	and	excel,not RES_EGA_INNER	;prevent this at end of string
p_ega_oc_outer_loop_exit:
	ret

;	Update ES:DI as would have been done by second pass, then check
;	to see if EGA reprogramming is necessary for a non-clipped section
;	of opaque text.

p_ega_oc_outer_loop_skip_pass_2:
	inc	di			;update di for whoever follows
	jmp	short p_ega_oc_outer_loop_chk_inner

p_ega_oc_outer_loop_prop	endp
	page

;--------------------------Private-Routine------------------------------;
;   p_ega_trans_clip
;   p_ega_trans
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
	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

	public	p_ega_trans_clip
	public	p_ega_trans

p_ega_trans_clip:
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
DRAW_ADJUST = 0				;DI has nor been incremented
	upd_dev	<ret>			;will generate ret

;---------------------------------------
p_ega_trans:
	or	ax,ax
if TRANSPARENT_SPEEDUP
	jz	@f
endif
	push	dx
	mov	dx,EGA_BASE + GRAF_DATA
	out	dx,al
	xchg	al,es:[di+1]
	mov	al,ah
	out	dx,al
	xchg	ah,es:[di]
	pop	dx
@@:
DRAW_ADJUST = 0				;DI has nor been incremented
	upd_dev	<ret>			;will generate ret
page

;--------------------------Private-Routine------------------------------;
; p_bm_opaque_white_on_white_clip
; p_bm_trans_white_clip
; p_bm_trans_white
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
;	Wed 18-Jan-1989 15:06:12 -by-  Amit Chatterjee [amitc]
;	included 16 bit gathering from the stack and introduced bolding
;       also made use of extended register set to speed up things a bit
;
;	Wed 22-Jul-1987 17:54:46 -by-  Walt Moore [waltm]
;	Wrote it.
;-----------------------------------------------------------------------;
	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

p_bm_opaque_white_on_white_clip:
	mov	al,0FFh

p_bm_trans_white_clip:
	and	al,ss_clip_mask
	or	es:[di],al

DRAW_ADJUST	= 0			;STOSB isn't used
	upd_bm	<ret>			;Will generate the ret
page

p_bm_trans_white:
	xchg	ah,al			;first byte was in ah
	or	es:[di],ax

DRAW_ADJUST	= 0			;di has not been updated
	upd_bm	<ret>

page
;--------------------------Private-Routine------------------------------;
; p_bm_opaque_black_on_black_clip
; p_bm_trans_black_clip
; p_bm_trans_black
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
	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

p_bm_opaque_black_on_black_clip:
	mov	al,0FFh

p_bm_trans_black_clip:
	and	al,ss_clip_mask
	not	al
	and	es:[di],al

DRAW_ADJUST	= 0			;STOSB isn't used
	upd_bm	<ret>			;Will generate the ret
page

p_bm_trans_black:
	not	ax
	xchg	ah,al			;ah had the first byte
	and	es:[di],ax		;output it
DRAW_ADJUST 	= 0			;di has not been updates
	upd_bm	<ret>

page
;--------------------------Private-Routine------------------------------;
; p_bm_opaque_black_on_white
; p_bm_opaque_white_on_black
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
	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

p_bm_opaque_black_on_white:
	not	ax

p_bm_opaque_white_on_black:
	xchg	al,ah			;ah had the first byte
	stosw

DRAW_ADJUST	= 2			;STOSW is used
	upd_bm	<ret>			;Will generate the ret
page
;--------------------------Private-Routine------------------------------;
; p_bm_opaque_black_on_white_clip
; p_bm_opaque_white_on_black_clip
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
	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

p_bm_opaque_black_on_white_clip:
	not	al

p_bm_opaque_white_on_black_clip:
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
; p_bm_opaque_white_on_white
; p_bm_opaque_black_on_black
;
;   Standard bitmap drawing functions for:
;
;	opaque, white on white, non-clipped
;	opaque, black on black, non-clipped
;
; Entry:
;	AL,AH = character to output
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
	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

p_bm_opaque_white_on_white:
	mov	ax,0FFFFh
	jmpnext

p_bm_opaque_black_on_black:
	xor	ax,ax
	jmpnext stop
	stosw

DRAW_ADJUST	= 2			;STOSW is used
	upd_bm	<ret>			;Will generate the ret
page

;--------------------------Private-Routine------------------------------;
; p_bm_trans_color_fix
; p_bm_trans_color
;
;   Standard bitmap drawing function for:
;   (now used only for clipped cases and 8 bit output)
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
	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

	public	p_bm_trans_color_fix
	public	p_bm_trans_color

p_bm_trans_color_fix:
	push	dx			;put return address on stack

p_bm_trans_color:
	push	ax
	push	cx
	push	di

	xchg	bp,ss_p_frame		;get frame pointer

	mov	ch,ss_clip_mask
	or	ch,ch
	mov	ah,al
	jz	p_bm_trans_color_no_clip

	mov	ah,ch
	and	ah,al
p_bm_trans_color_no_clip:
	not	ah
	mov	cl,num_planes
	mov	ch,byte ptr ss_colors[FOREGROUND]

p_bm_trans_color_loop:
	and	es:[di],ah
	ror	ch,1			;see if this color plane is used
	jnc	p_bm_trans_color_set_byte
	or	es:[di],al

p_bm_trans_color_set_byte:
	add	di,next_plane
	dec	cl
	jnz	p_bm_trans_color_loop

	xchg	bp,ss_p_frame		;restore bp and ss_p_frame

	pop	di
	pop	cx
	pop	ax

DRAW_ADJUST = 0
	upd_bm <ret>			;Will generate the ret
page

;--------------------------Private-Routine------------------------------;
; p_bm_trans_color_fix_nc
; p_bm_trans_color_nc
;
;   Standard bitmap drawing function for:
;   (now used only for non-clipped 16 bit output)
;
;	transparent, color
;
;  Entry points are for the fixed width code and proportional width code,
;  respectively.
;
; Entry (fixed):
;	AH,AL = characters to output
;	DX = return address
;	ES:DI --> destination byte
; Entry (prop):
;	AH,AL = characters to output
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
	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

	public	p_bm_trans_color_fix_nc
	public	p_bm_trans_color_nc

p_bm_trans_color_fix_nc:
	push	dx			;put return address on stack

p_bm_trans_color_nc:
	push	ax
	push	cx
	push	bx
	push	di
  	xchg	ah,al			;get the first byte in al,2nd in ah
	xchg	bp,ss_p_frame		;get frame pointer

	mov	bx,ax			;bx has the characters
	not	ax			;ax has their inverses

	mov	cl,num_planes
	mov	ch,byte ptr ss_colors[FOREGROUND]

p_bm_trans_color_nc_loop:
	and	word ptr es:[di],ax
	ror	ch,1			;see if this color plane is used
	jnc	p_bm_trans_color_set_byte_nc
	or	word ptr es:[di],bx

p_bm_trans_color_set_byte_nc:
	add	di,next_plane
	dec	cl
	jnz	p_bm_trans_color_nc_loop

	xchg	bp,ss_p_frame		;restore bp and frame pointers

	pop	di
	pop	bx
	pop	cx
	pop	ax

DRAW_ADJUST = 0
	upd_bm <ret>			;Will generate the ret
page
;--------------------------Private-Routine------------------------------;
; p_bm_opaque_color_fix
; p_bm_opaque_color
;
;   Standard bitmap drawing function for:
;   (now used only for clipped 8 bit output)
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
	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

	public	p_bm_opaque_color_fix
	public	p_bm_opaque_color
p_bm_opaque_color_fix:
	push	dx			;put return address on stack

p_bm_opaque_color:

	push	ax
	push	cx
	push	dx
	push	di

	xchg	bp,ss_p_frame		;get frame pointer

	mov	ch,ss_clip_mask
	mov	dh,num_planes
	mov	dl,special_bm_opaque_color

p_bm_opaque_color_partial:
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
	jnz	p_bm_opaque_color_partial	;handle next color plane

	xchg	bp,ss_p_frame		;restore bp and ss_p_frame

	pop	di
	pop	dx
	pop	cx
	pop	ax
DRAW_ADJUST = 0
	upd_bm <ret>			;Will generate the ret
page
;--------------------------Private-Routine------------------------------;
; p_bm_opaque_color_fix_nc
; p_bm_opaque_color_nc
;
;   Standard bitmap drawing function for:
;   (now used only for non-clipped 16 bit output)
;
;	opaque, color
;
; Entry:
;	AH,AL = characters to output
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
	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

	public	p_bm_opaque_color_fix_nc
	public	p_bm_opaque_color_nc
p_bm_opaque_color_fix_nc:
	push	dx			;put return address on stack

p_bm_opaque_color_nc:

	push	ax
	push	bx
	push	cx
	push	dx
	push	di

	xchg	bp,ss_p_frame		;get frame pointer

	mov	dh,num_planes
	mov	dl,special_bm_opaque_color

p_bm_opaque_color_partial_nc:
	shr	dl,1			;set C to inversion mask
	sbb	bx,bx
	and	bx,ax			;BH,BL=1 where we want NOT background
	shr	dl,1			;set C to background color
	sbb	cx,cx			
	xor	cx,bx			;AX=destination bytes
	mov	bx,word ptr es:[di]	; get the first byte
	xor	bl,ch			; CH has first processed src byte
	xor	bh,cl			; CL has second processed src byte
	xor	word ptr es:[di],bx	;first byte done
	add	di,next_plane		;point to next color plane
	dec	dh
	jnz	p_bm_opaque_color_partial_nc;handle next color plane

	xchg	bp,ss_p_frame		;restore bp and ss_p_frame

	pop	di
	pop	dx
	pop	cx
	pop	bx
	pop	ax

DRAW_ADJUST = 0
	upd_bm <ret>			;Will generate the ret
page

;--------------------------Private-Routine------------------------------;
; p_ega_opaque_non_clip:
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
	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

	public	p_ega_opaque_non_clip
p_ega_opaque_non_clip:
	xchg	al,ah
	mov	es:[di],ax

DRAW_ADJUST	= 0			;STOSW is not used.
	upd_dev <ret>			;Will generate the ret
page

;--------------------------Private-Routine------------------------------;
;   p_ega_opaque_clip_2
;   p_ega_opaque_clip_1
;
;   Device drawing functions for:
;
;	opaque, clipped
;
;   Two passes are required for this mode of text output.  Label
;   p_ega_opaque_clip_x is the drawing function for pass x.
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
	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

	public	p_ega_opaque_clip_2
	public	p_ega_opaque_clip_1
p_ega_opaque_clip_2:
	not	al
p_ega_opaque_clip_1:
	xchg	al,es:[di]

DRAW_ADJUST	= 0			;STOSB is not used
	upd_dev <ret>			;Will generate the ret
page

;--------------------------Private-Routine------------------------------;
; p_preset_pro_text
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
	assumes ds,nothing
	assumes es,nothing
	assumes ss,StrStuff

	public	p_preset_pro_text
p_preset_pro_text proc	near

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
	jnz	p_preset_pro_text_lookup_func
	cmp	num_planes,1
	jne	p_preset_pro_text_standard_func

	mov	ax,mono_non_clipped_drawing_functions[bx]

p_preset_pro_text_lookup_func:

	mov	si,special_case_non_clip_tables[bx]
	mov	di,special_case_clip_tables[bx]
	cmp	fontweight,0		;normal text ?
	jz	p_preset_pro_text_have_func
	mov	si,bold_special_case_non_clip_tables[bx]
	mov	di,bold_special_case_clip_tables[bx]


p_preset_pro_text_have_func:
	mov	non_clipped_table,si
	mov	clipped_table,di
	mov	ss_draw,ax

	mov	ax,clipped_drawing_functions[bx]
	mov	ss_draw_clipped,ax

	ret


p_preset_pro_text_standard_func:
	mov	si,pCodeOFFSET gen_nc
	mov	di,pCodeOFFSET gen_cl
	cmp	fontweight,0		;normal text ?
	jz	p_preset_pro_text_have_func
	mov	si,pCodeOFFSET bold_gen_nc
	mov	di,pCodeOFFSET bold_gen_cl
	jmp	short p_preset_pro_text_have_func

p_preset_pro_text endp
	page

	public	p_setup_ega_opaque_clip_magic
;--------------------------------------------------------------------------;
; p_setup_ega_opaque_clip_magic
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

p_setup_ega_opaque_clip_magic	proc near

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
p_setup_ega_opaque_clip_magic	endp
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
	one_char	<p_gen_nc>,<non_clipped_output>,<loop_logic>,<sub>,<>
	two_char	<p_gen_nc>,<non_clipped_output>,<loop_logic>,<sub>,<>
	three_char	<p_gen_nc>,<non_clipped_output>,<loop_logic>,<sub>,<>
	four_char	<p_gen_nc>,<non_clipped_output>,<loop_logic>,<sub>,<>
	five_char	<p_gen_nc>,<non_clipped_output>,<loop_logic>,<sub>,<>
	n_char		<p_gen_nc>,<non_clipped_output>,<loop_logic>,<sub>,<>

	bold_1_char	<p_b_gen_nc>,<non_clipped_output>,<loop_logic>,<sub>,<>
	bold_2_char	<p_b_gen_nc>,<non_clipped_output>,<loop_logic>,<sub>,<>
	bold_3_char	<p_b_gen_nc>,<non_clipped_output>,<loop_logic>,<sub>,<>
	bold_4_char	<p_b_gen_nc>,<non_clipped_output>,<loop_logic>,<sub>,<>
	bold_5_char	<p_b_gen_nc>,<non_clipped_output>,<loop_logic>,<sub>,<>
	bold_n_char	<p_b_gen_nc>,<non_clipped_output>,<loop_logic>,<sub>,<>

	one_char	<p_gen_cl>,<clipped_output>,<loop_logic>,<sub>,<didec>
	two_char	<p_gen_cl>,<clipped_output>,<loop_logic>,<sub>,<didec>
	three_char	<p_gen_cl>,<clipped_output>,<loop_logic>,<sub>,<didec>
	four_char	<p_gen_cl>,<clipped_output>,<loop_logic>,<sub>,<didec>
	five_char	<p_gen_cl>,<clipped_output>,<loop_logic>,<sub>,<didec>
	n_char		<p_gen_cl>,<clipped_output>,<loop_logic>,<sub>,<didec>

	bold_1_char	<p_b_gen_cl>,<clipped_output>,<loop_logic>,<sub>,<didec>
	bold_2_char	<p_b_gen_cl>,<clipped_output>,<loop_logic>,<sub>,<didec>
	bold_3_char	<p_b_gen_cl>,<clipped_output>,<loop_logic>,<sub>,<didec>
	bold_4_char	<p_b_gen_cl>,<clipped_output>,<loop_logic>,<sub>,<didec>
	bold_5_char	<p_b_gen_cl>,<clipped_output>,<loop_logic>,<sub>,<didec>
	bold_n_char	<p_b_gen_cl>,<clipped_output>,<loop_logic>,<sub>,<didec>

if SPECIAL_CASE_DEV_ONC eq 1
	one_char	<p_dev_onc>,<ega_onc>,<upd_dev loop_logic>,<inline>,<>
	two_char	<p_dev_onc>,<ega_onc>,<upd_dev loop_logic>,<inline>,<>
	three_char	<p_dev_onc>,<ega_onc>,<upd_dev loop_logic>,<inline>,<>
	four_char	<p_dev_onc>,<ega_onc>,<upd_dev loop_logic>,<inline>,<>
	five_char	<p_dev_onc>,<ega_onc>,<upd_dev loop_logic>,<sub>,<>
endif

if SPECIAL_CASE_DEV_TNC eq 1
	one_char	<p_dev_tnc>,<ega_tnc>,<upd_dev loop_logic>,<inline>,<>
	two_char	<p_dev_tnc>,<ega_tnc>,<upd_dev loop_logic>,<inline>,<>
	three_char	<p_dev_tnc>,<ega_tnc>,<upd_dev loop_logic>,<inline>,<>
	four_char	<p_dev_tnc>,<ega_tnc>,<upd_dev loop_logic>,<sub>,<>
;	n_char		<p_dev_tnc>,<ega_tnc>,<upd_dev loop_logic>,<sub>,<>
endif

if SPECIAL_CASE_DEV_TC eq 1
	one_char	<p_dev_tc>,<ega_tc>,<upd_dev loop_logic>,<sub>,<didec>
	two_char	<p_dev_tc>,<ega_tc>,<upd_dev loop_logic>,<sub>,<didec>
	three_char	<p_dev_tc>,<ega_tc>,<upd_dev loop_logic>,<sub>,<didec>
	four_char	<p_dev_tc>,<ega_tc>,<upd_dev loop_logic>,<sub>,<didec>
;	n_char		<p_dev_tc>,<ega_tc>,<upd_dev loop_logic>,<sub>,<didec>
endif

if SPECIAL_CASE_BM_OBWNC eq 1
	one_char	<p_bm_obwnc>,<obwnc>,<upd_bm loop_logic>,<sub>,<>
	two_char	<p_bm_obwnc>,<obwnc>,<upd_bm loop_logic>,<sub>,<>
	three_char	<p_bm_obwnc>,<obwnc>,<upd_bm loop_logic>,<sub>,<>
	four_char	<p_bm_obwnc>,<obwnc>,<upd_bm loop_logic>,<sub>,<>
;	n_char		<p_bm_obwnc>,<obwnc>,<upd_bm loop_logic>,<sub>,<>
endif

if SPECIAL_CASE_BM_OWBNC eq 1
	one_char	<p_bm_owbnc>,<owbnc>,<upd_bm loop_logic>,<sub>,<>
	two_char	<p_bm_owbnc>,<owbnc>,<upd_bm loop_logic>,<sub>,<>
	three_char	<p_bm_owbnc>,<owbnc>,<upd_bm loop_logic>,<sub>,<>
	four_char	<p_bm_owbnc>,<owbnc>,<upd_bm loop_logic>,<sub>,<>
;	n_char		<p_bm_owbnc>,<owbnc>,<upd_bm loop_logic>,<sub>,<>
endif

if SPECIAL_CASE_BM_TBNC eq 1
	one_char	<p_bm_tbnc>,<tbnc>,<upd_bm loop_logic>,<sub>,<>
	two_char	<p_bm_tbnc>,<tbnc>,<upd_bm loop_logic>,<sub>,<>
	three_char	<p_bm_tbnc>,<tbnc>,<upd_bm loop_logic>,<sub>,<>
	four_char	<p_bm_tbnc>,<tbnc>,<upd_bm loop_logic>,<sub>,<>
;	n_char		<p_bm_tbnc>,<tbnc>,<upd_bm loop_logic>,<sub>,<>
endif

if SPECIAL_CASE_BM_TWNC eq 1
	one_char	<p_bm_twnc>,<twnc>,<upd_bm loop_logic>,<sub>,<>
	two_char	<p_bm_twnc>,<twnc>,<upd_bm loop_logic>,<sub>,<>
	three_char	<p_bm_twnc>,<twnc>,<upd_bm loop_logic>,<sub>,<>
	four_char	<p_bm_twnc>,<twnc>,<upd_bm loop_logic>,<sub>,<>
;	n_char		<p_bm_twnc>,<twnc>,<upd_bm loop_logic>,<sub>,<>
endif

page
sEnd	pCode
	end

