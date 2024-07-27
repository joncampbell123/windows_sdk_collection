page	,132
;***************************************************************************
;									   *
;		Copyright (C) 1985-1986 by Microsoft Inc.		   *
;									   *
;***************************************************************************

TITLE	HiDither - High Resolution Dithering Alogrithm
%out	hidither


	.xlist
	include cmacros.inc
	.list


	public	Dither


sBegin	code
assumes cs,code
assumes ds,code

page
;---------------------------------------------------------------------------
;
;   Dither(value, lpBrush)
;
;   this totally rad function takes value between 0 - 255 and uses it to
;   create a 8 x 8 pile of bits that can be used as a brush. it does this
;   by reducing the input value to a number between 0 - 64 and then turns on
;   that many bits in the 8 x 8 field. thus the value 0 leads to brush of
;   all zeros, a 255 value leads to a brush of all 1's, and a value of
;   128 leads to the checkerboard grey.
;
;	the algorithm is equivalent to turning on bits based on this array
;
;	 0 32  8 40  2 34 10 42 	This table favors devices like
;	48 16 56 24 50 18 58 26 	the Hercules card where the aspect
;	12 44  4 36 14 46  6 38 	ratio is close to 1:1
;	60 28 52 20 62 30 54 22
;	 3 35 11 43  1 33  9 41
;	51 19 59 27 49 17 57 25
;	15 47  7 39 13 45  5 37
;	63 31 55 23 61 29 53 21
;
;	where given a value between 0 - 64 turn on the bit if the array value
;	is less then the desired value.
;
;   the stroke of genius involved here is to recoginze that to compute the
;   x and y shift for a given bit you consider the value to be three
;   blocks of 2 bits where the least significant pair should cause the
;   most fluctuation (4) the middle (2) and the most significant pair causes
;   the minor fluctuation (1). these get totaled up and the bit is shifted
;   to the proper location in the byte and it is stored at the proper y shift.
;
;   e.g.
;	27 =	01	10	11	- a value between 0 and 64
;   xshift 3 =	 1*(1) + 1*(2) + 0*(4)	- using values from XTab
;   yshift 5 =	 1*(1) + 0*(2) + 1*(4)	- using values from YTab
;	and as you can see the value 27 is down 5 and over 3 in the array
;	like beauty, eh!
;
;   Reference:	A Survey of Techniques for the Display of Continuous
;		Tone Pictures on Bilevel Displays,;
;		Jarvis, Judice, & Ninke;
;		COMPUTER GRAPHICS AND IMAGE PROCESSING 5, pp 13-40, (1976)
;
;
;
;	The value that is generated for grey is the desired
;	checkerboard grey, therefore grey is not special cased.
;	The value generated for light and dark grey are not the desired
;	greys, so they must be flagged when encountered.
;
;
;
;	Entry:	dl     =  value
;		es:di --> destination
;		si     =  previous grey indicator
;
;	Exit:	es:di --> next destination
;		si     =  New grey indicator
;			  d0:1 = 01b for dk grey
;			  d0:1 = 01b for lt grey
;
;
;	Uses:	ax,bx,cx,dx,di,ds,es,flags
;
;	NOTE:	This alogrithm assumes the size of a pattern to be 8x8


SizePattern	equ	8


Xtab	db	0,1,1,0
YTab	db	0,1,0,1


Dither	proc	near

	mov	ax,cs
	mov	ds,ax
	xor	ax,ax			;Clear out the recieving area
	stosw
	stosw
	stosw
	stosw
	sub	di,8			;--> back to start of plane
	errnz	SizePattern-8		;Pattern must be 8x8

	xor	bh,bh
	shr	dl,1			;Convert value 0-255 into
	adc	dl,al			;  value 0 - 64
	shr	dl,1
	mov	dh,3			;Will be using 00000011b a few times
	mov	al,00000001b		;Assume dark grey
	cmp	dl,16			;Dark grey?
	je	CheckDone		;  Yes
	mov	al,00000011b		;Assume lt grey
	cmp	dl,48			;Dark grey?
	je	CheckDone		;  Yes
	xor	ax,ax			;Not one of the special cases

CheckDone:
	shl	si,1			;Rotate old value
	shl	si,1
	or	si,ax			;Add in this grey flag

SetNextBit:				;For current value down to 0
	dec	dl
	jl	Done			;No more to compute
	xor	ax,ax
	mov	cl,dh
	mov	ch,dl

Next:					;Compute x & y shifts for current value
	shl	ax,1
	mov	bl,ch
	and	bl,dh
	add	ah,XTab[bx]
	add	al,YTab[bx]
	shr	ch,1
	shr	ch,1
	loop	Next

	mov	bl,al			;Perform x & y shifts and store the bit
	mov	cl,ah
	mov	al,80h
	shr	al,cl

	or	es:[di][bx],al
	jmp	SetNextBit		;More to compute

done:
	add	di,8			;--> to start of next plane
	errnz	SizePattern-8		;Pattern must be 8x8
	ret

Dither	endp


sEnd	code
end
