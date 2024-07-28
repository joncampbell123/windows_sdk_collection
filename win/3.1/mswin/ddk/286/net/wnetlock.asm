page		60, 132
title		Windows Network Printing Functions
;===============================================================================
;		Filename	WNETPRNT.ASM
;		Copyright	(C) 1989 by Research Machines
;		Copyright	(C) 1989-1990 Microsoft Corporation
;===============================================================================
; REVISIONS:	24/02/1989	Initial version
;		15/03/1989	Update to spec 0.56
;				Add WNetOpenJob and WNetCloseJob
;		16/03/1989	Change to STANDARD procedures (save si, di, ds)
;		23/03/1989	Change to segment name / file name
;				Update to spec 0.58
;		29/03/1989	Change smbPQEOriginatorName to offset
;		31/03/1989	Added WNetWatchQueue.
;				Tidied up comments.
;				Called ERRSetErrorCode.
;		04/04/1989	Bug fix to WNetWatchQueue
;				Add WNetUnwatchQueue
;				Add WNetLockQueueData
;				Add WNetUnlockQueueData
;===============================================================================

		memM	equ	1		; Middle memory model
		?WIN	=	1		; Windows prolog/epilog
		?PLM	=	1		; Pascal calling convention

		.xlist
include 	cmacros.inc
include 	windows.inc
include 	smb.inc
include 	wnet.inc
include 	wnetcaps.inc
include 	wnetprnt.inc
		.list

		.sall

    CBQDATA	equ	3000


externFP	MSNetFindLocalSession			; In file MSNET.ASM
externFP	MSNetMakeAssignListEntry		; In file MSNET.ASM
externFP	MSNetCancelAssignListEntry		; In file MSNET.ASM
externFP	MSNetCreateHandle			; In file MSNET.ASM
externFP	MSNetCloseHandle			; In file MSNET.ASM

externFP	UTLMemMove				; In file UTILS.ASM
externFP	UTLRemoveTrailingSpaces 		; In file UTILS.ASM
externFP	UTLRemoveTrailingColon			; In file UTILS.ASM

externFP	GlobalLock			; KERNEL things
externFP	GlobalUnlock
externFP	GlobalAlloc
externFP	GlobalFree

externNP	GetWholeQueue			; WNETPRNQ.ASM



;===============================================================================
; ============= DATA SEGMENT ===================================================
;===============================================================================

sBegin		DATA

globalW 	hHeapSpace, 0, 1	 ; global handle of the locked data

sEnd		DATA

;===============================================================================
; ============= CODE SEGMENT ===================================================
;===============================================================================

createSeg	_PRNT, PRNTCODE, BYTE, PUBLIC, CODE
sBegin		PRNTCODE
		assumes CS, PRNTCODE
		assumes DS, DATA


;===============================================================================
subttl		WNetLockQueueData
page
;===============================================================================
;
; DESCRIPTION .
; ENTRY .......
; EXIT ........
; COMMENTS ....
;
;===============================================================================

cProc		WNetLockQueueData, <FAR, PUBLIC>, <si, di>

		parmD	lpszLocal
		parmD	lpszUser
		parmD	lplpQ

		localD	 lpqBuffer

		localW	nBufferSize				; Size of user supplied buffer
		localW	nEntriesReturned			; Entries in the buffer
		localW	nTotalEntries				; Entries in the queue

		localW	nSessionNumber
		localW	nIndex					; Index to next queue entry
		localW	nRequiredBufferSize			; Size needed to hold all of queue

		localD	lpFrontBuffer				; Pointer to space for structures
		localW	nEndBuffer				; Offset to space for names
		localW	nSizeOriginName 			; Length current name

		localB	fBufferOverflow
		localW	nReturnCode

		localV	smbPQ, <size smbPrintQEntry>
cBegin
		mov	( nReturnCode ), WN_SUCCESS		; Ever optimistic

;---------------------------------------------------------------
; Check that we aren't already locking a queue
;---------------------------------------------------------------

		cmp	hHeapSpace, 0
		jz	WNLQDDataNotLocked

		mov	nReturnCode, WN_ALREADY_LOCKED
		jmp	WNGQExitNow

