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

/****************************************************************************\
*
* IOR (I/O Request Descriptor) Data Structure
*
\****************************************************************************/

/* H2INCSWITCHES -t */

typedef struct type_sdeffsd_req_usage { /* */
   USHORT _IOR_ioctl_drive;
   USHORT _IOR_ioctl_function ;
   ULONG _IOR_ioctl_control_param ;
   ULONG _IOR_ioctl_buffer_ptr;
   ULONG _IOR_ioctl_client_params;
   ULONG _IOR_ioctl_return;
} _type_sdeffsd_req_usage;


typedef union urequestor_usage { /* */
   ULONG  _IOR_requestor_usage[5];

   struct type_sdeffsd_req_usage sdeffsd_req_usage;
} _urequestor_usage;

/*
** bcb to ior equivalence
*/

//	BlockDev_Command_Block	_IOR
//	BD_CB_Next		IOR_ior_next
//	BD_CB_Command		IOR_func
//	BD_CB_Cmd_Status	IOR_status
//	BD_CB_Flags		IOR_flags
//	BD_CB_Cmd_Cplt_Proc	IOR_callback
//	BD_CB_Sector[2] 	IOR_start_addr
//	BD_CB_Count		IOR_xfer_count
//	*BD_CB_Buffer_Ptr	IOR_sgd_lin_phys
//	BD_CB_Reserved_Client	IOR_requestor_usage
//	BD_CB_Unit_Number	IOR_vol_designtr
//	*BD_CB_Vol_Handle	IOR_req_vol_handle

/*
** i/o request descriptor
*/

typedef  void (* CMDCPLT)(void);

typedef struct _IOR { /* */

      ULONG    IOR_next;	     		/* client link for BCB's 					*/ 
												/* (MBZ for IORF_VERSION_002) 			*/

      USHORT   IOR_func;	    		/* function to be performed - see   	*/
				       						/* defines below.		   					*/

      USHORT   IOR_status;				/* request status - see defines	   	*/
				       						/* below.			   						*/
      ULONG    IOR_flags;	     		/* request control flags - see	   	*/
				       						/* defines below.		   					*/
      CMDCPLT  IOR_callback;	  		/* address to call request back to if 	*/ 
												/* IORF_SYNC_COMMAND is not set. 		*/

      ULONG    IOR_start_addr[2];	/* volume relative starting addr 		*/
												/* if IORF_LOGICAL_START_SECTOR is set.*/
												/* physical if not set.						*/

      ULONG    IOR_xfer_count;		/* number of sectors to process if 		*/
												/* IORF_CHAR_COMMAND is not set, or # 	*/
												/* of bytes if it is set.             	*/
												/* MUST be set to zero if no data xfer */

      ULONG    IOR_buffer_ptr;		/* BlockDev client buffer pointer. 		*/
												/* Contains pointer to data buffer 		*/
												/* or to null terminated list of sgd's */
												/* depending on IORF_SCATTER_GATHER 	*/
												/* Undefined if no data transfer     	*/

      ULONG    IOR_private_client;	/* BlockDev/IOS client reserved	  		*/

      ULONG    IOR_private_IOS;    	/* reserved space for IOS	   			*/

      ULONG    IOR_private_port;		/* private area for port driver			*/

      union    urequestor_usage _ureq; 
												/* requestor usage area, also used for	*/
												/* IOCTL's										*/

      ULONG    IOR_req_req_handle;	/* requestor provided request	   		*/
				       						/* handle. often is a pointer to    	*/
				       						/* to this ior or its containing    	*/
				       						/* iop. pushed on the stack by IOS   	*/
				       						/* before IOR_callback is called			*/

      ULONG    IOR_req_vol_handle; 	/* requestor provided media handle  	*/
				       						/* designating the media to perform 	*/
				       						/* the function on (VRP).					*/

      ULONG    IOR_sgd_lin_phys; 	/* pointer to first physical sgd, as   */
												/*	contrasted with IOR_buffer_ptr,		*/
												/*	which points to the logical sgds.	*/
				       						/* this is either a linear or phys  	*/
				       						/* address, depending on the needs  	*/
				       						/* of the drivers, as indicated	   	*/
				       						/* via the DCB demand bits.		   	*/

      UCHAR    IOR_num_sgds;	    	/* number of phys sgd's pointed to by  */
												/* IOR_sgd_lin_phys							*/
      UCHAR    IOR_vol_designtr; 	/* numeric representation of the    	*/
				       						/* drive letter designating the	   	*/
				       						/* volume to perform the function   	*/
				       						/* on (c: = 2).				   			*/

      USHORT   IOR_ios_private_1;	/* reserved by IOS to force alignment 	*/

      ULONG    IOR_reserved_2[2];  	/* reserved for internal use 				*/

} IOR, *PIOR;

