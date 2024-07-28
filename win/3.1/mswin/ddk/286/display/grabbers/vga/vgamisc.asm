
		page	,132
		%out	VGA Grabber Support Routines
		name	VGAMISC
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


;
; NAME
;	VGAMISC.ASM
;
; DESCRIPTION
;	This file contains all the VGA helper routines that did not
;	logically belong in any other module.
;
; AUTHOR
;	Jim Christy
;
; HISTORY
;	1.00	060187	jhc	Epoch
;	1.01	091787	jhc	Changed WaitVRetrace to not cli; the vertical
;				retrace interval is long enough that we don't
;				need this.
;	1.10	071588	vvr	added routines to read/write from VGA
;				and VGA DAC state
;       1.11    080989   ac     IFDEFed code that has changed or is no longer
;				needed under windows 3.0 winoldaps.


		.xlist
		include	cmacros.inc
		.list

_TEXT		segment word	public	'CODE'
		assume	cs:_TEXT


		.xlist
		include vgaabs0.inc
		include dc.inc
		include vga.inc
		include fileio.inc
		.list

		extrn	DC:byte
		extrn	fGotSwapPath:byte
		extrn	AX2Hex:near
		

		public	BrstDet
		public	WaitVRetrace
		public	ScreenOn
		public	ScreenOff
		public	MakeBase
		public	MakeTempFile
		public	StrLen
		public	GetRegSet
		public	GetRegSetAttr
		public	RestRegSet
		public	SaveDACState
		public	RestoreDACState
;
; Macro to take care of time lag... 
;

IO_Delay macro
         jmp       $+2
         jmp       $+2
endm

ERR_TERM        =       92e8h       		; 8514 error term register.
FALSE		=	0
TRUE		=	1
RETRY_COUNT	=	3			;retry count for temp file creation

HexDigits	db	'0123456789ABCDEF'      ;lookup table for AX2Hex call


		subttl	BrstDet
		page


;
; BrstDet - determine number of scan lines for raster
;
;	BrstDet, similar to the IBM BIOS function by the same name, determines
;	whether the current raster should be 200 or 350 scanlines based on the
;	switch settings on the rear of the EGA card.  In a nutshell, switch
;	settings 0011 or 1001 indicate 350 lines, otherwise 200 lines.
;
; ENTRY
;	none
; EXIT
;	cf	=  0:	200 scanlines active
;	cf	=  1:	350 scanlines active
; USES
;	flags
;
BrstDet 	proc	near
		push	ax
		push	ds
		xor	ax,ax
		mov	ds,ax			;ds = 0
		assume	ds:Abs0
		mov	al,[Info3]		;get feature and switch info
		and	al,00001111b		;mask for switches
		cmp	al,00001001b		;most common config
		je	bd350

		cmp	al,00000011b		;less common
		je	bd350

		stc				;else set carry, cmc will clear
bd350:
		cmc				;toggle carry
		pop	ds
		pop	ax
		ret
BrstDet 	endp


		subttl	WaitVRetrace
		page


;
; WaitVRetrace - wait for VGA vertical retrace interval
;
; ENTRY
;	none
; EXIT
;	none
; USES
;	ax, dx, flags
;
WaitVRetrace	proc	near
		mov	dx,INPUT_STATUS_1 AND 0FF0Fh
		or	dl,cs:[DC.dcAddrPatch]
wvrInSync:
		in	al,dx			;get real-time status
		test	al,00001000b		;if already in Vretrace
		jnz	wvrInSync		;loop until we are not
wvrOutSync:
		in	al,dx			;get real-time status
		test	al,00001000b		;if not in Vretrace
		jz	wvrOutSync		;loop until we are
		ret
WaitVRetrace	endp



		subttl	ScreenOn
		page


