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
* DRP (Driver Registration Packet) Data Structure
*
\****************************************************************************/

/* C Definition */

typedef struct DRP { /* */
        CHAR    DRP_eyecatch_str[8];    /* eye catcher string */
        ULONG   DRP_LGN;                /* drivers load group number - see values below */
        PVOID   DRP_aer;                /* pointer to async event routine   */
        PVOID   DRP_ilb;                /* ILB virtual address              */
        CHAR    DRP_ascii_name[16];
        BYTE	 DRP_revision;				 /* driver revision 						 */
        ULONG   DRP_feature_code;       /* Feature Code                     */
        USHORT  DRP_if_requirements;    /* I/F Requirements                 */
		  UCHAR	 DRP_bus_type;				 /* type of I/O bus if port driver   */
        USHORT  DRP_reg_result;         /* Registration Results             */
        ULONG   DRP_reference_data;      /* field passed in on initialize aep */
		  UCHAR	 DRP_reserved1[2];		 /* filler for alignment */		    
		  ULONG	 DRP_reserved2[1];		 /* reserved - MBZ */
} DRP, *PDRP;


#define EyeCatcher      'XXXXXXXX'

/*
** Feature Code Definitions must match DVT feature code definitions in dvt.h
*/

#define DRP_FC_SCAN_DOWN   0x04    /*  on = bios scans targets from high to low*/
#define DRP_FC_IO_FOR_INQ_AEP 0x40 /*  on = PD needs to send I/O through IOP  */
										  	  /*  in response to an INQUIRY AEP.  Results */
										     /*  in CONFIGURE AEP for INQUIRY DCB.       */
         
#define DRP_FC_HALF_SEC    0x2000  /*  on = notify driver every half second    */
#define DRP_FC_1_SEC       0x2000  /*  on = notify driver every second         */
#define DRP_FC_2_SECS      0x4000  /*  on = notify driver every two seconds    */
#define DRP_FC_4_SECS      0x8000  /*  on = notify driver every four seconds   */
#define DRP_FC_DYNALOAD   0x10000  /*  on = driver was dynaloaded	by IOS    	 */
#define DRP_FC_NEED_PRELOAD   0x20000  /*  on = driver needs to hook I/O even  */
													/* before the port driver.  AEP_config_dcb */
                                       /* will be received before PD when set */
#define DRP_FC_NEED_PRE_POST_LOAD   0x40000  
													/* same as above, except that the drive */
													/* will receive 2 config_dcb calls for */
													/* each DCB, 1 before the port driver, */
													/* and 1 after layers before its load */
													/* group have been initialized.  Note */
													/* that care must be taken not to insert */
													/* TWICE in the same DCB.  				*/
/*
** I/F Requirements
*/

#define DRP_IF_ISA      0x0001  /* on = driver supports isa platforms       */
#define DRP_IF_EISA     0x0002  /* on = driver supports eisa platforms      */
#define DRP_IF_MCA      0x0004  /* on = driver supports mca platforms       */
#define DRP_IF_STD      0x00ff  /* on = drvr supports all standard platforms*/

/*
** I/O Bus Types
*/

#define DRP_BT_ESDI		0x00  	/* ESDI or ESDI emulator */
#define DRP_BT_SCSI		0x01  	/* SCSI or SCSI emulator */
#define DRP_BT_FLOPPY	0x02  	/* NEC FLOPPY or FLOPPY emulator */
#define DRP_BT_SMART		0x03  	/* smart device */
#define DRP_BT_ABIOS		0x04  	/* ABIOS or ABIOS emulator */

/*
** driver load group definitions
*/

