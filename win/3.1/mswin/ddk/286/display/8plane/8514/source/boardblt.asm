page		,132
title           IBM 8514 Bitblt Routines
.286c

.xlist
include 	CMACROS.INC
include 	gdidefs.inc
include 	8514.INC
include 	drvpal.inc
include 	savescrn.inc
.list

sBegin          Data
subttl          Data Declarations
page +

;First, some data externals:

externW 	FreeSpaceyOrg		;in IBMDRV.ASM
externW 	FreeSpaceyExt		;in IBMDRV.ASM
externW 	SaveSpaceyOrg		;in SAVESCRN.ASM
externB 	ShadowMemoryTrashed	;in SAVESCRN.ASM
externB 	SaveInUse		;in SAVESCRN.ASM
externB 	PaletteFlags		;in TBINIT.ASM
externB 	BitsPixel
;Include table ROP.INC here:

.xlist
include         ROP.INC
.list
externD	Ring3ToRing0
externD	Ring0ToRing3
sEnd            Data

externFP	TranslateBrush			;in TBINIT.ASM
externFP	TranslateDrawMode		;in TBINIT.ASM

subttl          Decide Type of Bitblt
page +
sBegin		Code

XlateBitMask	label	word
	dw	0			; no source --> patcopy of some sort
	dw	0ac0h			; w/ source --> may involve bitmap

assumes         cs,Code
assumes         ds,Data

externFP        <CursorExclude,CursorUnExclude> ;in ROUTINES.ASM
externNP	HandleBrush			;in BRUSH.ASM works fine
externNP        ReadScreen                      ;in READSCRN.ASM
externNP	BlockWrite			;in BLKWRI.ASM
externNP	BlockMove			;in BLKMOV.ASM

externW 	_cstods 			;in BITBLT.ASM

comment ~
List of functions in this module:
    BoardBltFar
    BoardBlt
    HostIsDest
    HostBLTManagement
    HostBLTOperation
    BoardIsDest
    GetDestOntoBoard
    DecodeRasterOp
    SourceCopy
    DestOp
    NegateResult
    BBExcludeCursor
    GetFreeRect

This module is called from BitBlt after it has been determined that the 8514
hardware is to be used to perform the actual blt operation.  All parameters
passed into this function have been verified and further checking or clipping
in this module will not be necessary.
~

public	BoardBltFar
BoardBltFar proc far
	pop	cx			;fetch return address
	pop	dx
	cCall	BoardBlt
	push	dx			;restore return address
	push	cx
	ret
BoardBltFar endp


;DEBUG	 = 1

cProc	BoardBlt,<NEAR,PUBLIC>, <ds, cx, dx>

	include BOARDBLT.INC		;stack frame definitions

cBegin
	assumes cs, Code
	assumes ds, nothing

	mov	ax, DstxOrg		;copy finally clipped destination X and
	mov	PatXOrg, ax		;Y origin into pattern X and Y origin.
	mov	bx, DstyOrg		;This is necessary to accomplish
	mov	PatYOrg, bx		;brush alignment in X and Y

ifdef	DEBUG
.386
	shl	eax, 16 		;Hi EAX: dest X as it was passed in
	shl	ebx, 16 		;Hi EBX: dest Y as it was passed in
	mov	cx, SrcxOrg
	shl	ecx, 16 		;Hi ECX: src X as it was passed in
	mov	dx, SrcyOrg
	shl	edx, 16 		;Hi EDX: src Y as it was passed in
	mov	si, xExt
	shl	esi, 16 		;Hi ESI: ext X as it was passed in
	mov	di, yExt
	shl	edi, 16 		;Hi EDI: ext Y as it was passed in
	cmp	xExt, 1024
	jbe	NormalXExtent
	int	3			;force break in case xExt > 1024
NormalXExtent:
.286
endif
	mov	ds, cs:[_cstods]	;make DS=Data
	assumes ds, Data

	mov	ax,word ptr Ring3ToRing0	;selector
	mov	word ptr ToRing0,ax
	mov	ax,word ptr Ring3ToRing0+2	;offset
	mov	word ptr ToRing0+2,ax
	mov	ax,word ptr Ring0ToRing3	;selector
	mov	word ptr ToRing3,ax
	mov	ax,word ptr Ring0ToRing3+2	;offset
	mov	word ptr ToRing3+2,ax

