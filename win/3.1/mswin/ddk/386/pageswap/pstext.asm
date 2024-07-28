PAGE 58,132
;******************************************************************************
TITLE PSTEXT.ASM - Text entries for page swap device
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1989, 1990
;
;   Title:	PSTEXT.ASM - Text entries for page swap device
;
;   Version:	1.01
;
;   Date:	07-Feb-1989
;
;   Author:	RAL
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   07-Feb-1989 RAL Contains system initialization file entries
;
;==============================================================================


	.386p

	PUBLIC PS_Page_Buffers_Ini

	INCLUDE VMM.Inc

BeginDoc
;******************************************************************************
;
;   The PageSwap VxD has one system initialization file entries.
;
;	PageBuffers= # 4K page buffers to hold asynchronous read/write pages.
;		     Default value is 4, min is 0, max is 32.
;
;==============================================================================
EndDoc


VxD_IDATA_SEG

PS_Page_Buffers_Ini	db	"PageBuffers", 0

VxD_IDATA_ENDS


	END
