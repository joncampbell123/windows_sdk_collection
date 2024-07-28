page		60, 132
title		Network interface code
;===============================================================================
; Filename	NETIFACE.ASM
; Copyright	(C) 1989 by Research Machines
; Copyright	(C) 1989-1990 by Microsoft Corporation
;===============================================================================
; REVISIONS:	24/02/1989	Initial Version
;		16/03/1989	Change to STANDARD procedures (save si, di, es)
;		23/03/1989	Change to segment name
;		29/03/1989	Removed LDS's
;		31/03/1989	Tidied up comments
;		ever since	Made it work
;===============================================================================

		memM	equ	1		; Middle memory model
		?WIN	=	1		; Windows prolog/epilog
		?PLM	=	1		; Pascal calling convention

		.xlist
include 	cmacros.inc
include 	windows.inc
include 	wnet.inc
include 	netbios.inc
include 	smb.inc
		.list

		.sall

;===============================================================================
; ============= EXTERNAL FUNCTIONS =============================================
;===============================================================================

externFP	UTLMemMove

;===============================================================================
; ============= DATA SEGMENT ===================================================
;===============================================================================

sBegin		DATA

sEnd		DATA

;===============================================================================
; ============= CODE SEGMENT ===================================================
;===============================================================================

createSeg	_PRNT, NETCODE, BYTE, PUBLIC, CODE
sBegin		NETCODE
		assumes CS, NETCODE
		assumes DS, DATA

;===============================================================================
; ============= STATIC FUNTIONS ================================================
;===============================================================================

;===============================================================================
; ============= PUBLIC FUNTIONS ================================================
;===============================================================================

;===============================================================================
subttl		NETNetBiosSend
page
;===============================================================================

cProc		NETNetBiosSend, <NEAR,PUBLIC>

		parmD	lpBuffer
		parmW	nSize
		parmW	nSession

		localV	rgbNCB, <size NCB>
cBegin

		lea	bx, rgbNCB

		mov	ss: [ bx ].ncbCommand, NetBiosSend

		mov	ax, ( nSession )
		mov	ss: [ bx ].ncbLocalSession, al

		mov	ax, word ptr ( lpBuffer + 0 )
		mov	word ptr ss: [ bx ].ncbBuffer + 0, ax
		mov	ax, word ptr ( lpBuffer + 2 )
		mov	word ptr ss: [ bx ].ncbBuffer + 2, ax

		mov	ax, ( nSize )
		mov	ss: [ bx ].ncbLength, ax

		mov	ss: [ bx ].ncbNetworkNumber, 0

;---------------------------------------------------------------
; Send the buffer across the local session.
;---------------------------------------------------------------

		push	ss
		pop	es		    ; es:bx -> NCB

		call	Netbioscall

		cmp	al, WN_SUCCESS
		jne	SendFail

		cmp	es: [ bx ].ncbReturnCode, WN_SUCCESS
		jne	SendFail

		mov	ax, WN_SUCCESS
		jmp	short SendExit

;---------------------------------------------------------------
; return FAILURE
;---------------------------------------------------------------

SendFail:	mov	ax, WN_NET_ERROR

SendExit:

cEnd

;===============================================================================
subttl		NETNetBiosReceive
page
;===============================================================================

cProc		NETNetBiosReceive, <NEAR,PUBLIC>

		parmD	lpBuffer
		parmW	nSize
		parmW	nSession

		localV	rgbNCB, <size NCB>
cBegin

		lea	bx, rgbNCB
		push	ss
		pop	es

		mov	es: [ bx ].ncbCommand, NetBiosReceive

		mov	ax, ( nSession )
		mov	es: [ bx ].ncbLocalSession, al

		mov	ax, word ptr ( lpBuffer + 0 )
		mov	word ptr es: [ bx ].ncbBuffer + 0, ax
		mov	ax, word ptr ( lpBuffer + 2 )
		mov	word ptr es: [ bx ].ncbBuffer + 2, ax

		mov	ax, ( nSize )
		mov	es: [ bx ].ncbLength, ax

		mov	es: [ bx ].ncbNetworkNumber, 0

;---------------------------------------------------------------
; Send the buffer across the local session.
;---------------------------------------------------------------

		call	netbioscall

		cmp	al, WN_SUCCESS
		jne	ReceiveFail

		cmp	es: [ bx ].ncbReturnCode, WN_SUCCESS
		jne	ReceiveFail

;---------------------------------------------------------------
; return SUCCESS
;---------------------------------------------------------------

		mov	ax, WN_SUCCESS
		jmp	short ReceiveExit