;First, get and save the length of the ROP and the operands contained within:

        mov     BoardBltFlags,0         ;initialise the flags
        mov     dx,seg_Rop              ;get the raster op code into DX
        or      dl,dl                   ;do we have a "complement" ROP?
                                        ;(is ROP code above 80h, ie: is a
                                        ; sign bit set?)
        jns     BBGotRop                ;no, we use the ROP as is
        not     dl                      ;otherwise complement the ROP code
        or      dl,dl                   ;is the ROP destination white?
        jnz     BBGotRop                ;nope, continue
        mov     ParseStringOffset,DataOFFSET DestWhite
                                        ;special case destination white
        mov     ParseStringLength,DestWhiteLen
        mov     seg_Rop,dx              ;and pretend we're not above 80h (so
                                        ;we don't NOT later)
        jmp     BBGetOperands     	;continue

public  BBGotRop
BBGotRop:
        mov     ax,dx                   ;copy the ROP code into AX
        mov     si,dx                   ;for multiplication by 3
        shl     ax,1                    ;multiply ROP code by 3 bytes per
                                        ;table entry
        add     si,ax                   ;get pointer to ROP table entry
        add     si,DataOFFSET ROP_TABLE ;add on the table base offset

;Now DS:SI points to the first byte of the ROP table entry.
;DX still contains the ROP code.

        lodsw                           ;get the address of the parse string
        mov     ParseStringOffset,ax    ;save the address of the string
        lodsb                           ;get the length and operands present
        xor     cx,cx                   ;clear CX
        mov     cl,al                   ;get the length of the parse string
        and     cl,0fh                  ;into CX
        and     al,0f0h                 ;isolate the operands present
	jz	BBNMultiPassRop		;This is 1 of the 16 multi-pass Rops.

;If there is a trailing NOT in the ROP, we can take a shortcut if the ROP code
;is above 80h.  Since we must NOT ROPs above 80h, a trailing NOT present will
;cause a cancellation and can be eliminated.  It is well worth testing for 
;this here.

        or      al,al                   ;is the trailing NOT flag set?
        jns     BBSaveRopCount          ;nope, continue
        cmp     seg_Rop,80h             ;is the ROP code above 80h?
        jb      BBSaveRopCount          ;nope, continue on normally
        dec     cx                      ;otherwise, bump down the parse string
	jcxz	BBNoAction		;length so we won't see the trailing
                                        ;NOT
        mov     seg_Rop,dx              ;and pretend our ROP is below 80h so
                                        ;we won't do the "above 80H" NOT

public  BBSaveRopCount
BBSaveRopCount:
        mov     ParseStringLength,cx    ;save the length of our parse string
        or      BoardBltFlags,al        ;save the operands present
        jmp     BBGetOperands     	;and continue

public  BBNoAction
BBNoAction:
        xor     ax,ax                   ;set error return code
	jmp	BBExit			;and get out now

public  BBNMultiPassRop
BBNMultiPassRop:
	mov	dx,word ptr Rop+2	;Save Rop code for negate test.	
        mov     si,ParseStringOffset    ;get the pointer to the Rop3.
	shr	cl,2			;cl = number of passes.
@@:	lodsw
	mov	word ptr Rop+2,ax	;Get Rop Code
	lodsw
	mov	word ptr Rop,ax		;Get parse string info.
	push	si
	arg	lpDstDev		;pointer to destination's PDEVICE
        arg	DstxOrg                 ;X-origin on destination bitmap
        arg     DstyOrg                 ;Y-origin on destination bitmap
        arg     lpSrcDev                ;pointer to source's PDEVICE
        arg     SrcxOrg                 ;X-origin on source bitmap
        arg     SrcyOrg                 ;Y-origin on source bitmap
        arg     xExt                    ;length in X
        arg     yExt                    ;length in Y
        arg     Rop                     ;tertiary raster operation code
        arg     lpPBrush                ;pointer to physical brush
        arg     lpDrawMode              ;pointer to DRAWMODE data structure
	cCall	BoardBlt
	pop	si
	loop	@b
        cmp     dx,80h		        ;was the original ROP code above 80h?
	jb	short @f		;yes, must negate the destination.
	mov	word ptr Rop+2,0055h	;Set rop code to negate destination.
	mov	word ptr Rop,0009h	;Get parse string info.
	arg	lpDstDev		;pointer to destination's PDEVICE
        arg	DstxOrg                 ;X-origin on destination bitmap
        arg     DstyOrg                 ;Y-origin on destination bitmap
        arg     lpSrcDev                ;pointer to source's PDEVICE
        arg     SrcxOrg                 ;X-origin on source bitmap
        arg     SrcyOrg                 ;Y-origin on source bitmap
        arg     xExt                    ;length in X
        arg     yExt                    ;length in Y
        arg     Rop                     ;tertiary raster operation code
        arg     lpPBrush                ;pointer to physical brush
        arg     lpDrawMode              ;pointer to DRAWMODE data structure
	cCall	BoardBlt
@@:	jmp	BBExit			;We're done.

; at this point: ds=Data
BBSetAccFlag:

