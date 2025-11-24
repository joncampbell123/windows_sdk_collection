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
title		Video 7 Mini-VDD Support Functions
.386p
;
;
.xlist
include 	VMM.INC
include 	MINIVDD.INC
include 	VIDEO7.INC
.list
;
;
subttl		Virtual Device Declaration
page +
Declare_Virtual_Device	VIDEO7, 				\
			3,					\
			1,					\
			MiniVDD_Control,			\
			Undefined_Device_ID,			\
			VDD_Init_Order, 			\
			,					\
			,                                       \					
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
WindowsVMHandle 	dd	?		;init'd at Device_Init
;
public	OurCBDataPointer
OurCBDataPointer	dd	?		;init'd at Device_Init
;
public	PlanarModeVMHandle
PlanarModeVMHandle	dd	?
;
public	MessageModeVMHandle
MessageModeVMHandle	dd	?		;init'd at Device_Init
;
public	MessageModeCBData
MessageModeCBData	db	size PerVMData dup(0)
;
public	TotalMemorySize
TotalMemorySize 	dd	?		;init'd at Sys_Critical_Init
;
public	VDDBank, VDDBankRegF9, VDDBankRegF6, VDDBankReg3C2, BankRegSave
VDDBank 		db	?
VDDBankRegF9		db	?
VDDBankRegF6		db	?
VDDBankReg3C2		db	?
BankRegSave		db	0ffh
;
public	LatchSaveF9, LatchSaveF6, LatchSave3C2
LatchSaveF9		db	?
LatchSaveF6		db	?
LatchSave3C2		db	?
;
public	SuperVGAModeFlag
SuperVGAModeFlag	db	0
;
public	InModeChangeFlag
InModeChangeFlag	db	0		;FFH if in VDD CRTC change
;
public	ReentryFlag
ReentryFlag		db	0		;FFH if virtualization reentered
;
public	DoneGettingMsgModeFlag
DoneGettingMsgModeFlag	db	0		;FFH if we have Msg Mode state
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
	push	ebx			;save VM handle over this
	mov	WindowsVMHandle,ebx	;save the Windows VM handle
;
;First, identify ourselves as a Video7 chipset and fail to load this MiniVDD
;if we aren't.
;
	xor	ebx,ebx 		;initialize this to zero
	mov	eax,6f00h		;func code: Get Video7 ID
	push	10h			;this is interrupt to execute
	VMMCall Exec_VxD_Int		;perform the INT 10H BIOS call
	cmp	bx,'V7' 		;are we a Video7 chipset?
	mov	ebx,WindowsVMHandle	;(restore Windows VM handle to EBX)
	jne	MVDI_ErrorExit		;we're not a Video7...  FAIL!
;
;We want to allocate space in the per-VM CB data structure.  This will
;be used by the mini-VDD to store per-VM data relating to states of registers.
;
	VMMCall _Allocate_Device_CB_Area,<<size PerVMData>,0>
	mov	OurCBDataPointer,eax	;save offset our our VxD's area
	or	eax,eax 		;was call sucessful?
	jz	MVDI_ErrorExit		;nope, leave fatally!
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
	MiniVDDDispatch GET_VDD_BANK,GetVDDBank
	MiniVDDDispatch SET_VDD_BANK,SetVDDBank
	MiniVDDDispatch RESET_BANK,ResetBank
	MiniVDDDispatch SET_LATCH_BANK,SetLatchBank
	MiniVDDDispatch RESET_LATCH_BANK,ResetLatchBank
	MiniVDDDispatch SAVE_REGISTERS,SaveRegisters
	MiniVDDDispatch RESTORE_REGISTERS,RestoreRegisters
	MiniVDDDispatch VIRTUALIZE_SEQUENCER_OUT,VirtualizeSequencerOut
	MiniVDDDispatch VIRTUALIZE_GCR_OUT,VirtualizeGCROut
	MiniVDDDispatch VIRTUALIZE_SEQUENCER_IN,VirtualizeSequencerIn
	MiniVDDDispatch DISPLAY_DRIVER_DISABLING,DisplayDriverDisabling
	MiniVDDDispatch PRE_HIRES_TO_VGA,PreCRTCModeChange
	MiniVDDDispatch POST_HIRES_TO_VGA,PostCRTCModeChange
	MiniVDDDispatch PRE_VGA_TO_HIRES,PreCRTCModeChange
	MiniVDDDispatch POST_VGA_TO_HIRES,PostCRTCModeChange
	MiniVDDDispatch GET_CHIP_ID,GetChipID
	MiniVDDDispatch SAVE_MESSAGE_MODE_STATE,SaveMsgModeState
	MiniVDDDispatch GET_TOTAL_VRAM_SIZE,GetTotalVRAMSize
;
public	MVDI_GetMessageMode
MVDI_GetMessageMode:
;
;Call the Main VDD to get the Message Mode Pseudo VM handle:
;
	VxDCall VDD_Get_Special_VM_IDs	;returns Msg Mode ID in EDI
					;	 Planare Mode ID in ESI
	mov	MessageModeVMHandle,edi ;save it for later use
	mov	PlanarModeVMHandle,esi	;
;
MVDI_GoodExit:
	clc				;return success
	jmp	MVDI_Exit		;
;
MVDI_ErrorExit:
	stc
;
MVDI_Exit:
	pop	ebx			;restore saved VM handle
	ret
EndProc MiniVDD_Dynamic_Init
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
	mov	eax,'V7'		;always return 'V7' to caller
	ret				;
