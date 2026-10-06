;       SCCSID  =  @(#)atdsk2.asm       5.23 87/03/24
        page    ,132
	title	ATDISK2 - MS OS/2 1.1 AT disk device driver
        name    ATDISK2                                         ; @@
        comment !                                               ; @@
********************* Start of Specifications *************************
*                                                                     *
*  Source File Name: ATDSK2.ASM                                       *
*                                                                     *
*  Descriptive Name: AT diskette device state machine                 *
*                                                                     *
*  Copyright:                                                         *
*                                                                     *
*  Status: Version 1.1                                                *
*                                                                     *
*  Function: Provides the interrupt handler and state machine for     *
*                     AT diskette device support                      *
*                                                                     *
*  Notes: N/A                                                         *
*                                                                     *
*    Dependencies: AT only                                            *
*                                                                     *
*    Restrictions: unshared interrupts                                *
*                                                                     *
*  Entry Points:  DriveInterrupt - interrupt handler                  *
*                 DriveExecute   - state machine                      *
*                                                                     *
*  External References:  See EXTRN statements below                   *
*                                                                     *
********************** End of Specifications ************************** !

; @@ This source file has been totally reworked from the Version 1.0 base
; @@ code. It has been so heavily modified and reformatted that it is not
; @@ practical to determine what lines of code are new or old on a line
; @@ by line basis.

        .286p

        include abdebug.inc             ; debug and listing control

        include absegs.inc              ; code and data segment definitions

        BREAK <External code entry points>
Code    segment

; Routines located in ABDSK1.ASM
        extrn   Disk_IO:near
        extrn   DoneRequest:near
        extrn   Do_DiskIO:near

; Routines located in ABDSK4.ASM
        extrn   SetTimer1:near
        extrn   SetTimer2:near
        extrn   Set_FTimerS:near
        extrn   Cancel_FTimer:near
        extrn   Set_FloppyTimerS:near   ;@p3005

; Routines located in ABSUBR.ASM
        extrn   Get_BDS_Addr:near
        extrn   GetDrivePtr:near
        extrn   Get_Request_Packet:near
        extrn   Release_Request_Packet:near
        extrn   PhysToVirtDSSI:near
        extrn   PhysToVirtESDI:near
        extrn   UN_PhysToVirt:near
        extrn   SSig:near
        extrn   SWait:near
        extrn   PRun:near
        extrn   SectorIndexToSectorSize:near
        extrn   FloppyChange:near
        extrn   Do_Check:near
        extrn   StatusComplete:near
        extrn   Flag_UM:near
Code    ends

        BREAK <External data references>
BioData segment

; Data located in ABDSK1.ASM
        extrn   Start_BDS:word
        extrn   DevHlpFunc:dword
        extrn   ScratchBufSem:byte
        extrn   ScratchBufPhys:dword
        extrn   DriveA:word
        extrn   DriveB:word
	extrn	ScratchBuffer:word

; Data located in ABDSK4.ASM
        extrn   TimerActive:byte

; Data located in ABSUBR.ASM
        extrn   NumFlop:byte
        extrn   harderrreqblk:dword     ; @@p3005
        extrn   dmaoverrunflag:byte     ; @@p3005
; Data located in ATINT13.ASM
        extrn   SemInt13:byte
        extrn   Block_Int13:byte        ;@@2846

BioData ends

DataInit segment

; Data located in ABINIT.ASM
        extrn  BPB48T:byte
        extrn  BPB96T:byte
        extrn  BPB35S:byte
        extrn  BPB35L:byte

        BREAK <Array of Pointer to BPB Tables>
              even                            ; ordered by form factor
   BPBTable   dw      DataOffset BPB48T       ; 0 48tpi drives
              dw      DataOffset BPB96T       ; 1 96tpi drives
              dw      DataOffset BPB35S       ; 2 720 KB 3.5 inch
              dw      0                       ; 3 not used 8" diskette
              dw      0                       ; 4 not used 8" diskette
              dw      0                       ; 5 not used hard file
              dw      0                       ; 6 not used tape drive
              dw      DataOffset BPB35L       ; 7 1.44 MB 3.5 inch
              dw      0                       ; 8 Other

DataInit ends


        .xlist                          ;
        include struc.inc               ; structured macros
if1
        include abmac.inc               ; private macros
endif
        include bpb.inc                 ; bios parameter block
        include devhlp.inc              ; device helper definitions
        include error.inc               ; DOS error codes
        include abdskequ.inc            ; general equates
        include abddrv.inc              ; IOStruc
        include atstate.inc             ; state machine definitions
        include abioctl.inc             ; IOCTL equates
        include abint13.inc             ; ROM and I/O equates
        include ibmrom.inc              ; AT low core ROM data area

        ListLevel %ablistlevel,%abdebug

        BREAK   <Data Segment>
BioData segment

Floppy_Reset    db      0               ; non-zero if in Rst765
LastDrv         db      0               ; last drive used - used for motor

; Disk base tables for the various media indices

DPT99   Drive_Table <0DFH,2,25H,2,15,1BH,0FFH,54H,0F6H,1,5> ; 96tpi in 96tpi
DPT49   Drive_Table <0EFH,2,25H,2,09,23H,0FFH,54H,0F6H,1,5> ; 48tpi in 96tpi
DPT44   Drive_Table <0DFH,2,25H,2,09,2AH,0FFH,50H,0F6H,15,2> ; 48tpi in 48tpi
DPT35   Drive_Table <0DFH,2,25H,2,09,2AH,0FFH,50H,0F6H,15,4> ; Micro Floppy

; This table is used to access the correct values to be passed to the NEC
; controller depending on the type of medium in the drive

DPTable     dw      DataOffset DPT99
            dw      DataOffset DPT49
            dw      DataOffset DPT44
            dw      DataOffset DPT35

; This table is used to get the correct data rate index to pass on to the
; controller by using the CurIndex value in the BDS for the drive.

RATETBL         db      RATE500         ; 96 tpi medium in 96 tpi drive
                db      RATE300         ; 48 tpi medium in 96 tpi drive
                db      RATE250         ; 48 tpi medium in 48 tpi drive
                db      RATE250         ; 720 KB medium in AT ext drive


; When we get a stage on interrupt return code the normal
; procedure would be to start off a "timeout" timer and cancel it when
; the interrupt occurs.
; There is a problem if the interrupt comes in BEFORE the the timer is set!
; (the nonexisting timer would be cancel and then the timer would be set
; with no one to cancel him - eek!)
; The solution to this problem is TimerCallFlag.  by incrementing it every
; time we want to start a timer and decrementing it every time a we want
; to cancel the timer we can tell if any timers are outstanding or NOT.

TimerCallFlag   dw      0

        public  Drive
Drive   IOStruc <,,IDLE>                ; info about current disk I/O

        db SIZE ATStruc - SIZE IOStruc dup (?)

BioData ends

        BREAK   <Code Segment>
Code    segment

        assume  cs:BiosSeg,ds:BiosGroup,es:nothing

        comment !
************************ Start of Specifications *********************
*                                                                    *
* Subroutine Name: DriveInterrupt                                    *
*                                                                    *
* Descriptine Name: Diskette controller interrupt handler            *
*                                                                    *
* Function: DevHlp_SetIRQ routine for diskette devices               *
*                                                                    *
* Notes:                                                             *
*                                                                    *
* This code replaces the ROM INT E handler.  We first check to see   *
* if we are emulating a ROM request.  If we are, we pass control off *
* to the the code that emulates the ROM routine, otherwise (if the   *
* drive is active) we flag that we have finished the operation that  *
* was requested, and re-enter the floppy finite state machine.       *
*                                                                    *
* Entry Point: DriveInterrupt                                        *
*                                                                    *
* Linkage: DevHlp_SetIRQ                                             *
*                                                                    *
* Input:                                                             *
*                                                                    *
* Exit-Normal:                                                       *
*                                                                    *
* Exit-Error:                                                        *
*                                                                    *
* Effects:                                                           *
*                                                                    *
* Internal References: DriveExecute                                  *
*                                                                    *
* External References: none                                          *
*                                                                    *
************************* End of Specifications ********************** !

        BREAK   <DriveInterrupt>

        Public DriveInterrupt
DriveInterrupt proc far                 ; DevHlp_SetIRQ entry point

; pusha                                 ;   @@2495
; SaveReg <ds,es>                       ;   @@2495
  .IF <Block_Int13  ne 0> or            ; Int 13 procee being blocked?    @@2846
  .IF <SemInt13     eq 0> and           ; direct Int13 request in progress ?
  .IF <Floppy_Reset eq 0> and           ; reset pending ?
  .IF <Drive.Active ne 0>               ; device active ?
     DISABLE                       ; This is to check to see if the Interrupt
     dec     TimerCallFlag         ; occurred before setting the timer!!!
     cmp     TimerCallFlag, 0      ; If so then we don't need to call timer
                                   ; services to turn off the timer!
     jl      no_timer_cancel       ; jmp if destination < source ! (signed)
                                   ;
     call    Cancel_FTimer         ;
   no_timer_cancel:                ;
     ENABLE                        ;
;;;  mov al,EOI                         ; send EOI to 8259
;;;  out INT_CONT,al                    ;   ...

     mov     al, IRQ_Flop            ; (Use EOI DevHelp)
     DEVHLP  DevHlp_EOI

     ENABLE                             ;   ...
     call DriveExecute                  ; go to next state in the machine
  .ELSE                                 ; Code taken almost verbatim from
     mov ax,RomData                     ; ROM since we cannot use the ROM
     mov es,ax                          ; in protext mode.
     assume  es:RomData                 ;   ...
     or  Seek_Status,F_FLOPPY_INTR      ; flag that interrupt occurred
     assume  es:nothing                 ;   ...
;;;  DISABLE
;;;  mov al,EOI                         ; send EOI
;;;  out INT_CONT,al                    ;   ...

     mov     al, IRQ_Flop            ; (Use EOI DevHelp)
     DEVHLP  DevHlp_EOI

     ENABLE                             ;   ...
  .ENDIF                                ;   ...
  clc                                   ;   must inform INT mrg that it is ours
; RestoreReg <es,ds>                    ;   @@2495
; popa                                  ;   @@2495
  ret                                   ; far return

DriveInterrupt endp

        BREAK <Floppy_Init>
;** Floppy_Init
;
;     Floppy_Init is called by DriveInit durint INIT 0 command processing.
;
;     It installs the diskette controller interrupt handler, unmasks the
;     controller, and installs DriveTimer for timer services.
;
;     ENTRY  DS - BiosGroup
;
;     EXIT   'C' set if any errors
;
;     USES - flags

CodeInit segment

        public Floppy_Init
Floppy_Init proc

  pusha                                   ;   ...
; mov di,DataOffset DriveA                ; hard code IOSoffset links
  mov di, DriveA                          ; hard code IOSoffset links
  mov [di].IOSoffset,DataOffset Drive     ;   and parent pointers
  mov [di].ParentDisk,di                  ;   ...

; mov di,DataOffset DriveB                ; real or pseudo B:
  mov di, DriveB                          ; real or pseudo B:
  mov [di].IOSoffset,DataOffset Drive     ;   ...
  mov [di].ParentDisk,di                  ;   ...

; Set up the form factor for the types of media that we recognize.
; For the rest, we set the form factor as "Other".

  mov di,Start_BDS                        ; set up the diskette BPB's
  .FOR dl = 0 to 1 near                   ;   ...
     mov dh,ffOther                       ; default
     .IF <[di].TotalPhysCyln eq 80>       ;   ...
        .IF <[di].BPBtrksiz eq 18>        ;   ...
           mov  dh,ff35_18                ; internal 1,44 MB 3.5 inch drive
        .ELSEIF <[di].BPBtrksiz eq 9>     ;   ...
           mov  dh,ff35_9                 ; 720 KB 3.5 inch diskette
        .ELSEIF <[di].BPBtrksiz eq 15>    ;   ...
           mov  dh,ff96tpi                ; external 5.25 inch drive
        .ENDIF                            ;   ...
     .ELSEIF <[di].TotalPhysCyln eq 40>   ;   ...
        .IF <[di].BPBtrksiz eq 9>         ;   ...
           mov dh,ff48tpi                 ;   ...
        .ENDIF                            ;   ...
     .ENDIF                               ;   ...
     mov [di].FormFactor,dh               ;   ...

; We need to do special things if we have a single drive system and are setting
; up a pseudo drive. It needs to have the same physical drive number as its
; counterpart, but the next drive letter. Also reset ownership flag.

;                          Real             Pseudo
;                         A:  B:            A:  B:

; DriveUnit               0   1             0   1    logical
; DriveNum                0   1             0   0    physical
; DriveLet                1   2             1   2    (DriveUnit + 1)
; fI_Own_Physical         1   1             1   0    (changes dynamically)
; fI_Am_Mult              0   0             1   0    (static)

     or  [di].DrvTypFlag,fI_Own_Physical  ; assume real devices
     mov [di].DriveNum,dl                 ; physical
     mov [di].DriveLet,dl                 ;
     inc [di].DriveLet                    ; logical ( 1-based )
     .IF <NumFlop ne 2>                   ; this is the pseudo-drive case
        .IF <dl eq 0>                     ; real A:
           or  [di].DrvTypFlag,fI_Am_Mult ;   see table above
        .ELSE                             ; pseudo B:
           and [di].DrvTypFlag,fChangeline;
           or  [di].DrvTypFlag,fI_Am_Mult ;
           mov [di].DriveNum,0            ; A: is the physical drive
        .ENDIF                            ;   ...
     .ENDIF                               ;   ...
     pusha                              ; save register
     SaveReg es                         ;
     xor bx,bx                          ;
     mov bl,[di].FormFactor             ; set FormFactor to BL
     .IF <bl ne ffOther>                ; handle known media types
        shl  bx,1                       ; bx is word index into table of
        mov  si,DataOffset BPBTable     ; table of offsets
        mov  si,word ptr [si+bx]        ; ds:si -> hard coded BPB
        push ds                         ;
        pop  es                         ;
        lea  di,[di].BPBRsecsiz         ; es:di -> RecBPB
        mov  cx,(size a_BPB) - 12       ; use short BPB
        repe movsb                      ; copy hard coded BPB to RecBPB
     .ELSE                              ; handle ffOther
        SaveReg dx
        xor dx,dx                       ;
        mov ax,[di].cCyln               ;
        mov bx,[di].BPBRnhead           ;
        mul bx                          ;
        mov bx,[di].BPBRtrksiz          ;
        mul bx                          ;
        mov [di].BPBRnsec,ax            ; Have the total number of sectors
        dec ax                          ;
        mov bx,515                      ;
        div bx                          ;
        or  dx,dx                       ;
        .IF nz                          ;
           inc ax                       ; round up number of FAT sectors
        .ENDIF                          ;
        mov [di].BPBRnfatsec,ax         ;
        RestoreReg dx                   ;
     .ENDIF                             ;
     RestoreReg es                      ; restore resisters
     popa                               ;
     mov di,[di].Link                     ; bump to next BDS entry
  .NEXT dl                                ;   ...

  mov bx,IRQ_Flop                         ; install the interrupt handler
  mov ax,CodeOffset DriveInterrupt        ;   ...
  xor dh, dh                              ; (we will share this interrupt
                                          ; level (0))
  DEVHLP DevHlp_SetIRQ                    ; sets carry on error
  .IF nc                                  ; carry means already owned

     DISABLE                              ; unmask the floppy disk interrupt
     in  al,INT_CONT+1                    ;   ...
     and al,NOT 40H                       ; reset bit 6
     IOWait                               ;   ...
     out INT_CONT+1,al                    ;   ...
     ENABLE                               ;   ...
     call Reset_Floppy                    ; reset the controller

  .ENDIF                                  ;   ...

  popa                                    ;   ...
  ret                                     ; return carry flag

Floppy_Init endp

CodeInit ends

        BREAK <DriveExecute>
        comment !
************************ Start of Specifications *********************
*                                                                    *
* Subroutine Name: DriveExecute                                      *
*                                                                    *
* Descriptine Name: Diskette State Machin  dispacher                 *
*                                                                    *
* Function: starts and sequences all diskette I/O operations         *
*                                                                    *
* Notes: 1. entered enabled from Disk_IO                             *
*        2. sets ES:BX to request block at top of Drive.Queue        *
*                                                                    *
* Entry Point: DriveExecute                                          *
*                                                                    *
* Linkage: near call from Disk_IO                                    *
*                                                                    *
* Input: all parameters are in Drive IOStruc                         *
*                                                                    *
* Exit-Normal: to the next state in the diskette state machine       *
*                                                                    *
* Exit-Error:   N/A                                                  *
*                                                                    *
* Effects: causes a transition to the next state                     *
*                                                                    *
* Internal References: see DriveDispatch table below                 *
*                                                                    *
* External References: see extrn statements at start of source file  *
*                                                                    *
************************* End of Specifications ********************** !

        BREAK <DriveExecute State Routine Table>
BioData segment

; if abdebug=1 (in ABDEBUG.INC), make the following label PUBLIC
;
; if abdebug ; @@
   public FlExstart,FlExCalc,FlExSelect,FlExRecal,FlExSettle,FlExRdWri
   public FlExDone,FlExIdle,FlExError,FlExRset,FlExTimOut
   public FlExChngLine1,FlExChngLine2,FlExChngLine3,FlExChngLine4
; endif     ; @@

DriveDispatch label word
        dw CodeOffset FlExStart        ; 0
        dw CodeOffset FlExCalc         ; 1
        dw CodeOffset FlExSelect       ; 2
        dw CodeOffset FlExRecal        ; 3
        dw CodeOffset FlExSeek         ; 4
        dw CodeOffset FlExSettle       ; 5
        dw CodeOffset FlExRdWri        ; 6
        dw CodeOffset FlExDone         ; 7
        dw CodeOffset FlExIdle         ; 8
        dw CodeOffset FlExError        ; 9
        dw CodeOffset FlExRset         ; 10
        dw CodeOffset FlExTimOut       ; 11
        dw CodeOffset FlExError        ; 12
        dw CodeOffset FlExError        ; 13
        dw CodeOffset FlExError        ; 14
        dw CodeOffset FlExChngLine1    ; 15
        dw CodeOffset FlExChngLine2    ; 16
        dw CodeOffset FlExChngLine3    ; 17
        dw CodeOffset FlExChngLine4    ; 18

BioData ends

        BREAK <DriveExecute - state machine>
        public DriveExecute
DriveExecute proc

  xor bh,bh                       ;   ...
  mov bl,Drive.State              ; get current state
  shl bx,1                        ; form a word offset
  jmp DriveDispatch[bx]           ; dispatch to correct routine


;** STATE JUMP macro
;
; When debugging, force all state transitions to go to DriveExecute so
; everything can be watched from a single breakpoint.

SJUMP macro target
if abdebug
  jmp DriveExecute
else
  jmp target
endif
      endm

        BREAK <State handlers>
;** Floppy state START
;
;       Do setup calculations to figure out sector, start
;       up motor, advance to CALC state.
;
;  Entered on initially picking up a new request to do and on error retries.

FlExStart:

  mov si,DataOffset Drive      ; si -> IOStruc
  les bx,[si].Queue            ; es:bx -> pointer to current request
  .IF <[si].ErrCnt eq 0>       ; Error count is 0 for a new request
     call Setup                ; Do setup calculations
  .ENDIF                       ;   ...
  mov [si].State,CALC          ; next state
  SJUMP flexc1                 ;   ...

;** Floppy state CALC
;
;       Calculate cylinder, head and sector, wait for motor
;       start or head load, advance to SELECT state.
;
;  Entered after START state and also on further sectors of a multiple sector
;  request.

FlExCalc:

  mov  si,DataOffset Drive      ; IOStruc
  les  bx,[si].Queue            ; es:bx -> current request packet
flexc1:                         ; FlExStart comes here
  mov  al,es:[bx].PktUnit       ; get unit
  call Get_BDS_Addr             ; ds:di = drive parameters
  call MapSector                ; get head, cylinder and sector
  mov  [si].State,SELECT        ; will advance to Select state
  .IF <es:[bx].PktCmd e DISKOP_CHKCHNG>
      mov [si].State,CHKCHNGLINE
  .ENDIF                        ;   ...
  call Sel765                   ; Select the drive and maybe wait
  LJNC DriveExecute             ; Did select with no waiting
  ret                           ; Have set a timer, wait for it.

;** Floppy state SELECT
;
;  Recalibrate the drive if FRestor is on in the device structure.
;  If Seek is needed, start it and advance to SEEK state.  Otherwise
;  advance to SETTLE state.

FlExSelect:

  call GetDrivePtr                 ; ds:bx -> DriveStruc

; Tell disk controller what type of medium is in the drive
;
; In order to read the disk, we need to set the transfer rate correctly. We
; do it as follows:
;       If  This is either a 48 tpi drive or a 3.5" drive
;           Use the rate of 250bps
;       Else
;           Try each rate until one succeeds or until we have tried all
;
        cmp     [BX].FormFactor,ff48tpi
        je      fsl0                        ; We have a 48tpi drive
        cmp     [BX].FormFactor,ffsmall
        jne     fsl1
; We have either a 3.5" drive or a 48 tpi drive
fsl0:
        mov     al,INDEX44
        mov     byte ptr [BX].CurIndex,al   ; Use INDEX44 for 48tpi and 3.5" drives
        or      byte ptr [bx].DrvFlag,fMedInd   ; Say we know the xfer rate
fsl1:

  SaveReg <dx,ax,di>               ; tell disk controller what type
  xor ah,ah                        ;   of medium is in the drive
  mov al,[bx].CurIndex             ; RATETBL index
  mov di,ax                        ; di contains index into RATETBL
  mov al,RATETBL[di]               ; pull out data rate
  mov dx,FD_PCTRL                  ; write out rate to controller register
  out dx,al                        ;   ...
  RestoreReg <di,ax,dx>            ;   ...
  or [bx].DrvFlag,Fmotoron         ; we've been selected, so motor is on
  test [bx].DrvFlag,Frestor        ; is a restore needed?
  .IF ne                           ; no
     mov  Drive.State,RECAL        ; next state will be recalibrate
     call Rcl765                   ; start the recalibrate
;    call SetTimer2                ; set a sanity/motor stop timer
     mov  ax, TIMEWAITTIME         ; specified wait time before timeout
     call Set_Timeout_Timer        ; set a TimeOut timer for 3 seconds
     ret                           ; done until floppy interrupt arrives
  .ENDIF                           ; if we have to seek, start it up
  mov  Drive.State,SEEK            ; next state is Seek
  call Seek765                     ; start the seek to cylinder
  LJNC FESEEK2                     ; already on correct cylinder, don't wait
; call GetDrivePtr                 ; ds:bx -> BDS entry
; call SetTimer2                   ; set sanity timer
  mov  ax, TIMEWAITTIME            ; specified wait time before timeout
  call Set_Timeout_Timer           ; set a TimeOut timer for 3 seconds
  ret                              ; done until interrupt on seek done

;** Floppy state RECAL
;
;       If error, set state is ERROR.  Else, load drive
;       specs into controller and advance to SELECT state.

FlExRecal:

  call Sense765                    ;   ...
  or   ax,ax                       ; Error in recal?
  jnz  FESEEK1                     ; Yes, pretend it was a seek error.
  call Spec765                     ; issue specify command.
  mov  Drive.State,SELECT          ; Back to select state now
  SJUMP FlExSelect                 ;   ...

;** Floppy state SEEK
;
;       If error, advance to ERROR state.  Otherwise, wait
;       for head to settle and advance to SETTLE state.

FlExSeek:

  call Sense765                    ; Get status of seek
  or   ax,ax                       ; Any error?
  jz   FESEEK2                     ; No

FESEEK1:
  or  Drive.ST1,8                  ; indicate seek error in an unused bit
  mov Drive.State,SERROR           ; Yes, next state is Error
  SJUMP FlExError                  ;

FESEEK2:
  mov  Drive.State,SETTLE          ; Next state is Settle
  mov  al,HeadSTTL                 ;
  call GetFloppyParam              ; Get the settle time in msecs
  xor  ah,ah                       ;
  test Drive.Flags,Fwrite          ;
  .IF z                            ; Use value obtained for write
     xor al,al                     ; Use zero for reads
  .ENDIF                           ;
  call SetTimer1                   ; Set the timer
  LJNC DriveExecute                ;
  ret                              ;

;** Floppy state CHKCHNGLINE
;
;       Read the DIR register to see if the changeline bit is set.

FlExChngLine1:

  call GetDrivePtr                    ; (DS:BX) = BDS for this drive.
  or   [bx].DrvFlag,Fmotoron          ; we've been selected, so motor is on
  les  bx,Drive.Queue                 ;
  DISABLE                             ; could go to DONE
  mov  dx,FD_PCTRL                    ;
  in   al,dx                          ;
  mov  byte ptr es:[bx].PktStatus,al  ; save status indicator
  test al,80H                         ; Test high bit
  .IF z                               ;
     mov  Drive.State,DONE            ;
     SJUMP FlExDone                   ; no changeline, so do not reset it.
  .ENDIF                              ;
  ENABLE                              ; not going to DONE
  call Rst765                         ; Reset the drive
  mov  Drive.State,SEEKRESET1         ;
  SJUMP FlExChngLine2                 ;

;** Floppy state SEEKRESET1
;
;       This state seeks to cylinder 1.
;
;       Reset the changeline indication. In order to reset the changeline,
;       we need to physically move the heads. We do this by seeking to
;       cylinder 1, and then seeking to cylinder 0. (This is an optimisation
;       since we know the next request will probably be a Read for the boot
;       sector, which is on cylinder 0).

FlExChngLine2:

  mov  Drive.Cyl,1                   ; Seek to cylinder 1
  mov  Drive.State,SEEKRESET0        ;
  call GetDrivePtr                   ;
  call Seek765                       ;
  LJNC DriveExecute                  ; We do not need to wait for seek.
; call SetTimer2                     ; start watch dog timer
  mov  ax, TIMEWAITTIME              ; specified wait time before timeout
  call Set_Timeout_Timer             ; set a TimeOut timer for 3 seconds
  ret                                ;   ...

;** Floppy state SEEKRESET0
;
;       Reset the changeline indication. This state seeks to cylinder 0.

FlExChngLine3:

  call Sense765                         ; g]t seek status and ignore it
  mov  Drive.Cyl,0                      ;
  mov  Drive.State,SENSESEEK0           ;
  call GetDrivePtr                      ;
  call Seek765                          ;
  LJNC DriveExecute                     ; we do not need to wait
