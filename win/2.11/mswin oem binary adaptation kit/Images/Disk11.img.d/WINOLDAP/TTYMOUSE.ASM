page ,132
;=======================================================================
;
; INT 33h Mouse Code
;
;	The following drivers are explicitly supported
;
;		Microsoft Mouse (all versions)
;		PC Mouse (versions 4.00 and later)
;		ATT-Mouse
;		HP Vectra Mouse (all versions)
;		HP Microsoft Mouse
;
;	In addition, any mouse driver which reasonably emulates
;	the Microsoft Mouse specs in the "Microsoft Mouse
;	Programming Guide" will be supported.
;
;
; History
;
;	Created for Windows 1.01 (in TTYSwap)
;
;	HP Added support for the HP Vectra mouse
;
;;	Modifications to this code made by W. S. Hall, AT&T IS, to support
;;	swap of the AT&T Mouse 6300 (Logitech) driver as well as the Microsoft
;;	mouse driver.  Areas of change can be found by searching for
;;	a double semicolon ';;'
;
;;	Code revised 6-13-86 to make it Logitech driver version independent
;;	by examining the jump at the beginning of the code and computing
;;	the length of the resident piece.
;
;	The code is rewritten to make it more readable and manageable
;	for Windows 2.0. Support is added for the new mouse state
;	functions. 6/11/87 Richard Hawes (HP)
;
;	The code is updated to support using the mouse in menu and
;	mark modes. 6/15/87 Richard Hawes (HP)
;
;	$Revision:   1.21  $
;	$Date:   09 Nov 1987 14:35:18  $
;
;==========================================================================
	.xlist
	include cmacros.inc
	include grabber.inc
	include mouse.inc
	.list

externNP	<IsVectra,SaveData,RestoreData> 	; TTYTASK
externNP	<GoGrab>				; TTYSWAP
externNP	<GoRealizeColor>
externNP	<SaveLinkMapping,RestoreLinkMapping>	; TTYSYS
externNP	<BadBeginPaint, BadEndPaint>

	public	SaveMouse,RestoreMouse,InquireMouse
	public	InitMouseVars, SaveNewMouse, RestoreNewMouse,
	public	SaveOldMouse, RestoreOldMouse, SaveHPMouse, RestoreHPMouse
	public	SaveATTMouse, RestoreATTMouse, SavePCMouse, RestorePCMouse
	public	SetMouse, GetMouseStatus
	public	SetMouseInterrupt
	public	MouseInterrupt, ConvertMouseCoords
	public	PrepMouse, ReleaseMouse
	public	EnableMouse, DisableMouse, MouseHook
	public	PrevMouseInterrupt, CheckButtonState, CheckMouseInterrupt
	public	PrepRAMMouse, ReleaseRAMMouse

;-----------------------------------------------
;
; Per task data stored in app DS
;
sBegin	DATA

;
; Pointers to mouse swap area if this app has one
;
externW 	MousePara
externW 	MouseSeg
;
; Grabber data structures
;
externB 	GrabData

sEnd	DATA

;------------------------------------------------
;
; Global data stored in CS
;
sBegin	CODE
assumes cs, CODE
assumes ds, DATA

;
; flag indicating this is a Vectra
;
externB 	fVectra

;
; Common Storage for the Microsoft Mouse driver state
;
globalD MouseVector, ?			; INT 33h vector
globalW MouseSize, 0			; The size of the mouse state in bytes
globalW pRestoreMouse, iqRet		; Pointer to the current save routine
globalW pSaveMouse, iqRet		; Pointer to the current restore
globalW HPMouseSize, HPMOUSE_SIZE	; Vectra Mouse size
globalW TrackSize, TRACK_SIZE		; Vectra Cursor size

globalW MouseCoords,0			; Current cursor pos
globalW MouseSave,0			; Current contents of cursor cell
globalB CursorStatus, 0 		; is cursor displayed or not
globalD MouseInt, 0			; address of old mouse int subroutine
globalW MouseMask, 0			; old mouse call mask
globalB MouseX, 0			; Scale factor X needed in CS
globalB MouseY, 0			; Scale factor Y needed in CS
globalB MouseInit, 0			; Non zero if mouse is initialized
globalB ButtonIntState, 0		; Actual state of the mouse buttons
globalB VerticalScale, 0		; Mickeys per row
globalB HorizontalScale, 0		; Mickeys per col
OldButtonState		db	0	; Area to save the old button state
DeltaY			dw	0
DeltaX			dw	0
GlobalB MouseGrab,0,GRABREQUESTLEN

externW fEndBlock


;***
;
; InitMouseVars - Initialize all the variables associated with the mouse.
;
;	This routine must be called first. It determines if a INT 33
;	mouse driver is present and then identifies its type.
;	In doing so, it calculates the size of the save area.
;
;	This function does not change the mouse driver state.
;
;	The mouse driver compatiblity is searched in the following order:
;
;		New Microsoft Mouse (4.0+)
;		New PC Mouse (5.0+)
;		ATT (Logitech)
;		Rom HP Mouse (A.01.02)
;		Ram HP Mouse (Patch versions)
;		Old Microsoft Mouse
;
;
; Entry
;	MouseSize is 0
;
; Exit
;	pSaveMouse, pRestoreMouse, MouseSize are set.
;
; Uses
;	C Convention
;
;
cProc	InitMouseVars, <NEAR>, <BP,DS,SI,DI>
localV	link, 32
cBegin
    ;
    ; Is a INT 33 mouse driver there?
    ;
	xor	ax,ax			; Look in INT table for a vector
	mov	es,ax
	les	bx, es:[33h*4]
	mov	word ptr cs:[MouseVector], bx	 ;save the vector
	mov	word ptr cs:[MouseVector+2], es
	mov	cx,es
	or	cx,bx
	jz	imLExit 		; int 33 vector not stuffed, so no mouse
	cmp	byte ptr es:[bx], 0CFh	; make sure it is not an IRET
	jne	imIsMouse
