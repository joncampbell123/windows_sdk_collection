                page    88,122
                title   DATA - Data Structures (2.0.1 Spec)

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

;------ DATA.ASM ------------------------------------------------------------;
;                                                                            ;
;       This file contains all of the data structures used by the MAC        ;
;       layer, including the device header and any initialization data.      ;
;                                                                            ;
;       Note: Various modules still contain dispatch tables and other small  ;
;       data items of local significance.                                    ;
;                                                                            ;
;       Note also: banners, keyword names and error messages which are       ;
;       language dependent are in MSG.ASM to make internationalization       ;
;       possible just by linking a different version of MSG.OBJ.             ;
;                                                                            ;
;       09-21-88 Connery                                                     ;
;       Original version.                                                    ;
;                                                                            ;
;       03-17-89 Connery                                                     ;
;       Added various tables for 16/4 support.                               ;
;                                                                            ;
;       02-25-90 2.0a Connery                                                ;
;       Added various tables for 16/4 support.  Moved some stuff to init     ;
;       data since the logic was moved to init from bind.  Added phys_ds_0.  ;
;       Use a single stack for system and interrupt handling (the            ;
;       discardable stack was getting overwritten when relocating the        ;
;       initialization code).  Make all data variables global, and get rid   ;
;       of mac_table.  Set txbufs, txbuflen, rxbuflen to 0 so we can         ;
;       default them more intelligently once we know the size of SRAM.       ;
;       Aligned stack on dword boundary.  Added days array.                  ;
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

;------ Imported Interfaces -------------------------------------------------;

                DOSXTRN devInterrupt:FAR        ; from init
                extrn   IndicationOff:NEAR      ; from recv
                extrn   IndicationOn:NEAR       ; from recv
                extrn   IntHandler:FAR          ; from intr
                extrn   ReceiveRelease:NEAR     ; from recv
                extrn   Request:NEAR            ; from req
                extrn   System:NEAR             ; from bind
                extrn   TransferData:NEAR       ; from recv
                extrn   TransmitChain:NEAR      ; from xmit
                extrn   devStrategy:FAR         ; from init
                extrn   keyn_alternate:BYTE     ; from msg
                extrn   keyn_drivername:BYTE    ; from msg
                extrn   keyn_earlyrelease:BYTE  ; from msg
                extrn   keyn_netaddr:BYTE       ; from msg
                extrn   keyn_primary:BYTE       ; from msg
                extrn   keyn_productid:BYTE     ; from msg
                extrn   keyn_ram:BYTE           ; from msg
                extrn   keyn_recvbuflen:BYTE    ; from msg
                extrn   keyn_recvbufs:BYTE      ; from msg
                extrn   keyn_transmits:BYTE     ; from msg
                extrn   keyn_xmitbuflen:BYTE    ; from msg
                extrn   keyn_xmitbufs:BYTE      ; from msg

NEWPAGE <Device Header>
;-------------- Device Header -----------------------------------------------;
;
; the name will be filled in at INIT time (either first_name or second_name)
;
                BEGIN_HEADER
                public  devHeader
IFDEF OS2
DEVATTR         EQU     DEV_CHAR_DEV+DEVLEV_1+DEV_GIOCTL
devHeader       SysDev  <-1,DEVATTR,offset CGROUP:devStrategy,0,'        '>
ELSE
devHeader       dd      -1
                dw      DEV_CHAR_DEV+DEV_IOCTL
                dw      offset CGROUP:devStrategy
                dw      offset CGROUP:devInterrupt
                db      '        '
ENDIF
                END_HEADER

;------ Data in Code Segment ------------------------------------------------;
;
; Address of data segment, retrievable from code segment
;
                BEGIN_CODE
                public  cs_ds
cs_ds           dw      0
                END_CODE

NEWPAGE <Data Definitions>
                BEGIN_DATA
