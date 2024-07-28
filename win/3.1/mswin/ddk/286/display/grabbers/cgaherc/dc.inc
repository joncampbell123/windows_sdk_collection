
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
  dcModeCtl	db	?			;3x8 mode reg data
  dcExModeCtl	db	?			;3xx extended mode reg data
  dcColorSelect db	?			;3D9 color select reg data
  dcCrtcParms	dw	?			;->CRTC parms for non-BIOS modes
  dcfSwitchGmt	db	?			;switch graphics/multiple text
DeviceContext	ends

