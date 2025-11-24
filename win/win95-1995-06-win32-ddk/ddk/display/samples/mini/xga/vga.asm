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
; VGA.ASM for the XGA
;----------------------------------------------------------------------------
        .xlist
        include cmacros.inc
        incDevice = 1                   ;Include control for gdidefs.inc
        incDrawMode = 1                 ;Include DRAWMODE structure
        include gdidefs.inc
        include minivdd.inc
	include macros.inc
        include dibeng.inc
	include valmode.inc
	include xga.inc
	.list
;
;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
GET386API       equ     1684h           ;Get API entry point from VxD.
VFLATD_VXD      equ     11Fh            ;id of VFLATD VxD
GET_VIDEO_SEL   equ     1               ;function # to call to get selector
STOP_IO_TRAP    equ     4000h           ; stop io trapping
START_IO_TRAP	equ	4007h		; re-start io trapping


;----------------------------------------------------------------------------
; E X T E R N S  and  P U B L I C S
;----------------------------------------------------------------------------
	externFP	SetRAMDAC_far		;in PALETTE.ASM
	externFP	AllocSelector		;in KERNEL.EXE
	externFP	FreeSelector		;in KERNEL.EXE
	externFP	PrestoChangoSelector	;in KERNEL.EXE
	externFP	GetPrivateProfileInt	;in KERNEL.EXE

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin  Data
externW wResolution			;in INIT.ASM
externW wBpp				;in INIT.ASM
externD lpColorTable			;in ENABLE.ASM
externD lpDriverPDevice 		;in ENABLE.ASM
externD VDDEntryPoint			;in INIT.ASM
externW OurVMHandle			;in INIT.ASM

public	OffScreenStartAddr
OffScreenStartAddr		dd	?
				dw	?
public	CursorStartAddr
CursorStartAddr 		dd	?
				dw	?

globalD XGAPixmapBaseAddr,0
globalD ColorFormatMask,0		;FFH if 8 BPP, FFFFH if 16 BPP

globalW wCurrentMode,0
globalW DeviceFlags,VRAM+MINIDRIVER+BANKEDVRAM
globalD dwVideoMemorySize,0
globalW wScreenWidth,0
globalW wScreenHeight,0
globalW wMaxWidth,0
globalW wMaxHeight,0
globalW wScreenWidthBytes,0
;
globalW NbrLinesOffScreen,0
globalW OffScreenStartLine,0
globalW XGAIOBaseAddr,0
globalW MemAccessReg,0
globalW CRTCRegisterBase,0
globalD XGARegs,0
globalW wChipId,0			;set to 1 if XGA has been found
globalW ScreenSelector,0		;selector for the screen
globalB WindowsEnabledFlag,1		;zero if Windows owns the screen
globalB MemAccessColorValue,0
globalB AGXChipID,0
sEnd	Data
;
;
subttl			Mode Set Tables
page +
createSeg _INIT,InitSeg,word,public,CODE
sBegin		InitSeg
assumes 	cs,InitSeg
;
;
externNP	XYtoRes 			;in INIT.ASM
externB 	szSection			;in INIT.ASM
externB 	szSystem			;in INIT.ASM
;
;
MonitorTypeString		db	'MONITOR-TYPE',0
;
;
subttl		Entry Point to Device Specific Hardware Enable
page +
cProc		PhysicalEnableFar,<FAR,PUBLIC>
;
cBegin		<nogen>
	cCall	PhysicalEnable
	ret
cEnd		<nogen>
;
;
cProc		PhysicalEnable,<NEAR,PUBLIC>
;
	localW	WriteableInitSelector
	localD	VFlatDEntryPoint
;
cBegin
.386
	assumes ds,Data
	cmp	wChipId,0		;have we identified the XGA yet?
	jne	PEGetMemSize		;yes! no need to do it again
;
;We call down to the XGA MiniVDD which has already identified us as an XGA
;or AGX chipset.  This call will return the chipset ID (XGA1_ID, XGA2_ID,
;or AGX_ID), the base I/O register, and the pixmap base address:
;
	mov	eax,VDD_REGISTER_DISPLAY_DRIVER_INFO
	movzx	ebx,OurVMHandle 	;(needed for call)
	xor	cl,cl			;this is function -- return XGA ID info
	call	dword ptr VDDEntryPoint ;call the VDD
	cmp	eax,VDD_REGISTER_DISPLAY_DRIVER_INFO
					;was call a success?
	je	PEFail			;nope, MiniVDD didn't handle the call!
;
;At this point:
;	EAX contains the physical address of the aperture.
;	DX contains the I/O base address (21x0).
;	DI contains the chipset ID (XGA1_ID, XGA2_ID, or AGX_ID).
;
	mov	wChipId,di		;DI contains the XGA or AGX model
	mov	XGAIOBaseAddr,dx	;
	mov	XGAPixmapBaseAddr,eax	;
