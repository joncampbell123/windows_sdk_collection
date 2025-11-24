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
*
* IDA (ios data area)
*
*
\****************************************************************************/

typedef struct IDA { /* */

/*
** miscellaneous variables.
*/

/* offset 00                                                                */

        ULONG   IDA_first_ice;          /* ios:32 pointer to first ice      */
        ULONG   IDA_current_ice;        /* ios:32 pointer to current ice    */
        ULONG   IDA_ios_mem_phys;       /* physical address of ios mem pool */
        ULONG   IDA_current_ice_len;    /* # of bytes left in config table  */

/* offset 0x10                                                              */

        ULONG   IDA_ios_timer;          /* counter - tics every 250 m-secs  */
                                        /* see equates below.               */
        ULONG   IDA_ios_counter;        /* number of times to loop for 10th */
                                        /* second cpu spin wait - used in   */
                                        /* ILB_wait_10th_sec                */
        ULONG   IDA_start_eci;          /* ios:32 pointer to start of esdi  */
                                        /* ctlr identification strings.     */
        ULONG   IDA_start_cct;          /* ios:32 pointer to start of ctlr  */
                                        /* characteristics tables.          */

/* offset 0x20                                                              */

        CHAR    IDA_esdi_p_in_use;		/* non-zero = a driver has claimed  */
													/* the primary esdi i/o addresses   */
													/* (1f0 thru 1f7, 3f6 and 3f7) and  */
													/* irq (irq 14).                    */

        CHAR    IDA_esdi_s_in_use;  	/* non-zero = a driver has claimed  */
													/* the primary esdi i/o addresses   */
													/* (170 thru 177, 376 and 377) and  */
													/* irq (irq 15).                    */

		  CHAR	 IDA_Num_Floppies;   	/* no. of floppies in the system	 */

		  CHAR	 PAD0;						/* dword align				   	    */

        USHORT  IDA_platform;       	/* platform (machine type) code -   */
													/* see values in ilb.h.             */

		  USHORT	 PAD1;						/* dword align				   	    */

        ULONG   IDA_flags;					/* various state flags, see below   */
													/* for value definitions            */

   	  PVOID IDA_platform_config_data;/* pointer to PCD chain */

/* offset 0x30 */

        PVOID   IDA_first_dcb;			/* pointer to first DCB */
        PVOID   IDA_logical_dev_map;	/* pointer to LDM array */
        PVOID   IDA_physical_dev_map;	/* pointer to IDM array */
        PVOID   IDA_rmm_phys_dcb;		/* pointer to the RMM DCB */

/* offset 0x40 */

        PVOID   IDA_first_vrp;			/* pointer to first VRP */
        ULONG   IDA_drive_letter_map;	/* logical drive bitmap for */
													/* assigning logical disk numbers */
        ULONG   IDA_cdrom_letter_map;	/* logical drive bitmap for */
													/* cd-rom devices */
        ULONG   IDA_rmd_ptr;				/* start of int 2fh startup chain */

/* offset 0x50 */

        ULONG   IDA_aspi_cam_rmds;		/* start of ASPI/CAM RMD's */
        ULONG	 IDA_big_mem_buf_virt;	/* big memory buffer ptrs */
        ULONG	 IDA_big_mem_buf_phys;							 
        ULONG	 IDA_big_mem_buf_size;	/* size of big memory buffer */

/* offset 0x60 */

		  ULONG	 IDA_ios_mem_pool_size;	/* size of IOS heap - size of IDA */
		  ULONG	 IDA_dbl_vol_map;			/* map of compressed dblspace volumes */
		  ULONG	 IDA_ontrack_data;  		/* pointer to any ontrack config data */
		  USHORT	 IDA_rm_irq;				/* bitmap of unknown real mode drivers' */
													/* irq's										*/
} IDA, *pIDA;

/*
** equates for use with IDA_ios_timer
*/

#define IDA_250_M_SECOND            1   /* quarter second of timer ticks    */
                                        /* note that this value can only be */
                                        /* used reliably after the code has */
                                        /* synced with the timer by some    */
                                        /* means.                           */
#define IDA_HALF_SECOND             2   /* half second of timer ticks.      */
#define IDA_ONE_SECOND          1 * 4   /* one second of timer ticks.       */
#define IDA_TWO_SECONDS         2 * 4   /* two seconds of timer ticks.      */
#define IDA_TEN_SECONDS        10 * 4   /* ten seconds of timer ticks.      */
#define IDA_40_SECONDS         40 * 4   /* 40 seconds of timer ticks.       */

