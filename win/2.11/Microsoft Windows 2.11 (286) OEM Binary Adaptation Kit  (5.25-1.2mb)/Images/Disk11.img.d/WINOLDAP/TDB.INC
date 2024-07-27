;
; Task Data Block
;
;   Contains all task specific data.
;

numTaskInts	equ	4
;
; Task Data is located at the top of the stack.
;
TDB             STRUC
TDB_next        DW  ?           ; next task in queue
TDB_taskSP      DW  ?           ; Saved SS:SP for this task
TDB_taskSS      DW  ?           ;
TDB_nEvents     DW  ?           ; Task event counter
TDB_priority    DB  ?           ; Task priority (0 is highest)
TDB_flags       DB  ?           ; Task flags (unused so far)

;
; These three fields must match locations in DS, as defined in WINSTART.ASM
; DO NOT MOVE THEM!
;
TDB_pStackTop   DW  ?           ; base of stack
TDB_pStackMin   DW  ?           ; minimum value of SP
TDB_pStackBot   DW  ?           ; bottom of SP

TDB_Module      DW  ?           ; Task module handle to free in killtask
TDB_Queue       DW  ?           ; Task Event Queue pointer

TDB_SignalProc  DD  ?           ; Task Signal procedure address
TDB_SwitchProc  DD  ?           ; Task extended state switch procedure address
TDB_Interchange DD  ?           ; Task data interchange procedure address

TDB_INTVECS     DD  numTaskInts DUP (?) ; Task specfic hardware interrupts
TDB_EMMSave     DW  ?           ; Offset within TDB of EMM save area
TDB_PDB         DW  ?           ; MSDOS Process Data Block (PDB)
TDB_DTA         DD  ?           ; MSDOS Disk Transfer Address
TDB_BRKF        DW  ?           ; MSDOS Break flag
TDB_Drive       DB  ?           ; MSDOS current drive
TDB_ErrorMode   DB  ?           ; MSDOS error mode byte
TDB_Directory   DB  65 DUP (?)  ; MSDOS current directory
                DB   1 DUP (?)  ; extra bytes to keep word aligned
TDB_sig         DW  ?           ; Signature word to detect bogus code
TDB             ENDS
TDBsize = SIZE TDB

; signature word used to check validity of a TDB
;
TDB_SIGNATURE  equ     'DT'

Task_Regs       struc
Task_DX         dw      ?
Task_BX         dw      ?
Task_ES         dw      ?
Task_CX         dw      ?
Task_AX         dw      ?
Task_DI         dw      ?
Task_SI         dw      ?
Task_DS         dw      ?
Task_BP         dw      ?
Task_IP         dw      ?
Task_CS         dw      ?
Task_Regs       ends
