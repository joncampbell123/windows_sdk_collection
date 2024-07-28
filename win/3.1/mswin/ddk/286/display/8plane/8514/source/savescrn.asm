page            ,132
title           Save Screen Bitmap in Unused Memory
.286c


.xlist
include CMACROS.INC
include 8514.INC
include savescrn.inc
include gdidefs.inc
.list

subttl          Data Area
sBegin          Data

externW 	FreeSpaceyOrg		;in INIT.ASM
externW 	FreeSpaceyExt		;in INIT.ASM

public	ShadowMemoryTrashed		;=1--> SaveScreenBitmap is valid
ShadowMemoryTrashed	db	0	;see note at SSB_RESTORE

public	SaveInUse
SaveInUse		db	0	;FF when save area is in use

public	SaveSpaceyOrg
SaveSpaceyOrg		dw	0

public	SaveSpaceyExt
SaveSpaceyExt		dw	0

SaveRect		db	(size RECT) dup(0)
bSaveRem		db	0
bSaveBands		db	0
sEnd            Data


subttl          Code Area
page +
sBegin          Code
assumes         cs,Code
assumes         ds,Data

externFP        <CursorExclude,CursorUnExclude> ;in ROUTINES.ASM

cProc	SaveScreenBitmap,<FAR,PUBLIC,WIN,PASCAL>,<si,di>
        parmD   lpRect
        parmW   Command

        localW  SrcxOrg
        localW  SrcyOrg
        localW  DstxOrg
        localW  DstyOrg
        localW  xExt
	localW	yExt
	localW	<wSrcXUpdate, wSrcYUpdate, wDstXUpdate, wDstYUpdate>
	localV	LocalRect, %(size RECT)
cBegin

;First, see if he wants us to do anything:

	mov	cx,Command		;get the passed command
	cmp	cx,2			;is this a CANCEL or a bad command?
	jb	SSB_GET_RECT		;no, go do a save or restore
	ja	SSB_ERROR_EXIT_NODE	;yep, get out

; At this point we are asked to clear the off-screen memory that was used to
; store a bitmap.


	test	SaveInUse, SAVEBITMAPCANCELED
	jnz	JustSetFlags
	mov	ax, SaveSpaceyOrg
	cmp	ax, 0300h
	jb	JustSetFlags
	test	ShadowMemoryTrashed, 01h
	jz	JustSetFlags
	mov	FreeSpaceyOrg, ax	;save operation was aborted.  Restore
	mov	ax, SaveSpaceyExt	;off-screen memory save area
	add	FreeSpaceyExt, ax
JustSetFlags:
	mov	SaveInUse, SAVEBITMAPCANCELED
	sub	ax, ax
	and	byte ptr [ShadowMemoryTrashed], 0feh
	mov	SaveSpaceyExt, ax
	mov	bSaveBands, al
SSBGoodExitNode:
	jmp	SSBGoodExit		;and return success

SSB_ERROR_EXIT_NODE:
	jmp	SSB_ERROR_EXIT

public  SSB_GET_RECT
SSB_GET_RECT:

;Next, get the rectangle coordinates locally:

	les	si,lpRect		;ES:SI-->pointer to rectangle
	assumes es, nothing
	lods	word ptr es:[si]	;get the left coordinate
	mov	LocalRect.left,ax
	lods	word ptr es:[si]	;get the top coordinate
	mov	LocalRect.top,ax
	lods	word ptr es:[si]	;get the right coordinate
	mov	LocalRect.right,ax
	sub	ax,LocalRect.left	;get xExt
	mov	xExt,ax
	lods	word ptr es:[si]	;get the bottom coordinate
	mov	LocalRect.bottom,ax
	sub	ax,LocalRect.top	;get yExt
	mov	yExt,ax
	dec	cx
	jcxz	SSB_RESTORE
	jmp	SSB_SAVE		;if command = 0, save the area

public  SSB_RESTORE
SSB_RESTORE:                                            

;It is a well known fact that other areas need to utilise off-screen memory
;for their bitmap manipulations.  If they trash our save area, they MUST
;(and will) set the ShadowMemoryTrashed flag to tell us that the saved
;bitmap is no longer valid.  In that case,  we will return an AX = 0 error code
;to GDI and get out.  We reset this flag whenever we save a new bitmap.

	test	SaveInUse, SAVEBITMAPUSED;are we being mistakenly called?
	jz	SSB_ERROR_EXIT		;yes, get out
	test	SaveInUse, SAVEBITMAPCANCELED
	jnz	SSB_ERROR_EXIT
	cmp	bSaveBands, 0
	je	SSB_ERROR_EXIT
	mov	si, DataOFFSET SaveRect
	lea	di, LocalRect
	push	ss
	pop	es			;make ES==SS
	mov	cx, 4			;are we dealing with the same rectangle
