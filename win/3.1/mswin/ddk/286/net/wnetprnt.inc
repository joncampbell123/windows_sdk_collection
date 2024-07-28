;===============================================================================
;		File	WNETPRNT.INC
;===============================================================================

MSNET_MAX_PRINT_JOBS			equ	64

;		Queue Structure

QUEUESTRUCT	struc
		pqName			dw	?
		pqComment		dw	?
		pqStatus		dw	?
		pqJobCount		dw	?
		pqPrinters		dw	?
QUEUESTRUCT	ends

;		Values for QUESTRUCT.pqStatus

		WNPRQ_ACTIVE		equ	00h
		WNPRQ_PAUSE		equ	01h
		WNPRQ_ERROR		equ	02h
		WNPRQ_PENDING		equ	03h
		WNPRQ_PROBLEM		equ	04h

;		Job Structure

JOBSTRUCT	struc
		pjID			dw	?
		pjUserName		dw	?
		pjParameters		dw	?
		pjPosition		dw	?
		pjStatus		dw	?
		pjSubmitted		dd	?
		pjSize			dd	?
		pjCopies		dw	?
		pjComment		dw	?
JOBSTRUCT	ends

;		Values for JOBSTRUCT.pjStatus

		WNPRJ_QSTATUS		equ	0000000000000111b

		WNPRJ_QS_QUEUED 	equ	0000000000000000b
		WNPRJ_QS_PASUED 	equ	0000000000000001b
		WNPRJ_QS_SPOOLING	equ	0000000000000010b
		WNPRJ_QS_PRINTING	equ	0000000000000011b

		WNPRJ_DEVSTATUS 	equ	0001111111111000b

		WNPRJ_DS_COMPLETE	equ	0000000000001000b
		WNPRJ_DS_INTERV 	equ	0000000000010000b
		WNPRJ_DS_ERROR		equ	0000000000100000b
		WNPRJ_DS_DESTOFFLINE	equ	0000000001000000b
		WNPRJ_DS_DESTPAUSED	equ	0000000010000000b
		WNPRJ_DS_NOTIFY 	equ	0000000100000000b
		WNPRJ_DS_DESTNOPAPER	equ	0000001000000000b
		WNPRJ_DS_DESTFORMCHANGE equ	0000010000000000b
		WNPRJ_DS_DESTCRTCHANGE	equ	0000100000000000b
		WNPRJ_DS_DESTPENCHANGE	equ	0001000000000000b

;		Details required by WNetWatchQueue

		MAX_WATCH_QUEUE 	equ	10h		; Maximum 16 queues
		WATCH_QUEUE_TIMER	equ	30 * 1000	; Thirty seconds

		SP_QUEUECHANGED 	equ	500h

WATCHQUEUE	struc
		wqWindowHandle		dw	?
		wqCheckSum		dw	?
WATCHQUEUE	ends
