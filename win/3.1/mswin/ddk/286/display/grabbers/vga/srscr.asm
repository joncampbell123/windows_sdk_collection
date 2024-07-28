
		page	,132
		%out	Save/RestScreen
		name	SRSCR
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


;
; NAME
;	SRSCR.ASM
;
; DESCRIPTION
;	This file contains the code for the original grabber entrypoints
;	SaveScreen and RestoreScreen, which handle the bulk of the context
;	switching process for the display.
;
; AUTHOR
;	Jim Christy
;
; HISTORY
;	1.00	060187	jhc	Epoch
;
;	1.10	071588	vvr	modified for vga. Now reads and writes 
;				from vga registers. During savescreen,these
;				registers are saved to a local location and
;				the moved to the save area. Reverse is done
;				when restoring the registers.
;
;	1.11	062989	vvr	added code to save/restore Misc. output
;				and Feature control register.
;

_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


NO_STDGRABENTRY =	1

		.xlist
		include dc.inc
		include vgaic.inc
		include vga.inc
		include vgaabs0.inc
		include vgaoem.inc
		include grabber.inc
		.list

		extrn	MAX_TOTGRPH:abs
		extrn	MAX_CDSIZE:abs

		extrn	DC:byte
		extrn	IC:byte
		extrn	fVectra:byte
		extrn	SaveTextSize:word
		extrn	SaveGrphSize:word
		extrn	VgaContextSize:word
		extrn	TempSaveAreaSize:word
		extrn	TempSaveAreaSeg:word
		extrn	TempSaveAreaOff:word
		extrn	saveFlags:byte

		extrn	GetHpVideoSeg:near
		extrn	ScreenOn:near
		extrn	ScreenOff:near
		extrn	OemBeep:near
		extrn	GetMode:near
		extrn	SetMode:near
		extrn	SaveFontFile:near
		extrn	RestFontFile:near
		extrn	RestBiosFonts:near

		extrn	GetRegSetAttr:near
		extrn	SaveDACState:near
		extrn	RestoreDACState:near
		extrn	WaitVRetrace:near
		extrn	GetRegSet:near
		extrn	RestRegSet:near

		extrn	SaveScreenFile:near
		extrn	RestScreenFile:near
		extrn	Native8514Mode:near
		public	SaveScreen
		public	RestoreScreen


FALSE		=	0
TRUE		=	1

ssOldAttrRegs	db	NUM_ATTR_REGS 	dup 	(?)
ssOldCRTCRegs	db	NUM_CRTC_REGS 	dup 	(?)
ssOldSeqRegs	db	NUM_SEQ_REGS 	dup 	(?)
ssOldGrphRegs	db	NUM_GRAPH_REGS	dup	(?)
ssOldMiscReg	db	?
ssOldFeatReg	db	?

ssOldSeqIndReg	db	?
ssOldCrtIndReg	db	?
ssOldAttrIndReg	db	?
ssOldGrxIndReg	db	?
		subttl	SaveScreen


;
; SaveScreen - save the current display context
;
; ENTRY
;	ax	=  size in bytes of screen save area
;	ds	=  cs
;	es:di	-> screen save area
; EXIT
;	cf	=  0	(screen was successfully saved)
; ERROR EXIT
;	cf	=  1	(unable to save screen)
; USES
;	all except bp
; NOTES
;	Winoldap guarantees that the offset portion of the screen save area
;	will always be zero, so it is safe to omit di from references to this
;	area.
;

SaveScreen	proc	far
		assume	ds:_TEXT

;----------------------------------------------------------------------------;
; save the segment of the buffer resesrved by winoldap that can be used to   ;
; buffer i/o to/from video buffer. It is not advisable to read or write dire-;
; -ctly from the video buffer as on some machines the i/o could use DMA and  ;
; not all adaptor cards respond to DMA handskaking well (VGA card on PS/2    ;
; Model 50Z does not). The initial part of the buffer will actually hold     ;
; useful information like register save data etc and will not be touched. The;
; work area offset will start after this and the size of the area will appro-;
; -priately be adjusted. Values saved here will also be accessed by RestorSc-;
; -reen as they are expected to be same. This work area will only be used in ;
; hires graphics modes.							     ;
;----------------------------------------------------------------------------;
					   
