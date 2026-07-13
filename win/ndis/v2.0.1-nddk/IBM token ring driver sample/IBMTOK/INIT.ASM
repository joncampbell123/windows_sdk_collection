                page    88,122
                title   INIT - Strategy routine and initialization (2.0.1 Spec)

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

;------ INIT.ASM ------------------------------------------------------------;
;                                                                            ;
;       This file contains the device strategy routine (where the INIT       ;
;       packet is received), and any initialization routines.  Aside from    ;
;       the strategy routine, most of the functions here should not be       ;
;       called except at INIT time as they are discarded.                    ;
;                                                                            ;
;       09-21-88 Connery                                                     ;
;       Original version.                                                    ;
;                                                                            ;
;       03-17-89 Connery                                                     ;
;       Added significant amounts of logic to identify 16/4 adapters and     ;
;       their characteristics (max packet size, data rates, size of SRAM,    ;
;       paging, early token release, etc).  Some of this logic was           ;
;       originally in Bind but as long as we don't touch the hardware        ;
;       until Bind we remain Start compatible.                               ;
;                                                                            ;
;       01-02-90 1.1d Connery                                                ;
;       Changed the protman version check to version 1 OR GREATER to allow   ;
;       the code to work with protman 2.0 (stupid)!                          ;
;                                                                            ;
;       02-25-90 2.0a Connery                                                ;
;       Always set mc_paddr to the burned-in address.  Export transmit       ;
;       queue depth in specific characteristics.  Allow RAM=3Com and         ;
;       RAM=IBM as well as RAM=0xABCD.  For RAM=IBM set the RAM location     ;
;       like IBM does.  Required a new RHS_ANYTHING option, and some         ;
;       changes to ParseKeyword.  Also limit the RAM location to A000-EC00   ;
;       instead of A000-FC00.  Removed multiple adapter support.  Added      ;
;       FASTGDT initialization code.  In order to initialize the QSPACE at   ;
;       INIT time rather than Bind (saving space), relocate the init code    ;
;       out of the way when necessary under DOS.  Set up more data           ;
;       statically.  Default txbufs, txbuflen at INIT time and just limit    ;
;       them at Bind for the jumpered data rate.  Default rxbuflen more      ;
;       intelligently.                                                       ;
;                                                                            ;
;       04-24-90 2.0c Connery                                                ;
;       Modify the relocation code which moves the initialization code out   ;
;       of the way of the volatile data to copy backwards (STD).  If an      ;
;       insufficient number of transmit queue entries are allocated the      ;
;       copy will overlap and create problems.                               ;
;                                                                            ;
;----------------------------------------------------------------------------;

.xlist
IFDEF OS2
include devsym.inc
include devhlp.inc
ENDIF
include includes.mac
.list
.lfcond
.sall

;------ Exported Interfaces -------------------------------------------------;

                DOSPUBL devInterrupt
                public  AdapterExists
                public  CheckIDString
                public  GetHardAddress
                public  ParseConfig
                public  ParseKeyword
                public  ParseNetAddr
                public  ParseProductID
                public  PutKeyword
                public  PutStrang
                public  PutString
                public  StringEquals
                public  devInit
                public  devStrategy

;-------Imported Interfaces -------------------------------------------------;

                OS2XTRN DOSCLOSE:FAR            ; OS/2 API Function
                OS2XTRN DOSDEVIOCTL:FAR         ; OS/2 API Function
                OS2XTRN DOSOPEN:FAR             ; OS/2 API Function
                OS2XTRN DOSPUTMESSAGE:FAR       ; OS/2 API Function
                OS2XTRN KBDCHARIN:FAR           ; OS/2 API Function
IFDEF ANYSTATS
                OS2XTRN DOSGETINFOSEG:FAR       ; OS/2 API Function
ENDIF

                DOSXTRN DDStkOff:WORD
                DOSXTRN DDStkSeg:WORD
                DOSXTRN DDTopStk:WORD
                DOSXTRN saved_bx:WORD
                DOSXTRN saved_es:WORD
                OS2XTRN CharData:BYTE
                OS2XTRN devHelper:DWORD
                OS2XTRN dummy:WORD
                OS2XTRN physDS:DWORD
                OS2XTRN segRX:WORD
                OS2XTRN segTX:WORD
IFDEF ANYSTATS
                OS2XTRN infoSeg:WORD
                DOSXTRN TickHandler:FAR
                DOSXTRN timerChain:DWORD
ENDIF
                extrn   EOIs:WORD
                extrn   IDString:BYTE
                extrn   IDString_A:BYTE
                extrn   IMRport:WORD
                extrn   IntLevel:BYTE
                extrn   IntLevel_A:BYTE
                extrn   action_taken:WORD
                extrn   adapterBits:BYTE
                extrn   charhex:NEAR
                extrn   common:BYTE
                extrn   cs_ds:WORD
                extrn   days:WORD
                extrn   devHeader:WORD
                extrn   disableMask:BYTE
                extrn   earlyRates:BYTE
                extrn   early_table:BYTE
                extrn   enableMask:BYTE
                extrn   hexchar:NEAR
                extrn   keyword_table:WORD
                extrn   max16_table:WORD
                extrn   max4_table:WORD
                extrn   max@16Mbps:WORD
                extrn   max@4Mbps:WORD
                extrn   mdispatch:BYTE
                extrn   msg_badconfigure:BYTE
                extrn   msg_closeparen:BYTE
                extrn   msg_crlf:BYTE
                extrn   msg_equals:BYTE
                extrn   msg_generalfailure:BYTE
                extrn   msg_invalidrhs:BYTE
                extrn   msg_moretocome:BYTE
                extrn   msg_nohardware:BYTE
                extrn   msg_nosuchkeyword:BYTE
                extrn   msg_openparen:BYTE
                extrn   msg_preamble:BYTE
                extrn   msg_pressakey:BYTE
                extrn   msg_quote:BYTE
                extrn   msg_signon:BYTE
                extrn   number_buffer:BYTE
                extrn   openHard:WORD
                extrn   our_name:WORD
                extrn   page_table:BYTE
                extrn   portOffset:WORD
                extrn   primAlt:BYTE
                extrn   productid:BYTE
                extrn   protmgr_handle:WORD
                extrn   protmgr_name:BYTE
                extrn   q_rqfree:BYTE
                extrn   q_txawaits:BYTE
                extrn   q_txfree:BYTE
                extrn   qrarray:BYTE
                extrn   qtarray:BYTE
                extrn   ramSize:BYTE
                extrn   ram_3Com:BYTE
                extrn   ram_IBM:BYTE
                extrn   ram_method:BYTE
                extrn   rate_table:BYTE
                extrn   realMMIO:WORD
                extrn   realSM:WORD
                extrn   releasePort:WORD
                extrn   rqblock:WORD
                extrn   rrr_table:BYTE
                extrn   rxbuflen:WORD
                extrn   rxbufs:WORD
                extrn   second_name:BYTE
                extrn   segMMIO:WORD
                extrn   segSM:WORD
                extrn   serialnumber:BYTE
                extrn   specific:BYTE
                extrn   sram_table:BYTE
                extrn   state:BYTE
                extrn   status:BYTE
                extrn   txbuflen:WORD
                extrn   txbufs:WORD
                extrn   upcase:NEAR

NEWPAGE <devStrategy - device Strategy Routine>
;------ devStrategy ---------------------------------------------------------;
;                                                                            ;
;       This is the device driver's strategy routine, invoked by the         ;
;       operating system to initialize the driver and handle IOCTL requests. ;
;       In this driver the only request packet we expect to see is the INIT  ;
;       packet, since all other communication is done through direct         ;
;       procedure calls to macEntry from other ring 0 device drivers.        ;
;                                                                            ;
;       inputs:                                                              ;
;               es:bx = pointer to request packet                            ;
;                                                                            ;
;       All registers are saved and restored by OS/2.                        ;
;                                                                            ;
;----------------------------------------------------------------------------;

                BEGIN_CODE

devStrategy     proc    far
;
; under DOS we merely save the packet for the interrupt routine
;
IFDEF DOS
                mov     cs:saved_bx,bx          ; save bx
                mov     cs:saved_es,es          ; save es
                ret
devStrategy     endp

devInterrupt    proc    far
                PUSHR   ax,bx,cx,dx,si,di,ds,es,bp
;
; since we only have about 20 bytes of stack space, switch stacks
;
                cli
                mov     DDStkSeg,ss
                mov     DDStkOff,sp
                mov     ax,cs
                mov     ss,ax
                mov     sp,offset DGROUP:DDTopStk
                sti

                mov     ds,ax                   ; set DS = CS
                les     bx,dword ptr saved_bx   ; restore ES:BX
ENDIF
;
; invoke periscope to load symbols
;
IFDEF DOS
IFDEF DEV
                jmp     @F
                public  pssname
pssname         db      'D:\TOK\DOS\IBMTOK.PSS',0
@@:             pushr   es,bx
                mov     bx,cs
                mov     dx,offset DGROUP:pssname
                int     67h
                popr    es,bx
ENDIF
ENDIF

;------ common DOS and OS/2 code --------------------------------------------;
;
; clear the direction flag to conform to Microsoft C calling conventions
;
                cld
;
; check the packet type
;
                cmp     es:[bx].PktCmd,CMDInit  ; Init packet?
                je      strat_init
;
; bad packet
;
                mov     es:[bx].PktStatus,STDON+STERR+3
;
; exit.  restore registers under DOS and return.
;
strat_exit:
IFDEF DOS
                cli
                mov     ss,DDStkSeg             ; switch stacks back
                mov     sp,DDStkOff
                sti

                POPR    ax,bx,cx,dx,si,di,ds,es,bp
ENDIF
                ret
;
; device driver initialization.  under DOS we relocate this routine along
; the way so fake a far call.
;
                public  strat_init
strat_init:
IFDEF DOS
                push    cs
ENDIF
                call    devInit
                jmp     strat_exit
;
; under DOS we relocate the INIT code out of the way of the QSPACE in order
; to initialize it.  to do this it will branch here with everything set up
; for the copy and a return address on the stack.
;
IFDEF DOS
                public  strat_copy
strat_copy:
        rep     movsb
                retf
ENDIF
;
; end of whichever procedure we're in.
;
IFDEF DOS
devInterrupt    endp
ELSE
devStrategy     endp
ENDIF

                END_CODE

NEWPAGE <devInit - device driver initialization>
;------ devInit -------------------------------------------------------------;
;                                                                            ;
;       void devInit()                                                       ;
;                                                                            ;
;       ES:BX = pointer to init packet                                       ;
;                                                                            ;
;       Only the critical registers are saved.                               ;
;                                                                            ;
;       Device driver initialization routine.  Called from the device driver ;
;       Strategy Routine when an init packet is received from OS/2.          ;
;                                                                            ;
;       This routine retrieves the configuration pointer from the protocol   ;
;       manager and uses it to configure the driver.  If successful, it      ;
;       registers itself with the protocol manager.                          ;
;                                                                            ;
;       To allow two copies of the driver to be installed we first attempt   ;
;       to open our own device name.  If that is successful then we use an   ;
;       alternate name.                                                      ;
;                                                                            ;
;       This code is discarded at the end of initialization.                 ;
;                                                                            ;
;----------------------------------------------------------------------------;

                BEGIN_INIT_CODE

                DLOCAL  device_packet
                DLOCAL  our_handle
