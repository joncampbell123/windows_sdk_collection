
title	**	Menu operations for SPUD

	.xlist
	include cmacros.inc
	include menu.inc
	include error.inc
	include tty.inc
	.list

;-------------------- MENU -------------------------------------------
;
;CONTENTS:
;
;   1)	Code to handle the new data structures of the SPUD OldAp
;	menuing system.
;
;PROGRAMMER:
;	Ralph Peterson
;
;---------------------------------------------------------------------
page


sBegin	data


ifdef	DEBUG
dbgNode    db	   0AH,0DH,'Node Address: ',0
dbgID	   db	   0AH,0DH,'ID          : ',0
dbgflags   db	   0AH,0DH,'Flags       : ',0
dbgitem    db	   0AH,0DH,'Item        : ',0
dbgtext    db	   0AH,0DH,'Text        : ',0
dbgwidth   db	   0AH,0DH,'Width       : ',0
dbghandle  db	   0AH,0DH,'Handle      : ',0
dbgnext    db	   0AH,0DH,'Next        : ',0

endif

externW 	MenuSeg
externW 	hMenuSeg		;handle to good app menu area
externB 	pifBehavior
GlobalW 	hWinMenu,?		;window menu handle for good apps
externW 	hMenu			;window system menu for good apps
externW 	hTTYWnd 		;WinOldAP window handle
externW 	OldApState		;Current state of this old app

staticB 	MacroUpdate,0,MENUPOKESIZE

GlobalW 	MenuTop,0FFFFH	;points to the top of the menu list
GlobalW 	MyMenuSeg,0	; internal menuseg address

temp		dw 0		;temporary storage variable
externW 	FirstCMPos		;first avail pos in control menu
sEnd	data

sBegin	code

	assumes cs,code
	assumes ds,data



externNP	<GetMenuNode, ReturnMenuNode, GetMacroInfo>
externFP	<CreateMenu, GlobalLock, GlobalUnlock, ChangeMenu>
externFP	<SetMenu,DrawMenuBar, EnableMenuItem, OemToAnsi, AnsiToOem>
PUBLIC		UpdateMenu, VerifyFlags, AddMenuItemBP, StringLength
PUBLIC		PrepMenusForClose, FixText, UnFixText
PUBLIC		GetMenuTitle, GetItem, ChangeMenuItem, AddMenu
PUBLIC		AM41, UM80
PUBLIC		AddMenuItemBC, ModifyMenu, DeleteMenuItemBP, DeleteMenuItemBP
PUBLIC		DeleteMenu, GetMenuItem, CopyText, MultBits
PUBLIC		LockMenuSeg, UnlockMenuSeg, GetMnemonic
ifdef	DEBUG
PUBLIC		DumpData
endif


;-------------- GetMenuItem --------------------------------------------
;
;	Description
;		Returns a pointer to the MenuInfo structure containing
;		data relating to the Menu ID passed if the ID is in
;		the menu passed
;
;	Entry
;		AX: = MenuID
;		SI: = pointer to the menu title structure
;
;	Exit
;		CF: = 0 if successful
;		CF: = 1 if unsuccessful
;		AX: = pointer to menu structure if found
;		BX: = pointer to previous item if found
;		      pointer to menu title structure if it
;		      is the first item in the menu
;
;	Modifies
;		AX,BX
;
;------------------------------------------------------------------------

GetMenuItem	PROC	NEAR

	push	SI		;save pointer
	push	ES
	push	DX
	call	LockMenuSeg
	or	DX,DX		;is there a MenuSeg
	jz	GMI80
	mov	ES,DX
	pop	DX
	mov	BX,SI
	test	ES:[SI].flags,MF_POPUP
	jz	GMI80		;return if not a popup
	mov	SI,ES:[SI].item ;get the first item
	or	SI,SI		;any menu items?
	jz	GMI80		;return if not
GMI20:
	cmp	ES:[SI].ID,AX	;is this the one
	jz	GMI90		;yes
	mov	BX,SI
	mov	SI,ES:[SI].next ;get the next one
	or	SI,SI		;is it the end?
	jz	GMI80		;yes, return none
	jmp	GMI20		;if not, check this one
GMI80:
	xor	SI,SI		;set up for negative return
	stc
	jmp	GMI95
GMI90:
	clc
GMI95:
	pushf
	call	UnlockMenuSeg
	popf
	mov	AX,SI		;get output in AX
	pop	ES
	pop	SI		;restore pointer
	ret

GetMenuItem	ENDP

;-------------- GetMenuTitle ---------------------------------------
;
;	Description
;		Returns a pointer to the MenuInfo structure containing
;		data relating to the Menu Title ID passed
;
;
;	Entry
;		AX: = Menu Title ID or position
;		BX: = 0 if search by ID
;		BX: <> 0 if search by position
;
;	Exit
;		CF: = 0 if successful
;		CF: = 1 if unsuccessful
;		AX: = pointer to menu structure if found
;		BX: = pointer to previous item if found
;		      0FFFFH if no previous item
;
;	Modifies
;		AX, BX, CX, DX
;
;------------------------------------------------------------------------


GetMenuTitle	PROC	NEAR

	push	SI
	push	ES
	call	LockMenuSeg
	or	DX,DX
	jz	GMT80
	mov	ES,DX
	mov	DX,BX			;DX is search type variable
	mov	BX,0FFFFH		;assume error
	mov	SI,MenuTop		;get the first menu title
	cmp	ES:[SI].ID,AX		;is this it
	jnz	GMT30			;no go on
	or	DX,DX			;are we searching by command
	jz	GMT90			;if so, we got it (system menu)
	mov	AX,ERR_OPERATION	;otherwise this is an error
	jmp	GMT80
GMT30:
	mov	BX,SI
	mov	SI,ES:[SI].next 	;get the first real menu
	xor	CX,CX			;counter at zero
GMT32:
	or	DX,DX			;position or command?
	jz	GMT40
	cmp	CX,AX			;is this the one
	jz	GMT90
	jmp	GMT50
GMT40:
	cmp	AX,ES:[SI].ID		;is this the one
	jz	GMT90			;yes, then done
GMT50:
	mov	BX,SI
	mov	SI,ES:[SI].next 	;get the next one
	inc	CX			;add one to counter
	or	SI,SI			;is this the end
	jnz	GMT32			;no , check this one
	mov	AX,ERR_PARAMETERS	;error code
	jmp	GMT80


GMT80:
	stc				;report error
	jmp	GMT95
GMT90:
	clc
GMT95:
	mov	AX,SI			;put result in AX
	pushf
	call	UnlockMenuSeg
	popf
	pop	ES
	pop	SI			;get pointer back
	ret

GetMenuTitle	ENDP



;-------------- GetItem --------------------------------------------
;
;	Description
;		Returns a pointer to the MenuInfo structure containing
;		data relating to the Menu ID or the Menu Title ID that
;		is passed
;
;	Entry
;		AX: = MenuID or MenuTitleID
;
;	Exit
;		CF: = 0 if successful
;		CF: = 1 if unsuccessful
;		AX: = pointer to menu structure if found
;		BX: = pointer to previous item if found
;		      0FFFFH if no previous item
;
;	Modifies
;		AX,BX,CX
;
;------------------------------------------------------------------------


GetItem 	PROC	NEAR

	push	SI
	push	ES
	push	DX
	call	LockMenuSeg
	or	DX,DX
	jz	GI80
	mov	ES,DX
	pop	DX
	mov	BX,0FFFFH
	mov	SI,MenuTop		;get the first menu title
GI20:
	cmp	ES:[SI].ID,AX		;is this it
	jz	GI90			;yes tell the world
	push	AX			;save the ID
	call	GetMenuItem		;see if it is in the menu
	pop	CX			;get the ID in CX
	jnc	GI95			;jump if it's there
	mov	AX,CX			;otherwise get ID back
	mov	BX,SI
	mov	SI,ES:[SI].next 	;get the next one
	or	SI,SI			;is this the end
	jnz	GI20			;no , check this one
GI80:
					;yes, fall through and report 0
	stc				;show error
	jmp	GI97
GI90:
	mov	AX,SI			;put result in AX
GI95:
	clc
GI97:
	pushf
	call	UnlockMenuSeg
	popf
	pop	ES
	pop	SI			;get pointer back
	ret

GetItem 	ENDP


;-------------- AddMenu -----------------------------------------
;
;	DESCRIPTION
;		This routine inserts or appends the Menu to the
;		menu bar.   This routine will create a sub-menu
;		for a good app menu if need be.
;
;
;	ENTRY
;		AX: = pointer to MenuInfo structure completely updated
;		BX: = position or 0FFFFH to append
;
;	EXIT
;		CF: = 0 if successful
;		CF: = 1 if unsuccessful
;		AX: = error code if CF: = 1
;
;	MODIFIES
;		AX, BX, CX, DX
;
;-------------------------------------------------------------------


AddMenu      PROC    NEAR

	push	SI
	push	DI
	push	ES
	mov	CX,MyMenuSeg
	mov	ES,CX
	push	AX
	push	BX
	mov	SI,AX
	mov	AX,ES:[SI].ID
	call	GetItem 		;check for duplicate
	pop	BX
	pop	AX
	jc	AM5
	mov	AX,ERR_PARAMETERS
	jmp	AM80
