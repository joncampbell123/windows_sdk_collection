        page    , 132
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   vsbd.asm
;
;   Copyright (c) 1991-1992 Microsoft Corporation.  All rights reserved.
;
;   Description:
;       This VxD handles contention for a Sound Blaster card between VMs.
;       Note this is NOT a full virtualizing driver.
;
;   Notes:
;       This VxD requires VADLIBD.386 to handle virtualization of the
;       Ad Lib compatible FM Synth chip on the Sound Blaster.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        .386p

;---------------------------------------------------------------------------;
;                             I N C L U D E S
;---------------------------------------------------------------------------;

        .xlist
        include vmm.inc
        include debug.inc
        include shell.inc
        include vpicd.inc
        include vadlibd.inc

Create_VSBD_Service_Table       equ 1   ; VSBD service table created
        include vsbd.inc
        .list

;---------------------------------------------------------------------------;
;                              E Q U A T E S
;---------------------------------------------------------------------------;

SQUIRTY macro stuff
IF1
%out ----&stuff&
ENDIF
endm

LOHIB struc
        lob         db  ?
        hib         db  ?
LOHIB ends

VSBD_CB_STRUCT struc
        dwSBCBFlags dd  ?
VSBD_CB_STRUCT ends

fCBAlreadyWarned    equ 00000001h

;
; Sound Blaster default base and IRQ
;
VSBD_DEFAULT_BASE   equ 220h    ; (factory) default port base
VSBD_DEFAULT_IRQ    equ 7h      ; (factory) default IRQ
VSBD_DEFAULT_DMA    equ 1h      ; (factory) default DMA channel

;
; Register offsets from base port for Sound Blaster DSP
;
SB_CMSD0            equ 0h     ; C/MS music voice 1-6 data port, write only
SB_CMSR0            equ 1h     ; C/MS music voice 1-6 register port, write only
SB_CMSD1            equ 2h     ; C/MS music voice 7-12 data port, write only
SB_CMSR1            equ 3h     ; C/MS music voice 7-12 register port, write only

SB_PRO_MIX_REG      equ 4h     ; Pro card mixer address register, write only
SB_PRO_MIX_DATA     equ 5h     ; Pro card mixer data register, read/write

SB_FMD0             equ 8h     ; FM music data/status port, read/write 
SB_FMR0             equ 9h     ; FM music data/status port, write only

SB_DSPRESET         equ 6h     ; DSP Reset, write only
SB_DSPRD            equ 0Ah    ; DSP Read data, read only
SB_DSPWR            equ 0Ch    ; DSP Write data or command, write
SB_DSPBUSY          equ 0Ch    ; DSP Write buffer status (bit 7)
SB_DSPDATAAVAIL     equ 0Eh    ; DSP Data available status (bit 7)

;
; Last port to trap (inclusive) for normal Sound Blaster and Pro card
;
SB_LAST_PORT        equ 0Eh

;
; Port related constants
;
SB_DSPREADY         equ 0AAh    ; DSP ready signal from SB_DSPRD
SB_GETDSPVER        equ 0E1h    ; DSP get firmware version command

;
; Flags for gwSBFlags
;
fSBDisableWarning       equ 00000001b
fSBVAdLibDInstalled     equ 00000010b

;---------------------------------------------------------------------------;
;           V I R T U A L   D E V I C E   D E C L A R A T I O N
;---------------------------------------------------------------------------;

Declare_Virtual_Device VSBD, 3, 0, VSBD_Control, VSBD_Device_ID \
            , VSBD_Init_Order, VSBD_API_Handler, VSBD_API_Handler

        .erre VADLIBD_Init_Order LE VSBD_Init_Order

;---------------------------------------------------------------------------;
;                  I N I T .   T I M E   O N L Y   D A T A
;---------------------------------------------------------------------------;

VxD_IDATA_SEG

gszSBSection        db  "sndblst.drv", 0
gszDisableWarning   db  "disablewarning", 0
gszPort             db  "port", 0
gszInt              db  "int", 0
gszDMAChannel       db  "dmachannel", 0

Begin_VxD_IO_Table VSBD_SB_Port_Table
        VxD_IO  SB_CMSD0,           VSBD_IO_Default
        VxD_IO  SB_CMSR0,           VSBD_IO_Default
        VxD_IO  SB_CMSD1,           VSBD_IO_Default
        VxD_IO  SB_CMSR1,           VSBD_IO_Default
        VxD_IO  04h,                VSBD_IO_Default
        VxD_IO  05h,                VSBD_IO_Default
        VxD_IO  SB_DSPRESET,        VSBD_IO_Default
        VxD_IO  07h,                VSBD_IO_Default

        VxD_IO  SB_FMD0,            VSBD_IO_Default
        VxD_IO  SB_FMR0,            VSBD_IO_Default

        VxD_IO  SB_DSPRD,           VSBD_IO_Default
        VxD_IO  0Bh,                VSBD_IO_Default
        VxD_IO  SB_DSPWR,           VSBD_IO_Default
        VxD_IO  0Dh,                VSBD_IO_Default
        VxD_IO  SB_DSPDATAAVAIL,    VSBD_IO_Default
End_VxD_IO_Table VSBD_SB_Port_Table

VSBD_SB_Port_Table_Entries EQU (($-VSBD_SB_Port_Table)-(SIZE VxD_IOT_Hdr)) / (SIZE VxD_IO_Struc)
        .errnz ((SB_LAST_PORT + 1) - VSBD_SB_Port_Table_Entries)

VSBD_IRQ_Descriptor VPICD_IRQ_Descriptor <VSBD_DEFAULT_IRQ,,        \
                                    offset32 VSBD_IRQ_Hw_Int_Proc,, \
                                    offset32 VSBD_IRQ_EOI_Proc,     \
                                    offset32 VSBD_IRQ_Mask_Changed_Proc,,>

VxD_IDATA_ENDS

;---------------------------------------------------------------------------;
;                   N O N P A G E A B L E   D A T A
;---------------------------------------------------------------------------;

VxD_LOCKED_DATA_SEG


VxD_LOCKED_DATA_ENDS

;---------------------------------------------------------------------------;
;                 P A G E A B L E  D A T A
;---------------------------------------------------------------------------;

VxD_DATA_SEG

;
; In 3.0 and 3.1, this is NONPAGEABLE!
;
extrn gszNoAccessMessage:byte

gdwSBCBOffset       dd  0   ; VM control block offset

gwSBBasePort        dw  0   ; base port address for Sound Blaster
gwSBIRQ             dw  0   ; IRQ for Sound Blaster
gwSBDMAChannel      dw  0   ; DMA channel for card
gwSBFlags           dw  0   ; flags for trapping, etc.
gdwSBIRQHandle      dd  0   ; IRQ handle from VPICD
gdwSBOwner          dd  0   ; VM handle owning this Sound Blaster
gdwAdLibOwner       dd  0   ; VM handle owning the FM synth

gdwSBLastOwner      dd  0   ; last VM to own Sound Blaster (0=fresh reset,-1=unknown state)

gwSBDSPVersion      dw  0
gwSBLastPort        dw  0   ; 0Eh for normal SB

