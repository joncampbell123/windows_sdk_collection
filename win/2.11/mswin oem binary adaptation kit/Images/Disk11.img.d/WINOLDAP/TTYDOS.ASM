page ,132

        title   INT21 - INT 21 handler for Extended mode

; ?NODATA = 1
include kernel.inc
include sched.inc
include	mac.inc

;++++++++  Vectra Extension  ++++++++++++++++++++++++++++++++++++++++++++++++++
;
;  - Trap DOS function calls and route Console I/O DOS calls to TTYFUNC. This
;    allows the console I/O part in DOS to be reentrant.   
;
;  - If the DOS application is declared NON-Switchable (WORST), then the
;    only DOS trap is the Stay Resident function.
;
;  Author:      Tuan Nguyen
;
;  History:	06/10/86	Microsoft V.1.03 adaptation
;		08/09/86	~~Do not trap I/O if app is WORST
;		09/09/86	~~TN~~ Patch offset of int21 in DisableInt21
;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

;
; Exit codes in upper byte
;
Exit_terminate      EQU     0
Exit_abort          EQU     0
Exit_Ctrl_C         EQU     1
Exit_Hard_Error     EQU     2
Exit_Keep_process   EQU     3

user_FG     equ word ptr [bp+6]
user_FL     equ byte ptr [bp+6]
user_CS     equ word ptr [bp+4]
user_IP     equ word ptr [bp+2]
user_BP     equ word ptr [bp]
user_DS     equ word ptr [bp-2]
user_DX     equ word ptr [bp-4]
user_DSDX   equ dword ptr [bp-4]
user_ES     equ word ptr [bp-6]
user_BX     equ word ptr [bp-8]
user_ESBX   equ dword ptr [bp-8]
user_AX     equ word ptr [bp-10]
user_AL     equ byte ptr [bp-10]
user_CX     equ word ptr [bp-12]
user_SI     equ word ptr [bp-14]
user_DI     equ word ptr [bp-16]

;~~TN 080986~~
SwiHook	    equ	00001000B		; Switcher hooks in

sBegin 	DATA
assumes DS, DATA

CurrentDisk	dw	0		; Saved current Oldapp disk
		db	'\\'		; Path root delimiter
CurrentDir	dw	-1
		dw	32 dup(0)	; Saved current directory
 
sEnd	DATA

page
sBegin  CODE
assumes CS,CODE

;!! Debugging purpose
;
	public	Read_keyboard, Interchange2, TestConDevice2
;*****

	public	int23Handler

ExternB		fhook			; (VECTASK) ~~TN 080986 ~~
ExternW 	TopPDB			; (VOEM)
ExternFP	Interchange		; (VECFUNC)
ExternFP	GetCurrentTask		; (KERNEL)
;
GlobalW		WinoldapDS, 0		; DATA segment of parent Winoldap
GlobalW		Win_taskSS, 0		; Winoldap Saved stack segment
GlobalW		Win_taskSP, 0		; Winoldap saved Stack Pointer
GlobalD		OrgInt21Proc, 0		; Original DOS int 21 vector
GlobalD		PDB_DOSCall, 0		; Original long call to DOS in PSP
GlobalW		fint21, 0		; =1: Int 21 trapped
GlobalB		DOS_version, 0		; DOS major version number
GlobalB		DOS_revision,0		; DOS minor version number

GlobalD		lpSftLink,0		; -> original end of sft table
GlobalD		lpDOSData,0		; -> DOS global state
GlobalD		lpFileTable,0		; -> beginning of Sft chain
GlobalW		FileEntrySize,0         ; Size of one sft entry

PrevInt24Proc 	dd	0   		; -> previous INT 24h handler
prevBCON        dd	0   		; -> previous DOS con device

page
;******************************************************************************
;
;                         WINDOWS mediated system calls.
;
;
;   Windows mediates certain system calls.   Several matters complicate
;   the mediation:
;
;   a.  MSDOS uses registers to pass arguments.  Thus, registers AND ANY RETURN
;       VALUES in registers must be preserved across tasks.
;
;   b.  MSDOS stores global state information that must be preserved on a task
;       by task basis.
;
;   c.  To handle multiple exec calls, the notion of a "parent" task is introduced.
;
;
;******************************************************************************

