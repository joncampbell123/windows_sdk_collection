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
title		Tseng Labs ET-4000 Mini-VDD Support Functions
.386p
;
;
.xlist
include 	VMM.INC
include 	MINIVDD.INC
include 	TSENG.INC
.list
;
;
subttl		Virtual Device Declaration
page +
Declare_Virtual_Device	TSENG,					\
			3,					\
			1,					\
			MiniVDD_Control,			\
			Undefined_Device_ID,			\
			VDD_Init_Order, 			\
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
WindowsVMHandle 	dd	?		;init'd at Device_Init
;
public	DisplayDriverStateAddr
DisplayDriverStateAddr	dd	?		;init'd at RegisterDisplayDriver
;
public	PlanarStateID, MessageModeID
PlanarStateID		dd	?		;init'd at RegisterPlanarState
MessageModeID		dd	?		;init'd at RegisterMessageMode
;
public	TotalMemorySize
TotalMemorySize 	dd	?		;init'd at Sys_Critical_Init
;
public	OurCBDataPointer
OurCBDataPointer	dd	?		;init'd at Device_Init
;
public	W32MemMappedSelector
W32MemMappedSelector	dd	?		;init'd at RegisterDisplayDriver
;
public	ChipID					;ET-4000 specific variable!
ChipID			db	?		;init'd at Sys_Critical_Init
;
public	TLModeChangeFlags
TLModeChangeFlags	db	?		;=FFH when in BIOS mode change
;
public	WindowsMiscOutputState
WindowsMiscOutputState	db	0		;init'd at DisplayDriverRegister
;
public	VDDBankLow, VDDBankHigh, BankRegSaveLow, BankRegSaveHigh
public	LatchSaveLow, LatchSaveHigh
VDDBankHigh		db	?		;VDDBankHigh and VDDBankLow
VDDBankLow		db	?		;must be in this order!!!
.errnz	offset VDDBankLow - offset VDDBankHigh - 1
BankRegSaveHigh 	db	?		;these must be left in
BankRegSaveLow		db	?		;proper order!
.errnz	offset BankRegSaveLow - offset BankRegSaveHigh - 1
LatchSaveLow		db	?		;
LatchSaveHigh		db	?		;
;
public	VDDPageOffsetFromBank
VDDPageOffsetFromBank	db	0		;
;
VxD_DATA_ENDS
;
;
subttl		Device Initialization
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
	mov	TLModeChangeFlags,0	;initialize this
;
;First, get the ChipID and fail if we're not a Tseng chipset:
;
	mov	dx,3cbh 		;Segment Select 2, W32 only
	in	al,dx			;read it
	ror	eax,8			;and save it in high byte of EAX
	mov	al,11h			;Test value
	out	dx,al			;Write it
	in	al,dx			;Read it
	and	al,33h			;Mask undefine bits
	cmp	al,11h			;Did we get back what we wrote?
	jne	MVDINotW32		;Nope, we're not a W32
	mov	al,22h			;Test value
	out	dx,al			;Write it
	in	al,dx			;Read it
	and	al,33h			;Mask undefine bits
	cmp	al,22h			;Did we get back what we wrote?
	jne	MVDINotW32		;Nope, we're not a W32
;
public	MVDIItsaW32
MVDIItsaW32:
	rol	eax,8			;Restore saved register state of 3CBH
	out	dx,al			;
	mov	ChipID,TL_W32		;indicate that we're a W32
	jmp	MVDIGetSpecialVMs	;and return to caller
;
public	MVDINotW32
MVDINotW32:
;
;We know we're not an ET-4000-W32. See if we're a plain ET-4000:
;
	rol	eax,8			;Restore saved register state of 3CBH
	out	dx,al			;
	mov	dx,3cdh 	        ;Set to bank switch register
	in	al,dx			;read it
	ror	eax,8			;and save it in high byte of EAX
	cmp	al,0FFh			;Does it look like it might exist?
	je	short @f		;no, or the bank is at 15. Skip unlock.

; write ET3000/4000 unlock "KEY"

	mov	dx,3bfh
	mov	al,3
	out	dx,al
	mov	dx,3D8h
	mov	al,0A0h
	out	dx,al
	mov	dx,3cdh

@@:	mov	al,11h			;Test value			 							
	out	dx,al			;Write it
	in	al,dx			;Read it
	cmp	al,11h			;Did we get back what we wrote?
	jne	MVDINotET4000		;Nope, we're not an ET-4000
	mov	al,22h			;Test value
	out	dx,al			;Write it
	in	al,dx			;Read it
	cmp	al,22h			;Did we get back what we wrote?
	jne	MVDINotET4000		;Nope, we're not an ET-4000

; Check for ET3000 h/w

	push	ebx
	mov	dx,3d4h
	in	al,dx
	mov	bl,al
	mov	al,33h
	out	dx,al
	inc	dx
	in	al,dx
	mov	bh,al		; save 3d4, 3d5 in bl,bh

	mov	al,05h		; high nibble always reads back as zero.
	out	dx,al
	dec	dx		; re-select the index register to avoid
	mov	al,33h		;  bus noise
	out	dx,al
	inc	dx
	in	al,dx		; get back value.
	mov	cl,al		; save for later comparison

	mov	al,0Ah
	out	dx,al
	dec	dx		; re-select the index register to avoid
	mov	al,33h		;  bus noise
	out	dx,al
	inc	dx
	in	al,dx		; get back value.
	mov	ch,al		; save for later comparison

	mov	al,bh
	out	dx,al		; restore original value of 3d5.
	dec	dx
	mov	al,bl
	out	dx,al		; restore original value of 3d4.
	pop	ebx

	cmp	cx,0A05h	; ET4000 if we can read back what we wrote.
	jne	MVDINotET4000	;Nope, we're not an ET-4000