VSBD_API_TABLE  LABEL DWORD
              .errnz SB_API_Get_Version - ($-VSBD_API_TABLE)/4
        dd  offset32 VSBD_API_Get_Version
              .errnz SB_API_Acquire_Sound_Blaster - ($-VSBD_API_TABLE)/4
        dd  offset32 VSBD_API_Acquire_Sound_Blaster
              .errnz SB_API_Release_Sound_Blaster - ($-VSBD_API_TABLE)/4
        dd  offset32 VSBD_API_Release_Sound_Blaster
              .errnz SB_API_Get_Sound_Blaster_Info - ($-VSBD_API_TABLE)/4
        dd  offset32 VSBD_API_Get_Sound_Blaster_Info

VSBD_API_MAX    EQU ($-VSBD_API_TABLE)/4

VxD_DATA_ENDS

;---------------------------------------------------------------------------;
;                             I C O D E
;---------------------------------------------------------------------------;

VxD_ICODE_SEG

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   VSBD_Device_Init
;
;   DESCRIPTION:
;       Device initialization routine
;
;   ENTRY:
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;   USES:
;       All Registers
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
BeginProc VSBD_Device_Init

        VMMcall _Allocate_Device_CB_Area, <<size VSBD_CB_STRUCT>, 0>
        or      eax, eax
        jz      short VSBD_Device_Init_Fail
        mov     [gdwSBCBOffset], eax

        mov     esi, offset32 gszSBSection      ; all stuff from this section

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   find the switch that turns off the "application cannot use SB" warning
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     edi, offset32 gszDisableWarning ; look for this entry
        xor     eax, eax                        ; default is warning on
        VMMcall Get_Profile_Boolean
        or      eax, eax                        ; Q: disable warning?
        jz      short VSBD_Device_Init_Warn     ;   N: then continue
        or      [gwSBFlags], fSBDisableWarning

VSBD_Device_Init_Warn:

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   Find the base IO address, IRQ, and DMA channel of the Sound Blaster.
;
;   If either the base port address or the IRQ value is -1, then the Sound
;   Blaster is currently not configured correctly so we will NOT install.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     ax, VSBD_DEFAULT_DMA            ; default value
        mov     edi, offset32 gszDMAChannel     ; look for this entry..
        VMMcall Get_Profile_Decimal_Int
        mov     [gwSBDMAChannel], ax            ; save it

        mov     al, -1                          ; default value
        mov     edi, offset32 gszInt            ; look for this entry..
        VMMcall Get_Profile_Decimal_Int
        cmp     al, -1                          ; Q: configured?
        je      short VSBD_Device_Init_Fail     ;   N: get out

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   If we are not running on an XT, then we need to virtualize IRQ 9 if
;   the Sound Blaster card is set on IRQ 2.  We don't run on an XT.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        cmp     ax, 2
        jne     short VSBD_Device_Init_Not_2
        mov     ax, 9

VSBD_Device_Init_Not_2:

        mov     [gwSBIRQ], ax                   ; save it
        xor     eax, eax                        ; clear ALL of EAX

        mov     ax, -1                          ; default value
        mov     edi, offset32 gszPort           ; look for this entry..
        VMMcall Get_Profile_Hex_Int
        cmp     ax, -1                          ; Q: configured?
        je      short VSBD_Device_Init_Fail     ;   N: get out
        mov     [gwSBBasePort], ax              ; save it

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   Now we need to put meathooks into the Sound Blaster so we can do the
;   contention management stuff.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        call    VSBD_Virtualize_Sound_Blaster   ; attempt to virtualize SB
        jc      short VSBD_Device_Init_Fail

VSBD_Device_Init_Exit:

        clc                                     ; always succeed
        ret

VSBD_Device_Init_Fail:

ifdef DEBUG
        cmp     al, -1
        jne     short @F
        Trace_Out "VSBD_Device_Init: Sound Blaster not configured correctly!"
@@:
endif

        Debug_Out "VSBD_Device_Init: FAILED!  VSBD.386 will not load."

        stc                                     ; fail to install!
        ret

EndProc VSBD_Device_Init

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   VSBD_Virtualize_Sound_Blaster
;
;   DESCRIPTION:
;       Virtualize a Sound Blaster.  Actually, this VxD will only do 
;       contention stuff, so this is a bad name for this function.
;
;       Trapping will be enabled on the ports.
;
;   ENTRY:
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;   USES:
;       FLAGS
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
BeginProc VSBD_Virtualize_Sound_Blaster

        pushad
        mov     dx, [gwSBBasePort]

ifdef DEBUG
        mov     ax, [gwSBIRQ]
        mov     cx, [gwSBDMAChannel]
        Trace_Out "VSBD_Virtualize_Sound_Blaster: Port #DX  IRQ #AX  DMA #CX"
endif

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   Verify that the Sound Blaster is in fact installed, get DSP version
;   and reset the card.
;
;   EDX = base port
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        call    VSBD_DSP_Reset
        jc      VSBD_VSB_Error

        call    VSBD_DSP_Get_Version
        jc      VSBD_VSB_Error

        mov     [gwSBDSPVersion], ax

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   Virtualize IRQ because the default VPICD handling is inappropriate
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     cx, [gwSBIRQ]                   ; IRQ for SB
        mov     [VSBD_IRQ_Descriptor.VID_IRQ_Number], cx
        mov     edi, offset32 VSBD_IRQ_Descriptor
        VxDcall VPICD_Virtualize_IRQ
        jc      VSBD_VSB_Error
        mov     [gdwSBIRQHandle], eax

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   Build an IO port table for Install_Mult_IO_Handlers based on the base
;   port address.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     [gwSBLastPort], SB_LAST_PORT

        mov     ecx, VSBD_SB_Port_Table_Entries     ; number of IO ports
        mov     esi, offset32 VSBD_SB_Port_Table

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   This is a ONE shot deal!!!
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

VSBD_VSB_Install_IO_Handlers:

        mov     edi, esi                        ; save a copy in EDI
        add     esi, (size VxD_IOT_Hdr)

VSBD_VSB_IO_Loop:

        add     [esi.VxD_IO_Port], dx           ; add port base to offset
        add     esi, (size VxD_IO_Struc)
        loop    VSBD_VSB_IO_Loop

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   tell VMM to trap Sound Blaster ports
;
;   EDI = pointer to IO struc
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
 
        VMMcall Install_Mult_IO_Handlers
        jc      short VSBD_VSB_IO_Error

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   Finally, we need to have VADLIBD.386 manage the contention for the
;   Ad Lib FM synth so make sure it is installed.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        VxDcall VADLIBD_Get_Version
        jnc     short VSBD_VSB_VAdLibD_Installed

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   VADLIBD isn't present, so we will do what we can to manage it.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        Trace_Out "VSBD: VADLIBD.386 is NOT installed!"

        mov     esi, offset32 VSBD_IO_Default
        mov     edx, VADLIBD_DEFAULT_BASE
        VMMcall Install_IO_Handler
        jc      short VSBD_VSB_IO_Error
        inc     edx
        VMMcall Install_IO_Handler
        jc      short VSBD_VSB_IO_Error

        jmp     short VSBD_VSB_Success

VSBD_VSB_VAdLibD_Installed:

        or      [gwSBFlags], fSBVAdLibDInstalled

