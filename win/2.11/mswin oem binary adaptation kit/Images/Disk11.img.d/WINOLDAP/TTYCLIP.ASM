
	.xlist
	include cmacros.inc

	NOTEXT		equ	1
	NORASTOPS	equ	1
	NOVK		equ	1
	NOMB		equ	1
	NOWM		equ	1
	NOSYSMETRICS	equ	1
	include windows.inc

	include tty.inc
	include ttyclip.inc
	include grabber.inc
	.list


;++++++++++++++++++++++++++++++++++++++++
;
; MICROSOFT TRIP BUG FIXES
;
; Fixed stack problems in MetaW2G routine  11/1/87
; Moved call to UpdateBadItemOne before GlobalUnlock so ES does not get trashed
;   before the call  (In MetaW2G)
; Added size of header to pointer before calling AnsiToOem in TextW2G.	This
;   is so we don't convert our format and size to ANSI.



;windows functions
ExternFP	ANSItoOEM
ExternFP	CloseClipboard
ExternFP	CreateBitMap
ExternFP	EmptyClipboard
ExternFP	GetBitmapBits
ExternFP	GetBitmapDimension
ExternFP	GetClipboardData
ExternFP	GetCurrentTask
ExternFP	GetDC
ExternFP	GetDeviceCaps
ExternFP	GetMetaFileBits
ExternFP	GetObject
ExternFP	GlobalAlloc
ExternFP	GlobalCompact
ExternFP	GlobalFree
ExternFP	GlobalLock
ExternFP	GlobalSize
ExternFP	GlobalUnlock
ExternFP	OemToAnsi
ExternFP	OpenClipboard
ExternFP	ReleaseDC
ExternFP	SetBitmapDimension
ExternFP	SetClipboardData
ExternFP	SetMetaFileBits


;winoldap data
ExternW 	ClipFormat
ExternW 	ClipHandle
ExternB 	PifBehavior
ExternW 	HandleSave
ExternW 	hDC
ExternW 	hTTYWnd
ExternW 	BufSeg
ExternW 	BufSize
ExternW 	Partition

;winoldap functions
ExternNP	DisableGrabber
ExternNP	EnableGrabber
ExternNP	MyAlloc
ExternNP	MyUnlock
ExternNP	RestoreVector
ExternNP	SetVector
ExternNP	GetOldApDS
ExternNP	AddANSIToClipboard

sBegin	code

	assumes cs, code
	assumes ds, data

	public	BadAppDataX
	public	BadCBCompact
	public	BadCloseCB
	public	BadEmptyCB
	public	BadEnumCBFormats
	public	BadGetCBData
	public	BadGetCBDataSize
	public	BadGetFreeSpPtr
	public	BadMatchItem
	public	BadOpenCB
	public	BadRegisterCBFormat
	public	BadSetCBData
	public	BadUpdateItemOne
	public	BadWriteCheck
	public	CBGetDeviceCaps
	public	CBIdentify
	public	DisableClipboard
	public	EnableClipboard
	public	GetAllDeviceCaps
	public	GetBadClipboard
	public	GoodCBCompact
	public	GoodCloseCB
	public	GoodEmptyCB
	public	GoodEnumCBFormats
	public	GoodGetCBData
	public	GoodGetCBDataSize
	public	GoodOpenCB
	public	GoodRegisterCBFormat
	public	GoodSetCBData
	public	Int2FHandler
	public	TextW2G
	public	BitW2G
	public	MetaW2G
	public	TextG2W
	public	BitG2W
	public	MetaG2W
	public	TextA2G
	public	BitA2G
	public	MetaA2G
	public	TextG2A
	public	BitG2A
	public	MetaG2A

	public	CallBadCB, RetBadCB	;purely for debugging

GlobalW 	CBDeviceCaps, 0, ASPECTXY	;table of devcaps

; int 2F storage

globalD 	prevInt2FProc,	?

;	booleans

GlobalB 	bCBOpen, 0 ;=1 if bad app CB currently open
GlobalB 	bCBWrite, 0;=1 if the bad app CB has been written to

;	data related to keeping track of contents of grabbuf

GlobalW 	CBItemTab, 0, 8 	  ;table of ptrs into grabbuf
GlobalB 	CBItemCount, 0		  ;= number of items

ClipBitmap	Bitmap	<>

;	App's Stack Segment and Stack Pointer

globalW 	AppSS,	0
globalW 	AppSP,	0

;	Jump table for procedures

GoodCBServices	dw	CBIdentify
		dw	GoodOpenCB
		dw	GoodEmptyCB
		dw	GoodSetCBData
		dw	GoodGetCBDataSize
		dw	GoodGetCBData
		dw	GoodRegisterCBFormat
		dw	GoodEnumCBFormats
		dw	GoodCloseCB
		dw	GoodCBCompact
		dw	CBGetDeviceCaps

BadCBServices	dw	CBIdentify
		dw	BadOpenCB
		dw	BadEmptyCB
		dw	BadSetCBData
		dw	BadGetCBDataSize
		dw	BadGetCBData
		dw	BadRegisterCBFormat
		dw	BadEnumCBFormats
		dw	BadCloseCB
		dw	BadCBCompact
		dw	CBGetDeviceCaps

;============================================================================
;			    GetAllDeviceCaps
;============================================================================
;
; Purpose	Fills in a table with DeviceCaps for all devices.
;
; Parameters	None.
;
; Return Values None.
;
; Notes 	Uses ax, bx.
;		Called at WinOldAp initialization.
;		hTTYWnd must must must be valid.
;		This routine is used for good and bad apps.
;
;============================================================================
GetAllDeviceCaps	proc	near
	cCall	GetDC, <[hTTYWnd]>
	mov	bx, ax			;bx = hDC for hTTYWnd

	push	es
	push	cs
	pop	es
	mov	di, codeOFFSET CBDeviceCaps ;the table to fill in
	mov	si, DRIVERVERSION
GDCLoop:
	push	bx			;keep track of hDC
	cCall	GetDeviceCaps, <bx, si> ;get a device at a time
	pop	bx
	stosw
	add	si, 2
	cmp	si, ASPECTXY		;last device
	jle	GDCLoop
	pop	es

	cCall	ReleaseDC, <[hTTYWnd], bx>
	ret
GetAllDeviceCaps	endp


;============================================================================
;			      EnableClipboard
;============================================================================
;
; Purpose	Prepares for clipboard services
;
; Notes 	See ttyoem.asm and ttytask.asm for parallelisms to
;		keyboard, etc.
;
;============================================================================
EnableClipboard proc	near
	push	ds
	mov	ax, 352FH
	int	21H
	mov	word ptr cs:[prevInt2Fproc], bx
	mov	word ptr cs:[prevInt2Fproc+2], es

	mov	dx, codeOFFSET Int2FHandler
	mov	al, 2FH
	call	SetVector
	pop	ds

	ret
EnableClipboard endp


;============================================================================
;			     DisableClipboard
;============================================================================
;
; Purpose	Restores Int 2FH
;
;============================================================================
DisableClipboard	proc	near
	push	ds
	lds	dx, cs:[prevInt2FProc]
	mov	al, 2FH
	call	RestoreVector
	pop	ds
	ret
DisableClipboard	endp


;============================================================================
;			      Int2FHandler
;============================================================================
;
; Purpose	Dispatch to services for DOS apps running under WinOldAp
;
; Parameters	ax = SubFunction, other registers vary based on SubFunction.
;		ds = application data segment
;		ss = application stack segment
;
; Return Values ax = 0: Error
;		   <> 0: OK
;
; Notes 	Identify must be called first to verify presence of services.
;
;============================================================================
Int2FHandler	proc	near
	cmp	ah, 17H
	je	WOAClipCheck1
	jmp	cs:[prevInt2FProc]
WOAClipCheck1:
	cmp	al, 0AH
	jle	WOAClipCheck2
	jmp	cs:[prevInt2FProc]
WOAClipCheck2:
	push	bp
	push	bx
	push	cx
	push	si
	push	di
	push	ds
	push	es
	pushf
	cld

	call	GetOldApDS			;ds = winoldap data seg
	xor	ah, ah
	mov	di, ax
	shl	di, 1
	test	[PIFBehavior], BADBITS
	jz	CBGoodApp
CallBadCB:
	call	BadWriteCheck
	call	cs:[di + BadCBServices]
RetBadCB:
	jmp	CBDone
CBGoodApp:

	mov	ax, ss
	mov	cs:[AppSS], ax
	mov	ax, sp
	mov	cs:[AppSP], ax

	push	CX
	push	DX
	push	ES
	cCall	GetCurrentTask
	mov	ds, ax
	pop	ES
	pop	DX
	pop	CX

	cli
	mov	ss, ds:[TDB_taskSS]
	mov	sp, ds:[TDB_taskSP]
	sti
	mov	bp, sp
	add	bp, TASKBP

	call	GetOldapDS
	call	cs:[di + GoodCBServices]

	cli
	mov	DI, cs:[AppSS]
	mov	ss, DI
	mov	DI, cs:[AppSP]
	mov	sp, DI
	sti

