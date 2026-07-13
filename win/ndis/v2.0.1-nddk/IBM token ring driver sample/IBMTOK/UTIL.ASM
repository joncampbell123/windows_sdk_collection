                page    88,122
                title   UTIL - Internal utility functions (2.0.1 Spec)

;****************************************************************************;
;*                                                                          *;
;*      (C) Copyright 1988,1989 by 3Com Corporation                         *;
;*                                                                          *;
;*      This IBM Token Ring NDIS Driver source is the property of Microsoft *;
;*      Corporation and is provided only as an example of an NDIS MAC       *;
;*      driver for use with Microsoft's OS/2 LAN Manager, DOS LAN Manager,  *;
;*      or LM/X products.                                                   *;
;*                                                                          *;
;****************************************************************************;

;------ UTIL.ASM ------------------------------------------------------------;
;                                                                            ;
;       This file contains utility functions used internally by the MAC as   ;
;       well as the timer tick handler for DOS.                              ;
;                                                                            ;
;       09-21-88 Connery                                                     ;
;       Original version.                                                    ;
;                                                                            ;
;       03-17-89 Connery                                                     ;
;       Extended the amount of time to wait for reset to complete.  The      ;
;       original version didn't wait long enough for the 16/4 adapters to    ;
;       get through their initialization.  Other 16/4 support changes.       ;
;       Some logic moved to init code.                                       ;
;                                                                            ;
;       10-16-89 1.1c Connery                                                ;
;       Reload the TVR with 0 after reset so the timer won't be going off    ;
;       anymore.  We can now sit in the debugger as long as we want under    ;
;       most conditions and not have the adapter die off.                    ;
;                                                                            ;
;       02-25-90 2.0a Connery                                                ;
;       Dropped multi-adapter support.  Dropped queue procedures.  Drop      ;
;       ENTER_CR and EXIT_CR.  Move post-reset logic into interrupt handler  ;
;       so it can be called during ResetMAC.  Move spin logic into Bind.     ;
;       Don't save registers in ResetAdapter--we may be off stack.  Create   ;
;       OpenAdapter subroutine out of RqOpenAdapter.  Added TickHandler.     ;
;                                                                            ;
;----------------------------------------------------------------------------;

.xlist
IFDEF OS2
include devhlp.inc
ENDIF
include includes.mac
.list
.lfcond
.sall

;------ Exported Interfaces -------------------------------------------------;

                public  FlushQueues
                public  OpenAdapter
                public  ResetAdapter
IFDEF ANYSTATS
                DOSPUBL TickHandler
ENDIF

;-------Imported Interfaces -------------------------------------------------;

                OS2XTRN devHelper:DWORD
                extrn   IMRport:WORD
                extrn   SRBinuse:BYTE
                extrn   acked:BYTE
                extrn   adapterBits:BYTE
                extrn   common:BYTE
                extrn   enableMask:BYTE
                extrn   multilist:BYTE
                extrn   oARB:WORD
                extrn   oASB:WORD
                extrn   oSRB:WORD
                extrn   oSSB:WORD
                extrn   openHard:WORD
                extrn   openSoft:WORD
                extrn   pdispatch:BYTE
                extrn   portOffset:WORD
                extrn   productid:BYTE
                extrn   protDS:WORD
                extrn   q_rqdone:BYTE
                extrn   q_rqfree:BYTE
                extrn   q_rqtodo:BYTE
                extrn   q_txawaits:BYTE
                extrn   q_txdone:BYTE
                extrn   q_txfree:BYTE
                extrn   q_txtodo:BYTE
                extrn   realSM:WORD
                extrn   rxRelease:WORD
                extrn   rxbuflen:WORD
                extrn   rxbufs:WORD
                extrn   segMMIO:WORD
                extrn   segSM:WORD
                extrn   specific:BYTE
                extrn   status:BYTE
                extrn   timerTicks:WORD
                extrn   txBuffer:WORD
                extrn   txNeeded:BYTE
                extrn   txbuflen:WORD
                extrn   txbufs:WORD
