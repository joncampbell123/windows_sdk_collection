page		60, 132
title		Windows Network User Logon / Logoff Functions
;===============================================================================
;		Filename	WNETUSER.ASM
;		Copyright	(C) 1989 by Research Machines
;		Copyright	(C) 1989-1990 by Microsoft Corporation
;===============================================================================
; REVISIONS:	24/02/1989	Initial version
;		23/03/1989	Change to segment name / file name
;		23/03/1989	Update to spec 0.58
;		31/03/1989	Tidied up comments
;===============================================================================

		memM	equ	1		; Middle memory model
		?WIN	=	1		; Windows prolog/epilog
		?PLM	=	1		; Pascal calling convention

		.xlist
include 	cmacros.inc
include 	windows.inc
include 	wnet.inc
include 	wnetcaps.inc
		.list

		.sall

externFP	UTLRemoveTrailingSpaces 			; In file UTILS.ASM
externFP	MSNetGetMachineName				; In file MSNET.ASM

externFP	lstrlen 		    ; in KERNEL
externFP	lstrcpy 		    ; in KERNEL

;===============================================================================
; ============= DATA SEGMENT ===================================================
;===============================================================================

sBegin		DATA

sEnd		DATA

;===============================================================================
; ============= CODE SEGMENT ===================================================
;===============================================================================

sBegin		CODE
		assumes CS, CODE
		assumes DS, DATA

;===============================================================================
; ============= EXPORTED FUNTIONS ==============================================
;===============================================================================

;===============================================================================
subttl		WNetGetUser
page
;===============================================================================
;
; DESCRIPTION . Return the current user name.
;		This is the same as the machine name.
; ENTRY ....... lpszUser points to the user supplied buffer.
;		lpnBufferSize points to a word holding the size of szUser.
; EXIT ........ AX hold WN_SUCCESS or not.
;		If success, szUser is filled with the current user name.
;		nBufferSize is set to the length of the current user name.
; COMMENT ..... Uses DosGetMachineName (5e00h)
;
;===============================================================================

cProc		WNetGetUser, <FAR, PUBLIC>, <si>

		parmD	lpszUser				; Pointer to buffer
		parmD	lpnBufferSize				; Size of buffer

		localW	nReturnCode				; Return code
		localV	szT, 16
cBegin

;---------------------------------------------------------------
; Get the machine name
;---------------------------------------------------------------

WNGUGetName:	push	ss
		lea	si,szT
		push	si
		cCall	MSNetGetMachineName			;

		cmp	ax, WN_SUCCESS				; Successful?
		je	TidyUserName				; Yes!

		mov	ax, WN_NET_ERROR			; Set return code
		jmp	short WNGUExit				; Exit
TidyUserName:

ifdef NO_DO_THIS_KEMOSABE
;---------------------------------------------------------------
; Remove trailing spaces from the user name
;---------------------------------------------------------------

		Arg	RegPairSSSI
		cCall	lstrlen 				; get length of username

		Arg	RegPairSSSI				; Remove trailing spaces
		Arg	ax					;
		cCall	UTLRemoveTrailingSpaces 		;
endif

;---------------------------------------------------------------
; Check if the users buffer is large enough to hold the name.
; Update nBufferSize to actual.
;---------------------------------------------------------------

		push	ss
		lea	ax, szT
		push	ax
		cCall	lstrlen 				;
		inc	ax					; Plus terminating NULL

		les	bx, ( lpnBufferSize )			; Pointer to buffer size
		xchg	ax, es: [ bx ]				; Update to size required
		cmp	ax, es: [ bx ]				; Large enough?
		jae	CopyUserName				; Yes!

		mov	ax, WN_MORE_DATA			; Name will not fit
		jmp	short WNGUExit

;---------------------------------------------------------------
; Copy the machine name to the user supplied buffer
;---------------------------------------------------------------

CopyUserName:	Arg	lpszUser				; -> user buffer
		Arg	RegPairSSSI				; -> machine name
		cCall	lstrcpy 				; Move those bytes

		mov	ax,WN_SUCCESS

;---------------------------------------------------------------
; Return to the caller
;---------------------------------------------------------------

WNGUExit:

cEnd

;===============================================================================
; ============= END OF WNETUSER ================================================
;===============================================================================

sEnd		CODE
		end