VSBD_VSB_Success:

        popad
        clc
        ret

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   We are at the point of no return.  We cannot undo Virtualize_IRQ so
;   we must stay in memory.  But we won't be doing much.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

VSBD_VSB_IO_Error:

        Trace_Out "VSBD_Virtualize_Sound_Blaster: Cannot trap ports!!"
        jmp     short VSBD_VSB_Success
        
VSBD_VSB_Error:

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   We failed to virtualize the port.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
        
        popad
        Trace_Out "VSBD_Virtualize_Sound_Blaster FAILED!"
        stc                                     ; flag error
        ret                                     ; ...and leave

EndProc VSBD_Virtualize_Sound_Blaster


VxD_ICODE_ENDS

;---------------------------------------------------------------------------;
;                   N O N P A G E A B L E   C O D E
;---------------------------------------------------------------------------;

VxD_LOCKED_CODE_SEG

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   VSBD_Control
;
;   DESCRIPTION:
;       Dispatch control messages to the correct handlers. Must be in locked
;       code segment. (All VxD segments are locked in 3.0 and 3.1)
;
;   ENTRY:
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;   USES:
;       FLAGS
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
BeginProc VSBD_Control

        Control_Dispatch Device_Init,           VSBD_Device_Init
        Control_Dispatch VM_Not_Executeable,    VSBD_VM_Not_Executeable

ifdef DEBUG
        Control_Dispatch Debug_Query,           VSBD_Debug_Dump
endif
        clc
        ret

EndProc VSBD_Control

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   VSBD_IRQ_Hw_Int_Proc
;
;   DESCRIPTION:
;       Reflects interrupt to current owner or handles it if no owner.
;
;   ENTRY:
;       EAX = IRQ Handle
;       EBX = Current VM Handle
;
;   EXIT:
;
;   USES:
;       Flags, EAX, EBX
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
BeginProc VSBD_IRQ_Hw_Int_Proc

        push    eax                         ; save IRQ handle
        mov     eax, [gdwSBOwner]           ; all interrupts go to owner
        or      eax, eax                    ; Q: is there an owner?
        jz      short VSBD_Int_Unowned      ;   N: set int request to curVM
        mov     ebx, eax                    ;   Y: set int request to owner
        Assumes_Fall_Through VSBD_Int_Unowned

VSBD_Int_Unowned:

        pop     eax                         ; restore IRQ handle
        Assert_VM_Handle ebx
        VxDcall VPICD_Set_Int_Request       ; set int request and return
        clc
        ret

EndProc VSBD_IRQ_Hw_Int_Proc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   VSBD_IRQ_EOI_Proc
;
;   DESCRIPTION:
;       Clears the interrupt request and does the physical EOI
;
;   ENTRY:
;       EAX = IRQ Handle
;       EBX = Current VM Handle
;
;   EXIT:
;
;   USES:
;       Flags
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
BeginProc VSBD_IRQ_EOI_Proc

        VxDcall VPICD_Clear_Int_Request     ; clear virtual IRQ request
        VxDjmp  VPICD_Phys_EOI              ; physical end of interrupt

EndProc VSBD_IRQ_EOI_Proc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   VSBD_IRQ_Mask_Changed_Proc
;
;   DESCRIPTION:
;       If the _owning_ VM is masking or unmasking the IRQ, then we need
;       to set the physical state accordingly.  It is perfectly OK for 
;       a non-owning VM to mask/unmask the IRQ and not disturb the owning
;       VM.  We can *NOT* assign ownership when the IRQ is [un]masked
;       because some apps will mask all IRQs so they can perform some
;       operations and then reset the PIC to the previous state.  This
;       operation has nothing to do with the Sound Blaster.
;
;       Ownership will only be assigned if someone _asks_ for it through
;       the provided API, or if someone touches ports on the card.
;
;   ENTRY:
;       EAX = IRQ Handle
;       EBX = Current VM Handle
;       ECX = 0 if VM is unmasking
;
;   EXIT:
;
;   USES:
;       Flags
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
BeginProc VSBD_IRQ_Mask_Changed_Proc

        cmp     [gdwSBOwner], ebx           ; Q: is this the owner?
        jne     short VSBD_Auto_Mask        ;   N: hmm...

        jecxz   VSBD_Mask_Unmasking

        VxDcall VPICD_Physically_Mask
        jmp     short VSBD_Mask_Exit

VSBD_Mask_Unmasking:

        VxDcall VPICD_Physically_Unmask
        jmp     short VSBD_Mask_Exit


VSBD_Auto_Mask:

        VxDcall VPICD_Set_Auto_Masking
        Assumes_Fall_Through VSBD_Mask_Exit

VSBD_Mask_Exit:

        clc
        ret

EndProc VSBD_IRQ_Mask_Changed_Proc

VxD_LOCKED_CODE_ENDS


;---------------------------------------------------------------------------;
;                P A G E A B L E  C O D E
;---------------------------------------------------------------------------;

VxD_CODE_SEG

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   VSBD_DSP_Read
;
;   DESCRIPTION:
;       Read a byte from the DSP.
;
;       If time out occurs before the DSP is ready then the carry flag
;       is set and no value is returned otherwise the carry flag is cleared
;       and the data value returned in AL.
;
;       The timeout value is very machine dependent.
;
;   ENTRY:
;       DX = Base port address of DSP
;
;   EXIT:
;       IF carry clear
;           success
;           AL = The byte read
;       ELSE
;           fail!
;
;   USES:
;       FLAGS, AL
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
BeginProc VSBD_DSP_Read

        push    ecx
        push    edx

ifdef DEBUG
        cmp     [gwSBBasePort], dx             ; Q: base port correct?
        je      short @F                       ;   Y: yep
        mov     cx, [gwSBBasePort]
        Debug_Out "VSBD_DSP_Read: Base #DX != Global Base #CX"
        mov     dx, cx
@@:
endif

        add     dx, SB_DSPDATAAVAIL            ; point to data avail status port
        mov     ecx, 1000                      ; set time out value

VSBD_DSP_Read_TO_Loop:

        in      al, dx                         ; get status
        test    al, 80H                        ; MSb set ?
        jnz     short VSBD_DSP_Read_Data_Ready ; jump if data ready

        loop    VSBD_DSP_Read_TO_Loop

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   TIMED OUT!
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        Trace_Out "VSBD_DSP_Read: TIMEOUT!"
        stc 
        jmp     short VSBD_DSP_Read_Exit


VSBD_DSP_Read_Data_Ready:

        sub     dx, (SB_DSPDATAAVAIL - SB_DSPRD)    ; got to the data reg
        in      al, dx                              ; get data byte
        clc

VSBD_DSP_Read_Exit:

        pop     edx
        pop     ecx
        ret

EndProc VSBD_DSP_Read

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   VSBD_DSP_Write
;
;   DESCRIPTION:
;       Writes a command or data byte to the DSP.
;
;       Carry set on exit if it times out. The reason the timeout loop
;       is so big is that the SpeakerOff command can take up to 220 msec
;       to execute - if you time out before that the next few DSP commands
;       you write won't work.
;
;   ENTRY:
;       AL = The byte to be written
;       DX = Base port address of DSP
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;   USES:
;       FLAGS
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
BeginProc VSBD_DSP_Write

        push    edx
        push    ecx
        push    eax

