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

;----------------------------------------------------------------------------
; DISABLE.ASM
;----------------------------------------------------------------------------
	.xlist
	include cmacros.inc
	incDevice = 1
	include gdidefs.inc
	include dibeng.inc
	include	minivdd.inc
	include macros.inc
        .list

;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
STOP_IO_TRAP    = 4000h		        ; stop io trapping
START_IO_TRAP	= 4007h 		; re-start io trapping

;----------------------------------------------------------------------------
; E X T E R N S
;----------------------------------------------------------------------------
	externNP RestoreInt2Fh		;Restore multiplexed interrupt

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin  Data
	externW OurVMHandle		;in VGA.ASM
	externD VDDEntryPoint		;in VGA.ASM
	externW wEnabled
sEnd    Data


;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
createSeg _INIT,InitSeg,word,public,CODE
sBegin	InitSeg
assumes cs,InitSeg
        .386
;----------------------------------------------------------------------------
; Disable
;   The display driver's physical disable routine is called to disable
;   graphics and enter a character mode of operation.
;----------------------------------------------------------------------------
cProc	Disable,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
	parmD	lp_device
cBegin
	mov	ax,DGROUP
	mov	ds,ax
        assumes ds,Data
        assumes es,nothing
        assumes gs,nothing
        assumes fs,nothing
	mov	wEnabled,0
	les	si,lp_device
lock	or	es:[si].deFlags,BUSY	;Set the BUSY bit
;
;Tell the VDD to start trapping the standard VGA ports again (such as the
;CRTC, Sequencer, and GCR registers).  This should be done before the change
;back to VGA text mode since the VDD needs to know about the text mode state
;that the Windows VM is going to be in after Disable is completed.
;
	mov	ax,START_IO_TRAP
	int	2fh			;start i/o trapping
;
;Call the VDD to inform it that we're disabling:
;
	mov	eax,VDD_DRIVER_UNREGISTER
					;pass the VDD API function code in EAX
	movzx	ebx,OurVMHandle 	;we need this for call
	call	VDDEntryPoint		;
;
;Now, set ourselves back to VGA text mode 3:
;
	mov	ax,3
	int	10h
;
;Unhook ourselves from the INT 2FH chain:
;
	call	RestoreInt2Fh
	mov	ax,-1			;Show success
cEnd
sEnd	InitSeg
end