imLExit:jmp	imExit
    ;
    ; There is a mouse driver: Save the INT33 vector
    ;
imIsMouse:
    ;
    ; Vectras need there link mapping saved and restored by this
    ; function.
    ;
	push	ss
	pop	es
	lea	di, link
	call	SaveLinkMapping 	; This function has no effect on
					; non Vectras.
    ;
    ; Is this a Vectra? If so, Is this the wonderfull ROM mouse driver?
    ;
imRomHPMouse:
	call	IsVectra		; Is this a Vectra?
	jz	imNewMouse		;   No, Can't be a HP Mouse
	mov	bx, word ptr cs:[MouseVector+2]
	cmp	bx, 0F000H		; Dose Int 33 point to ROM
	jne	imRamHPMouse		;   No, it might be a RAM HP Mouse
	jmp	imHPDef
    ;
    ; Is this patch version of the HP Mouse which is ram based ?
    ;
    ; Notice that I don't make this call with ROM mouse drivers
    ; because the first ROM mouse driver barfs on this call. Great
    ; code!!!
    ;
imRamHPMouse:
	xor	bx, bx			; Does this ID as an HP mouse?
	mov	AX, F33_INQUIRE 	;
	int	33H			;
	cmp	BX, 'HP'                ; HP ID Code (new version)
	jne	imNewMouse
      ;
      ; HPMOUSE:  Inquire LHPMOUSE to get the HPmouse and TRACK sizes
      ;
imHPSize:
	mov	AX, F_IO_CONTROL SHL 8 + SF_MOUSE_DSIZE
	mov	bp, V_LHPMOUSE
	int	6fh
      ;
      ; Returns:
      ;  AH=  RS_UNSUPPORTED - Use defaults
      ;  AH=  RS_SUCCESSFUL
      ;       BH= HPMOUSE size in paras
      ;       BL= TRACK size in paras
      ;
	cmp	ah, 0
	jne	imHPDef 		; Use default size
	mov	al, bh			; convert size in bytes and save
	mov	cl, 4
	shl	al, cl
	mov	cs:[HPmouseSize], ax
	xor	bh, bh
	shl	bx, cl
	mov	cs:[TrackSize], bx
imHPDef:
	mov	ax, cs:[HPMouseSize]
	add	ax, cs:[TrackSize]
	mov	cs:[MouseSize], ax
	mov	cs:[pSaveMouse], codeOffset SaveHPMouse
	mov	cs:[pRestoreMouse], codeOffset RestoreHPMouse
	jmp	imRestore
    ;
    ;
    ; Does it support Microsoft's function 21?
    ;
    ;	If so, its Microsoft Mouse version 4.0+ compatible.
    ;	This is prefered method of saving the mouse state.
    ;
imNewMouse:
	mov	ax, 21
	xor	bx, bx
	int	33h
	cmp	bx, 0
	je	imPCMouse
	mov	cs:[pSaveMouse], codeOffset SaveNewMouse
	mov	cs:[pRestoreMouse], codeOffset RestoreNewMouse
	mov	cs:[MouseSize], bx
	jmp	imRestore

    ;
    ; Does it support Mouse System's function B
    ;
    ;	If so, it's Mouse System version 5.0+ compatible
    ;
imPCMouse:
	mov	ax, 'B'
	xor	bx, bx
	int	33h
	cmp	bx, 0
	je	imATTMouse
	mov	cs:[pSaveMouse], codeOffset SavePCMouse
	mov	cs:[pRestoreMouse], codeOffset RestorePCMouse
	mov	cs:[MouseSize], bx
	jmp	imRestore
    ;
    ; Is it a ATT(Logitech) mouse driver
    ;
imATTMouse:
	les	bx, cs:[MouseVector]
	cmp	es:[bx+10h],'OL'        ;; see if logitech there
	jne	imOldMouse		;;
	mov	ax, '.1'                ; Further check requested by
	cmp	es:[bx+27h], ax 	;  Bill Hall
	je	imFoundATT		;
	cmp	es:[bx+28h], ax 	;
	jne	imOldMouse		;
imFoundATT:
	mov	ax,es:[101h]		;; Yes, calculate the save area
	add	ax,103h 		;; relative jmp + len of psp + 3 = dist to blowaway code
	mov	cs:[MouseSize],ax	;; save it
	mov	cs:[pSaveMouse], codeOffset SaveATTMouse
	mov	cs:[pRestoreMouse], codeOffset RestoreATTMouse
	jmp	imRestore

    ;
    ; Is this an old Microsoft Mouse driver 2.0 or less
    ;
imOldMouse:
	mov	ax,'M'
	int	33h
	cmp	word ptr es:[di],'oC'   ; Does secret mouse call point at
					; copyright message?
	jne	imExit			;   No, no mouse present.
	mov	cs:[pSaveMouse], codeOffset SaveOldMouse
	mov	cs:[pRestoreMouse], codeOffset RestoreOldMouse
	mov	ax, word ptr cs:[MouseVector]
	mov	cs:[MouseSize], ax
	errn$	imRestore
   ;
   ; Restore Vectra's HP-HIL mapping
   ;
