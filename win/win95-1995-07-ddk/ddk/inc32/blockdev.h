/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

/*******************************************************************************

     Title:	 extbcb.h -- Services and equates for BlockDev VxD

     Version:	 3.10

  ==============================================================================
*/
//
//   Service Declarations
//

/* ASM
Begin_Service_Table BlockDev

BlockDev_Service    BlockDev_Get_Version, LOCAL
BlockDev_Service    BlockDev_Register_Device, LOCAL
BlockDev_Service    BlockDev_Find_Int13_Drive, LOCAL
BlockDev_Service    BlockDev_Get_Device_List, LOCAL
BlockDev_Service    BlockDev_Send_Command, LOCAL
BlockDev_Service    BlockDev_Command_Complete, LOCAL
BlockDev_Service    BlockDev_Synchronous_Command, LOCAL

End_Service_Table   BlockDev
*/


//
//   Block Device Desctiptor data structrue
//



#define BD_Priv_Data_Size		0x30

#define BD_Major_Ver		3
#define BD_Minor_Ver		0x0A


typedef struct BlockDev_Device_Descriptor{ /* */
	ULONG BDD_Next;
	BYTE	BDD_Major_Version; // INIT <BD_Major_Ver>
	BYTE	BDD_Minor_Version; // INIT <BD_Minor_Ver>
	BYTE  BDD_Device_Type;
	BYTE  BDD_Int_13h_Number;
	ULONG BDD_Flags;
	ULONG BDD_Name_Ptr;
	ULONG BDD_Max_Sector[2];
	ULONG BDD_Sector_Size;
	ULONG BDD_Num_Heads;
	ULONG BDD_Num_Cylinders;
	ULONG BDD_Num_Sec_Per_Track;
	ULONG BDD_Sync_Cmd_Proc;
	ULONG BDD_Command_Proc;
	ULONG BDD_Hw_Int_Proc; // INIT <0>
	BYTE	BDD_Reserved_BlockDev[BD_Priv_Data_Size ];
}  _BlockDev_Device_Descriptor;


//
//   Flags for BDD_Flags field -- BUGBUG how can we use bit fields!
//
#define BDF_Int13_Drive 		0x01
#define BDF_Int13_Drive_Bit	0

#define BDF_Writeable			0x02
#define BDF_Writeable_Bit		1

#define BDF_Removable			0x04
#define BDF_Removable_Bit		2

#define BDF_Remote				0x08
#define BDF_Remote_Bit			3

#define BDF_Serial_Cmd			0x10
#define BDF_Serial_Cmd_Bit	4

#define BDF_Cache					0x20
#define BDF_Cache_Bit			5

//
// New flags from IOR follow.  Note that these are not in win 3.1
//

#define BDF_Bypass_Queue		0x800
#define BDF_Bypass_Queue_Bit	11

#define BDF_VERSION_002 	  0x0400
#define BDF_VERSION_002_BIT	10

//
//   Device types for BDD_Device_Type field
//
#define BDT_360K_5_Inch_Floppy		0x00
#define BDT_1200K_5_Inch_Floppy 	0x01
#define BDT_720K_3_Inch_Floppy		0x02
#define BDT_Single_Dens_8_Inch		0x03
#define BDT_Double_Dens_8_Inch		0x04
#define BDT_Fixed_Disk			0x05
#define BDT_Tape_Drive			0x06
#define BDT_Other			0x07


typedef struct BlockDev_Command_Block{
	ULONG BD_CB_Next;
	USHORT	 BD_CB_Command;
	USHORT	 BD_CB_Cmd_Status;
	ULONG BD_CB_Flags;
	ULONG BD_CB_Cmd_Cplt_Proc;
	ULONG	BD_CB_Sector[2];
	ULONG BD_CB_Count;
	ULONG BD_CB_Buffer_Ptr;
	ULONG BD_CB_Reserved_Client;
	ULONG BD_CB_Reserved_BlockDev;
	ULONG BD_CB_Reserved_FastDisk;
	ULONG	BD_CB_Spare[5 ];

   ULONG    BD_CB_Req_Req_Handle;/* requestor provided request	     */
				 /* handle. often is a pointer to    */
				 /* to this ior or its containing    */
				 /* iop.			     */

	ULONG	 BD_CB_Req_Vol_Handle;/* requestor provided media handle  */
				 /* designating the media to perform */
				 /* the function on.		     */

	ULONG	 BD_CD_SGD_Lin_Phys;  /* pointer to first sgd. note that  */
				 /* this is either a linear or	     */
				 /* address, depending on the needs  */
				 /* of the drivers, as indicated     */
				 /* via the demand bits.	     */

	UCHAR	 BD_CB_Num_SGDs;      /* number of sgds to process */
	UCHAR	 BD_CB_Vol_Designtr;  /* numeric representation of the	  */
				 /* drive letter designating the     */
				 /* volume to perform the function   */
				 /* on. 			     */
	USHORT	 BD_CB_reserved_1;    /* reserved to force alignment - must be zero */

}  _BlockDev_Command_Block;