AM5:
	xor	CX,CX			;zero the count
	mov	SI,MenuTop		;point to the first Menu Title
	cmp	SI,0FFFFH		;system menu must be there
	jne	AM10
	mov	AX,ERR_OPERATION	;error if not
	jmp	AM80
AM10:
	cmp	BX,0FFFFH		 ;are we appending
	je	AM20
	push	BX
	push	AX
	mov	AX,BX			;get position in AX
	mov	BX,01			;search by position
	call	GetMenuTitle		;get the item in that position
	pop	SI
	mov	CX,BX
	pop	BX
	jnc	AM30
	jmp	AM80			;AX already has error code

					;here if appending
AM20:
					;ES:AX -> new node
					;ES:SI -> system menu node
	cmp	ES:[SI].next,0
	jz	AM22
	mov	SI,ES:[SI].next 	;if not at end go to next one
	jmp	AM20
AM22:
	mov	DI,AX			;get in index register
	mov	ES:[SI].next,DI 	;update pointer
	mov	ES:[DI].next,0		;show end of list
	mov	SI,DI			;get new node in SI for good app proc
	jmp	AM40			;go do good app stuff

					;here if at insertion point
AM30:
					;ES:SI -> new node
					;ES:AX -> insertion node
					;ES:CX -> prev node
					;BX = position
	mov	DI,CX
	mov	DX,ES:[DI].next 	;update pointers
	mov	ES:[SI].next,DX
	mov	ES:[DI].next,SI
AM40:
	test	ES:[SI].flags,MF_POPUP	;is it a popup
	jz	AM41
	mov	ES:[SI].item,0		;item is initially 0 if a popup

					;~~GOOD APP MENU UPDATE HERE
					;at this point ES:SI -> new node
					;BX is the position of the new node
	test	[pifBehavior],BADBITS	;is this a good app
	jnz	AM50			;jump if not
	mov	DX,ES:[SI].flags	; set the wChange word
	or	DX,MF_INSERT+MF_BYPOSITION
	cmp	BX,0FFFFH		;are we appending
	jnz	AM41
	or	DX,MF_APPEND
AM41:
	mov	ES:[SI].handle,0	;zero if not a popup
	mov	AX,ES:[SI].ID		;get the ID
	test	es:[SI].flags,MF_POPUP	;is this a popup menu
	jz	AM42
	push	ES
	push	DX
	push	BX
	cCall	CreateMenu		;create the submenu
	pop	BX
	pop	DX
	pop	ES
	mov	ES:[SI].handle,AX	;if popup get the submenu handle
AM42:
	mov	CX,SI			;get pointer to string
	add	CX,text
	regptr	string,ES,CX		;make a long out of it
					;change the menu
	cmp	hWinMenu,0		;is there a windows menu
	jnz	AM44
	push	ES
	push	AX
	push	BX
	push	CX
	push	DX
	cCall	CreateMenu
	mov	hWinMenu,AX
	cCall	SetMenu,<hTTYWnd,hWinMenu>
	pop	DX
	pop	CX
	pop	BX
	pop	AX
	pop	ES
AM44:
	cCall	ChangeMenu,<hWinMenu,BX,string,AX,DX>
	cCall	DrawMenuBar,hTTYWnd


AM50:
	clc				;show success
	jmp	AM90
AM80:
	stc				;show error
AM90:
	pop	ES
	pop	DI
	pop	SI
	ret

AddMenu 	ENDP



;--------------- AddMenuItemBP ----------------------------------------
;
;	DESCRIPTION
;		This routine adds an menu item to a menu (may be the
;		system menu) using the MF_BYPOSITION flag.
;
;	ENTRY
;		AX: = pointer to a completely updated menu info
;		      structure. Menu ID is in the handle field.
;
;		BX: = position or 0FFFFH to append
;
;	EXIT
;		CF: = 0 if successful
;		CF: = 1 if unsuccessful
;		AX: = error code if CF: = 1
;
;	MODIFIES
;		AX, BX, CX, DX
;-------------------------------------------------------------------------

AddMenuItemBP	PROC	NEAR

	push	SI
	push	DI
	push	ES
	mov	CX,MyMenuSeg		  ;get the menu data segment
	mov	ES,CX
	mov	SI,AX
	mov	AX,ES:[SI].ID
	push	BX
	call	GetItem 		;check for duplicate
	pop	BX
	jc	AMIBP5
	mov	AX,ERR_PARAMETERS
	jmp	AMIBP80
AMIBP5:
	push	BX
	xor	BX,BX			;search by command
	mov	AX,ES:[SI].handle	;set up for call
	call	GetMenuTitle		;get a pointer to the menu
	pop	BX
	jnc	AMIBP10
	jmp	AMIBP80
AMIBP10:
	mov	DI,AX			;get menu title in DI
	mov	DX,DI			;save it in DX also
	test	word ptr ES:[DI].flags,MF_POPUP ;is it a popup
	jnz	AMIBP15
	mov	AX,ERR_OPERATION	;error code
	jmp	AMIBP80
AMIBP15:
	xor	CX,CX			;zero the counter
	mov	DI,ES:[DI].item 	;get the first item
	or	DI,DI			;is there one
	jnz	AMIBP25
	cmp	BX,0FFFFH		;should we append into empty menu
	jz	AMIBP42 		;yes, then OK append it
	mov	AX,ERR_PARAMETERS	;else error
	jmp	AMIBP80
AMIBP25:
	cmp	CX,BX			;is this the position
	jz	AMIBP50 		;then insert it (special case)
AMIBP27:
	cmp	CX,BX			;Do we want to insert here?
	jz	AMIBP52
	cmp	word ptr ES:[DI].next,0 ;at the end of the list
	jz	AMIBP40 		;jump if so
	mov	DI,ES:[DI].next 	;get the next item
	inc	CX			;increment counter
	jmp	AMIBP27 		;go again


AMIBP40:
					;Here if at end of list
	cmp	BX,0FFFFH		;are we supposed to append
	je	AMIBP44
	mov	AX,ERR_PARAMETERS	;show error
	jmp	AMIBP80
AMIBP42:
	or	CX,CX			;is this the first item
	jnz	AMIBP44
	mov	DI,DX			;get the menu title pointer in DI
	mov	ES:[DI].item,SI 	;set the pointer correctly
	jmp	AMIBP46
AMIBP44:
	mov	ES:[DI].next,SI 	;set the next pointer
AMIBP46:
	mov	ES:[SI].next,0		;set end of list flag
	jmp	AMIBP60 		;see if we need good app processing

AMIBP50:				;Here for insertions
					;first is special case of inserting
					;at position 0
	mov	DI,DX			;get the menu title pointer
	mov	AX,ES:[DI].item 	;save the current pointer
	mov	ES:[DI].item,SI 	;set the new one
	jmp	AMIBP54
AMIBP52:				; this is normal insertion process
	mov	AX,ES:[DI].next 	;insert the item
	mov	ES:[DI].next,SI
AMIBP54:
	mov	ES:[SI].next,AX 	;re connect the list


					;~~GOOD APP MENU UPDATE HERE
					;at this point ES:SI -> new node
					; ES:DX -> menu title
					; ES:DI -> previous node or menu title
					;	   depending on location
					; CX is the position of the new node
AMIBP60:
	test	[pifBehavior],BADBITS	;is this a good app
	jnz	AMIBP70 		;jump if not
	cmp	ES:[DI].handle,SystemMenuID ; is this in the system menu
	jnz	AMIBP61
	add	CX,FirstCMPos
AMIBP61:
	mov	DI,DX			;get the menu in DI

	mov	DX,ES:[SI].flags	;get the style bits
	or	DX,MF_BYPOSITION+MF_INSERT
	cmp	BX,0FFFFH		;are we appending
	jne	AMIBP62
	or	DX,MF_APPEND
AMIBP62:
	mov	BX,SI
	add	BX,text
	regptr	string,ES,BX
					;do the call
	cCall	ChangeMenu,<ES:[DI].handle,CX,string,ES:[SI].ID,DX>


AMIBP70:
	clc
	jmp	AMIBP90

AMIBP80:
	stc

AMIBP90:
	pop	ES
	pop	DI
	pop	SI
	ret

AddMenuItemBP	ENDP



;------------- AddMenuItemBC -------------------------------------------
;
;	DESCRIPTION
;		This routine adds an menu item to a menu (may be the
;		system menu) using the MF_BYCOMMAND flag.  All appends
;		should be routed through the MF_BYPOSITION commands.
;		Therefore, even if MF_BYCOMMAND is set, if MF_APPEND
;		is set, this function should not be used.
;
;	ENTRY
;		AX: = pointer to a completely updated menu info
;		      structure. Menu ID is in the handle field.
;
;		BX: = MenuID (wIDChangeItem)
;
;	EXIT
;		CF: = 0 if successful
;		CF: = 1 if unsuccessful
;		AX: = error code if CF: = 1
;
;	MODIFIES
;		AX, BX, CX, DX
;-------------------------------------------------------------------------

AddMenuItemBC	PROC	NEAR


	push	SI
	push	DI
	push	ES
	mov	CX,MyMenuSeg		  ;get the menu data segment
	mov	ES,CX
	mov	SI,AX
	mov	AX,ES:[SI].ID
	push	BX
	call	GetItem 		;check for duplicate
	pop	BX
	jc	AMIBC5
	mov	AX,ERR_PARAMETERS
	jmp	AMIBC80
