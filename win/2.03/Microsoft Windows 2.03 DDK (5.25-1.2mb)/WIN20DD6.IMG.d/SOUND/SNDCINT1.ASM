
        TITLE   MDD_CINT - C Interface for Multivoice Music Device Driver
?PLM = 1
?WIN = 1

.xlist
include cmacros.inc
.list

;-----------------------------------------------------------------------
;
;       Microsoft Multivoice Music Device Driver
;
;       C User Interface to OEM Machine Interface Module
;
;       (C) 1985 Microsoft Inc. - Last Revision March 5, 1985
;
;-----------------------------------------------------------------------
;
;       This module provides the OEM independent portion of code for
;       the Microsoft Multivoice Music and Sound Device Driver.
;       This device driver is intended for multi-tasking environments
;       such as Microsoft Windows etc.
;
;       This module interfaces the C user with the OEM dependent
;       portion of the Multivoice Music and Sound Device Driver.
;       OEMs must link machine specific code with this module to
;       create a device driver (or library) accessible to C programs.
;
;-----------------------------------------------------------------------
;
;       Code Model:   +-----------------------------------------------+
;                     |                                               |
;                     |  MDD_TEST: An Application                     |
;                     |                                               |
;                     +-----------------------------------------------+
;                     |                                               |
;                     |  MDD_CINT: C User Interface to                |
;                     |            OEM Machine Interface              |
;                     |                                               |
;                     +--------------------------+                    |
;                     |                          |                    |
;                     |  MDD_IBM:                |                    |
;                     |   IBM PC Specific Code   |                    |
;                     |                          |                    |
;                     +-------------+------------+--------+-----------+
;                     |             |                     |           |
;                     |  Hardware   |       MS-DOS        |  MDD_WIN  |
;                     |             |                     |           |
;                     +-------------+---------------------+-----------+
;
;-----------------------------------------------------------------------

        PAGE

;-----------------------------------------------------------------------
;
;                               Structures
;
;-----------------------------------------------------------------------

q_loc_block        STRUC
        q_segment       DW      0
        q_size          DW      0
        q_tempo         DB      0
        q_mode_nls      DB      0
        q_pitch         DB      0
q_loc_block        ENDS

        PAGE

sBegin DATA
;-----------------------------------------------------------------------
;
;                               Data
;
;-----------------------------------------------------------------------
PUBLIC  mdd_dstart
mdd_dstart      LABEL   WORD    ;marks start of music device driver data

globalW  full_dur,0     ;full duration for notes
globalB  nmvoices,0     ;number of music voices for this machine
globalB  nnvoices,0     ;number of noise voices for this machine
globalB  ntvoices,0     ;total number of voices for this machine
globalW  tew_ptr,0      ;Threshold event word pointer
globalW  pid,0          ;process id of task accessing Music Device Driver
                        ; 0 indicates that this device is not in use
                        ; non 0 indicates device open
PUBLIC  qlocinfo
qlocinfo q_loc_block <> ; queue 1
         q_loc_block <> ; queue 2
         q_loc_block <> ; queue 3
         q_loc_block <> ; queue 4
         q_loc_block <> ; queue 5
         q_loc_block <> ; queue 6
         q_loc_block <> ; queue 7
         q_loc_block <> ; queue 8
         q_loc_block <> ; queue 9
         q_loc_block <> ; queue 10
         q_loc_block <> ; queue 11
         q_loc_block <> ; queue 12
         q_loc_block <> ; queue 13
         q_loc_block <> ; queue 14
         q_loc_block <> ; queue 15
         q_loc_block <> ; queue 16


sEnd   DATA

sBegin CODE

assumes CS,CODE
assumes DS,DATA

externFP    <WaitSoundState>
externFP    <StartSound>
externFP    <CountVoiceNotes>
externFP    <SyncAllVoices>
externFP    <SetVoiceThreshold>
externFP    <StopSound>
externNP    <field_errors>
externFP    <GetCurrentTask>
externFP    <GlobalAlloc>
externFP    <GlobalRealloc>
externFP    <GlobalFree>

        PUBLIC  mdd_cstart
mdd_cstart:             ;marks beginning of music device driver code segment

        PAGE

;-----------------------------------------------------------------------
;
;                               Constants
;
;-----------------------------------------------------------------------

disable EQU     CLI
enable  EQU     STI

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

; /* constants used for specifing source in pqnoise N = clock frequency */

