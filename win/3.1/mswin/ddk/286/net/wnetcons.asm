page		60, 132
title		Windows Network Device Redirecting Functions
;===============================================================================
;		Filename	WNETCONS.ASM
;		Copyright	(C) 1989 by Research Machines
;		Copyright	(C) 1989-1990 Microsoft Corporation
;===============================================================================
; REVISIONS:	24/02/1989	Initial version
;		16/03/1989	Change to STANDARD procedures (save si, di, ds)
;		23/03/1989	Change to segment name / file name
;		23/03/1989	Update to spec 0.58
;		28/03/1989	Change clLocal & clRemote to offsets
;		31/03/1989	Tidied up comments
;		ever since	made it work
;===============================================================================

		memM	equ	1				; Middle memory model
		?WIN	=	1				; Windows prolog/epilog
		?PLM	=	1				; Pascal calling convention

		.xlist
include 	cmacros.inc
include 	windows.inc
include 	msdos.inc
include 	wnet.inc
include 	wnetcaps.inc
include 	wnetcons.inc
		.list

		.sall

externFP	MSNetMakeAssignListEntry			; In file MSNET.ASM
externFP	MSNetCancelAssignListEntry			; In file MSNET.ASM
externFP	MSNetGetAssignListEntry 			; In file MSNET.ASM

externFP	lstrlen
externFP	lstrcpy
externFP	lstrcmpi


;===============================================================================
; ============= DATA SEGMENT ===================================================
;===============================================================================

sBegin		DATA

sEnd		DATA

;===============================================================================
; ============= CODE SEGMENT ===================================================
;===============================================================================

createSeg	_CONS, CONSCODE, BYTE, PUBLIC, CODE
sBegin		CONSCODE
		assumes CS, CONSCODE
		assumes DS, DATA

;===============================================================================
; ============= EXPORTED FUNTIONS ==============================================
;===============================================================================

;===============================================================================
subttl		WNetAddConnection
page
;===============================================================================
;
; DESCRIPTION . Add an entry to the assign list.
; ENTRY .......
; EXIT ........
; COMMENT .....
;
;===============================================================================

cProc		WNetAddConnection, <FAR, PUBLIC>, <si>

		parmD	lpszNetPath
		parmD	lpszPassword
		parmD	lpszLocalName

		localW	nSizeNetPath				; Length of remote name

		localV	RemoteName, <size aleRemoteName>

cBegin
		lea	si,RemoteName

;---------------------------------------------------------------
; Form the destination string. This has the following format:
; <machine-name><pathname><0><password><0>
; Check first that this does not exceed 128 bytes.
;---------------------------------------------------------------

WNACCheckSize:	Arg	lpszNetPath				; Length of NetPath
		cCall	lstrlen 				;
		inc	ax					; Plus NULL
		mov	( nSizeNetPath ), ax			; Save

		Arg	lpszPassword				; Length of Password
		cCall	lstrlen 				;
		inc	ax					; Plus NULL

		add	ax, ( nSizeNetPath )			; Plus length of NetPath
		cmp	ax, size aleRemoteName			; More than 128 bytes?
		jbe	WNACFormString				;

		mov	ax, WN_BAD_NETNAME			; Set return code
		jmp	WNACExit				; Exit with FreeLocal

;---------------------------------------------------------------
; Now we are sure that the destination string fits, create it in
; the heap.
;---------------------------------------------------------------

WNACFormString: Arg	RegPairSSSI				; Copy <machine-name><pathname><0>
		Arg	lpszNetPath				;
		cCall	LStrcpy 				;

		mov	bx, si
		add	bx, ( nSizeNetPath )			; Adjust bx past first part

		Arg	RegPairSSBX				; Copy <password><0>
		Arg	lpszPassword				;
		cCall	LStrcpy 				;

;---------------------------------------------------------------
; Finally, make the assign list entry. This is the easy bit.
;---------------------------------------------------------------

		mov	ax, 0					; UserValue = 0

		Arg	lpszLocalName				;
		Arg	RegPairSSSI				;
		Arg	ax					;
		cCall	MSNetMakeAssignListEntry		;

;---------------------------------------------------------------
; Return to the caller
;---------------------------------------------------------------