AMIBC5:
	push	BX
	xor	BX,BX			;search by command
	mov	AX,ES:[SI].handle	;set up for call
	call	GetMenuTitle		;get a pointer to the menu
	pop	BX
	jnc	AMIBC10
	mov	AX,ERR_PARAMETERS	;get error code
	jmp	AMIBC80
AMIBC10:
	push	BX
	mov	BX,AX			;get menu item pointer
	pop	AX			;get menu ID
	push	SI			;save pointer
	mov	SI,BX			;need pointer to title for call
	call	GetMenuItem		;get the change item
	mov	CX,ES:[SI].handle	;get good app menu handle just in case
	pop	SI			;get new item pointer back
	jnc	AMIBC15
	mov	AX,ERR_PARAMETERS	;error code
	jmp	AMIBC80
AMIBC15:
					;AX = change item pointer
					;BX = prev change item
					;CX = Windows Menuhandle
					;SI = new item pointer
	mov	DI,BX			;get the prev in DI
	test	ES:[DI].flags,MF_POPUP	;is this the menu title
	jz	AMIBC20
	mov	ES:[DI].item,SI 	;set the new pointer
	jmp	AMIBC25
AMIBC20:
	mov	ES:[DI].next,SI
AMIBC25:
	mov	ES:[SI].next,AX 	;point the new node to the change node
	mov	DI,AX			;point to change node

					;~~GOOP APP MENU UPDATE HERE
	test	[pifBehavior],BADBITS
	jnz	AMIBC70 		;not a good app
	mov	DI,AX			;get a pointer to the change item
	mov	BX,SI
	add	BX,text 		;get a pointer to the string
	regptr	string,ES,BX
	mov	DX,ES:[SI].flags	;get the style
	or	DX,MF_BYCOMMAND+MF_INSERT ;set command
					;Do the change
	cCall	ChangeMenu,<CX,ES:[DI].ID,string,ES:[SI].ID,DX>

AMIBC70:
	clc
	jmp	AMIBC90
AMIBC80:
	stc
AMIBC90:
	pop	ES
	pop	DI
	pop	SI
	ret

AddMenuItemBC	ENDP

;----------------- DeleteMenu ----------------------------------------------
;
;	DESCRIPTION
;		This routine deletes the top level menu item that
;		is passed.  This command is only supported with
;		the MF_BYPOSITION flag set.  This routine will destroy
;		any good app popup menu associated with the menu title.
;		All menu item nods associated with the popup menu are
;		returned to the node pool.
;
;	ENTRY
;		AX: = position of menu to delete
;
;	EXIT
;		CF: = 0 is successful
;		CF: = 1 if failure
;		AX: = error code if CF: = 1
;
;	MODIFIES
;		AX, BX, CX, DX
;---------------------------------------------------------------------------
DeleteMenu	PROC	NEAR

	push	SI
	push	DI
	push	ES
	mov	CX,MyMenuSeg
	mov	ES,CX
	push	AX			;save position
	mov	BX,0FFFFH		;search by position
	call	GetMenuTitle		;get the menu title pointer
					;AX -> the one we want
					;BX -> points to the prev one
	pop	CX			;get the position back
	jc	DM80			;if error AX has error code already
	mov	SI,AX			;get in index reg
	mov	DI,BX			;get in index reg
	mov	DX,ES:[SI].next
	mov	ES:[DI].next,DX 	;remove it from the list

					;~~GOOD APP MENU UPDATE HERE
	test	[pifBehavior],BADBITS	;is it good
	jnz	DM90			;no
	mov	SI,AX
	test	ES:[SI].flags,MF_POPUP	;is this a popup menu
	xor	BX,BX			;need a zero
	xor	DX,DX			;get flags
	or	DX,MF_BYPOSITION+MF_DELETE
	push	AX			;save the pointer
	push	ES
					;do the deletion
	cCall	ChangeMenu,<hWinMenu,CX,BX,BX,BX,DX>
	cCall	DrawMenuBar,hTTYWnd
	pop	ES

	pop	AX
	jmp	DM90
DM80:

	stc
	jmp	DM95
DM90:
					;remove any sub menu items
	mov	SI,AX
	call	ReturnMenuNode
	test	ES:[SI].flags,MF_POPUP	;is this a popup menu
	jz	DM93
	mov	SI,ES:[SI].item 	;get first item
DM92:
	or	SI,SI			;is it a node
	jz	DM93
	mov	AX,SI
	mov	SI,ES:[SI].next
	call	ReturnMenuNode
	jmp	DM92
DM93:
	cmp	ES:[DI].ID,SystemMenuID ;is the prev the system menu
	jnz	DM94
	cmp	ES:[DI].next,0		;are there any other menus
	jnz	DM94
	xor	AX,AX
	mov	hWinMenu,AX		;show that no menu exists
	cCall	SetMenu,<hTTYWnd,AX>	;remove the menu
DM94:
	clc
DM95:
	pop	ES
	pop	DI
	pop	SI
	ret

DeleteMenu	ENDP




;----------------- DeleteMenuItemBP ----------------------------------------
;
;	DESCRIPTION
;		This routine deletes a menu item in a menu (may be the
;		system menu) using the MF_BYPOSITION flag.
;
;	ENTRY
;		AX: = Menu ID of popup menu
;		BX: = position
;
;	EXIT
;		CF: = 0 if successful
;		CF: = 1 if unsuccessful
;		AX: = error code if CF: = 1
;		AX: = pointer to deleted structure if CF: = 0
;
;	MODIFIES
;		AX, BX, CX, DX
;-------------------------------------------------------------------------
DeleteMenuItemBP   PROC    NEAR

	push	SI
	push	DI
	push	ES
	mov	CX,MyMenuSeg		  ;get the menu data segment
	mov	ES,CX
	push	BX
	xor	BX,BX			;search by command
	call	GetMenuTitle		;get a pointer to the menu
	pop	BX
	jnc	DMIBP10
	mov	AX,ERR_PARAMETERS	;get error code
	jmp	DMIBP80
DMIBP10:
	mov	DI,AX			;get menu title in DI
	mov	DX,DI			;save it in DX also
	test	word ptr ES:[DI].flags,MF_POPUP ;is it a popup
	jnz	DMIBP15
	mov	AX,ERR_OPERATION	;error code
	jmp	DMIBP80
DMIBP15:
	xor	CX,CX			;zero the counter
	mov	DI,ES:[DI].item 	;get the first item
DMIBP16:
	or	DI,DI			;is there one
	jnz	DMIBP17
	mov	AX,ERR_INVALID_OPERATION ;error code
	jmp	DMIBP80
DMIBP17:
	cmp	CX,BX			;is this the position
	jz	DMIBP60 		;then delete it
	mov	DX,DI			;save the prev pointer
	mov	DI,ES:[DI].next 	;get the next item
	inc	CX			;increment counter
	jmp	DMIBP16 		;go again


DMIBP60:				;here for deletions
					;ES:DI -> struct to delete
					;CX = position
					;ES:DX -> prev structure
					;ES:AX -> menu title structure
	mov	SI,DX
	push	BX
	or	CX,CX			;is this item 0?
	jnz	DMIBP62
	mov	BX,ES:[DI].next
	mov	ES:[SI].item,BX 	;change the menu title node
	jmp	DMIBP64
DMIBP62:
	mov	BX,ES:[DI].next
	mov	ES:[SI].next,BX 	;change the prev menu item


DMIBP64:
	cmp	ES:[DI].handle,SystemMenuID ;are we in the system menu
	jnz	DMIBP66
	add	CX,FirstCMPos		;first system menu pos is not zero
DMIBP66:
	pop	BX
					;~GOOD APP MENU UPDATE HERE
	test	[pifBehavior],BADBITS	;is it a good app
	jnz	DMIBP70
	mov	SI,AX			;get pointer to menu title
	xor	BX,BX			;get a zero
	xor	DX,DX			;clear ChangeMenu Flags
	or	DX,MF_DELETE+MF_BYPOSITION
					;make the change
	push	DI			;save the pointer
	cCall	ChangeMenu,<ES:[SI].handle,CX,BX,BX,BX,DX>
	pop	DI
DMIBP70:
	mov	AX,DI
	call	ReturnMenuNode		;return the node to the pool
	clc
	jmp	DMIBP90

DMIBP80:
	stc

DMIBP90:
	pop	ES
	pop	DI
	pop	SI
	ret

DeleteMenuItemBP   ENDP



;----------------- DeleteMenuItemBC ----------------------------------------
;
;	DESCRIPTION
;		This routine deletes a menu item in a menu (may be the
;		system menu) using the MF_BYCOMMAND flag.
;
;	ENTRY
;		AX: = Menu ID of popup menu
;		BX: = Menu ID of item to delete
;
;	EXIT
;		CF: = 0 if successful
;		CF: = 1 if unsuccessful
;		AX: = error code if CF: = 1
;		AX: = pointer to deleted structure if CF: = 0
;
;	MODIFIES
;		AX, BX, CX, DX
;-------------------------------------------------------------------------

DeleteMenuItemBC	PROC	NEAR


	push	SI
	push	DI
	push	ES
	mov	CX,MyMenuSeg		  ;get the menu data segment
	mov	ES,CX
	push	BX
	xor	BX,BX			;search by command
	call	GetMenuTitle		;get a pointer to the menu
	pop	BX
	jnc	DMIBC10 		;jump if OK
	mov	AX,ERR_INVALID_OPERATION
	jmp	DMIBC80
