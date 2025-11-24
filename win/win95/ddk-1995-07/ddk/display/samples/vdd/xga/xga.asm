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

page		,132
title		XGA Mini-VDD Support Functions
.386p
;
;
.xlist
include 	VMM.INC
include 	MINIVDD.INC
include 	VESA.INC
include 	XGA.INC
.list
;
;
subttl		Virtual Device Declaration
page +
Declare_Virtual_Device	XGA,					\
			3,					\
			1,					\
			MiniVDD_Control,			\
			Undefined_Device_ID,			\
			VDD_Init_Order, 			\
			,					\
			,					\
			,
;
;
subttl		Initialization Data
page +
VxD_IDATA_SEG
VxD_IDATA_ENDS
;
;
subttl		Locked Data Area
page +
VxD_LOCKED_DATA_SEG

VxD_LOCKED_DATA_ENDS
;
;
subttl		General Data Area
page +
VxD_DATA_SEG
;
public	WindowsVMHandle
WindowsVMHandle 	dd	?		;init'd at Dynamic_Init
;
public	VidMemLinearAddr
VidMemLinearAddr	dd	?		;the linear address of A0000H
;
public	OurCBDataPointer
OurCBDataPointer	dd	?		;init'd at Dynamic_Init
;
public	TotalMemorySize
TotalMemorySize 	dd	?		;init'd at Dynamic_Init
;
public	VESAStubLinearAddr
VESAStubLinearAddr	dd	0		;
;
public	XGARegBase, XGABankReg, XGAMemMappedRegs
XGARegBase		dd	?		;
XGABankReg		dd	?		;
XGACRTCReg		dd	?		;
XGAMemMappedRegs	dd	?		;
XGAPixmapBaseAddr	dd	?		;
;
public	ChipID
ChipID			dd	?		;identifies XGA/1 or XGA/2
;
public	RefreshRate
RefreshRate		dd	0		;
;
public	DisplayInfoStructure
DisplayInfoStructure	db	size DISPLAYINFO dup(0)
;
public	MemMappedRegsSelector
MemMappedRegsSelector	dw	0		;for display driver use
;
public	CRTCModeChangeFlags
CRTCModeChangeFlags	db	0		;FF = VDD is in CRTC mode change
;
public	XGASlotNbr
XGASlotNbr		db	0		;contains slot of active XGA
;
VxD_DATA_ENDS
;
;
subttl		MiniVDD Dynamic Initialization
page +
VxD_ICODE_SEG
;
;
public	MiniVDD_Dynamic_Init
BeginProc MiniVDD_Dynamic_Init
;
;Entry:
;	EBX contains the VM handle of the Windows VM.
;Exit:
;	If success, return NC.
;	If failure, return CY.
;
	mov	WindowsVMHandle,ebx	;save the Windows VM handle
;
;Get the linear address of physical A000:0H:
;
	VMMCall _MapPhysToLinear,<0a0000h,10000h,0>
	cmp	eax,-1			;was there an error?
	je	MVDI_ErrorExit		;yes, we can't run!
	mov	VidMemLinearAddr,eax	;everything's OK, save the linear addr
;
;Go try to identify the XGA card:
;
	call	Is_XGA			;returns 0 if not found
	or	ax,dx			;was XGA found?
	jz	MVDI_ErrorExit		;nope, don't load!
;
;IBM reserved 4 POS device ID's for the XGA series.  If we returned
;successfully from Is_XGA, it means that we found one of these four
;ID's.  However, only 2 of them have been defined.  If we don't ID
;specifically as an XGA/2, we assume that we only have an XGA/1.
;
	movzx	edi,di			;make sure top word of EDI is 0
	cmp	edi,XGA2_ID		;is it an XGA/2?
	je	@F			;yes, go save XGA/2 ChipID
	cmp	edi,AGX_ID		;is it an AGX?
	je	@F			;yes, go save AGX ChipID
	mov	edi,XGA1_ID		;nope, force it to be an XGA/1
@@:	mov	ChipID,edi		;DI contains the XGA model (XGA/1 or 2)
;
;Next, we want to allocate space in the per-VM CB data structure.  This will
;be used by the mini-VDD to store per-VM data relating to states of registers.
;
	VMMCall _Allocate_Device_CB_Area,<<size PerVMData>,0>
	mov	OurCBDataPointer,eax	;save offset our our VxD's area
	or	eax,eax 		;was call sucessful?
	jz	MVDI_ErrorExit		;nope, leave fatally!
;
public	MVDI_InstallVESAStub
MVDI_InstallVESAStub:
;
;Many VESA apps ignore the VESA spec and make far calls to INT 10H, function
;4F05H instead of doing the INT 10H.  Therefore, we have to place a stub in
;the virtual machine's code area which will translate this FAR call into
;a simple INT 10H, function 4F05H call.
;
;First, get the real value of the XGA/AGX banking register into our
;code stub:
;
	mov	eax,XGABankReg		;
	mov	VFS5BankRegPlaceholder,ax
;
;Now, go allocate some memory in the V86 area so we can place our banking stub
;there:
;
	VMMCall _Allocate_Global_V86_Data_Area,<VESA_STUB_SIZE, 0>
;
;The address that we get is a linear address.  We need to convert this to
;a V86 Segment:Offset address for the benefit of our real mode VESA apps:
;
	mov	edi,eax 		;we need this linear address for later
	mov	esi,eax 		;copy this for linear --> Seg:Off calc
	and	eax,0fh 		;get offset portion of address in EAX
	and	esi,NOT 0fh		;get segment portion of address in ESI
	shl	esi,12			;get segment portion into high word
	or	eax,esi 		;now EAX has Segment:Offset address
	mov	VESAStubLinearAddr,eax	;save return value
	or	eax,eax 		;did the call fail?
	jz	MVDI_InstallHooks	;yes, but it's not fatal!
	mov	esi,OFFSET32 VESA_4F05_Stub
	mov	ecx,VESA_STUB_SIZE	;
	rep	movsb			;mov 'er in!
;
public	MVDI_InstallHooks
MVDI_InstallHooks:
;
;The "master" VDD (VDD.386) contains all of the routines and procedures
;that are necessary to virtualize a standard VGA environment on high
;resolution super-VGA cards.  This "mini-VDD" provides some simple
;services which provide support which are peculiar to the chipset
;that we're providing support for.
;
;We must register the entry point addresses of our chipset-dependent
;routines with the "master" VDD.  To do this, we call a service in the
;"master" VDD, which returns the address of a dispatch table of the
;which is an array of the format:
;
;		dd	Address of support routine in this mini-VDD.
;			This address will be initialized to 0 by the "master"
;			VDD.  If we need to implement this functionality,
;			we fill in the address of our routine.
;
;Then, when the "master" VDD needs to call this function, it'll see if we've
;filled in the address field in for the function's entry and will call our
;routine if the address is filled in.  Otherwise, the VDD will skip the call
;and continue on its way without calling us.
;
;The following function calls the "master" VDD and returns a pointer to the
;dispatch table documented above in EDI and the number of functions
;supported in ECX. If the number of functions returned by the "master" VDD
;isn't equal to what we think it is, we return an error and don't allow
;Windows to continue running.
;
	VxDCall VDD_Get_Mini_Dispatch_Table
	cmp	ecx,NBR_MINI_VDD_FUNCTIONS	;perform a cursory version check
	jb	MVDI_ErrorExit			;oops, versions don't match!
;
public	MVDI_FillInTable
MVDI_FillInTable:
;
;Fill in the addresses of all the functions that we need to handle in this
;mini-VDD in the table provided by the "master" VDD whose address is
;contained in EDI.  Note that if you do not need to support a function,
;you do not need to fill in the dispatch table entry for that function.
;If you do not fill in an entry, the "master" VDD won't try to call
;this mini-VDD to support the function.  It'll just handle it in a
;default manner.
;
	MiniVDDDispatch REGISTER_DISPLAY_DRIVER,RegisterDisplayDriver
	MiniVDDDispatch GET_CHIP_ID,GetChipID
	MiniVDDDispatch PRE_HIRES_TO_VGA,PreHiResToVGA
	MiniVDDDispatch DISPLAY_DRIVER_DISABLING,DisplayDriverDisabling
	MiniVDDDispatch PRE_INT_10_MODE_SET,PreInt10ModeSet
	MiniVDDDispatch VESA_SUPPORT,VESASupport
	MiniVDDDispatch GET_TOTAL_VRAM_SIZE,GetTotalVRAMSize
	MiniVDDDispatch CHECK_SCREEN_SWITCH_OK,CheckScreenSwitchOK
	cmp	ChipID,AGX_ID		;running on the IIT AGX?
	je	MVDI_Setup2160Traps	;yes, don't need DAC virtualization
	MiniVDDDispatch VIRTUALIZE_DAC_OUT,VirtualizeDACOut
	MiniVDDDispatch VIRTUALIZE_DAC_IN,VirtualizeDACIn
	jmp	MVDI_GoodExit		;that's it for the XGA
;
public	MVDI_Setup2160Traps
MVDI_Setup2160Traps:
	MiniVDDDispatch POST_HIRES_TO_VGA,PostHiResToVGA
	mov	esi,OFFSET32 MiniVDD_Virtual2160
	mov	edx,2160h		;
	VMMCall Install_IO_Handler	;
	jc	MVDI_ErrorExit		;oops, couldn't install procedure
	mov	edx,2160h		;
	VMMCall Enable_Global_Trapping	;always leave it trapped
;
	mov	esi,OFFSET32 MiniVDD_VirtualAGXIndexedPorts
	mov	edx,216ah		;
	VMMCall Install_IO_Handler	;
	jc	MVDI_ErrorExit		;oops, couldn't install procedure
	mov	edx,216ah		;
	VMMCall Enable_Global_Trapping	;always leave it trapped
;
	mov	esi,OFFSET32 MiniVDD_VirtualAGXIndexedPorts
	mov	edx,216bh		;
	VMMCall Install_IO_Handler	;
	jc	MVDI_ErrorExit		;oops, couldn't install procedure
	mov	edx,216bh		;
	VMMCall Enable_Global_Trapping	;always leave it trapped
;
	mov	esi,OFFSET32 MiniVDD_VirtualAGXIndexedPorts
	mov	edx,216ch		;
	VMMCall Install_IO_Handler	;
	jc	MVDI_ErrorExit		;oops, couldn't install procedure
	mov	edx,216ch		;
	VMMCall Enable_Global_Trapping	;always leave it trapped
;
	mov	esi,OFFSET32 MiniVDD_VirtualAGXIndexedPorts
	mov	edx,216dh		;
	VMMCall Install_IO_Handler	;
	jc	MVDI_ErrorExit		;oops, couldn't install procedure
	mov	edx,216dh		;
	VMMCall Enable_Global_Trapping	;always leave it trapped
;
	mov	esi,OFFSET32 MiniVDD_VirtualAGXIndexedPorts
	mov	edx,216eh		;
	VMMCall Install_IO_Handler	;
	jc	MVDI_ErrorExit		;oops, couldn't install procedure
	mov	edx,216eh		;
	VMMCall Enable_Global_Trapping	;always leave it trapped
;
	mov	esi,OFFSET32 MiniVDD_VirtualAGXIndexedPorts
	mov	edx,216fh		;
	VMMCall Install_IO_Handler	;
	jc	MVDI_ErrorExit		;oops, couldn't install procedure
	mov	edx,216fh		;
	VMMCall Enable_Global_Trapping	;always leave it trapped
;
MVDI_GoodExit:
	clc				;return success
	jmp	short MVDI_Exit 	;
;
MVDI_ErrorExit:
	stc
;
MVDI_Exit:
	ret
EndProc MiniVDD_Dynamic_Init
;
;
subttl		Identify the XGA and Its Slot Address
page +
public	Is_XGA
BeginProc Is_XGA
;
;Entry:
;	Nothing assumed.
;Exit:
;	DX:AX contains the return code:
;		-- 1 if XGA was found.
;		-- 0 if XGA wasn't found.
;	DI contains the POS Adapter ID for the XGA if successful.
;	XGARegBase (the base port number for accessing the XGA) is set.
;	XGAMemMappedRegs --> the Co-Processor memory-mapped register set.
;	XGAPixmapBaseAddr --> the XGA's idea of where its pixmap is.
;	All other XGA's in system besides the one we found first are
;	disabled.
;
	cld				;set direction forward
	mov	TotalMemorySize,0	;initialize this to 0
;
;Now setup a loop to look in all of the PS/2 slots for the XGA card.  We
;look in the slots first because someone may have added an XGA/2 card to
;a motherboard that has an XGA/1 on it.  We'd rather find the XGA/2 than
;the XGA/1, so we search the slots first.
;
	mov	ebx,1			;start with slot 1
	mov	ecx,8			;check 8 slots
;
public	IXFindXGALoop
IXFindXGALoop:
	push	ecx			;save loop counter
	push	ebx			;save slot index
;
;Put the slot (contained in EBX) into setup mode:
;
	mov	al,bl			;get channel number to setup
	dec	al			;make it offset from 0
	or	al,8			;set bit 3 always
	out	96h,al			;
;
;Look for the XGA by reading the POS-ID and data
;bytes from the 6 consecutive ports offset from POSBaseAddr:
;
	mov	edx,100h		;
	in	al,dx			;get byte 0 (this is POS ID low word)
	mov	bl,al			;(save this)
;
	inc	edx			;
	in	al,dx			;get byte 1 (this is POS ID high word)
	mov	bh,al			;
	mov	di,bx			;
;
	inc	edx			;
	in	al,dx			;get byte 2 (this is POS data byte 2)
	mov	cl,al			;
;
	inc	edx			;
	in	al,dx			;get byte 3 (this is POS data byte 3)
	mov	ch,al			;
	mov	si,cx			;
;
	inc	edx			;
	in	al,dx			;get byte 4 (this is POS data byte 4)
	mov	ah,al			;
;
	inc	edx			;
	in	al,dx			;get byte 5 (this is POS data byte 5)
;
;Reenable the slot to normal mode:
;
	pop	ebx			;restore slot number
	pop	ecx			;restore loop counter
	push	eax			;save POS data in AX
	xor	al,al			;
	out	96h,al			;
	pop	eax			;
;
;Now, see if the XGA is in this slot by checking the POS ID (in DI)
;for a valid XGA value:
;
	call	CheckPOSIDForXGA	;CY returned in XGA was found
	jc	IXSavePOSData		;we found the XGA in this slot!
;
;No XGA was found.  Increment the slot index and continue the search.
;
	inc	ebx			;bump to next slot
	dec	ecx			;done with search?
	jnz	IXFindXGALoop		;not yet
;
public	IXCheckMotherboard
IXCheckMotherboard:
;
;Now, check the motherboard for an on-board XGA.
;
	mov	al,0dfh 		;put the system board into setup mode
	out	94h,al			;
;
;Look for the XGA by reading the POS-ID and data bytes from the 6 consecutive
;ports offset from POSBaseAddr:
;
	mov	edx,100h		;this is POS register base
	in	al,dx			;get byte 0 (this is POS ID low word)
	mov	bl,al			;(save this)
;
	inc	edx			;
	in	al,dx			;get byte 1 (this is POS ID high word)
	mov	bh,al			;
	mov	di,bx			;
;
	inc	edx			;
	in	al,dx			;get byte 2 (this is POS data byte 2)
	mov	cl,al			;
;
	inc	edx			;
	in	al,dx			;get byte 3 (this is POS data byte 3)
	mov	ch,al			;
	mov	si,cx			;
;
	inc	edx			;
	in	al,dx			;get byte 4 (this is POS data byte 4)
	mov	ah,al			;
;
	inc	edx			;
	in	al,dx			;get byte 5 (this is POS data byte 5)
;
;Reenable the system board for normal use:
;
	push	eax			;
	mov	al,0ffh 		;
	out	94h,al			;
	pop	eax			;
	xor	ebx,ebx 		;we are slot 0
;
;Now, see if the XGA is on the system board by checking the POS ID (in DI)
;for a valid XGA value:
;
	call	CheckPOSIDForXGA	;CY returned in XGA was found
	jc	IXSavePOSData		;we found the XGA on system board!
;
public	IXTryForAGX
IXTryForAGX:
;
;Let's see if we have an IIT AGX.  We know that its CRTC base is at 2160H, so
;we can check a few things to ID it:
;
	mov	dx,2160h		;set to AGX operating mode register
	mov	al,0ah			;write out a pattern to 2160H
	out	dx,al			;
	jmp	@F			;