IFDEF ANYSTATS
                DOSXTRN timerCount:DWORD
                DOSXTRN timerChain:DWORD
ENDIF

                BEGIN_CODE

NEWPAGE <ResetAdapter - Reset Adapter>
;------ ResetAdapter --------------------------------------------------------;
;                                                                            ;
;       void ResetAdapter()                                                  ;
;                                                                            ;
;       Registers AX,BX,CX,DX,SI,DI,ES are destroyed.                        ;
;       Returns with interrupts disabled.                                    ;
;                                                                            ;
;       This function resets and initializes the adapter.  It is used to     ;
;       get the adapter back into a known good state after a catastrophic    ;
;       error.  The adapter will be closed and any outstanding requests      ;
;       will be forgotten.                                                   ;
;                                                                            ;
;       We ignore the state of the SRBinuse flag, and just stomp on          ;
;       anything that is in progress.  We unmask the IMR for our interrupt   ;
;       level regardless of the indication offDepth.                         ;
;                                                                            ;
;       If used at any time other than initialization, we should ensure      ;
;       that all of the queues are emptied and all of the state variables    ;
;       reset to their initial states.                                       ;
;                                                                            ;
;       The IBM Token Ring adapter is reset by first writing to the reset    ;
;       port, waiting at least 50ms and then writing to the release port.    ;
;       The card goes through its diagnostics and then generates an SRB      ;
;       acknowledgement interrupt to the PC (even though we didn't issue     ;
;       an SRB, but the IBM hardware engineers pretend we did).              ;
;                                                                            ;
;       This routine uses a loop to delay the 50ms required.  Not the best   ;
;       programming practice, but I needed something simple that worked      ;
;       under both OS/2 and DOS.  The loop is made up of XCHG AX,AX          ;
;       instructions which have the nice characteristic that they take the   ;
;       same number of cycles on all of the Intel processors 8088 thru       ;
;       80486.  The loop is currently coded to take about 50ms on a 40MHz    ;
;       486, with the longest time being about half a second on a 4.77MHz    ;
;       PC.                                                                  ;
;                                                                            ;
;       It appears to be perfectly acceptable to hold the reset state on     ;
;       longer than necessary.  In all my testing it was also acceptable to  ;
;       hold it much shorter than necessary.                                 ;
;                                                                            ;
;----------------------------------------------------------------------------;

ResetAdapter    proc    near
                cli
;
; set acked flag to NO.  this is set to YES by the interrupt handler when
; the expected SRB indicating completion of initialization comes in
;
                mov     acked,NO
;
; clear the SRBinuse flag.
;
                mov     SRBinuse,NO
;
; indicate that the adapter is no longer open
;
                and     status.ms_flags.loword,NOT ST_MAC_IS_OPEN
;
; reset the SRB offset to zero, since the reset relocates it (and the SSB,
; ARB and ASB) and we want the interrupt handler to get the SRB location anew.
;
                mov     oSRB,0
;
; point ES at the MMIO/ROM
;
                mov     es,segMMIO
;
; Latch reset on for the adapter.  While it is held reset we cannot access
; either the MMIO or shared RAM.
;
                mov     dx,portOffset
                add     dx,PRESET
                out     dx,al
;
; enable interrupts since this next loop is too long with them disabled
;
                sti
;
; Wait for at least 50ms to give the adapter time to fully wake up.
;
                mov     cx,0                    ; 65,536 repetitions
                align   4                       ; for predictable 386 results
@@:             xchg    ax,ax                   ; seven xchgs at 3 cycles each
                xchg    ax,ax
                xchg    ax,ax
                xchg    ax,ax
                xchg    ax,ax
                xchg    ax,ax
                xchg    ax,ax
                loop    @B
;
; turn off interrupts for just a minute while we set up a few things
;
                cli