DMIBC10:
	mov	SI,AX			;set up for next call
	mov	AX,BX
	call	GetMenuItem		;get pointers to the item
	jnc	DMIBC15
	mov	AX,ERR_INVALID_OPERATION ;error code
	jmp	DMIBC80
DMIBC15:
					;ES:AX -> the item
					;ES:BX -> prev item
					;ES:SI -> menu title
	mov	DI,AX			;get the pointer to the structure
	push	DX
	cmp	BX,SI			;Is this the first item
	jnz	DMIBC20
	mov	DX,ES:[DI].next
	mov	ES:[SI].item,DX 	;update the pointer
	jmp	DMIBC25
DMIBC20:
	push	SI			;update normal pointers
	mov	SI,BX
	mov	DX,ES:[DI].next
	mov	ES:[SI].next,DX
	pop	SI
DMIBC25:
	pop	DX
					;~~GOOD APP MENU UPDATE HERE
	test	[pifBehavior],BADBITS
	jnz	DMIBC70
	xor	CX,CX			;need a zero
	xor	DX,DX
	or	DX,MF_BYCOMMAND+MF_DELETE ;get the flags
	push	AX			;save the return value
					;do the deletion
	cCall	ChangeMenu,<ES:[SI].handle,ES:[DI].ID,CX,CX,CX,DX>
	pop	AX			;get result back
DMIBC70:
	call	ReturnMenuNode
	clc				;AX already has the pointer
	jmp	DMIBC90

DMIBC80:
	stc				;AX already has error code
DMIBC90:
	pop	ES
	pop	DI
	pop	SI
	ret

DeleteMenuItemBC	ENDP




;------------------ ModifyMenu -----------------------------------------
;
;	DESCRIPTION
;		This routine changes the appearance and/or functionality
;		of a menu.
;
;	ENTRY
;		AX: = pointer to a MenuInfo structure with new data
;		      for the menu
;		CX: = position of menu to be changed
;	EXIT
;		CF: = 0 if successful
;		CF: = 1 if failure
;		AX: = error code if CF: = 1
;		AX: = unchanged if CF: = 0
;
;	MODIFIES
;		AX, BX, CX, DX
;
;-----------------------------------------------------------------------

ModifyMenu	PROC	NEAR

	push	SI
	push	DI
	push	ES
	push	AX
	mov	BX,MyMenuSeg
	mov	ES,BX
	push	AX
	push	CX
	mov	AX,CX
	mov	BX,0FFFFH		;search by position
	call	GetMenuTitle		;get a pointer to the menu
	pop	CX
	pop	SI			;SI now points to new data
	jnc	CM2
	jmp	CM80			;error, AX has error code (need long jmp)
CM2:
	mov	DI,AX			;DI points to old data
	mov	AX,ES:[SI].ID
	cmp	AX,ES:[DI].ID		;are we changing the ID
	jz	CM10
	push	CX
	call	GetItem 		;check for duplicate
	pop	CX
	jc	CM10
	mov	AX,ERR_PARAMETERS
	jmp	CM80
CM10:
	xor	BX,BX			;clear destroy flag
	mov	AX,ES:[DI].flags
	xor	AX,ES:[SI].flags	;see what has changed
	test	AX,MF_POPUP		;has the popup status changed
	jz	CM30
	test	ES:[DI].flags,MF_POPUP	;are we going to a popup?
	jz	CM20			;yes
	push	DI			;if not we must delete all submenu items
	mov	DI,ES:[DI].item
CM12:
	or	DI,DI
	jz	CM14
	mov	AX,DI			;get node to return
	mov	DI,ES:[DI].next 	;get the next pointer
	push	CX			;save position
	call	ReturnMenuNode		;return the node
	pop	CX
	jmp	CM12
CM14:
	pop	DI			;get the pointer back
	jmp	CM30
CM20:					;we are changing to a popup
	test	[pifBehavior],BADBITS
	jnz	CM30
	push	CX
	push	BX
	push	ES
	cCall	CreateMenu
	pop	ES
	pop	BX
	pop	CX
	mov	ES:[DI].handle,AX	;get the menu handle in the struc
	mov	ES:[DI].item,0		;if new popup, no macro is allowed

CM30:					;here to change everything
					;popup has bee taken care of
					;ES:DI -> old data
					;ES:SI -> new data
					;CX = position
	Call	CopyText		;copy text item
	push	DX
	mov	DX,ES:[SI].ID
	mov	ES:[DI].ID,DX		;copy the new ID
	mov	DL,ES:[SI].twidth
	mov	ES:[DI].twidth,DL
	mov	DL,ES:[SI].mnem
	mov	ES:[DI].mnem,DL
	mov	DL,ES:[SI].mnempos
	mov	ES:[DI].mnempos,DL
	mov	DX,ES:[SI].flags
	mov	ES:[DI].flags,DX	;copy the style flags
	mov	AX,ES:[DI].handle	;set up in advance for good app junk
	test	ES:[DI].flags,MF_POPUP	;is this a popup
	jnz	CM32			;if popup, handle is already there
	mov	DX,ES:[SI].item
	mov	ES:[DI].item,DX 	;otherwise get the macro ID
	mov	AX,ES:[DI].ID		;macro not popup for good app call
 CM32:
	pop	DX
					;~~GOOD APP MENU UPDATE HERE
	test	[pifBehavior],BADBITS	;is it a good app
	jnz	CM70
	mov	SI,DI
	add	SI,text
	regptr	string,ES,SI		;create a pointer to the string
	mov	DX,ES:[DI].flags
	or	DX,MF_BYPOSITION+MF_CHANGE
				       ;do the change
	push	BX
	cCall	ChangeMenu,<hWinMenu,CX,string,AX,DX>
	cCall	DrawMenuBar,hTTYWnd
	pop	BX
CM70:
	pop	AX			;get original AX
	clc
	jmp	CM90

CM80:
	stc
	pop	CX			;make the stack right

CM90:
	pop	ES
	pop	DI
	pop	SI
	ret

ModifyMenu	ENDP


;---------------- ChangeMenuItem ----------------------------------------
;
;	DESCRIPTION
;		This routine changes the contents of a menu item in a
;		menu.  It is not for use on menu titles.  The ChangeMenu
;		function should be used for this.
;
;	ENTRY
;		AX: = pointer to a menu info structure with the new data
;		BX: = ID to change if MF_BYCOMMAND
;		BX: = position if MF_BYPOSITION
;		CX: = MenuTitleID if MF_BYPOSITION
;		CX: = 0 if MF_BYCOMMAND
;
;	EXIT
;		CF: = 0 if success
;		CF: = 1 if failure
;		AX: = error code if CF: = 1
;
;	MODIFIES
;		AX, BX, CX, DX
;
;--------------------------------------------------------------------------


ChangeMenuItem	PROC	NEAR

	push	SI
	push	DI
	push	ES
	mov	DX,MyMenuSeg
	mov	ES,DX
	or	CX,CX
	jz	CMI40			;go do change by command
	mov	SI,AX			;get in protected index reg
	push	BX			;save the position
	mov	AX,CX			;get the menu title ID
	xor	BX,BX			;search by ID
	call	GetMenuTitle
	pop	BX
	jnc	CMI8
	jmp	CMI80			;error, AX has error code (need long jmp)

CMI8:
	mov	DI,AX			;get menutitle pointer in index reg
	test	ES:[DI].flags,MF_POPUP	;is it a popup
	jnz	CMI10
	mov	AX,ERR_INVALID_OPERATION ;error code
	jmp	CMI80
CMI10:
	mov	DX,DI			;save the menu title pointer
	xor	CX,CX			;clear counter
	mov	DI,ES:[DI].item 	;get the first item
CMI11:
	or	DI,DI			;is it null
	jnz	CMI12
	mov	AX,ERR_INVALID_OPERATION ;error code
	jmp	CMI80
CMI12:
	cmp	CX,BX
	je	CMI60
	mov	DI,ES:[DI].next
	inc	CX
	jmp	CMI11

CMI40:					;updating by command

	mov	SI,AX			;get in index register
	mov	AX,BX
	call	GetItem 		;find the item in all of the menu junk
	jnc	CMI41
	jmp	CMI80			;error, AX has error code
CMI41:
	mov	DI,AX			;get pointer in index reg
	mov	AX,ES:[DI].handle	;get the Menu ID
	xor	BX,BX			;search by ID
					;get position so we can use same info
	call	GetMenuTitle
	jnc	CMI41a			;need a long jump
	jmp	CMI80			;error, AX has error code
CMI41a:
	mov	DX,AX			;save menu title pointer
	push	SI			;save pointer
	mov	SI,AX			;put in index reg
	xor	CX,CX
	mov	SI,ES:[SI].item 	;get first item
	xor	CX,CX			;zero count
CMI42:
	or	SI,SI			;is this the end
	jnz	CMI45
	pop	SI			;make the stack right
	mov	AX,ERR_INVALID_OPERATION ;error code
	jmp	CMI80
CMI45:
	cmp	DI,SI			;same item?
	jz	CMI50
	inc	CX
	mov	SI,ES:[SI].next
	jmp	CMI42
CMI50:
	pop	SI			;get pointer to new one back