ifdef DEBUG
        cmp     [gwSBBasePort], dx          ; Q: base port correct?
        je      short @F                    ;   Y: yep
        mov     cx, [gwSBBasePort]
        Debug_Out "VSBD_DSP_Write: Base #DX != Global Base #CX"
        mov     dx, cx
@@:
endif

        add     dx, SB_DSPBUSY              ; point to data status port
        mov     ecx, 100000h                ; BIG timeout loop counter

VSBD_DSP_Write_TO_Loop:

        in      al, dx
        test    al, 80H                         ; test busy bit
        jz      short VSBD_DSP_Write_Port_Ready ; exit if not busy

        loop    VSBD_DSP_Write_TO_Loop      ; loop if not timed out

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   TIMED OUT!
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        Trace_Out "VSBD_DSP_Write: TIMEOUT!"
        pop     eax                         ; dump the data
        stc                                 ; set carry to show time out
        jmp     short VSBD_DSP_Write_Exit

VSBD_DSP_Write_Port_Ready:

        pop     eax                         ; get the data byte
        out     dx, al                      ; write it (same port)
        clc                                 ; no error

VSBD_DSP_Write_Exit:

        pop     ecx
        pop     edx
        ret

EndProc VSBD_DSP_Write

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   VSBD_DSP_Reset
;
;   DESCRIPTION:
;       Resets the DSP on the Sound Blaster.
;
;   ENTRY:
;       DX = Base port address of DSP
;
;   EXIT:
;       IF carry clear
;           success
;       ELSE
;           DSP not reset (hardware not present?)
;
;   USES:
;       FLAGS
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
BeginProc VSBD_DSP_Reset

        push    eax
        push    ecx
        push    edx                             ; push last!

        Trace_Out "VSBD_DSP_Reset: Resetting DSP at port base #DX"

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   To reset the DSP, we need to write to the RESET port (02x6h) a 1, wait
;   3 microseconds, write a 0 to the same port, THEN poll the ready port
;   (02xAh) for 0AAh
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        add     dx, SB_DSPRESET                 ; point to reset port
        mov     al, 1                           ; first write a '1'
        out     dx, al                          ; reset active

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   wait at least 3 microseconds for DSP reset to take a good hold (this
;   loop takes ~4us on a 50MHz i486).
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     ecx, 100
        loop    $

        xor     al, al                          ; now write a '0'
        out     dx, al                          ; clear the reset

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   read the data port to confirm 0AAH is there - supposedly, this should
;   only take ~100 microseconds.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     ecx, 25                         ; lots of tries
        pop     edx                             ; get back base port addr
        push    edx

VSBD_DSP_Reset_TO_Loop:

        call    VSBD_DSP_Read
        jc      short VSBD_DSP_Reset_TO_Read    ; if read timed out, try again

        cmp     al, SB_DSPREADY                 ; Q: correct return value?
        jz      short VSBD_DSP_Reset_Succeed    ;   Y: get out if so!

VSBD_DSP_Reset_TO_Read:

        loop    VSBD_DSP_Reset_TO_Loop
        Debug_Out "VSBD_DSP_Reset: Resetting DSP FAILED!"
        stc                                     ; flag bad reset error
        jmp     short VSBD_DSP_Reset_Exit

VSBD_DSP_Reset_Succeed:

        clc                                     ; flag success

VSBD_DSP_Reset_Exit:

        pop     edx
        pop     ecx
        pop     eax
        ret

EndProc VSBD_DSP_Reset

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   VSBD_DSP_Get_Version
;
;   DESCRIPTION:
;       Gets the DSP version of the Sound Blaster.  The DSP should have 
;       already been reset JUST BEFORE calling this function.  Calling this
;       function twice without resetting the DSP between calls may yield
;       some interesting results on the second call on a Thunder Board.
;       This is *by design* for the Thunder Board.
;
;   ENTRY:
;       DX = Base port address of DSP
;
;   EXIT:
;       IF carry clear
;           success
;           AX = DSP version (AH = major, AL = minor)
;       ELSE
;           DSP not found
;           AX = 0
;
;   USES:
;       FLAGS, AX
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
BeginProc VSBD_DSP_Get_Version

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   read the firmware version number of DSP
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
              
        mov     al, SB_GETDSPVER
        call    VSBD_DSP_Write
        jc      short VSBD_DSP_Get_Version_Fail

        call    VSBD_DSP_Read
        jc      short VSBD_DSP_Get_Version_Fail

        mov     ah, al                      ; save major version
        call    VSBD_DSP_Read
        jnc     short VSBD_DSP_Get_Version_Exit

VSBD_DSP_Get_Version_Fail:

        Debug_Out "VSBD_DSP_Get_Version: FAILED!!!"
        xor     ax, ax
        stc
        ret

VSBD_DSP_Get_Version_Exit:

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   AH has major, AL has minor
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        clc
        ret

EndProc VSBD_DSP_Get_Version

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   VSBD_Enable_Trapping
;
;   DESCRIPTION:
;       Enables trapping of board's ports in owning VM
;
;   ENTRY:
;       EBX = VM handle to enable trapping in
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;   USES:
;       FLAGS
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
BeginProc VSBD_Enable_Trapping

        push    esi
        push    eax
        push    ecx

        Assert_VM_Handle ebx

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   step through all ports to re-enable trapping for VM handle EBX
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     si, [gwSBLastPort]
        xor     ecx, ecx
        movzx   edx, [gwSBBasePort]

VSBD_Enable_Trap_IO:

        cmp     cx, SB_FMD0
        je      short VSBD_Enable_Trap_Skip
        cmp     cx, SB_FMR0
        je      short VSBD_Enable_Trap_Skip

        VMMcall Enable_Local_Trapping

VSBD_Enable_Trap_Skip:

        inc     dx
        cmp     cx, si
        je      short VSBD_Enable_Trapping_Exit
        inc     cx
        jmp     short VSBD_Enable_Trap_IO

VSBD_Enable_Trapping_Exit:

        pop     ecx
        pop     eax
        pop     esi
        clc
        ret

EndProc VSBD_Enable_Trapping

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   VSBD_Disable_Trapping
;
;   DESCRIPTION:
;       Disables trapping of board's ports in an owning VM
;
;   ENTRY:
;       EBX = VM handle to disable trapping in
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;   USES:
;       FLAGS
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
BeginProc VSBD_Disable_Trapping

        push    esi
        push    eax
        push    ecx
        push    edx

        Assert_VM_Handle ebx

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   step through all DSP related ports to disable trapping for VM handle EBX
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

VSBD_Disable_Trap_DSP:

        mov     si, [gwSBLastPort]
        xor     ecx, ecx
        movzx   edx, [gwSBBasePort]

VSBD_Disable_Trap_IO:

        cmp     cx, SB_FMD0
        je      short VSBD_Disable_Trap_Skip
        cmp     cx, SB_FMR0
        je      short VSBD_Disable_Trap_Skip

        VMMcall Disable_Local_Trapping