period512       EQU     0  ;/* source freq = N/512 high pitch, less coarse hiss */
period1024      EQU     1  ;/* source freq = N/1024 */
period2048      EQU     2  ;/* source freq = N/2048 low pitch, more coarse hiss */
periodvoice     EQU     3  ;/* source is frequency from voice channel (3) */
white512        EQU     4  ;/* source freq = N/512 high pitch, less coarse hiss */
white1024       EQU     5  ;/* source freq = N/1024 */
white2048       EQU     6  ;/* source freq = N/2048 low pitch, more coarse hiss */
whitevoice      EQU     7  ;/* source is frequency from voice channel (3) */

; /* constants used to specify return condition for pwait */

queueempty      EQU     0
threshold       EQU     1
allthreshold    EQU     2

; /* constants used to specify accent mode */

normal          EQU     0
leggato         EQU     1
staccato        EQU     2

;
; Other constants
;

qlocsize = SIZE q_loc_block

        PAGE

;-----------------------------------------------------------------------
;
;                               Macros
;
;-----------------------------------------------------------------------

callos  MACRO   func
        MOV     AH,LOW OFFSET func
        CLD                             ;fixes a pre DOS 3.0 bug
        INT     21H
        ENDM

        EXTRN   mdd_play:NEAR

        PAGE

PUBLIC  checkpid
checkpid:
        cCall   GetCurrentTask          ;get process id in [AX]
        CMP     AX,pid                  ;is it the current process?
        ret

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           pclose - close access to the play device
;
;       Call:           pclose()
;
;       Purpose:        Pclose will close access to device and allow
;                       another process to play music. All voice queues
;                       are flushed. Buffers allocated for voice queues
;                       will be freed. By definition, all music will
;                       stop when the when pclose is called, and hence
;                       pstop need not be called.
;
;       Entry:          none
;
;       Exit:           pid = 0 (indicates no application process)
;
;       Modified:       AX,BX,CX,DX
;
;-----------------------------------------------------------------------

cProc   CloseSound,<PUBLIC,FAR>,<si,di>
cBegin
        call    checkpid
        jnz     pclose_end              ; pid is not current one
        cCall   StopSound
        MOV     AL,f_term               ;function - terminate music
        CALL    mdd_play                ;OEM termination
                                        ; (this function never returns an error)
        XOR     AX,AX
        MOV     pid,AX                  ;free up the device for others to use
        ;
        ; give back all the music/noise queue memory to OS
        ;
        MOV     BX,OFFSET qlocinfo      ;first index into queue location control block
        MOV     CL,ntvoices             ;total number of voices
        XOR     CH,CH
q_toss:
        MOV     SI,BX
        MOV     AX,[SI].q_size          ;get the size of the memory
        OR      AX,AX                   ;has any memory been assigned?
        JZ      no_memory               ;brif no memory assigned
        MOV     AX,[SI].q_segment       ;segment to be given up
        push    bx
        push    cx
        cCall   GlobalFree,<ax>         ;memory deallocation
        pop     cx
        pop     bx
        XOR     AX,AX                   ;0 memory
        MOV     [SI].q_size,AX          ;size of queue is now 0 bytes
no_memory:
        ADD     BX,qlocsize             ;ptr to next block
        LOOP    q_toss                  ;toss the next block of memory away
pclose_end:
cEnd    pclose

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           popen - open access to the play device
;
;       Call:           popen()
;
;       Purpose:        Popen will open access to the music device.
;                       While the play device is opened it can not be
;                       opened by another process.
;
;                       Popen will return the number of music voices
;                       available to the user application.
;
;                       Note:   The number of noise voices available is
;                               assumed to be 0 or 1. There cannot be
;                               more than 1 noise queue with this
;                               existing interface. The user application
;                               must have some apriori knowledge of the
;                               existance of a noise channel.
;
;                       Note:   Many of the following routines are passed
;                               voice handles for the various music and
;                               noise queues. The noise channel is assumed
;                               to have the handle 1 + number of music voices.
;
;                       Note:   This interface specification will not
;                               allow more than 16 voices (music and noise
;                               combined).
;
;                       Error codes are returned as negative numbers.
;
;                       Popen will provide the application with a default
;                       music queue size of 32 notes per voice, where each
;                       note is assumed to take 7 bytes.
;
;                       The noise queue has a default size of 32 noises
;                       where each noise note is assummed to take 5 bytes.
;
;       Entry:          none
;
;       Exit:           [AX] = erdvna - device not available
;                       [AX] = erofm - out of memory
;
;                        else
;
;                       [AX] = number of music voices available on this machine
;                       pid = application process id
;                       nmvoices = number of music voices available to application
;                       nnvoices = number of noise voices available to application
;                       ntvoices = total number of voices available to application
;
;       Modified:       BX,CX,DX
;
;-----------------------------------------------------------------------

