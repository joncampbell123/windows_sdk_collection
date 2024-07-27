page

;***************************************************************************
;									   *
;		Copyright (C) 1985-1986 by Microsoft Inc.		   *
;									   *
;***************************************************************************



;	Raster Op Definitions
;
;
;	The include file COMMENT.BLT contains a good description
;	of the encoding of the raster operations.  It should be
;	read before examining the definitions that follow.
;
;	The sixteen-bit number indicating which raster Op is to be
;	performed is encoded in the following manner:


epsOff	=	0000000000000011B	;Offset within parse string
epsIndx =	0000000000011100B	;Parse string index
LogPar	=	0000000000100000B	;(1 indicates implied NOT as Logop6)
LogOp1	=	0000000011000000B	;Logical Operation #1
LogOp2	=	0000001100000000B	;Logical Operation #2
LogOp3	=	0000110000000000B	;Logical Operation #3
LogOp4	=	0011000000000000B	;Logical Operation #4
LogOp5	=	1100000000000000B	;Logical Operation #5


;	The parity bit is used to encode an optional sixth logical operation
;	which will always be a "NOT".  In most cases this is used to get an
;	even number of "NOT"s so that reduction can take place (two sequential
;	trailing "NOT"s cancel each other out and thus are eliminated).



;	Each LogOp (Logical Operation) is encoded as follows:

LogNOT	=	00B			;NOT result
LogXOR	=	01B			;XOR result with next operand
LogOR	=	10B			;OR  result with next operand
LogAND	=	11B			;AND result with next operand



;	The parse string is a string which contains the operands for
;	the logical operation sequences (source, destination, pattern).
;	The logic opcodes are applied to the current result and the next
;	element of the given string (unless the LogOp is a NOT which only
;	affects the result).
;
;	The string is encoded as eight two-bit numbers indicating which
;	operand is to be used

opDefs		struc
  OpSpec	db	?		  ;Special Operand as noted below
  OpSrc 	db	?		  ;Operand is source field
  OpDest	db	?		  ;Operand is destination field
  OpPat 	db	?		  ;Operand is pattern field
opDefs		ends



