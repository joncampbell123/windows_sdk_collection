;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   sndblst.inc
;
;   Copyright (c) 1991-1992 Microsoft Corporation.  All rights reserved.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   equates and structure definitions
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

TRUE                equ 1
FALSE               equ 0

PIC_EOI_MASTER      equ 20h             ; where to send master EOI
PIC_EOI_SLAVE       equ 0A0h            ; where to send slave EOI

PIC_IMR_MASTER      equ 21h
PIC_IMR_SLAVE       equ 0A1h

WOD_DMA_TIMEOUT     equ 2000            ; 2000 milliseconds

;
;   for DMA buffer ping/pong (gbDMABuffer)
;
DMA_BUFFER_DONE     equ 0               ; DMA should stop
DMA_BUFFER_PING     equ 1               ; currently DMA'ing into first half
DMA_BUFFER_PONG     equ 2               ; currently DMA'ing into last half

;
;   these equates MUST equal those in sndblst.h
;
IDS_ERRTWODRIVERS       equ 1
IDS_ERRMCANOTSUPPORTED  equ 2
IDS_ERROLDVDMAD         equ 3
IDS_ERRBADPORT          equ 4
IDS_ERRBADVERSION       equ 5
IDS_ERRBADINT           equ 6
IDS_ERRBADCONFIG        equ 16

;
;   MIDI input and output state flags...
;
MIF_ALLOCATED       equ 00000001b
MIF_STARTED         equ 00000010b
MIF_SUSPENDED       equ 00000100b
MIF_RESTART         equ 00001000b

MOF_ALLOCATED       equ 00000001b
MOF_SUSPENDED       equ 00000100b

;
;   Wave input and output state flags...
;
WIF_ALLOCATED       equ 00000001b
WIF_STARTED         equ 00000010b
WIF_SUSPENDED       equ 00000100b
WIF_RESTART         equ 00001000b

WOF_ALLOCATED       equ 00000001b
WOF_SUSPENDED       equ 00000100b
WOF_RESTART         equ 00001000b

;
;   For InitResetAdapter.  _POWERON is 'power on' state; _DEFAULT is what
;   this driver requires the adapter to be set to.
;
IRA_MODE_POWERON    equ 0
IRA_MODE_DEFAULT    equ 1

;
; DMA buffer allocation
;
DMAPHYSBUFSIZE      equ 1000h
DMAHALFBUFSIZE      equ  800H           ; 2k bytes
P_SIZE              equ 1000H           ; 4k bytes

;
; Addresses of DMA control registers (non-8-bit-channel specific)
;
DMA         equ 00H                     ; base address of controller
DMASMR      equ DMA+10                  ; single mask register
DMAMOD      equ DMA+11                  ; mode register
DMACLR      equ DMA+12                  ; byte pointer latch reset

;
; DMA command codes WITHOUT channel number (bits 0 and 1)
;
DMARD       equ 01000101b               ; single mode port -> memory (DSP read)
DMAWR       equ 01001001b               ; single mode memory -> port (DSP write)
DMARDA      equ 01010100b               ; auto init DSP read
DMAWRA      equ 01011000b               ; auto init DSP write
DMAAUTO     equ 00010000b               ; auto init mask

;
; Port offsets from base port for Sound Blaster DSP
;
DSP_PORT_CMSD0      equ 00h    ; C/MS music voice 1-6 data port, write only
DSP_PORT_CMSR0      equ 01h    ; C/MS music voice 1-6 register port, write only
DSP_PORT_CMSD1      equ 02h    ; C/MS music voice 7-12 data port, write only
DSP_PORT_CMSR1      equ 03h    ; C/MS music voice 7-12 register port, write only

DSP_PORT_MIXREG     equ 04h    ; Pro card mixer address register, write only
DSP_PORT_MIXDATA    equ 05h    ; Pro card mixer data register, read/write

DSP_PORT_RESET      equ 06h    ; DSP Reset, write only
DSP_PORT_07h        equ 07h    ; reserved port

DSP_PORT_FMD0       equ 08h    ; FM music data/status port, read/write 
DSP_PORT_FMR0       equ 09h    ; FM music data/status port, write only

DSP_PORT_RDDATA     equ 0Ah    ; DSP Read data, read only
DSP_PORT_0Bh        equ 0Bh    ; reserved port
DSP_PORT_WRDATA     equ 0Ch    ; DSP Write data or command, write
DSP_PORT_WRBUSY     equ 0Ch    ; DSP Write buffer status (bit 7), read
DSP_PORT_0Dh        equ 0Dh    ; reserved port
DSP_PORT_DATAAVAIL  equ 0Eh    ; DSP Data available status (bit 7), read only
DSP_PORT_0Fh        equ 0Fh    ; reserved port (used on Pro cards)

