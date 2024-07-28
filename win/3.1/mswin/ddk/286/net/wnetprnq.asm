page		60, 132
title		Windows Network Printing Functions
;===============================================================================
; Filename	MSNETPRN.ASM
; Copyright	(C) 1989 by Research Machines
;		(C) 1989-1990 by Microsoft Corp.
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
;		05/05/1989	Update to spec 0.59
;		09/05/1989	Get LockQueueData working!
;		ever since	Bug fixes.
;===============================================================================

		memM	equ	1		; Middle memory model
		?WIN	=	1		; Windows prolog/epilog
		?PLM	=	1		; Pascal calling convention

		.xlist
include 	cmacros.inc
include 	windows.inc
include 	wnet.inc
include 	wnetcaps.inc
include 	wnetprnt.inc
include 	smb.inc
		.list

		.sall

;===============================================================================
; ============= EXTERNAL FUNCTIONS =============================================
;===============================================================================

externFP	NETGetPrintQueue

externFP	UTLMemMove

externFP	UTLYearMonthDayToSeconds
externFP	UTLHourMinuteSecondToSeconds

;===============================================================================
; ============= CODE SEGMENT ===================================================
;===============================================================================

createSeg	_PRNT, PRNCODE, BYTE, PUBLIC, CODE
sBegin		PRNCODE
		assumes CS, PRNCODE
		assumes DS, DATA

;===============================================================================
; ============= STATIC FUNTIONS ================================================
;===============================================================================

;===============================================================================
subttl		SMBDateTimeToSeconds
page
;===============================================================================
;
; DESCRIPTION . Convert a date into seconds
; ENTRY ....... nDate = yyyyyyy mmmm ddddd
;		nTime = hhhhh mmmmmm sssss
; EXIT ........ dx:ax = seconds elapsed since 1/1/1970
;
;===============================================================================

cProc SMBDateTimeToSeconds, <NEAR,PUBLIC>, <si, di>

    parmW   nDate
    parmW   nTime

cBegin

;---------------------------------------------------------------
; Extract hour, min, sec (actually, we blow off the seconds...)
;---------------------------------------------------------------

    mov     ax, nTime

    mov     cl, 5
    shr     ax, cl
    mov     si, ax		    ; minutes in si
    and     si, 63

    inc     cl			    ; mov cl,6
    shr     ax, cl
    and     ax, 31
    mov     di, ax		    ; hours in di

;---------------------------------------------------------------
; Extract year, month, day
;---------------------------------------------------------------

    mov     ax, nDate
    mov     dx, ax
    and     dx, 31		; day in dx
    dec     dx			; make it zero based

    mov     cl, 5
    shr     ax, cl
    mov     bx, ax
    and     bx, 15		; month in bx
    dec     bx			; make it zero based

    mov     cl, 4
    shr     ax, cl
    and     ax, 127
    add     ax, 8		; convert to year since 1972 
                                ; (aligns to leap year)

;---------------------------------------------------------------
; Convert year/month/day to seconds
;---------------------------------------------------------------

    cCall   UTLYearMonthDayToSeconds, <ax,bx,dx>

    xchg    si, ax
    xchg    di, dx

;---------------------------------------------------------------
; Convert hour/minute/second to seconds
;---------------------------------------------------------------

    sub     cx, cx			    ; zero seconds...
    cCall   UTLHourMinuteSecondToSeconds, <dx,ax,cx>

    add     ax, si
    adc     dx, di

    add     ax, 6700h                ; Add 2 years of seconds (03c26700) 
    adc     dx, 03c2h                ; to give the number of seconds since
                                     ; 1970.
cEnd

;===============================================================================
subttl		CompactEntry
page
;===============================================================================
;
; DESCRIPTION .
; ENTRY .......
; EXIT ........
; COMMENTS ....
;
;===============================================================================

cProc		CompactEntry, <NEAR,PUBLIC>, <si, di>

    parmD   lpQueueEntries
    parmW   nIndex
    parmW   nEntriesInBuffer

cBegin

;---------------------------------------------------------------
; Calculate a pointer to the entry to compress.
;---------------------------------------------------------------

		les	bx, ( lpQueueEntries )			; Make ES:BX -> name
		mov	ax, size SMBPrintQEntry 		;
		mov	cx, ( nIndex )				;
		mul	cx					;
		add	bx, ax					;
		lea	bx, es: [ bx ].smbPQEOriginatorName	;

