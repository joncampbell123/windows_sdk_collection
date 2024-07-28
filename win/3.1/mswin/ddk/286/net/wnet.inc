;===============================================================================
;		File	WNET.INC
;===============================================================================

		IDD_RMLOGO		equ	30

		MAJOR_SPEC_VERSION	equ	0
		MINOR_SPEC_VERSION	equ	58
		SPEC_VERSION		equ	MAJOR_SPEC_VERSION * 100 + MINOR_SPEC_VERSION

		MAJOR_DRIVER_VERSION	equ	0
		MINOR_DRIVER_VERSION	equ	21
		DRIVER_VERSION		equ	MAJOR_DRIVER_VERSION * 100 + MINOR_DRIVER_VERSION

;		General return status codes

		WN_SUCCESS		equ	00h
		WN_NOT_SUPPORTED	equ	01h
		WN_NET_ERROR		equ	02h
		WN_MORE_DATA		equ	03h
		WN_BAD_POINTER		equ	04h
		WN_BAD_VALUE		equ	05h
		WN_BAD_PASSWORD 	equ	06h
		WN_ACCESS_DENIED	equ	07h
		WN_FUNCTION_BUSY	equ	08h
		WN_WINDOWS_ERROR	equ	09h
		WN_BAD_USER		equ	0ah
		WN_OUT_OF_MEMORY	equ	0bh
		WN_CANCEL		equ	0ch

;		Connection return status codes

		WN_NOT_CONNECTED	equ	30h
		WN_OPEN_FILES		equ	31h
		WN_BAD_NETNAME		equ	32h
		WN_BAD_LOCALNAME	equ	33h
		WN_ALREADY_CONNECTED	equ	34h
		WN_DEVICE_ERROR 	equ	35h
		WN_CONNECTION_CLOSED	equ	36h

;		Printing return status codes

		WN_BAD_JOBID		equ	40h
		WN_JOB_NOT_FOUND	equ	41h
		WN_JOB_NOT_HELD 	equ	42h
		WN_BAD_QUEUE		equ	43h
		WN_BAD_FILE_HANDLE	equ	44h
		WN_CANT_SET_COPIES	equ	45h
		WN_ALREADY_LOCKED	equ	46h

;		Queue Changed message

		SP_QUEUECHANGED 	equ	500h

;
;   MSNet will intermix output from multiple windows applications on the
;   server if the Windows spooler is bypassed.	Therefore, only allow a
;   single app to have the port open.  Enforce similar restriction for
;   queue watches.
;
		PORT_OPEN		equ	00000001b
		PORT_WATCHED		equ	00000010b

;		Commonly used cmacros

		RegPtr	RegPairDsSi, ds, si
		RegPtr	RegPairEsDi, es, di
		RegPtr	RegPairEsBx, es, bx
		RegPtr	RegPairDxAx, dx, ax
		RegPtr	RegPairSSBX, ss, bx
		RegPtr	RegPairSSSI, ss, si
		RegPtr	RegPairESDX, es, dx

StandardSave	macro
		push	si
		push	di
		push	ds
		endm

StandardRestore macro
		pop	ds
		pop	di
		pop	si
		endm

WatchEntry	struc
		fWatch	    dw	    ?
		hTask	    dw	    ?
		hf	    dw	    ?
WatchEntry	ends

externFP	dos3call
externFP	netbioscall
