
		page	,132
		%out	DevInit
		name	DEVINIT
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


;
; NAME
;	DEVINIT.ASM
;
; DESCRIPTION
;	This file contains the device-specific soft initialization code.
;
; SEE ALSO
;	InitScreen
;
; AUTHOR
;	Jim Christy
;
; HISTORY
;	1.00	060187	jhc	Epoch
;
;	1.10	071588	vvr	Added support for VGA. Eri is not required 
; 	1.11	062989	vvr	now add two mode bytes for misc and feature
;				in the vga context
;

_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		.xlist
		include vga.inc
		include dc.inc
		.list

		extrn	SaveTextSize:word
		extrn	SaveGrphSize:word
		extrn	DC:byte
		extrn	EnableSave:near

		public	DevInit
		public	VgaContextSize


FALSE		=	0
TRUE		=	1
MIN_ERI_VER	=	002h			;minimum acceptable ERI version

VgaContextSize	dw	0			;size of VGA context 


		subttl	DevInit


;
; DevInit - perform device-specific initializations
;
;	DevInit will be called only once during a given instance of the
;	grabber.  This gives the device-specific portions of the grabber a
;	chance to perform any soft initializations it may need to do EXCEPT
;	anything that may change the state of the display.  When DevInit is
;	called, the standard Windows device layer and video are still enabled,
;	and it is for this reason that the display state must not change.
;
;	If any hardware initializations are needed, these should be defered to
;	the InitScreen procedure, which is called after Windows layer has been
;	disabled and Winoldap has given the "thumbs up" to exec the oldap.
;
; ENTRY
;	ds	=  cs
; EXIT
;	none
; USES
;	flags
;
; NOTES
;	There is a bug in the HP Windows 1.03 Winoldap that causes it to
;	always call DisableSave ONCE, even though it never calls EnableSave.
;	DisableSave cannot tolerate an assymmetric call as such, so in order
;	for this grabber to work with older versions of Winoldap, we go ahead
;	and make the call to EnableSave ourselves.  This should really have
;	been fixed in the 1.03 Winoldap.  Future versions of the grabber
;	should remove this kludge.
;
DevInit 	proc	near
		assume	ds:_TEXT
		push	ax
		push	bx
		push	es

comment ^
		mov	ax, 1C00h		;int 10 subfuntion 1C only on
						; VGA BIOS
		mov	cx, 00000101b		;hardware, BIOS and DAC states
		int	10h			;got context size in bx
						;in 64 byte units
		mov	cx,07			;shift 7 times to 			
		shl	bx, cl			; multiply by 64
 		mov	[VgaContextSize],bx	;save the value for future
comment ^

		mov	bx,NUM_ATTR_REGS	;attribute controller
		add	bx,NUM_CRTC_REGS	;crtc
		add	bx,NUM_SEQ_REGS		;sequencer
		add	bx,NUM_GRAPH_REGS	;graphics controller
		add 	bx,0100h*3		; save DAC state also
		inc	bx			; misc_reg
		inc	bx			; feature_control
		mov	[VgaContextSize],bx	;palette also

		add 	SaveTextSize, bx	;adjust these by
		add	SaveGrphSize, bx	;  bx			

		pop	es
		pop	bx
		pop	ax
		ret
DevInit 	endp


_TEXT		ends
		end