;
; Address of device helper routine
;
IFDEF OS2
                align   4
                public  devHelper
devHelper       dd      0
ENDIF
;
; Timer ticks since last clear
;
IFDEF ANYSTATS
IFDEF DOS
                public  timerCount
timerCount      dd      0
;
; The next timer tick handler in the INT 08h chain
;
                public  timerChain
timerChain      dd      0
ENDIF
ENDIF
;
; Address of strategy packet for interrupt routine
;
IFDEF DOS
                public  saved_bx
saved_bx        dw      0
                public  saved_es
saved_es        dw      0
ENDIF
;
; counters for debugging
;
IFDEF DEV
                public  intstotal
intstotal       dw      0
                public  intsreal
intsreal        dw      0
                public  txsrbs
txsrbs          dw      0
                public  txarbs
txarbs          dw      0
                public  txasbs
txasbs          dw      0
                public  txssbs
txssbs          dw      0
                public  txsrbspended
txsrbspended    dw      0
                public  txlaters
txlaters        dw      0
                public  txcants
txcants         dw      0
                public  txfasters
txfasters       dw      0
                public  rxarbs
rxarbs          dw      0
                public  rxasbs
rxasbs          dw      0
                public  rxlaters
rxlaters        dw      0
                public  asbfrees
asbfrees        dw      0
                public  lastreason
lastreason      dw      0
                public  lastsrb
lastsrb         db      0
                public  lastarb
lastarb         db      0
ENDIF
;
; if SNAP is defined, allocate storage for the snap areas, which will hold
; some useful information (up to 16 bytes) for the last 64 packets transmitted
; and received by this driver.
;
IFDEF SNAP
                public  txsnapix
txsnapix        dw      0
                public  rxsnapix
rxsnapix        dw      0
                public  txsnaparea
txsnaparea      db      64 * 16 dup (0)
                public  rxsnaparea
rxsnaparea      db      64 * 16 dup (0)
ENDIF
;
; following is the 18-byte product id used on OpenAdapter.  the default
; specified here can be overriden via the PRODUCTID keyword in PROTOCOL.INI.
;
                public  productid
productid       db      01h,10h,0F0h,0F0h,0F0h,0F0h,0F0h,0F0h,0F0h,0F0h
                db      0F0h,0F0h,0F0h,0F0h,0F0h,0F0h,0F0h,0F0h
;
; Protocol Manager request block
;
                public  rqblock
rqblock         ReqBlock <>
;
; serial number string
;
                public  serialnumber
serialnumber    db      'IBM Token Ring Adapter',0
;
; the global info seg selector
;
IFDEF ANYSTATS
IFDEF OS2
                public  infoSeg
infoSeg         dw      0
ENDIF
ENDIF
                END_DATA

NEWPAGE <Queues>
;
; transmit and request queue entries.  to allow the transmit queue size to be
; configurable the transmit queue entries are placed in a separate segment so
; they can be positioned just in front of the discardable stuff.  under OS/2
; we can't extend the size of our data segment so the space is pre-allocated.
; under DOS we only need a label here and we will allocate the actual qentries
; over the init code and data.
;
; note: the point is that allocating space for 50 x 128+ bytes here makes the
;       file rather larger than necessary on disk and we want to be able to
;       put a lot of MAC drivers on a single floppy.
;
; note2: if we split the OS/2 data segment in two, a device header segment
;        low and moved the data segment high, we wouldn't have this problem
;        and could shrink the size of the file on disk, as well as the
;        real mode footprint.
;
                BEGIN_QSPACE
                even
                public  qtarray
IFDEF OS2
qtarray         db      MAX_TX_PENDING * QT_SIZE dup (0)
ELSE
qtarray         label   byte
ENDIF
                END_QSPACE

                BEGIN_DATA
                even
                public  qrarray
qrarray         db      MAX_RQ_PENDING * QR_SIZE dup (0)
                END_DATA

