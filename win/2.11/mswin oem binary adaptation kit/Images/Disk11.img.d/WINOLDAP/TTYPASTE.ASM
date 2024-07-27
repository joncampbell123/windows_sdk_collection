page ,132
.xlist
include cmacros.inc
include tty.inc
.list

title   **      PASTE / DOS for BAD Old Applications (TTYHP2)

;------  PASTE  --------------------------------------------------------------
;
;  This module contains:
;
;  1)  The code to set up and process a PASTE operation for BAD old applications
;      running under MS-WINDOWS. A PASTE is invoked when the user enters the
;      the MCP menu and selects the PASTE operation while running a bad app.
;      The PASTE option is only available in the menu if the data type of the
;      grabber is one of the following:
;
;               1 = Text screen grab  or copy (PC8 character set)
;               4 = Text screen grab  or copy (HP8 character set)
;               5 = Text from Clipboard (PC8 character set)
;
;
;  Programmer:  Tuan Nguyen, Ralph Peterson
;
;  Date:        05/01/86
;               09/20/86        ~~RP~~ Pass control to the previous int16H
;                               if function is AH=2.
;
;------------------------------------------------------------------------------

page

;---------------------------
;Structure of Paste (grabber) buffer if paste is valid
;
PasteStruc      Struc

PasteType       dw      ?       ;must be a 4 to paste
PasteSize       dw      ?       ;size of the buffer
PasteStart      dw      ?       ;Data begins here

PasteStruc      Ends

externW         Bufseg          ;segment of Paste (grabber) buffer
externW         BufSize         ;size of Paste (grabber) buffer (maximum)

externFP        GetCurrentTask
externNP        <GetDataX, PeekDataX, TTYPaste> ; Declared in VECSEL
externNP        <GoGrab>                        ;Declared in Vectask
externNP        <CheckInput, GetInput>

externD         PrevInt16Proc
externW         <TopPDB>
ExternW         <WinoldapDS, fExtend>   ; (VECDOS)


;++++++  GENERAL EQUATES  +++++++++++++++++++++++++++++++++++++++++++++++++++++
;

CF_TEXT         equ     1               ; Clipboard TEXT mode
;
EnableGrab      equ     3               ;Enable grabber function code for GoGrab
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

page

sbegin  code
        assumes         cs,code
        assumes         ds,data

        public          StartPASTE
        public          PASTEInt16, PASTEKey, PASTECheck
        public          GetOldApDS

;++++++  DATA DECLARATIONS  +++++++++++++++++++++++++++++++++++++++++++++++++++
;
PastePos        dw      0FFFFH          ;Position to retrieve chars from
                                        ;Paste buffer : Initialized to 0 by
                                        ;StartPaste and set to 0FFFFH when no
                                        ;more chars
OldApInt16      dd      0
GLOBALW         fPasteActive, 0         ; 1 if PASTEINT16 handler installed
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

.xlist
subttl  StartPASTE: Install PASTE code
page
.list
;=======   StartPASTE  ========================================================
;
;  Description:
;    Install the PASTE Int16 handler
;    Save current int16 vector locally
;
;  Inputs:
;    DS:        WINOLDAP data segment
;
;  Registers:
;    AX, BX, DX, ES
;
;==============================================================================

StartPASTE      PROC    NEAR

        mov     CS:[PastePos],PasteStart
        mov     AX, 3516H               ; Get current Int16 vector
        INT     21H                     ; and save it
        mov     word ptr CS:[fPasteActive],1
        mov     Word Ptr CS:[OldApInt16], BX
        mov     Word Ptr CS:[OldApInt16+2], ES

        mov     DX, CodeOffset PASTEInt16
        mov     AX, 2516H               ; Install PASTE Int16 handler
        push    DS
        push    CS
        pop     DS
        cli
        int     21H
        sti
        pop     DS
        ret

StartPASTE      ENDP