;
;Get VFlatD's API Entry Point so we can call it to get our screen selector:
;
	mov	ax,GET386API		;Int 2FH: GET DEVICE API ENTRY POINT
	mov	bx,VFLATD_VXD		;this is VxD ID for VFlatD
	int	2fh			;get the API Entry Point
	mov	ax,es			;get this for success check
	or	ax,di			;is VFlatD installed in the system?
	jz	PEFail			;oops, better leave!  No VFlatD!!!
	mov	seg_VFlatDEntryPoint,es ;
	mov	off_VFlatDEntryPoint,di ;
;
;Since the XGA can use different values for its banking registers depending
;on configuration, we must compile in the correct banking register value
;into our banking template that we'll be sending to VFlatD.
;
;First, create a Writeable selector for the InitSeg:
;
	arg	0			;func code: Alloc a new selector
	cCall	AllocSelector		;returns a selector in AX
	mov	WriteableInitSelector,ax;
	mov	bx,_INIT		;now alias it to InitSeg
	push	bx			;
	push	ax			;
	cCall	PrestoChangoSelector	;
;
;Now, get the proper value for the banking register and set it into the;
;banking template.
;
	mov	ax,XGAIOBaseAddr	;get base register determined earlier
	and	ax,00f0h		;mask to get only changeable nibble
	or	ax,2108h		;now AX has banking register to use
	mov	es,WriteableInitSelector;ES --> writeable Init
	mov	es:TemplateBankReg,ax	;set the proper value into the template
;
;Now, deallocate the WriteableInitSelector since we don't need it any more:
;
	arg	WriteableInitSelector	;this is selector to free
	cCall	FreeSelector		;
;
;Now we're ready to call VFlatD to get our screen selector returned in AX:
;
	mov	dx,GET_VIDEO_SEL	;this is function to call in VFlatD
	mov	ax,1024 		;this is number of pages needed
	mov	cx,cs			;make ES:DI --> bank switch template
	mov	es,cx			;
	mov	di,CodeOFFSET XGABankSwitchTemplate
	mov	cx,XGABankSwitchTemplateSize
	call	VFlatDEntryPoint	;go get the selector from VFlatD
	or	ax,ax			;did we fail to get a screen selector?
	jz	PEFail			;yes, exit in shame
	mov	ScreenSelector,ax	;we've got one!  save it!
;
public	PEGetMemSize
PEGetMemSize:
;
;We call a special function in our MiniVDD to get the memory size of the
;XGA that we're running on.  This will also put us into XGA HiRes mode.
;We call the MiniVDD instead of doing it ourselves in order to preserve
;the integrety of our paging system in the Main VDD.  If we write to
;Video RAM at this time (necessary for getting memory size on the XGA),
;we'll cause page faults which is something we don't want to do until we're
;all enabled.  The VDD operates at ring 0 so it can touch physical memory
;all it wants.
;
;In addition, the VDD's REGISTER_DISPLAY_DRIVER_INFO call will return
;to us a selector which points to our memory mapped co-processor registers.
;The VDD obtains this for us because it allocates a selector which points
;to the actual PHYSICAL address of the co-processor registers and not
;a linear address such as the _C000H selector provided by Kernel does.
;Therefore, we're immune to EMM386 allocating the memory mapped area
;as UMB space.
;
	mov	eax,VDD_REGISTER_DISPLAY_DRIVER_INFO
	movzx	ebx,OurVMHandle 	;(needed for call)
	mov	cl,1			;this is subfunction code
	call	dword ptr VDDEntryPoint ;call the VDD
	cmp	eax,VDD_REGISTER_DISPLAY_DRIVER_INFO
					;was call a success?
	je	PEFail			;nope, fail initialization!
	mov	dwVideoMemorySize,eax	;save memory size returned by VDD
	or	dx,dx			;did VDD alloc our selector OK?
	jz	PEFail			;nope, fail initialization
	mov	word ptr XGARegs,0	;
	mov	word ptr XGARegs+2,dx	;DX:0 --> memory mapped registers
;
public	PEFindMode
PEFindMode:
	cCall	FindMode		;now, go get the mode to set
	mov	wCurrentMode,bx 	;BX contains VESA mode number
	or	bx,bx			;were we successful in finding a mode?
	jnz	PESetupHeightVariables	;yes, continue
;
;We must fail initialization.  We may have set the XGA into HiRes mode already
;which will result in the VGA fallback screen not being visible.  We therefore
;must call the VDD to set us back into LoRes mode.
;
	mov	eax,VDD_DRIVER_UNREGISTER
					;pass the VDD API function code in EAX
	movzx	ebx,OurVMHandle 	;we need this for call
	call	VDDEntryPoint		;
	jmp	PEFail			;couldn't find a mode to setup
