                page    88,122
                title   XMIT - Transmit Functions (2.0.1 Spec)

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

;------ XMIT.ASM ------------------------------------------------------------;
;                                                                            ;
;       This file contains the TransmitChain entry which is concerned with   ;
;       the transmission of packets onto the network media.                  ;
;                                                                            ;
;       The process of transmitting a packet onto the network using an       ;
;       IBM Token Ring adapter is a reasonably complicated one and most      ;
;       of the action takes place in the interrupt handler rather than       ;
;       here.                                                                ;
;                                                                            ;
;       First, the driver requests a transmit buffer by issuing an SRB to    ;
;       the adapter.  The adapter responds by issuing a transmit request     ;
;       ARB to the driver, specifying the transmit buffer to use.  The       ;
;       driver copies the packet into the transmit buffer and then issues    ;
;       a transmit ASB to the adapter, specifying the number of bytes in     ;
;       the buffer.  Once the packet has circulated around the ring and      ;
;       has been removed and replaced by the token, the final transmit SSB   ;
;       is issued from the adapter to the driver.  This is where the         ;
;       TransmitComplete is issued to the protocol.                          ;
;                                                                            ;
;       The only thing we do here is to issue the transmit SRB to the        ;
;       adapter.  This contains no real information, so transmit requests    ;
;       are ALWAYS queued for the interrupt handler to deal with.  Once      ;
;       in a very long while the SRB will still be in use, processing some   ;
;       previous transmit or control request.  In that case we just bump     ;
;       a counter of the number of transmit SRBs that still need to be       ;
;       issued--since the transmit SRB contains no real information there    ;
;       is no need for any additional queues.                                ;
;                                                                            ;
;       Any number of transmit SRBs can be issued to the adapter, but the    ;
;       adapter serializes the rest of the process so that only one          ;
;       transmit buffer will be outstanding to the driver at any one         ;
;       time.                                                                ;
;                                                                            ;
;       The ARB and ASB are used by both the transmit and receive sides.     ;
;       The adapter serializes access to the ARB, but the ASB will           ;
;       occaisionally be in use when we want to start a transmission.  We    ;
;       therefore have one more queue where the transmit entry will be       ;
;       placed if such a collision occurs.  The transmission will then be    ;
;       started when the ASB becomes free.  Since only one transmit ARB      ;
;       will be outstanding from the adapter at a time, this queue only      ;
;       needs to hold one entry.                                             ;
;                                                                            ;
;       In general 802.5 adapters loop back any packets they send which      ;
;       they would receive if sent by someone else.  This loopback occurs    ;
;       after the transmit ASB and before the transmit SSB.  The loopback    ;
;       is synchronized so that the transmit and receive sides do not        ;
;       compete for the use of the ARB or ASB.                               ;
;                                                                            ;
;       The original logic for transmission generated 4 interrupts per       ;
;       transmit--SRB free, ARB transmit request, ASB free, and SSB          ;
;       transmit completed.  The reworked driver eliminates two of these.    ;
;       The SRB free interrupt is avoided by clearing SRBinuse when we       ;
;       get the transmit ARB.  Since the SRB is only used for transmits      ;
;       and requests (which can stand to be delayed a little bit if they     ;
;       overlap transmits) this does not seem a major problem.  Then the     ;
;       ASB free interrupt was eliminated (from receive as well) by          ;
;       relying on the fact that the ASB retcode goes to FFh when the        ;
;       ASB is available.  Only if the ASB is not available and we have      ;
;       to queue for later transmission do we request the ASB free           ;
;       interrupt.                                                           ;
;                                                                            ;
;       Another late breaking optimization... ask for a transmit buffer      ;
;       even when we have nothing to transmit.  Under low-load situations    ;
;       with full protocol stacks this will get us the transmit buffer       ;
;       before the TransmitChain is issued.  Then we can copy directly       ;
;       into the transmit buffer directly out of this routine, avoiding      ;
;       an interrupt in the critical path and optimizing a bit because       ;
;       of reduced queueing overhead.                                        ;
;                                                                            ;
;       09-21-88 Connery                                                     ;
;       Original version.                                                    ;
;                                                                            ;
;       02-25-90 2.0a Connery                                                ;
;       Removed multi-adapter support.  Added FASTGDT logic.  Use new        ;
;       ENQ/DEQ macros.  Dropped lastSRBtx flag.  Break statistics into      ;
;       ANY/ALL categories.                                                  ;
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

                public  TransmitChain