CBDone:
	popf
	pop	es
	pop	ds
	pop	di
	pop	si
	pop	cx
	pop	bx
	pop	bp
	iret

Int2FHandler	endp

;============================================================================
;			  WOACF2CF, CF2WOACF
;============================================================================
;
; Purpose	Converts to/from WOACF_ format ids from/to Windows CF_
;
; Parameters	dx = format id
;
; Return Values dx = conversion of id
;
; Notes 	These routines are necessary because of deviations in
;		the buffer structure of the ids, from the definition
;		of the windows clipboard ids.  The WOACF_ ids are the ones
;		that are stored in the grabbuf.  The CF_ ids are the ones
;		used for the clipboard API and interfacing to real windows.
;
;============================================================================
WOACF2CF	proc	near
	cmp	dx, WOACF_OEMTEXT
	jne	TryTextWOACF2
	mov	dx, CF_OEMTEXT
	jmp	TryNoMoreWOACF2
TryTextWOACF2:
	cmp	dx, WOACF_TEXT
	jne	TryDspTextWOACF2
	mov	dx, CF_TEXT
	jmp	TryNoMoreWOACF2
TryDspTextWOACF2:
	cmp	dx, WOACF_DSPTEXT
	jne	TryBitmapWOACF2
	mov	dx, CF_DSPTEXT
	jmp	TryNoMoreWOACF2
TryBitmapWOACF2:
	cmp	dx, WOACF_BITMAP
	jne	TryDspBitmapWOACF2
	mov	dx, CF_BITMAP
	jmp	TryNoMoreWOACF2
TryDspBitmapWOACF2:
	cmp	dx, WOACF_DSPBITMAP
	jne	TryMetaWOACF2
	mov	dx, CF_DSPBITMAP
	jmp	TryNoMoreWOACF2
TryMetaWOACF2:
	cmp	dx, WOACF_METAFILEPICT
	jne	TryDspMetaWOACF2
	mov	dx, CF_METAFILEPICT
	jmp	TryNoMoreWOACF2
TryDspMetaWOACF2:
	cmp	dx, WOACF_DSPMETAFILEPICT
	jne	ErrorWOACF2
	mov	dx, CF_DSPMETAFILEPICT
	jmp	TryNoMoreWOACF2
ErrorWOACF2:
	xor	dx, dx
TryNoMoreWOACF2:
	ret
WOACF2CF	endp


CF2WOACF	proc	near
	cmp	dx, CF_OEMTEXT
	jne	TryTextCF2
	mov	dx, WOACF_OEMTEXT
	jmp	TryNoMoreCF2
TryTextCF2:
	cmp	dx, CF_TEXT
	jne	TryDspTextCF2
	mov	dx, WOACF_TEXT
	jmp	TryNoMoreCF2
TryDspTextCF2:
	cmp	dx, CF_DSPTEXT
	jne	TryBitmapCF2
	mov	dx, WOACF_DSPTEXT
	jmp	TryNoMoreCF2
TryBitmapCF2:
	cmp	dx, CF_BITMAP
	jne	TryDspBitmapCF2
	mov	dx, WOACF_BITMAP
	jmp	TryNoMoreCF2
TryDspBitmapCF2:
	cmp	dx, CF_DSPBITMAP
	jne	TryMetaCF2
	mov	dx, WOACF_DSPBITMAP
	jmp	TryNoMoreCF2
TryMetaCF2:
	cmp	dx, CF_METAFILEPICT
	jne	TryDspMetaCF2
	mov	dx, WOACF_METAFILEPICT
	jmp	TryNoMoreCF2
TryDspMetaCF2:
	cmp	dx, CF_DSPMETAFILEPICT
	jne	ErrorCF2
	mov	dx, WOACF_DSPMETAFILEPICT
	jmp	TryNoMoreCF2

ErrorCF2:
	xor	dx, dx

TryNoMoreCF2:
	ret
CF2WOACF	endp




;============================================================================
;				TextW2G
;============================================================================
;
; Purpose	Copies Text from Windows clipboard to Grabbuf
;
; Parameters	ax = hClipData
;		di = windows clipboard format  (CF)
;		ds, ss = WinOldAp data segment
;
; Notes 	If the real windows clipboard contains ANSI text but not
;		OEM text, this proc will copy the text into the grabbuf
;		and convert to OEM, and then will try to copy it again
;		as ANSI.
;
;		Uses es, si, di, ax, bx, cx, dx.
;
;============================================================================
TextW2G proc	near

	push	bp
	mov	bp, sp
	sub	sp, SizeTW2GFrame

SizeTW2GFrame	equ	16

hClipDataTW2G	equ	word ptr [bp - 2]
CFormatTW2G	equ	word ptr [bp - 4]
WOACFormatTW2G	equ	word ptr [bp - 6]
DstOffTW2G	equ	word ptr [bp - 8]
SrcSegTW2G	equ	word ptr [bp - 10]
SrcOffTW2G	equ	word ptr [bp - 12]
TextSizeTW2G	equ	word ptr [bp - 14]
bBadAddANSITW2G equ	word ptr [bp - 16]

	push	ds
	push	es

	mov	hClipDataTW2G, ax		;save stuff
	mov	CFormatTW2G, di
	mov	dx, di
	call	CF2WOACF
	mov	WOACFormatTW2G, dx
	mov	bBadAddANSITW2G, 0		;initialize to zero

	cCall	GlobalLock, <hClipDataTW2G>	;save pointer to data
	mov	SrcSegTW2G, dx
	mov	SrcOffTW2G, ax

	cCall	GlobalSize, <hClipDataTW2G>	;get size to ask for
	or	dx, dx
	jnz	TrunTextW2G
	mov	cx, ax
	jmp	SaveSizeTW2G
TrunTextW2G:
	mov	cx, 0FFFFH
SaveSizeTW2G:
	mov	TextSizeTW2G, cx
	mov	es, BufSeg

	mov	dx, CFormatTW2G 	;if we're not getting ansi text
	cmp	dx, CF_TEXT		;then continue on
	jnz	GetTextW2G
	mov	dx, CF_OEMTEXT		;if already oem text in the bad
	mov	ds, BufSeg		;clipboard then continue on
	call	BadMatchItem
	jnc	GetTextW2G
	mov	bBadAddAnsiTW2G, 1
	mov	WOACFormatTW2G, WOACF_OEMTEXT

GetTextW2G:
	mov	cx, TextSizeTW2G	;the desired size
	push	ss
	pop	ds
	call	BadGetFreeSpPtr
	mov	DstOffTW2G, di		;save ptr to start of data
	jnc	NoTrunTW2G		;if we got desired size continue
	mov	cx, ax			;else use whatever is remaining
	jcxz	EndTextW2G		;if nothing left then quit
NoTrunTW2G:
	mov	ds, SrcSegTW2G		;ds:si -> clipboard data
	mov	si, SrcOffTW2g

	mov	ax, WOACFormatTW2G
	stosw
	mov	ax, cx
	stosw
	rep	movsb
	dec	di
	xor	al, al
	stosb

	call	BadUpdateItemOne		;inc cbitemcount

	mov	ax, bBadAddAnsiTW2G		;do we store as oem?
	or	ax, ax
	jz	EndTextW2G
	mov	di, DstOffTW2G			;convert to oem


; MSF  Must add size of header or we will convert our format and size also
	add	di,4				; account for header
; MSF Done

	RegPtr	lpStr, es, di
	cCall	AnsiToOem, <lpStr, lpStr>
	mov	WOACFormatTW2G, WOACF_TEXT
	mov	bBadAddAnsiTW2G, 0
	jmp	GetTextW2G

EndTextW2G:
	cCall	GlobalUnlock, <hClipDataTW2G>
	pop	es
	pop	ds
	add	sp, SizeTW2GFrame
	pop	bp
	ret

TextW2G endp

;============================================================================
;				BitW2G
;============================================================================
;
; Purpose	Copies a bitmap from Windows clipboard to Grabbuf
;
; Parameters	ax = hClipData
;		di = windows clipboard format (CF)
;		ds, ss = WinOldAp data segment
;
;		Uses es, si, di, ax, bx, cx, dx.
;
;============================================================================
BitW2G	proc	near
	push	bp
	mov	bp, sp
	sub	sp, SizeBW2GFrame

SizeBW2GFrame	equ	8

