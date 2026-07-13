;
; This file was generated automatically from the C include file
; and should not be edited directly.
;

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

;
; This file contains structures and equates defined in the
; LAN Manager Network Driver Interface Specification 2.0.1
;
; -- Revision History ---------------------------------------------------
;
; $Log: X:/VCS/H/MAC.H_V $
;
; Rev 1.1 27 Jul 1988 13:35:56 TuyenQ
; NDIS 1.0.1 changes.
; NDIS 2.0.1 changes.
;

;
; Commonly used typedefs
;

;typedef unsigned int WORD;
;typedef unsigned char BYTE;
;typedef unsigned long DWORD;

;
; Common Characteristics Table
;

cctable                 struc
cc_size                 dw      0               ; size of this table
cc_ndis_maj             db      0               ; Major NDIS version
cc_ndis_min             db      0               ; Minor NDIS version
cc_reserved             dw      0               ; service specific, MAC=0
cc_mod_major            db      0               ; major module version
cc_mod_minor            db      0               ; minor module version
cc_function             dd      0               ; module function flags
cc_name                 db      16 dup (0)      ; module name
cc_up_module            db      0               ; protocol level at upper module boundary
cc_up_if                db      0               ; upper module interface type
cc_low_module           db      0               ; protocol level at lower module boundary
cc_low_if               db      0               ; lower module interface type
cc_ID                   dw      0               ; module ID
cc_DS                   dw      0               ; module DS
cc_system               dd      0               ; module's system dispatch handler
cc_specific             dd      0               ; pointer to service specific characteristics
cc_status               dd      0               ; pointer to service specific status table
cc_udispatch            dd      0               ; pointer to upper dispatch table
cc_ldispatch            dd      0               ; pointer to lower dispatch table
cc_resv1                dd      0               ; reserved
cc_resv2                dd      0               ; reserved
cctable                 ends

CCTABLESIZE             equ     SIZE cctable

; Module function flags (for cctable.cc_function)

UPPERBIND               equ     1h
LOWERBIND               equ     2h
DYNAMICBIND             equ     4h

; Protocol level at upper boundary (for cctable.cc_up_module)

UL_MAC                  equ     1
UL_DATALINK             equ     2
UL_NETWORK              equ     3
UL_TRANSPORT            equ     4
UL_SESSION              equ     5
UL_NONE                 equ     0FFh

; Type of interface at upper boundary (for cctable.cc_up_if)

UI_PRIVATE              equ     0
UI_MAC                  equ     1
UI_SESSION_NCB          equ     1

; Protocol level at lower boundary (for cctable.cc_low_module)

LL_PHYSICAL             equ     0
LL_MAC                  equ     1
LL_DATALINK             equ     2
LL_NETWORK              equ     3
LL_TRANSPORT            equ     4
LL_SESSION              equ     5
LL_NONE                 equ     0FFh

; Type of interface at lower boundary (for cctable.cc_low_if)

LI_PRIVATE              equ     0
LI_MAC                  equ     1
LI_SESSION_NCB          equ     1

; Multicast address

mcaddr                  struc                   ; Multicast address
mac_address             db      16 dup (0)
mcaddr                  ends

; Multicast address table

matable                 struc
ma_max                  dw      0               ; no. of entries
ma_cur                  dw      0               ; current no. of multicast addr
ma_mbuff                db      (1)*(size mcaddr) dup (0)       ; multicast buffer area
matable                 ends

MATABLESIZE             equ     SIZE matable

;
; MAC service specific characteristic table
;

mctable                 struc
mc_size                 dw      0               ; size of this table
mc_name                 db      16 dup (0)      ; type name of MAC, eg 802.3
mc_addrsize             dw      0               ; address size
mc_paddr                db      16 dup (0)      ; permanent address
mc_caddr                db      16 dup (0)      ; current address
mc_faddr                db      4 dup (0)       ; functional address for adapter
mc_multicast            dd      0               ; pointer to multicast address table
mc_linkspeed            dd      0               ; link speed (bits/sec)
mc_flags                dd      0               ; service flags
mc_maxsize              dw      0               ; maxium frame size
mc_totaltxspc           dd      0               ; total transmit buffer space
mc_txblksize            dw      0               ; transmission buffer alloc blk size
mc_totalrxspc           dd      0               ; total reception buffer space
mc_rxblksize            dw      0               ; reception buffer alloc blk size
mc_code                 db      3 dup (0)       ; IEEE vendor code
mc_acode                db      0               ; vendor adapter code
mc_serial               dd      0               ; pointer to vendor serial no.
mc_interrupt            dw      0               ; adapter interrupt level
mc_txqdepth             dw      0               ; transmit queue depth
mc_maxblocks            dw      0               ; max number of data blocks
mctable                 ends