IFDEF DOS
                DLOCAL  year
                DLOCAL  month
                DLOCAL  day
                DLOCAL  hour
                DLOCAL  minute
                DLOCAL  second
                DLOCAL  since,dword
ENDIF

devInit         proc    near
                LOCALS
;
; save packet address
;
                mov     [bp].device_packet.off,bx
                mov     [bp].device_packet.segm,es
;
; display signon message
;
                mov     di,offset DGROUP:msg_signon
                call    PutStrang

                mov     di,offset DGROUP:msg_crlf
                call    PutStrang
;
; try and open our device name.  if not installed then just go on as normal
;
IFDEF DOS
                mov     dx,our_name
                mov     ax,3DC2h                ; open for private read
                int     21h                     ; call DOS to open
                jc      init_named
                mov     word ptr [bp].our_handle,ax
ELSE
                push    ds
                push    our_name
                push    ss
                lea     ax,[bp].our_handle
                push    ax
                push    ds
                push    offset DGROUP:action_taken
                push    0
                push    0                       ; file size (long)
                push    0                       ; file attribute
                push    1                       ; open flag: file must exist
                push    0C2h                    ; open mode (share R/W private)
                push    0
                push    0                       ; 0L reserved
                call    DOSOPEN
                test    ax,ax
                jnz     init_named
ENDIF
;
; close the other token ring driver
;
IFDEF DOS
                mov     bx,word ptr [bp].our_handle
                mov     ah,3Eh
                int     21h
ELSE
                push    word ptr [bp].our_handle
                call    DOSCLOSE
ENDIF
;
; change the device name we'll use to allow a second copy to be installed
;
                mov     our_name,offset DGROUP:second_name
;
; copy the name to be used to the device header
;
init_named:
                mov     ax,ds                   ; compute strlen(our_name)
                mov     es,ax
                mov     di,our_name
                xor     ax,ax
                mov     cx,ax
                dec     cx                      ; set maximum to FFFFh
        repne   scasb                           ; scan for null byte
                not     cx                      ; 1's complement remaining count
                dec     cx                      ; don't count null byte

                mov     si,our_name
                mov     di,offset DGROUP:devHeader.SDevName
        rep     movsb                           ; copy it
;
; obtain address of DOS Device Helper routine and save in devHelper
;
IFDEF OS2
                les     bx,[bp].device_packet
                mov     ax,es:[bx].InitDevHlp.off
                mov     dx,es:[bx].InitDevHlp.segm
                mov     devHelper.off,ax
                mov     devHelper.segm,dx
ENDIF
;
; set up cs_ds to point at our data segment
;
IFDEF DOS
                mov     cs:cs_ds,ds
ELSE
                cli                             ; Protect PhysToVirt
                push    ds
                pop     es                      ; ES = DS
                push    cs
                pop     ds
                mov     si,offset CGROUP:cs_ds  ; DS:SI = address of cs_ds
                mov     dl,DevHlp_VirtToPhys    ; get AX:BX = physical cs_ds
                call    es:devHelper
                push    es
                pop     ds                      ; restore DS
                mov     cx,SIZE cs_ds           ; only one word writable
                mov     dh,1                    ; return in ES:DI
                mov     dl,DevHlp_PhysToVirt    ; writable selector please
                call    devHelper
                L0DS                            ; protect mode dataseg
                mov     es:[di].loword,ax       ; save it in CS:cs_ds
                mov     dl,DevHlp_UnPhysToVirt  ; release selector to OS/2
                call    devHelper
                sti                             ; UnProtect
ENDIF
;
; open the Protocol Manager
;
IFDEF DOS
                mov     dx,offset CGROUP:protmgr_name
                mov     ax,3DC2h                ; open for private read
                int     21h                     ; call DOS to open
                SJC     init_fail
                mov     protmgr_handle,ax
ELSE
                push    ds
                push    offset DGROUP:protmgr_name
                push    ds
                push    offset DGROUP:protmgr_handle
                push    ds
                push    offset DGROUP:action_taken
                push    0
                push    0                       ; file size (long)
                push    0                       ; file attribute
                push    1                       ; open flag: file must exist
                push    0C2h                    ; open mode (share R/W private)
                push    0
                push    0                       ; 0L reserved
                call    DOSOPEN
                test    ax,ax
                SJNZ    init_fail
ENDIF
;
; invoke Protocol Manager with GET_PROTOCOL_MANAGER_INFO request block
;
                mov     rqblock.rb_opcode,GET_PROTMAN_INFO
IFDEF DOS
                mov     bx,protmgr_handle       ; handle into bx
                mov     cx,SIZE ReqBlock        ; number of bytes to read
                mov     dx,offset CGROUP:rqblock ; buffer to read into
                mov     ax,4402h                ; IOCTL read call
                int     21h                     ; call DOS
                SJC     init_fail               ; did IOCTL fail?
ELSE
                push    0
                push    0                       ; data area address (none)
                push    ds
                push    offset DGROUP:rqblock   ; request block address
                push    IONC_PM                 ; device function (pr'cl mgmt)
                push    IOC_NC                  ; device category (net control)
                push    protmgr_handle          ; handle
                call    DOSDEVIOCTL
                test    ax,ax                   ; check OS return code
                SJNZ    init_fail
ENDIF
                mov     ax,rqblock.rb_status
                test    ax,ax                   ; check protmgr return code
                SJNZ    init_fail
;
; check the protocol manager version number against what we expect
;
                cmp     rqblock.rb_word1,PMGRVER_NEED
                SJB     init_fail
;
; parse the configuration image returned
;
                les     bx,rqblock.rb_pointer1
                call    ParseConfig
                test    ax,ax
                SJNZ    init_fail
;
; initialize the entries in the common module characteristics table that
; we couldn't initialize statically (such as pointers which need the ring
; 0 data selector rather than the current ring 3 selector).
;
                L0DS
                mov     common.cc_DS,ax
                mov     common.cc_specific.segm,ax
                mov     common.cc_status.segm,ax
                mov     common.cc_udispatch.segm,ax

                L0CS
                mov     common.cc_system.segm,ax
;
; initialize entries in the MAC characteristics table (the permanent and
; current node addresses will have to wait until Bind time).  we'll fill
; in the transmit and receive space numbers after we get some info from
; the adapter itself.
;
                L0DS
                mov     specific.mc_multicast.segm,ax
                mov     specific.mc_serial.segm,ax
;
; initialize entries in the MAC dispatch table
;
                L0DS
                mov     mdispatch.md_cctable.segm,ax

                L0CS
                mov     mdispatch.md_request.segm,ax
                mov     mdispatch.md_txchain.segm,ax
                mov     mdispatch.md_txferdata.segm,ax
                mov     mdispatch.md_rxrelease.segm,ax
                mov     mdispatch.md_indication_on.segm,ax
                mov     mdispatch.md_indication_off.segm,ax
;
; invoke Protocol Manager with REGISTER_MODULE for this adapter.
;
; when passing the pointer to the MAC's common module we have to supply the
; protected mode DS selector rather than the current ring 3 DS value.
;
; also note that the tables we are providing access to may not yet be
; completely filled in which is okay since the protocol manager doesn't
; look at it, and the protocol won't get it until binding with us.
;
                mov     rqblock.rb_opcode,REGISTER_MODULE
                mov     rqblock.rb_pointer2.off,0
                mov     rqblock.rb_pointer2.segm,0
                mov     rqblock.rb_pointer1.off,offset DGROUP:common
                L0DS
                mov     rqblock.rb_pointer1.segm,ax
IFDEF DOS
                mov     bx,protmgr_handle       ; handle into bx
                mov     cx,SIZE ReqBlock        ; number of bytes to read
                mov     dx,offset CGROUP:rqblock ; buffer to read into
                mov     ax,4402h                ; IOCTL read call
                int     21h                     ; call DOS
                SJC     init_fail               ; did IOCTL fail?
