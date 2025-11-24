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

page            ,132
title           Cirrus Logic 542x Mini-VDD Support Functions
.386p
;
;
.xlist
include         VMM.INC
include         MINIVDD.INC
include         CIRRUS.INC
.list
;
;
subttl          Virtual Device Declaration
page +

Declare_Virtual_Device  CIRRUS,                                 \
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
;
subttl          Initialization Data
page +
VxD_IDATA_SEG

VxD_IDATA_ENDS
;
;
subttl          Locked Data Area
page +
VxD_LOCKED_DATA_SEG

VxD_LOCKED_DATA_ENDS
;
;
subttl          General Data Area
page +
VxD_DATA_SEG
;
public  WindowsVMHandle
WindowsVMHandle         dd      ?               ;init'd at Device_Init
;
public  PlanarStateID, MessageModeID
PlanarStateID           dd      ?               ;init'd at RegisterPlanarState
MessageModeID           dd      ?               ;init'd at RegisterMessageMode
;
public  TotalMemorySize
TotalMemorySize         dd      ?               ;init'd at Sys_Critical_Init
;
public  Vid_PhysA0000
Vid_PhysA0000           dd      0               ;
;
public  OurCBDataPointer
OurCBDataPointer        dd      ?               ;init'd at Device_Init
;
public  MemoryMappedSelector
MemoryMappedSelector    dd      0               ;init'd at RegisterDisplayDriver
;
public  ChipID                                  ;Cirrus specific variable!
ChipID                  dd      ?               ;init'd at Dynamic_Init
;
public  CLModeChangeFlags
CLModeChangeFlags       db      ?               ;=FFH when in BIOS mode change
;
public  DisplayEnabledFlag
DisplayEnabledFlag      db      0               ;=FFH when display in HiRes mode
;
public  WindowsMiscOutputState
WindowsMiscOutputState  db      0               ;init'd at DisplayDriverRegister
;
public  VDDBank, VDDBankControl, BankRegSave, LatchSaveOFFSET0
VDDBank                 db      ?               ;init'd at GetVDDBank
VDDBankControl          db      ?               ;init'd at GetVDDBank
BankRegSave             db      0ffh            ;this flag must be init'd!
LatchSaveOFFSET0        db      ?               ;
;
public  DisplayInfoStructure
DisplayInfoStructure    DISPLAYINFO     <>
;
VxD_DATA_ENDS
;
;
subttl          Device Initialization
page +
VxD_ICODE_SEG
;
;
public  MiniVDD_Dynamic_Init
BeginProc MiniVDD_Dynamic_Init
;
;Entry:
;       EBX contains the VM handle of the Windows VM.
;Exit:
;       If success, return NC.
;       If failure, return CY.
;
        push    ebx                     ;save Windows VM handle in EBX
        mov     WindowsVMHandle,ebx     ;save the Windows VM handle
        mov     CLModeChangeFlags,0     ;initialize this
;
;Get the linear address of physical A0000H addressable for 64K:
;
        VMMCall _MapPhysToLinear,<0A0000h,10000h,0>
        cmp     eax,-1                  ;was there an error?
        je      MVDIErrorExit           ;yes, better not load!
        mov     Vid_PhysA0000,eax       ;save this off!
;
;First, get identity of the card:
;
        call    is_it_CL64xx            ;go try to detect a CL-64XX series
        or      ebx,ebx                 ;was it a Cirrus CL-64XX series?
        jnz     MVDIGetMemorySize       ;yes, go get the memory size
;
;We don't have a CL-64xx, try for a Cirrus CL-542x chipset series:
;
        call    is_it_CL542x            ;go try to detect a CL-542x series
        or      ebx,ebx                 ;was is a Cirrus CL-542X series?
        jz      MVDIErrorExit           ;nope, better not let MiniVDD load!
;
public  MVDIGetMemorySize
MVDIGetMemorySize:
;
;At this point:
;       EBX is bitwise ChipID.
;
        mov     ChipID,ebx              ; save the value for later testing

        mov     ax,1200h                ;VGA ROM BIOS func: Get Video Data
        mov     bl,85h                  ;subfunction: get VRAM size
        push    dword ptr 10h           ;ROM BIOS: Video Interrupt
        VMMCall Exec_VxD_Int            ;returns AL=amount of vram in 64k units
        movzx   eax,al                  ;AX=amount of vram in 64k units
        shl     eax,10+6                ;1024 * 64 is 64k
        mov     TotalMemorySize,eax     ;save it for later use
;
public  MVDIGetSpecialVMs
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
        VxDCall VDD_Get_Special_VM_IDs  ;go get special VM information
        mov     PlanarStateID,esi       ;save off returned identifiers
        mov     MessageModeID,edi       ;
;
public  MVDIDispatch
MVDIDispatch:
;
;Based on the ChipID, dispatch to the correct code to perform Device_Init
;functionality:
;
        test    ChipID,CL_6420+CL_6440  ;are we a CL-64xx family chip?
        jnz     MVDIInit64xx            ;yes, go handle it!
;
public  MVDI_Init5422
MVDI_Init5422:
;
;We want to allocate space in the per-VM CB data structure.  This will
;be used by the mini-VDD to store per-VM data relating to states of registers.
;
        VMMCall _Allocate_Device_CB_Area,<<size PerVMData_54xx>,0>
        mov     OurCBDataPointer,eax    ;save offset our our VxD's area
        or      eax,eax                 ;was call sucessful?
        jz      MVDIErrorExit           ;nope, leave fatally!
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
;               dd      Address of support routine in this mini-VDD.
;                       This address will be initialized to 0 by the "master"
;                       VDD.  If we need to implement this functionality,
;                       we fill in the address of our routine.
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
        cmp     ecx,NBR_MINI_VDD_FUNCTIONS      ;perform a cursory version check
        jb      MVDIErrorExit                   ;oops, versions don't match!
;
public  MVDIFillInTable5422
MVDIFillInTable5422:
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
        MiniVDDDispatch SET_VDD_BANK,SetVDDBank542x
        MiniVDDDispatch RESET_BANK,ResetBank542x
        MiniVDDDispatch SET_LATCH_BANK,SetLatchBank542x
        MiniVDDDispatch RESET_LATCH_BANK,ResetLatchBank542x
        MiniVDDDispatch PRE_HIRES_TO_VGA,PreHiResToVGA
        MiniVDDDispatch POST_HIRES_TO_VGA,PostHiResToVGA
        MiniVDDDispatch PRE_VGA_TO_HIRES,PreVGAToHiRes
        MiniVDDDispatch POST_VGA_TO_HIRES,PostVGAToHiRes
        MiniVDDDispatch SAVE_REGISTERS,SaveRegisters542x
        MiniVDDDispatch RESTORE_REGISTERS,RestoreRegisters542x
        MiniVDDDispatch DISPLAY_DRIVER_DISABLING,DisplayDriverDisabling
        MiniVDDDispatch GET_TOTAL_VRAM_SIZE,GetTotalVRAMSize_5
        MiniVDDDispatch GET_CURRENT_BANK_WRITE,GetCurrentBankWrite_5
        MiniVDDDispatch GET_CURRENT_BANK_READ,GetCurrentBankRead_5
        MiniVDDDispatch GET_BANK_SIZE,GetBankSize_5
        MiniVDDDispatch SET_BANK,SetBank_5
        MiniVDDDispatch PRE_HIRES_SAVE_RESTORE,PreHiResSaveRestore_5
        MiniVDDDispatch POST_HIRES_SAVE_RESTORE,PostHiResSaveRestore_5
        MiniVDDDispatch GET_CHIP_ID,GetChipID
        MiniVDDDispatch VIRTUALIZE_SEQUENCER_OUT,VirtSeqOut54xx
        MiniVDDDispatch VIRTUALIZE_SEQUENCER_IN,VirtSeqIn54xx
        MiniVDDDispatch PRE_CRTC_MODE_CHANGE,PreCRTCModeChange_5
;
;If we're running on a BLTTER chip, we need to perform
;a few more tasks to compensate for the BLTer architecture:
;
        test    ChipID,IS_BLTTER                ;only register this for BLTTER's
        jz      MVDINotBltter                   ;that's it for the CL-5422
        MiniVDDDispatch MAKE_HARDWARE_NOT_BUSY,MakeHardwareNotBusy5426
MVDINotBltter:
;
; if we are on the NORDIC family, then virtualize the extended RAMDAC 
; locations (10h at 3 bytes each).
;
        test    ChipID,CL_7541+CL_7542+CL_7543  ; is it the NORDIC family?
        jz      MVDINotNordic                   ; no, continue with BLTTER setup
        MiniVDDDispatch VIRTUALIZE_CRTC_OUT,VirtCRTCOut754x
        MiniVDDDispatch VIRTUALIZE_DAC_OUT,VirtDACOut54xx
        MiniVDDDispatch VIRTUALIZE_DAC_IN,VirtDACIn54xx
MVDINotNordic:
;
; If we are on a 6235 Laptop controller, then we need to virtualize the
; CRTC registers so the LCD shadow registers are taken care of.
;
        test    ChipID,CL_6235+CL_62x5
        jz      MVDINot6235      
        MiniVDDDispatch VIRTUALIZE_CRTC_OUT,VirtCRTCOut54xx
        MiniVDDDispatch VIRTUALIZE_CRTC_IN,VirtCRTCIn54xx
MVDINot6235:
;
public  MVDI_SetupPortTrapping
MVDI_SetupPortTrapping:
;
;Now comes the hard part (conceptually).  We must call the "master" VDD to
;setup port trapping for any port that the Windows display driver would
;write to (or read from) in order to draw onto the display when it's
;running in Windows Hi-Res mode.  For example, on the IBM 8514/A display
;card, the hardware BLTer is used to draw onto the screen.  Ports such as
;9AE8H, E2E8H, BEE8H are used on the 8514/A to perform the drawing.  The
;VDD "system" must set I/O port traps on these registers so that we are
;informed that the Windows display driver is writing or reading these
;ports. Only set traps on those ports which the display driver would
;write or read in the process of drawing.
;
;The CL-5426 series BLTer engine uses extensions to the Graphics Controller
;Registers which are found on all standard VGA's.  When we register port
;3CEH (the GCR index port), the main VDD will execute special code so that
;the main VDD will be notified correctly when the Windows VM resumes
;execution when a DOS VM has the execution focus:
;
        mov     edx,3ceh                ;register port 3CEH ...
        mov     cl,WORD_LENGTHED        ;as a word lengthed port
        VxDCall VDD_Register_Virtual_Port
        jmp     MVDIGoodExit            ;that's it for the CL-5426
;
public  MVDIInit64xx
MVDIInit64xx:
;
;We want to allocate space in the per-VM CB data structure.  This will
;be used by the mini-VDD to store per-VM data relating to states of registers.
;
        VMMCall _Allocate_Device_CB_Area,<<size PerVMData_64xx>,0>
        mov     OurCBDataPointer,eax    ;save offset our our VxD's area
        or      eax,eax                 ;was call sucessful?
        jz      MVDIErrorExit           ;nope, leave fatally!
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
;               dd      Address of support routine in this mini-VDD.
;                       This address will be initialized to 0 by the "master"
;                       VDD.  If we need to implement this functionality,
;                       we fill in the address of our routine.
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
        cmp     ecx,NBR_MINI_VDD_FUNCTIONS      ;perform a cursory version check
        jb      MVDIErrorExit                   ;oops, versions don't match!
;
public  MVDIFillInTable64xx
MVDIFillInTable64xx:
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
        MiniVDDDispatch SET_VDD_BANK,SetVDDBank64xx
        MiniVDDDispatch RESET_BANK,ResetBank64xx
        MiniVDDDispatch SET_LATCH_BANK,SetLatchBank64xx
        MiniVDDDispatch RESET_LATCH_BANK,ResetLatchBank64xx
        MiniVDDDispatch PRE_HIRES_TO_VGA,PreHiResToVGA
        MiniVDDDispatch POST_HIRES_TO_VGA,PostHiResToVGA
        MiniVDDDispatch PRE_VGA_TO_HIRES,PreVGAToHiRes
        MiniVDDDispatch POST_VGA_TO_HIRES,PostVGAToHiRes
        MiniVDDDispatch SAVE_REGISTERS,SaveRegisters64xx
        MiniVDDDispatch RESTORE_REGISTERS,RestoreRegisters64xx
        MiniVDDDispatch VIRTUALIZE_GCR_IN,VirtGRIn64xx
        MiniVDDDispatch VIRTUALIZE_GCR_OUT,VirtGROut64xx
        MiniVDDDispatch DISPLAY_DRIVER_DISABLING,DisplayDriverDisabling
        MiniVDDDispatch GET_CHIP_ID,GetChipID
        jmp     MVDIGoodExit            ;that's it for the CL-64xx
;
MVDIGoodExit:
        clc                             ;return success
        jmp     MVDIExit                ;
;
MVDIErrorExit:
        stc
;
MVDIExit:
        pop     ebx                     ;restore Windows VM handle to EBX
        ret
EndProc MiniVDD_Dynamic_Init
;
;
subttl          Utility Card ID Routines for MiniVDD_Dynamic_Init
page +
;
; these two tables convert Int10h chip ID values to internal chipID values
; which are bit positions. These two tables match the ID tables in vga.asm
; of the cirrus.drv mini display driver.

public  MiniVDD_54xxChipIDTable
MiniVDD_54xxChipIDTable label word
                dw      11h,CL_5420             ; 5420
                dw      12h,CL_5420             ; 5420
                dw      13h,CL_5422             ; 5422
                dw      14h,CL_5422             ; 5424
                dw      15h,CL_5426             ; 5426
                dw      16h,CL_5420             ; 5420r1
                dw      18h,CL_5428             ; 5428
                dw      19h,CL_5429             ; 5429
                dw      20h,CL_62x5             ; 6205
                dw      21h,CL_62x5             ; 6215
                dw      22h,CL_62x5             ; 6225
                dw      23h,CL_6235             ; 6235
                dw      24h,CL_6245             ; 6245
                dw      31h,CL_5434             ; 5434
                dw      32h,CL_5430             ; 5430
                dw      33h,CL_5434             ; 5434 .6 micron
                dw      41h,CL_7542             ; Nordic 7542
                dw      42h,CL_7543             ; Viking 7543
                dw      43h,CL_7541             ; Nordic Lite 7541)
                dw      0,0                     ; end of table

public  MiniVDD_64xxChipIDTable
MiniVDD_64xxChipIDTable label word
; the following chips are the 64xx series, which is a different architecture!
                dw      40h,CL_6440             ; 6440
                dw      7,CL_6420               ; 6420
                dw      0,0                     ; end of table

public  is_it_CL542x
BeginProc is_it_CL542x
;
; For CIRRUS 542x, check to see if SR6 is toggle-able, and its an error if not.
;
        mov     edx,3c4h                ;EDX --> Sequencer index register
        in      al,dx                   ;get and save the Sequencer index reg
        ror     eax,8                   ;