;	The special operand is used for a few rops that would not fit into
;	an RPN format.	On the first occurance of an OpSpec, the current result
;	is "PUSHED", and the next operand is loaded.  On the second occurance
;	of the OpSpec, the given logic operation is performed between the
;	current result and the "PUSHED" value.
;
;	**NOTE**  Since there can be no guarantee that the client will call
;	the BLT routine with one of the 256 published raster ops, it is
;	possible that a value might be "PUSHED" and then never "POPPED".
;	If these "PUSHES" are made to the stack, then care must be made to
;	remove the "PUSHED" value.
;
;	In any case, since the raster op was not one of the published
;	"magic numbers", the BLT can be aborted or the result can be
;	computed to the extent possible.  The only restriction is that it
;	must not crash the system (i.e. don't leave extra stuff on the stack).
;
;		Simply: Compute garbage, but don't crash!




;	Define the parse strings to be allocated later.
;
;	An example parse string for the pattern "SDPSDPSD" would be
;	"0110110110110110B"


parseStr0	=	07AAAH		;src,pat,dest,dest,dest,dest,dest,dest
parseStr1	=	079E7H		;src,pat,dest,src,pat,dest,src,pat
parseStr2	=	06DB6H		;src,dest,pat,src,dest,pat,src,dest
parseStr3	=	0AAAAH		;dest,dest,dest,dest,dest,dest,dest,dest
parseStr4	=	0AAAAH		;dest,dest,dest,dest,dest,dest,dest,dest
parseStr5	=	04725H		;src,spec,src,pat,spec,dest,src,src
parseStr6	=	04739H		;src,spec,src,pat,spec,pat,dest,src
parseStr7	=	04639H		;src,spec,src,dest,spec,pat,dest,src



;	The following equates are for certain special functions that are
;	derived from the very first string (index of SpecParseStrIndex).
;
;	These strings will have their innerloops special cased for
;	speed enhancements (i.e MOVSx and STOSx for pattern copys and
;	white/black fill, and MOVSx for source copy if possible)

patCopy 	equ	0021H		;P    - dest = Pattern
NOTpatCopy	equ	0001H		;Pn   - dest = NOT Pattern
FillBlack	equ	0042H		;DDx  - dest = 0 (black)
FillWhite	equ	0062H		;DDxn - dest = 1
sourceCopy	equ	0020H		;S    - dest = source


	errnz	logXOR-01B		;These must hold true for above equates
	errnz	logOp1-0000000011000000B
	errnz	logPar-0000000000100000B
	errnz	parseStr0-7AAAH 	;  plus the string must be SPDD
page


;	maxBLTsize is the maximum stack space required for the BITBLT
;	code.  This is a hard number to compute.  It must be based on
;	the worst case situation:
;
;		worst phase alignment
;		worst color conversions
;		first byte present
;		last byte present
;		full inner loop
;		jump into the inner loop
;
;	and any other factor which could increase the size of the code.


maxBLTsize	=	450		;Max stack space a BLT will require




;	The following equates are for stuffing into the generated BLT.
;	For full 16-bit values, the two bytes are reversed so that they
;	come out correctly when stored in memory (i.e. the 8088 stores
;	the LSB first).



I_addALbyteI	  equ	004h		;ADD	al,byte immediate
I_addAXwordI	  equ	005h		;ADD	ax,word immediate
I_addDIwordI	  equ	0C781h		;ADD	di,immediate
I_addSIwordI	  equ	0C681h		;ADD	si,immediate
I_andAL_Mem	  equ	00622h		;AND	al,[addr]
I_andALbyteI	  equ	024h		;AND	al,byte immediate

I_cmpAXwordI	  equ	03Dh		;CMP	ax,word immediate
I_cmpCX_2	  equ	0F983h		;CMP	cx,2
I_CSoverride	  equ	02Eh		;CS:

I_decSI_decDI	  equ	04F4Eh		;DEC	si
					;DEC	di
I_esOverride	  equ	026h		;ES:

I_incSI_incDI	  equ	04746h		;INC	si
					;INC	di
I_jcp5h 	  equ	00372h		;JC	$+5
I_jcp0Dh	  equ	00B72h		;JC	$+0Dh
I_jcp12h	  equ	01072h		;JC	$+12h
I_JMPnear	  equ	0E9h		;JMP	near
I_JNC		  equ	073h		;JNC	short
I_jncp12h	  equ	01073h		;JNC	$+12h

I_leaAX_SI_Disp16 equ	0848Dh		;lea	ax,WordDisp[si]
I_leaAX_DI_Disp16 equ	0858Dh		;lea	ax,WordDisp[di]
I_lodsb 	  equ	0ACh		;LODSB
I_loop		  equ	0E2h		;LOOP

I_movAH_AL	  equ	0E08Ah		;MOV	ah,al
I_movAH_dest	  equ	0258Ah		;MOV	ah,[di]
I_movAH_SI_Disp16 equ	0A48Ah		;MOV	ah,WordDisp[si]
I_movAL_0FFH	  equ	0FFB0h		;MOV	al,0FFH
I_movAL_AH	  equ	0C48Ah		;MOV	al,ah
I_movAL_DL	  equ	0C28Ah		;MOV	al,dl
I_movAL_DH	  equ	0C68Ah		;MOV	al,dh
I_movAL_dest	  equ	0058Ah		;MOV	al,[di]
I_movAL_Mem	  equ	0A0h		;MOV	al,[addr]
I_movAL_SI_Disp16 equ	0848Ah		;MOV	al,WordDisp[si]
I_movAX_DS	  equ	0D88Ch		;MOV	ax,ds
I_movAX_DI	  equ	0C78Bh		;MOV	ax,di
I_movAX_ES	  equ	0C08Ch		;MOV	ax,es
I_movAX_SI	  equ	0C68Bh		;MOV	ax,si
I_movAXwordI	  equ	0B8h		;MOV	ax,immediate
I_movBL_AL	  equ	0D88Ah		;MOV	bl,al
I_movBLbyteI	  equ	0B3h		;MOV	bl,immediate
I_movBPwordI	  equ	0BDh		;MOV	bp,immediate
I_movBXwordI	  equ	0BBh		;MOV	bx,word immediate
I_movCX_DS	  equ	0D98Ch		;MOV	cx,ds
I_movCXwordI	  equ	0B9h		;MOV	cx,immediate
I_movDH_BX_Disp8  equ	0778Ah		;MOV	dh,ByteDisp[si]
I_movDI_AX	  equ	0F88Bh		;MOV	di,ax
I_movDL_AL	  equ	0D08Ah		;MOV	dl,al
I_movDS_AX	  equ	0D88Eh		;MOV	ds,ax
I_movDS_CX	  equ	0D98Eh		;MOV	ds,cx
I_movES_AX	  equ	0C08Eh		;MOV	es,ax
I_movMem_AL	  equ	0A2h		;MOV	[addr],al
I_movSI_AX	  equ	0F08Bh		;MOV	si,ax
I_movsb 	  equ	0A4h		;MOVSB
I_movsw 	  equ	0A5h		;MOVSW

I_notAL 	  equ	0D0F6h		;NOT	al
I_notAX 	  equ	0D0F7h		;NOT	ax

I_orAL_AH	  equ	0C40Ah		;XOR	al,ah
I_orAH_AL	  equ	0E00Ah		;XOR	ah,al

I_popBX 	  equ	05Bh		;POP	bx
I_popSI 	  equ	05Eh		;POP	si
I_popDI_popCX	  equ	0595Fh		;POP	di
					;POP	cx
I_pushBX	  equ	053h		;PUSH	bx
I_pushSI	  equ	056h		;PUSH	si
I_pushCX_pushDI   equ	05751h		;PUSH	cx
					;PUSH	di
I_rep		  equ	0F3h		;REP
I_retFAR	  equ	0CBh		;RET	far
I_rolAL_1	  equ	0C0D0h		;ROL	al,1
I_rorAL_1	  equ	0C8D0h		;ROR	al,1

I_shlBL_1	  equ	0E3D0h		;SHL	bl,1
I_SSoverride	  equ	036h		;SS:
I_stosb 	  equ	0AAh		;STOSB
I_stosw 	  equ	0ABh		;STOSW

I_xorAL_Mem	  equ	00632h		;XOR	al,[addr]
I_xorALbyteI	  equ	034h		;XOR	al,byte immediate
I_xorAXwordI	  equ	035h		;XOR	ax,word immediate
I_xorBH_BH	  equ	0FF32h		;XOR	BH,BH

page


;	Plane selection in the driver is based on a bitmask which is
;	shifted left to select the next plane.	This bitmask serves
;	many purposes:
;
;	    a)	When a carry is produced from this bitmask, all three
;		planes have been processed, and it is time to update
;		to the next scan line.	Remember that this BITBLT is
;		scan line oriented; Each plane of a scan line is BLTed
;		before the next scan line is processed.
;
;	    b)	It can be used to generate the read and write plane
;		masks for the EGA.
;
;		Masking the current bitmask with 00000111B gives the
;		value for the EGA's write plane register.  This masked
;		value can also be used to index into a three word table
;		containing the segment selectors for a huge bitmap.
;
;		Shifting the masked value right 1 bit gives the value
;		for the EGA's Read Map register.
;
;		This will not work if all four planes of the EGA are
;		used.  It only works for 1, 2, or 3 planes.
;
;	    c)	When a carry results from the left shift, all three planes
;		of the current scan line have been processed and it is
;		time to update to the next scan line.
;
;
;	Plane1 is the value for selecting plane one.