;
; Set the tick counter to zero--it is incremented by the interrupt handler
; on the occurance of a timer tick (ie. every 10ms).
;
                mov     timerTicks,0
;
; Enable interrupts through the PIC in case they are disabled.
;
                mov     dx,IMRport
                in      al,dx
                and     al,enableMask
                out     dx,al
;
; release the adapter reset latch
;
                mov     dx,portOffset
                add     dx,PRELEASE
                out     dx,al
;
; The reset disables interrupts internally for the adapter, reenable them
;
                mov     byte ptr es:ISRPH+ACASET,ISRPHENABLE+ISRPHNMI
;
; Set the adapter timer to interrupt every 10 milliseconds.  While the fact
; is not documented in the IBM Token Ring Tech Ref, this is crucial to proper
; adapter initialization.  If you set TVRL or TCRH to anything else you'll
; get a bring up code of 28H indicating an interrupt diagnostic failure.
;
                mov     byte ptr es:TVRL,1
                mov     byte ptr es:TCRH,TCRHENABLE+TCRHRUN+TCRHRELOAD
;
; if SRAM is pageable then disable paging
;
                test    adapterBits,PAGEABLE
                jz      @F
                mov     byte ptr es:[SRPRH],0   ; disable paging
@@:
;
; Set Shared RAM Segment to the computed location.  We use realSM for this
; even under OS/2, since segSM under OS/2 is an OS/2 virtual address and the
; hardware is designed for a real-mode segment address.  We set the RAM
; Relocation Register to the 16K boundary address of the RAM origin.
;
                mov     ax,realSM
                mov     es:RRRH,ah
;
; Return with interrupts still disabled.
;
                ret
ResetAdapter    endp

NEWPAGE <OpenAdapter - Open the Adapter>
;------ OpenAdapter ---------------------------------------------------------;
;                                                                            ;
;       void OpenAdapter()                                                   ;
;                                                                            ;
;       Registers AX,BX,CX,DX,SI,DI,ES are destroyed.                        ;
;       Returns with interrupts disabled.                                    ;
;                                                                            ;
;       Starts the process of openning the adapter.  The remainder is        ;
;       handled in the interrupt handler.                                    ;
;                                                                            ;
;       The parameters for the open are obtained from miscellaneous          ;
;       sources.  The open options are obtained by ORing those supplied on   ;
;       the OpenAdapter request with those hard-wired by keyword.  The       ;
;       station address is set by SetStationAddress which must be called     ;
;       before the open is done.  Similarly the current multicast and group  ;
;       addresses are used.  The number and size of the receive and          ;
;       transmit buffers are set by parameters in the PROTOCOL.INI file      ;
;       (though they are increased by 8 bytes to account for adapter         ;
;       overhead).                                                           ;
;                                                                            ;
;       A little experimentation with a Token Ring Adapter I (8K) shows      ;
;       that there is something between 6688 and 6711 bytes of memory        ;
;       available for receive and transmit buffers.  Presumably another      ;
;       8K is available on the Adapters II and /A.  The buffer sizes must    ;
;       be a multiple of 8, and there is an adapter overhead of 8 bytes      ;
;       on receive buffers and 6 bytes on transmit buffers.  The buffer      ;
;       sizes must be between 96 and 2048 bytes in length.  The defaults     ;
;       (if zero) are 112 bytes for receive and 600 bytes for transmit.  A   ;
;       packet may span multiple receive buffers, but must fit within a      ;
;       single transmit buffer.  If more memory is available it will be      ;
;       configured as receive buffers, so a reasonable strategy is just to   ;
;       compute some reasonable numbers for the 8K Adapter I and allow the   ;
;       Adapters II and /A to operate with more receive buffers.  If the     ;
;       number of receive buffers requested cannot be configured, then the   ;
;       retcode will be set to 30h, indicating insufficient memory.          ;
;                                                                            ;
;       A couple of possible configurations are:                             ;
;                                                                            ;
;               3 buffers of 2048 bytes = 6136 bytes                         ;
;               4 buffers of 1672 bytes = 6688 bytes                         ;
;                                                                            ;
;       A minimum of 1 transmit buffer and 2 receive buffers are necessary   ;
;       for a successful open.  According to the IBM Token Ring Adapter      ;
;       Technical Reference, the "integrity of adapter operation cannot be   ;
;       guaranteed" if the number of transmit buffers is greater than 2, so  ;
;       we enforce 2 as a maximum.  Setting the transmit buffers to more     ;
;       than one should theoretically help performance, but none of the      ;
;       adapters tested to date (IBM Token Ring Adapters I, II, /A, II       ;
;       short, 16/4, 16/4 /A) allow overlapped transmits so a second         ;
;       transmit buffer is pointless.                                        ;
;                                                                            ;
;       The operation of openning the adapter is very time consuming on      ;
;       a token ring network.  Normally, it seems to take between 20 and     ;
;       40 seconds or so.  The protocol should try and make use of this      ;
;       time to do something else if possible.                               ;
;                                                                            ;
;       The DIR.OPEN.ADAPTER operation is a little strange in that the       ;
;       results are returned in the SRB rather than an SSB.  Also the        ;
;       DIR.OPEN.ADAPTER SRB is bigger than there is normally room for       ;
;       so it can only be issued while the adapter is closed, and after      ;
;       the open completes, the adapter memory is reorganized, the SRB       ;
;       moved about and its size returned to normal.  The new location       ;
;       of the SRB (and the locations of the SSB, ARB and ASB) are returned  ;
;       as parameters in the SRB.                                            ;
;                                                                            ;
;----------------------------------------------------------------------------;

