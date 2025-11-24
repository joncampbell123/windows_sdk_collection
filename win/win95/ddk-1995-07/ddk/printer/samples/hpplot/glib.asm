; (c) Copyright 1987 MICROGRAFX, Inc., All Rights Reserved.  Permission to
; use this work for any purpose must be obtained in writing from MICROGRAFX,
; 1820 N. Greenville Ave., Richardson, Tx.  75081.

;******************************************************************************
;******************************************************************************
;
;				     glib
;
;******************************************************************************
;******************************************************************************



memM		=	1	; select medium model - memS = small model
?PLM		=	1	; select Pascal calling convention
?WIN		=	1	; windows prolog convention

include cmacros.inc

;* **************************** External Functions ************************** *
;* ********************************* History ******************************** *
;* ******************************** Constants ******************************* *
;* **************************** External Data ******************************* *
;* ****************************** Local Data ******************************** *

sBegin	DATA

; Angle table for arc tangent.

AtanTable label word
	  DD 58982400
	  DD 29491200
	  DD 17409668
	  DD 9198792
	  DD 4669451
	  DD 2343786
	  DD 1173036
	  DD 586661
	  DD 293348
	  DD 146676
	  DD 73338
	  DD 36669
	  DD 18335
	  DD 9167

; Sine table for integer sine and cosine.
SinTable label word
	 DW	0,   286,   572,   857,  1143,	1428,  1713,  1997,  2280
	 DW  2563,  2845,  3126,  3406,  3686,	3964,  4240,  4516,  4790
	 DW  5063,  5334,  5604,  5872,  6138,	6402,  6664,  6924,  7182
	 DW  7438,  7692,  7943,  8192,  8438,	8682,  8923,  9162,  9397
	 DW  9630,  9860, 10087, 10311, 10531, 10749, 10963, 11174, 11381
	 DW 11585, 11786, 11983, 12176, 12365, 12551, 12733, 12911, 13085
	 DW 13255, 13421, 13583, 13741, 13894, 14044, 14189, 14330, 14466
	 DW 14598, 14726, 14849, 14968, 15082, 15191, 15296, 15396, 15491
	 DW 15582, 15668, 15749, 15826, 15897, 15964, 16026, 16083, 16135
	 DW 16182, 16225, 16262, 16294, 16322, 16344, 16362, 16374, 16382
	 DW 16384

		staticW 	SemiX,?
		staticW 	SemiY,?
		staticW 	Z,?,3
		staticW 	X,?,3

;* ***************************** Global Data ******************************** *
		globalW 	nPoints,?

sEnd		DATA

;* ************************** Exported Routines ***************************** *

sBegin		CODE

assumes 	CS,CODE
assumes 	DS,DATA

;.............................................................................
; Arctan (Xorigin,Yorigin,X,Y) : int
;
; Given an origin and a point this function returns the arc tangent of
; point to the nearest 10th of a degree.
;
; The following algorithm is used to compute the arc tangent:
;
;  N = 14;
;  I = 0;
;  /* set up constants */
;  while I <= N
;  {
;     Constant [I] = floor (atn (1/(2^I)) * 180 / 3.141593 * 10 * 65536 + .5);
;     I = I + 1;
;  }
;  Z = -(X - Xorigin) * 65536;
;  X = (Y - Yorigin) * 65536;
;  I = 0;
;  A = 90 * 10 * 65536;
;  while I <= N
;  {
;     if Z < 0 then
;     {
;	 A = A - Constant [I];
;	 T = Z;
;	 Z = (Z + X) * 2;
;	 X = X - floor (T * (2 ^ (-2 * I)));
;     }
;     else
;     {
;	 A = A + Constant [I];
;	 T = Z;
;	 Z = (Z - X) * 2;
;	 X = X + floor (T * (2 ^ (-2 * I)));
;     }
;     I = I + 1;
;  }
;  A = int ((A / 65536 + .5) / 10);
;
;  Note: Although floating point values are used in this description, all
;	 values are scaled so integer arithmetic is used throughout.
;.............................................................................