ELSE
                push    0
                push    0                       ; data area address (none)
                push    ds
                push    offset DGROUP:rqblock   ; request block address
                push    IONC_PM                 ; device function (pr'cl mgmt)
                push    IOC_NC                  ; device category (net control)
                push    protmgr_handle          ; handle
                call    DOSDEVIOCTL
                test    ax,ax                   ; check OS return code
                sjnz    init_fail
                mov     ax,rqblock.rb_status
                test    ax,ax                   ; check protmgr return code
                sjnz    init_fail
ENDIF
;
; reserve GDT selectors for Token Ring Adapter memory access: segMMIO for
; MMIO access, segSM for shared memory access and segTX and segRX for doing
; PhysToGDT for the transmit and receive sides.  Don't assume they're
; contiguous.
;
IFDEF OS2
                mov     cx,1
                push    ds
                pop     es
                mov     di,offset DGROUP:segMMIO
                mov     dl,DevHlp_AllocGDTSelector
                call    devHelper
                SJC     init_fail

                mov     di,offset DGROUP:segSM
                call    devHelper

                mov     di,offset DGROUP:segTX
                call    devHelper

                mov     di,offset DGROUP:segRX
                call    devHelper
ENDIF
;
; if FASTGDT is defined, set physDS to the physical address of DS:0
;
IFDEF FASTGDT
                xor     si,si                   ; DS:SI is virtual address
                mov     dl,DevHlp_VirtToPhys
                call    devHelper
                SJC     init_fail
                mov     physDS.hiword,ax
                mov     physDS.loword,bx
ENDIF
;
; and point segTX and segRX at DS:0 to get all the fields initialized the way
; OS/2 wants them (so we can just fill in the offset and limit if we decide to
; do the PhysToGDT's ourselves, and not worry about the other fields).
;
; we could also fill in the limit field ahead of time, but filling it in
; correctly is just too useful as far as catching pointer errors.
;
IFDEF FASTGDT
                mov     cx,1                    ; unusable for now
                mov     si,segTX
                mov     dl,DevHlp_PhysToGDTSelector
                call    devHelper
                SJC     init_fail

                mov     ax,physDS.hiword        ; reload just in case
                mov     si,segRX
                call    devHelper
                SJC     init_fail
ENDIF
;
; if the adapter type is known, just check if it exists.  if not known try
; PRIMARY first, then ALTERNATE if not found.
;
                cmp     primAlt,ALTERNATE
                je      init_tryalt
                mov     al,PRIMARY
                call    AdapterExists
                jnc     init_found
                cmp     primAlt,PRIMARY
                SJE     init_nohardware
init_tryalt:
                mov     al,ALTERNATE
                call    AdapterExists
                SJC     init_nohardware
init_found:
                test    ax,ax                   ; configuration error?
                SJNZ    init_problem
;
; if the txbuflen and txbufs parameters have not been set, default them in
; a halfway reasonable fashion.  we'll only allocate two txbufs if there's
; enough memory.
;
                cmp     txbufs,0                ; if txbufs != 0
                jne     init_txbuflen           ; just check txbuflen
                mov     cx,1                    ; default is txbufs=1
                cmp     txbuflen,0              ; txbufs = 0.  is txbuflen = 0?
                jne     init_compute            ; if not, goto compute
                cmp     ramSize,16              ; both zero.  if SRAM < 16K
                jb      init_txbufs             ; then use txbufs=1
                jmp     short init_txbufs_2     ; else use txbufs=2
init_compute:   cmp     ramsize,16              ; txbufs = 0, txbuflen != 0
                jb      init_txbufs             ; if SRAM < 16K use txbufs=1
                mov     al,ramSize              ; SRAM size in K
                xor     ah,ah
                mov     cl,7                    ; x 1024 / 8
                shl     ax,cl                   ; (1/8) SRAM size
                mov     cx,1                    ; default again
                cmp     txbuflen,ax             ; if txbuflen > (1/8) SRAM size
                ja      init_txbufs             ; then use txbufs=1
init_txbufs_2:  mov     cx,2                    ; use txbufs=2
init_txbufs:    mov     txbufs,cx               ; CX = number of txbufs

init_txbuflen:  cmp     txbuflen,0              ; if txbuflen = 0
                jnz     init_txbufed
                mov     al,ramSize              ; SRAM size in K
                xor     ah,ah
                mov     cl,8                    ; x 1024 / 4
                shl     ax,cl                   ; (1/4) SRAM size
                mov     cl,txbufs.lobyte        ; divided by txbufs
                dec     cl
                shr     ax,cl
                mov     txbuflen,ax             ; is txbuflen
init_txbufed:
;
; if rxbuflen has not been set, default it to max(256, ramSize/64) so we get
; larger chunks with bigger SRAM sizes.
;
                mov     ax,rxbuflen
                test    ax,ax                   ; rxbuflen specified?
                jnz     @F
                mov     al,ramSize              ; SRAM size in K
                xor     ah,ah
                mov     cl,4                    ; x 1024 / 64
                shl     ax,cl                   ; (1/64) SRAM size
                cmp     ax,256                  ; take min with 256
                jae     @F
                mov     ax,256
@@:             mov     rxbuflen,ax
;
; fill in whatever else we can before Bind
;
                mov     ax,rxbuflen
                mov     specific.mc_rxblksize,ax
                mov     specific.mc_totalrxspc.hiword,0
                mov     specific.mc_totaltxspc.hiword,0
;
; initialize the date/time statistics last cleared to the current time, in
; number of seconds since January 1, 1970.  under OS/2 we can just get this
; out of the InfoSeg.
;
IFDEF ANYSTATS
IFDEF OS2
                push    ds
                push    offset DGROUP:infoSeg
                push    ds
                push    offset DGROUP:dummy
                call    DOSGETINFOSEG

                mov     es,infoSeg
                mov     ax,es:[is_elapsed].loword
                mov     dx,es:[is_elapsed].hiword
                mov     status.ms_cdate.loword,ax
                mov     status.ms_cdate.hiword,dx
ENDIF
ENDIF
;
; under DOS this is MUCH harder.  the only date/time we have is in the usual
; units, and we'll have to convert.  we'll use the same logic as the Microsoft
; C 5.1 time function, though we'll ignore such issues as time zone and
; daylight savings time.  and leap years are easy for the next century or
; so, since the every 4, but not every century, but every 400 years means that
; the every 4 rule works for the year 2000.
;
; note the check for date changed around getting time--otherwise we could be
; off by 24 hours.
;
IFDEF ANYSTATS
IFDEF DOS
                mov     [bp].year.loword,0
                mov     [bp].month.loword,0
                mov     [bp].day.loword,0
                mov     [bp].hour.loword,0
                mov     [bp].minute.loword,0
                mov     [bp].second.loword,0

@@:             mov     ah,2Ah                  ; get date
                int     21h                     ; CX=year DH=month DL=day
                mov     [bp].year.loword,cx
                mov     [bp].month.lobyte,dh
                mov     [bp].day.lobyte,dl

                mov     ah,2Ch                  ; get time
                int     21h                     ; CH=hour CL=min DH=sec
                mov     [bp].hour.lobyte,ch
                mov     [bp].minute.lobyte,cl
                mov     [bp].second.lobyte,dh

                mov     ah,2Ah                  ; get date again
                int     21h
                cmp     cx,[bp].year.loword     ; still the same?
                jne     @B                      ; nope, try again
                cmp     dh,[bp].month.lobyte
                jne     @B
                cmp     dl,[bp].day.lobyte
                jne     @B

                sub     [bp].year.loword,1980   ; offset year from 1980

                mov     cx,[bp].year.loword     ; CX=(year+3)/4
                add     cx,3
                shr     cx,1
                shr     cx,1

                mov     bx,0001h                ; BX:AX = 00015180h
                mov     ax,5180h                ;       = 24 * 60 * 60
                                                ;       = seconds/day

                mul     cx                      ; since = (year+3)/4 * secs/day
                mov     [bp].since.hiword,dx
                mov     [bp].since.loword,ax
                mov     ax,bx
                mul     cx
                add     [bp].since.hiword,ax

                mov     bx,[bp].month.loword    ; BX=month
                dec     bx
                shl     bx,1
                mov     ax,days[bx]             ; AX=days[month-1]
                cmp     [bp].month.loword,2     ; if month > february
                jbe     @F
                test    [bp].year.loword,3      ; and year is leap
                jnz     @F
                inc     ax                      ; then add one more day
@@:             mov     cx,ax                   ; monthdays in CX for now

                mov     ax,[bp].year.loword     ; AX=year
                mov     bx,365
                mul     bx                      ; AX=year*365
                add     ax,[bp].day.loword      ; AX=year*365+day
                add     ax,cx                   ; AX=year*365+day+monthdays
                mov     cx,ax                   ; daysince in CX for now

                mov     bx,0001h                ; BX:AX = 00015180h
                mov     ax,5180h                ;       = 24 * 60 * 60
                                                ;       = seconds/day
                mul     cx                      ; since += daysince * secs/day
                add     [bp].since.loword,ax
                adc     [bp].since.hiword,dx
                mov     ax,bx
                mul     cx
                add     [bp].since.hiword,ax

                mov     ax,[bp].hour.loword     ; AX = hour
                mov     cx,3600
                mul     cx                      ; DX:AX = hour*3600
                add     [bp].since.loword,ax    ; update since
                adc     [bp].since.hiword,dx

                mov     ax,[bp].minute.loword   ; AX = minute
                mov     cx,60
                mul     cx                      ; AX = minute*60
                add     [bp].since.loword,ax    ; update since
                adc     [bp].since.hiword,dx

                mov     ax,[bp].second.loword   ; AX = second
                add     [bp].since.loword,ax    ; update since
                adc     [bp].since.hiword,0

                add     [bp].since.loword,0A600h; 12CEA600 = seconds in 1970-79
                adc     [bp].since.hiword,012CEh; update since

                mov     ax,[bp].since.loword    ; update status table clear time
                mov     dx,[bp].since.hiword
                mov     status.ms_cdate.loword,ax
                mov     status.ms_cdate.hiword,dx
ENDIF
ENDIF
;
; since DOS is not reentrant, we'll just accumulate ticks to add to this time
; every time ClearStatistics is issued.  install our own tick handler in the
; chain for INT 08h
;
IFDEF ANYSTATS
IFDEF DOS
                mov     ax,3508h                ; get interrupt vector 08h
                int     21h                     ; returns ES:BX
                mov     timerChain.segm,es      ; and save it
                mov     timerChain.off,bx

                mov     ax,2508h                ; install a interrupt handler
                mov     dx,offset CGROUP:TickHandler
                int     21h                     ; takes DS:DX
ENDIF
ENDIF
;
; change state to initialized
;
                mov     state,INITIALIZED
;
; successful initialization.  close the protocol manager.
;
IFDEF DOS
                mov     bx,protmgr_handle
                mov     ah,3Eh
                int     21h
ELSE
                push    protmgr_handle
                call    DOSCLOSE
ENDIF
;
; return terminating addresses of code and data under OS/2 or the end of
; driver under DOS (assumes that LASTCODE precedes LASTDATA in CGROUP).  to
; determine this we need to retain exactly the right number of transmit
; queue entries that will be needed.
;
                mov     ax,specific.mc_txqdepth
                mov     cx,QT_SIZE
                mul     cx
                add     ax,offset DGROUP:qtarray

                les     bx,[bp].device_packet
IFDEF DOS
                mov     es:[bx].InitpEnd.off,ax
                mov     es:[bx].InitpEnd.segm,cs
ELSE
                mov     es:[bx].InitEcode,offset LASTCODE:0
                mov     es:[bx].InitEdata,ax
ENDIF
;
; we now want to fill in the QSPACE.  but since its going to be allocated
; over top of the initialization code/data under DOS, we have to move ourselves
; out of the way first.  CS:AX is the end of QSPACE.  since the copy operation
; may overlap, we do it out of the code that isn't moved, keeping as much here
; as possible to keep the code size down.  since there are no transmit queue
; entries allocated statically under DOS, it is guaranteed that the new location
; for the INIT code will be greater than its current location so we can safely
; do the copy backwards in case of overlap.  once relocated we should reference
; init data through cs: and avoid making calls to procedures not in init code.
;
IFDEF DOS
                mov     dx,cs                   ; CS:AX is end of QSPACE
                mov     cl,4                    ; offset >> 4
                shr     ax,cl
                add     ax,dx                   ; adjusted by CS
                inc     ax                      ; plus one is next segment
                mov     es,ax                   ; point at it with ES:DI
                xor     di,di
                mov     si,offset DGROUP:BEFORELAST ; DS:SI points at init code/data
                mov     cx,offset DGROUP:AFTERLAST
                sub     cx,offset DGROUP:BEFORELAST ; CX is size of init code/data
                add     si,cx                   ; set up for reverse direction
                dec     si
                add     di,cx
                dec     di
                std
                push    es                      ; set up return address
                mov     ax,offset LASTCODE:init_relocated
                push    ax
                jmp     strat_copy

                public  init_relocated
init_relocated:
                cld
ENDIF
;
; we can now fill in the QSPACE without stomping on ourselves.  zero out
; the qtarray space.
;
IFDEF DOS
                mov     ax,specific.mc_txqdepth
                mov     cx,QT_SIZE
                mul     cx
                mov     cx,ax
                push    ds
                pop     es
                mov     di,offset DGROUP:qtarray
                xor     al,al
        rep     stosb
ENDIF
;
; link up the doubly-linked request free queue using qrarray space
;
                mov     cx,MAX_RQ_PENDING
                mov     di,offset DGROUP:q_rqfree
                mov     si,offset DGROUP:qrarray
@@:             mov     [di].q_next,si
                mov     [si].q_prev,di
                mov     di,si
                add     si,QR_SIZE
                loop    @B

                mov     [di].q_next,offset DGROUP:q_rqfree
                mov     q_rqfree.q_prev,di
;
; link up the transmit free queue as a singly linked list terminated by NULL
;
                mov     cx,specific.mc_txqdepth
                mov     di,offset DGROUP:q_txfree
                mov     si,offset DGROUP:qtarray
@@:             mov     [di].q_next,si
                mov     di,si
                add     si,QT_SIZE
                loop    @B

                mov     [di].q_next,0
;
; both the tx free queue and the tx awaits queue do not use the tail pointer.
; zap it so a protection fault will occur if we reference it.
;
IFDEF DEV
                mov     q_txfree.q_tail,0FFFFh
                mov     q_txawaits.q_tail,0FFFFh
ENDIF
;
; exit, indicate packet has been processed.  under DOS, return far in case
; we've been relocated along the way.
;
init_exit:
                les     bx,[bp].device_packet
                mov     es:[bx].InitcUnit,0             ; character device
IFDEF OS2
                mov     es:[bx].InitpBPB.off,0
                mov     es:[bx].InitpBPB.segm,0
ENDIF
                mov     es:[bx].PktStatus,STDON
                leave
IFDEF DOS
                retf
ELSE
                ret
ENDIF
;
; configuration problem.  message to user.
;
init_problem:
                mov     di,offset DGROUP:msg_badconfigure
                jmp     short init_msgfail
;
; hardware not found.  message to user.
;
init_nohardware:
                mov     di,offset DGROUP:msg_nohardware
;
; message to user in DI.
;
init_msgfail:
                call    PutStrang
;
; failure to initialize driver.  message to user.
;
init_fail:
                mov     di,offset DGROUP:msg_generalfailure
                call    PutStrang

                mov     di,offset DGROUP:msg_pressakey
                call    PutStrang
;
; wait for a keystroke before continuing...
;
IFDEF OS2
                push    ds
                push    offset DGROUP:CharData
                push    0                       ; wait until one is available
                push    0                       ; keyboard handle
                call    KBDCHARIN
ELSE
                xor     ax,ax
                int     16h
ENDIF
;
; don't install anything, return zero size(s)
;
                les     bx,[bp].device_packet
IFDEF DOS
;
; don't formally install. For DOS 3.2 the device header must be installed
; but nothing else. Thus, we set the first character in the device name to
; a blank so the header cannot be used later in applications.
;
                mov     byte ptr cs:10,' '
;
; set the failed driver size to install just the header
;
                mov     es:[bx].InitpEnd.off,20
                mov     es:[bx].InitpEnd.segm,cs
ELSE
                mov     es:[bx].InitEcode,0
                mov     es:[bx].InitEdata,0
ENDIF
                jmp     init_exit
devInit         endp

NEWPAGE <CheckIDString - Check Adapter Identification String>
;------ CheckIDString -------------------------------------------------------;
;                                                                            ;
;       CheckIDString                                                        ;
;                                                                            ;
;       ES = segment/selector for Adapter MMIO area (at ES:0)                ;
;       SI = string to compare against                                       ;
;                                                                            ;
;       Returns:                                                             ;
;                                                                            ;
;       flags set for JE/JNE based on success/failure of comparison          ;
;                                                                            ;
;       All other registers preserved.                                       ;
;                                                                            ;
;       Compares the Token Ring Adapter's Identification String against      ;
;       that at DS:SI.  The ID String on the adapter is stored a little      ;
;       strangely, with only the low order nibble (4 bits) defined, and      ;
;       odd/even bytes equal.  Obviously some hardware hack.  So we mask     ;
;       off the hi nibble and skip alternate bytes.                          ;
;                                                                            ;
;----------------------------------------------------------------------------;

CheckIDString   proc    near
                PUSHR   ax,cx,si,di
;
; set DI to the start of the ID string on adapter, CX to number of bytes
;
                mov     di,ADIATRID
                mov     cx,TRID_SIZE
ckid_loop:
                mov     al,es:[di]
                and     al,0Fh
                cmp     al,[si]
                jne     ckid_exit               ; flags set for JNE
                inc     si
                add     di,2
                loop    ckid_loop

                xor     cx,cx                   ; set flags for JE
ckid_exit:
                POPR    ax,cx,si,di
                ret
CheckIDString   endp

NEWPAGE <GetHardAddress - Get Hardware Station Address>
;------ GetHardAddress ------------------------------------------------------;
;                                                                            ;
;       int GetHardAddress()                                                 ;
;                                                                            ;
;       SI = address to write hardware address                               ;
;       ES = selector to MMIO on adapter                                     ;
;                                                                            ;
;       Returns:                                                             ;
;                                                                            ;
;       AX = return code (zero if successful)                                ;
;                                                                            ;
;       Registers DS,SS,SP,BP and ES,SI,DI preserved.                        ;
;                                                                            ;
;       Returns the Hardware Address for this adapter.                       ;
;                                                                            ;
;       The burned-in station address for the Token Ring adapters is         ;
;       available in the Adapter Identification Area.  The ADIA is a little  ;
;       weird in that only the low order nibble of each byte is defined      ;
;       and even and odd bytes are the same.  So the code pulling the        ;
;       address out of the ADIA is a little strange.                         ;
;                                                                            ;
;----------------------------------------------------------------------------;

GetHardAddress  proc    near
                PUSHR   si,di
;
; we will use ES:DI as our source for the copy, and DS:SI as our destination
; which is a little unconventional, but with the nibble/even/odd weirdness
; this can't be a REP loop anyway...
;
                mov     di,ADIAADDR
                mov     cx,ADDRLEN
gh_loop:
                mov     ah,es:[di]              ; high nibble (68000 format)
                shl     ah,1
                shl     ah,1
                shl     ah,1
                shl     ah,1
                mov     al,es:[di]+2            ; low nibble
                and     al,0Fh
                or      al,ah                   ; combine to a byte
                mov     [si],al
                inc     si                      ; next byte
                add     di,4
                loop    gh_loop
;
; done.  return success.
;
                xor     ax,ax                   ; success!
                POPR    si,di
                ret
GetHardAddress  endp

NEWPAGE <AdapterExists - Check if adapter exists>
;------ AdapterExists -------------------------------------------------------;
;                                                                            ;
;       AdapterExists                                                        ;
;                                                                            ;
;       AL = PRIMARY or ALTERNATE                                            ;
;                                                                            ;
;       Returns:                                                             ;
;                                                                            ;
;       Carry set if not found.                                              ;
;       AX = error code (carry clear only)                                   ;
;                                                                            ;
;       All other registers preserved.                                       ;
;                                                                            ;
;       Given that the adapter is primary or alternate we read the switches  ;
;       at A20 or A24 as appropriate.  This gives us the start of the ROM    ;
;       on the adapter card.  By checking the adapter identification string  ;
;       against the legal possibilities we can see if that adapter actually  ;
;       exists or not.                                                       ;
;                                                                            ;
;       If the adapter is found to exist, then initializes those entries     ;
;       which depend on the adapter.  Certain characteristics still depend   ;
;       on whether the adapter is operating at either 4 or 16Mbps, and we    ;
;       won't know that until we reset it (at Bind time for compatibility    ;
;       with Start).  So we initialize assuming 4Mbps here.                  ;
;                                                                            ;
;       realMMIO is set to the real mode segment for Adapter MMIO area,      ;
;       even under OS/2.  This should not be loaded into a segment register  ;
;       and is only intended to allow the computation of the Shared RAM      ;
;       area without any VirtToPhys.                                         ;
;                                                                            ;
;----------------------------------------------------------------------------;

AdapterExists   proc    near
                PUSHR   bx,cx,dx,si,di,bp,es
;
; save original adapter type in BP
;
                mov     bp,ax
;
; set DX to port for switches (A20H for primary, A24H for alternate)
;
                mov     dx,PSWITCHES
                cmp     al,PRIMARY
                je      @F
                add     dx,ALTOFFSET
@@:
;
; read switches and convert to a real-mode segment in CX (the switches specify
; bits 19 through 14 of the address which must be in the upper 512K).
;
                in      al,dx
                mov     cx,0FC01h
                and     ch,al
                ror     cx,1                    ; eg. CC00h (DOS segment)
                mov     realMMIO,cx
;
; convert CX to an OS/2 physical address in AX:BX
;
IFDEF DOS
                mov     segMMIO,cx
ELSE
                mov     ax,cx
                mov     bx,cx
                mov     cl,12
                shr     ax,cl                   ; eg. 000Ch (high word)
                mov     cl,4
                shl     bx,cl                   ; eg. C000h (low word)
;
; virtualize the address so that segMMIO:0 is the start of the 8K MMIO area
;
                mov     cx,8*1024
                mov     si,segMMIO
                mov     dl,DevHlp_PhysToGDTSelector
                call    devHelper
                sjc     aex_problem
ENDIF
;
; point ES:0 at the start of the MMIO area for init time checks
;
ifdef DOS
                mov     es,segMMIO
else
                cli                             ; protect PhysToVirt
                mov     dh,1                    ; return ES:DI (DI zero)
                mov     dl,DevHlp_PhysToVirt    ; ring 3 address
                call    devHelper
                sjc     aex_problem
endif
;
; first check against the ID string assigned to the AT bus adapters.  if no
; match, try the ID string assigned to the PS/2 class adapters.
;
                mov     si,offset DGROUP:IDString
                call    CheckIDString
                je      @F
                mov     si,offset DGROUP:IDString_A
                call    CheckIDString
                SJNE    aex_fail
                or      adapterBits,ADAPTER_A
@@:
;
; successful match.  set primAlt from original parameter on call
;
                mov     ax,bp
                mov     primAlt,al
;
; if the RAM location is to be chosen the same as IBM, then set it up now.
;
                cmp     ram_method,RAM_ALA_IBM
                jne     aex_port
                mov     ax,0D800h               ; RAM segment for PRIMARY
                cmp     primAlt,PRIMARY
                je      @F
                mov     ax,0D400h               ; RAM segment for ALTERNATE
@@:             mov     realSM,ax
;
; if primary, set portOffset to 0, otherwise set it to 4.  this is the offset
; to apply to the primary port addresses to get the ports for this adapter
;
aex_port:
                xor     ax,ax
                cmp     primAlt,PRIMARY
                je      @F
                mov     ax,ALTOFFSET
@@:             mov     portOffset,ax
;
; calculate the interrupt release port and initialize it
;
                mov     ax,portOffset
                add     ax,PINTRELEASE
                mov     releasePort,ax
;
; determine the size of the shared RAM.  for the moment, we ignore SRAM
; paging so this is just the RRR (bits 3,2).
;
                mov     al,es:RRRL              ; fetch RRR odd
                and     al,RRRLSRAM_MASK        ; mask off SRAM size bits
                mov     cl,RRRLSRAM_SHIFT       ; shift right to isolate them
                shr     al,cl
                mov     bx,offset DGROUP:rrr_table ; for table lookup
                xlatb                           ; lookup ram size
                mov     ramSize,al              ; save SRAM size
;
; check to ensure that the RAM location is aligned properly, based on the
; SRAM size (page size if supported).  the test works even if RAM= wasn't
; specified (ie. realSM is 0).
;
                mov     al,ramSize              ; SRAM size in k
                mov     ah,64                   ; convert to 1K (segment)
                mul     ah                      ; AX = nK / 16
                dec     ax                      ; subtract one for mask
                test    realSM,ax               ; any bits set?
                sjnz    aex_problem             ; yes, not on boundary
;
; check whether the top 512 bytes of SRAM need to be initialized to zero
; after the adapter is reset.  only set if ramsize is also 64K.
;
                mov     al,es:[ADIASRAM]        ; SRAM size nibble
                and     al,0Fh                  ; retain low nibble
                cmp     al,NEEDS_INIT           ; 0Bh means 64K/needs zeroing
                jne     @F
                cmp     ramSize,64              ; 64K in use?
                jne     @F
                or      adapterBits,ZERO_TOP_512 ; set bit
@@:
;
; read the switches to get the interrupt level bits and translate that to
; the actual interrupt level using the appropriate IntLevel table (the
; Adapter/A has a different set of interrupt levels).
;
                mov     dx,portOffset
                add     dx,PSWITCHES
                in      al,dx
                and     al,MASK_INTLEVEL
                xor     ah,ah
                mov     bx,ax
                mov     al,IntLevel[bx]
                test    adapterBits,ADAPTER_A
                jz      @F
                mov     al,IntLevel_A[bx]
@@:
;
; on an AT class machine interrupt level 2 can be trapped as either level 2
; or level 9 since the hardware treats it as level 9 and the BIOS contains
; a stub handler for level 9 which invokes level 2.  however, the IBM 3270
; Program requires level 2 so we use level 2.  under OS/2 they have given up
; hiding the fact that level 2 is really level 9 and we must treat it as
; level 9.  so we do an operating system sensitive translation here.  since
; OS/2 only runs on AT class machines we do not need any CPU sensing logic.
;
IFDEF OS2
                cmp     al,2
                jne     @F
                mov     al,9
@@:
ENDIF
;
; save it in the MAC specific characteristics table.
;
                xor     ah,ah
                mov     specific.mc_interrupt,ax
;
; set up IMRport and EOIs based on the interrupt level.  if the interrupt
; level is less than 8 then we use the master PIC, otherwise the slave.  EOIs
; is set up with the command to issue to the slave in the low byte and the
; command to issue to the master in the high byte.  if the interrupt level is
; less than 8 the command to the slave will be a NOP.
;
                mov     IMRport,INT_1IMR
IFDEF DOS
                mov     EOIs,EOI_NOP+(EOI_NONSPEC*256)
ELSE
                xchg    al,ah                           ; int level in ah
                add     ax,EOI_NOP+(EOI_SPECIFIC*256)   ; specific EOI in ah
                mov     EOIs,ax
ENDIF
                cmp     specific.mc_interrupt,8
                jb      @F
                mov     IMRport,INT_2IMR
IFDEF DOS
                mov     EOIs,EOI_NONSPEC+(EOI_NONSPEC*256)
ELSE
                mov     EOIs,EOI_NONSPEC+((EOI_SPECIFIC+2)*256)
ENDIF
@@:
;
; compute interrupt enable/disable masks to use with the IMR (to enable
; interrupts for the adapter, AND enableMask with IMR, to disable interrupts
; for the adapter, OR disableMask with IMR)
;
                mov     cl,specific.mc_interrupt.lobyte
                and     cl,07
                mov     ah,1
                shl     ah,cl
                mov     disableMask,ah
                not     ah
                mov     enableMask,ah
;
; determine the shared memory location.  for the Adapter/A the address is
; set by POS so we just read the switches.  otherwise, the shared memory
; location may have been specified in the PROTOCOL.INI file.  if not, we
; set the shared memory to follow the ROM.
;
; set CX to the real mode segment for the shared RAM.
;
                test    adapterBits,ADAPTER_A
                jnz     aex_A

                mov     cx,realSM
                test    cx,cx
                jnz     aex_phys
;
; defaulting AT class adapter.  we want to configure the RAM immediately
; following the ROM on the next acceptable boundary, based on the SRAM size.
;
                mov     cx,realMMIO             ; ROM location
                add     cx,512                  ; skip past 8k ROM
                mov     al,ramSize              ; SRAM size in k
                mov     ah,64                   ; x 1k (in paragraphs)
                mul     ah                      ; AX = size in paragraphs
                dec     ax                      ; AX = bit mask for align
                add     cx,ax                   ; adjust up
                not     ax                      ; reverse mask (acceptable bits)
                and     cx,ax                   ; truncate down
                jmp     short aex_phys
;
; PS/2 adapter.  just read the RAM address set up by POS.
;
aex_A:
                mov     dx,portOffset
                add     dx,PSWITCHES2
                in      al,dx
                and     al,0FEh                 ; mask off MMIO domain bit
                mov     ch,al
                xor     cl,cl
;
; save the real-mode segment for use in ResetAdapter, then convert it to an
; OS/2 physical address in AX:BX
;
aex_phys:
                mov     realSM,cx
IFDEF DOS
                mov     segSM,cx
ELSE
                mov     ax,cx
                mov     bx,cx
                mov     cl,12
                shr     ax,cl
                mov     cl,4
                shl     bx,cl
;
; virtualize the address so that segSM:0 is the start of the shared memory
; area, giving the correct size.  note that CX=0 is equivalent to 64K for
; this devHelper call.
;
                mov     dl,ramSize              ; DX = ramsize in K
                xor     dh,dh
                mov     cl,10                   ; SHL 10 = multiply by 1024
                shl     dx,cl                   ; x 1024 = ramsize in bytes
                mov     cx,dx                   ; size now in CX
                mov     si,segSM
                mov     dl,DevHlp_PhysToGDTSelector
                call    devHelper
                SJC     aex_problem
ENDIF
;
; determine the maximum frame size at 4 Mbps...
;
                mov     al,es:[ADIARATE]        ; rates supported nibble
                and     al,0Fh                  ; retain low nibble
                mov     bx,offset DGROUP:rate_table ; table lookup
                xlatb                           ; AL = rates supported mask
                test    al,4                    ; 4Mbps support?
                jz      @F
                mov     bl,es:[ADIADHB4]        ; max frame @4Mbps nibble
                and     bl,0Fh                  ; retain low nibble
                xor     bh,bh                   ; zero high byte for index
                shl     bx,1                    ; convert to words
                mov     ax,max4_table[bx]       ; table lookup of frame size
                sub     ax,TXBUF_OVERHEAD       ; subtract out DHB overhead
                mov     max@4Mbps,ax            ; save it
@@:
;
; and at 16 Mbps...
;
                mov     al,es:[ADIARATE]        ; rates supported nibble
                and     al,0Fh                  ; retain low nibble
                mov     bx,offset DGROUP:rate_table ; table lookup
                xlatb                           ; AL = rates supported mask
                test    al,16                   ; 16Mbps support?
                jz      @F
                mov     bl,es:[ADIADHB16]       ; max frame @16Mbps nibble
                and     bl,0Fh                  ; retain low nibble
                xor     bh,bh                   ; zero high byte for index
                shl     bx,1                    ; convert to words
                mov     ax,max16_table[bx]      ; table lookup of frame size
                sub     ax,TXBUF_OVERHEAD       ; subtract out DHB overhead
                mov     max@16Mbps,ax           ; save it
@@:
;
; if a DHB size has been set, check that it doesn't exceed the maximum of
; the above two numbers.  it can still be too big for the data rate that
; is jumpered, but we won't know that till Bind time and we'll get an error
; from the Open Adapter at that point anyway...
;
                mov     ax,max@4Mbps            ; max @ 4 Mbps
                mov     cx,max@16Mbps           ; max @ 16 Mbps
                cmp     ax,cx                   ; take max(4Mbps,16Mbps)
                jae     @F
                mov     ax,cx
@@:             test    ax,ax                   ; problem?
                jz      aex_problem
                cmp     txbuflen,ax             ; requested > maximum?
                ja      aex_problem
;
; determine whether SRAM paging is possible (assuming 16Mbps)
;
                mov     al,es:[ADIAPAGE]        ; ADIA page nibble
                and     al,0Fh                  ; retain low nibble
                mov     bx,offset DGROUP:page_table ; for table lookup
                xlatb                           ; lookup page sizes supported
                test    ramSize,al              ; paging at current size?
                jz      @F
                mov     al,es:[ADIASRAM]        ; SRAM size nibble
                and     al,0Fh                  ; retain low nibble
                mov     bx,offset DGROUP:sram_table ; for table lookup
                xlatb                           ; lookup total SRAM
                cmp     al,ramSize              ; > SRAM page size?
                jbe     @F
                or      adapterBits,PAGEABLE    ; its pageable
@@:
;
; determine what rates early token release is supported at
;
                mov     al,es:[ADIAEARLY]       ; Early release rates nibble
                and     al,0Fh                  ; retain low nibble
                mov     bx,offset DGROUP:early_table ; for table lookup
                xlatb                           ; lookup early release rates
                mov     earlyRates,al  ; save it
;
; copy the burned-in address to the permanent address in the common
; characteristics table.  this is set to the burned-in address even if the
; NETADDRESS keyword was used (so LANMAN can use it for security checking).
;
                mov     si,offset DGROUP:specific.mc_paddr
                call    GetHardAddress
;
; fill in the current address with the same value unless the ADDRESSED bit is
; set, indicating the NETADDRESS keyword was used.
;
                test    adapterBits,ADDRESSED
                jnz     @F

                PUSHR   es,di
                mov     si,offset DGROUP:specific.mc_paddr
                mov     di,offset DGROUP:specific.mc_caddr
                mov     ax,ds
                mov     es,ax
                mov     cx,ADDRLEN
        rep     movsb
                POPR    es,di
@@:
;
; return with carry flag clear and AX set to zero (no error)
;
                xor     ax,ax                   ; clears carry
;
; done.  carry flag indicates success.  release PhysToVirt selector.
;
aex_exit:
ifdef OS2
                pushf                           ; save carry flag
                mov     dl,DevHlp_UnPhysToVirt  ; release selectors
                call    devHelper               ; do it
                popf                            ; restore carry flag
                sti                             ; rupts on again/unprotect
endif
                POPR    bx,cx,dx,si,di,bp,es
ifdef OS2
                sti                             ; rupts on again/unprotect
endif
                ret
;
; hardware not found.  return carry flag set.
;
aex_fail:
                stc
                jmp     short aex_exit
;
; configuration problem.  message to user.  return with carry flag clear.
;
aex_problem:
                mov     ax,CONFIGURATION_FAILURE
                clc
                jmp     short aex_exit
AdapterExists   endp

NEWPAGE <StringEquals - Compare Two Strings>
;------ StringEquals --------------------------------------------------------;
;                                                                            ;
;       int StringEquals()                                                   ;
;                                                                            ;
;       SI = string A                                                        ;
;       ES:DI = string B                                                     ;
;                                                                            ;
;       Returns:                                                             ;
;                                                                            ;
;       Flags set for JE/JNE.                                                ;
;                                                                            ;
;       All other registers preserved.                                       ;
;                                                                            ;
;       Compares two ASCIIZ strings for equality, ignoring case.  Both       ;
;       strings must be terminated by a null byte.                           ;
;                                                                            ;
;----------------------------------------------------------------------------;

StringEquals    proc    near
                PUSHR   ax,si,di
;
; convert characters from each to uppercase and then compare.  stop on NULL.
;
streq_loop:
                mov     al,[si]
                mov     ah,es:[di]
                test    ax,ax
                jz      streq_exit
                call    upcase                  ; convert AL to uppercase
                xchg    ah,al                   ; swap characters
                call    upcase                  ; convert old AH to uppercase
                cmp     al,ah
                jne     streq_exit

                inc     si
                inc     di
                jmp     short streq_loop
streq_exit:
                POPR    ax,si,di
                ret
StringEquals    endp

NEWPAGE <PutString/PutStrang - display message to screen>
;------ PutString -----------------------------------------------------------;
;                                                                            ;
;       void PutString()                                                     ;
;                                                                            ;
;       ES:DI = pointer to null terminated string                            ;
;                                                                            ;
;       Registers AX,CX,DX and flags are destroyed.  All others preserved.   ;
;                                                                            ;
;       outputs a null-terminated string to the screen.  under OS/2 this     ;
;       function uses the DOSPUTMESSAGE function, so can only be used at     ;
;       initialization time.                                                 ;
;                                                                            ;
;----------------------------------------------------------------------------;

PutString       proc    near
;
; set CX to the number of bytes in the string not including the null
;
                push    di
                xor     ax,ax
                mov     cx,ax
                dec     cx              ; set maximum to FFFFh
        repne   scasb                   ; scan for null byte
                not     cx              ; 1's complement the remaining count
                dec     cx              ; don't include the null in count
                pop     di
;
; put the string in ES:DI for CX bytes to stdout
;
IFDEF OS2
                push    1               ; standard output handle
                push    cx
                push    es
                push    di
                call    DOSPUTMESSAGE
ELSE
                push    ds
                push    bx
                mov     ax,es
                mov     ds,ax
                mov     dx,di
                mov     ah,40h          ; put string DS:DX for CX bytes
                mov     bx,1            ; standard output handle
                int     21h
                pop     bx
                pop     ds
ENDIF
                ret
PutString       endp

;------ PutStrang -----------------------------------------------------------;
;                                                                            ;
;       void PutStrang()                                                     ;
;                                                                            ;
;       DI = offset of null terminated string in data segment                ;
;                                                                            ;
;       Calls PutString after setting up ES.  The original value of ES       ;
;       is preserved across the call.                                        ;
;                                                                            ;
;----------------------------------------------------------------------------;

PutStrang       proc    near
                push    es
                mov     ax,ds
                mov     es,ax
                call    PutString
                pop     es
                ret
PutStrang       endp

NEWPAGE <LongToASCII>
;------ LongToASCII ---------------------------------------------------------;
;                                                                            ;
;       DX:AX = long to convert                                              ;
;       SI = base to use (2 to 31)                                           ;
;       ES:DI = where to put it                                              ;
;                                                                            ;
;       Converts the double word in DX:AX (unsigned) to an ASCII string      ;
;       in ES:DI using the base in SI.  The string will be null terminated.  ;
;                                                                            ;
;       Since it uses DIV this is not an optimal routine.                    ;
;                                                                            ;
;       Registers AX, CX, DX and flags are destroyed.  All others            ;
;       preserved.                                                           ;
;                                                                            ;
;----------------------------------------------------------------------------;

LongToASCII     proc    near
                PUSHR   bx,si,di

                push    di                      ; save start of buffer
;
; move DX:AX into BX:CX
;
                mov     bx,dx                   ; now in BX:CX
                mov     cx,ax
;
; if BX:CX is zero,skip the divide and jump right in
;
                mov     ax,bx                   ; BX:CX is zero?
                or      ax,cx
                jz      ltoa_digit              ; just use zero, skip divide
;
; loop
;
; divide BX:CX by SI.  result in BX:CX, remainder in AX.
;
ltoa_loop:
                xor     dx,dx                  ; BX:CX / SI -> BX:CX, AX
                mov     ax,bx
                div     si
                mov     bx,ax
                mov     ax,cx
                div     si
                mov     cx,ax
                mov     ax,dx
;
; single digit in AL.  convert to ASCII and store at ES:DI.
;
ltoa_digit:
                call    hexchar                 ; convert to ASCII
                stosb                           ; store at ES:DI, advance
;
; loop around again if still non-zero
;
                mov     ax,bx                  ; BX:CX is zero?
                or      ax,cx
                jnz     ltoa_loop               ; nope, loop around
;
; done processing.  put a null terminator on the end.
;
                mov     byte ptr es:[di],0
;
; the string is stored backwards.  reverse it by swapping bytes.
;
                pop     si                      ; restore start of buffer
ltoa_swaploop:
                dec     di                      ; back up one
                cmp     si,di                   ; check if done swapping
                jae     ltoa_exit               ; finished
                mov     ah,byte ptr es:[si]     ; swap characters
                mov     al,byte ptr es:[di]
                mov     byte ptr es:[di],ah
                mov     byte ptr es:[si],al
                inc     si                      ; forward one
                jmp     short ltoa_swaploop     ; back around
;
; done.
;
ltoa_exit:
                POPR    bx,si,di
                ret
LongToASCII     endp

NEWPAGE <PutKeyword>
;------ PutKeyword ----------------------------------------------------------;
;                                                                            ;
;       void PutKeyword()                                                    ;
;                                                                            ;
;       ES:BX = pointer to keyword entry                                     ;
;                                                                            ;
;       Displays an error message string to the screen in the approximate    ;
;       form of the original input line for this keyword entry.              ;
;                                                                            ;
;       Registers AX, CX, DX and flags are destroyed.  All others            ;
;       preserved.                                                           ;
;                                                                            ;
;----------------------------------------------------------------------------;

PutKeyword      proc    near
                push    di
;
; First display the preamble
;
                mov     di,offset DGROUP:msg_preamble
                call    PutStrang
;
; Then display the keyword itself
;
                lea     di,[bx].kw_keyword
                call    PutString
;
; If no parameters stop here
;
                cmp     es:[bx].kw_param_num, 0
                je      key_crlf
;
; Then an equals sign (' = ').
;
                mov     di,offset DGROUP:msg_equals
                call    PutStrang
;
; If the rhs is a string then display it in quotes.
;
                cmp     es:[bx].kw_param_type, KW_PARAM_STRING
                jne     key_long

                mov     di,offset DGROUP:msg_quote
                call    PutStrang

                lea     di,[bx].kw_param_value
                call    PutString

                mov     di,offset DGROUP:msg_quote
                call    PutStrang

                jmp     short key_more?
;
; The rhs is a long.  Display it in decimal.
;
key_long:
                push    es
                mov     dx,es:[bx].kw_param_value.hiword
                mov     ax,es:[bx].kw_param_value.loword
                mov     si,10                   ; decimal
                mov     cx,ds
                mov     es,cx
                mov     di,offset DGROUP:number_buffer
                call    LongToASCII             ; now string at ES:DI
                call    PutString               ; display it
                pop     es
;
; Then an open parenthesis and hex prefix (' (0x').
;
                mov     di,offset DGROUP:msg_openparen
                call    PutStrang
;
; Then the number in hex.
;
                push    es
                mov     dx,es:[bx].kw_param_value.hiword
                mov     ax,es:[bx].kw_param_value.loword
                mov     si,16                   ; hexadecimal
                mov     cx,ds
                mov     es,cx
                mov     di,offset DGROUP:number_buffer
                call    LongToASCII             ; now string at ES:DI
                call    PutString               ; display it
                pop     es
;
; Then an close parenthesis (')').
;
                mov     di,offset DGROUP:msg_closeparen
                call    PutStrang
;
; If there are any more parameter, indicate that
;
key_more?:
                cmp     es:[bx].kw_param_num, 1
                jbe     key_crlf

                mov     di,offset DGROUP:msg_moretocome
                call    PutStrang
;
; And finally a new line
;
key_crlf:
                mov     di,offset DGROUP:msg_crlf
                call    PutStrang

                pop     di
                ret
PutKeyword      endp

NEWPAGE <ParseKeyword - Identify Keyword>
;------ ParseKeyword --------------------------------------------------------;
;                                                                            ;
;       int ParseKeyword()                                                   ;
;                                                                            ;
;       ES:BX = pointer to keyword image in configuration                    ;
;                                                                            ;
;       Returns:                                                             ;
;                                                                            ;
;       AX = keyword id (0 if no match, FFFFh if bad rhs)                    ;
;                                                                            ;
;       All other registers except flags preserved.                          ;
;                                                                            ;
;       Given a keyword image, checks the keyword against those recognized   ;
;       by this driver and if a match, returns an integer non-zero keyword   ;
;       id from the keyword_table.  Also checks to ensure that the right     ;
;       hand side has the right number of entries of the right type and      ;
;       does a simple range check.                                           ;
;                                                                            ;
;       If a right hand side is required and none has been supplied (but     ;
;       we have a valid keyword) then return KEY_NOP.  This should be        ;
;       treated as a NOP, with no change from the default.                   ;
;                                                                            ;
;----------------------------------------------------------------------------;

ParseKeyword    proc    near
                PUSHR   bx,dx,si,di,es
;
; set SI to the first keyword in table
;
                mov     si,offset DGROUP:keyword_table
;
; loop until no more keywords in table
;
parsek_loop:
                cmp     [si].key_name,0
                SJE     parsek_badkey
;
; try for a match
;
                push    si
                mov     si,[si].key_name
                lea     di,es:[bx].kw_keyword
                call    StringEquals
                pop     si
                je      parsek_match
;
; next keyword
;
                add     si,SIZE key_entry
                jmp     parsek_loop
;
; matched keyword at SI.  if no rhs expected, confirm it.
;
parsek_match:
                cmp     [si].key_rhs,RHS_NONE
                jne     @F
                cmp     es:[bx].kw_param_num,0
                SJNE    parsek_syntax
                jmp     parsek_success
;
; must be a single rhs value.  if none then return KEY_NOP.
;
@@:             cmp     es:[bx].kw_param_num,0
                jne     @F
                mov     ax,KEY_NOP
                jmp     short parsek_exit

@@:             cmp     es:[bx].kw_param_num,1
                jne     parsek_syntax
;
; if anything allowed, it can be either a string or an integer.  if an integer
; then range check it.
;
                cmp     [si].key_rhs,RHS_ANYTHING
                jne     parsek_integer
                cmp     es:[bx].kw_param_type,KW_PARAM_LONG
                jne     parsek_any_string
                cmp     es:[bx].kw_param_value.hiword,0
                jne     parsek_syntax
                mov     ax,es:[bx].kw_param_value.loword
                cmp     ax,[si].key_min
                jb      parsek_syntax
                cmp     ax,[si].key_max
                ja      parsek_syntax
                jmp     short parsek_success
parsek_any_string:
                cmp     es:[bx].kw_param_type,KW_PARAM_STRING
                jne     parsek_syntax
                jmp     short parsek_success
;
; if an integer expected, confirm it and range check
;
parsek_integer:
                cmp     [si].key_rhs,RHS_INTEGER
                jne     parsek_string
                cmp     es:[bx].kw_param_type,KW_PARAM_LONG
                jne     parsek_syntax
                cmp     es:[bx].kw_param_value.hiword,0
                jne     parsek_syntax
                mov     ax,es:[bx].kw_param_value.loword
                cmp     ax,[si].key_min
                jb      parsek_syntax
                cmp     ax,[si].key_max
                ja      parsek_syntax
                jmp     short parsek_success
;
; if a string expected, confirm it and range check the length (note that the
; rhs length stored in the image includes the null terminator)
;
parsek_string:
                cmp     [si].key_rhs,RHS_STRING
                jne     parsek_syntax
                cmp     es:[bx].kw_param_type,KW_PARAM_STRING
                jne     parsek_syntax
                mov     ax,es:[bx].kw_param_len
                dec     ax
                cmp     ax,[si].key_min
                jb      parsek_syntax
                cmp     ax,[si].key_max
                ja      parsek_syntax
;
; successful match against keyword at SI, return key id in AX.
;
parsek_success:
                mov     ax,[si].key_id
parsek_exit:
                POPR    bx,dx,si,di,es
                ret
;
; no match on keyword.  return 0.
;
parsek_badkey:
                xor     ax,ax
                jmp     parsek_exit
;
; bad right hand side.  return FFFFh
;
parsek_syntax:
                mov     ax,0FFFFh
                jmp     parsek_exit
ParseKeyword    endp

NEWPAGE <ParseProductID - Parse PRODUCTID keyword>
;------ ParseProductID ------------------------------------------------------;
;                                                                            ;
;       void ParseProductID()                                                ;
;                                                                            ;
;       ES:BX = pointer to keyword image in configuration                    ;
;                                                                            ;
;       Returns:                                                             ;
;                                                                            ;
;       carry flag set if error.                                             ;
;                                                                            ;
;       All registers except flags preserved.                                ;
;                                                                            ;
;       sets the global productid from the PRODUCTID keyword in the          ;
;       PROTOCOL.INI file.  The string on the right hand side is parsed,     ;
;       with two hex characters converted to a byte.                         ;
;                                                                            ;
;       See ParseConfig for details on PRODUCTID and its restrictions.       ;
;                                                                            ;
;       The product id defaults to 0110F0...F0 (F0 is an EBCDIC zero)        ;
;       if not specified.  When parsing the PRODUCTID keyword, we must       ;
;       reset the first two bytes to zero before parsing.                    ;
;                                                                            ;
;----------------------------------------------------------------------------;

ParseProductID  proc    near
                PUSHR   ax,si,di
;
; initialize the first two bytes of productid to zero before parsing
;
                mov     word ptr productid,0
;
; make sure that the right hand side is an even number of bytes in length then
; divide it by two into CX.
;
                mov     cx,es:[bx].kw_param_len
                dec     cx                      ; don't count null byte
                test    cx,1
                jnz     parseid_fail
                shr     cx,1
;
; set ES:DI to the start of the rhs string and SI to the start of productid
;
                lea     di,es:[bx].kw_param_value
                mov     si,offset DGROUP:productid
;
; convert the two ascii hex characters at ES:DI to a byte in AL and store it
; at SI, incrementing both DI and SI for the next repetition
;
parseid_loop:
                mov     al,es:[di]
                inc     di
                call    charhex
                jc      parseid_fail
                mov     ah,al

                mov     al,es:[di]
                inc     di
                call    charhex
                jc      parseid_fail

                shl     ah,1
                shl     ah,1
                shl     ah,1
                shl     ah,1
                or      al,ah

                mov     [si],al
                inc     si
                loop    parseid_loop
;
; success
;
                clc
parseid_exit:
                POPR    ax,si,di
                ret
;
; failure
;
parseid_fail:
                stc
                jmp     parseid_exit
ParseProductID  endp

NEWPAGE <ParseNetAddr - Parse NETADDRESS keyword>
;------ ParseNetAddr --------------------------------------------------------;
;                                                                            ;
;       void ParseNetAddr()                                                  ;
;                                                                            ;
;       ES:BX = pointer to keyword image in configuration                    ;
;                                                                            ;
;       Returns:                                                             ;
;                                                                            ;
;       carry flag set if error.                                             ;
;                                                                            ;
;       All registers except flags preserved.                                ;
;                                                                            ;
;       sets the current address from the NETADDRESS keyword in the          ;
;       PROTOCOL.INI file.  The string on the right hand side is parsed,     ;
;       with two hex characters converted to a byte.  The string is          ;
;       guaranteed to be exactly 12 bytes in length.                         ;
;                                                                            ;
;----------------------------------------------------------------------------;

ParseNetAddr    proc    near
                PUSHR   ax,si,di
;
; set ES:DI to the start of the rhs string and SI to the start of the current
; address
;
                mov     si,offset DGROUP:specific.mc_caddr
                lea     di,es:[bx].kw_param_value
                mov     cx,6
;
; convert the two ascii hex characters at ES:DI to a byte in AL and store it
; at SI, incrementing both DI and SI for the next repetition
;
parsena_loop:
                mov     al,es:[di]
                inc     di
                call    charhex
                jc      parsena_fail
                mov     ah,al

                mov     al,es:[di]
                inc     di
                call    charhex
                jc      parsena_fail

                shl     ah,1
                shl     ah,1
                shl     ah,1
                shl     ah,1
                or      al,ah

                mov     [si],al
                inc     si
                loop    parsena_loop
;
; success.
;
                clc
parsena_exit:
                POPR    ax,si,di
                ret
;
; failure
;
parsena_fail:
                stc
                jmp     parsena_exit
ParseNetAddr    endp

NEWPAGE <ParseConfig - Parse PROTOCOL.INI image>
;------ ParseConfig ---------------------------------------------------------;
;                                                                            ;
;       int ParseConfig()                                                    ;
;                                                                            ;
;       ES:BX = pointer to an array of configuration memory images.          ;
;                                                                            ;
;       Returns:                                                             ;
;                                                                            ;
;       AX = return code, zero for success                                   ;
;                                                                            ;
;       All other registers except flags preserved.  This routine is called  ;
;       in device driver INIT context.                                       ;
;                                                                            ;
;       This routine takes the memory image of the PROTOCOL.INI file passed  ;
;       to us by the Protocol Manager and parses it, initializing various    ;
;       parameters in the process.  Note that ES:BX may be a NULL pointer,   ;
;       indicating that no PROTOCOL.INI file was found.  In this case we     ;
;       will initialize exactly one adapter with all default values.         ;
;                                                                            ;
;       See the NDIS Spec for the format of the configuration memory         ;
;       images.  The following keywords are recognized (defaults and any     ;
;       range in parentheses):                                               ;
;                                                                            ;
;               PRIMARY                         (see below)                  ;
;               ALTERNATE                                                    ;
;               EARLYRELEASE                    (see below)                  ;
;               RECVBUFS = integer              (2, 2 to 60)                 ;
;               RECVBUFSIZE = integer           (256, 256 to 17952)          ;
;               XMITBUFS = integer              (1, 1 to 2)                  ;
;               XMITBUFSIZE = integer           (see below, 256 to 17952)    ;
;               RAM = hex integer/"3Com"/IBM    (see below)                  ;
;               MAXTRANSMITS = integer          (6, 6 to MAX_TX_PENDING)     ;
;               PRODUCTID = "hex-string"        (see below)                  ;
;               NETADDRESS = "hex-string"       (see below)                  ;
;               DRIVERNAME = "string"                                        ;
;                                                                            ;
;       PRIMARY and ALTERNATE are opposites.  Use only one.  They            ;
;       specify which adapter to look for (there is a PRIMARY/ALTERNATE      ;
;       jumper on the adapter which determines which of two sets of          ;
;       ports the adapter will use).  If not specified the driver will       ;
;       try PRIMARY first and if no adapter is found, try ALTERNATE.         ;
;       Therefore these are normally not necessary.  However, if two         ;
;       adapters are installed in the same machine, PRIMARY should be        ;
;       specified for one and ALTERNATE for the other to be specific         ;
;       about which network adapter you mean.                                ;
;                                                                            ;
;       EARLYRELEASE specifies that the early token release option should    ;
;       be enabled if supported at the jumpered rate.  To make I&S easier    ;
;       the keyword is ignored if not supported.                             ;
;                                                                            ;
;       XMITBUFS, XMITBUFSIZE, RECVBUFS and RECVBUFSIZE determine the        ;
;       number and size of the transmit and receive buffers.                 ;
;                                                                            ;
;       XMITBUFS is the number of transmit buffers (DHBs) to configure.      ;
;       Although the configuration works just fine, none of the adapters     ;
;       tested to date support overlapped transmits (a major performance     ;
;       failing) so it is pointless to change this.                          ;
;                                                                            ;
;       XMITBUFSIZE is the size of a transmit buffer (DHB).  This sets the   ;
;       maximum packet size that can be handled by the adapter since         ;
;       transmit packets cannot span buffers.  If not specified this will    ;
;       default depending on the amount of SRAM available and the maximum    ;
;       packet size supported by this adapter.  For the original adapters    ;
;       (I, II and /A) a packet size of 2040 will always be used.  For the   ;
;       16/4 adapters the defaults depend upon the SRAM size and the data    ;
;       rate.  Most support a maximum packet size of 4456 bytes at 4Mbps     ;
;       and 17952 bytes at 16Mbps.  The default will be min(SRAM/4,maximum   ;
;       legal packet size).  These particular maximums are not guaranteed    ;
;       however and others are possible.  Up to about 4K performance         ;
;       improvements incur by increasing the packet size.  Beyond that       ;
;       there seems little point other than reducing the number of receive   ;
;       interrupts.  If specified, this will be checked at INIT time         ;
;       against the maximum packet sizes allowed for all of the data rates   ;
;       supported by this adapter.  It is possible that the packet size      ;
;       specified will slip through because it is supported but not at the   ;
;       data rate configured on the adapter.  If this happens a              ;
;       CONFIGURATION_FAILURE error will be returned from Open Adapter and   ;
;       (hopefully) reported to the user by NETBIND.                         ;
;                                                                            ;
;       RECVBUFS and RECVBUFSIZE determine the number and size of the        ;
;       receive buffers to use.  Although the default number of receive      ;
;       buffers requested defaults to 2 if not specified, the adapter        ;
;       automatically configures any remaining RAM as receive buffers, so    ;
;       the default is actually as many receive buffers as will fit, based   ;
;       on the buffer size.  Received packets can span multiple receive      ;
;       buffers so the default of 256 bytes allows you to receive a lot of   ;
;       small packets or fewer large packets.  The overhead in TransferData  ;
;       of using smaller receive buffers is minimal from measurements of     ;
;       thruput.  There is therefore little reason to change these.          ;
;                                                                            ;
;       See additional comments on Request OpenAdapter regarding reasonable  ;
;       values for RECVBUFS, RECVBUFSIZE, XMITBUFS and XMITBUFSIZE.          ;
;                                                                            ;
;       The RAM keyword determines where the shared RAM will be located.     ;
;       If not specified, the shared RAM is located immediately following    ;
;       the RAM on the next acceptable boundary (this is the same method     ;
;       the 3+ Token Ring driver used).  An acceptable boundary is defined   ;
;       as being aligned based on its own RAM size.  Thus the Adapter I      ;
;       with 8K of RAM aligns the RAM on the next 8K boundary following the  ;
;       ROM--ie.  immediately following the ROM (at CE00h if the default     ;
;       CC00h ROM location is used).  For a 16/4 adapter jumpered for 64K    ;
;       the RAM must be on a 64K boundary (at D000h if if the CC00h ROM      ;
;       location is used).  Since this driver does not use SRAM paging the   ;
;       size of the RAM is directly indicated by the switch settings (ie.    ;
;       16K is 16K not 64K using 16K pages).  If a hex value is specified    ;
;       it is the upper four bytes of the five byte physical address where   ;
;       the shared RAM should be located.  It must be between A000h and      ;
;       EC00h, and be located on a proper boundary.  The right hand side     ;
;       can also be "3Com" to specify the 3Com RAM location method (the      ;
;       default outlined above) or "IBM" to specify the IBM RAM location     ;
;       method (0xD800 for PRIMARY, 0xD400 for alternate).                   ;
;                                                                            ;
;       Normally RAM need not be specified.  No checks are made to           ;
;       guarantee it does not conflict with any other memory ranges.  If     ;
;       two adapters are installed, either let the RAM location default or   ;
;       exercise care to ensure that the two locations do not conflict.      ;
;                                                                            ;
;       Note that on the PS/2 adapters the shared RAM location is set up by  ;
;       POS and the RAM keyword is ignored.                                  ;
;                                                                            ;
;       MAXTRANSMITS determines the number of transmits which can be         ;
;       queued up.  Most protocols will handle running out of resources      ;
;       with little performance degredation.  However, DLC will drop         ;
;       significantly in performance if OUT_OF_RESOURCES is returned too     ;
;       often.  The default is 6, which should be sufficient for most        ;
;       cases, is adequate for window sizes up to about 6 or so.  On a       ;
;       heavily loaded server with a lot of active connections, you may      ;
;       want to increase this number since this driver is transmit limited,  ;
;       and with large windows on multiple active connections you may need   ;
;       a bigger transmit queue to handle them all.  Since each takes about  ;
;       128 bytes of memory this number can affect the resident size of the  ;
;       driver significantly.                                                ;
;                                                                            ;
;       The PRODUCTID keyword is entirely optional.  It is used to fill in   ;
;       the product id supplied to the adapter on open, which can be         ;
;       queried by network administration.  The right hand side consists of  ;
;       a string of hex characters which are used to fill in the 18 byte     ;
;       productid.  If the user does not specify this value then it          ;
;       defaults to "0110F0...F0".  See the IBM Token Ring Architecture      ;
;       Reference for details on what the product id should look like.  The  ;
;       logic here merely converts the 2-36 byte string into leading bytes   ;
;       (padded with zeroes) and does not enforce any particular values.     ;
;       Normally this is unnecessary unless the installation is using        ;
;       IBM Network Administration, in which case somebody familiar with     ;
;       the rules will have to go around setting PRODUCTIDs up.              ;
;                                                                            ;
;       NETADDRESS allows the user to configure the network address of       ;
;       the adapter, rather than using the one in ROM.  If not specified     ;
;       the address on the adapter will be used.  If specified, the rhs      ;
;       must be exactly 12 hex ascii characters which will be converted      ;
;       to the network address.  Like IBM we require that the address set    ;
;       be in the range 4000 0000 0000 to 4000 7FFF FFFF.  IBM suggests      ;
;       limiting yourself to decimal digits, but does not enforce this so    ;
;       we don't either (though apparently it is required for certain host   ;
;       connectivity).                                                       ;
;                                                                            ;
;----------------------------------------------------------------------------;

                DLOCAL  pdriver,4