imRestore:
	push	ss
	pop	es
	lea	di, link
	call	RestoreLinkMapping
imExit:
cEnd	InitMouseVars


;***
;
; InquireMouse - Return size in PARA of INT 33 mouse driver save area
;
; ENTRY:
;	MouseSize set
; EXIT:
;	AX is size of mouse save area in para
; USES:
;	AX,CX,FLAGS
;
InquireMouse:
	mov	ax, cs:[MouseSize]	; Get offset
	add	ax,15			; Para round up
	mov	cl,4			; Div by 16 for paras
	shr	ax,cl			; Convert to paras
	jz	iqRet			; if zero then no need for link mapping
	test	cs:[fVectra], 1 	; Is this a Vectra?
	jz	iqRet			;  No, OK
	inc	ax			;  Yes, add two for the link mapping.
	inc	ax
iqRet:	ret



;***
;
; SaveMouse - Save the state of the INT 33 driver
;
;	This function may not be called twice in a row with out
;	a corresponding RestoreMouse call. ;
;
; ENTRY:
;	SS = DS = Data of app to save mouse state into
; EXIT:
;	Mouse state saved in app partition if appropriate
;	Mouse re-initialized and turned off
; USES:
;	C Convention less ES
;
cProc	SaveMouse, <NEAR>, <ES,DS,SI,DI>
cBegin
	mov	cx, [MouseSeg]
	jcxz	smsx			; No mouse area, call is no-op
	mov	es, cx
    ;
    ; If this is a Vectra the link mapping needs to be saved.
    ;
	test	cs:[fVectra], 1
	jz	smNotVectra
	xor	di, di			;Save the link mapping
	call	SaveLinkMapping
	mov	bx, es			; the mouse save area is next
	inc	bx
	inc	bx
	mov	es, bx

smNotVectra:
	mov	cx, cs:[MouseSize]
	call	cs:[pSaveMouse]
    ;
    ; Put the mouse in the default state with cursor hidden.
    ;
    ; This done so that when we start up another app the it
    ; will not have the cursor displayed initially.
    ;
	xor	ax, ax
	int	33h
smsx:
cEnd

;***
;
; RestoreMouse - Restore the state of the Microsoft Mouse
;
; ENTRY:
;	SS = DS = Data of app to restore mouse state from
; EXIT:
;	Mouse state restored from app partition if appropriate
; USES:
;	C Convention less ES
;
cProc	RestoreMouse, <NEAR>, <ES,DS,SI,DI>
cBegin
	mov	cx, [MouseSeg]
	jcxz	rmx
   ;
   ; If this is a Vectra the link mapping must be restored. However
   ; the mouse must restored first because this operation will
   ; destroy the link mapping.
   ;
	test	cs:[fVectra], 1 	; Is this a Vectra?
	jz	rmCall
	push	cx			; Save
	inc	cx
	inc	cx
rmCall:
	mov	es, cx
	mov	cx, cs:[MouseSize]
	push	DS			; save DS for RestoreLinkMapping
	call	cs:[pRestoreMouse]
	pop	DS
   ;
   ; Restore the link mapping
   ;
	test	cs:[fVectra], 1
	jz	rmx
	pop	es
	xor	di, di
	call	RestoreLinkMapping
rmx:
cEnd

;***
;
; SaveNewMouse - Save the new mouse state using the function 22 code
;
; Entry
;	ES:0 - mouse segment
;
; Exit
;	MouseSeg is updated
;
; Uses
;	AX,BX,CX,DX,SI,DI,ES,DS
;
SaveNewMouse:
	mov	ax, 22
	jmp	nmsEntry		; Use the RestoreNewMouse code


;***
;
; RestoreNewMouse - Restore the new mouse state using the function 23 code.
;
; Entry
;	ES:0 - mouse segment
;	CX   - Mouse size
;
; Exit
;	The mouse driver state restored
;
; Uses
;	AX,BX,CX,DX,SI,DI,ES,DS
;
RestoreNewMouse:
	RESET_MOUSE
	mov	ax, 23
nmsEntry:
	xor	dx, dx
	int	33h
	ret


;***
;
; SavePCMouse - Save the PC mouse state using the P function.
;
; Entry
;	ES:0 - mouse segment
;	CX   - Mouse size
;
; Exit
;	MouseSeg is updated
;
; Uses
;	AX,BX,CX,DX,SI,DI,ES,DS
;
SavePCMouse:
	mov	ax, 'P'
	jmp	pmsEntry		; Use the RestorePCMouse code

;***
;
; RestorePCMouse - Restore the PC Mouse state using the R function.
;
; Entry
;	ES:0 - mouse segment
;	CX   - Mouse size
;
; Exit
;	The mouse driver state restored
;
; Uses
;	AX,BX,CX,DX,SI,DI,ES,DS
;
RestorePCMouse:
	mov	ax, 'R'
pmsEntry:
	mov	bx, cx
	xor	dx, dx
	int	33h
	ret

;***
;
; SaveATTMouse
;
;
; Entry
;	ES:0 - mouse segment
;	CX   - Mouse size
;
; Exit
;	MouseSeg is updated
;
; Uses
;	AX,BX,CX,DX,SI,DI,ES,DS
;
;
SaveATTMouse:
	mov	ax,2			;; hide cursor
	int	33h			;;
	jmp	SaveOldMouse

