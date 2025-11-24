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
* VRP (Volume Request Parameters) Data Structure
*
\****************************************************************************/

/* H2INCSWITCHES -t */

typedef struct _VRP { /* */

        ULONG   VRP_demand_flags;       /* demand flags which the fsd must  */
                                        /* satisfy. see defines below for   */
                                        /* values.                          */
        ULONG   VRP_event_flags;        /* flags to give fsd warning of     */
                                        /* special events without the       */
                                        /* overhead of a call to the i/o    */
                                        /* complex. the fsd may ignore      */
                                        /* flags (provided that it still    */
                                        /* prevents data corruption) in     */
                                        /* which case it will be notified   */
                                        /* on the next i/o to the volume.   */
                                        /* see defines below for values.    */
        USHORT  VRP_max_sgd;            /* maximum number of scatter/gather */
                                        /* descriptors that may be built.   */
                                        /* note that fsd's should use the   */
                                        /* lesser of this value and 17.     */
        USHORT  VRP_max_req_size;       /* maximum size of request packet.  */
                                        /* this is the size packet that the */
                                        /* fsd should allocate.             */
        ULONG   VRP_delta_to_ior;       /* the delta from the begining of   */
                                        /* the request packet to the start  */
                                        /* of the ior.                      */
        ULONG   VRP_block_size;         /* block size of volume             */
        ULONG   VRP_fsd_hvol;           /* file system's handle for this volume */
        ULONG   VRP_fsd_entry;          /* entry point for file system requests */
		  ULONG	 VRP_device_handle;      /* pointer to dragon handle for device */
		  ULONG	 VRP_partition_offset;   /* partition bias for logical volumes */
		  ULONG	 VRP_next_vrp;				 /* link to next VRP on global chain */
		  ULONG	 VRP_logical_handle;		 /* pointer to dragon logical device handle */
		  ULONG	 VRP_reserved;				 /* reserved - MBZ                   */

} VRP, *PVRP;


/*
** values for VRP_demand_flags. note that these flags must match the
** dcb flags. references to the dcb are avoided so that fsd writers
** do not have to include dcb.h and all that it may drag in
*/

#define VRP_dmd_small_memory    0x0010  /* data buffers must reside in      */
                                        /* in low 16M                       */

#define VRP_dmd_word_align      0x0200  /* data buffers must be word        */
                                        /* aligned.                         */

#define VRP_dmd_dword_align     0x0400  /* data buffers must be double word */
                                        /* aligned.                         */

#define VRP_dmd_phys_sgd        0x0800  /* scatter/gather descriptors must  */
                                        /* contain physical addresses (they */
                                        /* will contain linear addresses if */
                                        /* this demand bit is not set).     */

#define VRP_dmd_phys_sgd_ptr    0x1000  /* the pointer to the first         */
                                        /* scatter/gather descriptor        */
                                        /* (IOR_sgd_lin_phys) must be a     */
                                        /* physical address (it will be a   */
                                        /* linear addresses if this demand  */
                                        /* bit is not set).                 */

/* If foll. bit set, single floppy drive system -- voltrack needs to do
 * the toggling between A: and B: when accesses to made to these drive
 * letters.
 */
#define VRP_dmd_do_a_b_toggling 0x2000  /* Floppy A: B: toggle enable       */
#define VRP_dmd_removable_supp  0x4000  /* indicates device is removable    */

/* media supports software locking */

#define VRP_dmd_lock_unlock_media 0x00010000  

/* media supports electronic eject */

#define VRP_dmd_load_eject_media 0x00020000  

/* media supports command to clear media change */

#define VRP_dmd_clear_media_chg 0x00040000

/* note: flags 80000, and 100000h are reserved */
/* media allows absolute read/writes without an exclusive lock */

#define VRP_dmd_no_xcl_required 0x00200000

/* indicates that access to this volume can cause paging */

#define VRP_dmd_pageability	0x00400000  

/*
** values for VRP_event_flags
*/

#define VRP_ef_media_changed    0x0001  /* the media containing this volume */
                                        /* indicated change since most recent*/
													 /* i/o operation.                   */
#define VRP_ef_media_uncertain  0x0002  /* the media containing this volume */
                                        /* may have been changed since the  */
                                        /* most recent i/o operation.       */
#define VRP_ef_prompting        0x0004  /* the media containing this volume */
                                        /* is currently being prompted for  */
#define VRP_ef_input_share      0x0008  /* the input stream of a char dev   */
                                        /* is sharable                      */
#define VRP_ef_output_share     0x0010  /* the output stream of a char dev  */
                                        /* is sharable                      */
#define VRP_ef_user_canceled    0x0020  /* the user has hit CANCEL on a     */
                                        /* request for reinserting this vol */
#define VRP_ef_write_protected  0x0040  /* this volume is currently         */
                                        /* write protected.						 */
#define VRP_ef_real_mode_mapped 0x0080  /* this volume is currently accessed*/
                                        /* via the real mode mapper. 	    */
#define VRP_ef_ios_locked		0x8000  /* volume has it's device locked    */