@@:	in	al,dx			;read it back
	and	al,0fh			;get rid of undefined bits
	cmp	al,0ah			;was it readable?
	jne	IXErrorExit		;nope, musn't be an AGX
	mov	al,01h			;set back to VGA mode
	out	dx,al			;
;
	mov	dx,2168h		;set to AGX banking register
	mov	al,05h			;set to bank 05H
	out	dx,al			;
	jmp	@F			;
@@:	in	al,dx			;read it back
	cmp	al,05h			;was it readable?
	jne	IXErrorExit		;nope, musn't be an AGX
;
;We're an AGX, setup for it:
;
	mov	di,AGX_ID		;
	mov	XGARegBase,2160h	;
	mov	XGABankReg,2168h	;
	mov	XGAPixmapBaseAddr,0fd800000h
;
;We need to set the AGX so that its memory mapped registers are at
;physical address B1F00H:
;
	mov	edx,216ah		;EDX --> AGX indexed registers
	mov	al,6dh			;this is index for Mode Register 3
	mov	ah,03h			;set to base address B1F00H
	out	dx,ax			;
	mov	ebx,0b1f00h		;this is memory mapped register addr
	mov	XGASlotNbr,0		;AGX is not a MicroChannel card
	jmp	IXGetMemMappedRegsAddr	;go do common processing
;
public	IXSavePOSData
IXSavePOSData:
;
;The XGA was found.  Save off the POS data:
;
	mov	XGASlotNbr,bl		;save slot that we found XGA in
	movzx	ecx,si			;restore POS data bytes 2 & 3 to CX
;
;Get the instance of the XGA from bits 1-3 in POS data byte 2 (in CL):
;
	mov	ch,cl			;copy this to CH for later use
	and	cl,0eh			;isolate bits 1-3
	shr	cl,1			;now CL has the instance number
;
;Get the base address of the I/O registers:
;
	xor	ebx,ebx 		;make sure BH = 0
	mov	bl,cl			;get instance into BL
	mov	al,cl			;copy instance into AL for later
	shl	bl,4			;get instance in proper nibble
	or	ebx,2100h		;now EBX has the I/O base address
	mov	XGARegBase,ebx		;
	add	bl,8			;make EBX --> XGABankReg
	mov	XGABankReg,ebx		;save it for quick access later
;
;We must now get the physical address of where the XGA thinks its video
;memory is.  We'll need this to use the XGA hardware BLTer for block moves
;and brush BLT's.  POS register 4 (contained in AH) contains this information.
;
	mov	bl,ah			;get POS register 4 into BL
	and	bl,0feh 		;get rid of bit 0
	shl	ebx,24			;now EBX has high 7 bits of aperture
	movzx	edx,al			;get instance into EDX
	shl	edx,22			;
	or	edx,ebx 		;now EDX has physical addr of aperture
	mov	XGAPixmapBaseAddr,edx	;this is XGA's idea of VRAM start
;
;We need to get the memory-mapped address of the XGA Co-Processor registers
;from the POS register data currently held in CH.  The "instance" is contained
;in CL:
;
	movzx	ebx,cl			;copy instance into EBX
	shl	ebx,7			;multiply instance by 128
	add	ebx,1c00h		;add on fudge factor
	shr	ecx,12			;get ROM address field in CX
;
;The ROM address base is at 2000H * contents of the ROM address field:
;
	mov	eax,2000h		;multiply by 2000H
	mul	ecx			;now EAX has offset of ROM address
	add	ebx,eax 		;now EBX has offset of Co-Processor regs
	add	ebx,0c0000h		;add on physical address C0000H
;
public	IXGetMemMappedRegsAddr
IXGetMemMappedRegsAddr:
;
;We now get a linear address equivelent for the physical address that we've
;just calculated.
;
	VMMCall _MapPhysToLinear,<ebx, 256, 0>
	mov	XGAMemMappedRegs,eax	;save this for later use
;
public	IXDisableMotherboardXGA
IXDisableMotherboardXGA:
;
;If the user added an XGA card to a machine which had other XGA cards or had
;XGA on the motherboard, we need to make sure that all XGA's in the system
;besides the one we found are disabled.  If we found only one XGA on the
;motherboard or we found an AGX, we need not do this disable search.
;
	cmp	XGASlotNbr,0		;was XGA found on motherboard (or AGX)?
	je	IXGoodExit		;yes, don't need to do the following
	push	edi			;save the POS ID for the active XGA
;
;First, disable the Motherboard XGA if there is one:
;
	mov	al,0dfh 		;put the motherboard into setup mode
	out	94h,al			;
;
	mov	edx,100h		;this is POS register base
	in	al,dx			;get byte 0 (this is POS ID low word)
	mov	bl,al			;(save this)
	inc	edx			;
	in	al,dx			;get byte 1 (this is POS ID high word)
	mov	bh,al			;
	mov	di,bx			;
;
	call	CheckPOSIDForXGA	;do we have an XGA on the motherboard?
	jnc	@F			;nope, go reenable the motherboard
	call	DisableXGA		;yes, disable it!
;
@@:	mov	al,0ffh 		;
	out	94h,al			;
;
public	IXDisableOtherXGAs
IXDisableOtherXGAs:
;
;Now, search the other slots for any XGA's that we don't want to have enabled:
;
	mov	cl,1			;start with slot 1
	mov	ch,8			;there are 8 boards to check
;
public	IXDisableLoop
IXDisableLoop:
	cmp	cl,XGASlotNbr		;does this slot have XGA that we want?
	je	IXEndofDisableLoop	;yes, skip the check
	mov	al,cl			;get channel number to setup
	dec	al			;make it offset from 0
	or	al,8			;set bit 3 always
	out	96h,al			;
;
	mov	edx,100h		;this is POS register base
	in	al,dx			;get byte 0 (this is POS ID low word)
	mov	bl,al			;(save this)
	inc	edx			;
	in	al,dx			;get byte 1 (this is POS ID high word)
	mov	bh,al			;
	mov	di,bx			;
;
	call	CheckPOSIDForXGA	;do we have an XGA in this slot?
	jnc	@F			;nope, go reenable the slot
	call	DisableXGA		;yes, disable it!
;
@@:	xor	al,al			;
	out	96h,al			;
;
public	IXEndofDisableLoop
IXEndofDisableLoop:
	inc	cl			;bump to next slot
	dec	ch			;done checking?
	jnz	IXDisableLoop		;nope, go check some more
	pop	edi			;restore POS ID for active XGA
;
public	IXGoodExit
IXGoodExit:
	mov	ax,1			;return success to caller
	cwd				;in DX:AX
	jmp	IXExit			;and we're done
;
public	IXErrorExit
IXErrorExit:
	xor	ax,ax			;return failure to caller
	cwd				;in DX:AX
;
IXExit:
	ret				;
EndProc 	Is_XGA
;
;
subttl		Check the POS ID Word to See if it's an XGA
page +
public	CheckPOSIDForXGA
BeginProc CheckPOSIDForXGA
;
;Entry:
;	DI contains the POS ID word.
;Exit:
;	CY set if XGA was found.
;
	cmp	di,8fd8h		;was the XGA found?
	je	CPXFoundXGA		;yes
	cmp	di,8fd9h		;was the XGA found?
	je	CPXFoundXGA		;yes
	cmp	di,8fdah		;was the XGA found?
	je	CPXFoundXGA		;yes
	cmp	di,8fdbh		;was the XGA found?
	je	CPXFoundXGA		;yes
	clc				;XGA was not found
	jmp	CPXExit 		;
;
CPXFoundXGA:
	stc				;set CY denoting XGA was found
;
CPXExit:
	ret
EndProc 	CheckPOSIDForXGA
;
;
subttl		Disable an Unused XGA
page +
public	DisableXGA
BeginProc DisableXGA
;
;Entry:
;	Slot to be disabled is in setup mode.
;Exit:
;	CL & CH must be preserved.
;
	mov	edx,102h		;set to XGA enable POS register
	in	al,dx			;get the current value
	and	al,NOT 01h		;disable the XGA by clearing bit 0
	out	dx,al			;
;
DXExit:
	ret				;
EndProc DisableXGA
;
;
subttl		Return ChipID To Main VDD
page +
public	MiniVDD_GetChipID
BeginProc MiniVDD_GetChipID
;
;Entry:
;	Nothing assumed.
;Exit:
;	EAX contains the ChipID.
;	Preserve ALL other registers.
;
;This routine is used by the Main VDD's Plug&Play code to determine whether
;a card has been changed since the last time that Windows was booted.  We
;are called to return to ChipID.  This assures us that Plug&Play will detect
;a different card, even if both cards use this same MiniVDD.  If the ChipID
;has changed, Plug&Play will get wind of it and will take appropriate action.
;
	mov	eax,ChipID		;return ChipID to caller
	ret				;
EndProc 	MiniVDD_GetChipID
;
;
subttl		Int 10H VESA Stub For Supporting Calls to 4F05H
page +
public	VESA_4F05_Stub
VESA_4F05_Stub		proc	near
	mov	al,dl			;get bank to set in AL
	db	0bah			;this is MOV DX,abs16 prefix
;
public	VFS5BankRegPlaceholder
VFS5BankRegPlaceholder	label	word
	dw	?			;
	or	bh,bh			;setting the bank?
	jz	VFS5SetBank		;yes, go do it
;
public	VFS5GetBank
VFS5GetBank:
	xor	ah,ah			;make sure AH is 0
	in	al,dx			;get bank number in AL
	mov	edx,eax 		;return it in the expected place
	jmp	VFS5GoodExit		;and return success
;
public	VFS5SetBank
VFS5SetBank:
	out	dx,al			;set the bank
;
public	VFS5GoodExit
VFS5GoodExit:
	db	0b8h,4fh,00h		;this is MOV AX,004FH (success code)
	retf				;and do a far return to caller
VESA_4F05_Stub		endp
VESA_STUB_SIZE		equ	$ - OFFSET32 VESA_4F05_Stub
;
VxD_ICODE_ENDS
;
;
subttl		Dispatch Table for VMM Calling This Mini-VDD
page +
VxD_LOCKED_CODE_SEG
;
;
Begin_Control_Dispatch	MiniVDD
	Control_Dispatch Sys_Dynamic_Device_Init, MiniVDD_Dynamic_Init
	Control_Dispatch Device_Init,	     MiniVDD_Dynamic_Init
	Control_Dispatch System_Exit,	     MiniVDD_System_Exit
End_Control_Dispatch MiniVDD
;
;
subttl		System Exit Handler
page +
public	MiniVDD_System_Exit
BeginProc MiniVDD_System_Exit, DOSVM
;
;We need to return the CRTC ownership to the VGA here since Windows never
;calls the display driver's Disable routine.
;
	call	LeaveXGAMode		;
;
MSEExit:
	clc
	ret
EndProc MiniVDD_System_Exit
;
;
subttl		Register Display Driver Dependent Data
page +
public	MiniVDD_RegisterDisplayDriver
BeginProc MiniVDD_RegisterDisplayDriver, RARE
;
;Entry:
;	EBP --> the Client Register Structure (documented in VMM.INC)
;	The XGA is still in VGA mode.
;	Client_CL contains the function code:
;		0 = return XGA ID, CRTC register base, pixmap base address.
;		1 = return mem size, selector for memory mapped regs.
;Exit:
;	If Client_CL was 0:
;		Client_EAX contains the XGA Pixmap base address.
;		Client_DX contains the CRTC register base (21x0).
;		Client_DI contains the chipset ID (XGA1_ID, XGA2_ID, or AGX_ID).
;	If Client_CL was 1:
;		Client_EAX contains the total memory size contained on the XGA.
;		Client_DX contains the LDT selector for the memory mapped regs.
;		The XGA is in HiRes mode.
;
	cmp	[ebp].Client_CL,0	;doing subfunction 0?
	jne	MRDDSubFunc1		;nope, go do subfunction 1
;
public	MRDDSubFunc0
MRDDSubFunc0:
	mov	eax,XGAPixmapBaseAddr	;
	mov	[ebp].Client_EAX,eax	;
	mov	edx,XGARegBase		;
	mov	[ebp].Client_DX,dx	;
	mov	edi,ChipID		;
	mov	[ebp].Client_DI,di	;
	jmp	MRDDExit		;
;
public	MRDDSubFunc1
MRDDSubFunc1:
;
;This routine is called by the display driver immediately before setting
;the HiRes mode for Windows.
;
	call	GetXGAMemorySize	;
	mov	[ebp].Client_EAX,esi	;
;
;Get a ring 3 Selector:Offset value for XGAMemMappedRegs.  The display driver
;needs this Selector:Offset value.  The display driver is incapable of doing
;this itself because EMM386 or some other memory manager may have mapped
;linear memory over the memory mapped register region.	By giving the
;display driver a physical address for these registers, we guarantee that
;we'll run, even if EMM386 gets in the way of the ring 3 address:
;
	movzx	eax,MemMappedRegsSelector	;get current value
	or	eax,eax 			;do we already have selector?
	jnz	@F				;yes, don't get it again
	mov	eax,XGAMemMappedRegs		;get linear address base
	mov	ecx,1023			;we need a limit of 1K
	mov	edx,10010011b			;set present, DPL == undefined
						;    app selector, Data
	mov	edi,00010000b			;set AVL bit on
	VMMCall _BuildDescriptorDWORDs,<eax,ecx,edx,edi,0>
;
;Now, allocate the LDT selector to pass to the VDD:
;
	VMMCall _Allocate_LDT_Selector,<ebx,edx,eax,1,0>
	mov	MemMappedRegsSelector,ax	;save this for later flag
@@:	mov	[ebp].Client_DX,ax		;return this selector
;
;Go attempt to retrieve a RefreshRate from the registry:
;
	mov	eax,OFFSET32 DisplayInfoStructure
	mov	ecx,SIZE DISPLAYINFO	;pass size in ECX, address in EAX
	mov	[eax].diHdrSize,cx	;fill in this entry
	VxDCall VDD_Get_DISPLAYINFO	;get information from the VDD
	movzx	edi,DisplayInfoStructure.diRefreshRateMax
	mov	RefreshRate,edi 	;save it for later use
;
MRDDExit:
	ret				;
EndProc MiniVDD_RegisterDisplayDriver
;
;
subttl		Get XGA's Memory Size
page +
public	GetXGAMemorySize
BeginProc GetXGAMemorySize, RARE
;
;Entry:
;	Nothing assumed.
;Exit:
;	TotalMemorySize is set.
;
;First, set the XGA into extended mode:
;
	mov	edx,XGARegBase		;get base register determined earlier
	add	dl,04h			;DX --> Interrupt Enable Register (21x4)
	xor	al,al			;we want NO XGA interrupts enabled
	out	dx,al			;
;
	inc	dl			;DX --> Interrupt Status Register (21x5)
	mov	al,0ffh 		;set Interrupt Status to FFH
	out	dx,al			;
;
	sub	dl,05h			;DX --> Operating Mode Register (21x0)
	mov	al,04h			;set to Intel memory layout, HiRes mode
	out	dx,al			;
;
	inc	dl			;DX --> Aperture Control Register (21x1)
	mov	al,01h			;we want 64K aperture at A000:0H
	out	dx,al			;
;
	add	dl,7			;DX --> XGA Bank Register (21x8)
	mov	al,0ch			;this bank couldn't exist on a 512K XGA
	out	dx,al			;
;
;We now try writing to an address that could not possibly exist on
;a 512K XGA configuration.  If we get back what we've read, then we
;know we're on a 1024K XGA.  Otherwise, we're running on a 512K XGA:
;
	mov	esi,512*1024		;assume we're running on a 512K XGA
	mov	edi,VidMemLinearAddr	;EDI --> video memory at bank 0CH
	mov	byte ptr [edi],5ah	;write out a 5AH pattern
	cmp	byte ptr [edi],5ah	;did we get back what we wrote?
	jne	GXMSGotMemSize		;nope, we're 512K
	mov	byte ptr [edi],0	;write out a 0 pattern
	cmp	byte ptr [edi],0	;did we get back what we wrote?
	jne	GXMSGotMemSize		;nope, we're 512K
	mov	esi,1024*1024		;assume we're running on a 1 meg XGA
	cmp	ChipID,XGA1_ID		;running on an original XGA?
	je	GXMSGotMemSize		;yes, 1 meg was the limit on those