NEWPAGE <Disposable Data/Initialization Data>
                BEGIN_INIT_DATA
;
; A dummy word location for OS/2 calls
;
IFDEF OS2
                public  dummy
dummy           dw      0
ENDIF
;
; The number of days before each month
;
                public  days
days            dw      -1, 30, 58, 89, 119, 150, 180, 211, 242, 272, 303, 333, 364
;
; Whether the adapter is PRIMARY, ALTERNATE or yet to be determined
;
                public  primAlt
primAlt         db      UNKNOWN         ; UNKNOWN, PRIMARY or ALTERNATE
;
; physDS is the physical address of DS:0
;
IFDEF OS2
                public  physDS
physDS          dd      0
ENDIF
;
; number_buffer is a buffer used by the PutLong routine
;
                public  number_buffer
number_buffer   db      12 dup (0)
;
; following are the adapter identification strings for the Token Ring Adapters
; I and II (the same) and the Adapter/A for the microchannel PS/2s.
;
                public  IDString
IDString        db      05h,00h,04h,09h,04h,03h,04h,0Fh
                public  IDString_A
IDString_A      db      04h,0Dh,04h,01h,05h,02h,05h,03h
;
; following are the interrupt levels corresponding to the interrupt level
; set by the switches for the Adapters I and II (the same) and the /A.
;
                public  IntLevel
IntLevel        db      2,3,6,7
                public  IntLevel_A
IntLevel_A      db      2,3,10,11
;
; index table for rates supporting early token release based on the ADIAEARLY
; nibble [0..15]
;
                public  early_table
early_table     db      0                       ; 0: undefined, not supported
                db      0                       ; 1: undefined, not supported
                db      0                       ; 2: undefined, not supported
                db      0                       ; 3: undefined, not supported
                db      0                       ; 4: undefined, not supported
                db      0                       ; 5: undefined, not supported
                db      0                       ; 6: undefined, not supported
                db      0                       ; 7: undefined, not supported
                db      0                       ; 8: undefined, not supported
                db      0                       ; 9: undefined, not supported
                db      0                       ; A: undefined, not supported
                db      0                       ; B: undefined, not supported
                db      4+16                    ; C: 4Mbps and 16Mbps
                db      16                      ; D: 16Mbps
                db      4                       ; E: 4Mbps
                db      0                       ; F: not supported
;
; index table for data rates available based on the ADIARATE nibble [0..15]
;
                public  rate_table
rate_table      db      4                       ; 0: undefined, assume 4Mbps
                db      4                       ; 1: undefined, assume 4Mbps
                db      4                       ; 2: undefined, assume 4Mbps
                db      4                       ; 3: undefined, assume 4Mbps
                db      4                       ; 4: undefined, assume 4Mbps
                db      4                       ; 5: undefined, assume 4Mbps
                db      4                       ; 6: undefined, assume 4Mbps
                db      4                       ; 7: undefined, assume 4Mbps
                db      4                       ; 8: undefined, assume 4Mbps
                db      4                       ; 9: undefined, assume 4Mbps
                db      4                       ; A: undefined, assume 4Mbps
                db      4                       ; B: undefined, assume 4Mbps
                db      4                       ; C: undefined, assume 4Mbps
                db      4+16                    ; D: 4Mbps and 16Mbps
                db      16                      ; E: 16Mbps
                db      4                       ; F: 4Mbps
;
; index table for the amount of memory based on the ADIASRAM nibble [0..15]
;
                public  sram_table
sram_table      db      0                       ; 0: undefined, use RRR
                db      0                       ; 1: undefined, use RRR
                db      0                       ; 2: undefined, use RRR
                db      0                       ; 3: undefined, use RRR
                db      0                       ; 4: undefined, use RRR
                db      0                       ; 5: undefined, use RRR
                db      0                       ; 6: undefined, use RRR
                db      0                       ; 7: undefined, use RRR
                db      0                       ; 8: undefined, use RRR
                db      0                       ; 9: undefined, use RRR
                db      64                      ; A: 64k
                db      64                      ; B: 64k
                db      32                      ; C: 32k
                db      16                      ; D: 16k
                db      8                       ; E: 8k
                db      0                       ; F: use RRR
