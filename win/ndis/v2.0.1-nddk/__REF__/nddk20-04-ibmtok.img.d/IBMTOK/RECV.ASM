                page    88,122
                title   RECV - Receive Functions (2.0.1 Spec)

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

;------ RECV.ASM ------------------------------------------------------------;
;                                                                            ;
;       This file contains entries concerned with the reception of packets   ;
;       from the network media as well as the control of indications.        ;
;                                                                            ;
;       The process of receiving a packet off the network using an IBM       ;
;       Token Ring adapter is somewhat simpler than transmit, but most of    ;
;       the action still takes place in the interrupt handler rather than    ;
;       here in this file.                                                   ;
;                                                                            ;
;       A received packet goes into one or more of the receive buffers on    ;
;       the adapter.  To inform the driver of the received packet the        ;
;       adapter issues a received data ARB to the driver, giving it the      ;
;       address of the first recieve buffer in the chain holding the         ;
;       packet and its length.  The driver is expected to copy the packet    ;
;       out of the buffer(s) and release the buffer(s) back to the adapter   ;
;       with a release ASB.                                                  ;
;                                                                            ;
;       Although multiple packets may be in the adapters buffers at any      ;
;       time, only a single set of buffers will be handed over to the        ;
;       driver at any given time.  Until the release ASB is issued back      ;
;       to the adapter, the adapter will not issue another received data     ;
;       ARB to the driver.                                                   ;
;                                                                            ;
;       The ARB and ASB are used by both the transmit and receive sides.     ;
;       The adapter serializes access to the ARB, but the ASB will           ;
;       occaisionally be in use when we want to release a buffer back to     ;
;       the adapter.  If this happens the buffer release will be pended      ;
;       until the ASB becomes free.  Since only a single receive buffer      ;
;       chain is outstanding to the driver at any time, we do not need a     ;
;       queue to handle this situation, just a single pointer to a buffer    ;
;       chain to pend release will do.                                       ;
;                                                                            ;
;       In general 802.5 adapters loop back any packets they send which      ;
;       they would receive if sent by someone else.  This loopback occurs    ;
;       after the transmit ASB and before the transmit SSB.  The loopback    ;
;       is synchronized so that the transmit and receive sides do not        ;
;       compete for the use of the ARB or ASB.                               ;
;                                                                            ;
;       09-21-88 Connery                                                     ;
;       Original version.                                                    ;
;                                                                            ;
;       02-25-90 2.0a Connery                                                ;
;       Allow TransferData to be called more than once as per new NDIS       ;
;       spec recommendation.  Dropped multi-adapter support.  Added FASTGDT  ;
;       code.  Allow offset on TransferData to exceed length of first        ;
;       piece, as per NDIS 2.0.1.  Minor performance tweaks.                 ;
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

                public  IndicationOff
                public  IndicationOn
                public  ReceiveRelease
                public  TransferData

;------ Imported Interfaces -------------------------------------------------;

                DEXTRN  rxarbs:WORD
                DEXTRN  rxasbs:WORD
                OS2XTRN devHelper:DWORD
                OS2XTRN segRX:WORD
                extrn   IMRport:WORD
                extrn   cs_ds:WORD
                extrn   disableMask:BYTE
                extrn   enableMask:BYTE
                extrn   offDepth:BYTE
                extrn   rxHead:WORD
                extrn   rxSize:WORD
                extrn   segSM:WORD
IFDEF SNAP
                DEXTRN  rxsnapix:WORD
                DEXTRN  rxsnaparea:BYTE
ENDIF

                BEGIN_CODE

