
page	,132
title	**	Menu memory operations for SPUD

	.xlist
	include cmacros.inc
	include windows.inc
	include menu.inc
	include tty.inc
	.list

;-------------------- MENU MEMORY MANAGER -----------------------------
;
;CONTENTS:
;
;	1)	This file contains the Menu memory manager code
;		for SPUD.  The menu memory is assumed to be a
;		fixed, contiguous block of data.
;
;----------------------------------------------------------------------




sBegin	data

externW 	MenuSeg
externW 	MyMenuSeg
externW 	hMenuSeg
externW 	MenuPara
GlobalW 	MenuSegSize,0
externW 	OldApState
externW 	hMenu
externW 	MenuTop
externB 	pifBehavior
GlobalW 	FirstCMPos,0	;this holds the first avail position in control
				; menu

sEnd	data



sBegin	code
	assumes cs,code
	assumes ds,data

PUBLIC		InitMenuMem, GetMenuNode, ReturnMenuNode


ExternFP	<GlobalAlloc, GlobalLock, GlobalUnlock, GetVersion>
ExternFP	<GlobalReAlloc>
ExternNP	<LockMenuSeg, UnlockMenuSeg>
;------------- InitMenuMem ---------------------------------------------
;
;	DESCRIPTION
;		This routine initializes all nodes in the menu memory
;		block
;
;	ENTRY
;		none
;
;	EXIT
;		AX: = Number of free nodes
;
;	MODIFIED
;		AX,BX,CX,DX
;
;------------------------------------------------------------------------

InitMenuMem	PROC	NEAR


	push	SI
	push	DI
	push	ES			;save ES
	push	DS
	pop	ES			;get DS in ES
	mov	AX,MenuPara		;calculate the size of the area needed
	mov	CX,4
	shl	AX,CL
	mov	MenuSegSize,AX
	mov	CX,GMEM_MOVEABLE+GMEM_ZEROINIT ;make it moveable memory
	xor	BX,BX
	push	DS
	push	ES
	regptr	xsize,BX,AX
	cCall	GlobalAlloc,<CX,xsize> ;allocate the memory
	mov	hMenuSeg,AX
	call	LockMenuSeg
	pop	ES
	pop	DS
	or	DX,DX			; is there a menu seg
	jnz	IMM18			; return zero nodes if not
	xor	AX,AX			; No nodes
	mov	MenuPara,0		; don't set up for bad apps
	mov	MenuSegSize,0
	jmp	IMM95
IMM18:
	mov	CX,MenuItemSize 	;get the size of a Menu Item
	mov	BX,MenuSegSize
	xor	DI,DI			;structure count
	sub	BX,CX
	push	DS
	mov	DS,DX
	xor	DX,DX
	xor	SI,SI			;clear the offset
IMM20:
	cmp	SI,BX			;are we out of memory
	ja	IMM90			;jump if so
	mov	[SI].ID,DX		;mark the node
	inc	DI			;increment the counter
	add	SI,CX			;point to next item
	jmp	IMM20			;try again
IMM90:
	pop	DS			;need DS back for call to GetMenuNode
	mov	AX,DI
	push	AX
	call	UnlockMenuSeg		;MenuSeg unlocked for next call
	call	GetMenuNode		;get a node for the system menu
	call	LockMenuSeg		;get MenuSeg locked again,DX has MenuSeg
	push	DS
	mov	DS,DX			;get menuseg back
	mov	SI,AX			;get the address in index reg
	mov	[SI].ID,SystemMenuID	;set the system menu ID
	mov	[SI].flags,MF_POPUP
	mov	[SI].item,0		;nothing in it yet
	mov	byte ptr [SI].text,'-'
	mov	byte ptr [SI].text+1,0
	mov	[SI].twidth,1
	mov	[SI].mnem,' '           ;no really used, just for completeness
	mov	[SI].mnempos,0FFH	;no mnemonic this keeps SetMnemonic
					;from highlighting
	mov	AX,ES:hMenu
	mov	word ptr [SI].handle,AX
	mov	[SI].next,0
	mov	ES:MenuTop,SI
	pop	DS
	call	UnLockMenuSeg
	cCall	GetVersion		;get the windows version
	mov	FirstCMPos,FIRST_1X_CM_POS
	cmp	AL,02H			;is this 2.0 or above
	jl	IMM92
	mov	FirstCMPos,FIRST_2X_CM_POS
IMM92:
	pop	AX			;get result back
IMM95:
	pop	ES
	pop	DI
	pop	SI
	ret

InitMenuMem	ENDP


;-----------SetMenuSize--------------------------------------------
;
;	DESCRIPTION
;		This routine will set the menu buffer size larger for
;		a bad application.  Good applications are not affected
;		since their size can grow dynamically.
;
;	ENTRY
;		dwBytes: = number of bytes requested.  This number
;			   to the number required for the menu poke
;			   structure.  This is not necessarily the
;			   number of bytes that will be allocated
;			   since the MenuInfo and MenuPoke structures
;			   may not be the same size.
;		MenuSeg must not be locked
;
;	EXIT
;		AX: <> 0 if successful
;
;	MODIFIES
;		C Standard
;
;--------------------------------------------------------------------------
cProc	SetMenuSize,<PUBLIC,NEAR>
	parmW	dwBytes

