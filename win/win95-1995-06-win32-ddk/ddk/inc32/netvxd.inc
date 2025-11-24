;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

;NOINC
;#ifndef VXDNET_INC
;#define VXDNET_INC  1
;INC

;***   vxdnet.h - NetWork VxD IDs and InitOrder values
;


VNetSup_Device_ID  	equ	00480h
VRedir_Device_ID   	equ	00481h
VBrowse_Device_ID  	equ	00482h
VSHARE_Device_ID   	equ	00483h
SERVER_Device_ID   	equ	0032h
;#define IFSMgr_Device_ID    0x0484
MEMPROBE_Device_ID 	equ	00485h
VFAT_Device_ID     	equ	00486h
NWLINK_Device_ID   	equ	00487h
VNWLINK_Device_ID  	equ	00487h
NWSUP_Device_ID    	equ	00487h
VTDI_Device_ID     	equ	00488h
VIP_Device_ID      	equ	00489h
VTCP_Device_ID     	equ	0048Ah
VCache_Device_ID   	equ	0048Bh
VUDP_Device_ID     	equ	0048Ch
VAsync_Device_ID   	equ	0048Dh
NWREDIR_Device_ID  	equ	0048Eh
;#define STAT80_Device_ID    0x048F  // BUGBUG - This should go away soon
;#define SCSIPORT_Device_ID  0x0490  // BUGBUG - This should go away soon
FILESEC_Device_ID  	equ	00491h
NWSERVER_Device_ID 	equ	00492h
SECPROV_DEVICE_ID  	equ	00493h
NSCL_Device_ID     	equ	00494h  ; Name Client
WSTCP_DEVICE_ID    	equ	00495h  ; TCP/IP WinSock Provider
NDIS2SUP_Device_ID 	equ	00496h
MSODISUP_Device_ID 	equ	00497h
Splitter_Device_ID 	equ	00498h
PPP_Device_ID      	equ	00499h
VDHCP_Device_ID    	equ	0049Ah
VNBT_Device_ID     	equ	0049Bh
LOGGER_DEVICE_ID   	equ	0049Dh
EFILTER_Device_ID  	equ	0049Eh
FFILTER_Device_ID  	equ	0049Fh
TFILTER_Device_ID  	equ	004A0h
AFILTER_Device_ID  	equ	004A1h
IRLAMP_Device_ID   	equ	004A2h



MAC_Device_ID      	equ	00000h  ; Supposed to be Undefined_Device_Type
                                    ; from VMM.INC.
NDIS_DEVICE_ID     	equ	000028h ; NDIS wrapper - from VMM.H
VNB_Device_ID      	equ	000031h
WSOCK_DEVICE_ID    	equ	00003Eh ; WinSockets
WSIPX_DEVICE_ID    	equ	00003Fh ; WinSockets for IPX



;
;      The #ifdef below is so that 16-bit ASM code which doesn't
;      include vmm.inc can still include this file to get at the
;      device IDs.
;

ifdef VMM_Init_Order

MEMPROBE_Init_Order	equ	01000h + VMM_Init_Order
VShare_Init_Order  	equ	008000h + DOSMGR_Init_Order
;#define IFSMgr_Init_Order   0x10000 + V86MMGR_Init_Order
VNetSup_Init_Order 	equ	018000h + V86MMGR_Init_Order
VRedir_Init_Order  	equ	020000h + V86MMGR_Init_Order
SECPROV_Init_Order 	equ	02FF00h + V86MMGR_Init_Order
VServer_Init_Order 	equ	030000h + V86MMGR_Init_Order

VCache_Init_Order  	equ	00100h + VMM_Init_Order

NDIS_Init_Order    	equ	002000h + VNETBIOS_Init_Order
MAC_Init_Order     	equ	002000h + NDIS_Init_Order
GATEWAY_Init_Order 	equ	001000h + MAC_Init_Order
NETBEUI_Init_Order 	equ	002000h + MAC_Init_Order
PROTOCOL_Init_Order	equ	002000h + MAC_Init_Order
VTDI_Init_Order    	equ	002000h + MAC_Init_Order
VIP_Init_Order     	equ	002000h + VTDI_Init_Order
VTCP_Init_Order    	equ	002000h + VIP_Init_Order
VUDP_Init_Order    	equ	002000h + VIP_Init_Order
WSTCP_Init_Order   	equ	002000h + VTCP_Init_Order
VDHCP_Init_Order   	equ	001500h + VTCP_Init_Order
VNBT_Init_Order    	equ	002000h + VUDP_Init_Order

;#define FSD_Init_Order      0x00001 + IFSMgr_Init_Order

FILESEC_Init_Order 	equ	000100h + VServer_Init_Order

endif

;NOINC
;#endif  ; ifndef VXDNET_INC
;INC