NEWPAGE <TransferData - Copy Received Packet Into Protocol Buffers>
;------ TransferData --------------------------------------------------------;
;                                                                            ;
;       int far pascal TransferData(                                         ;
;                                       LPWORD Copied,                       ;
;                                       WORD Offset,                         ;
;                                       LPBUF Bufdesc,                       ;
;                                       WORD MACDS      );                   ;
;                                                                            ;
;       Called by the protocol to copy the current receive packet into the   ;
;       protocol buffers described by Bufdesc.  The copy operation starts    ;
;       at byte Offset and the number of bytes copied is returned in         ;
;       Copied.  This function should only be called from ReceiveLookahead,  ;
;       though it need not be called at all.                                 ;
;                                                                            ;
;       Bufdesc points to a transfer buffer descriptor describing a series   ;
;       of frame data blocks to be used for the transfer.  The packet will   ;
;       be copied in such a way as to fill in each block to capacity before  ;
;       proceeding to the next.  The buffer descriptor itself will not be    ;
;       modified by this operation.                                          ;
;                                                                            ;
;       Note: Failure to align data buffers on (physical) word boundaries    ;
;       may have significant performance consequences.                       ;
;                                                                            ;
;       Because of wait states on the memory, the 8MHz bus, waits on the     ;
;       adapter memory and the 8 bit data path to the Adapters I & II the    ;
;       time it takes to copy a packet to or from the adapter into system    ;
;       memory is substantially different from what might be theoretically   ;
;       predicted based solely on processor clock cycles.  Tests show that   ;
;       a single REP MOVSW takes about:                                      ;
;                                                                            ;
;               2.0 usec on a 386/20 (vs. 0.2 predicted)                     ;
;               2.0 usec on a 286/8 (vs. 0.5 predicted)                      ;
;               1.1 usec on a PS/2 Model 50 (vs. 0.4 predicted)              ;
;                                                                            ;
;       The Adapter/A has a 16 bit data path, which accounts for most of     ;
;       the improvement over the 8 bit adapters.  However, some of the       ;
;       improvement comes from the fact that the PS/2 Model 50 runs the bus  ;
;       at 10MHz instead of 8MHz, since it has about a 15% head start even   ;
;       using REP MOVSB.  Timing tests show no significant advantage to      ;
;       using the REP MOVSD form on 386 processors, and taking advantage of  ;
;       it significantly complicates things.                                 ;
;                                                                            ;
;       Although the IBM Token Ring Adapters do not seem to be capable of    ;
;       transmitting at anywhere near the 4Mbps rated capacity of the        ;
;       network, it is possible with multiple transmitters to fill the       ;
;       network to very close to that capacity and the adapters appear to    ;
;       be capable of receiving at the full bandwidth.  An Adapter II        ;
;       installed in a 386/20 easily received at 4.0E6 bits per second.      ;
;                                                                            ;
;----------------------------------------------------------------------------;

                DLOCAL  total_bytes
                DLOCAL  db_remaining
                DLOCAL  db_current,4
                DLOCAL  db_bytes
                DLOCAL  rx_head
                DLOCAL  rx_bytes

;------ early-outs ----------------------------------------------------------;
;
; starting offset is not in first receive buffer.  search for the receive
; buffer it is in and jump back.  ES:SI is the first rx head.  CX is the
; offset.  we enter the loop in the middle.  We can assume the offset is
; somewhere in the packet according to NDIS 2.0.1
;
                public  xfer_offset
@@:             sub     cx,ax                   ; subtract it out
                mov     ax,es:[si].rxbuf_next   ; next buffer
                xchg    ah,al
                sub     ax,2                    ; adjust to start of header
                mov     si,ax                   ; point at it
xfer_offset:
                mov     ax,es:[si].rxbuf_buflen ; next length
                xchg    ah,al
                cmp     cx,ax                   ; offset in this piece?
                jae     @B                      ; nope, loop around again

                mov     [bp].rx_head,si         ; set up new rx head
                sub     ax,cx                   ; subtract out offset
                jmp     short xfer_inside       ; and rejoin

;------ TransferData --------------------------------------------------------;
;
                align   4
TransferData    proc    far
                LOCALS
                PUSHR   si,di,ds
;
; set up DS.
;
                mov     ds,cs:cs_ds
;
; initialize all of the receive buffer variables adjusting for the specified
; start offset.  for the moment assume that the starting offset must be less
; than 256, so that it must be an offset into the first receive buffer.  if
; we find out its not, take an abnormal jump
;
                mov     cx,[bp].td_offset

                mov     si,rxHead
                mov     [bp].rx_head,si

                mov     es,segSM
                mov     ax,es:[si].rxbuf_buflen
                xchg    ah,al
                sub     ax,cx
                jbe     xfer_offset

                public  xfer_inside
xfer_inside:
                mov     [bp].rx_bytes,ax

                add     si,rxbuf_data
                add     si,cx
                mov     dx,si                   ; offset of start data

                mov     ax,rxSize
                sub     ax,[bp].td_offset
                mov     [bp].total_bytes,ax
;
; initialize the bytes copied return parameter to the number of bytes in the
; packet, adjusting for the specified offset
;
                les     di,[bp].td_copied
                mov     es:[di],ax
;
; set db_remaining to the number of frame blocks in the buffer descriptor
; and db_current (and ES:DI) to the address of the first chain entry
;
                les     di,[bp].td_bufdesc
                mov     ax,es:[di].td_data_count
                mov     [bp].db_remaining,ax
                add     di,td_chain
                mov     [bp].db_current.off,di
                mov     [bp].db_current.segm,es