;
public	PESetupHeightVariables
PESetupHeightVariables:
	mov	ax,word ptr dwVideoMemorySize
        mov     dx,word ptr dwVideoMemorySize+2
        div     wScreenWidthBytes       ;AX contains # of scan lines
	and	ax,0FFFh                ;Height >= 1000h is not allowed
	mov	wMaxHeight,ax		;
;
public	PESetupHiResMode
PESetupHiResMode:
;
;Save states for the Memory Access Mode register because we have to change it
;whenever transfering a monochrome bitmap to the screen:
;
	mov	dx,XGAIOBaseAddr	;
	and	dx,00f0h		;mask to get only changeable nibble
	or	dx,2109h		;DX --> Memory Access Mode Reg (21x9)
	mov	al,03h			;assume 8 BPP
	cmp	wBpp,8			;are we 8 BPP?
	je	@F			;yes! we've got the value
	mov	al,0ah			;assume 4 BPP (must do nibble swap)!
	cmp	wBpp,4			;are we 4 BPP?
	je	@F			;yes! we've got the value
	mov	al,04h			;we must be 16 BPP
	or	DeviceFlags,FIVE6FIVE	;set 5-6-5 colour format
@@:	mov	MemAccessReg,dx 	;
	mov	MemAccessColorValue,al	;
	inc	dx			;DX --> CRTC index register base (21xA)
	mov	CRTCRegisterBase,dx	;save this for later use
;
public	PECheckForAGX14
PECheckForAGX14:
;
;We need to differentiate between the AGX 14 and the AGX 15/16 since the
;AGX 14 cannot do hardware BLT's at 16 BPP.
;
	mov	AGXChipID,0		;make sure this is 0 for the XGA
	cmp	wChipId,AGX_ID		;running on an IIT AGX?
	jne	PESetMode		;nope, skip the following
	mov	AGXChipID,14h		;assume we're on an AGX 14
	mov	dx,216ah		;DX --> AGX indexed registers
	mov	al,6ch			;
	out	dx,al			;
	inc	dl			;DX --> AGX data register
	in	al,dx			;get the contents of AGX register 6CH
	mov	ah,al			;save the original contents
	xor	al,02h			;invert the value of bit 1
					;(this is a safe thing to do on AGX 15)
	out	dx,al			;set the new value into the register
	jmp	@F			;delay for a bit
@@:	in	al,dx			;get the value from register 6CH
	nop
	xor	al,02h			;set value back to what it was
	xchg	al,ah			;(get original value back in AL)
	out	dx,al			;set original value back to hardware
	cmp	al,ah			;did we get back what we wrote?
	jne	PESetMode		;nope, we must be on an AGX 14 or lower
	mov	AGXChipID,15h		;yes, mark that we can do hardware StrBLT
;
public	PESetMode
PESetMode:
;
;Our MiniVDD has full VESA support.  Therefore, we can simply do an INT 10H
;VESA call to set the mode and clear the screen!
;
	mov	ax,4f02h		;VESA Func Code: Set VESA HiRes Mode
	mov	bx,wCurrentMode 	;this is the mode to set!
	int	10h			;
;
public	PEHookUpWithVDD
PEHookUpWithVDD:
;
;The XGA/AGX driver uses a private flag called WindowsEnabledFlag to tell
;our drawing routines that we're either in Windows (WindowsEnabledFlag == 0)
;or in a full-screen DOS box (WindowsEnabledFlag == 1).  The VDD must know
;about this flag so it can automatically set it when we're going to full-screen
;mode.	Thus, we make a call to the VDD to register the Data segment address
;of this flag:
;
	mov	eax,VDD_SET_USER_FLAGS	;this is function code to do
	movzx	ebx,OurVMHandle 	;VDD PM API needs this
	mov	dx,_DATA		;pass address of flag in DX:CX
	mov	cx,DataOFFSET WindowsEnabledFlag
	call	dword ptr VDDEntryPoint ;
;
;The MiniVDD requires us to register ourselves with it in order to activate
;its ability to virtualize graphics mode DOS apps in a window or in the
;background.  The VDD will also allocate some memory so that it can perform
;this virtualization correctly.  Let's setup this call and pass down the
;proper information to the MiniVDD.
;
	movzx	eax,wScreenWidthBytes	;get screen pitch
	movzx	edx,wScreenHeight	;get height of visible screen
	mul	edx			;EAX has total bytes of visible screen
	mov	ecx,eax 		;pass visible memory size in ECX
	mov	eax,VDD_DRIVER_REGISTER ;this is function code for VDD PM API
	movzx	ebx,OurVMHandle 	;VDD PM API needs this
	mov	di,_TEXT		;send ES:DI --> routine that VDD calls
	mov	es,di			;to set us back into Windows mode
	mov	di,CodeOFFSET ResetHiResMode
	mov	edx,-1			;tell VDD to turn off 4 plane VGA
					;graphics in window (force full-screen)
	call	dword ptr VDDEntryPoint ;