public  BBGetOperands
BBGetOperands:
	push	ds			;save our DS=Data
	mov	ax, ss
	mov	es, ax			;make ES=SS
	lds	si, lpDstDev
	mov	bx, [si].bmType 	;get destination device type
	mov	DestType, bx		;save it
	or	bx, bx			;is destination the board
	jnz	DestIsBoard		;yes, don't copy anything
	lea	di, bbl_dst		;ES:DI-->local dst device header
	mov	cx, (size BITMAP)/2	;copy destination PDev header if bmp
rep	movsw				;copy dest bitmap header
DestIsBoard:
	test	BoardBltFlags, SrcPresent
	jz	NoSource		;this blt has no source
	lea	di, bbl_src		;ES:DI-->local scr dev header
	mov	cx, 5			;assume source is device
	lds	si, lpSrcDev		;is source device bmp or board?
	mov	dx, [si].bmType 	;get source device type
	mov	SrcType, dx		;save it for later
	or	dx, dx
	jnz	SourceIsBoard		;board or bitmap?
	mov	cx, (size BITMAP)/2
SourceIsBoard:
rep	movsw				;copy source bitmap header

NoSource:
	pop	ds
	assumes ds, Data
	mov	PaletteXlateFlag, 0	;assume no translation will be needed
	test	[PaletteFlags], BITBLTACCELERATE
	jz	BBNoTranslation

	mov	cx, bx
	sub	bx, bx			;use bx to accumulate flags
	sub	si, si			;use si to select proper LUT entry
	or	cx, cx			;CX: DestType
	jnz	BBGotDstType		;jump if it is not a bitmap
	mov	al, [BitsPixel] 	;is it a color bitmap
	cmp	bbl_dst.bmBitsPixel, al
	cmc
	adc	bh, bh			;CY if dest in color
BBGotDstType:
	or	bh, ch
	test	BoardBltFlags, SrcPresent
	jz	BBNoSource
	inc	si			; make si a word pointer
	inc	si
	or	dx, dx
	jnz	BBGotSrcType
	mov	al, [BitsPixel]
	cmp	bbl_src.bmBitsPixel, al
	cmc
	adc	bl, bl
BBGotSrcType:
	or	bl, dh
BBNoSource:
	mov	ax, bx
	shl	al, 1
	or	al, ah
	mov	ah, al
	and	ah, 03h
	and	al, 60h
	shr	al, 3
	or	al, ah
	mov	cl, al
	add	si, CodeOFFSET XlateBitMask
	mov	ax, cs:[si]
	shr	ax, cl
	and	al, 01h
	mov	ah, PaletteFlags
	and	al, ah
	shr	ah, 1
	and	al, ah
	or	PaletteFlags, NOMEMSTRBLT
	mov	PaletteXlateFlag, al	; Keep for later use
BBNoTranslation:

;At this point it is time to get the foreground (text)/background colors out
;of the DrawMode stucture.  However, this is only necessary if the either the
;source or the destination is monochrome.  Otherwise, we can save loading a
;far pointer.

	test	bbl_dst.bmBitsPixel, 1	;destination monochrome?
	jnz	BBDestMonochrome
	test	BoardBltFlags, SrcPresent
	jz	BBDetermineBLTType
	test	bbl_src.bmBitsPixel, 1	;source monochrome?
	jz	BBDetermineBLTType
BBDestMonochrome:
	cmp	seg_lpDrawMode, 0	;is pointer to draw mode valid?
	je	BBDone			;no, then exit now!
        les     di,lpDrawMode           ;get passed DRAWMODE
        mov     al,es:[di+4]            ;get the background colour
        mov     BackgroundColour,al     ;save it
        mov     al,es:[di+8]            ;as well as the foreground colour
        mov     ForegroundColour,al     ;save it

page
public  BBDetermineBLTType
BBDetermineBLTType:

;Determine which BLTing process we call dependent on whether the destination
;is the board or main memory:

        mov     bx,CodeOFFSET BoardIsDest       ;assume destination is board
        cmp     DestType,0                      ;is the destination main memory?
        jne     BBCallBLT                       ;nope, just go call the BLT
        mov     bx,CodeOFFSET HostIsDest        ;otherwise, host is destination

public  BBCallBLT
BBCallBLT:
	call	bx			;go call the proper BLT

public  BBDone
BBDone:
        cCall   CursorUnExclude         ;go free the exclusion
        mov     ax,1                    ;return code = GOOD EXIT

BBExit:
cEnd


subttl          BLT To Host Memory Bitmap
page +             
cProc           HostIsDest,<NEAR,PUBLIC>

cBegin