VSBD_Disable_Trap_Skip:

        inc     dx
        cmp     cx, si
        je      short VSBD_Disable_Trapping_Exit
        inc     cx
        jmp     short VSBD_Disable_Trap_IO

VSBD_Disable_Trapping_Exit:

        pop     edx
        pop     ecx
        pop     eax
        pop     esi
        clc
        ret

EndProc VSBD_Disable_Trapping

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   VSBD_VM_Not_Executeable
;
;   DESCRIPTION:
;       This procedure checks whether the VM being destroyed owns a Sound
;       Blaster.  If it does, then dwSBOwner is cleared and the DSP is
;       reset.  Note that we reset because the VM could have crashed and
;       left the Sound Blaster in an audibly annoying state.
;
;   ENTRY:
;       EBX = handle of VM being destroyed
;       EDX = Flags: VNE_Crashed, VNE_Nuked, VNE_CreateFail,
;                    VNE_CrInitFail, VNE_InitFail
;
;   EXIT:
;       Carry clear
;
;   USES:
;       FLAGS, EBX, EAX
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
BeginProc VSBD_VM_Not_Executeable

        cmp     [gdwSBOwner], ebx
        je      short VSBD_VNE_Continue

        cmp     [gdwAdLibOwner], ebx
        jne     short VSBD_VNE_Exit
        Assumes_Fall_Through VSBD_VNE_Continue

VSBD_VNE_Continue:

        mov     [gdwSBLastOwner], -1            ; not owner anymore/not reset

        mov     eax, fSB_ASB_Acquire_DSP + fSB_ASB_Acquire_AdLib_Synth
        call    VSBD_Release_Sound_Blaster

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   We already know that the VM that is going down 'owned' the SB.  Therefore
;   it is a safe assumption that any bits set in the flags probably mean
;   bad news - so we will reset the DSP.  The worst case is that the DSP
;   doesn't actually need reset and it will be reset; waste of time but
;   no harm done.  Currently, VNE_Crashed + VNE_Nuked + VNE_Closed would be
;   the only bits set.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        or      edx, edx
        jz      short VSBD_VNE_Exit

        movzx   edx, [gwSBBasePort]
        call    VSBD_DSP_Reset
        mov     [gdwSBLastOwner], 0             ; not owner anymore - reset

VSBD_VNE_Exit:

        clc
        ret

EndProc VSBD_VM_Not_Executeable

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   VSBD_Release_Sound_Blaster
;
;   DESCRIPTION:
;       This function will releases the SB from ownership by a VM. 
;
;   ENTRY:
;       EAX = Flags
;           fSB_ASB_Acquire_AdLib_Synth         equ 00000001b
;           fSB_ASB_Acquire_DSP                 equ 00000010b
;       EBX = VM handle wanting to release
;
;   EXIT:
;
;   USES:
;       Flags
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
BeginProc  VSBD_Release_Sound_Blaster

        push    eax
        push    edx

        test    eax, fSB_ASB_Acquire_AdLib_Synth
        jz      short VSBD_RSB_Try_DSP

        cmp     [gdwAdLibOwner], ebx
        jne     short VSBD_RSB_Try_DSP

        test    [gwSBFlags], fSBVAdLibDInstalled
        jnz     short VSBD_RSB_VAdLibD_Installed

        mov     edx, VADLIBD_DEFAULT_BASE
        VMMcall Enable_Local_Trapping
        inc     dx
        VMMcall Enable_Local_Trapping

        jmp     short VSBD_RSB_AdLib_Enable_Trap_Shadow

VSBD_RSB_VAdLibD_Installed:

        push    eax
        mov     edx, VADLIBD_DEFAULT_BASE       ; port 0388h
        VxDcall VADLIBD_Release_Synth
        pop     eax                             ; restore EAX

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   Enable trapping on Ad Lib shadow ports iff synth is being virtualized
;   and it is owned by VM EBX.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

VSBD_RSB_AdLib_Enable_Trap_Shadow:

        movzx   edx, [gwSBBasePort]
        add     dx, SB_FMD0
        VMMcall Enable_Local_Trapping
        inc     dx                              ; goto SB_FMR0 == (SB_FMD0+1)
        VMMcall Enable_Local_Trapping

        mov     [gdwAdLibOwner], 0

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   released DSP
;
;   EAX = flags
;   EBX = VM handle
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

VSBD_RSB_Try_DSP:

        test    eax, fSB_ASB_Acquire_DSP
        jz      short VSBD_RSB_Exit

        cmp     [gdwSBOwner], ebx
        jne     short VSBD_RSB_Exit

        mov     eax, [gdwSBIRQHandle]           ; EAX = IRQ handle 

ifdef DEBUG
        push    ecx
        VxDcall VPICD_Get_Complete_Status
        test    ecx, VPICD_Stat_IRET_Pending+VPICD_Stat_In_Service+VPICD_Stat_Phys_In_Serv+VPICD_Stat_Virt_Req+VPICD_Stat_Phys_Req+VPICD_Stat_Virt_Dev_Req
        jz      short @F
        Debug_Out "VSBD_Release_DSP: Releasing Sound Blaster with IRQ in service!!! #ECX"
@@:
        pop     ecx
endif

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   Mask the IRQ _first_ so no more interrupts will be generated after we
;   EOI--then clear any pending interrupts that have been 'set' into the
;   VM by us.
;
;   EAX = IRQ Handle
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        VxDcall VPICD_Physically_Mask
        VxDcall VPICD_Phys_EOI
        VxDcall VPICD_Clear_Int_Request         ; clear any pending request

        call    VSBD_Enable_Trapping
        mov     [gdwSBOwner], 0                 ; zero out owner VM handle

        mov     edx, ebx
        add     edx, [gdwSBCBOffset]
        and     [edx.dwSBCBFlags], not fCBAlreadyWarned

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   now set physical mask
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     eax, [gdwSBIRQHandle]
        VxDcall VPICD_Set_Auto_Masking
        Assumes_Fall_Through VSBD_RSB_Exit

VSBD_RSB_Exit:   

        pop     edx
        pop     eax
        clc
        ret
 
EndProc  VSBD_Release_Sound_Blaster

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   VSBD_Acquire_Sound_Blaster
;
;   DESCRIPTION:
;       This function acquires Sound Blaster for a VM.
;
;   ENTRY:
;       EAX = Flags
;           fSB_ASB_Acquire_AdLib_Synth         equ 00000001b
;           fSB_ASB_Acquire_DSP                 equ 00000010b
;           fSB_ASB_Auto_Reset_DSP              equ 00000100b
;       EBX = VM handle to own SB
;
;   EXIT:
;
;   USES:
;       Flags
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
BeginProc  VSBD_Acquire_Sound_Blaster

        push    eax
        push    ecx
        push    edx
        Assert_VM_Handle ebx

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   If VADLIBD is not installed, then always force total acquire.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        test    eax, fSB_ASB_Acquire_AdLib_Synth
        jz      short VSBD_ASB_Try_For_DSP

        cmp     [gdwAdLibOwner], 0
        je      short VSBD_ASB_No_AdLib_Owner

        cmp     [gdwAdLibOwner], ebx
        je      short VSBD_ASB_Try_For_DSP
        stc
        jmp     VSBD_ASB_Exit                   ; fail if can't acquire

