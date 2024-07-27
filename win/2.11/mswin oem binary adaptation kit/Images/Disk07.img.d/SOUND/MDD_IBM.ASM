
        TITLE   MDD_IBM - IBM Multivoice Music Device Driver Specifics
?PLM = 1
?WIN = 1

.xlist
include \lib\cmacros.inc
.list

;-----------------------------------------------------------------------
;
;       IBM PC Specific Code for the Multivoice Music Device Driver
;
;       Machine Interface Module
;
;       (C) 1985 Microsoft Inc. - Last Revision March 5, 1985
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

        PAGE

;-----------------------------------------------------------------------
;
;                               Data
;
;-----------------------------------------------------------------------

sBegin  DATA

PUBLIC  beepsw
PUBLIC  clk_tics, cur_voice_id
PUBLIC  $first
PUBLIC  int_length
PUBLIC  $machid, $music
;PUBLIC  old_freq
PUBLIC  qcblock
PUBLIC  rom_tt
PUBLIC  $sync, $synoff, $sndtim
PUBLIC  tew_flags, $tichip, tt_installed
PUBLIC  vceoff, $voice

beepsw  DB      0       ;used for BEEP ON/OFF (default is BEEP ON)
clk_tics DB     0       ;Clock tic modulo counter
cur_voice_id DB 0       ;current voice id - set by get_q_seg
$first  DB      0       ;this byte is used in $startnm and $sndoff
                        ;If it is set, then we do not mess with the speaker.