CMI60:
					;we have the pointer
					;ES:DI -> old node
					;ES:SI -> new node
					;ES:DX -> menu title node
					;CX = position
					;we can work by position in either
					;case now
	mov	AX,ES:[SI].ID
	cmp	AX,ES:[DI].ID
	jz	CMI61
	push	CX
	call	GetItem 		;check for duplicate
	pop	CX
	jc	CMI61
	mov	AX,ERR_PARAMETERS
	jmp	CMI80
CMI61:
	cmp	ES:[DI].handle,SystemMenuID ; is it the system menu
	jnz	CMI62
	add	CX,FirstCMPos
CMI62:
	call	CopyText		;copy the text string
	push	AX
	mov	AX,ES:[SI].item
	mov	ES:[DI].item,AX 	;copy the macro ID
	mov	AL,ES:[SI].twidth
	mov	ES:[DI].twidth,AL
	mov	AL,ES:[SI].mnem
	mov	ES:[DI].mnem,AL
	mov	AL,ES:[SI].mnempos
	mov	ES:[DI].mnempos,AL
	mov	AX,ES:[SI].ID
	mov	ES:[DI].ID,AX		;copy the ID's
	mov	AX,ES:[SI].flags
	mov	ES:[DI].flags,AX
	pop	AX

					;~~GOOD APP MENU UPDATE HERE
	test	[pifBehavior],BADBITS	;is it a good app
	jnz	CMI70
	mov	SI,DX			;get pointer to menu title node
	mov	BX,DI
	add	BX,text
	regptr	string,ES,BX
	mov	DX,ES:[DI].flags
	or	DX,MF_CHANGE+MF_BYPOSITION
					;do the change
	cCall	ChangeMenu,<ES:[SI].handle,CX,string,ES:[DI].ID,DX>


CMI70:
	clc
	jmp	CMI90
CMI80:
	stc
CMI90:
	pop	ES
	pop	DI
	pop	SI
	ret

ChangeMenuItem	ENDP


;------------- VerifyFlags ----------------------------------------------
;
;	DESCRIPTION
;		This routine verifies that the flags passed by the
;		shell are  a valid combination of flags.
;
;	ENTRY
;		AX: = Flags passed to WinOldAp
;		BX: = Macro ID passed
;		CX: = Menu ID passed
;
;	EXIT
;		CF: = 1 if error in flags
;		CF: = 0 if flags are OK
;		IF CF: = 0
;			AX: = command flag
;			BX: = how flag
;			CX: = style flag
;
;	MODIFIES
;		AX, BX, CX, DX
;-------------------------------------------------------------------------

VerifyFlags	PROC	NEAR


	push	SI
	and	AX,SUPPORT_MASK 	;remove any unsupported flags first
					;these will not create errors (ignored)
	mov	SI,AX			;save the original flags


					; check the commands first
	and	AX,COMMAND_MASK 	;get only op code bits
	call	MultBits		;are there multiple bits set
	jc	VF80			;yes, then error
	mov	DX,AX			;command is temporarily in DX
	mov	AX,SI			;get all flags back


					;now check data types
	and	AX,DATA_MASK		;get data type bits
	call	MultBits		;are multiple bits set
	jc	VF80			;error if so
	test	AX,MF_SEPARATOR 	;is this a separator call
	jz	VF10			;no it is not, so jump
	or	CX,CX			;a separator may not be a menu title
	jz	VF80			;if Menu ID is null - error
	and	SI,DATA_MASK+HOW_MASK	;if separator, all other styles invalid
	jmp	VF20
VF10:
	test	AX,MF_POPUP		;is this a popup
	jz	VF20			;jump if not
	or	CX,CX			;if popup MenuID must be null
	jnz	VF80
	mov	AX,SI			;check for attributes
	test	AX,CHECK_MASK		;popups cannot have checkmarks
	jnz	VF80
	jmp	VF30
VF20:
	test	AX,MF_DISABLED		;is it disabled
	jnz	VF30			;no macro needed if disabled
	push	AX
	push	BX
	push	CX
	push	DX
	xor	AX,AX
	cCall	GetMacroInfo,<BX,AX,AX> ;check the macro, must be valid
	pop	DX
	pop	CX
	pop	BX
	or	AX,AX			;is there one
	pop	AX
	jz	VF80			;if not valid, then error
VF30:
					;now check for how mask
	mov	AX,SI
	or	CX,CX			;is this to be a menu title?
	jnz	VF32
	test	AX,MF_BYPOSITION	;menu title only valid with by position
	jz	VF80			;error if not
	test	AX,MF_CHECKED		;cannot be checked either
	jnz	VF80
VF32:
					;finally check styles
	and	AX,STYLE_MASK		;isolate the style bits
	call	MultBits		;no multiple styles enable,disable,gray
	jc	VF80			; ,remove

					;now report results
	mov	AX,DX			;get from temporary
	mov	BX,SI
	and	BX,HOW_MASK		;get the how flag
	mov	CX,SI
	and	CX,CHECK_MASK+DATA_MASK+STYLE_MASK
	clc
	jmp	VF90

VF80:
	stc

VF90:
	pop	SI
	ret

VerifyFlags	ENDP




;------------- UpdateMenu ------------------------------------------------
;
;	DESCRIPTION
;		This routine receives a MenuPoke structure from the shell
;		via the DDE handler.  The structure is filled with data
;		to be used to modify a menu or menu item.
;
;	ENTRY
;		parameters are received via C convention on the stack
;		in the following order:
;			Offset to a MenuPoke structure
;			Segment of MenuPoke structure
;
;	EXIT
;		AX: = error code if unsuccessful
;		AX: = 0 if successful
;		DX: = Menu ID if successful
;
;	MODIFIES
;		AX, BX, CX, DX
;
;--------------------------------------------------------------------------

cProc	UpdateMenu,<NEAR,PUBLIC>,<ES,SI,DI>
	parmD	MenuReq

cBegin	UpdateMenu

	call	LockMenuSeg
	or	DX,DX
	jnz	UM2
	mov	AX,ERR_NO_MEMORY
	jmp	UM80
UM2:
	mov	MyMenuSeg,DX		  ;save MenuSeg
	call	UnLockMenuSeg		;don't really need it now
	mov	BX,SEG_MenuReq
	mov	ES,BX
	mov	SI,OFF_MenuReq
	mov	AX,ES:[SI].wChange	;get the flags in AX
	mov	BX,ES:[SI].wMacro
	mov	CX,ES:[SI].wMenuID
	call	VerifyFlags		;check the flags
					;AX = operation
					;BX = adressing type
					;CX = style flags
	jnc	UM5			;jump if error
	mov	AX,ERR_INVALID_FLAGS
	jmp	UM80
UM5:
	test	AX,MF_DELETE		;are we deleting?
	jz	UM3
	jmp	UM60			;yes, then do it (need long jump)
UM3:
	push	SI			; convert text to ANSI if need be
	add	SI,wItem
	call	FixText
	pop	SI
	test	CX,MF_POPUP
	jz	UM3a
	cmp	byte ptr ES:[SI].wItem,' '    ;is first char a space
	jnz	UM3a
	mov	AX,ERR_PARAMETERS
	jmp	UM80
UM3a:
	test	CX,MF_SEPARATOR
	jnz	UM4
	push	SI
	push	AX
	call	GetMnemonic		;adjust length of string and get mnemonic
	mov	Temp,AX 		  ;save it for later
	add	SI,wItem		;get pointer to the string
	call	StringLength		;make sure it is legal
	mov	DX,AX			;temporary storage
	pop	AX			;get original data back
	pop	SI
	cmp	DX,MaxMenuWidth
	jle	UM4
	mov	AX,ERR_PARAMETERS	;error code
	jmp	UM80

UM4:
	test	AX,MF_APPEND			;if append ignore wIDChangeItem
	jnz	UM6
	cmp	ES:[SI].wIDChangeItem,SystemMenuID  ;check system menu
						   ;special case
	jnz	UM6
	mov	AX,ERR_PARAMETERS	;error code
	jmp	UM80
UM6:
	cmp	ES:[SI].wIDNewItem,LOWMENUID
	jl	UM6a
	cmp	ES:[SI].wIDNewItem,HIGHMENUID
	jle	UM7
UM6a:
	mov	AX,ERR_INVALID_ID
	jmp	UM80
UM7:
	push	AX
	call	GetMenuNode
	mov	DI,AX		       ;DI -> new node if it exists
	pop	AX
	jnc	UM10
	mov	AX,ERR_NO_MEMORY       ;error code
	jmp	UM80
UM10:
	push	DX			;lock the segment down
	call	LockMenuSeg
	mov	MyMenuSeg,DX
	pop	DX
					;copy the data
	push	DS
	push	AX
	mov	AX,MyMenuSeg		  ;get the menu seg in AX
	push	ES			;swap ES into DS
	mov	ES,AX			;and get MenuSeg into ES
	;the next two lines must be before the pop DS since
	;temp is in the Data Segment
	mov	ES:[DI].twidth,DL	 ;save the width of the menu item
	mov	DX,Temp 		;need high and low bytes separate
	pop	DS			;
	mov	ES:[DI].mnem,DL 	;put mnemonic and its position in
	mov	ES:[DI].mnempos,DH	;structure
	mov	AX,[SI].wIDNewItem
	mov	ES:[DI].ID,AX
	mov	ES:[DI].flags,CX
	mov	AX,[SI].wMacro
	mov	ES:[DI].item,AX
	mov	AX,[SI].wMenuID
	mov	ES:[DI].handle,AX
	pop	AX			;get the operation back
	mov	DX,AX			;get the operation in DX
	mov	CX,MaxMenuWidth+1	;set up for string count
	push	SI			;save these pointers
	push	DI
	add	SI,wItem		;adjust pointers
	add	DI,text
	rep	movsb			;copy the string
	pop	DI			;get pointers back
	pop	SI
	push	DS
	pop	ES			;get the seg regs back the way they were
	pop	DS

					;now determine the routine to call

	mov	AX,DI			;get menuinfo struc in AX
	mov	CX,ES:[SI].wMenuID	;get menu ID in CX

	test	DX,MF_APPEND		;is this an append command
	jz	UM20
	mov	BX,0FFFFH		;append flag
	or	CX,CX			;is it a menu
	jnz	UM12
	call	AddMenu 		;add the menu
	jmp	UM70