cBegin	SetMenuSize
	push	SI
	xor	AX,AX
	cmp	OldApState,SF_NOT_STARTED
	jnz	SMSRet
	mov	AX,0FFFFH		;just in case
	test	pifbehavior,BADBITS
	jz	SMSRet			;if good app just ignore
	mov	AX,dwBytes		;get # in MenuPoke size
	or	AX,AX			;if no more requested just return
	jnz	SMS10			;      success
	mov	AX,0FFFFH		;show OK
	jmp	SMSRet
SMS10:
	xor	DX,DX
	mov	BX,MenuPokeSize 	;divisor
	idiv	BX			;AX is the number of menu items
	inc	AX
	inc	AX			;give a couple for padding and changes
	mov	BX,MenuItemSize
	imul	BX			;get the number of bytes
	add	AX,MenuSegSize		;add in the current size
	add	AX,0FH			;round to nearest paragraph
	shr	AX,1
	shr	AX,1
	shr	AX,1
	shr	AX,1
	mov	DX,AX			;save the number of paragraphs
	shl	AX,1
	shl	AX,1
	shl	AX,1
	shl	AX,1
	mov	CX,AX			;save the number of bytes
	mov	BX,GMEM_MOVEABLE+GMEM_ZEROINIT
	push	CX
	push	DX
	xor	SI,SI
	regptr	memsiz,SI,AX
	cCall	GlobalReAlloc,<hMenuSeg, memsiz, BX>
	pop	DX
	pop	CX
	or	AX,AX			;did it work?
	jz	SMSRet
	mov	MenuSegSize,CX
	mov	MenuPara,DX
	mov	hMenuSeg,AX		;the handle may have changed
SMSRet:
	pop	SI

cEnd	SetMenuSize









;------------GetMenuNode-------------------------------------------
;
;	DESCRIPTION
;		Returns an offset pointer into the MenuSeg of a free
;		menu node
;
;	ENTRY
;		MenuSeg is not locked by anyone
;
;	EXIT
;		CF: = 0 	a node was available
;		CF: = 1 	no nodes available
;		AX: = address if CF = 0
;		hMenuSeg and MenuSeg may be modified
;
;	MODIFIED
;		AX
;
;-------------------------------------------------------------------

GetMenuNode	PROC	NEAR

	push	ES
	push	SI
	push	DX
	push	CX
	push	BX
	call	LockMenuSeg
	mov	CX,MenuSegSize
	sub	CX,MenuItemSize 	;give the cushion
	mov	ES,DX
	xor	SI,SI
GMN10:
	cmp	ES:[SI].ID,0		   ;is it free
	jnz	GMN20
	mov	ES:[SI].ID,1		   ;mark it as in use
	jmp	GMN90
GMN20:
	add	SI,MenuItemSize 	;point to next one
GMN22:
	cmp	SI,CX			;are we at the end
	jle	GMN10
	cmp	OldApState,SF_NOT_STARTED ;if not started even bad apps can
	jz	GMN24			  ;grow dynamically
	test	pifbehavior,BADBITS
	jnz	GMN70
GMN24:
	call	UnlockMenuSeg
	mov	CX,MenuItemSize
	add	CX,0FH
	shr	CX,1			;convert to paragraphs
	shr	CX,1
	shr	CX,1
	shr	CX,1
	mov	AX,MenuPara
	add	AX,CX
	mov	DX,AX			;save the new paragraphs
	shl	AX,1		       ;convert back to bytes
	shl	AX,1
	shl	AX,1
	shl	AX,1
	xor	CX,CX
	regptr	memsiz,CX,AX
	mov	BX,GMEM_ZEROINIT+GMEM_MOVEABLE
	push	ES
	push	DX
	cCall	GlobalReAlloc,<hMenuSeg, memsiz, BX>
	pop	DX
	pop	ES
	or	AX,AX
	jz	GMN70
	mov	hMenuSeg,AX
	mov	MenuPara,DX
	shl	DX,1
	shl	DX,1
	shl	DX,1
	shl	DX,1
	mov	MenuSegSize,DX
	mov	CX,DX
	sub	CX,MenuItemSize
	call	LockMenuSeg
	mov	ES,DX
	jmp	GMN22

GMN70:
	call	UnLockMenuSeg
GMN80:
	stc				;show error
	jmp	GMN100
GMN90:
	call	UnLockMenuSeg
	clc				;show OK
	mov	AX,SI			;get offset in AX
GMN100:
	pop	BX
	pop	CX
	pop	DX
	pop	SI
	pop	ES
	ret


GetMenuNode	ENDP


;------------ReturnMenuNode-------------------------------------------
;
;	DESCRIPTION
;		Returns a node to the free pool.
;
;	ENTRY
;		AX: = offset into MenuSeg of node
;		MyMenuSeg is valid
;
;	EXIT
;		CF: = 0 if successful
;		CF: = 1 if unsuccessful
;
;	MODIFIED
;
;-------------------------------------------------------------------

ReturnMenuNode	PROC	NEAR

	push	DS
	push	SI
	push	CX
	push	BX
	mov	CX,MyMenuSeg
	mov	BX,MenuSegSize
	sub	BX,MenuItemSize 	;get the cushion
	mov	DS,CX
	cmp	AX,BX
	ja	RMN80			;report error
	mov	SI,AX
	mov	[SI].ID,0		;show it is free
	clc				;show OK
	jmp	RMN90
RMN80:
	stc				;show error
RMN90:
	pop	BX
	pop	CX
	pop	SI
	pop	DS
	ret

ReturnMenuNode	ENDP










sEnd	code
	END