ParseConfig     proc    near
                LOCALS
                PUSHR   bx,cx,dx,si,di,es

                jmp     short parse_driverloop

;------ parse_driverloop ----------------------------------------------------;
;
; loop through all driver configuration entries
;
parse_nextdriver:
                les     bx,[bp].pdriver
                les     bx,es:[bx].dr_next

parse_driverloop:
                mov     ax,es
                or      ax,bx
                SJZ     parse_done

                mov     [bp].pdriver.off,bx
                mov     [bp].pdriver.segm,es
;
; determine if this driver configuration is for us.  to do this we loop
; through all of the keyword assignments for this driver looking for
; "DRIVERNAME" with the correct right hand side.
;
                add     bx,dr_keyword
parse_nameloop:
                mov     ax,es                           ; any keyword?
                or      ax,bx
                SJZ     parse_nextdriver
;
; check if its "DRIVERNAME"
;
                call    ParseKeyword
                cmp     ax,KEY_DRIVERNAME
                jne     parse_notname
;
; check the rhs against our_name
;
                lea     di,es:[bx].kw_param_value
                mov     si,our_name
                call    StringEquals
                je      parse_ours
                jmp     parse_nextdriver
;
; keyword is not DRIVERNAME, next keyword
;
parse_notname:
                les     bx,es:[bx].kw_next
                jmp     parse_nameloop
