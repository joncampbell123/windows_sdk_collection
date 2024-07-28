PAGE 58,132
;******************************************************************************
TITLE i13trans.asm -
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1991
;
;   Title:	i13trans.asm -
;
;   Version:	1.00
;
;   Date:	13-Aug-1991
;
;   Author:	RAL
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   13-Aug-1991 RAL
;
;==============================================================================

	.386p

	INCLUDE VMM.INC

	PUBLIC	INIIndosPoll
	PUBLIC	INIOverlappedIO

VxD_IDATA_SEG

BeginDoc
;
;   If InDOSPolling is enabled then we will default to *NO* overlapped I/O
;   unless the user explicitly enables overalpped I/O.
;
INIIndosPoll	db	"INDOSPOLLING",0

;
;   The OverlappedIO switch can be used to enable/disable blocking on disk
;   I/O.  The default value is ON unless InDOSPolling is enabled in which
;   case the default is OFF.  This switch setting will override the default
;   in all cases, so it is possible to have overlapped I/O when InDOSPolling
;   is enabled.
;
INIOverlappedIO db	"OverlappedIO", 0
EndDoc

VxD_IDATA_ENDS

	END
