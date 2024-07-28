page		60, 132
title		Library Initialisation
;===============================================================================
;		Filename	LOADLIB.ASM
;		Copyright	(C) 1989 by Research Machines
;		Copyright	(C) 1989, 1990 Microsoft Corp.
;
;===============================================================================
; REVISIONS:	24/02/1989	Initial version
;===============================================================================

		memM	equ	1			; Middle memory model
		?WIN	=	1			; Windows prolog/epilog
		?PLM	=	1			; Pascal calling convention


		.xlist
include 	cmacros.inc
include 	windows.inc
include 	wnet.inc
include 	wnetcaps.inc
		.list

externFP	PostWarning
externFP	GetProfileInt

;===============================================================================
; ============= DATA SEGMENT ===================================================
;===============================================================================

sBegin		DATA

externW 	CapsTable

globalW 	hLibraryModule, 0, 1

szNetwork	db 'Network',0
szNoQuery	db 'NoQueryPrinter',0

sEnd		DATA

;===============================================================================
; ============= CODE SEGMENT ===================================================
;===============================================================================

sBegin		CODE
		assumes CS, CODE
		assumes DS, DATA

;===============================================================================
subttl		LibraryInitialisation
page
;===============================================================================

cProc		LibraryInit, <FAR, PUBLIC, NODATA>

cBegin
		mov	hLibraryModule, di

		mov	ax,00FFh      ; check if network installed
		int	2Ah
		mov	al,ah	      ; 0 in AH iff net not installed
		cmp	ax,0
		jnz	@F

		cCall	PostWarning

		sub	ax,ax		; return FALSE to unload
		jmp	li_exit

	;   If the user sets [Network]NoQueryPrinter=1
	;   in WIN.INI, disable the spooler print queue functions.
	;   This is for INT 21h compatible networks that do not
	;   support either NetBIOS or the SMB protocol.
	;

@@:		push	ds
		lea	ax, szNetwork
		push	ax
		push	ds
		lea	ax, szNoQuery
		push	ax
		sub	ax, ax
		push	ax
		call	GetProfileInt
		or	ax, ax
		jz	@F

		prtcaps =            WNNC_PRT_OpenJob
		prtcaps = prtcaps or WNNC_PRT_CloseJob

		mov	CapsTable[ 12 ], prtcaps
@@:
		mov	ax, 1
li_exit:
cEnd

cProc		WEP, <FAR, PUBLIC, NODATA>

;   parmW	fLeaving

cBegin	<nogen>

    xor     ax, ax
    retf    2

cEnd	<nogen>

;===============================================================================
; ============= END OF LOADLIB =================================================
;===============================================================================

sEnd		CODE

		end	LibraryInit