;
; for each chain entry in ES:DI
;
; set db_bytes to the number of bytes in the data block
;
xfer_dbloop:
                mov     cx,es:[di].cd_data_len
                mov     [bp].db_bytes,cx
;
; get the virtual address of the frame data block in ES:DI.  Under OS/2 we
; may be given a physical address which we will have to virtualize using the
; receive GDT selector reserved at INIT time.  if FASTGDT is defined do this
; ourselves in-line (the devhelp calls are just too many instructions).
;
IFDEF OS2
                cmp     es:[di].cd_ptr_type,CT_GDT
                jne     @F
                les     di,es:[di].cd_data_buff
                jmp     short xfer_rxloop
@@:
    IFDEF FASTGDT
                mov     al,es:[di].cd_data_buff.hiword.lobyte
                mov     bx,es:[di].cd_data_buff.loword
                les     di,dword ptr segRX
                mov     es:[di].segd_base15_0,bx
                mov     es:[di].segd_base23_16,al
                mov     es:[di].segd_lim15_0,cx ; CX-1 really, but screw it
                mov     es,di
                xor     di,di
    ELSE
                push    dx
                mov     si,segRX
                mov     ax,es:[di].cd_data_buff.hiword
                mov     bx,es:[di].cd_data_buff.loword
                mov     dl,DevHlp_PhysToGDTSelector
                call    devHelper
                mov     es,si
                xor     di,di
                pop     dx
    ENDIF
ELSE
                les     di,es:[di].cd_data_buff
ENDIF
;
; until the frame data block is filled up, loop through the receive buffers
;
; set CX = min(db_bytes, rx_bytes)
;
xfer_rxloop:
                mov     cx,[bp].db_bytes
                cmp     cx,[bp].rx_bytes
                jb      xfer_remains
                mov     cx,[bp].rx_bytes
;
; update the remaining bytes
;
xfer_remains:
                sub     [bp].total_bytes,cx
                sub     [bp].rx_bytes,cx
                sub     [bp].db_bytes,cx
;
; copy CX bytes from segSM:DX to ES:DI, and update DX for the next time
; around the loop.
;
                mov     si,dx                   ; offset of buffer data
                mov     ds,segSM

                shr     cx,1
        rep     movsw                           ; move words
                adc     cx,cx
        rep     movsb                           ; move remaining byte if odd

                mov     dx,si                   ; update after copy
                mov     ds,cs:cs_ds             ; restore DS
;
; if total_bytes is zero, then we are finished
;
                cmp     word ptr [bp].total_bytes,0
                jz      xfer_copied
;
; if rx_bytes is zero then go on to the next receive buffer
;
                cmp     word ptr [bp].rx_bytes,0
                jnz     xfer_dbnext

                mov     cx,es
                mov     es,segSM
                mov     si,[bp].rx_head
                mov     ax,es:[si].rxbuf_next
                xchg    ah,al
                sub     ax,2                    ; adjust to start of header
                mov     si,ax
                mov     [bp].rx_head,si
                mov     ax,es:[si].rxbuf_buflen
                xchg    ah,al
                mov     [bp].rx_bytes,ax
                mov     es,cx

                add     si,rxbuf_data
                mov     dx,si                   ; new buffer data offset
;
; if db_bytes is zero then go on to the next data block if any (in ES:DI)
;
xfer_dbnext:
                cmp     word ptr [bp].db_bytes,0
                jnz     xfer_rxloop

                add     [bp].db_current.off,CDSIZE
                dec     word ptr [bp].db_remaining
                jz      xfer_depleted
                les     di,[bp].db_current
                jmp     xfer_dbloop
;
; we ran out of frame data blocks before copying all of the packet.  adjust
; the bytes copied parameter
;
xfer_depleted:
                mov     ax,[bp].total_bytes
                les     di,[bp].td_copied
                sub     word ptr es:[di],ax
;
; done.  return SUCCESS.
;
xfer_copied:
                xor     ax,ax
;
; done.  return code in AX
;
                POPR    si,di,ds
                leave
                ret     TDRET
TransferData    endp

NEWPAGE <IndicationOn Entry>
;------ IndicationOn --------------------------------------------------------;
;                                                                            ;
;       int far pascal IndicationOn(WORD MACDS);                             ;
;                                                                            ;
;       This routine enables indications after they have been disabled by    ;
;       IndicationOff.  Since IndicationOn/Off can be nested, a counter      ;
;       is used to keep track of when indications should be enabled again.   ;
;       Therefore the protocol should not rely on indications being          ;
;       immediately enabled on return from this routine.                     ;
;                                                                            ;
;       This function disables interrupts.  It is the protocol's             ;
;       responsibility to enable them on return.  Note that this guarantees  ;
;       that indications will not be generated before this function          ;
;       returns.                                                             ;
;                                                                            ;
;       This function is critical path for DLC.                              ;
;                                                                            ;
;----------------------------------------------------------------------------;

                align   4