Plane1		equ	00100001b	;Plane index for plane 1





;	Increase is the flag used to show the the BLT operation will be
;	increasing in Y (Y+).
;
;	Decrease is the flag used to show that the BLT operation will be
;	decreasing in Y (Y-).
;
;	Stepleft is the flag used to show that the BLT will be stepping
;	left (i.e. start at the right hand corner of the source, stepping
;	left, or X-).
;
;	Stepright is the flag used to show that the BLT will be stepping
;	right (i.e. start at the left hand corner of the source, stepping
;	right, or X+).


increase	equ	1		;Incrementing
decrease	equ	-1		;Decrementing
stepleft	equ	0		;Stepping to the left
stepright	equ	1		;Stepping to the right



;	TheFlags
;
;	GagChoke	Set if the source and destination are of different
;			color formats.	When set, some form of color
;			conversion will be required.
;
;			Once you see what all is involved with color
;			conversion, you'll understand why this flag is
;			called this.
;
;	ColorPat	Set if, color pattern fetch code will be used.	If
;			clear, then mono pattern fetch code will be used.
;			Mono/color pattern fetch is always based on the
;			destination being Mono/color (it is the same).
;
;	PatPresent	Set if a pattern is involved in the BLT.
;
;	SrcPresent	Set if a source  is involved in the BLT.
;
;	SrcIsDev	Set if the source is the physical device.  Clear if
;			the source is a memory bitmap.
;
;	SrcIsColor	Set if the source is color, clear if monochrome.
;
;	DestIsDev	Set if the destination is the physical device.
;			Clear if the destination is a memory bitmap.
;
;	DestIsColor	Set if the destination is color, clear if monochrome.


