page		60, 132
title		Various Utility Functions
;===============================================================================
;		Filename	UTILS.ASM
;		Copyright	(C) 1989 by Research Machines
;				(C) 1989-1990 Microsoft Corp
;===============================================================================
; REVISIONS:	24/02/1989	Initial version
;		15/03/1989	Bug fix UTLToUpper ( remove AX from autosave list )
;		15/03/1989	Add UTLRemoveTrailingColon
;		16/03/1989	Change to STANDARD procedures (save si, di, es)
;		16/03/1989	UTLLocalAlloc returns long pointer to heap
;		16/03/1989	Add UTLAppendTrailingColon
;		22/03/1989	Tidy up functions
;		23/03/1989	Change to segment name
;		29/03/1989	Add UTLIsNetworkPath
;		31/03/1989	Tidied up comments
;		ever since	Make it work for real
;===============================================================================

		memM	equ	1		; Middle memory model
		?WIN	=	1		; Windows prolog/epilog
		?PLM	=	1		; Pascal calling convention

		.xlist
include 	cmacros.inc
include 	windows.inc
include 	wnet.inc
		.list

		.sall

externFP	lstrlen
externFP	lstrcpy

;===============================================================================
; ============= DATA SEGMENT ===================================================
;===============================================================================

sBegin		DATA

