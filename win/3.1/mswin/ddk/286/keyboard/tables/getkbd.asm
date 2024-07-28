; ******* GetKbd.asm ******************************************
;
;	Copyright (C) 1989-1990 by Microsoft Corporation.
;
;	Contains GetKbdTable() function to initialize keyboard
;	tables.
;
;	The scancode-to-virtual-keycode table keyTrTab[] in
;	the driver is patched (including the 'X1-X2' swap for
;	the AT keyboard) according to tables in this DLL.
;
;	The tables in the data segment of this module are updated
;	according to the keyboard type, and then the pointers and
;	counts in the driver are updated accordingly.
;
; *************************************************************
;	History
;
;	08 jan 89	peterbe		ifdef's for Ericsson/Nokia range
;					checking (only types 1..4 if no
;					Ericsson!)
;
;	17 aug 89	peterbe		externB X1,X2 in ifdef!
;
;	03 aug 89	peterbe		Add WEP() function.
;
;	18 jul 89	peterbe		Use __ROMBIOS selector when testing for
;					OLIVETTI copyright, in X1X2 code.
;
;	06 apr 89	peterbe		Corrected documentation of parameters.
;
;	13 feb 89	peterbe		Removed commented-out text.
;
;	09 feb 89	peterbe		Added DATE.INC to code segment.
;					Rewrote X1X2 code so we don't write
;					into CS!
;
;	26 jan 89	peterbe		Removed code for updating XLAT tables.
;					(Oem/Ansi).
;
;	20 sep 88	peterbe		Removed some unused data structures.
;
;	19 sep 88	peterbe		Changing patching so it doesn't write
;					in DLL's CODE segment.  Format of
;					'header' changed slightly to facilitate
;					this.  Overlay type, sizes in header
;					AFTER copying, with data pointed to
;					by BegPatches[TableType-1].
;	08 sep 88	peterbe		Finished CODE seg aliasing.  Need to
;					add code to release alias when done!
;	07 sep 88	peterbe		Beginning to add code for aliasing
;					CODE segment, which is written to!
;	25 aug 88	peterbe		Removed debug string.
;	11 aug 88	peterbe		Added code for patching szSGCaps,
;					fKeyType, TableType.
;	08 aug 88	peterbe		Added X1/X2 swap code.  This requires
;					conditional assembly flag X1X2 set
;					for ALL BUT USA TABLES!
;	05 aug 88	peterbe		Adding more code/data for patching.
;	04 aug 88	peterbe		Adding code/data for patching.
;	29 jul 88	peterbe		Making single-keyboard version work
;	28 jul 88	peterbe		Created
;	
; *************************************************************

include cmacros.inc

extrn	__ROMBIOS:abs		; selector for real segment 0F000H

sBegin DATA

; *************************************************************
;
;	Things to patch in tables in DATA segment.
; 
; *************************************************************

	externB	PatchATran4
	externB	VarAscCtrlVK
	externB	VarAscCtrl
    	externB AscCtlAltVK
    	externB AscCtlAlt
	externB AscShCtlAltVK
	externB AscShCtlAlt
	externB Morto
	externB MortoCode
	externB	CapitalTable

sEnd DATA


; *************************************************************
;
sBegin	CODE        ; Beginning of code segment
assumes	CS,CODE
assumes	DS,DATA
	
; *************************************************************
;
; 	Externals in CODE segment
; 
; *************************************************************

; Externals for offsets of patch tables in kbdXX.asm

	externW szPatchATran
	externW PatchATran

	externW szPatchACtrl
	externW PatchACtrlVK
	externW PatchACtrl

	externW szPatchACtlAlt
	externW PatchACtlAltVK
	externW PatchACtlAlt

	externW szPatchAShCtlAlt
	externW PatchAShCtlAltVK
	externW PatchAShCtlAlt

	externW szPatchMortoVK
	externW PatchMortoVK

	externW szPatchMortoCode
	externW PatchMortoCode

	externW szPatchCapital
	externW PatchCapital

; Externals for patching keyTrTab.

	externW	<nIndices>
	externB	<PatchIndices>
	externW	<TransPatches>

ifdef X1X2
	externB	<X1, X2>		; used for non-US AT keyboards.
endif

	externB Patch102		; entry in PatchIndices[] for
					; key 102 .. must change for Nokia 6.

; Externals for copying the header

	externW <szHeader>
	externB <headerBeg>

; Externals for patching the header.

	externB	HeaderBeg		; base of table

	externW szAscTran		; doesn't change

    ; The following values are changed in the header in the driver, after
    ; the table is copied.

	externW szAscControl		; does change
	externW szAscCtlAlt		; does change
	externW szAscShCtlAlt		; does change

	externW szMorto		; does change
	externW szDeadKey		; does change
	externW szSGCaps		; does change
	externW szCapital		; does change

	externB	fKeyType		; flags mainly for Enhanced kbd.
	externB	TableType		; == KeyType

	externW	BegPatches		; offsets of szN tables for patching
					; header.