;
;We're running on an XGA/2.  Check to see if we have 2 megabytes of memory:
;
	mov	edx,XGABankReg		;set to banking register (21x8)
	mov	al,11h			;this couldn't exist on a 1 meg system
	out	dx,al			;
	mov	byte ptr [edi],5ah	;write out a 5AH pattern
	cmp	byte ptr [edi],5ah	;did we get back what we wrote?
	jne	GXMSGotMemSize		;nope, we're 1 meg
	mov	byte ptr [edi],0	;write out a 0 pattern
	cmp	byte ptr [edi],0	;did we get back what we wrote?
	jne	GXMSGotMemSize		;nope, we're 1 meg
	add	esi,esi 		;hey, we're 2 meg!
;
GXMSGotMemSize:
	mov	TotalMemorySize,esi	;save this for later use
	mov	edx,XGABankReg		;reset back to bank zero (register 21x8)
	xor	al,al			;
	out	dx,al			;
	add	dl,2			;EDX --> XGA extended CRTC register
	mov	XGACRTCReg,edx		;save this for later use
;
GXMSExit:
	ret
EndProc 	GetXGAMemorySize
;
;
subttl		Utility Routine To Leave XGA HiRes Mode
page +
public	LeaveXGAMode
BeginProc LeaveXGAMode, DOSVM
	cmp	ChipID,AGX_ID		;running on an IIT AGX?
	je	LXMExit 		;yes, no need to do this
	mov	edx,XGARegBase		;EDX --> Operating Mode register
;
	inc	dl			;EDX --> Aperture Control register
	xor	al,al			;set no aperture
	out	dx,al			;
;
	add	dl,3			;EDX --> Interrupt Enable register
	out	dx,al			;set no interrupts
;
	inc	dl			;EDX --> Interrupt Status register
	mov	al,0ffh 		;clear all interupt status
	out	dx,al			;
;
	add	dl,5			;EDX --> XGA CRTC Index Register
	mov	ax,1550h		;Enable VFB, prepare for reset
	out	dx,ax			;
;
	mov	ax,1450h		;Enable VFB, reset CRT
	out	dx,ax			;
;
	mov	ax,51h			;Normal scale factors
	out	dx,ax			;
;
	mov	ax,0454h		;Select VGA's oscillator
	out	dx,ax			;
;
	mov	ax,70h			;Select external VGA oscillator
	out	dx,ax			;
;
	mov	ax,202ah		;No VSync interrupts
	out	dx,ax			;
;
	sub	dl,0ah			;EDX --> Operating Mode Register
	mov	al,01h			;switch to VGA mode
	out	dx,al			;
;
	mov	edx,3c3h		;
	mov	al,01h			;ensure VGA address decode
	out	dx,al			;
;
LXMExit:
	ret				;
EndProc LeaveXGAMode
;
;
subttl		Prepare to Enter a Standard VGA Mode from HiRes Mode
page +
public	MiniVDD_PreHiResToVGA
BeginProc MiniVDD_PreHiResToVGA, DOSVM
;
;Now, call MiniVDD_System_Exit because it contains the code which will
;switch us out of HiRes mode into standard VGA mode:
;
	cmp	ChipID,AGX_ID		;running on an AGX?
	jne	@F			;nope, skip this
	mov	CRTCModeChangeFlags,0ffh
@@:	call	LeaveXGAMode		;
;
MHTVExit:
	ret
EndProc MiniVDD_PreHiResToVGA
;
;
subttl		Post HiRes --> Full Screen VM Processing
page +
public	MiniVDD_PostHiResToVGA
BeginProc MiniVDD_PostHiResToVGA, DOSVM
;
;This routine is not called on an XGA.	It's only used on the AGX.
;
	mov	CRTCModeChangeFlags,0	;we're done with the CRTC mode change
;
MVTHExit:
	ret
EndProc MiniVDD_PostHiResToVGA
;
;
subttl		Display Driver Is Being Disabled Notification
page +
public	MiniVDD_DisplayDriverDisabling
BeginProc	MiniVDD_DisplayDriverDisabling, RARE
;
;The display driver is depending on us to take the hardware out of HiRes
;mode into VGA mode:
;
	call	LeaveXGAMode		;this puts hardware in VGA mode
;
MDDDExit:
	ret					;
EndProc MiniVDD_DisplayDriverDisabling
;
;
subttl		Routine Called When VM Does an Int 10H Mode Set
page +
public	MiniVDD_PreInt10ModeSet
BeginProc MiniVDD_PreInt10ModeSet, DOSVM
;
;Entry:
;	EAX contains the mode that we're setting to (in AL).
;	EBX contains the VM handle doing the mode set.
;	EBP --> VM's Client Registers.
;Exit:
;	Preserve everything that's used.
;
;This routine is called whenever a VM (including the Windows VM) does an
;INT 10H, function 0 mode set call.  The MiniVDD uses this routine to
;do anything necessary to implement the mode set.  In the case of the XGA,
;where the ROM BIOS itself doesn't take the screen out of HiRes mode (if it's
;currently in an XGA HiRes mode), we take care of restoring the screen to
;VGA mode.  This routine is called regardless of whether the VM is windowed
;or full-screen.  We therefore must be careful not to take the screen out
;of XGA HiRes mode if this is a windowed VM!
;
;Do nothing for the Windows VM:
;
	cmp	ebx,WindowsVMHandle	;is this the Windows VM?
	je	MPIMExit		;yes, do nothing
;
;Record the mode number in our CB data area:
;
	push	edi			;save everything that we use
	push	esi			;
	mov	edi,ebx 		;make EDI --> our CB data area
	add	edi,OurCBDataPointer	;
	mov	[edi].CurrentMode,ax	;
;
;Get the CRTC owner VM and see if we're running windowed or full-screen.  If
;we're running full-screen, call LeaveXGAMode to get us out of any potential
;HiRes mode that we may be in:
;
	VxDCall VDD_Get_VM_Info 	;get CRTC owner VM handle in EDI
	cmp	edi,WindowsVMHandle	;is CRTC controlled by Windows?
	je	@F			;yes, don't call LeaveXGAMode!
	push	eax			;save registers wrecked by LeaveXGAMode
	push	edx			;
	call	LeaveXGAMode		;nope, call LeaveXGAMode
	pop	edx			;restore saved registers
	pop	eax			;
@@:	pop	esi			;restore saved registers
	pop	edi			;
;
MPIMExit:
	ret					;
EndProc MiniVDD_PreInt10ModeSet
;
;
subttl		XGA VESA Tables
page +
BeginProc VESADataTables, DOSVM
;
public	VESADispatchTable
VESADispatchTable	label	dword
	dd	OFFSET32 VESAFunc0
	dd	OFFSET32 VESAFunc1
	dd	OFFSET32 VESAFunc2
	dd	OFFSET32 VESAFunc3
	dd	OFFSET32 VESAFunc4
	dd	OFFSET32 VESAFunc5
	dd	OFFSET32 VESAFunc6
	dd	OFFSET32 VESAFunc7
	dd	OFFSET32 VESAFunc8
	dd	OFFSET32 VESAFunc9
	dd	OFFSET32 VESAFuncA
LAST_VESA_FUNC	 equ	 (($-OFFSET32 VESADispatchTable) / 4) - 1
;
;
public	VESAModesTable
VESAModesTable		label	word
	dw	100h			;640x480x8
	dw	101h			;640x480x8
	dw	103h			;800x600x8
	dw	104h			;1024x768x4
	dw	105h			;1024x768x8
	dw	111h			;640x480x16 (5:6:5)
NbrVESAModes		equ	($ - OFFSET32 VESAModesTable) / 2
	dw	-1			;terminator
VESAModesTableLen	equ	$ - OFFSET32 VESAModesTable
;
;
public	VESAModesTable_AGX
VESAModesTable_AGX	label	word
	dw	100h			;640x480x8
	dw	101h			;640x480x8
	dw	103h			;800x600x8
	dw	104h			;1024x768x4
	dw	105h			;1024x768x8
	dw	106h			;1280x1024x4
	dw	107h			;1280x960x8
	dw	111h			;640x480x16 (5:6:5)
	dw	114h			;800x600x16 (5:6:5)
	dw	117h			;1024x768x16 (5:6:5)
NbrVESAModes_AGX	equ	($ - OFFSET32 VESAModesTable_AGX) / 2
	dw	-1			;terminator
VESAModesTableLen_AGX	equ	$ - OFFSET32 VESAModesTable_AGX
;
;
VESAModeEntry		struc
	ModeTableAddr		dd	?
	ModeTableLen		db	?
	ScreenBPP		db	?
	ScreenWidthInPixels	dw	?
	ScreenPitch		dw	?
	ScreenHeight		dw	?
	VMFiller		dd	?
VESAModeEntry		ends
.errnz	SIZE VESAModeEntry - 16
;
;
VESAModeLookupTable	label	dword
Mode100H  VESAModeEntry <OFFSET32 Mode640x400x8, ModeTableEntryLen, 8, 640, 640, 400>
Mode101H  VESAModeEntry <OFFSET32 Mode640x480x8, ModeTableEntryLen, 8, 640, 640, 480>
Mode102H  VESAModeEntry <>
Mode103H  VESAModeEntry <OFFSET32 Mode800x600x8x60, XGA2ModeTableEntryLen, 8, 800, 800, 600>
Mode104H  VESAModeEntry <OFFSET32 Mode1024x768x4, ModeTableEntryLen, 4, 1024, 512, 768>
Mode105H  VESAModeEntry <OFFSET32 Mode1024x768x8, ModeTableEntryLen, 8, 1024, 1024, 768>
Mode106H  VESAModeEntry <>
Mode107H  VESAModeEntry <OFFSET32 Mode1280x1024x8, XGA2ModeTableEntryLen, 8, 1280, 1280, 1024>
Mode108H  VESAModeEntry <>
Mode109H  VESAModeEntry <>
Mode10AH  VESAModeEntry <>
Mode10BH  VESAModeEntry <>
Mode10CH  VESAModeEntry <>
Mode10DH  VESAModeEntry <>
Mode10EH  VESAModeEntry <>
Mode10FH  VESAModeEntry <>
Mode110H  VESAModeEntry <>
Mode111H  VESAModeEntry <OFFSET32 Mode640x480x16, ModeTableEntryLen, 16, 640, 1280, 480>
Mode112H  VESAModeEntry <>
Mode113H  VESAModeEntry <>
Mode114H  VESAModeEntry <>
Mode115H  VESAModeEntry <>
Mode116H  VESAModeEntry <>
Mode117H  VESAModeEntry <>
HighestMode	equ (($ - OFFSET32 VESAModeLookupTable) / SIZE VESAModeEntry) - 1 + 100h
;
Mode640x400x8		label	word
	dw	0150h, 0050h, 6310h, 0011h, 4f12h, 0013h, 4f14h, 0015h
	dw	6316h, 0017h, 5518h, 0019h, 611ah, 001bh, 001ch, 001eh
	dw	0c020h, 0121h, 8f22h, 0123h, 8f24h, 0125h, 0c026h, 0127h
	dw	9c28h, 0129h, 9e2ah, 0ff2ch, 0ff2dh, 0036h, 0040h, 0041h
	dw	0042h, 5043h, 0044h, 0054h, 0351h, 0070h, 4750h
	dw	0055h, 0ff64h
	dw	0032h, 0035h, 0038h, 0039h, 003ah, 0ff3bh, 0ff3ch, 0ff3dh
ModeTableEntryLen	equ	($ - OFFSET32 Mode640x400x8)/2
;
Mode640x480x8		label	word
	dw	0150h, 0050h, 6310h, 0011h, 4f12h, 0013h, 4f14h, 0015h
	dw	6316h, 0017h, 5518h, 0019h, 611ah, 001bh, 001ch, 001eh
	dw	0c20h, 0221h, 0df22h, 0123h, 0df24h, 0125h, 0c26h, 0227h
	dw	0ea28h, 0129h, 0ec2ah, 0ff2ch, 0ff2dh, 0036h, 0040h, 0041h
	dw	0042h, 5043h, 0044h, 0054h, 0351h, 0070h, 0c750h
	dw	0055h, 0ff64h
	dw	0032h, 0035h, 0038h, 0039h, 003ah, 0ff3bh, 0ff3ch, 0ff3dh
;
Mode640x480x16		label	word
	dw	0150h, 0050h, 6310h, 0011h, 4f12h, 0013h, 4f14h, 0015h
	dw	6316h, 0017h, 5518h, 0019h, 611ah, 001bh, 001ch, 001eh
	dw	0c20h, 0221h, 0df22h, 0123h, 0df24h, 0125h, 0c26h, 0227h
	dw	0ea28h, 0129h, 0ec2ah, 0ff2ch, 0ff2dh, 0036h, 0040h, 0041h
	dw	0042h, 0a043h, 0044h, 0054h, 0451h, 0070h, 0c750h
	dw	0055h, 0ff64h
	dw	0032h, 0035h, 0038h, 0039h, 003ah, 0ff3bh, 0ff3ch, 0ff3dh
;
Mode1024x768x4		label	word
	dw	0150h, 0050h, 9d10h, 0011h, 7f12h, 0013h, 7f14h, 0015h
	dw	9d16h, 0017h, 8718h, 0019h, 9c1ah, 001bh, 401ch, 041eh
	dw	3020h, 0321h, 0ff22h, 0223h, 0ff24h, 0225h, 3026h, 0327h
	dw	0028h, 0329h, 082ah, 0ff2ch, 0ff2dh, 0036h, 0040h, 0041h
	dw	0042h, 4043h, 0044h, 0d54h, 0251h, 0070h, 0f50h
	dw	0055h, 0ff64h
	dw	0032h, 0035h, 0038h, 0039h, 003ah, 0ff3bh, 0ff3ch, 0ff3dh
;
Mode1024x768x8		label	word
	dw	0150h, 0050h, 9d10h, 0011h, 7f12h, 0013h, 7f14h, 0015h
	dw	9d16h, 0017h, 8718h, 0019h, 9c1ah, 001bh, 401ch, 041eh
	dw	3020h, 0321h, 0ff22h, 0223h, 0ff24h, 0225h, 3026h, 0327h
	dw	0028h, 0329h, 082ah, 0ff2ch, 0ff2dh, 0036h, 0040h, 0041h
	dw	0042h, 8043h, 0044h, 0d54h, 0351h, 0070h, 0f50h
	dw	0055h, 0ff64h
	dw	0032h, 0035h, 0038h, 0039h, 003ah, 0ff3bh, 0ff3ch, 0ff3dh
;
;Following are mode tables for modes which are only supported on the
;XGA/2:
;
Mode800x600x8x60	label	word
	dw	0150h, 0050h, 8310h, 0011h, 6312h, 0013h, 6314h, 0015h
	dw	8316h, 0017h, 6b18h, 0019h, 7b1ah, 001bh, 201ch, 021eh
	dw	7320h, 0221h, 5722h, 0223h, 5724h, 0225h, 7326h, 0227h
	dw	5828h, 0229h, 5c2ah, 0ff2ch, 0ff2dh, 0036h, 0040h, 0041h
	dw	0042h, 6443h, 0044h, 0154h, 0070h, 0351h, 4f58h, 8054h
	dw	0750h, 0055h, 0060h, 0061h, 0062h, 0063h, 0ff64h
	dw	0032h, 0035h, 0038h, 0039h, 003ah, 0ff3bh, 0ff3ch, 0ff3dh
XGA2ModeTableEntryLen	    equ     ($ - OFFSET32 Mode800x600x8x60)/2
;
Mode800x600x8x72	label	word
	dw	0150h, 0050h, 8110h, 0011h, 6312h, 0013h, 6314h, 0015h
	dw	8116h, 0017h, 7018h, 0019h, 7f1ah, 001bh, 001ch, 001eh
	dw	9920h, 0221h, 5722h, 0223h, 5724h, 0225h, 9926h, 0227h
	dw	7c28h, 0229h, 822ah, 0ff2ch, 0ff2dh, 0036h, 0040h, 0041h
	dw	0042h, 6443h, 0044h, 0154h, 0070h, 0351h, 6358h, 8154h
	dw	0750h, 0055h, 0060h, 0061h, 0062h, 0063h, 0ff64h
	dw	0032h, 0035h, 0038h, 0039h, 003ah, 0ff3bh, 0ff3ch, 0ff3dh