UM12:
	call	AddMenuItemBP		;append the menu item
	jmp	UM70

UM20:
	test	DX,MF_CHANGE		;is it a change call
	jz	UM30
	or	CX,CX			;is it a menu to change
	jnz	UM22
	mov	CX,ES:[SI].wIDChangeItem ;get position
	call	ModifyMenu
	jmp	UM26
UM22:
	test	BX,MF_BYPOSITION	;is this by position
	jz	UM24
	mov	BX,ES:[SI].wIDChangeItem
	mov	CX,ES:[SI].wMenuID
	jmp	UM25
UM24:
	mov	BX,ES:[SI].wIDChangeItem
	xor	CX,CX
UM25:
	call	ChangeMenuItem
UM26:
	jc	UM70
	pushf				;save flags
	push	AX			;save return value
	mov	AX,DI
	call	ReturnMenuNode		;return the temporary node
	pop	AX
	popf
	jmp	UM70


UM30:					;here to insert menus and items
	or	CX,CX			;is this a menu
	jnz	UM32
	mov	BX,ES:[SI].wIDChangeItem ;get position
	call	AddMenu 		;insert the menu
	jmp	UM70
UM32:
	test	BX,MF_BYPOSITION	;is it by position
	jz	UM34
	mov	BX,ES:[SI].wIDChangeItem ;get position
	call	AddMenuItemBP
	jmp	UM70
UM34:
	mov	BX,ES:[SI].wIDChangeItem ;get the ID to insert before
	call	AddMenuItemBC
	jmp	UM70

UM60:					;here for deletions
	mov	CX,ES:[SI].wMenuID
	or	CX,CX			;deleting a menu?
	jnz	UM62
	mov	AX,ES:[SI].wIDChangeItem ;get position
	call	DeleteMenu
	jmp	UM72
UM62:
	test	BX,MF_BYPOSITION
	jz	UM64
	mov	AX,CX			;get menu ID
	mov	BX,ES:[SI].wIDChangeItem ;position
	call	DeleteMenuItemBP
	jmp	UM72
UM64:
	mov	AX,CX			;get menu ID
	mov	BX,ES:[SI].wIDChangeItem ;get menu ID
	call	DeleteMenuItemBC
	jmp	UM72

UM70:
	jnc	UM72			;if no error don't have to return anything
					;need to return the node allocated if
					;an error occurred
	push	AX			;save return value
	mov	AX,DI
	call	ReturnMenuNode		;return the node previously acquired
	pop	AX
	jmp	UM80			;return the error code

UM72:
	xor	AX,AX			;return code
	mov	DX,ES:[SI].wIDNewItem	;return ID in DX

UM80:
	push	AX
	push	DX
	call	UnlockMenuSeg
	pop	DX
	pop	AX
UM90:

cEnd	UpdateMenu

;------------- FixText ----------------------------------------------
;
;	DESCRIPTION
;		This routine converts the OEM text string to Ansi
;		if the app is a good app.
;
;	ENTRY
;		ES:SI: = pointer to string
;
;	EXIT
;		If good app, wItem is converted to ANSI.
;
;	MODIFIES
;
;---------------------------------------------------------------------------
FixText PROC	NEAR
	test	ss:[pifbehavior],BADBITS
	jnz	FTRet
	push	ES
	push	AX
	push	BX
	push	CX
	push	DX
	regptr	essi,  es, si
	cCall	OemToAnsi,<essi,essi>
	pop	DX
	pop	CX
	pop	BX
	pop	AX
	pop	ES
FTRet:
	ret

FixText ENDP


;----------- UnFixText ----------------------------------------------
;
;	DESCRIPTION
;		This routine converts the ANSI text string to OEM
;		if the app is a good app.
;
;	ENTRY
;		ES:SI: = pointer to string
;
;	EXIT
;		If good app, wItem is converted to ANSI.
;
;	MODIFIES
;		none
;---------------------------------------------------------------------------
UnFixText PROC	  NEAR
	test	ss:[pifbehavior],BADBITS
	jnz	UFTRet
	push	ES
	push	AX
	push	BX
	push	CX
	push	DX
	regptr	essi,  es, si
	cCall	AnsiToOem,<essi,essi>
	pop	DX
	pop	CX
	pop	BX
	pop	AX
	pop	ES
UFTRet:
	ret

UnFixText ENDP









;------------- GetMnemonic ----------------------------------------------
;
;	DESCRIPTION
;		This routine parses the string of a menu item to find
;		the mnemonic character (tilde).  If it is a bad app
;		it removes the char from the string.  In either case
;		the charcter and position of the character in the
;		string are returned.
;
;	ENTRY
;		ES:SI: = pointer to menu poke structure
;
;	EXIT
;		If bad app mnemonic char is removed from string.
;		If a mnemonic exists:
;		    AH: = position od mnemonic in string
;		    AL: = mnemonic char
;		If no mnemonic	exists:
;		    AH: = 0FFH
;		    AL: = First char of string
;
;	MODIFIES
;		AX
;
;---------------------------------------------------------------------------
GetMnemonic	PROC	NEAR

	push	BX
	push	CX
	push	DI
	push	SI
	add	SI,wItem
	call	StringLength
	pop	SI
	mov	CX,AX
	dec	AX
	mov	BX,CX		;save length  (0 based)
	mov	DI,SI
	add	DI,wItem	;point at string
	add	DI,AX		;point at end of string
	std
	mov	AL,'&'          ;search for tilde
	repnz	scasb		;find it
	jz	GMFound
	mov	AH,0FFH 	;show not found
	mov	AL,byte ptr ES:[SI].wItem
	jmp	GMDone

GMFound:
	mov	AX,CX
	mov	AH,byte ptr ES:[DI+2]	;get the char
	xchg	AH,AL			;correct order
	test	pifbehavior,BADBITS	;is this a bad app
	jz	GMDone
	inc	DI			;point at tilde
	push	AX
	push	SI
	mov	SI,DI
	inc	SI
	sub	BX,CX
	mov	CX,BX		;how many to move
	cld
	push	DS
	mov	BX,ES
	mov	DS,BX
	rep	movsb
	xor	AL,AL
	stosb			;null terminator
	pop	DS
	pop	SI
	pop	AX		;get result back


GMDone:
	pop	DI
	pop	CX
	pop	BX
	ret

GetMnemonic	ENDP





;------------- CopyText --------------------------------------------------
;
;	DESCRIPTION
;		Copies the text string of a menu structure to another one.
;
;	ENTRY
;		ES:SI -> source structure
;		ES:DI -> destination structure
;
;	EXIT
;
;	MODIFIES
;		none
;
;--------------------------------------------------------------------------

CopyText	PROC	NEAR


	push	CX
	mov	CX,MaxMenuWidth+1	;get copy count
	push	SI
	push	DI
	add	SI,text 		;adjust the pointers
	add	DI,text
	mov	AX,ES
	push	DS			;set up DS
	mov	DS,AX
	rep	movsb			;do the string copy
	pop	DS			;get DS back
	pop	DI			;get the original pointers back
	pop	SI
	pop	CX
	ret

CopyText	ENDP

;------------------ MultBits ---------------------------------------------
;
;	DESCRIPTION
;		Counts the number of bits set in AX and notifies
;		caller if more than one is set.
;
;	ENTRY
;		AX: = register holding data
;
;	EXIT
;		CF: = 0 if 1 or less bits are set
;		CF: = 1 if more than 1 bit is set
;
;	MODIFIES
;
;--------------------------------------------------------------------------

MultBits	PROC	NEAR

	push	AX
	push	CX
	push	BX
	mov	CX,16			;16 bits in a register
	xor	BX,BX			;zero the count
MB10:
	ror	AX,1			;rotate one bit
	jnc	MB20
	inc	BX
	cmp	BX,1
	jg	MB80
MB20:
	loop	MB10
	clc
	jmp	MB90
MB80:
	stc
MB90:
	pop	BX
	pop	CX
	pop	AX
	ret

MultBits	ENDP




;----------------- StringLength -----------------------------------
;
;	DESCRIPTION
;		This routine gives the length of the null terminated
;		string passed.
;
;	ENTRY
;		ES:SI -> string
;
;	EXIT
;		AX: = count of chars not including null terminator
;
;	MODIFIES
;		AX
;
;---------------------------------------------------------------------
StringLength	PROC	NEAR

	push	SI
	push	BX
	push	DS			;SI is addressed via DS
	push	ES			;so get ES in DS
	pop	DS
	cld				;clear direction flag
	xor	BX,BX