;
; ScreenOn - enable VGA video stream
;
; ENTRY
;	none
; EXIT
;	none
; USES
;	ax, dx, flags
;
ScreenOn	proc	near
		mov	dx,INPUT_STATUS_1 AND 0FF0Fh
		or	dl,cs:[DC.dcAddrPatch]
		in	al,dx			;reset attr flip-flop

		push	dx
		mov	dl,ATTR_ADDR AND 0FFh
		mov	al,020h 		;enable video
		out	dx,al
		pop	dx

		in	al,dx			;reset attr flip-flop
		ret
ScreenOn	endp


		subttl	ScreenOff
		page


;
; ScreenOff - disable VGA video stream
;
; ENTRY
;	none
; EXIT
;	none
; USES
;	ax, dx, flags
;
ScreenOff	proc	near
		mov	dx,INPUT_STATUS_1 AND 0FF0Fh
		or	dl,cs:[DC.dcAddrPatch]
		in	al,dx			;reset attr flip-flop

		push	dx
		mov	dl,ATTR_ADDR AND 0FFh
		xor	al,al			;disable video
		out	dx,al
		pop	dx

		in	al,dx			;reset attr flip-flop
		ret
ScreenOff	endp


		subttl	MakeBase
		page


;
; MakeBase - find proper video params for given mode
;
;	MakeBase, similar to the IBM BIOS function by the same name, will
;	return a pointer to the correct table of video parameters to use when
;	initializing the VGA for a given mode.	The root of the list of tables
;	is derived from the ParmPtr in the SavePtr table.
;
; ENTRY
;	ah	=  video mode
;	ds	=  0
; EXIT
;	es:si	-> base of correct parameter table
; USES
;	ax, si, es, flags
;
; BUGS
;

MakeBase	proc	near
		assume	ds:Abs0 		;ds must be = 0
		les	si,[lpSavePtr]		;load up SavePtr
		les	si,es:[si]		;load up ParmPtr

vgamodes:

		add	si,00440h		;magic number !
		cmp	ah,10h
		ja	mbNoAdjust		;will be adjusted fine 
		sub	si,00440h
		test	[flags],00010001b	;Misc flags to check 400 lines
		jz	mbEgaModes		;not 400 lines
		add 	si,005C0h
		cmp	ah,1	 		; is it 0+ or 1+ mode
		jle	mbNoAdjust		; yes so get get offset
		add	si,00040h
		cmp	ah,3			; is it 2+ or 3+ mode
		jle	mbX			; yes, so get offset
		add	si,00040h		
		cmp	ah,7
		je	mbX			;already at the offset
		sub	si,00640h					

mbEgaModes:
		test	[Info],01100000b	;if 64K video memory,
		jz	mb64K			;  skip special graphics tests

		add	si,00440h		;bump to alt 640x350x1
		cmp	ah,00Fh 		;if this is what we want,
		je	mbX			;  we are done

		add	si,00040h		;bump to alt 640x350x4
		cmp	ah,010h 		;if this is what we want,
		je	mbX			;  we are done

		sub	si,00480h		;nope, not special graphics
mb64K:
		cmp	ah,003h 		;if not alpha,
		ja	mbNoAdjust		;  skip special alpha tests

		call	BrstDet 		;if not enhanced config,
		jnc	mbNoAdjust		;  no adjustment needed

		add	si,004C0h		;bump to enhanced alpha parms
mbNoAdjust:
		xor	al,al			;now use mode as final index
		shr	ax,1			;funky math does the job
		shr	ax,1
		add	si,ax
mbX:
		ret				;es:si -> correct table
MakeBase	endp


		subttl	MakeTempFile
		page


;
; MakeTempFile - build and open a Windows-compatible temporary filename
;
; ENTRY
;	ds:si	-> numeric part of filename to patch for randomness
;	ds:dx	-> beginning of pathname
; EXIT
;	cf	=  0
;	bx	=  handle to new file
; ERROR EXIT
;	cf	=  1
;	ax	=  error code
; USES
;	ax, bx, cx, flags
;
MakeTempFile	proc	near
		assume	ds:_TEXT
		cmp	[fGotSwapPath],TRUE		;if SetSwapDrive got called,
		je	mtfGo				;  proceed
		stc
		ret