repe	cmpsw
	or	cx, cx
	jnz	SSB_ERROR_EXIT		;nope, leave in disgrace

;Now, see whether we still have the saved area available to us:

	test	byte ptr [ShadowMemoryTrashed], 01h
					;has our save area been trashed?
	jz	SSB_ERROR_EXIT		;yes, leave in disgrace

;Now, de-allocate the space that was reserved during the save process.

	mov	ax, SaveSpaceyExt
	add	FreeSpaceyExt,ax
	and	ShadowMemoryTrashed, 0feh
	sub	ax, ax			;need to set a few variables to zero
	mov	SaveInUse, al		;show nothing is saved any more
	mov	wSrcYUpdate, ax 	;don't change src Y pos in banding loop
	mov	wDstXUpdate, ax 	;don't change dst X pos in banding loop
	mov	SaveInUse, al		;clear "save in use" flag
	mov	SaveSpaceyExt, ax	;clear save space Y extent
	mov	SrcxOrg, ax		;we always start saving at X=0
	mov	ax,SaveSpaceyOrg	;get place where we've saved to
	mov	FreeSpaceyOrg,ax	;free this up for use by others
	mov	SrcyOrg,ax		;save it for board operations

	mov	ax,LocalRect.left	;destination coordinates are the
	mov	DstxOrg,ax		;passed coordinates
	mov	bx,LocalRect.top
	mov	DstyOrg,bx
	mov	al, bSaveBands
	sub	ah, ah
	mov	si, ax
	mov	cl, bSaveRem
	sub	ch, ch
	mov	ax, xExt
	mov	wSrcXUpdate, ax
	mov	wDstYUpdate, 192
	jmp	SSB_COMMON		;go copy rectange

public  SSB_ERROR_EXIT
SSB_ERROR_EXIT:
	xor	ax,ax			;otherwise, return an error
	jmp	SSB_END 		;and leave in disgrace

public  SSB_GOOD_EXIT
SSB_GOOD_EXIT:
	jmp	SSBGoodExit		;and leave now

public  SSB_SAVE
SSB_SAVE:
	mov	cx, ax			;assume no banding needed
	mov	si, 1
	mov	bx, ax			;BX: yExt
	test	SaveInUse, SAVEBITMAPUSED   ;is our save area used already?
	jnz	SSB_ERROR_EXIT		;yes, get out with an error
	cmp	ax,FreeSpaceyExt	;is area too big?
	jbe	SSB_SAVE_OK		;no, it fits fine
	cmp	ax, 192 		;do we need to de-allocate bigfonts?
	jbe	SSB_FREE_SAVE_AREA	;yes, and try banding

	mov	cx, 192 		;CX: height of off-screen area
	sub	dx, dx			;for divide
	div	cx			;compute # of bands needed
	mov	cx, dx			;save remainder in CX
	neg	dx			;set CY if rem. non-zero
	adc	ax, 0			;include rem in # of bands needed
	mov	si, ax			;save # of bands needed in SI
	mul	xExt			;DX:AX total X ext needed
	neg	dx			;is DX non-zero
	jnz	SSB_ERROR_EXIT		;yes. It will not fit off screen
	cmp	ax, X_SIZE		;will it fit into off-screen memory?
	ja	SSB_ERROR_EXIT		;no.
SSB_FREE_SAVE_AREA:
	mov	[FreeSpaceyExt], 192	;deallocate all off-screen memory
	mov	[FreeSpaceyOrg], Y_SIZE+64  ;reset off-screen freespace
	mov	[ShadowMemoryTrashed], 1    ;hog off-screen for screen save

public  SSB_SAVE_OK
SSB_SAVE_OK:
	push	ds
	pop	es			;make ES: Data
	assumes es, Data
	mov	di, DataOFFSET SaveRect ;save the coordinates of the rect to
	mov	dx, si			;save band count in DX
	mov	ax, cx			;save remainder in AX
	mov	bSaveBands, dl		;save rem and band count for restore
	mov	bSaveRem, al
	mov	cx, 4
	lea	si, LocalRect
