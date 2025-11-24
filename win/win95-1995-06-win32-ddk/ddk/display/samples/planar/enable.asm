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

	page	,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	ENABLE.ASM
;
; This module contains the routine which is called when the device
; is to either enable itself or return it's GDIINFO.
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
	.286p
incDevice	= 1

	.xlist
	include cmacros.inc
	include gdidefs.inc
	include	mflags.inc
	include debug.inc
	.list

	externFP    GetModuleHandle
	externFP    GetProcAddress
	externFP    AllocCSToDSAlias
	externFP    FreeSelector
	externFP    FindResource
	externFP    LoadResource
	externFP    LockResource
	externFP    GetModuleHandle



	externNP hook_int_2Fh		;Hook into multiplexed interrupt
	externNP restore_int_2Fh	;Restore multiplexed interrupt
	externA PHYS_DEVICE_SIZE	;Size of physical device
	externA ScreenSelector		;selector for video buffer

ifdef PALETTES
	externFP SetPaletteTranslate    ;in color\ega\vga\palette.asm
endif


sBegin	Data

	externW ssb_mask		;Mask for save save screen bitmap bit
	externD lpDeviceForPenWindows

sEnd	Data

createSeg _INIT,InitSeg,word,public,CODE
sBegin	InitSeg
assumes cs,InitSeg

	externNP physical_enable	;Enable routine
	externNP physical_disable	;Disable routine
	externW  physical_device	;Device physical data
	externB  info_table_base	;GDIInfo table
	externW	 drivers_TC_caps	;Text Capability word
	externW	 drivers_RC_caps	;Raster capability word
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

	mov	ax,word ptr lp_device[0]		;save lpDevice
	mov	word ptr lpDeviceForPenWindows[0],ax
	mov	ax,word ptr lp_device[2]
	mov	word ptr lpDeviceForPenWindows[2],ax

	push	ds
	mov	ax,cs			;Set up ds=cs
	mov	ds,ax
	assumes ds,InitSeg

	cld
	les	di,lp_device		;--> device structure or GDIinfo dest.
	assumes es,nothing

	and	style,InquireInfo	;Is this the inquire function?
	jz	@f			;no
	jmp	short inquire_gdi_info	;  Yes, return GDIinfo
@@:
	errnz	InquireInfo-00000001b
	errnz	EnableDevice-00000000b
	errnz	InfoContext-8000h	;Ignore infomation context flag

;	Initialize passed device block

	lea	si,physical_device	;DS:SI --> physical device to copy
	mov	cx,PHYS_DEVICE_SIZE	;Set move count
	rep	movsb
	pop	ds
	assumes ds,Data

	push	ds
	call	hook_int_2Fh		;Hook into multiplexed interrupt
	call	physical_enable 	;Enable device
	pop	es
	assumes	es,Data
	or	ax,ax
	jnz	short exit_enable	
	call	restore_int_2Fh
	xor	ax,ax
	jmp	exit_enable
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
if	MASMFLAGS and PUBDEFS
	public	inquire_gdi_info
	public	exit_enable
endif
end