cProc	Arctan,<PUBLIC>,<di,si>
	parmW	PointX
	parmW	PointY
	parmW	ScaleX
	parmW	ScaleY
cBegin
	mov	ax,ScaleX
	mov	SemiX,ax
	mov	ax,ScaleY
	mov	SemiY,ax
	mov	ax,PointX	; Map point about origin.
	mov	dx,PointY
	call	get_angle
cEnd


;.............................................................................
; Cosine (Angle) : int
;
; Given an Angle (0 - 3600) this function returns the cosine of the angle.
; The angle is assumed to in 10ths of a degree. The calculated cosine is scaled
; by 16384.
;.............................................................................

cProc	Cosine,<PUBLIC>,<di,si>
	parmW	Angle
cBegin
	mov	si,Angle
	call	icos
cEnd


;.............................................................................
; Sine (Angle) : int
;
; Given an Angle (0 - 3600) this function returns the sine of the angle.
; The angle is assumed to in 10ths of a degree. The calculated sine is scaled
; by 16384.
;.............................................................................

cProc	Sine,<PUBLIC>,<di,si>
	parmW	Angle
cBegin
	mov	si,Angle
	call	isin
cEnd

;* **************************** Local Routines ****************************** *

;..............................................................................
; arc_tan
;
; This function returns the angle of the point in dx,ax in ax scaled by 10.
; The point is first scaled by Xaxis & Yaxis in order to handle an elliptical
; arc.
;..............................................................................

arc_tan:
	push	bx			; Save registers.
	push	cx
	push	dx
	push	di
	push	si
	xor	di,di			; Zero out registers.
	mov	bx,di
	cmp	SemiY,di		; Check for 0 Xaxis or Yaxis.
	jz	arc_tan99
	mov	bx,90 * 10
	cmp	SemiX,di
	jz	arc_tan99
	mov	bx,di
	mov	cx,di
	call	arc_tan_init		; Set up initial conditions.
arc_tan0:
	add	si,4			; Point to next constant.
	mov	cx,Z
	mov	ax,Z+2
	mov	dx,Z+4
	or	dx,dx			; Is Z < 0 ?
	js	arc_tan1		; Jump if yes.
	add	di,[si] 		; Angle = Angle + constant.
	adc	bx,[si+2]
	sub	cx,X			; Z = Z - X.
	sbb	ax,X+2
	sbb	dx,X+4
	call	update_z
	add	X,cx			; X = X + (old) Z.
	adc	X+2,ax
	adc	X+4,dx
	jmp	arc_tan2
arc_tan1:
	sub	di,[si] 		; Angle = Angle - constant.
	sbb	bx,[si+2]
	add	cx,X			; Z = Z + X.
	adc	ax,X+2
	adc	dx,X+4
	call	update_z
	sub	X,cx			; X = X - (old) Z.
	sbb	X+2,ax
	sbb	X+4,dx
arc_tan2:
	inc	nPoints 		; While NPoints <= 12.
	cmp	nPoints,12
	jle	arc_tan0
arc_tan99:
	add	di,32768		; Round off.
	adc	bx,0
	mov	ax,bx
	pop	si			; Restore registers.
	pop	di
	pop	dx
	pop	cx
	pop	bx
	ret

arc_tan_init:
	push	dx			; Save Y.
	neg	ax			; Initial Z.
	imul	SemiY
	mov	Z,cx
	mov	Z+2,ax
	mov	Z+4,dx
	pop	ax			; Restore Y.
	mul	SemiX			; initial X.
	mov	X,cx
	mov	X+2,ax
	mov	X+4,dx
	mov	si,offset AtanTable	; Pointer to constants.
	mov	di,[si] 		; Calculate angle in bx,di.
	mov	bx,[si+2]
	mov	nPoints,cx		; Set nPoints to 0.
	ret