;
;	Flags used in WNETPOLL.ASM (address returned by UTLIsSerialDevice()
;
externW 	rgfWatch

CPORTS		equ 4		; number of LPT ports

rgchPort	db  'LPT'	; used for compare
CCHPREFIX	equ 3		; length thereof

sEnd		DATA

;===============================================================================
; ============= CODE SEGMENT ===================================================
;===============================================================================

sBegin		CODE
		assumes CS, CODE
		assumes DS, DATA

;===============================================================================
; ============= PUBLIC FUNTIONS ================================================
;===============================================================================


;===============================================================================
subttl		UTLMemMove
page
;===============================================================================

cProc		UTLMemMove, <FAR, PUBLIC>, <si, di>

		parmD	lpDest
		parmD	lpSource
		parmW	nCount
cBegin
		lds	si, ( lpSource )
		les	di, ( lpDest )
		mov	cx, ( nCount )

		mov	ax, word ptr ( lpSource + 2 )		; ds
		cmp	ax, word ptr ( lpDest + 2 )		; es
		jne	MoveForward

		mov	ax, word ptr ( lpSource + 0 )		; si
		cmp	ax, word ptr ( lpDest + 0 )		; di
		ja	MoveForward

		add	si, cx
		dec	si
		add	di, cx
		dec	di
		std
	    rep movsb
		cld
		jmp	short Moved

MoveForward:	cld
	    rep movsb

Moved:

cEnd

;===============================================================================
subttl		UTLRemoveTrailingSpaces
page
;===============================================================================

cProc		UTLRemoveTrailingSpaces, <FAR, PUBLIC>, <si, di>

		parmD	lpString
		parmW	nSize
cBegin
		les	di, ( lpString )
		mov	cx, ( nSize )
		jcxz	Removed

		add	di, cx
		dec	di

		std
		mov	al, ' '
		repe	scasb
		cld
		mov	byte ptr es:[di+2],0
Removed:
cEnd

;===============================================================================
subttl		UTLRemoveTrailingColon
page
;===============================================================================

cProc		UTLRemoveTrailingColon, <FAR, PUBLIC>, <si, di>

		parmD	lpszString
cBegin
		Arg	lpszString
		cCall	LStrlen
		cmp	ax, 0
		je	RTCExit

		mov	bx, ax
		les	di, ( lpszString )
		cmp	byte ptr es: [ di + bx - 1 ], ':'
		jne	RTCExit

		mov	byte ptr es: [ di + bx - 1 ], NULL

RTCExit:	mov	ax, WN_SUCCESS

cEnd

;===============================================================================
subttl		UTLAppendTrailingColon
page
;===============================================================================

cProc		UTLAppendTrailingColon, <FAR, PUBLIC>, <si, di>

		parmD	lpszString
cBegin
		Arg	lpszString
		cCall	LStrlen
		cmp	ax, 0
		je	ATCExit

		mov	bx, ax
		les	di, ( lpszString )
		cmp	byte ptr es: [ di + bx - 1 ], ':'
		je	ATCExit

		mov	byte ptr es: [ di + bx ], ':'
		mov	byte ptr es: [ di + bx + 1 ], NULL

ATCExit:	mov	ax, WN_SUCCESS

cEnd

;===============================================================================
subttl		UTLToUpper
page
;===============================================================================

cProc		UTLToUpper, <FAR, PUBLIC>, <si, di>

		parmW	nCharacter
cBegin
		mov	ax, ( nCharacter )

		cmp	ax, 'a'
		jb	ToUpperExit
		cmp	ax, 'z'
		ja	ToUpperExit

		sub	ax, 'a'
		add	ax, 'A'

ToUpperExit:

cEnd

;===============================================================================
subttl		UTLStricmpColon
page
;===============================================================================
;
; DESCRIPTION . Compare strings without reguard for case, ignoring colons.
; ENTRY .......
; EXIT ........
; COMMENTS ....
;
;===============================================================================

cProc		UTLStricmpColon, <FAR, PUBLIC>, <si, di>

		parmD	lpszString1
		parmD	lpszString2
cBegin
		lds	si, lpszString1
		les	di, lpszString2

StricmpCChar1:	mov	al, ds: [ si ]				; Char from string1
		inc	si					;
		cmp	al, ':' 				;
		je	StricmpCChar1				;
StricmpCChar2:	mov	ah, es: [ di ]				; Char from string2
		inc	di					;
		cmp	ah, ':' 				;
		je	StricmpCChar2				;

		cmp	ax, 0					; Reached end of both?
		je	UTLStricmpColonExit			; Must be equal

		mov	bl, ah					; make AX = char1
		mov	ah, 0					; make BX = char2
		mov	bh, 0					;

;---------------------------------------------------------------
; Convert characters to upper case
;---------------------------------------------------------------

		push	es
		Arg	ax
		cCall	UTLToUpper
		xchg	ax, bx
		Arg	ax
		cCall	UTLToUpper
		xchg	ax, bx
		pop	es

;---------------------------------------------------------------
; Compare
;---------------------------------------------------------------

		sub	ax, bx					; Chars equal?
		jz	StricmpCChar1				; yep, try next

UTLStricmpColonExit:

cEnd

;===============================================================================
subttl		UTLIsSerialDevice
page
;===============================================================================
;
; DESCRIPTION . If szName != "LPT1:" .. "LPT4:", return NULL
;		For valid LPT's, points to a structure containing the
;		task handle of the task with the port open and the window
;		handle of the window watching the queue.
;
; ENTRY ....... lpszName points to local port
; EXIT ........ Returns non-zero iff device is a local port
;
;===============================================================================

cProc		UTLIsSerialDevice, <FAR, PUBLIC>, <si, di>

    parmD   lpszLocal

cBegin

    les     di,lpszLocal
    lea     si,rgchPort
    mov     cx,CCHPREFIX
    repe    cmpsb		; does it start with LPT? (BUG: Case sensitive)
    jnz     isd_invalid

    mov     al,es:[di]		; ok, which port?
    inc     di
    sub     al,'1'
    cmp     al,CPORTS-1 	; must be 1, 2, 3, or 4
    ja	    isd_invalid
    cbw
    xchg    ax,bx		; save number away in bx
    mov     al,es:[di]		; must end with \0 or :\0
    or	    al,al
    jz	    isd_valid
    cmp     al,':'		; second not colon?
    jnz     isd_invalid 	; invalid
    cmp     byte ptr es:[di+1],0
    jnz     isd_invalid

isd_valid:
    mov     ax, size WatchEntry
    mul     bx
    add     ax, dataoffset rgfWatch

    jmp     short isd_exit

isd_invalid:
    sub     ax,ax		; return NULL

isd_exit:
cEnd

;===============================================================================
subttl		UTLIsNetworkPath
page
;===============================================================================
;		If szName starts with '\\' return TRUE
;===============================================================================

cProc		UTLIsNetworkPath, <FAR, PUBLIC, NODATA>

		parmD	lpszName
cBegin
		les	bx, ( lpszName )
		mov	ax, TRUE
		cmp	word ptr es: [ bx ], '\\'
		je	UTLIsNetworkPathExit
		mov	ax, FALSE

UTLIsNetworkPathExit:

cEnd

;===============================================================================

;===============================================================================
subttl		UTLYearMonthDayToSeconds
page
;===============================================================================

daysafterfeb	dw  31				; mar
		dw  31+30			; apr
		dw  31+30+31			; may
		dw  31+30+31+30 		; jun
		dw  31+30+31+30+31		; jul
		dw  31+30+31+30+31+31		; aug
		dw  31+30+31+30+31+31+30	; sep
		dw  31+30+31+30+31+31+30+31	; oct
		dw  31+30+31+30+31+31+30+31+30	; nov

cProc		UTLYearMonthDayToSeconds, <FAR, PUBLIC, NODATA>

		parmW	nYear
		parmW	nMonth
		parmW	nDay
cBegin

    sub     ax, ax
    sub     dx, dx
    mov     cx, nYear
    mov     bx, 28		    ; number of days in Feb
    test    cx, 3		    ; this year a leaper?
    jnz     ymd_yearloop	    ; nope, skip

    inc     bx			    ; extra day in feb

ymd_yearloop:
    add     ax, 365		    ; 365 days in a year
    test    dx, 3		    ; not divisible by four
    jnz     ymd_notleap 	    ; damn the solar system anyway
    inc     ax			    ; leap year... 366 days
ymd_notleap:
    inc     dx
    loop    ymd_yearloop

    mov     cx, nMonth

    jcxz    ymd_nowdodays	    ; jan
    add     ax, 31
    dec     cx
    jz	    ymd_nowdodays	    ; feb
    add     ax, bx		    ; account for that DAMN leap day
    dec     cx
    jz	    ymd_nowdodays	    ; now march

    dec     cx
    shl     cx, 1
    mov     bx, cx
    add     ax, cs:daysafterfeb[bx] ; look it up

ymd_nowdodays:
    add     ax, nDay

    shl     ax, 1		    ; convert to seconds... days*2
    mov     cx, 12*60*60	    ; 12 = hours, *60*60 = secs
    mul     cx

cEnd

;===============================================================================
subttl		UTLHourMinuteSecondToSeconds
page
;===============================================================================

cProc		UTLHourMinuteSecondToSeconds, <FAR, PUBLIC, NODATA>

		parmW	nHour
		parmW	nMinute
		parmW	nSecond
cBegin
		mov	cx, 60

		mov	ax, ( nHour )			; Hours -> minutes
		mul	cl				; 0 to 1380

		add	ax, ( nMinute ) 		; Minutes -> seconds
		mul	cx				; 0 to 86340

		add	ax, ( nSecond ) 		; Seconds -> seconds
		adc	dx, 0				; 0 to 86399
cEnd

;===============================================================================
; ============= END OF UTILS ===================================================
;===============================================================================

sEnd		CODE
		end