GagChoke	equ	10000000b	;Going mono <==> color
ColorPat	equ	01000000b	;Use color pattern fetch code
PatPresent	equ	00100000b	;Pattern is involved in blt
SrcPresent	equ	00010000b	;Source is involved in blt
SrcIsDev	equ	00001000b	;Source is the device
SrcIsColor	equ	00000100b	;Source is color
DestIsDev	equ	00000010b	;Destination is the device
DestIsColor	equ	00000001b	;Destination is color




;	MooreFlags
;
;	MooreFlags pertain to color conversion only.  If color
;	conversion doesn't apply to the BLT, these flags will not
;	be defined.
;
;
;	RepOK		When RepOK is set, then the innerloop code can
;			use a REP MOVSx instruction.  This will be the
;			case if:
;
;			a)  The source is the EGA and the color compare
;			    register can be used to do the conversion
;			    from color to monochrome.
;
;			b)  The source is monochrome, the background
;			    color white, and the foreground color black,
;			    in which case color converison of the source
;			    would just give the source.
;
;	NoMunge 	Set under the same conditions as "b" above.


RepOK		equ	10000000b	;Using REP is ok (when GagChoke)
NoMunge 	equ	01000000b	;No mono ==> color conversion table
;		equ	00100000b
;		equ	00010000b
;		equ	00001000b
;		equ	00000100b
;		equ	00000010b
;		equ	00000001b

page
sBegin	code
assumes cs,code
assumes ds,nothing
assumes es,nothing
assumes ss,nothing


;	jmpCXnz   - near jump if CX-1 <> 0
;
;	jmpCXnz will skip the following near jump if CX-1 is zero.
;	CX will be left updated by this code.
;
;	jmpCxnz is used by both the inner loop code and the outer
;	loop code if a loop instruction cannot be used.


jmpCXnz:
	dec	cx			;Decrement counter
	jz	$+5
;	jmp	near label
	db	I_JMPnear		;JMP opcode

jmpCXnzLen	equ	$-jmpCXnz	;Length of procedure






;	phaseAlign - phase alignment code
;
;	The following code is the template that performs the phase
;	alignment masking.  The source has already been aligned to
;	the destination.
;
;	A copy of the aligned source is made.  The phase mask is then
;	applied to the source and the copy.  The previously unused
;	bits are ORed into the used bits of the current source, and
;	the unused bits of the current source then become the unused
;	bits for the next source.
;
;
;	It assumes:
;
;		bp  =  phase alignment mask
;		al  =  current byte to mask
;		bh  =  old unused bits

