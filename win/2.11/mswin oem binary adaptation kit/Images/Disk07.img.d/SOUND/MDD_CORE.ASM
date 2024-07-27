        TITLE   MDD_CORE - Interface for Multivoice Music Device Driver
?PLM = 1
?WIN = 1

XDDEF = 0

.xlist
include cmacros.inc
.list

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

;-----------------------------------------------------------------------
;
;                               Data
;
;-----------------------------------------------------------------------
sBegin DATA
PUBLIC  clk_tics, cur_voice_id
PUBLIC  $first
PUBLIC  int_length, in_tic_toc
PUBLIC  $machid, $music
PUBLIC  now_playing
;PUBLIC  old_freq
PUBLIC  qcblock
PUBLIC  $sync, $synoff, $sndtim
PUBLIC  tew_flags, $tichip, tt_installed
PUBLIC  vceoff, $voice

clk_tics DB     0       ;Clock tic modulo counter
cur_voice_id DB 0       ;current voice id - set by get_q_seg
$first  DB      0       ;this byte is used in $startnm and $sndoff
                        ;If it is set, then we do not mess with the speaker.
in_tic_toc DB   0       ;in tic_toc interrupt routine flag
                        ; 0 indicates not in tic_toc
                        ; 1 indicates in tic_toc
int_length DW 3 DUP (0) ;Length of interstice (non 0 indicates we're playing notes)
                        ;one word for each music voice
$machid DB      ?       ;machine ID. This byte is set by the
                        ;routine $GWINI as follows:
                        ;  0FFH if PC
                        ;  0FEH if PC XT
                        ;  0FDH if PC Junior
                        ;  0FCH if PC AT
$music  DB      0       ;music currently active or not byte
now_playing  DB 0       ;1 indicates we are linked with the rest of the
                        ;  sound library
                        ;0 indicates we are not linked with the rest of
                        ;  the sound library
;old_freq DW   3 DUP (?) ;these three words are used to
                        ;save the frequency of stacatto
                        ;and normal notes to be used
                        ;while queueing the pause part of the
                        ;note if SOUND ON is true.
qcblock que_ctrl_blk <> ;Voice 1 (Music)
        que_ctrl_blk <> ;Jr Voice 2 (Music)
        que_ctrl_blk <> ;Jr Voice 3 (Music)
        que_ctrl_blk <> ;Jr Voice 4 (Noise)
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


_BEEP	segment BYTE PUBLIC 'CODE'
EXTRN	fReEnter:BYTE
_BEEP	ends

sBegin  CODE

assumes CS,CODE
assumes DS,DATA
assumes ES,DATA
assumes SS,DATA


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
;       Name:           mach_init - Machine Specific Initialization.
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
;       Entry:          none
;
;       Exit:           PSW.C set if there is a hardware failure
;
;                        else
;
;                       PSW.C reset
;
;       Modified:       none
;
;-----------------------------------------------------------------------

        PUBLIC  mach_init

mach_init:
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
        MOV     DS,CS:mdd_dseg          ;get music device driver's data seg
        MOV     AX,DS                   ;make ES same as DS!!!!!!!
        MOV     ES,AX                   ; necessary ?????????????
        MOV     in_tic_toc,1            ;we're in tic_toc now
        CMP     tt_installed,2          ;[new]is this interrupt vector required?
        JZ      clk_tic                 ;[new]brif so - no need to do anything
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
        MOV     in_tic_toc,0            ;we're no longer in tic_toc
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
get_note:
        CMP     [BX].quenum,0           ;is queue empty?
        JZ      endvce                  ;Brif to turn off the voice
        CALL    get_sound               ;[DX] = Duration, [CX] = Freq
                                        ;[AL] = Attenuation
        JC      nxtret                  ;return if sync byte encountered

play_note:
        ;
        ; Output frequency and attenuation into the TI sound chip or
        ; the 8253 timer as the case may be.
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
        CALL    stop_music
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

; This code was added to fix a bug in the DoBeep routine.  We must let
; DoBeep know when the sound driver is actually making sound.  DoBeep
; will not screw with the speaker in this case.  The sound driver can,
; however do things while DoBeep has the speaker going.  This is not
; fatal and will simply result in some funny sounds.  Without this fix,
; DoBeep could go into continuous beep mode and never turn the speaker
; off. (Sat 10-Oct-1987 : bobgu)

; Set the flag that DoBeep looks at so that he won't touch the speaker
; if we are playing something (it's rude to interrupt a performance...)
	push	es
	mov	ax,seg _BEEP		; get DoBeep's code segment
	mov	es,ax			;  (its fixed so we can do this)
assumes ES,_BEEP
	inc	es:fReEnter		; inc the semaphore.
	pop	es
assumes ES,DATA

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

; This code was added to fix a bug in the DoBeep routine.  We must let
; DoBeep know when the sound driver is actually making sound.  DoBeep
; will not screw with the speaker in this case.  The sound driver can,
; however do things while DoBeep has the speaker going.  This is not
; fatal and will simply result in some funny sounds.  Without this fix,
; DoBeep could go into continuous beep mode and never turn the speaker
; off. (Sat 10-Oct-1987 : bobgu)

; Clear the flag that DoBeep looks at so that he can do beeps again
	cmp	$music,0
	jz	around			; didn't come through start_music
	push	es
	mov	ax,seg _BEEP		; get DoBeep's code segment
	mov	es,ax			;  (its fixed so we can do this)
assumes ES,_BEEP
	dec	es:fReEnter		; dec the semaphore.
	pop	es
assumes ES,DATA
around:

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
;	Fri 02-Oct-1987 10:27:38
;
;	The following fix was implemented to fix a bug wherein this
;	routine would never disable the speaker.  Whenever DoBeep was
;	called after sound was generated, the timer gated to the speaker
;	would run continuously.

if 1
	jne	notrnoff		;Should already be off
	mov	$first,0		;Show we disabled speaker

else	;bogus old way

        JE      notrnoff                ;brif not required to turn off
endif

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

	public	sndf1
sndf1:

;	Fri 02-Oct-1987 10:27:38
;
;	The following fix was implemented to fix a bug wherein this
;	routine was called from the timer interrupt.  In this situation,
;	we would not unhook ourselves from the timer interrupt, and end
;	up only calling about every two seconds, instead of 18 times a
;	second.

if 1
	disable 			;Just make sure, enable is performed
	push	ds			;  at the end of this routine.
	push	ax
	xor	ax,ax
	mov	tt_installed,al 	;Show de-installed
	mov	ds,ax
	assumes ds,nothing
	mov	ax,word ptr rom_tod[0]
	mov	word ptr ds:[tod_int*4][0],ax
	mov	ax,word ptr rom_tod[2]
	mov	word ptr ds:[tod_int*4][2],ax
	pop	ax
	pop	ds
	assumes ds,Data

else	;bogus old way

        CMP     in_tic_toc,1            ;are we in the interrupt routine?
        JE      skip_dos_call           ;brif in tic_toc - no DOS calls allowed
        PUSH    BX                      ;
        PUSH    DX                      ;
        PUSH    DS                      ;
        PUSH    AX                      ;
        XOR     AX,AX                   ;
        MOV     DS,AX                   ;
        rstvec  tod_int,CS:rom_tod      ;ensure INT 8 vectors to the ROM
                                        ; time of day interrupt service routine
        POP     AX                      ;
	POP	DS			;
        MOV     tt_installed,0          ;now de-installed
        POP     DX                      ;
        POP     BX                      ;
endif

skip_dos_call:
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

        PUBLIC  mdd_cend
mdd_cend:               ;marks end of music device driver code segment

sEnd    CODE

        END