; call SetTimer2                        ; start a watch dog timer
  mov  ax, TIMEWAITTIME                 ; specified wait time before timeout
  call Set_Timeout_Timer                ; set a TimeOut timer for 3 seconds
  ret                                   ;   ...

;** Floppy state SENSESEEK0
;
;       The final part of the sequence for resetting the changeline
;       indication. Sense the seek status and go to 'Done'.

FlExChngLine4:

  call Sense765                     ; get seek status and ignore it
  mov  Drive.State,DONE             ; next state
  jmp  flxd3                        ; we do not need to wait

;** Floppy state SETTLE
;
;       Start the read/write request and advance to the RDWRI state.

FlExSettle:

  mov  Drive.State,RDWRI          ; Advance to read/write state
  call RdWr765                    ; Start the I/O
  ; check for dma overrun error   ; @p3005 begin
  cmp     dmaoverrunflag,0
  jne     STG0
  ; Read,Write or Verify in progress
  SaveReg <ax,bx,es>
  mov     bx,word ptr harderrreqblk
  mov     ax,word ptr harderrreqblk+2
  call    PRun
  RestoreReg <es,bx,ax>
STG0:                             ; @p3005 end
  call GetDrivePtr                ; ds:bx -> DriveStruc
; call SetTimer2                  ; set sanity timer
  mov  ax, TIMEWAITTIME           ; specified wait time before timeout
  call Set_Timeout_Timer          ; set a TimeOut timer for 3 seconds
  ret                             ; Done until floppy interrupt

