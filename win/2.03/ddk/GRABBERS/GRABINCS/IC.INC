
;
; INFOCONTEXT STRUCTURE
;
;	InfoContext is a global structure shared by all the grabbers and may
;	be considered the oldap analog of the GdiInfo structure for Windows
;	drivers.  It provides information needed by routines at all levels of
;	the grabber source directory tree.  While storage for this structure
;	is always allocated in the leafnode directory module for each grabber,
;	it is typically validated by the GetMode routine.  Note that it is
;	essentially a superset of the GrabInfo structure returned by the
;	GetInfo entrypoint.  GrabInfo has plenty of reserved space so that
;	this information may be made available to the Winoldap layer in the
;	future as well as adding more fields if needed.
;
InfoContext	struc
  icDisplayId	db	?			;Display ID code
  icScrType	db	?			;Screen type code
  icSizeX	dw	?			;Horz raster size in .1mm units
  icSizeY	dw	?			;Vert raster size in .1mm units
  icCharsX	db	?			;Number of character columns
  icCharsY	db	?			;Number of character rows
  icMouseScaleX db	?			;Mouse to Grabber coord xform in X
  icMouseScaleY db	?			;Mouse to Grabber coord xform in Y
  icPixelsX	dw	?			;Number of pixels in X
  icPixelsY	dw	?			;Number od pixels in Y
  icWidthBytes	db	?			;Width in bytes of a row/scanline
  icBitsPixel	db	?			;Number of adjacent bits/pixel
  icPlanes	db	?			;Number of planes per pixel
  icInterlaceS	db	?			;Interlace shift factor
  icInterlaceM	db	?			;Interlace mask factor
  iclpScr	dd	?			;long pointer to screen
  icScrLen	dw	?			;current screen page length
InfoContext	ends