;First, we must exclude the cursor during board operations.  We determine
;whether the source is in the visible area of the screen.  If it is, we
;exclude it.  Otherwise, we just stop it from moving during operations.

        test    BoardBltFlags,SrcPresent;does this BLT have a source?
	jz	HIDStopCursor		;nope, just stop the cursor from moving
	mov	ax, SrcType
        or      ah,ah                   ;is the source in main memory?
        jz      HIDStopCursor           ;yes, just stop the cursor from moving
        or      BoardBltFlags,ReadFlag  ;set the reading-from-screen flag
        cCall   BBExcludeCursor         ;see if cursor needs exclusion
        jmp     short HIDCheckForSourceCopy

public  HIDStopCursor
HIDStopCursor:                           

;An FFFFH passed in the first parameter to CursorExclude will cause it to
;simply stop the cursor from moving without excluding it:

        push    word ptr 0ffffh         ;get this onto the stack
        sub     sp,6                    ;adjust the stack for 3 dummy params
	cCall	CursorExclude		;go stop the cursor

public  HIDCheckForSourceCopy
HIDCheckForSourceCopy:
        cmp     off_Rop,20h             ;are we doing a source copy?
        jne     HIDUseOffScreenMemory   ;nope, go do the BLT using the screen
        cmp     SrcType,0               ;is the source in main memory?
        je      HIDUseOffScreenMemory   ;yes, go do the BLT using the screen
        cCall   ReadScreen              ;otherwise, do the BLT directly
        jmp     short HIDExit           ;and we're done!

public  HIDUseOffScreenMemory
HIDUseOffScreenMemory:

;We run into a very complicated scheme here.  We potentially need to divide 
;off-screen memory into 2 parts where we store 2 operands of the BLT.
;
;               1) The destination (as it exists before the BLT).
;               2) The "ultimate" destination (as the BLT progresses).
;                                                                     
;Therefore, we have the potential for a BLT needing memory management in the
;X-direction as well as the Y-direction.  We will check whether there's a
;destination in the BLT.  If there is, we'll split available off-screen 
;memory into 2 halves (in X) and invoke memory management in X if necesary.

        cCall   GetFreeRect             ;get the largest free space available
        mov     ax,LocFreeSpaceyExt     ;get nbr of lines available to us
        cmp     ax,yExt                 ;are enough lines available?
        jae     HIDNoManagement         ;yes, do the BLT normally

public  HIDUseManagement
HIDUseManagement:

	mov	ax, LocFreeSpaceyExt	;depending on the BitBlt's phase the
	mov	YIncr, ax		;actual extent may be negative
	mov	cx, ax			;may need that a little later
	mov	YAdjust, 0
	mov	dx, seg_lpDstDev	;are source and destination the same
	mov	ax, off_lpDstDev	;device (bitmap)?  If so, we MUST do
	cmp	dx, seg_lpSrcDev	;some phase processing or else we
	jne	HIDUMLoop		;may trash the source before the Blt
	cmp	ax, off_lpSrcDev	;is completed.
	jne	HIDUMLoop

	mov	ax, SrcyOrg		;now it's time to determine whether
	cmp	ax, DstyOrg		;to do the Blt going up (SrcY < DstY)
	jge	HIDUMLoop		;or going down (default)

	neg	YIncr			;we're going up.  Negate Y org. incr.
	sub	dx, dx			;Now adjust Y origins as follows:
	mov	ax, yExt		;Y' = Y+((Ext/Band - 1)+Ext%Band)
	div	cx			;where Ext is the Y extent of the
	dec	ax			;BitBlt and Band the height of the
	mov	bx, dx			;band in off-screen memory that will
	mul	cx			;be used
	add	ax, bx
	add	DstyOrg, ax
	add	SrcyOrg, ax
	sub	cx, bx
	mov	YAdjust, cx

;We have to manage our BLT.  So, we create a loop, BLTing only the number of
;lines that we have available.

public  HIDUMLoop
HIDUMLoop:
        push    yExt                    ;save the starting yExt
        mov     ax,LocFreeSpaceyExt     ;get the nbr of lines that we can do
        mov     yExt,ax                 ;now yExt has the maximum nbr of lines
                                        ;that we can BLT during this pass
        cCall   HostBLTManagement       ;go do this slice of the BLT
	pop	yExt			;get back the starting yExt
	mov	ax, YIncr
        add     DstyOrg,ax              ;adjust our starting Y
        add     SrcyOrg,ax              ;in both source and destination
        mov     ax,LocFreeSpaceyExt     ;get nbr of lines that we've just done
        sub     yExt,ax                 ;subtract off nbr of lines just done
                                        ;now yExt has the nbr of lines yet to do
	jz	HIDExit			;If zero, we are done.
        cmp     yExt,ax                 ;do we still need to manage?
	ja	HIDUMLoop		;yes, go use memory management
	mov	ax, YAdjust
	add	DstyOrg, ax
	add	SrcyOrg, ax

