	page	,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	INQUIRE.ASM
;
; This module contains the routine which returns pointer shape
; information to the Window Manager.
;
; Created: 16-Jan-1987
; Author:  **** ***** [*****]
;
; Copyright (c) 1983-1987 Microsoft Corporation
;
; Exported Functions:	Inquire
;
; Public Functions:	none
;
; Public Data:		none
;
; General Description:
;
; Restrictions:
;
;-----------------------------------------------------------------------;


	.xlist
	include cmacros.inc
	include windefs.inc
	.list


createSeg _BLUEMOON,BlueMoonSeg,word,public,CODE
sBegin	BlueMoonSeg
assumes cs,BlueMoonSeg

	externB inquire_data		;Data to be returned

page
;--------------------------Exported-Routine-----------------------------;
; WORD Inquire(lpCURSORINFO)
; CURSORINFO far *lpCURSORINFO; //pointer where cursor info goes
; 
; Information about the pointer is returned to the caller in the
; given buffer.  The number of bytes copied into lpCURSORINFO
; is returned.
;
; Warnings:
;	Destroys AX,CX,ES,FLAGS
; Effects:
;	none
; Calls:
;	none
; History:
;	Wed 12-Aug-1987 17:29:30 -by-  **** ***** [*****]
;	Made non-resident
;
;	Fri 16-Jan-1987 17:52:12 -by-  **** ***** [*****]
;	Initial version
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; WORD Inquire(lpCURSORINFO)
; CURSORINFO far *lpCURSORINFO; 	//pointer where cursor info goes
; {
;   *lpCURSORINFO = inquire_data;	//copy entire structure
;   return (sizeof(CURSORINFO));
; }
;-----------------------------------------------------------------------;

	assumes ds,Data

cProc	Inquire,<PUBLIC,FAR,WIN,PASCAL>,<si,di>

	parmD	lp_cursor_info		;Where to put the data

cBegin
	push	cs
	pop	ds
	assumes ds,BlueMoonSeg

	les	di,lp_cursor_info	;--> destination area
	assumes es,nothing

	mov	si,BlueMoonSegOFFSET inquire_data
	mov	ax,size CURSORINFO	;Return code is # of bytes moved
	mov	cx,ax
	cld
	rep	movsb

cEnd

sEnd	Seldom
end