/*
** command codes for IOR_func as defined above
*/

/*
**  IOR_READ 
**
**  Function:
**
**  Reads the # of sectors/bytes as specified in IOR_xfer_count.  
**  See PDK documentation for specifics of parameter setting.
**
**
**  Notes:
**
*/  

#define  IOR_READ		 0x00	  

/*
**  IOR_WRITE
**
**  Function:
**
**  Writes the # of sectors/bytes as specified in IOR_xfer_count.
**  See PDK documentation for specifics of parameter setting.
**
**  Notes:
**
*/  

#define  IOR_WRITE		 0x01	  

/*
**  IOR_VERIFY
**
**  Function:
**
**  Verifies the # of sectors/bytes as specified in IOR_xfer_count.
**
**  Notes:
**
*/  

#define  IOR_VERIFY		 0x02	  /* verify data on device	      */

/*
**  IOR_CANCEL
**
**  Function:
**
**  Cancels the IOR referenced in ...
**
**  Notes:
**  
**  Currently not supported in Chicago
**
*/  

#define  IOR_CANCEL		 0x03	  

/*
**  IOR_WRITEV
**
**  Function:
**
**  Writes and verifies the # of sectors/bytes as specified in IOR_xfer_count.
**
**  Notes:
**
**  Some hardware (IDE, for example) does not support this function.
**
*/  

#define  IOR_WRITEV		 0x04	 

/*
**  IOR_MEDIA_CHECK
**
**  Function:
**
**  Checks to see if media has changed, and returns status in IOR_status.
**
**  Notes:
**
**  Obsolete.  Use MEDIA_CHECK_RESET below
**
*/  

#define  IOR_MEDIA_CHECK	 0x05	 

/*
**  IOR_MEDIA_CHECK_RESET
**
**  Function:
**
**  Checks to see if media has changed, and returns status in IOR_status.
**
**  Notes:
**
*/  

#define  IOR_MEDIA_CHECK_RESET	 0x06	  

/*
**  IOR_LOAD_MEDIA
**
**  Function:
**
**  Performs a LOAD MEDIA operation on removable drives.
**
**  Notes:
**
*/  

#define  IOR_LOAD_MEDIA 	 0x07	 

/*
**  IOR_EJECT_MEDIA
**
**  Function:
**
**  Performs an EJECT MEDIA operation on removable drives.  
**
**  Notes:
**
**	 For disk and CD-ROM devices, applications are queried before eject,
**  and the lock count is checked for zero.  If one of these checks fails,
**  the user is queried to see if he wants to proceed.
*/  

#define  IOR_EJECT_MEDIA	 0x08	  

/*
**  IOR_LOCK_MEDIA
**
**  Function:
**
**  Performs a LOCK MEDIA operation on removable drives.  
**
**  Notes:
**
**  LOCK and UNLOCK MEDIA calls must be synchronized, as a depth counter is
**  maintained by IOS.
*/  

#define  IOR_LOCK_MEDIA 	 0x09	 

/*
**  IOR_UNLOCK_MEDIA
**
**  Function:
**
**  Performs an UNLOCK MEDIA operation on removable drives.  
**
**  Notes:
**
**  LOCK and UNLOCK MEDIA calls must be synchronized, as a depth counter is
**  maintained by IOS.  UNLOCK MEDIA is delayed by about 5 seconds.
*/  

