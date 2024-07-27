
        TITLE   sndcint2 - C Interface for Music Device Driver, part 2
?PLM = 1
?WIN = 1

XDDEF = 0

.xlist
include cmacros.inc
.list

sBegin DATA
;-----------------------------------------------------------------------
;
;                               Data
;
;-----------------------------------------------------------------------
externW  <full_dur>     ;full duration for notes
externB  <nmvoices>     ;number of music voices for this machine
externB  <nnvoices>     ;number of noise voices for this machine
externB  <ntvoices>     ;total number of voices for this machine
externW  <pid>          ;process id of task accessing Music Device Driver
                        ; 0 indicates that this device is not in use
                        ; non 0 indicates device open
externW  tew_ptr        ;Threshold event word pointer

sEnd   DATA

sBegin CODE

assumes CS,CODE
assumes DS,DATA
assumes ES,DATA
assumes SS,DATA

externNP    <mdd_play>
externNP    <checkpid>
externFP    <GetThresholdStatus>
externFP    <GetThresholdEvent>
externFP    <Yield>

; /* error codes */

erdvna  EQU     -1      ;/* device not available */
erofm   EQU     -2      ;/* out of memory */
ermact  EQU     -3      ;/* music active */
erqful  EQU     -4      ;/* queue full */
erbdnt  EQU     -5      ;/* invalid note */
erbdln  EQU     -6      ;/* invalid note length */
erbdcc  EQU     -7      ;/* invalid note count */
erbdtp  EQU     -8      ;/* invalid tempo */
erbdvl  EQU     -9      ;/* invalid volume */
erbdmd  EQU     -10     ;/* invalid mode */
erbdsh  EQU     -11     ;/* invalid shape */
erbdpt  EQU     -12     ;/* invalid pitch */
erbdfq  EQU     -13     ;/* invalid frequency */
erbddr  EQU     -14     ;/* invalid duration */
erbdsr  EQU     -15     ;/* invalid source */
erbdst  EQU     -16     ;/* invalid state */

f_init      = 1         ;- initialize and return voices
f_q_size    = 2         ;- initialize queues with queue size
f_q_note    = 3         ;- queue a note or rest
f_volume    = 4         ;- queue volume change
f_envelope  = 5         ;- queue envelope change
f_start     = 6         ;- start music
f_stop      = 7         ;- stop music
f_sync      = 8         ;- synchronize voices
f_term      = 9         ;- terminate
f_ev_ptr    = 10        ;- get event flag pointer
f_flags     = 11        ;- get event flag and clear it
f_trap      = 12        ;- set trap level
f_noise     = 13        ;- queue noise
f_num_notes = 14        ;- get number of notes in queue

; /* constants used to specify return condition for pwait */

queueempty      EQU     0
threshold       EQU     1
allthreshold    EQU     2

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           field_errors - check for errors after call to OEM
;
;       Purpose:        This routine checks for errors after calls
;                       to the OEM dependent routine mdd_play. This routine
;                       translates OEM interface errors into user
;                       interface errors
;
;       Entry:          [AX] = error code passed by mdd_play
;                       [AX] = err_qfull  = 1 - No room in the queue for this request
;                       [AX] = err_freq   = 2 - Unsupported frequency
;                       [AX] = err_vol    = 3 - Unsupported volume index
;                       [AX] = err_eshape = 4 - Unsupported envelope shape
;                       [AX] = err_dur    = 5 - Unsupported envelope duration
;                       [AX] = err_noise  = 6 - Unsupported noise source
;
;       Exit:           [AX] = error code passed to user
;                       [AX] = erqful - queue full
;                       [AX] = erbdfq - invalid frequency
;                       [AX] = erbdvl - invalid volume
;                       [AX] = erbdsh - invalid shape
;                       [AX] = erbddr - invalid duration
;                       [AX] = erbdsr - invalid source
;
;       Modified:       none
;
;-----------------------------------------------------------------------

        PUBLIC  field_errors

field_errors:

        XOR     AH,AH                   ;strip off any junk
        DEC     AX                      ;0 based error
        SHL     AX,1                    ;word pointer
        ADD     AX,OFFSET error_table   ;into error table
        PUSH    SI
        MOV     SI,AX
        MOV     AX,CS:[SI]              ;get the translated error message
        POP     SI
        RET

