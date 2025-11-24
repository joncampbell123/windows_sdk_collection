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
; Module Name:	CONTROL.ASM
;
; This module contains the escape handler for the color display
; drivers.
;
; It also contains stubs for the SetAttribute function and the
; DeviceBitmap function, both of which this driver does not
; support.
;
; Exported Functions:	Control
;
; Public Functions:	none
;
; Public Data:		none
;
; General Description:
;
;   Control is the routine which is called when an Escape call
;   is made to GDI.  This driver only implements two of the
;   escape functions.  These functions set and get the color
;   table.  Since the color table is not setable for the display,
;   the current value is just returned.
;
;   Support for QUERYESCSUPPORT has now been added.  This escape
;   function is required of all drivers.  It informs the caller
;   which escape functions are supported (QUERYESCSUPPORT will
;   return TRUE for QUERYESCSUPPORT!).
;
; Restrictions:
;
;-----------------------------------------------------------------------;


incControl	=	1		;Include control for gdidefs.inc

	.xlist
	include cmacros.inc
	include gdidefs.inc
	include macros.inc
	include	mflags.inc
	include	cursor.inc
	.list


	??_out	Control
	externFP exclude_far	
	externFP unexclude_far
sBegin	Data
	externW		Qsize
	externW		Qhead
	externW		Qtail
wMouseTrails	dw	?
public	wMouseTrails
externB	CURSOR_STATUS
sEnd	Data

	externA COLOR_TBL_SIZE		;# entries in the color table
        externFP GetProfileInt   	;Kernel!GetProfileInt
        externFP WriteProfileString 	;Kernel!WriteProfileString

createSeg _BLUEMOON,BlueMoonSeg,word,public,CODE
sBegin	BlueMoonSeg
assumes cs,BlueMoonSeg

	externD BlueMoonSeg_color_table ;Color table values

szwindows	db	'windows',0
szMouseTrails	db	'MouseTrails',0

page
;--------------------------Exported-Routine-----------------------------;
; Control
;
;   Control is defined so that device specific commands can be issued
;   that are not supported by GDI.  Some of the Control functions have
;   been defined by GDI so that devices that can perform them (and need
;   to) can do so, such as clear device.
;
;   Clear device will not be allowed.  Nobody should do that to the
;   system console!
;
;   Set/Get color table will be implemented.  GetColorTable will
;   return the color in the table for the index passed.
;
;   SetColorTable should return the color that was actually set
;   into the table.  Since this driver doesn't allow setting the
;   color table, the Set function is the same as the Get function.
;
; Entry:
;	None
; Returns:
;	AX = 1 if success
; Error Returns:
;	AX = 0 if error
; Registers Preserved:
;	SI,DI,DS,BP
; Registers Destroyed:
;	AX,BX,CX,DX,ES,FLAGS
; Calls:
;	None
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


assumes ds,Data
assumes es,nothing

cProc	Control,<FAR,PUBLIC,WIN,PASCAL>,<si,di>

	parmD	lp_device
	parmW	function
	parmD	lp_in_data
	parmD	lp_out_data

	localV	bString,%(4)			;4 byte string.
	localB	bFlushProfile

        org     $+1
_cstods label   word
        org     $-1

cBegin
	xor	ax,ax			;Assume error
	lds	di,lp_in_data		;Get pointer to the data
	assumes ds,nothing
	mov	bx,function
	cmp	bx,MOUSETRAILS
	je	alter_mouse_trails
	cmp	bx,SETCOLORTABLE	;Process set/get color table
	je	is_color_table
	cmp	bx,GETCOLORTABLE
	je	is_color_table
	cmp	bx,QUERYESCSUPPORT
	jne	exit_control		;Is not set, get, query
	mov	bx,wptr [di]		;Get queried escape function number
	cmp	bx,MOUSETRAILS
	je	exit_num_mouse_images
	cmp	bx,GETCOLORTABLE
	je	exit_control_inc_ax
	cmp	bx,QUERYESCSUPPORT
	je	exit_control_inc_ax
	jmp	short exit_control	;We don't support it

is_color_table:
	mov	si,wptr [di]		;Get color table index
	cmp	si,COLOR_TBL_SIZE	;Legal index?
	jae	exit_control		;Illegal index, return error
	shl	si,1			;Compute color table address
	shl	si,1
	add	si,BlueMoonSegOFFSET BlueMoonSeg_color_table
	les	di,lp_out_data		;--> where value is returned
	assumes es,nothing

	cld
	movs	wptr es:[di],wptr cs:[si]
	movs	wptr es:[di],wptr cs:[si]

exit_control_inc_ax:
	inc	ax			;Return 1 to show OK

exit_control:

cEnd

;--------------------------------------------------------
; MouseTrails escape support.
;--------------------------------------------------------
IS_EXCLUDED	equ 000000001b		;Cursor is excluded.
IS_BUSY      	equ 000000010b		;Critical section.
IS_NULL	    	equ 000000100b		;Cursor is null.
HAS_MOVED	equ 000001000b		;Cursor has moved.
IS_VISIBLE   	equ 000010000b		;Cursor is visible.