#define  IOR_UNLOCK_MEDIA	 0x0A	  

/*
**  IOR_REQUEST_SENSE
**
**  Function:
**
**  Performs an REQUEST SENSE operation on removable drives.  
**
**  Notes:
**
**	 Currently supported for SCSI devices only.
**
*/  

#define  IOR_REQUEST_SENSE	 0x0B	  

/*
**  IOR_COMPUTE_GEOM
**
**  Function:
**
**  Results in a reexamination of volume and device characteristics.
**
**  Notes:
**
**  When this command is received, each IOS layer assumes the device has 
**  changed, and performs whatever initialization required.  For example,
**  the disk TSD will reexamine the partitions of a device.
**
*/  

#define  IOR_COMPUTE_GEOM	 0x0C	  

/*
**  IOR_GEN_IOCTL
**
**  Function:
**	
**	 Function permits device-specific IOCtl's to be issued to IOS drivers.
**  Also supports standard Int 21 function 44xx DOS IOCtl's.  See CDIOCTL.H
**  for examples of CD IOCtl's.
**
**  aliasing for fields used by IOCTL's - 
**  this section gives both the normal and IOCTL names for the fields
**
**  	IOR_ioctl_drive - BX from 16 bit IOCTL - usually a 1 based drive number
**  	IOR_ioctl_function - AX from 16 bit IOCTL
**  	IOR_ioctl_handle - useful handle? or original?
**  	IOR_ioctl_control_param - CX zero extended to ULONG
**  	IOR_ioctl_buffer_ptr - DS:DX mapped flat general buffer from the IOCTL
**  	IOR_ioctl_client_params - a pointer to the parameters passed in from
**			the calling application the IORF_16BIT_IOCTL flag must be used to
**			determine the structure type.  If it is a 16 bit IOCTL, the pointer
**			will be to the CLIENT_REGS structure.  32 bit is as of yet undef'ed
**  	IOR_ioctl_return - return value to be mapped to the IOCTL caller on exit
**  	IOR_ioctl_buffer_length - size of buffer pointed to by 
**			IOR_ioctl_buffer_ptr	(not used by int 21 440d IOCTL calls)
**
**  Notes:
**
*/  

#define  IOR_GEN_IOCTL		 0x0D	

/*
**  IOR_FORMAT
**
**  Function:
**
**  Allows issuance of a low level format packet as defined in the int 13
**  specification.
**
**  Notes:
**
**  IOR_format_address_field => low level sector map
**  See IOP.H IOP_FORMAT_XXX fields for additional information.
*/  

#define  IOR_FORMAT		 0x0E	  

/*
**  IOR_SCSI_PASS_THROUGH
**
**  Function:
**
**  Each layer passes this request down with modification to the SCSI port
**  driver.  
**
**  Notes:
**
**  The IOR must contain a pointer to a valid SRB.  Also, "ebx" must point
**  to the BDD portion of the DCB when IOS_SendCommand is called with a 
**  passthrough request.
*/  

#define  IOR_SCSI_PASS_THROUGH	 0x0F

/*
**  IOR_CLEAR_QUEUE
**
**  Function:
**
**  Callback of request indicates all requests issued prior to this request
**  have been completed.
**
**  Notes:
*/  

#define  IOR_CLEAR_QUEUE	 0x10	

/*
**  IOR_DOS_RESET
**
**  Function:
**
**  Issued in response to an int 13 reset by the int 13 vxd
**
**  Notes:
**
**  Currently used only by and for the IOS floppy driver.
*/  

#define  IOR_DOS_RESET		 0x11	 

/*
**  IOR_SCSI_REQUEST
**
**  Function:
**
**  generic function that indicates the request is for SCSI.
**
**  Notes:
**
**  SRB field in IOR must be valid.  Normally used by IOS layers to send 
**  internal requests to SCSI devices.
*/  

#define  IOR_SCSI_REQUEST 	0x12	  