cProc   MyOpenSound,<PUBLIC,FAR>
cBegin
        mov     ax,pid
        OR      ax,ax                   ;is there another process currently
        JZ      available               ; accessing the music device driver?
        JMP     devinuse                ;brif device in use
available:
        cCall   GetCurrentTask          ;get process id
        OR      AX,AX                   ;is it NULL - if so then os failure
        JNZ     not_devinuse
        JMP     devinuse                ;brif device in use
not_devinuse:
        MOV     pid,AX                  ; and store for future reference
        ;
        ; Get the total number of voices available from the OEM
        ;
        MOV     AL,f_init               ;OEM initialization
        CALL    mdd_play                ;returns OEM number of music/noise voices
        ;
        ; PSW.C set indicates device cannot be opened
        ;
        ;  else
        ;
        ; [AL] = number of music voices (Max 16)
        ; [AH] = number of noise voices (Max 1)
        ;
        JNC     dev_ok
        JMP     devinuse                ; brif device failure (fake device in use)
dev_ok:
        MOV     nmvoices,AL             ;number of music voices available to user
        MOV     ntvoices,AL             ;total number of voices available to user
        MOV     nnvoices,AH             ;number of noise voices available to user
        OR      AH,AH
        JZ      no_noise
        XOR     AH,AH
        INC     AL                      ;max one noise channel
        MOV     ntvoices,AL             ;determine the total number of voices
        DEC     AL                      ;number of music voices
no_noise:
        PUSH    AX                      ;save number of music voices
        ;
        ; Now set up some defaults for each voice:
        ;
        ;       Mode                    NORMAL
        ;       Tempo                   120 quarter notes
        ;       Music Queue Size        32 notes per voice
        ;       Noise Queue Size        32 noise notes if noise available
        ;
        ; [AX] = number of music queues
        ;
        MOV     BX,OFFSET qlocinfo      ;first index into queue location control block
        MOV     CX,AX                   ;counter for number of music queues
queue_224:
        MOV     SI,BX                   ;index into queue location control table
        ;
        ; Get memory from OS for 32 notes per music queue
        ;
        push    cx
        xor     dx,dx
        MOV     AX,224D                 ;enough room for 32 notes (32*7)
        regptr  DXAX,dx,ax
        cCall   GlobalAlloc,<dx,dxax>   ; global fixed object
        pop     cx
        or      ax,ax
        jnz     popen1
        jmp     internal_error
popen1:
        MOV     [SI].q_segment,AX       ;segment location for this queue
        MOV     AX,0224D                ;size of this queue
        MOV     [SI].q_size,AX
        MOV     AL,0120D                ;default tempo for this queue
        MOV     [SI].q_tempo,AL
        MOV     AL,normal               ;normal mode for this queue
        MOV     [SI].q_mode_nls,AL
        mov     bx,si
        ADD     BX,qlocsize             ;next queue index
        LOOP    queue_224               ;do again for the other music queues
        MOV     SI,BX                   ;index into next queue location control table
        ;
        ; Do the same sort of initialization for a noise queue if it exists
        ;
        ;       default size is 32*5
        ;
        XOR     DH,DH
        MOV     DL,ntvoices             ;total number of queues
        POP     AX                      ;number of music queues
        PUSH    AX
        CMP     DX,AX                   ;is there a noise voice?
        JZ      oem_iniq                ;if not, continue with oem initialization
        ;
        ; Get memory from OS for 32 notes per noise queue
        ;
        push    bx
        push    dx
        xor     dx,dx
        MOV     AX,160D                 ;enough room for 32 notes (32*5)
        regptr  DXAX,dx,ax
        cCall   GlobalAlloc,<dx,dxax>   ;fixed global memory allocation
        pop     dx
        pop     bx
        or      ax,ax
        jz      internal_error
        MOV     [SI].q_segment,AX       ;segment location for this queue
        MOV     AX,0160D                ;size of this queue
        MOV     [SI].q_size,AX
        JMP     SHORT loop_iniq
oem_iniq:
        SUB     BX,qlocsize             ;last legitimate queue index