DosTraps    DW  0
tmp0        DW  ?
tmp1        DW  ?
tmp2        DW  ?

CtrlSP      DW  ?

DosTrap1        db      3eh                     ; close
                dw      FileHandleCall
                db      42H
                dw      FileHandleCall
                db      45H
                dw      FileHandleCall
                db      46H
                dw      FileHandleCall
                db      57H
                dw      FileHandleCall
                db      5CH
                dw      FileHandleCall

DosTrap2        db      06h
                dw      Direct_Console_IO
                db      0bh
                dw      Check_Keyboard_Status
                db      44h
                dw      Xenix_Status
                db      07h
                dw      Direct_Console_Input
                db      08h
                dw      Read_Keyboard
                db      0ah
                dw      Buffered_Keyboard_Input
                db      0ch
                dw      Flush_Buffer
                db      01h
                dw      Read_Keyboard_and_Echo
                db      3fh
                dw      Xenix_Read

DosTrap3        db      25h
                dw      Set_Vector
                db      31H
                dw      StayResident
DosDefault      db      ?
                dw      PassOnThrough

page

cProc   EnDOS,<NEAR>,<BP,DS>
cBegin
	mov	DS, CS:[WinoldapDS]	; DS= our DATA

;~~TN 080986~~
        mov     ax,codeOffset DosTrap3	; No
        mov     cs:[DosTraps], ax
	test	CS:[fhook], SwiHook	; Is app context-switchable?
	jz	ends2			; NO
	mov	ax, codeOffset DosTrap1
	mov	CS:[DosTraps], ax
;~~
ends2:
	lES     BX, CS:[lpDOSData]
        mov     ax,codeOffset ConDev
        xchg    ax,es:[bx+12]
        mov     word ptr cs:[prevBCON],ax
        mov     ax,cs
        xchg    ax,es:[bx+14]
        mov     word ptr cs:[prevBCON+2],ax

;  Restore current disk and directory only if the directory is valid (<> 0)

	mov	AX, [CurrentDir]
	cmp	AX, -1			; First Time?
	jne	EnDNotFirst
;
;~~tqn 062487
;
;
; Save the original int24handler
; Install our int24handler for the first time.

	xor	AX, AX
	mov	[CurrentDir], AX
	mov	ES, AX
	mov	BX, 4 *24h
	lES	DX, ES:[BX]
	mov	word ptr CS:[PrevInt24Proc], DX
	mov	word ptr CS:[PrevInt24Proc+2], ES
	mov	ES, AX
	lea	AX, Int24Handler
	mov	ES:[BX], AX
	mov	ES:[BX +2], CS
	jmp short ends1

EnDNotFirst:
;~~
	mov	BX, AX
	mov    	AH, 0Eh
	mov	DL, Byte Ptr [CurrentDisk]
	pushf
	call	CS:[OrgInt21Proc]

	or 	BX ,BX
	jz	ends1			; Don't restore invalid directory
	lea	DX, CurrentDir -1
	mov	AH, 3BH
	pushf
	call	CS:[OrgInt21Proc]

ends1:
cEnd			



cProc   DisDOS,<NEAR>,<BP,DS>

cBegin
;
;~~tqn 061587
;
; we have to restore the corect CON device before making any DOS call since
; the original DOS dispatcher will issue a read command to CON in DSKSTATCHK
; call.	since the original CON device is now enabled, we have to make sure
; that ^C state is disabled throughout the disk swapping operations.
;
	lES 	BX, CS:[lpDOSDATA]	; Restore original Console driver
        lds     ax,cs:[prevBCON]
        mov     es:[bx+12],ax
        mov     es:[bx+14],ds

	mov 	DS, CS:[WinOldApDS]	; DS= Our DATA
;~~
        mov     ax,3301h
	xor	DL, DL			; Make sure ^C State checking is OFF
	pushf
	call	CS:[OrgInt21Proc]