;
;The MiniVDD will return the total memory used by both the visible screen
;and the VDD itself.  We can therefore calculate the location of the start
;of off-screen scratch memory which the driver can use for various purposes:
;
;At this point:
;	EAX contains VDD_DRIVER_REGISTER if the call failed.
;	EAX contains the size in bytes of the visible screen plus the
;	memory allocated by the VDD.
;
	cmp	eax,VDD_DRIVER_REGISTER ;did the call do anything?
	jne	PELocateOffScreenMemory ;yes, we're running with the MiniVDD
	mov	eax,ecx 		;no, get visible memory size into EAX
;
public	PELocateOffScreenMemory
PELocateOffScreenMemory:
;
;At this point:
;	EAX contains amount of video memory used by visible screen + VDD.
;
	cmp	wChipId,AGX_ID		;running on an AGX?
	jne	@F			;nope, skip the following
	add	eax, 0FFFFh
	and	eax, 0FFFF0000h		; make offscreen memory 64K boundry
	mov	CursorStartAddr, eax	; 1st 64K offscr memory for cursor
	push	eax			; calculate cursor address using formula
	mov	ebx, eax		; CurAddr = (linear addr/800h) XOR DFh
	shr	ebx, 11			; CurAddr[7:0] --> 216A:6E
	xor	bx, 0DFh		; CurAddr[9:8] --> 216A:6F[1:0]
	mov	dx, XGAIOBaseAddr
	add	dx, 0ah			; 21xA
	mov	al, 6eh
	out	dx, al
	inc	dx
	mov	al, bl
	out	dx, al
	shr	bx, 8
	and	bx, 3
	dec	dx
	mov	al, 6fh
	out	dx, al
	inc	dx
	in	al, dx
	or	al, bl
	out	dx, al
	pop	eax
	add	eax, 10000h		;second 64K is our real offscreen memory
@@:	push	eax			;save memory used for now
	mov	OffScreenStartAddr,eax	;save this for our BLTer routines
	mov	dx,ScreenSelector	;
	mov	word ptr OffScreenStartAddr+4,dx
	mov	word ptr CursorStartAddr+4,dx
	mov	edx,dwVideoMemorySize	;get total memory size
	sub	edx,eax 		;EDX == bytes of video memory left
	mov	eax,edx 		;(get this into EAX for divide)
	xor	edx,edx 		;zero EDX for dword divide
	movzx	ebx,wScreenWidthBytes	;divide total memory left by pitch
	div	ebx			;AX == height of off-screen memory area
	mov	NbrLinesOffScreen,ax	;
	pop	eax			;get back memory used returned by VDD
	xor	edx,edx 		;zero EDX for dword lengthed divide
	div	ebx			;AX == # of lines used by screen & VDD
	or	dx,dx			;any remainder?
	jz	short @f		;nope, we're OK
	inc	ax			;yes! bump up another line
@@:	mov	OffScreenStartLine,ax	;save adjusted value
;
;Call the VDD to order it to save the current register state.  This will
;hopefully assure that the Windows VM has the running state saved:
;
	mov	eax,VDD_SAVE_DRIVER_STATE
	movzx	ebx,OurVMHandle 	;
	call	dword ptr VDDEntryPoint ;
;
public	PESetupXGACoprocessor
PESetupXGACoprocessor:
;
;We setup 3 XGA PixMaps, all pointing to the XGA's video memory.  The first
;PixMap is the visible screen.	The second and third are located at the
;start of off-screen memory and are used as the "pattern" PixMap (for
;monochrome BLT's and possibly StrBLT's) and colour brush BLT pixmaps
;respectively.	So, save off the base addresses of these PixMaps so we
;can load them as we need to use them:
;
	movzx	eax,OffScreenStartLine	;get starting line off off-screen area
	movzx	edx,wScreenWidthBytes	;
	mul	edx			;now EAX has adder to off-screen PixMaps
	mov	esi,XGAPixmapBaseAddr	;we'll save off-screen addr here
	add	esi,eax 		;now ESI --> off-screen PixMap area