;
        mov     al,06h                  ;this is "unlock extension regs" index
        out     dx,al                   ;
        inc     edx                     ;
        in      al,dx                   ;get and save current value of SR6
        ror     eax,8                   ;
        mov     al,12h                  ;this will unlock the extension regs
        out     dx,al                   ;
        in      al,dx                   ;read back this register
        cmp     al,12h                  ;must be the same as what we wrote
        jne     NotCirrus542x           ;oops, it isn't a CL-542xx
        not     al                      ;set an arbitrary value that isn't 12H
        out     dx,al                   ;
        in      al,dx                   ;any other value than 12H returns 0FH
        cmp     al,0fh                  ;must be 0FH
        jne     NotCirrus542x
;
        mov     al,12h                  ;re-open the extension registers
        out     dx,al                   ;
;
;Call the Cirrus ROM BIOS to get the ChipID byte:
;
        mov     ax,1200h                ;VGA ROM BIOS func: DoExtendedCirrusFunction
        mov     bl,80h                  ;subfunction: get ChipID
        push    dword ptr 10h           ;ROM BIOS: Video Interrupt
        VMMCall Exec_VxD_Int            ;returns AL=ChipID 
        lea     ebx,MiniVDD_54xxChipIDTable
CheckID54xx:
        mov     cl,[ebx]        ; get ID in table entry
        and     cl,cl           ; Q: end of table?
        jz      NotCirrus542x   ; Y: chip not found
        add     ebx,4
        cmp     al,cl           ; Q: does the id match?
        jnz     CheckID54xx     ; N: try next table entry
        movzx   ebx,word ptr [ebx-2]
                                ; Y: get the bitwise ID value
        jmp     is_it_CL542x_Exit
NotCirrus542x:
        xor     ebx,ebx                 ;return that we're not a CL-542x
;
is_it_CL542x_Exit:
        mov     edx,3c4h                ;we must restore the stuff we touched
        mov     al,06h                  ;first, restore Sequencer register 6
        out     dx,al                   ;
        inc     edx                     ;
        rol     eax,8                   ; restore the saved SR6 value
        out     dx,al                   ;
        dec     edx                     ;
        rol     eax,8                   ;restore the saved Sequencer index reg
        out     dx,al                   ;
        ret                             ;
EndProc is_it_CL542x
;
;
public  is_it_CL64xx
BeginProc is_it_CL64xx
        mov     edx,3ceh                ;EDX --> GCR index register
        in      al,dx                   ;get and save the GCR index register
        ror     eax,8                   ;
        mov     al,0ah                  ;this is "unlock extension regs" index
        out     dx,al                   ;
        inc     edx                     ;
        in      al,dx                   ;get & save the current value of GCR 0AH
        ror     eax,8                   ;
;
        mov     al,0ceh                 ;re-lock the extension registers
        out     dx,al                   ;
        in      al,dx                   ;now, read back GCR register 0AH
        cmp     al,0h                   ;should be a 0 if we're a CL-64xx
        jnz     NotCirrus64xx           ;oops, we're not a CL-64xx!
;
        mov     al,0ech                 ;unlock the extension registers
        out     dx,al                   ;
        in      al,dx                   ;now, read back GCR register 0AH
        cmp     al,1                    ;should be a 1 if we're a CL-64xx
        jnz     NotCirrus64xx           ;oops, we're not a CL-64xx!
;
;Now get the chip id (a 6440 new deluxe chip or a plain old 6420):
;
        mov     ax,1200h                ;VGA ROM BIOS func: DoExtendedCirrusFunction
        mov     bl,80h                  ;subfunction: get ChipID
        push    dword ptr 10h           ;ROM BIOS: Video Interrupt
        VMMCall Exec_VxD_Int            ;returns AL=ChipID 
        lea     ebx,MiniVDD_64xxChipIDTable
CheckID64xx:
        mov     cl,[ebx]        ; get ID in table entry
        and     cl,cl           ; Q: end of table?
        jz      NotCirrus64xx   ; Y: chip not found
        add     ebx,4
        cmp     al,cl           ; Q: does the id match?
        jnz     CheckID64xx     ; N: try next table entry
        movzx   ebx,word ptr [ebx-2]
                                ; Y: get the bitwise ID value
        jmp     is_it_CL64xx_Exit       ;and go finish up
;
NotCirrus64xx:
        xor     ebx,ebx         ;return value = not 64xx
;
is_it_CL64xx_Exit:
;
;Restore the registers that we've played with:
;
        mov     edx,3ceh                ;EDX --> GCR index register
        mov     al,0ah                  ;set to GCR register 0AH
        out     dx,al                   ;
        inc     edx                     ;EDX --> GCR data register
        rol     eax,8                   ;restore saved value of GCR 0AH
        out     dx,al                   ;
        dec     edx                     ;EDX --> GCR index register again
        rol     eax,8                   ;saved GCR index value
        out     dx,al                   ;
        ret                             ;
EndProc is_it_CL64xx
;
;
subttl          Return ChipID To Main VDD
page +
Public  MiniVDD_GetChipID
BeginProc MiniVDD_GetChipID
;
;Entry:
;       Nothing assumed.
;Exit:
;       EAX contains the ChipID.
;       Preserve ALL other registers.
;
;This routine is used by the Main VDD's Plug&Play code to determine whether
;a card has been changed since the last time that Windows was booted.  We
;are called to return to ChipID.  This assures us that Plug&Play will detect
;a different card, even if both cards use this same MiniVDD.  If the ChipID
;has changed, Plug&Play will get wind of it and will take appropriate action.
;
        mov     eax,ChipID              ;return ChipID to caller
        ret                             ;
EndProc         MiniVDD_GetChipID
;
;
VxD_ICODE_ENDS
;
;
subttl          Dispatch Table for VMM Calling This Mini-VDD
page +
VxD_LOCKED_CODE_SEG
;
;
Begin_Control_Dispatch  MiniVDD
        Control_Dispatch Sys_Dynamic_Device_Init, MiniVDD_Dynamic_Init
        Control_Dispatch Device_Init,        MiniVDD_Dynamic_Init