error_table:
        DW      erqful                  ;err_qfull  = erqful
        DW      erbdfq                  ;err_freq   = erbdfq
        DW      erbdvl                  ;err_vol    = erbdvl
        DW      erbdsh                  ;err_eshape = erbdsh
        DW      erbddr                  ;err_dur    = erbddr
        DW      erbdsr                  ;err_noise  = erbdsr

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           pwait - wait on specified state
;
;       Call:           pwait(state)
;                       int state;
;
;       Purpose:        Pwait will not return until the driver enters
;                       the specified state.
;                       State can be one of the following:
;                        * Queue empty:   All voice queues are empty
;                                         and sound drivers turned off.
;                        * Threshold:     A voice queue has dropped below
;                                         threshold, returns voice.
;                        * All Threshold: All voices have reached threshold
;
;       Entry:          state on stack - one of the following:
;                               0 = queueempty
;                               1 = threshold
;                               2 = allthreshold
;
;       Exit:           AX = erbdst - Invalid state
;
;       Modified:       BX,CX,DX
;
;-----------------------------------------------------------------------

cProc   WaitSoundState,<PUBLIC,FAR>,<si,di>
        parmW   state
;        localV  msg,16
cBegin
        call    checkpid
        jnz     pw_0                    ; pid not current one, quit
        mov     ax,state
        CMP     AX,queueempty
        JE      pw_queue_empty          ;wait until all queues are empty
        CMP     AX,threshold
        JE      pw_threshold            ;wait until one of the voice queues
                                        ; drops below the threshold
        cmp     ax,allthreshold
        je      pw_all_threshold        ; can only be 0, 1, or 2

        jmp     bad_state               ; third case

pw_queue_empty:
        ;
        ; wait until all queues are empty
        ;
        MOV     AH,ntvoices
next_q_empty:
        DEC     AH                      ;0 based voice
        MOV     AL,f_num_notes          ;function to get number of notes
empty_yet:
        PUSH    AX
        cCall   Yield
        POP     AX
        PUSH    AX
        CALL    mdd_play                ;get number of notes
                                        ; (this function never returns an error)
        POP     AX
        OR      BX,BX                   ;are there any notes in this queue?
        JNZ     empty_yet               ;nope - still notes in the queue
        OR      AH,AH                   ;any more voices to check?
        JNZ     next_q_empty            ;brif yes
pw_0:
        JMP     SHORT pw_done           ;we're done waiting

pw_threshold:
        ;
        ; wait until one of the voice queues drops below the threshold
        ;
        ; Poll all voice queues until at least one voice is below the
        ; threshold. (Indicated by a set bit in flags word.)
        ;
        cCall   Yield
        cCall   GetThresholdStatus      ;
        OR      ax,ax                   ;test if any voice below threshold
        JZ      pw_threshold            ;none yet
        JMP     SHORT pw_done           ;we're done waiting

pw_all_threshold:
        ;
        ; wait until all the voice queues drop below the threshold
        ;
        ; produce a mask with ntvoices set bits as follows:
        ;
        ;       0000.....1111
        ;
        MOV     CL,ntvoices             ;total number of voices
        XOR     AX,AX
masking:
        INC     AX
        DEC     CL
        JZ      mask_done
        SHL     AX,1
        JMP     SHORT masking
mask_done:
        ;
        ; poll the threshold flags
        ;
        PUSH    AX                      ;mask
        cCall   GetThresholdEvent,<ax>
        POP     CX                      ;mask
        PUSH    BP
        MOV     BP,AX
        ;
        ; [BP] = pointer to queue status flags
        ; [CX] = mask 0000....1111
        ;
check_again:
        push    cx
        cCall   Yield
        pop     cx
        MOV     BX,[BP]                 ;get status of all flags
        CMP     BX,CX                   ;are all of them set ???
        JNE     check_again
        POP     BP
        cCall   GetThresholdStatus      ; gets and clears the flags
pw_done:
        XOR     AX,AX                   ;return no errors
pw_exit:
cEnd

        ;
        ; Error Handler
        ;
bad_state:
        MOV     AX,erbdst               ;Invalid state error
        JMP     SHORT pw_exit

sEnd    CODE

END
