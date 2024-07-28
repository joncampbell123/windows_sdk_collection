
;
; DEVICECONTEXT STRUCTURE
;
;	The DeviceContext structure is a display-dependent structure known
;	only to grabber modules in this level of the tree and below.  Its
;	layout, content, and/or length will vary from grabber to grabber in
;	other branches of the grabber source directory tree.
;
DeviceContext	struc
  dcScrMode	db	?			;BIOS screen mode
  dcScrStart	dw	?			;regen start position
  dcCursorPosn	dw	?			;cursor position in CRTC format
  dcCursorMode	dw	?			;cursor start/stop scanlines
  dcAddrPatch	db	?			;3Dx / 3Bx patch byte
  dcfSwitchGmt	db	?			;Switch graphics/multiple text
  dcFileNum	dw	?			;->random number in swapfile
  dcFontBank	db	4	dup (?) 	;ERI font info
  dcSwapPath	db	64	dup (?) 	;full swap path
DeviceContext	ends