;
;Now, let's program the XGA so it'll recognize all three of our PixMaps.
;
;First, the visible screen:
;
	les	di,XGARegs		;ES:DI --> XGA's memory mapped regs
	mov	es:[di].PixmapIndex,1	;Pixmap 1 is the visible screen PixMap
	mov	eax,XGAPixmapBaseAddr	;this is XGA's idea of start of VRAM
	mov	es:[di].PixmapBasePointer,eax
	mov	ax,wScreenWidthBytes	;get pitch of screen as PixMap's width
	cmp	wChipId,AGX_ID		;running on an AGX?
	jne	@F			;nope, continue
	cmp	wBpp,16 		;running at 16 BPP?
	jne	@F			;nope, continue
	shr	ax,1			;divide pitch by 2 for 16 BPP
@@:	dec	ax			;XGA needs this
	mov	es:[di].PixmapWidth,ax	;
	mov	ax,wMaxHeight		;get height of screen as PixMap's height
	dec	ax			;XGA needs this
	mov	es:[di].PixmapHeight,ax ;
	mov	es:[di].PixmapFormat,3	;3 is the code for 8 BPP pixmap format
	cmp	wChipId,AGX_ID		;running on an AGX?
	jne	@F			;nope, skip the following
	cmp	wBpp,16 		;running 16 BPP?
	jne	@F			;nope, we're correctly set for 8 BPP
	mov	es:[di].PixmapFormat,4	;4 is the code for 16 BPP pixmap format
;
;Now, the colour pattern (8 BPP) pixmap:
;
@@:	mov	es:[di].PixmapIndex,2	;Pixmap 2 is the off-screen colour map
	mov	es:[di].PixmapBasePointer,esi
					;this is start of off-screen VRAM
	mov	ax,wScreenWidthBytes	;get pitch of screen as PixMap's width
	cmp	wChipId,AGX_ID		;running on an AGX?
	jne	@F			;nope, continue
	cmp	wBpp,16 		;running at 16 BPP?
	jne	@F			;nope, continue
	shr	ax,1			;divide pitch by 2 for 16 BPP
@@:	dec	ax			;XGA needs this
	mov	es:[di].PixmapWidth,ax	;
	mov	ax,NbrLinesOffScreen	;get height of screen as PixMap's height
	dec	ax			;XGA needs this
	mov	es:[di].PixmapHeight,ax ;
	mov	es:[di].PixmapFormat,3	;3 is the code for 8 BPP pixmap format
	cmp	wChipId,AGX_ID		;running on an AGX?
	jne	@F			;nope, skip the following
	cmp	wBpp,16 		;running 16 BPP?
	jne	@F			;nope, we're correctly set for 8 BPP
	mov	es:[di].PixmapFormat,4	;4 is the code for 16 BPP pixmap format
;
;And lastly, the monochrome pattern PixMap.  The monochrome pattern PixMap
;will be used for mono patterned brush BLT's and StrBLT's.  The ordering
;of pixels within the byte in monochrome source brushes and strings is
;the so-called Motorola ordering.  Therefore, we set this PixMap to have
;Motorola ordering:
;
@@:	mov	es:[di].PixmapIndex,3	;Pixmap 3 is the mono-pattern PixMap
	mov	es:[di].PixmapBasePointer,esi
					;this is start of off-screen VRAM
	mov	es:[di].PixmapFormat,0	;the pattern PixMap is always monochrome
;
;Set some states to the coprocessor that will be constant throughout the
;session:
;
	mov	es:[di].CoprocessorControl,0
	mov	es:[di].DestColorCompCond,04h
	mov	es:[di].CarryChainMask,0ffh
	mov	es:[di].PixelBitMask,0ffh
	mov	ColorFormatMask,0ffh	;set mask to 8 bits for BLTer stuff
	cmp	wChipId,AGX_ID		;running on an AGX?
	jne	@F			;nope, skip the following
	cmp	wBpp,16 		;running 16 BPP?
	jne	@F			;nope, we're correctly set for 8 BPP
	mov	es:[di].PixelBitMask,0ffffh
					;set PixelBitMask to FFFFH for 16 BPP
	mov	ColorFormatMask,0ffffh	;set mask to 16 bits for BLTer stuff
@@:	mov	WindowsEnabledFlag,0	;we're ready for screen accesses
	clc				;indicate success
	jmp	PEExit			;and we're finished
;
public	PEFail
PEFail:
	stc				;indicate failure
