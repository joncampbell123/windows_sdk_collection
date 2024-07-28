	page	,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	ENABLE.ASM
;
; This module contains the routine which is called when the device
; is to either enable itself or return it's GDIINFO.
;
; Created: 16-Jan-1987
; Author:  Walt Moore [waltm]
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
	externFP    FatalExit
	externFP    FindResource
	externFP    LoadResource
	externFP    LockResource
	externFP    GetModuleHandle



	externNP hook_int_2Fh		;Hook into multiplexed interrupt
	externNP restore_int_2Fh	;Restore multiplexed interrupt
	externA PHYS_DEVICE_SIZE	;Size of physical device
	externA	__WinFlags		;real/protected mode indicator
	externA ScreenSelector		;selector for video buffer

;----------------------------------------------------------------------------;
; define the flag bit in __WinFlags					     ;
;----------------------------------------------------------------------------;
	public	WINP

WINR	equ	00000000b		;real mode, 8086
WINP	equ	00000001b		;protected mode
WIN286	equ	00000010b		;running on a 286 machine
WIN386	equ	00000100b		;running on a 386 machine

;-----------------------------------------------------------------------------;

ifdef PALETTES
	externFP SetPaletteTranslate    ;in color\ega\vga\palette.asm
endif


sBegin	Data

	externW ssb_mask		;Mask for save save screen bitmap bit

if	MASMFLAGS and PENWIN
	externD lpDeviceForPenWindows
endif

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
; History:
;  Fri 04-Jan-1991 14:50 -by-  Ray Patrick [raypat]
;  Removed redundant code that patches the ScreenSelector into 
;  the first word of the passed device block.  This value is
;  set at load time and is defined in the physical device declaration
;  (a BITMAP structure) in the file VGA.ASM.
;
;  Thu 03-Nov-1988 14:59:15 -by-  Amit Chatterjee [amitc]
; Added a call to initialize the palette translation table
;
;  Mon 21-Sep-1987 00:20:57 -by-  Walt Moore [waltm]
; Added call to hook_int_2Fh
;
;  Wed 12-Aug-1987 17:16:37 -by-  Walt Moore [waltm]
; Made non-resident.
;
;  Tue 19-May-1987 22:01:34 -by-  Bob Grudem [bobgru]
; Added code to modify GDI info table if EGA doesn't have enough
; memory to make use of save_screen_bitmap
;
;  Fri 26-Jun-1987 15:00:00 -by-  Bob Grudem [bobgru]
; Removed code mentioned above and put it in EGAINIT.ASM, in an INIT
; segment.  This restores the integrity of the device-dependence levels
; within the Mondo Tree Structure of Death.
;
;  Fri 16-Jan-1987 17:52:12 -by-  Walt Moore [waltm]
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

if	MASMFLAGS and PENWIN
	mov	ax,word ptr lp_device[0]		;save lpDevice
	mov	word ptr lpDeviceForPenWindows[0],ax
	mov	ax,word ptr lp_device[2]
	mov	word ptr lpDeviceForPenWindows[2],ax
endif

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
	push	dx			;save string resource id.
	lds	si,lp_device
	push	es
	call	physical_disable
	pop	es
	call	restore_int_2Fh
	pop	di			;Get string resource id in di.
	call	BootErrorMsg		;Error string output handler.

	mov	ah,08h
	int	21h

	xor	ax,ax
	cCall	FatalExit,<ax>
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

IFE MASMFLAGS and ROM; in ROM we leave GDIINFO alone.  it should be initialized
		; to the proper state (depending on the processor type the
		; system is built for

;----------------------------------------------------------------------------;
; for real mode drivers we want to reset the BIGFONT capabilities and BOLD   ;
; TEXT capabilities.						             ;
;----------------------------------------------------------------------------;
	mov	cx,__WinFlags
if DEBUG_286
	mov	cx,WIN286
endif
	test	cx,WINP			;Are we in protected mode?
	jz	@f			;no. Modify GDIINFO structure.
	test	cx,WIN286		;Are we on a 32 bit processor?
	jz	driver_info_ok		;yes.  GDIINFO structure is okay.

; get a data segment alias for InitSeg

@@:	push	es
	mov	ax, cs
	cCall	AllocCSToDSAlias, <ax>
	mov	es, ax
	assumes	es, InitSeg

; mask out BOLD TEXT capabilities

	mov	ax,es:[drivers_TC_caps]	;text capabilities defined here
	and	ax, not TC_EA_DOUBLE	;reset bold text bit
	mov	es:[drivers_TC_caps],ax	;save it

; mask out RC_BIGFONT from drivers_RC_caps words

	mov	ax,es:[drivers_RC_caps]	;get the word
	and	ax, not RC_BIGFONT	;mask out BIG FONT capability.
	mov	es:[drivers_RC_caps],ax	;save it back

; now release the alias selector

	mov	ax,es
	xor	bx,bx
	mov	es,bx			;invalidate es before freeing it
	cCall	FreeSelector,<ax>
	pop	es

ENDIF ; ROM

driver_info_ok:
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
;----------------------------------------------------------------------------;
;BootErrorMsg
;Note: Must be in text mode for this to work.
;October, 1991 -by- [raypat]
;Entry:
; di: String Resource ID to print out.
;----------------------------------------------------------------------------;
BootErrorMsg	proc	near
	lea	ax,szDisplay
	farPtr	lpModuleName,cs,ax
	cCall	GetModuleHandle,<lpModuleName>		;ax = is module handle.
	mov	si,ax
	lea	bx,szRCDATA
	farPtr	ResourceID,0,di
	farPtr	ResourceType,cs,bx
	cCall	FindResource,<si,ResourceID,ResourceType>
	cCall	LoadResource,<si,ax>	  	       ;ax is handle to resource
	cCall	LockResource,<ax>		       ;ax is handle to mem.

; Now dx:ax --> string resource.

	mov	ds,dx
	mov	dx,ax
	mov	ax,0900h
	int	21h
	ret
szDisplay	db	'DISPLAY',0
szRCDATA	db	'STRINGS',0
BootErrorMsg	endp

sEnd	InitSeg
if	MASMFLAGS and PUBDEFS
	public	inquire_gdi_info
	public	exit_enable
endif
end