hClipDataBW2G	equ	[bp - 2]
FormatBW2G	equ	[bp - 4]
SizeBW2G	equ	[bp - 6]
BufOffsetBW2G	equ	[bp - 8]

	push	ds

	mov	hClipDataBW2G, ax
	mov	dx, di
	call	CF2WOACF
	mov	FormatBW2G, dx

	mov	ax, 22			;sizeof bitmap struc
	mov	si, codeOffset ClipBitmap
	RegPtr	lpObject, cs, si
	cCall	GetObject, <hClipDataBW2G, ax, lpObject>
	mov	ax, word ptr cs:[ClipBitmap + bmHeight]
	mul	word ptr cs:[ClipBitmap + bmWidthBytes]
	add	ax, 12
	mov	SizeBW2G, ax

	mov	es, [BufSeg]		;es:di -> grab buffer
	mov	cx, ax
	call	BadGetFreeSpPtr 	;can it all fit?
	jnc	BitSizeOkBW2G		;yes, copy it
	jmp	EndBitW2G		;no, quit

BitSizeOkBW2G:
	mov	BufOffsetBW2G, di

	push	cs			;ds:si -> ClipBitmap
	pop	ds

	mov	ax, FormatBW2G		;format
	stosw
	mov	ax, cx			;size
	stosw
	lodsw				;bmType, ignore for grabber
	lodsw				;bmWidth
	stosw
	lodsw				;bmHeight
	stosw
	lodsw				;bmWidthBytes
	lodsb				;bmPlanes
	xor	ah, ah
	stosw
	lodsb				;bmBitsPixel
	stosw

	mov	si, hClipDataBW2G	;hBitmap
	push	es
	cCall	GetBitmapDimension, <si>;dx = height in .1mm; ax = width
	pop	es
	stosw
	mov	ax, dx
	stosw

	mov	ax, SizeBW2G
	sub	ax, 12
	xor	bx, bx
	RegPtr	dwCount, bx, ax
	RegPtr	lpBits, es, di
	push	es
	cCall	GetBitmapBits, <si, dwCount, lpBits>
	pop	es
	call	BadUpdateItemOne
EndBitW2G:
	pop	ds
	add	sp, SizeBW2GFrame
	pop	bp
	ret
BitW2G	endp



;============================================================================
;				MetaW2G
;============================================================================
;
; Purpose	Copies a metafile from Windows clipboard to Grabbuf
;
; Parameters	ax = hClipData
;		di = windows clipboard format (CF)
;		ds, ss = WinOldAp data segment
;
; Notes 	Uses es, si, di, ax, bx, cx, dx.
;
;============================================================================
MetaW2G proc	near
	push	bp
	mov	bp, sp
	sub	sp, SizeMW2GFrame

SizeMW2GFrame	equ	12

hClipDataMW2G	equ	[bp - 2]
FormatMW2G	equ	[bp - 4]
MaxSizeMW2G	equ	[bp - 6]
BufOffsetMW2G	equ	[bp - 8]
hMetafileMW2G	equ	[bp - 10]
BufSegMW2G	equ	[bp - 12]

	push	ds
	mov	hClipDataMW2G, ax	;hMetafile STRUCTURE
	mov	dx, di
	call	CF2WOACF
	mov	FormatMW2G, dx

	mov	es, [BufSeg]
	mov	BufSegMW2G, es
	call	BadGetFreeSpPtr
	mov	MaxSizeMW2G, ax
	mov	BufOffsetMW2G, di

	cCall	GlobalLock, <hClipDataMW2G>
	mov	ds, dx			;ds:si -> windows metafile struc
	mov	si, ax

	mov	es, BufSegMW2G
	mov	ax, FormatMW2G		;write format into grabbuf
	stosw
	xor	ax, ax			;don't know size yet
	stosw
	mov	cx, 3			;mm, xExt, yExt
	rep	movsw
	lodsw				;ax = hMF
	mov	hMetafileMW2G, ax
	cCall	GlobalUnlock, <hClipDataMW2G>
;;;	cCall	GetMetaFileBits, <hMetafileMW2G>
;;;	mov	hMetafileMW2G, ax
	cCall	GlobalSize, <hMetafileMW2G>
	add	ax, 6			;MSF remove push AX
	mov	bx, MaxSizeMW2G
	cmp	ax, bx

; MSF The following code has been modified by RCP 11/1/87

	jbe	SizeOkMetaW2G		;if bad we must clean up GrabBuf
	mov	es, BufSegMW2G
	sub	di,10			;10 bytes were pre-setup
	xor	ax,ax			;want to write zeros
	mov	cx,5			;5 words
	rep	stosw
	jmp	EndMetaW2G



SizeOkMetaW2G:
	push	ax			 ; MSF We save the size here
	cCall	GlobalLock, <hMetafileMW2G>
	pop	cx
; MSF Don't need size since we have it and stackis right

;	pop	cx			;size of metafile bits

; MSF Done
	mov	es, BufSegMW2G
	mov	bx, BufOffsetMW2G
	mov	es:[bx + gbSize], cx
	sub	cx, 6
	mov	ds, dx			;pointer to bits from globallock
	mov	si, ax
	rep	movsb			;copy the bits in

;MSF Put the call here - see below
	call	BadUpdateItemOne
; MSF Done

	cCall	GlobalUnlock, <hMetafileMW2G>
;	inc	cs:[CBItemCount]

; MSF Move this call before GlobalUnlock so ES does not get trashed
;	call	BadUpdateItemOne
;MSF Done

EndMetaW2G:
	pop	ds
	add	sp, SizeMW2GFrame
	pop	bp
	ret
MetaW2G endp


;============================================================================
;			   BadUpdateItemOne
;============================================================================
;
; Purpose	Mark high bit of format id of first item in grabbuf
;
; Parameters	es = BufSeg
;
; Return Values CBItemCount = CBItemCount + 1
;
; Notes 	Called by routines that place data in grabbuf at switchin
;		time.
;
;		Uses ax
;
;============================================================================
BadUpdateItemOne proc	 near
	mov	ax, es:0
	or	ah, 10000000B
	mov	word ptr es:0, ax
	inc	cs:[CBItemCount]
	ret
BadUpdateItemOne endp


;============================================================================
;			      GetBadClipboard
;============================================================================
;
; Purpose	Gets data from windows clipboard and puts in buffer
;
; Parameters	ds, ss = WinOldAp data segment
;
; Notes 	This is called when we are switching into a bad app.
;
;============================================================================
GetBadClipboard:
	mov	CX,[BufSeg]		;get grabber (destination) segment
	or	cx, cx
	jnz	TryAllGBC
	jmp	EndGBC
TryAllGBC:
	mov	cs:[CBItemCount], 0
	cCall	OpenClipboard, <hTTYWnd>

	mov	di, CF_OEMTEXT
	cCall	GetClipboardData, <di>
	or	ax, ax
	jz	TryTextGBC
	call	TextW2G
TryTextGBC:
	mov	di, CF_TEXT
	cCall	GetClipboardData, <di>
	or	ax, ax
	jz	TryDspTextGBC
	call	TextW2G
TryDspTextGBC:
	mov	di, CF_DSPTEXT
	cCall	GetClipboardData, <di>
	or	ax, ax
	jz	TryBitmapGBC
	call	TextW2G
TryBitmapGBC:
	mov	di, CF_BITMAP
	cCall	GetClipboardData, <di>
	or	ax, ax
	jz	TryMetaGBC
	call	BitW2G
TryDspBitmapGBC:
	mov	di, CF_DSPBITMAP
	cCall	GetClipboardData, <di>
	or	ax, ax
	jz	TryMetaGBC
	call	BitW2G
TryMetaGBC:
	mov	di, CF_METAFILEPICT
	cCall	GetClipboardData, <di>
	or	ax, ax
	jz	TryDspMetaGBC
	call	MetaW2G
TryDspMetaGBC:
	mov	di, CF_DSPMETAFILEPICT
	cCall	GetClipboardData, <di>
	or	ax, ax
	jz	EndGBC
	call	MetaW2G
EndGBC:
	cCall	CloseClipboard
	mov	cs:[bCBWrite], 0
	ret


;============================================================================
;				TextG2W
;============================================================================
;
; Purpose	Copies text from Grabbuf to Windows
;
; Parameters	dx = format (WOACF)
;		ax = size
;		ds:si -> text in grabbuf
;
; Return Values si = wFormat (CF)
;		di = hMem
;
; Notes 	If OEM and not ANSI, this routine will first render
;		OemToAnsi( OEM ) and then render OEM.
;
;============================================================================
TextG2W proc	near
	push	bp
	mov	bp, sp
	sub	sp, SizeTG2WFrame

SizeTG2WFrame	equ	14