;
; index table for the amount of memory based on the RRR bits (3,2)
;
                public  rrr_table
rrr_table       db      8                       ; 0: 8k
                db      16                      ; 1: 16k
                db      32                      ; 2: 32k
                db      64                      ; 3: 64k
;
; index table for the page sizes supported based on the ADIAPAGE nibble [0..15]
; each byte is set up as a bit mask for testing against SRAM sizes in K.
;
                public  page_table
page_table      db      0                       ; 0: undefined, no paging
                db      0                       ; 1: undefined, no paging
                db      0                       ; 2: undefined, no paging
                db      0                       ; 3: undefined, no paging
                db      0                       ; 4: undefined, no paging
                db      0                       ; 5: undefined, no paging
                db      0                       ; 6: undefined, no paging
                db      0                       ; 7: undefined, no paging
                db      0                       ; 8: undefined, no paging
                db      0                       ; 9: undefined, no paging
                db      0                       ; A: undefined, no paging
                db      0                       ; B: undefined, no paging
                db      16+32                   ; C: 16k and 32k pages
                db      32                      ; D: 32k page
                db      16                      ; E: 16k page
                db      0                       ; F: no paging
;
; index table for the maximum frame size at 4Mbps based on the ADIADHB_4MBPS
; nibble [0..15]
;
                public  max4_table
max4_table      dw      2048                    ; 0: undefined, assume default
                dw      2048                    ; 1: undefined, assume default
                dw      2048                    ; 2: undefined, assume default
                dw      2048                    ; 3: undefined, assume default
                dw      2048                    ; 4: undefined, assume default
                dw      2048                    ; 5: undefined, assume default
                dw      2048                    ; 6: undefined, assume default
                dw      2048                    ; 7: undefined, assume default
                dw      2048                    ; 8: undefined, assume default
                dw      2048                    ; 9: undefined, assume default
                dw      2048                    ; A: undefined, assume default
                dw      2048                    ; B: undefined, assume default
                dw      2048                    ; C: undefined, assume default
                dw      4464                    ; D: 4464
                dw      4096                    ; E: 4096
                dw      2048                    ; F: 2048
;
; index table for the maximum frame size at 16Mbps based on the ADIADHB_16MBPS
; nibble [0..15]
;
                public  max16_table
max16_table     dw      2048                    ; 0: undefined, assume default
                dw      2048                    ; 1: undefined, assume default
                dw      2048                    ; 2: undefined, assume default
                dw      2048                    ; 3: undefined, assume default
                dw      2048                    ; 4: undefined, assume default
                dw      2048                    ; 5: undefined, assume default
                dw      2048                    ; 6: undefined, assume default
                dw      2048                    ; 7: undefined, assume default
                dw      2048                    ; 8: undefined, assume default
                dw      2048                    ; 9: undefined, assume default
                dw      2048                    ; A: undefined, assume default
                dw      17960                   ; B: 17960
                dw      16384                   ; C: 16384
                dw      8192                    ; D: 8192
                dw      4096                    ; E: 4096
                dw      2048                    ; F: 2048
;
; parameters for protocol manager open
;
                public  protmgr_handle
protmgr_handle  dw      0                       ; Protocol Manager Open Handle
                public  protmgr_name
ifdef OS2
protmgr_name    db      '/DEV/PROTMAN$',0       ; Protocol Manager Device Name
else
protmgr_name    db      'PROTMAN$',0            ; Protocol Manager Device Name
endif
                public  action_taken
action_taken    dw      0
;
; our_name contains the offset of the device name we are using.
;
                public  first_name