; *************************************************************
;
;	Patch table for main patch loop.
;
;	This consists of a series of 4-word entries, terminated
;	by a 0 word.  These look like
;
;	dw	CODEoffset <6-word table of patch sizes>
;	dw	CODEoffset <6-word table of patch source offsets>
;	dw	DATAoffset <destination in table>
;	dw	CODEoffset <(destination) size field in header>
;
;	In certain cases, the size field entry is a dummy value
;	because it isn't needed, or a temporary value, since the
;	value needs to be adjusted.
;
; *************************************************************


MainPatchTable label word

	; main shifted/unshifted translation table
	dw	CODEoffset szPatchATran
	dw	CODEoffset PatchATran
	dw	DATAoffset PatchATran4

	; Control key VK table
	dw	CODEoffset szPatchACtrl
	dw	CODEoffset PatchACtrlVK
	dw	DATAoffset VarAscCtrlVK


	; control key translated table
	dw	CODEoffset szPatchACtrl
	dw	CODEoffset PatchACtrl
	dw	DATAoffset VarAscCtrl

	; control-alt VK table
	dw	CODEoffset szPatchACtlAlt
	dw	CODEoffset PatchACtlAltVK
	dw	DATAoffset AscCtlAltVK


	; control-alt translated table
	dw	CODEoffset szPatchACtlAlt
	dw	CODEoffset PatchACtlAlt
	dw	DATAoffset AscCtlAlt

	; 
	; shift-control-alt VK table
	dw	CODEoffset szPatchAShCtlAlt
	dw	CODEoffset PatchAShCtlAltVK
	dw	DATAoffset AscShCtlAltVK


	; shift-control-alt translated table
	dw	CODEoffset szPatchAShCtlAlt
	dw	CODEoffset PatchAShCtlAlt
	dw	DATAoffset AscShCtlAlt

	; Dead-key VK+shift table patches
	dw	CODEoffset szPatchMortoVK
	dw	CODEoffset PatchMortoVK
	dw	DATAoffset Morto


	; Dead-key translated table patches
	dw	CODEoffset szPatchMortoCode
	dw	CODEoffset PatchMortoCode
	dw	DATAoffset MortoCode

	; Capital table patches.
	dw	CODEoffset szPatchCapital
	dw	CODEoffset PatchCapital
	dw	DATAoffset CapitalTable

	dw	0	; terminator.

; Date, so we can verify that KEYBOARD.DRV and KBDDLL.MOD come from same
; build! ..
include date.inc

; *************************************************************
;
; GetKbdTable()
;
;	Is called with
;
;	parmW:	Keyboard type	1: XT, M24 83-key
;				2: Olivetti M24 102-key 'ICO'
;				3: AT 84- or 86-key
;				4: RT Enhanced 101- or 102-key
;				Old Ericsson keyboards:
;				5: Nokia (aka Ericsson) 1050
;				6: Nokia
;
;	parmD:	far Pointer to keyTrTab[] in driver
;
;	parmD:	far Pointer to the header for the translation tables
;
;	The tables are patched, and various data are copied to
;	the driver, as described below.
;
;	This returns a FAR pointer to this DLL's DATA segment in DX:AX
;	(AX is 0).
;
;	This routine is in a LOADONCALL DISPOSABLE segment, so
;	its memory may be reclaimed.  The DATA segment is fixed.
;
; *************************************************************

cProc GetKbdTable,<PUBLIC,FAR>,<si,di>

    ParmW nKeyType		; keyboard type 1..6
    ParmD lpKeyTrans		; pointer to keyTrTab[] in driver.
    ParmD lpHeader		; pointer to set of pointers/sizes in
				; TABS.ASM

    localD pProc		; pointer to 
    localW hKernel		; handle for kernel.exe

    localW bxIndex		; index calculated from nKeyType


cBegin GetKbdTable

    ; all string operations go UP!

	cld

    ; Make patches to tables in data segment, depending on keyboard
    ; type.
    ; Begin by setting up segment registers and checking the keyboard
    ; type.

	push	ds				; save DS
	push	ds
	pop	es				; ES -> DATA
	push	cs
	pop	ds				; DS -> CODE (or alias)

assumes	CS,CODE
assumes	DS,CODE					; or alias for CODE!

	mov	bx,nKeyType			; get keyboard type

	dec	bx				; make BX a WORD index:
	shl	bx,1				; [bx] = (nKeyType-1) * 2
	mov	bxIndex,bx			; save for later

ifdef NOKIA					; support old Ericsson kbd's?
	cmp	bx,6				; don't patch type 4
	je	PatchDone
	cmp	bx,10				; .. or > 6
	ja	PatchDone
else						; IBM, Olivetti-compatible only:
	cmp	bx,6				; don't patch type 4
	jae	PatchDone			; or greater!
endif

	mov	di,CODEoffset MainPatchTable

    ; in the most of this loop, DI points to the current block
    ; in MainPatchTable, and BX is an index to the size and
    ; source-address tables.

