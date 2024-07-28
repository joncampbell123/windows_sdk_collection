        page    ,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	PIXEL.ASM
;
; This module contains the Set/Get Pixel routine.
;
; Created: 22-Feb-1987
; Author:  Walt Moore [waltm]
; Modified for Hires drivers by Fred Einstein [fredei]  4-Nov-1987
;
; Copyright (c) 1984-1987 Microsoft Corporation
;
; Exported Functions:	Pixel
;
; Public Functions:	none
;
; Public Data:		none
;
; General Description:
;
;   Pixel is used to either set a pixel to a given color with the
;   current binary raster operation, or to return the color of the
;   the pixel at the given location.
;
; Restrictions:
;
;-----------------------------------------------------------------------;
.286c

incDrawMode	= 1			;Include control for gdidefs.inc

	.xlist
	include cmacros.inc
	include gdidefs.inc
	include display.inc
	include macros.mac
        .list

	??_out	Pixel

BITSPIXEL	equ	8

subttl          Data Segment Definitions
page +
sBegin          data
externB         Rop2TranslateTable              ;in INIT.ASM
externB         ColorFlagTable                  ;in PALETTE.DAT
sEnd            data

subttl          Code Segment Definitions
page +
sBegin          code
assumes         cs,code
externFP        CursorExclude                   ;in ROUTINES.ASM
externFP        CursorUnExclude                 ;in ROUTINES.ASM
sEnd            code


subttl          Pixel Code Segment Definitions
page +
createSeg   _DEIGHT, DynamicEight, word, public, CODE
sBegin	    DynamicEight

assumes cs, DynamicEight
assumes ds, nothing

.xlist  
include         8514.INC
.list

externW 	DrawModeTbl8			;in output8.asm

public	rot_bit_tbl8
rot_bit_tbl8	label	byte
		db	10000000b	;Table to map bit index into
		db	01000000b	;  a bit mask
		db	00100000b
		db	00010000b
		db	00001000b
		db	00000100b
		db	00000010b
		db	00000001b
page

;--------------------------Exported-Routine-----------------------------;
; Pixel
;
;   Set or Get a Given Pixel
;
;   The given pixel is set to the given color or the given pixel's
;   physical color is returned.
;
;   The physical device may be the screen or a monochrome bitmap.
;
;   If lp_draw_mode is NULL, then the physical color of the pixel is
;   returned.  If lp_draw_mode isn't NULL, then the pixel will be set
;   to the physical color passed in, combined with the pixel already
;   at that location according to the raster-op in lp_draw_mode.  Pixel
;   doesn't pay attention to the background mode.
;
;   No clipping of the input value is required.  GDI clips the
;   coordinate before it is passed in, for both Set and Get.
;
; Entry:
; Returns:
;	DX:AX = physical color if get pixel
;	DX:AX = positive if no error and set was OK.
; Error Returns:
;	DX:AX = 8000:0000H if error occured
; Registers Preserved:
;	SI,DI,DS,BP
; Registers Destroyed:
;	AX,BX,CX,DX,ES,FLAGS
; Calls:
;	exclude_far
;	unexclude_far
; History:
;	Sat 31-Oct-1987 00:21:06 -by-  Walt Moore [waltm]
;	Added clipping of the (X,Y)
;
;	Tue 18-Aug-1987 14:50:37 -by-  Walt Moore [waltm]
;	Added test of the disabled flag.
;
;	Tue 03-Mar-1987 20:42:07 -by-  Kent Settle [kentse]
;	Moved a mov instruction in EGA ROP handling code.
;
;	Sun 22-Feb-1987 16:29:09 -by-  Walt Moore [waltm]
;	Created.
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


        assumes ds,nothing
	assumes es,nothing