;
; this configuration is ours, set ES:BX to the driver configuration again
;
parse_ours:
                les     bx,[bp].pdriver
;
; copy the logical driver name out of the driver configuration into the
; common module characteristics table
;
                mov     di,offset DGROUP:common.cc_name
                lea     si,es:[bx].dr_logical_name
                push    es                              ; swap ES & DS
                push    ds
                pop     es
                pop     ds
                mov     cx,16
        rep     movsb
                push    ds                              ; swap ES & DS back
                push    es
                pop     ds
                pop     es
;
; scan the keyword list of the current config entry
;
                add     bx,dr_keyword
parse_keyloop:
                mov     ax,es
                or      ax,bx                   ; any more keywords?
                SJZ     parse_done              ; if not, we're done
;
; check against the keywords recognized by this driver.
;
                call    ParseKeyword
                cmp     ax,KEY_NOP
                SJE     parse_nextkey
                cmp     ax,KEY_DRIVERNAME
                SJE     parse_nextkey
                cmp     ax,KEY_PRIMARY
                SJE     parse_primary
                cmp     ax,KEY_ALTERNATE
                SJE     parse_alternate
                cmp     ax,KEY_EARLYRELEASE
                SJE     parse_earlyrelease
                cmp     ax,KEY_PRODUCTID
                SJE     parse_productid
                cmp     ax,KEY_NETADDR
                SJE     parse_netaddr
                cmp     ax,KEY_RAM
                je      parse_ram
                mov     cx,es:[bx].kw_param_value.loword
                cmp     ax,KEY_RECVBUFS
                SJE     parse_rxbufs
                cmp     ax,KEY_RECVBUFLEN
                SJE     parse_rxlen
                cmp     ax,KEY_XMITBUFS
                SJE     parse_txbufs
                cmp     ax,KEY_XMITBUFLEN
                SJE     parse_txlen
                cmp     ax,KEY_TRANSMITS
                SJE     parse_transmits
                test    ax,ax
                SJZ     parse_badkey
                jmp     parse_badrhs
