        TITLE   MDD_PLAY - IBM Multivoice Music Device Driver Specifics
?PLM = 1
?WIN = 1

XDDEF = 0

.xlist
include cmacros.inc
.list

        PAGE

;-----------------------------------------------------------------------
;
;                               Data
;
;-----------------------------------------------------------------------

sBegin  DATA

PUBLIC  beepsw
beepsw  DB      0       ;used for BEEP ON/OFF (default is BEEP ON)
EXTRN   cur_voice_id:BYTE       ;current voice id - set by get_q_seg
EXTRN   $first:BYTE             ;this byte is used in start_music and $sndoff
                                ;If it is set, then we do not mess with the speaker.
EXTRN   now_playing:BYTE        ;0 indicates beep() function completed
                                ;1 indicates in want to start beep() function
                                ;2 indicates beep() function currently beeping speaker
EXTRN   qcblock:WORD            ;queue control block
EXTRN   tew_flags:WORD          ;Threshold Event Word Flags
                                ; bit 0 = 0 indicates music voice 1 queue above threshold
                                ; bit 0 = 1 indicates music voice 1 queue below threshold
                                ; bit 1 = 0 indicates music voice 2 queue above threshold
                                ; bit 1 = 1 indicates music voice 2 queue below threshold
                                ; bit 2 = 0 indicates music voice 3 queue above threshold
                                ; bit 2 = 1 indicates music voice 3 queue below threshold
                                ; bit 3 = 0 indicates noise voice 4 queue above threshold
                                ; bit 3 = 1 indicates noise voice 4 queue below threshold
EXTRN   $tichip:BYTE            ;TI chip active or not flag
                                ; 0 indicates SOUND OFF executed or no multivoice
                                ; 1 indicates SOUND ON executed or multivoice
EXTRN   tt_installed:BYTE       ;Tic_Toc Interrupt handler active or not flag
                                ; 0 indicates tic_toc not installed
                                ; 1 indicates tic_toc installed
                                ; 2 indicates tic_toc installed but may be de-installed
EXTRN   vceoff:BYTE             ;keeps track of # of voices off at any time
                                ; 00000000 all voices off and ready to start again
                                ; set bit - voice off
                                ; cleared bit - voice on
sEnd    DATA

        PAGE
;-----------------------------------------------------------------------
;
;       IBM PC Specific Code for the Multivoice Music Device Driver
;
;       Machine Interface Module
;
;       (C) 1985 Microsoft Inc. - Last Revision March 19, 1985
;
;-----------------------------------------------------------------------
;
;       This module provides is a sample of the OEM dependent
;       portion of code for the Microsoft Multivoice Music and
;       Sound Device Driver. This sample code for the music device
;       driver is intended for the complete family of IBM PC's
;       including the PC, XT, AT and Junior models.
;
;       This module interfaces with the OEM in dependent
;       portion of the Multivoice Music and Sound Device Driver.
;       OEMs must link this machine specific code with the OEM
;       independent module to create a device driver (or library)
;       accessible to C programs.
;
;-----------------------------------------------------------------------

sBegin  CODE

assumes CS,CODE
assumes DS,DATA
assumes ES,DATA
assumes SS,DATA

;-----------------------------------------------------------------------
;
;                               Structures
;
;-----------------------------------------------------------------------

