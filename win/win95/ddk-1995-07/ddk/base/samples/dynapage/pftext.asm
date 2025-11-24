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

PAGE 58,132
;******************************************************************************
TITLE PFTEXT.ASM - Text entries for page file device
;******************************************************************************
;
;   Title:	PFTEXT.ASM - Text entries for page file device
;
;==============================================================================


	.386p

	PUBLIC PF_Enable_Ini
	PUBLIC PF_Swap_File_Ini
	PUBLIC PF_Swap_Drive_Ini
	PUBLIC PF_Max_Size_Ini
	PUBLIC PF_Min_Size_Ini
	PUBLIC PF_Min_Free_Ini


	INCLUDE VMM.Inc

BeginDoc
;******************************************************************************
;
;   The PageFile VxD has one system initialization file entry which is
;   associated with paging in general regardless of type (permanent or temp):
;
;	Paging= On or Off to enable or disable demand paging
;		Default value: ON
;
;   The PageFile VxD has four system initialization file entries which are
;   associated with the TEMPORY SWAP FILE configuration:
;
;	PagingFile= Fully qualified pathname including drive and starting
;		    at the root directory to a file to be used for
;		    paging.  This file will be created when Windows boots
;		    and will be deleted when Windows exits.  This entry
;		    overrides the PagingDrive entry.
;	PagingDrive= Drive letter to place paging file on. The paging file
;		     will be placed in the ROOT of this drive.
;		Default= same drive and directory as SYSTEM.INI is in
;		NOTE THAT YOU CANNOT CHANGE THE DIRECTORY, only the drive.
;	MaxPagingFileSize= MAX # of K bytes for paging file
;		Default value: as large as allowed by MinUserDiskSpace
;	MinUserDiskSpace= # of K bytes to leave free at grow time on swap disk
;		Default value: 512 (it was 2000 on 3.1)
;	MinPagingFileSize= size of K bytes initially allocated to paging file
;		Default value: 0.  New for 4.0.
;
;==============================================================================
EndDoc


VxD_IDATA_SEG

PF_Enable_Ini	  db "Paging", 0
PF_Swap_File_Ini  db "PagingFile", 0
PF_Swap_Drive_Ini db "PagingDrive", 0
PF_Max_Size_Ini   db "MaxPagingFileSize", 0
PF_Min_Free_Ini   db "MinUserDiskSpace", 0
PF_Min_Size_Ini   db "MinPagingFileSize", 0


VxD_IDATA_ENDS


	END