loop_iniq:
        ;
        ; Initialize the individual queues by passing the location of these
        ; queues to the OEM. The OEM is responsible for initializing his
        ; queue control blocks.
        ;
        ; Initialization starts with the last valid queue and continues to
        ; queue number 1.
        ;
        ; [DX] = total number of queues (counter)
        ; [SI] = [BX] = pointer to start of queue location block
        ;
        MOV     SI,BX                   ;current queue location block index
        PUSH    BX
        ;
        ; mdd_play function f_q_size assumes the following:
        ;
        ;  [AH] = voice (0 based)
        ;  [BX] = number of bytes in queue memory for this voice
        ;  [CX:0000] points to the start of music queue memory
        ;
        MOV     AX,DX                   ;current voice (music or noise)
        PUSH    AX                      ;voice
        MOV     AH,AL                   ;voice (1 based)
        DEC     AH                      ;voice (0 based)
        MOV     AL,f_q_size             ;report queue size to OEM
        MOV     BX,[SI].q_size          ;segment size for queue
        MOV     CX,[SI].q_segment       ;segment location for this queue
        CALL    mdd_play                ;OEM queue initialization
                                        ; this function never returns an error
        POP     AX                      ;voice
        ;
        ; psetthreshold requires voice and cnote on the stack
        ;
        PUSH    DX                      ;voice counter
        PUSH    AX                      ;voice on stack
        XOR     CX,CX                   ;0 threshold
        cCall   SetVoiceThreshold,<ax,cx>
        POP     AX                      ;voice
        POP     DX                      ;voice counter
        ;
        ; also set up default volume
        ;
        PUSH    AX
        DEC     AX                      ;voice now 0 based
        MOV     AH,AL
        MOV     BX,0FFFFH               ;default volume
        MOV     AL,f_volume             ;change volume function
        CALL    mdd_play                ;default volume is max
                                        ;there should be no errors
        POP     AX
        ;
        ; do it again for the next voice
        ;
        POP     BX                      ;ptr to queue control block
        SUB     BX,qlocsize             ;last legitimate queue index
        DEC     DX
        JNZ     loop_iniq               ;do again for the other voices
        ;
        ; Queue a sync byte to each music queue
        ;
        cCall   SyncAllVoices           ; synchronize all voices
        ;
        ; get the location of the threshold event word and store it
        ;
        MOV     AL,f_ev_ptr             ;get trap flags pointer
        CALL    mdd_play                ;Get the event word location from
                                        ; the OEM (return in [AX])
                                        ; (this function never returns an error)
        MOV     tew_ptr,AX              ;and save for future reference
        ;
        POP     AX                      ;number of voices available to the user
        ;
        ; initialization complete
        ;
opendone:
cEnd    popen

        ;
        ; Error handlers:
        ;

internal_error:                         ;os error
        cCall   CloseSound              ; free all memory assigned so far
        MOV     AX,erofm                ;out of memory error
        JMP     SHORT opendone

devinuse:
        MOV     AX,erdvna               ;device unavailable (in use)
        JMP     SHORT opendone

        PAGE
;-----------------------------------------------------------------------
;
;       Name:           psetqsize - set queue size
;
;       Call:           psetqsize(voice, cb)
;
;       Purpose:        Psetqsize will allocate cb bytes for the
;                       specified voice queue. The default queue size
;                       at popen time is 32 notes per voice with 7 bytes
;                       per note. For a single voice system this is 224 bytes.
;                       In addition to the memory required for the music
;                       is the memory required for the music queue control
;                       blocks which is 14 bytes per voice plus 14 bytes
;                       for the Noise channel. The amount of queue space
;                       consumed varies. All queues are locked in memory.
;                       Allocation of queues can not be done while music is
;                       playing. Psetqsize will return an error in this
;                       case.
;
;                       There is one queue for each voice including one
;                       queue for each noise channel. Each of these queues
;                       may be a different size and may be changed in size
;                       independently of each other. Additionally, each
;                       queue is in a separate segment.
;
;                       Note:   This routine operates with both the music
;                               and the noise queues.
;
;                       Note:   The user may alter the queue size of any queue
;                               which has no notes.
;
;       Entry:          voice, cb on stack
;
;       Exit:           AX = erotm - out of memory
;                          = ermact - music active
;
;       Modified:       BX,CX,DX
;
;-----------------------------------------------------------------------

cProc   SetVoiceQueueSize,<PUBLIC,FAR>,<si,di>
        parmW   voice
        parmW   cb