;***
;
; RestoreATTMouse
;
; Entry
;	ES:0 - mouse segment
;	CX   - Mouse size
;
; Exit
;	The mouse driver state restored
;
; Uses
;	AX,BX,CX,DX,SI,DI,ES,DS
;
RestoreATTMouse:
	mov	ax, es
	mov	ds, ax
	mov	es, word ptr cs:[MouseVector+2] ; Dest is mouse driver
	call	MoveMouse	; Blat it in
	xor	cx,cx
	xor	dx,dx
	xor	si,si
	xor	di,di
	mov	ax,16		; show mouse cursor if it was on
	int	33H
	mov	ax,32		;; get mode we left it in
	int	33h

	mov	ax,33		;; set the mode
	int	33h

	call	MoveMouse
	mov	ax,1		;; restore the cursor
	int	33h
	ret

;**
;
; SaveHPMouse - Save the HPMouse state by copying its data area.
;
;	The HP Mouse has two data areas V_LHPMOUSE and V_STRACK.
;	See the "HP Vectra Techinical Reference Manual" for details.
;	A new HP-HIL Mouse Microsoft Mouse driver has been released
;	by HP which was written by Microsoft. The NewHPMouse code
;	is used for this driver.
;
; Entry
;	ES:0 - mouse segment
;	CX   - Mouse size
;
; Exit
;	The mouse driver state is saved
;
; Uses
;	AX,BX,CX,DX,SI,DI,ES,DS
;
SaveHPMouse:
	EnterCrit
	xor	di, di			; MOUSE must be first
	mov	ax, V_LHPMOUSE
	mov	si, cs:[HPMouseSize]
	cCall	SaveData, <ax, di, si, di> ; Vector, start, end, loc
	add	di, si			; Save TRACK
	mov	ax, V_STRACK
	mov	si, cs:[TrackSize]
	xor	dx,dx
	cCall	SaveData, <ax, dx, si, di>
	LeaveCrit
	HIDE_MOUSE_CURSOR
	ret

;**
;
; RestoreHPMouse
;
;	The HP Mouse has two data areas V_LHPMOUSE and V_STRACK.
;	See the "HP Vectra Techinical Reference Manual" for details.
;	A new HP-HIL Mouse Microsoft Mouse driver has been released
;	by HP which was written by Microsoft. The NewHPMouse code
;	is used for this driver.
;
; Entry
;	ES:0 - mouse segment
;	CX   - Mouse size
;
; Exit
;	The mouse driver state is restored
;
; Uses
;	AX,BX,CX,DX,SI,DI,ES,DS
;
RestoreHPMouse:
	xor	ax, ax
	int	33h
	EnterCrit
	xor	di, di			; MOUSE is always first
	mov	ax, es
	mov	ds, ax
	mov	ax, V_LHPMOUSE
	mov	si, cs:[HPMouseSize]
	cCall	RestoreData, <ax, di, si, di>; Vector, start, end, loc
	add	di, si			; Restore TRACK
	mov	ax, V_STRACK
	mov	si, cs:[TrackSize]
	xor	dx,dx
	cCall	RestoreData, <ax, dx, si, di>
	LeaveCrit
	ret

;**
;
; SaveOldMouse - Save the mouse state by copying the data area
;
;	As a freak of nature, old Microsoft mouse drivers stored
;	there data from CS:0 to CS:Entry. This allows us
;	to save the data area even though there are now explicit
;	calls to save it. However, not all the state might be
;	there.
;
; Entry
;	ES:0 - mouse segment
;	CX   - Mouse size
;
; Exit
;	The mouse driver state is saved
;
; Uses
;	AX,BX,CX,DX,SI,DI,ES,DS
;
SaveOldMouse:
	mov	ds,word ptr cs:[MouseVector+2]	; Source is mouse driver seg
	call	MoveMouse		;
	ret
;**
;
; RestoreOldMouse - Restore the old mouse data area.
;
;	As a freak of nature, old Microsoft mouse drivers stored
;	there data from CS:0 to CS:Entry. This allows us
;	to save the data area even though there are now explicit
;	calls to save it. However, not all the state might be
;	there.
;
; Entry
;	ES:0 - mouse segment
;	CX   - Mouse size
;
; Exit
;	The mouse driver state is saved
;
; Uses
;	AX,BX,CX,DX,SI,DI,ES,DS
;
RestoreOldMouse:
	mov	ax, es
	mov	ds, ax
	mov	es, word ptr cs:[MouseVector+2] ; Dest is mouse driver
	call	MoveMouse	; Blat it in
	xor	cx,cx
	xor	dx,dx
	xor	si,si
	xor	di,di
	mov	ax,16		; show mouse cursor if it was on
	int	33H
	errn$	MoveMouse	; RestoreMouse falls into MoveMouse

;**
;
; MoveMouse - Transfer MouseSize bytes of mouse area
;
; ENTRY:
;	DS:0 -> Source of mouse area
;	ES:0 -> Dest of mouse area
;	MouseSize is set
;
; EXIT:
;	Mouse area moved
;
; USES:
;	DI,SI,CX,DS,FLAGS
;
MoveMouse:
	cld
	xor	di,di
	xor	si,si
	mov	cx, cs:[MouseSize]
	shr	cx,1
	EnterCrit			; No Mouse INTs while copying
	rep	movsw
	LeaveCrit
	ret