; check to see if the adaptor is in a native 8514 mode, if it is fail the
; save screen call.

		push	ax
		push	dx

		mov	dx,SEQ_ADDR		;save sequencer address.
		in	al,dx
		mov	ssOldSeqIndReg,al
	
		mov	dx,ATTR_ADDR		;save attribute address
		in	al,dx
		mov	ssOldAttrIndReg,al

		mov	dx,CRTC_ADDR		;save crtc address
		in	al,dx
		mov	ssOldCrtIndReg,al

		mov	dx,GRAPH_ADDR	;save graphics controller address
		in	al,dx
		mov	ssOldGrxIndReg,al

		pop	dx
		pop	ax		

		call	Native8514Mode		;in 8514's native mode ?
		jnc	@f			;no
		jmp	ssErr			;fail the call
@@:
		mov	TempSaveAreaSeg,es	;save segment of save area
		push	ax			;save
		push	di			;save
		push	cx
		mov	cx,MAX_CDSIZE		;di = size of DC & IC structures
		add	cx,VgaContextSize	;di = di + VGA context size
		add	di,cx			;offset past these areas
		mov	TempSaveAreaOff,di	;save resuable area offset
		sub	ax,cx			;modify size
		mov	TempSaveAreaSize,ax	;save size
		pop	cx
		pop	di
		pop	ax

		call	GetMode 		;validate all data structures
		jnc	@F
		jmp	ssErr			;unknown mode.
@@:
		mov	[DC.dcfSwitchGmt],FALSE ;assume saving a text page
		cmp	ax,[SaveTextSize]	;if space for text page only,
		je	ssChkAlloc		;  assumption was right
		mov	[DC.dcfSwitchGmt],TRUE	;else show graphic/multiple text
ssChkAlloc:
		mov	dx,MAX_CDSIZE		;dx = size of DC & IC structures
		add	dx,VgaContextSize	;dx = dx + VGA context size

		ifdef	VGACOLOR
		test	[IC.icScrType],ST_LARGE ;if screen will go to disk,
		jnz	ssNoAdd 		;do not add in screen size
		endif

		add	dx,[IC.icScrLen]	;dx = dx + screen size
ssNoAdd:
		cmp	ax,dx			;if ax >= dx,
		jae	ssSaveFontFile		;  then we have enough space
		jmp	ssErr			;else complain to Winoldap
ssSaveFontFile:
;		call	ScreenOff
		call	SaveFontFile		;else to save font info
		jnc	ssSaveScreenFile
		jmp	ssErr			;if error, leave now
ssSaveScreenFile:
		ifdef	VGACOLOR
		test	[IC.icScrType],ST_LARGE ;if screen is not large
		jz	ssSaveDcIc		;  continue

		call	SaveScreenFile		;else save it to disk file
		jnc	ssSFok
		jmp	ssErr			;if error, leave now
ssSFok:
		endif

ssSaveDcIc:

		cld				;save DC & IC structures
		mov	si,offset DC
		mov	cx,(SIZE DeviceContext + SIZE InfoContext)/2
		if	(SIZE DeviceContext + SIZE InfoContext) AND 1
		movsb
		endif
		rep	movsw
ssSaveBiosData:
		xor	ax,ax
		mov	ds,ax			;ds = 0
		assume	ds:Abs0

		mov	si,offset CrtMode	;save std BIOS video data
		mov	cx,(SIZE VideoBiosData)/2
		if	(SIZE VideoBiosData) AND 1
		movsb
		endif
		rep	movsw

		mov	si,offset Rows		;save EGA BIOS video data
		mov	cx,(SIZE VgaBiosData)/2
		if	(SIZE VgaBiosData) AND 1
		movsb
		endif
		rep	movsw

		mov	si,offset lpSavePtr	;save EGA SavePtr
		movsw
		movsw

		cmp	cs:[fVectra],TRUE	;if not running on Vectra,
		jne	ssSaveScreen0		;  skip HP-specific stuff

		call	GetHpVideoSeg		;else get HP EX-BIOS video seg
		mov	ds,ax			;load it into ds
		assume	ds:nothing
		xor	si,si			;start at offset 0
		mov	cx,(SIZE HpBiosData)/2	;save HP EX-BIOS video data
		if	(SIZE HpBiosData) AND 1
		movsb
		endif
		rep	movsw
