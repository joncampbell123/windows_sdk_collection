;***************************************************************************
;*  THASM.ASM
;*
;*      Assembly support routines for the TOOLHELPER sample program
;*
;***************************************************************************

;** Set up values for cMacros
?PLM    =       1                       ;PASCAL-style parameter handling
?WIN    =       0                       ;No Windows prolog/epilog

;** Symbols
FAULT_1         EQU     2001
FAULT_2         EQU     2002
FAULT_3         EQU     2003
FAULT_4         EQU     2004
FAULT_5         EQU     2005
FAULT_6         EQU     2006
FAULT_7         EQU     2007
FAULT_8         EQU     2008

.286c

        INCLUDE cMacros.INC
        INCLUDE TOOLHELP.INC

sBegin  DATA

ODSTest DB      'THSample: Test Output Debug String', 0dh, 0ah, 0

sEnd

;** Functions
?PLM = 0
externNP MyCFaultHandler
?PLM = 1
externFP TerminateApp
externFP OutputDebugString
externFP GetWindowWord

sBegin  CODE
        ASSUMES cs,CODE

;  Fault
;
;       Causes one of three faults:
;               FAULT_1 --> GP Fault
;               FAULT_2 --> Illegal opcode
;               FAULT_3 --> Divide by zero
;               FAULT_4 --> Int 1
;               FAULT_5 --> Int 3
;               FAULT_6 --> RIP
;               FAULT_7 --> OutputDebugString
;               FAULT_8 --> Stack Fault

cProc   Fault, <PUBLIC,NEAR>, <si,di,ds>
        parmW   wType
cBegin
        mov     ax,wType                ;Get the type
        cmp     ax,FAULT_1              ;GP Fault?
        je      GPFault                 ;Yes
        cmp     ax,FAULT_2              ;Illegal opcode?
        je      IllOpcode               ;Yes
        cmp     ax,FAULT_3              ;Divide by zero?
        je      DivZero                 ;Yes
        cmp     ax,FAULT_4              ;Int 1?
        je      Int1
        cmp     ax,FAULT_5              ;Int 3?
        je      Int3
        cmp     ax,FAULT_6              ;RIP?
        je      Rip
        cmp     ax,FAULT_7              ;ODS?
        je      ODS
        cmp     ax,FAULT_8              ;Stack Fault?
        je      StackFault
        jmp     F_End                   ;Not recognized, get out

        ;** Produce GP Fault
GPFault:
        mov     cs:[0],BYTE PTR 55h     ;Can't write to CS
        jmp     F_End                   ;If restarted

        ;** Illegal opcode
IllOpcode:
        DB      0c4h                    ;LES Opcode
        DB      0c0h                    ;  LES AX,AX (should make #UD Excep)
        jmp     F_End                   ;In case it's not

        ;** Divide by zero
DivZero:
        xor     bx,bx                   ;Get a zero
        div     bx                      ;Divide by it
        jmp     F_End                   ;Done

Int1:   int     1                       ;Cause trace interrupt
        jmp     F_End                   ;Get out

Int3:   int     3                       ;Cause software int
        jmp     F_End

Rip:    mov     ax,-1                   ;Bad handle
        cCall   GetWindowWord, <ax,ax>  ;Guaranteed RIP
        jmp     F_End

ODS:    mov     ax,OFFSET ODSTest       ;Test string
        cCall   OutputDebugString, <ds,ax>
        jmp     F_End

StackFault:
        mov     ax,ss:[bp+7fffh]        ;Causes stack fault
        jmp     F_End

F_End:

cEnd


;  MyFaultHandler
;       Handles all faults passed to it.  Makes the processing ready
;       for a C function, then calls it.  The handler is reentrant
;       The entry frame looks like this:
;               ------------
;       SP--->  |  Ret IP  |  [SP + 00h]
;               |  Ret CS  |  [SP + 02h]
;               |    AX    |  [SP + 04h]
;               |Exception#|  [SP + 06h]
;               |  Handle  |  [SP + 08h]
;               |    IP    |  [SP + 0Ah]
;               |    CS    |  [SP + 0Ch]
;               |   Flags  |  [SP + 0Eh]
;               ------------
;

cProc   MyFaultHandler, <FAR,PUBLIC>
cBegin  NOGEN

        push    bp                      ;Make a stack frame
        mov     bp,sp
        pusha                           ;Save all registers
        push    ds
        push    es

        ;** Get instance data segment and prepare for C call
        mov     ds,ax                   ;Since this function uses
                                        ;  MakeProcInstance(), AX has the
                                        ;  DS value in it.

        ;** Call the C function:  It returns 0 to nuke the app,
        ;*      1 to restart the instruction, 2 to chain on
        ;*      The entry frame to the function looks like this:
        ;*              ------------
        ;*       BP---->|  Old BP  |  [BP + 00h] <-- Added by C routine
        ;*              |    ES    |  [BP + 02h]
        ;*              |    DS    |  [BP + 04h]
        ;*              |    DI    |  [BP + 06h]
        ;*              |    SI    |  [BP + 08h]
        ;*              |    BP    |  [BP + 0Ah]
        ;*              | Dummy SP |  [BP + 0Ch]
        ;*              |    BX    |  [BP + 0Eh]
        ;*              |    DX    |  [BP + 10h]
        ;*              |    CX    |  [BP + 12h]
        ;*              |    AX    |  [BP + 14h]
        ;*   Old BP---->|Old,Old BP|  [BP + 16h]
        ;*              |  Ret IP  |  [BP + 18h]
        ;*              |  Ret CS  |  [BP + 1Ah]
        ;*              |    AX    |  [BP + 1Ch]
        ;*              |Exception#|  [BP + 1Eh]
        ;*              |  Handle  |  [BP + 20h]
        ;*              |    IP    |  [BP + 22h]
        ;*              |    CS    |  [BP + 24h]
        ;*              |   Flags  |  [BP + 26h]
        ;*              ------------
        ;**
        cCall   MyCFaultHandler

        ;** Decode the return value
        or      ax,ax                   ;Check for zero
        jz      MFH_NukeApp             ;Nuke it
        dec     ax                      ;Check for 1
        jz      MFH_Restart             ;Restart instruction

MFH_ChainOn:
        pop     es                      ;Chain on to next fault handler
        pop     ds
        popa
        pop     bp
        retf

MFH_Restart:
        pop     es                      ;Clear stack
        pop     ds
        popa
        pop     bp
        add     sp,10                   ;Clear the return stuff
        iret                            ;Restart instruction

MFH_NukeApp:
        pop     es                      ;Clear stack
        pop     ds
        popa
        pop     bp
        add     sp,10                   ;Point to IRET frame
        cCall   TerminateApp, <0,NO_UAE_BOX>

cEnd    NOGEN

sEnd

        END