;** Floppy state RDWRI
;
;       This state is entered before issuing the  ?????? (incomplete)
;       If error, next state is Error.  Otherwise next state is Done.

FlExRdWri:

  call Fini765                           ; Get status of I/O
  or   ax,ax                             ; Any error?
  .IF nz                                 ; No
     mov  Drive.State,SERROR             ; Yes, go to error state
     SJUMP FlExError                     ;
  .ENDIF                                 ;
  call GetDrivePtr                       ; Remember that we have now
  or   byte ptr [bx].DrvFlag,FMedInd     ;   determined the type of
  mov  al,byte ptr [bx].CurIndex         ;   medium in this drive
  mov  byte ptr [bx].LastInd,al          ;
  call SetROMDskStat                     ; Set the ROMs status bytes
  mov  Drive.State,DONE                  ; I/O is done
  SJUMP FlExDone                         ; transition to next state

;** Floppy state DONE
;
;       If whole request is now complete, mark the request
;       as done and then start the next one if there is one.
;       If the request is not yet done, adjust values to show
;       the amount of the request done and then go back to the
;       Calc state to do next part.
;
;       ENTRY   Drive IOStruc
;
;       EXIT    NONE
;
;       USES    AX, BX, CX, DI, Flags

FlExDone:

  DISABLE                                ; prevent completion races
  mov ax,Drive.Flags                     ; see if the scratch buffer used
  and ax,Fwrite+Fverify+Fwrap            ; only interested in these bits
  .IF <ax e Fwrap>                       ; the transfer was made through
     mov  dmaoverrunflag,0ffh            ; Freeze Hard Error Daemon p3005
     SaveReg <ax,es>			 ;   the scratch buffer; copy it out
     mov  si, ScratchBuffer		 ; ds:si -> offset to scratch buffer
     mov  cx,Drive.NumBytes		 ; cx = # bytes to write from scr
     mov  bx,word ptr Drive.RealAddr	 ;
     mov  ax,word ptr Drive.RealAddr+2	 ; ax:bx = real buffer (32-bit address)
     call PhysToVirtESDI		 ; es:di = real buffer (virtual addr)
     cld				 ;   ...
     rep  movsb 			 ; copy into real buffer
     call UN_PhysToVirt 		 ; its ok to call this since we know
     RestoreReg <es,ax> 		 ; that DS points to our data segment
  .ENDIF                                 ;   ...

  and Drive.Flags,NOT FWrap              ; reset buffer used flag
  and ax,Fwrite+Fverify                  ; just want to see these bits
  .IF <ax e Fwrite+Fverify>              ; just do write part of write+verify?
     and Drive.Flags,NOT FWrite          ; yes, do verify next
     mov Drive.State,SETTLE              ;   ...
     DISABLE                             ; don't let timer pop
     SJUMP FlExSettle                    ;   ...
  .ENDIF                                 ;   ...

  test Drive.Flags,fSPECIAL              ; is it an internal packet ?
  .IF z                                  ; no special processing for these
     test Drive.Flags,fIOCTL             ; handle IOCTL verify requests
     .IF nz and                          ;   ...
     .IF <ax e Fverify>                  ; just do verify part of write+verify?
        or Drive.Flags,Fwrite            ; yes, flip write back up for next
     .ENDIF                              ;   ...
     mov ax,Drive.NumSectors             ; ax = # of sectors we did
     .IF <ax ne 0>                       ; did we do any i/o? error if not
        sub Drive.Count,ax               ; adjust count to number left
        .IF nz                               ; nz = more to do
           add word ptr Drive.First,ax       ; advance sector number
           adc word ptr Drive.First+2,0      ;   ...
           mov ax,Drive.NumBytes             ; number of bytes handled
           add word ptr Drive.RealAddr,ax    ; advance data address
           adc word ptr Drive.RealAddr+2,0   ;   ...
           mov Drive.State,CALC              ; go to Calc state
           DISABLE                           ; don't let timer pop
                                             ; P3005 BEGIN
           cmp     dmaoverrunflag,0          ; If Hard Error Daemon Frozen
           jne     DON0                      ; then next instruction is a noop
           mov     dmaoverrunflag,0ffh       ; Freeze Hard Error Daemon
           mov     al,Drive.Current
           call    Get_BDS_Addr
           mov     [di].DmaOverrunCnt,DMATIMEDELAYCNT
           ; delay for 1 second before doing remaining of request
           mov     ax,VIDEODELAY
           ENABLE
           call    Set_FloppyTimerS
           ret