.xlist
subttl  PASTEInt16: PASTE handler
page
.list
;========  PASTEInt16  ========================================================
;
;  Description:
;    Route data from Clipboard to console input calls from BAD Old App.
;    Assumes that inputs are made using BIOS Int16 type of calls.
;    Do not trap calls made from ouside WINDOWS (it would be coming from the
;    SYMDEB).
;
;==============================================================================

PASTEInt16      PROC    FAR

        push    BP                      ; Use BP as index to items on stack
        mov     BP, SP
        mov     BP, SS:[BP+4]           ; BP= Caller's code segment
        cmp     CS:[TopPDB], BP         ; Goto ROM BIOS if caller is not within
        jbe     PInt16b                 ; WINDOWS. It must then come from SYMDEB

PInt16a:
        pop     BP

; jump to most recent int 16             ~~RP 091886~~
;       jmp     CS:[PrevInt16Proc]
        jmp     CS:[OldApInt16]         ;~~RP 091886~~
PInt16b:
        sti
        cmp     AH, 2                   ; go to ROM BIOS if Shift status call
;
; don't just pass on code 2, pass on all codes          ~~RP 091886~~
;       jnz     PInt16b0

        jb      PInt16b0                ;~~RP 091886~~
        ja      PInt16b1                ;do not mod return value ~~RP 091886~~
        pushf
; call  most recent int 16
;       call    CS:[PrevInt16Proc]      ; Shift Status returned in AL
        call    CS:[OldApInt16]         ;~~RP 091886~~
        and     AL, 11110111B           ; Always force ALT state to be disabled
        jmp     PInt16Ret0

PInt16b1:                               ;~~RP 091886~~
        pushf                           ;~~RP 091886~~
        call    CS:[OldApInt16]         ;~~RP 091886~~
        jmp     PInt16Ret0              ;~~RP 091886~~

Pint16b0:
        push    ES                      ; Save Context
        push    DS
        push    DI
        push    SI
        push    DX
        push    CX
        push    BX
        call    GetOldApDS              ; Set DS= WINOLDAP DATA segment

        or      AH, AH
        jnz     PInt16d                 ; Ckeck Get status call if AH <> 0
        call    PASTEKey                ; Get data from Clipboard and return
        jnc     PInt16Ret               ; Scancode + ASCII.

PInt16c:
        push    DS                      ;save DS for grabber re-installation
        lds     DX, CS:[OldApInt16]     ; Mark end of PASTE operation
        mov     AX, 2516H               ; Install back the Oldap Int16 handler
        cli
        int     21H
        pop     DS
        mov     word ptr CS:[fPasteActive],0 ; Mark an end of paste
        sti

PInt16Flush:
        call    CheckInput              ; Flush Keystrokes pending in the
        jz      PInt16Ret               ; ROM BIOS keyboard buffer. ZF= 1
        call    GetInput
        jmp     PInt16Flush

PInt16d:
        cmp     AH, 1                   ; Make sure it is a valid call
        jnz     PInt16Ret               ; Return if invalid
        call    PASTECheck              ; Check Status call: AH= 1
        jc      PInt16c                 ; CY= 1: End of PASTE

PInt16Ret:
        pop     BX                      ; Restore context and return
        pop     CX
        pop     DX
        pop     SI
        pop     DI
        pop     DS
        pop     ES

PInt16Ret0:
        pop     BP
        ret     2                       ; Interrupt ret with new flags set

PASTEInt16      ENDP

page
;========  PASTEKey  ==========================================================
;
;  Description:
;    Emulates the BIOS Int16, AH=0 call.
;    Since data retrieved from the Clipboard is ASCII type, no scancode
;    translation is needed. The scancode returned is hard-coded 0.
;    Data is now read from the Clipboard data area.
;
;  Inputs:
;    DS:        WINOLDAP data segment
;
;  Ouputs:
;    CF:        1: End of Paste
;               0: AL= ASCII, AH= 0 (see IBM tech Ref for detailed encoding.)
;
;  Registers:
;    AX,
;
;==============================================================================

