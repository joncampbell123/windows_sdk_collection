	TITLE AVWINA.ASM
	page 60,132

;-----------------------------------------------------------------------;
;	AVWINA.ASM							;
;									;
;	Based on FLAT.ASM from the Microsoft Video for Windows capture	;
;	driver VBLASTER.DRV.  Used with permission from Microsoft.	;
;									;
;	Version 0.4	Updated for VxP500				;
;-----------------------------------------------------------------------;
;******************************************************************************
;  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;  PURPOSE.
;
;  Copyright (c) 1994 - 1995 Microsoft Corporation.	All Rights Reserved.
;******************************************************************************


?PLM=1		; PASCAL Calling convention is DEFAULT
?WIN=0		; Windows calling convention

.xlist
include cmacros.inc
.list


LONG	struc
lo	dw	?
hi	dw	?
LONG	ends

 	externFP	AllocSelector		; in KERNEL
 	externFP	SetSelectorBase		; in KERNEL
 	externFP	SetSelectorLimit	; in KERNEL

	externW		_AVIndexPort		; in AVWIN.C


;---------------------------------------;
;	Segment Declarations		;
;---------------------------------------;

ifndef	SEGNAME
	SEGNAME equ <_TEXT>
endif

createSeg %SEGNAME, CodeSeg, word, public, CODE

.386

sBegin	CodeSeg
	assumes cs,CodeSeg
	assumes ds,nothing
	assumes es,nothing


;----------------------------------------------------------------
;	CreatePhysicalSelector(base, limit)			;
;								;
;	returns DX:AX = selector:offset to video buffer		;
;----------------------------------------------------------------
	assumes ds,Data
	assumes es,nothing

cProc CreatePhysicalSelector,<FAR,PASCAL,PUBLIC>,<si,di>
	ParmD   base
	ParmD   limit
cBegin
	mov	cx, base.lo	; BX:CX = physical base of memory.
	mov	bx, base.hi

	mov	di, limit.lo	; SI:DI = extent of memory.
	mov	si, limit.hi

	mov	ax, 0800h	; Call DPMI.
	int	31h
	jnc	short dpmi_no_error

dpmi_error:
	xor	si, si
	jmp	short exit

dpmi_no_error:			; BX:CX contains linear base.
	mov	di, cx		; Save it in SI:DI.
	mov	si, bx

;	Now create a selector that points to the memory

	cCall   AllocSelector, <ds>
	xchg	si, ax		; SI = selector, AX = base.hi.

	cCall   SetSelectorBase,<si, ax, di>
	cCall   SetSelectorLimit,<si, limit>
exit:
	mov	dx, si		; DX:AX points to memory.
	xor	ax, ax
cEnd


;----------------------------------------------------------------
;	CreateRealSelector(para)				;
;								;
;	returns AX = selector to video buffer			;
;----------------------------------------------------------------
	assumes ds,Data
	assumes es,nothing

cProc CreateRealSelector,<FAR,PASCAL,PUBLIC>,<si,di>
	ParmW   para
cBegin
	mov	ax, 0002h	; Call DPMI.
	mov	bx, para	; BX = paragraph number of real memory.
	int	31h
	jnc	short CRS_Okay

CRS_Error:
	xor	dx, dx		; If error, return DXAX = 0;
	jmp	short CRS_Exit

CRS_Okay:
	mov	dx, ax		; If okay, return far pointer in DXAX.

CRS_Exit:
	xor	ax, ax		; Offset is always zero.
cEnd


;----------------------------------------------------------------
;	AV_ReadVideoMemory(MemSelector, MemOffset)		;
;								;
;	Returns a word of video memory in AX.			;
;----------------------------------------------------------------
cProc AV_ReadVideoMemory,<FAR,PASCAL,PUBLIC>,<si,di,ds,es>
        ParmW   MemSelector
        ParmD   MemOffset
cBegin
	mov	ax, DGROUP
	mov	ds, ax
        mov     es, MemSelector

        mov     ebx, MemOffset		; Set BX = Offset in 16K window.
	and	bx, 7FFFh

	mov	eax, MemOffset		; Set AL = 16K Page.
	shr	eax, 15
	shl	al, 1
	mov	ah, al
	mov	al, 16h			; Write page to register R16.
	mov	dx, _AVIndexPort
	out	dx, ax

        mov     al, es:[bx]		; Byte access for VxP500.
        mov     al, es:[bx]		; Read twice to workaround problem?
        mov     ah, es:[bx+1]
        mov     ah, es:[bx+1]