update_z:
	shl	cx,1
	rcl	ax,1
	rcl	dx,1
	xchg	cx,Z
	xchg	ax,Z+2
	xchg	dx,Z+4
	push	bx
	mov	bx,nPoints
	or	bx,bx
	jz	update_z99
	shl	bx,1
update_z0:
	sar	dx,1
	rcr	ax,1
	rcr	cx,1
	dec	bx
	jnz	update_z0
update_z99:
	pop	bx
	ret


;............................................................................
; get_angle
;
; Given a point in ax,dx this function determines arc tangent or angle of the
; given point. The angle is returned in ax.
;............................................................................

get_angle:
	call	get_quadrant
	cmp	cl,5
	jne	get_angle_10
	xor	ax,ax
	jmp	get_angle_end
get_angle_10:
	call	arc_tan
	cmp	cl,2
	jle	get_angle_end
	add	ax,180 * 10
get_angle_end:
	ret

;............................................................................
; get_quadrant
;
; Determine quadrant of ax,dx & return in cl.
; Quadrants are arranged as follows:
;
;	2 | 1	5 for origin
;	-----
;	3 | 4
;
; This function also maps the point to the proper form for arc_tan.
;............................................................................

get_quadrant:
	mov	cl,5
	neg	ax		; is X <= 0 ?
	jz	get_quadrant1	; jump if X = 0
	jg	get_quadrant0	; jump if X < 0

; X positive

	or	dx,dx		; test Y
	jg	q4		; jump if Y > 0
	jmp	q1		; jump if Y <= 0

; X negative

get_quadrant0:
	or	dx,dx
	jl	q2
	jmp	q3

; X = 0

get_quadrant1:
	or	dx,dx
	jz	q5
	jle	q2
	jmp	q3

; calculate quadrant

q1:	dec	cl
q2:	dec	cl
	neg	ax
	neg	dx
q3:	dec	cl
q4:	dec	cl
q5:	ret


;..............................................................................
; icos, isin
;
; Given a degree in si return cosine (sine) in ax scaled by 16384.
; Degrees range from 0 - 3600. bx,cx,dx are destroyed.
;..............................................................................

; given degree in si return cos(si) in ax scaled by 32768

icos:
	push	si
	call	reduce_angle
	neg	si
	add	si,90 * 10
	call	isin0
	test	cl,02h
	jz	icos99
	neg	ax
icos99:
	pop	si
	ret

; given degree in si return sin(si) in ax scaled by 16384

isin:
	push	si
	call	reduce_angle
	call	isin0
	cmp	cl,02h
	jg	isin9
	neg	ax
isin9:
	pop	si
	ret
isin0:
	mov	ax,si
	mov	bl,10
	div	bl
	mov	bl,al
	xor	bh,bh
	shl	bx,1
	mov	ch,ah
	mov	ax, SinTable[bx]
	or	ch,ch
	jz	isin99
	mov	si,ax
	sub	ax, SinTable[bx+2]
	neg	ax
	mov	bl,ch
	xor	bh,bh
	mul	bx
	add	ax,5
	adc	dx,0
	mov	bx,10
	div	bx
	add	ax,si
isin99:
	ret

;..............................................................................
; reduce_angle
;
; This function reduces the angle in si to the range 0 - 90 * 10. It also
; determines the quadrant of the angle which is returned in cl.
;..............................................................................

reduce_angle:
	mov	cl,1
	or	si,si
	jns	reduce_angle0
	add	si,360 * 10
reduce_angle0:
	cmp	si,360 * 10
	jle	reduce_angle1
	sub	si,360 * 10
	jmp	reduce_angle0
reduce_angle1:
	cmp	si,180 * 10
	jl	reduce_angle2
	add	cl,2
	sub	si,180 * 10
reduce_angle2:
	cmp	si,90 * 10
	jle	reduce_angle99
	neg	si
	add	si,180 * 10
	inc	cl
reduce_angle99:
	ret

sEnd	CODE

end