;
public	MVDIItsanET4000
MVDIItsanET4000:
	mov	dx,3cdh
	rol	eax,8			;Restore saved register state of 3CDH
	out	dx,al			;
	mov	ChipID,ET4000		;indicate that we're an ET-4000
	jmp	MVDIGetSpecialVMs	;and we're done
;
public	MVDINotET4000
MVDINotET4000:
	rol	eax,8			;Restore saved register state of 3CDH
	out	dx,al			;
	jmp	MVDIErrorExit		;don't load this MiniVDD
;
public	MVDIGetSpecialVMs
MVDIGetSpecialVMs:
;
;There are two special VM states that the "main" VDD establishes.  The
;first is the planar state which is simply a state that the "main" VDD
;restores to establish the VGA 4 plane mode.  When we restore the states
;at MiniVDD_RestoreRegisterState, we check for the special identifier which
;flags that we're restoring this state.  If we find that we are restoring
;the planar state, we have the opportunity to special case the register
;state restore.
;
;Similarly, there is a special state called "Message Mode".  This is the
;state when the Shell VxD is displaying a full-screen message (usually
;with a blue background) telling the user of a serious anomaly in the
;running state of Windows.  We also retrieve the special VM handle
;for the "Message Mode" state so we can handle it special too if needed.
;
	VxDCall VDD_Get_Special_VM_IDs	;go get special VM information
	mov	PlanarStateID,esi	;save off returned identifiers
	mov	MessageModeID,edi	;
;
public	MVDIInitET4000
MVDIInitET4000:
;
;We want to allocate space in the per-VM CB data structure.  This will
;be used by the mini-VDD to store per-VM data relating to states of registers.
;
	VMMCall _Allocate_Device_CB_Area,<<size PerVMData>,0>
	mov	OurCBDataPointer,eax	;save offset our our VxD's area
	or	eax,eax 		;was call sucessful?
	jz	MVDIErrorExit		;nope, leave fatally!
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
;is less than what we think it is, we return an error and don't allow
;Windows to continue running.
;
	VxDCall VDD_Get_Mini_Dispatch_Table
	cmp	ecx,NBR_MINI_VDD_FUNCTIONS	;perform a cursory version check
	jb	MVDIErrorExit			;oops, versions don't match!
;
public	MVDIFillInTable
MVDIFillInTable:
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
	MiniVDDDispatch PRE_HIRES_TO_VGA,PreHiResToVGA
	MiniVDDDispatch POST_HIRES_TO_VGA,PostHiResToVGA
	MiniVDDDispatch PRE_VGA_TO_HIRES,PreVGAToHiRes
	MiniVDDDispatch POST_VGA_TO_HIRES,PostVGAToHiRes
	MiniVDDDispatch RESTORE_REGISTERS,RestoreRegisters
	MiniVDDDispatch GET_CURRENT_BANK_READ, GetCurrentBankRead
	MiniVDDDispatch GET_CURRENT_BANK_WRITE, GetCurrentBankWrite
	MiniVDDDispatch SET_BANK, SetBank
	MiniVDDDispatch GET_CHIP_ID, GetChipID
	cmp	ChipID,ET4000			;running on an ET4000 (not W32)?
	je	MVDITakeoverBankingPorts	;yes, skip this
	MiniVDDDispatch VIRTUALIZE_CRTC_OUT,VirtualizeCRTCOut
	MiniVDDDispatch ENABLE_TRAPS,EnableTraps
	MiniVDDDispatch DISABLE_TRAPS,DisableTraps
;
public	MVDITakeoverBankingPorts
MVDITakeoverBankingPorts:
;
;On the ET-4000, the ROM BIOS oft-times reads and writes the banking ports
;3CDH and 3CBH (W32 only).  We must stop this for windowed VM's since the
;VDD must control the banking.	Therefore, we register virtualization routines
;which will do the correct actions with these ports.
;
	mov	edx,3cdh			;takeover port 3CDH
	mov	ecx,MiniVDD_VirtualizeVMBanking ;this is handler routine
	VxDCall VDD_Takeover_VGA_Port		;take it over
;
public	MVDITakeoverW32Ports
MVDITakeoverW32Ports:
	cmp	ChipID,ET4000			;running on an ET4000 (not W32)?
	je	MVDIGoodExit			;yes, we're done
	mov	edx,3cbh			;on a W32, takeover port 3CBH
	mov	ecx,MiniVDD_VirtualizeVMBanking ;this is handler routine
	VxDCall VDD_Takeover_VGA_Port		;take it over
;
;Now, for the W32 extension ports:
;
	mov	esi,OFFSET32 MiniVDD_Virtual217AH
	mov	edx,217ah		;
	VMMCall Install_IO_Handler	;
	jc	MVDIErrorExit		;oops, couldn't install procedure
	VMMCall Disable_Global_Trapping ;don't trap yet, set it later
;
	mov	esi,OFFSET32 MiniVDD_Virtual217AH
	mov	edx,217bh		;
	VMMCall Install_IO_Handler	;
	jc	MVDIErrorExit		;oops, couldn't install procedure
	VMMCall Disable_Global_Trapping ;don't trap yet, set it later
;
MVDIGoodExit:
	clc				;return success
	jmp	MVDIExit		;
;
MVDIErrorExit:
	stc
;
MVDIExit:
	ret
EndProc MiniVDD_Dynamic_Init
;
;
subttl		MiniVDD Init_Complete Entry Point
page +
public	MiniVDD_Init_Complete
BeginProc MiniVDD_Init_Complete
	cmp	ChipID,ET4000		;running on an ET4000 (not W32)?
	jne	MICExit 		;nope, on W32's we don't need it
