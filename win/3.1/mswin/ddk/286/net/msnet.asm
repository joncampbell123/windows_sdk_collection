page		60, 132
title		Access to MSNET Functions
;===============================================================================
;		Filename	MSNET.ASM
;		Copyright	(C) 1989 by Research Machines
;		Copyright	(C) 1989-1990 by Microsoft Corporation
;===============================================================================
; REVISIONS:	24/02/1989	Initial Version
;		15/03/1989	Add WNetCreateHandle and WNetCloseHandle
;		16/03/1989	Change to STANDARD procedures (save si, di, es)
;		29/03/1989	Change to WNetCancelConnection - allow net path
;		30/03/1989	Added MSNetGetMachineName. Updated comments.
;===============================================================================

		memM	equ	1				; Middle memory model
		?WIN	=	1				; Windows prolog/epilog
		?PLM	=	1				; Pascal calling convention

		.xlist
include 	cmacros.inc
include 	windows.inc
include 	wnet.inc
include 	msdos.inc
		.list

		.sall

externFP	UTLStricmpColon 				; In file UTILS.ASM
externFP	UTLIsSerialDevice				; In file UTILS.ASM
externFP	UTLIsNetworkPath				; In file UTILS.ASM
externFP	UTLAppendTrailingColon				; In file UTILS.ASM

externFP	lstrcmp
externFP	lstrcpy
externFP	lstrlen

;===============================================================================
; ============= DATA SEGMENT ===================================================
;===============================================================================

sBegin		DATA

public ErrorXTbl
ErrorXTbl   db	00h, WN_SUCCESS 	    ; Yay, no error
	    db	05h, WN_ACCESS_DENIED	    ; access denied
	    db	08h, WN_OUT_OF_MEMORY	    ; out of memory somewhere
            db  0bh, WN_BAD_NETNAME         ; weirdo dos error.
	    db	13h, WN_ACCESS_DENIED	    ; write protected disk
	    db	18h, WN_BAD_VALUE	    ; bad struct length
	    db	32h, WN_NOT_SUPPORTED	    ; net not supported
	    db	35h, WN_BAD_NETNAME	    ; network name not found
	    db	40h, WN_BAD_NETNAME	    ; network name deleted
	    db	41h, WN_ACCESS_DENIED	    ; network access denied
	    db	44h, WN_BAD_NETNAME	    ; net name limit exceeded?????
	    db	54h, WN_NOT_CONNECTED	    ; too many redirections
	    db	55h, WN_ALREADY_CONNECTED   ; connection already used
	    db	56h, WN_BAD_PASSWORD	    ; password is incorrect

NERRORS     equ ($-ErrorXTbl)/2

sEnd		DATA

;===============================================================================
; ============= CODE SEGMENT ===================================================
;===============================================================================

createSeg	_MSN, MSNCODE, BYTE, PUBLIC, CODE
sBegin		MSNCODE
		assumes CS, MSNCODE
		assumes DS, DATA

;===============================================================================
subttl		MapMSDOSErrorCode
page
;===============================================================================
;
; DESCRIPTION . Convert MSDOS error code into a Windows errorcode
; ENTRY ....... carry set => error
; EXIT ........ ax = Windows error
; COMMENT .....
;
;===============================================================================

cProc MapMSDOSError, <NEAR, PUBLIC>
cBegin <nogen>

    jnc     maperror_none

    mov     ah, 59h		; get error code
    sub     bx, bx
    call    dos3call

    mov     bl, al		; save dos error code
    push    si
    lea     si, ErrorXTbl	; point to table
    mov     cx, NERRORS

maperror_loop:
    lodsw
    cmp     al,bl
    jz	    maperror_exit
    loop    maperror_loop

    pop     si
    mov     ax, WN_NET_ERROR	; generic error code
    ret

maperror_exit:
    pop     si
    mov     al,ah
    sub     ah,ah
    ret

maperror_none:

    sub     ax, ax		; WN_SUCCESS
    ret

cEnd   <nogen>

;===============================================================================
; ============= PUBLIC FUNTIONS ================================================
;===============================================================================

;===============================================================================
subttl		MSNetMakeAssignListEntry
page
;===============================================================================
;
; DESCRIPTION . Redirect a local device across the network
; ENTRY ....... lpszLocalName -> device name
;		lpszRemoteName -> net path including password
;		nUserValue = 0 (usually)
; EXIT ........ AX = SUCCESS or not
; COMMENT .....
;
;===============================================================================

cProc		MSNetMakeAssignListEntry, <FAR, PUBLIC>, <si, di>

		parmD	lpszLocalName
		parmD	lpszRemoteName
		parmW	nUserValue