VSBD_ASB_No_AdLib_Owner:

        test    [gwSBFlags], fSBVAdLibDInstalled
        jz      short VSBD_ASB_AdLib_Disable_Trap

        push    eax
        mov     edx, VADLIBD_DEFAULT_BASE       ; port 0388h
        VxDcall VADLIBD_Acquire_Synth
        pop     eax                             ; restore EAX
        jc      VSBD_ASB_Exit                   ; fail if can't acquire
        jnc     short VSBD_ASB_AdLib_Disable_Trap_Shadow

VSBD_ASB_AdLib_Disable_Trap:

        mov     edx, VADLIBD_DEFAULT_BASE
        VMMcall Disable_Local_Trapping
        inc     dx
        VMMcall Disable_Local_Trapping

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   Disable trapping on Ad Lib shadow ports iff synth is being virtualized
;   and it is owned by VM EBX.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

VSBD_ASB_AdLib_Disable_Trap_Shadow:

        movzx   edx, [gwSBBasePort]
        add     dx, SB_FMD0
        VMMcall Disable_Local_Trapping
        inc     dx                              ; goto SB_FMR0
        VMMcall Disable_Local_Trapping

        mov     [gdwAdLibOwner], ebx

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   acquire DSP.
;
;   EAX = flags
;   EBX = VM handle
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

VSBD_ASB_Try_For_DSP:

        test    eax, fSB_ASB_Acquire_DSP
        jz      VSBD_ASB_Exit                   ; test clears carry

        cmp     [gdwSBOwner], 0
        je      short VSBD_ASB_Do_It

        cmp     [gdwSBOwner], ebx
        je      VSBD_ASB_Success

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   Failed to acquire SB because it is currently owned - if we already 
;   acquired the Ad Lib, then release it.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        test    eax, fSB_ASB_Acquire_AdLib_Synth
        jz      short VSBD_ASB_Fail

        test    [gwSBFlags], fSBVAdLibDInstalled
        jnz     short VSBD_ASB_Fail_VAdLibD_Installed

        mov     edx, VADLIBD_DEFAULT_BASE
        VMMcall Enable_Local_Trapping
        inc     dx
        VMMcall Enable_Local_Trapping

        mov     [gdwAdLibOwner], 0
        jmp     short VSBD_ASB_Fail

VSBD_ASB_Fail_VAdLibD_Installed:

        mov     edx, VADLIBD_DEFAULT_BASE       ; port 0388h
        VxDcall VADLIBD_Release_Synth
        mov     [gdwAdLibOwner], 0

VSBD_ASB_Fail:

        stc
        jc      short VSBD_ASB_Exit

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   EAX = Flags
;   EBX = VM handle
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

VSBD_ASB_Do_It:

        mov     [gdwSBOwner], ebx               ; assign ownership
        call    VSBD_Disable_Trapping

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   If the caller is using 'fSB_ASB_Auto_Reset_DSP' when trying to acquire,
;   then we will reset the DSP for the caller:
;       1. If the caller's VM was NOT the last owner
;       2. If last owner is '0' then VSBD already freshly reset the DSP
;          so there is no need to reset it again.
;   If the auto-reset is not used, then the DSP will be reset unless 2.
;   is true.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        test    eax, fSB_ASB_Auto_Reset_DSP
        jz      short VSBD_ASB_Safety_Reset

        cmp     [gdwSBLastOwner], ebx
        je      short VSBD_ASB_Dont_Reset

VSBD_ASB_Safety_Reset:

        cmp     [gdwSBLastOwner], 0
        je      short VSBD_ASB_Dont_Reset

        movzx   edx, [gwSBBasePort]
        call    VSBD_DSP_Reset

VSBD_ASB_Dont_Reset:

        mov     [gdwSBLastOwner], ebx           ; set last owner appropriately

        mov     eax, [gdwSBIRQHandle]
        VxDcall VPICD_Set_Auto_Masking
        Assumes_Fall_Through VSBD_ASB_Success

VSBD_ASB_Success:

        clc

VSBD_ASB_Exit:

        pop     edx
        pop     ecx
        pop     eax
        ret
 
EndProc  VSBD_Acquire_Sound_Blaster


BeginDoc
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   VSBD_Get_Version, SERVICE
;
;   DESCRIPTION:
;       Get VSBD device version.
;
;   ENTRY:
;
;   EXIT:
;       IF Carry clear
;           EAX is version; AH = Major, AL = Minor
;       ELSE
;           VSBD device not installed
;
;   USES:
;       Flags, EAX
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
EndDoc
BeginProc VSBD_Get_Version, SERVICE

        mov     eax, (VSBD_Ver_Major shl 8) or VSBD_Ver_Minor
        clc
        ret

EndProc VSBD_Get_Version


BeginDoc
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   VSBD_Get_Sound_Blaster_Focus, SERVICE
;
;   DESCRIPTION:
;       Return the VM handle of the current owner of an Sound Blaster. Focus
;       can be set with a Set_Device_Focus System_Control call specifying
;       the VSBD device.
;
;   ENTRY:
;       EAX = Base port address of SB in question.
;
;   EXIT:
;       Carry clear - EBX = VM Handle, 0 if no owner
;       Carry set - port not virtualized, EBX remains unchanged
;
;   USES:
;       Flags, EBX
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
EndDoc
BeginProc VSBD_Get_Sound_Blaster_Focus, SERVICE

        cmp     [gwSBBasePort], ax
        je      short VSBD_Get_SB_Focus_Valid

        stc
        ret

VSBD_Get_SB_Focus_Valid:

        mov     ebx, [gdwSBOwner]               ; put owner in EBX
        clc
        ret

EndProc VSBD_Get_Sound_Blaster_Focus

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   VSBD_Warning
;
;   DESCRIPTION:
;
;   ENTRY:
;       EDX = port being touched
;       EBX = VM to bring up warning dlg for
;
;   EXIT:
;
;   USES:
;       Flags, ESI, EDI, EAX, ECX
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
BeginProc VSBD_Warning

        mov     esi, ebx
        add     esi, [gdwSBCBOffset]
        test    [esi.dwSBCBFlags], fCBAlreadyWarned
        jnz     short VSBD_IO_Skip_Warning          ;   Y: 

ifdef DEBUG
        mov     eax, [gdwSBOwner]                   ; get SB owner
        Trace_Out "VSBD: #EBX is touching SB Port #DX--#EAX owns it!!"
@@:
endif

        or      [esi.dwSBCBFlags], fCBAlreadyWarned

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   Finally check to see if warnings are enabled (default).  If they are,
;   then see if we are currently sitting in a warning waiting for the
;   user's response; if all is clear then put up the warning.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        test    [gwSBFlags], fSBDisableWarning      ; Q: warning disabled?
        jnz     short VSBD_IO_Skip_Warning          ;   Y: skip warning

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   Have SHELL put up an appropriate warning.  This needs to be changed
;   to a contention dlg box soon!
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     eax, MB_OK or MB_ICONEXCLAMATION    ; message box flags
        mov     ecx, offset32 gszNoAccessMessage    ; string
        xor     esi, esi                            ; no callback
        xor     edi, edi                            ; default caption
        VxDcall SHELL_Message               