DON0:                                        ; p3005 END
           SJUMP FlExCalc                    ;   ...
        .ENDIF                               ;   ...
     .ENDIF                              ;   ...
  .ENDIF                                 ;   ...

flxd3:                                   ; other states come here when done

  les bx,Drive.Queue                     ; get request packet address
  .IF <es:[bx].PktCmd ne DISKOP_CHKCHNG> ; release the scratch buffer
     mov  al,es:[bx].PktUnit             ;   semaphore if it was obtained
     call Get_BDS_Addr                   ;   in Disk_IO via CheckWrap
     mov  si,word ptr es:[bx].IOpData    ; set up for the CheckDMAWrap call
     mov  cx,es:[bx].IOcount             ;   ...
     mov  ax,[di].BPBsecsiz              ;   ...
     call CheckDMAWrap                   ; see if a wrap was possible
     .IF c                               ; c = wrap may have occured
        SaveReg bx                       ;   and the semaphore is held
        mov  bx,DataOffset ScratchBufSem ;   ...
        call SSig                        ; release the semaphore
        RestoreReg bx                    ;   ...
     .ENDIF                              ;   ...
  .ENDIF                                 ;   ...

  DISABLE                                ;
  mov  al,es:[bx].PktUnit                ; get last drive accessed
  .IF <al be 2>                          ; don't do for aliased drives
    mov  cx,ROMData                      ; update ROM area
    mov  es,cx                           ;    ...
    mov  es:[ActpDrv],al                 ;    ...
  .ENDIF                                 ;   ...
  mov     dmaoverrunflag,0FFH            ; Signal Hard Error Daemon p3005
  mov  di,DataOffset Drive               ; ds:di -> IOStruc for finished I/O
  ENABLE                                 ;   ...
  call DoneRequest                       ; perform PRun and redrive
  jmp  DriveExecute                      ; to either Start or Idle

;** Floppy state Idle
;
;       Nothing hapenning except possible motor off timeout.

FlExIdle:

  call GetDrivePtr                       ;   ...
  call SetTimer2                         ; Set the motor timer
  ret                                    ;   ...

;** Floppy state ERROR
;
;       If error count not exceeded, restore the drive and start
;       the request over again.  Otherwise set error in the packet and
;       advance to the Done state.

FlExError:
                                        ; @P3005 BEGIN
  call GetDrivePtr             ; Get a pointer to the drive info
  or [bx].DrvFlag,Frestor      ; Restore needed always!
  .IF  <<byte ptr Drive.ST1> eq 10H>    ; did we have dma overrun

        ; dma overrun error has occured wait 2 seconds.
        cmp     dmaoverrunflag,0        ; Signal Hard Error Daemon to yield cpu
        jne     xzx
        dec     [bx].DmaOverrunCnt
        jz      ER0
        mov     dmaoverrunflag,0FFH     ; Signal Hard Error Daemon to block
        mov     ax,DMATIMEDELAY
        mov     Drive.State,Calc        ; go back to do calculations
        call    Set_FloppyTimerS
        ret
     xzx:
        mov     dmaoverrunflag,0        ; Signal Hard Error Daemon to yield cpu
        mov     Drive.State,Calc        ; go back to do calculations
        SJUMP   FlExCalc                ;
   .ENDIF                               ; @p3005 END

; We have an error. We see if it is a genuine one, or if it happened while
; we were determining the type of medium that we have in the drive. If it
; is the latter, we increment the index and try the operation again. If it is
; the former, we handle it as any 'normal' error - reset and retry.

  test byte ptr [bx].DrvFlag,FMedInd   ; do we have a valid media type?
  .IF z and                            ;
  .IF <<byte ptr Drive.ST0> ne 048H>   ; Did we have a drive_not_ready error?
     push    ax                        ;
     mov     al,byte ptr [BX].CurIndex ;
     inc     al                        ;
     .IF <al a MAXINDEX>               ; max index that we can use
        xor al,al                      ; We have wrapped around
     .ENDIF                            ;
     cmp     al,byte ptr [BX].LastInd  ; Is it the one we started with?
     mov     byte ptr [BX].CurIndex,al ;
     pop     ax                        ;
     .IF nz                            ; Yes. Treat this as a normal error
        mov     Drive.State,Calc       ; go back to do calculations
        SJUMP   FlExCalc               ;
     .ENDIF                            ;
  .ENDIF                               ;

ER0:                                   ; p3305
  mov     [bx].DmaOverrunCnt,DMATIMEDELAYCNT ; p3305

; Normal error handle - move to the error handling state, which is RSET

  call  Rst765                       ; Reset the controller
  mov   Drive.State,RSET             ;
  SJUMP FlExRset                     ; go to next state

;** Floppy State Rset
;
; Check to see if we have exceeded the error limit. If we have not, we go
; and try again; otherwise we flag the request as done, and return the error
; to the DOS.

FlExRset:

  .IF <Drive.ErrCnt b ErrLim>      ; Reach error limit?
     inc Drive.ErrCnt              ; We are doing another try
     mov Drive.State,START         ; Restart the request
     SJUMP FlExStart               ; Back to state machine loop
  .ENDIF                           ;   ...
  mov  Drive.State,DONE            ; Request is done.
  call GetDrivePtr                 ; Get pointer to drive information
  mov  di,bx                       ; DS:DI -> BDS for the drive
  and  [di].DrvFlag,NOT FMedInd    ; Reset 'medium known' flag
  mov bl,[di].DriveNum             ; Reset the DSK_STATE byte for
  .IF <bl be 1>                    ; the drive in the ROM's data area
     xor  bh,bh                    ;
     SaveReg es                    ;
     mov  ax,ROMData               ;
     mov  es,ax                    ;
     mov  es:[DSK_STATE[bx]],0     ;
     RestoreReg es                 ;
  .ENDIF                           ; Set error bits in request packet
  mov ax,word ptr Drive.ST0        ; Get ST0, ST1
  mov bl,al                        ;
  and bl,0C0H                      ; Get termination bits

  cmp     bl,40h
  jne     df2                     ; Not abnormal termination - assume gen failure
  mov     bl,ERROR_I24_NOT_READY
  cmp     al,048h                 ; Drive Not Ready
  je      ErrorFound
  mov     bl,ERROR_I24_SEEK
  test    ah,80h                  ; Bad seek
  jne     ErrorFound
  mov     bl,ERROR_I24_CRC
  test    ah,20h                  ; CRC error
  jne     ErrorFound
  mov     bl,ERROR_I24_SECTOR_NOT_FOUND
  test    ah,05h                  ; Sector not found
  jne     ErrorFound
  mov     bl,ERROR_I24_WRITE_PROTECT
  test    ah,02h                  ; Write protect
  jne     ErrorFound
df2:
  mov     bl,ERROR_I24_GEN_FAILURE ; Catch-all error
ErrorFound:
  SaveReg es                       ;
  les di,Drive.Queue               ; get ptr to request packet
  .IF <bl e error_I24_not_ready>   ;
    call Flag_UM                   ; walk packet list, and return
  .ENDIF                           ;   uncertain media on those packets
  mov al,bl                        ; set up error code
  mov ah,(STERR+STDON)/256         ;   ...
  mov es:[di].PktStatus,ax         ; set error and code in packet
  RestoreReg es                    ;   ...
  mov Drive.NumSectors,0           ; say that no i/o took place
  jmp flxd3                        ; advance to Done state (via shortcut)

;**   Floppy State TIMOUT
;
; We encountered a timeout on the drive. We cannot do anything further so we
; flag ErrorLim errors, so that we do not retry this operation, and move
; to the next state - RSET.

FlExTimOut:

  call Rst765                  ; Reset the controller
  call GetDrivePtr             ; Get pointer to drive data struc
  or   [bx].DrvFlag,fRestor    ; Force a restore always
  mov  Drive.ErrCnt,ErrLim     ; Don't retry this one
  mov  Drive.ST0,048H          ; Set not ready error
  mov  Drive.State,RSET        ;
  SJUMP FlExRset               ; Go on to next state

DriveExecute   ENDP

        BREAK <Setup>
;** Setup - Set request parameters into local structure.
;
;       Setup sets the Unit, First, Addr, Count and Flags fields in the
;       device structure which are use to drive the I/O.  The following
;       flags are affected:
;               FWrite  This is a write request, not a read
;               FVerify This is a write with verify (verify when write is
;                       cleared)
;       Other fields are copied from the DOS request packet
;
;       ENTRY   DS:SI   Pointer to device variables IOStruc
;               ES:BX   Current request
;
;       EXIT    The following variables are set
;               [SI].Unit
;               [SI].First              The hidden sectors are added.
;               [SI].RealAddr
;               [SI].CurrAddr
;               [SI].Count
;               [SI].Flags
;               [SI].StartSec
;
;       USES AX, DI, FLAGS

        public Setup
