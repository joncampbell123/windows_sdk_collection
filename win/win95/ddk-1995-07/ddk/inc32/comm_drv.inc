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

ifndef Not_VxD

Begin_Service_Table COMMDRVR

COMMDRVR_Service		COMMDRVR_Get_Version,LOCAL

COMMDRVR_Service		__ComApiBreak
COMMDRVR_Service		__ComApiBufOvr
COMMDRVR_Service		__ComApiClear
COMMDRVR_Service		__ComApiClose
COMMDRVR_Service		__ComApiIoctl
COMMDRVR_Service		__ComApiOpen
COMMDRVR_Service		__ComApiPrtclDel
COMMDRVR_Service		__ComApiPrtclIns
COMMDRVR_Service		__ComApiPrtclIoctl
COMMDRVR_Service		__ComApiPurge
COMMDRVR_Service		__ComApiQueryState
COMMDRVR_Service		__ComApiQueryStatus
COMMDRVR_Service		__ComApiRead
COMMDRVR_Service		__ComApiSetState
COMMDRVR_Service		__ComApiWrite

COMMDRVR_Service		__ComAsyncCallBack
COMMDRVR_Service		__ComAsyncGetReadPtr
COMMDRVR_Service		__ComAsyncRead

COMMDRVR_Service		__ComPrtclIoctl
COMMDRVR_Service		__ComPrtclWriteNext

COMMDRVR_Service		__ComRegisterPort
COMMDRVR_Service		__ComRegisterPrtcl

COMMDRVR_Service		__ComApiPort
COMMDRVR_Service		__ComApiQueryInfo

End_Service_Table   COMMDRVR

endif
COMM_Major_Ver	EQU	03H
COMM_Minor_Ver	EQU	0AH
ifndef TRUE
TRUE	EQU	1
endif
iodelay macro
	jmp	$+2
	jmp	$+2
endm

ifdef COM_INTERNALS
COM_CHNL_HDL STRUC  
	DD	?
COM_CHNL_HDL ENDS
COM_PRTCL_INST STRUC  
	DD	?
COM_PRTCL_INST ENDS
else
COM_CHNL_HDL STRUC  
	DD	?
COM_CHNL_HDL ENDS
COM_PRTCL_INST STRUC  
	DD	?
COM_PRTCL_INST ENDS
endif
COM_ERROR STRUC  
	DW	?
COM_ERROR ENDS
COM_FIND_HDL STRUC  
	DD	?
COM_FIND_HDL ENDS
COM_MODEM_HDL STRUC  
	DD	?
COM_MODEM_HDL ENDS
COM_MSG STRUC  
	DD	?
COM_MSG ENDS
COM_PORT_HDL STRUC  
	DD	?
COM_PORT_HDL ENDS
COM_PRTCL_HDL STRUC  
	DD	?
COM_PRTCL_HDL ENDS
COM_SIZE STRUC  
	DD	?
COM_SIZE ENDS
ifdef IS_16
COM_NAME STRUC  
	DD	?
COM_NAME ENDS
else
COM_NAME STRUC  
	DD	?
COM_NAME ENDS
endif
COM_ERR_OK	EQU	0
COM_ERR_TX_FULL	EQU	1
COM_ERR_NOT_MINE	EQU	2
COM_ERR_BREAK	EQU	10
COM_ERR_NO_MEM	EQU	11
COM_ERR_NOT_FOUND	EQU	12
COM_ERR_IN_USE	EQU	13
COM_ERR_UNKNOWN	EQU	14
COM_ERR_BAD_CMD	EQU	15
COM_ERR_BAD_LEN	EQU	16
COM_ERR_BAD_RATE	EQU	17
COM_ERR_BAD_SIZE	EQU	18
COM_ERR_BAD_PARITY	EQU	19
COM_ERR_BAD_PARAM	EQU	20
COM_ERR_RX_FULL	EQU	21
COM_ERR_RX_OVERRUN	EQU	22
COM_ERR_RX_PARITY	EQU	23
COM_ERR_RX_FRAME	EQU	24
COM_ERR_RX_BREAK	EQU	25
COM_READ	EQU	01H
COM_WRITE	EQU	02H
COM_ON_MSG	EQU	01H
COM_ON_RX	EQU	02H
COM_ON_TX	EQU	04H
COM_OFF_MSG	EQU	10H
COM_OFF_RX	EQU	20H
COM_OFF_TX	EQU	40H
COM_MSG_LINE_CHANGE	EQU	0001H
COM_MSG_MODEM_CHANGE	EQU	0002H
COM_MSG_PAUSED	EQU	0003H
COM_MSG_READ_ASYNC	EQU	0004H
COM_MSG_READ_DATA	EQU	0005H
COM_MSG_READ_HIGH	EQU	0006H
COM_MSG_READ_LOW	EQU	0007H
COM_MSG_READ_OVERRUN	EQU	0008H
COM_MSG_READ_TIMEOUT	EQU	0009H
COM_MSG_READ_TIMEOUT_1ST	EQU	000AH
COM_MSG_RESUMED	EQU	000BH
COM_MSG_WRITE_HIGH	EQU	000CH
COM_MSG_WRITE_LOW	EQU	000DH
COM_MSG_WRITE_OVERRUN	EQU	000EH
COM_MSG_WRITE_TIMEOUT	EQU	000FH
COM_MSG_WRITE_TIMEOUT_1ST	EQU	0010H

