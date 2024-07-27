        page    ,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	DISABLE.ASM
;
; This module contains the routine which is called when the device
; is to disable itself.
;
; Created: 16-Jan-1987
; Author:  **** ***** [*****]
;
; Copyright (c) 1983-1987 Microsoft Corporation
;
; Exported Functions:	Disable
;
; Public Functions:	none
;
; Public Data:		none
;
; General Description:
;
;	The Display is called to diable itself on one of two occasions.
;	The first is when an old application is being run (e.g. WORD).
;	In this instance, Disable will be called to disable the display
;	hardware while the old application runs, then Enable will be
;	called at some later time to reenable the device.  Any state
;	which is being maintained on the device must be saved by the
;	driver.
;
;	The second situation where the Disable routine is called is
;	when Windows is ending the session.  In this case, no state
;	need be save.  The display should return to a character mode.
;
;	Unfortunately, there is no way to distinguish these two modes.
;
; Restrictions:
;
;-----------------------------------------------------------------------;

	.xlist
	include cmacros.inc
	.list

	externNP restore_int_2Fh	;Restore multiplexed interrupt
	externNP physical_disable	;Routine to do the work


createSeg _INIT,InitSeg,word,public,CODE
sBegin	InitSeg
assumes cs,InitSeg
page
;--------------------------Exported-Routine-----------------------------;
; INT Disable(lpPDevice)
; DEVICE lpPDevice;
;
; The display driver's physical disable routine is called to disable
; graphics and enter a character mode of operation.
;
; Warnings:
;	Destroys AX,BX,CX,DX,ES,FLAGS
; Effects:
;	none
; Calls:
;	physical_disable
; History:
;	Tue 18-Aug-1987 18:06:47 -by-  **** ***** [*****]
;	Pass ES = Data to physical disable
;
;	Wed 12-Aug-1987 17:29:30 -by-  **** ***** [*****]
;	Made non-resident
;
;	Fri 16-Jan-1987 17:52:12 -by-  **** ***** [*****]
;	Initial version
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; INT Disable(lpPDevice)
; DEVICE lpPDevice;
; {
;   physical_disable(lpPDevice);	// Do all the work here
;   return(-1); 			// Show success
; }
;-----------------------------------------------------------------------;


cProc	Disable,<FAR,PUBLIC,WIN,PASCAL>,<si,di>

	parmD	lp_device

cBegin

	push	ds			;To break less drivers, pass
	pop	es			;  DGROUP in ES instead of DS
	assumes	es,Data

	lds	si,lp_device		;--> logical device
	assumes ds,nothing

	push	es			;physical_disable can destroy ES
	call	physical_disable	;Restore device
	pop	es			;need DGROUP in ES for restore_int_2Fh
	assumes	es,Data
	call	restore_int_2Fh
	mov	ax,-1			;Show success

cEnd

sEnd	InitSeg
end