;  Save current disk and directory before a context switch

	mov	AH, 19H			; Get current disk
	pushf
	call	CS:[OrgInt21Proc]	; AL= drive number
	mov	Byte Ptr [CurrentDisk], AL

	mov	AH, 47H			; Get current directory and save
	inc	AL
	mov	DL, AL
	lea	SI, CurrentDir		; DS:SI -> Save current directory
	pushf
	call	CS:[OrgInt21Proc]
	jnc	ddos1
	mov	Word Ptr [CurrentDir], 0 ; Mark directory as invalid
				
ddos1:
cEnd

page

cProc   EnableInt21,<PUBLIC,NEAR>

cBegin
        mov     ax,1
        xchg    ax,cs:[fInt21]          ; set hook count to 1
        or      ax,ax                   ; was it zero?
        jz      enhook                 
	jmp	ena21			; no, just leave
EnHook:
        cCall   EnDOS

            ;
            ; Setup INT 21h to go through our routine.  Use absolute writes.
            ; Int21 will not set itself under the debugger.
            ;
	push	DS

; Save and replace the DOSCall pointer in TopPDB

	mov	DS, CS:[TopPDB]
	mov	BX, 5  			; Offset to CPM Call in TopPDB
	LES	AX, Dword Ptr [BX +1]
	mov	Word Ptr CS:[PDB_DOSCall], AX
	mov	Word Ptr CS:[PDB_DOSCall +2], ES

	mov	AX, CodeOffset PDB_Call_System_Entry
	mov	Word Ptr [BX +1], AX
	mov	Word Ptr [BX +3], CS
						 
        xor     bx,bx
        mov     es,bx
        mov     bx,20h*4
    cli					  
        mov     word ptr es:[bx],codeOFFSET Int20Handler
        mov     word ptr es:[bx +2],cs

        mov     word ptr es:[bx+4*1  ],codeOFFSET Int21Handler
        mov     word ptr es:[bx+4*1+2],cs	    

        mov     word ptr es:[bx+4*7  ],codeOffset Int27Handler
        mov     word ptr es:[bx+4*7+2],cs
    sti
	pop	DS
ena21:
cEnd


cProc   DisableInt21,<PUBLIC,NEAR>

cBegin
	push	DS
;
;  Current disk and directory are reset back to the TDB settings on the  
;  next task switch.
;
        xor     ax,ax
        xchg    ax,cs:[fInt21]          ; set hook count to zero
        or      ax,ax                   ; was it non zero?
        jz      dis21                   ; no, just leave

    cli

; Restore the saved PDB_Call entry in the TopPDB

DisR1:					;~~TN 080986~~
	mov	DS, CS:[TopPDB]
	LES	AX, CS:[PDB_DOSCall]
	mov	BX, 5			; BX= Offset to CPM Call in TopPDB
	mov	Word Ptr [BX +1], AX				 
	mov	Word Ptr [BX +3], ES
		
        xor     bx,bx                       ; Windows int 21 handler prevents
        mov     es,bx                       ; int 21 update.  We do it directly.
        mov     bx, 4 *20H

	mov	AX, CS			; Patch int20, int21 vector segment with the
	mov	ES:[BX +2], AX		; current code segment
	mov	ES:[BX +6], AX
	mov	ES:[BX+18], AX		;~~tqn 062487 Patch int24 segment

	lea	AX, Int21Entry		; Patch int21 offset
	mov	ES:[BX +4], AX
    sti	

        cCall   DisDOS		        ; take out all hooks

dis21:
	pop	DS
cEnd		  

;******************************************************************************
;
;   PDB call system entry point.  One way to call the DOS is to do a long call
;   to location 5 in the PSP.  Location PSP:5 contains a long call into the
;   DOS dispatcher.
;
;******************************************************************************

PDB_Call_System_Entry   proc    far  ; PDB_Call_System entry point.

	pop 	AX			; IP from the long call at 5
	pop 	AX			; Segment from the long call at 5
	pop 	CS:[tmp1]		; IP from the call to 5
	pushf				; Reorder stack to make it compatible
	cli
	push	AX			; with Int 21H convention
	push	CS:[tmp1]
	sti
	mov	AH, CL			; Get DOS function
	jmp	Int21Handler		; Far jump to Int21Handler

PDB_Call_System_Entry   endp

