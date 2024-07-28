PAGE 58,132
;******************************************************************************
TITLE PFTEXT.ASM - Text entries for page file device
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1989, 1990
;
;   Title:	PFTEXT.ASM - Text entries for page file device
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
;    4-Oct-1990 JEM Now belongs to PageFile device, not PageSwap
;
;==============================================================================


	.386p

	PUBLIC PF_Enable_Ini
	PUBLIC PF_Swap_File_Ini
	PUBLIC PF_Swap_Drive_Ini
	PUBLIC PF_Min_Free_Ini
	PUBLIC PF_Max_Size_Ini

	PUBLIC PF_Invalid_Part_Msg
	PUBLIC PF_Caption_Title_Msg

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
;	MinUserDiskSpace= # of K bytes to reserve for user files on swap disk
;		Default value: 2000
;	MaxPagingFileSize= MAX # of K bytes for paging file
;		Default value: as large as allowed by MinUserDiskSpace up to
;			       65536 (64Meg)
;   The PageFile VxD has two system initialization file entries which are
;   associated with the PERMANENT SWAP FILE configuration:
;
;	PermSwapDOSDrive=  This one is not used by PageFile. It is used by
;			   the real mode WIN386 loader and the SWAPFILE utility
;			   to record the DOS drive to create the swap file on.
;			   It is a string of length 1.
;	PermSwapSizeK= This one is not used by PageFile. It is used by
;		       the real mode WIN386 loader and the SWAPFILE utility
;		       to record the desired size of the permanent swap file.
;		       This is a decimal int and gives the desired size in
;		       KiloBytes.
;
;==============================================================================
EndDoc


VxD_IDATA_SEG

PF_Enable_Ini	  db "Paging", 0
PF_Swap_File_Ini  db "PagingFile", 0
PF_Swap_Drive_Ini db "PagingDrive", 0
PF_Min_Free_Ini   db "MinUserDiskSpace", 0
PF_Max_Size_Ini   db "MaxPagingFileSize", 0


PF_Invalid_Part_Msg LABEL BYTE
db "The permanent swap file is corrupt.",13,10,13,10
db "You need to create a new swap file. Choose the 386 Enhanced icon "
db "in Control Panel, and then choose the Virtual Memory button. "
db "for Help with Virtual Memory settings, press F1 while using "
db "the dialog box.",13,10,13,10
db "Do you want to delete the corrupt swap file?",13,10, 0

PF_Caption_Title_Msg db "Corrupt Swap-File Warning", 0


VxD_IDATA_ENDS


	END