WNACExit:
cEnd

;===============================================================================
subttl		WNetCancelConnection
page
;===============================================================================
;
; DESCRIPTION . Remove an entry from the assign list.
; ENTRY .......
; EXIT ........
; COMMENT .....
;
;===============================================================================

cProc		WNetCancelConnection, <FAR, PUBLIC>

		parmD	lpszName
		parmW	fForce					; IGNORED

cBegin
;---------------------------------------------------------------
; Cancel the assign list entry using DosCancelAssignListEntry.
;---------------------------------------------------------------

		Arg	lpszName				;
		cCall	MSNetCancelAssignListEntry		;

		cmp	ax, WN_SUCCESS				;
		je	WNCCExit				;

		mov	ax, WN_NOT_CONNECTED			; Set return code

;---------------------------------------------------------------
; Return to the caller
;---------------------------------------------------------------

WNCCExit:
cEnd

;===============================================================================
subttl		WNetGetConnection
page
;===============================================================================
;
; DESCRIPTION . Return a list of assign list entries.
; ENTRY .......
; EXIT ........
; COMMENT .....
;
;===============================================================================

cProc		WNetGetConnection, <FAR, PUBLIC>, <si,di>

	parmD	lpLocal
	parmD	lpRemote
	parmD	lpcbBuffer

	localV	szFind,16
	localV	szLocal,16
	localV	szRemote,128
	localW	iEntry
	localW	iStatus

cBegin
	cCall	lstrlen,<lpLocal>	; check for length too long
	cmp	ax, 16
	jle	@F
	jmp	wngc_badname

@@:
	push	ax			; save length
	lea	si,szFind
	cCall	lstrcpy,<RegPairSSSI,lpLocal>
	pop	ax			; restore
	cmp	ax,2			    ; HACK: Is this a Drive?
	je	wngc_nocolon		    ; Yup, leave the colon on
	add	si,ax			    ; Nope, this is an LPT port
	dec	si			    ; Strip off the trailing colon
	cmp	byte ptr ss:[si],':'
	jnz	wngc_nocolon
	mov	byte ptr ss:[si],0
wngc_nocolon:
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	lea	si,szLocal
	lea	di,szRemote
	mov	iEntry,0

	RegPtr	szLoc,ss,si
	RegPtr  szRem,ss,di

wngc_loop:
	mov	ax,5F02h
	mov	bx,iEntry
	inc	iEntry
	push	bp
	call	dos3call
	pop	bp
	jc	wngc_notfound

	mov	iStatus, WN_SUCCESS
	test	bh,1		; is the device valid
	jz	@F
	mov	iStatus, WN_DEVICE_ERROR
@@:

	push	es		; Save ES

	lea	bx,szFind
	cCall	lstrcmpi,<szLoc,RegPairSSBX>

	pop	es		; Restore ES

	or	ax,ax
	jnz	wngc_loop

	cCall	lstrlen,<szRem>
	inc	ax		; account for NULL

	les	bx,lpcbBuffer	; get size of caller's buffer
	cmp	ax,es:[bx]	; is it smaller than we need?
	ja	wngc_too_small	; go deal with that mess
	mov	es:[bx],ax	; else tell him how much we used
	mov	cx,ax
	mov	ax,iStatus	; indicate success or drive error
	jmp	short wngc_copy

wngc_badname:
	mov	ax,WN_BAD_NETNAME
	jmp	short wngc_exit

wngc_notfound:
	mov	ax,WN_NOT_CONNECTED
	jmp	short wngc_exit

wngc_too_small:
	mov	cx,es:[bx]      
        mov     es:[bx],ax      ; indicate the required buffer size
	mov	ax,WN_MORE_DATA	; indicate buffer is too small
        jcxz    wngc_exit       ; check for null length buffer.

wngc_copy:
	dec	cx		; account for null
	les	di,lpRemote	; point to user's destination
	lea	si,szRemote
	rep	movsb		; blast the string
	mov	byte ptr es:[di],0	 ; null terminate in all cases

wngc_exit:
cEnd

;===============================================================================
; ============= END OF WNETCONS ================================================
;===============================================================================

sEnd		CONSCODE
		end