;
Mode1024x768x8x60	label	word
	dw	0150h, 0050h, 0a710h, 0011h, 7f12h, 0013h, 7f14h, 0015h
	dw	0a716h, 0017h, 8818h, 0019h, 991ah, 001bh, 001ch, 001eh
	dw	2520h, 0321h, 0ff22h, 0223h, 0ff24h, 0225h, 2526h, 0327h
	dw	0228h, 0329h, 082ah, 0ff2ch, 0ff2dh, 0036h, 0040h, 0041h
	dw	0042h, 8043h, 0044h, 0154h, 0070h, 0351h, 8058h, 8154h
	dw	0c750h, 0055h, 0060h, 0061h, 0062h, 0063h, 0ff64h
	dw	0032h, 0035h, 0038h, 0039h, 003ah, 0ff3bh, 0ff3ch, 0ff3dh
;
Mode1024x768x8x72	label	word
	dw	0150h, 0050h, 0a910h, 0011h, 7f12h, 0013h, 7f14h, 0015h
	dw	0a916h, 0017h, 8618h, 0019h, 0a91ah, 001bh, 001ch, 001eh
	dw	2620h, 0321h, 0ff22h, 0223h, 0ff24h, 0225h, 2626h, 0327h
	dw	0ff28h, 0229h, 072ah, 0ff2ch, 0ff2dh, 0036h, 0040h, 0041h
	dw	0042h, 8043h, 0044h, 0154h, 0070h, 0351h, 8e58h, 8154h
	dw	0750h, 0055h, 0060h, 0061h, 0062h, 0063h, 0ff64h
	dw	0032h, 0035h, 0038h, 0039h, 003ah, 0ff3bh, 0ff3ch, 0ff3dh
;
Mode1024x768x8x75	label	word
	dw	0150h, 0050h, 0af10h, 0011h, 7f12h, 0013h, 7f14h, 0015h
	dw	0af16h, 0017h, 8618h, 0019h, 0ae1ah, 001bh, 001ch, 001eh
	dw	2520h, 0321h, 0ff22h, 0223h, 0ff24h, 0225h, 2526h, 0327h
	dw	0ff28h, 0229h, 072ah, 0ff2ch, 0ff2dh, 0036h, 0040h, 0041h
	dw	0042h, 8043h, 0044h, 0154h, 0070h, 0351h, 9558h, 8154h
	dw	0750h, 0055h, 0060h, 0061h, 0062h, 0063h, 0ff64h
	dw	0032h, 0035h, 0038h, 0039h, 003ah, 0ff3bh, 0ff3ch, 0ff3dh
;
Mode1280x1024x8 	label	word
	dw	0150h, 0050h, 0d110h, 0011h, 9f12h, 0013h, 9f14h, 0015h
	dw	0d116h, 0017h, 0a718h, 0019h, 0cf1ah, 001bh, 001ch, 001eh
	dw	4420h, 0421h, 0ff22h, 0323h, 0ff24h, 0325h, 4426h, 0427h
	dw	0228h, 0429h, 0a2ah, 0ff2ch, 0ff2dh, 0036h, 0040h, 0041h
	dw	0042h, 0a043h, 0044h, 0154h, 0070h, 0351h, 9158h, 8154h
	dw	4f50h, 0055h, 0060h, 0061h, 0062h, 0063h, 0ff64h
	dw	0032h, 0035h, 0038h, 0039h, 003ah, 0ff3bh, 0ff3ch, 0ff3dh
;
;Following are the tables for the IIT AGX:
;
AGXModeLookupTable	label	dword
Mode100H_AGX  VESAModeEntry <OFFSET32 Mode640x400x8, ModeTableEntryLen, 8, 640, 640, 400>
Mode101H_AGX  VESAModeEntry <OFFSET32 Mode640x480x8_AGX, ModeTableEntryLen_AGX, 8, 640, 1024, 480>
Mode102H_AGX  VESAModeEntry <>
Mode103H_AGX  VESAModeEntry <OFFSET32 Mode800x600x8_AGX, ModeTableEntryLen_AGX, 8, 800, 1024, 600>
Mode104H_AGX  VESAModeEntry <OFFSET32 Mode1024x768x4_AGX, ModeTableEntryLen_AGX, 4, 1024, 512, 768>
Mode105H_AGX  VESAModeEntry <OFFSET32 Mode1024x768x8_AGX, ModeTableEntryLen_AGX, 8, 1024, 1024, 768>
Mode106H_AGX  VESAModeEntry <OFFSET32 Mode1280x1024x4_AGX, ModeTableEntryLen_AGX, 8, 1280, 640, 1024>
Mode107H_AGX  VESAModeEntry <OFFSET32 Mode1280x960x8_AGX, ModeTableEntryLen_AGX, 8, 1280, 2048, 960>
Mode108H_AGX  VESAModeEntry <>
Mode109H_AGX  VESAModeEntry <>
Mode10AH_AGX  VESAModeEntry <>
Mode10BH_AGX  VESAModeEntry <>
Mode10CH_AGX  VESAModeEntry <>
Mode10DH_AGX  VESAModeEntry <>
Mode10EH_AGX  VESAModeEntry <>
Mode10FH_AGX  VESAModeEntry <>
Mode110H_AGX  VESAModeEntry <>
Mode111H_AGX  VESAModeEntry <OFFSET32 Mode640x480x16_AGX, ModeTableEntryLen_AGX, 16, 640, 2048, 480>
Mode112H_AGX  VESAModeEntry <>
Mode113H_AGX  VESAModeEntry <>
Mode114H_AGX  VESAModeEntry <OFFSET32 Mode800x600x16_AGX, ModeTableEntryLen_AGX, 16, 800, 1600, 600>
Mode115H_AGX  VESAModeEntry <>
Mode116H_AGX  VESAModeEntry <>
Mode117H_AGX  VESAModeEntry <OFFSET32 Mode1024x768x16_AGX, ModeTableEntryLen_AGX, 16, 1024, 2048, 768>
;
;
Mode640x480x8_AGX	label	word
	dw	0150h, 0050h, 5F10h, 0011h, 4f12h, 0013h, 4f14h, 0015h
	dw	5F16h, 0017h, 5518h, 0019h, 611ah, 001bh, 001ch, 001eh
	dw	0c20h, 0221h,0df22h, 0123h, 0df24h, 0125h, 0c26h, 0227h
	dw	0ea28h, 0129h,0ec2ah,0ff2ch,0ff2dh, 0036h, 0040h, 0041h
	dw	0042h, 8043h, 0044h, 0054h, 0351h, 0070h,0c750h, 0055h
	dw	307fh, 036Dh,0DF6Eh, 416Fh, 0477h
ModeTableEntryLen_AGX	equ	($ - OFFSET32 Mode640x480x8_AGX)/2
;
Mode640x480x16_AGX	label	word
	dw	0150h, 0050h, 5f10h, 0011h, 4f12h, 0013h, 4f14h, 0015h
	dw	5f16h, 0017h, 5518h, 0019h, 611ah, 001bh, 001ch, 001eh
	dw	0c20h, 0221h,0df22h, 0123h,0df24h, 0125h, 0c26h, 0227h
	dw	0ea28h, 0129h,0ec2ah,0ff2ch,0ff2dh, 0036h, 0040h, 0041h
	dw	0042h, 0043h, 0144h, 0d54h, 0451h, 0070h,0c750h, 0055h
	dw	307fh, 036Dh, 3F6Eh, 556fh, 2277h
;
Mode800x600x8_AGX	label	word
	dw	0150h, 0050h, 8010h, 0011h, 6312h, 0013h, 6314h, 0015h
	dw	8016h, 0017h, 6e18h, 0019h, 781ah, 001bh, 001ch, 001eh
	dw	7320h, 0221h, 5722h, 0223h, 5724h, 0225h, 7326h, 0227h
	dw	5828h, 0229h, 1c2ah,0ff2ch, 072dh, 0036h, 0040h, 0041h
	dw	0042h, 8043h, 0044h, 0d54h, 0351h, 0070h, 0750h, 0055h
	dw	127fh, 036Dh, 9F6Eh, 516fh, 0477h
;
Mode800x600x16_AGX	label	word
	dw	0150h, 0050h, 0fc10h, 0011h, 0c712h, 0013h, 0c714h, 0015h
	dw	0fc16h, 0017h, 0d118h, 0019h, 0dd1ah, 001bh, 001ch, 001eh
	dw	8020h, 0221h, 5722h, 0223h, 5724h, 0225h, 8026h, 0227h
	dw	5e28h, 0229h, 0d2ah, 0ff2ch, 0ff2dh, 0036h, 0040h, 0041h
	dw	0042h, 0c843h, 0044h, 0d54h, 0351h, 0070h, 0750h, 0055h
	dw	127fh, 036Dh, 9F6Eh, 156fh, 0077h
;
Mode1024x768x4_AGX	label	word
	dw	0150h, 0050h,0a410h, 0011h, 7f12h, 0013h, 7f14h, 0015h
	dw	0a416h, 0017h, 8a18h, 0019h, 931ah, 001bh, 401ch, 041eh
	dw	2520h, 0321h,0ff22h, 0223h,0ff24h, 0225h, 2526h, 0327h
	dw	0028h, 0329h, 082ah,0ff2ch,0ff2dh, 0036h, 0040h, 0041h
	dw	0042h, 4043h, 0044h, 0d54h, 0251h, 0070h, 0f50h, 0055h
	dw	327fh, 036Dh, 1F6Eh, 556fh, 3477h
;
Mode1024x768x8_AGX	label	word
	dw	0150h, 0050h,0a410h, 0011h, 7f12h, 0013h, 7f14h, 0015h
	dw	0a416h, 0017h, 8a18h, 0019h, 931ah, 001bh, 401ch, 041eh
	dw	2520h, 0321h,0ff22h, 0223h,0ff24h, 0225h, 2526h, 0327h
	dw	0028h, 0329h, 082ah,0ff2ch,0ff2dh, 0036h, 0040h, 0041h
	dw	0042h, 8043h, 0044h, 0d54h, 0351h, 0070h, 1750h, 0055h
	dw	327fh, 036Dh, 5F6Eh, 656fh, 3477h
;
Mode1024x768x16_AGX	 label	 word
	dw	0150h, 0050h,0a710h, 0011h, 7f12h, 0013h, 7f14h, 0015h
	dw	0a716h, 0017h, 8218h, 0019h, 931ah, 001bh, 401ch, 041eh
	dw	2520h, 0321h,0ff22h, 0223h,0ff24h, 0225h, 2526h, 0327h
	dw	0228h, 0329h, 082ah, 0ff2ch, 072dh, 0036h, 0040h, 0041h
	dw	0042h, 0043h, 0144h, 0d54h, 0451h, 0070h, 0b50h, 0055h
	dw	007fh, 036Dh, 5c6Eh, 746fh, 2677h
;
Mode1280x1024x4_AGX	label	word
	dw	0150h, 0050h,0cc10h, 0011h, 9f12h, 0013h, 9f14h, 0015h
	dw	0cc16h, 0017h,0b018h, 0019h,0be1ah, 001bh, 401ch, 001eh
	dw	0e020h, 0321h,0bf22h, 0323h,0bf24h, 0325h, 0e026h, 0327h
	dw	0c128h, 0329h, 052ah,0ff2ch, 072dh, 0036h, 0040h, 0041h
	dw	0042h, 0043h, 0144h, 0d54h, 0351h, 0070h, 0050h, 0055h
	dw	367fh, 036Dh, 016Eh, 776fh, 1477h
;
Mode1280x960x8_AGX	label	word
	dw	0150h, 0050h,0cc10h, 0011h, 9f12h, 0013h, 9f14h, 0015h
	dw	0cc16h, 0017h,0b018h, 0019h,0be1ah, 001bh, 401ch, 001eh
	dw	0e020h, 0321h,0bf22h, 0323h,0bf24h, 0325h, 0e026h, 0327h
	dw	0c128h, 0329h, 052ah,0ff2ch, 072dh, 0036h, 0040h, 0041h
	dw	0042h, 0043h, 0144h, 0d54h, 0351h, 0070h, 0050h, 0055h
	dw	367fh, 036Dh, 016Eh, 776fh, 2477h
;
Palette_16BPP		label	byte
	db	0,0,0 SHL 2
	db	0,0,2 SHL 2
	db	0,0,4 SHL 2
	db	0,0,6 SHL 2
	db	0,0,8 SHL 2
	db	0,0,10 SHL 2
	db	0,0,12 SHL 2
	db	0,0,14 SHL 2
	db	0,0,16 SHL 2
	db	0,0,18 SHL 2
	db	0,0,20 SHL 2
	db	0,0,22 SHL 2
	db	0,0,24 SHL 2
	db	0,0,26 SHL 2
	db	0,0,28 SHL 2
	db	0,0,30 SHL 2
	db	0,0,32 SHL 2
	db	0,0,34 SHL 2
	db	0,0,36 SHL 2
	db	0,0,38 SHL 2
	db	0,0,40 SHL 2
	db	0,0,42 SHL 2
	db	0,0,44 SHL 2
	db	0,0,46 SHL 2
	db	0,0,48 SHL 2
	db	0,0,50 SHL 2
	db	0,0,52 SHL 2
	db	0,0,54 SHL 2
	db	0,0,56 SHL 2
	db	0,0,58 SHL 2
	db	0,0,60 SHL 2
	db	0,0,62 SHL 2

	db	0,0,0 SHL 2
	db	0,0,2 SHL 2
	db	0,0,4 SHL 2
	db	0,0,6 SHL 2
	db	0,0,8 SHL 2
	db	0,0,10 SHL 2
	db	0,0,12 SHL 2
	db	0,0,14 SHL 2
	db	0,0,16 SHL 2
	db	0,0,18 SHL 2
	db	0,0,20 SHL 2
	db	0,0,22 SHL 2
	db	0,0,24 SHL 2
	db	0,0,26 SHL 2
	db	0,0,28 SHL 2
	db	0,0,30 SHL 2
	db	0,0,32 SHL 2
	db	0,0,34 SHL 2
	db	0,0,36 SHL 2
	db	0,0,38 SHL 2
	db	0,0,40 SHL 2
	db	0,0,42 SHL 2
	db	0,0,44 SHL 2
	db	0,0,46 SHL 2
	db	0,0,48 SHL 2
	db	0,0,50 SHL 2
	db	0,0,52 SHL 2
	db	0,0,54 SHL 2
	db	0,0,56 SHL 2
	db	0,0,58 SHL 2
	db	0,0,60 SHL 2
	db	0,0,62 SHL 2

	db	0,0,0 SHL 2
	db	0,0,2 SHL 2
	db	0,0,4 SHL 2
	db	0,0,6 SHL 2
	db	0,0,8 SHL 2
	db	0,0,10 SHL 2
	db	0,0,12 SHL 2
	db	0,0,14 SHL 2
	db	0,0,16 SHL 2
	db	0,0,18 SHL 2
	db	0,0,20 SHL 2
	db	0,0,22 SHL 2
	db	0,0,24 SHL 2
	db	0,0,26 SHL 2
	db	0,0,28 SHL 2
	db	0,0,30 SHL 2
	db	0,0,32 SHL 2
	db	0,0,34 SHL 2
	db	0,0,36 SHL 2
	db	0,0,38 SHL 2
	db	0,0,40 SHL 2
	db	0,0,42 SHL 2
	db	0,0,44 SHL 2
	db	0,0,46 SHL 2
	db	0,0,48 SHL 2
	db	0,0,50 SHL 2
	db	0,0,52 SHL 2
	db	0,0,54 SHL 2
	db	0,0,56 SHL 2
	db	0,0,58 SHL 2
	db	0,0,60 SHL 2
	db	0,0,62 SHL 2

	db	0,0,0 SHL 2
	db	0,0,2 SHL 2
	db	0,0,4 SHL 2
	db	0,0,6 SHL 2
	db	0,0,8 SHL 2
	db	0,0,10 SHL 2
	db	0,0,12 SHL 2
	db	0,0,14 SHL 2
	db	0,0,16 SHL 2
	db	0,0,18 SHL 2
	db	0,0,20 SHL 2
	db	0,0,22 SHL 2
	db	0,0,24 SHL 2
	db	0,0,26 SHL 2
	db	0,0,28 SHL 2
	db	0,0,30 SHL 2
	db	0,0,32 SHL 2
	db	0,0,34 SHL 2
	db	0,0,36 SHL 2
	db	0,0,38 SHL 2
	db	0,0,40 SHL 2
	db	0,0,42 SHL 2
	db	0,0,44 SHL 2
	db	0,0,46 SHL 2
	db	0,0,48 SHL 2
	db	0,0,50 SHL 2
	db	0,0,52 SHL 2
	db	0,0,54 SHL 2
	db	0,0,56 SHL 2
	db	0,0,58 SHL 2
	db	0,0,60 SHL 2
	db	0,0,62 SHL 2