MCTABLESIZE             equ     SIZE mctable

;
; MAC service flags (for mctable.mc_flags)
;

SV_BROADCAST            equ     0001h
SV_MULTICAST            equ     0002h
SV_GROUPADDR            equ     0004h
SV_PROMISCUOUS          equ     0008h
SV_SOFTWARE             equ     0010h
SV_CURSTAT              equ     0020h
SV_INITDIAG             equ     0040h
SV_LOOPBACK             equ     0080h
SV_RXCHAININD           equ     0100h
SV_SRCROUTE             equ     0200h
SV_RESET                equ     0400h
SV_OPENCLOSE            equ     0800h
SV_INTERRUPT            equ     1000h
SV_SRBRIDGE             equ     2000h
SV_GDTADDR              equ     4000h
SV_MULTIXFER            equ     8000h

;
; MAC service specific status table
;

mstable                 struc
ms_size                 dw      0               ; size of this table
ms_ddate                dd      0               ; Date/time of last diagnostic
ms_flags                dd      0               ; MAC status flags
ms_filter               dw      0               ; current packet filter bit mask
ms_media_spec           dd      0               ; pointer to media specific status
ms_cdate                dd      0               ; Date/time of last clear statistic
ms_total_recv           dd      0               ; total frames received
ms_crc_rerr             dd      0               ; CRC error on receive
ms_total_rbytes         dd      0               ; total bytes received
ms_nobuf_rerr           dd      0               ; frames discarded due to lack of receive buffers
ms_mult_recv            dd      0               ; multicast frames received
ms_broad_recv           dd      0               ; broadcast frames received
ms_frame_rerr           dd      0               ; frame error on receive
ms_too_long             dd      0               ; frame exceeding maxium size
ms_too_short            dd      0               ; frame smaller than minium size
ms_mult_rbytes          dd      0               ; multicast bytes received
ms_broad_rbytes         dd      0               ; broadcast bytes received
ms_hard_rerr            dd      0               ; frames discarded due to hardware error
ms_total_sent           dd      0               ; frames sent
ms_total_sbytes         dd      0               ; total no. of bytes sent
ms_mult_sent            dd      0               ; multicast frames sent
ms_broad_sent           dd      0               ; broadcast frames sent
ms_broad_sbytes         dd      0               ; broadcast bytes sent
ms_mult_sbytes          dd      0               ; multicast bytes sent
ms_timeout_serr         dd      0               ; sending error due to timeout
ms_hard_serr            dd      0               ; sending error due to hardware
mstable                 ends

MSTABLESIZE             equ     SIZE mstable

; MAC service specific status flags (for mstable.ms_flags)

ST_NOT_INS              equ     0000h
ST_DIAG_ERR             equ     0001h
ST_CONFIG_ERROR         equ     0002h
ST_FAULT                equ     0003h
ST_SOFT_FAULT           equ     0004h
ST_OK                   equ     0007h
ST_MAC_IS_BOUND         equ     0008h
ST_MAC_IS_OPEN          equ     0010h
ST_DIAGNOSTICS          equ     0020h

; Current packet filter mask (for mstable.ms_filter)

FM_DIRECTED             equ     0001h
FM_MULTICAST            equ     0001h
FM_GROUP                equ     0001h
FM_FUNCTIONAL           equ     0001h
FM_BROADCAST            equ     0002h
FM_PROMISCUOUS          equ     0004h
FM_SRC_ROUTING          equ     0008h

;
; MAC dispatch table
;

mdtable                 struc
md_cctable              dd      0               ; MAC's characteristic table
md_request              dd      0               ; MAC's request entry point
md_txchain              dd      0               ; MAC's transmit chain entry point
md_txferdata            dd      0               ; MAC's transfer data entry point
md_rxrelease            dd      0               ; MAC's receive release entry point
md_indication_on        dd      0               ; MAC's indication on entry point
md_indication_off       dd      0               ; MAC's indication off entry point
mdtable                 ends

