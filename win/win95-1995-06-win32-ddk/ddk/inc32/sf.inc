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

SF	STRUC
SFLink	DD	?
SFCount	DW	?
SFTable	DW	?
SF	ENDS

sf_entry	STRUC
sf_ref_count	DW	?
sf_mode	DW	?
sf_attr	DB	?
sf_flags	DW	?
sf_devptr	DD	?
sf_firclus	DW	?
sf_time	DW	?
sf_date	DW	?
sf_size	DD	?
sf_position	DD	?
sf_cluspos	DW	?
sf_dirsec	DD	?
sf_dirpos	DB	?
sf_name	DB	11 DUP (?)
sf_chain	DD	?
sf_UID	DW	?
sf_PID	DW	?
sf_MFT	DW	?
sf_lstclus	DW	?
sf_IFS_HDR	DD	?
sf_entry	ENDS
sf_fsda 	EQU	BYTE PTR sf_cluspos
sf_serial_ID	EQU	WORD PTR sf_firclus
sf_netid	EQU	BYTE PTR sf_cluspos
sf_OpenAge	EQU	WORD PTR sf_position+2
sf_LRU		EQU	WORD PTR sf_position

SF_DEFAULT_NUMBER	EQU	5
SF_BUSY	EQU	(-1)
SF_ISFCB	EQU	8000H
SF_NOLASTACCESS	EQU	04H
SF_ISNET	EQU	8000H
SF_CLOSE_NODATE	EQU	4000H
SF_PIPE	EQU	2000H
SF_NO_INHERIT	EQU	1000H
SF_NET_SPOOL	EQU	0800H
DEVID_FILE_CLEAN	EQU	40H
DEVID_FILE_MASK_DRIVE	EQU	3FH
DEVID_DEVICE	EQU	80H
DEVID_DEVICE_EOF	EQU	40H
DEVID_DEVICE_RAW	EQU	20H
DEVID_DEVICE_SPECIAL	EQU	10H
DEVID_DEVICE_CLOCK	EQU	08H
DEVID_DEVICE_NULL	EQU	04H
DEVID_DEVICE_CON_OUT	EQU	02H
DEVID_DEVICE_CON_IN	EQU	01H
DEVID_ISDEV	EQU	80H
DEVID_EOF	EQU	40H
DEVID_RAW	EQU	20H
DEVID_SPECIAL	EQU	10H
DEVID_ISCLK	EQU	08H
DEVID_ISNUL	EQU	04H
DEVID_ISCOT	EQU	02H
DEVID_ISCIN	EQU	01H
DEVID_BLOCK_DEV	EQU	1FH