;---------------------------------------------------------------
; Calculate the number of bytes to compress.
;---------------------------------------------------------------

		mov	ax, ( nEntriesInBuffer )
		sub	ax, ( nIndex )
		dec	ax
		jz	NoCompression				; No comprendo

		mov	cx, size SMBPrintQEntry
		sub	cx, size smbPQEOriginatorName
		mul	cx
		mov	cx, ax

;---------------------------------------------------------------
; Calculate a pointer to the following entry.
;---------------------------------------------------------------

		mov	dx, es
		mov	ax, bx
		add	ax, size smbPQEOriginatorName

;---------------------------------------------------------------
; Compress those entries.
;---------------------------------------------------------------

		Arg	RegPairEsBx				; lpDestination
		Arg	RegPairDxAx				; lpSource
		Arg	cx					; nCount
		cCall	UTLMemMove				; Write all over interrupt table

;---------------------------------------------------------------
; Return to caller.
;---------------------------------------------------------------

NoCompression:

cEnd

;===============================================================================
subttl		MoveNamesDown
page
;===============================================================================
;
; DESCRIPTION .
; ENTRY .......
; EXIT ........
; COMMENTS ....
;
;===============================================================================

cProc		MoveNamesDown, <NEAR,PUBLIC>, <si, di>

		parmD	lpQueueEntries
		parmD	lpNameSpace
		parmW	nEntriesInBuffer

		localW	nIndex
cBegin

;---------------------------------------------------------------
; Move all the originators names to the end of the buffer
;---------------------------------------------------------------

		mov	ax, ( nEntriesInBuffer )		; Move backwards
		dec	ax					; through the list
		mov	( nIndex ), ax				; starting at the end.

MoveNameDown:	mov	ax, ( nIndex )				; Have we finished?
		cmp	ax, 0					;
		jl	LastNameMoved				;

;---------------------------------------------------------------
; Get a pointer to the name in the SMB
;---------------------------------------------------------------

		les	bx, ( lpQueueEntries )			; Make ES:BX -> name
		mov	ax, size SMBPrintQEntry 		;
		mov	cx, ( nIndex )				;
		mul	cx					;
		add	bx, ax					;
		lea	bx, es: [ bx ].smbPQEOriginatorName	;

;---------------------------------------------------------------
; Get a pointer to the 16 byte space at the end of the buffer
;---------------------------------------------------------------

		mov	ax, ( nIndex )				; Make DX:AX -> space
		mov	cx, size smbPQEOriginatorName		;
		mul	cx					;
		add	ax, word ptr ( lpNameSpace + 0 )	;
		mov	dx, word ptr ( lpNameSpace + 2 )	;

;---------------------------------------------------------------
; Copy the name to the end of the buffer
;---------------------------------------------------------------

		; zero terminate (leaving spaces)
		mov	byte ptr es:[bx].(size smbPQEOriginatorName-1),0

		mov	cx, size smbPQEOriginatorName		;

		Arg	RegPairDxAx				; lpDestination
		Arg	RegPairEsBx				; lpSource
		Arg	cx					; nCount
		cCall	UTLMemMove				;

;---------------------------------------------------------------
; Compact the current queue entry by moving the following
; entries down and overwriting the originators name.
;---------------------------------------------------------------

		Arg	lpQueueEntries
		Arg	nIndex
		Arg	nEntriesInBuffer
		cCall	CompactEntry

;---------------------------------------------------------------
; Loop for next entry
;---------------------------------------------------------------

		dec	( nIndex )				;
		jmp	short MoveNameDown			;

;---------------------------------------------------------------
; Return to the caller.
;---------------------------------------------------------------

LastNameMoved:

cEnd

;===============================================================================
subttl		ConvertEntries
page
;===============================================================================
;
; DESCRIPTION .
; ENTRY .......
; EXIT ........
; COMMENTS ....
;
;===============================================================================

cProc		ConvertEntries, <NEAR,PUBLIC>, <si, di>

		parmD	lpqBuffer
		parmD	lpNames
		parmW	nEntriesInBuffer

		localW	nIndex
		localW	nDate
		localW	nTime
		localW	nStatus
		localW	nSpoolFileNumber
		localD	lSize
		localW	pUserName
cBegin

;---------------------------------------------------------------
; Convert all the entries in the buffer
;---------------------------------------------------------------

		mov	ax, ( nEntriesInBuffer )		; Move backwards
		dec	ax					; through the list
		mov	( nIndex ), ax				; starting at the end.

ConvertEntry:	mov	ax, ( nIndex )				; Have we finished?
		cmp	ax, 0					;
		jge	MoreToGo				;
		jmp	LastEntryConverted			;
MoreToGo:

;---------------------------------------------------------------
; Get a pointer to the SMB entry
;---------------------------------------------------------------

		les	bx, ( lpqBuffer )			; Make ES:BX -> SMB

		mov	ax, size SMBPrintQEntry 		;
		sub	ax, size smbPQEOriginatorName		;
		mov	cx, ( nIndex )				;
		mul	cx					;
		add	bx, ax					;

;---------------------------------------------------------------
; Extract all the details of interest from the SMB
;---------------------------------------------------------------

		mov	ax, es: [ bx ].smbPQEDate		; Time / date
		mov	( nDate ), ax				;
		mov	ax, es: [ bx ].smbPQETime		;
		mov	( nTime ), ax				;

		mov	al, es: [ bx ].smbPQEStatus		; Status
		mov	byte ptr ( nStatus ), al		;

		mov	ax, es: [ bx ].smbPQESpoolFileNumber	; Spool file number
		mov	( nSpoolFileNumber ), ax		;

		mov	ax, word ptr es: [ bx ].smbPQESpoolFileSize + 0
		mov	word ptr ( lSize + 0 ), ax
		mov	ax, word ptr es: [ bx ].smbPQESpoolFileSize + 2
		mov	word ptr ( lSize + 2 ), ax

;---------------------------------------------------------------
; Convert some of the data as necessary
;---------------------------------------------------------------

		mov	cl, byte ptr ( nStatus )		; cl = 1 to ff
		dec	cl					; cl = 0 to fe
		and	cl, 00000111b				; cl = 0 to 3
		shl	cl, 1					; cl = 0 to 6
		shl	cl, 1					; cl = 0 to 12
		mov	ax, 2031h				; 0->1, 1->3, 2->0, 3->2
		shr	ax, cl					;
		and	ax, 0000000000000111b			;
		mov	( nStatus ), ax 			;

		mov	dx, ( nDate )				; Submitted
		mov	ax, ( nTime )				;
		Arg	dx					;
		Arg	ax					;
		cCall	SMBDateTimeToSeconds			;
		mov	( nDate ), dx				;
		mov	( nTime ), ax				;

		mov	ax, size smbPQEOriginatorName		; pName
		mov	cx, ( nIndex )				;
		mul	cx					;
		add	ax, word ptr ( lpNames + 0 )		;
		sub	ax, word ptr ( lpqBuffer )		;
		mov	( pUserName ), ax			;

;---------------------------------------------------------------
; Get a pointer to the JOBSTRUCT entry
;---------------------------------------------------------------

		les	bx, ( lpqBuffer )			; Make ES:BX -> JOBSTRUCT
		add	bx, size QUEUESTRUCT			;
		mov	ax, ( nIndex )				;
		mov	cx, size JOBSTRUCT			;
		mul	cx					;
		add	bx, ax					;

;---------------------------------------------------------------
; Complete the details in the JOBSTRUCT
;---------------------------------------------------------------

		mov	ax, ( nSpoolFileNumber )		; JobID
		mov	es: [ bx ].pjID, ax			;

		mov	ax, ( pUserName )			; UserName
		mov	es: [ bx ].pjUserName, ax		;

		mov	es: [ bx ].pjParameters, NULL		; Parameters

		mov	ax, ( nIndex )				; Position
		inc	ax					; (plus 1)
		mov	es: [ bx ].pjPosition, ax		;

		mov	ax, ( nStatus ) 			; Status
		mov	es: [ bx ].pjStatus, ax 		;

		mov	ax, ( nTime )				; Submitted
		mov	word ptr es: [ bx ].pjSubmitted + 0, ax ;
		mov	ax, ( nDate )				;
		mov	word ptr es: [ bx ].pjSubmitted + 2, ax ;

		mov	ax, word ptr ( lSize + 0 )		; Size
		mov	word ptr es: [ bx ].pjSize + 0, ax	;
		mov	ax, word ptr ( lSize + 2 )		;
		mov	word ptr es: [ bx ].pjSize + 2, ax	;

		mov	es: [ bx ].pjCopies, 1			; Copies

		mov	es: [ bx ].pjComment, NULL		; Comment

;---------------------------------------------------------------
; Loop for next entry
;---------------------------------------------------------------

		dec	( nIndex )				;
		jmp	ConvertEntry				;

;---------------------------------------------------------------
; Return to the caller.
;---------------------------------------------------------------

LastEntryConverted:

cEnd

;===============================================================================
subttl		CompleteQueueStruct
page
;===============================================================================
;
; DESCRIPTION .
; ENTRY .......
; EXIT ........
; COMMENTS ....
;
;===============================================================================