#define IDA_INIT_COUNTER        2       /* initial value for IDA_ios_counter*/

/*
** equates for use with IDA_flags
*/

#define	IDAF_BOOT_COMPLETE	0x0001	/* indicate boot complete           */
#define	IDAF_REAL_MODE_ONLY	0x0002	/* indicates no protected mode port */
													/* drivers may load, except floppy  */
#define	IDAF_FORCE_RM_IO		0x0004	/* indicates only use real mode drivers */
#define	IDAF_REAL_MODE_LOGICAL  0x0008/* indicates protected mode port    */
#define	IDAF_FORCE_PM_IO 		0x0010	/* forces protected mode I/O        */
#define	IDAF_V86_AREA_TAKEN	0x0020	/* area already taken               */
#define	IDAF_BIG_MEMORY		0x0040	/* indicates big memory system      */
#define	IDAF_MAPPER_BUSY		0x0080	/* real mode mapper is busy         */
#define	IDAF_NEED_4BHOOK		0x0100	/* rmm needs to hook 4b 	    */
#define	IDAF_INT13_DUE_TO_RMM	0x0200/* int 13 because of rmm calling    */
													/* real mode driver */
#define	IDAF_MAPPER_INIT		0x0400	/* real mode mapper init            */
#define	IDAF_PUNT_CDS			0x0800	/* force CD access through real mode*/
#define	IDAF_MSCDEX_PRESENT	0x1000	/* indicates MSCDEX in the system   */
#define	IDAF_DOS_PAGER			0x2000	/* system is paging through DOS  */
#define	IDAF_SHUTDOWN			0x4000  	/* system is shutting down          */

#define	IDAF_INT13_FCN_8_BIT	15			/* currently processing fcn 8 */
#define	IDAF_INT13_FCN_8 		1 << IDAF_INT13_FCN_8_BIT /* 0x8000 */

#define	IDAF_STAC_PRESENT_BIT 16		/* stac is present in the system */
#define	IDAF_STAC_PRESENT 	1 << IDAF_STAC_PRESENT_BIT

#define	IDAF_START_SUCCESS_BIT 17		/* device started regardless of return */
													/* code										*/
#define	IDAF_START_SUCCESS 	1 << IDAF_START_SUCCESS_BIT

#define	IDAF_CANT_SHARE_IRQ_BIT	18  	/* this bit is set by SCSIPORT if	*/
													/* there is an usafe real mode drvr */
													/* that hooks the IRQ for the device*/

#define	IDAF_CANT_SHARE_IRQ 1 << IDAF_CANT_SHARE_IRQ_BIT

#define	IDAF_NO_BOOT_CONFIG_BIT	19		/* indicates that a devnode being */
													/* does not have any boot config.*/

#define	IDAF_NO_BOOT_CONFIG 1 << IDAF_NO_BOOT_CONFIG_BIT

#define	IDAF_512_BYTE_EMUL_POSSIBLE_BIT 20 
													/*indicates that ASPIDISK is loaded*/
#define	IDAF_512_BYTE_EMUL_POSSIBLE 1 << IDAF_512_BYTE_EMUL_POSSIBLE_BIT

#define	IDAF_INT13_IGNORE_COPY_PROT_BIT 21 
										/*indicates that an IOCTL that accesses*/
										/* copy protection information is in   */
										/* progress 						   */
#define	IDAF_INT13_IGNORE_COPY_PROT 1 << IDAF_INT13_IGNORE_COPY_PROT_BIT

#define	IDAF_EZ_DRIVE_BIT 22    		/* indicates EZ drive present */
#define	IDAF_EZ_DRIVE 1 << IDAF_EZ_DRIVE_BIT

#define	IDAF_NO_SPARROW_BIT 23    		/* indicates EZ drive present */
#define	IDAF_NO_SPARROW 1 << IDAF_NO_SPARROW_BIT

#ifdef	NEC_98
#define	IDAF_BIOSCDROM_PRESENT_BIT	31	/* indicates CDROM driver which uses BIOS in the system */
#define	IDAF_BIOSCDROM_PRESENT	1 << IDAF_BIOSCDROM_PRESENT_BIT
#endif


/*
** platform mask for use with IDA_platform
*/

#define IS_ISA          0x0001          /* platform is isa.                 */
#define IS_EISA         0x0002          /* platform is eisa.                */
#define IS_MCA          0x0004          /* platform is microchannel.*/
#define IS_FAMILY3      0x0008          /* platform is that weird Family 3! */