public  HIDNoManagement
HIDNoManagement:
	cCall	HostBLTManagement

public  HIDExit
HIDExit:
cEnd


page
cProc           HostBLTManagement,<NEAR,PUBLIC>

cBegin                                       

;Now it's time to see if we need to manage in the X-direction.  We will have
;to do so if the BLT involves a destination operand and if the xExt is more
;than 1/2 of the screen size.

        test    BoardBltFlags,DstPresent;have we a destination?
        jz      DHODoBLT                ;nope, just go do the BLT to the origin
                                        ;of invisible memory
        mov     ax,(X_SIZE/2)           ;now AX has the size that we have
                                        ;available in the X-direction and
                                        ;the starting X-coordinate of the
                                        ;"ultimate" destination
        cmp     ax,xExt                 ;do we have enough room in X?
        jae     DHONoManagement         ;yes, we don't have to manage

public  DHOManageX
DHOManageX:
        push    SrcxOrg                 ;we'd better save the X-coordinates
        push    DstxOrg                 ;in case we're managing in Y too
	push	xExt

public  DHOMXLoop
DHOMXLoop:

;We have to manage our BLT in X.  So, we create a loop, BLTing only the 
;number of pixels in X that we have available.

        push    xExt                    ;save the starting xExt
        mov     ax,(X_SIZE/2)           ;get nbr of pixels that we can do
        mov     xExt,ax                 ;now xExt has the maximum nbr of pixels
                                        ;that we can BLT during this pass
	cCall	GetDestOntoBoard
        mov     ax,(X_SIZE/2)           ;this is the X-address of off-screen
                                        ;memory for our BLT
        mov     bx,FreeSpaceyOrg        ;this is the Y-address of off-screen
                                        ;memory for our BLT
        cCall   HostBLTOperation        ;go do this slice of the BLT
        pop     xExt                    ;get back the starting xExt
        mov     ax,(X_SIZE/2)           ;get size of slice just done
        add     DstxOrg,ax              ;adjust our starting X
        add     SrcxOrg,ax              ;in both destination and source
        sub     xExt,ax                 ;subtract off nbr of pixels just done
                                        ;now xExt has the width yet to do
        cmp     xExt,ax                 ;do we still need to manage?
        ja      DHOMXLoop               ;yes, go use memory management
	cCall	GetDestOntoBoard
        mov     ax,(X_SIZE/2)           ;this is the X-address of off-screen
                                        ;memory for our BLT
        mov     bx,FreeSpaceyOrg        ;this is the Y-address of off-screen
                                        ;memory for our BLT
        cCall   HostBLTOperation        ;go do this slice of the BLT
        pop     xExt                    ;get back original values
	pop	DstxOrg
	pop	SrcxOrg
        jmp     short DHOExit           ;we're done now

public  DHONoManagement
DHONoManagement:                              
        cCall   GetDestOntoBoard        ;go and put the destination onto the
                                        ;board at (0,FreeSpaceyOrg)
        mov     ax,(X_SIZE/2)           ;restore the starting X-coordinate of
                                        ;our ultimate destination
        mov     bx,FreeSpaceyOrg        ;get the starting Y-coordinate of
                                        ;free off-screen workspace
	cCall	HostBLTOperation
	jmp	short DHOExit

public  DHODoBLT
DHODoBLT:

;If there's a Rop that involves a non-replace (e.g. AND, XOR, NOT, OR) function,
;we must put the destination down onto the board and then do the BLT on top
;of it.  There's no need to manage since we just do our work on top of the
;destination.

        mov     si,ParseStringOffset    ;get the pointer to the Rop3.
        lodsb                           ;get the first operation into AL
        shr     al,4                    ;get the passed function for the first
                                        ;operation
        cmp     al,ReplaceMode          ;is it a replace?
        je      DHODirectBLT            ;yes, skip getting the dest on-board
        cCall   GetDestOntoBoard        ;no, we must put down the dest first

public  DHODirectBLT
DHODirectBLT:                                                               
        xor     ax,ax                   ;get starting X-coordinate of off-screen
                                        ;memory
        mov     bx,FreeSpaceyOrg        ;get the starting Y-coordinate of
                                        ;free off-screen workspace
	cCall	HostBLTOperation

public  DHOExit
DHOExit:
cEnd


page
cProc           HostBLTOperation,<NEAR,PUBLIC>

cBegin  