/*
**  IOR_SET_WRITE_STATUS
**
**  Function:
**
**  results in the correct setting of DCB_DEV_WRITABLE flag in the DCB if
**  SUCCESS status is returned.
**
**  Notes:
** 
**  Currently only supported by the IOS floppy driver.
*/  

#define  IOR_SET_WRITE_STATUS 0x22

/*
**  IOR_RESTART_QUEUE
**
**  Function:
**
**  "Placeholder" function that results in restarting of the queue provided
**  by the IOS queuing services.  Has no other effect.  
**
**  Notes:
**
**  Should be sent after the DCB_QUEUE_FREEZE field has been decremented to
**  zero.
*/  

#define  IOR_RESTART_QUEUE	0x23 

/*
**  IOR_ABORT_QUEUE
**
**  Function:
**
**	 Causes each layer to call back each queued request with status of 
**  IORS_CANCELED
**
**  Notes:
**
*/  

#define  IOR_ABORT_QUEUE	0x24 

/*
**  IOR_SPIN_DOWN
**
**  Function:
**
**  Causes the specified drive to be spun down.
**
**  Notes:
**
*/  

#define  IOR_SPIN_DOWN   	0x25	 

/*
**  IOR_SPIN_UP
**
**  Function:
**
**  Causes the specified drive to be spun up.
**
**  Notes:
**
*/  

#define  IOR_SPIN_UP	   	0x26	 

/*
**  IOR_FLUSH_DRIVE
**
**  Function:
**
**  Causes layer drivers to flush out dirty data.
**
**  Notes:
**
**  This is a synchronous function and should not be called at 
**  event/interrupt time.
**  
*/  

#define  IOR_FLUSH_DRIVE	0x27	 


/*
**  IOR_FLUSH_DRIVE_AND_DISCARD
**
**  Function:
**
**  Causes layer drivers to flush out dirty data and then dicard cached 
**  data.
**
**  Notes:
**
**  This is a synchronous function and should not be called at 
**  event/interrupt time.
**  
*/  

#define  IOR_FLUSH_DRIVE_AND_DISCARD 0x28


/*
**  IOR_FSD_EXT
**
**  Function:
**
**  Sends private command from an IOS client to the FSD extension
**  layer (DRP_FSD_EXT_1 or DRP_FSD_EXT_2) such as a Compressed
**  Volume Manager (CVM).
**
**  Notes:
**
**  This call is similar in intent and usage to IOR_GEN_IOCTL;
**  however, it can be used in the swapper path, so, in general,
**  the handling driver should not touch pageable code or data.
**  
*/  

#define  IOR_FSD_EXT 0x29

/*
**  IOR_FLOPTICAL_MODE_SENSE
**
**  Function:
**
**  Issues MODE SENSE command to page 0x2e, as is required to indicate to the
**  floptical that it is supported.
**
**  Notes:
**
**  Don't use this command, as it is used by the system.
**  
*/  

#define  IOR_FLOPTICAL_MODE_SENSE 0x2A

/*
** IOR status codes for IOR_status
*/

#define IORS_SUCCESS		 			0x00 	/* command completed successfully 	*/

#define IORS_SUCCESS_WITH_RETRY  0x01  /* successful, but retries required */

#define IORS_SUCCESS_WITH_ECC	 	0x02	/* ditto, but with ECC 					*/

#define IORS_INVALID_SECTOR	 	0x10 	/* old blockdev error code for 		*/
													/* requests on an invalid sector 	*/
													/*	number 									*/
#define IORS_CANCELED		 		0x11	/* command was canceled 				*/

#define IORS_CMD_IN_PROGRESS	 	0x12 	/* command is in progress, and can't*/
													/* be canceled 							*/

#define IORS_INVALID_CMD_PTR	 	0x13 	/* cancel of an invalid command 		*/

#define IORS_MEDIA_ERROR	 		0x14	/* media failure 							*/

#define IORS_DEVICE_ERROR	 		0x15	/* error on recalib. drive, etc.		*/

#define IORS_INVALID_COMMAND	 	0x16	/* Command not supported or invalid	*/