;------ Imported Interfaces -------------------------------------------------;

                DEXTRN  txarbs:WORD
                DEXTRN  txasbs:WORD
                DEXTRN  txcants:WORD
                DEXTRN  txfasters:WORD
                DEXTRN  txlaters:WORD
                DEXTRN  txsrbs:WORD
                DEXTRN  txsrbspended:WORD
                DEXTRN  txssbs:WORD
                OS2XTRN devHelper:DWORD
                OS2XTRN segTX:WORD
                extrn   SRBinuse:BYTE
                extrn   cs_ds:WORD
                extrn   oARB:WORD
                extrn   oASB:WORD
                extrn   oSRB:WORD
                extrn   oSSB:WORD
                extrn   q_txawaits:BYTE
                extrn   q_txdone:BYTE
                extrn   q_txfree:BYTE
                extrn   q_txtodo:BYTE
                extrn   rxRelease:WORD
                extrn   segMMIO:WORD
                extrn   segSM:WORD
                extrn   state:BYTE
                extrn   status:BYTE
                extrn   txBuffer:WORD
                extrn   txCorrelate:BYTE
                extrn   txNeeded:BYTE
IFDEF SNAP
                DEXTRN  txsnaparea:BYTE
                DEXTRN  txsnapix:WORD
ENDIF

                BEGIN_CODE

NEWPAGE <TransmitChain - Transmit Packet Onto Network>
;------ TransmitChain -------------------------------------------------------;
;                                                                            ;
;       int far pascal TransmitChain(                                        ;
;                                       WORD ProtID,                         ;
;                                       WORD Handle,                         ;
;                                       DWORD Bufdesc,                       ;
;                                       WORD MACDS      );                   ;
;                                                                            ;
;       Transmits a packet onto the network.  The packet is described by     ;
;       some immediate data (which will be transmitted first if present)     ;
;       and a series of frame data blocks.  The buffer descriptor and the    ;
;       immediate data are assumed to have a lifetime only as long as the    ;
;       synchronous portion of the command, meaning that we must copy this   ;
;       information when we queue the request for asynchronous processing.   ;
;       Note that there is a limit of 64 bytes on the immediate data.        ;
;                                                                            ;
;       Note: Failure to align data buffers on (physical) word boundaries    ;
;       may have disastrous performance consequences.                        ;
;                                                                            ;
;       Copying the packet to the adapter's transmit buffer takes            ;
;       significantly longer than you might expect from abstract estimates   ;
;       based solely on cpu cycles.  See RECV.ASM for specific details.      ;
;                                                                            ;
;       Theoretically the Token Ring can carry very close to its 4Mbps       ;
;       rated capacity.  However, measurements with the actual copy to the   ;
;       adapter removed indicate that current IBM Token Ring Adapters for    ;
;       the PC top out around 3Mbps for transmit so 4Mbps is not achievable  ;
;       with a single transmitter.                                           ;
;                                                                            ;
;       Multiple (more than one) transmit buffer should theoretically        ;
;       help, by allowing for some overlap between copying the packet to     ;
;       the adapter and transmitting it on the network.  However, with       ;
;       current IBM Token Ring Adapters, the driver is never given more      ;
;       than one of the transmit buffers.  It appears that the adapter       ;
;       configures the second transmit buffer (the number of receive         ;
;       buffers that can be requested goes down), but no second buffer       ;
;       address is ever seen.  The driver still allows a second transmit     ;
;       buffer to be configured, and will default to two transmit buffers    ;
;       if there is sufficient memory, but this appears to make no           ;
;       performance difference with the current adapters.  As a              ;
;       consolation, IBM's LAN Support software does not seem to be able to  ;
;       get this to work either since measured performance doesn't change    ;
;       between 1 and 2 DHBs.                                                ;
;                                                                            ;
;----------------------------------------------------------------------------;

;------ Early-Out Errors ----------------------------------------------------;
;
; invalid state
;
                public  xmit_invalid
xmit_invalid:
                mov     ax,INVALID_FUNCTION
                jmp     xmit_exit
;
; no free queue entries.  return error.
;
                public  xmit_unable
xmit_unable:
                DINC    txcants
                mov     ax,OUT_OF_RESOURCES
                jmp     xmit_exit
;
; jump to xmit_faster (too far away)
;
IFDEF PREFETCH
                align   4
goto_xmit_faster:
                jmp     xmit_faster
ENDIF

;------ TransmitChain -------------------------------------------------------;

                DLOCAL  queue_entry
                DLOCAL  txbuf

                align   4