;Entry:
;       AX has the starting X-coordinate of off-screen workspace
;       BX has the starting Y-coordinate of off-screen workspace

	push	DstxOrg 		;save our real DstxOrg
	push	DstyOrg
        push    ax                      ;save X-coordinate of our off-screen
                                        ;workspace
        push    bx                      ;and save the Y-coordinate of our
                                        ;off-screen workspace
        mov     DstxOrg,ax              ;this is the X-coordinate in off-screen
                                        ;memory that we're going to BLT to
        mov     DstyOrg,bx              ;this is the Y-coordinate in off-screen
                                        ;memory that we're going to BLT to

;We're now ready to BLT to the board's invisible free space area.  The process
;DecodeRasterOp will be faked out so that it'll BLT to the X & Y origins of
;the off-screen workspace.

        cCall   DecodeRasterOp          ;go ahead and do the BLT
        pop     bx                      ;get back Y-coordinate of our off-screen
                                        ;workspace
        pop     ax                      ;get back X-coordinate of our off-screen
                                        ;workspace    
        pop     DstyOrg                 ;get back the real DstyOrg
        pop     DstxOrg                 ;get back the real DstxOrg
        push    SrcxOrg                 ;save our real SrcxOrg
        push    SrcyOrg                 ;save our real SrcyOrg
        mov     SrcxOrg,ax              ;now we've got the address of off-screen
        mov     SrcyOrg,bx              ;memory as the source for the BLT
        cCall   ReadScreen              ;go read our BLT into its proper place
                                        ;in main memory
        pop     SrcyOrg                 ;restore saved source coordinates
	pop	SrcxOrg
cEnd


subttl          BLT To the 8514
page +
cProc           BoardIsDest,<NEAR,PUBLIC>

cBegin
	assumes ds, Data

	test	[PaletteFlags], BITBLTACCELERATE
	je	BIDNoTranslation
	cCall	<FAR PTR TranslateBrush>, <lpPBrush>
	mov	seg_lpPBrush, dx
	mov	off_lpPBrush, ax
;	 mov	 dx, 1
	cCall	<FAR PTR TranslateDrawMode>, <lpDrawMode>
	mov	seg_lpDrawMode, dx
	mov	off_lpDrawMode, ax
BIDNoTranslation:
        and     BoardBltFlags,WriteFlag ;tell cursor exclusion that we're
                                        ;writing
        cCall   BBExcludeCursor         ;go turn off the cursor if it's around

;We will need to use off-screen memory as a scratch area in case our BLT 
;involves a destination operand (since we may want to use our destination
;and it may already be covered up by previous operations).  We also may need
;to invoke memory management if there is not a large enough rectangle in 
;which to store the destination.

        test    BoardBltFlags,DstPresent;any destination operands involved?
        jz      BIDNoManagement         ;nope, we don't need to do any fudging
        cCall   GetFreeRect             ;go make us a rectangle to use
        mov     ax,LocFreeSpaceyExt     ;get nbr of lines available to us
        cmp     ax,yExt                 ;are enough lines available?
        jb      BIDUseManagement        ;nope, go use memory management
        cCall   GetDestOntoBoard        ;yes, go move the destination into
                                        ;invisible memory
        jmp     short BIDNoManagement   ;and do the BLT normally

public  BIDUseManagement
BIDUseManagement:

;We have to manage our BLT.  So, we create a loop, BLTing only the number of
;lines that we have available.

        push    yExt                    ;save the starting yExt
        mov     ax,LocFreeSpaceyExt     ;get the nbr of lines that we can do
        mov     yExt,ax                 ;now yExt has the maximum nbr of lines
                                        ;that we can BLT during this pass
        cCall   GetDestOntoBoard        ;move the destination into invisible
                                        ;memory
        cCall   DecodeRasterOp          ;do the BLT
        pop     yExt                    ;get back the starting yExt
        mov     ax,LocFreeSpaceyExt     ;get nbr of lines that we've just done
        add     DstyOrg,ax              ;adjust our starting Y
        add     SrcyOrg,ax              ;in both source and destination
        sub     yExt,ax                 ;subtract off nbr of lines just done
                                        ;now yExt has the nbr of lines yet to do
        cmp     yExt,ax                 ;do we still need to manage?
        ja      BIDUseManagement        ;yes, go use memory management
        cCall   GetDestOntoBoard        ;and do the last pass without memory
                                        ;management

public  BIDNoManagement
BIDNoManagement:
        call    DecodeRasterOp          ;go draw onto the board
cEnd


subttl          Get Destination into A Known Location
page +             
cProc           GetDestOntoBoard,<NEAR,PUBLIC>

cBegin