#define IORS_NO_DEVICE		 		0x17 	/* physical or logical device 		*/
													/* nonexistant 							*/

#define IORS_NO_MEDIA		 		0x18 	/* media removed from device 			*/

#define IORS_UNCERTAIN_MEDIA	 	0x19 	/* media may have changed 				*/

#define IORS_UNREC_ERROR	 		0x1A  /* un-recoverable error 				*/

#define IORS_HW_FAILURE 	 		0x1B  /* general hardware failure 			*/

#define IORS_UNFORMATTED_MEDIA	0x1C  /* unformatted media 					*/

#define IORS_MEMORY_ERROR	 		0x1D  /* error allocating DMA buffer space*/
													/* for I/O									*/

#define IORS_TIME_OUT		 		0x1E  /* device timed out 						*/

#define IORS_WRITE_PROTECT	 		0x1F  /* write protect error 					*/

#define IORS_NOT_READY		 		0x20  /* device not ready 						*/

#define IORS_BUSY		 				0x21 /* device is busy 							*/

#define IORS_VOL_NOT_LOCKED	 	0x22  /* volume received unlock without lk*/

#define IORS_VOL_LOCKED 	 		0x23  /* eject received with drive locked	*/												 				

#define IORS_VOL_NOT_REMOVABLE	0x24  /* removable request to non-remov   */
													/* drive 									*/

#define IORS_VOL_IN_USE 	 		0x25  /* volume is in use - used for eject*/

#define IORS_LOCK_COUNT_EXCEEDED 0x26  /* volume lock count exceeded 		*/

#define IORS_VALID_EJECT_FAILED  0x27  /* eject command was not accepted 	*/

#define IORS_ILLEGAL_ACCESS_MODE 0x28  /* cdrom read on audio or attempt   */
													/* play on data							*/

#define IORS_LOCK_VIOLATION	 	0x29  /* illegal access to locked device 	*/

#define IORS_WRONG_MEDIA	 		0x2a  /* wrong media in drive 				*/

#define IORS_OUT_OF_SPACE	 		0x2b  /* no space on media 					*/

#define IORS_BIG_IO_BREAKUP_FAILED 0x2c/* attempt to split big I/O barfed 	*/

#define IORS_ERROR_DESIGNTR	 	0x10  /* bit indicating error occurred 	*/

#define IORS_INVALID_PARM	IORS_ERROR_DESIGNTR  
													/* bit indicating error occurred 	*/


/*
** IOR flag defines for IOR_flags
*/

#define IORF_HIGH_PRIORITY_BIT	0 		/* binary priority indication 		*/
#define IORF_HIGH_PRIORITY	(1 << IORF_HIGH_PRIORITY_BIT)  /* 0x0001 */

#define IORF_SCATTER_GATHER_BIT  1 		/* bit indicating BDCB physical SGD */
													/* list present 							*/
#define IORF_SCATTER_GATHER	(1 << IORF_SCATTER_GATHER_BIT) /* 0x0002 */

#define IORF_DONT_CACHE_BIT	 	2 		/* bit indicating that the BDCB data*/
													/*  should not be cached 				*/
#define IORF_DONT_CACHE 	 (1 << IORF_DONT_CACHE_BIT)    /* 0x0004 */

#define IORF_BYPASS_VOLTRK	0x0008		/* request will not be volume trked	*/
#define IORF_BYPASS_VOLTRK_BIT	3

#define IORF_16BIT_IOCTL	0x0010		/* if true, 16 bit IOCTL calling 	*/
													/* app, else 32 bit ioctl				*/
#define IORF_16BIT_IOCTL_BIT	 4

#define IORF_SWAPPER_IO	0x0020			/* request is from the swapper 		*/
#define IORF_SWAPPER_IO_BIT   5

#define IORF_DOUBLE_BUFFER	  0x0040  	/* indicates that the request is to */
													/* be completed via double 			*/
													/* buffering.  client must 			*/
													/* ensure that both the sgd buffer 	*/
													/* pointers and IOR_sgd_lin_phys		*/
													/* are virtual pointers. 			  	*/