;***
;
; EnableMouse DisableMoues - Install and uninstall the INT33 mouse hook
;
; Entry
;
; Exit
;
; Uses
;	Convention
;
EnableMouse:
	xor	ax, ax			; ES:BX -> int 33 vector
	mov	es, ax
	mov	bx, 33h*4
	mov	ax, codeOffset MouseHook; DX:AX -> MouseHook
	mov	dx, cs
	mov	es:[bx], ax		; Install MouseHook
	mov	es:[bx+2], dx
	ret

DisableMouse:
	xor	ax, ax			; ES:BX -> int 33 vector
	mov	es, ax
	mov	bx, 33h*4
	mov	ax, word ptr MouseVector; DX:AX -> MouseVector
	mov	dx, word ptr MouseVector + 2
	mov	es:[bx], ax		; Install the real mouse driver
	mov	es:[bx+2], dx
	ret

;***
;
; MouseHook - Dummy INT 33 mouse driver.
;
;	This dummy driver shows that the mouse is not available.
;	It is installed when good applications run. It was neccessary
;	because some "Good" (ie MultiPlan) do blind int 33 calls
;	which will mess up some mice.
;
; Entry
;
; Exit
;
; Uses
;
MouseHook:
	iret


;***
;
; SetMouse - Set the mouse to a known state
;
;
; Entry
;	AX - bit 0 - Show cursor
;	     bit 1 - DX contain a new position:
;
;	     DL - X coordinate in grabber coordinates
;	     DH - Y coordinate
;
; Exit
;	Mouse driver state is set
;	GrabData is updated
;
; Uses
;	C Convention less ES
;
cProc	SetMouse, <NEAR>, <SI,DI,ES>
localW	mode
cBegin
	test	CS:MouseInit,MOUSE_INIT
	jnz	smBegin
	jmp	smExit

smBegin:
	mov	mode, ax
    ;
    ; Reset the mouse
    ;
	test	mode, SM_RESET
	jz	smPosition
	RESET_MOUSE
	push	DX			;in case of set position
	GET_MOUSE_STATUS		;get position from driver
	call	ConvertMouseCoords
	mov	CS:MouseCoords,DX
	pop	DX
    ;
    ; Set the mouse marker
    ;
smPosition:
	test	mode, SM_SET_POSITION
	jz	smCursor
	mov	bx, dx
	cli				;no mouse ints now
	mov	AL,CS:CursorStatus
	or	AL,AL
	push	AX
	jz	smPos10
	call	RemoveCursor
smPos10:
	mov	CS:MouseCoords,dx
	mov	AL,DL
	mov	BL,MOUSE_CELL_WIDTH
	imul	BL
	mov	CX,AX
	mov	AL,DH
	mov	BL,MOUSE_CELL_HEIGHT
	imul	BL
	mov	DX,AX
	SET_MOUSE_POSITION
	pop	AX
	or	AL,AL			;Was the cursor on
	jz	smPos20
	call	ShowCursor
smPos20:
	sti
    ;
    ; Show the cursor if set
    ;
smCursor:
	test	mode, SM_SHOW_CURSOR
	jz	smHide
	call	ShowCursor
    ;
    ; Hide the cursor if set
    ;
smHide:
	test	mode, SM_HIDE_CURSOR
	jz	smExit
	call	RemoveCursor

smExit:
cEnd


;***
;
; PrepMouse - This routine initializes all Mouse variables for use of the mouse
;
; Entry
;	GrabData must be correct
;	BeginPaint has been called
;
; Exit
;
; Uses
;	C convention less ES
;
PrepMouse:
	push	DI
	test	CS:MouseInit,MOUSE_INIT+MOUSE_REMOVED
	jnz	PrepMouseRet
Prep10:
	cmp	MouseSeg,0
	jz	PrepMouseRet
Prep20:
	lea	DI,SaveHPMouse
	cmp	[pSaveMouse],DI
	jnz	Prep30
	call	SaveMouse			;resets (i.e. freezes) mouse
	mov	CS:MouseInit,MOUSE_REMOVED
	jmp	PrepMouseRet
Prep30:
	call	PrepRAMMouse

PrepMouseRet:
	pop	DI
	ret





;***
;
; PrepRAMMouse - This routine initializes all Mouse variables for use of the
;		 mouse
;
; Entry
;	GrabData must be correct
;	BeginPaint has been called
;
; Exit
;
; Uses
;	C convention less ES
;
PrepRAMMouse:
	push	DI
	lea	DI,GrabData
	mov	AL,[DI].giCharsX
	dec	AL
	mov	CS:MouseX,AL
	mov	AL,[DI].giCharsY
	dec	AL
	mov	CS:MouseY,AL
	mov	CS:HorizontalScale,4
	mov	CS:VerticalScale,8
	GET_MOUSE_STATUS		    ;set up mouse with same pos
	mov	CS:OldButtonState,BL
	mov	CS:ButtonIntState,0	    ;initialize mouse to button up
	call	ConvertMouseCoords
	push	DX
	cCall	SaveMouse		    ; Mouse is reset by this call
	mov	CS:MouseInit,MOUSE_INIT      ;Must be here for call to SetMouse
	mov	AL,CS:MouseX
	inc	AL
	mov	BL,MOUSE_CELL_WIDTH
	mul	BL
	xor	CX,CX
	mov	DX,AX
	mov	AX,7
	int	33H			;set min and max horz cursor position
	mov	AL,CS:MouseY
	inc	AL
	mov	BL,MOUSE_CELL_HEIGHT
	mul	BL
	xor	CX,CX
	mov	DX,AX
	mov	AX,8
	int	33H			;set min and max vert cursor position
	call	ResetMickey
	call	SetMouseInterrupt
	call	BadEndPaint		; this will remove the hardware cursor
	call	BadBeginPaint		; if it has moved since prev BeginPaint
	pop	DX
	mov	AX,SM_MENU_MODE
	call	SetMouse
	pop	DI
	ret