SL10:
	lodsb
	or	AL,AL
	jz	SL80
	inc	BX
	jmp	SL10

SL80:
	mov	AX,BX
	pop	DS
	pop	BX
	pop	SI
	ret
StringLength	ENDP


;---------------PrepMenusForClose---------------------------------------
;
;	DESCRIPTION
;		This routine prepares all of the menus to wait for a close
;		command.  This routine is called after the app is terminated.
;		It is called before the menu is brought up for closing.  This
;		routine sort of goes in through the back door.	It does not
;		call UpdateMenu as this would take quite a bit more time.
;		The EnableMenuItem call is much simpler and requires less
;		information.
;
;	ENTRY
;
;	EXIT
;
;	MODIFIES
;		AX, BX, CX, DX, ES
;
;--------------------------------------------------------------------------
PrepMenusForClose	PROC	NEAR

	push	DI
	call	LockMenuSeg
	or	DX,DX			;are there menus
	jz	PMFCRet
	mov	ES,DX
	mov	DI,MenuTop
	mov	DX,MF_GRAYED+MF_BYPOSITION
	xor	CX,CX			;initialize the count
PMFC10:
	mov	DI,ES:[DI].next 	;get the next menu if there is one
	or	DI,DI
	jz	PMFCRet
	and	ES:[DI].flags,NOT STYLE_MASK
	or	ES:[DI].flags,MF_GRAYED
	test	pifbehavior,BADBITS
	jnz	PMFC20
	push	CX
	push	DX
	push	ES
	cCall	EnableMenuItem,<hWinMenu, CX, DX>
	pop	ES
	pop	DX
	pop	CX
PMFC20:
	inc	CX			;point to next one by position
	jmp	PMFC10

PMFCRet:
	test	pifbehavior,BADBITS
	jnz	PMFCRet1
	cCall	DrawMenuBar,hTTYWnd
PMFCRet1:
	call	UnLockMenuSeg
	pop	DI
	ret

PrepMenusForClose	ENDP

;----------------LockMenuSeg--------------------------------------------
;
;	DESCRIPTION
;		This routine returns the MenuSeg address in DX.
;
;	ENTRY
;		hMenuSeg valid for good apps
;		MenuSeg valid for bad apps
;
;	EXIT
;		DX: = the menu segment address
;		if DX is 0 then no buffer was created
;
;	MODIFIES
;		DX
;-------------------------------------------------------------------------
LockMenuSeg	PROC	NEAR

	cmp	OldApState,SF_NOT_STARTED
	je	LMS10
	test	[pifBehavior],BADBITS	;is it a bad app
	jnz	LMSBad
LMS10:
	xor	DX,DX
	cmp	hMenuSeg,0
	jz	LMSRet			; return none if no mem handle
	push	AX
	push	BX
	push	CX
	push	ES
	cCall	GlobalLock,hMenuSeg
	mov	CX,4
	shr	AX,CL
	add	DX,AX
	pop	ES
	pop	CX
	pop	BX
	pop	AX
	jmp	LMSRet

LMSBad:
	mov	DX,MenuSeg

LMSRet:
	ret

LockMenuSeg	ENDP

;-----------------UnlockMenuSeg-----------------------------------------
;
;	DESCRIPTION
;		Unlocks the global menu area if need be.
;
;	ENTRY
;
;	EXIT
;
;	MODIFIES
;
;-------------------------------------------------------------------------
UnlockMenuSeg	PROC	NEAR

	cmp	OldApState,SF_NOT_STARTED
	je	UMS10
	test	[pifBehavior],BADBITS
	jnz	UMSRet
UMS10:
	cmp	hMenuSeg,0			;nothing to unlock if handle
	jz	UMSRet				; null
	push	ES
	push	AX
	push	BX
	push	CX
	push	DX
	cCall	GlobalUnlock,hMenuSeg
	pop	DX
	pop	CX
	pop	BX
	pop	AX
	pop	ES

UMSRet:
	ret
UnlockMenuSeg	ENDP

;--------------- GetMenuList --------------------------------------------
;
;	DESCRIPTION
;		This routine passes back a list of menuitems in the
;		system that are accessable via the DDE protocol.  The
;		following is the format.
;
;		word 1	    number of words in the list
;		word 2 - n  menu ids in the following format:
;
;			     Menu Title ID
;			     Menu Item ID
;			     Menu Item ID
;				 :
;				 :
;			     0
;			     Menu Title ID
;			     Menu Item ID
;				 :
;				 :
;
;	ENTRY
;		Dummy: = unused dummy parameter
;		lpInfo: = long pointer to a data area to be filled
;			  if lpInfo is null only the size of the
;			  data area required is returned in AX
;
;	EXIT
;		lpInfo area filled if not null
;		AX: = count of words needed
;
;	MODIFIES
;		AX
;
;--------------------------------------------------------------------------
cProc	GetMenuList,<NEAR, PUBLIC>
	parmW	Dummy
	parmD	lpInfo

cBegin	GetMenuList
	push	ES
	push	CX
	push	DX
	push	DI
	push	SI
	push	DS
	cmp	MyMenuSeg,0		   ;is there a bufseg
	jnz	GML5
	jmp	GMLErr
GML5:
	mov	DI,OFF_lpInfo
	mov	AX,SEG_lpInfo
	xor	DL,DL			;clear count only flag
	or	AX,AX			;is the pointer null
	jnz	GML10
	mov	DL,1			;set the count only flag
GML10:
	mov	ES,AX			;get the segment in ES
	mov	SI,MenuTop		;get the first (system menu)
	push	DX			;save flag
	call	LockMenuSeg
	mov	MyMenuSeg,DX
	mov	DS,DX
	pop	DX
	xor	CX,CX			;clear count
	push	DI			;save original pointer
	inc	DI
	inc	DI			;point to first item position
	jmp	GML12			;always a system menu
GML11:
	or	SI,SI			;is the end
	jz	GML60			;done with list
GML12:
	xor	DH,DH			;enter item flag
	call	PutInList
	test	[SI].flags,MF_POPUP	;is this a popup
	jz	GML40			;don't proccess sub menu if so
	push	SI
	mov	SI,[SI].item		;get the first item in sub menu
GML15:
	or	SI,SI			;is this the last item
	jz	GML35			;end of sub menu
	xor	DH,DH			;enter item flag
	call	PutInList
	mov	SI,[SI].next
	jmp	GML15
GML35:
	pop	SI
GML40:
	mov	DH,1			;enter separator flag
	call	PutInList		;insert a separator
	mov	SI,[SI].next		;get next menu title
	jmp	GML11			;process this menu
GML60:
	pop	DI			;get back pointer to structure
	or	DL,DL
	jnz	GML70			;only put in count if need to
	mov	ES:[DI],CX		;enter the count
GML70:
	mov	AX,CX
	inc	AX			;add one word for count variable
	shl	AX,1			;2 bytes per word
	jmp	GMLRet
GMLErr:
	mov	DX,ERR_NO_MEMORY
	xor	AX,AX
GMLRet:
	pop	DS			;need DS for next call
	call	UnlockMenuSeg
	pop	SI
	pop	DI
	pop	DX
	pop	CX
	pop	ES

cEnd	GetMenuList

;------------------- PutInList ------------------------------------------
;
;	DESCRIPTION
;		This routine adds an item to the menu list if need be.
;		It also updates the count.
;
;	ENTRY
;		ES:DI -> next position in list
;		DS:SI -> menu info structure to be handled
;		DL: = 0 if list should be updated
;		DL: <> 0 if only count should be updated
;		DH: = 0 if item should be entered in list
;		DH: <> 0 if separator should be added only
;		CX: = count
;
;	EXIT
;		ES:DI updated if necessary
;		CX updated
;
;	MODIFIES
;		CX, DI
;
;-------------------------------------------------------------------------
PutInList	PROC	NEAR

	or	DL,DL
	jnz	PIL10
	or	DH,DH			;should we only do separator
	jnz	PIL5
	push	AX
	mov	AX,[SI].ID
	mov	word ptr ES:[DI],AX
	pop	AX
	inc	DI
	inc	DI
	jmp	PIL10
PIL5:
	mov	word ptr ES:[DI],0
	inc	DI
	inc	DI
PIL10:
	inc	CX			;update count
	ret
PutInList	ENDP



;-------------------- GetMenuInfo --------------------------------------
;
;	DESCRIPTION
;		This routine returns information on a specific menu item.
;		The format returned is that of the MenuPoke structure.
;		If the pointer passed is null only the size of the
;		MenuPoke structure is returned in AX.
;
;	ENTRY
;		wSearchItem: = ID of item to get info on
;		lpInfo -> pointer to a data area to be filled
;
;	EXIT
;		lpInfo is filled with data if applicable
;		AX: = size of structure
;		      or 0 if error
;		DX: = error code if error
;
;	MODIFIES
;		AX
;
;---------------------------------------------------------------------------
cProc	GetMenuInfo,<NEAR, PUBLIC>
	parmW	wSearchItem
	parmD	lpInfo

cBegin	GetMenuInfo
	push	ES
	call	LockMenuSeg
	mov	MyMenuSeg,DX
	mov	ES,DX
	mov	DI,OFF_lpInfo
	mov	AX,SEG_lpInfo
	or	AX,AX
	jnz	GetMI10
	mov	AX,size MenuPoke
	jmp	GetMIRet