mtfGo:
		mov	cx,RETRY_COUNT
mtfChkOpen:
		push	cx
		push	dx
		mov	ah,02Ch
		int	021h				;get time
		mov	ax,dx				;ax=minutes:seconds
		mov	bx,offset HexDigits
		call	AX2Hex				;patch fname for randomness
		pop	dx
		pop	cx
		mov	al,FAC_READWRITE		;file access code
		OpenFile				;if file doesn't exist,
		jc	mtfCreate			;  we may create it

		mov	bx,ax
		CloseFile				;else close it and
		loop	mtfChkOpen			;  try another
		stc
		ret
mtfCreate:
		mov	cx,FA_NORMAL			;file attribute
		CreateFile
		xchg	ax,bx
		ret
MakeTempFile	endp


		subttl	StrLen
		page


;
; StrLen - get length of string
;
; ENTRY
;	es:di	-> ASCIIZ string
; EXIT
;	cx	=  length of string in bytes, not including NULL at end
; USES
;	cx, flags
;
StrLen		proc	near
		cld
		push	ax
		push	di
		xor	al,al			;search for a NULL
		mov	cx,0FFFFh		;max search = one segment
		repne	scasb			;try to find a NULL
		inc	cx			;rep always goes one too far
		inc	cx			;don't include NULL in count
		neg	cx			;length = 0FFFFh - cx
		pop	di
		pop	ax
		ret
StrLen		endp

;
; GetRegSetAttr - get palette registers
;
; ENTRY
;	es:di	-> address where to read
;	cl	= register count
;	ch	= starting index register 
;	dx	= address/data register port of Attribute Controller
;		 		
; EXIT
;	none
; USES
;	ax
;

GetRegSetAttr	proc	near

		
		xor	ax,ax
		mov	al,ch
		xor	ch,ch
gNextRegAtt:
		push	ax
		mov 	dx,INPUT_STATUS_1 AND 0FF0Fh
		or	dl,[DC.dcAddrPatch]
		in	al,dx		   		;reset attr flipflop
		mov	dx,ATTR_ADDR
		pop	ax		
		out	dx,al
		inc	dx
		mov	ah,al
		in	al,dx
		stosb
		mov	al,ah
		inc	al
		dec	dx
		loop	gNextRegAtt
		ret

GetRegSetAttr	endp

;
; GetRegSet - get a set VGA Registers 
;
; ENTRY
;	es:di	-> address where to read
;	cl	= register count
;	ch	= starting index register 
;	dx	= address register port (Sequencer or Graphics Controller)
;		 		
; EXIT
;	none
; USES
;	ax
;

GetRegSet	proc	near
		
		xor	ax,ax
		mov	al,ch
		xor	ch,ch
gNextReg:
		out	dx,al
		inc	dx
		mov	ah,al
		in	al,dx
		stosb
		mov	al,ah
		inc	al
		dec	dx
		loop	gNextReg
		ret

GetRegSet	endp

;
; RestRegSet - Restore a set VGA Registers 
;
; ENTRY
;	es:di	-> address where to read
;	cl	= register count
;	ch	= starting index register 
;	dx	= address register port (Sequencer or Graphics Controller)
;		 		
; EXIT
;	none
; USES
;	ax
;

RestRegSet	proc	near
		
		xor	ax,ax
		mov	al,ch
		xor	ch,ch
rNextReg:
		mov	ah, byte ptr es:[di]
		out	dx,ax
		inc	ax
		inc	di
		loop	rNextReg
		ret

RestRegSet	endp

;
; SaveDACState - Save  DAC VGA Registers 
;
; ENTRY
;	es:di	-> address where to read
;	 		
; EXIT
;	di -> next address to save from in the buffer
;
; USES
;	ax,cx,dx
;