first_name      db      'IBMTOK$',0
                public  second_name
second_name     db      'IBMTOK2$',0

                public  our_name
our_name        dw      offset DGROUP:first_name
;
; KbdCharIn structure
;
IFDEF OS2
                public  CharData
CharData        db      10 dup (0)
ENDIF
;
; the two possible right hand sides for RAM= (if not an integer)
;
                public  ram_3Com
ram_3Com        db      '3Com',0
                public  ram_IBM
ram_IBM         db      'IBM',0
;
; the method for determining the RAM= location
;
                public  ram_method
ram_method      db      RAM_AFTER_ROM

NEWPAGE <Keyword Table>
;------ Keyword Table -------------------------------------------------------;
;
; The following table defines the keywords that are recognized by this driver
; as well as a few of their characteristics (right hand side required, range
; of legal values, etc) to allow general purpose validation/processing
;
                public  keyword_table
keyword_table   label   word

        key_entry <offset DGROUP:keyn_drivername,KEY_DRIVERNAME,RHS_STRING,1,8>
        key_entry <offset DGROUP:keyn_primary,KEY_PRIMARY,RHS_NONE,0,0>
        key_entry <offset DGROUP:keyn_alternate,KEY_ALTERNATE,RHS_NONE,0,0>
        key_entry <offset DGROUP:keyn_recvbufs,KEY_RECVBUFS,RHS_INTEGER,2,60>
        key_entry <offset DGROUP:keyn_recvbuflen,KEY_RECVBUFLEN,RHS_INTEGER,256,17952>
        key_entry <offset DGROUP:keyn_xmitbufs,KEY_XMITBUFS,RHS_INTEGER,1,2>
        key_entry <offset DGROUP:keyn_xmitbuflen,KEY_XMITBUFLEN,RHS_INTEGER,256,17952>
        key_entry <offset DGROUP:keyn_ram,KEY_RAM,RHS_ANYTHING,0A000h,0EC00h>
        key_entry <offset DGROUP:keyn_productid,KEY_PRODUCTID,RHS_STRING,2,36>
        key_entry <offset DGROUP:keyn_netaddr,KEY_NETADDR,RHS_STRING,12,12>
        key_entry <offset DGROUP:keyn_transmits,KEY_TRANSMITS,RHS_INTEGER,6,MAX_TX_PENDING>
        key_entry <offset DGROUP:keyn_earlyrelease,KEY_EARLYRELEASE,RHS_NONE,0,0>

                dw      0

                END_INIT_DATA

NEWPAGE <MAC Configuration Data>
;------ MAC Configuration Data ----------------------------------------------;

                BEGIN_DATA
;
; Interrupt handler nest level.  Two levels (1 and 2) allowed.
;
                public  nestLevel
nestLevel       db      0               ; current interrupt nest level
;
; SRB in use flag.
;
                public  SRBinuse
SRBinuse        db      NO              ; YES = SRB in use
;
; Acked flag, set when SRB acknowledged.
;
                public  acked
acked           db      NO              ; YES = SRB acknowledged
;
; Indication off depth.  If non-zero, interrupts from adapter are masked off.
;
                public  offDepth
offDepth        db      0               ; IndicationOff depth
                public  startDepth
startDepth      db      0               ; offDepth after StartReset
;
; The number of transmit SRBs that need to be issued
;
                public  txNeeded
txNeeded        db      0               ; number of tx SRBs that need issuing
;
; Various bits indicating adapter type
;
                public  adapterBits
adapterBits     db      0               ; various bits indicating adapter type
;
; The current adapter rate (4 or 16) in Mbps and the rates at which early
; token release is supported
;
                public  rate
rate            db      4               ; 4 or 16 Mbps
                public  earlyRates
earlyRates      db      0               ; early token release rate mask
;
; The size of the SRAM on the adapter in K
;
                public  ramSize