GetMI10:
	push	BX
	push	CX
	push	SI
	push	DI
	xor	BX,BX			;search by ID
	mov	AX,wSearchItem
	call	GetMenuTitle		;let's see if it is a Menu Title
	jc	GetMI20
	xor	CX,CX			;CX = wMenuID
	mov	SI,AX
	jmp	GetMI50
GetMI20:
	mov	AX,wSearchItem
	call	GetItem
	jc	GetMI80 		;if carry is set it will report error
	mov	SI,AX			;get pointer in SI
	mov	CX,ES:[SI].handle

GetMI50:
	push	DI
	push	DS
	mov	DS,SEG_lpInfo		;get the lpInfo segment
	mov	DS:[DI].wIDChangeItem,0
	mov	AX,ES:[SI].ID
	mov	DS:[DI].wIDNewItem,AX
	mov	DS:[DI].wMenuID,CX	 ;the menu ID was saved in CX earlier
	xor	AX,AX			;get a zero
	test	ES:[SI].flags,MF_POPUP	;if it is a popup the macro is 0
	jnz	GetMI52
	mov	AX,ES:[SI].item
GetMI52:
	mov	DS:[DI].wMacro,AX
	mov	AX,ES:[SI].flags
	mov	DS:[DI].wChange,AX
	mov	CX,MaxMenuWidth
	inc	CX			;copy null terminator also
	add	DI,wItem
	add	SI,text
	pop	DS
	mov	ES,SEG_lpInfo
	push	DS
	mov	DS,MyMenuSeg
	push	DI
	cld
	rep	movsb
	pop	SI			; ES:SI points to wItem
	call	UnFixText		; shells expect OEM char set
	pop	DS
	pop	DI
	mov	AX,size MenuPoke
	jmp	GetMI90

GetMI80:
	xor	AX,AX
	mov	DX,ERR_PARAMETERS
GetMI90:
	pop	DI
	pop	SI
	pop	CX
	pop	BX
GetMIRet:
	call	UnlockMenuSeg
	pop	ES

cEnd	GetMenuInfo


;-------------MacroUpdateMenu---------------------------------------------
;
;	DESCRIPTION
;		This routine updates the flags and the macro ID for a
;		menu item.  This routine is called only by the macro
;		executor.
;
;	ENTRY
;		dMenuID - MenuID to be changed
;		dFlags	- flags to use
;		dMacroID - new macro ID, null if no change requested
;
;	EXIT
;		AX: = 0 if successful
;		AX: = error code if unsuccessful
;
;	MODIFIES
;		AX, BX, CX, DX
;
;----------------------------------------------------------------------------
cProc	MacroUpdateMenu,<PUBLIC, NEAR>
	parmW	dMenuID
	parmW	dFlags
	parmW	dMacroID


cBegin	MacroUpdateMenu

	push	ES
	push	SI
	push	DI
	call	LockMenuSeg
	or	DX,DX
	jnz	MUM10
	mov	AX,ERR_NO_MEMORY
	jmp	MUM90
MUM10:
	mov	ES,DX
	mov	AX,dMenuID		;get the item ID
	call	GetItem 		;get a pointer to the item
	jc	MUM80
	mov	SI,AX			;save the pointer
	mov	AX,dMenuID
	xor	BX,BX			;search by command
	call	GetMenuTitle		;see if it is a menu title
	jnc	MUM80			;if it is then it is an error
	mov	AX,dMenuID		;get the ID back
	call	GetItem 		;get the matching node
	jc	MUM80			;error if it don't exist guys
	mov	SI,AX
	mov	DI,dataOffset MacroUpdate
	mov	AX,dFlags		;get the flags
	or	AX,MF_CHANGE+MF_BYCOMMAND
	mov	DS:[DI].wChange,AX
	mov	AX,ES:[SI].ID
	mov	DS:[DI].wIDChangeItem,AX
	mov	DS:[DI].wIDNewItem,AX
	mov	AX,ES:[SI].handle
	mov	DS:[DI].wMenuID,AX
	mov	AX,dMacroID
	mov	DS:[DI].wMacro,AX
	mov	AX,DS			;swap segment regs
	mov	BX,ES
	mov	ES,AX
	mov	DS,BX
	mov	CX,MaxMenuWidth+1	;max length of string
	cld
	push	DI
	add	DI,wItem
	add	SI,text
	rep	movsb			;copy the text
	pop	DI
	mov	AX,DS			;swap the regs back
	mov	BX,ES
	mov	ES,AX
	mov	DS,BX
	regptr	DSDI,DS,DI
	cCall	UpdateMenu,DSDI 	;try to make the change
	jmp	MUM90


MUM80:
	mov	AX,ERR_PARAMETERS
MUM90:
	call	UnlockMenuSeg
	pop	DI
	pop	SI
	pop	ES

cEnd MacroUpdateMenu







ifdef	DEBUG

;-------------- ItoAPrint ----------------------------------------------
;
;	DESCRIPTION
;		This routine converts the number passed to chars in
;		hexadecimal and prints them.
;
;	ENTRY
;		AX: = number
;
;	EXIT
;
;	MODIFIES
;		AX
;
;--------------------------------------------------------------------------

ItoAPrint	PROC	NEAR

	push	BX
	push	CX
	push	DX
	mov	BX,AX			;get a copy
	xor	DX,DX			;port 0
	and	AX,0F000h		;isolate first char
	mov	CL,12
	shr	AX,CL
	call	ConvertChar
	xor	AH,AH
	int	17H
	mov	AX,BX
	and	AX,00F00H
	mov	CL,8
	shr	AX,CL
	call	ConvertChar
	xor	AH,AH
	int	17H
	mov	AX,BX
	and	AX,000F0H
	mov	CL,4
	shr	AX,CL
	call	ConvertChar
	xor	AH,AH
	int	17H
	mov	AX,BX
	and	AX,000FH
	call	ConvertChar
	xor	AH,AH
	int	17H
	pop	DX
	pop	CX
	pop	BX
	ret

ItoAPrint	ENDP

;---------------ConvertChar--------------------------------------------
;
;	DESCRIPTION
;		This routine converts a 4 bit code to an alphanumeric
;		character.
;
;	ENTRY
;		AL: = code
;
;	EXIT
;		AL: = character
;
;	MODIFIES
;		AX
;
;------------------------------------------------------------------------
ConvertChar	PROC	NEAR
	cmp	AL,9
	ja	CC10
	add	AL,030H
	jmp	CC90
CC10:
	sub	AL,10
	add	AL,41H
CC90:
	ret
ConvertChar	ENDP



;--------------PrintString------------------------------------------------
;
;	DESCRIPTION
;		This routine prints a zero terminated string.
;
;	ENTRY
;		DS:SI -> string
;
;	EXIT
;
;	MODIFIES
;		none
;
;-------------------------------------------------------------------------
PrintString	PROC	NEAR

	push	AX
	push	DX
	push	SI

	xor	DX,DX
	xor	AH,AH
PS10:
	mov	AL,byte ptr [SI]
	xor	AH,AH
	inc	SI
	or	AL,AL
	jz	PS90
	int	17H
	jmp	PS10
PS90:
	pop	SI
	pop	DX
	pop	AX
	ret
PrintString	ENDP

;-----------DumpData------------------------------------------------
;
;	DESCRIPTION
;		This routine dumps the MenuSeg data area.
;
;	ENTRY
;		AX: = number of nodes to dump
;
;	EXIT
;
;	MODIFIES
;		none
;
;---------------------------------------------------------------------

DumpData	PROC	NEAR

	push	AX
	push	CX
	push	DX
	push	DX
	push	SI
	push	DI
	push	ES
	mov	CX,AX			;get count in CX
	push	CX
	call	LockMenuSeg
	mov	MyMenuSeg,DX
DD10:
	mov	ES,MyMenuSeg
	mov	DX,ES
	pop	CX
	xor	DI,DI
DD20:
	mov	SI,dataOffset dbgNode
	call	PrintString
	mov	AX,DI
	call	ItoAPrint
	mov	SI,dataOffset dbgID
	call	PrintString
	mov	AX,ES:[DI].ID
	call	ItoAPrint
	mov	SI,dataOffset dbgflags
	call	PrintString
	mov	AX,ES:[DI].flags
	call	ItoAPrint
	mov	SI,dataOffset dbgitem
	call	PrintString
	mov	AX,ES:[DI].item
	call	ItoAPrint
	mov	SI,dataOffset dbgtext
	call	PrintString
	mov	SI,DI
	add	SI,text
	push	DS
	mov	DS,DX
	call	PrintString
	pop	DS
	mov	SI,dataOffset dbgwidth
	call	PrintString
	mov	AL,ES:[DI].twidth
	xor	AH,AH
	call	ItoAPrint
	mov	SI,dataOffset dbghandle
	call	PrintString
	mov	AX,ES:[DI].handle
	call	ItoAPrint
	mov	SI,dataOffset dbgnext
	call	PrintString
	mov	AX,ES:[DI].next
	call	ItoAPrint
	add	DI,MenuItemSize
	mov	AX,000AH
	int	17H
	mov	AX,000AH
	int	17H
	mov	AX,000DH
	int	17H
	loop	DD20

	call	UnlockMenuSeg
DD90:
	pop	ES
	pop	DI
	pop	SI
	pop	DX
	pop	CX
	pop	BX
	pop	AX
	ret

DumpData	ENDP
endif




sEnd	code
	END