cProc	Pixel8, <FAR,PUBLIC,WIN,PASCAL>,<si,di>

        parmD   lpDevice                ;Pointer to device
        parmW   x                       ;X coordinate of pixel
        parmW   y                       ;Y coordinate of pixel
        parmD   PhysColor               ;Physical color to set pixel to
        parmD   lpDrawMode              ;Drawing mode to use, or null if Get

        localW  Rop2Index               ;
        localB  ColourFormat            ;from the PDEVICE structure

cBegin	<nogen>
        lds     si,lpDevice             ;--> physical device
        cmp     word ptr [si],0         ;is this a main memory bitmap?
        je      PXMemory                ;yes, go do main memory pixel operation
	cCall	BoardPixelOperation	;no, go do it on the device
	jmp	MemorySetMonoBit	;Go set the mono bit in ah and exit.
;
;
subttl  Memory Pixel Operations
page +       
PXMemory:
        mov     cl,[si+9]               ;save the colour format locally
        mov     ColourFormat,cl         ;
;
;	If the X or Y coordinate is outside the surface of the bitmap
;       return an error code of 8000:0000.  The test will
;	be performed as an unsigned compare to test for the range of
;	0:n-1.
;                                                                              
	mov	cx,x
	cmp	cx,[si].bmWidth
	jae	pixel_clipped
	mov	cx,y
	cmp	cx,[si].bmHeight
        jb      pixel_20
;             
pixel_clipped:       
        mov     dx,8000h                ;show an error
        xor     ax,ax                   ;
        jmp     pixel_exit              ;and get out now

;       If it is a huge bitmap, special processing must be 
;       performed to compute the Y address.

pixel_20:
	mov	ax,y			;Need Y coordinate a few times
	xor	dx,dx			;Set segment bias to 0
	mov	cx,[si].bmSegmentIndex	;Is this a huge bitmap?
	jcxz	pixel_40		;  No