;***
;
; ReleaseMouse - This routine restoress all Mouse variables for use of the
;		 mouse
;
; Entry
;	GrabData must be correct
;	BeginPaint has been called
;
; Exit
;
; Uses
;	C convention less ES
;

ReleaseMouse:
	test	CS:MouseInit,MOUSE_INIT+MOUSE_REMOVED
	jz	ReleaseMouseRet
	cmp	CS:[fEndBlock],0
	je	ReleaseMouseRet
	test	CS:MouseInit,MOUSE_REMOVED
	jz	RelMouse10
	call	RestoreMouse
	jmp	RelMouse95
RelMouse10:
	call	ReleaseRAMMouse
RelMouse95:
	mov	CS:MouseInit,MOUSE_CLEAR
ReleaseMouseRet:
	ret


;***
;
; ReleaseRAMMouse - This routine restores all Mouse variables for use of the
;		 mouse
;
; Entry
;	GrabData must be correct
;	BeginPaint has been called
;
; Exit
;
; Uses
;	C convention less ES
;
ReleaseRAMMouse:
	push	ES
	call	GetMouseStatus
	push	DX
	mov	AX,SM_HIDE_CURSOR
	call	SetMouse
	call	BadEndPaint		; This will relieve mouse jumping
					; if hardware cursor is used, must
					; be used with the following BeginPaint
	cCall	RestoreMouse
	pop	DX
	push	DX
	mov	AX,SM_SET_POSITION
	call	SetMouse		; won't use grabber since cursor hid
	call	CheckMouseInterrupt
	pop	BX
	test	CX,BUTTON_FLAGS
	jz	RelRAMMouse90
	push	BX
	push	CX
	call	CheckButtonState
	pop	CX
	pop	BX
	test	AX,CX			; did any changes occur that we care
	jz	RelRAMMouse90
	call	PrevMouseInterrupt
RelRAMMouse90:
	call	BadBeginPaint
	pop	ES
	ret



;***
;
; CheckButtonState - Checks to see if the button state has changed
;		     since going into menu mode.
;
;
;
;
; Entry
;	CS:OldButtonState = Button state prior to Menus
;	CS:ButtonIntState = Button State now
;
; Exit
;	AX = Flags to use for subroutine interrupt call
;
; Uses
;	C convention less ES
;
CheckButtonState:
	xor	AX,AX			;assume no change
	mov	BL,CS:ButtonIntState
	mov	BH,CS:OldButtonState	;get Old Button State
	xor	BH,BL			;have they changed
	jz	CBSRet
	test	BH,01H			;Left button changed?
	jz	CBS10
	mov	AX,LEFT_DOWN		;assume down
	test	BL,LEFT_STATE		;is it down
	jnz	CBS10
	shl	AX,1			;no it is up
CBS10:
	test	BH,02H			;did right button  change
	jz	CBSRet
	mov	CX,RIGHT_DOWN		;assume down
	test	BL,RIGHT_STATE
	jz	CBS20
	shl	CX,1			;no it is up
CBS20:
	or	AX,CX			;combine right and left button
CBSRet:
	ret


;***
;
; PrevMouseInterrupt - Calls the previous mouse interrupt to update
;			the mouse
;
;
; Entry
;	AX	= Flags to use for call
;	BX	= Current cursor pos in grabber coordinates
;	ES:DX	= Vector to Mouse Interrupt routine
;
; Exit
;
; Uses
;	C convention less ES
;
PrevMouseInterrupt:
	push	SI
	push	DI
	push	DS
	push	ES
	push	AX				; save flags for later
	push	BX				; save pos for later
	mov	word ptr CS:MouseInt,DX
	mov	word ptr CS:MouseInt+2,ES

	mov	AL,BL
	mov	BL,MOUSE_CELL_WIDTH
	imul	BL
	mov	CX,AX
	pop	BX				;get pos back
	mov	AL,BH
	mov	BL,MOUSE_CELL_HEIGHT
	imul	BL
	mov	DX,AX
	xor	SI,SI
	xor	DI,DI
	xor	BX,BX
	mov	BL,CS:ButtonIntState
	pop	AX				; get flags for call back
	call	CS:MouseInt			;call their interrupt routine
	pop	ES
	pop	DS
	pop	DI
	pop	SI
	ret


;***
;
; GetMouseStatus - Returns the mouse status in grabber coordinates
;
;	A transformation is done between Mouse Virtual coordinates
;	to the grabber character coordinates.
;
; Entry
;
; Exit
;	BX - Button status: bit 0 - left     0 if no mouse present
;			    bit 1 - right
;			    bit 2 - middle
;
;	DL - X coordinate (Grabber coordinates)
;	DH - Y coordinate
;
; Uses
;	C convention less ES
;
GetMouseStatus:
    ;
    ; Is a mouse present?
    ;
	xor	bx, bx
	test	CS:MouseInit,MOUSE_INIT
	jz	gmsRet
    ;
    ; Yes, get the status
    ;
	mov	DX,CS:MouseCoords
	mov	BL,CS:ButtonIntState
gmsRet:
	ret