BufSegTG2W	equ	word ptr	[bp - 2]
OldApSegTG2W	equ	word ptr	[bp - 4]
DataOffTG2W	equ	word ptr	[bp - 6]
WOACFormatTG2W	equ	word ptr	[bp - 8]
hTextTG2W	equ	word ptr	[bp - 10]
TextSizeTG2W	equ	word ptr	[bp - 12]
bBadAddOEMTG2W	equ	word ptr	[bp - 14]

	mov	WOACFormatTG2W, dx
	mov	DataOffTG2W, si
	mov	BufSegTG2W, ds
	mov	OldApSegTG2W, ss
	inc	ax		;make room for null at end
	mov	TextSizeTG2W, ax
	mov	bBadAddOEMTG2W, 0

	cmp	dx, WOACF_OEMTEXT	;if not OEM continue
	jnz	SetTextG2W
	mov	dx, CF_TEXT		;if already ANSI continue
	cCall	GetClipboardData, <dx>
	or	ax, ax
	jnz	SetTextG2W
	mov	bBadAddOEMTG2W, 1
SetTextG2W:
	mov	ds, OldApSegTG2W
	mov	ax, TextSizeTG2W
	call	MyAlloc 	;get movable data, locked
	jnz	AllocTextG2W
	jmp	End2TextG2W
AllocTextG2W:
	mov	es, dx			;es:di -> global memory object
	mov	di, ax
	mov	ds, BufSegTG2W		;ds:si -> data
	mov	si, DataOffTG2W
	mov	cx, TextSizeTG2W	;cx = size
	rep	movsb
	mov	byte ptr es:[di], 0	;null terminate

	mov	cx, bBadAddOEMTG2W
	jcxz	EndTextG2W
	sub	di, TextSizeTG2W
	regptr	lpStr, es, di
	cCall	OemToAnsi, <lpStr, lpStr>
	mov	ds, OldApSegTG2W
	call	MyUnlock
	mov	ax, CF_TEXT
	cCall	SetClipboardData, <ax, di>
	mov	bBadAddOEMTG2W, 0
	jmp	SetTextG2W

EndTextG2W:
	mov	ds, OldApSegTG2W
	call	MyUnlock		;returns handle in di
	mov	dx, WOACFormatTG2W
	call	WOACF2CF
	cCall	SetClipboardData, <dx, di>
End2TextG2W:
	mov	ds, BufSegTG2W
	add	sp, SizeTG2WFrame
	pop	bp
	ret
TextG2W endp


;============================================================================
;				BitG2W
;============================================================================
;
; Purpose	Copies a bitmap from grabbuf to windows
;
; Parameters	dx = format (WOACF)
;		ax = size
;		ds:si -> data in grabbuf
;
; Return Values si = wFormat (CF)
;		di = hMem
;
;============================================================================
BitG2W	proc	near
	mov	di, dx
	sub	si, 4			;point si to start of grab struc
	mov	bx, si
	add	bx, gbWidth2		;short format bits offset
	regptr	pB, ds, bx
	cmp	di, WOACF_SHORTBITMAP	;short bitmap?
	jz	uhb1
	add	bx, gbBits - gbWidth2	;Extended format
uhb1:
cCall CreateBitmap,<[si.gbWidth],[si.gbHeight],[si.gbPlanes],[si.gbPixel], pB>
	or	ax,ax			;OK?
	jnz	CrBitOkBDX
	inc	[ClipHandle]
	jmp	End2BitG2W

CrBitOkBDX:
	push	di			;wFormat
	xchg	di, ax			;di now = hBitmap
	cmp	ax, WOACF_SHORTBITMAP	;short bitmap?
	jz	EndBitG2W		;yes, don't set dimensions
SetBmDimBDX:				;set dimensions
	cCall	SetBitmapDimension, <di, [si.gbWidth2], [si.gbHeigh2]>
EndBitG2W:
	pop	dx			;wFormat
	call	WOACF2CF		;convert to windows format
	cCall	SetClipboardData, <dx, di>
End2BitG2W:
	ret
BitG2W	endp


;============================================================================
;				MetaG2W
;============================================================================
;
; Purpose	Copies a metafile from grabbuf to windows
;
; Parameters	dx = format (WOACF)
;		ax = size
;		ds:si -> data in grabbuf
;
; Return Values si = wFormat (CF)
;		di = hMem
;
;============================================================================
MetaG2W proc	near
	push	bp
	mov	bp, sp
	sub	sp, SizeMG2WFrame

SizeMG2WFrame	equ	12

BufSegMG2W	equ	[bp - 2]
OldApSegMG2W	equ	[bp - 4]
DataOffMG2W	equ	[bp - 6]
WOACFormatMG2W	equ	[bp - 8]
hMetafileMG2W	equ	[bp - 10]
SizeMG2W	equ	[bp - 12]

	mov	BufSegMG2W, ds
	mov	OldApSegMG2W, ss
	mov	DataOffMG2W, si
	mov	WOACFormatMG2W, dx


	;first allocate enough memory for the metafile data

	sub	ax, 6		;subtract out header size
	mov	SizeMG2W, ax
	mov	ds, OldApSegMG2W
	call	MyAlloc
	jnz	HeaderMG2W
	jmp	EndMG2W
HeaderMG2W:
	mov	es, dx		;es:di -> global memory
	mov	di, ax
	mov	ds, BufSegMG2W	;ds:si -> metafile header
	add	si, 6		;point si past header towards bits
	mov	cx, SizeMG2W	;size of bits
	rep	movsb		;copy metafile bits
	mov	ds, OldApSegMG2W
	mov	di, [HandleSave]
	cCall	SetMetaFileBits, <di>
	call	MyUnlock	;returns hMem in di and ax
;	cCall	SetMetaFileBits, <di>
	mov	hMetafileMG2W, ax

	;now allocate enough memory for the metafile structure

	mov	ax, 8		;sizeof mm, xExt, yExt, hMetafile
	mov	ds, OldApSegMG2W
	call	MyAlloc
	jnz	BodyMG2W
	cCall	GlobalFree, <hMetafileMG2W>
	jmp	EndMG2W
BodyMG2W:
	mov	es, dx
	mov	di, ax
	mov	ds, BufSegMG2W
	mov	si, DataOffMG2W
	mov	cx, 3		;mm, xExt, yExt
	rep	movsw
	mov	ax, hMetafileMG2W
	stosw
	mov	ds, OldApSegMG2W
	call	MyUnlock	;di = hMem
	mov	dx, WOACFormatMG2W
	call	WOACF2CF
	cCall	SetClipboardData, <dx, di>
EndMG2W:
	add	sp, SizeMG2WFrame
	pop	bp

	ret
MetaG2W endp


;============================================================================
;				BadWriteCheck
;============================================================================
;
; Purpose	Tests if someone else (MCP, snapshot) modified grabbuf.
;
; Parameters	ds = WinOldAp dataseg
;
; Return Values If grabbuf modified, bCBWrite set,
;				     CBItemCount set to 1.
;
;============================================================================
BadWriteCheck	 proc	 near

	push	ds
	push	si
	push	ax

	mov	ds, [BufSeg]
	xor	si, si
	lodsw
	or	ah, ah				;if ah is zero
	jnz	EndBWrChk
	or	al, al				;and al is non-zero
	jz	EndBWrChk
	mov	cs:[bCBWrite], 1		;it has been written to
	mov	cs:[CBItemCount], 1
EndBWrChk:
	pop	ax
	pop	si
	pop	ds
	ret
BadWriteCheck	 endp


;============================================================================
;				BadAppDataX
;============================================================================
;
; Purpose	Copies data from bad app cb (grabbuf) to windows cb
;
; Parameters	ds, ss = WinOldAp data segment
;
; Notes 	Uses all but ds, ss, bp
;
;		If data grabbed
;			ClipHandle = Handle to global object containing data
;			ClipFormat = Format of data
;		else
;			ClipHandle == 0 if no data
;			ClipHandle == 1 if memory problem rendering data
;
;============================================================================
BadAppDataX	proc	near
	cmp	[Partition], 0		; Skip if partition has been released
	je	NoBDX

	mov	cx,[BufSeg]		;get handle to Grab Segment
	jcxz	NoBDX			;If no grab segment, done

	call	BadWriteCheck		;check to see if snapshot or MCP

	mov	al, cs:[bCBWrite]
	or	al, al			;any updating necessary?
	jz	NoBDX

;	xor	ch, ch
;	mov	cl, cs:[CBItemCount]	;how many items do we have
;	jcxz	NoBDX			;if none then done
	jmp	DoBDX

NoBDX:
	ret

DoBDX:
	push	cx
	cCall	OpenClipboard, <hTTYWnd>
	cCall	EmptyClipboard
	pop	cx

	push	ds
	mov	ds, [BufSeg]

	mov	dx, CF_OEMTEXT
	call	BadMatchItem
	jc	TryTextBDX
	mov	dx, cx
	call	TextG2W