EndProc 	MiniVDD_GetChipID
;
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
End_Control_Dispatch MiniVDD
;
;
subttl		Register Display Driver Dependent Data
page +
public	MiniVDD_RegisterDisplayDriver
BeginProc MiniVDD_RegisterDisplayDriver, RARE
;
;Oft-times, the display driver must set flags whenever it is doing
;certain things to a hardware BLTer (such as "I'm in the middle of
;transferring a source bitmap").  This routine is called to allow
;this mini-VDD to get data (usually containing addresses inside the
;display driver's Data segment) directly from the display driver.
;We can also save appropriately needed states here.
;
;	The only client registers that are reserved (and cannot be used
;to pass data in) are EAX (which contains the function code which the
;display driver uses to call the "main" VDD) and EBX (which contains the
;Windows VM handle).  When we get called by the "main" VDD, we'll be
;passed a flag in Client_AL which will be set to non-zero if the VFLATD
;VxD is being used for banked support or zero if VFLATD is NOT involved in
;this session.
;
;Entry:
;	EBP --> the Client Register Structure (documented in VMM.INC)
;	Client_DX contains the display driver "extra" screen selector.
;	Client_ECX contains the total amount of memory on the card.
;
;Since the display driver has already calculated the amount of memory on
;the card, we'll let it tell us.  Since we're called before GetVDDBank,
;we can safely obtain the information at this time.
;
	mov	eax,[ebp].Client_ECX	;get total memory size
	mov	TotalMemorySize,eax	;
;
;The Video 7 FRAMEBUF.DRV needs to utilize two selectors during
;"block move" operations.  Since the MiniVDD depends on writes to the
;VFLATD selector as its method for knowing when to set the hardware in
;and out of Windows HiRes mode, writes to this second selector won't cause
;us to be called.  Therefore, it might be possible during a block move to
;have the hardware state!  We now call a special service provided by
;the Main VDD to "register" this second screen selector and setup the
;mechanism by which writes to this selector will be trapped and handled
;at the appropriate time:
;
	movzx	eax,[ebp].Client_DX	;get passed in "extra" selector
	VxDCall VDD_Register_Extra_Screen_Selector
;
MRDDExit:
	ret				;
EndProc MiniVDD_RegisterDisplayDriver
;
;
subttl		Calculate and Save Banking Register Values
page +
public	MiniVDD_GetVDDBank
BeginProc MiniVDD_GetVDDBank, RARE
;
;In order to virtualize VGA graphics mode applications in a window on the
;Windows desktop, the VDD requires you to reserve at least 32K of off-screen
;video memory for the exclusive use of the VDD.  Although it's best to
;allocate a full 64K of off-screen video memory, the VDD can work with a
;minimum of 32K.  This function is called by the "main" VDD when the
;display driver registers with it.  This function must calculate the
;correct bank of the off-screen memory and the offset within that bank of
;the VDD reserved memory.  Note that it's best if the offset value returned
;is zero (that is, you start on a bank boundry).
;
;The mini-VDD saves the banking register values locally for later
;use during VGA virtualization.
;
;Note that the "main" VDD assumes that the hardware banks are 64K in length.
;
;Entry:
;	EBX contains the Windows VM handle (which must be preserved).
;	ECX contains the byte offset of where in video memory the VDD
;	    memory is to be placed (this will be the first byte of
;	    off-screen memory).
;Exit:
;	EBX must still contain the Windows VM handle.
;	ECX contains the starting address of the VDD virtualization area
;	EDX contains the amount of memory that we allocated to
;	the VDD virtualization area.  We set EDX == ECX if we are in
;	a "memory shy" configuration.  That is, to tell the "main"
;	VDD that we cannot support VGA 4 plane graphics in the background.
;
	mov	SuperVGAModeFlag,0ffh	;flag so that routines will know
					;that we're not running 4 plane mode
;
;The Video7 VRAM II cards are VERY weird in terms of how the hardware looks
;at the banking bits under various conditions.	Some of the banking bits
;aren't looked at if Sequencer Register 4's Chain4 bit isn't turned on.
;The net result of this is that we always have to use bank 0CH as the
;VDD bank, even if we are running in 800x600x8 mode (in which case the
;banking register would normally be calculated as bank 0AH).  So....
;lets fixup this idiocy and always use bank 0CH:
;
	mov	ecx,1024*768		;this puts us at bank 0CH
	cmp	ecx,TotalMemorySize	;is the memory size too small?
	jae	MGVBMemoryShy		;yes, don't allow virtualization
;
;For the Video 7 chipsets, our VGA virtualization banks are 64K in length.
;So, let's find out in which bank the off-screen memory starts:
;
	mov	eax,ecx 		;get start address of off-screen memory
	shr	eax,16			;divide by 64K per bank - AL has bank #
;
;At this point:
;	AL contains the bank number of the start of off-screen memory
;	CX contains the offset within the bank of the start of
;	   off-screen memory.
;
;It is desirable for us to start the VDD memory area on an even bank
;boundry.  This is possible if we have at least one full 64K bank
;of video memory remaining.  To determine this, we must calculate
;the total number of banks available in video memory.  If there's
;at least one more than what's in AL, we can start on an even bank
;boundry!
;
	or	cx,cx			;are we already on an even bank boundry?
	jz	MGVBCalcAmountToUse	;yes, go see if we can alloc 64K
	mov	edx,TotalMemorySize	;get total amount of video memory
	shr	edx,16			;now DL has total banks on system
	dec	dl			;now DL has last possible bank nbr
	cmp	al,dl			;can we start on an even bank boundry?
	jae	MGVBMemoryShy		;nope, turn off VGA virtualization
	inc	al			;yes! put ourselves at next bank boundry
	xor	cx,cx			;zero the offset in ECX
	add	ecx,10000h		;and set ECX = new start of VDD area
;
public	MGVBCalcAmountToUse
MGVBCalcAmountToUse:
;
;At this point:
;	AL contains the bank number for VDD use.
;	ECX contains the 32 bit address of the start of this memory.
;
	mov	edx,TotalMemorySize	;get total amount of video memory
	sub	edx,ecx 		;EDX = amount of memory from start
					;of VDD bank till end of video memory
	cmp	edx,64*1024		;do we have more than 64K left?
	mov	edx,64*1024		;(assume we do have more than 64K left)
	ja	MGVBGetBankingValues	;we can use an entire 64K bank for VDD!
	mov	edx,32*1024		;we can only use 32K for the VDD!
;
public	MGVBGetBankingValues
MGVBGetBankingValues:
;
;At this point:
;	AL contains the 64K bank number for the VDD virtualization bank.
;	ECX contains the 32 bit start address of the VDD virtualization area.
;	EDX contains the size of the VDD virtualization area (either 32K
;	    or 64K).
;
;We should setup the values for the V7 banking registers so that we can set
;them quickly when called upon by the "main" VDD to do so:
;
	push	edx			;save size of VDD area for now
	push	ebx			;we need this as a work register
	mov	BankRegSave,0ffh	;make sure this wasn't messed up
					;by board initialization
	mov	VDDBank,al		;save this for later compares
;
;Now, get the actual physical register values to set to the hardware:
;
	mov	bl,al			;this is value for Extension Reg F9H
	and	bl,1			;
	mov	VDDBankRegF9,bl 	;save it for use in setting VDD bank
;
	mov	ah,al			;get value to OR into 3C2H for VDD bank
	and	ah,2			;
	shl	ah,4			;this is the so-called "Page Select" bit
	mov	VDDBankReg3C2,ah	;
;
	and	al,0ch			;get value for Extension Reg F6H
	mov	bh,al			;
	shr	al,2			;duplicate this into both bit pairs
	or	bh,al			;this is the 256K bank select value
	mov	VDDBankRegF6,bh 	;
	pop	ebx			;restore saved registers
	pop	edx			;
	jmp	MGVBExit		;
;
public	MGVBMemoryShy
MGVBMemoryShy:
;
;If we reach this point, it means that the Windows visible screen overlaps
;into the very last bank of available video memory.  This creates a
;"memory-shy" configuration which prohibits us from running windowed
;and background EGA/VGA graphics mode apps.
;
;At this point:
;	ECX contains the value passed in at the beginning of the routine.
;
	mov	edx,ecx 		;indicate a "memory-shy" configuration
;
MGVBExit:
	ret
EndProc MiniVDD_GetVDDBank
;
;
subttl		Banking Handling Code for VGA Virtualization
page +
public	MiniVDD_SetVDDBank
BeginProc MiniVDD_SetVDDBank, DOSVM
;
;This routine is called when we need to virtualize something in the
;off-screen region of the video memory.  You should save the current
;state of the banking registers and then set your banking registers to
;the off-screen memory region.
;
;Entry:
;	EAX contains the page number requested for the VDD area.
;	EBX contains the MemC owner's VM handle.
;Exit:
;	Save any registers that you use.
;
	cmp	BankRegSave,0ffh	;are we already set to the VDD bank?
	jne	MSTVExit		;yes! just go adjust mid-bank offsets
	cmp	SuperVGAModeFlag,0	;running in 4 plane VGA mode?
	je	MSTVExit		;yes, don't do anything here
;
	push	eax			;
	push	ecx			;
	push	edx			;
	mov	edx,3cch		;EDX --> MiscOutputReg read index
	in	al,dx			;get current value
	mov	cl,al			;save this for getting old bank value
	shr	cl,4			;get bit that we want in 2's bit place
	and	cl,02h			;now we have 2's bit of our old bank
	and	al,NOT 20h		;get rid of old page select bit
	or	al,VDDBankReg3C2	;set the VDD bank's page select bit
	mov	dl,0c2h 		;EDX --> MiscOutputReg write index
	out	dx,al			;and set the page select stuff
;
	mov	dl,0c4h 		;EDX --> Sequencer Index Register
	in	al,dx			;get current value
	ror	eax,8			;and save it in high byte of EDX
;
	mov	ax,0ea06h		;this'll unlock the Video 7 extensions
	out	dx,ax			;
;
	mov	al,0f9h 		;set to register F9H
	out	dx,al			;
	inc	dl			;EDX --> Sequencer Data Register
	in	al,dx			;get current value of register F9H
	and	al,01h			;now we have 1's bit of our old bank
	or	cl,al			;now CL has 1's and 2's bit of old bank
	mov	al,VDDBankRegF9 	;get 1's bit of VDD bank
	out	dx,al			;set this bit
	dec	dl			;EDX --> Sequencer Index Register
;
	mov	al,0f6h 		;set to register F6H
	out	dx,al			;
	inc	dl			;EDX --> Sequencer Data Register
	in	al,dx			;get current value of register F6H
	mov	ch,al			;this has bits 2 & 3 of old bank value
	and	ch,03h			;isolate bits 2 & 3 of old bank value
	shl	ch,2			;get them into position
	or	cl,ch			;now CL has value for BankRegSave
	mov	BankRegSave,cl		;save it off for later restore
	and	al,0f0h 		;save unrelated stuff in high nibble
	or	al,VDDBankRegF6 	;now we have value for register F6
	out	dx,al			;
	dec	dl			;EDX --> Sequencer Index Register
;
	rol	eax,8			;AL now contains saved Seq index value
	out	dx,al			;
	pop	edx			;restore saved registers
	pop	ecx			;
	pop	eax			;
;
MSTVExit:
	ret				;
EndProc MiniVDD_SetVDDBank
;
;
public	MiniVDD_ResetBank
BeginProc MiniVDD_ResetBank, DOSVM
;
;This routine is called when the VDD is done using the off-screen memory
;and we want to restore to the bank that we were in before we started using
;the off-screen memory.  Note that if the bank that's currently set in the
;hardware's banking registers is NOT the VDD bank, that we do not wish to
;reset the banking since someone else purposely changed it and we don't
;want to override those purposeful changes.
;
;Entry:
;	EBX contains the VM handle.
;Exit:
;	Save anything that you use.
;
	cmp	BankRegSave,0ffh	;is there any banking to restore?
	je	MRBExit 		;nope, skip this!
;
;We may need to do something.  Get the bank that's currently set into the
;hardware and see if it's changed.  If it has, we conclude that someone's
;changed the banking without our knowledge and we shouldn't restore our
;saved bank.  If it hasn't changed, then we assume we should change it back.
;
	push	eax			;
	push	ecx			;
	push	edx			;
;
	mov	edx,3cch		;EDX --> MiscOutputReg read index
	in	al,dx			;get current value
	mov	cl,al			;CL is our work register
	shr	cl,4			;get bit that we want in 2's bit place
	and	cl,02h			;now we have 2's bit of current bank
;
	mov	dl,0c4h 		;EDX --> Sequencer Index Register
	in	al,dx			;get current value
	ror	eax,8			;and save it in high byte of EDX
;
	mov	ax,0ea06h		;this'll unlock the Video 7 extensions
	out	dx,ax			;
;
	mov	al,0f9h 		;set to register F9H
	out	dx,al			;
	inc	dl			;EDX --> Sequencer Data Register
	in	al,dx			;get current value of register F9H
	and	al,01h			;now we have 1's bit of current bank
	or	cl,al			;now CL has 1's and 2's bit of bank
	dec	dl			;EDX --> Sequencer Index Register
;
	mov	al,0f6h 		;set to register F6H
	out	dx,al			;
	inc	dl			;EDX --> Sequencer Data Register
	in	al,dx			;get current value of register F6H
	and	al,03h			;isolate bits 2 & 3 of old bank value
	shl	al,2			;get them into position
	or	cl,al			;now CL has value to compare
	dec	dl			;EDX --> Sequencer Index Register
;
	rol	eax,8			;AL now contains saved Seq index value
	out	dx,al			;restore it for now
;
	cmp	cl,VDDBank		;has the bank changed since we saved it?
	jne	MRBResetBankSaveFlags	;yes! better not do the reset
;
;It's safe to restore the banking registers!
;
	mov	dl,0cch 		;EDX --> MiscOutputReg read index
	in	al,dx			;get current value
	mov	ah,BankRegSave		;get 2's bit from BankRegSave
	and	ah,02h			;
	shl	ah,04h			;get it into the 20H's bit position
	and	ax,20dfh		;mask bits from both values
	or	al,ah			;combine the correct bits
	out	dx,al			;
;
	mov	dl,0c4h 		;EDX --> Sequencer Index Register
	in	al,dx			;get current value
	ror	eax,8			;and save it in high byte of EDX
;
	mov	al,0f9h 		;set to register F9H
	mov	ah,BankRegSave		;get 1's bit from BankRegSave
	and	ah,01h			;
	out	dx,ax			;set register F9H
;
	mov	al,BankRegSave		;get high 2 bits of BankRegSave
	and	al,0ch			;
	mov	ah,al			;duplicate it into both 2 bit pairs
	shr	al,2			;
	or	ah,al			;now AH has low nibble of register F6
	mov	al,0f6h 		;set to register F6H
	out	dx,al			;
	inc	dl			;EDX --> Sequencer Data Register
	in	al,dx			;get current value of register F6H
	and	ax,0ff0h		;mask proper nibbles
	or	al,ah			;combine them
	out	dx,al			;and set them to the hardware
	dec	dl			;EDX --> Sequencer Index Register
;
	rol	eax,8			;AL now contains saved Seq index value
	out	dx,al			;
;
public	MRBResetBankSaveFlags
MRBResetBankSaveFlags:
	mov	BankRegSave,0ffh	;flag that we're not set to VDD bank
	pop	edx			;restore saved registers
	pop	ecx			;
	pop	eax			;
;
MRBExit:
	ret				;
EndProc MiniVDD_ResetBank
;
;
subttl		Set To Latch Scratchpad Bank
page +
public	MiniVDD_SetLatchBank
BeginProc MiniVDD_SetLatchBank, DOSVM
;
;When virtualizing the VGA 4 plane mode, we have to save and restore the
;latches occasionally.	This routine allows you to set to an off-screen
;bank (in this case and in most cases, the VDD bank) in order to prepare
;for restoring the VGA latches.
;
;Entry:
;	Nothing assumed.
;Exit:
;	Save anything that you use.
;
	cmp	SuperVGAModeFlag,0	;running in 4 plane VGA mode?
	je	MSLBExit		;yes, don't do anything here
	push	eax			;save registers that we use
	push	edx			;
;
	mov	edx,3cch		;EDX --> MiscOutputReg read index
	in	al,dx			;get current value
	mov	LatchSave3C2,al 	;save this for later restore
	and	al,NOT 20h		;get rid of old page select bit
	or	al,VDDBankReg3C2	;set the VDD bank's page select bit
	mov	dl,0c2h 		;EDX --> MiscOutputReg write index
	out	dx,al			;and set the page select stuff
;
	mov	dl,0c4h 		;EDX --> Sequencer Index Register
	in	al,dx			;get current value
	ror	eax,8			;and save it in high byte of EDX
;
	mov	ax,0ea06h		;this'll unlock the Video 7 extensions
	out	dx,ax			;
;
	mov	al,0f9h 		;set to register F9H
	out	dx,al			;
	inc	dl			;EDX --> Sequencer Data Register
	in	al,dx			;get current value of register F9H
	mov	LatchSaveF9,al		;save it for later restore
	mov	al,VDDBankRegF9 	;get 1's bit of VDD bank
	out	dx,al			;set this bit
	dec	dl			;EDX --> Sequencer Index Register
;
	mov	al,0f6h 		;set to register F6H
	out	dx,al			;
	inc	dl			;EDX --> Sequencer Data Register
	in	al,dx			;get current value of register F6H
	mov	LatchSaveF6,al		;save it for later restore
	and	al,0f0h 		;save unrelated stuff in high nibble
	or	al,VDDBankRegF6 	;now we have value for register F6
	out	dx,al			;
	dec	dl			;EDX --> Sequencer Index Register
;
	rol	eax,8			;AL now contains saved Seq index value
	out	dx,al			;
	pop	edx			;restore saved registers
	pop	eax			;
;
MSLBExit:
	ret				;
EndProc MiniVDD_SetLatchBank
;
;
subttl		Reset Banking After Latch Operations
page +
public	MiniVDD_ResetLatchBank
BeginProc MiniVDD_ResetLatchBank, DOSVM
;
;This routine reverses the latch save that we did prior to restoring the
;latches.  Just restore the states that you saved.
;
;Entry:
;	Nothing assumed.
;Exit:
;	Save anything that you use.
;
	cmp	SuperVGAModeFlag,0	;running in 4 plane VGA mode?
	je	MRLBExit		;yes, don't do anything here
	push	eax			;save registers that we use
	push	edx			;
;
;Just restore the three saved registers:
;
	mov	edx,3c2h		;EDX --> MiscOutputReg write index
	mov	al,LatchSave3C2 	;get saved value for 3C2H
	out	dx,al			;and restore it
;
	mov	dl,0c4h 		;EDX --> Sequencer Index Register
	in	al,dx			;get current value
	ror	eax,8			;and save it in high byte of EDX
;
	mov	ax,0ea06h		;this'll unlock the Video 7 extensions
	out	dx,ax			;
;
	mov	al,0f9h 		;set to register F9H
	mov	ah,LatchSaveF9		;get saved value
	out	dx,ax			;restore it
;
	mov	al,0f6h 		;set to register F6H
	mov	ah,LatchSaveF6		;get saved value
	out	dx,ax			;restore it
;
	rol	eax,8			;AL now contains saved Seq index value
	out	dx,al			;
;
	pop	edx			;restore saved registers
	pop	eax			;
;
MRLBExit:
	ret				;
EndProc MiniVDD_ResetLatchBank
;
;
subttl		Save and Restore Routines for Extension Registers
page +
public	MiniVDD_SaveRegisters
BeginProc MiniVDD_SaveRegisters, DOSVM
;
;This routine is called whenever the "main" VDD is called upon to save
;the register state.  The "main" VDD will save all of the states of the
;"normal" VGA registers (CRTC, Sequencer, GCR, Attribute etc.) and will
;then call this routine to allow the mini-VDD to save register states
;that are hardware dependent.  These registers will be restored during the
;routine MiniVDD_RestoreRegisters.  Typically, only a few registers
;relating to the memory access mode of the board are necessary to save
;and restore.  Specific registers that control the hardware BLTer probably
;need not be saved.  The register state is saved in the mini-VDD's
;"CB data area" which is a per-VM state saving area.  The mini-VDD's
;CB data area was allocated at Device_Init time.
;
;Entry:
;	EBX contains the VM handle for which these registers are being saved.
;Exit:
;	You must preserve EBX, EDX, EDI, and ESI.
;
	push	edx			;save required registers
	push	edi			;
	cmp	ebx,MessageModeVMHandle ;saving for the Message Mode VM?
	je	MSRSExit		;yes, don't bother
	cmp	ebx,PlanarModeVMHandle	;saving for the Planar Mode VM?
	je	MSRSExit		;yes, don't bother
;
;First, unlock the Video7 Extension registers:
;
	mov	edx,3c4h		;EDX --> Sequencer index register
	in	al,dx			;get and save current Sequencer index
	ror	eax,8			;in high byte of EAX
;
;Video7 VRAM I cards do screwy things to Sequencer Register 4 during a text mode
;scroll.  These things screw up the saved state of the Main VDD.  Therefore,
;since the main VDD hasn't saved the Sequencer Registers yet, we change the
;physical state of Sequencer Register 4 to what it should be.  Then, the
;Main VDD gets the correct state saved.
;
	cmp	ebx,WindowsVMHandle	;is Windows VM doing this state save?
	je	MSRSSaveV7Extensions	;yes, don't mess things up!
	push	ebx			;save this register
	add	ebx,OurCBDataPointer	;make EBX --> CB data area
	test	[ebx].GCRReg6State,01h	;are we in text or graphics mode?
	pop	ebx			;(restore EBX --> VM handle)
	jnz	MSRSSaveV7Extensions	;in graphics mode, nothing to fix
	mov	al,4			;set to index 4
	out	dx,al			;
	inc	dl			;EDX --> Sequencer Data Register
	in	al,dx			;get its value
	cmp	al,06h			;could BIOS be messing around?
	jne	@F			;nope, continue
	mov	al,02h			;yes, fix the problem!
	out	dx,al			;
;
;Oh golly gosh!  The VRAM I messes with the GCR Register 8 to do scrolling too!
;We'd better fix it by restoring it to an FFH!
;
@@:	mov	dl,0ceh 		;EDX --> GCR Index Register
	in	al,dx			;get current GCR index
	mov	ah,al			;save it in AH
	mov	al,08h			;set to index 8
	out	dx,al			;
	inc	dl			;EDX --> GCR Data Register
	in	al,dx			;get current value
	or	al,al			;could BIOS be messing around?
	jnz	@F			;nope, continue
	dec	al			;make AL == FFH
	out	dx,al			;and set GCR register 8 accordingly
@@:	dec	dl			;EDX --> GCR Index Register
	mov	al,ah			;reset GCR index register
	out	dx,al			;
;
public	MSRSSaveV7Extensions
MSRSSaveV7Extensions:
;
;Now, begin the save state of all the Video7 Extension Registers.
;
	mov	dl,0c4h 		;EDX --> Sequencer Index Register
	mov	ax,0ea06h		;this'll unlock the Video 7 extensions
	out	dx,ax			;
;
;Now, setup a loop to save all of the V7 extension states:
;
	mov	edi,ebx 		;get pointer to our CB data area
	add	edi,OurCBDataPointer	;EDI --> our CB data area for this VM
	mov	ecx,80h 		;this is starting register to read
;
MSRSLoop:
	mov	al,cl			;
	out	dx,al			;
	inc	edx			;EDX --> Seq data register
	in	al,dx			;get data
	mov	[edi+ecx-80h],al	;save the data in our CB data structure
	dec	edx			;EDX --> Seq index register
	inc	cl			;bump to next index to read
	jnz	MSRSLoop		;if we didn't roll over to 0, keep going
;
;Restore the Sequencer index register saved in the high byte of EAX:
;
	rol	eax,8			;
	out	dx,al			;
;
MSRSExit:
	pop	edi			;restore saved registers
	pop	edx			;
	ret
EndProc MiniVDD_SaveRegisters
;
;
public	MiniVDD_RestoreRegisters
BeginProc MiniVDD_RestoreRegisters, DOSVM
;
;This routine is called whenever the "main" VDD is called upon to restore
;the register state.  The "main" VDD will restore all of the states of the
;"normal" VGA registers (CRTC, Sequencer, GCR, Attribute etc.) and will
;then call this routine to allow the mini-VDD to restore register states
;that are hardware dependent.  These registers were saved during the
;routine MiniVDD_SaveRegisterState.  Typically, only a few registers
;relating to the memory access mode of the board are necessary to save
;and restore.  Specific registers that control the hardware BLTer probably
;need not be saved.  The register state is saved in the mini-VDD's
;"CB data area" which is a per-VM state saving area.  The mini-VDD's
;CB data area was allocated at Device_Init time.
;
;Entry:
;	ESI contains the VM handle for the MemC owner VM.
;	ECX contains the VM handle for the CRTC owner VM.
;Exit:
;	You must preserve EBX and EDX.	The caller preserves everything else.
;
;A short explanation of the terms "CRTC owner" and "MemC owner" is in order.
;The CRTC owner VM is the VM that owns the screen mode.  If you're running
;on the Windows desktop, then the Windows VM is the CRTC owner.  If you're
;running a full-screen DOS box, then that DOS box's VM is the CRTC owner.
;If you're running a DOS box in a window, then the CRTC owner is Windows
;but the MemC owner is the DOS VM.  What significance does this have?
;Well, when you restore the register state of a DOS VM running in a
;Window, it means that you're getting ready to VIRTUALIZE the VGA by
;using the off-screen memory.  Your VGA hardware must be setup to write
;to this memory in EXACTLY THE SAME WAY AS IF THE VGA WAS RUNNING IN
;NATIVE VGA MODE.  But...  you also must preserve the appearance of the
;Windows screen which is being displayed on the VISIBLE screen.  Thus,
;we have the screen looking like it's running in Windows HiRes packed
;pixel mode from the user's perspective, but the CPU sees the video
;memory as a 4 plane VGA.  Thus, we present this routine with both
;the CRTC owner and the MemC owner's VM handles.  Therefore, you can
;restore those states from the CRTC owner that preserve the appearance
;of the screen while restoring those states from the MemC owner that
;control how the CPU sees the video memory.
;
;For Video7, we only want to restore the timing registers when entering
;Windows HiRes mode when the SuperVGAModeFlag is zero.	In this case,
;we're running with the VGA or SuperVGA drivers and the display driver
;is not involved in resetting to Windows HiRes mode as it is when we're
;running with FRAMEBUF.DRV.  Therefore, we must restore the timing registers
;for the Windows HiRes (640x480x4 or 800x600x4) running state.
;
	push	edx			;save caller's EDX
;
;Video7 VRAM I cards do screwy things to GCR Register 8 during a text mode
;scroll.  These things screw up the saved state of the Main VDD.  Therefore,
;since the main VDD hasn't saved the Sequencer Registers yet, we change the
;physical state of GCR Register 8 to what it should be.  Then, the
;Main VDD gets the correct state saved.
;
	cmp	ebx,WindowsVMHandle	;is Windows VM doing this state save?
	je	MRRSRestoreV7Extensions ;yes, don't mess things up!
	cmp	ebx,MessageModeVMHandle ;are we restoring for Message Mode?
	je	MRRSRestoreV7Extensions ;yes, don't mess things up!
	cmp	ebx,PlanarModeVMHandle	;are we restoring forced Planar Mode?
	je	MRRSExit		;yes, don't bother
	push	ebx			;save this register
	add	ebx,OurCBDataPointer	;make EBX --> CB data area
	test	[ebx].GCRReg6State,01h	;are we in text or graphics mode?
	pop	ebx			;(restore EBX --> VM handle)
	jnz	MRRSRestoreV7Extensions ;in graphics mode, nothing to fix
	mov	edx,3ceh		;EDX --> GCR Index Register
	in	al,dx			;get current GCR index
	mov	ah,al			;save it in AH
	mov	al,08h			;set to index 8
	out	dx,al			;
	inc	dl			;EDX --> GCR Data Register
	in	al,dx			;get current value
	or	al,al			;could BIOS be messing around?
	jnz	@F			;nope, continue
	dec	al			;make AL == FFH
	out	dx,al			;and set GCR register 8 accordingly
@@:	dec	dl			;EDX --> GCR Index Register
	mov	al,ah			;reset GCR index register
	out	dx,al			;
;
public	MRRSRestoreV7Extensions
MRRSRestoreV7Extensions:
;
;First, unlock the Video7 Extension registers:
;
	mov	edx,3c4h		;EDX --> Sequencer index register
	in	al,dx			;get and save current Sequencer index
	ror	eax,8			;in high byte of EAX
	mov	ax,0ea06h		;this'll unlock the Video 7 extensions
	out	dx,ax			;
	cmp	ecx,MessageModeVMHandle ;are we restoring Message Mode?
	jne	@F			;nope, go add on CB data pointer
	mov	ecx,OFFSET32 MessageModeCBData
	sub	ecx,OurCBDataPointer	;(correct for add in next line)
@@:	add	ecx,OurCBDataPointer	;ECX --> CB data area for CRTC VM
;
public	MRRSCheckIfRestoreNeeded
MRRSCheckIfRestoreNeeded:
;
;In order to set the registers that we're concerned with, we need to put
;the Sequencer in reset mode.  This blanks the screen.	Therefore, we
;don't want to do it unless the state of the registers that we're concerned
;with have changed.  So, read the registers first. If any of them have
;changed, then do the restore.	Otherwise, skip it.
;
	xor	al,al			;make this zero for compares
	or	al,[ecx].V7ExA4 	;get state of A4 saved state OR'd in
	or	al,[ecx].V7ExF8 	;get state of F8 saved state OR'd in
	or	al,[ecx].V7ExFD 	;get state of FD saved state OR'd in
	jz	MRRSDoneWithRestore	;VM state isn't init'd yet, don't do it!
;
	mov	al,0a4h 		;set to index A4H
	out	dx,al			;
	inc	edx			;EDX --> Seq data register
	in	al,dx			;get data
	dec	edx			;EDX --> Seq index register
	cmp	al,[ecx].V7ExA4 	;is it the same as what we've saved?
	jne	MRRSDoRestore		;nope, better do the restore
;
	mov	al,0f8h 		;set to index F8H
	out	dx,al			;
	inc	edx			;EDX --> Seq data register
	in	al,dx			;get data
	dec	edx			;EDX --> Seq index register
	cmp	al,[ecx].V7ExF8 	;is it the same as what we've saved?
	jne	MRRSDoRestore		;nope, better do the restore
;
	mov	al,0fdh 		;set to index FDH
	out	dx,al			;
	inc	edx			;EDX --> Seq data register
	in	al,dx			;get data
	dec	edx			;EDX --> Seq index register
	cmp	al,[ecx].V7ExFD 	;is it the same as what we've saved?
	je	MRRSDoneWithRestore	;yep, skip the restore
;
public	MRRSDoRestore
MRRSDoRestore:
;
;Put ourselves in Sequencer Reset mode:
;
	mov	eax,0100h		;
	out	dx,ax			;
;
;Now, restore the three timing registers that we care about:
;
	mov	al,0a4h 		;set to index A4H
	mov	ah,[ecx].V7ExA4 	;get the data from our CB data structure
	out	dx,ax			;
;
	mov	al,0f8h 		;set to index F8H
	mov	ah,[ecx].V7ExF8 	;get the data from our CB data structure
	out	dx,ax			;
;
	mov	al,0fdh 		;set to index FDH
	mov	ah,[ecx].V7ExFD 	;get the data from our CB data structure
	out	dx,ax			;
;
;Put ourselves out of Sequencer Reset state:
;
	mov	ax,0300h		;
	out	dx,ax			;
;
public	MRRSDoneWithRestore
MRRSDoneWithRestore:
;
;Restore the Sequencer index register saved in the high byte of EAX:
;
	rol	eax,8			;
	out	dx,al			;
;
MRRSExit:
	pop	edx			;
	ret				;
EndProc MiniVDD_RestoreRegisters
;
;
subttl		Flag That We're Starting and Ending a CRTC Mode Change
page +
public	MiniVDD_PreCRTCModeChange
BeginProc MiniVDD_PreCRTCModeChange, DOSVM
	mov	InModeChangeFlag,0ffh	;flag that we're in a VDD CRTC change
	ret				;
EndProc MiniVDD_PreCRTCModeChange
;
;
public	MiniVDD_PostCRTCModeChange
BeginProc MiniVDD_PostCRTCModeChange, DOSVM
	mov	InModeChangeFlag,0	;flag that we're NOT in a CRTC change
	ret				;
EndProc MiniVDD_PostCRTCModeChange
;
;
subttl		Display Driver Is Being Disabled Notification
page +
public	MiniVDD_DisplayDriverDisabling
BeginProc	MiniVDD_DisplayDriverDisabling, RARE
;
;The display driver is in its Disable routine and is about to set the
;hardware back into VGA text mode.  Since this could either mean that
;the Windows session is ending or that some Windows application is switching
;to a VGA mode to display something full screen (such as MediaPlayer), we
;need to disable our MiniVDD_RestoreRegisters code because we're liable
;to restore a Windows HiRes state when we shouldn't!  Thus, clear the
;DisplayEnabledFlag to prevent this:
;
	mov	SuperVGAModeFlag,0	;don't do a RestoreRegState
;
MDDDExit:
	ret				;
EndProc MiniVDD_DisplayDriverDisabling
;
;
subttl		Collect States From Sequencer OUT's
page +
public	MiniVDD_VirtualizeSequencerOut
BeginProc MiniVDD_VirtualizeSequencerOut, DOSVM
;
;Entry:
;	AL contains the value to output to Sequencer.
;	EBX contains the VM handle of the VM doing the OUT.
;	ECX contains the index to output.
;	EDX --> Sequencer Data Register.
;Exit:
;	If it isn't one of ours, return NC.
;	If it is one of ours, return CY.
;
;The Video7 VRAM I ROM BIOS does screwy things to Sequencer Register 4 during
;screen scrolls.  It sets it to an 06H which is "planar" state instead of 02H
;which is "text mode" state.  We must therefore attempt to fix this in the
;Main VDD's saved state so that it never restores an 06H during a CRTC mode
;change.
;
	cmp	ReentryFlag,0		;are we being reentered?
	jne	MVSONotOneofOurs	;nope, continue
	mov	ReentryFlag,0ffh	;prevent ourselves from being reentered
	cmp	cl,04h			;is it Sequencer Register 04H?
	jne	MVSOVirtualV7Extensions ;nope, continue
	cmp	ebx,WindowsVMHandle	;is VM doing the OUT the Windows VM?
	je	MVSOVirtualV7Extensions ;yes, we're not interested
	cmp	al,06h			;is he trying to get screwy with us?
	jne	MVSOVirtualV7Extensions ;nope, just let it go through
;
;We are trying to set Sequencer Register 04H to 06H.  We must do some more
;investigation to see if we're in text mode or graphics mode.  If we're in
;text mode (indicated by GCR Register 06H, bit 0 being 0), we must do the
;fixup.
;
	push	ebx			;save this register
	add	ebx,OurCBDataPointer	;make EBX --> CB data area
	mov	ah,[ebx].GCRReg6State	;get state of GCR register 6
	mov	ch,[ebx].SeqReg2State	;get state of Sequencer register 2
	pop	ebx			;(restore EBX --> VM handle)
	cmp	ch,03h			;in a text mode (planes 0 & 1 mapped)?
	mov	ch,0			;(clear this without disturbing flags)
	jne	MVSOVirtualV7Extensions ;
	test	ah,01h			;in graphics mode?
	jnz	MVSOVirtualV7Extensions ;in graphics mode, nothing to fix

;
public	MVSOFixScrewyBIOS
MVSOFixScrewyBIOS:
;
;OK, we're setting Sequencer Register 04H to an 06H and we're in text mode.
;This means that the BIOS is doing the screwy scrolling trick.	We must
;therefore simulate I/O to the Sequencer Register from this VM
;
	push	eax			;
	push	ebx			;
	push	ecx			;
	push	edx			;
	mov	eax,0204h		;
	mov	ecx,WORD_OUTPUT 	;
	dec	dl			;EDX --> Sequencer Index Register
	VMMCall Simulate_VM_IO		;
	pop	edx			;
	pop	ecx			;
	pop	ebx			;
	pop	eax			;
	VxDCall VDD_Get_VM_Info 	;get CRTC owner VM handle in EDI
	cmp	ebx,edi 		;is CRTC controlled by caller?
	je	MVSOPhysicalOUT 	;yes, go do physical OUT
	jmp	MVSOWeHandledIt 	;nope, don't do physical OUT!
;
public	MVSOVirtualV7Extensions
MVSOVirtualV7Extensions:
	cmp	InModeChangeFlag,0	;in a VDD CRTC change?
	jne	MVSOPhysicalOUT 	;yes, don't save the state
	mov	edi,ebx 		;get our CB data pointer in EDX
	add	edi,OurCBDataPointer	;
	cmp	cl,02h			;do we need to collect register 2 state?
	jne	@F			;nope, continue
	mov	[edi].SeqReg2State,al	;save the state
	jmp	MVSONotOneOfOurs	;and send it back to the main VDD
@@:	cmp	cl,80h			;is it one of ours?
	jb	MVSONotOneofOurs	;nope, let caller handle it
	and	ecx,0ffh		;just in case index is messed up
	sub	cl,80h			;now index is offset from 0
	mov	[edi+ecx],al		;save the value virtually
	VxDCall VDD_Get_VM_Info 	;get CRTC owner VM handle in EDI
	cmp	ebx,edi 		;is CRTC controlled by caller?
	jne	MVSOWeHandledIt 	;nope, we just virtualized it
	cmp	DoneGettingMsgModeFlag,0;are we done getting Msg Mode state?
	je	MVSOWeHandledIt 	;not yet!  better not do physical write!
	add	cl,80h			;restore index = 80H --> FFH
;
public	MVSOPhysicalOUT
MVSOPhysicalOUT:
	dec	dl			;EDX --> Sequencer Index Register
	mov	ch,al			;get value for data register in CH
	in	al,dx			;get current value of index register
	ror	eax,8			;save it in high byte of EAX
	mov	ax,cx			;get index in AL, data in AH
	out	dx,ax			;set index and data to hardware
	rol	eax,8			;get saved index back in AL
	out	dx,al			;and restore it to the hardware
;
public	MVSOWeHandledIt
MVSOWeHandledIt:
	stc				;indicate that we handled it
	jmp	MVSOExit		;and go return the value
;
MVSONotOneofOurs:
	clc				;clear CY so caller will handle it
;
MVSOExit:
	mov	ReentryFlag,0		;free the semaphore
	ret				;
EndProc MiniVDD_VirtualizeSequencerOut
;
;
subttl		Return States Requested By Sequencer IN's
page +
public	MiniVDD_VirtualizeSequencerIn
BeginProc MiniVDD_VirtualizeSequencerIn, DOSVM
;
;Entry:
;	EBX contains the VM handle of the VM doing the IN.
;	ECX contains the index to read.
;	EDX --> Sequencer Data Register.
;Exit:
;	AL contains the value to return to the caller.
;	If it isn't one of ours, preserve the flags.
;	If it is one of ours, return the value and set CY flag.
;
	pushf				;save the flags over this
	cmp	cl,80h			;is it a Video7 extension register?
	jb	MVSINotOneofOurs	;nope, leave
;
public	MVSIItsOneofOurs
MVSIItsOneofOurs:
	popf				;fix the Stack
	VxDCall VDD_Get_VM_Info 	;get CRTC owner VM handle in EDI
	cmp	ebx,edi 		;is CRTC controlled by caller?
	je	MVSIPhysicalIN		;yes, go do the physical IN
	cmp	InModeChangeFlag,0	;in a VDD CRTC change?
	jne	MVSIPhysicalIN		;yes, go do the physical IN
;
public	MVSIVirtualIN
MVSIVirtualIN:
	sub	ecx,80h 		;extension registers start at 80H
	add	ecx,OurCBDataPointer	;now ECX --> place to retrieve from
	mov	al,[ecx+ebx]		;return the value from this register
	stc				;indicate that we handled it
	jmp	MVSIExit		;and go return the value
;
public	MVSIPhysicalIN
MVSIPhysicalIN:
	dec	dl			;EDX --> Sequencer Index Register
	in	al,dx			;get current value of this register
	mov	ah,al			;save it in AH
	mov	al,cl			;get index to read
	out	dx,al			;set it to hardware
	inc	dl			;EDX --> Sequencer Data Register
	in	al,dx			;get the data into AL
	dec	dl			;EDX --> Sequencer Index Register
	xchg	al,ah			;save data just read in AH
					;get saved index in AL
	out	dx,al			;restore saved value of index register
	mov	al,ah			;and return physical data in AL
	stc				;indicate that we handled it
	jmp	MVSIExit		;and go return the value
;
MVSINotOneofOurs:
	popf				;restore flags so caller will handle it
;
MVSIExit:
	ret				;
EndProc MiniVDD_VirtualizeSequencerIn
;
;
subttl		Virtualize GCR OUT's
page +
public	MiniVDD_VirtualizeGCROut
BeginProc MiniVDD_VirtualizeGCROut, DOSVM
;
;Entry:
;	AL contains the value to output to Sequencer.
;	EBX contains the VM handle of the VM doing the OUT.
;	ECX contains the index to output.
;	EDX --> Sequencer Data Register.
;Exit:
;	We always return NC since we are simply examining writes to GCR 06H.
;
	pushfd				;save the flags
	cmp	cl,06h			;is it one we're interested in?
	jne	MVGOExit		;nope, we're done!
	push	ebx			;save our VM handle
	add	ebx,OurCBDataPointer	;EBX --> CB data area for this VM
	mov	[ebx].GCRReg6State,al	;just save the state
	pop	ebx			;restore saved VM handle
;
MVGOExit:
	popfd				;restore the flags
	ret				;and we're done
EndProc MiniVDD_VirtualizeGCROut
;
;
subttl		Save Message Mode State For V7 Extended Registers
page +
public	MiniVDD_SaveMsgModeState
BeginProc MiniVDD_SaveMsgModeState, RARE
;
;This routine allows the MiniVDD to copy virtualized states and save them in
;a special Message Mode state structure.  When this routine is called, the
;Main VDD has executed a totally virtualized mode change to the blue-screen
;Message Mode state (ie: Mode 3 in SBCS Windows and Mode 12H in Far East
;Windows).  Therefore, no screen changes are evident but the states have
;been saved in the CB data structure for the Windows VM (since the virtualized
;mode change was done when the Windows VM was the "Currently executing VM".
;Thus, when this routine is called, we have the Message Mode state for all
;of the V7 Extension Registers saved in the CB data structure for the
;Windows VM.  We simply copy this state into the special Message Mode state
;data structure.
;
;Entry:
;	Nothing assumed.
;Exit:
;	Save everything that you use!
;
;The source of our state save copy is the Windows VM's CB data structure:
;
	cld				;make sure direction is set forward
	push	esi			;
	push	edi			;
	push	ecx			;
	mov	esi,WindowsVMHandle	;
	add	esi,OurCBDataPointer	;ESI --> Windows VM's CB data
	mov	edi,OFFSET32 MessageModeCBData
					;EDI --> Message Mode VM's CB data
	mov	ecx,size PerVMData	;
	rep	movsb			;copy the Message Mode state!
	mov	DoneGettingMsgModeFlag,-1
					;indicate that we've got Msg Mode state
;
SMMSExit:
	pop	ecx			;restore saved registers
	pop	edi			;
	pop	esi			;
	ret				;we're done
EndProc MiniVDD_SaveMsgModeState
;
;
subttl			Return Total Memory Size For VESA Save/Restore
page +
public	MiniVDD_GetTotalVRAMSize
BeginProc MiniVDD_GetTotalVRAMSize
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
;It is VERY important that this VESA function is implemented for Video 7 since
;their VESA.COM doesn't do it correctly!
;
	mov	ecx,TotalMemorySize	;
	stc				;
	ret				;return to caller
EndProc MiniVDD_GetTotalVRAMSize
;
;
VxD_LOCKED_CODE_ENDS
;
;
end