PASTEKey        PROC    NEAR

        call    GetPasteData            ; Get ASCII from Clipboard
        jc      PKeyEnd                 ; show End of Paste
        xor     AH, AH                  ; Return AL,AH = ASCII, 0
        cmp     AL,0DH                  ; is it CR
        jnz     PKey1
        mov     AH,28                   ;CR scancode in AH
PKey1:
        clc                             ; Always return OK
        jmp     PKeyRet

PKeyEnd:                                ; Set CY to mark an end of PASTE
        stc
PKeyRet:
        ret

PASTEkey        ENDP

page
;========  PASTECheck  ========================================================
;
;  Description:
;    Emulates the Int16, AH= 1 function.
;    Data is now retrieved from the clipboard and the corresponding scan code
;    is hardcoded to 0.
;
;  Inputs:
;    DS:        WINOLDAP data segment
;
;  Outputs:
;    ZF:        0: AL:  ASCII Value
;                  AH:  0: No scancode translation
;               1: No input available: End of Paste
;   CF:         0: Operation OK
;               1: End Of Paste
;
;  Registers:
;
;==============================================================================

PASTECheck      PROC    NEAR

        call    LookPasteData           ; DL= ASCII value
        jc      PCheckEnd               ;jump if end of paste
        xor     AH, AH                  ; Hard-coded scancode = 0
        or      AL, AL                  ; Set ZF= 0 since AH is always <> 0
        clc                             ; ZF= CF= 0: OK condition
        ret

PCheckEnd:
        mov     CS:[PastePos],0FFFFH    ;show  End of Paste
        stc                             ; Flag End of Paste; ZF= 1
        ret

PASTECheck      ENDP

;========  GetOldApDS  ========================================================
;
;  Description:
;    Get the data segment of WINOLDAP.
;    It makes use of an undocumented call to WINDOWS and is thus version-dependent
;
;  Outputs:
;    DS:        WINOLDAP data segment
;
;==============================================================================

GetOldApDS      PROC    NEAR

        push    AX
        push    DX

        mov     DS, CS:[WinoldapDS]     ; Assume that we are in Extended mode
        cmp     CS:[fExtend], 1         ; Are we in an extended mode
        jz      GetDSRet                ; Yes

        ccall   GetCurrentTask          ; AX: returned task handle
        mov     DS, AX
        mov     DS, DS:[TDB_TaskSS]     ; Get WINOLDAP segment from TDB

GetDSRet:
        pop     DX
        pop     AX
        ret

GetOldApDS      ENDP



;============LookPasteData===============================================
;
;       Description:
;               Retrieves a character from the Paste buffer and checks
;               if this is the end of the buffer.
;
;       Entry:
;
;       Exit:
;               AL = char if available
;               CY set if end of paste
;
;       Modified:
;               AX, SI
;
;============================================================================

LookPasteData   PROC    NEAR

        push    DS
        mov     AX,[BufSeg]
        mov     DS,AX
        mov     SI, Word ptr CS:[PastePos]
        cmp     SI,0FFFFH
        jz      LPD8
        mov     AX, Word ptr DS:[SI]
        or      AL,AL                   ;null character
        clc
        jnz     LPD9
LPD8:
        stc
LPD9:
        pop     DS
        ret

LookPasteData   ENDP

;==============GetPasteData=========================================
;
;       Description:
;               Retrieves a character from the Paste buffer and updates
;               the pointer into the buffer.  Also reports if no more
;               characters are available.
;
;       Entry:
;
;       Exit:
;               AL = character
;               CY set if no more chars
;
;       Modified:
;               AX, SI
;
;========================================================================

GetPasteData    PROC    NEAR

        call    LookPasteData
        jc      GPD8
        inc     SI              ;update pointer to next char
        cmp     AX,0A0DH        ;is it CR,LF
        jnz     GPD5
        inc     SI              ; update pointer to skip linefeed
GPD5:
        mov     word ptr CS:[PastePos],SI
        xor     AH,AH
        clc
GPD8:
        ret

GetPasteData    ENDP


send    Code
        END