End_Control_Dispatch MiniVDD
;
;
subttl          Register Display Driver Dependent Data
page +
public  MiniVDD_RegisterDisplayDriver
BeginProc MiniVDD_RegisterDisplayDriver, RARE
;
;Oft-times, the display driver must set flags whenever it is doing
;certain things to a hardware BLTer (such as "I'm in the middle of
;transferring a source bitmap").  This routine is called to allow
;this mini-VDD to get data (usually containing addresses inside the
;display driver's Data segment) directly from the display driver.
;We can also save appropriately needed states here.
;
;       The only client registers that are reserved (and cannot be used
;to pass data in) are EAX (which contains the function code which the
;display driver uses to call the "main" VDD) and EBX (which contains the
;Windows VM handle).  When we get called by the "main" VDD, we'll be
;passed a flag in Client_AL which will be set to non-zero if the VFLATD
;VxD is being used for banked support or zero if VFLATD is NOT involved in
;this session.
;
;Entry:
;       EBP --> the Client Register Structure (documented in VMM.INC)
;       Client_DX contains the display driver "extra" screen selector.
;       Client_DS:DI contains the address of display driver's InSrcBLT flag.
;               -- Client_DI is 0 if InSrcBLT control not necessary.
;
;There is a bug on certain VGA ROM BIOS's with the CIRRUS cards that results in
;loss of synch when switching from full-screen back into Windows HiRes mode.
;This is caused by a race condition in these ROM BIOS's which results in
;a mis-setting of the Miscallaneous Output Register.  Since we know that
;we're in Windows HiRes mode now, we must save off the correct state of
;this register so that we can restore it properly when needed:
;
        mov     edx,3cch                ;this is where we read Misc Output Reg
        in      al,dx                   ;
        mov     WindowsMiscOutputState,al
;
;The Cirrus Logic drivers need to utilize two selectors during "block move"
;operations.  Since the MiniVDD depends on writes to the VFLATD selector
;as its method for knowing when to set the hardware in and out of Windows
;HiRes mode, writes to this second selector won't cause us to be called.
;Therefore, it might be possible during a block move to have the hardware
;state!  We now call a special service provided by the Main VDD to
;"register" this second screen selector and setup the mechanism by which
;writes to this selector will be trapped and handled at the appropriate
;time:
;
        mov     eax,[ebp].Client_EDX    ;get passed in "extra" selector
        cmp     eax,0b8000h             ;is it physical address B8000H?
        jne     MRDDRegisterSelector    ;no, just register as descibed above
        mov     ecx,MemoryMappedSelector;assume we already have a selector
        or      ecx,ecx                 ;already have a selector?
        jnz     @F                      ;yes, just return the same one
        VMMCall _MapPhysToLinear,<eax, 32*1024, 0>
;
;Make the descriptor for this linear address so we can use it to allocate the
;LDT selector that we need:
;
        VMMCall _BuildDescriptorDWORDs,<eax, (32*1024)/4096, RW_DATA_TYPE, D_GRAN_PAGE, 0>
;
;Now, create an LDT selector for this linear memory:
;
        VMMCall _Allocate_LDT_Selector,<ebx, edx, eax, 1, 0>
        mov     MemoryMappedSelector,eax;save it for our own use
@@:     mov     eax,MemoryMappedSelector
        mov     [ebp].Client_DX,ax      ;return newly alloc'd selector to caller
;
public  MRDDRegisterSelector
MRDDRegisterSelector:
        VxDCall VDD_Register_Extra_Screen_Selector
;
public  MRDDRegisterInSrcBLT
MRDDRegisterInSrcBLT:
        cmp     [ebp].Client_DI,0       ;does chipset require InSrcBLT control?
        je      MRDDExit                ;nope, don't register InSrcBLT!
        mov     ax,(Client_DS SHL 8) + Client_DI
        VMMCall Map_Flat                ;returns EAX = the linear address
;
;We need to register the InSrcBLTAddr with the Main VDD so it won't allow
;a MemC change when the display driver is doing a Source BLT (ie: sending
;data from system memory to the screen while the BLTer is waiting).
;
        VxDCall VDD_Set_Sleep_Flag_Addr ;
;
MRDDExit:
        ret                             ;
EndProc MiniVDD_RegisterDisplayDriver
;
;
subttl          Set Refresh Rates Before Display Driver Sets Mode
page +
public  MiniVDD_PreCRTCModeChange_5
BeginProc MiniVDD_PreCRTCModeChange_5, RARE
;
;This routine allows the display driver to call us before it does the
;mode set using INT 10H.  This MiniVDD is going to use the routine to
;get the refresh rate by getting a DISPLAYINFO data structure (see
;MINIVDD.INC) and then setting the refresh rate by calling the Cirrus
;specific BIOS functions.
;
;Entry:
;       Nothing assumed.
;Exit:
;       Nothing assumed.
;
;The Main VDD has already retrieved the Refresh_Rate= and X_Resolution entries
;from SYSTEM.INI and will return those values if the registry has nothing
;better to offer.
;
        mov     eax,OFFSET32 DisplayInfoStructure
        mov     ecx,SIZE DISPLAYINFO    ;pass size in ECX, address in EAX
        mov     [eax].diHdrSize,cx      ;fill in this entry
        push    eax                     ;save EAX --> DisplayInfoStructure
        VxDCall VDD_Get_DISPLAYINFO     ;get information from the VDD
        pop     edi                     ;restore EAX --> DisplayInfoStructure
        test    [edi].diInfoFlags,MONITOR_INFO_DISABLED_BY_USER OR \
                        MONITOR_INFO_NOT_VALID OR REGISTRY_RESOLUTION_NOT_VALID
        jnz     MPMCExit                ;we are not to set the refresh rate!
;
;Since there are three different Cirrus ROM BIOS calls involved in refresh
;rate setting (dependent on the generation of the card), we need to invoke
;all three of them:
;
;First, use the BIOS inquiry function to get the current resolution and
;refresh rate bits to fill into our BIOS call registers on the A4H BIOS call.
;We must read this now (before we do the A2 BIOS call) because on cards
;that are "A4 capable", the A2 call will set values that will be wrong.
;Therefore, we do this call NOW and save the values for the A4 call before
;we do the potentially damaging A2 call:
;
        push    edi                     ;save EDI --> DISPLAYINFO
        mov     ah,12h                  ;Video BIOS call: Inquire User Options
        mov     bl,9ah                  ;
        push    10h                     ;
        VMMCall Exec_VxD_Int            ;
        pop     edi                     ;restore EDI --> DISPLAYINFO
        push    eax                     ;save returned values till we do A4 call
        push    ebx                     ;
        push    ecx                     ;
;
;Now, set the refresh rate using INT 10H, function A2H.  This is an old,
;obsolete way to set the refresh rate but is used on certain older generation
;cards:
;
        test    [edi].diInfoFlags,REFRESH_RATE_MAX_ONLY
                                        ;did we not get horizontal rate data?
        jnz     MPMCDoA3BIOSCall        ;nope, don't call A2H
        movzx   ecx,[edi].diLowHorz     ;get low horizontal value into ECX
        movzx   edx,[edi].diHighHorz    ;get high horizontal value into EDX
        mov     esi,edx                 ;(get this into ESI for compare)
        or      esi,ecx                 ;are LowHorz and HighHorz zero?
        jz      MPMCDoA3BIOSCall        ;yep, don't call A2H
;
        mov     al,7                    ;setup for a 64.0 KHz horizontal max
        cmp     edx,64                  ;is it a super-duper monitor?
        jae     MPMCA2BIOSCommon        ;yes, setup for it
;
        mov     al,6                    ;setup for a 56.0 KHz horizontal max
        cmp     edx,56                  ;is it a super monitor?
        jae     MPMCA2BIOSCommon        ;yes, setup for it
;
        mov     al,5                    ;setup for a 48.0 KHz horizontal max
        cmp     edx,48                  ;is it a duper monitor?
        jae     MPMCA2BIOSCommon        ;yes, setup for it
;
        mov     al,4                    ;setup for a 37.8 KHz horizontal max
        cmp     edx,37                  ;is it an OK monitor?
        jae     MPMCA2BIOSCommon        ;yes, setup for it
;
        mov     al,3                    ;setup for a 35.5 KHz horizontal max
        cmp     edx,35                  ;is it a bad monitor?
        jae     MPMCA2BIOSCommon        ;yes, setup for it
;
        xor     al,al                   ;setup for a 31.5 KHz single scan
;
MPMCA2BIOSCommon:
        push    edi                     ;save EDI --> DISPLAYINFO
        mov     ah,12h                  ;ROM BIOS: Extended VGA Functions
        mov     bl,0a2h                 ;Cirrus Function: SetMonitorType
        push    10h                     ;
        VMMCall Exec_VxD_Int            ;
        pop     edi                     ;restore EDI --> DISPLAYINFO
;
public  MPMCDoA3BIOSCall
MPMCDoA3BIOSCall:
        movzx   ecx,[edi].diRefreshRateMax
        cmp     [edi].diXRes,640        ;running at 640x480?
        jne     MPMCDoA4BIOSCall        ;nope, no need to do A3 BIOS call
        mov     al,1                    ;assume we can run at 72Hz
        cmp     ecx,72                  ;can we do 72Hz?
        jae     @F                      ;sure can, go set it!
        xor     al,al                   ;oops, set to 60Hz
@@:     push    edi                     ;save EDI --> DISPLAYINFO
        mov     ah,12h                  ;ROM BIOS: Extended VGA Functions
        mov     bl,0a3h                 ;Cirrus Function: SetRefreshType
        push    10h                     ;
        VMMCall Exec_VxD_Int            ;
        pop     edi                     ;restore EDI --> DISPLAYINFO
;
public  MPMCDoA4BIOSCall
MPMCDoA4BIOSCall:
        pop     ecx                     ;restore values from inquiry call
        pop     ebx                     ;
        pop     eax                     ;
;
;We always want to force the maximum scanlines field to 1024 lines:
;
        mov     al,03h                  ;this flags "Maximum Scanlines = 1024"
;
;Get the 640x480 refresh rate field isolated in bit 4 of AH:
;
        and     ah,40h                  ;isolate the field
        shr     ah,2                    ;and get it in bit 4 of AH
        or      al,ah                   ;now AL is pre-filled for call
;
;Get the 800x600 refresh rate field isolated in the low nibble of BH and
;the 1024x768 refresh rate field isolated in the high nibble of BH:
;
        mov     bh,ch                   ;now BH is pre-filled for call
;
;Get the 1280x1024 refresh rate field isolated in the high nibble of CH:
;
        mov     ch,cl                   ;
        shl     ch,3                    ;
        and     ch,70h                  ;top bit is unused in high nibble
;
;First, dispatch to the correct resolution handler:
;
        movzx   esi,[edi].diRefreshRateMax
        movzx   edx,[edi].diYRes        ;
        or      edx,edx                 ;any Y-resolution in the structure?
        jz      MPMCExit                ;nope, blow out of here!
        cmp     edx,480                 ;running 640x480?
        je      MPMC_A4_640             ;yes, go handle it
        cmp     edx,600                 ;running 800x600?
        je      MPMC_A4_800             ;yes, go handle it
        cmp     edx,768                 ;running 1024x768?
        je      MPMC_A4_1024            ;yes, go handle it
;
public  MPMC_A4_1280
MPMC_A4_1280:
        mov     ch,20h                  ;assume we can do 70 Hz
        cmp     esi,70                  ;can we do 70 Hz?
        jae     MPMC_A4_Common          ;yes, go set it
        mov     ch,10h                  ;assume we can do 60 Hz
        cmp     esi,60                  ;can we do 60 Hz?
        jae     MPMC_A4_Common          ;yes, go set it
        xor     ch,ch                   ;set to 43 Hz (interlaced)
        jmp     MPMC_A4_Common          ;yes, go set it
;
public  MPMC_A4_1024
MPMC_A4_1024:
        and     bh,NOT 0f0h             ;get rid of 1024x768 bits from BH
        or      bh,40h                  ;assume we can do 75 Hz
        cmp     esi,75                  ;can we do 75 Hz?
        jae     MPMC_A4_Common          ;yes, go set it
        and     bh,NOT 0f0h             ;get rid of 1024x768 bits from BH
        or      bh,30h                  ;assume we can do 72 Hz
        cmp     esi,72                  ;can we do 72 Hz?
        jae     MPMC_A4_Common          ;yes, go set it
        and     bh,NOT 0f0h             ;get rid of 1024x768 bits from BH
        or      bh,20h                  ;assume we can do 70 Hz
        cmp     esi,70                  ;can we do 70 Hz?
        jae     MPMC_A4_Common          ;yes, go set it
        and     bh,NOT 0f0h             ;get rid of 1024x768 bits from BH
        or      bh,10h                  ;assume we can do 60 Hz
        cmp     esi,60                  ;can we do 60 Hz?
        jae     MPMC_A4_Common          ;yes, go set it
        and     bh,NOT 0f0h             ;set to 43 Hz (interlaced)
        jmp     MPMC_A4_Common          ;yes, go set it
;
public  MPMC_A4_800
MPMC_A4_800:
        and     bh,NOT 0fh              ;get rid of 800x600 bits from BH
        or      bh,03h                  ;assume we can do 75 Hz
        cmp     esi,75                  ;can we do 75 Hz?
        jae     MPMC_A4_Common          ;yes, go set it
        and     bh,NOT 0fh              ;get rid of 800x600 bits from BH
        or      bh,02h                  ;assume we can do 72 Hz
        cmp     esi,72                  ;can we do 72 Hz?
        jae     MPMC_A4_Common          ;yes, go set it
        and     bh,NOT 0fh              ;get rid of 800x600 bits from BH
        or      bh,01h                  ;assume we can do 60 Hz
        cmp     esi,60                  ;can we do 60 Hz?
        jae     MPMC_A4_Common          ;yes, go set it
        and     bh,NOT 0fh              ;set to 56 Hz
        jmp     MPMC_A4_Common          ;yes, go set it
;
public  MPMC_A4_640
MPMC_A4_640:
        or      al,10h                  ;assume we can do 75 Hz
        cmp     esi,75                  ;can we do 75 Hz?
        jae     MPMC_A4_Common          ;yes, go set it
        and     al,NOT 10h              ;set 640x480 at 60 Hz
;
public  MPMC_A4_Common
MPMC_A4_Common:
        mov     ah,12h                  ;ROM BIOS: Extended VGA Functions
        mov     bl,0a4h                 ;Cirrus Function: SetMonitorType
        push    10h                     ;
        VMMCall Exec_VxD_Int            ;
;
MPMCExit:
        ret                             ;return to main VDD
EndProc MiniVDD_PreCRTCModeChange_5
;
;
subttl          Calculate and Save Banking Register Values
page +
public  MiniVDD_GetVDDBank
BeginProc MiniVDD_GetVDDBank, DOSVM
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
;       EBX contains the Windows VM handle (which must be preserved).
;       ECX contains the byte offset of where in video memory the VDD
;           memory is to be placed (this will be the first byte of
;           off-screen memory).
;Exit:
;       AH contains the page offset from start of bank of virtualization area.
;       EBX must still contain the Windows VM handle.
;       ECX contains the starting address of the VDD virtualization area
;       EDX contains the amount of memory that we allocated to
;       the VDD virtualization area.  We set EDX == ECX if we are in
;       a "memory shy" configuration.  That is, to tell the "main"
;       VDD that we cannot support VGA 4 plane graphics in the background.
;
        mov     DisplayEnabledFlag,0ffh ;flag that we're in HiRes mode
        mov     BankRegSave,0ffh        ;re-init this to "no banking"
;
;Let's find out in which 64K bank the off-screen memory starts:
;
        mov     eax,ecx                 ;get start address of off-screen memory
        shr     eax,16                  ;divide by 64K per bank -- AL has bank #
;
;At this point:
;       AL contains the bank number of the start of off-screen memory
;       CX contains the offset within the bank of the start of
;          off-screen memory.
;
;It is desirable for us to start the VDD memory area on an even bank
;boundry.  This is possible if we have at least one full 64K bank
;of video memory remaining.  To determine this, we must calculate
;the total number of banks available in video memory.  If there's
;at least one more than what's in AL, we can start on an even bank
;boundry!
;
        or      cx,cx                   ;are we already on an even bank boundry?
        jz      MGVBCalcAmountToUse     ;yes, go see if we can alloc 64K
        mov     edx,TotalMemorySize     ;get total amount of video memory
        shr     edx,16                  ;now DL has total banks on system
        dec     dl                      ;now DL has last possible bank nbr
        cmp     al,dl                   ;can we start on an even bank boundry?
        jae     MGVBMemoryShy           ;nope, turn off VGA virtualization
        inc     al                      ;no, put ourselves at next bank boundry
        xor     cx,cx                   ;zero the offset in low word of ECX
        add     ecx,10000h              ;and set ECX = new start of VDD area
;
public  MGVBCalcAmountToUse
MGVBCalcAmountToUse:
;
;At this point:
;       AL contains the bank number for VDD use.
;       ECX contains the 32 bit address of the start of this memory.
;
        mov     edx,TotalMemorySize     ;get total amount of video memory
        sub     edx,ecx                 ;EDX = amount of memory from start
                                        ;of VDD bank till end of video memory
        cmp     edx,64*1024             ;do we have more than 64K left?
        mov     edx,64*1024             ;(assume we do have more than 64K left)
        ja      MGVBGetBankingValues    ;we can use an entire 64K bank for VDD!
        mov     edx,32*1024             ;we can only use 32K for the VDD!
;
public  MGVBGetBankingValues
MGVBGetBankingValues:
;
;At this point:
;       AL contains the bank number for the VDD virtualization bank.
;       ECX contains the 32 bit start address of the VDD virtualization area.
;       EDX contains the size of the VDD virtualization area (either 32K,
;           64K, or -1 indicating a memory shy configuration).
;
;We should setup the values for the CIRRUS banking registers so that we can set
;them quickly when called upon by the "main" VDD to do so:
;
        test    ChipID,CL_6420+CL_6440  ;running on a CL-64xx family chip?
        jnz     MGVBHandleCL64xx        ;yes! go setup banking for CL-64xx chips
;
public  MGVBHandleCL542x
MGVBHandleCL542x:
;
;We setup saved values for banking.  On the CL-542x family cards with more
;than 1 meg of memory on them, we must use 16K banks (each bank number
;represents 16K of video memory).  On cards that have 1 meg or less, we
;use 4K banks (we could use 16K also on 1 meg configurations, but then we
;would not be compatible with 62x5 products).
;
        push    ecx                     ;save value in ECX for now
        mov     ecx,4                   ;assume 4K banks (multiply by 16)
                                        ;VDDBankControl value is in CH
        cmp     TotalMemorySize,1024*1024
        jle      @F                      ;
        mov     ecx,2002h               ;we need to use 16K banks (mult by 4)
                                        ;VDDBankControl value is in CH
;
;Shift over the 64K bank value in AL by the 4K or 16K shift value contained
;in CL and save the value for the banking register:
;
@@:     shl     al,cl                   ;
        mov     VDDBankControl,ch       ;save off correct value for GCR reg 0BH
        shr     al,2                    ;divide by 4 because we have 4 planes
        mov     VDDBank,al              ;save this for use in virtualization
        pop     ecx                     ;restore saved ECX
        jmp     MGVBExit                ;we're done!
;
public  MGVBHandleCL64xx
MGVBHandleCL64xx:
;
;On the CL-64xx series, we always have 4K banks:
;
        shl     al,4                    ;its a 4K bank value (multiply by 16)
        shr     al,2                    ;divide by 4 because we have 4 planes
        mov     VDDBank,al              ;save this for use in virtualization
        jmp     MGVBExit                ;we're done!
;
public  MGVBMemoryShy
MGVBMemoryShy:
;
;If we reach this point, it means that the Windows visible screen overlaps
;into the very last bank of available video memory.  This creates a
;"memory-shy" configuration which prohibits us from running windowed
;and background EGA/VGA graphics mode apps.
;
;At this point:
;       ECX contains the value passed in at the beginning of the routine.
;
        mov     edx,ecx                 ;indicate a "memory-shy" configuration
;
MGVBExit:
        xor     ah,ah                   ;no page offsets for now
        ret
EndProc MiniVDD_GetVDDBank
;
;
subttl          Banking Handling Code for VGA Virtualization
page +
public  MiniVDD_SetVDDBank542x
BeginProc MiniVDD_SetVDDBank542x, DOSVM
;
;This routine is called when we need to virtualize something in the
;off-screen region of the video memory.  You should save the current
;state of the banking registers and then set your banking registers to
;the off-screen memory region.
;
;Entry:
;       EBX contains the MemC owner's VM handle.
;Exit:
;       Save any registers that you use.
;
        cmp     BankRegSave,0ffh        ;are we already set to the VDD bank?
        jne     MSVExit_5               ;yes! don't do it again!
        push    eax                     ;
        push    edx                     ;
;
;Let's make sure that the CIRRUS extended registers are unlocked.  This is
;probably not necessary when our CRTC VM is the Windows VM but most likely
;is when the CRTC VM is not the Windows VM:
;
        unlock_extension_regs_542x
        mov     edx,3ceh                ;EDX --> GCR index register
        in      al,dx                   ;get current GCR index
        ror     eax,8                   ;save it in top byte of EAX
;
        mov     al,09h                  ;set to banking register
        out     dx,al                   ;
        inc     edx                     ;
        in      al,dx                   ;now AL has current bank state
        mov     BankRegSave,al          ;save this for later restore
;
        mov     al,VDDBank              ;now, set the VDD bank
        out     dx,al                   ;
;
        dec     edx                     ;EDX --> GCR index register
        rol     eax,8                   ;AL now contains saved GCR index value
        out     dx,al                   ;

        pop     edx                     ;restore saved registers
        pop     eax                     ;
;
MSVExit_5:
        ret                             ;
EndProc MiniVDD_SetVDDBank542x
;
;
public  MiniVDD_SetVDDBank64xx
BeginProc MiniVDD_SetVDDBank64xx, DOSVM
;
;This routine is called when we need to virtualize something in the
;off-screen region of the video memory.  You should save the current
;state of the banking registers and then set your banking registers to
;the off-screen memory region.
;
;Entry:
;       EBX contains the MemC owner's VM handle.
;Exit:
;       Save any registers that you use.
;
        cmp     BankRegSave,0ffh        ;are we already set to the VDD bank?
        jne     MSVExit_6               ;yes! don't do it again!
        push    eax                     ;
        push    edx                     ;
;
;Let's make sure that the CIRRUS extended registers are unlocked.  This is
;probably not necessary when our CRTC VM is the Windows VM but most likely
;is when the CRTC VM is not the Windows VM:
;
        unlock_extension_regs_64xx
;
        mov     edx,3ceh                ;EDX --> GCR index register
        in      al,dx                   ;get current GCR index
        ror     eax,8                   ;save it in top byte of EAX
;
;Make sure banking is enabled and set to one 64K bank:
;
        mov     ax,030dh                ;
        out     dx,ax                   ;
;
        mov     al,0eh                  ;set to banking register
        out     dx,al                   ;
        inc     edx                     ;
        in      al,dx                   ;now AL has current bank state
        mov     BankRegSave,al          ;save this for later restore
;
        mov     al,VDDBank              ;now, set the VDD bank
        out     dx,al                   ;
;
        dec     edx                     ;EDX --> GCR index register
        rol     eax,8                   ;AL now contains saved GCR index value
        out     dx,al                   ;

        pop     edx                     ;restore saved registers
        pop     eax                     ;
;
MSVExit_6:
        ret                             ;
EndProc MiniVDD_SetVDDBank64xx
;
;
public  MiniVDD_ResetBank542x
BeginProc MiniVDD_ResetBank542x, DOSVM
;
;This routine is called when the VDD is done using the off-screen memory
;and we want to restore to the bank that we were in before we started using
;the off-screen memory.  Note that if the bank that's currently set in the
;hardware's banking registers is NOT the VDD bank, that we do not wish to
;reset the banking since someone else purposely changed it and we don't
;want to override those purposeful changes.
;
;Entry:
;       EBX contains the VM handle.
;Exit:
;       Save anything that you use.
;
        cmp     BankRegSave,0ffh        ;is there any banking to restore?
        je      MRBExit_5               ;nope, skip this!
;
;We may need to do something.  Save off the current GCR index register state:
;
        push    eax                     ;
        push    edx                     ;
;
;Probably not necessary when our CRTC VM is the Windows VM but most likely
;is when the CRTC VM is not the Windows VM:
;
        unlock_extension_regs_542x
;
;Get the values of the banking register so we can see if its still
;set to "our" bank.  If it isn't, then Windows has already switched
;it to something for its own purposes and we'd best not restore it!
;
        mov     edx,3ceh                ;EDX --> GCR index register
        in      al,dx                   ;get current GCR index
        ror     eax,8                   ;save it in top byte of EAX
;
        mov     al,09h                  ;this is the 542x banking register
        out     dx,al                   ;
        inc     edx                     ;
        in      al,dx                   ;
;
        cmp     al,VDDBank              ;are we still set to VDD bank?
        jne     MRBResetBankSaveFlags_5 ;nope, don't physically reset bank regs
;
;It's safe to restore the banking registers!
;
;At this point:
;       EDX -> GCR Data Register
;
        mov     al,BankRegSave          ;get bank to restore
        out     dx,al                   ;
;
public  MRBResetBankSaveFlags_5
MRBResetBankSaveFlags_5:
        mov     BankRegSave,0ffh        ;flag that we're not set to VDD bank
;
;Lastly, restore the GCR index register that we saved earlier.
;
        mov     edx,3ceh                ;make sure EDX --> GCR index register
        rol     eax,8                   ;AL now contains saved GCR index value
        out     dx,al                   ;
        pop     edx                     ;restore saved registers
        pop     eax                     ;
;
MRBExit_5:
        ret                             ;
EndProc MiniVDD_ResetBank542x
;
;
public  MiniVDD_ResetBank64xx
BeginProc MiniVDD_ResetBank64xx, DOSVM
;
;This routine is called when the VDD is done using the off-screen memory
;and we want to restore to the bank that we were in before we started using
;the off-screen memory.  Note that if the bank that's currently set in the
;hardware's banking registers is NOT the VDD bank, that we do not wish to
;reset the banking since someone else purposely changed it and we don't
;want to override those purposeful changes.
;
;Entry:
;       EBX contains the VM handle.
;Exit:
;       Save anything that you use.
;
        cmp     BankRegSave,0ffh        ;is there any banking to restore?
        je      MRBExit_6               ;nope, skip this!
;
;We may need to do something.  Save off the current GCR index register state:
;
        push    eax                     ;
        push    edx                     ;
;
;Probably not necessary when our CRTC VM is the Windows VM but most likely
;is when the CRTC VM is not the Windows VM:
;
        unlock_extension_regs_64xx
;
;Get the values of the banking register so we can see if its still
;set to "our" bank.  If it isn't, then Windows has already switched
;it to something for its own purposes and we'd best not restore it!
;
        mov     edx,3ceh                ;EDX --> GCR index register
        in      al,dx                   ;get current GCR index
        ror     eax,8                   ;save it in top byte of EAX
;
        mov     al,0eh                  ;this is the 64xx banking register
        out     dx,al                   ;
        inc     edx                     ;
        in      al,dx                   ;
;
        cmp     al,VDDBank              ;are we still set to VDD bank?
        jne     MRBResetBankSaveFlags_6 ;nope, don't physically reset bank regs
;
;It's safe to restore the banking registers!
;
;At this point:
;       EDX -> GCR Data Register
;
        mov     al,BankRegSave          ;get bank to restore
        out     dx,al                   ;
;
public  MRBResetBankSaveFlags_6
MRBResetBankSaveFlags_6:
        mov     BankRegSave,0ffh        ;flag that we're not set to VDD bank
;
;Lastly, restore the GCR index register that we saved earlier.
;
        mov     edx,3ceh                ;make sure EDX --> GCR index register
        rol     eax,8                   ;AL now contains saved GCR index value
        out     dx,al                   ;
        pop     edx                     ;restore saved registers
        pop     eax                     ;
;
MRBExit_6:
        ret                             ;
EndProc MiniVDD_ResetBank64xx
;
;
subttl          Set To Latch Scratchpad Bank
page +
public  MiniVDD_SetLatchBank542x
BeginProc MiniVDD_SetLatchBank542x, DOSVM
;
;When virtualizing the VGA 4 plane mode, we have to save and restore the
;latches occasionally.  This routine allows you to set to an off-screen
;bank (in this case and in most cases, the VDD bank) in order to prepare
;for restoring the VGA latches.  This routine is NOT called for saving
;the latches since this is done by simply using the standard VGA CRTC
;register 22H which all super-VGA's possess (we hope).
;
;Entry:
;       Nothing assumed.
;Exit:
;       Save anything that you use.
;
        push    eax                     ;save registers that we use
        push    edx                     ;
        unlock_extension_regs_542x
;
;Get and save the GCR index register value:
;
        mov     edx,3ceh                ;
        in      al,dx                   ;
        ror     eax,8                   ;
;
;And set to our reserved latch bank:
;
        mov     al,09h                  ;set to banking register
        out     dx,al                   ;
        inc     edx                     ;
        in      al,dx                   ;get current value
        mov     LatchSaveOFFSET0,al     ;and save it
        mov     al,VDDBank              ;
        out     dx,al                   ;
;
;Restore the GCR index register:
;
        dec     edx                     ;
        rol     eax,8                   ;
        out     dx,al                   ;
;
MSLBExit_5:
        pop     edx                     ;restore saved registers
        pop     eax                     ;
        ret                             ;
EndProc MiniVDD_SetLatchBank542x
;
;
public  MiniVDD_SetLatchBank64xx
BeginProc MiniVDD_SetLatchBank64xx, DOSVM
;
;When virtualizing the VGA 4 plane mode, we have to save and restore the
;latches occasionally.  This routine allows you to set to an off-screen
;bank (in this case and in most cases, the VDD bank) in order to prepare
;for restoring the VGA latches.  This routine is NOT called for saving
;the latches since this is done by simply using the standard VGA CRTC
;register 22H which all super-VGA's possess (we hope).
;
;Entry:
;       Nothing assumed.
;Exit:
;       Save anything that you use.
;
        push    eax                     ;save registers that we use
        push    edx                     ;
        unlock_extension_regs_64xx
;
;Get and save the GCR index register value:
;
        mov     edx,3ceh                ;
        in      al,dx                   ;
        ror     eax,8                   ;
;
;Make sure banking is enabled and set to one 64K bank:
;
        mov     ax,030dh                ;
        out     dx,ax                   ;
;
;And set to our reserved latch bank:
;
        mov     al,0eh                  ;set to banking register
        out     dx,al                   ;
        inc     edx                     ;
        in      al,dx                   ;get current value
        mov     LatchSaveOFFSET0,al     ;and save it
        mov     al,VDDBank              ;
        out     dx,al                   ;
;
;Restore the GCR index register:
;
        dec     edx                     ;
        rol     eax,8                   ;
        out     dx,al                   ;
;
MSLBExit_6:
        pop     edx                     ;restore saved registers
        pop     eax                     ;
        ret                             ;
EndProc MiniVDD_SetLatchBank64xx
;
;
subttl          Reset Banking After Latch Operations
page +
public  MiniVDD_ResetLatchBank542x
BeginProc MiniVDD_ResetLatchBank542x, DOSVM
;
;This routine reverses the latch save that we did prior to restoring the
;latches.  Just restore the states that you saved.
;
;Entry:
;       Nothing assumed.
;Exit:
;       Save anything that you use.
;
        push    eax                     ;save registers that we use
        push    edx                     ;
        mov     edx,3ceh                ;EDX --> GCR index register
        in      al,dx                   ;get and save GCR index value
        ror     eax,8                   ;
;
;Reset the banking register:
;
        mov     al,09h                  ;set to our banking register
        mov     ah,LatchSaveOFFSET0     ;get saved bank value
        out     dx,ax                   ;
;
;Restore the GCR index register:
;
        rol     eax,8                   ;
        out     dx,al                   ;
;
MRLBExit_5:
        pop     edx                     ;restore saved registers
        pop     eax                     ;
        ret                             ;
EndProc MiniVDD_ResetLatchBank542x
;
;
public  MiniVDD_ResetLatchBank64xx
BeginProc MiniVDD_ResetLatchBank64xx, DOSVM
;
;This routine reverses the latch save that we did prior to restoring the
;latches.  Just restore the states that you saved.
;
;Entry:
;       Nothing assumed.
;Exit:
;       Save anything that you use.
;
        push    eax                     ;save registers that we use
        push    edx                     ;
        mov     edx,3ceh                ;EDX --> GCR index register
        in      al,dx                   ;get and save GCR index value
        ror     eax,8                   ;
;
;Reset the banking register:
;
        mov     al,0eh                  ;set to our banking register
        mov     ah,LatchSaveOFFSET0     ;get saved bank value
        out     dx,ax                   ;
;
;Restore the GCR index register:
;
        rol     eax,8                   ;
        out     dx,al                   ;
;
MRLBExit_6:
        pop     edx                     ;restore saved registers
        pop     eax                     ;
        ret                             ;
EndProc MiniVDD_ResetLatchBank64xx
;
;
subttl          Prepare to Enter a Standard VGA Mode from HiRes Mode
page +
public  MiniVDD_PreHiResToVGA
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
;The S3 chipset's mode change will get "stuck" if we're in the middle of
;transferring a source bitmap to the graphics engine.  By doing a "warm
;reset" of the chipset, we can avoid this eventuality:
;
        or      CLModeChangeFlags,GOING_TO_VGA_MODE
;
;On the CL-5426 family, reset the graphics engine, just to be safe....
;
        test    ChipID,IS_BLTTER        ;are we a BLTTER chipset?
        jz      MHTVExit                ;nope, we're done!
        mov     edx,3ceh                ;yes, EDX --> GCR index register
        mov     ax,0431h                ;this will cause a graphics engine reset
        out     dx,ax
;
MHTVExit:
        ret
EndProc MiniVDD_PreHiResToVGA
;
;
public  MiniVDD_PostHiResToVGA
BeginProc MiniVDD_PostHiResToVGA, DOSVM
;
;This routine is called after the ROM BIOS call is made to place the hardware
;back in the standard VGA state.  You should reenable trapping and do any
;other post-processing that your hardware might need:
;
        mov     CLModeChangeFlags,0     ;flag that we're done with mode change
;
;We need to make sure that we're set to bank 0.  There's some laptops whose
;BIOS's don't do this!!!!
;
        mov     edx,3ceh                ;EDX --> GCR index register
        in      al,dx                   ;get current GCR index
        ror     eax,8                   ;save it in high byte of EAX
        mov     ax,09h                  ;set GCR offset register 0 to bank 0
        out     dx,ax                   ;
        mov     ax,0ah                  ;set GCR offset register 1 to bank 0
        out     dx,ax                   ;
        rol     eax,8                   ;get back previous GCR index value
        out     dx,al                   ;
;
MPHVExit:
        ret                             ;
EndProc MiniVDD_PostHiResToVGA
;
;
subttl          Prepare to Enter HiRes Mode From a Standard VGA Mode
page +
public  MiniVDD_PreVGAToHiRes
BeginProc MiniVDD_PreVGAToHiRes, DOSVM
;
;We are notified that the VDD is about to call the display driver to
;switch into HiRes mode from a full screen VGA DOS box.  We can do
;anything necessary to prepare for this.  In the case of the S3 VDD,
;we simply set a flag telling us that we're about to change modes.
;
;Entry:
;       EBX contains the Windows VM handle.
;Exit:
;       Nothing assumed.
;
        or      CLModeChangeFlags,GOING_TO_WINDOWS_MODE
;
MSVHExit:
        ret                             ;
EndProc MiniVDD_PreVGAToHiRes
;
;
public  MiniVDD_PostVGAToHiRes
BeginProc MiniVDD_PostVGAToHiRes, DOSVM
;
;We are notified that the VDD is done setting the hardware state back
;to HiRes mode.  We simply unset our CLModeChangeFlags in this case.
;
;Entry:
;       EBX contains the Windows VM handle.
;Exit:
;       Nothing assumed.
;
        mov    CLModeChangeFlags,0      ;
;
MPVHExit:
        ret                             ;
EndProc MiniVDD_PostVGAToHiRes
;
;
subttl          Save and Restore Routines for Extension Registers
page +
public  MiniVDD_SaveRegisters542x
BeginProc MiniVDD_SaveRegisters542x, DOSVM
;
;This routine is called whenever the "main" VDD is called upon to save
;the register state.  The "main" VDD will save all of the states of the
;"normal" VGA registers (CRTC, Sequencer, GCR, Attribute etc.) and will
;then call this routine to allow the mini-VDD to save register states
;that are hardware dependent.  These registers will be restored during the
;routine MiniVDD_RestoreRegisterState.  Typically, only a few registers
;relating to the memory access mode of the board are necessary to save
;and restore.  Specific registers that control the hardware BLTer probably
;need not be saved.  The register state is saved in the mini-VDD's
;"CB data area" which is a per-VM state saving area.  The mini-VDD's
;CB data area was allocated at Device_Init time.
;
;Entry:
;       EBX contains the VM handle for which these registers are being saved.
;Exit:
;       You must preserve EBX, EDX, EDI, and ESI.
;
        push    edx                     ;save required registers
        push    edi                     ;
        unlock_extension_regs_542x
        mov     edi,ebx                 ;get pointer to our CB data area
        add     edi,OurCBDataPointer    ;EDI --> our CB data area for this VM
;
        mov     edx,3ceh                ;EDX --> GCR index register
        in      al,dx                   ;get and save current GCR index ...
        ror     eax,8                   ;in high byte of EAX
;
;We need to save the state of GCR register B since it affects how memory
;is looked at by the CPU and may be messed with during Windows DOS mode
;changes:
;
        mov     al,0bh                  ;set to GCR index B
        out     dx,al                   ;
        inc     edx                     ;EDX --> GCR data register
        in      al,dx                   ;get data from GCR register B
        dec     edx                     ;EDX --> GCR index register
        mov     [edi].GCRRegB,al        ;save the data in our CB data structure
;
        xor     al,al                   ;set to GCR index 0
        out     dx,al                   ;
        inc     edx                     ;EDX --> GCR data register
        in      al,dx                   ;get data from GCR register 0
        dec     edx                     ;EDX --> GCR index register
        mov     [edi].GCRReg0,al        ;save the data in our CB data structure
;
        mov     al,01h                  ;set to GCR index 1
        out     dx,al                   ;
        inc     edx                     ;EDX --> GCR data register
        in      al,dx                   ;get data from GCR register 1
        dec     edx                     ;EDX --> GCR index register
        mov     [edi].GCRReg1,al        ;save the data in our CB data structure
;
        mov     al,05h                  ;set to GCR index 5
        out     dx,al                   ;
        inc     edx                     ;EDX --> GCR data register
        in      al,dx                   ;get data from GCR register 5
        dec     edx                     ;EDX --> GCR index register
        mov     [edi].GCRReg5,al        ;save the data in our CB data structure
;
;Restore the GCR index register saved in the high byte of EAX:
;
        rol     eax,8                   ;
        out     dx,al                   ;
;
;Now, do Sequencer registers 2, and 12:
;
        mov     edx,3c4h                ;EDX --> Sequencer index register
        in      al,dx                   ;get and save current Sequencer index ...
        ror     eax,8                   ;in high byte of EAX
;
;We need to save the state of Sequencer register 2 since the Cirrus chipset
;defines all 8 bits instead of only the low nibble:
;
        mov     al,02h                  ;set to Sequencer index 2
        out     dx,al                   ;
        inc     edx                     ;EDX --> Sequencer data register
        in      al,dx                   ;get data from Sequencer register 2
        dec     edx                     ;EDX --> Sequencer index register
        mov     [edi].SeqReg2,al        ;save the data in our CB data structure
;
        mov     al,12h                  ;set to Sequencer index 12
        out     dx,al                   ;
        inc     edx                     ;EDX --> Sequencer data register
        in      al,dx                   ;get data from Sequencer register 12
        dec     edx                     ;EDX --> Sequencer index register
        mov     [edi].SeqReg12,al       ;save the data in our CB data structure

        mov     [edi].CRTCReg1D,0
;
;Restore the Sequencer index register saved in the high byte of EAX:
;
        rol     eax,8                   ;
        out     dx,al                   ;
;
; On the 7541,7542, and 7543, we need to save the state of the Hidden RAMDAC
; locations, since the BIOS uses these for a scratchpad data area.
; Also, we need to save and restore the scratchpad registers SR9 and SR0A.

        test    ChipID,CL_7541+CL_7542+CL_7543  ; only do on Nordic family!
        jz      MSRSExit_5                      ; not Nordic!
;
; zero out the high bit of CR11. This will enable writes to the CRTC.
;
        mov     [edi].CRTCReg11,0

        in      al,dx                   ;get and save current Sequencer index ...
        ror     eax,8                   ;in high byte of EAX
;
; save the state of SR9 and SR0A first.
;
        mov     al,9h                  ;set to Sequencer index 9
        out     dx,al                   ;
        inc     edx                     ;EDX --> Sequencer data register
        in      al,dx                   ;get data from Sequencer register 9
        dec     edx                     ;EDX --> Sequencer index register
        mov     [edi].SeqReg9,al       ;save the data in our CB data structure

        mov     al,0Ah                  ;set to Sequencer index 0ah
        out     dx,al                   ;
        inc     edx                     ;EDX --> Sequencer data register
        in      al,dx                   ;get data from Sequencer register 0A
        dec     edx                     ;EDX --> Sequencer index register
        mov     [edi].SeqRegA,al       ;save the data in our CB data structure

; now save the state of the hidden DAC registers.
; enable SR12[2], which enables access to the hidden DAC area.
        mov     al,12h                  ; cursor attribute
        out     dx,al
        inc     edx
        in      al,dx                   ; AH has old SR12 value
        ror     eax,8                   ; save old SR12 in high byte of EAX
        or      al,2                    ; allow access to RAMDAC cursor colors
        out     dx,al
;
; now save the hidden DAC area.
        mov     edx,3c7h                ; DAC read index.
        add     edi,HiddenDAC           ; point to the data area
        xor     ah,ah                   ; beginning index value
        mov     ecx,10h                 ; number of data areas
        cld                             ; don't assume!
@@:        
        mov     al,ah
        out     dx,al
        stosb                           ;save the index
        add     edx, 2                  ;EDX-> DAC data register
        in      al,dx                   ;Get red
        stosb                           ;save it
        in      al,dx                   ;green
        stosb                           ;save it
        in      al,dx                   ;blue
        stosb                           ;save it
        sub     edx, 2                  ;EDX -> DAC read index
        inc     ah                      ; 
        loop    @B

; now restore the value of SR12
        mov     edx,3c5h                ;EDX --> Sequencer data register
        rol     eax,8                   ;
        out     dx,al                   ;
        dec     edx                     ;EDX --> Sequencer index register
;
;Restore the Sequencer index register saved in the high byte of EAX:
;
        rol     eax,8                   ; get the saved SR index
        out     dx,al                   ; restore the saved SR index
;
MSRSExit_5:
        pop     edi                     ;restore saved registers
        pop     edx                     ;
        ret
EndProc MiniVDD_SaveRegisters542x
;
;
subttl          Save and Restore Routines for Extension Registers
page +
public  MiniVDD_SaveRegisters64xx
BeginProc MiniVDD_SaveRegisters64xx, DOSVM
;
;This routine is called whenever the "main" VDD is called upon to save
;the register state.  The "main" VDD will save all of the states of the
;"normal" VGA registers (CRTC, Sequencer, GCR, Attribute etc.) and will
;then call this routine to allow the mini-VDD to save register states
;that are hardware dependent.  These registers will be restored during the
;routine MiniVDD_RestoreRegisterState.  Typically, only a few registers
;relating to the memory access mode of the board are necessary to save
;and restore.  Specific registers that control the hardware BLTer probably
;need not be saved.  The register state is saved in the mini-VDD's
;"CB data area" which is a per-VM state saving area.  The mini-VDD's
;CB data area was allocated at Device_Init time.
;
;Entry:
;       EBX contains the VM handle for which these registers are being saved.
;Exit:
;       You must preserve EBX, EDX, EDI, and ESI.
;
        push    edx                     ;save required registers
        push    edi                     ;
        mov     edi,ebx                 ;get pointer to our CB data area
        add     edi,OurCBDataPointer    ;EDI --> our CB data area for this VM
;
        mov     edx,3ceh                ;EDX --> GCR index register
        in      al,dx                   ;get and save current GCR index ...
        ror     eax,8                   ;in high byte of EAX
;
        mov     ax,0ec0ah               ;this will unlock the extension regs
        out     dx,ax                   ;
;
;We need to save the state of GCR register A1H since it must track the
;value in Sequencer register 4 and the the VGA app is sure to change it:
;
        mov     al,0a1h                 ;set to GCR index A1H
        out     dx,al                   ;
        inc     edx                     ;EDX --> GCR data register
        in      al,dx                   ;get data from GCR register A1H
        dec     edx                     ;EDX --> GCR index register
        mov     [edi].GCRRegA1,al       ;save the data in our CB data structure
;
;We probably also should save the state of GCR register (the banking register):
;
        mov     al,0dh                  ;set to GCR index 0DH
        out     dx,al                   ;
        inc     edx                     ;EDX --> GCR data register
        in      al,dx                   ;get data from GCR register 0DH
        dec     edx                     ;EDX --> GCR index register
        mov     [edi].GCRRegD,al        ;save the data in our CB data structure

        mov     al,0ah                  ;set to GCR index 0AH
        out     dx,al                   ;
        inc     edx                     ;EDX --> GCR data register
        in      al,dx                   ;get data from GCR register 0DH
        dec     edx                     ;EDX --> GCR index register
        mov     [edi].GCRReg0A,al       ;store the data for GR0A

;
; For the 6440 chipset, we also want to save GRAE, and GRB0. These are the
; color expansion control registers.
;
        test    ChipID,CL_6440
        jz      @f
        
        mov     al,0AEh                 ;set to GCR index 0AEh
        out     dx,al                   ;
        inc     edx                     ;EDX --> GCR data register
        in      al,dx                   ;get data from GCR register 0AEh
        dec     edx                     ;EDX --> GCR index register
        mov     [edi].GCRRegAE,al       ;save the data in our CB data structure

        mov     al,0B0h                 ;set to GCR index 0B0h
        out     dx,al                   ;
        inc     edx                     ;EDX --> GCR data register
        in      al,dx                   ;get data from GCR register 0B0h
        dec     edx                     ;EDX --> GCR index register
        mov     [edi].GCRRegB0,al       ;save the data in our CB data structure

@@:
;
;Restore the GCR index register saved in the high byte of EAX:
;
        rol     eax,8                   ;
        out     dx,al                   ;
;
MSRSExit_6:
        pop     edi                     ;restore saved registers
        pop     edx                     ;
        ret
EndProc MiniVDD_SaveRegisters64xx
;
;
public  MiniVDD_RestoreRegisters542x
BeginProc MiniVDD_RestoreRegisters542x, DOSVM
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
;       ESI contains the VM handle for the MemC owner VM.
;       ECX contains the VM handle for the CRTC owner VM.
;Exit:
;       You must preserve EBX and EDX.  The caller preserves everything else.
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
        push    edx                     ;save caller's EDX
        cmp     DisplayEnabledFlag,0    ;is display in HiRes mode yet?
        je      MRRSExit_5              ;nope, don't do anything yet
        unlock_extension_regs_542x
;
;Always restore CRTC register 1B, bit 1 so that we can address the full
;VRAM at all times:
;
        mov     edx,3d4h                ;EDX --> CRTC index register
        in      al,dx                   ;save value of index register
        ror     eax,8                   ;
        mov     al,1bh                  ;
        out     dx,al                   ;
        inc     edx                     ;
        in      al,dx                   ;get current state
        or      al,02h                  ;always set bit 1 on
        out     dx,al                   ;
        dec     edx                     ;
        rol     eax,8                   ;restore CRTC index register
        out     dx,al                   ;
;
;When Windows owns the CRTC, we must restore the state of the MiscOutputReg
;to the value that we saved at SaveDriverState:
;
        cmp     ecx,WindowsVMHandle     ;Windows owns the CRTC state?
        jne     @F                      ;nope, skip this
        mov     al,WindowsMiscOutputState
        or      al,al                   ;have we init'd this state yet?
        jz      @F                      ;nope! don't set a bogus value!
        mov     edx,3c2h                ;write the MiscOutput Register
        out     dx,al                   ;restore the correct state
@@:     cmp     esi,MessageModeID       ;are we to restore message mode?
        je      MRRSExit_5              ;yes! Cirrus doesn't do anything here
        cmp     esi,PlanarStateID       ;are we to restore planar state?
        je      MRRSRestorePlanarState_5
                                        ;yes, go explicitely restore it
        test    CLModeChangeFlags,GOING_TO_WINDOWS_MODE OR GOING_TO_VGA_MODE
        jnz     MRRSExit_5              ;in a mode change, don't restore now
        cmp     esi,WindowsVMHandle     ;restoring for the Windows VM?
        jne     MRRSRestoreVMState_5    ;nope, go restore from the saved state
;
public  MRRSRestoreWindowsVM_5
MRRSRestoreWindowsVM_5:
;
        add     esi,OurCBDataPointer    ;ESI --> MemC's CB data
        mov     edx,3ceh                ;EDX --> GCR index register
        in      al,dx                   ;get and save current GCR index ...
        ror     eax,8                   ;in high byte of EAX
;
;We need to restore the state of the saved GCR and Sequencer registers:
;
        mov     al,0bh                  ;set to GCR index B
        mov     ah,[esi].GCRRegB        ;get MemC owner's value for this reg
        out     dx,ax                   ;
;
        xor     al,al                   ;set to GCR index 0
        mov     ah,[esi].GCRReg0        ;get MemC owner's value for this reg
        out     dx,ax                   ;
;
        mov     al,01h                  ;set to GCR index 1
        mov     ah,[esi].GCRReg1        ;get MemC owner's value for this reg
        out     dx,ax                   ;
;
        mov     al,05h                  ;set to GCR index 5
        mov     ah,[esi].GCRReg5        ;get MemC owner's value for this reg
        out     dx,ax                   ;
;
;Restore the GCR index register saved in the high byte of EAX:
;
        rol     eax,8                   ;
        out     dx,al                   ;
;
;Now, Sequencer registers 2 & 12:
;
        mov     edx,3c4h                ;EDX --> Sequencer index register
        in      al,dx                   ;get and save current Seq index ...
        ror     eax,8                   ;in high byte of EAX
;
        mov     al,02h                  ;set to Seq index 2
        mov     ah,[esi].SeqReg2        ;get MemC owner's value for this reg
        out     dx,ax                   ;

        mov     al,12h                  ;set to Seq index 12
        mov     ah,[esi].SeqReg12       ;get MemC owner's value for this reg
        out     dx,ax                   ;

        test    ChipID,CL_7541+CL_7542+CL_7543  ; is it the NORDIC family?
        jz      @F                      ; no, don't restore SR9 and SR0A.

        mov     al,09h                  ;set to Seq index 9
        mov     ah,[esi].SeqReg9        ;get MemC owner's value for this reg
        out     dx,ax                   ;

        mov     al,0ah                  ;set to Seq index A
        mov     ah,[esi].SeqRegA        ;get MemC owner's value for this reg
        out     dx,ax                   ;
@@:
;
;Restore the Sequencer index register saved in the high byte of EAX:
;
        rol     eax,8                   ;
        out     dx,al                   ;
        jmp     MRRSExit_5              ;
;
public  MRRSRestorePlanarState_5
MRRSRestorePlanarState_5:
;
;We just need to set GCR Register 0BH to planar state:
;
        mov     edx,3ceh                ;
        in      al,dx                   ;
        ror     eax,8                   ;
        mov     al,0bh                  ;
        mov     ah,VDDBankControl       ;turn off 2 page mode ...
                                        ;but preserve 4K/16K banking value
        out     dx,ax                   ;
        rol     eax,8                   ;
        out     dx,al                   ;
        jmp     MRRSExit_5              ;
;
public  MRRSRestoreVMState_5
MRRSRestoreVMState_5:
        add     esi,OurCBDataPointer    ;ESI --> MemC's CB data
        mov     edx,3ceh                ;EDX --> GCR index register
        in      al,dx                   ;get and save current GCR index ...
        ror     eax,8                   ;in high byte of EAX
;
;We need to restore the state of GCR register B since it affects how memory
;is looked at by the CPU and may be messed with during Windows DOS mode
;changes.  If Windows owns the CRTC (ECX == WindowsVMHandle), it infers
;that we are running this VM windowed, and therefore must preserve the
;4K/16K flag in GCR register 0BH since our banking code relies on this.
;If we're running full-screen (ECX != WindowsVMHandle), then we just
;restore our saved state.
;
        mov     al,0bh                  ;set to GCR index B
        mov     ah,[esi].GCRRegB        ;get MemC owner's value for this reg
        cmp     ecx,WindowsVMHandle     ;are we running in a Windowed VM?
        jne     @F                      ;no, just restore saved state
        mov     ah,VDDBankControl       ;yes, restore saved state
@@:     out     dx,ax                   ;
;
;Restore the GCR index register saved in the high byte of EAX:
;
        rol     eax,8                   ;
        out     dx,al                   ;
; Restore the state of a couple of SR registers, too.
        mov     edx,3c4h                ;EDX --> Sequencer index register
        in      al,dx                   ;get and save current Seq index ...
        ror     eax,8                   ;in high byte of EAX
;
;Restore the Sequencer index register saved in the high byte of EAX:
;
        rol     eax,8                   ;
        out     dx,al                   ;

;
; ok, if wee are on a Nordic family chip(CL_7541+CL_7542+CL_7543), then we
; need to restore the hidden DAC area here, so the temporary variables 
; supplied to the DOS VM are correct.
;
        test    ChipID,CL_7541+CL_7542+CL_7543  ; is it the NORDIC family?
        jz      MRRSExit_5                      ; No, skip to the end
 
        mov     edx,3c4h                ;EDX --> Sequencer index register
        in      al,dx                   ;get and save current Seq index ...
        ror     eax,8                   ;in high byte of EAX
;
; enable SR12[2], which enables access to the hidden DAC area.
        mov     al,12h                  ; cursor attribute
        out     dx,al
        inc     edx                     ; EDX --> Sequencer Data Register
        in      al,dx                   ; AH has old SR12 value
        ror     eax,8                   ; save old SR12 in high byte of EAX
        or      al,2                    ; allow access to RAMDAC cursor colors
        out     dx,al
;
; now restore the hidden DAC area.
        mov     edx,3c8h                ; EDX --> DAC write index.
        add     esi,HiddenDAC           ; point to the data area
        mov     ecx,10h                 ; number of data areas
@@:        
        mov     al,[esi+0]              ;get the index
        and     al,0fh                  ; ensure its in range
        cmp     al,0Bh                  ; restore only 0B, 0C, 0D, and 0E.
        jb      MRRSNextEntry
        cmp     al,0Fh                  ; restore only 0B, 0C, 0D, and 0E.
        je      MRRSNextEntry
            
        out     dx,al                   ;set the index
        inc     edx                     ;EDX-> DAC data register
        mov     al,[esi+1]              ;get the data
        out     dx,al                   ; set it
        mov     al,[esi+2]              ;get the data
        out     dx,al                   ; set it
        mov     al,[esi+3]              ;get the data
        dec     edx                     ;EDX -> DAC write index
MRRSNextEntry:
        add     esi,4                   ; point to next entry
        loop    @B

; now restore the value of SR12
        mov     edx,3c5h                ;EDX --> Sequencer data register
        rol     eax,8                   ;
        out     dx,al                   ;
        dec     edx                     ;EDX --> Sequencer index register
;
;Restore the Sequencer index register saved in the high byte of EAX:
;
        rol     eax,8                   ;
        out     dx,al                   ;

MRRSExit_5:
        pop     edx                     ;
        ret                             ;
EndProc MiniVDD_RestoreRegisters542x
;
;
public  MiniVDD_RestoreRegisters64xx
BeginProc MiniVDD_RestoreRegisters64xx, DOSVM
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
;       ESI contains the VM handle for the MemC owner VM.
;       ECX contains the VM handle for the CRTC owner VM.
;Exit:
;       You must preserve EBX and EDX.  The caller preserves everything else.
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
        push    edx                     ;save caller's EDX
        cmp     DisplayEnabledFlag,0    ;is display in HiRes mode yet?
        je      MRRSExit_6              ;nope, don't do anything yet
;
;When Windows owns the CRTC, we must restore the state of the MiscOutputReg
;to the value that we saved at SaveDriverState:
;
        cmp     ecx,WindowsVMHandle     ;Windows owns the CRTC state?
        jne     @F                      ;nope, skip this
        mov     al,WindowsMiscOutputState
        or      al,al                   ;have we init'd this state yet?
        jz      @F                      ;nope! don't set a bogus value!
        mov     edx,3c2h                ;write the MiscOutput Register
        out     dx,al                   ;restore the correct state
@@:     cmp     esi,MessageModeID       ;are we to restore message mode?
        je      MRRSExit_6              ;yes! Cirrus doesn't do anything here
        cmp     esi,PlanarStateID       ;are we to restore planar state?
        je      MRRSRestorePlanarState_6
                                        ;yes, go explicitely restore it
        test    CLModeChangeFlags,GOING_TO_WINDOWS_MODE OR GOING_TO_VGA_MODE
        jz      MRRSRestoreVMState_6    ;not in a mode change, go restore
        jmp     MRRSExit_6              ;in a mode change, don't restore now
;
public  MRRSRestorePlanarState_6
MRRSRestorePlanarState_6:
;
;We just need to set GCR Register 0A1H to planar state:
;
        mov     edx,3ceh                ;
        in      al,dx                   ;
        ror     eax,8                   ;
;
        mov     ax,0ec0ah               ;this will unlock the extension regs
        out     dx,ax                   ;
;
        mov     al,0a1h                 ;
        out     dx,al                   ;
        inc     edx                     ;
        in      al,dx                   ;get current value
        and     al,NOT 30h              ;turn off chain4 bits
        out     dx,al                   ;
        dec     edx                     ;
;
        test    ChipID,CL_6440          ;are we a CL-6440 chip?
        jz      @F                      ; No, skip this part
;
; If we are running on the 6440, force GCR register B0H to a zero to
; turn off all color expansion features:
;
        mov     ax,00b0h                ; 0 means disabled everything
        out     dx,ax                   ;
@@:
        rol     eax,8                   ;
        out     dx,al                   ;
        jmp     MRRSExit_6              ;
;
public  MRRSRestoreVMState_6
MRRSRestoreVMState_6:
        mov     ecx,esi                 ;save MemC owner VM handle in ECX
        add     esi,OurCBDataPointer    ;ESI --> MemC's CB data
        mov     edx,GRAPHICS_INDEX      ;EDX --> 3CEH
        in      al,dx                   ;get and save current GCR index ...
        ror     eax,8                   ;in high byte of EAX
;
;Make sure the extension registers are unlocked:
;
        mov     ax,0ec0ah               ;
        out     dx,ax                   ;
;
;We need to restore the state of GCR register D since it affects how memory
;is looked at by the CPU and may be messed with during Windows DOS mode
;changes:
;
        mov     al,0dh                  ;set to GCR index D
        mov     ah,[esi].GCRRegD        ;get MemC owner's value for this reg
        cmp     ecx,WindowsVMHandle     ;restoring for the Windows VM?
        je      @F                      ;yes! just restore saved state
        or      ah,03h                  ;no, better make sure banking is on
@@:     out     dx,ax                   ;
;
;Restore the state of GCR registers A1H since this must track the state
;of Sequencer Register 4's Chain4 bit:
;
        mov     al,0a1h                 ;
        mov     ah,[esi].GCRRegA1       ;
        out     dx,ax                   ;
;
; If we are running on the 6440, restore the state of GCR registers 
; AEH and B0 since they control the special write modes(4 and 5) that 
; do color expansion.
;
        test    ChipID,CL_6440          ;are we a CL-6440 chip?
        jz      @F                      ; No, skip this part
        mov     al,0b0h                 ; we must do B0 first to enable
        mov     ah,[esi].GCRRegB0       ; then write to AE
        out     dx,ax                   ;
        cmp     ecx,WindowsVMHandle     ; is the MemC owner the Windows VM?
        jne     @F                      ; nope, don't restore AEH
        mov     al,0aeh                 ; we must do B0 first to enable
        mov     ah,[esi].GCRRegAE       ; 
        out     dx,ax                   ;

@@:
;
;Restore the GCR index register saved in the high byte of EAX:
;
        rol     eax,8                   ;
        out     dx,al                   ;
;
MRRSExit_6:
        pop     edx                     ;
        ret                             ;
EndProc MiniVDD_RestoreRegisters64xx
;
;
subttl          Set Hardware to a Not Busy State
page +
public  MiniVDD_MakeHardwareNotBusy5426
BeginProc MiniVDD_MakeHardwareNotBusy5426, DOSVM
;
;Quite often, we need to make sure that the hardware state is not busy
;before changing the MemC mode from the Windows HiRes state to VGA state
;or vice-versa.  This routine allows you to do this (to the best of your
;ability).  You should try to return a state where the hardware BLTer
;isn't busy.
;
;Entry:
;       EAX contains the CRTC owner's VM handle.
;       EBX contains the currently running VM handle.
;       ECX contains the MemC owner's VM handle.
;       EDX contains the CRTC index register.
;Exit:
;       You must save all registers that you destroy except for EAX & ECX.
;
        push    edx                     ;save registers that we use for test
        cmp     eax,WindowsVMHandle     ;is CRTC VM Windows?
        jne     MHNBBoardNotBusy        ;nope, skip this entire mess!
;
        unlock_extension_regs_542x
        mov     edx,GRAPHICS_INDEX      ;EDX -> graphics index
;
;Now, perform the actual test for board-not-busy:
;
        mov     ecx,1000h               ;this is a safety net loop counter
        mov     al,BLT_STATUS_REGISTER
        out     dx,al
        inc     dx
;
MHNBBoardBusyCheck:
        in      al,dx                   ;
        test    al,BLT_BUSY_BIT         ;is hardware busy?
        jz      MHNBBoardNotBusy        ;nope, continue
        loopd   MHNBBoardBusyCheck      ;wait for it to finish
        int     3                       ; for debug
;
public  MHNBBoardNotBusy
MHNBBoardNotBusy:
        pop     edx                     ;restore CRTC port number
;
MHNBExit:
        ret                             ;
EndProc MiniVDD_MakeHardwareNotBusy5426
;
;
subttl          Display Driver Is Being Disabled Notification
page +
public  MiniVDD_DisplayDriverDisabling
BeginProc       MiniVDD_DisplayDriverDisabling, RARE
;
;The display driver is in its Disable routine and is about to set the
;hardware back into VGA text mode.  Since this could either mean that
;the Windows session is ending or that some Windows application is switching
;to a VGA mode to display something full screen (such as MediaPlayer), we
;need to disable our MiniVDD_RestoreRegisters code because we're liable
;to restore a Windows HiRes state when we shouldn't!  Thus, clear the
;DisplayEnabledFlag to prevent this:
;
        mov     DisplayEnabledFlag,0    ;don't do a RestoreRegState
;
MDDDExit:
        ret                             ;
EndProc MiniVDD_DisplayDriverDisabling
;
;
subttl          Virtualize Sequencer Register Extensions
page +
public  MiniVDD_VirtSeqOut54xx
BeginProc MiniVDD_VirtSeqOut54xx, DOSVM
;
;This routine is called when the Sequencer or data register (3C5H) is trapped
;when a VM is being virtualized.  It gives you the opportunity to special
;case virtualization of the Sequencer registers for your particular
;device's needs.
;
;Entry:
;       AL contains the byte to output to port.
;       EBX contains the VM Handle for this Sequencer output.
;       ECX contains the Sequencer index of the port to virtualize.
;       EDX contains the physical register to write or virtualize.
;Exit:
;       AL contains the byte to output to port.
;       Set CY to tell caller that WE handled this Sequencer output.
;       Set NC to tell caller (Main VDD) to handle this Sequencer output
;               in whatever way it normally does
;       EBX, ECX, and EDX should be preserved.
;
        cmp     cl,12h                  ;is it Sequencer register 12?
        jne     @F                      ;nope, try another

        VxDCall VDD_Get_VM_Info         ;get MemC owner VM handle in ESI
;
;Save the new state of SR12 here.
;
        add     esi,OurCBDataPointer    ;
        mov     [esi].SeqReg12,al       ;save this state
        jmp     VSOExit_5
@@:
        test    ChipID,CL_7541+CL_7542+CL_7543  ; is it the NORDIC family?
        jz      VSOExit_5               ; only virtualize the rest for NORDIC!

        cmp     cl,09h                  ;is it Sequencer register 9?
        jne     @F                      ;nope, try another

        VxDCall VDD_Get_VM_Info         ;get MemC owner VM handle in ESI
;
;Save the new state of SR9 here.
;
        add     esi,OurCBDataPointer    ;
        mov     [esi].SeqReg9,al       ;save this state
        stc
        jmp     VSOExit2_5
@@:
        cmp     cl,0Ah                  ;is it Sequencer register 0A?
        jne     @F                      ;nope, try another

        VxDCall VDD_Get_VM_Info         ;get MemC owner VM handle in ESI
;
;Save the new state of SR0A here.
;
        add     esi,OurCBDataPointer    ;
        mov     [esi].SeqRegA,al       ;save this state
        stc
        jmp     VSOExit2_5
@@:
;
VSOExit_5:
        clc                             ;tell Main VDD to handle these normally
VSOExit2_5:
        ret                             ;
EndProc MiniVDD_VirtSeqOut54xx

page +
public  MiniVDD_VirtSeqIn54xx
BeginProc MiniVDD_VirtSeqIn54xx, DOSVM
;
;This routine is called when the Sequencer or data register (3C5H) is trapped
;when a VM is being virtualized.  It gives you the opportunity to special
;case virtualization of the Sequencer registers for your particular
;device's needs.
;
;Entry:
;       EBX contains the VM Handle for this Sequencer input.
;       ECX contains the Sequencer index of the port to virtualize.
;       EDX contains the physical register to write or virtualize.
;Exit:
;       Set CY to tell caller that we virtualized the port and AL has the value.
;       EBX, ECX, and EDX should be preserved.
;
        cmp     cl,12h                  ;is it Sequencer register 12?
        jne     @F                      ;nope, just leave
        VxDCall VDD_Get_VM_Info         ;get MemC owner VM handle in ESI
;
;Save the new state of SR12 here.
;
        add     esi,OurCBDataPointer    ;point to our data area.
        mov     al,[esi].SeqReg12       ;get this state
        stc                             ; we did the virtualization        
        jmp     VSIExit_5
@@:
        test    ChipID,CL_7541+CL_7542+CL_7543  ; is it the NORDIC family?
        jz      VSIExit2_5              ; only virtualize the rest for NORDIC!

        cmp     cl,9h                   ;is it Sequencer register 9?
        jne     @F                      ;nope, just leave
        VxDCall VDD_Get_VM_Info         ;get MemC owner VM handle in ESI
;
;Get the state of SR09 here.
;
        add     esi,OurCBDataPointer    ;point to our data area.
        mov     al,[esi].SeqReg9        ;get this state
        stc                             ; we did the virtualization        
        jmp     VSIExit_5
@@:
        cmp     cl,0Ah                  ;is it Sequencer register 0A?
        jne     VSIExit2_5              ;nope, just leave
        VxDCall VDD_Get_VM_Info         ;get MemC owner VM handle in ESI
;
;Get the state of SR0A here.
;
        add     esi,OurCBDataPointer    ;point to our data area.
        mov     al,[esi].SeqRegA        ;get this state
        stc                             ; we did the virtualization        
        jmp     VSIExit_5
VSIExit2_5:
        clc                             ; we did not do virtualization
VSIExit_5:
        ret                             ;
EndProc MiniVDD_VirtSeqIn54xx

subttl          Virtualize CRTC Register Extensions
page +
public  MiniVDD_VirtCRTCOut54xx
BeginProc MiniVDD_VirtCRTCOut54xx, DOSVM
;
;This routine is called when the CRTC or data register (3C5H) is trapped
;when a VM is being virtualized.  It gives you the opportunity to special
;case virtualization of the CRTC registers for your particular
;device's needs.
;
;Entry:
;       AL contains the byte to output to port.
;       EBX contains the VM Handle for this CRTC output.
;       ECX contains the CRTC index of the port to virtualize.
;       EDX contains the physical register to write or virtualize.
;Exit:
;       AL contains the byte to output to port.
;       Set CY to tell caller that WE handled this CRTC output.
;       Set NC to tell caller (Main VDD) to handle this CRTC output
;               in whatever way it normally does
;       EBX, ECX, and EDX should be preserved.
;
        cmp     cl,19h                  ; CRTC extension register?
        jb      VCOExit_5               ; No, do normal processing

        cmp     cl,1dh                  ; Shadow control register?
        jnz     @F                      ; no, skip to extended reg. processing
        push    ebx                     ; yes, save EBX for later
        add     ebx,OurCBDataPointer    ;EBX --> VM's CB data area
        mov     [ebx].CRTCReg1D,al      ;save current value over save/restore
        pop     ebx                     ; restore EBX
        jmp     VCO_SpecialExit_5       ; done with CR1D work
@@:
        cmp     cl,1Bh                  ; protect the extended memory wrap bit
        jnz     @F                      ; not extended memory wrap register
        or      al,2                    ; keep this enabled always
@@:
        cmp     cl,1Eh                  ; CR1E is the flat panel shading(reverse video)
        jz      VCO_SpecialExit_5       ; dont output this register
;
; all other extended registers come here
;
        dec     edx                     ; EDX --> CRTC index register
        mov     ah,al                   ; data to output to AH
        mov     al,cl                   ; index to AL for output
        out     dx,ax                   ; output the data
        inc     edx                     ; EDX --> CRTC data register
VCO_SpecialExit_5:
        stc                             ; say we did it, so the value's not stored
        ret                             ; done!
VCOExit_5:
        push    ebx                     ; save EBX for later
        add     ebx,OurCBDataPointer    ;EBX --> VM's CB data area
        test    [ebx].CRTCReg1D,80h     ; CR1D shadow bit set?
        pop     ebx                     ; restore EBX(does not hose flags)
        jnz     VCO_SpecialExit_5       ; shadow bit set, exit w/o I/O
        clc                             ;tell caller we didn't do output
        ret                             ;
EndProc MiniVDD_VirtCRTCOut54xx

page +
public  MiniVDD_VirtCRTCIn54xx
BeginProc MiniVDD_VirtCRTCIn54xx, DOSVM
;
;This routine is called when the CRTC or data register (3C5H) is trapped
;when a VM is being virtualized.  It gives you the opportunity to special
;case virtualization of the CRTC registers for your particular
;device's needs.
;
;Entry:
;       EBX contains the VM Handle for this CRTC input.
;       ECX contains the CRTC index of the port to virtualize.
;       EDX contains the physical register to write or virtualize.
;Exit:
;       Set CY to tell caller that we virtualized the port and AL has the value.
;       EBX, ECX, and EDX should be preserved.
;
        cmp     cl,19h
        jb      VCI_Exit_5

        cmp     cl,1dh
        jnz     @F
        push    ebx
        add     ebx,OurCBDataPointer    ;EBX --> VM's CB data area
        mov     al,[ebx].CRTCReg1D      ;get current value over save/restore
        pop     ebx
        jmp     VCI_SpecialExit_5
@@:
        dec     edx                     ; to index
        in      al,dx
        ror     eax,8                   ; save index

        mov     al,cl                   ; index to AL for output
        out     dx,al                   ; set the index
        inc     edx                     ; EDX --> CRTC data register
        in      al,dx                   ; get the data register
        dec     edx                     ; EDX --> CRTC index register

        rol     eax,8                   ; this puts the result in AH, AL=index
        out     dx,al                   ; restore index
        inc     edx                     ; EDX --> graphics data register
        mov     al,ah                   ; result to AL
VCI_SpecialExit_5:
        stc                             ; tell caller WE did the input
        ret
VCI_Exit_5:
        clc
        ret                             ;
EndProc MiniVDD_VirtCRTCIn54xx


public  MiniVDD_VirtCRTCOut754x
BeginProc MiniVDD_VirtCRTCOut754x, DOSVM
;
;This routine is called when the CRTC or data register (3C5H) is trapped
;when a VM is being virtualized.  It gives you the opportunity to special
;case virtualization of the CRTC registers for your particular
;device's needs.
;
;Entry:
;       AL contains the byte to output to port.
;       EBX contains the VM Handle for this CRTC output.
;       ECX contains the CRTC index of the port to virtualize.
;       EDX contains the physical register to write or virtualize.
;Exit:
;       AL contains the byte to output to port.
;       Set CY to tell caller that WE handled this CRTC output.
;       Set NC to tell caller (Main VDD) to handle this CRTC output
;               in whatever way it normally does
;       EBX, ECX, and EDX should be preserved.
;
        cmp     cl,7                    ; look at CR0 to CR7
        ja      @F
        push    ebx                     ; save EBX for later
        add     ebx,OurCBDataPointer    ;EBX --> VM's CB data area
        test    [ebx].CRTCReg11,80h     ; CR11 saved here
        pop     ebx                     ; restore EBX(does not hose flags)
        jz      VCO_Exit_7              ; we didnt do the output
        jmp     VCO_Exit_7b             ; we did do the output(no saving of value)
@@:
        cmp     cl,11h                  ; Q: is output CR11?
        jnz     @F
        push    ebx                     ; save EBX for later
        add     ebx,OurCBDataPointer    ;EBX --> VM's CB data area
        mov     [ebx].CRTCReg11,al      ; CR11 saved here
        pop     ebx                     ; restore EBX(does not hose flags)
@@:        
VCO_Exit_7:
        clc                             ; tell caller we didn't do the output
        ret                             ;
VCO_Exit_7b:
        stc                             ; tell caller we did do the output
        ret                             ;

EndProc MiniVDD_VirtCRTCOut754x

subttl          Virtualize RAMDAC Register Extensions
page +
public  MiniVDD_VirtDACOut54xx
BeginProc MiniVDD_VirtDACOut54xx, DOSVM
;
;This routine is called when the RAMDAC is trapped when a VM is being 
;virtualized.  It gives you the opportunity to special
;case virtualization of the RAMDAC for your particular device's needs.
;
;Entry:
;       AL contains the byte to output to port.
;       EBX contains the VM Handle for this RAMDAC output.
;       ECX contains the RAMDAC index of the port to virtualize.
;       EDX contains the physical register to write or virtualize.
;Exit:
;       AL contains the byte to output to port.
;       Set CY to tell caller that WE DID virtualize this RAMDAC output.
;       EBX, ECX, EDX, and ESI should be preserved.
;
        cmp     edx,3C6h                ; RAMDAC Pixel Mask?
        jnz     VDOCheck_3C7            ; no, check the other index values
VDOIs_3C6:
        clc                             ; tell caller to handle it
        jmp     VDOExit_5               ; then exit.
VDOCheck_3C7:
        cmp     edx,3C7h                ; DAC read index?
        jnz     VDOCheck_3C8            ; no, check the other index values
VDOIs_3C7:
        push    esi
        VxDCall VDD_Get_VM_Info         ;get MemC owner VM handle in ESI
        add     esi,OurCBDataPointer    ;point to our data area.
        mov     [esi].DACIndex,al
        mov     [esi].DACPixelIndex,1
        mov     [esi].DACPixelMode,READ_MODE
        pop     esi
        clc                             ; tell caller to handle it normally
        jmp     VDOExit_5               ; then exit.

VDOCheck_3C8:
        cmp     edx,3C8h                ; DAC write index?
        jnz     VDOIs_3C9               ; no, check the other index values
VDOIs_3C8:
        push    esi
        VxDCall VDD_Get_VM_Info         ;get MemC owner VM handle in ESI
        add     esi,OurCBDataPointer    ;point to our data area.
        mov     [esi].DACIndex,al
        mov     [esi].DACPixelIndex,1
        mov     [esi].DACPixelMode,WRITE_MODE
        pop     esi
        clc                             ; tell caller to handle it
        jmp     VDOExit_5               ; then exit.

VDOIs_3C9:
        push    esi
        VxDCall VDD_Get_VM_Info         ;get MemC owner VM handle in ESI
        add     esi,OurCBDataPointer    ;point to our data area.
        test    [esi].SeqReg12,2        ; is the hidden DAC area enabled?
        jnz     VDO_Is_3C9_HiddenEnabled; yes, 
        pop     esi
        clc                             ; let caller handle normally
        jmp     VDOExit_5
VDO_Is_3C9_HiddenEnabled:
; the hidden DAC area is enabled, so we need to save the output value
; in our data area.
        push    ebx                     ; save VM handle
        movzx   ebx,[esi].DACIndex
        and     ebx,0fh                 ; 0 to 0Fh only!
        shl     ebx,2                   ; times 4 for DAC save size
        add     ebx,[esi].DACPixelIndex
        mov     [esi][ebx].HiddenDAC,al ; save the hidden DAC value
        mov     ebx,[esi].DACPixelIndex ; get the pixel index
        cmp     ebx,3                   ; value less than three
        jb      @F                      ; yes, 
        xor     ebx,ebx                 ; its 3 or greater, so zero it
        inc     [esi].DACIndex          ; and increment the RAMDAC index
                                        ; byte value will wrap FF->0
@@:                                     ;
        inc     ebx                     ; then inc to 1,2 or 3.
        mov     [esi].DACPixelIndex,ebx ;
        pop     ebx                     ; restore VM handle
        pop     esi
        stc                             ; CY set to say we did virtualization
VDOExit_5:
        ret                             ;
EndProc MiniVDD_VirtDACOut54xx

page +
public  MiniVDD_VirtDACIn54xx
BeginProc MiniVDD_VirtDACIn54xx, DOSVM
;
;This routine is called when the RAMDAC is trapped when a VM is being 
;virtualized.  It gives you the opportunity to special
;case virtualization of the RAMDAC registers for your particular
;device's needs.
;
;Entry:
;       EBX contains the VM Handle for this RAMDAC input.
;       ECX contains the RAMDAC index of the port to virtualize.
;       EDX contains the physical register to write or virtualize.
;Exit:
;       Set CY to tell caller that we virtualized the port and AL has the value.
;       EBX, ECX, and EDX should be preserved.
;
        cmp     edx,3C6h                ; RAMDAC Pixel Mask?
        jnz     VDICheck_3C7            ; no, check the other index values
VDIIs_3C6:
        clc                             ; tell caller to handle it
        jmp     VDIExit_5               ; then exit.
VDICheck_3C7:
        cmp     edx,3C7h                ; DAC read index?
        jnz     VDICheck_3C8            ; no, check the other index values
VDIIs_3C7:
        push    esi
        VxDCall VDD_Get_VM_Info         ;get MemC owner VM handle in ESI
        add     esi,OurCBDataPointer    ;point to our data area.
        mov     al,[esi].DACPixelMode   ; return value to al
        pop     esi
        stc                             ; tell caller we handled it
        jmp     VDIExit_5               ; then exit.
VDICheck_3C8:
        cmp     edx,3C8h                ; DAC write index?
        jnz     VDIIs_3C9               ; no, check the other index values
VDIIs_3C8:
        push    esi
        VxDCall VDD_Get_VM_Info         ;get MemC owner VM handle in ESI
        add     esi,OurCBDataPointer    ;point to our data area.
        test    [esi].SeqReg12,2        ; is the hidden DAC area enabled?
        jnz     VDIIs_3C8_2             ; yes, return our value, not regular one
        pop     esi
        clc                             ; tell caller to handle it
        jmp      VDIExit_5              ; no, do normal case
VDIIs_3C8_2:
        mov     al,[esi].DACIndex
        pop     esi
        stc                             ; tell caller we handled it
        jmp      VDIExit_5              ; no, do normal case
VDIIs_3C9:
        push    esi
        VxDCall VDD_Get_VM_Info         ;get MemC owner VM handle in ESI
;
        add     esi,OurCBDataPointer    ;point to our data area.
        test    [esi].SeqReg12,2        ; is the hidden DAC area enabled?
        jnz     VDI_Is_3C9_HiddenEnabled; yes, return hidden values
        pop     esi
        clc                             ; tell caller to handle it
        jmp      VDIExit_5              ; no, do normal case
VDI_Is_3C9_HiddenEnabled:
; the hidden DAC area is enabled, so we need to get the input value
; from our data area.
        push    ebx                     ; save VM handle
        movzx   ebx,[esi].DACIndex
        and     ebx,0fh                 ; 0 to 0Fh only!
        shl     ebx,2                   ; times 4 for DAC save size
        add     ebx,[esi].DACPixelIndex
        mov     al,[esi][ebx].HiddenDAC ; save the hidden DAC value
        mov     ebx,[esi].DACPixelIndex ; get the pixel index
        cmp     ebx,3                   ; value less than three
        jb      @F                      ; yes, 
        xor     ebx,ebx                 ; its 3 or greater, so zero it
        inc     [esi].DACIndex          ; and increment the RAMDAC index
                                        ; byte value will wrap FF->0
@@:                                     ;
        inc     ebx                     ; then inc to 1,2 or 3.
        mov     [esi].DACPixelIndex,ebx ;
        pop     ebx                     ; restore VM handle
        pop     esi
        stc                             ; we did the virtualization
VDIExit_5:
        ret                             ;
EndProc MiniVDD_VirtDACIn54xx
;
;
subttl          Virtualize Graphics Controller Register Extensions
page +
public  MiniVDD_VirtGRIn64xx
BeginProc MiniVDD_VirtGRIn64xx, DOSVM
;
;This routine is called when the Graphics Controller data register (3CFH) is trapped
;when a VM is being virtualized.  It gives you the opportunity to special
;case virtualization of the Graphics Controller registers for your particular
;device's needs.
;
;Entry:
;       EBX contains the VM Handle for this Graphics Controller output.
;       ECX contains the Graphics Controller index of the port to virtualize.
;       EDX contains the physical register to write or virtualize.
;Exit:
;       Set CY to tell caller that we virtualized the port and AL has the value.
;       Set NC to tell caller (Main VDD) to handle this input in whatever 
;       way it normally does.
;       EBX, ECX, EDX and ESI should be preserved.
;
        cmp     cl,0Ah                  ; is it the extension control reg?
        jz      VGI6_DoExtensionControl ; yes, get answer from CBData
        cmp     cl,30h                  ; is it extension register?
        ja      VGI6_DoExtension        ; yes, do actual output to get result
        clc                             ; tell caller we didn't do input
        jmp     VGI6Exit                ; exit vector
VGI6_DoExtension:
;
        dec     edx                     ; to index
        in      al,dx
        ror     eax,8                   ; save index

        mov     al,cl                   ; index to AL for output
        out     dx,al                   ; set the index
        inc     edx                     ; EDX --> graphics data register
        in      al,dx                   ; get the data register
        dec     edx                     ; EDX --> graphics index register

        rol     eax,8                   ; this puts the result in AH, AL=index
        out     dx,al                   ; restore index
        inc     edx                     ; EDX --> graphics data register
        mov     al,ah                   ; result to AL
        stc                             ; tell caller WE did the input
        jmp     VGI6Exit                ; exit vector

VGI6_DoExtensionControl:
        push    esi
        VxDCall VDD_Get_VM_Info         ; get MemC owner VM handle in ESI
        add     esi,OurCBDataPointer    ; point to our data area.
        mov     al,[esi].GCRReg0A       
        and     al,1                    ; safety first!
        pop     esi
        stc
VGI6Exit:
        ret                             ;

EndProc MiniVDD_VirtGRIn64xx

public  MiniVDD_VirtGROut64xx
BeginProc MiniVDD_VirtGROut64xx, DOSVM
;
;This routine is called when the Graphics Controller data register (3CFH) is trapped
;when a VM is being virtualized.  It gives you the opportunity to special
;case virtualization of the Graphics Controller registers for your particular
;device's needs.
;
;Entry:
;       AL contains the byte to output to port.
;       EBX contains the VM Handle for this Graphics Controller output.
;       ECX contains the Graphics Controller index of the port to virtualize.
;       EDX contains the physical register to write or virtualize.
;Exit:
;       AL contains the byte to output to port.
;       Set CY to tell caller that WE handled this Graphics Controller output.
;       Set NC to tell caller (Main VDD) to handle this Graphics Controller output
;               in whatever way it normally does.
;       EBX, ECX, EDX and ESI should be preserved.
;
        cmp     cl,0b0h                 ; is he doing an OUT to register B0H?
        je      VGO6VirtualizeB0H       ; yes, go handle it
        cmp     cl,0Ah                  ; is it the extension control reg?
        jne     VGO6LetCallerHandle     ; nope, we don't care about it
;
public  VGO6DoExtensionControl
VGO6DoExtensionControl:
;
; if the value is EC, put a 1 in GCRReg0A to read back. if the value is
; CE, put a zero there. If neither EC or CE, ignore the write.
;
        mov     edi,ebx                 ; make EDI --> VM's CB data area
        add     edi,OurCBDataPointer    ;
        cmp     al,0ECh                 ; EC is the 'enable' command
        jnz     @F                      ;
        mov     [edi].GCRReg0A,1        ; will read back a 1 if enabled
@@:                                     ;
        cmp     al,0CEh                 ; CE is the 'disable' command
        jnz     VGO6WeHandledIt         ;
        mov     [edi].GCRReg0A,0        ; will read back a 0 if disabled
;
public  VGO6WeHandledIt
VGO6WeHandledIt:
        stc                             ; tell caller we DID do the output
        jmp     VGO6Exit                ;
;
public  VGO6VirtualizeB0H
VGO6VirtualizeB0H:
;
;We simply collect the state of this register.  If we're running on the 6440
;chipset, we'll restore it when appropriate:
;
        mov     edi,ebx                 ; make EDI --> Our CB data area
        add     edi,OurCBDataPointer    ;
        mov     [edi].GCRRegB0,al       ; save what he's writing
;
public  VGO6LetCallerHandle
VGO6LetCallerHandle:
        clc                             ; and let caller handle the rest of it
;
VGO6Exit:
        ret
EndProc MiniVDD_VirtGROut64xx
;
;
subttl          Return Total Memory Size on Card
page +
public  MiniVDD_GetTotalVRAMSize_5
BeginProc MiniVDD_GetTotalVRAMSize_5, DOSVM
;
;Entry:
;       EBX contains the Current VM Handle (which is also the CRTC owner's
;       VM Handle).
;       EBP --> VM's Client Registers.
;Exit:
;       CY is returned upon success.
;       All registers (except ECX) must be preserved over the call.
;       ECX will contain the total VRAM size in bytes.
;
        mov     ecx,TotalMemorySize     ;
        stc                             ;
        ret                             ;return to caller
EndProc MiniVDD_GetTotalVRAMSize_5
;
;
subttl          Return The Current Write Bank
page +
public  MiniVDD_GetCurrentBankWrite_5
BeginProc MiniVDD_GetCurrentBankWrite_5, DOSVM
;
;Entry:
;       EBX contains the VM Handle (Always the "CurrentVM").
;       EBP --> Client Register structure for VM
;Exit:
;       CY is returned upon success.
;       All registers (except EDX & EAX) must be preserved over the call.
;       EDX will contain the current write bank (bank A) set in hardware.
;
;NOTE: We really don't care whether these banks are configured as write or
;      read banks.  The reason that we use this terminology is because the
;      VESA standard defines two "windows" that can be set separately.
;      This routine gives back the first window.
;
        mov     edx,3ceh                ;EDX --> GCR index register
        in      al,dx                   ;get current index register value
        ror     eax,8                   ;save it in top byte of EAX
        mov     al,09h                  ;this is index for bank register A
        out     dx,al                   ;
        inc     dl                      ;EDX --> GCR data register
        in      al,dx                   ;
        dec     dl                      ;EDX --> GCR index register
        rol     eax,8                   ;restore previous contents of GCR index
        out     dx,al                   ;
        movzx   edx,ah                  ;return bank in EDX register
;
MGBWExit_5:
        stc                             ;return success to caller
        ret                             ;
EndProc MiniVDD_GetCurrentBankWrite_5
;
;
subttl          Return The Current Read Bank
page +
public  MiniVDD_GetCurrentBankRead_5
BeginProc MiniVDD_GetCurrentBankRead_5, DOSVM
;
;Entry:
;       EBX contains the VM Handle (Always the "CurrentVM").
;       EBP --> Client Register structure for VM
;Exit:
;       CY is returned upon success.
;       All registers (except EDX & EAX) must be preserved over the call.
;       EDX will contain the current read bank (bank A) set in hardware.
;
;NOTE: We really don't care whether these banks are configured as write or
;      read banks.  The reason that we use this terminology is because the
;      VESA standard defines two "windows" that can be set separately.
;      This routine gives back the second window.
;
        mov     edx,3ceh                ;EDX --> GCR index register
        in      al,dx                   ;get current index register value
        ror     eax,8                   ;save it in top byte of EAX
        mov     al,0bh                  ;set to extensions register index
        out     dx,al                   ;
        inc     dl                      ;EDX --> GCR data register
        in      al,dx                   ;get contents of GCR register 0BH
        mov     ah,al                   ;save it in AH
        dec     dl                      ;EDX --> GCR index register
        mov     al,0ah                  ;(assume we are using GCR A)
        test    ah,01h                  ;using GCR register A as a bank reg?
        jnz     @F                      ;yes! go read register A
        mov     al,09h                  ;hey! we're using GCR register 9
@@:     out     dx,al                   ;
        inc     dl                      ;EDX --> GCR data register
        in      al,dx                   ;get banking register
        dec     dl                      ;EDX --> GCR index register
        rol     eax,8                   ;restore previous contents of GCR index
        out     dx,al                   ;
        movzx   edx,ah                  ;return bank value in EDX
;
MGBRExit_5:
        stc                             ;return success to caller
        ret                             ;
EndProc MiniVDD_GetCurrentBankRead_5
;
;
subttl          Return the Bank Size
page +
public  MiniVDD_GetBankSize_5
BeginProc MiniVDD_GetBankSize_5, DOSVM
;
;Entry:
;       EBX contains the VM Handle (Always the "CurrentVM").
;       ECX contains the BIOS mode number that we're currently in.
;       EBP --> Client Register structure for VM
;Exit:
;       CY is returned upon success.
;       All registers (except EDX & EAX) must be preserved over the call.
;       EDX will contain the current bank size.
;       EAX will contain the physical address of the memory aperture or
;               zero to indicate VRAM at A000H.
;
;Our bank size (for HiRes save/restore purposes) is 16K if we have more than
;1 meg of VRAM on board, or 4K if we have less than 1 meg of VRAM on board.
;
        mov     edx,4*1024              ;assume 4K banks
        cmp     TotalMemorySize,1024*1024
                                        ;more than 1 meg on board?
        jbe     MGBSExit_5              ;nope, value in EDX is correct
        shl     edx,2                   ;we have 16K banks
;
MGBSExit_5:
        xor     eax,eax                 ;indicate VRAM is at A000H
        stc                             ;return success to caller
        ret                             ;
EndProc MiniVDD_GetBankSize_5
;
;
subttl          Prepare For a HiRes Screen Save/Restore
page +
public  MiniVDD_PreHiResSaveRestore_5
BeginProc MiniVDD_PreHiResSaveRestore_5, DOSVM
;
;Entry:
;       EBX contains the VM Handle (Always the "CurrentVM").
;       ECX contains the BIOS mode number that we're currently in.
;       EBP --> Client Register structure for VM
;Exit:
;       CY is returned upon success.
;       All registers must be preserved over the call.
;
;Our bank size (for HiRes save/restore purposes) is 16K if we have more than
;1 meg of VRAM on board, or 4K if we have less than 1 meg of VRAM on board.
;If we have more than 1 megabyte of VRAM, force the bank size to 16K.
;
        push    edx                     ;save registers that we use
        push    eax                     ;
        push    ebx                     ;
        add     ebx,OurCBDataPointer    ;EBX --> VM's CB data area
        mov     edx,3ceh                ;EDX --> GCR index register
        in      al,dx                   ;get current index register value
        ror     eax,8                   ;save it in top byte of EAX
        mov     al,0bh                  ;set to GCR register 0BH
        out     dx,al                   ;
        inc     dl                      ;EDX --> GCR data register
        in      al,dx                   ;get current value of GCR register 0BH
        mov     [ebx].GCRRegB,al        ;save current value over save/restore
        xor     al,al                   ;assume < 1 meg on board
        cmp     TotalMemorySize,1024*1024
                                        ;more than 1 meg on board?
        jbe     @F                      ;nope, set GCR register B to a 0
        mov     al,20h                  ;set bit 5 indicating 16K granularity
@@:     out     dx,al                   ;set GCR register B
        rol     eax,8                   ;restore previous contents of GCR index
        dec     dl                      ;EDX --> GCR index register
        out     dx,al                   ;
        pop     ebx                     ;restore saved registers
        pop     eax                     ;
        pop     edx                     ;
        stc                             ;return success to caller
        ret                             ;
EndProc MiniVDD_PreHiResSaveRestore_5
;
;
subttl          Clean Up After a HiRes Screen Save/Restore
page +
public  MiniVDD_PostHiResSaveRestore_5
BeginProc MiniVDD_PostHiResSaveRestore_5, DOSVM
;
;Entry:
;       EBX contains the VM Handle (Always the "CurrentVM").
;       ECX contains the BIOS mode number that we're currently in.
;       EBP --> Client Register structure for VM
;Exit:
;       CY is returned upon success.
;       All registers must be preserved over the call.
;
;Our bank size (for HiRes save/restore purposes) is 16K if we have more than
;1 meg of VRAM on board, or 4K if we have less than 1 meg of VRAM on board.
;If we have more than 1 megabyte of VRAM, force the bank size to 16K.
;
        push    edx                     ;save registers that we use
        push    eax                     ;
        push    ebx                     ;
        add     ebx,OurCBDataPointer    ;EBX --> VM's CB data area
        mov     edx,3ceh                ;EDX --> GCR index register
        in      al,dx                   ;get current index register value
        ror     eax,8                   ;save it in top byte of EAX
        mov     al,0bh                  ;set to GCR register 0BH
        mov     ah,[ebx].GCRRegB        ;restore what the software wants
        out     dx,ax                   ;
        rol     eax,8                   ;restore previous contents of GCR index
        out     dx,al                   ;
        pop     ebx                     ;restore saved registers
        pop     eax                     ;
        pop     edx                     ;
        stc                             ;return success to caller
        ret                             ;
EndProc MiniVDD_PostHiResSaveRestore_5
;
;
subttl          Set the Bank During a HiRes Save/Restore
page +
public  MiniVDD_SetBank_5
BeginProc MiniVDD_SetBank_5, DOSVM
;
;Entry:
;       EAX contains the read bank to set.
;       EDX contains the write bank to set.
;       EBX contains the VM Handle (Always the "CurrentVM").
;       EBP --> Client Register structure for VM
;Exit:
;       CY is returned upon success.
;       All registers must be preserved over the call.
;       EDX will contain the current write bank (bank A) set in hardware.
;
        push    ecx                     ;save this over call
        push    edx                     ;
        push    eax                     ;
        mov     ecx,eax                 ;get read bank to set into ECX
        mov     ah,dl                   ;get write bank value in AH
        mov     edx,3ceh                ;EDX --> GCR index register
        in      al,dx                   ;get current index register value
        ror     eax,8                   ;save it in top byte of EAX
;
        mov     ah,al                   ;get write bank value into AH
        mov     al,09h                  ;set GCR index to register 09H
        out     dx,ax                   ;set the write bank
comment |
;
        mov     al,0bh                  ;we need to read GCR 0BH
        out     dx,al                   ;
        inc     dl                      ;EDX --> GCR data register
        in      al,dx                   ;
        dec     dl                      ;EDX --> GCR index register
        test    al,01h                  ;do we have to set read bank separately?
        jz      MSVDoneSettingBank_5    ;nope, we're done setting the bank
;
        mov     al,0ah                  ;set GCR index to register 0AH
        mov     ah,cl                   ;set the read bank
        out     dx,ax                   ;
end comment |
;
public  MSVDoneSettingBank_5
MSVDoneSettingBank_5:
        rol     eax,8                   ;restore GCR index register
        out     dx,al                   ;
        pop     eax                     ;
        pop     edx                     ;
        pop     ecx                     ;restore saved ECX
        stc                             ;return success to caller
        ret                             ;
EndProc MiniVDD_SetBank_5
;
;
VxD_LOCKED_CODE_ENDS
;
;
end