TryTextBDX:
	mov	dx, CF_TEXT
	call	BadMatchItem
	jc	TryDspTextBDX
	mov	dx, cx
	call	TextG2W
TryDspTextBDX:
	mov	dx, CF_DSPTEXT
	call	BadMatchItem
	jc	TryBitmapBDX
	mov	dx, cx
	call	TextG2W
TryBitmapBDX:
	mov	dx, CF_BITMAP
	call	BadMatchItem
	jc	TryDspBitmapBDX
	mov	dx, cx
	call	BitG2W
TryDspBitmapBDX:
	mov	dx, CF_DSPBITMAP
	call	BadMatchItem
	jc	TryMetaBDX
	mov	dx, cx
	call	BitG2W
TryMetaBDX:
	mov	dx, CF_METAFILEPICT
	call	BadMatchItem
	jc	TryDspMetaBDX
	mov	dx, cx
	call	MetaG2W
TryDspMetaBDX:
	mov	dx, CF_DSPMETAFILEPICT
	call	BadMatchItem
	jc	TryNoMoreBDX
	mov	dx, cx
	call	MetaG2W
TryNoMoreBDX:

	pop	ds
	cCall	CloseClipboard

	mov	[ClipHandle], 0
;	mov	es,[BufSeg]
;	mov	es:[gbSize],0	; remember that we grabbed it already
	call	AddANSIToClipboard
	ret
BadAppDataX	endp

;============================================================================
;				 CBIdentify
;============================================================================
;
; Purpose	IdentifyWinOldApVersion
;
; Parameters	ax = 1700H
;
; Return Values ax = 1700H: Clipboard functions not available.
;		   <> 1700H: al = Major Version Number
;			     ah = Minor Version Number
;
; Notes 	Call this guy first to be sure!
;
;============================================================================
CBIdentify     proc    near
	mov	ax, 00002H
	ret
CBIdentify     endp

;============================================================================
;			       CBGetDeviceCaps
;============================================================================
;
; Purpose	GetDeviceCaps
;
; Parameters	ax = 170AH
;		dx = GDI information index
;
; Return Values ax = integer value of desired item
;
; Notes 	Same routine for good/bad.
;
;============================================================================
CBGetDeviceCaps proc	near
	mov	bx, codeOFFSET CBDeviceCaps
	add	bx, dx
	mov	ax, word ptr cs:[bx]
	ret
CBGetDeviceCaps endp


;============================================================================
;			      GoodOpenCB
;============================================================================
;
; Purpose	OpenClipboard
;
; Parameters	ax = 1701H
;
; Return Values ax = 0: Clipboard already open
;		   <> 0: OK
;
; Notes
;
;============================================================================
GoodOpenCB	proc	near

	cCall	OpenClipboard,<hTTYWnd>
	ret

GoodOpenCB	endp

;============================================================================
;			       GoodEmptyCB
;============================================================================
;
; Purpose	EmptyClipboard
;
; Parameters	ax = 1702H
;
; Return Values ax = 0: error occured
;		   <> 0: ok, clipboard emptied
;
; Notes
;
;============================================================================
GoodEmptyCB	proc	near

	cCall	EmptyClipboard
	ret

GoodEmptyCB	endp



;============================================================================
;			    GoodSetCBData
;============================================================================
;
; Purpose	SetClipboardData
;
; Parameters	ax = 1703H
;		dx = WinOldAp-supported clipboard format
;		es:bx = pointer to data
;		si:cx = size of the data
;
; Return Values ax = 0: error occured
;		   <> 0: ok
;
; Notes
;
;============================================================================
GoodSetCBData	proc	near

	push	BP
	mov	BP,SP
	sub	SP,SizeClipFrame

SizeClipFrame	equ	16

OldapDataSeg	equ	[bp - 2]
OldapDataOff	equ	[bp - 4]
SizeHigh	equ	word ptr [bp - 6]
SizeLow 	equ	word ptr [bp - 8]
SourceSeg	equ	[bp - 10]
SourceOff	equ	[bp - 12]
gHandle 	equ	word ptr [bp - 14]
wFmt		equ	word ptr [bp - 16]


	push	DS
	mov	OldapDataOff,BX
	mov	OldapDataSeg,ES
	mov	SizeHigh,SI
	mov	SizeLow,CX
	mov	wFmt,DX
	cmp	DX,CF_TEXT
	jz	GSD10
	cmp	DX,CF_OEMTEXT
	jz	GSD10
	cmp	DX,CF_BITMAP
	jz	GSD20
	cmp	DX,CF_DSPBITMAP
	jz	GSD20
	cmp	DX,CF_METAFILEPICT
	jnz	GSD05				;relative jump out of range
	jmp	GSD30
GSD05:
	cmp	DX,CF_DSPMETAFILEPICT
	jnz	GSD06				;relative jump out of range
	jmp	GSD30

GSD06:	jmp	GSD99				;unsupported format


GSD10:
	mov	AX,GMEM_MOVEABLE+GMEM_ZEROINIT+GMEM_SHAREALL
	regptr	dwSize,SI,CX
	cCall	GlobalAlloc,<AX,dwSize>
	or	AX,AX				;see if we can get data
	jz	GSD99				;error can't alloc
	mov	gHandle,AX			;save hMem
	cCall	GlobalLock,<AX> 		;lock to get absolute address
	mov	DI,AX				;offset in DI destination
	mov	ES,DX				;segment in ES ES:DI
	mov	DS,OldapDataSeg
	mov	SI,OldapDataOff
	mov	BX,SizeHigh
	mov	CX,SizeLow
	call	DataToWinoldap

GSD15:
	cCall	SetClipboardData,<wFmt,gHandle>      ;the crux of the routine
	push	AX
	push	DX
	cCall	GlobalUnlock,<gHandle>		     ;unlock the memory
	pop	DX
	pop	AX				;AX has error code

GSD80:
	pop	DS
	add	SP,SizeClipFrame
	pop	BP
	ret
GSD99:
	xor	AX,AX
	jmp	GSD80

GSD20:
	mov	DI,BX
	add	BX,SIZE woabitmap
	regptr	pBitMap,ES,BX
	xor	CX,CX
	mov	DX,CX
	mov	CL,ES:[DI.woabmPlanes]
	mov	DL,ES:[DI.woabmBitsPixel]
	cCall	CreateBitMap,<ES:[DI.woabmWidth],ES:[DI.woabmHeight],CX,DX,pBitMap>
	or	AX,AX
	jz	GSD99
	mov	gHandle,AX
	cCall	SetBitMapDimension,<gHandle,ES:[DI.woabmWidth2],ES:[DI.woabmHeight2]>
	cCall	SetClipboardData,<wFmt,gHandle>
	jmp	GSD80

GSD30:
	mov	AX,GMEM_MOVEABLE+GMEM_ZEROINIT+GMEM_SHAREALL
	regptr	dwSize,SI,CX
	cCall	GlobalAlloc,<AX,dwSize>
	or	AX,AX				;check if we can get data
	jz	GSD99				;no return
	push	AX
	cCall	GlobalLock,<AX>
	mov	ES,DX
	mov	DI,AX
	mov	DS,OldapDataSeg
	mov	SI,OldapDataOff
	add	SI,SIZE woametafilepict
	sub	SI,2				;one handle is one word
	mov	CX,SizeLow
	mov	BX,SizeHigh
	sub	CX,SIZE woametafilepict
	add	CX,2				;one handle
	call	DataToWinoldap
	pop	AX
	mov	DI,AX
	cCall	SetMetaFileBits,<AX>
	mov	gHandle,AX
	mov	AX,DI
	cCall	GlobalUnlock,<AX>
	xor	SI,SI
	mov	CX,SIZE woametafilepict
	mov	AX,GMEM_MOVEABLE+GMEM_ZEROINIT+GMEM_SHAREALL
	regptr	dwSize,SI,CX
	cCall	GlobalAlloc,<AX,dwSize>
	or	AX, AX
	jnz	GSD40
	jmp	GSD99
GSD40:
	push	AX				;this is the clipboard handle
	cCall	GlobalLock,<AX>
	mov	ES,DX
	mov	DI,AX
	mov	DS,OldapDataSeg
	mov	SI,OldapDataOff
	mov	CX,3
	rep	movsw
	mov	AX,gHandle
	stosw					;now hMF is in clipboard area
	pop	AX
	mov	DI,AX			;save hMem
	cCall	SetClipboardData,<wFmt,AX>
	mov	AX,DI
	cCall	GlobalUnlock,<AX>
	mov	AX,1
	jmp	GSD80				;dummy

GoodSetCBData	endp