;	This is a huge bitmap.	Compute which segment the Y coordinate
;	is in.	Assuming that no huge bitmap will be bigger than two
;	or three segments, iteratively computing the value would be
;	faster than a divide, especially if Y is in the first segment
;	(which would always be the case for a huge color bitmap that
;	didn't have planes >64K).


	mov	bx,[si].bmScanSegment	;Get # scans per segment

pixel_30:
	add	dx,cx			;Show in next segment
	sub	ax,bx			;See if in this segment
        jnc     pixel_30                ;Not in current segment, try next
	add	ax,bx			;Restore correct Y
	sub	dx,cx			;Show correct segment


;	This is a memory DC.  If this is a monochrome memory DC, set up
;	the inner loop so that it will terminate after one time through
;	and set the color to be equal to the mono bit in the physical
;	color.
;
;	Currently:
;		AX     =  Y coordinate
;		DX     =  Segment bias for huge bitmaps
;		DS:SI --> PDevice

pixel_40:
	mov	bx,[si].bmWidthBytes	;Get Y multiplier
	add	dx,wptr [si].bmBits[2]	;Compute segment of the bits
	mov	si,wptr [si].bmBits[0]	;Get offset of the bits
	mov	ds,dx			;Set DS:SI --> to the bits
	assumes ds,nothing

	mul	bx			;Compute start of scan
        add     si,ax                   ;DS:SI --> start of scanline byte is in
;
;	Currently:
;		DS:SI --> the bitmap, start of the correct scan

pixel_70:
	mov	ax,x			;Get X coordinate
        mov     bx,ax                                           
        cmp     ColourFormat,8          ;are we in colour?
        je      pixel_71                ;yes, skip the divide and AND
        shr     ax,3                    ;Compute byte offset from start of scan
	and	bx,00000111B		;Get bit mask for bit
	mov	ch,rot_bit_tbl8[bx]

;
pixel_71:
        add     si,ax                   ;ds:si --> byte of pixel
        mov     di,off_lpDrawMode       ;If a drawmode was given
        mov     bx,seg_lpDrawMode       ;  then set the pixel, else
	mov	ax,bx			;  return it's physical color
	or	ax,di
        jnz     pixel_100               ;Given, operation is set pixel
        jmp     short pixel_200         ;Not given, return pixel color
;
subttl  Memory SetPixel Operation
page

;	The operation to be performed is SetPixel.  Currently:
;
;               CH    =   bit mask (if monchrome) (FF if colour)
;               DS:SI --> byte that pixel is to be set in
;		BX:DI --> physical drawing mode

pixel_100:
	mov	es,bx			;es:di --> drawmode
	assumes es,nothing

	mov	bx,es:[di].Rop2 	;Get drawing mode to use
	dec	bx			;Make it zero based
        and     bx,000Fh                ;Keep it valid

;
;	The SetPixel operation is to be performed on a memory bitmap.
;
;
;	    AH	Inverse of the bitmask
;	    AL	The bitmask
;	    BX	Work
;	    CH	Pen Color
;	    CL	mask for getting pixel color
;	 DS:SI	destination pointer
;	    DI	ROP table address for the rop
;

        shl     bx,1                    ;Set rop_indexes table address
        mov     Rop2Index,bx            ;(save it for colour)
	lea	di,cs:rop_indexes[bx]	;  for the given rop
        mov     ah,ch                   ;Get mask for bit(s) being altered
	mov	al,ah			;  and also create a mask for
        not     ah                      ;  ANDing                

        mov     bx,off_PhysColor        ;Get color for the pixel into BL
        cmp     ColourFormat,8          ;are we in colour?
        je      MemorySetColourPixel    ;yes, go do it for colour

MemorySetMonoPixel:

;If we're in monochome, the colour will be either 0 or FF.  In order for the
;ROP calling procedure to work, we must have BX = 0 for black and BX = 1 for
;white.

	and	bx, 100h		;mask all but "intensity bit" in BH
	xchg	bl, bh			;BX: 0-->black, 1-->white
	mov	bl,bptr cs:[di][bx]	;Get delta to the drawing function
	add	bx,DynamicEightOFFSET pixel_base_address
        jmp     bx                      ;Invoke the function             

MemorySetColourPixel:
        mov     al,bl                   ;get the pen colour into AL
        mov     ah,[si]                 ;get the pixel to be acted upon
        mov     bx,Rop2Index            ;get the offset into DrawModeTbl for
                                        ;desired ROP2
	add	bx, DynamicEightOFFSET DrawModeTbl8
                                        ;add on base of table
        call    cs:[bx]                 ;go do the Rop2
        mov     [si],al                 ;and put the completed byte into the
                                        ;bitmap
        jmp     short pixel_is_dest     ;go do finishing up code

pixel_base_address:			;Deltas are computed from here
pixel_is_0:
	and	[si],ah 		;Set pixel to a 0
	jmpnext

pixel_is_inverted:
	xor	[si],al 		;Invert destination
	jmpnext

pixel_is_1:
	or	[si],al 		;Set pixel to a 1
	jmpnext stop

pixel_is_dest:
	xor	ax,ax			;Set dx:ax = 0:0
	cwd
        jmp     short pixel_exit        ;Return 0:0 to show success
;
;
subttl  Memory GetPixel Operation
page

;	The operation to be performed is get pixel.  The color of the
;       pixel will be returned.
;
;	Currently:
;
;		CH    =   bit mask
;		DS:SI --> byte bit is to be set in


WHITE	equ	C0_BIT+C1_BIT+C2_BIT+C3_BIT+MONO_BIT+ONES_OR_ZEROS
COLORED equ     ONES_OR_ZEROS

pixel_200:
        cmp     ColourFormat,8          ;are we in colour?
        je      MemoryGetColourPixel    ;yes, go do it
;
MemoryGetMonoPixel:
        xor     ax,ax                   ;Assume pixel is black
        cwd                             ;
	test	[si],ch 		;Is pixel black?
        jz      short MemorySetMonoBit  ;  It is, return color in AX:DX
        dec     al                      ;  No, set AX:DX for white
        jmp     short MemorySetMonoBit  ;and leave
;
MemoryGetColourPixel:
        xor     ax,ax                   ;clear AX & DX
        cwd                             ;
        mov     al,[si]                 ;get the pixel's colour and we're done!
;
MemorySetMonoBit:
	xor	ah,ah			;Assume mono bit is off.
	mov	bl,al			;al = physical index.
	cmp	bl,10			;Low 10 colors? 
	jb	short @f		;Yes.  Go look up mono bit.
	cmp	bl,246			;Non-system color?
	jb	short pixel_exit	;Yes. Map these to black.
	sub	bl,236
@@:	mov	si,DGROUP		;make ds:si --> ColorFlagTable
	mov	ds,si
	mov	si,DataOFFSET ColorFlagTable
	xor	bh,bh
	mov	ah,[si][bx]		;ah: mono accel. bit
	shr	ah,4
pixel_exit:
cEnd
;
;
subttl          8514 Pixel Operations
page +
assumes         ds,data
cProc		BoardPixelOperation, <NEAR>
cBegin             
mov             ax,seg Rop2TranslateTable
mov             ds,ax                   ;reestablish pointer to Data in DS
;
;       Exclude the cursor from the drawing area:
;
mov             ax,X                    ;get our X coordinate
sub             ax,5                    ;exclude within 5 pixels around it
push            ax                      ;pass it
mov             ax,Y                    ;get our Y coordinate
sub             ax,5                    ;exclude within 5 raster lines around it
push            ax                      ;pass it
mov             ax,X                    ;
add             ax,5                    ;
push            ax                      ;pass it
mov             ax,Y                    ;
add             ax,5                    ;
push            ax                      ;now we're ready to do the exclusion
cCall           CursorExclude           ;go for it
;
;       Now see whether we are to set a pixel on the 8514 or retrieve a
;pixel from the 8514:
;                
cmp             SEG_lpDRAWMODE,0        ;is DRAWMODE null?     
jne             BPO_SET_PIXEL           ;nope, go set a pixel on board
;                                                                       
BPO_GET_PIXEL:  
CheckFIFOSpace  SIX_WORDS
;
;Set read-plane enable to all on:
;                                 
mov             ax,0ffh                 ;
mov             dx,READ_ENABLE_PORT     ;
out             dx,ax                   ;
;
;Set the extents on board:
;                            
xor             ax,ax                   ;read a 1 x 1 rectangle
mov             dx,RECT_WIDTH_PORT      ;
out             dx,ax                   ;
mov             dx,RECT_HEIGHT_PORT     ;
out             dx,ax                   ;
;
;Set the coordinates to read from:
;
mov             ax,X                    ;set the X-coordinate
mov             dx,Srcx_PORT            ;
out             dx,ax                   ;
mov             ax,Y                    ;get the Y-coordinate
mov             dx,Srcy_PORT            ;
out             dx,ax                   ;                          
mov             ax,3318h                ;get command to send
mov             dx,COMMAND_FLAG_PORT    ;
out             dx,ax                   ;
;
BPO_GP_1:
mov             dx,9ae8h                ;get status to see if there's data to
in              ax,dx                   ;read from the board
and             ah,1                    ;is there data available?
jz              BPO_GP_1                ;nope, keep waiting
mov             dx,PATTERN_DEFINE_PORT  ;set DX to variable data port
in              ax,dx                   ;get the pixel into AL
mov             bl,al                   ;get it into BL for return
jmp short       BPO_EXIT                ;and get out
;
BPO_SET_PIXEL:
MakeEmptyFIFO
les             di,lpDRAWMODE           ;get pointer to DRAWMODE into ES:DI
mov             al,es:[di+0]            ;get ROP2 value into AL
mov             cx,OFF_PhysColor        ;get physical colour into CL
;
;Now translate the MS-WINDOWS ROP2 code into the 8514 function:
;
dec             al                      ;make ROP2 offset from 0
mov             bx,dataOFFSET Rop2TranslateTable
xlat            Rop2TranslateTable      ;now AL has the 8514 function
or              al,20h                  ;make function 1 look at foreground
mov             dx,FUNCTION_1_PORT      ;
out             dx,ax                   ;
;                                                                   
;Now, set the colour:
;
mov             ax,cx                   ;get colour into AX
mov             dx,COLOUR_1_PORT        ;
out             dx,ax                   ;
;                                        
;Set the mode to solid colour:
;
mov             dx,MODE_PORT            ;
mov             ax,0a000h               ;this is "normal mode"
out             dx,ax                   ;
;
;Lastly, blop the pixel out onto the screen:
;
mov             ax,X                    ;set X-coordinate
mov             dx,Srcx_PORT            ;
out             dx,ax                   ;
mov             ax,Y                    ;set Y-coordinate
mov             dx,Srcy_PORT            ;
out             dx,ax                   ;
xor             ax,ax                   ;X & Y extents are always 1
mov             dx,RECT_WIDTH_PORT      ;     
out             dx,ax                   ;
mov             dx,RECT_HEIGHT_PORT     ;
out             dx,ax                   ;
mov             ax,40f3h                ;this is command to draw rectangle
mov             dx,COMMAND_FLAG_PORT    ;
out             dx,ax                   ;                        
xor             bl,bl                   ;make sure return value = 0
;                                                                  
BPO_EXIT:
cCall           CursorUnExclude         ;go free the exclusion area
xor             ax,ax                   ;clear AX & DX
cwd                                     ;
mov             al,bl                   ;set pixel colour into AL
cEnd
;
;
subttl          ROP Macros and Tables
page +
;	make_rop - Make a ROP For Pixel
;
;	MakeROP makes a raster operation for the pixel routine.
;	The raster operation generated is based on the following
;	table which shows the ROP broken down into the boolean
;	result for each plane based on what the pen color is for
;	the plane.
;
;
;
;		Color	  Result			Color	  Result
;
;	DDx	  0	    0			DPa	  0	    0
;		  1	    0				  1	   dest
;
;	DPon	  0	  ~dest 		DPxn	  0	  ~dest
;		  1	    0				  1	   dest
;
;	DPna	  0	   dest 		D	  0	   dest
;		  1	    0				  1	   dest
;
;	Pn	  0	    1			DPno	  0	    1
;		  1	    0				  1	   dest
;
;	PDna	  0	    0			P	  0	    0
;		  1	  ~dest 			  1	    1
;
;	Dn	  0	  ~dest 		PDno	  0	  ~dest
;		  1	  ~dest 			  1	    1
;
;	DPx	  0	   dest 		DPo	  0	   dest
;		  1	  ~dest 			  1	    1
;
;	DPan	  0	    1			DDxn	  0	    1
;		  1	  ~dest 			  1	    1


make_rop macro	l,ops

irp x,<ops>
ifidn	<&&x>,<0>
	db	pixel_is_0-pixel_base_address
endif
ifidn	<&&x>,<1>
	db	pixel_is_1-pixel_base_address
endif
ifidn	<&&x>,<~dest>
	db	pixel_is_inverted-pixel_base_address
endif
ifidn	<&&x>,<dest>
	db	pixel_is_dest-pixel_base_address
endif
endm
endm


rop_indexes	label	byte
make_rop DDx,<0,0>
make_rop DPon,<~dest,0>
make_rop DPna,<dest,0>
make_rop Pn,<1,0>
make_rop PDna,<0,~dest>
make_rop Dn,<~dest,~dest>
make_rop DPx,<dest,~dest>
make_rop DPan,<1,~dest>
make_rop DPa,<0,dest>
make_rop DPxn,<~dest,dest>
make_rop D,<dest,dest>
make_rop DPno,<1,dest>
make_rop P,<0,1>
make_rop PDno,<~dest,1>
make_rop DPo,<dest,1>
make_rop DDxn,<1,1>

sEnd	DynamicEight
end