;
;There is an Orchid card which has a so-called "ergonomic refresh".  This
;is controlled by port 3DDH.  We therefore virtualize this port so we don't
;screw up the screen on a virtualized mode (such as MODE CO80) call:
;
	mov	edx,3ddh		;takeover port 3DDH
	mov	ecx,MiniVDD_VirtualTLExtensions
	VxDCall VDD_Takeover_VGA_Port	;take it over
	mov	edx,3ddh		;
	VMMCall Enable_Global_Trapping	;and enable trapping on it
;
MICExit:
	clc				;always return good
	ret				;
EndProc MiniVDD_Init_Complete
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
	movzx	eax,ChipID		;always return 'V7' to caller
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
	Control_Dispatch Init_Complete,      MiniVDD_Init_Complete
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
;	Client_ECX contains the total VRAM size on the card.
;
	mov	eax,[ebp].Client_ECX	;get total memory size on card
	mov	TotalMemorySize,eax	;save it off.
;
;There is a bug on certain VGA ROM BIOS's with the ET-4000 that results in
;loss of synch when switching from full-screen back into Windows HiRes mode.
;This is caused by a race condition in these ROM BIOS's which results in
;a mis-setting of the Miscallaneous Output Register.  Since we know that
;we're in Windows HiRes mode now, we must save off the correct state of
;this register so that we can restore it properly when needed:
;
	mov	edx,3cch		;this is where we read Misc Output Reg
	in	al,dx			;
	mov	WindowsMiscOutputState,al
;
;The Tseng Labs ET-4000 (not W32) need to utilize two selectors during
;"block move" operations.  Since the MiniVDD depends on writes to the
;VFLATD selector as its method for knowing when to set the hardware in
;and out of Windows HiRes mode, writes to this second selector won't cause
;us to be called.  Therefore, it might be possible during a block move to
;have the hardware state!  We now call a special service provided by
;the Main VDD to "register" this second screen selector and setup the
;mechanism by which writes to this selector will be trapped and handled
;at the appropriate time:
;
	cmp	ChipID,ET4000		;running on an ET4000 (not W32)?
	jne	MRDDAllocW32Selector	;nope, go do other stuff for the W32
	movzx	eax,[ebp].Client_DX	;get passed in "extra" selector
	VxDCall VDD_Register_Extra_Screen_Selector
	mov	[ebp].Client_DX,0	;set return value to zero (not like W32)
	jmp	MRDDExit		;we're done on the ET4000
;
public	MRDDAllocW32Selector
MRDDAllocW32Selector:
;
;We need to allocate and register a screen selector for the W32 memory mapped
;registers.  Let's go ahead and do this:
;
	mov	eax,W32MemMappedSelector;assume we already have a selector
	or	eax,eax 		;already have a selector?
	jnz	@F			;yes, just return the same one
	mov	edx,[ebp].Client_EDX	;get passed in memory mapped physical
					;selector
	VMMCall _MapPhysToLinear,<edx, 32*1024, 0>
;
;Make the descriptor for this linear address so we can use it to allocate the
;LDT selector that we need:
;
	VMMCall _BuildDescriptorDWORDs,<eax, (32*1024)/4096, RW_DATA_TYPE, D_GRAN_PAGE, 0>
;
;Now, create an LDT selector for this linear memory:
;
	VMMCall _Allocate_LDT_Selector,<ebx, edx, eax, 1, 0>
	mov	W32MemMappedSelector,eax;save it for our own use
	push	eax			;save selector over call
	VxDCall VDD_Register_Extra_Screen_Selector
	pop	eax			;restore saved selector
