;===============================================================================
;		File	NETBIOS.INC
;===============================================================================

NETBIOS 				equ	5ch

NetBiosReset				equ	32h	; General commands
NetBiosCancel				equ	35h
NetBiosAdaptorStatus			equ	33h
NetBiosUnlink				equ	70h

NetBiosAddUniqueName			equ	30h	; Name service commands
NetBiosAddGroupName			equ	36h
NetBiosDeleteName			equ	31h

NetBiosSendSpecificDatagram		equ	20h	; Datagram commands
NetBiosReceiveSpecificDatagram		equ	21h
NetBiosSendBroadcastDatagram		equ	22h
NetBiosReceiveBroadcastDatagram 	equ	23h

NetBiosCallName 			equ	10h	; Session commands
NetBiosListen				equ	11h
NetBiosHangup				equ	12h
NetBiosSend				equ	14h
NetBiosChainSend			equ	17h
NetBiosReceive				equ	15h
NetBiosReceiveAny			equ	16h
NetBiosSessionStatus			equ	34h

NETBIOS_ASYNC				equ	80h

NCB		struc
		ncbCommand		db	?
		ncbReturnCode		db	?
		ncbLocalSession 	db	?
		ncbNumber		db	?
		ncbBuffer		dd	?
		ncbLength		dw	?
		ncbCallName		db	16 dup ( ? )
		ncbName 		db	16 dup ( ? )
		ncbReceiveTimeout	db	?
		ncbSendTimeout		db	?
		ncbPostAddress		dd	?
		ncbNetworkNumber	db	?
		ncbCommandComplete	db	?
		ncbReserved		db	14 dup ( ? )
NCB		ends