tagCOM_DCB	STRUC
wLen	DW	?
wVer	DW	?
baudPort	DD	?
baudRaw	DD	?
baudComp	DD	?
DupSwitch	DD	?
ReadBufLen	DD	?
WriteBufLen	DD	?
TxDelay	DW	?
ByteSize	DB	?
Parity	DB	?
StopBits	DB	?
tagCOM_DCB_fs	DB	?
tagCOM_DCB	ENDS
fParity	EQU	01h
fParity_SHIFT	EQU	00h
fError	EQU	02h
fError_SHIFT	EQU	01h
fCompress	EQU	04h
fCompress_SHIFT	EQU	02h
COM_DCB STRUC  
	DB	SIZE tagCOM_DCB DUP (?)
COM_DCB ENDS

tagCOM_INFO	STRUC
wLen	DW	?
wVer	DW	?
maxBaudPort	DD	?
maxBaudRaw	DD	?
maxBaudComp	DD	?
DupSwitch	DD	?
wNumPorts	DW	?
tagCOM_INFO_fs	DB	?
tagCOM_INFO	ENDS
fOpen	EQU	01h
fOpen_SHIFT	EQU	00h
COM_INFO STRUC  
	DB	SIZE tagCOM_INFO DUP (?)
COM_INFO ENDS

tagCOM_STAT	STRUC
wLen	DW	?
wVer	DW	?
wInQue	DW	?
wOutQue	DW	?
wInBuf	DW	?
wOutBuf	DW	?
tagCOM_STAT_fs	DB	?
tagCOM_STAT	ENDS
fCd	EQU	01h
fCd_SHIFT	EQU	00h
fRlsd	EQU	02h
fRlsd_SHIFT	EQU	01h
fDsr	EQU	04h
fDsr_SHIFT	EQU	02h
fCts	EQU	08h
fCts_SHIFT	EQU	03h
fTxNext	EQU	010h
fTxNext_SHIFT	EQU	04h
COM_STAT STRUC  
	DB	SIZE tagCOM_STAT DUP (?)
COM_STAT ENDS

tagCOM_FAX	STRUC
iDummy	DD	?
tagCOM_FAX	ENDS
COM_FAX STRUC  
	DB	SIZE tagCOM_FAX DUP (?)
COM_FAX ENDS
ifdef NEVER

tagCOM_IOCTL	STRUC
wLen	DW	?
wVer	DW	?
wCmd	DW	?
wErr	DW	?
ifdef IS_16
pvDataWrite	DD	?
pvDataRead	DD	?
else
pvDataWrite	DD	?
pvDataRead	DD	?
endif
uWriteLen	DW	?
uReadLen	DW	?
tagCOM_IOCTL	ENDS
COM_IOCTL STRUC  
	DB	SIZE tagCOM_IOCTL DUP (?)
COM_IOCTL ENDS
endif

tagCOM_PORT_FUNC	STRUC
fnBreak	DD	?
fnClear	DD	?
fnClose	DD	?
fnGetReadPtr	DD	?
fnGetWritePtr	DD	?
fnIoctl	DD	?
fnOnOff	DD	?
fnOpen	DD	?
fnPurge	DD	?
fnQueryInfo	DD	?
fnQueryState	DD	?
fnQueryStatus	DD	?
fnSetPtr	DD	?
fnSetState	DD	?
fnWrite	DD	?
fnWriteNext	DD	?
tagCOM_PORT_FUNC	ENDS
COM_PORT_FUNC STRUC  
	DB	SIZE tagCOM_PORT_FUNC DUP (?)
COM_PORT_FUNC ENDS

tagCOM_PORT_REG	STRUC
wLen	DW	?
wVer	DW	?
psName	DD	?
hPort	DD	?
Funcs	DB	SIZE COM_PORT_FUNC DUP (?)
tagCOM_PORT_REG	ENDS
COM_PORT_REG STRUC  
	DB	SIZE tagCOM_PORT_REG DUP (?)
COM_PORT_REG ENDS

tagCOM_MODEM_FUNC	STRUC
wLen	DW	?
wVer	DW	?
fnAnswer	DD	?
fnAutoAnswer	DD	?
fnDial	DD	?
fnHangUp	DD	?
tagCOM_MODEM_FUNC	ENDS
COM_MODEM_FUNC STRUC  
	DB	SIZE tagCOM_MODEM_FUNC DUP (?)
COM_MODEM_FUNC ENDS

tagCOM_PRTCL_PROP	STRUC
wLen	DW	?
wVer	DW	?
psName	DD	?
uBufMinLen	DD	?
tagCOM_PRTCL_PROP	ENDS
COM_PRTCL_PROP STRUC  
	DB	SIZE tagCOM_PRTCL_PROP DUP (?)
COM_PRTCL_PROP ENDS

tagCOM_PRTCL_FUNC	STRUC
fnBreak	DD	?
fnCallBack	DD	?
fnClear	DD	?
fnCtor	DD	?
fnDtor	DD	?
fnIoctl	DD	?
fnPurge	DD	?
fnQueryState	DD	?
fnQueryStatus	DD	?
fnRead	DD	?
fnSetPtr	DD	?
fnSetState	DD	?
fnWrite	DD	?
fnWriteNext	DD	?
tagCOM_PRTCL_FUNC	ENDS
COM_PRTCL_FUNC STRUC  
	DB	SIZE tagCOM_PRTCL_FUNC DUP (?)
COM_PRTCL_FUNC ENDS

tagCOM_PRTCL_REG	STRUC
wLen	DW	?
wVer	DW	?
wInQueMin	DW	?
wOutQueMin	DW	?
wInRes	DW	?
wOutRes	DW	?
psName	DD	?
Funcs	DB	SIZE COM_PRTCL_FUNC DUP (?)
tagCOM_PRTCL_REG	ENDS
COM_PRTCL_REG STRUC  
	DB	SIZE tagCOM_PRTCL_REG DUP (?)
COM_PRTCL_REG ENDS