;***
;
; ConvertMouseCoords - Converts mouse driver coordinates to grabber
;		       coordinates
;
;
; Entry
;		CX = Horizontal coordinate
;		DX = Vertical Coordinate
;
; Exit
;
;	DL - X coordinate (Grabber coordinates)
;	DH - Y coordinate
;
; Uses
;	C convention less ES
;
ConvertMouseCoords:

	mov	AX,DX
	mov	BL,MOUSE_CELL_HEIGHT
	idiv	BL
	mov	DH,AL
	mov	AX,CX
	mov	BL,MOUSE_CELL_WIDTH
	idiv	BL
	mov	DL,AL
	ret



;***
;
; SetMouseInterrupt - This routine calls the mouse driver to set up
;		      the new interrupt routine.
;
;
; Entry
;
; Exit
;
; Uses
;	C convention less ES
;
SetMouseInterrupt:
	push	ES
	mov	AX,CS
	mov	ES,AX
	mov	AX,12
	lea	DX,MouseInterrupt
	mov	CX,001FH
	int	33H
	pop	ES
	ret


;***
;
; CheckMouseInterrupt - This routine restores the old mouse interrupt
;			  and call mask.
;
;
; Entry
;
; Exit
;	ES:DX	= Mouse Interrupt vector
;	CX	= Mouse Interrupt Mask
;
; Uses
;	C convention
;

CheckMouseInterrupt:
	xor	DX,DX
	mov	ES,DX
	mov	CX,DX
	mov	AX,20
	int	33H
	push	ES
	push	DX
	push	CX
	mov	AX,20
	int	33H
	pop	CX
	pop	DX
	pop	ES
	ret



;***
;
; MouseInterrupt - Mouse Interrupt return to track and display the mouse
;		   cursor using the Grabber Get and Put block calls.
;
;
; Entry
;	CX = Horz cursor coord. (Mouse)
;	DX = Vert cursor coord. (Mouse)
;
; Exit
;	Mouse Cursor is updated
;
; Uses
;	C convention less ES (Mouse ISR saves and restores regs)
;
MouseInterrupt	PROC	FAR

						; is set

	mov	CS:ButtonIntState,BL
	call	GetNewPosition


	cmp	DX,CS:MouseCoords
	je	MouseIntRet
	test	CS:CursorStatus,0FFH
	jz	MouseIntRet
	call	RemoveCursor
	mov	CS:MouseCoords,DX
	call	ShowCursor
	call	ResetMickey
MouseIntRet:
	ret

MouseInterrupt	ENDP



;***
;
; GetNewPosition - This routine calculates the new mouse position from
;		   the old position and the mickey counts
;
;
; Entry
;
; Exit
;
; Uses
;	C convention less ES
;
GetNewPosition:
	mov	AX,11
	int	33H			;get Mickey counts
	mov	AX,DX
	add	AX,CS:DeltaY
	mov	CS:DeltaY,AX
	mov	BL,CS:VerticalScale
	idiv	BL
	cbw
	mov	DX,CS:MouseCoords
	xchg	DH,DL
	xor	DH,DH
	add	DX,AX
	mov	AX,CX
	add	AX,CS:DeltaX
	mov	CS:DeltaX,AX
	mov	BL,CS:HorizontalScale
	idiv	BL
	cbw
	mov	CX,CS:MouseCoords
	xor	CH,CH
	add	CX,AX
	;
	; RangeCheck
	;
	mov	AL,CS:MouseY
	cbw
	cmp	DX,AX
	jle	GNP10
	call	ResetMickey
	mov	DX,AX
	jmp	GNP20
GNP10:
	cmp	DX,0
	jge	GNP20
	call	ResetMickey
	xor	DX,DX
GNP20:
	mov	AL,CS:MouseX
	cbw
	cmp	CX,AX
	jle	GNP30
	call	ResetMickey
	mov	CX,AX
	jmp	GNP40
GNP30:
	cmp	CX,0
	jge	GNP40
	call	ResetMickey
	xor	CX,CX
GNP40:
	xchg	DH,DL			;get coords right
	mov	DL,CL
	ret




;***
;
; ResetMickey - This routine resets the mouse driver Mickey count
;
;
; Entry
;
; Exit
;
; Uses
;	none
;
ResetMickey:
	mov	CS:DeltaY,0
	mov	CS:DeltaX,0
	ret
COMMENT %
	push	AX
	push	CX
	push	DX
	mov	AX,11
	int	33H				;reset mickey count
	pop	DX
	pop	CX
	pop	AX
	ret
%


;***
;
; RemoveCursor -  This routine restores the screen area under the cursor
;		  and removes the cursor
;
;
; Entry
;	CS:MouseCoords is valid
;
; Exit
;	Mouse Cursor is removed
;
; Uses
;
RemoveCursor:
	test	CS:CursorStatus,0FFH
	jz	RemCursRet
	mov	CS:CursorStatus,0
	push	ES
	push	DI
	push	SI
	push	BX
	push	AX
	mov	AX,CS
	mov	ES,AX
	lea	DI,MouseGrab
	mov	AX,CS:MouseSave
	mov	BX,CS:MouseCoords
	mov	ES:[DI].grXorg,BL
	mov	ES:[DI].grYorg,BH
	mov	ES:[DI].grXext,1
	mov	ES:[DI].grYext,1
	mov	ES:[DI].grStyle,FMT_NATIVE
	lea	SI,MouseSave
	mov	word ptr ES:[DI].grlpData,SI
	mov	word ptr ES:[DI].grlpData+2,CS
	mov	AX,GRAB_PUTBLOCK
	mov	SI,InquireGrab
	call	GoGrab
	pop	AX
	pop	BX
	pop	SI
	pop	DI
	pop	ES