Setup proc

  and  [si].Flags,Factive               ; Zero out flags.
  mov  cl, [si].Current                 ; Save last request (used only on
                                        ; format commands)
  mov  al,es:[bx].PktUnit               ;
  mov  [si].Current,al                  ; Set drive needed
  call Get_BDS_Addr                     ; Get BDS for this drive
  mov  al,[di].DriveNum                 ;
  mov  [si].PhysDrv,al                  ; Save physical drive number
  .IF <es:[bx].PktCmd e DISKOP_CHKCHNG> ;
     or [si].Flags,fSPECIAL+fINTERNAL   ; Set 'special function' flag
     ret                                ;
  .ENDIF                                ;

  mov ax,es:[bx].IOcount                ;
  mov [si].Count,ax                     ; Set number of sectors to do.
  mov ax,word ptr es:[bx].IOpData       ;
  mov word ptr [si].RealAddr,ax         ;
  mov word ptr [si].CurrAddr,ax         ;
  mov ax,word ptr es:[BX].IOpData+2     ;
  mov word ptr [si].RealAddr+2,ax       ; copy data address
  mov word ptr [si].CurrAddr+2,ax       ;
  mov byte ptr [si].SectorSize,2        ; assume 512-byte sectors
  mov ax,word ptr es:[bx].IOPhysRBA+2   ; get the starting sector #
  mov word ptr [si].First+2,ax          ;   already has hidden sector
  mov ax,word ptr es:[bx].IOPhysRBA     ;   adjustment
  mov word ptr [si].First,ax            ; set 1st sector of I/O
  mov word ptr [si].StartSec,0          ; set sector offset to 0
                                        ;   used for non-standard IOCTL
                                        ;   track layouts

  .IF <es:[bx].PktCmd e DISKOP_READ>
     or [si].Flags,fREAD
  .ELSEIF <es:[bx].PktCmd e DISKOP_NCREAD>
     or [si].Flags,fREAD+fNOCACHE
  .ELSEIF <es:[bx].PktCmd e DISKOP_INTERNREAD>
     or [si].Flags,fINTERNAL+fREAD+fNOCACHE
  .ELSEIF <es:[bx].PktCmd e DISKOP_WRITE>
     or [si].Flags,fWRITE
  .ELSEIF <es:[bx].PktCmd e DISKOP_NCWRITE>
     or [si].Flags,fWRITE+fNOCACHE
  .ELSEIF <es:[bx].PktCmd e DISKOP_WRITEV>
     or [si].Flags,fWRITE+fVERIFY
  .ELSEIF <es:[bx].PktCmd e DISKOP_NCWRITEV>
     or [si].Flags,fWRITE+fVERIFY+fNOCACHE
  .ELSE                                 ; it is an IOCTL command
     or  [si].Flags,fIOCTL+fINTERNAL    ; set IOCTL flags
     mov al,es:[BX].IOC_SecSiz          ;   ...
     mov [si].SectorSize,al             ; SectorSize = IOC_secSiz
     mov ax,es:[BX].IOC_StartSec        ;   ...
     mov [si].StartSec,ax               ; StartSec   = IOC_StartSec
     mov al,es:[BX].IOC_Fun             ;   ...
     .IF <al e IOCTL_Read>              ; set operation type flag
        or [si].Flags,fRead             ;   ...
     .ELSEIF <al e IOCTL_Write>         ;   ...
        or [si].Flags,fWrite            ;   ...
     .ELSEIF <al e IOCTL_Verify>        ;   ...
        or [si].Flags,fVerify+fNOCACHE  ;   ...
     .ELSEIF <al e IOCTL_Format>        ;   ...
        or  [si].Flags,fFormat+fNOCACHE ;   ...
        cmp  cl, [si].Current           ; Compare this request with the last
        je   no_restore                 ; request.  If we switched drives then
                                        ; a restore must be done before a format
        or   [di].DrvFlag, Frestor      ; is issued as per controler spec.
       no_restore:                      ; NOTE: ch = saved last current drive.
                                        ;
     .ENDIF                             ;   ...
  .ENDIF                                ;   ...

  ret                                   ;   ...

Setup endp

        BREAK <MapSector>
;** MapSector - compute head, sector, cylinder
;
;       MapSector takes the fields set up by Setup, and figures out the
;       head, cylinder and sector involved.  If the request involves
;       multiple sectors, it figures out how many can be done at once
;       based on the number of sectors left on the track and that the
;       target addresses' offset does not wrap around 64k (the DMA on the
;       PC uses a 20 bit address, but the high 4 bits do not change when
;       the low 16 cycle back to 0).  If the request wraps around 64k,
;       it is split into 2 or 3 pieces which are all data before wrap,
;       after wrap and the wrap itself.  The wrap itself is transferred via a
;       temp buffer (ScratchBuffer)
;
;       ENTRY   DS:SI   Pointer to device variables IOStruc
;               ES:BX   Current Request
;               DS:DI   BPB for drive
;
;       EXIT    The following variables are set
;                       [SI].Flags
;                       [SI].Head
;                       [SI].Cyl
;                       [SI].Sector
;                       [SI].NumSectors
;                       [SI].SecToGo
;                       [SI].NumBytes
;                       [SI].Addr
;
;       USES    AX, CX, DX, Flags

        public  MapSector
MapSector proc

  test [si].Flags,fSPECIAL            ; quick exit if fSPECIAL
  RETNZ                               ; no setting up for SPECIAL functions
  mov  cx,word ptr [si].RealAddr      ; copy RealAddr to Addr
  mov  word ptr [si].Addr,cx          ;   RealAddr is updated in FlExDone
  mov  cx,word ptr [si].RealAddr+2    ;   for operations that need multiple
  mov  word ptr [si].Addr+2,cx        ;   read/writes

; Calculate the head, cylinder, and sector of the start of the request

  mov ax,word ptr [si].First          ; get request packet starting sector
  mov dx,word ptr [si].First+2        ;    ...
  div  [di].BPBtrksiz                 ; divide by sectors/track, assume AX OK
  add  dx,[si].StartSec               ; add sector offet to starting sector
  mov  [si].Sector,dl                 ; set sector to start at, assume < 256
  xor  dx,dx                          ; zero out the remainder
  div  [di].BPBnhead                  ; divide by number of heads
  mov  [si].Head,dl                   ; set head number
  mov  [si].Cyl,ax                    ; set cylinder number
  .IF <BIT [di].DrvTypFlag z fNon_Removable> and ; Floppy
  .IF <BIT [si].Flags z fIOCTL>       ; but not IOCTL
     mov  ax,[di].BPBtrksiz           ;
     sub  al,[si].Sector              ; al = # of sectors left on track
     xor  ah,ah                       ;
     .IF <ax a [si].Count>            ;
        mov ax,[si].Count             ; yes, use the actual # wanted
     .ENDIF                           ;   ...
  .ELSE                               ; fixed disk case
     mov ax,[si].Count                ; let it step heads
  .ENDIF                              ;   ...
  mov [si].NumSectors,ax              ; set number to do this time
  mov [si].SecToGo,ax                 ;   ...

; Now we have to normalize offset (add in paragraph) and then see if adding
; [si].NumSectors causes overflow.  If it does, DMA will trash memory, so
; decrement NumSectors and loop. We do not worry about DMA violations if we
; are accessing the fixed disk.
; We are using 32-bit addresses so cannot do any segmentation arithmetic

  mov  cx,word ptr [si].Addr             ; get low 16 bits of physical address
  test [si].Flags,fFormat                ; is it a format request ?
  .IF z                                  ; not a format request
     .REPEAT                             ;
        mov   al,[si].SectorSize         ;
        call  SectorIndexToSectorSize    ;
        mul   [si].NumSectors            ; Get # of bytes in transfer
        mov   [si].NumBytes,ax           ; Set # of bytes involved
        add   ax,cx                      ; Get final offset
        RETNC                            ; No overflow, DMA will be OK.
        RETZ                             ; Overflow exact?  Request is OK.
        test  [di].DrvTypFlag,fNon_Removable
        RETNZ                            ; Do not worry about DMA if fixed disk
        dec   [si].NumSectors            ; Overflow, try using 1 less
        dec   [si].SecToGo               ;   ...
     .UNTIL z                            ;   ...

; If we got here, no sectors can be transferred before the 64K
; boundry.  One sector must be transferred through a scratch buffer.

     inc [si].NumSectors                 ; Doing 1 sector of I/O
     inc [si].SecToGo                    ;   ...
  .ELSE                                  ; format operation

; For the format operation, we need to ensure that the track table does not
; cross over a DMA boundary.

     mov   ax,4                           ; size of entry for each sector
     mul   [si].NumSectors                ; Get # of bytes in transfer
     mov   [si].NumBytes,ax               ; Set # of bytes involved
     add   ax,cx                          ; Get final offset
     RETNC                                ; No overflow, DMA will be OK.
     RETZ                                 ; overflow exact?  Request is OK.
     test  [di].DrvTypFlag,fNon_Removable ; fixed disk ?
     RETNZ                                ; don't worry about DMA if fixed disk
  .ENDIF                                  ;   ...
  or [si].Flags,FWrap                     ; flag we are using scratch
  mov  ax,word ptr ScratchBufPhys         ; get 32 bit physical address of
  mov  word ptr [si].Addr,ax              ;   ScratchBuffer
  mov  ax,word ptr ScratchBufPhys+2       ;   ...
  mov  word ptr [si].Addr+2,ax            ; change buffer to scratch
  test [si].Flags,FWrite+fFormat          ; doing a write or format?
  .IF nz                                  ; must move data in scratch buffer
     SaveReg <es,ds,di,si,bx>		  ;   ...
     mov  cx,[si].NumBytes		  ; move 512 bytes or 4 bytes
     push ds				  ; es:di -> offset to scratch buffer
     pop  es				  ; es:di -> offset to scratch buffer
     mov  di, ScratchBuffer		  ; ""
     mov  ax, word ptr [si].RealAddr+2
     mov  bx, word ptr [si].RealAddr
     call PhysToVirtDSSI		  ; ds:si = virtual data address
     cld				  ;
     rep movsb				  ; copy the write buffer
     mov dl,DevHlp_UNPhysToVirt
     call dword ptr es:[DevHlpFunc]
     RestoreReg <bx,si,di,ds,es>
  .ENDIF
  ret