;
PEExit:
.286c
cEnd
;
;
subttl		Verify Running Mode For XGA
page +
cProc		FindMode,<NEAR,PUBLIC>
;
cBegin
.386
;
;We are passed in the desired resolution and colour format from the Enable
;caller.  We need to see if the desired mode is possible on the XGA and
;possible with our memory configuration.
;
;Entry:
;	Nothing assumed.
;Exit:
;	If the mode is supported, BX contains the VESA mode number.
;	If the mode isn't supported, BX = 0.
;
;The XGA/1 can run in the following configurations:
;
;	640x480 with 8 BPP
;	640x480 with 16 BPP (not on 512K XGA's)
;	1024x768 with 4 BPP			}These are interlaced modes
;	1024x768 with 8 BPP (not on 512K XGA's) }
;
;Additionally, the XGA/2 can run the following:
;
;	800x600 with 8 BPP
;	1024x768 with 8 BPP (non-interlaced)
;	1280x1024 with 8 BPP
;
;The IIT AGX can run in the following modes in addition to those above:
;
;	800x600 with 16 BPP
;	1024x768 with 16 BPP
;
	mov	ax,wResolution
	mov	dx,wBpp
;
;See what resolution they want:
;
	cmp	ax,1			;do they want 640x480?
	je	FM640x480		;yes, go handle it!
	cmp	ax,2			;do they want 800x600?
	je	FM800x600		;yes, go handle it!
	cmp	ax,3			;do they want 1024x768?
	je	FM1024x768		;yes, go handle it!
	cmp	ax,4			;do they want 1280x1024?
	je	FM1280x1024		;yes, go handle it!
	jmp	FMErrorExit		;oops, no support for their request!
;
public	FM640x480
FM640x480:
	mov	wScreenWidth,640	;setup variables for 640x480
	mov	wScreenHeight,480	;
	mov	bx,101h 		;assume they want Mode 101H (8 BPP)
	cmp	dx,8			;do they want 8 BPP?
	jne	FM640x480x16		;nope, go try for 16 BPP
	mov	wScreenWidthBytes,640	;
	mov	wMaxWidth,640		;
	cmp	wChipId,AGX_ID		;running on an IIT AGX?
	jne	@F			;nope, pitch is setup OK
	mov	wScreenWidthBytes,1024	;yes, AGX must have power of 2 pitch
	mov	wMaxWidth,1024		;
@@:	or	DeviceFlags,BANKEDSCAN	;scans will cross banks
	jmp	FMExit			;we support it!
;
public	FM640x480x16
FM640x480x16:
	cmp	dx,16			;do they want 16 BPP?
	jne	FMErrorExit		;oops, no support for their request!
	cmp	dwVideoMemorySize,1024*1024
	jb	FMErrorExit		;can't support it on 512K configuration
	mov	bx,111h 		;set to mode 111H (16 BPP)
	mov	wScreenWidthBytes,1280	;
	mov	wMaxWidth,1280		;
	cmp	wChipId,AGX_ID		;running on an IIT AGX?
	jne	@F			;nope, pitch is setup OK
	mov	wScreenWidthBytes,2048	;yes, AGX must have power of 2 pitch
	mov	wMaxWidth,2048		;
@@:	or	DeviceFlags,BANKEDSCAN	;scans will cross banks
	jmp	FMExit			;return Mode 111H for 640x480x16
;
public	FM800x600
FM800x600:
	cmp	wChipId,XGA1_ID 	;running on an XGA/1?
	je	FMErrorExit		;yep, can't support it
	mov	wScreenWidth,800	;setup variables for 800x600
	mov	wScreenHeight,600	;
	mov	wScreenWidthBytes,800	;
	mov	wMaxWidth,800		;
	mov	bx,103h 		;set to VESA mode 103H
	cmp	wChipId,AGX_ID		;running on an IIT AGX?
	jne	@F			;nope, pitch is setup OK
	cmp	dwVideoMemorySize,1024*1024
					;do they have at least 1 meg?
	jb	FMErrorExit		;no, the AGX needs 1 meg for 800x600x8
	mov	wScreenWidthBytes,1024	;yes, AGX must have power of 2 pitch
	mov	wMaxWidth,1024		;
	cmp	dx,16			;do they want 16 BPP on the AGX?
	jne	@F			;nope, everything's setup
	cmp	dwVideoMemorySize,2*1024*1024
					;do they have 2 megs?
	jb	FMErrorExit		;nope, they can't do it!
	mov	bx,114h 		;set to VESA mode 114H
	mov	wScreenWidthBytes,1600	;set AGX pitch to 1600 bytes
	mov	wMaxWidth,1600		;
	mov	dx,8			;(so test down below will pass)
@@:	cmp	dx,8			;do they want 8 BPP?
	jne	FMErrorExit		;nope, we don't do anything else
	or	DeviceFlags,BANKEDSCAN	;scans will cross banks
	jmp	FMExit			;
;
public	FM1024x768
FM1024x768:
	mov	wScreenWidth,1024	;setup variables for 1024x768
	mov	wScreenHeight,768	;
	mov	wScreenWidthBytes,512	;
	mov	wMaxWidth,512		;
	mov	bx,104h 		;assume they want mode 104H (4 BPP)
	cmp	dx,4			;do they want 4 BPP?
	je	FMExit			;we support it!
	cmp	dwVideoMemorySize,1024*1024
	jb	FMErrorExit		;modes above 1024x768x4 need 1 meg
	mov	wScreenWidthBytes,1024	;
	mov	wMaxWidth,1024		;
	mov	bx,105h 		;set to VESA mode 105H
	cmp	dx,8			;do they want 8 BPP?
	je	FMExit			;yes, we're setup for 8 BPP
	cmp	dwVideoMemorySize,1024*1024*2
					;modes above 1024x768x8 need 2 megs
	jb	FMErrorExit		;
	cmp	wChipId,AGX_ID		;running on the IIT AGX?
	jne	FMErrorExit		;nope, XGA's don't support 1024x768x16
	cmp	dx,16			;do they want 16 BPP?
	jne	FMErrorExit		;nope, we don't support anything else
	mov	bx,117h 		;set to VESA mode 117H
	mov	wScreenWidthBytes,2048	;
	mov	wMaxWidth,2048		;
	jmp	FMExit			;
;
public	FM1280x1024
FM1280x1024:
	cmp	wChipId,AGX_ID		;running on an AGX?
	jne	FMErrorExit		;nope, can't support it
	mov	wScreenWidth,1280	;setup variables for 1280x1024x8
;
public	FM1280x1024x4
FM1280x1024x4:
	cmp	dx,4			;do they want 4 BPP?
	jne	FM1280x960x8		;
	cmp	dwVideoMemorySize,1024*1024
	jb	FMErrorExit		;not enough memory to support it!
	mov	wScreenHeight,1024	;
	mov	bx,106h 		;set to VESA mode 106H
	mov	wScreenWidthBytes,640	;
	mov	wMaxWidth,640		;
	or	DeviceFlags,BANKEDSCAN	;scans will cross banks
	jmp	FMExit			;and we're done!
;
public	FM1280x960x8
FM1280x960x8:
	cmp	dx,8			;do they want 8 BPP?
	jne	FMErrorExit		;nope, we don't do anything else
	cmp	dwVideoMemorySize,1024*1024*2
	jb	FMErrorExit		;not enough memory to support it!
	mov	wScreenHeight,960	;
	mov	bx,107h 		;set to VESA mode 107H
	mov	wScreenWidthBytes,2048	;
	mov	wMaxWidth,2048		;
	or	DeviceFlags,BANKEDSCAN	;scans will cross banks
	jmp	FMExit			;we support it!
;
public	FMErrorExit
FMErrorExit:
	xor	bx,bx
;
FMExit:
.286c
cEnd
;
;
subttl		Validate Mode Entry Point
page +
cProc		ValidateMode,<FAR,PUBLIC>
;
	parmD	lpValMode
;
cBegin
.386
;
;Entry:
;	lpValMode = DISPVALMODE structure
;Exit:
;	AX = VALMODE_YES (0), VALMODE_MAYBE (1), VALMODE_NO (2)
;
	assumes ds,Data
	push	esi			;save 32 bit versions of ESI & EDI
	push	edi			;
	push	wResolution		;save current resolution 
	push	wBpp			; and bpp.
	push	wScreenWidth		;
;
public	VMDetectXGA
VMDetectXGA:
;
;If we haven't already done so (ie: we're checking whether we can go into
;an XGA mode while running VGA 640x480x4 Plane), we must detect and
;initialize the XGA:
;
	cmp	wChipId,0		;have we identified the XGA yet?
	jne	VMGetMemorySize 	;yes! no need to do it again
