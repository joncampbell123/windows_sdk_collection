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

        parmD   lpDestDev               ;--> to destination bitmap descriptor
        parmW   DestxOrg                ;Destination origin - x coordinate
        parmW   DestyOrg                ;Destination origin - y coordinate
        parmD   lpSrcDev                ;--> to source bitmap descriptor
        parmW   SrcxOrg                 ;Source origin - x coordinate
        parmW   SrcyOrg                 ;Source origin - y coordinate
        parmW   xExt                    ;x extent of the BLT
        parmW   yExt                    ;y extent of the BLT
        parmD   Rop                     ;Raster operation descriptor
        parmD   lpPBrush                ;--> to a physical brush (pattern)
        parmD   lpDrawMode              ;--> to a drawmode

	localW	DeviceFlags
	localW	SrcDeltaScan
	localW	DestDeltaScan
	localW	DestHeight
	localW  SrcFlags
	localW	pSaveBank
	localW	pRestoreBank
	localW	pSetSBank
	localW	pSetDBank
	localW	pSetBank
	localW	StartScan
	localW	nScans
	localW	nPixels
	localW	StartX
	localW	nQuadPixels
	localW	InterleaveBase

	localB	bMirrorBrush
	localB	bDirection
	localB	PatRow
	localB	PatCol
	localB  bCurrentBankSetting
	localB  bNextBank
	localB	bBankShiftCount