ramSize         db      8               ; SRAM size in k
;
; Receiving flag, YES = receive packet, NO = ignore them
;
                public  receiving
receiving       db      NO              ; YES = generate Indications
;
; The indicate flag for passing upwards on indications
;
                public  indications
indications     db      0FFh            ; Indicate flag
;
; Enable and disable masks for the PIC
;
                public  disableMask
disableMask     db      0               ; mask to disable interrupts through PIC
                public  enableMask
enableMask      db      0               ; mask to enable interrupts through PIC
;
; Indication Complete pending flag
; Interrupt Indication pending flag
;
                public  ICpending
ICpending       db      NO              ; YES = Indication Complete pending
                public  Intpending
Intpending      db      NO              ; YES = Indication Interrupt pending
;
; A counter for timer ticks
;
                even
                public  timerTicks
timerTicks      dw      0               ; interrupt timer counter
;
; The remaining reasons for the current interrupt from the adapter
;
                public  reasoning
reasoning       dw      0               ; remaining interrupt reasons
;
; The first port on the adapter and the release port (optimization)
;
                public  portOffset
portOffset      dw      0               ; offset from primary ports (0 or 4)
                public  releasePort
releasePort     dw      0               ; interrupt release port
;
; Interrupt Mask Register port
;
                public  IMRport
IMRport         dw      0               ; interrupt mask port (IMR on PIC)
;
; EOIs to issue to 8259
;
                public  EOIs
EOIs            dw      0               ; EOIs to issue (lo=2nd PIC, hi=1st)
;
; Number and size of transmit and receive buffers.  Defaults are zero so we
; can do adapter-sensitive defaulting if not specified.
;
                public  txbufs
txbufs          dw      0               ; number of transmit buffers
                public  txbuflen
txbuflen        dw      0               ; size of transmit buffers (0=default)
                public  rxbufs
rxbufs          dw      2               ; number of receive buffers
                public  rxbuflen
rxbuflen        dw      0               ; size of receive buffers
;
; The maximum packet size supported at each data rate
;
                public  max@4Mbps
max@4Mbps       dw      0               ; maximum packet size @ 4Mbps
                public  max@16Mbps
max@16Mbps      dw      0               ; maximum packet size @ 16Mbps
;
; The protocol's dataseg for quick access
;
                public  protDS
protDS          dw      0               ; protocol's dataseg
;
; Current state, and state before initiating ResetMAC
;
                public  state
state           db      EMBRYONIC       ; adapter/driver state
                public  oldstate
oldstate        db      EMBRYONIC       ; original state before reset started
;
; The interrupt reasons we are interested in.  Set to 0 temporarily by ResetMAC.
;
                public  reasonMask
reasonMask      dw      ISRPMASK        ; mask for reasons
;
; Return code (or Bring-up code) for last command
;
                public  retcode
retcode         dw      0               ; Return code for last command
;
; Open options.  Those supplied on the OpenAdapter Request and those forced
; set by keywords in PROTOCOL.INI
;
                public  openSoft
openSoft        dw      0               ; options on OpenAdapter Request
                public  openHard
openHard        dw      0               ; options hard-wired by keyword
;
; Offsets to SRB, ARB, ASB and SSB in shared RAM
;
                public  oSRB
oSRB            dw      0               ; system request block offset in segSM
                public  oARB
oARB            dw      0               ; adapter request block offset in segSM
                public  oASB
oASB            dw      0               ; adapter status block offset in segSM
                public  oSSB
oSSB            dw      0               ; system status block offset in segSM
;
; the real MMIO and SRAM segment
;
                public  realMMIO
realMMIO        dw      0               ; real mode MMIO segment
                public  realSM
realSM          dw      0               ; real mode Shared Memory segment
;
; The receive buffer in progress and its size
;
                public  rxHead
rxHead          dw      0               ; first receive buffer in chain
                public  rxSize
rxSize          dw      0               ; size of current received frame
;
; The receive buffer awaiting release (or 0)
;
                public  rxRelease