;Set up some common parameters for getting the destination onto a known
;proper place in invisible memory.

        push    SrcxOrg                 ;save the source X-origin
        push    SrcyOrg                 ;save the source Y-origin
        push    DstxOrg                 ;save the destination X-origin
        push    DstyOrg                 ;save the destination Y-origin
        mov     ax,DstxOrg              ;get the passed destination X-origin
        mov     SrcxOrg,ax              ;make it the source for this operation
        mov     ax,DstyOrg              ;get the passed destination Y-origin
        mov     SrcyOrg,ax              ;make it the source for this operation
        mov     DstxOrg,0               ;always BLT to X-coordinate 0
        mov     ax,FreeSpaceyOrg        ;get the Y-coordinate to BLT
                                        ;the destination to
        mov     DstyOrg,ax              ;set up coordinates to BLT
                                        ;our destination to

;Now decide whether we need to block write the destination from main memory
;or whether we can block move it to invisible memory:

        mov     al,ReplaceMode          ;set the mix mode to REPLACE
        cmp     DestType,0              ;is destination in main memory?
        jne     GDOBBlockMove           ;nope, go do a block move

public  GDOBBlockWrite
GDOBBlockWrite:
        push    seg_lpSrcDev            ;save the passed source
	push	off_lpSrcDev
        mov     bx,seg_lpDstDev         ;replace the source PDevice with that
        mov     seg_lpSrcDev,bx         ;of the destination
	mov	bx,off_lpDstDev
	mov	off_lpSrcDev,bx
        cCall   BlockWrite              ;go do the BLT
        pop     off_lpSrcDev            ;restore pointer to original source
	pop	seg_lpSrcDev
        jmp     short GDOBExit          ;and get out

public  GDOBBlockMove
GDOBBlockMove:
        cCall   BlockMove               ;BLT the destination to where we want it

GDOBExit:
        pop     DstyOrg                 ;restore saved coordinates for BLT
	pop	DstxOrg
	pop	SrcyOrg
	pop	SrcxOrg
cEnd


subttl          Raster Processing Routines for BoardBlt
page +    
cProc           DecodeRasterOp,<NEAR,PUBLIC>

cBegin

;Note: The Rops from 80H to FFH are simply the same ROPs as 7FH to 00H with
;NOTs stuck on the end of them!

        mov     si,ParseStringOffset    ;get the offset of the parse string
        mov     cx,ParseStringLength    ;get the length of the parse string

;Now DS:SI points to the first byte of the ROP table entry and CX contains
;the length of our parse string.

public  DROLoop
DROLoop:
        push    cx                      ;save the loop counter
        lodsb                           ;get the next ROP to do into AL
                                        ;the low 4 bits contain the operand,
                                        ;the high 4 bits contain the mix mode
        push    si                      ;save pointer to next operation
        shr     al,1                    ;get unary negate flag into carry
        jnc     DROBinaryOp             ;it's not a negate, go check other ops
        cCall   NegateResult            ;go do the negation
        jmp     short DROEndLoop        ;and go get next ROP!

public  DROBinaryOp
DROBinaryOp:

;At this point:
;       Source operand flag is about to be shifted into carry from AL
;       Bits 6, 5, 4, & 3 have the mix mode.

        shr     al,1                    ;get the source flag into carry
        jnc     DROPattern              ;the operand isn't a source
        shr     al,2                    ;get mix mode into lowest part of AL
        call    SourceCopy              ;and go do the source operation
        jmp     short DROEndLoop        ;and go get next ROP

public  DROPattern
DROPattern:
        shr     al,1                    ;get the pattern flag into carry
        jnc     DRODest                 ;the operand isn't a pattern
        shr     al,1                    ;get mix mode into lowest part of AL
        les     di,lpPBrush             ;get the brush
        cmp     byte ptr es:[di],1      ;are we hollow?
        jne     DROPCallHandleBrush     ;nope, continue

;If we're hollow, we must check out the mix mode to see if it's DestBlack or
;DestWhite.  If it's either of these, then we want to continue the call, if
;not, just skip the call to HandleBrush.

        cmp     al,1                    ;is mode DestBlack?
        je      DROPCallHandleBrush     ;yes, go do the call
        cmp     al,2                    ;is mode DestWhite?
        jne     DROEndLoop              ;nope, don't call HandleBrush

public  DROPCallHandleBrush
DROPCallHandleBrush:
        arg     es                      ;this is lpPBrush
	arg	di
	arg	lpDrawMode
	arg	DstxOrg
	arg	DstyOrg
	arg	xExt
	arg	yExt
	arg	PatXOrg
	arg	PatYOrg
        cCall   HandleBrush             ;yes! go do it!
        jmp     short DROEndLoop        ;and go get next ROP

public  DRODest
DRODest:
        shr     al,1                    ;get the destination flag into carry
        jnc     DROEndLoop              ;nope, it's an error.... just punt
        call    DestOp                  ;yes! go do it!

DROEndLoop:
        pop     si                      ;restore saved registers
	pop	cx
        loop    DROLoop                 ;go decode next operation