IndicationOn    proc    far
                cli
;
; set ES to our data segment
;
                mov     es,cs:cs_ds
;
; sanity checks for development.  ensure that indications aren't already
; on and then make sure they really were off.
;
IFDEF DEV
                cmp     es:offDepth,0
                jnz     @F
                BPOINT  on_already
@@:
                mov     dx,es:IMRport
                in      al,dx
                test    al,es:disableMask
                jnz     @F
                BPOINT  on_notoff
@@:
ENDIF
;
; decrement the nest level.  if non-zero just return.
;
                dec     es:offDepth
                jnz     on_exit
;
; enable interrupts through the the PIC IMR (clear the bit for our int level)
;
                mov     dx,es:IMRport
                in      al,dx
                and     al,es:enableMask
                out     dx,al
;
; done.  return SUCCESS.
;
on_exit:
                xor     ax,ax
                ret     STDRET
IndicationOn    endp

NEWPAGE <IndicationOff Entry>
;------ IndicationOff -------------------------------------------------------;
;                                                                            ;
;       int far pascal IndicationOff(WORD MACDS);                            ;
;                                                                            ;
;       This routine disables indications.  They can be reenabled by a call  ;
;       to IndicationOn.  Since IndicationOn/Off can be nested, a counter    ;
;       is used to keep track of when indications should be enabled again.   ;
;                                                                            ;
;       This function disables interrupts.  It is the protocol's             ;
;       responsibility to enable them on return.                             ;
;                                                                            ;
;       This function is critical path for DLC.                              ;
;                                                                            ;
;----------------------------------------------------------------------------;

                align   4
IndicationOff   proc    far
                cli
;
; set ES to our data segment
;
                mov     es,cs:cs_ds
;
; sanity checks for development.  make sure that indications are really
; in the state we think they are.
;
IFDEF DEV
                mov     dx,es:IMRport
                in      al,dx
                cmp     es:offDepth,0
                jnz     @F
                test    al,es:disableMask
                jz      off_sane
                BPOINT  off_noton
@@:             test    al,es:disableMask
                jnz     off_sane
                BPOINT  off_notoff
off_sane:
ENDIF
;
; increment the nest level
;
                inc     es:offDepth
;
; disable interrupts through the PIC IMR (set the bit for our int level)
;
                mov     dx,es:IMRport
                in      al,dx
                or      al,es:disableMask
                out     dx,al
;
; done.  return SUCCESS.
;
                xor     ax,ax
                ret     STDRET
IndicationOff   endp

NEWPAGE <ReceiveRelease Dummy Entry>
;------ ReceiveRelease ------------------------------------------------------;
;                                                                            ;
;       This entry is provided to conform to the spec requirement that no    ;
;       entry point be NULL.  Since this driver never generates              ;
;       ReceiveChain indications to the protocol, this function should       ;
;       never be invoked.                                                    ;
;                                                                            ;
;----------------------------------------------------------------------------;

ReceiveRelease  proc    far
                mov     ax,GENERAL_FAILURE
                ret     RRRET
ReceiveRelease  endp

NEWPAGE <SnapRx - Snapshot Receive Buffer>
;------ SnapRx --------------------------------------------------------------;
;                                                                            ;
;       void SnapRx()                                                        ;
;                                                                            ;
;       ES:DI = pointer to receive ARB                                       ;
;                                                                            ;
;       Preserves all registers except flags.                                ;
;                                                                            ;
;       Snaps up to 16 bytes of information (packet number, packet size,     ;
;       parts of the packet) into the receive snap area, a circular queue    ;
;       of the last 64 packets received.  Should be enabled only in debug    ;
;       versions.                                                            ;
;                                                                            ;
;----------------------------------------------------------------------------;

IFDEF SNAP
                public  SnapRx
SnapRx          proc    near
                pusha
                push    es

                mov     ax,es:[di].rxarb_buffer
                xchg    ah,al
                mov     cx,es:[di].rxarb_framelen
                xchg    ch,cl
                mov     di,ax
                add     di,rxbuf_data
                mov     es,segSM

                mov     si,rxsnapix
                inc     rxsnapix
                and     rxsnapix,3Fh
                shl     si,1
                shl     si,1
                shl     si,1
                shl     si,1
                add     si,rxsnaparea

                mov     ax,rxarbs
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

                pop     es
                popa
                ret
SnapRx          endp
ENDIF

                END_CODE
                end