SaveDACState	proc	near
		push	bx
		xor	bx,bx
		mov	cx,00100h		;256 registers to read				       
		mov	ax,01017h		;read a block of DAC regs
		mov	dx,di
		int	10h
		add	di,cx
		shl	cx,1
		add	di,cx			;di = di + 256*3
		pop	bx		
		ret
SaveDACState	endp

;
; RestoreDACState - Restore  DAC VGA Registers 
;
; ENTRY
;	es:di	-> address where to read
;	 		
; EXIT
;	di -> next address to read
;
; USES
;	ax,cx,dx
;

RestoreDACState	proc	near
		push	bx
		xor	bx,bx
		mov	cx,00100h		;256 registers to read				       
		mov	ax,01012h		;read a block of DAC regs
		mov	dx,di
		int	10h
		add	di,cx
		shl	cx,1
		add	di,cx			;di = di + 256*3
		pop	bx		
		ret
RestoreDACState	endp

;----------------------------------------------------------------------------;
; Native8514Mode:							     ;
;									     ;
; This routine tests to see if the adaptor is in a native 8514 mode and if so;
; returns carrys set. If 8514 is not present or the adaptor is not configured;
; in the native mode, carry will be cleared on exit. This routine uses IBM's ;
; AI programing interface and does not deal with the hardware directly. So   ;
; for programs that program the 8514 hardware directly, this routine will not;
; work.								             ;
;----------------------------------------------------------------------------;

cProc	Native8514Mode,<NEAR,PUBLIC,PASCAL>,<es,di,bx,ax,cx>

	localV	HQMODE_Params,20	;parameter buffer for querry mode
	localW	AI_Seg			;segment for the AI calls
	localW	AI_Off			;offset for the AI calls

cBegin
	push	dx
        mov  dx, ERR_TERM        ; load DX with port address (error term port).
        mov  ax, 5555h           ; load AX with value to write to port.
        out  dx, ax              ; Write the data.
        IO_Delay
        IO_Delay
        in   ax, dx              ; Now read the data back in.
        cmp  ax, 5555h           ; Q: is 8524 present ?
	pop	dx
	clc			 ; No 8514
        jne  Native8514ModeRet   ;  N: indicate 8514 not present.

; test  to see whether INT 7FH has been hooked or not.

	xor	bx,bx			;IDT segment
	mov	es,bx			;es points to IDT segment
	mov	bx,7fh * 4		;offset for INT 7fh
	mov	ax,es:[bx]		;get the offset
	or	ax,es:[bx+2]		;get the segment,is the vector zero ?
	jz	Native8514ModeRet	;AI not loaded, (carry is clear)

; This part is real risky now. Having an INT 7F vector does not really mean
; that it has an AI interface. We depend on the carry being set by the next
; call. So we will start off by doing a set carry. If it is not AI then I
; hope that others will not change carry on the stack before doing IRET.

; get the call address for the AI interface.

	stc				;default: no AI
	mov	ax,105h			;get the interface link table address
	int	7fh			;if carry is set, intreface is not in
	cmc				;carry clear if interface not in
	jnc	Native8514ModeRet	;no AI.

; save the address of the link table.

	mov	AI_Seg,cx		;save the segment
	mov	AI_Off,dx		;and the offset

; now prepare the parameters for the HQMODE call and make the call.

	push	ss
	pop	es			;es=ss
	lea	di,HQMODE_Params	;es:di points to the parm block
	push	es
	push	di			;save
	push	es			;segment of the parameter block
	push	di			;offset of the parameter block
	mov	ax,20			;size of the block
	stosw				;save it
	mov	es,AI_Seg		;get the segment for the table
	mov	bx,1dh*4		;entry offset for HWMODE call
	add	bx,AI_Off		;entry offset in link table
	call	dword ptr es:[bx];call query mode
	pop	di
	pop	es			;es:di points to the parameter block
	cmp	byte ptr es:[di+2],0ffh	;not 0ffh means native mode

; carry will be set if the daptor is in native mode

Native8514ModeRet:

cEnd

_TEXT		ends
		end