;
;This queue structure is used by the MUSIC queues.
;
que_ctrl_blk    STRUC
        quenote DW      ?       ;number of notes in queue
        quetop  DW      ?       ;pointer to highest memory location of queue
                                ; also queue length (in bytes)
        quenum  DW      ?       ;queue counter in bytes - number of bytes in queue
        queget  DW      ?       ;where to get next byte from queue
        queput  DW      ?       ;where to put next byte into queue
        queseg  DW      ?       ;segment of queue (queseg:0 is first byte)
        qthresh DW      ?       ;threshold determining queue event (set by user)
        quevol  DB      ?       ;default volume (may change as we're playing)
que_ctrl_blk    ENDS

        PAGE

;-----------------------------------------------------------------------
;
;                               Constants
;
;-----------------------------------------------------------------------

clkint = 20H            ;X'20' clock interrupt vector
disable EQU     CLI
debug = 0               ;1=debug versions, 0=delivered versions
enable  EQU     STI
eoi    = 20H            ;X'20' 8259 end_of_interrupt ACK

err_qfull   = 1         ;No room in the queue for this request
err_freq    = 2         ;Unsupported frequency
err_vol     = 3         ;Unsupported volume index
err_eshape  = 4         ;Unsupported envelope shape
err_dur     = 5         ;Unsupported envelope duration
err_noise   = 6         ;Unsupported noise source

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

intao  = 20H            ;X'20' 8259 int control port
mskreg = 21H            ;8259 interrupt mask register
note_size = 7           ;7 bytes required by each music note
num_queues =4           ;number of queues
qcbsize = SIZE que_ctrl_blk
speaker = 61H           ;speaker latch port address
spkron = 3              ;Speaker on mask
spkren = 10H            ;speaker enable/disable mask
square = 0B6H           ;timer 2 square wave - mode 3.
tiaddr = 0C0H           ;X'C0' TI sound chip address
timer0 =  40H           ;timer 0 port address
timer2 =  42H           ;timer 2 port address
tmrcmd =  43H           ;timer control port
tod_int =  8H           ;ROM time of day interrupt
type_note  = 1          ;first byte of a note in a queue
type_vol   = 2          ;first byte of a volume change in a queue
type_env   = 3          ;first byte of a envelope change in a queue
type_noise = 4          ;first byte of a noise note in a queue
type_sync  = 0FFH       ;only byte of a sync mark in a queue


EXTRN   mach_init:NEAR
EXTRN   getmchnid:NEAR
EXTRN   get_q_seg:NEAR
EXTRN   start_music:NEAR
EXTRN   stop_music:NEAR
EXTRN   $sndoff:NEAR

        PAGE

;-----------------------------------------------------------------------
;
;                               Macros
;
;-----------------------------------------------------------------------

pause   MACRO                           ;macro to insure that an instruction
        JMP     $+2                     ;fetch occurs between IN and/or OUT
        ENDM                            ;instructions on the PC AT machine

        PAGE

;-----------------------------------------------------------------------
;
;                   Interrupt Vectors for the IBM PC
;
;-----------------------------------------------------------------------
;
;  Before Installation of Music Device Driver:
;
;                   +-------------------+
;                   |                   |
;       INT_8 ----->|  ROM_Time_Of_Day  |
;                   |     18.2 / s      |
;                   |                   |
;                   +-------------------+
;
;  After Installation of Music Device Driver but before Music Plays:
;
;                   +-------------------+
;                   |                   |
;       INT_8 ----->|  ROM_Time_Of_Day  |
;                   |     18.2 / s      |
;       rom_tod --->|                   |
;                   |                   |
;                   +-------------------+
;
;  After Installation of Music Device Driver and as Music Plays:
;
;                   +-------------------+
;                   |                   |
;       INT_8 ----->|    MDD_tic_toc    |
;                   |     572.4 / s     |
;                   |                   |
;                   |              JMP -|--------+
;                   |                   |        |
;                   +-------------------+        |
;                                                |
;                   +-------------------+        |
;                   |                   |        |
;       rom_tod --->|  ROM_Time_Of_Day  |<-------+
;                   |     18.2 / s      |
;                   |                   |
;                   +-------------------+
;
;-----------------------------------------------------------------------

        PAGE

;-----------------------------------------------------------------------
;
;       Changes made to the GW-BASIC Interface Specification for
;       $donote, $bleep and $rdplay.
;
;-----------------------------------------------------------------------
;
;       Note that the following changes refer to the multivoice version
;       of the $donote interface. This $donote interface is the one
;       required by all GW-BASIC 2.0 (and above) Compiler products and
;       all GW-BASIC 2.0 (and above) Interpreter products with the
;       Junior feature set.
;
;    1. All references to hard coded addresses should be removed.
;       Memory and interrupt vectors reserved for BASIC should
;       not be used.
;
;    2. The routine mdd_voice now performs some of the initialization
;       that was originally performed by $gwini.
;
;    3. The routine mdd_term now performs some of the termination that
;       was originally performed by $gwterm.
;
;    4. The Music Device Driver assumes dynamic queues rather than
;       static queues. Hence, it will be necessary to alter the references
;       to these queues accordingly. Refer to the mdd_iniq routine.
;
;    5. In GW-BASIC normal and staccato notes are queued as two notes,
;       where the first note is the real part of the note, and the second
;       part is the interstice (or rest period between notes). This has
;       been changed so that the interstice duration is now passed to $donote
;       with the rest of the note information.
;
;    6. The $donote routine no longer has the volume parameter passed to
;       it when notes are queued. Changes in volume are passsed in a
;       separate call to $donote.
;
;    7. The SOUND OFF/ON and BEEP OFF/ON calls for $donote have been removed.
;       The OEM is now responsible for determining when to perform these
;       functions. ??????
;
;    8. The $bleep routine is no longer required. Beeps are queued as
;       .25 second 800 Hz signals.
;
;    9. This interface must now maintain a word in memory that is used
;       by both the user and the c interface for event trapping. This
;       word describes the current status of all queues with repect
;       to an threshold level also set by the user. The word contains
;       one bit per queue. A set bit indicates that the queue contains fewer
;       notes than the threshold level. Typically, when the queue reaches
;       the threshold, more notes are added by the user.
;       In this example, we use tew_flags to maintain the threshold event
;       word flags, and mdd_event to report it's location to the c
;       interface.
;
;-----------------------------------------------------------------------

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           mdd_play - Music Device Driver OEM interface
;
;       Purpose:        This routine handles all calls from the
;                       independent portion of the music device driver
;                       to the OEM specific code for the music
;                       device driver. The following description should
;                       be all that is required for an OEM to implement
;                       the music device driver in any machine.
;
;       Entry:          [AL] = function code as follows:
;
;                            = f_init - initialize and return voices
;                            = f_q_size - initialize queues with queue size
;                            = f_q_note - queue a note or rest
;                            = f_volume - queue volume change
;                            = f_envelope - queue envelope change
;                            = f_start - start music
;                            = f_stop - stop music
;                            = f_sync - synchronize voices
;                            = f_term - terminate
;                            = f_ev_ptr - get event flag pointer
;                            = f_flags - get event flag and clear it
;                            = f_trap - set trap level
;                            = f_noise - queue noise
;                            = f_num_notes - get number of notes in queue
;
;                       The other registers depend upon the function and
;                       are detailed below.
;
;       Exit:           Exit conditions are dependent on the required
;                       function except when an error condition occurs.
;
;                       PSW.C set if an error condition is indicated and
;                       [AL] = err_qfull   - No room in the queue or
;                       [AL] = err_freq    - Unsupported frequency or
;                       [AL] = err_vol     - Unsupported volume index or
;                       [AL] = err_eshape  - Unsupported envelope shape or
;                       [AL] = err_dur     - Unsupported envelope duration or
;                       [AL] = err_noise   - Unsupported noise source
;
;                       else
;
;                       PSW.C reset if there are no errors. See the specific
;                       function for a description of registers on exit.
;
;       Function Entry Conditions are as follows:
;
;       f_init:         This routine is called whenever the Music
;                       Device Driver is opened. Machine specific
;                       initialization must occur at this time.
;
;                       If there is a hardware problem, or if the
;                       device is not physically installed, an error
;                       condition must be returned and the Music
;                       Device Driver will not be opened.
;
;                       This routine is expected to return the
;                       number of voices available if there are
;                       no hardware problems. The number of voices
;                       are required in part for the default allocation
;                       of memory for the music queues.
;
;                       Note:   This interface can handle at most 16
;                               voices total.
;
;                       Note:   This interface can handle at most 1
;                               noise voice. The noise voice is assumed
;                               to have a handle of 1 + number of music
;                               voices.
;
;                       Entry:          [AL] = f_init
;
;                       Exit:           PSW.C set if device not available
;                                                 or errors detected
;
;                                       else
;
;                                       PSW.C reset and
;                                       [AH] = number of noise voices
;                                              available (max 1)
;                                       [AL] = number of music voices
;                                              available (max 16)
;
;                       Modified:       none
;
;       f_q_size:       This routine is called whenever one of the Music
;                       Device Driver queues is changed in size or
;                       location.
;
;                       The queue for the given voice is expected to be
;                       initialized if the number of bytes on entry is
;                       non 0. If the queue size is 0 bytes, then this
;                       voice will not be used to play any music.
;
;                       Note:   This routine must not alter the threshold
;                               event detection value for this queue.
;
;                       Entry:          [AL] = f_q_size
;                                       [AH] = voice (0 based)
;                                       [BX] = number of bytes in queue
;                                              memory for this voice
;                                       [CX:0000] points to the start of
;                                                 music queue memory
;
;                       Exit:           none
;
;                       Modified:       none
;
;       f_q_note:       This function queues either a note or a pause.
;
;                       The interstice parameter is the duration of the
;                       short rest that follows all staccato and normal
;                       notes.
;
;                       Both duration parameters are understood to be in
;                       units of 2.5 milliseconds.
;
;                       Pauses have 0 note duration. Frequency should be
;                       ignored in this case.
;
;                       Entry:          [AL] = f_q_note
;                                       [AH] = Voice (0 based)
;                                       [BX] = Frequency
;                                       [CX] = Duration of note
;                                       [DX] = Duration of interstice if any
;
;                       Exit:           PSW.C set indicates one of the following:
;                                       [AL] = err_qfull   - No room in the queue or
;                                       [AL] = err_freq    - Unsupported frequency or
;
;                                       else
;
;                                       PSW.C reset
;
;                       Modified:       none
;
;       f_volume:       This function queues a volume change in the
;                       appropriate music queue.
;
;                       Entry:          [AL] = f_volume
;                                       [AH] = voice (0 based)
;                                       [BX] = volume (default is 0FFFFH)
;
;                       Exit:           PSW.C set indicates one of the following errors:
;                                       [AL] = err_qfull   - No room in the queue or
;                                       [AL] = err_vol     - Unsupported volume index or
;
;                                       else
;
;                                       PSW.C reset
;
;       f_envelope:     This function queues an envelope change in the
;                       appropriate music queue.
;
;                       Entry:          [AL] = f_envelope
;                                       [AH] = voice (0 based)
;                                       [BX] = shape
;                                       [CX] = duration / period
;
;                       Exit:           PSW.C set indicates one of the following errors:
;                                       [AL] = err_qfull   - No room in the queue or
;                                       [AL] = err_eshape  - Unsupported envelope shape or
;                                       [AL] = err_dur     - Unsupported envelope duration or
;
;                                       else
;
;                                       PSW.C reset
;
;                       Modified:       none
;
;       f_start:        This function starts the music playing. All voices
;                       voices are activated.
;
;                       This function requests that notes queued for each
;                       voice be played. Notes should be played until the
;                       queue is empty. This function will be called to
;                       restart music when it is possible that there are
;                       notes in the queue that should be played and music
;                       is not playing. No other event should start music.
;
;                       Entry:          [AL] = f_start
;
;                       Exit:           none
;
;                       Modified:       none
;
;       f_stop:         This function stops the music playing and flushes
;                       all remaining notes from all the queues.
;
;                       Entry:          [AL] = f_stop
;
;                       Exit:           none
;
;                       Modified:       none
;
;       f_sync:         This function queues in a sync byte into the
;                       appropriate music queue. sync bytes are used to
;                       synchronize voices. Whenever a sync byte is
;                       encountered in a music queue, the corresponding
;                       queue is suspended until sync bytes are encountered
;                       in every other music queue.
;
;                       When speaker activity is started, each voice queue is
;                       processed until no notes remain OR until a
;                       syncronization byte is encountered. When a
;                       syncronization byte is encountered in a voice
;                       queue, that voice is suspended until a synchronization
;                       byte is encountered in every voice queue. Then the
;                       voice queues are processed again until each queue
;                       is empty or another synchronization byte is
;                       encountered in a voice queue in which case it gets
;                       suspended as before.
;
;                       Entry:          [AL] = f_sync
;                                       [AH] = Voice (0 based)
;
;                       Exit:           PSW.C set if no space in queue and
;                                       [AL] = err_qfull
;
;                                       else
;
;                                       PSW.C reset
;
;                       Modified:       none
;
;       f_term:         This routine is called when ever the Music
;                       Device Driver is closed. Machine specific
;                       termination must occur at this time.
;
;                       Entry:          [AL] = f_term
;
;                       Exit:           none
;
;                       Modified:       none
;
;       f_ev_ptr:       This function returns a short pointer to the
;                       data segment location of the threshold event
;                       word flags. These flags may be monitored by
;                       the c interface or by the user to determine
;                       the status of individual queues.
;
;                       Entry:          [AL] = f_ev_ptr
;
;                       Exit:           [AX] = pointer to threshold event flags
;
;                       Modified:       none
;
;       f_flags:        This function returns the threshold flags
;                       and then clears these flags.
;
;                       Entry:          [AL] = f_flags
;
;                       Exit:           [BX] = threshold event flags
;
;                       Modified:       none
;
;       f_trap:         This function sets the threshold level for event
;                       detection. An event occurs when a music queue
;                       shrinks from n to n-1 notes where n is specified
;                       as the entry condition to this function.
;
;                       Entry:          [AL] = f_trap
;                                       [AH] = voice (0 based)
;                                       [BX] = the number of notes specified
;                                              for the threshold
;
;                       Exit:           none
;
;                       Modified:       none
;
;       f_noise:        This function queues noises to the noise channel queue.
;
;                       Entry:          [AL] = f_noise
;                                       [CX] = Source
;                                       [DX] = Duration
;
;                       Exit:           PSW.C set if there's an error and one of:
;                                       [AL] = err_qfull if no room in the
;                                                        queue for this request
;                                       [AL] = err_noise if unsupported noise
;                                                        source
;
;                                       else
;
;                                       PSW.C reset
;
;                       Modified:       none
;
;       f_num_notes:    This function returns the number of notes remaining
;                       in a queue.
;
;                       Entry:          [AL] = f_num_notes
;                                       [AH] = voice (0 based)
;
;                       Exit:           [BX] = number of notes in this queue
;
;                       Modified:       none
;
;
;-----------------------------------------------------------------------

        PUBLIC  mdd_play

mdd_play:

        PUSH    SI
        PUSH    AX
        XOR     AH,AH                   ;We're interested only in the function code
        DEC     AL                      ; which was 1 based
        SHL     AX,1                    ;Turn it into a word pointer
        MOV     SI,AX                   ; relative to play_table
        POP     AX
        CALL    CS:f_table[SI]          ;branch to the appropriate routine
        POP     SI
        RET

        PUBLIC  f_table

f_table DW OFFSET mdd_voice             ;f_init - initialize and return voices
        DW OFFSET mdd_iniq              ;f_q_size - initialize queues with queue size
        DW OFFSET queue_note            ;f_q_note - queue a note or rest
        DW OFFSET queue_volume          ;f_volume - queue volume change
        DW OFFSET q_envelope            ;f_envelope - queue envelope change
        DW OFFSET start_music           ;f_start - start music
        DW OFFSET stop_music            ;f_stop - stop music
        DW OFFSET queue_sync            ;f_sync - synchronize voices
        DW OFFSET mdd_term              ;f_term - terminate
        DW OFFSET mdd_event             ;f_ev_ptr - get event flag pointer
        DW OFFSET mdd_flags             ;f_flags - get event flag and clear it
        DW OFFSET set_threshold         ;f_trap - set trap level
        DW OFFSET queue_noise           ;f_noise - queue noise
        DW OFFSET notes_in_queue        ;f_num_notes - get number of notes in queue

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           mdd_voice - Machine Specific Initialization.
;
;       Purpose:        This routine is called whenever the Music
;                       Device Driver is opened. Machine specific
;                       initialization must occur at this time.
;
;                       If there is a hardware problem, or if the
;                       device is not physically installed, an error
;                       condition must be returned and the Music
;                       Device Driver will not be opened.
;
;                       This routine is expected to return the
;                       number of voices available if there are
;                       no hardware problems. The number of voices
;                       are required in part for the default allocation
;                       of memory for the music queues.
;
;                       Note:   This interface can handle at most 16
;                               voices total.
;
;                       Note:   This interface can handle at most 1
;                               noise voice. The noise voice is assumed
;                               to have a handle of 1 + number of music
;                               voices.
;
;                       In the IBM specific case, we must determine
;                       the machine we are currently running on, and
;                       report the number of voices appropriately.
;
;       Entry:          none
;
;       Exit:           PSW.C set if there is a hardware failure
;
;                        else
;
;                       PSW.C reset and
;                       [AH] = number of noise voices available (max 1)
;                       [AL] = number of music voices available (max 16)
;
;       Modified:       none
;
;-----------------------------------------------------------------------

        PUBLIC  mdd_voice

mdd_voice:
        MOV     now_playing,1           ;now playing music
        CALL    mach_init               ; Determine what machine we're currently running on
        XOR     AH,AH
        CALL    getmchnid               ;is this a junior?
        JZ      ini_jr                  ; brif so
        MOV     AL,0FFH                 ; else must be single voice
        DEC     AH
ini_jr: INC     AH                      ;number of noise voices (1 for Jr, else 0)
        NEG     AL                      ;get us 3 for Junior, 1 for others
        CLC                             ;no errors
        enable
        RET

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           mdd_iniq - Music/Noise Queue Initialization
;
;       Purpose:        This routine is called whenever one of the Music
;                       Device Driver queues is changed in size or
;                       location.
;
;                       The queue for the given voice is expected to be
;                       initialized if the number of bytes on entry is
;                       non 0. If the queue size is 0 bytes, then this
;                       voice will not be used to play any music.
;
;                       Each queue is initialized as follows:
;
;                               queue control           queue:
;                                  block:
;                                                     +--------+
;                                                +--->|  high  |
;                                  quenote = 0   |    | memory |
;                                  quetop -------+    |        |
;                                  quenum = 0         |        |
;                                  queget ------+     |        |
;                                  queput ----+ |     |        |
;                                             | |     |  low   |
;                                  queseg:0---+-+---->| memory |
;                                  qthresh = p        +--------+
;
;                       Note:   This routine must not alter the threshold
;                               event detection value for this queue.
;
;       Entry:          [AH] = voice (0 based)
;                       [BX] = number of bytes in queue memory for this voice
;                       [CX:0000] points to the start of music queue memory
;
;       Exit:           none
;
;       Modified:       none
;
;-----------------------------------------------------------------------

        PUBLIC  mdd_iniq                ;use ini_que to save a few bytes ?????????

mdd_iniq:
        PUSH    AX
        PUSH    DX
        PUSH    SI
        MOV     AL,AH                   ;0 based voice
        XOR     AH,AH
        MOV     DL,qcbsize              ;size of queue control block
        MUL     DL                      ;offset into queue control block
        MOV     DX,OFFSET qcblock       ;
        ADD     AX,DX                   ;address of this voice's q control block
        MOV     SI,AX                   ;index into queue control block
        MOV     [SI].queseg,CX          ;queue segment location
        MOV     [SI].quetop,BX          ;queue top (and also length)
        XOR     AX,AX
        MOV     [SI].queget,AX          ;initialize get vector for this queue
        MOV     [SI].queput,AX          ;initialize put vector for this queue
        MOV     [SI].quenote,AX         ;initialize number of notes
        MOV     [SI].quenum,AX          ;initialize number of bytes in queue
        POP     SI
        POP     DX
        POP     AX
        RET

        PAGE

;-----------------------------------------------------------------------
;
;       Queue Management
;
;-----------------------------------------------------------------------
;
;       The following is the suggested manner for OEM queue management.
;
;       Music Queues:
;
;                  Item          element         #bytes required
;                  ----          -------         ---------------
;
;               Music Note      type (01)               1
;                               frequency               2
;                               duration                2
;                               interstice              2
;
;               Accent          type (02)               1
;                               volume                  1
;
;               Envelope        type (03)               1
;                               shape                   1
;                               duration/period         1
;
;               Sync Mark       type (FF)               1
;
;
;       Noise Queues:
;
;                  Item          element         #bytes required
;                  ----          -------         ---------------
;
;               Noise Note      type (04)               1
;                               source                  1
;                               duration                2
;
;               Accent          type (02)               1
;                               volume                  1
;
;               Sync Mark       type (FF)               1
;
;-----------------------------------------------------------------------

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           queue_in - add an element to the queue
;
;       Purpose:        This routine queues in a byte contained in [AL]
;                       into the appropriate music control queue.
;                       Address of the music control queue is contained
;                       in [BX].
;
;                       The queues are circular.
;
;                       Additions to the queue are made as follows:
;
;                          queue        queue control           queue
;                         before:          block:               after:
;
;                       +--------+                            +--------+
;                       |  note  |<--------quetop------------>|  note  | high
;                       |  note  |     n = quenote = n + 1    |  note  | memory
;                       |  note  |         queget------------>|  note  |
;                       |        |                            |        |
;                       |        |                            |        |
;                       |        |     m = quenum = m + 7     |        |
;                       |        |      +--queput------------>|        |
;                       |        |<-----+                     |  note  |
;                       |  note  |                            |  note  | low
;                       |  note  |<-------queseg:0----------->|  note  | memory
;                       +--------+        qthresh = p         +--------+
;
;       Entry:          [AL] = Byte to be queued
;                       [BX] = Address of appropriate music queue control block
;                       [ES] = segment of queue
;                       [ES:00] pointer to queue
;                       [DS:BX] pointer to queue control block
;
;       Exit:           PSW.C set if no space in queue and
;                       [AL] = 1
;
;                       else
;
;                       PSW.C reset
;
;       Modified:       quenum, queput, queget
;
;-----------------------------------------------------------------------

        PUBLIC  queue_in

queue_in:

        PUSH    SI
        MOV     SI,[BX].queput          ;get put PTR
        MOV     ES:[SI],AL              ;queue in the byte
        INC     SI                      ;advance put PTR
        CMP     SI,[BX].quetop          ;wrap around ?
        JNZ     nwrque                  ;No
        MOV     SI,0                    ;get queue bottom
nwrque:
        MOV     [BX].queput,SI          ;store new put PTR
        INC     [BX].quenum
        POP     SI
        RET

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           check_space - check for space in the queue
;
;       Purpose:        This routine checks for space in the appropriate
;                       music queue. It sets carry if there is no space
;                       and returns the error code 1 in [AL]
;                       ******IMPORTANT******
;                       Clear or set carry to indicate space available or
;                       not available respectively.
;
;                       Note:   As we require 6 bytes in all to queue a note
;                               completely (1+2+2+1) make sure that there is
;                               space for 6 bytes in the appropriate music
;                               queue.
;
;                       Note:   This routine works with both music and
;                               noise queues.
;
;       Entry:          [BX] = address of appropriate music queue control block
;
;       Exit:           PSW.C set if no space in queue and
;                       [AL] = err_qfull
;
;                       else
;
;                       PSW.C reset
;
;       Modified:       none
;
;-----------------------------------------------------------------------

        PUBLIC  check_space

check_space:
        PUSH    AX                      ;save register...
        MOV     AX,[BX].quetop          ;get size of queue
        SUB     AX,note_size+1          ;size of note entry in queue
                                        ; plus one for a sync byte
        CMP     [BX].quenum,AX          ;test if queue is full
        POP     AX                      ;restore register
        JA      nospce                  ;Brif not
        CLC                             ;else indicate space available
        RET
nospce:
        MOV     AL,err_qfull            ;error code for queue full
        STC                             ;carry set to indicate no space in queue
        RET

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           queue_note - add a note or rest to one of the queues
;
;       Purpose:        This routine queues either a note or a pause.
;
;                       This routine calls a number of device dependent
;                       routines. These device dependent routines check
;                       for the validity of frequency, duration and volume.
;                       They also output these quantities to either the TI
;                       chip or the 8253 timer chip.
;
;                       It either queues in the whole note or doesn't
;                       queue at all.
;
;                       The reason for using OLDFREQ is as follows: The
;                       problem arises when noise is using source 7 and
;                       voice 3 is playing stacatto or normal notes. During
;                       the period the note plays a pause, the frequency
;                       of the note is different from what it was during the
;                       first half. This causes an unpleasant effect as far
;                       as noise is concerned. Solution....queue in same
;                       frequency for pause if SOUND ON has been executed.
;
;                       The interstice parameter is the duration of the
;                       short rest that follows all staccato and normal
;                       notes.
;
;                       Both duration parameters are understood to be in
;                       units of 2.5 milliseconds.
;
;                       Pauses have 0 note duration and 0 duration. Frequency
;                       may be ignored in this case.
;
;       Entry:          [AH] = Voice (0 based)
;                       [BX] = Frequency        ;what about fractional part ?????
;                       [CX] = Duration of note
;                       [DX] = Duration of interstice if any
;
;       Exit:           PSW.C set indicates one of the following:
;                       [AL] = err_qfull   - No room in the queue or
;                       [AL] = err_freq    - Unsupported frequency or
;
;                       else
;
;                       PSW.C reset
;
;       Modified:       none
;
;-----------------------------------------------------------------------

;       ** Queue a note:        ?????
;          This function is passed a voice identification, volume,
;          frequency and a duration. The note is queued for the
;          specified voice. No attempt to start the speaker is made.
;
;       ** Queue a rest:
;          This function is passed a voice identification and a
;          duration. Stacatto and Normal mode notes are built from
;          a note and a short pause. This function is used to play
;          the fraction of a note that defines a short pause in case
;          of Stacatto and Normal notes.
;

        PUBLIC  queue_note

queue_note:

        disable                         ;don't want a partial note in the queue
        PUSH    BX                      ;frequency
        CALL    get_q_seg               ;[ES:0000] points to queue
                                        ;[DS:BX] points to queue control block
        CALL    check_space             ;is there enough room for another note ?
        JC      queue_full              ;returns PSW.C set and [AL] = err_qfull
        POP     AX                      ;frequency
        OR      AX,AX                   ;0 frequency ?
        JZ      queue_pause             ;brif rest/pause
        OR      CX,CX                   ;0 note duration?
        JZ      queue_pause             ;brif rest/pause
        ;
        ; [AX] = frequency
        ; [BX] = pointer to queue control block
        ; [CX] = note duration
        ; [DX] = interstice duration
        ;
        CALL    check_freq              ;check frequency in AX
        JC      freq_err                ;brif frequency error
        CALL    check_duration          ;check note duration
interstice:
        XCHG    CX,DX                   ;and also interstice
        CALL    check_duration          ;check interstice duration
        ;
        ; [AX] = frequency      (may be 0 if pause)
        ; [BX] = pointer to queue control block
        ; [CX] = interstice duration
        ; [DX] = note duration  (will be ignored if frequency 0)
        ;
        INC     [BX].quenote            ;everything's valid - update note counter
        PUSH    AX                      ;save frequency
        MOV     AL,type_note            ;queue up a note
        CALL    queue_in                ;start the queing
        POP     AX                      ;frequency
        CALL    freq_to_tics            ;turn frequency in [AX] into timer tics
        CALL    queue_in                ;LSB of frequency
        MOV     AL,AH                   ;MSB of frequency
        CALL    queue_in
        MOV     AX,DX                   ;note duration
        CALL    queue_in                ;LSB of note duration
        MOV     AL,AH                   ;MSB of note duration
        CALL    queue_in
        MOV     AX,CX                   ;interstice duration
        CALL    queue_in                ;LSB of interstice duration
        MOV     AL,AH                   ;MSB of interstice duration
        CALL    queue_in
        JMP     SHORT all_queued

queue_pause:
        ;
        ; [DX] = duration of interstice (only valid parameter)
        ;
        XOR     AX,AX                   ;ensure 0 frequency
        DEC     AX                      ;by using an inaudible frequency
        JMP     SHORT interstice

all_queued:
        CLC                             ;return no errors

freq_err:                               ;PSW.C set and [AL] = err_freq
        PUSH    BX                      ;dummy push

queue_full:                             ;PSW.C set and [AL] = err_qfull
        POP     BX                      ;clean up stack
        enable
        RET

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           queue_sync - queue a sync mark
;
;       Purpose:        This routine queues in a SYNC byte into the
;                       appropriate music queue. SYNC bytes are used to
;                       synchronize voices. Whenever a SYNC byte is
;                       encountered in a music queue, the corresponding
;                       queue is suspended until SYNC bytes are encountered
;                       in every other music queue.
;
;       Entry:          [AH] = Voice Id (0 based)
;
;       Exit:           PSW.C set if no space in queue and
;                       [AL] = err_qfull
;
;                       else
;
;                       PSW.C reset
;
;       Modified:       none
;
;-----------------------------------------------------------------------

        PUBLIC  queue_sync

queue_sync:
        CMP     $tichip,1               ;sound ON ?
        JE      qsync1                  ;brif so
        CLC                             ;else just return
        RET                             ;
qsync1:                                 ;
        PUSH    BX                      ;save [BX]
        CALL    get_q_seg               ;[ES:0000] points to queue
                                        ;[DS:BX] points to queue control block
        PUSH    AX                      ;save register...
        MOV     AX,[BX].quetop          ;get size of queue
        CMP     [BX].quenum,AX          ;test if queue is full
        POP     AX                      ;restore register
        JB      spceok                  ;Brif not full
        MOV     AL,err_qfull            ;error code for queue full
        STC                             ;set carry to indicate no space
        JMP     SHORT qsyret
spceok:
        disable                         ;interrupts off while queueing
        PUSH    AX
        MOV     AL,type_sync            ;SYNC byte in [AL]
        CALL    queue_in
        POP     AX
        enable                          ;restore interrupts
        CLC                             ;clear carry to indicate no error
qsyret:
        POP     BX                      ;restore [BX]
        RET

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           queue_volume - queue a volume change in music queue
;
;       Purpose:        This function queues a volume change in the
;                       appropriate music queue.
;
;                       This routine checks for legal volume and converts
;                       the volume into attenuation and further formats
;                       this attenuation to suit the needs of the TI chip.
;                       If sound is directed to the 8253 timer it just
;                       returns.
;
;       Entry:          [AL] = f_volume
;                       [AH] = voice (0 based)
;                       [BX] = volume (default is 0FFFFH)
;
;       Exit:           PSW.C set indicates one of the following errors:
;                       [AL] = err_qfull   - No room in the queue or
;                       [AL] = err_vol     - Unsupported volume index or
;
;                       else
;
;                       PSW.C reset
;
;       Modified:       none
;
;-----------------------------------------------------------------------

        PUBLIC  queue_volume

queue_volume:
        PUSH    BX
        PUSH    CX
        ;
        ; first check if we even have to worry about volume
        ;
        CALL    getmchnid               ;is this a Junior or a Senior?
        JNZ     ignore_vol              ;Non-Junior - ignore volume change
        ;
        ; check if there is space in the queue
        ;
        PUSH    BX                      ;volume change
        CALL    get_q_seg               ;[DS:BX] points to queue contol block
                                        ;[ES:0] points to queue
        CALL    check_space             ;see if there's space in the queue
        POP     CX                      ;volume change
        JC      no_space
        INC     CX                      ;check for default volume
        JZ      def_volume              ;brif default volume (FFFF)
        DEC     CX                      ;volume
        CMP     CX,15D                  ;is volume greater than 15?
        JA      vol_error               ;brif invalid volume
        JMP     SHORT good_vol          ;
def_volume:
        MOV     CX,08H                  ;default volume is 8
good_vol:
        ;
        ; now convert the volume into an attenuation that the
        ; hardware can understand (PC Jr only)
        ;
        ;       [AH] = 0 based voice
        ;       [CX] = volume 0..15
        ;       [DS:BX] points to queue contol block
        ;       [ES:0] points to queue
        ;
        ; for music voices:
        ;
        ;       attenuation = 1cc1vvvv  where vvvv = 15-volume
        ;                                          = 0000  0 dB
        ;                                          = 0001  2 dB
        ;                                          = 0010  4 dB
        ;                                          = 0011  6 dB
        ;                                          = 0100  8 dB
        ;                                          = 0101 10 dB
        ;                                          = 0110 12 dB
        ;                                          = 0111 14 dB
        ;                                          = 1000 16 dB
        ;                                          = 1001 18 dB
        ;                                          = 1010 20 dB
        ;                                          = 1011 22 dB
        ;                                          = 1100 24 dB
        ;                                          = 1101 26 dB
        ;                                          = 1110 28 dB
        ;                                          = 1111  off
        ;
        ;                                       cc = 00 for first music voice
        ;                                       cc = 01 for second music voice
        ;                                       cc = 10 for third music voice
        ;
        ; for noise channel:
        ;
        ;       attenuation = 1111vvvv  where vvvv = 15-volume as described above
        ;
        ;       [AH] = 0 based voice
        ;       [CX] = volume 0..15
        ;       [DS:BX] points to queue contol block
        ;       [ES:0] points to queue
        ;
        PUSH    BX
        NEG     CL
        ADD     CL,15                   ;calculate 15-volume
        MOV     AL,CL                   ;[AL] = attenuation
        CMP     AH,3                    ;NOISE channel ?
        JE      noise_att               ;brif so
        MOV     BH,AH                   ;0 based voice 0..2
        MOV     CL,5                    ;shift 5 left
        SHL     BH,CL
        AND     BH,10010000B            ;set the MSB and the attenuation bit
        OR      AL,BH                   ;and combine with attenuation
        JMP     SHORT que_vol           ;
noise_att:
        OR      AL,0F0H                 ;adjust noise attenuation
que_vol:
        POP     BX                      ;pointer to queue control block
        ;
        ; now put the information into the queue
        ;
        ; [AL] = attenuation
        ; [AH] = voice
        ; [BX] = address of appropriate music queue control block
        ; [ES] = points to queue segment base
        ;
        PUSH    AX                      ;save the attenuation
        MOV     AL,type_vol             ;queue the volume change
        CALL    queue_in                ;
        POP     AX
        CALL    queue_in                ;and the new volume
        MOV     [BX].quevol,AL          ;save the default volume in the queue control block
ignore_vol:
        CLC                             ;return no errors

no_space:                               ;has PSW.C set and [AL] = err_qfull

vol_return:
        POP     CX
        POP     BX
        RET

        ;
        ; Error conditions:
        ;
vol_error:
        MOV     AL,err_vol              ;invalid volume
        STC                             ;PSW.C set indicates error
        JMP     SHORT vol_return

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           q_envelope - queue envelope change
;
;       Purpose:        This routine queues an envelope change in the
;                       appropriate music queue.
;
;       Entry:          [AH] = voice (0 based)
;                       [BX] = shape
;                       [CX] = duration / period
;
;       Exit:           PSW.C set indicates one of the following errors:
;                       [AL] = err_qfull   - No room in the queue or
;                       [AL] = err_eshape  - Unsupported envelope shape or
;                       [AL] = err_dur     - Unsupported envelope duration or
;
;                       else
;
;                       PSW.C reset
;
;       Modified:       none
;
;-----------------------------------------------------------------------

        PUBLIC  q_envelope

q_envelope:
        CLC                             ;IBM has no support for envelopes
        RET                             ; therefore ignore them

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           queue_noise - queue in noise note
;
;       Purpose:        This routine sets up the TI chip to make noise.
;
;       Entry:          [CX] = Source
;                       [DX] = Duration
;
;       Exit:           PSW.C set if there's an error and one of:
;                       [AL] = err_qfull if no room in the
;                                        queue for this request
;                       [AL] = err_noise if unsupported noise
;                                        source
;
;                       else
;
;                       PSW.C reset
;
;       Modified:       none
;
;-----------------------------------------------------------------------

        PUBLIC  queue_noise

queue_noise:

        MOV     AX,3                    ;noise channel is #3
        CALL    get_q_seg               ;[DS:BX] points to queue contol block
                                        ;[ES:0] points to queue
        CMP     CX,7                    ;is source > 7 ?
        JA      noise_error
        CALL    check_space             ;room in queue ?
        JC      space_out
        XCHG    CX,DX                   ;CX = duration, DX = source
        CALL    check_duration
        MOV     AL,type_noise           ;queue in a type noise byte
        CALL    queue_in
        MOV     AL,DL                   ;queue in source
        CALL    queue_in
        MOV     AX,CX                   ;noise note duration
        CALL    queue_in                ;LSB of note duration
        MOV     AL,AH                   ;MSB of note duration
        CALL    queue_in
        AND     vceoff,0F7H             ;turn on NOISE
        INC     [BX].quenote            ;update note counter
        CLC                             ;indicate no errors

space_out:                              ;has PSW.C set and AL = err_qfull

noise_exit:
        RET

noise_error:
        MOV     AL,err_noise
        STC                             ;indicate error condition
        JMP     SHORT noise_exit

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           check_duration - check duration of note
;
;       Purpose:        This routine adjusts the duration [DX] so that
;                       it matches the chip in question, i.e. either the
;                       TI chip or the 8253 timer chip. If it is the
;                       8253 timer it multiplies the given duration by
;                       1.5 and if it is the TI chip it multiplies
;                       the given duration by 32. In either case if there
;                       is an overflow, check_duration sets [DX] to 65535.
;
;       Entry:          [CX] = duration (1 = 2.5 millisecs)
;
;       Exit:           [CX] = [CX] * 1.5 if timer2 in action
;                              [CX] * 32 if TI chip in action   ????????
;
;       Modified:       none
;
;-----------------------------------------------------------------------

        PUBLIC  check_duration

check_duration:
        PUSH    AX
        MOV     AX,CX                   ;copy duration for adjustment
        SHR     AX,1                    ;divide by 2
        ADD     CX,AX                   ;duration = 1.5 * original duration
        JNB     durok                   ;Brif no overflow
        MOV     CX,65535D               ;else use maximum duration
durok:
        POP     AX
        RET

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           check_freq - check the frequency value
;
;       Purpose:        This routine checks to see if the frequency is
;                       in the valid range ( 37 to 32767 ).
;                       If it finds the frequency to be invalid it returns
;                       with PSW.C set and [AL] set to the error code 3.
;
;       Entry:          [AX] = frequency
;
;       Exit:           PSW.C set if invalid frequency and
;                       [AL] = err_freq
;
;                       else
;
;                       PSW.C reset and
;                       [AX] = frequency (unchanged)
;
;       Modified:       none
;
;-----------------------------------------------------------------------

        PUBLIC  check_freq

check_freq:
        CMP     AX,37D                  ;check for valid frequency
        JB      frqerr                  ;if invalid return
        CMP     AX,32767D               ;error code
        JNA     frqret                  ;else return with carry clear
frqerr:
        MOV     AL,err_freq             ;[AL] = error code for bad frequency
        STC                             ;set carry to indicate error
        RET
frqret:
        CLC                             ;clear carry to indicate no error
        RET

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           freq_to_tics
;
;       Purpose:
;                       This routine converts the frequency in Hz to clock
;                       tics required by the 8253 timer chip or the TI sound
;                       chip as the case may be. If sound is directed to the
;                       8253 timer then the tics are calculated using the
;                       formula :
;
;                               tics = clock frequency / given frequency
;
;                       If sound is directed to the TI chip then the tics are
;                       calculated using :
;
;                               tics = clock frequency / ( 32 * given frequency )
;
;                       In the former case the clock frequency is 1.19MHz and
;                       in the latter case it is 3.579MHz
;
;       Entry:          [AX] = given frequency
;                       [cur_voice_id] = Voice Id (0 based)
;
;       Exit:           [CX] = tics
;
;       Modified:       none
;
;-----------------------------------------------------------------------

        PUBLIC  freq_to_tics          ;needs optimization ????

freq_to_tics:

        PUSH    DX                      ;save [DX]
        PUSH    BX                      ;save [BX]
        PUSH    CX

        MOV     CX,AX                   ;given freq
        MOV     AH,cur_voice_id         ;given voice

        ; PUSH    AX
        MOV     BH,AH                   ;save voice id in BH
        CMP     $tichip,1               ;is TI chip active?
        JE      titics                  ;Brif to get tics for TI chip
        MOV     DX,12H                  ;clock freq =
        MOV     AX,34DCH                ; 1.193180 MHz
        DIV     CX                      ;count = clock / frequency
        MOV     CX,AX                   ;clock tics to [CX]
        JMP     SHORT ticret
titics:
        MOV     DX,01H                  ;[DX:AX] = 3.579 MHz/32
        MOV     AX,0B4E3H               ;remember - for TICHIP TICS = CLK freq/freq*32
        DIV     CX                      ;Quotient in AX
        CMP     AX,1023                 ;within TI chip's range?
        JBE     ticsok                  ;Brif to formatting frequency
        MOV     AX,1023                 ;else store largest number
ticsok:
        PUSH    AX                      ;save tics
        AND     AL,15D                  ;retain least 4 significant bits
        SHL     BH,1                    ;convert Voice to 0,2,4
        MOV     CL,4                    ;shift count to [CL]
        SHL     BH,CL                   ;transfer lower nibble
                                        ;to higher nibble
        AND     BH,70H                  ;retain only bits 5,6,7
        OR      BH,80H                  ;set bit 8, bcos it is first byte
        OR      AL,BH                   ;[AL] = attenuation
        MOV     BH,AL                   ;save it in [BH]
        POP     AX                      ;get frequency
        AND     AX,3F0H                 ;retain bits 5,6,7,8,9,10
        SHR     AX,CL                   ;shift right 4 times to
                                        ;make these bits 0,1,2,3,4,5
        MOV     CH,AL
        AND     CH,07FH                 ;reset bit 8, bcos it is second byte
        MOV     CL,BH                   ;store lower byte in [CL]
ticret:
       ; POP     AX
        MOV     AX,CX                   ;new

        POP     CX
        POP     BX                      ;restore [BX]
        POP     DX                      ;restore [DX]
        RET

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           mdd_event - return pointer to event flags
;
;       Purpose:        This routine returns a short pointer to the
;                       data segment location of the threshold event
;                       word flags. These flags may be monitored by
;                       the c interface or by the user to determine
;                       the status of individual queues.
;
;       Entry:          none
;
;       Exit:           [AX] = pointer to threshold event flags
;
;       Modified:       none
;
;-----------------------------------------------------------------------

        PUBLIC  mdd_event

mdd_event:

        MOV     AX,OFFSET tew_flags     ;get the threshold event word location
        RET

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           mdd_flags - return flags and clear flags
;
;       Purpose:        This routine returns the threshold flags
;                       and then clears these flags.
;
;       Entry:          none
;
;       Exit:           [BX] = threshold event flags
;
;       Modified:       tew_flags
;
;-----------------------------------------------------------------------

        PUBLIC  mdd_flags

mdd_flags:
        CMP     tt_installed,2          ;[new]check if all voices off
        JNE     get_flags               ;[new]
        CALL    stop_music              ;[new]if all voices off - stop music
get_flags:                              ;[new]
        MOV     BX,tew_flags            ;return this to the user
        MOV     tew_flags,0             ;and clear the event flags
        RET

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           notes_in_queue
;
;       Purpose:        This routine returns the number of notes remaining
;                       in a queue.
;
;       Entry:          [AH] = voice (0 based)
;
;       Exit:           [BX] = number of notes in this queue
;
;       Modified:       none
;
;-----------------------------------------------------------------------

        PUBLIC  notes_in_queue

notes_in_queue:
        XOR     BX,BX
        CMP     tt_installed,0          ;{0} - if tic_toc is not installed, then
        JE      no_notes                ; we should report 0 notes
        CMP     tt_installed,2          ;{2} - if tic_toc installed but no longer
        JNE     get_notes               ; needed, then we should stop any
        CALL    stop_music              ; music, and de-install tic_toc
        JMP     SHORT no_notes          ; and report 0 notes
get_notes:                              ;{1} - if tic_toc is installed, then we
                                        ; should report the number of notes
                                        ; in the queue or 1, which ever is
                                        ; higher
        CALL    get_q_seg               ;[DS:BX] points to queue control block
                                        ;[ES:0] points to queue
        MOV     SI,[BX].quenote         ;get the # of remaining notes
        MOV     BX,SI                   ;return # of notes in queue
        OR      BX,BX                   ;any notes left
        JNZ     notes_left
        INC     BX                      ;else add one if tic_toc still installed
no_notes:
notes_left:
        RET

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           set_threshold
;
;       Purpose:        This routine sets the threshold level for event
;                       detection. An event occurs when a music queue
;                       shrinks from n to n-1 notes where n is specified
;                       as the entry condition to this routine.
;
;       Entry:          [AH] = voice (0 based)
;                       [BX] = the number of notes specified for the threshold
;
;       Exit:           none
;
;       Modified:       none
;
;-----------------------------------------------------------------------

        PUBLIC  set_threshold

set_threshold:
        MOV     CX,BX                   ;save threshold as specified by user
        CALL    get_q_seg               ;[DS:BX] points to queue control block
                                        ;[ES:0] points to queue
        MOV     [BX].qthresh,CX         ;save the threshold for this voice
        RET

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           mdd_term - Machine Specific Termination.
;
;       Purpose:        This routine is called when ever the Music
;                       Device Driver is closed. Machine specific
;                       termination must occur at this time.
;
;                       In the IBM specific case the ROM timer tick
;                       interrupt service routine is re-installed.
;
;       Entry:          none
;
;       Exit:           none
;
;       Modified:       none
;
;-----------------------------------------------------------------------

        PUBLIC  mdd_term

mdd_term:
        PUSH    ES
        MOV     $first,0                ;turn off speaker in $sndoff
        disable                         ;
        CALL    $sndoff                 ;turn off sound
;
; Smart enough to handle the Junior ?????
;
;do a SOUND OFF/BEEP ON before terminating
        MOV     AX,00FCH                ;set [AX] for sound OFF
        CALL    $sonoff                 ;do SOUND OFF
        DEC     AH
        INC     AL                      ;set [AX] for BEEP ON
        CALL    $bonoff                 ;do BEEP ON
osext:
        disable
        POP     ES                      ;restore [ES]
        pause                           ;make sure instruction fetch has occurred
        IN      AL,mskreg               ;get IMR into [AL]
        AND     AL,0FEH                 ;unmask timer interrupt
        pause                           ;make sure instruction fetch has occurred
        OUT     mskreg,AL               ;write mask to IMR
        enable
        MOV     now_playing,0           ;no longer playing
        RET

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           $sonoff
;
;       Purpose:        This routine supports the SOUND ON/OFF statements.
;                       If its a SOUND ON call it sets TICHIP to 1 and
;                       selects the external speaker by doing an INT 80H
;                       with [AL] set equal to 3. SOUND OFF sets TICHIP
;                       back to 0, disables the external speaker and
;                       enables the internal speaker.
;
;       Entry:          [AL] = FC
;                       [AH] = 0 for SOUND OFF
;                              FF for SOUND ON
;                FC : begin
;                       if ([AH] = 0) then
;                           SNDOFF
;                       else
;                           if ([AH] = 0FFH) then
;                               SNDON
;                           else
;                               set carry /* error */
;                     end ;
;
;       Exit:           none
;
;       Modified:       none
;
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
;
;       ** Support SOUND ON/OFF, BEEP ON/OFF statements:
;          The PC Junior machine has two speakers, one internal and the other
;          external. Depending on the SOUND ON/OFF and the BEEP ON/OFF
;          statements these speakers get activated as follows:
;
;               SOUND ON....sound will come from the external speaker
;                           and the internal speaker is disabled. If
;                           SOUND ON is used, then BEEP ON/OFF has
;                           no effect.
;
;               SOUND OFF, BEEP ON....sends sound through both the
;                                     speakers. (default)
;
;               SOUND OFF, BEEP OFF....sends sound only through the
;                                      internal speaker.
;
;       This routine should support both the TI sound chip and the 8253
;       timer chip. The differences between the two chips is taken care
;       of in certain device dependent routines. There is one important
;       point though - volume has no significance if we are outputting
;       things to the 8253 timer.
;
;-----------------------------------------------------------------------

        PUBLIC  $sonoff

$sonoff:
        CALL    getmchnid               ;pc junior ?
        JNZ     onofer                  ;brif not - error
        OR      AH,AH                   ;is it SOUND OFF call ?
        JZ      sndoff                  ;brif so
        INC     AH                      ;is it a SOUND ON call ?
        JZ      sndon                   ;brif so
onofer:
        STC                             ;else set carry, unsupported function
        RET                             ;return
sndon:
        MOV     $tichip,1               ;TICHIP is active now
        pause
        IN      AL,speaker              ;setting PB4 to 1 disables
        OR      AL,spkren               ;speaker
        pause
        OUT     speaker,AL              ;disable speaker
        MOV     AL,3                    ;select TI sound chip
        JMP     SHORT onofxt
sndoff:
        MOV     $tichip,0               ;TICHIP is disabled now
        MOV     $first,0                ;TI chip is off so we need to
                                        ;reinitialize the speaker and timer2
        pause
        IN      AL,speaker              ;setting PB4 to zero
        AND     AL,0EFH                 ;enables speaker
        pause
        OUT     speaker,AL              ;enable speaker
        MOV     AL,beepsw               ;if BEEP was ON restore BEEP ON
onofxt:
        MOV     AH,80H                  ;set sound mux
        INT     26                      ;set multiplexer
        RET

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           $bonoff
;
;       Purpose:        This routine supports the BEEP ON/OFF statements.
;
;       Entry:          [AL] = FD
;                       [AH] = 0 for BEEP OFF
;                              FF for BEEP ON
;                FD : begin
;                       if ([AH] = 0) then
;                           BPOFF
;                       else
;                          if ([AH] = 0FFH) then
;                              BPON
;                          else
;                               set carry /* error */
;                     end ;
;
;       Exit:           none
;
;       Modified:       none
;
;-----------------------------------------------------------------------

        PUBLIC  $bonoff

$bonoff:
        CALL    getmchnid               ;PC Junior ?
        JNZ     bperor                  ;Brif not - error
        OR      AH,AH                   ;is it a BEEP OFF call ?
        JZ      bpoff                   ;Brif so
        INC     AH                      ;is it a BEEP ON call ?
        JZ      bpon                    ;Brif so
bperor:
        STC                             ;else STC (unsupported function)
        RET
bpoff:
        MOV     AH,2                    ;set BEEP OFF
bpon:
        MOV     beepsw,AH               ;selects timer 2 or cassette as sound source
        CMP     $tichip,0               ;if SOUND is ON then BEEP ON/OFF
        JNE     bpxt                    ;has no effect
        XCHG    AL,AH                   ;[AL] = 0 or 2
        MOV     AH,80H                  ;set sound mux
        INT     26D                     ;set multiplexer
bpxt:
        RET

sEnd    CODE

        END