;
; RAM keyword.  the rhs may be an integer (specifying the segment to use for
; the adapter SRAM) or a string.  if its an integer we verify at this time
; that it is on an 8k boundary (we'll check later if it is actually aligned
; correctly for the size of RAM being used).  if its a string, it must be
; either '3Com' or 'IBM'.  set ram_method appropriately.
;
                public  parse_ram
parse_ram:
                cmp     es:[bx].kw_param_type,KW_PARAM_LONG
                jne     @F
                mov     cx,es:[bx].kw_param_value.loword
                test    cx,01FFh
                SJNZ    parse_badrhs
                mov     realSM,cx
                mov     ram_method,RAM_ADDRESSED
                jmp     parse_nextkey
@@:
                cmp     es:[bx].kw_param_type,KW_PARAM_STRING
                SJNE    parse_badrhs
                push    di
                mov     si,offset DGROUP:ram_3Com
                lea     di,es:[bx].kw_param_value
                call    StringEquals
                je      parse_ram_3Com
                mov     si,offset DGROUP:ram_IBM
                call    StringEquals
                je      parse_ram_IBM
                pop     di
                jmp     parse_badrhs
parse_ram_3Com:
                mov     ram_method,RAM_AFTER_ROM
                mov     realSM,0
                jmp     short parse_nextkey