MDTABLESIZE             equ     SIZE mdtable

;
; Protocol dispatch table
;

pdtable                 struc
pd_cctable              dd      0               ; protocol's characteristic table
pd_if                   dd      0               ; protocol's interface flags
pd_reqconfirm           dd      0               ; protocol's request confirmation EP
pd_txconfirm            dd      0               ; protocol's transmit confimation EP
pd_rxlookahead          dd      0               ; protocol's receive lookahead EP
pd_indcomplete          dd      0               ; protocol's indication complete EP
pd_rxchain              dd      0               ; protocol's receive chain EP
pd_status               dd      0               ; protocol's status indication EP
pdtable                 ends

PDTABLESIZE             equ     SIZE pdtable

; Interface flag in protocol dispatch table (for pdtable.pd_if)

IF_NONLLC               equ     0001h
IF_LSAP                 equ     0002h
IF_NONLSAP              equ     0004h

;
; Protocol service specific characteristic table
;

pctable                 struc
pc_size                 dw      0               ; size of this table
pc_name                 db      16 dup (0)      ; type name of protocol
pc_type                 dw      0               ; protocol type
pctable                 ends

PCTABLESIZE             equ     SIZE mctable


;
; request function opcode
;

INITIATE_DIAGNOSTICS    equ     1
READ_ERROR_LOG          equ     2
SET_STATION_ADDRESS     equ     3
OPEN_ADAPTER            equ     4
CLOSE_ADAPTER           equ     5
RESET_MAC               equ     6
SET_PACKET_FILTER       equ     7
ADD_MULTICAST_ADDRESS   equ     8
DELETE_MULTICAST_ADDRESS        equ     9
UPDATE_STATISTICS       equ     10
CLEAR_STATISTICS        equ     11
INTERRUPT               equ     12
SET_FUNCTIONAL_ADDRESS  equ     13
SET_LOOKAHEAD           equ     14

MAX_REQUEST_OPCODE      equ     14
MAX_LOOKAHEAD           equ     256
MIN_LOOKAHEAD           equ     64

;
; opcode for indication
;

RING_STATUS             equ     1
ADAPTER_CHECK           equ     2
START_RESET             equ     3
INTERRUPT_INDICATION    equ     4
END_RESET               equ     5

MAX_STATUS_OPCODE       equ     5

;
; Opcode for system request
;

INITIATE_BIND           equ     1
BIND                    equ     2
INITIATE_PREBIND        equ     3
INITIATE_UNBIND         equ     4
UNBIND                  equ     5

MAX_SYSTEM_OPCODE       equ     5

;
; Error codes
;

SUCCESS                 equ     0000h
WAIT_FOR_RELEASE        equ     0001h
REQUEST_QUEUED          equ     0002h
FRAME_NOT_RECOGNIZED    equ     0003h
FRAME_REJECTED          equ     0004h
FORWARD_FRAME           equ     0005h
OUT_OF_RESOURCES        equ     0006h
INVALID_PARAMETER       equ     0007h
INVALID_FUNCTION        equ     0008h
NOT_SUPPORTED           equ     0009h
HARDWARE_ERROR          equ     000Ah
TRANSMIT_ERROR          equ     000Bh
NO_SUCH_DESTINATION     equ     000Ch
BUFFER_TOO_SMALL        equ     000Dh

ALREADY_STARTED         equ     0020h
INCOMPLETE_BINDING      equ     0021h
DRIVER_NOT_INITIALIZED  equ     0022h
HARDWARE_NOT_FOUND      equ     0023h
HARDWARE_FAILURE        equ     0024h
CONFIGURATION_FAILURE   equ     0025h
INTERRUPT_CONFLICT      equ     0026h
INCOMPATIBLE_MAC        equ     0027h
INITIALIZATION_FAILED   equ     0028h
NO_BINDING              equ     0029h
NETWORK_MAY_NOT_BE_CONNECTED    equ     002Ah
INCOMPATIBLE_OS_VERSION equ     002Bh
ALREADY_REGISTERED      equ     002Ch
PATH_NOT_FOUND          equ     002Dh
INSUFFICIENT_MEMORY     equ     002Eh
INFO_NOT_FOUND          equ     002Fh

GENERAL_FAILURE         equ     00FFh

;
; Buffer chain descriptor for transmit/transfer data
;