phaseAlign:
	mov	ah,al			;Make a copy of aligned source
	and	ax,bp			;Masked used, unused bits
	or	al,bh			;Mask in old unused bits
	mov	bh,ah			;Save new unused bits

phaseAlignLen	equ	$-phaseAlign	;Length of procedure




;	MaskedStore is a templete for storing the first and last bytes
;	of a BLT.  The unaltered bits are saved and the altered bits
;	set in the byte, then the byte is stored.
;
;
;	It assumes:
;
;		al  =  The byte to be BLTed to the destination bitmap.
;		       All logic operations have been performed on this
;		       byte.
;
;		ah  =  The destination byte.
;
;	The AND immediate will be fixed up.


maskedStore:
	and	ax,0FFFFH		;Mask altered/unaltered bits
	or	al,ah			;Combine the bits
	stosb				;And store the result

maskedStoreLen	equ	$-maskedStore	;Length of the template
maskedStoreMask equ	-5		;Offset to where mask goes
page


;	Pattern Fetch Code
;
;	The pattern fetch code will be created on the fly since
;	most of the instructions need fixups.
;
;	This template is really just a comment to indicate what
;	the generated code should look like.
;
;	Entry:	None
;
;	Exit:	dh = pattern
;
;	Uses:	ax,bx,cx,dh,flags
;
;
;	The following registers are available to the pattern fetch
;	logic (as implemented herein):
;
;		ax,bx,cx,dx,flags
;
;
;	For monochrome brushes:
;
;	    mov     ax,1234h		;Load segment of the brush
;	    mov     bx,1234h		;Load offset of the brush
;	    mov     cx,ds		;Save DS
;	    mov     ds,ax		;ds:bx --> brush
;	    mov     dh,7[bx]		;Get next brush byte
;	    mov     al,cs:[1234h]	;Get brush index
;	    add     al,direction	;Add displacement to next byte (+1/-1)
;	    and     al,00000111B	;Keep it in range
;	    mov     cs:[1234h],al	;Store displacement to next byte
;	    mov     ds,cx		;Restore ds
;
;
;	For color brushes:
;
;	    mov     ax,1234h		;Load segment of the brush
;	    mov     bx,1234h		;Load offset of the brush
;	    mov     cx,ds		;Save DS
;	    mov     ds,ax		;ds:bx --> brush
;	    mov     dh,7[bx]		;Get next brush byte
;	    mov     al,cs:[1234h]	;Get brush index
;	    add     al,SIZE Pattern	;Add displacement to next plane's bits
;	    and     al,00011111B	;Keep it within the brush
;	    mov     cs:[1234h],al	;Store displacement to next plane's bits
;	    mov     ds,cx		;Restore ds
;
;
;	For both templates, cs:[1234] is the address of the 7 in the
;	"mov dh,7[bx]" instruction.  This is the index to this scan's
;	bit pattern in the brush.  This value will range from 0 to
;	(SIZE pattern)-1 for monochrome devices, and from 0 to
;	((NumberPlanes)*(SIZE pattern))-1 for color devices.
;
;	For color brushes, cs:[1234] must also be fixed up when the next
;	scan line is selected, else it would index into the monochrome
;	portion of the brush (e.g. 1,9,17,25, where 25 is not part of the
;	color brush).
page