;
;We call down to the XGA MiniVDD which may have already identified us as an XGA
;or AGX chipset.  This call will return the chipset ID (XGA1_ID, XGA2_ID,
;or AGX_ID), the base I/O register, and the pixmap base address:
;
	mov	eax,VDD_REGISTER_DISPLAY_DRIVER_INFO
	movzx	ebx,OurVMHandle 	;(needed for call)
	xor	cl,cl			;this is function -- return XGA ID info
	call	dword ptr VDDEntryPoint ;call the VDD
	cmp	eax,VDD_REGISTER_DISPLAY_DRIVER_INFO
					;was call a success?
	mov	ax,VALMODE_NO_UNKNOWN	;(if no MiniVDD, we return "Maybe")
	je	VMExit			;nope, MiniVDD didn't handle the call!
;
;At this point:
;	DI contains the chipset ID (XGA1_ID, XGA2_ID, or AGX_ID).
;
	cmp	di,XGA1_ID		;is it the XGA/1?
	je	@F			;yes, go save it
	cmp	di,XGA2_ID		;is it the XGA/2?
	je	@F			;yes, go save it
	cmp	di,AGX_ID		;is it the AGX?
	jne	VMExit			;nope, return "Maybe"
@@:	mov	wChipId,di		;DI contains the XGA model (XGA/1 or 2)
;
public	VMGetMemorySize
VMGetMemorySize:
;
;Now, get the memory size if we don't have it already:
;
	cmp	dwVideoMemorySize,0	;do we have memory size yet?
	jne	VMGetModeInfo		;yes! we don't have to do this again