parse_ram_IBM:
                mov     ram_method,RAM_ALA_IBM
                mov     realSM,0
                jmp     short parse_nextkey
;
; for PRIMARY and ALTERNATE ensure that we haven't already seen a keyword
; specifying the card type
;
parse_primary:
                cmp     primAlt,UNKNOWN
                SJNE    parse_cfgerr
                mov     primAlt,PRIMARY
                jmp     short parse_nextkey
parse_alternate:
                cmp     primAlt,UNKNOWN
                jne     parse_cfgerr
                mov     primAlt,ALTERNATE
                jmp     short parse_nextkey
;
; EARLYRELEASE keyword.  set the bit in the hard-wired open options mask.
; we'll turn it off later if its not supported at the operating data rate.
;
parse_earlyrelease:
                or      openHard,OPEN_EARLYTOKEN
                jmp     short parse_nextkey
;
; PRODUCTID parameter.  the rhs is a string of ascii characters specifying
; the hex digits to make up the 18 byte product id.
;
parse_productid:
                call    ParseProductID
                jc      parse_badrhs
                jmp     short parse_nextkey
;
; NETADDRESS parameter.  the rhs is a string of 12 hex ascii characters
; specifying the network address to use instead of the one in ROM.  once
; set, ensure the address is in legal range.  if successful, set the
; ADDRESSED bit (determines where mc_caddr will come from).
;
parse_netaddr:
                call    ParseNetAddr
                jc      parse_badrhs
                cmp     word ptr specific.mc_caddr[0],0040h
                jne     parse_badrhs
                test    byte ptr specific.mc_caddr[2],80h
                jnz     parse_badrhs
                or      adapterBits,ADDRESSED
                jmp     short parse_nextkey