;---------------------------------------------------------------
; Initialize stuff, then grab a buffer
;---------------------------------------------------------------

WNLQDDataNotLocked:
		mov	nBufferSize,CBQDATA
		mov	nEndBuffer,CBQDATA

		mov	ax,GMEM_MOVEABLE	    ; let it wiggle around
		push	ax			    ; in case we realloc
		sub	ax,ax
		push	ax
		push	nBufferSize		    ; size (as a dword)
		cCall	GlobalAlloc
		or	ax,ax			    ; did we get it?
		jnz	WNLQDHaveBuffer

		mov	nReturnCode, WN_OUT_OF_MEMORY
		jmp	WNGQExitFree		    ; suffering and death

WNLQDHaveBuffer:
		mov	hHeapSpace,ax		    ; keep the handle
		push	ax
		cCall	GlobalLock		    ; lock it
		mov	word ptr lpqBuffer[0],ax    ; save the pointer
		mov	word ptr lpqBuffer[2],dx

;---------------------------------------------------------------
; lpszDestination is a device name (a printer hopefully).
; Search down the assign list for the local name. If not found,
; give up. (who said I was a quitter?).
;
; The spec requires display of UNC shares because of the spooler's show
; other command.  Deal with it later, folks.  Probably call the name in
; this function and add code below to toss it out later.
;---------------------------------------------------------------

ValidDevice:	Arg	lpszLocal				;
		cCall	MSNetFindLocalSession			;
		cmp	ax, NULL				;
		jne	SessionFound				;

		mov	( nReturnCode ), WN_BAD_QUEUE		; Set return code
		jmp	WNGQExitFree				; Exit with LocalFree

SessionFound:


		Arg	ax			; session number
		Arg	lpszUser
		Arg	lpqBuffer

		lea	bx,nEntriesReturned
		Arg	RegPairSSBX		; lpnEntriesReturned

		cCall	GetWholeQueue

		mov	( nReturnCode ), ax
		or	ax,ax			; return success?
		jnz	WNGQExitFree

;---------------------------------------------------------------
; End of entries in queue. Complete the details in the QUEUESTRUCT
; at the head of the user supplied buffer.
;---------------------------------------------------------------

EndOfList:	les	bx, ( lpqBuffer )

		mov	es: [ bx ].pqName, NULL
		mov	es: [ bx ].pqComment, NULL
		mov	es: [ bx ].pqStatus, WNPRQ_ACTIVE
		mov	ax, ( nEntriesReturned )
		mov	es: [ bx ].pqJobCount, ax
		mov	es: [ bx ].pqPrinters, 1

		mov	ax,bx
		mov	dx,es

		les	bx,lplpQ	; hand back pointer to locked structure
		mov	es:[bx],ax
		mov	es:[bx+2],dx

;---------------------------------------------------------------
; We have successfully filled the buffer with print queue entries.
;---------------------------------------------------------------

		jmp	short WNGQExitNow


;---------------------------------------------------------------
; Return to the calling process after freeing heap space.
;---------------------------------------------------------------

WNGQExitFree:	sub	si,si
		xchg	si,hHeapSpace
		push	si
		cCall	GlobalUnlock
		push	si
		cCall	GlobalFree

;---------------------------------------------------------------
; Return to the calling process without freeing heap space.
;---------------------------------------------------------------

WNGQExitNow:
		mov	ax,nReturnCode

cEnd


;========================================================================
;
;   WNetUnlockQueueData
;
;

cProc WNetUnlockQueueData, <FAR, PUBLIC>, <SI>

    parmD   lpsz

cBegin
	mov	ax, WN_ALREADY_LOCKED

	mov	si,hHeapSpace

	or	si,si
	jz	wnuqd_exit

	cCall	GlobalUnlock, <si>
	cCall	GlobalFree, <si>

	sub	ax,ax

	mov	hHeapSpace, ax

wnuqd_exit:
cEnd

sEnd		PRNTCODE

		end