typedef struct BlockDev_Scatter_Gather{
	ULONG BD_SG_Count;
	ULONG BD_SG_Buffer_Ptr;
}  _BlockDev_Scatter_Gather	;


//
//   Commands
//   NOTE:  Commands 0x8000h-FFFFh are reserved for device specific command
//
#define BDC_Read 0
#define  BDC_Write 1
#define  BDC_Verify 2
#define  BDC_Cancel 3

//
// VERSION 2 Commands follow
//

#define  BDC_Lock_Volume	0x09
#define  BDC_Unlock_Volume	0x0a


//
//   Equates for command status. All codes below 10h imply successful completion
//   Error code value are > 10h
//
#define  BDS_Success 0		// Completed successfully
#define  BDS_Success_With_Retries	1		// Completed successfully after retries
#define  BDS_Success_With_ECC		2		// Successful after error correction

#define  BDS_First_Error_Code		0x10		// first error code value
#define  BDS_Invalid_Sector_Number	0x10		// Invalid sector number
#define  BDS_Canceled			0x11		// Command was canceled
#define  BDS_Cmd_In_Progress		0x12		// Can't cancel cmd in progress
#define  BDS_Invalid_Cmd_Ptr		0x13		// Cancel of invalid cmd pointer
#define  BDS_Media_Error			0x14		// Read/Write failed
#define  BDS_Device_Error		0x15		// Device/Adapter failed
#define  BDS_Invalid_Command		0x16
#define 	BDS_No_Device		0x17  /* physical or logical device nonexistant */
#define 	BDS_No_Media		0x18  /* media removed from device */
#define 	BDS_Uncertain_Media	0x19  /* media may have changed */
#define 	BDS_Unrec_Error 	0x1a  /* un-recoverable error */
#define 	BDS_Hw_Failure		0x1b  /* general hardware failure */
#define 	BDS_Unformatted_Media	0x1c  /* unformatted media */
#define 	BDS_Memory_Error	0x1d  /* error allocating dma buffer space for i/o*/
#define 	BDS_Time_Out		0x1e  /* drive time-out */
#define 	BDS_Write_Protect	0x1f  /* write protect error */
#define 	BDS_Not_Ready		0x20  /* device not ready */
#define 	BDS_Busy					   0x21 /* device is busy */



//
//   Flags for commands
//
#define  BDCF_High_Priority		0x01
#define  BDCF_High_Priority_Bit 0

#define  BDCF_Scatter_Gather		0x02
#define  BDCF_Scatter_Gather_Bit 1

#define  BDCF_Dont_Cache			0x04
#define  BDCF_Dont_Cache_Bit		2

#define 	BDCF_Bypass_Voltrk_Bit	3
#define 	BDCF_Bypass_Voltrk		(1 << BDCF_Bypass_Voltrk_Bit)

#define 	BDCF_SWAPPER_IO	0x0020	/* request is from the swapper */

#define  BDCF_Double_Buffer	  0x0040  /* indicates that the request is to be completed */
													 /* via double buffering.  client must */
													 /* ensure that both the sgd buffer */
													 /* pointers and IOR_sgd_lin_phys are */
													 /* virtual pointers. */
#define  BDCF_Double_Buffer_Bit   6

#define	BDCF_Bypass_Queue_Bit	11

#define 	BDCF_Bypass_Queue	  (1 << BDCF_Bypass_Queue_Bit)  
													/* request should bypass IOS queuing */
													/* should be used only dragon drivers */
#define	BDCF_Logical_Start_Sector_Bit	19
#define 	BDCF_Logical_Start_Sector	  (1 << BDCF_Logical_Start_Sector_Bit)  
													/* request contains the logical starting */
													/* sector, rather than the physical  */

//
//   Equates for synchronous commands
//
#define  BD_SC_Get_Version		0x0000


//
//   Equates for error returns from synchronous command
//
#define  BD_SC_Err_Invalid_Cmd		0x0001


//
//   Value specified in CX register when API call-out Int 0x2F executed
//   The Hw_Detect_Start and End APIs are used by block devices to notify
//   TSR and DOS device driver software that they are performing hardware
//   detection. This may, for example, disable a write-behind cache.
//
#define  BlockDev_API_Hw_Detect_Start	0x0001
#define  BlockDev_API_Hw_Detect_End		0x0002

