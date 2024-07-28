;===============================================================================
;		File	WNETCONS.INC
;===============================================================================

;		Connection List Structure

CONNLIST	struc
		clLocal 		dw	?	; redirected local device
		clRemote		dw	?	; name of remote resource
		clStatus		dw	?	; status see NGC_
		clType			dw	?	; see STYPE_
CONNLIST	ends

;		Values for CONNLIST.clStatus

		WNGC_OK 		equ	00h	; Ok
		WNGC_PAUSED		equ	01h	; The connection is paused
		WNGC_DISCONNECTED	equ	02h	; The connection is disconnected
		WNGC_ERROR		equ	03h	; The network is down (again!)

;		Values for CONNLIST.clType

		WNGC_DISKTREE		equ	01h
		WNGC_PRINTQ		equ	02h
		WNGC_DEVICE		equ	03h
		WNGC_IPC		equ	04h