cBegin
;---------------------------------------------------------------
; Find out if the local name is a printer or disk drive. NULL
; device names count as drives for now
;---------------------------------------------------------------

		Arg	lpszLocalName				;
		cCall	UTLIsSerialDevice			;
		or	ax,ax
		jnz	DeviceIsSerial				;

		mov	bl, 4					; Device is block, or NULL
		jmp	short MakeALEntry			;
DeviceIsSerial: mov	bl, 3

;---------------------------------------------------------------
; Make the assign list entry
;---------------------------------------------------------------
MakeALEntry:
		push	ds
		mov	ax, DosMakeAssignListEntry
		mov	cx, nUserValue
		lds	si, ( lpszLocalName )
		les	di, ( lpszRemoteName )
		call	dos3call
		pop	ds

		call	MapMSDOSError
cEnd

;===============================================================================
subttl		MSNetCancelAssignListEntry
page
;===============================================================================
;
; DESCRIPTION . Cancel a redirection across the network
; ENTRY ....... lpszName points to a local / remote name to cancel
; EXIT ........ AX holds return code
; COMMENT ..... If szName is a full network path, we must use the local device
;		name if there is one. NOT IMPLEMENTED YET.
;
;===============================================================================

cProc		MSNetCancelAssignListEntry, <FAR, PUBLIC>, <si>

		parmD	lpszName

cBegin

WNCECancel:	push	ds
		mov	ax, DosCancelAssignListEntry
		lds	si,lpszName
		call	dos3call
		pop	ds

		call	MapMSDOSError
WNCEExit:

cEnd

;===============================================================================
subttl		MSNetGetAssignListEntry
page
;===============================================================================
;
; DESCRIPTION . Return an assign list entry
; ENTRY ....... lpBuffer -> space for a single assign list entry (16+128)
;		nIndex = index into assign list
; EXIT ........ AX = SUCCESS or not
;		BX = 3 for printer, 4 for drive
;		CX = user value
; COMMENTS ....
;
;===============================================================================

cProc		MSNetGetAssignListEntry, <FAR, PUBLIC>, <si, di>

		parmD	lpBuffer
		parmW	nIndex
cBegin
		push	ds
		mov	ax, DosGetAssignListEntry
		mov	bx, ( nIndex )
		lds	si, ( lpBuffer )
		les	di, ( lpBuffer )
		add	di, 16
		call	dos3call
		pop	ds

		call	MapMSDOSError

		mov	bh, 0
cEnd

;===============================================================================
subttl		MSNetFindLocalSession
page
;===============================================================================
;
; DESCRIPTION .
; ENTRY .......
; EXIT ........
; COMMENT .....
;
;===============================================================================

cProc		MSNetFindLocalSession, <FAR, PUBLIC>, <SI,DI>

		parmD	lpszLocalName

		localW	nIndex
		localW	nSessionNumber

		localV	rgbALE, <size AssignListEntry>
cBegin
		lea	si,rgbALE

;---------------------------------------------------------------
; Now we need to flip through the assign entry list looking for
; the local device name.
;---------------------------------------------------------------

WNFLSStart:	mov	( nIndex ), 0				; First entry

		push	ds
		mov	ax,ss
		mov	ds,ax
		mov	es,ax

WNFLSNextEntry:
		mov	ax, DosGetAssignListEntry2		; As function 5f02h
		mov	bx, ( nIndex )				; but returns session
		lea	di, rgbALE.aleRemoteName
		lea	si, rgbALE.aleLocalName 		; make ds:si -> local name
		push	bp					;
		int	MSDOS					; can't use dos3call because of bp
		mov	ax, bp					;
		pop	bp					;
		mov	( nSessionNumber ), ax			; Save session

		jnc	CheckLocal				; A session found.
		jmp	short WNFLSExit 			; End of list.

;---------------------------------------------------------------
; Does the local name in the assign list match the name we are
; looking for?
;---------------------------------------------------------------

CheckLocal:	lea	bx, rgbALE.aleLocalName 		;

		Arg	lpszLocalName				; Compare strings
		Arg	RegPairSsBx				;
		cCall	UTLStricmpColon 			;
		or	ax,ax
		je	WNFLSSuccess				   ;

;---------------------------------------------------------------
; Try next assign list entry			  ;
;---------------------------------------------------------------

		inc	( nIndex )
		jmp	short WNFLSNextEntry

;---------------------------------------------------------------
; The local net path has been found in the assign list. The
; session number is in ( nSessionNumber ).
;---------------------------------------------------------------

WNFLSSuccess:	mov	ax,nSessionNumber

WNFLSExit:
		pop	ds

cEnd