cBegin
        call    checkpid
        jnz     qset_done
        mov     ax,voice
        cmp     al,ntvoices
        ja      qset_done

        ;
        ; test if music currently playing and report error accordingly
        ;
        cCall   CountVoiceNotes,<voice>
        OR      AX,AX                   ;are there any notes?
        JNZ     queue_not_empty         ;brif queue not empty
        mov     ax,voice
        DEC     AL                      ;0 based voice number
        mov     voice,ax
        ;
        ; deallocate the memory that is currently assigned to this voice
        ;
        MOV     BX,OFFSET qlocinfo      ;first index into queue location control block
        MOV     CL,qlocsize
        MUL     CL                      ;get index to queue location table
        ADD     AX,BX
        MOV     SI,AX
        MOV     AX,[SI].q_segment       ;current segment for this voice queue
        cCall   GlobalFree,<ax>
        ;
        ;
        ; save in queue location control table
        ;
        mov     ax,cb
        MOV     [SI].q_size,AX          ;queue size for this voice
        OR      AX,AX                   ;did the user request 0 bytes?
        JZ      cb_bytes                ;if it's 0, then don't allocate os memory
        ;
        ; allocate the memory from the OS
        ;
        xor     dx,dx
        regptr  DXAX,dx,ax
        cCall   GlobalAlloc,<dx,dxax>   ; fixed global
        or      ax,ax
        jz      mem_failure             ;os memory failure
        MOV     [SI].q_segment,AX       ;queue segment for this voice
cb_bytes:
        ;
        ; report the new segment for the music/noise queue to the OEM
        ;
        ; mdd_play function f_q_size assumes the following:
        ;
        ;  [AH] = voice (0 based)
        ;  [BX] = number of bytes in queue memory for this voice
        ;  [CX:0000] points to the start of music queue memory
        ;
        MOV     AX,voice                ;current voice (music or noise)
        MOV     AH,AL                   ;voice (0 based)
        MOV     AL,f_q_size             ;function - report queue size to OEM
        MOV     BX,[SI].q_size          ;segment size for queue (may be 0)
        MOV     CX,[SI].q_segment       ;segment location for this queue
        CALL    mdd_play                ;OEM queue initialization
                                        ; (this function never returns an error)
qset_done:
        XOR     AX,AX                   ;return no errors
qset_exit:
cEnd

        ;
        ; Error Handlers:
        ;
mem_failure:
        XOR     AX,AX                   ;no bytes allowed - OS problem
        MOV     [SI].q_size,AX          ;queue size for this voice
        MOV     AX,erofm                ;out of memory error
        JMP     SHORT qset_exit
queue_not_empty:
        MOV     AX,ermact               ;music active error
        JMP     SHORT qset_exit

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           pqnote - place note in specified voice queue
;
;       Call:           pqnote(voice, value, len, cdots)
;                       int   voice;
;                       int   value;
;                       int   len;
;                       int   cdots;
;
;                       struct note far *pnote;
;
;       Purpose:        Pqnote will queue the note information in the
;                       specified voice queue. If not enough room is
;                       in the queue an error is returned. If voice is
;                       out of range then the call to pqnote is ignored.
;                       Space for a single sync mark is always left
;                       in the queue.
;
;                       Note:   This routine will only operate on music
;                               queues. Noise queues are ignored.
;
;                       Note:   Note values of 0 are interpreted as pauses.
;
;                       Note:   Note values are adjusted by the pitch wheel,
;                               which is set by pqaccent.
;
;       Entry:          voice on stack
;                       note on stack as follows: value (0 to 84)
;                                                 length (1 to 64)
;                                                 cdots (0 to ?)
;
;       Exit:           AX = erqful - Queue full
;                          = erbdnt - Invalid note
;                          = erbdln - Invalid note length
;                          = erbdcc - Invalid cdot note count
;
;       Modified:       BX,CX,DX
;
;-----------------------------------------------------------------------

cProc   SetVoiceNote,<PUBLIC,FAR>,<si,di>
        parmW   voice
        parmW   value
        parmW   len
        parmW   cdots
cBegin
        call    checkpid
        jnz     qn_0
        mov     ax,voice
        cmp     al,ntvoices
        jbe     qn_1
qn_0:
        jmp     qn_2                    ; return no error