;============================================================================
;			   GoodGetCBDataSize
;============================================================================
;
; Purpose	GetClipboardDataSize
;
; Parameters	ax = 1704H
;		dx = WinOldAp-supported clipboard format
;
; Return Values dx:ax size of data in bytes of given format
;		   = 0 if data of format given is not present
;
; Notes
;
;============================================================================
GoodGetCBDataSize	proc	near

	push	BP
	mov	BP,SP

	mov	DI,DX				;save the value
	cCall	GetClipBoardData,<DX>
	or	AX,AX
	jnz	GDD01
	jmp	GetSizeError			;no data of given format
GDD01:
	mov	BX,DI				;format in BX
	cmp	BX,CF_TEXT
	jz	GDS10
	cmp	BX,CF_OEMTEXT
	jz	GDS10
	cmp	BX,CF_BITMAP
	jz	GDS20
	cmp	BX,CF_DSPBITMAP
	jz	GDS20
	cmp	BX,CF_METAFILEPICT
	jz	GDS30
	cmp	BX,CF_DSPMETAFILEPICT
	jz	GDS30
	jmp	GetSizeError

GDS10:
	cCall	GlobalSize,<AX>
	jmp	GetSizeExit			;exit

GDS20:
	sub	sp,SIZE BITMAP
	mov	BX,SP
	regptr	lpBM,SS,BX
	mov	CX, SIZE BITMAP 		;AX has hClipMem
	cCall	GetObject,<AX,CX,lpBM>
	mov	BX,SP
	cmp	AX,SIZE BITMAP			;now AX has SIZE of BITMAP
	jnz	GDS25				;something wrong!
	mov	AX,SS:[BX.bmWidthBytes] 	;get width of line
	xor	CX,CX
	mov	CL,SS:[BX.bmPlanes]		;now the number of planes
	mul	CX				;DX:AX is Width*Planes
	mul	SS:[BX.bmHeight]		;DX:AX is Width*Planes*Height
	add	AX,SIZE WOABITMAP		;add the size of structure
	adc	DX,0				;if more than 64kb

GDS25:
	add	SP,SIZE BITMAP
	jmp	GetSizeExit

GDS30:
	mov	DI,AX
	cCall	GlobalSize,<AX>
	mov	AX,DI
	cCall	GlobalLock,<AX>
	mov	DS,DX
	mov	BX,AX				;
	mov	AX,DS:[BX.woahMeta]
	cCall	GlobalSize,<AX> 		;size in DX:AX
	add	AX,SIZE WOAMETAFILEPICT
	sub	AX,2				;sub one handle length
	push	DX
	push	AX
	cCall	GlobalUnlock,<DI>
	pop	AX
	pop	DX
	jmp	GetSizeExit

GetSizeError:
	xor	DX,DX

GetSizeExit:
	pop	BP
	ret


GoodGetCBDataSize	endp


;============================================================================
;			       GoodGetCBData
;============================================================================
;
; Purpose	GetClipboardData
;
; Parameters	ax = 1705H
;		dx = WinOldAp-supported clipboard format
;		es:bx = pointer to location to place data
; Return Values ax = 0: error occured
;		   <> 0: ok
;
; Notes
;
;============================================================================
GoodGetCBData	proc	near

	push	BP
	mov	BP,SP
	sub	SP,SizeClip

SizeClip	equ	16

OldDataSeg	equ	[bp - 2]
OldDataOff	equ	[bp - 4]
DataHigh	equ	word ptr [bp - 6]
DataLow 	equ	word ptr [bp - 8]
SrcSeg		equ	[bp - 10]
SrcOff		equ	[bp - 12]
gHand		equ	word ptr [bp - 14]
wFrmt		equ	word ptr [bp - 16]

	mov	OldDataSeg,ES			;save ES
	mov	OldDataOff,BX			;and BX
	mov	wFrmt,DX			;and format

	cCall	GetClipboardData,<DX>
	mov	gHand,AX			;clipboard hClipMem
	or	AX,AX
	jz	GetDataError			;no data of this format
	mov	CX,wFrmt
	cmp	CX,CF_TEXT
	jnz	GGD01
	jmp	GetGoodText			;get text data
GGD01:
	cmp	CX,CF_OEMTEXT
	jnz	GGD02
	jmp	GetGoodText			;for oem text also
GGD02:
	cmp	CX,CF_BITMAP
	jnz	GGD03
	jmp	GetGoodBitmap		;get bitmap for goodapp
GGD03:
	cmp	CX,CF_DSPBITMAP
	jnz	GGD04
	jmp	GetGoodBitmap
GGD04:
	cmp	CX,CF_METAFILEPICT
	jnz	GGD05
	jmp	GetGoodMeta
GGD05:
	cmp	CX,CF_DSPMETAFILEPICT
	jnz	GetDataError
	jmp	GetGoodMeta

GetDataError:

	xor	DX,DX				;error

GetDataExit:

	add	SP, SizeClip
	pop	BP
	ret


GetGoodText:

	cCall	GlobalSize,<gHand>	   ;size of the data in clipboard
	mov	DataHigh,DX		;high word of size
	mov	DataLow,AX		;low word of size
	cCall	GlobalLock,<gHand>
	mov	DS,DX			;segment address in DS
	mov	SI,AX			;source address offset
	mov	BX,DataHigh		;high word of size
	mov	CX,DataLow
	mov	DI,OldDataOff		;destination from BX
	mov	ES,OldDataSeg		;restore ES
	call	DataToWinoldap		;general routine

	cCall	GlobalUnlock,<gHand>
;	cCall	GlobalFree,<gHand>
	mov	AX,1			;if AX = 0 it is error
	jmp	GetDataExit

GetGoodBitmap:

	les	DI,OldDataOff			;retrive ES:BX into ES:DI
	push	ES
	regptr	lpOldapBM,ES,DI
	mov	CX,SIZE BITMAP
	cCall	GetObject,<gHandle,cx,lpOldapBM>
	pop	ES
	cmp	AX,SIZE BITMAP
	jnz	GetDataError
	mov	word ptr ES:[DI.woabmBits.2],ES
	mov	word ptr ES:[DI.woabmBits],DI
	add	word ptr ES:[DI.woabmBits], SIZE WOABITMAP

	cCall	GetBitMapDimension,<gHandle>
	mov	word ptr ES:[DI.woabmWidth2],AX
	mov	word ptr ES:[DI.woabmHeight2],DX

	mov	AX,ES:[DI.woabmWidthBytes]		  ;get width of line
	xor	CX,CX
	mov	CL,ES:[DI.woabmPlanes]
	mul	CX				;DX:AX has planes*WidthBytes
	mul	ES:[DI.woabmHeight]		;DX:AX has planes*WidthBytes*Ht

	mov	BX,word ptr ES:[DI.woabmBits]
	regptr	lpBits,ES,BX
	regptr	lpSzBits,DX,AX
	cCall	GetBitMapBits,<gHand,lpSzBits,lpBits>
	or	AX,DX				;check if successful
	jz	GGD_OUT 			;something wrong??
	jmp	GetDataExit			;everything OK!!
GGD_OUT:
	jmp	GetDataError			;relative jump out of range


GetGoodMeta:
	push	AX
	cCall	GlobalLock,<AX> 		;AX is hClipMem
	mov	DS,DX
	mov	SI,AX
	mov	ES,OldDataSeg
	mov	DI,OldDataOff
	mov	CX,3
	rep	movsw				;three parameters
	mov	AX,DS:[SI]			;now AX has hMF
	mov	gHand,AX
	pop	AX
	cCall	GlobalUnlock,<AX>
	cCall	GlobalSize,<gHand>
	mov	CX,AX				;low size in CX
	mov	BX,DX				;high size in BX
	cCall	GlobalLock,<gHand>
	mov	DS,DX
	mov	SI,AX				;offset
	mov	ES,OldDataSeg
	mov	DI,OldDataOff			;destination offset
	add	DI,SIZE woametafilepict
	sub	DI,2				;size of hMeta
	call	DataToWinoldap			;copy data from global to oldap
	cCall	GlobalUnlock,<gHand>		;unlock hMem
	mov	AX,1				;if zero error
	jmp	GetDataExit

GoodGetCBData	endP

;--------DataToWinoldap------------------------------------
;
;	 DESCRIPTION
;		Copies data to/from Windows to Winoldap
;
;	 ENTRY
;		ES:DI	-> destination address
;		DS:SI	-> source address
;		BX	-> size of data high word (can be larager than 64K)
;		CX	-> size of data low word
;
;	 EXIT
;		data moved
;	 USES
;
;----------------------------------------------------------

DataToWinoldap:

	or	BX,BX			;is data >64Kb ?
	jz	DTW20			;less than 64Kb
	push	CX
DTW10:
	xor	CX,CX
	rep	movsb
	dec	BX
	jnz	DTW10
	pop	CX
