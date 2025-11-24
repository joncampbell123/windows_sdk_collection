;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

page		,132
title		Unimplemented GDI Feature Stubs
;
;
.xlist
include 	CMACROS.INC
.list
;
;
sBegin		Code
assumes 	cs,Code
assumes 	ds,Data
;
;	In MS-WINDOWS 4.xx, there are a number of unimplemented GDI calls to
;the device driver.  The GDI expects the device driver to return these calls
;to it with an appropriate return code.  This file simply exists to do that
;task in a convenient and non-intrusive manner.
;
subttl		Save Screen Bitmap Function
page +
cProc		SaveScreenBitmap,<FAR,PUBLIC,WIN,PASCAL>
;
	parmD	lpRect
	parmW	Command
;
cBegin
	xor	ax,ax			;return a failure
cEnd
;
;
sEnd		Code
end