qn_1:
        DEC     AL                      ;0 based voice
        ;
        ; Get index into queue location / control table
        ;
        MOV     BX,OFFSET qlocinfo      ;table with segment/size/volume/tempo/mode
        MOV     CL,qlocsize
        MUL     CL                      ;AX was 0 based voice number
        ADD     AX,BX                   ;index into this table
        MOV     SI,AX
        ;
        ; Pull the note information from the stack
        ;
        mov     bx,len
        mov     ax,value
calc_dur:
        ; Calculate Duration:
        ;
        ;       [AX] = note value (which may be altered by the pitch wheel)
        ;       [SI] = index to queue location/control info for this voice
        ;       [BX] = 1/note_length
        ;
        ; Duration Parameters:
        ;
        ;       Note Length:    1 to 1/64
        ;
        ;       Tempo:          Number of Quarter Notes per Minute
        ;
        ;       Mode:           leggato  1/1 time
        ;                       normal   7/8 time
        ;                       staccato 3/4 time
        ;
        ;       Cdot            length = length * (2-(1/2)^cdots)
        ;
        ; Duration is calculated as follows:
        ;
        ;       4/tempo = minutes/note
        ;
        ;       (60*4*mode*note_length*(2-(1/2)^cdot)) / tempo = seconds/note
        ;
        ;       (60*4*mode*note_length*(2-(1/2)^cdot)) / (tempo*.0025)
        ;
        ;               = # of 2.5 milliseconds / note
        ;
        ; Simplifying:
        ;
        ;       (96000*mode*(2-(1/2)^cdot)) / ((1/note_length)*tempo) = duration
        ;
        PUSH    AX                      ;Save note value
        MOV     DL,BL                   ;1/note length = 1/(1..64)
        MOV     AL,[SI].q_tempo         ;get tempo (beats per unit time)
        MUL     DL                      ;[AX] = (1/note_length) * tempo
        MOV     CX,AX                   ;[CX] = (1/note_length) * tempo
        MOV     DX,1                    ;96000 (4*60/.0025) = 17700 Hex
        MOV     AX,7700H                ;(special time constant)
        DIV     CX                      ;[AX] = [DX:AX]/[CX] =
                                        ; (96000) / ((1/note_length)*tempo)
        OR      AX,AX                   ;If duration is zero, get out.
        mov     bx,cdots                ;[BX] = number of dots (cdot)
        JNZ     qn_cont2
        JMP     zero_dur                ;zero duration (don't queue it)
qn_cont2:
        ;
        ; Calculate the effects of dots next
        ;
        OR      BX,BX                   ;check for any dots
        JZ      no_dots                 ; brif no dots at all
                                        ;[AX] = duration thus far
                                        ;[BX] = number of dots (cdot)
        MOV     CX,AX
        ;
        ; repeat the following for each dot
        ;
cdts:   SHR     CX,1                    ;length * (1/2)
        DEC     BX
        JNZ     cdts                    ;repeat for each dot
        MOV     BX,AX
        SUB     BX,CX                   ;(length) - (length * (1/2))^cdots
        ADD     AX,BX                   ;(length * 2) - (length * (1/2))^cdots
        JNC     no_dots
        JMP     bad_nl                  ;Invalid Note Length if overflow

no_dots:
        ;
        ; Test if note value is 0. If so, we have a complete pause
        ; rather than a playable note.
        ;
        POP     CX                      ;refresh note value
        ;
        ; IF note value 0 then this is a complete rest note from the user.
        ;
        ; Therefore, we want to queue the rest with full note duration.
        ;
        ; [AX] = duration = (96000*(2-(1/2)^cdot)) / ((1/note_length)*tempo)
        ;
        ; Effects of Staccato / Legatto or Normal not calculated yet.
        ; (And they don't have to be !!!)
        ;
        OR      CX,CX                   ;if the note value is 0 we have a rest
        JNZ     qn_note
        JMP     qn_rest                 ;queue rest only
qn_note:
        PUSH    CX
        ;
        ; If note value non zero, we continue calculating real note duration.
        ;
        ; [AX] = duration = (96000*(2-(1/2)^cdot)) / ((1/note_length)*tempo)
        ;
        MOV     full_dur,AX             ;save full duration
        ;
        ; Now calculate the effects of mode (leggato/normal/staccato)
        ;
        MOV     CL,[SI].q_mode_nls      ;Using scale for shift count
        CMP     CL,LOW leggato          ;
        JZ      note_val                ;Brif Legatto (1/1 time)
        MOV     BX,3                    ;Staccato multiplier (3/4)
        CMP     CL,LOW staccato         ; (2)
        JZ      mode_calc               ;Brif Staccato
        MOV     CL,BL                   ; else Normal (7/8)
        MOV     BL,7                    ;
mode_calc:
        MUL     BL                      ;Duration *7 or *3
        SHR     AX,CL                   ;Duration /8 or /4
        OR      AX,AX                   ;Note duration of 0 shows roundoff
        JNZ     note_val                ;
        INC     AX                      ;If zero then make 1

note_val:
        MOV     DX,AX                   ;Duration in DX
        ;
        ;       [DX]=(96000*mode*(2-(1/2)^cdot)) / ((1/note_length)*tempo)
        ;
        ; Check out the note value
        ;
        POP     AX                      ;note value
        CMP     AX,84                   ;Must be 84 or less
        JBE     qn_cont4
        JMP     bad_val                 ;brif error
qn_cont4:
        ;
        ; adjust note value with default pitch
        ;
        ADD     AL,[SI].q_pitch         ;get the current pitch
        CMP     AL,84D                  ;check for wrap around
        JBE     qn_cont5
        SUB     AL,84D                  ;perform modulus 84 arithmetic
qn_cont5:
        ;
        ; 7 octaves each with 12 notes:  C C# D D# E F F# G G# A A# B
        ;
        DEC     AX                      ;we want note 1..12 not 0..11
        MOV     BL,12                   ;12 notes per octave
        DIV     BL                      ; determine octave and note
        MOV     BX,AX                   ;[BH:BL] = note:octave
        INC     BH                      ;we want note 1..12 not 0..11
        PUSH    BX
        MOV     BL,BH
        XOR     BH,BH                   ;note now in BL
        SHL     BL,1                    ; double it (we're after a word)
        ADD     BX,OFFSET note_tbl-2    ;address of note frequency table
        MOV     AX,CS:[BX]              ;get freq. of note BL for high octave
        POP     BX                      ;[BH:BL] = note:octave
        MOV     CL,6                    ;highest octave
        SUB     CL,BL                   ;6-octave
        SHR     AX,CL                   ;divide by 2 for each octave from 6
        ADC     AX,0                    ;add carry appropriately to round up
        ;
        ; [AX] = frequency in Hz
        ; [DX] = duration in 2.5 millisecond intervals
        ;
        ; Now queue the note plus a pause if Normal or Staccato
        ;
        MOV     BX,AX                   ;[BX] = frequency in Hz
        MOV     CX,DX                   ;note duration
        MOV     DX,full_dur             ;full duration of note including interstice
        SUB     DX,CX                   ;interstice duration
queue_it:
        mov     ax,voice
        mov     ah,al                   ;[AH] = voice
        DEC     AH                      ;voice must be 0 based for mdd_play
        MOV     AL,f_q_note             ;function to queue a note
        CALL    mdd_play                ;queue the note
        JC      note_errors             ;return error condition to user
qn_2:
        XOR     AX,AX                   ;No error condition
qn_exit:
cEnd    pqnote

qn_rest:
        ;
        ; [AX] = duration
        ;
        MOV     DX,AX                   ;this is now the interstice duration
                                        ;with [CX] = 0 note duration
        XOR     BX,BX                   ;and [BX] = 0 frequency
        JMP     SHORT queue_it

;
; Handle all error conditions:
;

note_errors:
        CALL    field_errors            ;return appropriate error code
        JMP     SHORT qn_exit

bad_nl:
        POP     AX                      ;pop bogus note value
bad_nl2:
        MOV     AX,erbdln               ;Invalid Note Length
        JMP     SHORT qn_exit

bad_val:
        MOV     AX,erbdnt               ;Invalid Note
        JMP     SHORT qn_exit

zero_dur:
        POP     AX                      ;pop bogus note value
        JMP     SHORT qn_exit

;
; table of note frequencies:
; these are the frequencies in hertz of the top octave (6)
; divide these down by powers of two to get all other octaves
;
        PUBLIC  note_tbl
note_tbl:
        DW      4186                    ;C      1
        DW      4435                    ;C#     2
        DW      4699                    ;D      3
        DW      4978                    ;D#     4
        DW      5274                    ;E      5
        DW      5588                    ;F      6
        DW      5920                    ;F#     7
        DW      6272                    ;G      8
        DW      6645                    ;G#     9
        DW      7040                    ;A     10
        DW      7459                    ;A#    11
        DW      7902                    ;B     12

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           pqaccent - queue accent information
;
;       Call:           pqaccent(voice, paccent)
;                       int   voice;
;                       int   tempo;
;                       int   volume;
;                       int   mode;
;                       int   pitch;
;
;       Purpose:        Pqaccent will queue the accent information in the
;                       specified voice queue. If there is not enough room
;                       in the queue an error is returned. If voice is out
;                       of range pqaccent is ignored. Space for a single
;                       sync mark is always left in the queue. The effects
;                       of pqaccent last until the next pqaccent call.
;                       The pqaccent queue is not counted as a note.
;
;                       Note:   This routine will not operate on the
;                               noise voice queue.
;
;                       Note:   Pitch is interpreted as follows:
;                               There is a pitch wheel which varies from
;                               0 to 83 where 0 is the default value.
;                               Pitch is added to note value in pqnote
;                               using modulus 84 arithmetic. For example
;                               note value 20 + pitch 30 gives an
;                               effective note value of 50. Another
;                               example: note value 70 + pitch value 30
;                               gives an effective note value of 16.
;
;                       Note:   The volume can be set at the default level
;                               by using volume 65535.
;
;       Entry:          voice on stack
;                       accent on stack as follows: tempo (32 to 255)
;                                                   volume (0 to 65535)
;                                                   mode (n,s,l)
;                                                   pitch (0 to 83)
;
;       Exit:           AX = erqful - Queue full
;                          = erbdtp - Invalid tempo
;                          = erbdvl - Invalid volume
;                          = erbdmd - Invalid mode
;
;       Modified:       BX,CX,DX
;
;-----------------------------------------------------------------------

cProc   SetVoiceAccent,<PUBLIC,FAR>,<si,di>
        parmW   voice
        parmW   tempo
        parmW   volume
        parmW   mode
        parmW   pitch
cBegin
        call    checkpid
        jnz     qa_end
        mov     ax,voice
        cmp     al,ntvoices
        ja      qa_end

        DEC     AL                      ;0 based voice number
        ;
        ; Get index into queue location / control table
        ;
        MOV     BX,OFFSET qlocinfo
        MOV     CL,qlocsize
        MUL     CL                      ;AX was 0 based voice number
        ADD     AX,BX                   ;index for this voice location etc.
        MOV     SI,AX
        mov     bx,volume               ;volume (0 to 65535)
        ;
        ; Give the volume change information to the OEM.
        ;
        mov     ax,voice
        mov     ah,al                   ;[ah] = voice
        DEC     AH                      ;(0 based)
        ;
        ; [AH] = voice (0 based)
        ; [BX] = volume
        ;
        MOV     AL,f_volume             ;change volume function
        CALL    mdd_play
        JC      bad_volume              ;some errors detected
        ;
        ;
        mov     ax,tempo                ;tempo (32 to 255)
        MOV     CX,mode                 ;mode (normal/leggato/staccato)
        MOV     DX,pitch                ;pitch (0 to 84)
        ;
        ; Now:  [AX] = tempo (32 to 255)
        ;       [CX] = mode (normal/leggato/staccato)
        ;       [DX] = pitch (0 to 83) in pitch wheel
        ;
        ; Test and store these values
        ;
        OR      AH,AH                   ;Test tempo <= 255
        JNZ     bad_tempo               ;Invalid Tempo
        CMP     AL,32D                  ;Test tempo >= 32
        JB      bad_tempo               ;Invalid Tempo
        MOV     [SI].q_tempo,AL         ;Save the tempo for this voice
        CMP     CX,2                    ;Test mode 0,1,2
        JA      bad_mode                ;Invalid Mode
        MOV     [SI].q_mode_nls,CL      ;Save the mode for this voice
        CMP     DX,83D                  ;Test pitch wheel <=83
        JA      bad_pitch               ;Invalid pitch
        MOV     [SI].q_pitch,DL         ;Save the pitch for this voice
qa_end:
        XOR     AX,AX                   ;No error condition
qa_exit:
cEnd    pqaccent

        ;
        ; Error Handlers:
        ;
bad_pitch:
        MOV     AX,erbdpt               ;Invalid Pitch
        JMP     SHORT qa_exit
bad_mode:
        MOV     AX,erbdmd               ;Invalid mode
        JMP     SHORT qa_exit
bad_tempo:
        MOV     AX,erbdtp               ;Invalid tempo
        JMP     SHORT qa_exit
bad_volume:
        CALL    field_errors            ;queue full or bad volume
        JMP     SHORT qa_exit

sEnd    CODE

        END