DTW20:
	rep	movsb
	ret

;============================================================================
;			 GoodRegisterCBFormat
;============================================================================
;
; Purpose	RegisterClipboardFormat
;
; Notes 	Not Implemented.
;
;============================================================================
GoodRegisterCBFormat	proc	near
	xor	ax, ax
	ret
GoodRegisterCBFormat	endp


;============================================================================
;			    GoodEnumCBFormats
;============================================================================
;
; Purpose	EnumClipboardFormats
;
; Notes 	Not Implemented.
;
;============================================================================
GoodEnumCBFormats	proc	near
	xor	ax, ax
	ret
GoodEnumCBFormats	endp


;============================================================================
;				 GoodCloseCB
;============================================================================
;
; Purpose	CloseClipboard
;
; Parameters	ax = 1708H
;
; Return Values ax = 0: error occured
;		   <> 0: ok
;
; Notes
;
;============================================================================
GoodCloseCB	proc	near

	cCall	CloseClipboard
	ret

GoodCloseCB	endp




;============================================================================
;			       GoodCBCompact
;============================================================================
;
; Purpose	Determine how much memory is available for placing data.
;
; Parameters	ax = 1709H
;
; Return Values dx:ax = number of bytes available
;
; Notes
;
;============================================================================
GoodCBCompact	proc	near
	mov	DX,1
	mov	CX,0FFFFh
;	xor	CX,CX				;low word of long
	cCall	GlobalCompact,<DX,CX>		;for 64k + 1 bytes
	ret

GoodCBCompact	endp

;============================================================================
;			      BadOpenCB
;============================================================================
;
; Purpose	Bad App OpenClipboard (ax = 1701H)
;
; Parameters	ds = WinOldAp data segment
;		ss = application stack segment
;
; Return Values ax = 0: Clipboard already open
;		   <> 0: OK
;
; Notes 	Nothing done except to refer to status byte for future ops
;		If status is still open at switch time, then we keep windows
;		clipboard open too.
;
;============================================================================
BadOpenCB	proc	near
	mov	cs:bCBOpen, 1
	call	DisableGrabber
	inc	ax
	ret
BadOpenCB	endp


;============================================================================
;			       BadEmptyCB
;============================================================================
;
; Purpose	Bad App EmptyClipboard (ax = 1702H)
;
; Parameters	ds = WinOldAp data segment
;		ss = application stack segment
;
; Return Values ax = 0: error occured
;		   <> 0: ok, clipboard emptied
;
;============================================================================
BadEmptyCB	proc	near
	mov	al, cs:bCBOpen
	or	al, al
	jz	EndBEmp

	push	cs			;clear out the table
	pop	es
	mov	di, codeOFFSET CBItemTab
	mov	cx, 8
	xor	ax, ax
	rep	stosw
	mov	cs:[CBItemCount], 0	;set counter to zero (MOST important)

	mov	cs:[bCBWrite], 1	;mark as being updated

	mov	ax, 1
	mov	es, [BufSeg]		;make buffer think it is empty too!
	mov	es:[gbType], 0
	mov	es:[gbSize], 0
EndBEmp:
	ret
BadEmptyCB	endp


;============================================================================
;				TextA2G
;============================================================================
;
; Purpose	Copies text data from app to grabbuf.
;
; Parameters	cx = size of data
;		es:di -> where to place data
;
;============================================================================
TextA2G proc	near
	call	CF2WOACF
	mov	ax, dx
	stosw
	mov	ax, cx		;size
	stosw
	rep	movsb
	mov	cs:[bCBWrite], 1
	inc	cs:[CBItemCount]
	ret
TextA2G endp


;============================================================================
;				BitA2G
;============================================================================
;
; Purpose	Copies bitmap data from app to grabbuf.
;
; Parameters	cx = size of data
;		es:di -> where to place data
;
;============================================================================
BitA2G	proc	near
	lodsw			;read bmType = 0, but ignore
	call	CF2WOACF
	mov	ax, dx
	stosw			;grabbuf id
	push	cx
	mov	ax, cx		;woaclip size
	sub	ax, 4		;correct for grabber size difference
	stosw			;gbSize
	lodsw			;width
	stosw
	lodsw			;height
	stosw
	lodsw			;widthbytes
	lodsb			;planes
	xor	ah, ah
	stosw
	lodsb			;bitspixel
	stosw
	lodsw			;qword pointer to bits
	lodsw
	lodsw
	lodsw
	lodsw			;width2
	stosw
	lodsw			;heigh2
	stosw
	pop	cx		;size
	sub	cx, 22		;subtract sizeof woaclip bitmap header
	rep	movsb
	mov	ax, 1
	mov	cs:[bCBWrite], 1
	inc	cs:[CBItemCount]
	ret
BitA2G	endp

;============================================================================
;				MetaA2G
;============================================================================
;
; Purpose	Copies metafile data from app to grabbuf.
;
; Parameters	cx = size of data
;		es:di -> where to place data
;
;============================================================================
MetaA2G proc	near
	call	CF2WOACF
	mov	ax, dx
	stosw
	mov	ax, cx		;ax = size
	stosw
	rep	movsb
	mov	cs:[bCBWrite], 1
	inc	cs:[CBItemCount]
	ret
MetaA2G endp

;============================================================================
;				BadGetFreeSpPtr
;============================================================================
;
; Purpose	Determines offset of next available free space in grabbuf.
;
; Parameters	cx = size of data
;		ds = WinOldAp data segment
;		es = BufSeg
;
; Return Values di = offset of BufSeg where data may be added.
;		ax = free space left
;		carry set if cx > free space left
;
; Notes 	CBItemTab is updated to point to the next item,
;		whether or not the size of the data will fit.
;
;============================================================================
BadGetFreeSpPtr proc	near
	push	bx

	mov	di, codeOFFSET CBItemTab
	xor	bx, bx			;set di to point to next free space
	mov	bl, cs:[CBItemCount]
	or	bx, bx			;is this the 1st entry or the Nth?
	jnz	ItemNBFrSpPtr
	xchg	bx, di			;1st
	jmp	FillTabBFrSpPtr
ItemNBFrSpPtr:
	sub	bx, 1			;Nth
	shl	bx, 1			;bx=offset of entry for N-1 entry
	add	di, bx			;di-> pointer to last item
	mov	bx, di
	mov	di, cs:[di]		;di-> last item
	add	di, es:[di + 2] 	;di-> next free space - header size
	add	di, 4			;di-> next free space
	add	bx, 2			;bx-> next entry
FillTabBFrSpPtr:
	mov	cs:[bx], di		;set ptr into bufseg
	mov	ax, [BufSize]
	cmp	ax, di			;is ptr already past (>) bufsize?
	jnc	SomeSpaceBFrSpPtr
	mov	ax, 0			;don't XOR - preserve CY
	jmp	EndBFrSpPtr
SomeSpaceBFrSpPtr:
	sub	ax, di			;ax = remaining space
	cmp	ax, cx			;(re)sets carry as necessary
EndBFrSpPtr:
	pop	bx
	ret
BadGetFreeSpPtr endp


;============================================================================
;			    BadSetCBData
;============================================================================
;
; Purpose	Bad App SetClipboardData (ax = 1703H)
;
; Paramters	dx = WinOldAp-supported clipboard format
;		es:bx = pointer to data
;		si:cx = size of data
;		ds = WinOldAp data segment
;		ss = application stack segment
;
; Return Values ax = 0: error occured
;		   <> 0: ok
;		es = BufSeg
;		ds = app destination seg
;
;============================================================================
BadSetCBData	proc	near
	mov	al, cs:bCBOpen		;duplicate open requirement
	or	al, al
	jnz	OpenBSet
	jmp	EndBSet
OpenBSet:
	push	ds
	push	cx
	mov	ds, [BufSeg]
	call	BadMatchItem
	pop	cx
	pop	ds
	jc	NoMatBSet
	xor	ax, ax
	jmp	EndBset
NoMatBSet:
	push	es
	mov	es, [BufSeg]
	mov	si, bx

	call	BadGetFreeSpPtr 	;es:di -> free space

	pop	ds			;ds:si -> app data to set
	jnc	TryTextBSet		;data can be accomodated
	xor	ax, ax			;data too big
	jmp	EndBSet

TryTextBSet:
	cmp	dx, CF_TEXT
	jne	TryOEMTextBSet
	call	TextA2G
	jmp	EndBSet
TryOEMTextBSet:
	cmp	dx, CF_OEMTEXT
	jne	TryDspTextBSet
	call	TextA2G
	jmp	EndBSet
TryDspTextBSet:
	cmp	dx, CF_DSPTEXT
	jne	TryBitmapBSet
	call	TextA2G
	jmp	EndBSet
