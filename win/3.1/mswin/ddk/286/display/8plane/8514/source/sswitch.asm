	page	,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	SSWITCH.ASM
;
;   This module contains the functions:
;
;
; Created: 16-Sep-1987
; Author:  Bob Grudem [bobgru]
; Modified for the 8514:  Fredric J. Einstein [fredei]
;
; Copyright (c) 1984-1987 Microsoft Corporation
;
; Exported Functions:	none
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


	.286c

	.xlist
	include cmacros.inc
        include macros.mac
        include 8514.inc
        include oemres.inc
	include	debug.inc
	.list

	externFP ScreenSwitchEnable	;Imported from KEYBOARD.DRV
	externFP GetModuleHandle
	externFP GetProcAddress
	externFP AllocCSToDSAlias	;Imported from KERNEL
	externFP AllocSelector
	externFP GlobalWire
	externFP GlobalFix
	externFP GetPrivateProfileInt        ;imported from KERNEL (charlkin)

SCREEN_SWITCH_OUT equ	4001h		;Moving 3xBox to background
SCREEN_SWITCH_IN  equ	4002h		;Moving 3xBox to foreground
DOS_VERSION	  equ	0310h		;Earliest DOS we must support (03.10)
HOT_KEY_VERSION   equ	1000h		;Version with hot key support (10.00)


INT_MULT	equ	2Fh		;Multiplexed interrupt number


sBegin	Data

pre_switch	label	word
		dw	pre_switch_to_background
                dw      pre_switch_to_foreground

switch_proc     label   word
                dw      switch_to_background
                dw      switch_to_foreground

post_switch	label	word
		dw	post_switch_to_background
		dw	post_switch_to_foreground


switch_control	db	0		;Switch control flags
PREVENT_SWITCH	equ	10000000b	;Don't allow switch (DOS 3.x, 4.x)
DO_SWITCHING	equ	01000000b	;Have to do switching
INT_2F_HOOKED	equ	00000001b	;Have hooked int 2Fh
DISABLE_HOT_KEY equ	00000010b	;Set if keyboard disabling required

FLAGS_ON_STACK	equ	4		;iret --> offset(0), seg(2), flags(4)
CARRY_FLAG	equ	00000001b


REPAINT_EXPORT_INDEX	equ	275
repaint_addr	dd	0
repaint_disable db	0h		;ok to call user to repaint
user_string	db	'USER',0
repaint_pending db	0		;repaint is pending.
externW 	TempSelector		;defined in DATA.ASM and used in BITBLT

public	f96DPI
; charlkin 4/10/91 for 96dpi (multires)
f96DPI      db      0       

sEnd	Data


sBegin	Code
assumes cs,Code

	externW		_cstods
	externFP	HardwareEnable	;in INIT.ASM

prev_int_2Fh	dd	0		;Previous int 2Fh vector
page

;----------------------------------------------------------------------------;
; UserRepaintDisable:							     ;
;								             ;
; USER calls this function to tell the display driver whether it expects     ;
; the repaint call to be postponed or not.			             ;
;								             ;
; If RepaintDisable is TRUE then the display driver should not send          ;
; repaint requests to USER immediately but wait till it is enabled again.    ;
; USER will enable repaint by seting RepaintDisable to FALSE.		     ;
;----------------------------------------------------------------------------;

cProc	UserRepaintDisable,<FAR,PASCAL,PUBLIC>

	parmB	RepaintDisable		;TRUE to Disable/FALSE to enable

cBegin

	assumes	ds,Data

	mov	al,RepaintDisable	;get the value
	mov	repaint_disable,al	;save it
	or	al,al			;being enabled again
	jnz	@f			;no.
	cmp	repaint_pending,0ffh	;pending repaint ?
	jnz	@f			;no.
	call	pre_switch_to_foreground;call repaint
	mov	repaint_pending,0	;done with pending call
@@:

cEnd
;-------------------------Interrupt-Handler-----------------------------;
; screen_switch_hook
;
; Watches calls to the OS/2 multiplex interrupt chain, and traps calls
; to the driver to save or restore the state of the display hardware
; before a context switch.
;
; If a save/restore call is recognized, then it will be dispatched
; to the device dependent handler unless PREVENT_SWITCH is
; set in switch_control.
;
;   If we're not running under OS/2, it is possible a pop-up not
;   caused by a hot key could try and grab the screen away from us .
;   In this case, we'll return with 'C' set to show that it's a real
;   bad time and try again later (they may still grab it, though).
;   In this case, PREVENT_SWITCH could be set.
;
; Entry:
;	AH = multiplex number
;	AL = function code
; Returns:
;	'C' set if screen switch cannot occur (DOS's < 10.0)
;	'C' clear otherwise
; Registers Preserved:
;	AL,BX,CX,DX,SI,DI,BP,DS,ES,SS
; Registers Destroyed:
;	AH,FLAGS
; Calls:
;
; History:
;	Sun 20-Sep-1987 23:02:58 -by-  Walt Moore [waltm]
;	Added switch_control flag,
;
;	Wed 16-Sep-1987 20:17:08 -by-  Bob Grudem [bobgru]
;	Wrote it.
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing
public  screen_switch_hook
screen_switch_hook proc far

	cmp	ax,SCREEN_SWITCH_IN
	je	screen_switch_occurring
	cmp	ax,SCREEN_SWITCH_OUT
	je	screen_switch_occurring
	jmp	prev_int_2Fh		;Not ours, pass it along

screen_switch_occurring:
	push	bp			;set up frame for altering flags
	mov	bp,sp

	push	ds
	mov	ds,[_cstods]
	assumes	ds,Data

	mov	ah,switch_control
	add	ah,ah
	jc	exit_screen_switch_error
        errnz   PREVENT_SWITCH-10000000b
                                                                      
do_screen_switch:
	pusha
	push	es
	and	ax,00000010b		;Use D1 of function to index into
                                        ;  the two word dispatch table
        xchg    ax,bx
	errnz	SCREEN_SWITCH_OUT-4001h
	errnz	SCREEN_SWITCH_IN-4002h
        call    pre_switch[bx]      
        call    switch_proc[bx]         ;call the correct switching procedure
	call	post_switch[bx]
	pop	es
	assumes es,nothing

	popa
	pop	ds
	assumes ds,nothing


;	Note that BP is still on the stack (hence FLAGS_ON_STACK+2).

	and	wptr [bp][FLAGS_ON_STACK][2],not CARRY_FLAG
	pop	bp
	iret

exit_screen_switch_error:
	pop	ds
	assumes ds,nothing

	or	wptr [bp][FLAGS_ON_STACK][2],CARRY_FLAG
	pop	bp
	iret

screen_switch_hook      endp
;
;
page
public  switch_to_foreground
switch_to_foreground    proc    near
push    bx                                      ;save our index to proper 
                                                ;procedure          
cCall   HardwareEnable                          ;go reset the hardware into
                                                ;8514 mode
pop     bx                                      ;restore saved index
ret
switch_to_foreground    endp
;
;                           
public  switch_to_background
switch_to_background    proc    near
ret                                             ;
switch_to_background    endp

page
;---------------------------Public-Routine-----------------------------;
; pre_switch_to_foreground
;
; This function is called when switching to the foreground, before
; any device-specific code has executed.
;
; Entry:
;	DS = Data
; Returns:
;	DS = Data
; Registers Preserved:
;	BX,SI,DI,BP,DS
; Registers Destroyed:
;	AX,CX,DX,ES,FLAGS
; Calls:
; History:
;	Mon 05-Oct-1987 20:13:46 -by-  Walt Moore [waltm]
;	Moved repaint address fetch to hook_int_2F
;
;	Mon 05-Oct-1987 20:17:08 -by-  Bob Grudem [bobgru]
;	Wrote it.
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

pre_switch_to_foreground        proc    near
        push    bx                      ;Save dispatch table index
	mov	ax,wptr repaint_addr[0] ;We expect to always have the
	or	ax,wptr repaint_addr[2] ;  address, but let's be sure
	jz	pre_switch_to_fore_exit
	cmp	repaint_disable,0	;is repaint enabled ?
	jz	@f			;yes.
	mov	repaint_pending,0ffh	;repaint pending
	jmp	short pre_switch_to_fore_exit
@@:
	call	repaint_addr		;Force repaint of all windows

pre_switch_to_fore_exit:
	pop	bx
	ret

pre_switch_to_foreground	endp


;---------------------------Public-Routine-----------------------------;
; pre_switch_to_background
;
; This function is called when switching to the background, before
; any device-specific code has executed.
;
; Entry:
;	DS = Data
; Returns:
;	DS = Data
; Registers Preserved:
;	AH,BX,CX,DX,SI,DI,BP,DS,ES,FLAGS
; Registers Destroyed:
;	AL
; Calls:
; History:
;	Mon 05-Oct-1987 20:17:08 -by-  Bob Grudem [bobgru]
;	Wrote it.
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

pre_switch_to_background	proc	near

	ret

pre_switch_to_background	endp


;---------------------------Public-Routine-----------------------------;
; post_switch_to_foreground
;
; This function is called when switching to the foreground, after
; any device-specific code has executed.
;
; Entry:
;	DS = Data
; Returns:
;	DS = Data
; Registers Preserved:
;	AH,BX,CX,DX,SI,DI,BP,DS,ES,FLAGS
; Registers Destroyed:
;	AL
; Calls:
; History:
;	Mon 05-Oct-1987 20:17:08 -by-  Bob Grudem [bobgru]
;	Wrote it.
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

post_switch_to_foreground	proc	near

	ret

post_switch_to_foreground	endp


;---------------------------Public-Routine-----------------------------;
; post_switch_to_background
;
; This function is called when switching to the background, after
; any device-specific code has executed.
;
; Entry:
;	DS = Data
; Returns:
;	DS = Data
; Registers Preserved:
;	AX,BX,CX,DX,SI,DI,BP,DS,ES,FLAGS
; Registers Destroyed:
;	None
; Calls:
; History:
;	Mon 05-Oct-1987 20:17:08 -by-  Bob Grudem [bobgru]
;	Wrote it.
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

post_switch_to_background	proc	near

	ret

post_switch_to_background	endp


sEnd	Code
page

createSeg _INIT,InitSeg,word,public,CODE
sBegin	InitSeg
assumes cs,InitSeg

szSection db    '8514.DRV', 00H
szKey     db    'dpi', 00H
szProfile db    'SYSTEM.INI', 00H

;---------------------------Public-Routine-----------------------------;
; hook_int_2Fh
;
; Installs a link in the 2Fh multiplex interrupt chain to watch for
; calls to the driver to save or restore the state of the display
; hardware before a context switch.
;
; This function is called whenever the driver recieves an enable call.
;
; Entry:
;	DS = Data
; Returns:
;	DS = Data
; Registers Preserved:
;	BX,CX,DX,SI,DI,BP,DS,ES
; Registers Destroyed:
;	AX,flags
; Calls:
;	none
; History:
;	Mon 05-Oct-1987 20:13:46 -by-  Walt Moore [waltm]
;	Moved getting the repaint procedure address to this
;	routine.
;
;	Sun 20-Sep-1987 19:00:13 -by-  Walt Moore [waltm]
;	Added addressibility to the Code segment where stuff
;	is stored.  Added switch_control flag.
;
;	Wed 16-Sep-1987 20:17:08 -by-  Bob Grudem [bobgru]
;	Wrote it.
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

		public	hook_int_2Fh
hook_int_2Fh	proc	near

	push	ds

;	See if we already have repaint_addr.
	mov	ax,wptr [repaint_addr][0]
	or	ax,wptr [repaint_addr][2]
	jnz	hook_int_2f_repaint_addr

;	Need module handle to pass to GetProcAddress.
	mov	ax,DataOFFSET user_string
	farPtr	module_name,ds,ax
	cCall	GetModuleHandle,<module_name>
	
;	Get the value of repaint_addr.
	xchg	ax,bx
	mov	ax,REPAINT_EXPORT_INDEX
	cwd
	farPtr	func_number,dx,ax
	cCall	GetProcAddress,<bx,func_number>
	mov	wptr [repaint_addr][0],ax
	mov	wptr [repaint_addr][2],dx

hook_int_2f_repaint_addr:

	mov	al,switch_control	;Only hook if we have the correct DOS
	xor	al,INT_2F_HOOKED
	test	al,DO_SWITCHING+INT_2F_HOOKED
	jz	hook_int_done		;Don't need to hook it


	cli	
	or	switch_control,INT_2F_HOOKED
;	 xor	 ax,ax

	push	ds
	mov	ax, CodeBASE
	cCall	AllocCSToDSAlias, <ax>
	mov	ds,ax
	assumes ds,Code

	mov	ax, 3500h+INT_MULT	; get old vector and keep it in a save
	int	21h			; place
	mov	wptr prev_int_2Fh[2], es
	mov	wptr prev_int_2Fh[0], bx

	mov	ax, CodeBASE
	mov	dx, CodeOFFSET screen_switch_hook
	mov	ds, ax
	mov	ax, 2500h+INT_MULT
	int	21h

	pop	ds
	assumes ds, Data
	sti
if 0
	mov	ax,CodeBASE		;Code is a static segment, so it
	mov	es,ax			;  won't be moving on us
	assumes es,Code

	xchg	ax,wptr ds:[INT_MULT*4][2]
	mov	wptr prev_int_2Fh[2],ax
	mov	ax,CodeOFFSET screen_switch_hook
	xchg	ax,wptr ds:[INT_MULT*4][0]
	mov	wptr prev_int_2Fh[0],ax
	sti
endif
hook_int_done:
	pop	ds
	assumes ds,nothing
	ret

hook_int_2Fh	endp
page

;---------------------------Public-Routine-----------------------------;
; restore_int_2Fh
;
; If we installed ourselves into int 2Fh, we'll restore the previous
; vector.
;
; This function is called whenever the driver receives a disable call.
;
; Entry:
;	ES = Data
; Returns:
;	ES = Data
; Registers Preserved:
;	BX,CX,DX,SI,DI,BP,DS
; Registers Destroyed:
;	AX,ES,flags
; Calls:
;	none
; History:
;	Sun 20-Sep-1987 19:00:13 -by-  Walt Moore [waltm]
;	Added addressibility to the Code segment where stuff
;	is stored.  Added switch_control flag.
;
;	Wed 16-Sep-1987 20:17:08 -by-  Bob Grudem [bobgru]
;	Wrote it.
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

		public	restore_int_2Fh

restore_int_2Fh proc    near
;
;ES & DS are destroyed by this proc.
;
        assumes ds,Data
        assumes es,nothing


	test	switch_control,INT_2F_HOOKED
	jz	restore_done
	cli	
	and	switch_control,not INT_2F_HOOKED

	mov	ax, CodeBASE
	cCall	AllocCSToDSAlias, <ax>
	mov	es, ax
	assumes es, Code

	push	ds
	lds	dx, prev_int_2Fh	; previous vector was stored in Code
	mov	ax, 2500h+INT_MULT
	int	21h
	pop	ds
if 0
	xor	ax,ax
	mov	ds,ax
	assumes ds,nothing

	mov	ax,CodeBASE		;Code is a static segment, so it
	mov	es,ax			;  won't be moving on us
	assumes es,Code

	mov	ax,wptr prev_int_2Fh[0]
	xchg	wptr ds:[INT_MULT*4][0],ax
	mov	ax,wptr prev_int_2Fh[2]
	xchg	wptr ds:[INT_MULT*4][2],ax
endif
	sti

restore_done:
	ret

restore_int_2Fh endp
page

;---------------------------Public-Routine-----------------------------;
; driver_initialization
;
; The very first thing to do when the 8514 display driver is loaded is
; to lock down its data segment.  The data segment is marked as moveable,
; but in reality it should be fixed.  It is marked moveable to make
; far calls from one discardable code segment into another discardable
; segment.  If the data segment were fixed such a call would fail in low
; memory situations (real mode only) as kernel would try to discard the
; segment of the calling function but gets all confused when it sees a
; fixed DS and causes the system to curl up and die.
;
; Windows display driver initialization.  All display drivers which
; require special support for screen group switching will have this
; as their load time entry point.  This function will perform its
; initialization, then call the device specific initialization code.
;
; The DOS version number is checked, and the internal flags for
; screen group switching are set.
;
; Entry:
;	CX = size of heap
;	DI = module handle
;	DS = automatic data segment
;	ES:SI = address of command line (not used)
; Returns:
;	DS = Data
; Registers Preserved:
;	SI,DI,BP,DS
; Registers Destroyed:
;	AX,BX,CX,DX,ES,FLAGS
; Calls:
;	none
; History:
;	Sun 20-Sep-1987 19:00:13 -by-  Walt Moore [waltm]
;	Wrote it.
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

cProc   driver_initialization,<FAR,PUBLIC>,<si,di>

cBegin
	cCall	GlobalWire, <ds>	;force data segment in low mem
	cCall	GlobalFix, <ds> 	;and make sure it doesn't move
	
	push	0			;pass it a null
	cCall	AllocSelector		; get us a free selector for misc.
	mov	TempSelector, ax	; use.

	mov	ah,30h			;Check DOS version number
        int     21h
	xchg	al,ah			;Correct order for comparing
	xor	bl,bl			;Accumulate flags here
        cmp     ax,DOS_VERSION          ;Earliest DOS we must support (3.10)
                                        ;10 or higher means OS/2
	jb	dont_support_switching
	or	bl,DO_SWITCHING 	;Have to handle screen group switches
	cmp	ax,HOT_KEY_VERSION
	jb	save_switch_control
	or	bl,DISABLE_HOT_KEY	;Can disable hot key

save_switch_control:
	mov	switch_control,bl

dont_support_switching:      

; (charlkin 4/10/91 multires)
	push	ax
        cCall    SetMultiResSetting      ;Determine 120 dpi or 96 dpi
	pop	ax
cEnd

;**************************************************************
;*  cProc   GetMultiResSetting,<NEAR,PUBLIC>
;*
;*  Description: 
;*
;*   This function returns in AX the multi-res setting defined
;*   in the system.ini file.  Currently the driver supports the
;*   the following multi-res settings:
;*
;*      MULTI_120DPI    equ     0
;*      MULTI_96DPI     equ     1
;*
;*   These are defined in 8514.INC.
;*
;*  Psuedo Code:
;*
;*   WORD FAR PASCAL GetMultiResSetting( void )
;*   {
;*      switch (GetPrivateProfileInt( "8514.DRV", "dpi", 120, "SYSTEM.INI" ))
;*      {
;*         case 96:
;*            return MULTIRES_96DPI ;
;*   
;*         case 120:
;*         default:
;*            return MULTIRES_120DPI ;
;*      }
;*   }   
;*
;*
;*  Comments:
;*
;****************************************************************
cProc   SetMultiResSetting,<NEAR,PUBLIC>
cBegin
	farPtr	lpszSection,cs,si
	farPtr	lpszKey,cs,di
	farPtr	lpszProfile,cs,ax
        mov     si,InitSegOFFSET szSection    ;Section string ("8514.DRV")
        mov     di,InitSegOFFSET szKey        ;key string ("dpi")
        mov     ax,InitSegOFFSET  szProfile   ;Profile string ("system.ini")
        cCall   GetPrivateProfileInt, <lpszSection,lpszKey,120,lpszProfile>
        sub     ax,96         		      ;Was 96 returned?
        jne     DefaultDPI             	      ; nope...
        mov     ax, MULTIRES_96DPI            ;We're using 96dpi!
        jmp     EndMultiRes
DefaultDPI:
        xor     ax,ax                         ;We're using 120dpi!
EndMultiRes:
        mov     BYTE PTR f96DPI, al
cEnd SetMultiResSetting

;****************************************************************
;*  cProc   GetDriverResourceID,<FAR,PUBLIC,WIN,PASCAL>,<si,di>
;*
;*  Description: 
;*
;*           DWORD  GetDriverResourceID(iResId, lpResType)
;*
;*           This function will be called by Windows before an
;*           icon, cursor, bitmaps, or OEMBIN resources are
;*           loaded from the display driver. This gives the
;*           display driver an opportunity to map the given
;*           resource id onto another resource id and return
;*           it.
;*
;*           Parameter 
;*                    Type/Description
;*
;*           iResId    
;*                    INT    Id of the requested resource
;*
;*           lpResType          
;*                    LPSTR  Pointer to the
;*                           requested resource type; If the HIWORD
;*                           of this parameter is NULL, then the
;*                           LOWORD contains the resource type id.
;*
;*       Return Value
;*           The return value indicates a resource id
;*           which is the result of the mapping done by this
;*           function.  The HIWORD should be 0?
;*
;*       Comments
;*           This function must be exported by the
;*           driver with an ordinal number of 450.
;*
;*           If the function decides not to do any mapping, it
;*           must return the value passed in through "iResId"
;*           parameter.
;*
;*           The type of the resource returned by this function
;*           is the same as the type of the resource passed in
;*           to this function.  That is, this function should
;*           not attempt to map a resource of one type onto a
;*           resource of another type.
;*
;****************************************************************
cProc   GetDriverResourceID,<FAR,PUBLIC,WIN,PASCAL>
        parmW  iResID
        parmD  lpResType
cBegin
	mov	ax,iResID		;Get res id into ax.
	cmp	ax,1
	je	short GDR_MaybeLoad4PlaneColors

;-----------------------------------------------------------------
; if f96DPI is not set to MULTIRES_96DPI then do not map
; (use 120 dpi resources).
;-----------------------------------------------------------------
GDR_CheckFor96DPI:
        cmp     byte ptr f96DPI, MULTIRES_96DPI
        jne     short GDR_Exit

;-----------------------------------------------------------------
; if the ID identifies an icon or cursor then do not map it. 
; Otherwise, map it by subtracting 2000 from it.  If the ID
; is a 1 or 3, add 2000 to it.
;-----------------------------------------------------------------
	mov	dx,2000
        cmp     ax, OCR_ICOCUR          ;ICOCUR is the "largest" icon/cursor
	jg	short GDR_MapIt		;ID.  If the ID is bigger than this
        cmp	ax,1                    ;it must be a bitmap
	je	short GDR_Mapit
        cmp     ax,3
	jne	short GDR_Exit
GDR_MapIt:
        add     ax,dx                   ;96 dpi resource IDs are +2000
GDR_Exit:
	xor	dx,dx		        ;dx must be zero.                
cEnd GetDriverResourceID

GDR_MaybeLoad4PlaneColors:
	push	ax
	mov	dx,42e8h
	in	ax,dx
if DEBUG_4PLANE
	mov	ax,0
endif
	and	ax,80h			;NZ if 8 plane mode
	pop	ax
	jnz	short GDR_CheckFor96DPI
	mov	ax,2100			;Assume 120 dpi.
        cmp     byte ptr f96DPI, MULTIRES_96DPI
        jne     short GDR_Exit
	inc	ax			;2101 is config.asm for 96dpi 4plane.
	jmp	short GDR_Exit


sEnd    InitSeg
end	driver_initialization