#define DRP_IFS_BIT             0x00    /* reserved for ifs manager         */
#define DRP_RESRVD1_BIT         0x01    /* reserved - do not use            */
#define DRP_FSD_BIT             0x02    /* Installable File system layer    */
#define DRP_FSD_EXT_1_BIT	0x03    /* FSD extension, e.g., DBLSPACE    */
#define DRP_FSD_EXT_2_BIT	0x04    /* FSD extension, e.g., encryption  */
#define DRP_VOLTRK_BIT          0x05    /* post-volume tracking value added driver*/
#define DRP_CLASS_DRV_BIT       0x06    /* class driver                     */
#define DRP_TSD_BIT             0x07    /* type specific driver             */
#define DRP_VSD_1_BIT           0x08    /* driver is in vendor enhancement layer 1*/
#define DRP_VSD_2_BIT           0x09    /* driver is in vendor enhancement layer 2*/
#define DRP_VSD_3_BIT           0x0a    /* driver is in vendor enhancement layer 3*/
#define DRP_SCSI_LAYER_BIT      0x0b    /* SCSI'izer                        */
#define DRP_VSD_4_BIT           0x0c    /* driver is in vendor enhancement layer 4*/
#define DRP_VSD_5_BIT           0x0d    /* driver is in vendor enhancement layer 5*/
#define DRP_VSD_6_BIT           0x0e    /* driver is in vendor enhancement layer 6*/
#define DRP_VSD_7_BIT           0x0f    /* driver is in vendor enhancement layer 7*/
#define DRP_VSD_8_BIT           0x10    /* driver is in vendor enhancement layer 8*/
#define DRP_VSD_9_BIT           0x11    /* driver is in vendor enhancement layer 9*/
#define DRP_RESRVD18_BIT        0x12    /* reserved do not use              */
#define DRP_MISC_PD_BIT         0x13    /* other port drivers               */
#define DRP_NT_MPD_BIT          0x14    /* NT style mini-port drivers       */
#define DRP_NT_PD_BIT        	  0x15    /* NT style port (hw independ) drvr */
#define DRP_ESDI_PD_BIT         0x16    /* ESDI port driver                 */
#define DRP_ESDIEMUL_PD_BIT     0x17    /* port drivers for esdi emulators  */
#define DRP_RESRVD23_BIT        0x18    /* reserved do not use              */
#define DRP_ABIOS_PD_BIT        0x19    /* ABIOS port driver                */
#define DRP_ABIOS_PREMPT_PD_BIT 0x1a    /* ABIOS pre-empting port drivers   */
#define DRP_NEC_FLOPPY_BIT      0x1b    /* NEC floppy driver layer          */
#define DRP_RESRVD28_BIT        0x1c    /* reserved do not use              */
#define DRP_SOC_SER_DRV_BIT     0x1d    /* socket service drivers           */
#define DRP_SOC_DRV_BIT         0x1e    /* socket drivers                   */
#define DRP_IOS_REG_BIT         0x1f    /* reserved for ios registry        */
#define DRP_LGN_MAX             0x1f    /* maximum valid load group number  */

#define DRP_IFS                 ( 1 << DRP_IFS_BIT              )
#define DRP_RESRVD1             ( 1 << DRP_RESRVD1_BIT          )
#define DRP_FSD                 ( 1 << DRP_FSD_BIT              )
#define DRP_FSD_EXT_1		(1 << DRP_FSD_EXT_1_BIT)
#define DRP_FSD_EXT_2		(1 << DRP_FSD_EXT_2_BIT)
#define DRP_VOLTRK              ( 1 << DRP_VOLTRK_BIT           )
#define DRP_CLASS_DRV           ( 1 << DRP_CLASS_DRV_BIT        )
#define DRP_TSD                 ( 1 << DRP_TSD_BIT              )
#define DRP_VSD_1               ( 1 << DRP_VSD_1_BIT            )
#define DRP_VSD_2               ( 1 << DRP_VSD_2_BIT            )
#define DRP_VSD_3               ( 1 << DRP_VSD_3_BIT            )
#define DRP_SCSI_LAYER          ( 1 << DRP_SCSI_LAYER_BIT       )
#define DRP_VSD_4               ( 1 << DRP_VSD_4_BIT            )
#define DRP_VSD_5               ( 1 << DRP_VSD_5_BIT            )
#define DRP_VSD_6               ( 1 << DRP_VSD_6_BIT            )
#define DRP_VSD_7               ( 1 << DRP_VSD_7_BIT            )
#define DRP_VSD_8               ( 1 << DRP_VSD_8_BIT            )
#define DRP_VSD_9               ( 1 << DRP_VSD_9_BIT            )
#define DRP_RESRVD18            ( 1 << DRP_RESRVD18_BIT         )
#define DRP_MISC_PD             ( 1 << DRP_MISC_PD_BIT          )
#define DRP_NT_MPD              ( 1 << DRP_NT_MPD_BIT           )
#define DRP_NT_PD               ( 1 << DRP_NT_PD_BIT            )
#define DRP_RESRVD20            ( 1 << DRP_RESRVD20_BIT         )
#define DRP_ESDI_PD             ( 1 << DRP_ESDI_PD_BIT          )
#define DRP_ESDIEMUL_PD         ( 1 << DRP_ESDIEMUL_PD_BIT      )
#define DRP_RESRVD23            ( 1 << DRP_RESRVD23_BIT         )
#define DRP_ABIOS_PD            ( 1 << DRP_ABIOS_PD_BIT         )
#define DRP_ABIOS_PREMPT_PD     ( 1 << DRP_ABIOS_PREMPT_PD_BIT  )
#define DRP_NEC_FLOPPY          ( 1 << DRP_NEC_FLOPPY_BIT       )
#define DRP_RESRVD28            ( 1 << DRP_RESRVD28_BIT         )
#define DRP_SOC_SER_DRV         ( 1 << DRP_SOC_SER_DRV_BIT      )
#define DRP_SOC_DRV             ( 1 << DRP_SOC_DRV_BIT          )
#define DRP_IOS_REG             ( 1 << DRP_IOS_REG_BIT          )

/*
** Registration Results
*/

#define DRP_REMAIN_RESIDENT     1       /* Driver should remain resident    */
#define DRP_MINIMIZE            2       /* Driver should minimize           */
#define DRP_ABORT               3       /* Driver should not load           */
#define DRP_INVALID_LAYER       4       /* bad layer number - abort driver  */