;
; DSP commands
;
MIDIRD          equ 31h         ; interrupt-driven midi input
MIDIRD2         equ 35h         ; interrupt-driven midi input
MIDIRDTS        equ 37h         ; interrupt-driven midi input (time-stamped)
MIDIWR          equ 38h         ; midi output
WAVEWR          equ 14h         ; interrupt-driven 8 bit linear wave output
WAVEWRA         equ 1Ch         ; auto mode 8 bit out
WAVERD          equ 24h         ; interrupt-driven 8 bit linear wave input
WAVERDA         equ 2Ch         ; auto mode 8 bit in
HIGHSPEED       equ 90h         ; high speed mode
SETSAMPRATE     equ 40h         ; set sample rate
SETBLCKSIZE     equ 48h         ; set block size
SPKRON          equ 0D1h        ; speaker on
SPKROFF         equ 0D3h        ; speaker on
SPKRSTATUS      equ 0D8h        ; speaker status (0=off, FF=on)
HALTDMA         equ 0D0h        ; halt DMA
CONTDMA         equ 0D4h        ; continue DMA
STOPAUTO        equ 0DAh        ; stop DMA autoinit mode
GETDSPVER       equ 0E1h        ; get dsp version
GENERATEINT     equ 0F2h        ; cause sndblst to generate an interrupt.

;
; who's using the interrupt
;
INT_FREE        equ 0           ; nobody
INT_WAVEOUT     equ 1           ; wave output
INT_WAVEIN      equ 2           ; wave input
INT_MIDIIN      equ 3           ; midi input

;
; offsets of mixer ports
;
MIXADDRPORT     equ 4h
MIXDATAPORT     equ 5h
WARMSTARTPORT   equ 6h

;
; mixer registers
;
DATARESETREG    equ 00h
MSTRVOLREG      equ 02h
VOICEVOLREG     equ 04h
FMVOLREG        equ 06h
CDVOLREG        equ 08h
MICVOLREG       equ 0Ah
ADCINPREG       equ 0Ch
VTSCREG         equ 0Eh

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; EnterCrit
;
;   this macro saves the current state of the interrupt flag on the
;   stack, and then disables interrupts
;
; Registers Destroyed:
;   BX, FLAGS
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
EnterCrit macro
    local   no_cli
    pushf
    pushf
    pop     bx
    test    bh, 2           ; if interrupts are already off, dont blow
    jz      no_cli          ; ... ~300 clocks doing the cli
    cli
no_cli:
endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; LeaveCrit
;   this macro restores the interrupt state saved by EnterCrit
;
; Registers Destroyed:
;   BX, FLAGS
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
LeaveCrit macro reg
    local   no_sti
    pop     bx
    test    bh, 2
    jz      no_sti
    sti
no_sti:
endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; pause
;   macro to insure that an instruction fetch occurs between IN and/or
;   OUT instructions on the PC-AT machine ALSO defeat the prefetch cache!
;
; Registers Destroyed:
;   None
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
pause   macro               
        jmp     $+2         
endm                

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   debug support
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ifdef DEBUG
    extrn OutputDebugStr:far          ; mmsystem
    extrn _wDebugLevel:word           ; initc.c
endif

D1 macro text
    DOUT 1, < ",13,10,"SNDBLST: &text&>
    endm
D2 macro text
    DOUT 2, < &text&>
    endm
D3 macro text
    DOUT 3, < &text&>
    endm
D4 macro text
    DOUT 4, < &text&>
    endm

DOUT macro level, text
    local   string_buffer
    local   wrong_level

ifdef DEBUG

_DATA segment
string_buffer label byte
    db      "&text&", 0
_DATA ends

    cmp     [_wDebugLevel], level
    jl      wrong_level
    push    ds
    push    DataOFFSET string_buffer
    call    OutputDebugStr
wrong_level:
endif
    endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   assert macros
;
;   AssertF byte        -- fail iff byte==0
;   AssertT byte        -- fail iff byte!=0
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

AssertF     macro exp
    local   assert_ok
ifdef DEBUG
    push    ax
    
    mov     al, exp
    or      al, al
    jnz     assert_ok

    D1      <AssertF fail (&exp&)>
    int     3

assert_ok:
    pop     ax
endif
    endm

AssertT     macro exp
    local   assert_ok
ifdef DEBUG
    push    ax
    
    mov     al, exp
    or      al, al
    jz      assert_ok

    D1      <AssertT fail (&exp&)>
    int     3

assert_ok:
    pop     ax
endif
    endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; reminder macro
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

BUG macro stuff
if1
%out ----&stuff&
endif
endm