ssSaveScreen0:
		ifdef	VGACOLOR
		test	cs:[IC.icScrType],ST_LARGE ;if screen saved to disk
		jnz	ssSaveEgaRegs		   ;  don't save it to memory
		endif
		mov	cx,cs:[IC.icScrLen]	   ;cx = screen length
		lds	si,cs:[IC.iclpScr]	   ;ds:si -> screen
		cmp	cs:[DC.dcfSwitchGmt],TRUE  ;if text page only,
		jne	ssSaveScreen1		   ;  don't adjust si and cx

		xor	si,si			;else start at offset 0
		mov	cx,MAX_TOTGRPH		;and save maximum amount
ssSaveScreen1:
		shr	cx,1			;cx = cx/2 for word mov
		rep	movsw
		rcl	cx,1			;odd count will mov one more
		rep	movsb
ssSaveEgaRegs:

		push	di
		push	es

		mov	ax,cs
		mov	es,ax
		mov	ds,ax

		mov	di, offset ssOldCRTCRegs
		call	WaitVRetrace
		mov	cx,NUM_CRTC_REGS
		mov	dx,CRTC_ADDR AND 0FF0FH
		or	dl, CS:[DC.dcAddrPatch]
		call	GetRegSet

		mov 	bx, offset ssOldCRTCRegs
		call	ProgramERICrtc

		mov	di, offset ssOldAttrRegs
		call 	WaitVRetrace
		mov	cx,NUM_ATTR_REGS
		mov	dx,ATTR_ADDR
		cli
		call	GetRegSetAttr
		sti

		mov	bx, offset ssOldAttrRegs
		call	ProgramERIAttr

		mov	dx,MISC_READ_R		; miscellaneous reg 
		in	al,dx				
		mov	ssOldMiscReg,al

		call	ProgramERIMisc

		mov	dx,FEAT_CONTROL		; feature
		in	al,dx
		mov	ssOldFeatReg,al

		call	ProgramERIFeature
		
		mov	cx,NUM_GRAPH_REGS	; index 0 and count = 9
		mov	dx,GRAPH_ADDR
		mov	di,offset ssOldGrphRegs
		call	GetRegSet

		mov	bx, offset ssOldGrphRegs
		call	ProgramERIGrx

		mov	cx,NUM_SEQ_REGS		; index 0 and count = 5
		mov	dx,SEQ_ADDR
		mov	di,offset ssOldSeqRegs
		call	GetRegSet

		mov	bx, offset ssOldSeqRegs
		call	ProgramERISeq

		mov	si, offset ssOldAttrRegs
		mov	cx, NUM_ATTR_REGS + NUM_CRTC_REGS
		add	cx, NUM_SEQ_REGS  + NUM_GRAPH_REGS	
		inc	cx					; one for MISC
		inc	cx					; one for FEAT
		pop	es
		pop	di
		rep 	movsb			;save registers somewhere else
		call	SaveDACState
comment ^
;vvr
		call	ScreenOff
		mov	bx, di			; es:bx points to save address
		mov	ax, 1C01h		; save function of BIOS call
		mov	cx, 00000111b		; save only hardware areas
		int 	10h			;save it 
;vvr

comment ^
ssX:
		call	ScreenOn		;re-enable the display
		clc				;show success
		ret
ssErr:
		call	ScreenOn		;re-enable the display
		call	OemBeep 		;show error
		stc
		ret
SaveScreen	endp


		subttl	RestoreScreen
		page


;
; RestoreScreen - restore the previously saved display context
;
; ENTRY
;	ax	=  size in bytes of screen save area
;	ds	=  cs
;	es:di	-> screen save area
; EXIT
;	cf	=  0	(screen was successfully restored)
; ERROR EXIT
;	cf	=  1	(unable to restore screen)
; USES
;	all except bp
; NOTES
;	Winoldap guarantees that the offset portion of the screen save area
;	will always be zero, so it is safe to omit di from references to this
;	area.
;

RestoreScreen	proc	far
		assume	ds:nothing

		cld

		mov	ax,es			;xchg ds, es
		mov	bx,ds
		mov	es,bx			;now es = cs
		mov	ds,ax			;now ds = es on entry
		mov	si,di

rsRestDcIc:
		mov	di,offset DC		;restore DC & IC structures
		mov	cx,(SIZE DeviceContext + SIZE InfoContext)/2
		if	(SIZE DeviceContext + SIZE InfoContext) AND 1
		movsb
		endif
		rep	movsw

; Restore the Flags byte in the Ega/Vga video area BEFORE doing the setmode
; call.  This causes the bios to program the CRTC for the proper # of scan
; lines, and sets the proper ROM fonts for text modes.	Before this was done,
; a graphics app in 350 line mode would cause a problem for a text mode app
; which was initially in 400 line mode.

		push	es
		xor	ax,ax
		mov	es,ax
		assume	es:ABS0