;******************************************************************************
;
;   Interrupt 27 handler.
;
;******************************************************************************
labelFP <Int27Handler>
        mov     ax,3100H                ; Int27
        jmp     short Int21Entry

;******************************************************************************
;
;   Interrupt 20 handler.
;
;******************************************************************************
labelNP <PUBLIC,Int20Handler>

        xor     ax, ax			; it is just a terminate

;******************************************************************************
;
;   Interrupt 21 handler.			       

;
;******************************************************************************
labelNP <PUBLIC,Int21Handler>

Int21Entry:
        inc     bp
        push    bp
        push    ds
        mov     cs:[DosDefault],ah
        mov     bp,cs:[DosTraps]
        sub     bp,3
slp:    add     bp,3
        cmp     ah,cs:[bp]
        jnz     slp
        push    word ptr cs:[bp+1]
        mov     bp,sp
        add     bp,4
        sti
slp1    proc    near							 
        ret
slp1    endp


page
;======= Int24Handler ========================================================
;
; Winoldap int24handler. It is installed first in EnDOS. During a context switch
; from an oldapp to Windows, it gets restored in SaveApp (TTYEXT) and the code
; segment is patched in DisableInt21 after the Winoldap reallocation is done
; by MoveWindows.
;
; The original int24 handler is stuffed back by Swapvectors and RestoreVectors 
; when Windows is reenabled during a context switch of after exiting the oldapp.
;
; If the user code does not belong to Winoldap, pass the call to PrevInt24Proc
; and return the status to DOS;
; Else, always return FAIL action code to DOS.
;
;
; Entry:
;		!!INTERRUPTS DISABLED
;  AX:		Error status
;		d7= 0: disk error
;		    1: other device error
;  BP:SI	-> Device Header Control Block
;
; Exit:
;  AL:		Action code (see DOS Tech ref)
;		0:	Ignore
;		1:	Retry
;		2:	Abort through int23h
;		3:	Fail
; Regs:
;  None
;
;=============================================================================


cproc	Int24Handler,<PUBLIC,NEAR>

cBegin

	pushf
	push	BP
	mov	BP, SP
	mov	AX, CS

	cmp	SS:[BP +1EH], AX	; Is User Winoldap?
	pop	BP
	je	Int24Ours		; Yes, jump
	call    CS:[PrevInt24Proc]
	iret

Int24Ours:
	POPFF
	mov	AL, 3			; Return FAIL
	iret
cEnd


;======  Int23Handler  ========================================================
;
; Winoldap needs to install its own ^C handler when running any type of oldapp
; to filter any batch processing. This will fix the problem of receiving the
; "terminate batch file (y/n)" prompt after hitting a ^C while running an 
; oldapp. This ONLY happened if you run Windows from a batch file.
;
; Entry:
;
; Exit:
;
; Uses:
;
;==============================================================================

Int23Handler	proc	FAR

	
DISK_RESET	EQU	0DH

;
; This is the default system INT 23 handler.  All processes get it by default.
;
;
; We have now received a ^C for some process, we will return ABORT
;
	STI				; re-enable ints
	PUSH	AX			; Only register zaped by disk reset call
	MOV	AH,DISK_RESET
	INT	21h			; Reset disks in case files were open
	POP	AX
	STC				; Tell DOS to abort
	ret	    			; Leave flags on stack
 
Int23Handler	endp


SaveRegs:
        cli
        pop     cs:[tmp0]       ; bp is already saved
        push    dx
        push    es
        push    bx
        push    ax
        push    cx
        push    si
        push    di
        and     USER_FL,11111110b         ; clc flag
        push    cs:[tmp0]
        cld
        sti
        ret

RestoreRegs:
        cli
        pop     cs:[tmp0]
        pop     di
        pop     si
        pop     cx
        pop     ax
        pop     bx
        pop     es
        pop     dx
        pop     ds
        pop     bp
        dec     bp
        jmp     cs:[tmp0]

StayResident:
        mov     AH, 4CH                 ; Just do a normal termination
        jmp     Int21Entry

page

Interchange1:
        or      ah,10000000b                    ; enable ^C, ^S checking
InterChange2:

	push	DS
	pop	ES			; Set ES= caller's DS
        mov     di,ss                   ; save current stack