;===============================================================================
subttl		MSNetFindLocalName
page
;===============================================================================
;
; DESCRIPTION .
; ENTRY .......
; EXIT ........
; COMMENT .....
;
;===============================================================================

cProc		MSNetFindLocalName, <FAR, PUBLIC>, <si>

		parmD	lpszRemoteName
		parmD	lpszLocalName

		localW	nIndex
		localW	nReturnCode

		localV	rgbALE, <size AssignListEntry>
cBegin

;---------------------------------------------------------------
; Now we need to flip through the assign entry list looking for
; the remote device name.
;---------------------------------------------------------------

WNFLNStart:	mov	( nIndex ), 0				; First entry

WNFLNNextEntry: StandardSave					; Save SI, DI, DS
		mov	ax,ss
		mov	ds,ax
		mov	es,ax
		mov	ax, DosGetAssignListEntry		; Function 5f02h
		mov	bx, ( nIndex )				;
		lea	si, rgbALE.aleLocalName
		lea	di, rgbALE.aleRemoteName
		call	dos3call

		StandardRestore 				; Restore SI, DI, DS

		jnc	CheckForMatch				; An entry found.
		mov	ax , not WN_SUCCESS			; End of list.
		jmp	short WNFLNExit

;---------------------------------------------------------------
; Does the remote name in the assign list match the name we are
; looking for?
;---------------------------------------------------------------

CheckForMatch:	lea	bx, rgbALE.aleRemoteName		;

		Arg	lpszRemoteName				; Compare strings
		Arg	RegPairSsBx				;
		cCall	LStrcmp 			     ;
		cmp	ax, 0					; Equal ?
		je	WNFLNSuccess				;

;---------------------------------------------------------------
; Try next assign list entry
;---------------------------------------------------------------

		inc	( nIndex )
		jmp	short WNFLNNextEntry

;---------------------------------------------------------------
; The remote net path has been found in the assign list.
; Copy the associated local name to the users buffer.
;---------------------------------------------------------------

WNFLNSuccess:	lea	bx, rgbALE.aleLocalName 	    ;

		Arg	lpszLocalName
		Arg	RegPairSsBx
		cCall	LStrcpy
		mov	ax, WN_SUCCESS

WNFLNExit:

cEnd

;===============================================================================
subttl		MSNetGetMachineName
page
;===============================================================================
;
; DESCRIPTION . Returns the local machine name using DosGetMachineName
; ENTRY ....... lpszMachineName points to space for machine name
; EXIT ........ AX hold WN_SUCCESS or not
;		szMachineName completed with local machine name
; COMMENT .....
;
;===============================================================================

cProc		MSNetGetMachineName, <FAR, PUBLIC>, <si, di>

		parmD	lpszMachineName

cBegin
;---------------------------------------------------------------
; Call dos to get the local machine name.
;---------------------------------------------------------------

		push	ds

		mov	ax, DosGetMachineName
		lds	dx, ( lpszMachineName )
		call	dos3call

		pop	ds
		call	MapMSDOSError

cEnd

;===============================================================================
subttl		MSNetCreateHandle
page
;===============================================================================
;
; DESCRIPTION .
; ENTRY .......
; EXIT ........
; COMMENT .....
;
;===============================================================================

cProc		MSNetCreateHandle, <FAR, PUBLIC>

		parmD	lpszName
		parmW	nAttribute

		localW	handle
cBegin
		mov	( nReturnCode ), NULL			; Ever pessimistic

		mov	ah, DosCreateHandle
		lds	dx, ( lpszName )
		mov	cx, ( nAttribute )
		call	dos3call
		jc	ch_nohandle

		mov	handle, ax
		xchg	bx, ax		    ; handle in bx
		mov	ax, 4400h	    ; get device bits
		call	dos3call
		jc	ch_skipbits	    ; handle error
		sub	dh, dh
		or	dl, 20h 	    ; set raw mode bit
		mov	bx, handle
		mov	ax, 4401h	    ; set device bits
		call	dos3call
ch_skipbits:
		mov	ax, handle
		jmp	short create_exit

ch_nohandle:
		call	MapMSDosError
		neg	ax		    ; hack, return negative
create_exit:

cEnd

;===============================================================================
subttl		MSNetCloseHandle
page
;===============================================================================
;
; DESCRIPTION .
; ENTRY .......
; EXIT ........
; COMMENT .....
;
;===============================================================================

cProc		MSNetCloseHandle, <FAR, PUBLIC>, <si, di, ds>

		parmW	nHandle

cBegin
		mov	ah, DosCloseHandle
		mov	bx, ( nHandle )
		call	dos3call

		call	MapMSDOSError

cEnd

;===============================================================================
; ============= END OF MSNET ===================================================
;===============================================================================

sEnd		MSNCODE
		end