exit_num_mouse_images:
	mov	ds,_cstods	
	assumes	ds,Data
	mov	ax,wMouseTrails		
	or	ax,ax			;If wMouseTrails is neg.
	jl	short @f		;just return that value.
	mov	ax,Qsize		;Otherwise, return the value
@@:	jmp	exit_control

alter_mouse_trails:
	mov	bx,wptr [di]		;Get command argument.
	mov	ds,_cstods	
	assumes	ds,Data

; Special hack for PenWindows -- they want to quickly disable MouseTrails
; without causing win.ini to be written to.  So if bx = -2, we behave the
; same as if it was 0 (disable) except that we don't write to win.ini.  
; If bx = -3, we behave the same as -1 (enable) except we don't write to
; win.ini either.

	mov	ax,wMouseTrails		;ax = cached wMouseTrails
	add	bx,2			;-2 => 0  -3 => -1
	mov	bFlushProfile,bl        ;bFlushProfile > 0 ? Write to win.ini
	jle	short @f
	sub	bx,2
@@:	or	bx,bx			;bx = escape command.
	jl	enable_mouse_trails	;If neg, enable mouse trails.
	jg	short set_mouse_trails	;if positive, set the # of trails.
	jmp	short disable_mouse_trails;If 0, disable mouse trails.

set_mouse_trails:
	mov	ax,bx
	cmp	ax,7
	jle	short @f
	mov	ax,7
@@:	mov	wMouseTrails,ax
	call	set_Qsize
	jmp	short write_profile

enable_mouse_trails:
	or	ax,ax			;if wMouseTrails was zero, that
	jnz	short @f		;means it wasn't present in win.ini.
	mov	ax,7			;Initialize it to 7 in this case.
	mov	wMouseTrails,ax
	call	set_Qsize
	jmp	short write_profile
@@:	jl	short @f		;If its positive, then
	jmp	exit_control		;nothing to do.
@@:	cwd				;Its negative.  Take the absolute 
	xor	ax,dx			;value and update Qsize 
	sub	ax,dx			;and wMouseTrails.
	mov	wMouseTrails,ax
	call	set_Qsize
	jmp	short write_profile	

disable_mouse_trails:
	or	ax,ax
	jg	short @f		;If already negative, nothing to do.
	jmp	exit_control
@@:	neg	ax
	mov	wMouseTrails,ax		;update cache variable.
	mov	ax,1
	call	set_Qsize
	mov	ax,wMouseTrails

write_profile:			;Updates win.ini with ax, and returns ax.
	cmp	bFlushProfile,0 ;bFlushProfile > 0 ? write to win.ini : exit
	jle	short write_profile_exit
	mov	bx,ax		;Save ax in bx.
	cwd			;Absolute value with this nifty trick.
	xor	ax,dx
	sub	ax,dx
	add	ax,48		;convert to ascii. Number '0' is 45.
	shl	ax,8		;put in high byte.
	mov	al,32		;put a blank character as the 1st char.
	or	bx,bx		;Are we negative?
	jg	short @f	;no.
	mov	al,45		;yes. Put in ascii negative sign. '-' is 48.
@@:	mov	word ptr bString,ax   ;Build output string.
	mov	byte ptr bString+2,0  ;Make sure it's NULL terminated.
	lea	ax,bString
	mov	si,BlueMoonSegOFFSET szwindows	
	mov	di,BlueMoonSegOFFSET szMouseTrails
	push	bx
	cCall	WriteProfileString, <cs, si, cs, di, ss, ax> ;write it.
	pop	ax		;Get original number back.
write_profile_exit:
	jmp	exit_control	;return number of images in ax.

set_Qsize	proc	near
	mov	cx,0			;left
	mov	dx,0			;top
	mov	si,SCREEN_WIDTH		;right
	mov	di,SCREEN_HEIGHT	;bottom
	push	ax
	call	exclude_far
	pop	ax
	mov	cl,IS_BUSY		;set draw busy and get current
	xchg	cl,CURSOR_STATUS	;status into CL	
	xor	bx,bx
	mov	Qhead,bx
	mov	Qtail,bx
	mov	Qsize,ax
	xchg	cl,CURSOR_STATUS	;put old status back.
	call	unexclude_far
	ret
set_Qsize	endp

	
cProc DeviceBitmap,<FAR,PUBLIC,WIN,PASCAL>,<si,di>

      parmD lpDevice
      parmW command
      parmD lpBitmap
      parmD lpBits

cBegin	<nogen>

	xor	ax,ax
	ret	14

cEnd	<nogen>




cProc SetAttribute,<FAR,PUBLIC,WIN,PASCAL>,<si,di>

      parmD  lpDevice
      parmW  stateNum
      parmW  index
      parmD  attribute

cBegin	<nogen>

	xor	ax,ax
	ret	12

cEnd	<nogen>

sEnd	BlueMoonSeg
if	MASMFLAGS and PUBDEFS
	public	is_color_table
	public	exit_control
endif
end