;	Color ==> Mono Fetch Code
;
;	The color ==> mono fetch code is generated when the source
;	is a color bitmap and the destination is monochrome.  If the
;	source is the screen and the destination is monochrome, the
;	EGA's color compare register will be used instead of this code.
;
;	The code maps 1 bits to the background color that was passed
;	in, and 0 bits to the foreground color.  It does it by XORing
;	each byte with the background color for that plane (00 or FF),
;	and then ORing the results.  The result of the ORs is then
;	complemented, giving 1's wherever the background color was
;	found, and 0's wherever the background color wasn't found.
;
;	This template is really just a comment to indicate what
;	the generated code should look like.
;
;	The following registers are available to the source fetch
;	logic (as implemented herein):
;
;	    ah,al,ds
;
;
;	Exit:	al  =  source byte
;		si --> next source byte
;
;
;	For bitmaps:
;
;	    mov     al,NextPlane[si]	    ;Get green byte of source
;	    mov     ah,2*NextPlane[si]	    ;Get blue  byte of source
;	    xor     ax,Green+(Blue*256)     ;XOR with plane's color
;	    or	    ah,al		    ;OR the result
;	    lodsb			    ;Get red source
;	    xor     al,RedColor 	    ;XOR with red color
;	    or	    al,ah		    ;OR with previous result
;	    not     al			    ;NOT to give 1's where background
;
;
;	    where NextPlane is defined to be:
;
;		a)  bmWidthPlane for bitmaps <64K
;		b)  bmWidthBytes for bitmaps >64K
page

;	Mono ==> Color Fetch Code
;
;	The mono ==> color fetch code is generated when the source
;	is a monochrome bitmap and the destination is color.
;
;	When going from mono to color, 1 bits are considered to be
;	the background color, and 0 bits are considered to be the
;	foreground color.
;
;	For each plane:
;
;	  If the foreground=background=1, then 1 can be used in
;	  place of the source.
;
;	  If the foreground=background=0, then 0 can be used in
;	  place of the source.
;
;	  If the foreground=0 and background=1, then the source
;	  can be used as is.
;
;	  If the foreground=1 and background=0, then the source
;	  must be complemented before using.
;
;
;	A table will be created for processing the monochrome
;	bitmap for each plane of the destination.  The table
;	should look like:
;
;	     BackGnd   ForeGnd	  Result    AND  XOR
;		1	  1	    1	     00   FF
;		0	  0	    0	     00   00
;		1	  0	    S	     FF   00
;		0	  1	not S	     FF   FF
;
;	From this, it can be seen that the XOR mask is the same as the
;	foreground color.  The AND mask is the XOR of the foreground
;	and the background color.
;
;	It can also be seen that if the background color is white and the
;	foreground (text) color is black, then the conversion needn't be
;	generated (it just gives the source).
;
;	The template for rotating the AND and XOR table for the plane
;	select code is also shown.  It just does a three word rotate
;	on the AND and XOR masks on the stack.	It is performed at the
;	end of a scan in anticipation of the next color for that scan.
;
;
;	    lodsb			    ;Get next byte of source
;	    and     al,byte ptr ss:[1234h]  ;Process against current AND
;	    xor     al,byte ptr ss:[1234h]  ;  and XOR masks


RotANDXOR:
;	lea	bp,ColorMungeTBl	    ;--> AND/XOR masks
	mov	ax,word ptr 4[bp]	    ;Rotate next color's AND and
	xchg	ax,word ptr 2[bp]	    ;  XOR mask into palce
	xchg	ax,word ptr [bp]
	mov	word ptr 4[bp],ax

LenRotANDXOR	equ	$-RotANDXOR
page

;	Plane Looping Logic
;
;	The plane looping logic is copied into any color destination
;	BLT.  It shifts the plane selection mask to the next plane,
;	and if it is still valid, jumps to the start of the blt code.
;
;	The plane selection mask will generate a carry when shifted
;	after the last plane has been processed for the current scan.
;
;	Two different jumps could be generated depending on the
;	displacement of the jump.
;
;	This template is really just a comment to indicate what
;	the generated code should look like.
;
;	Entry:	bl = plane select mask
;
;	Exit:	bl = plane select mask
;
;	Uses:	bl,flags
;
;	    shl     bl,1	    ;Set next scanline?
;	    jc	    $+5 	    ;  Yes
;	    jmp     StartOfLoop     ;  No, still have some planes left
;
;		or
;
;	    shl     bl,1	    ;Set next scanline?
;	    jnc     StartOfLoop     ;  No overflow yet
page