MapSector endp

        BREAK   <Rcl765>
;** Rcl765 - Recalibrate a floppy drive
;
;         Recalibrate the current drive.  Clear Restore flag, set
;       cylinder to unknown and issue command to controller.
;
;       ENTRY   (DS:BX) = Pointer to drive structure
;
;       EXIT    NONE
;
;       USES    AX, DX

        DebugPublic Rcl765
Rcl765 proc

  and  [bx].DrvFlag,NOT Frestor    ; Have restored drive
  mov  [bx].CurCyl,-1              ; Flag don't know where we are
  mov  al,FD_CRESET
  call Put765                      ; Put out reset command
  mov  al,Drive.PhysDrv            ; Get current drive number
  call Put765                      ; Tell controller which drive
  ret

Rcl765 endp

        BREAK <Rst765>
;**  Rst765  - Reset the floppy controller.
;
;       ENTRY   DS = BiosGroup
;
;       EXIT    'C' set if controller timed out.
;
;       USES    AX, CX, DX, Flags
;
;       NOTE:  This routine should be called whenever you get an error from
;       the floppy controller.  It performs basically the same functions
;       that the IBM ROM <Reset> function performs, namely it sets the
;       DOR to disable interrupts and reset the controller chip, then
;       it reenables interrupts and restarts the controller.

        public Reset_Floppy
Reset_Floppy label near

Rst765 proc

  SaveReg <bx,cx,es>          ;   ...
  call GetDORImage            ; @@p2067
  mov  cx,RomData             ;
  mov  es,cx                  ;
  assume es:RomData           ;
  inc Floppy_Reset            ; indicate reset pending to interrupt handler
  mov dx,FD_PSEL              ;
  DISABLE                     ;
  mov al,Drive.DOR            ;
  and al,NOT DORmask          ;
  or  al,08H                  ; Only reset the controller
  out dx,al                   ; Reprogram the DOR to reset the drive
  mov Seek_Status,0           ; Indicate recal required
  mov Diskette_Status,0       ; Indicate ROM diskette status is OK
  or  al,DORmask              ; Raise the reset line
  mov Drive.DOR,al            ; Update DOR value
  mov cx,10000                ;
  out dx,al                   ;
  ENABLE                      ; sit here and wait for the floppy int

  .FOR bl = 1 to 2                      ; repeat this loop twice
     xor cx,cx                          ;   ...
     .REPEAT                            ; busy loop until interrupt from
        test Seek_Status,F_FLOPPY_INTR  ;   diskette controller
        .LEAVE nz                       ;   ...
     .LOOP                              ;   ...
     test Seek_Status,F_FLOPPY_INTR     ;   ...
     .LEAVE nz                          ;   ...
  .NEXT bl                              ; repeat the big loop 2 times
  and Seek_Status,NOT F_FLOPPY_INTR     ; clear interrupt flag
  assume es:nothing                     ;   ...

  xor cx,cx                             ; another big loop
  .REPEAT                               ;   ...
     call Sense765                      ; sense interrupt status
    .LEAVE <al eq 02H>                  ; illegal command?
  .LOOP                                 ;   ...

  dec Floppy_Reset                      ; Clear reset pending flag
  RestoreReg <es,cx,bx>                 ;   ...
  ret                                   ;   ... .

Rst765 endp

        BREAK <Spec765>
;**     Spec765 - Load the drive specs into the controller.
;
;       USES    AX,DX

       Public Spec765
Spec765 proc

  mov  al,FD_CSPEC
  call Put765
  mov  al,DiskSpecify_1
  call GetFloppyParam
  call Put765
  mov  al,DiskSpecify_2
  call GetFloppyParam
  call Put765
  ret

Spec765 endp

        BREAK <Sense765>
;**  Sense765  -  Get the interrupt status from the controller.
;
;    This routine is called after the interrupt issued by either a
;  recalibrate or seek command.  It issues a Sense Interrupt Status
;  command to the FDC and returns the relevant status bits in the 765's
;  ST0 register.  The FDC sets the interrupt code (Bits 6 and 7) of the
;  ST0 register to one of the following values:
;
;       Bit 6   Bit 7
;         0       0             Normal termination of seek command (no error)
;         1       0             Abnormal termination of Seek or Recalibrate.
;         1       1             Ready Line changed state (drive door opened)
;
;       NOTE    If the value of bits 6 and 7 of ST0 is 10, and the
;               SEEK END bit is 0 (i.e. ST0=80H), this indicates that an
;               illegal command had been issued to the NEC and we should
;               return WITHOUT attempting to read the PCN register.
;               (If we do attempt to read the register, we will hang).
;
;       ENTRY   NONE
;
;       EXIT    (AX) = Contents of bits 6 and 7 as described above.
;
;       USES    AX,CX,DX, Flags

        Public Sense765
Sense765 proc

  mov  al,FD_CSENSE           ; Get status
  call Put765                 ;   ...
  call Get765                 ; Read ST0
  .IF <al ne 80H>             ;
     push ax                  ; Save status
     call Get765              ; Read and ignore present cylinder num
     pop  AX                  ; Restore status
  .ENDIF                      ;
  shr al,6                    ; Shift bits down
  and ax,3                    ; Leave only error bits
  ret                         ;   ...

Sense765 endp

        BREAK <Sel765>
;**  Sel765  - Select the specified drive.
;
;  Select the current drive.  Return carry set if must wait until drive is
;  ready. DriveExecute will be called again when the drive is ready.  The
;  code must wait either for a motor start or head load delay, otherwise it
;  returns with carry clear.
;
;       ENTRY   NONE
;
;       EXIT    'C' Set   - We have to wait for a motor on/head load delay
;               'C' Clear - We do not have to wait.
;
;       USES    AX,BX,CX,DX

        Public Sel765
Sel765 proc

  call GetDORImage                   ; @@p2067
  mov  dx,FD_PSEL                    ; set dx = Digital Output Register
  mov  cl,Drive.PhysDrv              ; get physical drive number
  cmp  cl,LastDrv                    ; wanted same as current?
  mov  LastDrv,cl                    ; set new current unit
  .IF e                              ; if same as last one
     mov  al,1 SHL 4                 ;   ...
     shl  al,cl                      ; al = Bit for drive's motor on
     test al,Drive.DOR               ; is the drive's motor still on?
     .IF ne                          ; no, must turn it back on
        mov  al,Drive.DOR            ; ok, drive is on
        out  dx,al                   ; spit out DOR register, to make sure
        clc                          ; clear carry, don't have to wait
        ret                          ; return with no wait
     .ENDIF                          ;   ...
  .ENDIF                             ;   ...

;  The drive # had to be changed.  Select the new unit number.
;  CL = New drive number to activate.
;  DX = FD_PSEL (Floppy DOR)

  mov  al,NOT(3)                     ; Drive select is low 2 bits
  and  al,Drive.DOR                  ; lose old select bits
  or   al,DORmask                    ;
  mov  cl,Drive.PhysDrv              ; get unit number
  or   al,cl                         ; put in new select bits
  mov  Drive.DOR,al                  ; save new bits
  mov  al,1 SHL 4                    ;
  shl  al,cl                         ; get bit for motor is on
  test al,Drive.DOR                  ; is drive's motor on?
  .IF ne                             ; No, must start motor
     mov  al,Drive.DOR               ;
     out  dx,al                      ; load the head
     call UpdateRomMotorStatus       ;                                 @p2067
     mov  ax,DelayLoad               ; load head delay
     call SetTimer1                  ; wait for timer pop
     ret                             ;   ...
  .ENDIF                             ;   ...

;  The new drive's motor was not on.  We must turn the drive motor
;  on, and wait Motor_Start (a field in the floppy parameter table)
;  milliseconds.
;  DX = FD_PSEL

  or   Drive.DOR,al                  ; Turn on motor start bit
  mov  al,Drive.DOR                  ;
  out  dx,al                         ; Start the motor
  call UpdateRomMotorStatus          ;                                 @p2067
  mov  al,Motor_Start                ;
  call GetFloppyParam                ; Get the proper delay time in 1/8 sec
  test Drive.Flags,Fwrite            ;
  .IF nz and                         ;
  .IF <al b 8>                       ; For write, delay must be at least 1 sec.
     mov  AL,8                       ;   ...
  .ENDIF                             ;   ...
  mov  cl,125                        ;   ...
  mul  cl                            ; convert to milliseconds
  call SetTimer1                     ; Set timer for motor startup
  ret                                ;   ...

Sel765 endp

        BREAK <Seek765>
;**  Seek765 - Tell the floppy controller to seek to a new cylinder.
;
;       ENTRY   (DS:BX) = Pointer to drive structure for request.
;       EXIT    'C' Set if we are going to perform a seek.
;               'C' Clear if we are already on the correct cylinder.
;       USES    AX, DX

        DebugPublic Seek765
Seek765 proc

  mov  ax,Drive.Cyl            ; get cylinder wanted
  cmp  ax,[bx].CurCyl          ; already on cylinder?
  RETZ                         ; yes, return with carry clear
  mov  [bx].CurCyl,AX          ; set the new current cylinder
  mov  al,FD_CSEEK             ;
  call Put765                  ; seek command
  mov  al,Drive.Head           ; get head desired
  shl  al,2                    ; move head # 2 bits left
  or   al,Drive.PhysDrv        ; low 2 bits are unit (hhuu)
  call Put765                  ; put out drive and head select
  mov  ax,Drive.Cyl            ;
  .IF <[bx].CurIndex e 1>      ; 48 tpi medium in 96 tpi drive requires
     shl ax,1                  ; a double step
  .ENDIF                       ;   ...
  CALL Put765                  ; give controller the cylinder
  SaveReg bx                   ;
  mov bl,[bx].DriveNum         ;
  xor bh,bh                    ;
  .IF <bx le 1>                ; update ROM area
     SaveReg <es,cx>           ;
     mov cx,ROMData            ;
     mov es,cx                 ;
     mov es:[DSK_TRK[bx]],al   ;
     RestoreReg <cx,es>        ;
  .ENDIF                       ;
  RestoreReg bx                ;
  stc                          ; Set carry, must wait interrupt
  ret                          ;   ...

Seek765 endp

        BREAK <RdWr765>