RemCursRet:
	ret




;***
;
; ShowCursor -	This routine saves the current data under the cursor
;		and draws the cursor.
;
;
; Entry
;	CS:MouseCoords is valid
;
; Exit
;	Mouse Cursor is displayed
;
; Uses
;
ShowCursor:
	test	CS:CursorStatus,0FFH
	jnz	ShowCursRet
	push	ES
	push	DI
	push	SI
	push	BX
	push	AX
	mov	AX,CS
	mov	ES,AX
	lea	DI,MouseGrab
	mov	AX,CS:MouseCoords
	mov	ES:[DI].grXorg,AL
	mov	ES:[DI].grYorg,AH
	mov	ES:[DI].grXext,1
	mov	ES:[DI].grYext,1
	lea	SI,MouseSave
	mov	word ptr ES:[DI].grlpData,SI
	mov	word ptr ES:[DI].grlpData+2,CS
	mov	ES:[DI].grStyle,FMT_NATIVE
	mov	AX,GRAB_GETBLOCK
	mov	SI,InquireGrab
	push	ES
	push	DI
	call	GoGrab
	pop	DI
	pop	ES
	mov	ES:[DI].grChar,MOUSE_CURSOR
	mov	BH,LC_APPTITLE
	call	GoRealizeColor
	mov	ES:[DI].grAttr,BH
	mov	ES:[DI].grStyle,FMT_OTHER+F_BOTH
	mov	AX,GRAB_PUTBLOCK
	mov	SI,InquireGrab
	call	GoGrab
	mov	CS:CursorStatus,0FFH
	pop	AX
	pop	BX
	pop	SI
	pop	DI
	pop	ES
ShowCursRet:
	ret


ifdef DEBUG


DebugHeader	db	'Row  Col   R C',0DH,0AH,0
HexTable	db	'0123456789ABCDEF'
DebugBuffer	db	6 DUP(0)

;
; XmitReg - Transmits a 16 bit reg content
;
; ENTRY
;	ax	= word to convert
;
; EXIT
;	none
;
; USES
;	AX,flags
XmitReg:
	push	DS
	push	SI
	push	BX
	push	CS
	pop	DS
	lea	BX,HexTable
	lea	SI,DebugBuffer
	call	AX2Hex
	mov	AX,0020H		;space and null
	mov	[SI][4],AX
	call	XmitString
	pop	BX
	pop	SI
	pop	DS
	ret


; XmitByte - Transmits a 8 bit reg content
;
; ENTRY
;	ah	= byte to convert
;
; EXIT
;	none
;
; USES
;	AX,flags
XmitByte:
	push	DS
	push	SI
	push	BX
	push	CS
	pop	DS
	lea	BX,HexTable
	lea	SI,DebugBuffer
	call	AH2Hex
	mov	AX,0020H		;space and null
	mov	[SI][2],AX
	call	XmitString
	pop	BX
	pop	SI
	pop	DS
	ret

;
; XmitCRLF - Sends a CR lf sequence to com port
;
; ENTRY
;
; EXIT
;
; USES
;	AX, flags
;
XmitCRLF:
	mov	AL,0dH
	call	XmitChar
	mov	AL,0AH
	call	XmitChar
	ret
;
; AX2Hex - binary to hex converter
;
; ENTRY
;	ax	=  word to convert (AX2Hex)
;	ah	=  byte to convert (AH2Hex)
;
;	ds:si	-> 4-byte output buffer (AX2Hex)
;	ds:si	-> 2-byte output buffer (AH2Hex)
;
;	ds:bx	-> 16-byte lookup table to use for xlat
; EXIT
;	none
; USES
;	ax, flags
;
AX2Hex		proc	near
		assume	ds:nothing
		push	ax
		mov	ah,al
		shr	ah,1
		shr	ah,1
		shr	ah,1
		shr	ah,1
		and	al,00001111b
		xlat
		xchg	al,ah
		xlat
		mov	ds:[si][2],ax
		pop	ax

AH2Hex		proc	near
		mov	al,ah
		shr	ah,1
		shr	ah,1
		shr	ah,1
		shr	ah,1
		and	al,00001111b
		xlat
		xchg	al,ah
		xlat
		mov	ds:[si],ax
		ret
AH2Hex		endp
AX2Hex		endp

;
;
; InitDebug - Sends header to comm port
;
; ENTRY
;
; EXIT
;	none
; USES
;	ax, flags
;
InitDebug:
	push	DS
	push	SI
	lea	SI,DebugHeader
	push	CS
	pop	DS
	call	XmitString
	pop	SI
	pop	DS
	ret

; XmitString - Sends a string to comm port
;
; ENTRY
;	DS:SI points to string
;
; EXIT
;	none
; USES
;	ax, flags
;
XmitString:
	push	SI
XmitString10:
	lodsb
	cmp	AL,0
	jz	XmitStringEnd
	call	XmitChar
	jmp	XmitString10
XmitStringEnd:
	pop	SI
	ret


;
; XmitChar - Sends char to comm port
;
; ENTRY
;	AL = char
;
; EXIT
;	none
; USES
;	ax, flags
;
XmitChar:
	push	DX
	mov	AH,01
	mov	DX,0
	int	14H
	pop	DX
	ret



endif

sEnd	CODE
end