PatchLoop:
	mov	si,word ptr[di]			; base of size array
	or	si,si				; 0 value terminates
	jz	PatchEnd

	mov	cx,word ptr[si+bx]		; get count from table
	cmp	cx,0
	jle	PatchNext			; do nothing if [CX] negative

						; [CX] > 0, so copy an array:
	mov	si,word ptr[di+2]		; get base of source array
	mov	si,word ptr[si+bx]		; get source address
	push	di				; save old DI
	mov	di,word ptr[di+4]		; get destination offset
	rep movsb				; copy the data!
	pop	di				; restore old DI

PatchNext:
	add	di,6				; move DI to next parameter
	jmp	PatchLoop			; block in MainPatchTable

PatchEnd:
PatchDone:
	pop	ds

assumes	CS,CODE
assumes	DS,DATA

    ; Patch the keyboard driver's keyTrTab scancode-to-VK-code translation
    ; table.  Again, this depends on the keyboard type.
    ; Translations for scan codes 29h and 2bh are swapped on AT-type
    ; keyboards for non-Olivetti systems.

;DoVKPatch:

	push	ds				; save DS,
	push	cs				; set DS=CS
	pop	ds
	assumes DS,CODE

	mov	cx,nIndices			; get count

    ; now we set up to patch keyTrTab[]

ifdef NOKIA
	cmp	bxIndex,10			; keyboard type in range 1..5?
	jb	KeyTypeOk			; 1..4
	je	KeyTypeNok6
else
	cmp	bxIndex,6			; kbd type in range 1..4?
	jbe	KeyTypeOk
endif

ZerKeyType:
	xor	bx,bx				; too big, clear index
	mov	bxIndex,bx			; and save it
ifdef NOKIA
	jmp	short KeyTypeOk

KeyTypeNok6:					; for Nokia 6, change key
	mov	Patch102,98			; 98 instead of 86
endif

KeyTypeOk:
	mov	si,[TransPatches+bx]		; set source for this key. type
	les	di,lpKeyTrans			; set dest. to keyTrTab[0]
	xor	bx,bx				; clear index to PatchIndices

    ; Now we patch keyTrTab[].
    ; this loop uses AL, BX (2 ways!), CX, SI, DI, DS, ES !!
    ; Index into keyTrTab[] is obtained from PatchIndices[]
VKloop:
	push	bx				; save index to PatchIndices[].
	mov	bl,[PatchIndices+bx]		; BH remains 0.
	lodsb					; move * (DS:SI++) to
	mov	es:[di+bx],al			; * (DS:(DI+PatchIndices[n++]))
	pop	bx				; BX is index to PatchIndices,
	inc	bl				; increment index.
	loop	VKloop

    ; Handle the famous X1/X2 scancode swap for the AT-type keyboard
    ; (for non-US keyboards only -- this is controled by 'X1X2'
    ; conditional assembly flag, which should be set for all but
    ; US keyboard).  We don't do this for Olivetti AT (M28) keyboards.

ifdef X1X2
	mov	ax,__ROMBIOS			; Is this an Olivetti system?
	mov	es,ax				; get ROM copyright message --
	mov	ax,es:[0c050h]			; 'LO' in AX if it's Olivetti 
	cmp	bxIndex,4			; AT-type (3) keyboard?
	jne	NoX1X2				; if not, no swap.
	cmp	ax,'LO'				; Olivetti ROM?
	je	NoX1X2				; if so, no swap.

SwapX1X2:
						; exchange X1, X2 entries in
	xor	ah,ah				; keyTrTab[] in driver.
	mov	al,X1				; get X1's index
	mov	si,ax				;  -> SI
	mov	al,X2				; get X2's index
	mov	di,ax				;  -> DI
	les	bx,lpKeyTrans			; ES:BX -> keyTrTab[0]
	mov	al,es:[BX+SI]			; swap keyTrTab[X1]
	xchg	al,es:[BX+DI]			;   and
	mov	es:[BX+SI],al			;      keyTrTab[X2].
NoX1X2:

endif ; X1X2

    ; Copy the pointers and sizes in the header to the driver

	mov	cx,szHeader			; get byte count
	mov	si,CODEoffset headerBeg		; set source
	les	di,lpHeader			; set destination
	rep movsb				; copy the header.

    ; If the table type is not 4, overlay the first part of the
    ; header with values in szN (see BegPatches[] in a keyboard table
    ; file)

	mov	bx,bxIndex			; (nKeyType - 1) * 2
	cmp	bx,6				; enhanced keyboard?
	je	NoPatchHeaderSizes		; skip if type 4
	mov	si,[BegPatches+bx]		; get offset of patches
	mov	cx,16				; size of szN[]
	les	di,lpHeader			; set destination
	rep movsb				; copy the first part
NoPatchHeaderSizes:				; of the header.

    ; and we're done.  Return a FAR pointer to this DLL's DATA segment.

GKTexit:
	pop	ds
	assumes	DS,DATA

	xor	ax,ax				; AX = 0
	push	ds
	pop	dx				; DX = DATA

GKTend:

cEnd GetKbdTable

; WEP():  This function is called when the DLL is loaded or unloaded.
;
; in our case, we do nothing but return.

cProc	WEP,<PUBLIC,FAR>

;; wParam foo				; for reference.. this has 1 param.

cBegin	nogen

	ret	2			; flush the parameter on exit

cEnd	nogen

sEnd CODE

end