rxRelease       dw      0               ; offset of rx buffer to release
;
; The transmit buffer offset if any (or 0) and its correlate
;
                public  txBuffer
txBuffer        dw      0               ; tx buffer available if any
                public  txCorrelate
txCorrelate     db      0               ; tx correlate for above
                db      ?
;
; Selectors used for various purposes.  If FASTGDT is defined, we set these
; up for its purposes.
;
                align   4
                public  segMMIO
segMMIO         dw      0               ; GDT selector for MMIO (or DOS segment)
IFDEF FASTGDT
                dw      OS2_GDT
ENDIF

                public  segSM
segSM           dw      0               ; GDT selector for Shared Memory (ditto)
IFDEF FASTGDT
                dw      OS2_GDT
ENDIF

IFDEF OS2
                public  segTX
segTX           dw      0               ; GDT selector for transmit
ENDIF
IFDEF FASTGDT
                dw      OS2_GDT
ENDIF

IFDEF OS2
                public  segRX
segRX           dw      0               ; GDT selector for receive
ENDIF
IFDEF FASTGDT
                dw      OS2_GDT
ENDIF
;
; Queues headers, initialized to empty.
;
                even
                INITQ   q_rqfree        ; request free queue
                INITQ   q_rqtodo        ; request todo queue
                INITQ   q_rqdone        ; request awaiting completion queue
                INITQ   q_rqconfirm     ; request confirmation queue
                FREEQ   q_txfree        ; transmit free queue
                INITQ   q_txtodo        ; transmit todo queue
                FREEQ   q_txawaits      ; transmit awaiting start queue
                INITQ   q_txdone        ; transmit awaiting completion queue
                INITQ   q_txconfirm     ; transmit confirmation queue
;
; Common characteristics table
;
                public  common
common          label   word
                dw      CCTABLESIZE
                db      PROTMAN_VERSION_MAJOR   ; Major NDIS version
                db      PROTMAN_VERSION_MINOR   ; Minor NDIS version
                dw      0                       ; reserved
                db      MAJOR_MAC_VERSION       ; major module version
                db      MINOR_MAC_VERSION       ; minor module version
                dd      UPPERBIND               ; function flags
                db      'IBMTOK',10 dup (0)     ; module name
                db      UL_MAC                  ; upper module type
                db      UI_MAC                  ; upper module interface
                db      LL_PHYSICAL             ; lower module type
                db      0                       ; lower module interface
                dw      ?                       ; module ID (set by protmgr)
                dw      ?                       ; module DS
                dw      offset CGROUP:System    ; system dispatch entry
                dw      ?                       ; + selector
                dw      offset DGROUP:specific  ; specific characteristics table
                dw      ?                       ; + selector
                dw      offset DGROUP:status    ; mac status table
                dw      ?                       ; + selector
                dw      offset DGROUP:mdispatch ; mac's upper dispatch table
                dw      ?
                dd      0                       ; no lower dispatch table
                dd      0                       ; reserved
                dd      0                       ; reserved

                CLAIM   <($-common) eq CCTABLESIZE>
;
; MAC-specific characteristics table
;
SV1             equ     SV_BROADCAST+SV_GROUPADDR+SV_SOFTWARE+SV_LOOPBACK
SV2             equ     SV_OPENCLOSE+SV_INTERRUPT+SV_SRCROUTE+SV_CURSTAT
SV3             equ     SV_GDTADDR+SV_MULTIXFER+SV_RESET

                public  specific
