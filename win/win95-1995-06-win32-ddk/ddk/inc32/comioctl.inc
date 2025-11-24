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

COM_IOCTL_MSG_QUERY_CALLBACK	EQU	0
COM_IOCTL_MSG_SET_CALLBACK	EQU	1
COM_IOCTL_MSG_QUERY_REGS	EQU	2
COM_IOCTL_MSG_SET_REGS	EQU	3
COM_IOCTL_MSG_HW_QUERY	EQU	4
COM_IOCTL_MSG_HW_SET	EQU	5
COM_IOCTL_MSG_X_QUERY	EQU	6
COM_IOCTL_MSG_X_SET	EQU	7
COM_IOCTL_MSG_X_ON	EQU	8
COM_IOCTL_MSG_X_OFF	EQU	9
COM_IOCTL_PRIVATE	EQU	8000H

tagCOM_IOCTL_CALLBACK	STRUC
wMsg	DW	?
dwNum	DD	?
dwAlt	DD	?
tagCOM_IOCTL_CALLBACK	ENDS
COM_IOCTL_CALLBACK STRUC  
	DB	SIZE tagCOM_IOCTL_CALLBACK DUP (?)
COM_IOCTL_CALLBACK ENDS

tagCOM_IOCTL_REGS	STRUC
LineCtrl	DB	?
MdmCtrl	DB	?
LineStat	DB	?
MdmStat	DB	?
LineMask	DB	?
MdmMask	DB	?
tagCOM_IOCTL_REGS	ENDS
COM_IOCTL_REGS STRUC  
	DB	SIZE tagCOM_IOCTL_REGS DUP (?)
COM_IOCTL_REGS ENDS

tagCOM_IOCTL_X	STRUC
XonLim	DD	?
XoffLim	DD	?
XonChr	DB	?
XoffChr	DB	?
tagCOM_IOCTL_X_fs	DB	?
tagCOM_IOCTL_X	ENDS
fOutX	EQU	01h
fOutX_SHIFT	EQU	00h
fInX	EQU	02h
fInX_SHIFT	EQU	01h
fXoffHold	EQU	04h
fXoffHold_SHIFT	EQU	02h
fXoffSent	EQU	08h
fXoffSent_SHIFT	EQU	03h
COM_IOCTL_X STRUC  
	DB	SIZE tagCOM_IOCTL_X DUP (?)
COM_IOCTL_X ENDS

tagCOM_IOCTL_HW	STRUC
onLim	DD	?
offLim	DD	?
tagCOM_IOCTL_HW_fs	DW	?
tagCOM_IOCTL_HW	ENDS
fRtsOn	EQU	01h
fRtsOn_SHIFT	EQU	00h
fDtrOn	EQU	02h
fDtrOn_SHIFT	EQU	01h
fRtsRx	EQU	04h
fRtsRx_SHIFT	EQU	02h
fDtrRx	EQU	08h
fDtrRx_SHIFT	EQU	03h
fRlsdTx	EQU	010h
fRlsdTx_SHIFT	EQU	04h
fRiTx	EQU	020h
fRiTx_SHIFT	EQU	05h
fDsrTx	EQU	040h
fDsrTx_SHIFT	EQU	06h
fCtsTx	EQU	080h
fCtsTx_SHIFT	EQU	07h
fRlsdHold	EQU	0100h
fRlsdHold_SHIFT	EQU	08h
fRiHold	EQU	0200h
fRiHold_SHIFT	EQU	09h
fDsrHold	EQU	0400h
fDsrHold_SHIFT	EQU	0ah
fCtsHold	EQU	0800h
fCtsHold_SHIFT	EQU	0bh
COM_IOCTL_HW STRUC  
	DB	SIZE tagCOM_IOCTL_HW DUP (?)
COM_IOCTL_HW ENDS