MAX_CHAIN               equ     8
MAX_IMMEDIATE           equ     64
CT_PHYSICAL             equ     0
CT_GDT                  equ     2

chaindesc               struc
cd_ptr_type             db      0               ; Type of pointer
cd_reserved             db      0
cd_data_len             dw      0               ; bytes of data
cd_data_buff            dd      0               ; data buffer (virtual or physical)
chaindesc               ends

CDSIZE                  equ     SIZE chaindesc

;
; Transmit buffer descriptor
;

txbufdesc               struc
tx_immed_bytes          dw      0               ; immediate data
tx_immed_buff           dd      0               ; virtual address of buffer
tx_data_count           dw      0               ; chained data count
tx_chain                db      (1)*(size chaindesc) dup (0)    ; first chain descr block
txbufdesc               ends

TXBDSIZE                equ     MAX_CHAIN * CDSIZE + SIZE txbufdesc - 2

;
; Tranfer data buffer descriptor
;

tddesc                  struc
td_data_count           dw      0               ; no. of chained buffer
td_chain                db      (1)*(size chaindesc) dup (0)    ; first chain desc block
tddesc                  ends

TDSIZE                  equ     SIZE tddesc - 2 + MAX_CHAIN * CDSIZE

;
; Buffer chain descriptor for receive_chain
;

rxchaindesc             struc
rc_data_len             dw      0               ; bytes of data
rc_data_buff            dd      0               ; data buffer (virtual or physical)
rxchaindesc             ends

RCSIZE                  equ     SIZE rxchaindesc

;
; Receive buffer descriptor
;

rxbufdesc               struc
rx_data_count           dw      0               ; no. of recevie chain count
rx_chain                db      (1)*(size rxchaindesc) dup (0)  ; first chain desc block
rxbufdesc               ends

RXBDSIZE                equ     SIZE rxbufdesc - 2 + MAX_CHAIN * RCSIZE


;
; Protocol Manager request block
;

ReqBlock                struc
rb_opcode               dw      0               ; request opcode
rb_status               dw      0               ; status at completion of request
rb_pointer1             dd      0               ; far pointer
rb_pointer2             dd      0               ; far pointer
rb_word1                dw      0               ; word parameter
ReqBlock                ends

;
; Protocol Manager IOCTL category code and function code
;

IOC_NC                  equ     81h
IONC_PM                 equ     58h

;
; Protocol Manager request opcodes
;

GET_PROTMAN_INFO        equ     1
REGISTER_MODULE         equ     2
BIND_AND_START          equ     3
GET_PROTMAN_LINKAGE     equ     4
GET_PROTOCOLINI_PATH    equ     5
REGISTER_PROTMAN_INFO   equ     6
INIT_AND_REGISTER       equ     7
UNBIND_AND_STOP         equ     8
BIND_STATUS             equ     9
REGISTER_STATUS         equ     10

;
; The next request code is not in the NDIS spec 1.0.1
; It is defined for 3Com software to enumerate addresses of MAC's
; characteristics tables.
;

ENUM_MAC_INFO           equ     101h

PROTMAN_VERSION_MAJOR   equ     02h
PROTMAN_VERSION_MINOR   equ     00h

;
; Configuration Memory Image
;

driver_config           struc                   ; Driver configuration representation
dr_next                 dd      0               ; next driver's configuration image
dr_prev                 dd      0               ; previous driver's configuration image
dr_logical_name         db      16 dup (0)      ; driver's logical name
dr_keyword              dw      0               ; first keyword entry
driver_config           ends

;
; Keyword Entry
;

keyword_entry           struc
kw_next                 dd      0               ; next keyword entry
kw_prev                 dd      0               ; previous keyword entry
kw_keyword              db      16 dup (0)      ; keyword name
kw_param_num            dw      0               ; number of parameters encoded

; the following fields repeat for the number specified in kw_param_num

kw_param_type           dw      0               ; parameter type
kw_param_len            dw      0               ; length of parameter value
kw_param_value          dd      0               ; parameter value (variable length)
keyword_entry           ends

KW_PARAM_LONG           equ     0
KW_PARAM_STRING         equ     1

;
; Bindlings list
;

mod_name                struc
module_name             db      16 dup (0)
mod_name                ends

bindings_list           struc
num_bindings            dw      0
bound_driver            db      (1)*(size mod_name) dup (0)
bindings_list           ends