specific        label   word
                dw      MCTABLESIZE
                db      '802.5', 11 dup (0)
                dw      ADDRLEN
                db      16 dup (0)      ; permanent node address
                db      16 dup (0)      ; current node address
                db      4 dup (0)       ; current functional address
                dw      offset DGROUP:multilist
                dw      ?               ; + selector
                dd      4000000         ; in bits per second (4Mbps)
                dd      SV1+SV2+SV3     ; service characteristics
                dw      2040            ; maximum packet size
                dd      ?               ; total tx buffer space
                dw      ?               ; tx buffer size
                dd      ?               ; total rx buffer space
                dw      ?               ; rx buffer size
                db      02h,60h,8Ch     ; IEEE vendor code (3Com)
                db      10              ; adapter code (3Com code for IBM TRA)
                dw      offset DGROUP:serialnumber
                dw      ?               ; + selector
                dw      0               ; interrupt level used
                dw      DEF_TX_PENDING  ; transmit queue depth supported
                dw      MAX_CHAIN       ; maximum number of data blocks

                CLAIM   <($-specific) eq MCTABLESIZE>
;
; Multicast address list
;
; used only for the single group address
;
                public  multilist
multilist       label   word
                dw      1               ; maximum number of entries
                dw      0               ; number of entries
                db      16 dup (0)      ; single entry table

                CLAIM   <($-multilist) eq MATABLESIZE>
;
; MAC-specific status table
;
                even
                public  status
status          label   word
                dw      MSTABLESIZE
                dd      NOT_MAINTAINED  ; date/time of last diagnostic
                dd      0               ; MAC status flags (0 initially)
                dw      0               ; current packet filter (0 initially)
                dd      0               ; pointer to media specific statistics (none)
                dd      NOT_MAINTAINED  ; date/time of last clear statistic (set if maintained)
                dd      INIT_ANY        ; total frames received
                dd      NOT_MAINTAINED  ; CRC error on receive
                dd      INIT_ANY        ; total bytes received
                dd      NOT_MAINTAINED  ; frames discarded (no buffers)
                dd      INIT_ALL        ; multicast frames received
                dd      INIT_ALL        ; broadcast frames received
                dd      NOT_MAINTAINED  ; frame error on receive
                dd      NOT_MAINTAINED  ; frame exceeding maximum size
                dd      NOT_MAINTAINED  ; frame smaller than minium size
                dd      INIT_ALL        ; multicast bytes received
                dd      INIT_ALL        ; broadcast bytes received
                dd      NOT_MAINTAINED  ; frames discarded due to hardware error
                dd      INIT_ANY        ; total frames sent
                dd      INIT_ANY        ; total no. of bytes sent
                dd      INIT_ALL        ; multicast frames sent
                dd      INIT_ALL        ; broadcast frames sent
                dd      INIT_ALL        ; broadcast bytes sent
                dd      INIT_ALL        ; multicast bytes sent
                dd      NOT_MAINTAINED  ; sending error due to timeout
                dd      INIT_ANY        ; sending error due to hardware

                CLAIM   <($-status) eq MSTABLESIZE>
;
; MAC upper dispatch table
;
                align   4
                public  mdispatch
mdispatch       label   byte
                dw      offset DGROUP:common
                dw      ?
                dw      offset CGROUP:Request
                dw      ?
                dw      offset CGROUP:TransmitChain
                dw      ?
                dw      offset CGROUP:TransferData
                dw      ?
                dw      offset CGROUP:ReceiveRelease
                dw      ?
                dw      offset CGROUP:IndicationOn
                dw      ?
                dw      offset CGROUP:IndicationOff
                dw      ?

                CLAIM   <($-mdispatch) eq MDTABLESIZE>
;
; protocol lower dispatch table (local copy)
;
                align   4
                public  pdispatch
pdispatch       label   byte
                pdtable <>
;
; stack for the DOS device driver
;
IFDEF DOS
                align   4
                public  DDStk
DDStk           db      (STACKSIZE / 4) dup ('STAK')
                public  DDTopStk
DDTopStk        dw      0AAAAh
                public  DDStkSeg
DDStkSeg        dw      0
                public  DDStkOff
DDStkOff        dw      0
ENDIF

                END_DATA
                end
