                page    88,122
                title   BIND - System entry and Bind routines (2.0.1 Spec)

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

;------ BIND.ASM ------------------------------------------------------------;
;                                                                            ;
;       This file contains the System entry, and support procedures for the  ;
;       Bind operation.                                                      ;
;                                                                            ;
;       09-21-88 Connery                                                     ;
;       Original version.                                                    ;
;                                                                            ;
;       03-17-89 Connery                                                     ;
;       Support EARLYRELEASE keyword.  16/4 adapter support.  Don't init     ;
;       txbuflen unless set in PROTOCOL.INI.  If still zero at Bind then     ;
;       default it based on maximum packet size supported at the jumpered    ;
;       data rate.  Moved as much logic as possible to INIT time.            ;
;                                                                            ;
;       02-25-90 2.0a Connery                                                ;
;       Dropped multi-adapter support.  Moved some initialization code out   ;
;       of here to init.  Moved the spin logic waiting for ResetAdapter to   ;
;       complete here so ResetAdapter can be used as part of ResetMAC as     ;
;       well.  Trivial performance tweaks.  Use INT 21h to install           ;
;       interrupt vector (its smaller).  Merge InitAdapter into System.      ;
;       Verify FASTGDT assumptions.                                          ;
;                                                                            ;
;       03-19-90 2.0b Connery                                                ;
;       Corrected FASTGDT checking code to zero AH like its supposed to.     ;
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

                public  System

;------ Imported Interfaces -------------------------------------------------;

                OS2XTRN devHelper:DWORD
                OS2XTRN segRX:WORD
                extrn   acked:BYTE
                extrn   common:BYTE
                extrn   cs_ds:WORD
                extrn   earlyRates:BYTE
                extrn   max@16Mbps:WORD
                extrn   max@4Mbps:WORD
                extrn   openHard:WORD
                extrn   pdispatch:BYTE
                extrn   protDS:WORD
                extrn   ramSize:BYTE
                extrn   rate:BYTE
                extrn   retcode:WORD
                extrn   rxbuflen:WORD
                extrn   specific:BYTE
                extrn   state:BYTE
                extrn   status:BYTE
                extrn   timerTicks:WORD
                extrn   txbuflen:WORD
                extrn   txbufs:WORD

                extrn   IntHandler:FAR          ; from intr
                extrn   ResetAdapter:NEAR       ; from util

                BEGIN_CODE

NEWPAGE <System Entry>
;------ System --------------------------------------------------------------;
;                                                                            ;
;       int far pascal System(                                               ;
;                               DWORD Param1,                                ;
;                               DWORD Param2,                                ;
;                               WORD Param3,                                 ;
;                               WORD Opcode,                                 ;
;                               WORD MACDS      );                           ;
;                                                                            ;
;       Used by the protocol manager or the upper protocol during the        ;
;       Binding process.  Only a Bind is expected by the MAC.  For Bind      ;
;       the parameters are:                                                  ;
;                                                                            ;
;               Param1  = far pointer to protocol's cctable                  ;
;               Param2  = far pointer to address to set to MAC's cctable     ;
;               Param3  = unused                                             ;
;                                                                            ;
;       After this function returns successfully, the MAC is ready for       ;
;       use, though the adapter still needs to be openned and a non-zero     ;
;       packet filter set to get transmission and reception going.           ;
;                                                                            ;
;----------------------------------------------------------------------------;

System          proc    far
                push    bp
                mov     bp,sp
                PUSHR   si,di,ds
;
; set up DS.
;
                mov     ds,cs:cs_ds
;
; fail anything other than a bind request
;
                cmp     [bp].sy_opcode,BIND
                je      @F
sy_fail:
                mov     ax,GENERAL_FAILURE
                jmp     sy_exit
@@:
;
; ensure the state is correct
;
                cmp     state,INITIALIZED
                je      @F
                mov     ax,DRIVER_NOT_INITIALIZED
                jmp     sy_exit
@@:
;
; if we're going to do PhysToGDT in-line, verify that our assumptions are
; valid.  make sure OS2_GDT points to the GDT.
;
IFDEF FASTGDT
                push    bp
                sub     sp,6
                mov     bp,sp
                sgdt    fword ptr [bp]
                mov     cx,[bp].sgdt_limit
                inc     cx
                xor     ah,ah
                mov     al,[bp].sgdt_base23_16
                mov     bx,[bp].sgdt_base15_0
                add     sp,6
                pop     bp
                mov     si,segRX
                mov     dl,DevHlp_PhysToGDTSelector
                call    devHelper
                jc      sy_fail
                mov     es,si
                cmp     es:[OS2_GDT].segd_base15_0,bx
                jne     sy_incompatible
                cmp     es:[OS2_GDT].segd_base23_16,al
                je      @F
sy_incompatible:
                mov     ax,INCOMPATIBLE_OS_VERSION
                jmp     sy_exit
@@:
ENDIF
;
; set the bound bit in the status field
;
                or      status.ms_flags.loword,ST_MAC_IS_BOUND
;
; return common characteristics table address
;
                les     di,[bp].sy_param2
                mov     es:[di].segm,ds
                mov     es:[di].off,offset DGROUP:common
;
; save the protocol's module dataseg
;
                les     di,[bp].sy_param1
                mov     ax,es:[di].cc_DS
                mov     protDS,ax