rep	movs	word ptr es:[di], word ptr ss:[si]
	mov	cx, ax
	mov	si, dx

	mov	ax, 192 		;AX: height of save area, BX: X extent
	sub	bx, ax			;compute min(AX, BX)
	sbb	dx, dx
	and	bx, dx
	add	bx, ax			;BX: min(AX, BX) from above
	mov	SaveSpaceyExt, bx	;this is how much space we need (in Y)
	or	[ShadowMemoryTrashed], 1  ;see note at SSB_RESTORE
	mov	SaveInUse, SAVEBITMAPUSED ;set the "in use" flag
	sub	ax, ax
	mov	DstxOrg, ax		;always save starting at coordinate 0
	mov	wSrcXUpdate, ax 	;don't change source X position in loop
	mov	wDstYUpdate, ax 	;don't change dest. Y pos. either
	mov	wSrcYUpdate, 192	;advance src Y pos be save area height
	mov	ax, xExt
	mov	wDstXUpdate, ax 	;advance dest. X position by X extent
	mov	ax,FreeSpaceyOrg	;get place we're going to save
					;our screen to
	mov	SaveSpaceyOrg,ax	;save this for our restore
	mov	DstyOrg,ax		;save it for sending to the board
	add	ax,bx			;space for use by others
	mov	FreeSpaceyOrg,ax
	sub	FreeSpaceyExt,bx	;allocate some space in Y
	mov	ax,LocalRect.left	;source coordinates are the
	mov	SrcxOrg,ax		;passed coordinates
	mov	bx,LocalRect.top
	mov	SrcyOrg,bx

public  SSB_COMMON
SSB_COMMON:                                                             

;Our next task will be to exclude the cursor from the region:

	push	LocalRect.left		;pass down our exclusion area
	push	LocalRect.top		;to CursorExclude
	push	LocalRect.right
	push	LocalRect.bottom
	cCall	CursorExclude		;go exclude the cursor
	CheckFIFOSpace	FOUR_WORDS
	mov	al,67h			;set the foreground to replace
	mov	dx,FUNCTION_0_PORT	;set the background
	out	dx,ax
	mov	dx,FUNCTION_1_PORT	;(we're always opaque)
	out	dx,ax

;Set up the plane enable appropriately:

	mov	al,0ffh 		;set to read & write to all
	mov	dx,READ_ENABLE_PORT	;planes at once
	out	dx,ax

;Now, set the mode to "block move pattern":

	mov	ax,0a000h
	mov	dx,MODE_PORT
	out	dx,ax

	dec	si			;account for remainder done separately
	jz	SSBDoRemainder

	mov	yExt, 192		;set Y extent to off-screen area height
SSBBandingLoop:
	cCall	CopyRect		;copy first band
	mov	ax, wSrcXUpdate 	;now update all coordinates to the next
	add	SrcxOrg, ax		;band to do
	mov	ax, wSrcYUpdate
	add	SrcyOrg, ax
	mov	ax, wDstXUpdate
	add	DstxOrg, ax
	mov	ax, wDstYUpdate
	add	DstyOrg, ax
	dec	si
	jnz	SSBBandingLoop

SSBDoRemainder:
	jcxz	SSB_EXIT		;in case remainder is zero
	mov	yExt, cx		;set the remainder Y extent
	cCall	CopyRect		;copy what is left to be done

public  SSB_EXIT
SSB_EXIT:
	cCall	CursorUnExclude 	;go free up the cursor

SSBGoodExit:
	mov	ax,1			;return success code

public  SSB_END
SSB_END:
cEnd

cProc	CopyRect, <NEAR, PUBLIC>
cBegin
	CheckFIFOSpace	SEVEN_WORDS
	mov	ax,xExt 		;get the X-extent
	dec	ax
	mov	dx,RECT_WIDTH_PORT	;set the width
	out	dx,ax
	mov	ax,yExt 		;get the Y-extent
	dec	ax
	mov	dx,RECT_HEIGHT_PORT	;set the height
	out	dx,ax
	mov	ax,SrcxOrg		;set starting source X
	mov	dx,Srcx_PORT
	out	dx,ax
	mov	ax,DstxOrg		;set starting destination X
	mov	dx,Dstx_PORT
	out	dx,ax
	mov	ax,SrcyOrg		;set starting source Y
	mov	dx,Srcy_PORT
	out	dx,ax
	mov	ax,DstyOrg		;set starting destination Y
	mov	dx,Dsty_PORT
	out	dx,ax

;Now, send the command:

	mov	ax,0c0b3h		;get command into AX
	mov	dx,COMMAND_FLAG_PORT
	out	dx,ax
cEnd

sEnd	Code
end