#define IORF_DOUBLE_BUFFER_BIT	 6

#define IORF_SYNC_CMD_DONE	0x0080		/* used by IOS only to indicate 		*/
													/* completion 								*/
													/* of synchronous command 				*/
#define IORF_SYNC_CMD_DONE_BIT	 7

#define IORF_SYNC_COMMAND	0x0100		/* indicates synchronous command 	*/
													/* (complete before return from IOS */
#define IORF_SYNC_COMMAND_BIT	 8

#define IORF_CHAR_COMMAND	0x0200		/* indicates transfer counts and 	*/
													/* scatter/gather buffer sizes are 	*/
													/* byte values 							*/
#define IORF_CHAR_COMMAND_BIT	 9

#define IORF_VERSION_002	  0x0400  	/* indicates use of extended BCB 	*/
													/* (IOR) format for request			*/
#define IORF_VERSION_002_BIT	10

#define IORF_BYPASS_QUEUE	  0x0800  	/* request should bypass IOS queuing*/
													/* internal IOS driver use only 		*/
#define IORF_BYPASS_QUEUE_BIT	11

#define IORF_BLOCKDEV_EMULATE	0x1000	/* this request started as a BCB 	*/
#define IORF_BLOCKDEV_EMULATE_BIT 12

#define IORF_POSTPONED_VOL_OPS	0x2000
#define IORF_POSTPONED_VOL_OPS_BIT 13

#define IORF_INHIBIT_GEOM_RECOMPUTE	0x4000	
#define IORF_INHIBIT_GEOM_RECOMPUTE_BIT 14 
													/* force inhibit of device geometry */
												   /* recomputation. 						*/

#define IORF_SRB_VALID	0x8000			/* indicates that IOP_srb points to */
#define IORF_SRB_VALID_BIT 15				/* a valid SRB	 							*/					

#define IORF_BYPASS_A_B 0x10000			/* indicates that VOLTRK should 		*/
#define IOFR_BYPASS_A_B_BIT 16			/* bypass AB check 						*/

#define IORF_QUIET_VOLTRK	0x20000		/* indicates that volume tracking 	*/
#define IORF_QUIET_VOLTRL_BIT 17			/* just return error when wrong		*/
													/* media in drive (no dialog box)	*/

#define IORF_AUDIO_DATA_READ	0x40000 	/* indicates an audio data read 		*/
#define IORF_AUDIO_DATA_READ_BIT 18		

#define IORF_LOGICAL_START_SECTOR	0x80000 
#define IORF_LOGICAL_START_SECTOR_BIT 19
													/* IOR_start_addr is logical			*/

#define IORF_PARTITION_BIAS_ADDED	0x100000 
#define IORF_PARTITION_BIAS_ADDED_BIT 20 /* set by TSD only to indicate that */
													/* it has added in the bias, as req'd */
													/* by IORF_LOGICAL_START_SECTOR      */

#define IORF_DATA_IN_BIT 21				/* indicates a data read operation 	*/ 
#define IORF_DATA_IN	(1 << IORF_DATA_IN_BIT)

#define IORF_DATA_OUT_BIT 22				/* indicates a data write operation */
#define IORF_DATA_OUT	(1 << IORF_DATA_OUT_BIT)

#define IORF_VOL_RETRY_BIT 23				/* indicates volume tracking retried*/
#define IORF_VOL_RETRY	(1 << IORF_VOL_RETRY_BIT)
													/* request (internal use only)		*/

#define	IORF_NO_COMPRESS_BIT 24			/* do not compress data 				*/
#define	IORF_NO_COMPRESS (1 << IORF_NO_COMPRESS_BIT)

#define	IORF_DIRECT_IO_BIT 25			/* E.g., INT 25h or INT 26h 			*/
#define	IORF_DIRECT_IO	(1 << IORF_DIRECT_IO_BIT)

#define IORF_PHYS_SGDS_BIT 26
#define IORF_PHYS_SGDS (1 << IORF_PHYS_SGDS_BIT) 
													/* indicates IOR_sgd_lin_phys valid */

