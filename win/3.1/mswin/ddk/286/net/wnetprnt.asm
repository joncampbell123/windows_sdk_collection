page		60, 132
title		Windows Network Printing Functions
;===============================================================================
;		Filename	WNETPRNT.ASM
;		Copyright	(C) 1989 by Research Machines
;				(C) 1989-1990 Microsoft Corp.
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
;		ever since	Make it work
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

externFP	MSNetFindLocalSession			; In file MSNET.ASM
externFP	MSNetMakeAssignListEntry		; In file MSNET.ASM
externFP	MSNetCancelAssignListEntry		; In file MSNET.ASM
externFP	MSNetCreateHandle			; In file MSNET.ASM
externFP	MSNetCloseHandle			; In file MSNET.ASM

externFP	WNetGetConnection

externFP	UTLMemMove				; In file UTILS.ASM
externFP	UTLRemoveTrailingSpaces 		; In file UTILS.ASM
externFP	UTLRemoveTrailingColon			; In file UTILS.ASM
externFP	UTLIsSerialDevice

externFP	lstrcpy
externFP	GetCurrentTask

;===============================================================================
; ============= DATA SEGMENT ===================================================
;===============================================================================

sBegin		DATA

externW 	rgfWatch

staticW 	hTimer, 0, 1
staticW 	nWatchedQueues, 0, 1
globalW 	QueueHandleTable, 0, MAX_WATCH_QUEUE

sEnd		DATA

;===============================================================================
; ============= CODE SEGMENT ===================================================
;===============================================================================

createSeg	_PRNT, PRNTCODE, BYTE, PUBLIC, CODE
sBegin		PRNTCODE
		assumes CS, PRNTCODE
		assumes DS, DATA


;===============================================================================
subttl		WNetOpenJob
page
;===============================================================================

cProc		WNetOpenJob, <FAR, PUBLIC>, <si, di>

		parmD	lpszQueue
		parmD	lpszJobTitle
		parmW	nCopies
		parmD	lphPrintFile

		localV	szPort, 64
cBegin

;---------------------------------------------------------------
; Determine if the the port is a redirected port
;---------------------------------------------------------------

		push	seg_lpszQueue
		push	off_lpszQueue
		call	UTLIsSerialDevice
		or	ax, ax
		jz	OJ_BadQueue

		mov	di, ax
		cmp	[di].hTask, 0
		jz	OJ_CheckRedir

		mov	ax, WN_ALREADY_LOCKED
		jmp	short WNOJExit

OJ_CheckRedir:
		lea	si,szPort
		les	bx, lphPrintFile
		mov	word ptr es:[bx],64
		cCall	WNetGetConnection,<lpszQueue,RegPairSSSI,RegPairESBX>
		or	ax,ax
		jnz	WNOJExit	    ; if not redir'd, return error.

;---------------------------------------------------------------
; Copy the queue name to the local heap in case we need to alter it.
;---------------------------------------------------------------


DuplicateName:	Arg	RegPairSSSI
		Arg	lpszQueue
		cCall	LStrcpy

;---------------------------------------------------------------
; lpszQueue may be a device name (LPT1, LPT2...), in which case
; we must remove the trailing colon.
;---------------------------------------------------------------

		Arg	RegPairSSSI
		cCall	UTLRemoveTrailingColon

;---------------------------------------------------------------
; Open the print job with DosCreateHandle.
;---------------------------------------------------------------

		mov	ax, 0					; File attribute (NORMAL)
		Arg	RegPairSSSI				;
		Arg	ax					;
		cCall	MSNetCreateHandle			;
		or	ax, ax
		jns	ReturnHandle

		neg	ax			; make error positive
		jmp	WNOJExit
OJ_BadQueue:
		mov	ax, WN_BAD_QUEUE
		jmp	WNOJExit

;---------------------------------------------------------------
; Return the DOS file handle in pFH
;---------------------------------------------------------------

ReturnHandle:	les	bx, ( lphPrintFile )
		mov	es: [ bx ], ax

;---------------------------------------------------------------
; Only one task should have a net port open at any given time.
; Enforce this.
;---------------------------------------------------------------
		mov	[di].hf, ax				 ; save handle for close
		call	GetCurrentTask
		mov	[di].hTask, ax				    ; save current task handle
		sub	ax,ax					; return success

;---------------------------------------------------------------
; If ( nCopies ) == 1 then we have succeeded.
; If ( nCopies ) != 1 then return WN_CANT_SET_COPIES.
;---------------------------------------------------------------

WNOJSuccess:	cmp	( nCopies ), 1
		je	WNOJExit

		mov	ax, WN_CANT_SET_COPIES

WNOJExit:
cEnd

;===============================================================================
subttl		WNetCloseJob
page
;===============================================================================
;
; DESCRIPTION .
; ENTRY .......
; EXIT ........
; COMMENTS ....
;
;===============================================================================

cProc		WNetCloseJob, <FAR, PUBLIC>

		parmW	hPrintFile
		parmD	lpJobID
		parmD	lpszQueue
		localW	nReturnCode
cBegin
		mov	( nReturnCode ), WN_SUCCESS		; Ever optimistic

		call	GetCurrentTask
		mov	dx, hPrintFile
		mov	bx, dataoffset rgfWatch
		mov	cx, 4
find_handle_loop:
		cmp	[bx].hTask, ax
		jnz	loopit
		cmp	[bx].hf, dx
		jz	found_handle
loopit:
		add	bx, size WatchEntry
		loop	find_handle_loop

		mov	ax, WN_BAD_FILE_HANDLE

		jmp	short WNCJExitNow

found_handle:
		mov	[bx].hTask, 0

;---------------------------------------------------------------
; Close file with DosCloseHandle.
;---------------------------------------------------------------

		Arg	hPrintFile				;
		cCall	MSNetCloseHandle			;

		cmp	ax, WN_SUCCESS				;
		je	WNCJExitNow
		mov	( nReturnCode ), WN_NET_ERROR		; Set return code
WNCJExitNow:
cEnd

;=============================================================================
;
;

cProc	WNetWriteJob, <FAR, PUBLIC>

    parmW   hJob
    parmD   lpData
    parmD   lpcb

cBegin
    push    ds
    lds     dx, lpData
    les     bx, lpcb
    mov     cx, es:[bx]
    mov     bx, hJob
    mov     ah, 40h
    call    dos3call
    pop     ds
    jc	    wj_error
    les     bx, lpcb
    mov     es:[bx], ax
    sub     ax, ax
    jmp     short wj_exit
wj_error:
    mov     ax, WN_NET_ERROR
wj_exit:
cEnd

;===============================================================================
; ============= END OF WNETPRNT ================================================
;===============================================================================

sEnd		PRNTCODE
		end