;
;~~tqn 071887
;
	cmp	DI, CS:[Win_TaskSS]	; Is caller ourselves?
	jne	InterchSwitch		; No, switch to our stack
       	call	interchange
	jmp short intr4

InterchSwitch:
;~~
        push    bp
        mov     si,sp

	cli
        mov     ss,CS:[Win_taskSS]      ; switch to new stack
        mov     sp,CS:[Win_taskSP]
	sti
        mov     bp,sp
        add     bp,TASKBP               ; set up valid frame
        call    interchange             ; far call to TTY window
	cli
        mov     ss,di
        mov     sp,si
	sti
        pop     bp

intr4:  jcxz    intr7                   ; ^C detected?
        pop     ax                      ; remove return address
        call    RestoreRegs             ; restore user registers, cli
        mov     cs:[CtrlSP],sp
        int     23h                     ; generate ctrl-c
        pushf
        pop     cs:[tmp0]               ; save flags
        cmp     sp,cs:[CtrlSP]
        jz      intr6
        add     sp,2                    ; assume off by two...
        cmp     sp,cs:[CtrlSP]          ; ..correct?
        jnz     intr5                   ; somethings wrong, better exit
        push    cs:[tmp0]               ; restore flags
        popf
        jnc     intr6                   ; no carry just return
intr5:  mov     ax,4c00h
intr6:  jmp     Int21Entry
intr7:  ret

page
;********************************************************************
;
;  Console I/O Traps
;
;*******************************************************************

CheckHandle:                    ; check if xenix handle is connected...
        push    ax
        call    TestConDevice
        jmp     short hchk1

CheckInput:
        push    ax
        xor     ax,ax
        call    TestConDevice2

hchk1:  pop     ax
        jz      intr7                   ; if CON device return...
        pop     ds                      ; discard return address, and continue

PassOnThrough:
PassOnThroughC:
        cmp     ah,3DH                  ; open system call?
        jnz     PassOnThrough3

;!!     call    GrowSFT                 ; make sure we can grow sft

PassOnThrough3:
        pop     ds
        pop     bp
        dec     bp
        cli
        jmp     cs:[OrgInt21Proc]

page

TestConDevice:
        mov     ax,bx
TestConDevice2:
        push    es
        push    bx
	push	AX			; Save file handle
	mov	AH, 51H			; Get Current PDB in BX
	pushf
	call	CS:[OrgInt21Proc]
        mov     ES, BX    		; ES= Current PDB
        pop     bx                              ; get file handle
        mov     al,es:[bx+PDB_JFN_Table]
        cmp     al,0FEH                         ; special close flag?
        jnz     idx0
        inc     es:[bx+PDB_JFN_Table]
tcd1:   or      ax,ax                           ; reset zero flag
        jmp     short idx5

idx0:   les     bx,cs:[lpDOSData]      
	add	bx, 4			; ES:BX -> lpFileTable
idx1:   les     bx,es:[bx+sftLink]
        cmp     bx,-1
        jz      tcd1
        sub     al,byte ptr es:[bx+sftCount]     ; this allocation has this many files
        jae     idx1
        add     al,byte ptr es:[bx+sftCount]
        lea     bx,[bx+sftFile+sf_fcb+1]
        or      al,al
        jz      idx3
idx2:   add     bx,cs:[FileEntrySize]
        dec     al
        jnz     idx2
idx3:   cmp     cs:[DOS_version],3      ; are we running under 2.x?
        jb      idx4
        test    byte ptr es:[bx+sf_flags-sf_fcb],10000000b   ; is this remote sft?
        jnz     idx5                                         ; yes, cant be CON

        add     bx,sf_name-sf_fcb-1
        cmp     cs:[DOS_revision],0
        jz      idx4
        dec     bx                      ; 3.1, 3.2 have this here
idx4:   cmp     es:[bx+0],"OC"
        jnz     idx5
        cmp     es:[bx+2]," N"
idx5:   pop     bx
        pop     es
        ret                     ; and interpret the call