#define IORF_IO_TOO_BIG_BIT 27
#define IORF_IO_TOO_BIG	(1 << IORF_IO_TOO_BIG_BIT)
										   		/* indicates I/O is too big for 		*/
													/* single pass  (set by IOS criteria*/
													/* code).									*/
#define IORF_WIN32_BIT 28
#define IORF_WIN32	(1 << IORF_WIN32_BIT)
										   		/* indicates I/O is from 32 bit API */
#define IORF_CHAR_DEVICE_BIT 29
#define IORF_CHAR_DEVICE	(1 << IORF_CHAR_DEVICE_BIT)

#define IORF_PHYS_CMD_BIT 30				/* indicates I/O is for a physical 	*/
													/* device 									*/
#define IORF_PHYS_CMD	(1 << IORF_PHYS_CMD_BIT)

#define IORF_IDE_RESERVED_BIT 31				/* indicates I/O is for a physical 	*/
													/* device 									*/
#define IORF_IDE_RESERVED	(1 << IORF_IDE_RESERVED_BIT)


/*
** safety check to ensure IOR and BCB align
*/

/*ASM
.errnz	 BD_CB_Next-IOR_next
.errnz	 BD_CB_Command - IOR_func
.errnz	 BD_CB_Cmd_Status - IOR_status
.errnz	 BD_CB_Flags - IOR_flags
.errnz	 BD_CB_Cmd_Cplt_Proc - IOR_callback
.errnz	 BD_CB_Sector - IOR_start_addr
.errnz	 BD_CB_Count - IOR_xfer_count
.errnz	 BD_CB_Buffer_Ptr - IOR_buffer_ptr
.errnz	 BD_CB_Reserved_Client - IOR_private_client
.errnz	BD_CB_Spare - _ureq

*/



#define IOR_requestor_usage _ureq._IOR_requestor_usage

#define IOR_ioctl_drive _ureq.sdeffsd_req_usage._IOR_ioctl_drive

#define IOR_ioctl_function _ureq.sdeffsd_req_usage._IOR_ioctl_function

#define IOR_ioctl_handle _ureq.sdeffsd_req_usage._IOR_ioctl_drive

#define IOR_ioctl_control_param _ureq.sdeffsd_req_usage._IOR_ioctl_control_param

#define IOR_ioctl_buffer_ptr _ureq.sdeffsd_req_usage._IOR_ioctl_buffer_ptr

#define IOR_ioctl_client_params _ureq.sdeffsd_req_usage._IOR_ioctl_client_params

#define IOR_ioctl_return _ureq.sdeffsd_req_usage._IOR_ioctl_return

#define IOR_ioctl_buffer_length IOR_private_client

/*
** IOR equivalents for FORMAT packet
*/

#define IOR_format_address_field IOR_sgd_lin_phys
												/* ptr to address field struct*/

/* BlockDev compatibility check
**
*/

/* ASM
.errnz BDC_Read - IOR_READ
.errnz BDC_Write - IOR_WRITE
.errnz BDC_Verify - IOR_VERIFY
.errnz BDC_Cancel - IOR_CANCEL
*/


/*
** Blockdev compatibility check
*/

/* ASM

.errnz BDS_Success - IORS_SUCCESS
.errnz BDS_Success_With_Retries - IORS_SUCCESS_WITH_RETRY
.errnz BDS_Success_With_ECC -  IORS_SUCCESS_WITH_ECC

.errnz BDS_First_Error_Code - IORS_ERROR_DESIGNTR
.errnz BDS_Invalid_Sector_Number -  IORS_INVALID_SECTOR
.errnz BDS_Canceled - IORS_CANCELED
.errnz BDS_Cmd_In_Progress - IORS_CMD_IN_PROGRESS
.errnz BDS_Invalid_Cmd_Ptr - IORS_INVALID_CMD_PTR
.errnz BDS_Media_Error - IORS_MEDIA_ERROR
.errnz BDS_Device_Error - IORS_DEVICE_ERROR
.errnz BDS_Invalid_Command - IORS_INVALID_COMMAND
*/