int_length DW 3 DUP (0) ;Length of interstice (non 0 indicates we're playing notes)
                        ;one word for each music voice
$machid DB      ?       ;machine ID. This byte is set by the
                        ;routine $GWINI as follows:
                        ;  0FFH if PC
                        ;  0FEH if PC XT
                        ;  0FDH if PC Junior
                        ;  0FCH if PC AT
$music  DB      0       ;music currently active or not byte
;old_freq DW   3 DUP (?) ;these three words are used to
                        ;save the frequency of stacatto
                        ;and normal notes to be used
                        ;while queueing the pause part of the
                        ;note if SOUND ON is true.
qcblock que_ctrl_blk <> ;Voice 1 (Music)
        que_ctrl_blk <> ;Jr Voice 2 (Music)
        que_ctrl_blk <> ;Jr Voice 3 (Music)
        que_ctrl_blk <> ;Jr Voice 4 (Noise)
rom_tt  DD      ?       ;Save area for vector to ROM timer tick interrupt routine
$sndtim DW      0,0,0,0 ;four words reserved for SNDTIM i
$sync   DB      0       ;# of voices waiting for synchronization
$synoff DB      0,0,0   ;These bytes will be set to 1 if the
                        ;corresponding voices are suspended
                        ;because of encountering a SYNC byte
tew_flags DW    0       ;Threshold Event Word Flags
                        ; bit 0 = 0 indicates music voice 1 queue above threshold
                        ; bit 0 = 1 indicates music voice 1 queue below threshold
                        ; bit 1 = 0 indicates music voice 2 queue above threshold
                        ; bit 1 = 1 indicates music voice 2 queue below threshold
                        ; bit 2 = 0 indicates music voice 3 queue above threshold
                        ; bit 2 = 1 indicates music voice 3 queue below threshold
                        ; bit 3 = 0 indicates noise voice 4 queue above threshold
                        ; bit 3 = 1 indicates noise voice 4 queue below threshold
$tichip DB      0       ;TI chip active or not flag
                        ; 0 indicates SOUND OFF executed or no multivoice
                        ; 1 indicates SOUND ON executed or multivoice
tt_installed DB 0       ;Tic_Toc Interrupt handler active or not flag
                        ; 0 indicates tic_toc not installed
                        ; 1 indicates tic_toc installed
                        ; 2 indicates tic_toc installed but may be de-installed
vceoff  DB      0       ;keeps track of # of voices off at any time
                        ; 00000000 all voices off and ready to start again
                        ; set bit - voice off
                        ; cleared bit - voice on
$voice  DW      1,1,1,1 ;four words reserved for VOICE i
                        ;voice i is said to be active iff
                        ;VOICE i = 1, else voice i is suspended

PUBLIC  mdd_dend
mdd_dend        LABEL   WORD    ;marks end of music device driver data

sEnd    DATA

sBegin  CODE

assumes CS,CODE
assumes DS,DATA


;
;       Code Segment Variables
;

        PUBLIC   mdd_dseg
mdd_dseg DW     ?       ;address of Music Device Driver data segment

        PUBLIC  rom_tod
rom_tod DD      ?       ;vector to ROM time of day interrupt routine

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

pause   MACRO                           ;macro to insure that an instruction
        JMP     $+2                     ;fetch occurs between IN and/or OUT
        ENDM                            ;instructions on the PC AT machine

savint  MACRO   savloc,intvec           ;
        MOV     AL,intvec/4             ;[AL] = interrupt
        callos  35H                     ;function 35h Get Interrupt Vector
                                        ;returns es:bx pointer to interrupt routine
        MOV     WORD PTR savloc,BX      ;savloc = offset
        MOV     WORD PTR savloc+2,ES    ;savloc+2 = segment
        ENDM                            ;

setvec  MACRO   interrupt,offset_adr    ;Set interrupt vector function call
        MOV     AL,interrupt            ;[AL] interrupt number
        MOV     DX,OFFSET offset_adr    ;[DS:DX] interrupt handling routine
        callos  25h                     ;Set Vector
        ENDM                            ;

rstvec  MACRO   interrupt,save_adr      ;Restore interrupt from saved
        MOV     AL,interrupt            ;[AL] interrupt number
        LDS     DX,DWORD PTR save_adr   ;[DS:DX] interrupt handling routine
        callos  25h                     ;Set Vector
        ENDM                            ;

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
;                       Entry:          [AL] = f_trap
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
;                       Entry:          [AH] = voice (0 based)
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
        ;
        ; Determine what machine we're currently running on and store
        ; this for future reference.
        ;
        ;       Macine   ID     Voices
        ;
        ;       PC      0FFH       1
        ;       PC XT   0FEH       1
        ;       Junior  0FDH       3
        ;       PC AT   0FCH       1
        ;
        ; The machine Id is obtained from the location 0FFFF:0E.
        ;
        PUSH    ES
        MOV     BX,0FFFFH               ;segment 0FFFFH
        MOV     ES,BX                   ;address using the ES register
        MOV     AL,ES:[000EH]           ;get the machine ID in [AL]
        MOV     $machid,AL              ;store it in $MACHID
        POP     ES
        XOR     AH,AH
        CALL    getmchnid               ;is this a junior?
        JZ      ini_jr                  ; brif so
        MOV     AL,0FFH                 ; else must be single voice
        DEC     AH
ini_jr: INC     AH                      ;number of noise voices (1 for Jr, else 0)
        NEG     AL                      ;get us 3 for Junior, 1 for others
        PUSH    AX                      ;save number of voices for return
        ;
        ; Get the interrupt vector for the ROM time of day
        ; interrupt routine (interrupt 8) and save this for
        ; future use.
        ;
        savint  CS:rom_tod,tod_int*4    ;save the old time of day interrupt
        ;
        ; Save the data segment for future use.
        ;
        MOV     CS:mdd_dseg,DS          ;Save DS of Music Device Driver
        CALL    $sndoff                 ;turn off sound
                                        ;which returns CLC to indicate no errors
        POP     AX                      ;get number of music/noise voices back
        enable
        RET

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           getmchnid - Get the Machine ID (is it a Junior?)
;
;       Purpose:        This routine is used to check if the the current
;                       machine is the PC Jr or not. It returns with PSW.Z
;                       set if the current machine is the PC Jr else PSW.Z
;                       is reset.
;
;       Entry:          none
;
;       Exit:           PSW.Z set if currently on PC JR.
;
;       Modified:       none
;
;-----------------------------------------------------------------------

        PUBLIC  getmchnid

getmchnid:

        CMP     $machid,0FDH            ;PC Jr ?
        RET                             ;PSW.Z set if its PC Jr.

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
;       Name:           ini_que - Music/Noise Queue Initialization
;
;       Purpose:        This routine is called whenever one of the
;                       queues is no longer required and can be
;                       initialized to the same condition as mdd_iniq.
;
;                       The routine is functionally the same as
;                       mdd_iniq except that size and location do
;                       not change.
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
;                                  queseg:0---+-+-+-->| memory |
;                                  qthresh = p        +--------+
;
;                       Note:   This routine must not alter the threshold
;                               event detection value for this queue.
;
;       Entry:          [AX] = voice (0 based)
;
;       Exit:           none
;
;       Modified:       none
;
;-----------------------------------------------------------------------

        PUBLIC  ini_que

ini_que:

        PUSH    AX                      ;0 based voice
        PUSH    DX
        PUSH    SI
        MOV     DL,qcbsize              ;size of queue control block
        MUL     DL                      ;offset into queue control block
        MOV     DX,OFFSET qcblock       ;
        ADD     AX,DX                   ;address of this voice's q control block
        MOV     SI,AX                   ;index into queue control block
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
;       Name:           get_q_seg - get the queue segment into ES
;
;       Purpose:        This routine gets the segment location of
;                       the queue for the given voice, and the address
;                       of the queue control block for this voice.
;
;       Entry:          [AH] = Voice Id (0 based)
;
;       Exit:           [BX] = qcblock + [AH] * qcbsize
;                       [ES] = [BX].queseg
;                       [DS:BX] points to queue control block
;                       [ES:0000] points to this queue
;                       cur_voice_id = 0 based voice
;
;       Modified:       none
;
;-----------------------------------------------------------------------

        PUBLIC  get_q_seg

get_q_seg:

        PUSH    AX
        MOV     cur_voice_id,AH         ;save the current voice number (0 based)
        MOV     AL,AH                   ;0 based voice
        MOV     BL,qcbsize
        MUL     BL                      ;get offset for this voice
        MOV     BX,OFFSET qcblock       ;queue control block
        ADD     BX,AX                   ;[BX] points to queue control info for this voice
        MOV     AX,[BX].queseg          ;get the queue segment location
        MOV     ES,AX                   ; and set ES to it
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
;       Name:           queue_out - remove element from the queue
;
;       Purpose:        This routine supports get_sound by dequeueing one
;                       byte from the appropriate music queue and
;                       updating the associated variables.
;
;                       The queues are circular.
;
;                       Notes are removed from the queue as follows:
;
;                          queue        queue control           queue
;                         before:          block:               after:
;
;                       +--------+                            +--------+
;                       |  note  |<--------quetop------------>|  note  | high
;                       |  note  |     n = quenote = n - 1    |  note  | memory
;                       |  note  |    +----queget------------>|  note  |
;                       |  note  |<---+                       |        |
;                       |        |                            |        |
;                       |        |     m = quenum = m - 7     |        |
;                       |        |<--------queput------------>|        |
;                       |  note  |                            |  note  |
;                       |  note  |                            |  note  | low
;                       |  note  |<-------queseg:0----------->|  note  | memory
;                       +--------+        qthresh = p         +--------+
;
;       Entry:          [BX] = Adrress of the appropriate music queue control block
;                       [ES] = segment of queue
;                       [ES:0000] pointer to queue
;                       [DS:BX] pointer to queue control block
;
;       Exit:           [AL] = dequeued byte
;
;       Modified:       quenum, queget, queput
;
;-----------------------------------------------------------------------

        PUBLIC  queue_out

queue_out:

        PUSH    SI                      ;save SI
        MOV     SI,[BX].queget          ;fetch get PTR
        MOV     AL,ES:[SI]              ;get the byte
        INC     SI
        CMP     SI,[BX].quetop          ;wraparound?
        JNZ     nwrdqu                  ;Brif not
        MOV     SI,0                    ;get queue bottom PTR
nwrdqu:
        MOV     [BX].queget,SI          ;store new get PTR
        DEC     [BX].quenum             ;dec # of bytes in queue
        POP     SI                      ;restore SI
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
;       Name:           tic_toc
;
;       Purpose:        The timer interrupt, vectors here BEFORE updating the
;                       time of the day. This routine helps support the
;                       Music routines. Tic_toc keeps decrementing the duration
;                       count until it becomes zero at which point it will call
;                       next_sound to get the next sound or turn off the speaker.
;
;       2. TIC_TOC.... The TIMER interrupt vectors here before
;                       updating the time of the day. Normal
;                       frequency of the timer interrupt is 18.2
;                       times per second. This frequency is changed
;                       so that it interrupts at 572.4 times per
;                       second (exactly 32 times faster). This increase
;                       in speed gives better music performance. The
;                       basic philosophy in handling music is described
;                       below :
;
;               Music is composed of notes and a note consists of:
;               1. Frequency
;               2. Duration
;               3. Volume
;                       The frequency is loaded into the 8253 timer
;               chip or the TI chip as the case may be. The duration
;               is saved in some memory location and each time the
;               timer interrupts, we decrement the duration by 1 and
;               when the duration becomes zero we are all done. It is
;               important to note that the 8253 operates in the mode
;               wherein it reloads itself each time it counts down to
;               zero. In case of the PC Junior machine which supports the
;               TICHIP we have another parameter namely Volume which
;               gets loaded into the TICHIP at the same time as
;               frequency. Handling the TICHIP is rather tricky and
;               the PC Junior technical reference manual discusses it in
;               detail. Another good reference for the TI SOUND CHIP
;               is the "BYTE MAGAZINE, JULY 1982".
;               We have to remember one important thing, since we
;               change the frequency of the TIMER interrupt whenever
;               music is playing, we should see to it that the actual
;               timer interrupt (one which updates the time of the day)
;               gets called once every 32 times our timer interrupt
;               gets called. The figure 32 comes from the fact that
;               the TIMER is 32 times as fast as what it used to be.
;               Also when music stops playing we SHOULD change the
;               timer frequency back to its original rate.
;               The figure below illustrates how the TIMER interrupt
;               ( INT 8H ) is handled :
;
;                 Timer INT
;                  vectors
;                ------------        IF MUSIC      --------------
;               |     -------|------------------->| TIC_TOC      |
;                ------------    |     ACTIVE     |    Interrupt |
;               |    CS      |   |                |   service    |
;                ------------    |                |   routine for|
;                                |                |   handling   |
;                                |                |   music      |
;                                |                |     _________|______
;                                | ELSE IF         --------------       |
;                                | MUSIC NOT                            |
;                                | ACTIVE                               |
;                                |                                      |
;                                |                 ------------         |
;                                 --------------->| ROM Timer  |        |
;                                                 | interrupt  |<-------
;                                                 | service    |
;                                                 | routine    |
;                                                 | updates    |
;                                                 | time of    |
;                                                 | day.       |
;                                                 |            |
;                                                 | (F000:FEA5)|
;                                                  ------------
;
;               The routine TIC_TOC calls on the following routines:
;
;               1. next_sound.... This looks to see if any more entries
;                             are present in the sound queue and if so
;                             calls get_sound else shuts off voices.
;               2. get_sound.... This just pulls out an entry from the
;                             appropriate sound queue. It is evident
;                             that the notes are stored in a QUEUE.
;               In the case of MULTIVOICE there are 4 queues, 3 for
;               the three channels and one for the NOISE channel.
;               3. event_trap.... This routine checks to see if any play
;                             event occured and if so sets the plaflg flag.
;
;       Entry:          none
;
;       Exit:           none
;
;       Modified:       none
;
;-----------------------------------------------------------------------

        PUBLIC  tic_toc

;
; Note: Don't turn off tic_toc until a few ticks after the last note
;       finished playing ??????
;

tic_toc:
        PUSH    AX
        PUSH    BX
        PUSH    CX
        PUSH    DX
        PUSH    SI
        PUSH    DI
        PUSH    DS
        PUSH    ES
        CMP     tt_installed,2          ;[new]is this interrupt vector required?
        JZ      clk_tic                 ;[new]brif so - no need to do anything
        MOV     DS,CS:mdd_dseg          ;get music device driver's data seg
        MOV     AX,DS                   ;make ES same as DS!!!!!!!
        MOV     ES,AX                   ; necessary ?????????????
        XOR     AX,AX
        MOV     SI,AX                   ;[SI] = voice Id*2
        MOV     CX,num_queues           ;[CX] used as counter
        CMP     $tichip,1               ;is the TI chip active?
        JE      tic0                    ;Brif so to process sound for TI chip
        CMP     $sndtim[SI],AX          ;has SND_TIM zeroed out?
        JZ      nxtone                  ;Brif so to get next sound
        DEC     $sndtim[SI]             ;SND_TIM - 1
        JNZ     clk_tic                 ;
nxtone:
        CALL    next_sound              ;get next sound
        JMP     SHORT clk_tic
tic0:
        CMP     $voice[SI],AX           ;is voice i active( 'i' in [CL])?
        JZ      tic2                    ;brif not
        CMP     $sndtim[SI],AX          ;has SND_TIM i zeroed out?
        JZ      tic1                    ;Brif so to get next sound
        DEC     $sndtim[SI]             ;Decrement SND_TIM i
        JNZ     tic2                    ;Brif nonzero to check next channel
tic1:
        CALL    next_sound
tic2:
        INC     SI                      ;
        INC     SI                      ;make SI point to next channel
        LOOP    tic0                    ;check the other channels
clk_tic:
        DEC     clk_tics                ;clock tick -1
        AND     clk_tics,1FH            ;since 32 times as fast
        POP     ES
        POP     DS
        POP     DI
        POP     SI
        POP     DX
        POP     CX
        POP     BX
        JNZ     clktix                  ;do ROM clock INT once every 32 times
        POP     AX
IFE     debug
        JMP     CS:rom_tod              ;Jump to ROM clock INT service routine
                                        ; which will perform an IRET
ELSE
        JMP     clkret
ENDIF                                   ;IFE debug
clktix:
        MOV     AL,eoi                  ;send End-of-Interrupt
        pause                           ;make sure instruction fetch has occurred
        OUT     intao,AL                ;to 8259
                                        ; - allows pending interrupts to be processed
        POP     AX
clkret:
IFE     debug
        IRET
ELSE
        RET
ENDIF                                   ;IF debug

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           next_sound
;
;       Purpose:        This routine supports the interrupt routine tic_toc. It
;                       either gets the next sound from the appropriate sound
;                       queue by calling get_sound or else it turns off the voice
;                       by calling endvce.
;
;       Entry:          [SI] = voice id*2
;
;       Exit:           none
;
;       Modified:       BX,DX
;
;-----------------------------------------------------------------------

        PUBLIC  next_sound

next_sound:
        PUSH    CX                      ;next_sound trashes [CX]
        PUSH    AX                      ;next_sound trashes [AX]
        MOV     AX,SI                   ;get the voice id * 2
        SHR     AX,1                    ; and divide by two to get voice
        MOV     AH,AL                   ;voice in [AH]
        CALL    get_q_seg               ;[DS:BX] points to queue contol block
                                        ;[ES:0] points to queue
        CMP     [BX].quenum,0           ;is queue empty?
        JZ      endvce                  ;Brif to turn off the voice
        CALL    get_sound               ;[DX] = Duration, [CX] = Freq
                                        ;[AL] = Attenuation
        JC      nxtret                  ;return if sync byte encountered
;
;       Output frequency and attenuation
;       into the TI sound chip or the 8253
;       timer as the case may be.
;
        CMP     $tichip,1               ;is the TI chip active?
        JE      to_ti                   ;Brif so
        MOV     AL,CL                   ;LSB of freq
        OUT     timer2,AL               ;no need to pause bcos of JE TO_TI
        MOV     AL,CH
        pause                           ;make sure instruction fetch has occurred
        OUT     timer2,AL               ;MSB of frequency
        JMP     SHORT nxsnd1
to_ti:
        PUSH    AX                      ;save attenuation
        MOV     AL,CL                   ;LSB of frequency
        OUT     tiaddr,AL
        MOV     AL,CH                   ;MSB of frequency
        OUT     tiaddr,AL
        POP     AX                      ;unsave attenuation
        OUT     tiaddr,AL               ;output attenuation
nxsnd1:
        MOV     $sndtim[SI],DX          ;SND_TIM[i] = Duration
        JMP     SHORT nxtret
endvce:
        CMP     $tichip,1               ;is the TI chip active?
        JNE     turnoff                 ;Brif not to turn off sound
        MOV     $voice[SI],0            ;else turn off voice i
        MOV     AX,SI                   ;[AL] = 0,2,4,6
        MOV     AH,AL                   ;[AH] = 0,2,4,6
        ADD     AL,1                    ;[AL] = 1,3,5,7
        MOV     CL,4                    ;shiftcount = 4
        SHL     AL,CL                   ;[AL] = 00010000,00110000,01010000,01110000
        OR      AL,8FH                  ;[AL] = 1rrrvvvv  (full attenuation)
        OUT     tiaddr,AL               ;TURN OFF THE VOICE
        SHR     AH,1                    ;[AH] = 0,1,2,3
        MOV     CL,AH                   ;restore voice ID in [CL]
        MOV     AH,1                    ;setting the mask bit in [AH]
        ROL     AH,CL                   ;[AH] = 0000MMMM
        OR      vceoff,AH               ;turn off the appropriate channel
        CMP     vceoff,0FH              ;all channels off ?
        JE      turnoff                 ;Brif so to turnoff
        JMP     SHORT nxtret            ;else return
turnoff:
        MOV     vceoff,0                ;reset VCEOFF
        MOV     $sync,0                 ;no voices waiting for the SYNC byte
;[new]        CALL    stop_music
        MOV     tt_installed,2          ;tic_toc may now be de-installed
nxtret:
        POP     AX                      ;restore [AX]
        POP     CX                      ;restore [CX]
        RET

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           get_sound
;
;       Purpose:        This routine supports next_sound.
;
;                       It also supports play event trapping by setting the play
;                       event flag whenever the number of notes in the usic
;                       background queue goes below the number specified in the
;                       ON PLAY(n)... statement, provided play trapping is enabled.
;                       get_sound makes several calls to queue_out to get notes out of the
;                       music queue. After the very first call to queue_out, get_sound
;                       checks to see if the byte is a SYNC byte and if it is then
;                       get_sound suspends that voice until it finds a SYNC byte in
;                       every other music queue.
;
;       Entry:          [DS:BX] = address of appropriate music control block
;                       [SI] = Voice Id*2
;                       [ES:0] points to queue
;
;       Exit:           PSW.C set to indicate that a SYNC byte was encountered
;                             in the music queue.
;
;                       else
;
;                       PSW.C reset
;                       [AL] = attenuation
;                       [CX] = frequency in Hz (or source if noise)
;                       [DX] = duration in clock tics
;
;       Modified:       AX,BX,CX,DX
;
;-----------------------------------------------------------------------

        PUBLIC  get_sound

get_sound:
        ;
        ; check to see if we're in the middle of a note/interstice
        ;
        MOV     DX,[SI].int_length      ;interstice length
        OR      DX,DX                   ;see if there's any interstice to play
        JNZ     get_int                 ;brif interstice - don't get next note
        ;
        ; get next note from queue
        ;
        CALL    queue_out               ;[AL] = type of queue entry
        ;
        ; [AL] = type_note   if it's a note / interstice combination
        ;      = type_sync   if it's a sync mark
        ;      = type_vol    if it's a volume change
        ;      = type_noise  if it's a noise note
        ;      = type_env    if it's an envelope change
        ;
        ; no need to check for type_env
        ;
        CMP     AL,type_note            ;is it a music note?
        JE      music_note              ;brif music
        CMP     AL,type_sync            ;is it a SYNC byte?
        JE      a_sync_mark             ;brif sync mark
        CMP     AL,type_noise           ;is it a noise note?
        JNE     vol_change              ;brif not noise
        JMP     noise_note              ;brif noise
vol_change:
        JMP     volume_change           ;must be a volume change (type_vol)

music_note:
        CALL    queue_out               ;frequency
        MOV     CL,AL                   ;LSB first
        CALL    queue_out               ;MSB second
        MOV     CH,AL
        CALL    queue_out               ;note duration
        MOV     DL,AL                   ;LSB first
        CALL    queue_out               ;MSB second
        MOV     DH,AL
        PUSH    DX                      ;note duration
        CALL    queue_out               ;interstice duration
        MOV     DL,AL                   ;LSB first
        CALL    queue_out               ;MSB second
        MOV     DH,AL
        MOV     [SI].int_length,DX      ;save the length of interstice (even if 0)
        POP     DX                      ;note duration
        ;
        ; [CX] = frequency
        ; [DX] = note duration
        ;
        OR      CX,CX                   ;0 frequency indicates no note
        JNZ     audible                 ;brfi if there's a note
        MOV     DX,[SI].int_length      ;get the interstice length
        JMP     SHORT get_int           ;handle interstice now

audible:
        MOV     AL,[BX].quevol          ;get the current volume
        JMP     SHORT got_note          ;and return to next_sound

get_int:
        ;
        ; return an interstice (or pause) note to next_sound
        ;
        ; [DX] = int_length = length of pause
        ;
        MOV     [SI].int_length,0       ;zero out this value for next time
        MOV     CX,12H                  ;default frequency (1.193180/65535)

        MOV     AL,0                    ;use with zero volume
        JMP     SHORT gets_done

a_sync_mark:
        INC     $sync                   ;one more SYNC byte obtained
        CMP     $sync,3                 ;found three SYNC bytes? - no check for four ??????
        JE      activate                ;Brif to activate the queues
        MOV     $voice[SI],0            ;else turn off the corr. voice
        SHR     SI,1                    ;[SI] = 0,1,2
        MOV     BYTE PTR $synoff[SI],1  ;WAS SUSPENDED BCOS OF A SYNC BYTE
        SHL     SI,1                    ;restore [si]
        STC                             ;PSW.C set to indicate SYNC byte
        JMP     SHORT gets_exit         ;return from routine
activate:
        MOV     $sync,0                 ;Clear the SYNC flag
        MOV     $voice,1                ;activate voice 0
        MOV     $voice[2],1             ;activate voice 1
        MOV     $voice[4],1             ;activate voice 2
        MOV     WORD PTR $synoff,0      ;no voices suspended
        MOV     BYTE PTR $synoff+2,0    ;bcos of SYNC bytes
        STC                             ;set carry to indicate all SYNC bytes
        JMP     SHORT gets_exit         ;found and return

noise_note:
        CALL    queue_out               ;noise source
        MOV     CL,AL
        XOR     CH,CH
        CALL    queue_out               ;noise note duration
        MOV     DL,AL                   ;LSB first
        CALL    queue_out               ;MSB second
        MOV     DH,AL
        JMP     SHORT got_note

volume_change:
        CALL    queue_out               ;get new volume
        MOV     [BX].quevol,AL          ;and save it
        JMP     SHORT gets_done

got_note:
        ;
        ; general queue maintenance:
        ;
        DEC     [BX].quenote            ;decrement # of notes in queue
        CALL    event_trap              ;check for play events
gets_done:
        CLC                             ;indicates not a sync byte
gets_exit:
        RET

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           event_trap - check for queue thresholding events
;
;       Purpose:        This procedure checks for the occurrence of any
;                       play events. A play event is said to occur under the
;                       following circumstances:
;
;                       1. If it is NOT multi-voice then whenever the number of
;                          notes in the BACKGROUND music queue goes below the
;                          # specified in the ON PLAY(n) statement a play event
;                          is recorded provided play trapping is enabled and we
;                          had started with more than 'n' notes in the queue.
;
;                       2. If it is multi-voice then a play event is recorded
;                          when the last voice goes from n to n-1 under exactly
;                          similar circumstances.
;
;       Entry:          [DS:BX] = address of the appropriate music control block
;                       [SI] = 0 based voice i.d. *2
;
;       Exit:           bits in tew_flags may be set
;
;       Modified:       none
;
;-----------------------------------------------------------------------

        PUBLIC  event_trap

event_trap:
        ;
        ; run this routine for every voice - keep results in tew_flags
        ;
        PUSH    AX
        PUSH    CX
        PUSH    DX
        MOV     CX,SI                   ;2 * voice id
        SHR     CL,1                    ;0 based voice
        MOV     AX,1
        SHL     AX,CL                   ;[AX] = 0000000000001 for voice 0
                                        ;[AX] = 0000000000010 for voice 1
                                        ;[AX] = 0000000000100 for voice 2
                                        ;[AX] = 0000000001000 for voice 3
        MOV     DX,[BX].qthresh         ;get the user defined threshold
        CMP     [BX].quenote,DX         ;notes in queue = threshold ?
        JNZ     no_event                ;Brif not
        OR      tew_flags,AX            ;set flag for this event
no_event:
        POP     DX
        POP     CX
        POP     AX
        RET

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           start_music - start music playing
;
;       Purpose:        This routine starts the music.
;
;                       It does the following things:
;
;                       1. Change the interrupt vector to point at our
;                          handler
;                       2. Modify timer2 to interrupt 32 times faster
;                       3. Turn on the speaker and start timer2 only if
;                          timer2 is active.
;
;       Entry:          none
;
;       Exit:           none
;
;       Modified:       none
;
;-----------------------------------------------------------------------

        PUBLIC  start_music

start_music:
        PUSH    AX                      ; These are not necessary ?????
        PUSH    BX                      ;
        PUSH    CX                      ;
        PUSH    DX                      ;
        disable                         ;needed ?????
        pause                           ;make sure instruction fetch has occurred
        IN      AL,mskreg               ;get IMR into [AL]
        OR      AL,01H                  ;mask out timer interrupt
        pause                           ;make sure instruction fetch has occurred
        OUT     mskreg,AL               ;write mask to IMR
        CMP     $tichip,1
        JNE     not_ti
        CMP     $synoff,1               ;VOICE 0 suspended bcos of SYNC byte?
        JE      nx1                     ;Brif so, do not activate it
        MOV     $voice,1                ;enable voice 0
        AND     vceoff,0FEH
nx1:
        CMP     $synoff+1,1             ;VOICE 1 suspended bcos of SYNC byte?
        JE      nx2                     ;Brif so,without activating it
        MOV     $voice+2,1              ;enable voice 1
        AND     vceoff,0FDH
nx2:
        CMP     $synoff+2,1             ;VOICE 2 suspended bcos of SYNC byte?
        JE      nx3                     ;Brif so without activating it
        MOV     $voice+4,1              ;enable voice 2
        AND     vceoff,0FBH
nx3:
        MOV     $voice+6,1              ;enable noise channel
        AND     vceoff,0F7H
not_ti:
        CMP     $music,1                ;is music currently on?
        JZ      strtmxt                 ;Brif so
        MOV     $music,1                ;else make music currently active
s_voice:
        PUSH    DS
        PUSH    CS
        POP     DS                      ;[DS] := [CS]
IFE     debug
        setvec  clkint/4,tic_toc        ;modify timer2 interrupt vector
ENDIF                                   ;IFE debug
        POP     DS
        MOV     tt_installed,1          ;tic_toc now installed
        MOV     AX,2048                 ;modify timer2 to interrupt
        pause                           ;make sure instruction fetch has occurred
        OUT     timer0,AL               ;at 32 times the
        MOV     AL,AH                   ;original
        pause                           ;make sure instruction fetch has occurred
        OUT     timer0,AL               ;rate
        CMP     $tichip,1               ;is TI chip active
        JE      tiactv                  ;Brif so
        CMP     $first,1                ;timer2 and speaker already initialized ?
        JE      tiactv                  ;brif so
        MOV     AL,square               ;else set timer2 in square
        pause                           ;make sure instruction fetch has occurred
        OUT     tmrcmd,AL               ;wave mode
        pause                           ;make sure instruction fetch has occurred
        IN      AL,speaker              ;turn on the
        OR      AL,spkron               ;speaker
        pause                           ;make sure instruction fetch has occurred
        OUT     speaker,AL
        MOV     $first,1                ;timer2 and speaker initialization done
tiactv:
strtmxt:
        pause                           ;make sure instruction fetch has occurred
        IN      AL,mskreg               ;get IMR into [AL]
        AND     AL,0FEH                 ;unmask timer interrupt
        pause                           ;make sure instruction fetch has occurred
        OUT     mskreg,AL               ;write mask to IMR
        enable                          ;interrupts
        POP     DX                      ;
        POP     CX
        POP     BX
        POP     AX
IF      debug
more_tics:
        CALL    tic_toc
        JMP     SHORT more_tics
ENDIF                                   ;IF debug
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
;       Name:           stop_music
;
;       Purpose:        This function stops the music playing and flushes
;                       all remaining notes from all the queues.
;
;       Entry:          none
;
;       Exit:           none
;
;       Modified:       none
;
;-----------------------------------------------------------------------

        PUBLIC  stop_music

stop_music:

        disable
        MOV     vceoff,0                ;[new]reset VCEOFF
        MOV     $sync,0                 ;[new]no voices waiting for the SYNC byte
        PUSH    AX
        pause                           ;make sure instruction fetch has occurred
        IN      AL,mskreg               ;get IMR into [AL]
        OR      AL,01H                  ;mask out timer interrupt
        pause                           ;make sure instruction fetch has occurred
        OUT     mskreg,AL               ;write mask to IMR
        CALL    $sndoff
        pause                           ;make sure instruction fetch has occurred
        IN      AL,mskreg               ;get IMR into [AL]
        AND     AL,0FEH                 ;unmask timer interrupt
        pause                           ;make sure instruction fetch has occurred
        OUT     mskreg,AL               ;write mask to IMR
        enable                          ;interrupts
        POP     AX
        RET                             ;returns with no errors

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
        RET

        PAGE

;-----------------------------------------------------------------------
;
;       Name:           $sndoff - turn all music off
;
;       Purpose:
;                       This routine stops music and flushes the music
;                       queue(s). It resets the timer interrupt vector,
;                       resets the timer count and clears the flags
;                       VOICE i and SND_TIM i.
;
;       Entry:          none
;
;       Exit:           none
;
;       Modified:       none
;
;-----------------------------------------------------------------------

        PUBLIC  $sndoff

$sndoff:
        disable
        PUSH    AX
        PUSH    BX
        PUSH    CX
        CALL    get_q_seg               ;[AX] is 0 based voice 1
                                        ;[DS:BX] points to queue control block
                                        ;[ES:0] points to queue
        PUSH    DS                      ;[ES] must be same as [DS] for the
        POP     ES                      ; STOS instructions that may follow
        XOR     AX,AX                   ;zero out [AX]
        MOV     $voice,AX               ;reset VOICE 0 and
        MOV     $sndtim,AX              ; SND_TIM 0
        MOV     $music,AL               ;music is currently OFF
        CALL    ini_que                 ;re-initialize first music queue
        CALL    getmchnid               ;Junior (multivoice) ?
        JNZ     senior                  ;brif not multivoice
        PUSH    DI
        PUSH    SI                      ;
        XOR     AX,AX                   ;0 - used to reset stuff
        MOV     CX,3D                   ;[CX] = word count
        MOV     DI,OFFSET $voice+2      ;first word is reset above
                                        ;[DI] = address of $voice i
        CLD                             ;just to be safe
REP     STOSW                           ;clear voice i
        MOV     CX,4D                   ;[CX] = word count
        MOV     DI,OFFSET $sndtim       ;[DI] = address of $sndtim i
REP     STOSW                           ;clear $sndtim i
        MOV     CX,3D                   ;[CX] = word count
        MOV     DI,OFFSET $synoff       ;[DI] = address of $sndtim i
                                        ;Also clear $synoff, $synoff+1,
                                        ;$synoff+2. This indicates that no
                                        ;voice is suspended bcos of
                                        ;encountering SYNC bytes.
REP     STOSB                           ;clear $synoff i
        MOV     $sync,AL                ;Also clear $sync bcos music is OFF
        ;
        ; Turn off Junior Hardware
        ;
        MOV     AL,9FH                  ;turn off voice 0
        OUT     tiaddr,AL
        MOV     AL,0BFH                 ;turn off voice 1
        OUT     tiaddr,AL
        MOV     AL,0DFH                 ;turn off voice 2
        OUT     tiaddr,AL
        MOV     AL,0FFH                 ;turn off voice 3
        OUT     tiaddr,AL
        MOV     AX,1                    ;voice 2 (0 based)

music2:                                 ;
        XCHG    AH,AL                   ;get_q_seg likes 0 based voice in [AH]
        CALL    get_q_seg               ;[ES:0000] points to queue
                                        ;[DS:BX] points to queue control block
        XCHG    AL,AH                   ;ini_que likes 0 based voice in [AX]
        CALL    ini_que

        INC     AL                      ;do it for the next voice
        CMP     AL,3                    ; until all voices are done
        JBE     music2
        POP     SI
        POP     DI
senior:
        CMP     $first,1                ;should the speaker be turned off ?
        JE      notrnoff                ;brif not required to turn off
        pause                           ;make sure instruction fetch has occurred
        IN      AL,speaker
        AND     AL,NOT spkron           ;turn off speaker
        pause                           ;make sure instruction fetch has occurred
        OUT     speaker,AL
        JMP     SHORT sndf1
notrnoff:                               ;
        pause                           ;make sure instruction fetch has occurred
        MOV     AL,1DH
        OUT     timer2,AL
        MOV     AL,0
        pause
        OUT     timer2,AL
sndf1:
        PUSH    BX                      ;
        PUSH    DX                      ;
        PUSH    DS                      ;
        PUSH    AX                      ;
        XOR     AX,AX                   ;
        MOV     DS,AX                   ;
        rstvec  tod_int,CS:rom_tod      ;ensure INT 8 vectors to the ROM
                                        ; time of day interrupt service routine
        POP     AX                      ;
        POP     DS                      ;
        MOV     tt_installed,0          ;now de-installed
        POP     DX                      ;
        POP     BX                      ;
        XOR     AX,AX
        pause                           ;make sure instruction fetch has occurred
        OUT     timer0,AL               ;restore timer2 count
        pause                           ;make sure instruction fetch has occurred
        OUT     timer0,AL
        POP     CX
        POP     BX
        POP     AX
        CLC                             ;clear carry to indicate no error
        enable
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

        PUBLIC  mdd_cend
mdd_cend:               ;marks end of music device driver code segment

sEnd    CODE

        END