EndProc VESADataTables
;
;
subttl		Dispatch To Proper VESA Handling Routine
page +
public	MiniVDD_VESASupport
BeginProc	MiniVDD_VESASupport, DOSVM
;
;Entry:
;	AX contains the VESA function code.
;	EBX contains the VM handle that wants to do the VESA call.
;	EBP --> VM's Client Registers.
;Exit:
;	CY returned means that MiniVDD has handled the VESA call completely.
;
;This is the dispatch point for VESA handling on the XGA under Win '95.  We
;dispatch to each routine as defined by the VESA spec.
;
	pushad				;
;
;If the application calls VESA function 4F13H -- the VESA audio interface
;(specifically, Nickelodeon's "Are You Afraid of the Dark" app calls this),
;we want to chain to the VESA.COM hopefully provided by the application since
;we assume that if it's calling 4F13H, it knows what it's doing.  Therefore,
;we return that we didn't handle the call by setting CY clear:
;
	cmp	al,13h			;calling VESA sound interface 4F13H?
	je	MVVSExit		;yes, set CY clear -- we don't handle it
	cmp	al,LAST_VESA_FUNC	;is it a valid function?
	ja	MVVSBadFunctionCode	;nope, just return failure to caller
	movzx	eax,al			;make sure top word of EAX is 0
	call	[(eax*4)+OFFSET32 VESADispatchTable]
					;call the function to do
	jmp	MVVSWeHandledCall	;and we're done!
;
public	MVVSBadFunctionCode
MVVSBadFunctionCode:
	mov	[ebp].Client_AX,0100h	;indicate that function code is bad
;
public	MVVSWeHandledCall
MVVSWeHandledCall:
	stc				;indicate we handled VESA call
;
public	MVVSExit
MVVSExit:
	popad				;
	ret				;and return to caller
EndProc MiniVDD_VESASupport
;
;
subttl		Support For VESA Function 0 -- Return Controller Info
page +
public	VESAFunc0
BeginProc VESAFunc0, DOSVM
;
;Entry:
;	EBX contains the VM handle.
;	EBP --> Client Registers.
;	Client_ES:Client_DI --> Seg:Off addr of buffer to stuff data into.
;Exit:
;	Client_AX contains the return status from the call.
;
;First, get the linear address of the guy's VBE information block:
;
	mov	ah,Client_ES		;get client register equates in
	mov	al,Client_DI		;
	VMMCall Map_Flat		;EAX contains the linear address
	cmp	eax,-1			;did the call fail?
	je	VF0Failure		;yep, return it to caller
;
;Check to see if he wants VBE 2.0 data or VBE 1.0 data by seeing the
;VbeSignature that he passed down.  If it's 'VBE2', then he's passed a
;512 byte buffer and wants VBE 2.0 extensions.	If not, we assume that
;he only wants VBE 1.0 stuff.
;
	cld				;set direction forward
	mov	cl,1			;assume he wants only VBE 1.0 stuff
	mov	edx,dword ptr [eax].VbeSignature
					;get the VbeSignature string
	cmp	dl,'V'			;is it what we want?
	je	@F			;yes, continue
	cmp	dl,'v'			;is it what we want?
	jne	VF0GotVersion		;nope, he wants VBE 1.0
;
@@:	cmp	dh,'B'			;is it what we want?
	je	@F			;yes, continue
	cmp	dh,'b'			;is it what we want?
	jne	VF0GotVersion		;nope, he wants VBE 1.0
;
@@:	shr	edx,16			;get access to high word
	cmp	dl,'E'			;is it what we want?
	je	@F			;yes, continue
	cmp	dl,'e'			;is it what we want?
	jne	VF0GotVersion		;nope, he wants VBE 1.0
;
@@:	cmp	dh,'2'			;is it what we want?
	jne	VF0GotVersion		;nope, he wants VBE 1.0
	mov	cl,2			;yes! he wants VBE 2.0
;
public	VF0GotVersion
VF0GotVersion:
;
;At this point:
;	CL contains 1 for VBE 1.0 info or 2 for VBE 2.0 info.
;
;Now, put the string "VESA" into VbeSignature:
;
	mov	dword ptr [eax].VbeSignature,'ASEV'
;
;And the version into VbeVersion:
;
	mov	[eax].VbeVersion,0200h
;
;Put the string 'XGA' into the OemStringPtr area and establish a pointer
;to it.
;
	mov	si,[ebp].Client_ES	;get segment addr of OemString
	shl	esi,16			;
	lea	edx,[eax].VbeV1Scratch	;assume caller passed a VBE1 structure
	mov	si,VbeV1Scratch 	;
	cmp	cl,1			;did he pass a VBE 1 structure?
	je	@F			;yes, EDX --> place to put string
	lea	edx,[eax].VESAOemData	;it's a VBE 2 structure
	mov	si,VESAOemData		;
@@:	mov	[eax].OemStringPtr,esi	;stuff the address of the Oem string
	mov	dword ptr [edx],'AGX'	;
	mov	byte ptr [edx+3],0	;NULL terminate the string
;
;Set the capabilities for the XGA as 6 bit fixed DAC, NOT VGA compatible, Normal
;RAMDAC operation:
;
	mov	dword ptr [eax].VESACaps,02h
;
;Now, construct the video mode table.  For VESA 1.0 structures, we place this
;in the fourth byte of the VbeV1Scratch area.  For VESA 2.0 structures,
;we place this in the VESAReserved area.
;
	push	ecx			;save the structure type in CL
	mov	si,[ebp].Client_ES	;get segment addr of Modes list
	shl	esi,16			;
	lea	edi,[eax].VbeV1Scratch+4;assume caller passed a VBE1 structure
	mov	si,VbeV1Scratch+4	;
	cmp	cl,1			;did he pass a VBE 1 structure?
	je	@F			;yes, EDI --> place to put Modes list
	lea	edi,[eax].VESAReserved	;it's a VBE 2 structure
	mov	si,VESAReserved 	;
@@:	mov	[eax].VideoModePtr,esi	;stuff the address of the Modes list
	mov	esi,OFFSET32 VESAModesTable
	mov	ecx,VESAModesTableLen	;
	cmp	ChipID,AGX_ID		;running on the AGX?
	jne	@F			;nope, we're setup for the XGA
	mov	esi,OFFSET32 VESAModesTable_AGX
	mov	ecx,VESAModesTableLen_AGX
@@:	rep	movsb			;
	pop	ecx			;restore structure type to CL
;
;Next, the VESA total memory size in number of 64K blocks available:
;
	mov	edx,TotalMemorySize	;
	shr	edx,16			;divide by 64K
	mov	[eax].VESATotalMemory,dx;
;
;If we were passed a VESA 1 structure, we're done!
;
	cmp	cl,1			;passed a VBE 1.0 structure?
	je	VF0GoodExit		;yes, we're done!
;
;Fill in the OemSoftwareRev as VBE version 2.0:
;
	mov	[eax].OemSoftwareRev,0200h
;
;We're pretty sure that VESA authors don't care too much about the
;vendor name, product name, or OEM Product rev information.  Just pass
;back NULL pointers for this:
;
	mov	[eax].OemVendorNamePtr,0
	mov	[eax].OemProductNamePtr,0
	mov	[eax].OemProductRevPtr,0
;
public	VF0GoodExit
VF0GoodExit:
	mov	[ebp].Client_AX,4fh	;return success!
	jmp	VF0Exit 		;
;
public	VF0Failure
VF0Failure:
	mov	[ebp].Client_AX,014fh	;return failure to caller
;
VF0Exit:
	ret				;return to dispatch routine
EndProc VESAFunc0
;
;
subttl		Support For VESA Function 1 -- Return Mode Info
page +
public	VESAFunc1
BeginProc VESAFunc1, DOSVM
;
;Entry:
;	EBX contains the VM handle.
;	EBP --> Client Registers.
;	Client_CX contains the mode number to query.
;	Client_ES:Client_DI --> Seg:Off addr of buffer to stuff data into.
;Exit:
;	Client_AX contains the return status from the call.
;
;First, get the linear address of the guy's VBE information block:
;
	mov	ah,Client_ES		;get client register equates in
	mov	al,Client_DI		;
	VMMCall Map_Flat		;EAX contains the linear address
	cmp	eax,-1			;did the call fail?
	je	VF1Failure		;yep, return it to caller
;
;Clear the mode information block to all zeroes:
;
	cld				;set direction forward
	push	eax			;save EAX --> Mode Information Block
	mov	edi,eax 		;EDI --> Mode Information Block
	xor	eax,eax 		;
	mov	ecx,256/4		;clear 256 bytes
	rep	stosd			;
;
;See if this mode is one of those that we might support:
;
	mov	ax,[ebp].Client_CX	;AX has the mode that we want
	and	ax,NOT 0c080h		;get rid of "don't erase screen" bits
	mov	ecx,NbrVESAModes	;this is nbr that we potentially support
	mov	edi,OFFSET32 VESAModesTable
	cmp	ChipID,AGX_ID		;running on the IIT AGX?
	jne	@F			;nope, we're setup for the XGA
	mov	ecx,NbrVESAModes_AGX	;this is nbr that we potentially support
	mov	edi,OFFSET32 VESAModesTable_AGX
@@:	repne	scasw			;
	movzx	ecx,ax			;copy mode number into ECX
	pop	eax			;restore EAX --> Mode Information Block
	jne	VF1Failure		;mode wasn't in table, fail the call
;
;We setup the default attributes for the Mode Attributes flag.	We also
;assume (until proven otherwise), that we support the mode.
;
	or	[eax].VModeAttributes,3bh
;
;If we're running on an XGA/1, we can't do 800x600:
;
	cmp	cl,03h			;trying to run 800x600?
	jne	VF1CheckMemory		;nope, we're OK so far
	cmp	ChipID,XGA1_ID		;running on an XGA/1?
	jne	VF1CheckMemory		;nope, we're OK so far
;
public	VF1ModeNotSupported
VF1ModeNotSupported:
	and	byte ptr [eax].VModeAttributes,NOT 01h
					;yes, can't do 800x600 on XGA/1!
	jmp	VF1FillInWinAttributes	;and continue
;
public	VF1CheckMemory
VF1CheckMemory:
;
;If we're running in 512K, we can only do 640x480, 800x600, & 1024x768x4:
;
	cmp	cl,04h			;running above 1024x768x4?
	jbe	VF1FillInWinAttributes	;nope, we can run on 512K
	cmp	TotalMemorySize,1024*1024
					;do we have at least 1 meg?
	jb	VF1ModeNotSupported	;nope, we can't support this mode
	cmp	cl,07h			;running 1280x1024x8?
	je	VF1CheckFor2Megs	;yes, go check for 2 megs
	cmp	cl,14h			;running 800x600x16 on the AGX?
	je	VF1CheckFor2Megs	;yes, go check for 2 megs
	cmp	cl,17h			;running 1024x768x16 on the AGX?
	jne	VF1FillInWinAttributes	;nope, we can support this mode
;
public	VF1CheckFor2Megs
VF1CheckFor2Megs:
	cmp	TotalMemorySize,2*1024*1024
					;do we have 2 megs?
	jb	VF1ModeNotSupported	;nope, can't support this mode
;
public	VF1CheckAGXOnlyModes
VF1CheckAGXOnlyModes:
	cmp	ChipID,AGX_ID		;running on an IIT AGX?
	je	VF1FillInWinAttributes	;
	cmp	cl,11h			;running above mode 111H?
	ja	VF1ModeNotSupported	;yes, can't run these on the XGA!
;
public	VF1FillInWinAttributes
VF1FillInWinAttributes:
;
;We only have one "relocatable window", that is, we have only one banking
;register:
;
	mov	[eax].VWinAAttributes,07h
	mov	[eax].VWinBAttributes,00h
	mov	[eax].VWinGranularity,64;we align banks on 64K boundries
	mov	[eax].VWinSize,64	;our banks are 64K each
	mov	[eax].VWinASegment,0a000h
					;we bank memory in at A000H
	mov	[eax].VWinBSegment,0	;no "B" window
	mov	esi,VESAStubLinearAddr	;
	mov	[eax].VWinFuncPtr,esi	;give back address of callable banker
;
public	VF1FillInScreenAttributes
VF1FillInScreenAttributes:
;
;Retrieve resolution and BPP information from the mode table:
;
	movzx	esi,cl			;get mode number (offset from 0) in ESI
	shl	esi,4			;multiply by 16 bytes per entry
.errnz	SIZE VESAModeEntry - 16
	mov	edi,OFFSET32 VESAModeLookupTable
	cmp	ChipID,AGX_ID		;running on the IIT AGX?
	jne	@F			;nope, we have the correct table
	mov	edi,OFFSET32 AGXModeLookupTable
@@:	add	edi,esi 		;EDI --> entry for our mode
	mov	dx,[edi].ScreenWidthInPixels
	mov	[eax].VXResolution,dx	;
	mov	dx,[edi].ScreenPitch	;
	mov	[eax].VBytesPerScanLine,dx
	mov	dx,[edi].ScreenHeight	;
	mov	[eax].VYResolution,dx	;
	mov	dl,[edi].ScreenBPP	;
	mov	[eax].VBitsPerPixel,dl	;
;
;Fill in other miscallaneous fields:
;
	mov	[eax].VXCharSize,0	;these are graphics only modes
	mov	[eax].VYCharSize,0	;
	mov	[eax].VNbrOfPlanes,1	;we are running packed pixel
	mov	[eax].VMemoryModel,04h	;we're packed pixel memory model
	mov	[eax].VNbrOfBanks,1	;
	mov	[eax].VBankSize,0	;
	mov	[eax].VNbrOfImagePages,0;no hardware panning
	mov	[eax].VReserved,1	;
;
;Now, fill in the RGB information for the 16 BPP mode:
;
	cmp	cl,11h			;running in 640x480x16 BPP mode?
	jne	@F			;nope, no need to fill in RGB info
	mov	[eax].VRedMaskSize,5	;
	mov	[eax].VRedFieldPosition,11
	mov	[eax].VGreenMaskSize,6	;
	mov	[eax].VGreenFieldPosition,5
	mov	[eax].VBlueMaskSize,5	;
	mov	[eax].VBlueFieldPosition,0
;
;And lastly, the off-screen information:
;
@@:	push	eax			;save EAX --> Mode Info block
	movzx	edx,[eax].VBytesPerScanLine
	movzx	eax,[eax].VYResolution	;
	mul	edx			;EAX has total bytes on-screen
	mov	edx,eax 		;save this in EDX
	pop	eax			;restore EAX --> Mode Info block
	mov	[eax].VOffScreenMemOffset,edx
					;this is start of off-screen memory
	mov	ecx,TotalMemorySize	;
	sub	ecx,edx 		;this is off-screen memory available
	shr	ecx,10			;get it in kilobytes
	mov	[eax].VOffScreenMemSize,cx
;
public	VF1GoodExit
VF1GoodExit:
	mov	[ebp].Client_AX,4fh	;return success!
	jmp	VF1Exit 		;
;
public	VF1Failure
VF1Failure:
	mov	[ebp].Client_AX,014fh	;return failure to caller
;
VF1Exit:
	ret				;return to dispatch routine
EndProc VESAFunc1
;
;
subttl		Support For VESA Function 2 -- Set Mode
page +
public	VESAFunc2
BeginProc VESAFunc2, DOSVM
;
;Entry:
;	EBX contains the VM handle.
;	EBP --> Client Registers.
;	Client_BX contains the mode number to set.
;Exit:
;	Client_AX contains the return status from the call.
;
;See if this mode is one of those that we might support:
;
	cld				;set direction forward
	movzx	eax,[ebp].Client_BX	;get the passed in mode number
	test	ah,04h			;trying to use a linear frame buffer?
	jnz	VF2Failure		;yes, we don't support them!
	and	ax,NOT 0c080h		;get rid of flag bits in mode nbr
	cmp	ax,13h			;is it a standard VGA mode?
	jbe	VF2StandardVGAMode	;yes, just go do an INT 10H, function 0
	mov	ecx,NbrVESAModes	;this is nbr that we potentially support
	mov	edi,OFFSET32 VESAModesTable
	cmp	ChipID,AGX_ID		;running on the IIT AGX?
	jne	@F			;nope, we're setup for the XGA
	mov	ecx,NbrVESAModes_AGX	;this is nbr that we potentially support
	mov	edi,OFFSET32 VESAModesTable_AGX
@@:	repne	scasw			;
	jne	VF2Failure		;mode wasn't in table, fail the call
;
;If we're running on an XGA/1, we can't do 800x600:
;
	cmp	al,03h			;trying to run 800x600?
	jne	VF2CheckMemory		;nope, we're OK so far
	cmp	ChipID,XGA1_ID		;running on an XGA/1?
	je	VF2Failure		;yes, we can't do 800x600
;
public	VF2CheckMemory
VF2CheckMemory:
;
;If we're running in 512K, we can only do 640x480, 800x600, & 1024x768x4:
;
	cmp	al,04h			;running above 1024x768x4?
	jbe	VF2GetModeTable 	;nope, we can run on 512K
	cmp	TotalMemorySize,1024*1024
					;do we have at least 1 meg?
	jb	VF2Failure		;nope, we can't support this mode
	cmp	al,07h			;running 1280x1024x8?
	je	VF2CheckFor2Megs	;yes, go make sure we have 2 megs of RAM
	cmp	al,14h			;running 800x600x16?
	je	VF2CheckFor2Megs	;yes, go make sure we have 2 megs of RAM
	cmp	al,17h			;running 1024x768x16?
	jne	VF2GetModeTable 	;nope, we can support this mode
;
public	VF2CheckFor2Megs
VF2CheckFor2Megs:
	cmp	TotalMemorySize,2*1024*1024
					;do we have 2 megs?
	jb	VF2Failure		;nope, we can't support this mode
;
public	VF2CheckAGXOnlyModes
VF2CheckAGXOnlyModes:
	cmp	ChipID,AGX_ID		;running on an IIT AGX?
	je	VF2GetModeTable 	;yes, we can support the mode
	cmp	al,11h			;running above mode 111H?
	ja	VF2Failure		;yes, can't run these on the XGA!
;
public	VF2GetModeTable
VF2GetModeTable:
	sub	eax,100h		;make mode number offset from 100H
	shl	eax,4			;multiply by 16 bytes per entry
.errnz	SIZE VESAModeEntry - 16
	cmp	ChipID,AGX_ID		;running on an IIT AGX?
	jne	@F			;nope, use XGA lookup tables
	mov	cl,[ebp].Client_BL	;get the low byte of the mode number
	cmp	cl,04h			;running at 1024x768x4?
	je	@F			;yes, use XGA lookup table for now
	cmp	cl,05h			;running at 1024x768x8?
	je	@F			;yes, use XGA lookup table for now
	mov	esi,[eax+OFFSET32 AGXModeLookupTable].ModeTableAddr
					;ESI --> mode table to set
	movzx	ecx,[eax+OFFSET32 AGXModeLookupTable].ModeTableLen
					;ECX contains nbr of words in mode table
	jmp	VF2SetupMode		;
@@:	mov	esi,[eax+OFFSET32 VESAModeLookupTable].ModeTableAddr
					;ESI --> mode table to set
	movzx	ecx,[eax+OFFSET32 VESAModeLookupTable].ModeTableLen
					;ECX contains nbr of words in mode table
;
public	VF2AdjustForRefreshRate
VF2AdjustForRefreshRate:
;
;We only want to adjust the refresh rate for the Windows driver.  VESA mode
;apps get the lowest common denominator.
;
	cmp	ebx,WindowsVMHandle	;running in the Windows VM?
	jne	VF2SetupMode		;nope, just use defaults for DOS boxes
;
;The XGA/1 had no refresh rate adjustment:
;
	cmp	ChipID,XGA1_ID		;running on an XGA/1?
	je	VF2SetupMode		;yes, no refresh rate adjust possible
;
;If we reach this point, we've either got an XGA/2 or an IIT AGX running
;1024x768x8 or 1024x768x4.  If the refresh rate is set to 60 Hz or less, we
;use what we've got since it's the lowest common denominator:
;
	cmp	RefreshRate,60		;is refresh rate set to below 60?
	jb	VF2SetupMode		;yes, no need to change what we've got
;
;On the AGX, if we're running 72 Hz, we use the AGX tables for 1024x768.
;Otherwise, we stick with the interlaced table for the XGA/1 which will give
;a compatible interlaced mode on the AGX:
;
	movzx	eax,[ebp].Client_BL	;get low byte of mode number
	cmp	ChipID,AGX_ID		;running on the IIT AGX at 1024x768?
	jne	@F			;nope, must be on the XGA/2
	cmp	RefreshRate,72		;can he do 72 Hz?
	jb	VF2SetupMode		;nope, just use the XGA/1's table
	shl	eax,4			;multiply by 16 bytes per entry
.errnz	SIZE VESAModeEntry - 16
	mov	esi,[eax+OFFSET32 AGXModeLookupTable].ModeTableAddr
					;ESI --> mode table to set
	movzx	ecx,[eax+OFFSET32 AGXModeLookupTable].ModeTableLen
					;ECX contains nbr of words in mode table
	jmp	VF2SetupMode		;
@@:	cmp	al,03h			;running 800x600?
	jne	@F			;nope, try 1024x768
	cmp	RefreshRate,72		;can he do 72 Hz?
	jb	VF2SetupMode		;nope, we're already set for 60 Hz
	mov	esi,OFFSET32 Mode800x600x8x72
					;yes! set him up for 72 Hz!
	jmp	VF2SetupMode		;
@@:	cmp	al,05h			;running 1024x768x8?
	jne	VF2SetupMode		;nope, just use what we've got
	mov	esi,OFFSET32 Mode1024x768x8x60
	mov	ecx,XGA2ModeTableEntryLen
					;setup to let him do 60 Hz
	cmp	RefreshRate,72		;can he do 72 Hz?
	jb	VF2SetupMode		;nope, let him do 60 Hz
	mov	esi,OFFSET32 Mode1024x768x8x72
					;setup to let him do 72 Hz!
	cmp	RefreshRate,75		;can he do 75 Hz?
	jb	VF2SetupMode		;nope, let him do 72 Hz
	mov	esi,OFFSET32 Mode1024x768x8x75
;
public	VF2SetupMode
VF2SetupMode:
;
;When the XGA is in HiRes mode (such as we're going to do in a minute), the
;VGA registers are not readable.  This causes the Main VDD to go koo-koo and
;assign the system NULL page instead of video memory to the VM at A0000H.
;We therefore need to PRE-ASSIGN all 16 pages between A0000H and B0000H
;to our VM BEFORE we enter the XGA's HiRes mode. By doing this, we won't cause
;any page faults when the VESA app touches A0000H (in the course of its normal
;drawing procedures). Thus, all the pages will get assigned correctly, the
;Main VDD won't try to read any unreadable registers, and we'll be happy.
;
;First, set ourselves into video mode 12H.  This will make sure that the
;video hardware is setup to map at address A000:0 rather than B800:0.
;The Main VDD requires this in order to properly map the 16 pages of video
;memory to us:
;
	mov	eax,12h 		;
	push	10h			;
	VMMCall Exec_VxD_Int		;
;
;In order to cause page faults at Ring 0, we must use the VM's idea of A0000H,
;not the Ring 0 linear address of A0000H.
;
	mov	eax,[ebx+CB_High_Linear];EAX --> VM's 0:0
	add	eax,0a0000h		;EAX --> VM's A0000H
	mov	dl,16			;we need to touch 16 pages
@@:	mov	dword ptr [eax],0f0f0f0fh
					;touch the page
	add	eax,1000h		;bump to next page
	dec	dl			;done touching pages?
	jnz	@B			;nope, go touch another page
;
public	VF2SetXGAExtendedMode
VF2SetXGAExtendedMode:
	cmp	ChipID,AGX_ID		;don't do this for the AGX!
	je	@F			;we're on an AGX
	mov	edx,3c3h		;we need to disable the VGA explicitely
	xor	al,al			;
	out	dx,al			;
@@:	mov	edx,XGARegBase		;get base register determined earlier
	add	dl,04h			;DX --> Interrupt Enable Register (21x4)
	xor	al,al			;we want NO XGA interrupts enabled
	out	dx,al			;
;
	inc	dl			;DX --> Interrupt Status Register (21x5)
	mov	al,0ffh 		;set Interrupt Status to FFH
	out	dx,al			;
;
	sub	dl,05h			;DX --> Operating Mode Register (21x0)
	mov	al,04h			;set to Intel memory layout, HiRes mode
	cmp	ChipID,AGX_ID		;running on an IIT AGX?
	jne	@F			;nope, we're setup for the XGA
	mov	al,05h			;this is value for the AGX
@@:	out	dx,al			;
;
	inc	dl			;DX --> Aperture Control Register (21x1)
	mov	al,01h			;we want 64K aperture at A000:0H
	out	dx,al			;
;
	add	dl,05h			;EDX --> Virtual Memory Control (21x6)
	xor	al,al			;no virtual memory stuff required
	out	dx,al			;
;
	add	dl,02h			;EDX --> XGA banking register (21x8)
	out	dx,al			;set to bank 0
;
	inc	dl			;now EDX --> MemoryAccessMode (21x9)
	mov	al,03h			;assume 16 BPP
	cmp	[ebp].Client_BL,11h	;running 16 BPP?
	je	@F			;yes, go set it
	mov	al,0ah			;assume 4 BPP (set for nibble swapping)
	cmp	[ebp].Client_BL,04h	;running 4 BPP?
	je	@F			;yes, go set it
	mov	al,03h			;must be 8 BPP
@@:	out	dx,al			;
;
;OK, now we've setup all of the discrete registers for our running mode.  We
;now must setup the XGA's extended CRTC registers.  This is done from a table
;which is pointed to by ESI with length ECX.
;
	inc	dl			;EDX --> XGA extended CRTC index (21xA)
	rep	outs dx,word ptr [esi]	;setup the mode!
;
public	VF2Setup16BPPDAC
VF2Setup16BPPDAC:
;
;We need to setup a special DAC configuration if we're running 16 BPP:
;
	cmp	[ebp].Client_BL,11h	;running mode 111H (16 BPP)?
	jb	VF2SaveCurrentMode	;nope, don't need to do this!
	cmp	ChipID,AGX_ID		;running on an IIT AGX?
	jne	VF2SetupXGA16BPPDAC	;nope, go do XGA procedure
;
public	VF2SetupAGX16BPPDAC
VF2SetupAGX16BPPDAC:
	mov	edx,3c6h
	mov	al,0ffh
	out	dx,al
	mov	edx,216ah
	mov	al,51h			;turn on RS2
	out	dx,al
	mov	edx,3c6h
	in	al,dx
	in	al,dx
	in	al,dx
	in	al,dx
	mov	al,0e0h 		;set DAC to 5-6-5 16 BPP colour mode
	out	dx,al			;
	jmp	VF2SaveCurrentMode	;
;
public	VF2SetupXGA16BPPDAC
VF2SetupXGA16BPPDAC:
	mov	esi,OFFSET32 Palette_16BPP
	mov	cl,128			;we need to program 128 values
;
;We check the Extended CRTC Index Register 55H to see if we should load
;the first 128 palette entries or the last 128 entries.
;
;At this point:
;	EDX --> XGA Extended CRTC index register.
;
	mov	al,55h			;
	out	dx,al			;
	inc	dl			;DX --> XGA Extended CRTC data register
	in	al,dx			;
	dec	dl			;DX --> XGA Extended CRTC index register
	test	al,80h			;should we load the low 128 entries?
	mov	ch,0			;assume we start at palette entry 0
	jnz	@F			;yes, AX is set correctly
	mov	ch,128			;no, we must load the upper 128 entries
;
;First initialize the XGA's Palette Sequencer Register (index 66H) to
;RGB ordering and starting with the RED index
;
@@:	mov	ax,0066h		;initialize palette sequence reg
	out	dx,ax			;
@@:	mov	al,60h			;this is XGA Palette Index Register
	mov	ah,ch			;get next index
	out	dx,ax			;
	mov	al,65h			;this is palette data register
	mov	ah,[esi]		;get RED value
	out	dx,ax			;
	mov	ah,[esi+1]		;similarly for GREEN
	out	dx,ax			;
	mov	ah,[esi+2]		;similarly for BLUE
	out	dx,ax			;
	add	esi,3			;bump to next RGB triplet
	inc	ch			;bump the index
	dec	cl			;perform a loop maneuver
	jnz	@B			;
;
public	VF2SaveCurrentMode
VF2SaveCurrentMode:
;
;We've now successfully setup the HiRes mode for running our VESA app.  Let's
;save the mode in our CB data area for later use:
;
	mov	ax,[ebp].Client_BX	;get mode that we just set
	mov	edi,ebx 		;make EDI --> CB data area for this VM
	add	edi,OurCBDataPointer	;
	mov	[edi].CurrentMode,ax	;save the mode number
;
public	VF2StartDACTranslate
VF2StartDACTranslate:
;
;When the XGA is in HiRes mode (as we've just done), the standard VGA DAC
;registers 3C6H-3C9H are not used.  Rather, the XGA uses a proprietary DAC
;programming scheme.  We therefore must virtualize the DAC registers 3C6H-3C9H
;since most VESA apps assume that these are the palette registers and program
;them directly.  We have virtualization code to translate accesses to the VGA
;DAC into XGA accesses.
;
	cmp	ebx,WindowsVMHandle	;doing mode change for the Windows VM?
	je	VF2EraseScreen		;yes, don't bother trapping DAC
	cmp	ChipID,AGX_ID		;running on an IIT AGX?
	je	VF2EraseScreen		;yes, AGX uses standard VGA DAC regs
	mov	edx,3c7h		;enable trapping for our VM
	VMMCall Enable_Local_Trapping	;
;
	mov	edx,3c8h		;enable trapping for our VM
	VMMCall Enable_Local_Trapping	;
;
	mov	edx,3c9h		;enable trapping for our VM
	VMMCall Enable_Local_Trapping	;
;
public	VF2EraseScreen
VF2EraseScreen:
;
;If the sign bit of our mode number is set, we don't erase the screen.
;
	test	byte ptr [edi].CurrentMode+1,80h
	jnz	VF2GoodExit		;do not erase the screen
	mov	esi,TotalMemorySize	;we want to clear all of VRAM
	mov	edx,XGABankReg		;EDX --> banking register
	xor	eax,eax 		;start at bank 0
	out	dx,al			;(just in case)
@@:	push	eax			;save bank register value
	mov	edi,VidMemLinearAddr	;EDI --> linear address of A0000H
	mov	ecx,(64*1024)/4 	;ECX has nbr of DWORDS to clear
	xor	eax,eax 		;clear to zero
	rep	stosd			;clear 64K
	pop	eax			;restore saved bank value in AL
	inc	eax			;bump to next bank
	out	dx,al			;
	sub	esi,64*1024		;are we done yet?
	jg	@B			;nope, go do next 64K
	xor	eax,eax 		;reset to bank 0
	out	dx,al			;
	jmp	VF2GoodExit		;hey we're done!
;
public	VF2StandardVGAMode
VF2StandardVGAMode:
	call	LeaveXGAMode		;get us out of XGA HiRes mode
	movzx	eax,[ebp].Client_BL	;
	push	10h			;
	VMMCall Exec_VxD_Int		;just do a standard INT 10H mode set
;
public	VF2GoodExit
VF2GoodExit:
	mov	[ebp].Client_AX,4fh	;return success!
	jmp	VF2Exit 		;
;
public	VF2Failure
VF2Failure:
	mov	[ebp].Client_AX,014fh	;return failure to caller
;
VF2Exit:
	ret				;return to dispatch routine
EndProc VESAFunc2
;
;
subttl		Support For VESA Function 3 -- Retrieve Mode
page +
public	VESAFunc3
BeginProc VESAFunc3, DOSVM
;
;Entry:
;	EBX contains the VM handle.
;	EBP --> Client Registers.
;Exit:
;	Client_AX contains the return status from the call (always success).
;	Client_BX contains the mode number that VM is running in.
;
	mov	edi,ebx 		;make EDI --> our CB data area
	add	edi,OurCBDataPointer	;
	mov	ax,[edi].CurrentMode	;get the mode that we're in
	mov	[ebp].Client_BX,ax	;and send it back to the caller
	mov	[ebp].Client_AX,4fh	;return success!
;
VF3Exit:
	ret				;return to dispatch routine
EndProc VESAFunc3
;
;
subttl		Support For VESA Function 4 -- Save/Restore State
page +
public	VESAFunc4
BeginProc VESAFunc4, DOSVM
;
;Entry:
;	EBX contains the VM handle.
;	EBP --> Client Registers.
;	Client_DL contains 0 if querying the save/restore buffer size.
;	Client_DL contains 1 if requesting a save state.
;	Client_DL contains 2 if requesting a restore state.
;	Client_CX contains the requested states bitmask:
;		Bit 0 set == save/restore controller state.
;		Bit 1 set == save/restore BIOS data area state.
;		Bit 2 set == save/restore DAC state.
;		Bit 3 set == save/restore register state.
;	Client_ES:Client_BX --> Buffer to save/restore states to/from.
;Exit:
;	Client_AX contains the return status from the call.
;	Client_BX contains the state size if Client_DL == 0.
;
;We don't save/restore states.  Just return 0 if it's sub-function 0 and
;return success.
;
	cmp	[ebp].Client_DL,0	;is it a "get state save size" request?
	jne	VF4GoodExit		;nope, just leave successfully
	mov	[ebp].Client_BX,0	;yes, return state size 0
;
public	VF4GoodExit
VF4GoodExit:
	mov	[ebp].Client_AX,4fh	;return success!
;
VF4Exit:
	ret				;return to dispatch routine
EndProc VESAFunc4
;
;
subttl		Support For VESA Function 5 -- Display Window Control
page +
public	VESAFunc5
BeginProc VESAFunc5, DOSVM
;
;Entry:
;	EBX contains the VM handle.
;	EBP --> Client Registers.
;	Client_BH == 0 if we're to set the bank.
;	Client_BH == 1 if we're to get the bank.
;	Client_BL contains the "window" number (0 or 1).
;	Client_DX contains the bank to set (if Client_BH == 0).
;Exit:
;	Client_AX contains the return status from the call.
;	Client_DX contains the bank number (if Client_BH == 1).
;
	mov	edx,XGABankReg		;EDX --> banking register
	cmp	[ebp].Client_BH,0	;setting the bank?
	je	VF5SetBank		;yes, go do it
;
public	VF5GetBank
VF5GetBank:
	xor	ah,ah			;make sure AH is 0
	in	al,dx			;get bank number in AL
	mov	[ebp].Client_DX,ax	;return it in the expected place
	jmp	VF5GoodExit		;and return success
;
public	VF5SetBank
VF5SetBank:
	mov	al,[ebp].Client_DL	;get bank to set
	out	dx,al			;set the bank
;
public	VF5GoodExit
VF5GoodExit:
	mov	[ebp].Client_AX,4fh	;return success!
;
VF5Exit:
	ret				;return to dispatch routine
EndProc VESAFunc5
;
;
subttl		Support For VESA Function 6 -- Get/Set Logical Scanline Length
page +
public	VESAFunc6
BeginProc VESAFunc6, DOSVM
;
;Entry:
;	EBX contains the VM handle.
;	EBP --> Client Registers.
;	BL contains the sub-function:
;		-- 0 == Set scanline length in pixels.
;		-- 1 == Get scanline length.
;		-- 2 == Set scanline length in bytes.
;		-- 3 == Get maximum scanline length.
;	Client_CX contains the scanline lengths in pixels/bytes if BL == 0/2.
;Exit:
;	Client_AX contains the return status from the call.
;	Client_BX contains bytes per line.
;	Client_CX contains pixels per line.
;	Client_DX contains the maximum number of scanlines.
;
	movzx	ecx,[ebp].Client_BL	;
	cmp	cl,NbrVF6Funcs		;is function code request OK?
	jae	VF6Failure		;nope, leave with a failure
	mov	edi,ebx 		;make EDI --> CB data area
	add	edi,OurCBDataPointer	;
	movzx	edi,[edi].CurrentMode	;EDI contains the mode number to lookup
	cmp	edi,HighestMode 	;is it a valid mode?
	ja	VF6Failure		;nope, leave now!
	sub	edi,100h		;offset the mode number from zero
	jl	VF6Failure		;bad mode!  don't support it
	shl	edi,4			;multiply by 16 bytes per entry
	cmp	ChipID,AGX_ID		;running on the IIT AGX?
	jne	@F			;nope, use the XGA tables
	movzx	esi,[edi+OFFSET32 AGXModeLookupTable].ScreenWidthInPixels
					;ESI has width of screen in pixels
	movzx	edi,[edi+OFFSET32 AGXModeLookupTable].ScreenPitch
					;EDI has width of screen in bytes
	jmp	VF6CheckValidity	;we've got the data
@@:	movzx	esi,[edi+OFFSET32 VESAModeLookupTable].ScreenWidthInPixels
					;ESI has width of screen in pixels
	movzx	edi,[edi+OFFSET32 VESAModeLookupTable].ScreenPitch
					;EDI has width of screen in bytes
;
public	VF6CheckValidity
VF6CheckValidity:
	or	esi,esi 		;did we get a zero entry?
	jz	VF6Failure		;yes, bad mode!
	mov	eax,TotalMemorySize	;get bytes of total memory
	xor	edx,edx 		;zero EDX for dword lengthed divide
	div	edi			;now EAX has total nbr of scanlines
;
;The VESA spec is unclear regarding when we're supposed to return data.  We
;therefore always return the data in the client registers:
;
	movzx	edx,[ebp].Client_CX	;save input data from Client_CX
	mov	[ebp].Client_BX,di	;return bytes per scanline in Client_BX
	mov	[ebp].Client_CX,si	;return pixels per scanline in Client_CX
	mov	[ebp].Client_DX,ax	;return total scanlines in Client_DX
;
;Now, dispatch to the proper handler:
;
	jmp	[(ecx*4)+OFFSET32 VF6DispatchTable]
;
public	VF6SetLengthInPixels
VF6SetLengthInPixels:
;
;At this point:
;	EDX contains the data passed into this function from Client_CX,
;		that is, the desired width in pixels/bytes.
;
	cmp	edx,esi 		;is it the same as what it's set to?
	jne	VF6Failure		;nope, leave with a failure
	jmp	VF6GoodExit		;yes, leave successfully!
;
public	VF6SetLengthInBytes
VF6SetLengthInBytes:
;
;At this point:
;	EDX contains the data passed into this function from Client_CX,
;		that is, the desired width in pixels/bytes.
;
	cmp	edx,edi 		;is it the same as what it's set to?
	jne	VF6Failure		;nope, leave with a failure
;
public	VF6GoodExit
VF6GoodExit:
	mov	[ebp].Client_AX,4fh	;return success!
	jmp	VF6Exit 		;
;
public	VF6Failure
VF6Failure:
	mov	[ebp].Client_AX,014fh	;return failure to caller
;
VF6Exit:
	ret				;return to dispatch routine
;
;
VF6DispatchTable	label	dword
	dd	OFFSET32 VF6SetLengthInPixels
	dd	OFFSET32 VF6GoodExit
	dd	OFFSET32 VF6SetLengthInBytes
	dd	OFFSET32 VF6GoodExit
NbrVF6Funcs	equ ($ - OFFSET32 VF6DispatchTable)/4
;
;
EndProc VESAFunc6
;
;
subttl		Support For VESA Function 7 -- Set/Get Display Start
page +
public	VESAFunc7
BeginProc VESAFunc7, DOSVM
;
;Entry:
;	EBX contains the VM handle.
;	EBP --> Client Registers.
;	BL contains the sub-function code:
;		-- 0 = Set Display Start.
;		-- 1 = Get Display Start.
;		-- 80h = Set Display Start during vertical retrace.
;	BH must be 0.
;	CX = Starting X pixel.
;	DX = Starting Y pixel.
;Exit:
;	Client_AX contains the return status from the call.
;	BH must be 0.
;	CX = Starting X pixel.
;	DX = Starting Y pixel.
;
	cmp	[ebp].Client_BL,01h	;is it a "get display start" call?
	jne	VF7Failure		;nope, fail the call
	mov	[ebp].Client_BH,0	;return a zero in BH
	mov	[ebp].Client_CX,0	;return display start in X as 0
	mov	[ebp].Client_DX,0	;return display start in Y as 0
;
public	VF7GoodExit
VF7GoodExit:
	mov	[ebp].Client_AX,4fh	;return success!
	jmp	VF7Exit 		;
;
public	VF7Failure
VF7Failure:
	mov	[ebp].Client_AX,014fh	;return failure to caller
;
VF7Exit:
	ret				;return to dispatch routine
EndProc VESAFunc7
;
;
subttl		Support For VESA Function 8 -- Set/Get Palette Format
page +
public	VESAFunc8
BeginProc VESAFunc8, DOSVM
;
;Entry:
;	EBX contains the VM handle.
;	EBP --> Client Registers.
;	Client_BL contains 0 for Set Format or 1 for Get Format.
;	Client_BH contains the new format to set.
;Exit:
;	Client_AX contains the return status from the call.
;
	cmp	[ebp].Client_BL,1	;does caller want to do a "Get Format"?
	jne	VF8Failure		;nope, return failure
	mov	[ebp].Client_BH,6	;we only have a 6 bit DAC
;
public	VF8GoodExit
VF8GoodExit:
	mov	[ebp].Client_AX,4fh	;return success!
	jmp	VF8Exit 		;
;
public	VF8Failure
VF8Failure:
	mov	[ebp].Client_AX,014fh	;return failure to caller
;
VF8Exit:
	ret				;return to dispatch routine
EndProc VESAFunc8
;
;
subttl		Support For VESA Function 9 -- Set/Get Palette Data
page +
public	VESAFunc9
BeginProc VESAFunc9, DOSVM
;
;Entry:
;	EBX contains the VM handle.
;	EBP --> Client Registers.
;	Client_BL has the sub-function:
;		-- 0 == Set Palette Data.
;		-- 1 == Get Palette Data.
;		-- 2 == Set Secondary Palette Data.
;		-- 3 == Get Secondary Palette Data.
;		-- 80H == Set Palette Data during vertical retrace with screen
;			  blanked.
;	Client_CX contains the number of palette entries to do.
;	Client_DX contains the first palette entry to do.
;	Client_ES:Client_DI --> palette entries to do.
;	Format of palette data pointed to by Client_ES:Client_DI is
;		Byte 0 = alignment byte.
;		Byte 1 = RED value (in lowest 6 bits of byte).
;		Byte 2 = GREEN value (in lowest 6 bits of byte).
;		Byte 3 = BLUE value (in lowest 6 bits of byte).
;Exit:
;	Client_AX contains the return status from the call.
;
	mov	cl,[ebp].Client_BL	;get subfunction code in CL
	and	cl,NOT 80h		;get rid of 80H bit (not needed on XGA)
	or	cl,cl			;setting palette?
	jz	VF9SetPalette		;yes, go do it
	cmp	cl,1			;getting palette?
	jne	VF9NoSecondaryPalette	;nope, send back error return value
;
public	VF9GetPalette
VF9GetPalette:
	mov	ah,Client_ES		;get client register equates in
	mov	al,Client_DI		;
	VMMCall Map_Flat		;EAX contains the linear address
	cmp	eax,-1			;did the call fail?
	je	VF9Failure		;yes, don't go any further!
	mov	edi,eax 		;EDI --> place to put palette entries
	movzx	ecx,[ebp].Client_DX	;get starting index into ECX
	movzx	esi,[ebp].Client_CX	;get nbr of entries to do in ESI
;
;Get ready to read the palette entries:
;
	cmp	ChipID,AGX_ID		;running on an IIT AGX?
	je	VF9ReadAGXPalette	;yes, go do it for the AGX
	mov	edx,XGACRTCReg		;EDX --> XGA's extended CRTC regs (21xA)
	mov	eax,0066h		;initialize palette sequence reg
	out	dx,ax			;
;
public	VF9ReadDACLoop
VF9ReadDACLoop:
	mov	al,60h			;this is CRTC index for palette index
	mov	ah,cl			;get palette entry to read
	out	dx,ax			;set the index
;
	mov	al,65h			;this is CRTC index for palette data
	out	dx,al			;
	mov	[edi],cl		;set the index as the alignment byte
	inc	dl			;EDX --> XGA indexed data register
;
	in	al,dx			;get RED value
	mov	[edi+1],al		;save it in return area
	in	al,dx			;get GREEN value
	mov	[edi+2],al		;save it in return area
	in	al,dx			;get BLUE value
	mov	[edi+3],al		;save it in return area
;
	dec	dl			;EDX --> XGA index register
	add	edi,4			;bump to next entry in return area
	inc	ecx			;bump to next DAC entry to read
	dec	esi			;any more to do?
	jnz	VF9ReadDACLoop		;yes, go read the next entry!
	jmp	VF9GoodExit		;hey! we're done!
;
public	VF9ReadAGXPalette
VF9ReadAGXPalette:
	mov	edx,3c7h		;EDX --> DAC read index register
;
public	VF9ReadDACLoop_AGX
VF9ReadDACLoop_AGX:
	mov	al,cl			;get palette entry to read
	out	dx,al			;set the index
;
	mov	dl,0c9h 		;this is DAC Data register
	mov	[edi],cl		;set the index as the alignment byte
	in	al,dx			;get RED value
	mov	[edi+1],al		;save it in return area
	in	al,dx			;get GREEN value
	mov	[edi+2],al		;save it in return area
	in	al,dx			;get BLUE value
	mov	[edi+3],al		;save it in return area
;
	mov	dl,0c7h 		;EDX --> DAC read index register
	add	edi,4			;bump to next entry in return area
	inc	ecx			;bump to next DAC entry to read
	dec	esi			;any more to do?
	jnz	VF9ReadDACLoop_AGX	;yes, go read the next entry!
	jmp	VF9GoodExit		;hey! we're done!
;
public	VF9SetPalette
VF9SetPalette:
	mov	ah,Client_ES		;get client register equates in
	mov	al,Client_DI		;
	VMMCall Map_Flat		;EAX contains the linear address
	cmp	eax,-1			;did the call fail?
	je	VF9Failure		;yes, don't go any further!
	mov	esi,eax 		;ESI --> place to read palette entries
	movzx	ecx,[ebp].Client_DX	;get starting index into ECX
	movzx	edi,[ebp].Client_CX	;get nbr of entries to do in EDI
;
;Get ready to set the palette entries:
;
	cmp	ChipID,AGX_ID		;running on an IIT AGX?
	je	VF9SetAGXPalette	;yes, use alternate method
	mov	edx,XGACRTCReg		;EDX --> XGA's extended CRTC regs (21xA)
	mov	eax,0066h		;initialize palette sequence reg
	out	dx,ax			;
;
public	VF9WriteDACLoop
VF9WriteDACLoop:
	mov	al,60h			;this is CRTC index for palette index
	mov	ah,cl			;get palette entry to write
	out	dx,ax			;set the index
;
	mov	al,65h			;this is CRTC index for palette data
	mov	ah,[esi+1]		;get RED value
	out	dx,ax			;
	mov	ah,[esi+2]		;get GREEN value
	out	dx,ax			;
	mov	ah,[esi+3]		;get BLUE value
	out	dx,ax			;
;
	add	esi,4			;bump to next DAC entry
	inc	ecx			;bump to next DAC entry to write
	dec	edi			;any more to do?
	jnz	VF9WriteDACLoop 	;yes, go write the next entry!
	jmp	VF9GoodExit		;and we're done
;
public	VF9SetAGXPalette
VF9SetAGXPalette:
	mov	edx,3c8h		;EDX --> DAC Write Index register
;
public	VF9WriteDACLoop_AGX
VF9WriteDACLoop_AGX:
	mov	al,cl			;get palette entry to write
	out	dx,al			;set the index
	inc	dl			;EDX --> DAC Data Register
	mov	al,[esi+1]		;get RED value
	out	dx,al			;
	mov	al,[esi+2]		;get GREEN value
	out	dx,al			;
	mov	al,[esi+3]		;get BLUE value
	out	dx,al			;
	dec	dl			;EDX --> DAC Write Index register
	add	esi,4			;bump to next DAC entry
	inc	ecx			;bump to next DAC entry to write
	dec	edi			;any more to do?
	jnz	VF9WriteDACLoop_AGX	;yes, go write the next entry!
;
public	VF9GoodExit
VF9GoodExit:
	mov	[ebp].Client_AX,4fh	;return success!
	jmp	VF9Exit 		;
;
public	VF9Failure
VF9Failure:
	mov	[ebp].Client_AX,014fh	;return failure to caller
	jmp	VF9Exit 		;
;
public	VF9NoSecondaryPalette
VF9NoSecondaryPalette:
	mov	[ebp].Client_AX,024fh	;return failure to caller
;
VF9Exit:
	ret				;return to dispatch routine
EndProc VESAFunc9
;
;
subttl		Support For VESA Function 10 -- Return Protected Mode Interface
page +
public	VESAFuncA
BeginProc VESAFuncA, DOSVM
;
;Entry:
;	EBX contains the VM handle.
;	EBP --> Client Registers.
;Exit:
;	Client_AX contains the return status from the call.
;
public	VFAFailure
VFAFailure:
	mov	[ebp].Client_AX,014fh	;return failure to caller
;
VFAExit:
	ret				;return to dispatch routine
EndProc VESAFuncA
;
;
subttl		Return Total Memory Size on Card
page +
public	MiniVDD_GetTotalVRAMSize
BeginProc MiniVDD_GetTotalVRAMSize, DOSVM
;
;Entry:
;	EBX contains the Current VM Handle (which is also the CRTC owner's
;	VM Handle).
;	EBP --> VM's Client Registers.
;Exit:
;	CY is returned upon success.
;	All registers (except ECX) must be preserved over the call.
;	ECX will contain the total VRAM size in bytes.
;
	mov	ecx,TotalMemorySize	;
	stc				;
	ret				;return to caller
EndProc MiniVDD_GetTotalVRAMSize
;
;
subttl		Allow or Disallow Switching Away From a VESA Mode App
page +
public	MiniVDD_CheckScreenSwitchOK
BeginProc MiniVDD_CheckScreenSwitchOK
;
;Entry:
;	EAX = -1 if we're in a VESA mode.
;	EBX contains the VM handle that we're switching away from.
;	ECX contains the mode number.
;Exit:
;	Preserve everything that you use.
;	CY returned if screen switch should be prohibited.
;	NC returned if it's OK to switch away.
;
;The XGA's HiRes screen cannot be reliably saved and restored if we're in a
;VESA mode.  This is because of the fact that the XGA uses its own private
;DAC registers (instead of VGA DAC registers 3C6H-3C9H) and that the XGA
;turns off the VGA registers when in HiRes mode.  So.... we disallow switching
;away from any VESA HiRes mode DOS box.
;
	push	edi			;save what we use
	mov	edi,ebx 		;make EDI --> CB data area for VM
	add	edi,OurCBDataPointer	;
	movzx	edi,[edi].CurrentMode	;EDI contains the mode number
	and	edi,NOT 0c080h		;get rid of extraneous flag bits
	cmp	edi,100h		;are we in a VESA mode (sets CY if
					;in a mode below 100H)
	cmc				;clear the CY if in a standard VGA mode
	pop	edi			;restore saved EDI
	ret				;and return answer to caller
EndProc MiniVDD_CheckScreenSwitchOK
;
;
subttl		Remap DAC Write Accesses When In VESA Modes
page +
public	MiniVDD_VirtualizeDACOut
BeginProc MiniVDD_VirtualizeDACOut, DOSVM
;
;Entry:
;	AL contains the value for the register.
;	EBX contains the VM handle for the write.
;	ECX contains the I/O type (should always be Byte_Output).
;	EDX contains the port to write to.
;Exit:
;	CY returned if we handled the I/O.
;	NC returned if the Main VDD should deal with it.
;
	push	esi			;save registers that we use
	push	edi			;
	cmp	ebx,WindowsVMHandle	;is this I/O for the Windows VM?
	je	MVDOLetCallerHandle	;yes, we don't care about it
	cmp	dl,0c6h 		;is it an access to port 3C6H?
	je	MVDOLetCallerHandle	;yes, we don't care about it
	VxDCall VDD_Get_VM_Info 	;get CRTC owner VM handle in EDI
	cmp	ebx,edi 		;do we own the CRTC?
	jne	MVDOLetCallerHandle	;nope, let the caller handle it
	mov	edi,ebx 		;make EDI --> our VM's CB data area
	add	edi,OurCBDataPointer	;
	movzx	esi,[edi].CurrentMode	;ESI has the mode number
	and	esi,NOT 0c080h		;get rid of extraneous bits in mode nbr
	cmp	esi,100h		;is it a VESA HiRes mode?
	jb	MVDOLetCallerHandle	;nope, go let the caller deal with it
;
public	MVDOReadIndex
MVDOReadIndex:
;
;The XGA uses a private DAC access scheme when in HiRes mode (in other words,
;we don't use the VGA DAC ports 3C6H-3C9H).  Thus, we must translate the access
;to the VGA DAC port into an XGA DAC access.
;
	cmp	dl,0c7h 		;are we setting the DAC read index?
	jne	MVDOWriteIndex		;nope, try port 3C8H
;
;First initialize the XGA's Palette Sequencer Register (index 66H) to
;RGB ordering and starting with the RED index:
;
	mov	edx,XGACRTCReg		;EDX --> XGA's private CRTC index reg
	mov	cl,al			;save read index in CL
	mov	ax,0066h		;initialize the palette sequencer
	out	dx,ax			;
;
;Now, set the index:
;
	mov	ah,cl			;get index into AH
	mov	al,62h			;this is CRTC index for palette index
	out	dx,ax			;
	mov	[edi].DACReadIndex,ah	;save the read index in our CB data
;
;Indicate that we're doing a read cycle and that we're starting with RED:
;
	mov	[edi].DACReadStatus,RED_CYCLE
	jmp	MVDOWeHandledIt 	;
;
public	MVDOWriteIndex
MVDOWriteIndex:
	cmp	dl,0c8h 		;are we setting the DAC write index?
	jne	MVDOWriteData		;nope, must be writing to the DAC
;
;First initialize the XGA's Palette Sequencer Register (index 66H) to
;RGB ordering and starting with the RED index:
;
	mov	edx,XGACRTCReg		;EDX --> XGA's private CRTC index reg
	mov	cl,al			;save write index in CL
	mov	ax,0066h		;initialize the palette sequencer
	out	dx,ax			;
;
;Now, set the index:
;
	mov	ah,cl			;get index into AH
	mov	al,60h			;this is CRTC index for palette index
	out	dx,ax			;
	mov	[edi].DACWriteIndex,ah	;save the write index in our CB data
;
;Indicate that we're doing a write cycle and that we're starting with RED:
;
	mov	[edi].DACWriteStatus,RED_CYCLE
	jmp	MVDOWeHandledIt 	;
;
public	MVDOWriteData
MVDOWriteData:
	mov	edx,XGACRTCReg		;EDX --> XGA's private CRTC index reg
	mov	cl,al			;save data byte in CL
	cmp	[edi].DACWriteStatus,DONE_WITH_CYCLE
					;finished with a cycle?
	jb	@F			;not yet, just write the data
	mov	[edi].DACWriteStatus,RED_CYCLE
					;reset the cycle
	inc	[edi].DACWriteIndex	;bump to the next palette entry
	mov	al,60h			;set to the palette index register
	mov	ah,[edi].DACWriteIndex	;and reset the index
	out	dx,ax			;
@@:	mov	ah,cl			;get data to write into AH
	shl	ah,2			;get it into top 6 bits
	mov	al,65h			;this is index for palette write data
	out	dx,ax			;write the data to the port
	inc	[edi].DACWriteStatus	;bump to next color
	jmp	MVDOWeHandledIt 	;and continue
;
public	MVDOLetCallerHandle
MVDOLetCallerHandle:
	clc				;the caller should handle it
	jmp	MVDOExit		;
;
public	MVDOWeHandledIt
MVDOWeHandledIt:
	stc				;indicate that we handled the I/O
;
MVDOExit:
	pop	edi			;restore saved registers
	pop	esi			;
	ret				;
EndProc MiniVDD_VirtualizeDACOut
;
;
subttl		Remap DAC Read Accesses When In VESA Modes
page +
public	MiniVDD_VirtualizeDACIn
BeginProc MiniVDD_VirtualizeDACIn, DOSVM
;
;Entry:
;	EBX contains the VM handle for the write.
;	ECX contains the I/O type (should always be Byte_Output).
;	EDX contains the port to write to.
;Exit:
;	CY returned if we handled the I/O.
;		-- AL contains the value to return.
;	NC returned if the Main VDD should deal with it.
;
	push	esi			;save registers that we use
	push	edi			;
	cmp	ebx,WindowsVMHandle	;is this I/O for the Windows VM?
	je	MVDILetCallerHandle	;yes, we don't care about it
	cmp	dl,0c6h 		;is it an access to port 3C6H?
	je	MVDILetCallerHandle	;yes, we don't care about it
	VxDCall VDD_Get_VM_Info 	;get CRTC owner VM handle in EDI
	cmp	ebx,edi 		;do we own the CRTC?
	jne	MVDILetCallerHandle	;nope, let the caller handle it
	mov	edi,ebx 		;make EDI --> our VM's CB data area
	add	edi,OurCBDataPointer	;
	movzx	esi,[edi].CurrentMode	;ESI has the mode number
	and	esi,NOT 0c080h		;get rid of extraneous bits in mode nbr
	cmp	esi,100h		;is it a VESA HiRes mode?
	jb	MVDILetCallerHandle	;nope, go let the caller deal with it
;
public	MVDIDACStatus
MVDIDACStatus:
;
;The XGA uses a private DAC access scheme when in HiRes mode (in other words,
;we don't use the VGA DAC ports 3C6H-3C9H).  Thus, we must translate the access
;to the VGA DAC port into an XGA DAC access.
;
	cmp	dl,0c7h 		;are we trying to read the DAC status?
	jne	MVDIReadIndex		;nope, try port 3C8H
	xor	al,al			;always return that we're in a WRITE
	jmp	MVDIWeHandledIt 	;
;
public	MVDIReadIndex
MVDIReadIndex:
	cmp	dl,0c8h 		;are we reading the DAC write index?
	jne	MVDIReadData		;nope, must be reading from the DAC
	mov	al,[edi].DACWriteIndex	;just return the write index
	jmp	MVDIWeHandledIt 	;
;
public	MVDIReadData
MVDIReadData:
	mov	edx,XGACRTCReg		;EDX --> XGA's private CRTC index reg
	cmp	[edi].DACReadStatus,DONE_WITH_CYCLE
					;finished with a cycle?
	jb	@F			;not yet, just write the data
	mov	[edi].DACReadStatus,RED_CYCLE
					;reset the cycle
	inc	[edi].DACReadIndex	;bump to the next palette entry
	mov	al,62h			;set to the palette index register
	mov	ah,[edi].DACReadIndex	;and reset the index
	out	dx,ax			;
@@:	mov	al,65h			;this is index for palette read data
	out	dx,al			;set to the palette data register
	inc	dl			;EDX --> XGA Private CRTC data reg
	in	al,dx			;get the data
	shr	al,2			;get it into low 6 bits as expected
	inc	[edi].DACReadStatus	;bump to next color
	jmp	MVDIWeHandledIt 	;and continue
;
public	MVDILetCallerHandle
MVDILetCallerHandle:
	clc				;the caller should handle it
	jmp	MVDIExit		;
;
public	MVDIWeHandledIt
MVDIWeHandledIt:
	stc				;indicate that we handled the I/O
;
MVDIExit:
	pop	edi			;restore saved registers
	pop	esi			;
	ret				;
EndProc MiniVDD_VirtualizeDACIn
;
;
subttl		Virtualize Port 2160H on the IIT AGX
page +
public	MiniVDD_Virtual2160
BeginProc MiniVDD_Virtual2160, DOSVM
;
;Entry:
;	AL contains the value for port 2160H
;	EBX contains the VM handle trying to access port 2160H.
;	ECX contains the type of I/O being done (always Byte_IO).
;	EDX contains the port (2160H).
;Exit:
;
;The AGX needs this register virtualized because its INT 10H ROM BIOS
;turns off access to the memory mapped BLTer registers whenever going to
;a standard VGA mode.  This is OK if we're full-screen, but if we're
;windowed, the co-processor will die and we'll freeze.  Thus, we always
;force accessibility to the memory mapped registers if Windows owns the
;CRTC.
;
;If the CRTCModeChangeFlags are set to FFH, it means that we're to let the
;actual write to 2160H take place.  This happens when either the display
;driver is disabled (ie: not in HiRes mode), or when the Main VDD is doing
;a mode change to full-screen mode.
;
	cmp	CRTCModeChangeFlags,0	;should we virtualize this?
	jne	MV2DoPhysicalIO 	;nope, let it happen
	test	cl,04h			;doing an OUT to 2160H?
	jz	MV2DoPhysicalIO 	;nope, go do physical IO
	VxDCall VDD_Get_VM_Info 	;get CRTC owner VM handle in EDI
	cmp	ebx,edi 		;does VM own the CRTC?
	je	MV2DoPhysicalIO 	;yes, just do physical IO
	cmp	edi,WindowsVMHandle	;are we running a Windowed VM?
	jne	MV2DoPhysicalIO 	;
	or	al,05h			;yes, make sure XGA memory mapped
					;regs are accessible
;
public	MV2DoPhysicalIO
MV2DoPhysicalIO:
	VxDJmp	VDD_Do_Physical_IO	;go do the I/O
EndProc MiniVDD_Virtual2160
;
;
subttl		Virtualize AGX Indexed Ports
page +
public	MiniVDD_VirtualAGXIndexedPorts
BeginProc MiniVDD_VirtualAGXIndexedPorts, DOSVM
;
;Entry:
;	AL contains the value for the AGX indexed ports.
;	EBX contains the VM handle trying to access AGX indexed ports.
;	ECX contains the type of I/O being done.
;	EDX contains the port number.
;Exit:
;
;The AGX needs this register virtualized because its INT 10H ROM BIOS messes
;with these ports.  We need to stop all I/O if in a windowed VM.
;
;If the CRTCModeChangeFlags are set to FFH, it means that we're to let the
;actual write to the port take place.  This happens when either the display
;driver is disabled (ie: not in HiRes mode), or when the Main VDD is doing
;a mode change to full-screen mode.
;
	test	cl,OUTPUT		;doing an OUT to AGX port?
	jz	MVAHandleIN		;nope, we're doing an IN
	cmp	dl,6ah			;writing the index?
	jne	MVAHandleOUT		;nope, continue
	mov	edi,ebx 		;make EDI --> OurCBData area
	add	edi,OurCBDataPointer	;
	mov	[edi].AGXIndex,al	;save the index that we're setting to
	jmp	MVAHandleOUT		;and continue
;
public	MVAHandleIN
MVAHandleIN:
	mov	edi,ebx 		;make EDI --> OurCBData area
	add	edi,OurCBDataPointer	;
	cmp	[edi].AGXIndex,6fh	;doing an IN from index 6FH?
	jne	MVADoPhysicalIO 	;nope, go do physical IN
	VxDCall VDD_Get_VM_Info 	;get CRTC owner VM handle in EDI
	cmp	ebx,edi 		;does VM own the CRTC?
	je	MVADoPhysicalIO 	;yes, go do physical IN
;
;Register 6FH, bit 2 being set causes the AGX BIOS to add 2 to the cursor
;position that's set in text modes.  Since we set bit 2 when running a
;16 BPP mode, we get a erroneous cursor position when running a windowed
;text mode DOS box.  We therefore don't allow bit 2 to be returned to the
;caller when we're not the CRTC owner.
;
	mov	dl,6ah			;EDX --> AGX index register
	mov	al,6fh			;set to index 6FH
	out	dx,al			;
	inc	dl			;EDX --> AGX data register
	in	al,dx			;get contents of AGX indexed reg 6FH
	and	al,NOT 04h		;get rid of "X Refresh Split" bit
	jmp	MVADontDoPhysicalIO	;and return this value
;
public	MVAHandleOUT
MVAHandleOUT:
	cmp	CRTCModeChangeFlags,0	;should we virtualize this?
	jne	MVADoPhysicalIO 	;nope, let it happen
	VxDCall VDD_Get_VM_Info 	;get CRTC owner VM handle in EDI
	cmp	ebx,edi 		;does the VM own the CRTC?
	jne	MVADontDoPhysicalIO	;nope, don't allow the IO
;
public	MVADoPhysicalIO
MVADoPhysicalIO:
	VxDJmp	VDD_Do_Physical_IO	;go do the I/O
;
public	MVADontDoPhysicalIO
MVADontDoPhysicalIO:
	ret				;
EndProc MiniVDD_VirtualAGXIndexedPorts
;
;
VxD_LOCKED_CODE_ENDS
;
;
end
