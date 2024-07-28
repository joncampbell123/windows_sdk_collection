PAGE	58,132
;******************************************************************************
TITLE	DNTRANS - Messages for DOSNET device
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp. 1986-1990
;
;   Module:   DNTRANS - Messages for DOSNET device
;
;   Version:  1.00
;
;   Date:     Jun 9, 1989
;
;   Author:   ARR
;
;******************************************************************************
;
;   CHANGE LOG:
;     DATE     VERSION	   Description
;   --------  --------	  --------------------------------------------------
;   06/09/89  1.00	  Original
;
;******************************************************************************

	.386p

	INCLUDE VMM.Inc
	INCLUDE Debug.Inc

VxD_DATA_SEG

	public	DNNetUseGlobalMsg
	public	DNNetUseGlobalMsgDrive
	public	DNNetUseInherMsg
	public	DNNetUseInherMsgDrive
	public	DNNetUseParentMsg
	public	DNNetUseParentMsgDrive
	public	DNDontDestroyMsg
	public	DNDontDestroyMsgDrive

BeginMsg
;
; This message is displayed when a VM (including SYS VM) attempts to
;   "net use /d" a drive connection that was set up before WIN386 Started
;
DNNetUseGlobalMsg label byte
			db	"You cannot break the network connection to drive "
DNNetUseGlobalMsgDrive	db	"A: because this connection was set up before"
			db	" Windows/386 was started. You will have to exit"
			db	" Windows/386 to break this connection.",0
;
; This message is displayed when a VM (NOT the SYS VM) attempts to
;   "net use /d" a drive connection that it inherited from the SYS VM.
;
DNNetUseInherMsg label byte
			db	"You cannot break the network connection to drive "
DNNetUseInherMsgDrive	db	"A: because this connection was obtained from Windows"
			db	" when you started this application. You can only"
			db	" break this connection in Windows provided all"
			db	" applications which obtained it from Windows"
			db	" have been closed.",0
;
; This message is displayed when the SYS VM attempts to "net use /d" a
;   drive connection that it passed on to another VM which is still running.
;
DNNetUseParentMsg label byte
			db	"You cannot break the network connection to drive "
DNNetUseParentMsgDrive	db	"A: because this connection was passed on to"
			db	" some standard applications which are still"
			db	" running. To break this connection, all"
			db	" standard applications which obtained it"
			db	" must be closed first.",0
;
; This message is displayed when an attempt is made to Terminate or Close
;    a VM which has outstanding LOCAL net connections
;
DNDontDestroyMsg label byte
			db	"Terminating this application is not advised"
			db	" because is has a network connection to drive "
DNDontDestroyMsgDrive	db	"A: which should be broken first.",0

EndMsg

VxD_DATA_ENDS

VxD_IDATA_SEG

VxD_IDATA_ENDS

	END