;
; for the parameters which specify the number and size of the receive and
; transmit buffers, just save them for later use on OpenAdapter.  ensure
; that the transmit and receive buffer sizes are a multiple of 8.
;
parse_rxbufs:
                mov     rxbufs,cx
                jmp     short parse_nextkey
parse_rxlen:
                test    cx,07h
                jnz     parse_badrhs
                mov     rxbuflen,cx
                jmp     short parse_nextkey
parse_txbufs:
                mov     txbufs,cx
                jmp     short parse_nextkey
parse_txlen:
                test    cx,07h
                jnz     parse_badrhs
                mov     txbuflen,cx
                jmp     short parse_nextkey
parse_transmits:
                mov     specific.mc_txqdepth,cx
                jmp     short parse_nextkey
;
; process next keyword if any
;
parse_nextkey:
                les     bx,es:[bx].kw_next
                jmp     parse_keyloop

;------ parse_done ----------------------------------------------------------;
;
; return success.
;
parse_done:
                xor     ax,ax
parse_exit:
                POPR    bx,cx,dx,si,di,es
                leave
                ret

;------ error handling ------------------------------------------------------;
;
; unrecognized keyword.  es:[bx] is the keyword entry.
;
parse_badkey:
                mov     di,offset DGROUP:msg_nosuchkeyword
                jmp     short parse_errline
;
; invalid right hand side.  es:[bx] is the keyword entry.
;
parse_badrhs:
                mov     di,offset DGROUP:msg_invalidrhs
;
; output error message in DI to user, followed by the failure line for es:[bx]
;
parse_errline:
                call    PutStrang
                call    PutKeyword

                mov     ax,INVALID_PARAMETER
                jmp     short parse_exit
;
; configuration error.
;
parse_cfgerr:
                mov     di,offset DGROUP:msg_badconfigure
;
; output error message in DI to user.
;
parse_errmsg:
                call    PutStrang

                mov     ax,INVALID_PARAMETER
                jmp     parse_exit
;
; general init failure (no adapters configured, too many adapters configured,
; unable to allocate GDT entry)
;
parse_fail:
                mov     ax,GENERAL_FAILURE
                jmp     parse_exit
ParseConfig     endp

                END_INIT_CODE
                end