;**  RdWr765  - Issue a read/write command to NEC765 Disk controller
;
;       Start the Read/Write/Format.  Set up the DMA channel and give a read,
;       write or format command to the controller, depending on flag.
;
;       ENTRY   Relevant fields in Drive (IOStruc) are set up.
;
;       EXIT    NONE
;
;       USES    AX, CX, DX

        DebugPublic RdWr765
RdWr765 proc

  mov  ah,FD_DMA                     ;
  mov  si,DataOffset Drive           ;
  test Drive.Flags,FFormat           ; format ?
  .IF z                              ; z = not format
     test Drive.Flags,Fwrite         ; write ?
     .IF z                           ; z = write
        mov  al,DMA_READ             ; this is a read
        test Drive.Flags,FVerify     ; this test MUST be done after
        .IF ne                       ;    the test for FWrite
           mov al,DMA_VERIFY         ;
        .ENDIF                       ;   ...
        call DMAsetup                ; Set up the DMA.
        xchg al,ah                   ;
        out  PDMA+10,al              ; initialize the floppy DMA channel.
        mov  al,FD_CREAD             ; want to read.
     .ELSE                           ; handle write
        mov  al,DMA_WRITE            ;
        call DMAsetup                ; set DMA up for write
        xchg al,ah                   ;
        out  PDMA+10,al              ; initialize the floppy DMA channel
        mov  al,FD_CWRITE            ; want to write
     .ENDIF                          ;
     call Put765                     ; Put out command
     mov  al,Drive.Head              ;
     shl  al,2                       ; Form HHxx Binary
     or   al,Drive.PhysDrv           ; Form HHUU
     call Put765                     ; Output unit and head
     mov  ax,Drive.Cyl               ;
     call Put765                     ; Output cylinder
     mov  al,Drive.Head              ;
     call Put765                     ; Output head again?
     mov  al,Drive.Sector            ;
     inc  al                         ;
     call Put765                     ; Output sector
     mov  al,Drive.SectorSize        ;
     call Put765                     ; Tell controller sector size
     mov  al,EOT                     ;
     call GetFloppyParam             ; Get number of sectors/cylinder
     call Put765                     ; Tell controller
     mov  al,GapLength               ;
     call GetFloppyParam             ; Gap length for Read/Write
     call Put765                     ; Tell controller gap length
     mov  al,DTL                     ;
     call GetFloppyParam             ; Get value for DTL
     call Put765                     ; Since bytes/sector#0, this is a
  .ELSE                              ; meaningless value, but controller
                                     ; wants to see something

; We are formatting the track. We issue a WRITE to the DMA, sending out the
; information that the format operation needs (C,H,R,N), and then issue the
; format command to the disk controller.

     mov  al,DMA_WRITE               ;
     call DMAsetup                   ; Set DMA up for write.
     xchg al,ah                      ;
     out  PDMA+10,AL                 ; Initialize the floppy DMA channel.
     mov  al,FD_CFORMAT              ; Want to format.
     call Put765                     ; Put out command
     mov  al,Drive.Head              ;
     shl  al,2                       ; Form HHxx Binary
     or   al,Drive.PhysDrv           ; Form HHUU
     call Put765                     ; Output unit and head
     mov  al,Drive.SectorSize        ; Number of bytes per sector
     call Put765                     ;
     mov  ax,Drive.NumSectors        ;
     call Put765                     ; Output # of sectors on this track
     mov  al,FMTGapLength            ;
     call GetFloppyParam             ; Gap length for Format.
     call Put765                     ; Tell controller gap length
     mov  al,FMTFillByte             ;
     call GetFloppyParam             ; Get fill byte for Format
     call Put765                     ;
  .ENDIF                             ;
  ret                                ;

RdWr765 endp

        BREAK <Fini765>
;**  Fini765  gets the completion status.
;
;       ENTRY   NONE
;
;       EXIT    AL - Error code if AL non zero.
;
;       USES    AX, CX, DX, Flags

        Public Fini765
Fini765 proc

  SaveReg <ds,di,es>
  push ds
  pop  es
  mov  di,DataOffset Drive.ST0    ; ES:DI->location to store status
  cld                             ; @@p2067
  mov  cx,7                       ; Read 7 bytes from the NEC765
  .REPEAT                         ;
     call  Get765                 ; Get a byte from the 765
     stosb                        ; Save it in the floppy structure.
  .LOOP                           ; Get another byte.
  mov al,Drive.ST0                ;
  shr al,6                        ; Get the error code from the ST0 reg.
  and ax,3                        ; Mask off all but return value.
  mov cx,ROMData                  ; @@p2067
  mov es,cx                       ; @@p2067
  mov di,offset NEC_Status        ; @@p2067
  mov si,DataOffset Drive.ST0     ; @@p2067
  mov  cx,7                       ; @@p2067
  cld                             ; @@p2067
  rep movsb                       ; @@p2067
  RestoreReg <es,di,ds>           ;
  ret                             ;

Fini765 endp

;**  UpdateRomMotorStatus Updates the Rom Motor Status and Select byte.
;
;  Input   :    AL = Drive.DOR
;  Destroys:    CX,AX
;  Returns:     none
;
        Public UpdateRomMotorStatus
UpdateRomMotorStatus proc

  push es                            ;                                 @p2067
  mov  cx,RomData                    ;                                 @p2067
  mov  es,cx                         ;                                 @p2067
  assume es:RomData                  ;                                 @p2067
  and  al,NOT DORmask                ;                                 @p2067
  ror  al,4                          ;                                 @p2067
  mov  Motor_Status,al               ; tell ROM about drive motor state@p2067
  mov  Motor_Count,07fh              ; keep the motor on
  pop  es                            ;                                 @p2067
  assume es:nothing                  ;                                 @p2067

   ret
UpdateRomMotorStatus endp

;**  GetDORImage gets a DOR image from the motor status byte in BIOS Data area
;
;  Destroys:    none
;  Returns:     Drive.DOR

        Public GetDORImage
GetDORImage proc

  SaveReg <es,cx,ax>                 ; @@p2067
  mov  cx,RomData                    ; @@p2067
  mov  es,cx                         ; @@p2067
  assume es:RomData                  ; @@p2067
  mov  Motor_Count,07fh              ; keep motor running if on
  mov  al,Motor_Status               ; @@p2067
  ror  al,4                          ; @@p2067
  or   al,0ch                        ; @@p2067
  mov   Drive.DOR,al                 ; @@p2067
  RestoreReg <ax,cx,es>              ; @@p2067
  assume es:nothing                  ; @@p2067
  ret
GetDORImage endp

        BREAK <Get765 & Put765>
;**  Put765  writes a command to the controller.
;
;       ENTRY   AL = value
;
;       EXIT    NONE
;
;       USES    DX, Flags

       public Put765
Put765 proc

  ERRNZ <FD_PDAT-FD_PSTAT-1>         ; FD_PStat and FD_PDat must be contig.
  mov ah,al                          ; Save the value to write
  mov dx,FD_PSTAT                    ;
  .REPEAT                            ;
     in  al,dx                       ; Get status
     and al,FD_SDIO+FD_SRQM          ;
  .UNTIL <al e FD_SRQM>              ; Controller ready for data?
  mov al,ah                          ; Get value back
  inc dx                             ;
  out dx,al                          ; Put out value
  ret                                ;

Put765 endp

;**  Get765  gets a value back from the controller into AL.
;
;  Destroys:    AX,DX
;  Returns:     AL

        Public Get765
Get765 proc

  ERRNZ <FD_PDAT-FD_PSTAT-1>    ; FD_PSTAT and FD_PDAT must be contig
  .REPEAT                       ; until ready
     mov dx,FD_PSTAT            ; read the status port
     in  al,dx                  ; get status
     and al,FD_SDIO+FD_SRQM     ; mask off status bits
  .UNTIL <al e FD_SDIO+FD_SRQM> ; wait for ready
  inc dx                        ; dx points to FD_PDAT port now
  in  al,dx                     ; get value from controller
  ret                           ;   ...

Get765 endp

        BREAK <DMAsetup>
;***    DMAsetup - Set the DMA channel up to do the I/O
;
;       ENTRY   AL = DMA mode
;               AH = DMA channel number (2 or 3 only)
;               SI = pointer to device parameters
;
;       EXIT    AX is preserved
;
;       USES     CX, DX, Flags

        Public DMAsetup
