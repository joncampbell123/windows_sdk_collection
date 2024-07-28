
;
; NAME
;	DC.INC
;
; DESCRIPTION
;	This file supplies the structure definition for the grabber
;	DeviceContext data structure.
;
;	The DeviceContext structure is a display-dependent structure known
;	only to grabber modules in this level of the tree and below.  Its
;	layout, content, and/or length will vary from grabber to grabber in
;	other branches of the grabber source directory tree.
;
; AUTHOR
;	Jim Christy
;
; HISTORY
;	1.00	060187	jhc	Epoch
;

DeviceContext	struc
  dcScrMode	db	?			;BIOS screen mode
  dcScrStart	dw	?			;regen start position
  dcCursorPosn	dw	?			;cursor position in CRTC format
  dcCursorMode	dw	?			;cursor start/stop scanlines
  dcAddrPatch	db	?			;3Dx / 3Bx patch byte
  dcfSwitchGmt	db	?			;Switch graphics/multiple text
  dcFileNum	dw	?			;->random number in swapfile
  dcFontBank	db	8	dup (?) 	;VGA has 8 font banks
  dcSwapPath	db	64	dup (?) 	;full swap path
DeviceContext	ends