@@:	mov	[ebp].Client_DX,ax	;return newly alloc'd selector to caller
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
;	AH contains the page offset from start of bank of virtualization area.
;	EBX must still contain the Windows VM handle.
;	ECX contains the starting address of the VDD virtualization area
;	EDX contains the amount of memory that we allocated to
;	the VDD virtualization area.  We set EDX == ECX if we are in
;	a "memory shy" configuration.  That is, to tell the "main"
;	VDD that we cannot support VGA 4 plane graphics in the background.
;
;Let's find out in which 64K bank the off-screen memory starts:
;
	mov	eax,ecx 		;get start address of off-screen memory
	shr	eax,16			;divide by 64K per bank -- AL has bank #
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
	xor	cx,cx			;zero the offset in low word of ECX
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
;	AL contains the bank number for the VDD virtualization bank.
;	ECX contains the 32 bit start address of the VDD virtualization area.
;	EDX contains the size of the VDD virtualization area (either 32K or
;	    64K.
;
	push	edx			;save size of VDD area for now
	push	ebx			;we need this as a work register
;
;When the MemC is in 4 plane mode (such as when virtualizing 4 plane
;VGA apps in a window), we must actually divide the bank number that
;we calculated above (currently in AL) by 4.  If our bank number isn't
;easily divisible by 4, we must return the odd number to the main VDD
;so it can adjust accordingly:
;
	mov	ah,al			;copy bank number to AH for odd calc
	shr	al,2			;this is the physical bank that we use
	and	ah,03h			;this is page offset from bank's start
	shl	ah,2			;
	mov	VDDPageOffsetFromBank,ah
					;save this for SetVDDBank
	mov	bl,al			;copy bank number to BL & BH for now
	mov	bh,al			;
;
;First, the low banking bits for the ET-4000 and W32 series register 3CDH:
;
	and	bl,0fh			;get low bits of VDD bank in BL
	mov	al,bl			;copy them into BL for nibble duplicate
	shl	bl,4			;
	or	al,bl			;now AL has value for register 3CDH
	mov	VDDBankLow,al		;save this for use in virtualization
	mov	VDDBankHigh,0		;assume we're on a plain ET4000
	cmp	ChipID,ET4000		;are we running on an ET4000?
	je	@F			;yes, there's no high banking stuff!
;
;Now, the high banking bits used only on the W32 series:
;
	and	bh,030h 		;isolate high 2 bits of VDD bank in BH
	mov	bl,bh			;copy them into BL for nibble duplicate
	shr	bl,4			;duplicate the nibbles
	or	bl,bh			;
	mov	VDDBankHigh,bl		;and save the high VDD banking register
@@:	pop	ebx			;restore saved registers
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
;Banking has been enabled by the call to ACCESS_VGA_MEMORY_MODE (if the
;driver ever runs in flat linear mode).
;
;Entry:
;	EBX contains the MemC owner's VM handle.
;Exit:
;	Save any registers that you use.
;
;VGA.DRV and SUPERVGA.DRV never call MiniVDD_RegisterDisplayDriver.  Therefore,
;if we're running with either of these drivers, TotalMemorySize will be 0.
;
	cmp	TotalMemorySize,0	;are we running with VGA.DRV?
	je	MSVBExit		;yes, don't do this!
	push	eax			;
	push	edx			;
;
;Becuase we have an "extra" (non-VFlatD controlled) screen selector, it's
;possible that we didn't get a call to ResetBank before we were called
;again.  We therefore must check to see if the banking has been changed
;since last time:
;
	mov	edx,3cdh		;EDX --> low banking register
	in	al,dx			;get low banking register ...
	mov	ah,al			;into AH
	xor	al,al			;assume we're running on an ET4000
	cmp	ChipID,ET4000		;running on an ET4000 (not W32)?
	je	@F			;yes, don't get high banking register
	mov	dl,0cbh 		;EDX --> high banking register
	in	al,dx			;get high banking register into AL
@@:	cmp	ax,word ptr VDDBankHigh ;are we still set to VDD bank?
	je	MSVBAdjustPage		;yes, don't re-do this!
;
public	MSVBSetToVDDBank
MSVBSetToVDDBank:
	mov	word ptr BankRegSaveHigh,ax
					;save hardware's current bank state
	mov	dl,0cdh 		;EDX --> low banking register
	mov	al,VDDBankLow		;set to our reserved bank
	out	dx,al			;
;
	cmp	ChipID,ET4000		;running on a vanilla ET4000 (not W32)?
	je	MSVBAdjustPage		;yes, no high banking on this chipset!
	mov	dl,0cbh 		;EDX --> high banking register
	mov	al,VDDBankHigh		;set to our reserved bank
	out	dx,al			;
;
public	MSVBAdjustPage
MSVBAdjustPage:
;
;Our VDD virtualization area sometimes starts in the middle of a bank.	We
;therefore must offset the page passed to us in EAX (actually in AL) by the
;number of pages that the VDD virtualization area is offset from the start
;of the bank.  This value was calculated at GetVDDBank.  We therefore
;modify the passed in value in AL so that the VGA app starts writing in
;the proper place (below the screen instead of on top of it!!!).
;
	pop	edx			;restore saved registers
	pop	eax			;
	and	al,0fh			;get page offset within bank
	add	al,VDDPageOffsetFromBank;correct for any offset that we have
;
MSVBExit:
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
	push	eax			;
	push	edx			;
;
;VGA.DRV and SUPERVGA.DRV never call MiniVDD_RegisterDisplayDriver.  Therefore,
;if we're running with either of these drivers, TotalMemorySize will be 0.
;
	cmp	TotalMemorySize,0	;are we running with VGA.DRV?
	je	MRBExit 		;yes, don't do this!
;
;Get the values of both banking registers so we can see if they're still
;set to "our" bank.  If they aren't, then Windows has already switched
;them to something for its own purposes and we'd best not restore them!
;
	mov	edx,3cdh		;EDX --> low banking register
	in	al,dx			;
	mov	ah,al			;get current value into AH
	xor	al,al			;assume we're on an ET4000 (not W32)
	cmp	ChipID,ET4000		;running on the ET4000 (not W32)?
	je	@F			;yes, there's no high banking register!
	mov	dl,0cbh 		;EDX --> high banking register
	in	al,dx			;get current value into AL
@@:	cmp	ax,word ptr VDDBankHigh ;are registers still set to VDD bank?
	jne	MRBExit 		;nope, don't physically reset bank regs
;
;It's safe to restore the banking registers!
;
	mov	dl,0cdh 		;EDX --> low banking register
	mov	al,BankRegSaveLow	;get bank to restore
	out	dx,al			;
	cmp	ChipID,ET4000		;running on the ET4000 (not W32)?
	jbe	MRBExit 		;yes! no register high banking on ET4000
	mov	dl,0cbh 		;EDX --> high banking register
	mov	al,BankRegSaveHigh	;get bank to restore
	out	dx,al			;
;
MRBExit:
	pop	edx			;restore saved registers
	pop	eax			;
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
	push	eax			;save registers that we use
	push	edx			;
;
;VGA.DRV and SUPERVGA.DRV never call MiniVDD_RegisterDisplayDriver.  Therefore,
;if we're running with either of these drivers, TotalMemorySize will be 0.
;
	cmp	TotalMemorySize,0	;are we running with VGA.DRV?
	je	MSLBExit		;yes, don't do this!
;
;And set to our reserved latch bank:
;
	mov	edx,3cdh		;EDX --> low banking register
	in	al,dx			;get current value of banking register
	mov	LatchSaveLow,al 	;
	mov	al,VDDBankLow		;
	out	dx,al			;
;
	cmp	ChipID,ET4000		;running on an ET4000 (not W32)?
	jbe	MSLBExit		;yep, don't fool with high banking reg
	mov	dl,0cbh 		;EDX --> high banking register
	in	al,dx			;
	mov	LatchSaveHigh,al	;
	mov	al,VDDBankHigh		;
	out	dx,al			;
;
MSLBExit:
	pop	edx			;restore saved registers
	pop	eax			;
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
	push	eax			;save registers that we use
	push	edx			;
;
;VGA.DRV and SUPERVGA.DRV never call MiniVDD_RegisterDisplayDriver.  Therefore,
;if we're running with either of these drivers, TotalMemorySize will be 0.
;
	cmp	TotalMemorySize,0	;are we running with VGA.DRV?
	je	MRLBExit		;yes, don't do this!
;
;Reset the low banking register:
;
	mov	edx,3cdh		;EDX --> low banking register
	mov	al,LatchSaveLow 	;
	out	dx,al			;
;
	cmp	ChipID,ET4000		;running on an ET4000 (not W32)?
	jbe	MRLBExit		;yep, no high banking register!
	mov	dl,0cbh 		;EDX --> high banking register
	mov	al,LatchSaveHigh	;
	out	dx,al			;
;
MRLBExit:
	pop	edx			;restore saved registers
	pop	eax			;
	ret				;
EndProc MiniVDD_ResetLatchBank
;
;
subttl		Prepare to Enter a Standard VGA Mode from HiRes Mode
page +
public	MiniVDD_PreHiResToVGA
BeginProc MiniVDD_PreHiResToVGA, DOSVM
;
;When the VDD is about to switch from Windows HiRes mode to any of the
;standard VGA modes (for real -- not virtualized), this routine will be
;called.  You need to determine exactly what your hardware requires in
;order to accomplish this task.  For example, you should disable trapping
;on registers that the mini-VDD is specifically handling (such as 4AE8H
;in the case of the S3 chipset), make sure that the hardware is "ready"
;to undergo a mode transition (make sure that your hardware's graphics
;engine isn't in the middle of an operation that can't be interrupted)
;etc.  If your hardware does not return to a standard VGA mode via
;a call to INT 10H, function 0, you should also make sure to do whatever
;it takes to restore your hardware to a standard VGA mode at this time.
;Try not to touch video memory during your transition to standard VGA as
;this will disturb the reliability of the system.
;
	or	TLModeChangeFlags,GOING_TO_VGA_MODE
	cmp	ChipID,ET4000		;on an ET-4000?
	jne	MHTVExit		;on a W32, skip the following
;
;Disable trapping on port 3DDH which is used on some Orchid cards to
;set the "ergonomic refresh rate":
;
	mov	edx,3ddh		;
	VMMCall Disable_Global_Trapping ;go disable trapping on port in EDX
;
MHTVExit:
	ret
EndProc MiniVDD_PreHiResToVGA
;
;
public	MiniVDD_PostHiResToVGA
BeginProc MiniVDD_PostHiResToVGA, DOSVM
;
;This routine is called after the ROM BIOS call is made to place the hardware
;back in the standard VGA state.  You should reenable trapping and do any
;other post-processing that your hardware might need:
;
	mov	TLModeChangeFlags,0	;flag that we're done with mode change
;
MPHVExit:
	ret				;
EndProc MiniVDD_PostHiResToVGA
;
;
subttl		Prepare to Enter HiRes Mode From a Standard VGA Mode
page +
public	MiniVDD_PreVGAToHiRes
BeginProc MiniVDD_PreVGAToHiRes, DOSVM
;
;We are notified that the VDD is about to call the display driver to
;switch into HiRes mode from a full screen VGA DOS box.  We can do
;anything necessary to prepare for this.  In the case of the S3 VDD,
;we simply set a flag telling us that we're about to change modes.
;
;Entry:
;	EBX contains the Windows VM handle.
;Exit:
;	Nothing assumed.
;
	or	TLModeChangeFlags,GOING_TO_WINDOWS_MODE
;
MSVHExit:
	ret				;
EndProc MiniVDD_PreVGAToHiRes
;
;
public	MiniVDD_PostVGAToHiRes
BeginProc MiniVDD_PostVGAToHiRes, DOSVM
;
;We are notified that the VDD is done setting the hardware state back
;to HiRes mode.  We simply unset our TLModeChangeFlags in this case.
;
;Entry:
;	EBX contains the Windows VM handle.
;Exit:
;	Nothing assumed.
;
	mov	TLModeChangeFlags,0	;
	cmp	ChipID,ET4000		;on an ET-4000?
	jne	MPVHExit		;on a W32, skip the following
;
;Reenable trapping on port 3DDH which is used on some Orchid cards to
;control "ergonomic refresh rates":
;
	mov	edx,3ddh		;
	VMMCall Enable_Global_Trapping	;go re-enable trapping on port in EDX
;
MPVHExit:
	ret				;
EndProc MiniVDD_PostVGAToHiRes
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
;If the CRTC owner is Windows, we need to make sure that the Miscallaneous
;Output Register state is correct.  This is due to a race condition bug on
;some BIOS's.  We saved the correct state for Windows HiResMode at
;MiniVDD_RegisterDisplayDriver and we need to make sure it's restored here:
;
	push	edx			;save caller's EDX
;
;When Windows owns the CRTC, we must restore the state of the MiscOutputReg
;to the value that we saved at SaveDriverState:
;
	cmp	ecx,WindowsVMHandle	;Windows owns the CRTC state?
	jne	@F			;nope, skip this
	mov	al,WindowsMiscOutputState
	or	al,al			;have we init'd this state yet?
	jz	@F			;nope! don't set a bogus value!
	mov	edx,3c2h		;write the MiscOutput Register
	out	dx,al			;restore the correct state
@@:	cmp	esi,MessageModeID	;are we to restore message mode?
	je	MRRSExit		;yes! Cirrus doesn't do anything here
	cmp	esi,PlanarStateID	;are we to restore planar state?
	je	MRRSRestorePlanarState	;yes, go explicitely restore it
	test	TLModeChangeFlags,GOING_TO_WINDOWS_MODE OR GOING_TO_VGA_MODE
	jnz	MRRSExit		;in a mode change, don't restore now
;
public	MRRSRestoreVMState
MRRSRestoreVMState:
;
public	MRRSRestorePlanarState
MRRSRestorePlanarState:
;
MRRSExit:
	pop	edx			;
	ret				;
EndProc MiniVDD_RestoreRegisters
;
;
subttl		Enable and Disable Register Trapping
page +
public	MiniVDD_EnableTraps
BeginProc MiniVDD_EnableTraps, DOSVM
;
;As stated elsewhere, the VDD needs to set traps on the BLTer registers used
;by the display driver whenever it is using the off-screen memory to
;virtualize VGA graphics mode in the background or in a window.  The reason
;the VDD does this is to receive notification on the first access to a
;BLTer register AFTER returning to the Windows VM.  Then, the VDD switches
;the state of the hardware back to that appropriate for running Windows
;HiRes mode.  In this routine, all you need to do is call the VMM service
;Enable_Global_Trapping for each of the ports that you registered at
;MiniVDD_Device_Init.  Then, the VDD will receive notification at the
;proper time and will switch states properly.
;
;Entry:
;	Nothing assumed.
;Exit:
;	Just Preserve EBX, and ESI.
;
	mov	edx,217ah		;extended register index
	VMMCall Enable_Global_Trapping
;
	mov	edx,217bh		;extended register data
	VMMCall Enable_Global_Trapping
;
MERTExit:
	ret				;
EndProc MiniVDD_EnableTraps
;
;
public	MiniVDD_DisableTraps
BeginProc MiniVDD_DisableTraps, DOSVM
;
;See comment at EnableRegTraps.
;
;Entry:
;	Nothing assumed.
;Exit:
;	Just Preserve EBX and ESI.
;
	mov	edx,217ah		;ATI VGA extended register index
	VMMCall Disable_Global_Trapping
;
	mov	edx,217bh		;ATI VGA extended register data
	VMMCall Disable_Global_Trapping
;
MDRTExit:
	ret				;
EndProc MiniVDD_DisableTraps
;
;
subttl		Return Current READ Bank To Caller
page +
public	MiniVDD_GetCurrentBankRead
BeginProc MiniVDD_GetCurrentBankRead, DOSVM
;
;This routine is called during the save/restore process for VESA HiRes modes.
;The Tseng Labs VESA TSR has a bad bug in it whereby it's supposed to return
;the result from a VESA function 4F05H in the DX register but it POPs DX
;right before returning!!!!  So, we have to implement this in the MiniVDD
;instead.
;
;Entry:
;	EBX contains the VM handle.
;Exit:
;	EDX contains the read bank number.
;	CY should be set indicating that we handled it.
;
	push	eax			;save EAX over this
	mov	edx,3cdh		;EDX --> low banking port
	in	al,dx			;get the value in the low banking port
	mov	ah,al			;copy it to AH
	xor	al,al			;assume no high banking
	cmp	ChipID,ET4000		;running on an ET4000 (not W32)?
	je	@F			;yes, don't do the high banking
	mov	dl,0cbh 		;EDX --> high banking port
	in	al,dx			;AL has the high banking port value
@@:	and	ax,0f030h		;mask to get only the read bank
	shr	ah,4			;get low read bank into low nibble of AH
	or	al,ah			;now AL has the read bank
	movzx	edx,al			;return the answer in EDX
	pop	eax			;restore saved EAX
	stc				;indicate that we handled it
	ret				;
EndProc MiniVDD_GetCurrentBankRead
;
;
subttl		Return Current WRITE Bank To Caller
page +
public	MiniVDD_GetCurrentBankWrite
BeginProc MiniVDD_GetCurrentBankWrite, DOSVM
;
;This routine is called during the save/restore process for VESA HiRes modes.
;The Tseng Labs VESA TSR has a bad bug in it whereby it's supposed to return
;the result from a VESA function 4F05H in the DX register but it POPs DX
;right before returning!!!!  So, we have to implement this in the MiniVDD
;instead.
;
;Entry:
;	EBX contains the VM handle.
;Exit:
;	EDX contains the read bank number.
;	CY should be set indicating that we handled it.
;
	push	eax			;save EAX over this
	mov	edx,3cdh		;EDX --> low banking port
	in	al,dx			;get the value in the low banking port
	mov	ah,al			;copy it to AH
	xor	al,al			;assume no high banking
	cmp	ChipID,ET4000		;running on an ET4000 (not W32)?
	je	@F			;yes, don't do the high banking
	mov	dl,0cbh 		;EDX --> high banking port
	in	al,dx			;AL has the high banking port value
@@:	and	ax,0f03h		;mask to get only the write bank
	shl	al,4			;get high read bank into high nibble
	or	al,ah			;now AL has the read bank
	movzx	edx,al			;return the answer in EDX
	pop	eax			;restore saved EAX
	stc				;indicate that we handled it
	ret				;
EndProc MiniVDD_GetCurrentBankWrite
;
;
subttl		Set Bank To Tseng Hardware
page +
public	MiniVDD_SetBank
BeginProc MiniVDD_SetBank, DOSVM
;
;This routine is called during the save/restore process for VESA HiRes modes.
;The Tseng Labs VESA TSR has a bad bug in it whereby it's supposed to return
;the result from a VESA function 4F05H in the DX register but it POPs DX
;right before returning!!!!  So, we have to implement this in the MiniVDD
;instead.
;
;Entry:
;	EBX contains the VM handle.
;	EDX contains the saved state of the WRITE banking register.
;	EAX contains the saved state of the READ banking register.
;Exit:
;	CY should be set indicating that we handled it.
;
	push	eax			;save EAX over this
	push	edx			;
;
;Our banking values have a maximum size of a byte, let's first combine the
;READ & WRITE banks into byte lengthed values
;
	mov	dh,al			;put READ banking value into DH for now
	mov	eax,edx 		;get WRITE bank into AL
					;get READ bank into AH
	and	ax,0f0fh		;AL has LOW write bits
					;AH has LOW read bits
	shl	ah,4			;get LOW read bits into position
	or	al,ah			;now AL has value for 3CDH
;
	and	dx,0f0f0h		;DL has HIGH write bits
					;DH has HIGH read bits
	shr	dl,4			;get HIGH write bits into position
	or	dl,dh			;now DL has value for 3CBH
	mov	ah,dl			;now AH has value for 3CBH
;
	mov	edx,3cdh		;EDX --> low banking register
	out	dx,al			;
	cmp	ChipID,ET4000		;running on an ET4000 (not W32)?
	je	@F			;yes, don't do the high banking
	mov	dl,0cbh 		;EDX --> high banking register
	mov	al,ah			;
	out	dx,al			;
;
@@:	pop	edx			;restore saved registers
	pop	eax			;
	stc				;indicate that we handled it
	ret				;
EndProc MiniVDD_SetBank
;
;
subttl		Virtualize Tseng Banking Registers
page +
public	MiniVDD_VirtualizeVMBanking
BeginProc MiniVDD_VirtualizeVMBanking
;
;This routine allows us to virtualize VM writes to the banking ports
;3CBH (W32 only) and 3CDH which may be written to by the non-Windows
;VM application (such as the VGA ROM BIOS).  We want to stop these
;writes because we need to control the banking ourselves when running
;Windowed or background mode.
;
;Entry:
;	AL contains the data to output if it's a write.
;	EBX contains the VM handle writing to the port.
;	ECX contains the virtualization flags.
;Exit:
;	AL contains the virtual data if it's a read.
;
	test	TLModeChangeFlags,GOING_TO_WINDOWS_MODE OR GOING_TO_VGA_MODE
	jnz	MVVBPhysicalIO		;in a CRTC change, allow physical IO
	VxDCall VDD_Get_VM_Info 	;get MemC owner in ESI, CRTC in EDI
	cmp	ebx,edi 		;does VM own the CRTC?
	jne	MVVBVirtualIO		;nope, better virtualize the IO
	cmp	ebx,WindowsVMHandle	;TSENG.DRV doing banking (not VFlatD)?
	jne	MVVBPhysicalIO		;nope, go do physical IO
;
public	MVVBDisplayDriverIsBanking
MVVBDisplayDriverIsBanking:
;
;The display driver is trying to set or get the bank while we've got it
;set down in the VDD virtualization area.  If the driver is trying to
;get the bank, we don't want to return the VDD virtualization area.  Rather,
;we want to return the bank that we saved (VFlatD's bank).  This is currently
;contained in the BankRegSave variable(s).  If the display driver is trying
;to set the bank by itself, we want to put the thing that it's trying to
;set into BankRegSave since this is what we want restored to the banking
;register when the Windows VM regains control of the MemC state and begins
;writing to the display memory.
;
;First, get to the proper variable (BankRegSaveHigh or BankRegSaveLow):
;
	and	edx,0ffffh		;make sure top word of EDX is 0
	sub	edx,3cbh		;make 3CB -> 0, 3CD -> 2
	shr	edx,1			;make 3CB -> 0, 3CD -> 1
	add	edx,OFFSET32 BankRegSaveHigh
;
;Now, dispatch to the proper action:
;
	cmp	ecx,Byte_Input		;TSENG.DRV trying to read the bank?
	je	MVVBDisplayDriverBankRead
;
public	MVVBDisplayDriverBankWrite
MVVBDisplayDriverBankWrite:
;
;Set the value in AL into the proper variable of BankRegSaveHigh or
;BankRegSaveLow:
;
	mov	[edx],al		;we'll restore this value properly!
	jmp	MVVBExit		;and we're done!
;
public	MVVBDisplayDriverBankRead
MVVBDisplayDriverBankRead:
;
;Get the proper value of BankRegSaveHigh or BankRegSaveLow to return to
;the display driver in AL:
;
	mov	al,[edx]		;now AL has VFlatD's selector value
	jmp	MVVBExit		;and we're done!
;
public	MVVBVirtualIO
MVVBVirtualIO:
;
;First, make the IO port offset from 0:
;
	cmp	ecx,Byte_Output 	;writing virtual state?
	je	MVVBSaveVirtualOut	;yes, go do it
;
public	MVVBVirtualInput
MVVBVirtualInput:
	and	edx,0ffffh		;make sure top word of EDX is 0
	sub	edx,3cbh		;now EDX is offset into CB data struct
	add	edx,OurCBDataPointer	;EDX --> our saved register state
	mov	al,[edx+ebx]		;get the saved virtual value!
	jmp	MVVBExit		;and we're done!
;
public	MVVBPhysicalIO
MVVBPhysicalIO:
	cmp	ecx,Byte_Input		;doing input from register?
	je	MVVBPhysicalInput	;yes, go do it
;
public	MVVBPhysicalOutput
MVVBPhysicalOutput:
	out	dx,al			;do the physical OUT
;
MVVBSaveVirtualOut:
	and	edx,0ffffh		;make sure top word of EDX is 0
	sub	edx,3cbh		;now EDX is offset into CB data struct
	add	edx,OurCBDataPointer	;
	mov	[edx+ebx],al		;save the virtual value!
	jmp	MVVBExit		;and we're done!
;
public	MVVBPhysicalInput
MVVBPhysicalInput:
	in	al,dx			;do the IN and return data in AL
;
MVVBExit:
	ret				;
EndProc MiniVDD_VirtualizeVMBanking
;
;
subttl		Virtualize the Orchid Extended Register 3DDH
page +
public	MiniVDD_VirtualTLExtensions
BeginProc MiniVDD_VirtualTLExtensions, DOSVM
;
;This routine is only used on ET-4000's!
;
;The Orchid TL-4000 card has an extended register at port 3DDH which
;controls timings for its "ergonomic refresh rate".
;Therefore, when we're running in a window, and don't really want this
;extension register to be messed with, we must virtualize it so that
;writes to it are thrown out.
;
;Entry:
;	AX (or AL) contains the value to be read/written on port.
;	EBX contains the handle of the VM that is accessing the register.
;	ECX contains flags telling us what kind of I/O to do (see VMM.INC).
;	DX contains the destination port (3DDH).
;
	push	edi			;save these just in case
	push	esi			;
	VxDCall VDD_Get_VM_Info 	;get CRTC owner VM handle in EDI
	cmp	edi,WindowsVMHandle	;is CRTC controlled by Windows?
	jne	MVAEPerformPhysicalIO	;nope, allow the physical I/O
	cmp	ebx,edi 		;is calling VM handle also in Windows?
	jne	MVAEExit		;nope, toss it
;
public	MVAEPerformPhysicalIO
MVAEPerformPhysicalIO:
	cmp	ecx,Byte_Output 	;does he want to output a byte?
	je	MVAEPhysicalByteOutput	;yes, go do it
;
public	MVAEPhysicalByteInput
MVAEPhysicalByteInput:
	in	al,dx			;just do physical input
	jmp	MVAEExit		;and we're done!
;
public	MVAEPhysicalByteOutput
MVAEPhysicalByteOutput:
	out	dx,al			;just write the byte to it
	jmp	MVAEExit		;and we're done
;
MVAEExit:
	pop	esi			;restore saved registers
	pop	edi			;
	ret				;
EndProc MiniVDD_VirtualTLExtensions
;
;
subttl		Virtualize Writes to CRTC Register 36H (W32 Only)
page +
public	MiniVDD_VirtualizeCRTCOut
BeginProc MiniVDD_VirtualizeCRTCOut, DOSVM
;
;This routine is only used on W32's!
;
;Entry:
;	AL contains the value to send to the port.
;	EBX contains the VM handle which is doing the OUT.
;	ECX contains the index of the CRTC port to act on.
;	EDX contains the CRTC port (3D5H)
;Exit:
;	CY returned if we completely handled I/O, NC returned if
;	we want caller to handle it (Main VDD will decide whether to
;       do I/O or not).
;	Destroys ESI & EDI (caller doesn't care about these).
;
	cmp	ecx,31h 		;is it a port we're interested in?
	jb	MVCOWeDidntHandle	;nope, just let caller worry about it
	test	TLModeChangeFlags,GOING_TO_WINDOWS_MODE OR GOING_TO_VGA_MODE
	jnz	MVCOWeDidntHandle	;in a CRTC change, do physical I/O
	VxDCall VDD_Get_VM_Info 	;get CRTC owner VM handle in EDI
	cmp	ebx,edi 		;does caller own the CRTC?
	je	MVCOWeDidntHandle	;yes, let caller do physical I/O
;
public	MVCOWeHandledIt
MVCOWeHandledIt:
	stc				;ignore it I/O by returning CY
	jmp	MVCOExit		;we're done
;
MVCOWeDidntHandle:
	clc				;let caller handle it
;
MVCOExit:
	ret				;
EndProc MiniVDD_VirtualizeCRTCOut
;
;
subttl		Virtualize W32 Extension Registers 217AH & 217BH
page +
public	MiniVDD_Virtual217AH
BeginProc MiniVDD_Virtual217AH, DOSVM
;
;This routine is only used on W32's!
;
;This port is used by the Tseng W32 for such things as controlling the
;hardware cursor etc.  We want to prevent all writes to this thing when
;virtualizing in a window.
;
;Entry:
;	AX (or AL) contains the value to be read/written on port.
;	EBX contains the handle of the VM that is accessing the register.
;	ECX contains flags telling us what kind of I/O to do (see VMM.INC).
;	DX contains the destination port (either 217AH or 217BH).
;
	push	edi			;save these just in case
	push	esi			;
	test	TLModeChangeFlags,GOING_TO_WINDOWS_MODE OR GOING_TO_VGA_MODE
	jnz	MVPerformPhysicalIO	;in a CRTC change, allow physical IO
	VxDCall VDD_Get_VM_Info 	;get CRTC owner VM handle in EDI
	cmp	edi,WindowsVMHandle	;is CRTC controlled by Windows?
	jne	MVPerformPhysicalIO	;nope, allow the physical I/O
	cmp	ebx,edi 		;is calling VM handle also in Windows?
	jne	MVVirtualize		;nope, we need to virtualize it
;
public	MVPerformPhysicalIO
MVPerformPhysicalIO:
	cmp	ecx,Byte_Output 	;does he want to output a byte?
	je	MVPhysicalByteOutput	;yes, go do it
	cmp	ecx,Word_Output 	;does he want to output a word?
	je	MVPhysicalWordOutput	;yes, go do it
;
public	MVPhysicalByteInput
MVPhysicalByteInput:
	in	al,dx			;just do physical input
	jmp	MVExit			;and we're done!
;
public	MVPhysicalWordOutput
MVPhysicalWordOutput:
	out	dx,ax			;just write to it
	jmp	MVExit			;and we're done
;
public	MVPhysicalByteOutput
MVPhysicalByteOutput:
	out	dx,al			;just write the byte to it
	jmp	MVExit			;and we're done
;
public	MVVirtualize
MVVirtualize:
	cmp	ecx,Byte_Output 	;does he want to output a byte?
	je	MVVirtualByteOutput	;yes, go do it
	cmp	ecx,Word_Output 	;does he want to output a word?
	je	MVVirtualWordOutput	;yes, go do it
;
public	MVVirtualByteInput
MVVirtualByteInput:
	jmp	MVPhysicalByteInput	;
;
public	MVVirtualWordOutput
MVVirtualByteOutput:
;
public	MVVirtualWordOutput
MVVirtualWordOutput:
;
MVExit:
	pop	esi			;restore saved registers
	pop	edi			;
	ret				;
EndProc MiniVDD_Virtual217AH
;
;
VxD_LOCKED_CODE_ENDS
;
;
end