DMAsetup proc

  SaveReg ax                   ;   ...
  DISABLE                      ; Prevent net card from coming in.
  xchg ah,al                   ; (AL) = DMA Mode
  or   al,4                    ;
  out  PDMA+10,al              ; set channel's mask bit
  IOWait                       ;
  out  PDMA+12,al              ; clear byte pointer F/F
  pop  ax                      ; restore channel and mode
  push ax                      ;
  or   al,ah                   ; add channel number to command
  out  PDMA+11,al              ; Set DMA mode
  mov  dx,PDMA                 ;
  rol  ah,1                    ;
  add  dl,ah                   ;
  mov  ax,word ptr [si].Addr   ; Get low word of address
  out  dx,al                   ; Output low byte of address
  IOWait                       ;
  xchg al,ah                   ;
  out  dx,al                   ; Output second byte of address
  inc  dx                      ; address `word' count register
  mov  ax,[si].NumBytes        ; # bytes in request
  dec  ax                      ;
  out  dx,al                   ;
  xchg al,ah                   ;
  IOWait                       ;
  out  dx,al                   ; Tell DMA how many bytes
  pop  ax                      ; get back DMA channel number
  push ax                      ;
  mov  dl,PDMAX                ;
  add  dl,ah                   ;
  mov  ax,word ptr [si].Addr+2 ;
  out  dx,al                   ; Output highest byte of address
  ENABLE                       ;   ...
  RestoreReg ax                ;   ...
  ret                          ;   ...

DMAsetup endp

        BREAK <SetROMDskStat>
;**  SetROMDskStat - Set the ROM DSK_STATE byte correctly
;
;       SetROMDskStat sets the ROM's DSK_STATE byte correctly for
;       drives 0 and 1. We need to maintain these bytes, together
;       with the ones that signify the current cylinder because the
;       ROM would otherwise have no way of determining what happened.
;
;       ENTRY   BX - BDS for the drive
;               DS - BiosGroup
;
;       EXIT    NONE
;
;       ALL registers preserved except Flags
;
;       ALGORITHM
;           The value placed in DSK_STATE in the ROM data area is
;           as follows:
;               Bits 0,1,2 - (5 - CurIndex)
;               Bit  3     - 0
;               Bit  4     - FMedInd flag bit in the DrvFlag field
;               Bit  5     - Set to 1 if CurIndex=1
;               Bits 6,7   - CurIndex

        Public SetROMDskStat
SetROMDskStat proc

  pusha                        ;   ...
  mov di,bx                    ; DI -> BDS for drive
  xor bh,bh                    ;
  mov bl,[di].Curindex         ; Bits 6 and 7 are the value of
  mov al,RATETBL[bx]           ; the RATE based on the value of CurIndex
  shl al,6                     ; Sets bits 6 and 7
  mov bl,[di].DriveNum         ; See if this drive is > 1
  .IF <bl be 1>                ; If it is, we do not need to do the setup
     mov ah,[di].CurIndex      ; Bit 5 is set if CurIndex is 1
     and ah,1                  ;
     shl ah,5                  ;
     or  al,ah                 ; Sets bit 5
     mov cx,[di].DrvFlag       ; Bit 4 is the value of the
     and cl,FMedInd            ; FMedInd flag bit in DrvFlag
     or  al,cl                 ; Bit 3 is reserved = 0
     mov ah,5                  ; Bits 0,1,2 are set to (5 - CurIndex)
     sub ah,[di].CurIndex      ;
     or  al,ah                 ;
     SaveReg es                ; bx contans drive number
     mov cx,ROMData            ; update ROM area
     mov es,cx                 ;
     mov es:[DSK_STATE[bx]],al ;
     mov es:[LASTRATE],al      ; fix 1.0 bug.
     RestoreReg es             ;
  .ENDIF                       ;
  popa                         ;   ...
  ret                          ;   ...

SetROMDskStat endp


public Media_Check
Media_Check proc near
        Call Do_Check
        ret
Media_Check endp

        BREAK <GetFloppyParam>
;**     GetFloppyParam - Read controller parameter
;
;       GetFloppyParam is called to get a disk parameter from the parameter
;  block set up by the BIOS.  This block allows disk parameters to be changed
;  from the standard.
;
; We call GetDrivePtr to get the drive parameters. We can do this so long as
; Drive.Current has been set up correctly, i.e. we assume that GetFloppyParam
; is never called before Setup.
;
;       ENTRY   AL = parameter desired (see FloppyParam structure)
;               BX = pointer to DriveStruc for this drive
;               DS = BiosGroup
;
;       EXIT    AL = parameter byte desired
;
;       USES    AX, Flags

        Public GetFloppyParam
GetFloppyParam proc

  SaveReg bx                       ;   ...
  call GetDrivePtr                 ; ds:bx -> drive parameters
  mov  bl,byte ptr [bx].CurIndex   ; get media type index
  xor  bh,bh                       ;   ...
  or   bl,bl                       ; set to zero if negative
  .IF s or                         ;   or too large
  .IF <bl a MAXINDEX>              ;   ...
     xor bl,bl                     ;   ...
  .ENDIF                           ;   ...
  shl bx,1                         ; get word index
  mov bx,DPTable[BX]               ; get correct disk base table
  xor ah,ah                        ;   ...
  add bx,ax                        ; add in block offset
  mov al,[bx]                      ;   ...
  RestoreReg bx                    ;   ...
  ret                              ;   ...

GetFloppyParam  endp

        BREAK <CheckWrap>
;**  CheckWrap - check whether a request crosses a 64Kb boundary
;
;       See if a sector in the an I/O request will cross a 65K physical
;       memory boundary.  We do this because a DMA bug forces us
;       to read straddle sectors into some other non-65K-cross location
;       and then copy the date over to its straddle location.
;
;       We have a scratch buffer reserved for this purpose.  We must reserve
;       it now because at the time of the actual read/write we may get
;       deadlocked:  the buffer may already be reserved by a routine that
;       wants to read into it but that read is further down in the queue and
;       cannot be processed until the current read is done, which in turn
;       is waiting on the buffer.
;
;       The boundary-crossing request may not be the first request on the
;       queue, but if we get the buffer then we know that none of the other
;       requests on the queue want the buffer.
;
;       CheckWrap is called at SYSTEM/TASK and BOOT time.  Since there won't
;       be any contention at boot time we won't ever try to use DevHlp.
;       Its easier to go ahead and set and clear ScratchBufSem at Boot
;       time then it is to be checking for Boottime.
;
;       ENTRY   SI      Low Word of Transfer Address (Physical address)
;               DS:DI   Pointer to BPB
;               CX      Sector count
;               ES:BX   Request packet
;
;       EXIT    Interrupts Enabled (perhaps)
;               ScratchBuffer reserved, if needed
;
;       USES    BP, Flags
;
;       Notes:
;               1) CheckDMAWrap will only be called when DOS is present,
;                  during the bootstrap process, it is never called, since
;                  there cannot be contention for the scratch buffer semaphore
;
;               2) We explicitly test to see if the request will cross the
;                  64K boundary on an even sector break.  If that is the case,
;                  we will not be using the ScratchBuffer, so we do not have
;                  to wait.

        public CheckWrap
CheckWrap proc

  SaveReg ax                            ;   ...
  mov  ax,[di].BPBsecsiz                ; get sector size
  call CheckDMAWrap                     ; see if it crosses
  .IF c                                 ; c = crosses the boundary
     push bx                            ;   ...
     mov  bx,DataOffset ScratchBufSem   ; obtain the semaphore and maybe
     call SWait                         ;   block
     pop  bx                            ;   ...
  .ENDIF                                ;   ...
  RestoreReg ax                         ;   ...
  ret                                   ;   ...

CheckWrap endp

        BREAK <CheckDMAWrap>
;**  CheckDMAWrap - Check to see if request will wrap 64K boundary
;
;       CheckDMAWrap checks to see if the I/O request will wrap a
;       64K boundary.
;
;       ENTRY   AX      Sector size for medium (Usually 512)
;               CX      Sector count
;               ES:BX   Request Packet
;               SI      Low word of transfer address
;
;       EXIT    'C' set if request would wrap 64K boundary
;               'C' clear otherwise
;
;       USES    Flags

        Public CheckDMAWrap
CheckDMAWrap proc

  SaveReg <ax,dx>                          ;   ...
  .IF <es:[bx].PktCmd e DISKOP_IOCTL>      ; format gets special treatment
     mov  al,es:[bx].IOC_SecSiz            ; set up read/write requests
     call SectorIndexToSectorSize          ; ax = sector size
     .IF <es:[bx].IOC_Fun e IOCTL_Format>  ; handle format special
        mov ax,4                           ; 4 bytes for every sector
     .ENDIF                                ;   ...
  .ENDIF                                   ;   ...
  mul cx                                   ; carry means it is > 64K
  .IF nc and                               ; more than 64K, we'll wrap
  mov dx,si                                ; si = starting address
  add dx,ax                                ; see if offset+nbytes overflows
  .IF c and                                ; nc = doesn't cross boundary
  clc                                      ; assume we will not wrap
  .IF nz and                               ; 0 = fits exactly
  test dx,01FFH                            ;
  .IF nz                                   ; even sector boundary
     stc                                   ; say that we will wrap
  .ENDIF                                   ;   ...
  RestoreReg <dx,ax>                       ;   ...
  ret                                      ;   ...

CheckDMAWrap endp

        BREAK <TMotorOff>
;** TMotorOff - turn off drive motor
;
;   TMotorOff is called by timer services to turn off the diskette drive motor
;   when the timer has expired.  Turn off the drive motor and if the device
;   is active, call the state machine to handle a timeout error.
;
;   ENTRY CS and DS set by timer services
;
;   EXIT  DriveStruc updated
;
;   USES  flags

        public TMotorOff
TMotorOff proc

        call GetDORImage                       ; @@p2067
        mov al,1 shl 4                         ;   ...
        shl al,cl                              ; get bit mask for motor on
        test Drive.DOR,al                      ; is motor already off ?
        .IF nz                                 ; nz = running
           not al                              ; get all bits except this one
           and Drive.DOR,al                    ; clear this motor on
           mov dx,FD_PSEL                      ; set up for motor off command
           mov al,Drive.DOR                    ;    ...
           out dx,al                           ; turn off motor
;          shr al,4                            ; @p2067
           and  al,NOT DORmask                 ;                       @p2067
           ror  al,4                           ;                       @p2067
           SaveReg es                          ;   ...
           mov dx,RomData                      ; update ROM data
           mov es,dx                           ; es -> ROM data area
           assume es:RomData                   ;   ...
           mov Motor_Status,al                 ; tell ROM motor changed
           mov Motor_Count,0
           RestoreReg es                       ;   ...
           assume es:nothing                   ;   ...
        .ENDIF                                 ;   ...
  ret                                          ;   ...
TMotorOff endp

;** FTimeout    - Time out occurred
;
;   FTimeout is called by timer services indicating the an event did
;   not occur within the specified time period.  (This will generate a
;   "Drive not ready error".
;
;   ENTRY CS and DS set by timer services
;
;   EXIT  DriveStruc updated
;
;   USES  flags

        public FTimeout
FTimeout  proc

        dec     TimerCallFlag               ; occurred before interrupt!!!
        SaveReg <di,cx>                     ; active drive had a time out so
        mov  Drive.State,TIMOUT             ;   it's an error
        call DriveExecute                   ;   call state machine
        RestoreReg <cx,di>                  ;   ...
        RET
FTimeout  endp


;** Set_Timeout_Timer   - Time out occurred
;
;   Set_Timeout_Timer is called
;
;   ENTRY       AX = value in seconds to set timer
;
;   EXIT
;
;   USES  flags
Set_Timeout_Timer       proc    near
        DISABLE                      ; This is to check to see if the Interrupt
        inc     TimerCallFlag        ; occurred before setting the timer!!!
        cmp     TimerCallFlag, 0     ; If so then we don't need to call timer
                                     ; services to turn off the timer!
        jle     no_set_timer         ; jmp if the destination is less then the source (signed)
        call    Set_FTimerS
        jc      no_set_timer         ; IF Carry Flag is set THEN
internalerror:  jmp     short   internalerror
no_set_timer:
        ENABLE

        ret
Set_Timeout_Timer       endp


        BREAK <Stubs>
;** Do_ResetUM
;
;   Do_ResetUM is a NOP in the AT device driver.

        public Do_ResetUM
Do_ResetUM proc

  ret

Do_ResetUM endp

Code    ends

        end
