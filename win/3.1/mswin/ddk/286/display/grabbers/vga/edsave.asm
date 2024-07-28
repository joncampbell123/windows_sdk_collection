
		page	,132
		%out	Enable/DisableSave
		name	EDSAVE
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


;
; NAME
;	EDSAVE.ASM
;
; DESCRIPTION
;	This file contains the extended subfunction entrypoints EnableSave
;	and DisableSave.
;
; AUTHOR
;	Jim Christy
;
; HISTORY
;	1.00	060187	jhc	Epoch
;
;	1.10	071588	vvr	Added code to intercept Int10 and get RAM
;				loadable font info. Int 10, Subfunction 11h
;				Also intercepts SETMODE and in graphics modes,
;				makes the font info invalid				
;

_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		.xlist
		include abs0.inc
		include dc.inc

		.list

		extrn	DC:byte
		extrn   InternalSetModeCall:byte
		extrn	saveFlags:byte
		extrn	BrstDet:near

		public	EnableSave
		public	DisableSave

		public	GrbInt10Rout
	
lpOldSavePtr	dd	?			;old EGA BIOS Save Area pointer

Int10Vector	dd	?			;original Int10 address


		subttl	EnableSave

GrbInt10Rout	proc	far

		assume ds:nothing

;		sti
		push	bx
		push	ax

; if the call is from within the grabbers, pass it one .

		cmp	byte ptr cs:[InternalSetModeCall],0ffh
		jz	NotOurs

		or	ah,ah			;is it set mode?
		jnz	grSetFont		;check sub function 11h

; reset the font banks.

		mov	word ptr cs:[DC.dcFontBank][0],0FFFFh
		mov	word ptr cs:[DC.dcFontBank][2],0FFFFh
		mov	word ptr cs:[DC.dcFontBank][4],0FFFFh
		mov	word ptr cs:[DC.dcFontBank][6],0FFFFh
@@:
;we save the flag bytes at this point because the effect of setting
;this byte before and after is different.

		push	es
		xor	bx,bx
		mov	es,bx
		mov	bl,byte ptr es:[489h]
		mov	cs:[saveFlags],bl
		pop	es
		mov	bx,0FF04h		;assume 8x16 font
		cmp	al,07			;is it mono mode?
		jz	grsmDoFonts		; set font info properly
		mov	bl,0FFh			;assume graphics mode
		cmp	al,03			;is it text mode?
		ja	grsmDoFonts		; no it is not
		jmp	NotOurs			;text modes;leave it alone
comment ^
		cmp	word ptr cs:[DC.dcFontBank][0],0
		mov	bl,004			;assume 400 line mode
		call	BrstDet			; see 200 or 350 line mode
		jc	grsmDoFonts		;if 350 do it
		dec	bl			;else show 8x8 font
		dec	bl			;
comment ^
grsmDoFonts:
		mov	word ptr cs:[DC.dcFontBank][0],bx		;save it
		jmp	NotOurs			;get out now
	
grSetFont:	
		cmp	ah,11h
		jne	NotOurs
		cmp	al,20h
		jae	NotOurs
		and	al,0Fh		;get last four bits
		cmp	al,4
		ja	NotOurs		
		cmp	al,3
		je	NotOurs

grDoFonts:
		and	bx,0007		;font bank
		mov	byte ptr cs:[DC.dcFontBank].[bx],al		;save it
		mov	word ptr cs:[DC.dcFontBank][2],0FFFFh
		mov	word ptr cs:[DC.dcFontBank][4],0FFFFh
		mov	word ptr cs:[DC.dcFontBank][6],0FFFFh
		
NotOurs:

		pop	ax
		pop 	bx
;		cli
		jmp	cs:[Int10Vector]

		ret
GrbInt10Rout	endp

;
; EnableSave - enable video context switching
;
;	Analogous to EnableGrab, this entrypoint gives the grabber the chance
;	to install any hooks needed for context switching.
;
;	For the EGA, we must save the current EGA SavePtr, since the oldap may
;	change it.  If this was not done and the oldap had indeed changed it,
;	after Winoldap removed the oldap from memory, the modified SavePtr
;	would be a dangling pointer and would crash the EGA BIOS during the
;	next int 010h call.
;
; ENTRY
;	ds	=  cs
; EXIT
;	none
; USES
;	ax, ds, flags
;
EnableSave	proc	near
		xor	ax,ax
		mov	ds,ax
		assume	ds:ABS0
		lds	ax,[lpSavePtr]
		mov	word ptr cs:[lpOldSavePtr],ax
		mov	word ptr cs:[lpOldSavePtr][2],ds

		xor	ax,ax
		mov	ds,ax
		
;~vvr  Set Int10 vector to point to our trapping routine
;
		cli
		push	es
		les	bx,ds:[4*10h]
		mov	word ptr cs:[Int10Vector],bx
		mov	word ptr cs:[Int10Vector + 2], es
		mov	bx, offset GrbInt10Rout
		mov	ds:[4*10h],bx
		mov	ds:[4*10h + 2], cs			   
		pop	es
		sti
;~vvr
		ret
EnableSave	endp


		subttl	DisableSave


;
; DisableSave -  disable video context switching
;
;	Analogous to DisableGrab, this entrypoint gives the grabber the chance
;	to remove any hooks installed by EnableSave.
;
;	For the EGA, we restore the EGA SavePtr.
;
; ENTRY
;	ds	=  cs
; EXIT
;	none
; USES
;	ax, ds, flags
;
DisableSave	proc	near
		push	es
		xor	ax,ax
		mov	ds,ax
		assume	ds:ABS0
		les	ax,cs:[lpOldSavePtr]
		cli
		mov	word ptr [lpSavePtr],ax
		mov	word ptr [lpSavePtr][2],es
;~vvr Reset Int10 Vector to original values
		mov	ax,word ptr cs:[Int10Vector]
		mov	ds:[4*10h],ax
		mov	ax,word ptr cs:[Int10Vector + 2]
		mov	ds:[4*10h+2],ax
;~vvr
		sti
		pop	es
		ret
DisableSave	endp



_TEXT		ends
		end