;
; copy the protocol's lower dispatch table to pdispatch
;
                push    ds
                lds     si,es:[di].cc_ldispatch
                pop     es
                mov     di,offset DGROUP:pdispatch
                mov     cx,PDTABLESIZE
        rep     movsb
                push    es
                pop     ds
;
; install the hardware interrupt handler
;
IFDEF DOS
                mov     al,specific.mc_interrupt.lobyte
                mov     cl,08h                  ; IRQ 0 = INT 08h
                cmp     al,07h                  ; IRQ 0 to 7 ?
                jbe     @F
                mov     cl,70h - 08h            ; IRQ 8 = INT 70h
@@:             add     al,cl
                mov     ah,25h                  ; Set Interrupt Vector
                mov     dx,offset CGROUP:IntHandler ; Assume DS = CS
                int     21h
ELSE
                mov     bx,specific.mc_interrupt
                mov     ax,offset CGROUP:IntHandler
                mov     dx,(0*256)+DevHlp_SetIRQ; unshared interrupt
                call    devHelper
                jnc     @F                      ; interrupt in use?
                mov     ax,INTERRUPT_CONFLICT   ; return error
                jmp     sy_exit
@@:
ENDIF
;
; Reset the adapter
;
                call    ResetAdapter
;
; Reenable interrupts so that the response to the reset and the timer tick
; interrupts can come in from the adapter
;
                sti
;
; Wait for the initialization to complete.  This normally takes about 500ms
; on the original adapters, but seems to take almost 2s on the 16/4 adapters
; so we wait for 1000 ticks (10 seconds) before giving up.  We also want to
; give up if no interrupts are happening at all, so we keep track of the
; number of times through the loop.
;
                xor     cx,cx
                xor     dx,dx                   ; DX:CX is dword counter
                align   4
@@:             cmp     acked,YES
                je      sy_complete
                cmp     timerTicks,1000
                jae     sy_timeout
                add     cx,1
                adc     dx,0
                cmp     dx,200                  ; 200 x 65,536 times = 13E6
                jb      @B                      ; or about 10s on 40MHz 386
;
; reset failed, too much time went by.
;
                public  sy_timeout
sy_timeout:
                mov     ax,HARDWARE_FAILURE
                jmp     sy_exit
;
; the initialization complete message came in from the processor, check the
; bring up code to see if it was successful
;
                public  sy_complete
sy_complete:
                mov     ax,HARDWARE_FAILURE
                cmp     retcode,0
                SJNE    sy_exit
;
; adjust the linkspeed if 16Mbps (16000000d = F42400h)
;
                cmp     rate,16
                jne     @F
                mov     specific.mc_linkspeed.hiword,00F4h
                mov     specific.mc_linkspeed.loword,2400h
@@:
;
; if early token release not supported at current rate turn it off
;
                mov     al,rate                 ; current data rate
                test    earlyRates,al           ; early release at this rate?
                jnz     @F
                and     openHard,NOT OPEN_EARLYTOKEN
@@:
;
; limit the transmit buffer size to the maximum allowed at jumpered rate
;
                mov     ax,txbuflen
                mov     bx,max@4Mbps            ; set BX = maximum frame size
                cmp     rate,16
                jne     @F
                mov     bx,max@16Mbps           ; for the current data rate
@@:             cmp     ax,bx                   ; take minimum
                jb      @F
                mov     ax,bx
@@:             mov     txbuflen,ax             ; and use it.
;
; fill in the transmit and receive total space and block sizes.  we filter
; out the token ring adapter overhead and the overhead per transmit or
; receive buffer in doing this.
;
                mov     ax,txbuflen             ; size of tx buffer
                mov     specific.mc_txblksize,ax
                mov     specific.mc_maxsize,ax
                mul     txbufs                  ; x number of tx buffers
                mov     specific.mc_totaltxspc.loword,ax ; = tx buffer space

                mov     al,ramSize              ; size of SRAM in K
                xor     ah,ah
                mov     cl,10                   ; SHL 10 = multiply by 1024
                shl     ax,cl                   ; size of SRAM in bytes (0=64K)
                sub     ax,ADAPTER_OVERHEAD     ; - adapter overhead
                mov     bx,ax                   ; = buffer space

                mov     ax,txbuflen             ; tx buffer size
                add     ax,TXBUF_OVERHEAD       ; + overhead
                mul     txbufs                  ; x tx buffers = tx buffer space
                sub     bx,ax                   ; buffer space - tx buffer space
                mov     ax,CONFIGURATION_FAILURE
                jbe     sy_exit                 ; < 0 means no space left
                mov     ax,bx                   ; = rx buffer space

                mov     cx,rxbuflen             ; rx buffer size
                add     cx,RXBUF_OVERHEAD       ; + overhead = bytes each
                div     cx                      ; rx buffer space / bytes each
                                                ; = number of rx buffers
                mul     rxbuflen                ; x rx buffer size
                mov     specific.mc_totalrxspc.loword,ax ; = rx buffer space
;
; return success.
;
                xor     ax,ax
;
; done.  return code in AX
;
                public  sy_exit
sy_exit:
                POPR    si,di,ds
                pop     bp
                ret     SYRET
System          endp

                END_CODE
                end