Xenix_Status:
        cmp     al,6                    ; Input Status?
        jz      xs1                     ; yes, trap it
        cmp     al,4
        jb      xs2
        cmp     al,7
        jz      xs2
        cmp     al,10
        jz      xs2
        jmp     PassOnThroughC
xs1:    call    CheckHandle             ; Console Device?
        call    SaveRegs
        mov     ah,11
        jmp     DoGET
xs2:    jmp     FileHandleCall

Xenix_Read:
        call    SaveRegs
        call    TestConDevice
        jnz     DoDisk
        mov     ah,5
        call    Interchange1
        mov     User_AX,ax
        jmp     DoRet

DoDisk:
	call	RestoreRegs
	jmp	CS:[OrgInt21Proc]

Flush_Buffer:
        call    CheckInput
        call    SaveRegs
        push    dx
        push    ax
        call    Interchange2
        pop     ax
        pop     dx
        mov     ah,al
        mov     al,0
        cmp     ah,1
        jz      DoGET
        cmp     ah,10
        jz      Buffered_Keyboard_Input2
        cmp     ah,6
        jz      dcio1
        cmp     ah,7
        jz      GetCh
        cmp     ah,8
        mov     ah,7
        jz      DoGet
fbf1:   jmp     DoRet

Direct_Console_IO:
        cmp     dl,-1                           ; wants a key?
        jz      dcio0                           ; yes, proceed
        jmp     PassOnThrough3                  ; display character
		 
dcio0:  call    CheckInput 
        call    SaveRegs

dcio1:
        or      User_FL,01000000b         ; set ZF
        mov     ah,11
        call    Interchange2    ; get keyboard status
        or      al,al
        jz      RETAL
        and     User_FL,10111111b         ; clear ZF
GetCh:  mov     ah,7
        call    Interchange2
        jmp     short RETAL

Direct_Console_Input:
        call    CheckInput
        call    SaveRegs
        jmp     GetCh

Read_Keyboard_and_Echo:
Check_Keyboard_Status:
        call    CheckInput
        call    SaveRegs
DoGET:  call    Interchange1
RETAL:  mov     User_AL,al
        jmp     short DoRET

Buffered_Keyboard_Input:
        call    CheckInput
        call    SaveRegs
Buffered_Keyboard_Input2:
        mov     di,dx
        mov     ds,User_DS
        mov     cx,[di]
        add     dx,2
        call    Interchange1
        mov     di,User_DX
        mov     ds,User_DS
        or      al,al
        jz      bki1
        dec     al
bki1:   mov     [di+1],al
        jmp     short DoRet

DoRet:
        pop     di
        pop     si
        pop     cx
        pop     ax
        pop     bx
        pop     es
        pop     dx
        pop     ds
        pop     bp
        dec     bp
        iret

Read_Keyboard:
        call    CheckInput
        call    SaveRegs
        mov     ah,7
        jmp     DoGet

Set_Vector:
        pop     ds
        pop     bp              ; clean up stack
        dec     bp
        cmp     al,21h          ; trying to reset our traps?
        jnz     svc1            ; no, go ahead
        iret                    ; sorry pal, no way.
svc1:   jmp     cs:[OrgInt21Proc]


FileHandleCall:
        push    es
       	push	AX
	push	BX
	push    di
	mov	AH, 51H			; Get current PDB in BX
	pushf
	call	CS:[OrgInt21Proc]
	mov	ES, BX
        cmp     es:[bx+PDB_JFN_Table],0FEH
        jnz     fhc
        inc     es:[bx+PDB_JFN_Table]
fhc:    pop     di
	pop	BX
	pop	AX
        pop     es
        jmp     PassOnThroughC


page
;----------------------------------------------------------------
;
;  Fake Console driver to intercept DOS keyboard status calls
;
ConDev  dd      ?
        dw      1000000000000011b
        dw      Strat
        dw      Entry

Strat   proc    far
        mov     word ptr cs:[ConDev],bx
        mov     word ptr cs:[ConDev+2],es
        ret
Strat   endp

Entry   proc    far
        push    ds
        push    bx
        lds     bx,cs:[ConDev]
        mov     word ptr [bx+3],0300h           ; indicate no keys ready
        pop     bx
        pop     ds
        ret
Entry   endp

sEnd    CODE

end