VSBD_IO_Skip_Warning:

        ret

EndProc VSBD_Warning

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   VSBD_IO_Default
;
;   DESCRIPTION:
;       Handle IO trapping of the Sound Blaster ports.
;
;   ENTRY:
;       EBX = VM Handle.
;       ECX = Type of I/O
;       EDX = Port number
;       EBP = Pointer to client register structure
;
;   EXIT:
;       EAX = data input or output depending on type of I/O
;
;   USES:
;       FLAGS
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
BeginProc VSBD_IO_Default

        push    eax                             ; save
        mov     eax, [gdwSBOwner]               ; get SB owner

        cmp     eax, ebx                        ; Q: does this VM own it?

ifdef DEBUG
        jne     short VSBD_IODef_New_Owner      ;   N: then try to assign owner
        Debug_Out "VSBD: #EAX OWNS SB AND TRAPPING IS ENABLED!?!"
endif

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   Trapping should have been disabled for the owning VM!  But, if
;   somehow we got hosed, allow access to the owner.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        je      short VSBD_IODef_Allow_Access

VSBD_IODef_New_Owner:

        or      eax, eax                        ; Q: is there already an owner?
        jnz     short VSBD_IODef_Not_Owner      ;   Y: yes, fail call!

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   We are going to auto-assign this SB to VM that is trying to use the
;   board; it is currently unowned.  However, because this is an 'auto-assign'
;   we need to acquire 'all' of the sound blaster, so include the Ad Lib
;   in the acquire.
;
;   EAX = 0
;   EBX = VM handle
;   ECX = type of I/O
;   EDX = port touched
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        push    ebx
        push    ecx
        push    edx
        mov     eax, fSB_ASB_Auto_Reset_DSP + fSB_ASB_Acquire_DSP + fSB_ASB_Acquire_AdLib_Synth
        call    VSBD_Acquire_Sound_Blaster      ; aquire SB
        pop     edx
        pop     ecx
        pop     ebx
        jc      short VSBD_IODef_Not_Owner      ; fail if cannot acquire!

VSBD_IODef_Allow_Access:

        pop     eax                             ; restore

        Dispatch_Byte_IO Fall_Through, <short VSBD_IODef_Real_Out>
        in      al, dx                          ; input from physical port
        jmp     short VSBD_IODef_Exit

VSBD_IODef_Real_Out:

        out     dx, al                          ; output to physical port
        Assumes_Fall_Through VSBD_IODef_Exit

VSBD_IODef_Exit:

        ret

VSBD_IODef_Not_Owner:

        pop     eax
        call    VSBD_Warning
        xor     eax, eax                        ; fail input with -1 value
        dec     eax
        ret

EndProc VSBD_IO_Default

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   VSBD_API_Handler
;
;   DESCRIPTION:
;       This is the single entry point for a VM (pmode or rmode) to make
;       a Sound Blaster driver's life easier
;
;   ENTRY:
;       EBX = Current VM Handle
;       EBP = Pointer to Client Register Structure.
;
;       Client_DX = Function number
;
;       And API specific Client_?? registers.
;
;   EXIT:
;       Client_EFLAGS _PLUS_ any API specific Client_?? registers.
;
;   USES:
;       FLAGS
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
BeginProc VSBD_API_Handler

        movzx   eax, [ebp.Client_DX]            ; get API index
        cmp     eax, VSBD_API_MAX               ; Q: index in range?
        jae     short VSBD_API_Fail             ;   N: hmm...

        call    VSBD_API_Table[eax*4]           ;   Y: call appropriate API
        jc      short VSBD_API_Fail

        and     [ebp.Client_Flags], not CF_Mask ; clear carry
        ret

VSBD_API_Fail:

        or      [ebp.Client_EFLAGS], CF_Mask    ; set carry
        ret

EndProc VSBD_API_Handler


BeginDoc
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   VSBD_API_Get_Version, PMAPI, RMAPI
;
;   DESCRIPTION:
;       Version call for API entry point
;
;   ENTRY:
;       EBX = Current VM Handle
;       EBP = Pointer to Client Register Structure.
;
;       Client_DX = SB_API_Get_Version (0)
;
;   EXIT:
;       Client_EFLAGS = carry clear
;       Client_AX = Version
;
;   USES:
;       FLAGS, EAX
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
EndDoc
BeginProc VSBD_API_Get_Version

        Assert_Client_Ptr ebp

        VxDcall VSBD_Get_Version                ; get version in EAX
        mov     [ebp.Client_AX], ax             ; dump it in client's AX
        clc
        ret

EndProc VSBD_API_Get_Version

BeginDoc
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   VSBD_API_Acquire_Sound_Blaster, PMAPI, RMAPI
;
;   DESCRIPTION:
;       Assigns ownership to VM
;
;   ENTRY:
;       EBX = Current VM Handle (VM to assign ownership to)
;       EBP = Pointer to Client Register Structure.
;
;       Client_AX = Base of SB to acquire (for example, 0240h)
;       Client_BX = Flags
;           fSB_ASB_Acquire_AdLib_Synth         equ 00000001b
;           fSB_ASB_Acquire_DSP                 equ 00000010b
;           fSB_ASB_Auto_Reset_DSP              equ 00000100b
;       Client_DX = SB_API_Acquire_Sound_Blaster (1)
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;       IF Client_EFLAGS = carry clear
;           success, ownership assigned
;       ELSE Client_EFLAGS = carry set
;           fail, Client_AX is error code:
;
;           SB_API_ASB_Err_Bad_Sound_Blaster      equ 01h
;               The SB base specified is not being virtualized by
;               VSBD.
;
;           SB_API_ASB_Err_Already_Owned  equ 02h
;               The SB is currently owned by another VM.
;
;   USES:
;       Flags, EAX, EDX, ESI
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
EndDoc
BeginProc VSBD_API_Acquire_Sound_Blaster

        Assert_Client_Ptr ebp
        movzx   eax, [ebp.Client_BX]            ; flags in EAX

ifdef DEBUG
        test    eax, not (fSB_ASB_Auto_Reset_DSP + fSB_ASB_Acquire_DSP + fSB_ASB_Acquire_AdLib_Synth)
        jz      short @F                        ; no bogus flags
        Debug_Out "SB_API_Acquire_Sound_Blaster: Reserved flags used! #EAX"
        mov     [ebp.Client_AX], -1
        jmp     short VSBD_API_ASB_Error_Exit
@@:
endif

        movzx   edx, [ebp.Client_AX]            ; SB base in EDX
        or      edx, edx                        ; Q:
        jz      short VSBD_API_ASB_Bad_SB_Exit

        cmp     [gwSBBasePort], dx              ; Q: correct port base?
        je      short VSBD_API_ASB_Verify_SB    ;   Y: then continue

VSBD_API_ASB_Bad_SB_Exit:

        mov     [ebp.Client_AX], SB_API_ASB_Err_Bad_SB


VSBD_API_ASB_Error_Exit:

        stc
        ret

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   EAX = flags
;   EBX = VM handle to own SB
;   EDX = SB base to acquire
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