cProc		CompleteQueueStruct, <NEAR,PUBLIC>, <si, di>

		parmD	lpQueueStruct
		parmW	nEntriesInTotal
cBegin
		les	bx, ( lpQueueStruct )
		mov	cx, ( nEntriesInTotal )

		mov	es: [ bx ].pqName, NULL
		mov	es: [ bx ].pqComment, NULL
		mov	es: [ bx ].pqStatus, WNPRQ_ACTIVE
		mov	es: [ bx ].pqJobCount, cx
		mov	es: [ bx ].pqPrinters, 1
cEnd

;===============================================================================
subttl		ReformatBuffer
page
;===============================================================================
;
; DESCRIPTION .
; ENTRY .......
; EXIT ........
; COMMENTS ....
;
;===============================================================================

cProc		ReformatBuffer, <NEAR,PUBLIC>, <si, di, ds>

		parmD	lpqBuffer
		parmW	nEntriesInBuffer
		parmW	nEntriesInTotal

		localW	nIndex
		localD	lpNameSpace
cBegin

;---------------------------------------------------------------
; Calculate where all the originators names will be stored.
; This will be at the end of the buffer after all the JOBSTRUCTS.
;---------------------------------------------------------------

		les	bx, ( lpqBuffer )
		mov	ax, size JOBSTRUCT
		mov	cx, ( nEntriesInBuffer )
		mul	cx
		add	ax, size QUEUESTRUCT
		add	bx, ax

		mov	word ptr ( lpNameSpace + 0 ), bx
		mov	word ptr ( lpNameSpace + 2 ), es

;---------------------------------------------------------------
; Move all the originators names to their final resting place.
;---------------------------------------------------------------

		Arg	lpqBuffer
		Arg	lpNameSpace
		Arg	nEntriesInBuffer
		cCall	MoveNamesDown

;---------------------------------------------------------------
; Change all the SMB format entries to WinNet format entries.
;---------------------------------------------------------------

		Arg	lpqBuffer
		Arg	lpNameSpace
		Arg	nEntriesInBuffer
		cCall	ConvertEntries

;---------------------------------------------------------------
; Complete the QUEUESTRUCT at the head of the buffer.
;---------------------------------------------------------------

		Arg	lpqBuffer
		Arg	nEntriesInBuffer
		cCall	CompleteQueueStruct

;---------------------------------------------------------------
; Return to the caller.
;---------------------------------------------------------------

cEnd

;===============================================================================
subttl		GetWholeQueue
page
;===============================================================================
;
; DESCRIPTION .
; ENTRY .......
; EXIT ........
; COMMENTS ....
;
;===============================================================================

cProc		GetWholeQueue, <NEAR,PUBLIC>, <si, di>

		parmW	nSessionNumber
		parmD	lpszUser
		parmD	lpqBuffer
		parmD	lpnEntriesReturned

		localW	nReturnCode
		localW	nEntriesInBuffer
		localW	nEntriesInTotal
cBegin
		mov	( nReturnCode ), WN_SUCCESS

;---------------------------------------------------------------
; Read the queue
;---------------------------------------------------------------

		mov	ax, 0
		mov	cx, MSNET_MAX_PRINT_JOBS

		Arg	lpqBuffer				; Pointer to buffer
		Arg	ax					; Start index
		Arg	cx					; Count
		Arg	nSessionNumber				; Session number
		cCall	NETGetPrintQueue			; Fill buffer

		cmp	ax, WN_SUCCESS
		je	SaveEntryCount

		mov	( nReturnCode ), ax
		jmp	GetWholePrintQueueExit

SaveEntryCount: mov	( nEntriesInBuffer ), cx
		mov	( nEntriesInTotal ), cx

;---------------------------------------------------------------
; Convert the SMB format buffer into a WinNet format buffer.
;---------------------------------------------------------------

		Arg	lpqBuffer
		Arg	nEntriesInBuffer
		Arg	nEntriesInTotal
		cCall	ReformatBuffer

;---------------------------------------------------------------
; Return the number of entries in the buffer.
;---------------------------------------------------------------

		les	bx, ( lpnEntriesReturned )
		mov	ax, ( nEntriesInBuffer )
		mov	es: [ bx ], ax

;---------------------------------------------------------------
; Return
;---------------------------------------------------------------

GetWholePrintQueueExit:

		mov	ax, ( nReturnCode )
cEnd

;===============================================================================
; ============= END OF MSNETPRN ================================================
;===============================================================================

sEnd		PRNCODE
		end