TransmitChain   proc    far
                LOCALS
                PUSHR   si,di,ds
;
; set up DS.
;
                mov     ds,cs:cs_ds
;
; ensure the adapter is openned
;
                cmp     state,OPENNED
                jne     xmit_invalid
;
; disable interrupts for the remainder.  there are probably smaller critical
; sections here, but there seems no great reason to find them.
;
                cli
;
; grab a free qentry.
;
                DEQHEAD q_txfree,xmit_unable
;
; check if a transmit buffer is already available.  if so, another path...
;
IFDEF PREFETCH
                cmp     txBuffer,0
                jnz     goto_xmit_faster
ENDIF
;
; set ES:DI to the queue entry and save it for later in BX
;
                mov     ax,ds
                mov     es,ax
                mov     di,si                           ;ES:DI => qentry
                mov     bx,di
;
; copy the handle and protid to the queue entry (DS:DI and ES:DI the same)
;
                mov     ax,[bp].tx_handle
                mov     [di].qt_handle,ax
                mov     ax,[bp].tx_protid
                mov     [di].qt_protid,ax
;
; copy the immediate data if any
;
                lds     si,[bp].tx_bufdesc              ;DS:SI => bufdesc
                mov     cx,ds:[si].tx_immed_bytes       ;CX = # bytes idata
                mov     es:[di].qt_ibytes,cx
                jcxz    @F

                lds     si,ds:[si].tx_immed_buff        ;DS:SI => idata
                add     di,qt_ibuffer                   ;ES:DI => qentry.ibuffer

                shr     cx,1
        rep     movsw                                   ;move words
                adc     cx,cx
        rep     movsb                                   ;move last byte if odd

                mov     di,bx                           ;ES:DI => qentry
                lds     si,[bp].tx_bufdesc              ;DS:SI => bufdesc
@@:
;
; check if there is anything in the buffer descriptor chain to be copied
;
                mov     cx,ds:[si].tx_data_count        ;CX = # blocks
                mov     es:[di].qt_blocks,cx
                jcxz    @F
;
; set CX to the number of words in the buffer descriptor chain to copy.  make
; sure CDSIZE is 8 since we doing explicit shifts here (true in 2.0.1 NDIS)
;
                CLAIM   <CDSIZE EQ 8>
                shl     cx,1                            ;CX * 2
                shl     cx,1                            ;CX * 4 = # words
;
; copy the buffer descriptor chain to the queue entry.
;
                add     di,qt_chain                     ;ES:DI => qentry.chain
                add     si,tx_chain                     ;DS:SI => bufdesc.chain
        rep     movsw
@@:
;
; qentry all filled in.  restore DS:SI to the queue entry again
;
                mov     ds,cs:cs_ds
                mov     si,bx                           ;DS:SI => qentry
;
; enqueue the qentry on the transmit todo queue
;
                ENQ     q_txtodo,di
;
; check if the SRB is in use.  if so, just bump a counter of the number of
; transmit request SRBs we need to issue and return.
;
                public  xmit_srb
xmit_srb:
                mov     cx,1                    ; number of transmit SRBs needed
                mov     al,YES
                xchg    al,SRBinuse
                test    al,al
                jnz     xmit_queued
;
; set ES:DI to point to the SRB on the adapter
;
                mov     es,segSM
                mov     di,oSRB
;
; set the command code for TRANSMIT.DIR.FRAME and dummy out the station id
;
                mov     es:[di].srb_command,DIR_TRANSMIT
                mov     es:[di].txsrb_stationid,0
;
; hit the command ready bit
;
                DINC    txsrbs
                mov     es,segMMIO
                mov     byte ptr es:ISRAL+ACASET,ISRALSRBRDY
;
; No new transmit SRBs needed
;
                xor     cx,cx
;
; return to protocol.  CX holds the number of transmit SRBs that need to be
; issued (0 or 1)
;
                align   4
                public  xmit_queued
xmit_queued:
                add     txNeeded,cl
                DADD    txsrbspended,cx
                mov     ax,REQUEST_QUEUED
;
; done.  return code in AX.  reenable interrupts in case they are disabled.
;
                public  xmit_exit
xmit_exit:
                sti
                POPR    si,di,ds
                leave
                ret     TXRET

;------ xmit_faster ---------------------------------------------------------;
;
; we had a transmit buffer already available.  save the queue entry in case
; we need it (we won't if no confirmation was requested and we don't get
; a collision with the receive side on access to the ASB).
;
IFDEF PREFETCH
                align   4
                public  xmit_faster