;;		mov	al,[si+(size VideoBiosData)].ebdFlags
		mov	al,cs:[saveFlags]
		mov	es:[Rows+ebdFlags],al
		pop	es

		call	SetMode 		;restore old video state

;----------------------------------------------------------------------------;
; we will restore the font bansks (if custom fonts were loaded) after restor-;
; -ing the screen and the resgisters as the code might use winoldaps save    ;
; area to transfer the file data into video memory.	   		     ;
;----------------------------------------------------------------------------;

rsRestScreenFile:

		call	ScreenOff		;shut off video for now
		ifdef	VGACOLOR
		test	cs:[IC.icScrType],ST_LARGE  ;if screen not on disk,
		jz	rsRestBiosData		    ;  go restore BIOS data

		call	RestScreenFile		;else restore from disk
		jnc	rsSFOK
		jmp	rsErr			;if error, leave now
rsSFOK:
		endif

rsRestBiosData:
		xor	ax,ax
		mov	es,ax			;ds = 0
		assume	es:Abs0

		mov	di,offset CrtMode	;restore std BIOS data
		mov	cx,(SIZE VideoBiosData)/2
		if	(SIZE VideoBiosData) AND 1
		movsb
		endif
		rep	movsw

		mov	di,offset Rows		;restore EGA BIOS data
		mov	cx,(SIZE VgaBiosData)/2
		if	(SIZE VgaBiosData) AND 1
		movsb
		endif
		rep	movsw

		mov	di,offset lpSavePtr	;restore EGA SavePtr
		movsw
		movsw

		cmp	cs:[fVectra],TRUE	;if not running on Vectra,
		jne	rsRestScreen0		;  skip HP-specific stuff

		call	GetHpVideoSeg		;else get HP EX-BIOS video seg
		mov	es,ax			;load it into es
		assume	es:nothing
		xor	di,di			;start at offset 0
		mov	cx,(SIZE HpBiosData)/2	;restore HP EX-BIOS video data
		if	(SIZE HpBiosData) AND 1
		movsb
		endif
		rep	movsw
rsRestScreen0:
		ifdef	VGACOLOR
		test	cs:[IC.icScrType],ST_LARGE ;if screen restored from disk,
		jnz	rsRestEgaRegs		   ;  don't restore from memory
		endif
		mov	cx,cs:[IC.icScrLen]	   ;cx = screen length
		les	di,cs:[IC.iclpScr]	   ;es:di -> screen
		cmp	cs:[DC.dcfSwitchGmt],TRUE  ;if text page only,
		jne	rsRestScreen1		   ;  don't adjust di and cx

		xor	di,di			;else start at offset 0
		mov	cx,MAX_TOTGRPH		;and restore maximum amount
rsRestScreen1:
		shr	cx,1			;cx = cx/2 for word mov
		rep	movsw
		rcl	cx,1			;odd count will mov one more
		rep	movsb
rsRestEgaRegs:
		loop	$			;wait for display to settle
		loop	$			;helps prevent "screen bounce"

; now restore the BIOS fonts if any were loaded. Custom fonts would be restored
; later.

		call	RestBiosFonts

		mov	ax,cs
		mov	es,ax

		mov	cx, NUM_ATTR_REGS + NUM_CRTC_REGS
		add	cx, NUM_SEQ_REGS  + NUM_GRAPH_REGS
		inc	cx					; misc
		inc	cx					; feat
		mov	di, offset ssOldAttrRegs
		rep	movsb
		push	si			;save instance si
		push	ds			; save instance ds
		mov	ds,ax

		cli					;try removing this?!

		mov	di, offset ssOldCRTCRegs
		call	WaitVRetrace
		mov	cx,NUM_CRTC_REGS
		mov	dx,CRTC_ADDR AND 0FF0Fh
		or	dl, CS:[DC.dcAddrPatch]
		call	RestRegSet			;restore CRTC contents
		sti		     

		mov	bx, offset ssOldCRTCRegs
		call	ProgramERICrtc

		xor	ah,ah
		mov	al, es:[ssOldMiscReg]		; restore Misc output reg
		mov 	dx, MISC_OUTPUT
		out	dx, al

		call	ProgramERIMisc
		
		mov	al, es:[ssOldFeatReg]		; restore Feature control reg
		mov 	dx, FEAT_CONTROL_R AND 0FF0FH
		or	dl, cs:[DC.dcAddrPatch]		; make sure works on mono also
		out	dx, al

		call	ProgramERIFeature

		mov	di, offset ssOldSeqRegs
		mov	cx,NUM_SEQ_REGS
		mov	dx,SEQ_ADDR
		call	RestRegSet			;restore Seq contents

		mov	bx, offset ssOldSeqRegs
		call	ProgramERISeq

		mov	di, offset ssOldGrphRegs
		mov	cx,NUM_GRAPH_REGS
		mov	dx,GRAPH_ADDR
		call	RestRegSet			;restore Gr.Ctrlr contents

 		mov	bx,offset ssOldGrphRegs
		call	ProgramERIGrx

		mov	si,offset ssOldAttrRegs
		call	WaitVRetrace		;prevent glitches
		xor 	cx,cx
		mov	cl,NUM_ATTR_REGS
		mov	bx,si
		xor	ax,ax
		mov	dx,ATTR_ADDR
