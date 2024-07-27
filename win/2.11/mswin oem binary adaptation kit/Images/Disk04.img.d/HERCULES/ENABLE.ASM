	page	,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	ENABLE.ASM
;
; This module contains the routine which is called when the device
; is to either enable itself or return it's GDIINFO.
;
; Created: 16-Jan-1987
; Author:  **** ***** [*****]
;
; Copyright (c) 1983-1987 Microsoft Corporation
;
; Exported Functions:	Enable
;
; Public Functions:	none
;
; Public Data:		_cstods
;
; General Description:
;
;	The Display is called to enable itself on one of two occasions.
;
;	The first situation where the Disable routine is called is
;	when Windows is starting the session.  For this situation,
;	the driver will also be asked to return information about
;	the device hardware (e.g. resolution, etc).
;
;	The second is when an old application was run (e.g. WORD).
;	In this instance, Enable will be called to enable the display
;	hardware after the old application ran.
;
;	Unfortunately, there is no way to distinguish these two modes.
;
; Restrictions:
;
;-----------------------------------------------------------------------;

incDevice	= 1

	.xlist
	include cmacros.inc
	include gdidefs.inc
	.list


	externNP hook_int_2Fh		;Hook into multiplexed interrupt
	externA PHYS_DEVICE_SIZE	;Size of physical device


sBegin	Data

	externW ssb_mask		;Mask for save save screen bitmap bit

sEnd	Data





createSeg _INIT,InitSeg,word,public,CODE
sBegin	InitSeg
assumes cs,InitSeg


	externNP physical_enable	;Enable routine
	externB  physical_device	;Device physical data
	externB  info_table_base	;GDIInfo table
page
;--------------------------Exported-Routine-----------------------------;
; INT Enable(lpDevice,style,lpDeviceType,lpOutputFile,lpStuff)
; DEVICE lpDevice;		//device block or GDIInfo destination
; INT	 style; 		//Style of initialization
; LPSTR  lpDeviceType;		//Device type (i.e FX80, HP7470, ...)
; LPSTR  lpOutputFile;		//DOS output file name (if applicable)
; LPSTR  lpStuff;		//Device specific information
;
; Enable - Enable Device
;
; The given device is either initialized or the GDI information
; for the given device is returned.
;
; If style=InquireInfo, then GDI is asking that the parameters
; passed be interpreted and the appropriate GDI information
; for the device be returned in lpDevice.
;
; If style=EnableDevice, then GDI is requesting that the device
; be initialized and lpDevice be initialized with whatever
; data is needed by the device.
;
; The three other pointers passed in will be the same for both
; calls, allowing for the device to request only the minimum
; required for a device that is supported.  These will be
; ASCIIZ strings or NULL pointers if no parameter was given.
; These strings are ignored by the display drvier.
;
; For the inquire function, the number of bytes of GDIINFO placed
; into lpDevice is returned.  For the enable function, non-zero is
; returned for success.  In both cases, zero is returned for an error.
;
;
; Warnings:
;	Destroys AX,BX,CX,DX,ES,FLAGS
; Effects:
;	none
; Calls:
;	PhysicalEnable
; History:
;  Mon 21-Sep-1987 00:20:57 -by-  **** ***** [*****]
; Added call to hook_int_2Fh
;
;  Wed 12-Aug-1987 17:16:37 -by-  **** ***** [*****]
; Made non-resident.
;
;  Tue 19-May-1987 22:01:34 -by-  *** ****** [******]
; Added code to modify GDI info table if EGA doesn't have enough
; memory to make use of save_screen_bitmap
;
;  Fri 26-Jun-1987 15:00:00 -by-  *** ****** [******]
; Removed code mentioned above and put it in EGAINIT.ASM, in an INIT
; segment.  This restores the integrity of the device-dependence levels
; within the Mondo Tree Structure of Death.
;
;  Fri 16-Jan-1987 17:52:12 -by-  **** ***** [*****]
; Initial version
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; INT Enable(lpDevice,style,lpDeviceType,lpOutputFile,lpStuff)
; DEVICE lpDevice;		//device block or GDIInfo destination
; INT	 style; 		//Style of initialization
; LPSTR  lpDeviceType;		//Device type (i.e FX80, HP7470, ...)
; LPSTR  lpOutputFile;		//DOS output file name (if applicable)
; LPSTR  lpStuff;		//Device specific information
; {
;   if (style == inquire)
;   {
;	*(GDIINFO)lpDevice = (GDIINFO)info_table_base; //copy GDIINFO
;	return (sizeof(GDIINFO));
;   }
;
;   *lpDevice = (DEVICE)physical_device;    //Initialize Physical device
;   hook_int_2Fh();
;   return(physical_enable(lpDevice));	    //Initialize hardware
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

cProc	Enable,<FAR,PUBLIC,WIN,PASCAL>,<si,di>

	parmD	lp_device		;Physical device or GDIinfo destination
	parmW	style			;Style, Enable Device, or Inquire Info
	parmD	lp_device_type		;Device type (i.e FX80, HP7470, ...)
	parmD	lp_output_file		;DOS output file name (if applicable)
	parmD	lp_stuff		;Device specific information

cBegin
	push	ds
	mov	ax,cs			;Set up ds=cs
	mov	ds,ax
	assumes ds,InitSeg

	cld
	les	di,lp_device		;--> device structure or GDIinfo dest.
	assumes es,nothing

	and	style,InquireInfo	;Is this the inquire function?
	jnz	inquire_gdi_info	;  Yes, return GDIinfo
	errnz	InquireInfo-00000001b
	errnz	EnableDevice-00000000b
	errnz	InfoContext-8000h	;Ignore infomation context flag

;	Initialize passed device block

	lea	si,physical_device	;DS:SI --> physical device to copy
	mov	cx,PHYS_DEVICE_SIZE	;Set move count
	rep	movsb
	pop	ds
	assumes ds,Data

	call	hook_int_2Fh		;Hook into multiplexed interrupt
	call	physical_enable 	;Enable device
	jmp	short exit_enable
page

;	inquire_gdi_info - Inquire Device Specific Information
;
;	The GDI device specific information is returned to the caller
;
;	The information is based on the three pointers passed in.
;	Normally this data would be interpreted and the correct
;	GDINFO returned.  This allows for dynamically returning
;	the info based on the specifics of the device actually
;	being used (i.e. a driver supporting two similar plotters
;	could return the extents of the actual plotter in use).
;
;	These parameters are ignored for display drivers.
;
;	Currently:
;		ES:DI --> where GDIINFO goes
;		DS    =   CS

inquire_gdi_info:
	mov	si,InitSegOFFSET info_table_base
	mov	cx,size GDIINFO
	mov	ax,cx			;return size of GDIInfo
	rep	movsb

	pop	ds
	assumes ds,Data
	mov	bx,ssb_mask
	and	es:[di].dpRaster[-size GDIINFO],bx

exit_enable:

cEnd

sEnd	InitSeg

	ifdef	PUBDEFS
	include enable.pub
	endif

end