%out For now, we assume 1 meg of memory for ValidateMode.  Fix this later!
	mov	dwVideoMemorySize,1024*1024
;
public	VMGetModeInfo
VMGetModeInfo:
	les	di,lpValMode
	mov	si,es:[di].dvmXRes
	shl	esi,16
	mov	si,es:[di].dvmYRes
	call	XYtoRes			;returns with ax = wResolution value
	mov	wResolution,ax		;set this into variable for call
	mov	ax,es:[di].dvmBpp
	mov	wBpp,ax
	call	FindMode
	or	bx,bx
	jz	VMErrorExit
	mov	ax,VALMODE_YES		;return that we support the mode
	jmp	VMExit			;and we're done!
;
public	VMErrorExit
VMErrorExit:
	mov	ax,VALMODE_NO_NOMEM	;oops, we don't support the mode
;
VMExit:
	pop	wScreenWidth		;
	pop	wBpp			;restore everything we've saved
	pop	wResolution		;
	pop	edi			;
	pop	esi			;
.286c
cEnd
;
;
subttl		Bank Switching Template To Be Copied to VFlatD
page +
public	XGABankSwitchTemplate
XGABankSwitchTemplate	label byte
;
;We copy this template to VFlatD when we call it to obtain the screen
;selector.  VFlatD copies it to its in-line code for bank switching.
;We therefore must be very careful to use opcodes that will behave in
;the desired manner in a USE32 code segment.  Upon entry to this
;code, AL will contain the bank to set.
;
;For the XGA, all we need to do is OUT 21x8,al.  The proper value for
;"x" will be determinted by PhysicalEnable.
;
	db	0bah			;this is MOV EDX opcode prefix (USE32)
;
public	TemplateBankReg
TemplateBankReg 	dw	?	;this is filled in by PhysicalEnable
			dw	0	;(we're moving a 32 bit value into EDX)
	db	0eeh			;this is OUT DX,AL (USE32)
XGABankSwitchTemplateSize = $ - InitSegOFFSET XGABankSwitchTemplate
;
;
sEnd	InitSeg
;
;
subttl		Reset HiRes Mode Upon Returning From a Full Screen DOS Box
page +
sBegin	Code
assumes cs,Code
;
;
cProc		ResetHiResMode,<FAR,PUBLIC>
;
; ResetHiResMode
;   This function is called by the VDD to restore the graphics adapter 
;   into graphics mode when switching to the system vm.  Note that this
;   function is in a page-locked segment.
;   
cBegin		<nogen>
.386
	mov	ax,DGROUP		;make DS --> Data
	mov	ds,ax
        assumes ds,Data
	push	esi
	push	edi
	call	PhysicalEnableFar	;
;
;We now un-set the BUSY bit that was set when we went out of Windows HiRes
;mode.	Also, set our own flag which is needed since the BUSY bit won't
;allow us to move the hardware cursor during a DIB engine drawing routine.
;
	les	bx,lpDriverPDevice	;
	and	DeviceFlags,NOT BUSY	;
	mov	ax,DeviceFlags		;un-set the busy bit
	mov	es:[bx].deFlags,ax	;this indicates that we're enabled
	mov	WindowsEnabledFlag,0	;set copy for our drawing routines too!
;
;Check if we need to re-set up the palette.  The palette DAC was already
;setup for 16 BPP in the call to PhysicalEnable.
;
	cmp	wBpp,8			;do we need to re-setup the palette?
	ja	RHR_CallVDD
	push	ds
	lds	si,lpColorTable		;ds:[si]-->color table
	mov	cx,256
	je	@F
	mov	cx,16
@@:	xor	ax,ax
	call	SetRAMDAC_far		;set up initial palette
	pop	ds
;
public	RHR_CallVDD
RHR_CallVDD:
;
;Call the VDD to order it to save the current register state.  This will
;hopefully assure that the Windows VM has the running state saved:
;
	mov	eax,VDD_SAVE_DRIVER_STATE
	movzx	ebx,OurVMHandle 	;
	call	dword ptr VDDEntryPoint ;
;
RHR_Exit:
	pop	edi
	pop	esi
	retf
.286c
cEnd		<nogen>
;
;
sEnd	Code

end