rsAttrLoop:
		mov	ah,ds:[bx]		;blast palette regs
		out	dx,al
		xchg	al,ah
		out	dx,al
		xchg	al,ah

		inc	bx
		inc	ax
		loop	rsAttrLoop

		mov	bx,offset ssOldAttrRegs
		call	ProgramERIAttr

		pop	es			;restore instance ds for call
		pop	di			;restore instance si					
		call	RestoreDACState
comment ^
;vvr
		mov	ax, ds
		mov	es, ax
		mov	bx, si
		mov	ax, 1C02h		; restore context 
		mov	cx, 00000111b
		int	10h			; do it
;vvr
comment ^

;----------------------------------------------------------------------------;
; now restore the fonts.						     ;
;----------------------------------------------------------------------------;


		call	RestFontFile		;else restore EGA font banks
 		call	ScreenOn		;turn on video
rsX:
		push	dx
		push	ax

		mov	al,ssOldSeqIndReg
		mov	dx,SEQ_ADDR
		out	dx,al

		mov	al,ssOldCrtIndReg
		mov	dx,CRTC_ADDR
		out	dx,al

		mov	al,ssOldGrxIndReg
		mov	dx,GRAPH_ADDR
		out	dx,al

		mov	al,ssOldAttrIndReg
		mov	dx,ATTR_ADDR
		out	dx,al

		pop	ax
		pop	dx
		clc				;show success
		ret
rsErr:
		call	OemBeep 		;show error
		stc
		ret
RestoreScreen	endp


ProgramERICrtc	proc 	near

		mov	dx,0			;register ID
		mov	ah,0f3h			;write range code
		mov	cx,NUM_CRTC_REGS	;start and #
		push	bx
		int	10h
		pop	bx
		mov	dx,0h			;register ID
		mov	ah,0f7h			;write default
		int	10h
		ret

ProgramERICrtc	endp

ProgramERIAttr	proc 	near

		mov	dx,18h			;register ID
		mov	ah,0f3h			;write range code
		mov	cx,NUM_ATTR_REGS	;start and #
		push	bx
		int	10h
		pop	bx
		mov	dx,18h			;register ID
		mov	ah,0f7h			;write default
		int	10h
		ret

ProgramERIAttr	endp

ProgramERIGrx	proc 	near

		mov	dx,10h			;register ID
		mov	ah,0f3h			;write range code
		mov	cx,NUM_GRAPH_REGS	;start and #
		push	bx
		int	10h
		pop	bx
		mov	dx,10h			;register ID
		mov	ah,0f7h			;write default
		int	10h
		ret

ProgramERIGrx	endp

ProgramERISeq	proc 	near

		mov	dx,8			;register ID
		mov	ah,0f3h			;write range code
		mov	cx,NUM_SEQ_REGS		;start and #
		push	bx
		int	10h
		pop	bx
		mov	dx,8h			;register ID
		mov	ah,0f7h			;write default
		int	10h

		ret

ProgramERISeq	endp

ProgramERIMisc	proc 	near

		mov	dx,20h			;register ID
		mov	ah,0f1h			;write one register code
		mov	bl,al			;data to write
		int	10h
		ret

ProgramERIMisc	endp

ProgramERIFeature proc 	near

		mov	dx,28h			;register ID
		mov	ah,0f1h			;write one register code
		mov	bl,al			;data to write
		int	10h
		ret

ProgramERIFeature endp

_TEXT		ends
		end
