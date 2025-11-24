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

;
;Include file for MiniVDD definitions:
;
;
;The PerVMData structure.  This structure represents the our reserved space
;in the so-called CB data structure.  The CB data structure is created for
;each VM started in the system.  We can therefore, save states that we need
;to remember on a per-VM basis.  This data will vary for every mini-VDD.
;The ATI chipset requires a lot of registers states for its extension registers
;to be saved.
;
PerVMData	struc
	ATIEx00 	db	?
	ATIEx01 	db	?
	ATIEx02 	db	?
	ATIEx03 	db	?
	ATIEx04 	db	?
	ATIEx05 	db	?
			db	1ah dup(?)
	ATIEx20 	db	?
			db	?
			db	?
	ATIEx23 	db	?
	ATIEx24 	db	?
	ATIEx25 	db	?
	ATIEx26 	db	?
			db	?
	ATIEx28 	db	?
	ATIEx29 	db	?
			db	?
	ATIEx2b 	db	?
			db	?
	ATIEx2d 	db	?
	ATIEx2e 	db	?
			db	?
	ATIEx30 	db	?
	ATIEx31 	db	?
	ATIEx32 	db	?
	ATIEx33 	db	?
	ATIEx34 	db	?
	ATIEx35 	db	?
	ATIEx36 	db	?
	ATIEx37 	db	?
	ATIEx38 	db	?
	ATIEx39 	db	?
	ATIEx3a 	db	?
	ATIEx3b 	db	?
	ATIEx3c 	db	?
	ATIEx3d 	db	?
	ATIEx3e 	db	?
	ATIEx3f 	db	?
	ATIExIndex	db	?
	ATI4AE8State	dw	?
	ATI4AEEState	dw	?
	CurrentMode	dw	?
PerVMData	ends