OpenAdapter     proc    near
                cli
;
; set up an SRB for DIR.OPEN.ADAPTER.  remember that word fields are in
; Motorola byte order (low,high), byte-swapped from Intel.
;
                mov     es,segSM
                mov     di,oSRB
                mov     es:[di].srb_command,DIR_OPENADAPTER
                mov     ax,openSoft             ; from Request OpenAdapter
                or      ax,openHard             ; forced by keyword
                xchg    ah,al
                mov     es:[di].openr_options,ax
;
; set the node address from mac specific characteristics table (which is
; initialized to the hardware address, but can be set through SetStationAddress)
;
                mov     si,offset DGROUP:specific.mc_caddr
                add     di,openr_nodeaddr
                mov     cx,ADDRLEN/2
        rep     movsw
;
; set the group address from the current multicast list.  if there are no
; multicasts in the list, the group address is set to zero.  otherwise set
; the group address to the last 4 bytes of the single multicast address
; allowed (the first two bytes are assumed to be 80 00 hex).
;
                cmp     multilist.ma_cur,0
                jnz     rqopn_copygroup
                xor     ax,ax
                stosw
                stosw
                jmp     short rqopn_funct
rqopn_copygroup:
                mov     si,offset DGROUP:multilist.ma_mbuff[2]
                lodsw
                stosw
                lodsw
                stosw
;
; copy the functional address from the mac specific characteristics table
;
rqopn_funct:
                mov     si,offset DGROUP:specific.mc_faddr
                lodsw
                stosw
                lodsw
                stosw
;
; set the numbers and sizes of the transmit and receive buffers as configured
; from the PROTOCOL.INI file (or as defaulted if not specified).  the sizes
; need to be increased by 8 to account for adapter overhead.  note that the
; transmit buffer size on the adapter is only a single byte.
;
                mov     ax,rxbufs
                xchg    ah,al
                stosw
                mov     ax,rxbuflen
                add     ax,RXBUF_OVERHEAD
                xchg    ah,al
                stosw
                mov     ax,txbuflen
                add     ax,TXBUF_OVERHEAD
                xchg    ah,al
                stosw
                mov     ax,txbufs               ; AH is zero
                stosw
