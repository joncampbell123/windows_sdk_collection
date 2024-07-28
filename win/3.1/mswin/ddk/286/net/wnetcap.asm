page		60, 132
title		Windows Network Capabilitity Functions
;===============================================================================
;		Filename	WNETCAP.ASM
;		Copyright	(C) 1989 by Research Machines
;		Copyright	(C) 1989, 1990 Microsoft Corporation
;===============================================================================
; REVISIONS:	24/02/1989	Initial version
;		16/03/1989	Change to STANDARD procedures (save si, di, ds)
;		1/5/89		Made it real
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

externFP	MessageBox

;===============================================================================
sBegin		CODE
		assumes cs, CODE
		assumes ds, DATA
;===============================================================================


;===============================================================================
subttl		WNetGetCaps
page
;===============================================================================

cProc		WNetGetCaps, <FAR, PUBLIC>

		parmW	nIndex
cBegin
		mov	ax, NULL

		mov	bx, ( nIndex )
		cmp	bx, WNNC_SPEC_VERSION
		jb	WNetGetCapsExit
		cmp	bx, WNNC_ERROR
		ja	WNetGetCapsExit

		dec	bx
		shl	bx, 1
		mov	ax, word ptr CapsTable[ bx ]

WNetGetCapsExit:

cEnd

;===============================================================================
sEnd		CODE
;===============================================================================
sBegin		DATA
;===============================================================================

public		CapsTable
labelW		CapsTable

		dw	0310h			; Spec Version

		dw	WNNC_NET_MSNet		; Net type

		dw	0300h			; Driver Version

		dw	WNNC_USR_GetUser	; username caps

		dw 	0			; index 5 unused

		concaps	=            WNNC_CON_AddConnection
		concaps = concaps or WNNC_CON_CancelConnection
		concaps = concaps or WNNC_CON_GetConnections
		dw	concaps			; connection caps

		prtcaps =            WNNC_PRT_OpenJob
		prtcaps = prtcaps or WNNC_PRT_CloseJob
		prtcaps = prtcaps or WNNC_PRT_WatchQueue
		prtcaps = prtcaps or WNNC_PRT_UnwatchQueue
		prtcaps = prtcaps or WNNC_PRT_LockQueueData
		prtcaps = prtcaps or WNNC_PRT_UnlockQueueData
		prtcaps = prtcaps or WNNC_PRT_ChangeMsg
		prtcaps = prtcaps or WNNC_PRT_NoArbitraryLock
		prtcaps = prtcaps or WNNC_PRT_WriteJob
		dw	prtcaps			; printing caps

		dlgcaps =	     WNNC_DEV_DevMode
		dw	dlgcaps

		dw	0			; index 9 unused

		dw	0			; no error caps

;===============================================================================
sEnd		DATA
;===============================================================================

		end