xmit_faster:
                DINC    txfasters
;
; save the queue entry for later
;
                mov     word ptr [bp].queue_entry,si
;
; set ES:DI to the transmit buffer (and reset txbuffer to 0), then save the
; transmit buffer offset for later
;
                xor     di,di
                xchg    di,txBuffer
                mov     es,segSM
                mov     [bp].txbuf,di
;
; enable interrupts since this may take a while...
;
                sti
;
; copy the immediate data if any
;
                lds     si,[bp].tx_bufdesc              ;DS:SI => bufdesc
                mov     cx,ds:[si].tx_immed_bytes       ;CX = # bytes idata
                jcxz    @F

                lds     si,ds:[si].tx_immed_buff        ;DS:SI => idata

                shr     cx,1
        rep     movsw
                adc     cx,cx
        rep     movsb

                lds     si,[bp].tx_bufdesc              ;DS:SI => bufdesc
@@:
;
; check if there are any data blocks to copy.
;
                mov     cx,ds:[si].tx_data_count        ;CX = # blocks
                jcxz    xmit_fcopied
;
; set DX to the number of blocks remaining, and DS:SI to the first chain entry
;
                mov     dx,cx                           ;DX = # blocks
                add     si,tx_chain                     ;DS:SI => 1st chain
;
; set DS:SI to the virtual address of the frame data, and CX to the number of
; bytes in the block.  under OS/2 we may have to virtualize a physical address.
; if FASTGDT is defined do this ourselves in-line (the devhelp calls are just
; too many instructions).
;
                align   4
                public  xmit_floop
xmit_floop:
                push    ds
                push    si

                mov     cx,ds:[si].cd_data_len
IFDEF OS2
                cmp     ds:[si].cd_ptr_type,CT_GDT
                jne     @F
                lds     si,ds:[si].cd_data_buff
                jmp     short xmit_fcopyb
@@:
    IFDEF FASTGDT
                mov     al,ds:[si].cd_data_buff.hiword.lobyte
                mov     bx,ds:[si].cd_data_buff.loword
                mov     ds,cs:cs_ds
                lds     si,dword ptr segTX
                mov     ds:[si].segd_base15_0,bx
                mov     ds:[si].segd_base23_16,al
                mov     ds:[si].segd_lim15_0,cx ; CX-1 really, but screw it
                mov     ds,si
                xor     si,si
    ELSE
                push    dx
                mov     ax,ds:[si].cd_data_buff.hiword
                mov     bx,ds:[si].cd_data_buff.loword
                mov     ds,cs:cs_ds
                mov     si,segTX
                mov     dl,DevHlp_PhysToGDTSelector
                call    devHelper
                mov     ds,si
                xor     si,si
                pop     dx
    ENDIF
ELSE
                lds     si,ds:[si].cd_data_buff
ENDIF
;
; copy the frame data block (CX bytes from DS:SI to ES:DI)
;
                align   4
                public  xmit_fcopyb
xmit_fcopyb:
                shr     cx,1
        rep     movsw
                adc     cx,cx
        rep     movsb
;
; next frame data block (in DS:SI)
;
                pop     si
                pop     ds

                add     si,CDSIZE
                dec     dx
                jnz     xmit_floop
;
; transmit buffer filled in.  disable interrupts for the remainder and set
; DS:SI to the queue entry again.
;
                align   4
                public  xmit_fcopied
xmit_fcopied:
                cli
                mov     ds,cs:cs_ds
                mov     si,word ptr [bp].queue_entry
;
; set CX to the number of bytes copied
;
                mov     cx,di
                sub     cx,[bp].txbuf
;
; update statistics on packets, total bytes transmitted
;
                INCANY  ms_total_sent
                ADDANY  ms_total_sbytes,cx
IFDEF ALLSTATS
                mov     di,[bp].txbuf
                test    byte ptr es:[di][DA][0],80h             ;broadcast bit?
                jz      @F
                INCALL  ms_mult_sent
                ADDALL  ms_mult_sbytes,cx
                test    byte ptr es:[di][DA][2],80h             ;broadcast?
                jz      @F
                INCALL  ms_broad_sent
                ADDALL  ms_broad_sbytes,cx
@@:
ENDIF
;
; snap transmit buffer for debugging
;
IFDEF SNAP
                mov     di,[bp].txbuf
                call    SnapTx
ENDIF
;
; we want to send an ASB to the adapter to start the transmission.  if the
; ASB is available then take it over.
;
                mov     di,oASB
                cmp     es:[di].asb_retcode,0FFh
                DJNE    xmit_flater