public  DROCheckComplementRop
DROCheckComplementRop:
        cmp     seg_Rop,80h             ;have we a ROP from 80H to FFH?
        jb      DROExit                 ;nope, we can get out!
        cCall   NegateResult            ;yes, 80H to FFH are complements of
                                        ;00H to 7FH ROPs!

DROExit:
cEnd


page
cProc           SourceCopy,<NEAR,PUBLIC>

;Entry:
;       Desired function (ROP) is in AL.
;	Coordinates have been "fixed up" if necesary.

cBegin
        mov     bx,CodeOFFSET BlockWrite;assume source is in main memory
;	 les	 di,lpSrcDev		 ;now ES:DI points to source device
;	 cmp	 byte ptr es:[di+1],0	 ;is this memory to board?
	cmp	bbl_src.bmType, 0
        je      SCCallRoutine           ;yes, go call the proper routine
        mov     bx,CodeOFFSET BlockMove ;source is on the board

public  SCCallRoutine
SCCallRoutine:
        cCall   bx                      ;call the proper routine
cEnd


page
cProc           DestOp,<NEAR,PUBLIC>

cBegin

;Entry:
;       AL has MIX MODE (ROP) to do on destination

        push    SrcxOrg                 ;save the source coordinates
	push	SrcyOrg
        mov     SrcxOrg,0               ;destination is always at coordinate 0
        mov     bx,FreeSpaceyOrg        ;get Y-origin of destination
        mov     SrcyOrg,bx              ;this is line to BLT from
        cCall   BlockMove               ;go block move the dest onto destination
        pop     SrcyOrg                 ;restore source coordinates
	pop	SrcxOrg
cEnd


page
cProc           NegateResult,<NEAR,PUBLIC>

cBegin

;Set ourselves up with a solid brush for complementing (we don't care what 
;colour it is).

        push    off_lpPBrush            ;save the pointer to our real brush
	push	seg_lpPBrush
        mov     NotBrush,0              ;make sure we are using a solid brush
        lea     di,ss:NotBrush          ;get offset of this phony solid brush
        mov     off_lpPBrush,di         ;and save the pointer to it
	mov	seg_lpPBrush,ss
        mov     al,NOTMode              ;set function to NOT
        arg     lpPBrush                ;send down the arguments
	arg	lpDrawMode
	arg	DstxOrg
	arg	DstyOrg
	arg	xExt
	arg	yExt
	arg	PatXOrg
	arg	PatYOrg
        cCall   HandleBrush             ;go NOT the destination
        pop     seg_lpPBrush            ;restore pointer to our real brush
	pop	off_lpPBrush
cEnd


subttl          Cursor Exclusion Routine for BoardBlt
page +
cProc           BBExcludeCursor,<NEAR,PUBLIC>

cBegin

;Note:  AX will be destroyed by this proc.

        test    BoardBltFlags,ReadFlag  ;are we reading from the screen?
        jnz     BBECRead                ;yep, go do read processing

public  BBECWrite
BBECWrite:
        push    DstxOrg                 ;get starting X
        push    DstyOrg                 ;get starting Y
        mov     ax,DstxOrg              ;get ending X
	add	ax,xExt
	push	ax
        mov     ax,DstyOrg              ;get ending Y
	add	ax,yExt
	push	ax
        jmp     short BBECCommon        ;and go do common processing

public  BBECRead
BBECRead:
	push	SrcxOrg
	push	SrcyOrg
        mov     ax,SrcxOrg              ;get ending X
	add	ax,xExt
	push	ax
        mov     ax,SrcyOrg              ;get ending Y
	add	ax,yExt
	push	ax

BBECCommon:
        cCall   CursorExclude           ;go exclude the cursor
cEnd


subttl          Determine Off-Screen Memory Usage for BoardBlt
page +
cProc           GetFreeRect,<NEAR,PUBLIC>

cBegin

;First, determine if we have enough space available to do the whole BLT 
;without disturbing the contents of off-screen memory:

	mov	ax,yExt 		;get the nbr of lines that we
					;need for this BLT
	mov	bx,FreeSpaceyExt	;get the nbr of lines available
	mov	LocFreeSpaceyExt,bx	;save it locally
	cmp	ax,bx			;is there currently enough space?
	jbe	GFRExit 		;yes, no adjustment needed

	mov	ShadowMemoryTrashed, 0	;Show save screen/font is invalid
	mov	ax,Y_SIZE+64		;and use all of 
	mov	FreeSpaceyOrg,ax	;offscreen memory.
	mov	bx,192
	mov	FreeSpaceyExt,bx
	mov	LocFreeSpaceyExt,bx	;update nbr of lines available.
	mov	FreeSpaceyExt, bx

public  GFRExit
GFRExit:
cEnd

sEnd            Code
end