VSBD_API_ASB_Verify_SB:

        mov     [ebp.Client_AX], 0              ; assume success
        cmp     [gdwSBLastOwner], ebx           ; Q: last owned by this VM?
        je      short @F

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   The DSP was last owned by some other fool than the calling VM.  Signal
;   this by setting the high bit of the return code.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     [ebp.Client_AX], SB_API_ASB_Err_State_Unknown

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   EAX = flags passed from the caller
;   EBX = VM to acquire
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

@@:
        call    VSBD_Acquire_Sound_Blaster      ; assign ownership
        jc      short VSBD_API_ASB_Already_Owned
        ret
    
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   SB and/or Ad Lib is currently owned by another VM!
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

VSBD_API_ASB_Already_Owned:

        mov     [ebp.Client_AX], SB_API_ASB_Err_Already_Owned
        jmp     short VSBD_API_ASB_Error_Exit

EndProc VSBD_API_Acquire_Sound_Blaster

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   VSBD_API_Release_Sound_Blaster, PMAPI, RMAPI
;
;   DESCRIPTION:
;       Releases ownership of an owned SB so other VM's can use it.
;       Only the current owning VM can release a SB!
;
;   ENTRY:
;       EBX = Current VM Handle (VM to release ownership)
;       EBP = Pointer to Client Register Structure.
;
;       Client_AX = Base of SB to release (for example, 0240h)
;       Client_BX = Flags
;           fSB_ASB_Acquire_AdLib_Synth         equ 00000001b
;           fSB_ASB_Acquire_DSP                 equ 00000010b
;       Client_DX = SB_API_Release_Sound_Blaster (2)
;
;   EXIT:
;       Carry clear if no error, set otherwise
;
;       IF Client_EFLAGS = carry clear
;           success, ownership released
;       ELSE Client_EFLAGS = carry set
;           fail, Client_AX is error code:
;
;           SB_API_RSB_Err_Bad_SB      equ 01h
;               The SB base specified is not being virtualized by
;               VSBD.
;
;           SB_API_RSB_Err_Not_Yours      equ 02h
;               The SB is NOT owned by callers VM.
;
;   USES:
;       Flags, EAX, EDX, ESI
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
BeginProc  VSBD_API_Release_Sound_Blaster

        Assert_Client_Ptr ebp
        movzx   eax, [ebp.Client_BX]            ; flags in EAX

ifdef DEBUG
        test    eax, not (fSB_ASB_Acquire_DSP + fSB_ASB_Acquire_AdLib_Synth)
        jz      short @F                        ; no bogus flags
        Debug_Out "SB_API_Release_Sound_Blaster: Reserved flags used! #EAX"
        mov     [ebp.Client_AX], -1
        jmp     short VSBD_API_RSB_Error_Exit
@@:
endif

        movzx   edx, [ebp.Client_AX]            ; SB base in EDX
        or      edx, edx
        jz      short VSBD_API_RSB_Bad_SB_Exit

        cmp     [gwSBBasePort], dx
        je      short VSBD_API_RSB_Verify_SB

VSBD_API_RSB_Bad_SB_Exit:

        mov     [ebp.Client_AX], SB_API_RSB_Err_Bad_SB

VSBD_API_RSB_Error_Exit:

        stc
        ret

VSBD_API_RSB_Verify_SB:

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   EAX = flags
;   EBX = VM handle to release 
;   EDX = SB base to release
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        cmp     [gdwSBOwner], ebx               ; Q: is SB owned by VM?
        je      short VSBD_API_RSB_Release      ;   Y: then release it

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   SB is currently owned by another VM! (or not owned)
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     [ebp.Client_AX], SB_API_RSB_Err_Not_Yours
        jmp     short VSBD_API_RSB_Error_Exit

VSBD_API_RSB_Release:

        call    VSBD_Release_Sound_Blaster      ; release ownership

        mov     [ebp.Client_AX], 0              ; success
        clc
        ret

EndProc VSBD_API_Release_Sound_Blaster

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   VSBD_Get_Sound_Blaster_Info, PMAPI, RMAPI
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;       Return (carry clear, success):
;           Client_AX = DSP version
;           Client_BX = Flags: fSB_ASB_Acquire_DSP, fSB_ASB_Acquire_AdLib_Synth
;           Client_CH = IRQ
;           Client_CL = DMA channel
;           Client_DX = Base port of SB
;
;   USES:
;       Flags, EAX, ECX
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
BeginProc VSBD_API_Get_Sound_Blaster_Info

        Assert_Client_Ptr ebp

        movzx   eax, [gwSBDSPVersion]
        mov     [ebp.Client_AX], ax
        movzx   eax, [gwSBBasePort]
        mov     [ebp.Client_DX], ax
        movzx   eax, [gwSBIRQ]
        mov     ch, al
        movzx   eax, [gwSBDMAChannel]
        mov     cl, al
        mov     [ebp.Client_CX], cx

        xor     eax, eax
        mov     [ebp.Client_BX], ax
        cmp     [gdwSBOwner], eax
        je      short @F
        or      [ebp.Client_BX], fSB_ASB_Acquire_DSP

@@:
        cmp     [gdwAdLibOwner], eax
        je      short @F
        or      [ebp.Client_BX], fSB_ASB_Acquire_AdLib_Synth
@@:

        clc
        ret
    
EndProc VSBD_API_Get_Sound_Blaster_Info


ifdef DEBUG

;---------------------------------------------------------------------------;
;              B E G I N:  D E B U G G I N G   A N N E X
;---------------------------------------------------------------------------;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   VSBD_Debug_Dump
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;       Flags
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
BeginProc VSBD_Debug_Dump

        pushad

        Trace_Out "VSBD Debug Dump-O-Matic:"
        movzx   eax, [gwSBBasePort]
        Trace_Out "     gwSBBasePort: #AX"
        movzx   eax, [gwSBIRQ]
        Trace_Out "          gwSBIRQ: #AX"
        movzx   eax, [gwSBDMAChannel]
        Trace_Out "   gwSBDMAChannel: #AX"
        movzx   eax, [gwSBDSPVersion]
        Trace_Out "   gwSBDSPVersion: #AX"
        mov     eax, [gdwSBIRQHandle]
        Trace_Out "   gdwSBIRQHandle: #EAX"
        mov     eax, [gdwSBOwner]
        Trace_Out "       gdwSBOwner: #EAX"
        mov     eax, [gdwSBLastOwner]
        Trace_Out "   gdwSBLastOwner: #EAX"
        mov     eax, [gdwAdLibOwner]
        Trace_Out "    gdwAdLibOwner: #EAX"
        VMMcall Get_Cur_VM_Handle
        Trace_Out "       Current VM: #EBX"
        VMMcall Get_Sys_VM_Handle
        Trace_Out "        System VM: #EBX"
        movzx   eax, [gwSBFlags]
        VMMcall Debug_Convert_Hex_Binary
        Trace_Out "        gwSBFlags: #EAXb"
        Trace_Out "            flags: 1=fSBAdLibInstalled   0=fSBDisableWarning"

VSBD_Debug_Exit:
        popad
        ret

EndProc VSBD_Debug_Dump

;---------------------------------------------------------------------------;
;              E N D:  D E B U G G I N G   A N N E X
;---------------------------------------------------------------------------;
endif


VxD_CODE_ENDS

        end