TryBitmapBSet:
	cmp	dx, CF_BITMAP
	jne	TryDspBitmapBSet
	call	BitA2G
	jmp	EndBSet
TryDspBitmapBSet:
	cmp	dx, CF_DSPBITMAP
	jne	TryMetaBset
	call	BitA2G
	jmp	EndBSet
TryMetaBSet:
	cmp	dx, CF_METAFILEPICT
	jne	TryDspMetaBSet
	call	MetaA2G
	jmp	EndBSet
TryDspMetaBSet:
	cmp	dx, CF_DSPMETAFILEPICT
	jne	TryNoMoreBSet
	call	MetaA2G
	jmp	EndBSet
TryNoMoreBSet:
	xor	ax, ax
	jmp	End2BSet
EndBSet:
;	call	BadUpdateItemOne
End2BSet:
	ret
BadSetCBData	endp


;============================================================================
;			       BadMatchItem
;============================================================================
;
; Purpose	Determines if data of a given format in the clipboard.
;
; Parameters	dx = WinOldAp-supported clipboard format
;		ds = BufSeg
;
; Return Values carry set: data not in grabbuf
;		ds:si -> data in grabbuf, past grab header
;		ax = size
;		cx = WOACF
;
;============================================================================
BadMatchItem	proc	near
	push	bx
	push	dx

	call	CF2WOACF		;convert to format ids in grabbuf

	xor	ch, ch
	mov	cl, cs:[CBItemCount]
	jcxz	NoMatBMat
	mov	bx, CodeOFFSET CBItemTab
LoopBMat:
	mov	si, cs:[bx]
	lodsw				;ax = WOACF of item
	and	ah, 01111111B
	cmp	ax, dx			;does it match?
	je	MatchBMat		;we did in fact
	add	bx, 2			;no match, try next entry
	loop	LoopBMat		;if there is one
NoMatBMat:
	stc				;no match
	jmp	EndBMat
MatchBMat:
	mov	cx, ax			;format
	lodsw				;ax = size
	clc
EndBMat:
	pop	dx
	pop	bx
	ret
BadMatchItem	endp


;============================================================================
;			   BadGetCBDataSize
;============================================================================
;
; Purpose	Bad App GetClipboardDataSize (ax = 1704H)
;
; Parameters	dx = WinOldAp-supported clipboard format
;		ds = WinOldAp data segment
;		ss = application stack segment
;
; Return Values ax = 0: error occured
;		   <> 0: ok
;		dx:ax = size of data
;
;============================================================================
BadGetCBDataSize	proc	near
	push	ds
	mov	al, cs:bCBOpen		;is clipboard open?
	or	al, al
	jnz	TryMatBSiz
	jmp	EndBSiz

TryMatBSiz:
	mov	ds, [BufSeg]
	call	BadMatchItem		;is specified format in clipboard?
	jc	NoMatBSiz
TryBitmapBSiz:
	cmp	dx, CF_BITMAP
	jne	TryDspBitmapBSiz
	jmp	BitmapBSiz
TryDspBitmapBSiz:
	cmp	dx, CF_DSPBITMAP
	jne	EndBSiz
BitmapBSiz:
	add	ax, 2		;WidthBytes, Bits
	jmp	EndBSiz
NoMatBSiz:
	xor	ax, ax

EndBSiz:
	xor	dx, dx		;can't be bigger than 64K
	pop	ds
	ret
BadGetCBDataSize	endp

;============================================================================
;			      TextG2A
;============================================================================
;
; Purpose	Copies text from grabbuf to app.
;
; Parameters	ds:si -> grabbuf text structure
;		es:di -> app buffer
;
;============================================================================
TextG2A proc	near
	rep	movsb
	mov	ax, 1
	ret
TextG2A endp


;============================================================================
;				BitG2A
;============================================================================
;
; Purpose	Copies bitmap from grabbuf to app.
;
; Parameters	ds:si -> grabbuf bitmap structure
;		es:di -> app buffer
;		ax = WOACF
;		cx = grabbuf size
;
;============================================================================
BitG2A	proc	near
	push	dx
	push	cx
	sub	cx, 12
	push	cx
	xor	dx, dx

	xor	ax, ax		;bmType = 0
	stosw
	lodsw			;width
	stosw
	lodsw			;height
	stosw
	xchg	cx, ax		;ax = size, cx = height
	div	cx		;widthbytes = height / size

	stosw
	lodsw			;planes
	stosb
	lodsw			;bitspixel
	stosb

	mov	ax, di		;offset of bits
	add	ax, 12
	stosw
	mov	ax, es		;segment of bits
	stosw
	xor	ax, ax
	stosw
	stosw

	lodsw			;width2
	stosw
	lodsw			;heigh2
	stosw
	pop	cx
	rep	movsb
	mov	ax, 1
	pop	cx
	pop	dx
	ret
BitG2A	endp

;============================================================================
;				MetaG2A
;============================================================================
;
; Purpose	Copies metafile from grabbuf to app.
;
; Parameters	ds:si -> grabbuf text structure
;		es:di -> app buffer
;
;============================================================================
MetaG2A proc	near
	rep	movsb
	mov	ax, 1
	ret
MetaG2A endp


;============================================================================
;			       BadGetCBData
;============================================================================
;
; Purpose	Bad App GetClipboardData (ax = 1705H)
;
; Parameters	dx = WinOldAp-supported clipboard format
;		es:bx = pointer to location to place data
;		ds = WinOldAp data segment
;		ss = application stack segment
;
; Return Values ax = 0: error occured
;		   <> 0: ok
;
; Notes
;
;============================================================================
BadGetCBData	proc	near
	push	ds
	mov	al, cs:bCBOpen
	or	al, al
	jnz	TryMatBGet
	jmp	EndBGet

TryMatBGet:
	mov	ds, [BufSeg]
	call	BadMatchItem
	jc	TryNoMoreBGet

	xchg	cx, ax
	mov	di, bx			;es:di -> dest

	cmp	dx, CF_TEXT
	jne	TryOEMTextBGet
	call	TextG2A
	jmp	EndBGet
TryOEMTextBGet:
	cmp	dx, CF_OEMTEXT
	jne	TryDspTextBGet
	call	TextG2A
	jmp	EndBGet
TryDspTextBGet:
	cmp	dx, CF_DSPTEXT
	jne	TryBitmapBGet
	call	TextG2A
	jmp	EndBGet
TryBitmapBGet:
	cmp	dx, CF_BITMAP
	jne	TryDspBitmapBGet
	call	BitG2A
	jmp	EndBGet
TryDspBitmapBGet:
	cmp	dx, CF_DSPBITMAP
	jne	TryMetaBGet
	call	BitG2A
	jmp	EndBGet
TryMetaBGet:
	cmp	dx, CF_METAFILEPICT
	jne	TryDspMetaBGet
	call	MetaG2A
	jmp	EndBGet
TryDspMetaBGet:
	cmp	dx, CF_DSPMETAFILEPICT
	jne	TryNoMoreBGet
	call	MetaG2A
	jmp	EndBGet
TryNoMoreBGet:
	xor	ax, ax

EndBGet:
	pop	ds
	ret
BadGetCBData	endp



;============================================================================
;			 BadRegisterCBFormat
;============================================================================
;
; Purpose	Bad App RegisterClipboardFormat (ax = 1706H)
;
; Notes 	Not Implemented.
;
;============================================================================
BadRegisterCBFormat	proc	near
	xor	ax, ax
	ret
BadRegisterCBFormat	endp


;============================================================================
;			    BadEnumCBFormats
;============================================================================
;
; Purpose	Bad App EnumClipboardFormats (ax = 1707H)
;
; Notes 	Not Implemented.
;
;============================================================================
BadEnumCBFormats	proc	near
	xor	ax, ax
	ret
BadEnumCBFormats	endp


;============================================================================
;				 BadCloseCB
;============================================================================
;
; Purpose	Bad App CloseClipboard (ax = 1708H)
;
; Parameters	ds = WinOldAp data segment
;		ss = application stack segment
;
; Return Values ax = 0: error occured
;		   <> 0: ok
;
;============================================================================
BadCloseCB	proc	near
	mov	cs:bCBOpen, 0
	call	EnableGrabber
	mov	ax, 1
	ret
BadCloseCB	endp

;============================================================================
;			       BadCBCompact
;============================================================================
;
; Purpose	Bad App ClipboardCompact (ax = 1709H)
;
; Parameters	ds = WinOldAp data segment
;		ss = application stack segment
;
; Return Values dx:ax = number of bytes available
;
; Notes 	Uses es,
;
;============================================================================
BadCBCompact	proc	near
	xor	dx, dx		;>64K not supported
	mov	es, BufSeg
	call	BadGetFreeSpPtr
	ret
BadCBCompact	endp


sEnd	code
	end