;
; ES:DI points to the ASB which we can use to start transmission.  interrupts
; are off and must remain so until we hit the ASB ready bit.
;
; if the handle is zero, no confirmation is needed.  just move the qentry in SI
; back to the free queue.  otherwise, enqueue it on the tx awaiting completion
; queue for when the tx SSB comes in.
;
                cmp     [bp].tx_handle,0
                jnz     xmit_fpicky

                ENQHEAD q_txfree
;
; set up the ASB to start the transmission.  set the retcode to zero so that
; any test against FFh (completed) will fail.
;
                public  xmit_fasb
xmit_fasb:
                mov     di,oASB
                mov     es:[di].asb_command,DIR_TRANSMIT
                mov     al,txCorrelate
                mov     es:[di].asb_correlate,al
                mov     es:[di].asb_retcode,0
                mov     es:[di].txasb_stationid,0
                xchg    ch,cl
                mov     es:[di].txasb_framelen,cx
;
; hit the ASB ready bit to inform the adapter to process it, and if a receive
; buffer is waiting to be released, make sure the adapter will interrupt us
; when this ASB is finished processing.
;
                DINC    txasbs
                mov     es,segMMIO
                cmp     rxRelease,0
                jnz     xmit_fparallel

                mov     byte ptr es:ISRAL+ACASET,ISRALASBRDY
                jmp     xmit_srb
xmit_fparallel:
                mov     byte ptr es:ISRAL+ACASET,ISRALASBRDY+ISRALFREEASB
                jmp     xmit_srb
;
; enqueue the qentry in SI on the tx awaiting confirmation queue, then rejoin
; the normal start transmit processing
;
                public  xmit_fpicky
xmit_fpicky:
                mov     ax,[bp].tx_handle
                mov     [si].qt_handle,ax
                mov     ax,[bp].tx_protid
                mov     [si].qt_protid,ax
                mov     al,txCorrelate
                mov     [si].qt_correlate,al

                ENQ     q_txdone,di
                jmp     short xmit_fasb
;
; the ASB is in use (which happens very rarely).  make sure the adapter will
; interrupt us when the ASB is available.  enqueue the queue entry on the
; tx awaiting start queue where it can be started when the ASB free interrupt
; comes in from the adapter.
;
                public  xmit_flater
xmit_flater:
                DINC    txlaters

                mov     ax,[bp].tx_handle
                mov     [si].qt_handle,ax
                mov     ax,[bp].tx_protid
                mov     [si].qt_protid,ax
                mov     al,txCorrelate
                mov     [si].qt_correlate,al
                mov     [si].qt_bytes,cx

                ENQONLY q_txawaits

                mov     es,segMMIO
                mov     byte ptr es:ISRAL+ACASET,ISRALFREEASB
                jmp     xmit_srb
ENDIF
TransmitChain   endp

NEWPAGE <SnapTx - Snapshot Transmit Buffer>
;------ SnapTx --------------------------------------------------------------;
;                                                                            ;
;       void SnapTx()                                                        ;
;                                                                            ;
;       ES:DI = pointer to transmit buffer                                   ;
;       CX = number of bytes in transmit buffer                              ;
;                                                                            ;
;       Preserves all registers except flags.                                ;
;                                                                            ;
;       Snaps up to 16 bytes of information (packet number, packet size,     ;
;       parts of the packet) into the transmit snap area, a circular queue   ;
;       of the last 64 packets transmitted.  Should be enabled only in       ;
;       debug versions.                                                      ;
;                                                                            ;
;----------------------------------------------------------------------------;

IFDEF SNAP
                public  SnapTx
SnapTx          proc    near
                pusha

                mov     si,txsnapix
                inc     txsnapix
                and     txsnapix,3Fh
                shl     si,1
                shl     si,1
                shl     si,1
                shl     si,1
                add     si,txsnaparea

                mov     ax,txarbs
                mov     word ptr [si][0],ax
                mov     word ptr [si][2],cx

                mov     ax,word ptr es:[di][14]
                mov     word ptr [si][4],ax
                mov     ax,word ptr es:[di][16]
                mov     word ptr [si][6],ax
                mov     ax,word ptr es:[di][18]
                mov     word ptr [si][8],ax
                mov     ax,word ptr es:[di][20]
                mov     word ptr [si][10],ax
                mov     ax,word ptr es:[di][22]
                mov     word ptr [si][12],ax

                popa
                ret
SnapTx          endp
ENDIF

                END_CODE
                end