;	The following two bit mask tables are used for fetching
;	the first and last byte used-bits bitmask.


		public	bitmaskTBL1
bitmaskTBL1	label	byte
	db	11111111B		;Masks for leftmost byte
	db	01111111B
	db	00111111B
	db	00011111B
	db	00001111B
	db	00000111B
	db	00000011B
	db	00000001B


		public	bitmaskTBL2
bitmaskTBL2	label	byte
	db	10000000B		;Masks for rightmost byte
	db	11000000B
	db	11100000B
	db	11110000B
	db	11111000B
	db	11111100B
	db	11111110B
	db	11111111B



;	PhaseTBL1 is used for loading the "used" bits and "saved" bits
;	bitmasks for cases 1,2,3 where the step direction is left to
;	right.	If it weren't for the case of zero, this could be done
;	with a simple rotate of 00FF.	For cases 4,5,6, a simple rotate
;	can create the mask needed.

phaseTBL1	label	word			;Yes, words!
	db	11111111B,00000000B		;Used bits, saved bits
	db	00000001B,11111110B
	db	00000011B,11111100B
	db	00000111B,11111000B
	db	00001111B,11110000B
	db	00011111B,11100000B
	db	00111111B,11000000B
	db	01111111B,10000000B
page

;	OpTbl contains templates of the actions to be performed for the
;	current logic operation when the action portion of the BLT is
;	being created.
;
;	The table is based on a combination of the LogOp bits and the
;	operand bits.  The LogOp is used as d4:d3 of the table index,
;	and the operand is used as d2:d1 of the table index.
;
;	The exception to this is the operand "D" when the destination
;	is in memory instead of the AH registed.  In this case, the
;	segment override is prestuffed, and the LogOp is used for
;	d2:d1 of the table index with d4:d3 set to zero (this space
;	is available in the table since NOT always works with whatever
;	happens to be in AL).
;
;	The table layout is very dependant on the definition of the
;	LogOps and Operands, and there are many errnz checks following
;	to ensure that assumptions are correct.
;
;	Also, all entries in this table must be two bytes long, and some
;	attempt will be made to check for this with an errnz, but it may
;	not hold true in all cases if any instruction is changed.

OpTbl	label	word

	not	al			;Complement
	xor	al,[di] 		;XOR with destination (es:[di])
	or	al,[di] 		;OR  with destination (es:[di])
	and	al,[di] 		;AND with destination (es:[di])


	xor	al,bl			;XOR with "PUSH"ed value
	xor	al,dl			;XOR with source
	xor	al,ah			;XOR with destination
	xor	al,dh			;XOR with pattern


	or	al,bl			;OR with "PUSH"ed value
	or	al,dl			;OR with source
	or	al,ah			;OR with destination
	or	al,dh			;OR with pattern


	and	al,bl			;AND with "PUSH"ed value
	and	al,dl			;AND with source
	and	al,ah			;AND with destination
	and	al,dh			;AND with pattern


	errn$	OpTbl,-32		;Should be 32 bytes long
	errnz	OpSpec			;Index d2:d1
	errnz	OpSrc-1
	errnz	OpDest-2
	errnz	OpPat-3
	errnz	LogNOT			;Index d4:d3 unless dest not in reg,
	errnz	LogXOR-1		;  then index d2:d1
	errnz	LogOR-2
	errnz	LogAND-3
page

;	Generate the parse strings.  Some of the strings will have special
;	values.  These are the special cased string (index of 0), and the
;	strings with pushes/pops (indexes >= SpecParseStrIndx)

parseStrings	label	word


SpecParseStrIndx equ	($-parseStrings)/2	;Special cased strings
	dw	parseStr0


	dw	parseStr1			;General parse strings
	dw	parseStr2
	dw	parseStr3
	dw	parseStr4


cmplxParseStr	equ	($-parseStrings)/2	;Complex parse strings (they
	dw	parseStr5			;  contain push/pop sequence)
	dw	parseStr6
	dw	parseStr7