;
; zero out the DLC options
;
                xor     ax,ax
                mov     cx,(SIZE openr_zeroes)/2
        rep     stosw
;
; copy the product id
;
                mov     si,offset DGROUP:productid
                mov     cx,(SIZE openr_productid)/2
        rep     movsw
;
; hit the command ready bit
;
                mov     es,segMMIO
                mov     byte ptr es:ISRAL+ACASET,ISRALSRBRDY+ISRALFREESRB
                ret
OpenAdapter     endp

NEWPAGE <FlushQueues - Clear out Transmit/Request Queues>
;------ FlushQueues ---------------------------------------------------------;
;                                                                            ;
;       FlushQueues()                                                        ;
;                                                                            ;
;       Registers DS,SS,SP,BP and ES,SI,DI preserved.                        ;
;                                                                            ;
;       Empties out the in-progress Requests and generates failed            ;
;       confirmations for them.  Empties out the various transmit            ;
;       in-progress queues and generates failed confirmations for them.      ;
;       Resets various indicators.                                           ;
;                                                                            ;
;       Enables interrupts during processing.  Must be called with           ;
;       interrupts disabled.  Returns with interrupts disabled.              ;
;                                                                            ;
;----------------------------------------------------------------------------;

FlushQueues     proc    near
                PUSHR   si,di
;
; set the receive buffer to release to NULL
;
                mov     rxRelease,0
;
; set the txneeded counter to zero and transmit buffer to NULL
;
                mov     txNeeded,0
                mov     txBuffer,0
;
; loop until no more entries on the request todo queue or request awaiting
; completion queue.
;
flush_rqtodo:
                DEQ     q_rqtodo,flush_rqdone
                jmp     short @F
flush_rqdone:
                DEQ     q_rqdone,flush_txtodo
@@:
;
; generate a failed confirmation for the request in SI if a non-zero handle
;
                cmp     [si].qr_handle,0
                jz      @F

                sti

                push    [si].qr_protid
                push    common.cc_ID
                push    [si].qr_handle
                pushc   INVALID_FUNCTION,ax
                push    [si].qr_opcode
                push    protDS
                call    pdispatch.pd_reqconfirm

                cli
@@:
;
; move the queue entry in SI to the request free queue and loop around
;
                ENQ     q_rqfree,di
                jmp     flush_rqtodo
;
; loop until no more entries on any of the transmit queues
;
flush_txtodo:
                DEQ     q_txtodo,flush_txawaits
                jmp     short @F
flush_txawaits:
                DEQONLY q_txawaits,flush_txdone
                jmp     short @F
flush_txdone:
                DEQ     q_txdone,flush_exit
@@:
;
; generate a failed confirmation for the request in SI if a non-zero handle
;
                cmp     [si].qt_handle,0
                jz      @F

                sti

                push    [si].qt_protid
                push    common.cc_ID
                push    [si].qt_handle
                pushc   INVALID_FUNCTION,ax
                push    protDS
                call    pdispatch.pd_txconfirm

                cli
@@:
;
; move the queue entry in SI to the transmit free queue and loop around
;
                ENQHEAD q_txfree
                jmp     flush_txtodo
;
; done.
;
flush_exit:
                POPR    si,di
                ret
FlushQueues     endp

;------ TickHandler ---------------------------------------------------------;
;                                                                            ;
;       Timer Tick Handler for DOS                                           ;
;                                                                            ;
;       Just increments a global counter of ticks, used by RqClearStats.     ;
;                                                                            ;
;----------------------------------------------------------------------------;

IFDEF ANYSTATS
IFDEF DOS
TickHandler     proc    far
                add     cs:timerCount.loword,1
                adc     cs:timerCount.hiword,0
                jmp     cs:timerChain
TickHandler     endp
ENDIF
ENDIF

                END_CODE
                end