cEnd


;----------------------------------------------------------------
;	AV_WriteVideoMemory(MemSelector, MemOffset)		;
;								;
;	Writes a word of video memory from AX.			;
;----------------------------------------------------------------
cProc AV_WriteVideoMemory,<FAR,PASCAL,PUBLIC>,<si,di,ds>
        ParmW   MemSelector
        ParmD   MemOffset
        ParmW   MemValue
cBegin
	mov	ax, DGROUP
	mov	ds, ax
        mov     es,MemSelector

        mov     ebx, MemOffset		; Set BX = Offset in 16K window.
	and	bx, 7FFFh

	mov	eax, MemOffset		; Set AL = 16K Page.
	shr	eax, 15
	shl	al, 1
	mov	ah, al
	mov	al, 16h			; Write page to register R16.
	mov	dx, _AVIndexPort
	out	dx, ax

        mov     ax,MemValue
        mov     es:[bx], al		; Byte access for VxP500.
        mov     es:[bx+1], ah
cEnd


;----------------------------------------------------------------
;	GetHSyncPolarity()					;
;								;
;	Returns 0 if active low, 1 if active high.		;
;----------------------------------------------------------------
cProc GetHSyncPolarity,<FAR,PASCAL,PUBLIC>,<si,di,ds,es>
	localD	HSync0
	localD	HSync1
cBegin
	mov	ax, DGROUP
	mov	ds, ax

	mov	dx, _AVIndexPort	; DX = VxP-500 index port.
	mov	al, 0D3h		; Select VGA Interrupt Status reg.
	out	dx, al
	inc	dx			; DX = VxP-500 data port.

	xor	eax, eax		; Initialize local counters.
	mov	HSync0, eax
	mov	HSync1, eax

;	Wait for HSync bit to go high, then low.
	mov	cx, 0FFFFh
@@:
	in	al, dx
	test	al, 1
	loopz	@b
	mov	cx, 0FFFFh
@@:
	in	al, dx
	test	al, 1
	loopnz	@b

;	Count how long it stays low.
	mov	cx, 0FFFFh
@@:
	inc	HSync0
	in	al, dx
	test	al, 1
	loopz	@b

;	Then count how long it stays high.
	mov	cx, 0FFFFh
@@:
	inc	HSync1
	in	al, dx
	test	al, 1
	loopnz	@b

;	See which of HSync0 and HSync1 is bigger to determine polarity.
	mov	eax, HSync1
	cmp	eax, HSync0
	mov	ax, 0
	adc	ax, 0
cEnd


;----------------------------------------------------------------
;	GetVSyncPolarity()					;
;								;
;	Returns 0 if active low, 1 if active high.		;
;----------------------------------------------------------------
cProc GetVSyncPolarity,<FAR,PASCAL,PUBLIC>,<si,di,ds,es>
	localD	VSync0
	localD	VSync1
cBegin
	mov	ax, DGROUP
	mov	ds, ax

	mov	dx, _AVIndexPort	; DX = VxP-500 index port.
	mov	al, 0D3h		; Select VGA Interrupt Status reg.
	out	dx, al
	inc	dx			; DX = VxP-500 data port.

	xor	eax, eax		; Initialize local counters.
	mov	VSync0, eax
	mov	VSync1, eax

;	Wait for VSync bit to go high, then low.
	mov	cx, 0FFFFh
@@:
	in	al, dx
	test	al, 2
	loopz	@b
	mov	cx, 0FFFFh
@@:
	in	al, dx
	test	al, 2
	loopnz	@b

;	Count how long it stays low.
	mov	cx, 0FFFFh
@@:
	inc	VSync0
	in	al, dx
	test	al, 2
	loopz	@b

;	Then count how long it stays high.
	mov	cx, 0FFFFh
@@:
	inc	VSync1
	in	al, dx
	test	al, 2
	loopnz	@b

;	See which of VSync0 and VSync1 is bigger to determine polarity.
	mov	eax, VSync1
	cmp	eax, VSync0
	mov	ax, 0
	adc	ax, 0
cEnd


sEnd	CodeSeg
end