;---------------------------------------------------------------
; Free the NCB and return FAILURE
;---------------------------------------------------------------

ReceiveFail:	mov	ax, WN_NET_ERROR

ReceiveExit:

cEnd

;===============================================================================
subttl		NETGetPrintQueue
page
;===============================================================================
;
; DESCRIPTION .
; ENTRY .......
; EXIT ........
; COMMENTS .... The caller must ensure that Buffer is large enough
;		to hold all the print queue entries plus an SMB request
;		header (maximum 1834 bytes). If not, they are
;		lightly soya margarine'ed wholewheat toast.
;
;===============================================================================

cProc		NETGetPrintQueue, <FAR, PUBLIC>, <si, di>

		parmD	lpBuffer
		parmW	nIndex
		parmW	nCount
		parmW	nSessionNumber

		localW	nEntriesReturned
		localW	nReturnCode
cBegin
		mov	( nReturnCode ), WN_SUCCESS		; ever optimistic

;---------------------------------------------------------------
; Use the start of the buffer as an SMB request packet.
;---------------------------------------------------------------

		les	bx, ( lpBuffer )

		; zero out the buffer
		mov	di, bx
		mov	cx, size SMBGetPrintQ / 2
		sub	ax,ax
		rep	stosw

		; initialize the header magic bytes
		mov	byte ptr es: [ bx ].smbHeader + 0, 0ffh
		mov	byte ptr es: [ bx ].smbHeader + 1, 'S'
		mov	byte ptr es: [ bx ].smbHeader + 2, 'M'
		mov	byte ptr es: [ bx ].smbHeader + 3, 'B'

		; set the fields for the 'get spool queue' function
		mov	es: [ bx ].smbCommandCode, SMBReturnPrintQueue
		mov	es: [ bx ].smbParameterCount, 2
		mov	ax, ( nCount )
		mov	es: [ bx ].smbGPQMaxCount, ax
		mov	ax, ( nIndex )
		mov	es: [ bx ].smbGPQStartIndex, ax
		mov	es: [ bx ].smbGPQByteCount, 0

;---------------------------------------------------------------
; Send the request over the specified session.
;---------------------------------------------------------------

		mov	ax, size SMBGetPrintQ			; ?

		Arg	lpBuffer				; -> SMB request
		Arg	ax					; size
		Arg	nSessionNumber				; session
		cCall	NETNetBiosSend

		cmp	ax, WN_SUCCESS
		je	WaitForQueue

		mov	( nReturnCode ), ax
		jmp	short GetPrintQueueExit

;---------------------------------------------------------------
; Wait for the reply to come back.
;---------------------------------------------------------------

WaitForQueue:	mov	ax, size SMBPrintQEntry
		mov	cx, ( nCount )
		mul	cx
		add	ax, size SMBGetPrintQ

		Arg	lpBuffer				; -> SMB request
		Arg	ax					; size
		Arg	nSessionNumber				; session
		cCall	NETNetBiosReceive
								;
		cmp	ax, WN_SUCCESS				;
		jne	NetworkError				;

		les	bx, ( lpBuffer )			;

		cmp	es: [ bx ].smbErrorCodeClass, WN_SUCCESS;
		je	ReturnCount				;

NetworkError:	mov	( nReturnCode ), WN_NET_ERROR
		jmp	short GetPrintQueueExit

;---------------------------------------------------------------
; Find the number of entries in the buffer.
;---------------------------------------------------------------

ReturnCount:	les	bx, ( lpBuffer )
		mov	ax, es: [ bx ].smbGPQMaxCount
		mov	( nEntriesReturned ), ax

;---------------------------------------------------------------
; Shuffle the entries to the head of the buffer, overwriting
; the SMB header.
;---------------------------------------------------------------

		mov	ax, size SMBPrintQEntry 		; Calculate number
		mov	cx, ( nEntriesReturned )		; of bytes used in
		mul	cx					; queue entries.

		les	bx, ( lpBuffer )			; Calculate source
		add	bx, size SMBGetPrintQ			; address for move.

		Arg	lpBuffer				; lpDestination
		Arg	RegPairEsBx				; lpSource
		Arg	ax					; nCount
		cCall	UTLMemMove				;

;---------------------------------------------------------------
; Return to caller.
;---------------------------------------------------------------

GetPrintQueueExit:

		mov	ax, ( nReturnCode )
		mov	cx, ( nEntriesReturned )
cEnd


;===============================================================================
; ============= END OF NETIFACE ================================================
;===============================================================================

sEnd		NETCODE
		end
