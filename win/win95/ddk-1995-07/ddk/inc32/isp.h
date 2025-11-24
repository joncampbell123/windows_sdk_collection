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
*	ISP (IOS Services Packet) Data Structure
*
\****************************************************************************/

/*
** define the ios service function codes
*/

#define ISP_CREATE_DDB		    0	/* create ddb */
#define ISP_CREATE_DCB		    1	/* create dcb (Physical) */
#define ISP_CREATE_IOP		    2	/* create iop */
#define ISP_ALLOC_MEM		    3	/* allocate memory */
#define ISP_DEALLOC_MEM		    4	/* deallocate memory */
#define ISP_INSERT_CALLDOWN	    5   /* insert entry in calldown table */
#define ISP_ASSOCIATE_DCB	    6	/* associate dcb & relative drive # */
#define ISP_GET_DCB		    7	/* return DCB for logical volume */
#define ISP_GET_FIRST_NEXT_DCB	    8   /* get first/next DCB */
#define ISP_DEALLOC_DDB		    9	/* deallocate DDB */
#define ISP_DESTROY_DCB		    10	/* destroy DCB */
#define ISP_QUERY_MATCHING_DCBS	    11  /* ask for logical-to-physical info */
#define ISP_QUERY_REMOVE_DCB	    12	/* ask if we can remove a device */
#define ISP_DEVICE_REMOVED	    13	/* indicate that a device has been removed */
#define ISP_DEVICE_ARRIVED	    14	/* indicate that a device has arrived */
#define ISP_DISASSOCIATE_DCB	    15	/* disassociate dcb & drive # */
#define ISP_DRIVE_LETTER_PICK	    16	/* pick a drive letter */
#define ISP_REGISTRY_READ	    17	/* read registry */
#define	ISP_FIND_LDM_ENTRY	    18	/* find LDM entry */
#define	ISP_DELETE_LDM_ENTRY	    19	/* delete LDM entry */
#define	ISP_BROADCAST_AEP		20	/* broadcast an AEP from a driver */

/*
** define the basic ios service packet
*/
typedef struct ISP { /* */

    USHORT ISP_func;		    /* function to be performed */
    USHORT ISP_result;		/* result: zero = no error */

} ISP, *PISP;

/*
** define the ios service packet for the create ddb function:
**
** This is typically called by a port driver to create a per controller
** data structure at AER_INITIALIZE time.
*/
typedef struct ISP_ddb_create { /* */

    ISP		ISP_ddb_hdr;	    /* Standard header */
    USHORT  ISP_ddb_size;	/* size of ddb to create */
    ULONG	ISP_ddb_ptr;	    /* 32-bit offset to ddb */
    UCHAR	ISP_ddb_flags;	    /* flags - see definitions below */
    UCHAR	ISP_pad0[1];	    /* Pad to DWORD boundary */
} ISP_ddb_create, *PISP_ddb_create;

#define	ISP_DDB_FLAG_NEED_PHYS_CONTIG	0x01  /* DDB must be physically contiguous */
							    /* DDB_phys_addr valid only if set */

/*
** define the ios service packet for the create dcb function
*/
typedef struct ISP_dcb_create { /* */

    ISP		ISP_dcb_hdr;	    /* Standard header */
    USHORT  ISP_dcb_size;	/* size of dcb to create */
    ULONG	ISP_dcb_ptr;	    /* 32-bit offset to dcb */
    UCHAR	ISP_pad1[2];	    /* Pad to DWORD boundary */

} ISP_dcb_create, *PISP_dcb_create;


/*
** define the ios service packet for the create iop function
*/
typedef struct ISP_IOP_create { /* */

    ISP		ISP_i_c_hdr;	    /* Standard header */
    USHORT  ISP_IOP_size;	/* size of IOP to allocate */
    ULONG	ISP_delta_to_ior;   /* offset to IOR within IOP */
    ULONG	ISP_IOP_ptr;	    /* 32-bit offset to IOP */
    UCHAR	ISP_i_c_flags;	    /* allocation flags as defined below*/
    UCHAR	ISP_pad2[1];	    /* Pad to DWORD boundary */

} ISP_IOP_alloc, *PISP_IOP_alloc;

/*
** define the ios service packet for the allocate memory function
*/
typedef struct ISP_mem_alloc { /* */

    ISP		ISP_mem_a_hdr;	    /* Standard header */
    USHORT  ISP_mem_size;	/* byte size of memory to allocate */
    ULONG	ISP_mem_ptr;	    /* 32-bit offset to memory block */
    USHORT  ISP_mem_type;	/* med type code for memory block */
    UCHAR	ISP_mem_flags;	    /* allocation flags as defined below*/
    UCHAR	ISP_pad3[3];	    /* Pad to DWORD boundary */

} ISP_mem_alloc, *PISP_mem_alloc;

/*
** flags for use with ISP_iop_alloc and ISP_mem_alloc above
*/

#define	ISP_M_FL_INTERRUPT_TIME	0x01	/* must be set if alloc is at async time */
#define	ISP_M_FL_MUST_SUCCEED	0x02  /* ios will not fail the allocate if set. */
#define	ISP_M_FL_EXTERNAL_IOP	0x04  /* register the IOP instead of allocating it. */
#define	ISP_M_FL_SMART_ALLOC	    0x08  /* heap allocate if not DOS pager */
#define	ISP_M_FL_PERSISTENT_IOP  0x10  /* allocation is for a long time IOP */
#define	ISP_M_FL_USE_SYSTEM_HEAP 0x20  /* don't allocate from IOS heap */

/*
** define the ios service packet for the de-allocate memory function
*/
typedef struct ISP_mem_dealloc { /* */

    ISP		ISP_mem_da_hdr;	    /* Standard header */
    ULONG	ISP_mem_ptr_da;	    /* 32-bit offset to memry to dealloc*/

} ISP_mem_dealloc, *PISP_mem_dealloc;


/*
**  ISP_INSERT_CALLDOWN:
**
**  This service is called by a layer driver if it needs to insert its
** request entry point into the DCB call down list. This ensures that the
** driver will see all i/o requests to that device. The driver can also
** allocate a per i/o request packet area called expansion area. This becomes
** part of the IOP and can be accessed when the request is being processed
** by each layer.
*/

typedef struct ISP_calldown_insert { /* */

    ISP		ISP_i_cd_hdr;		/* Standard header */
    ULONG	ISP_i_cd_dcb;		/* 32-bit offset to dcb */
    PVOID	ISP_i_cd_req;		/* 32-bit pointer to request routine*/
    ULONG	ISP_i_cd_ddb;		/* 32-bit offset to ddb */
					    /* zero if none. */
    USHORT  ISP_i_cd_expan_len;	/* size of IOP expansion length */
					    /* zero if none. */
    ULONG	ISP_i_cd_flags;	    /* demand flag bits */
    UCHAR	ISP_i_cd_lgn;		/* load group number */
    UCHAR	ISP_pad4[1];		/* Pad to DWORD boundary */

} ISP_calldown_insert, *PISP_calldown_insert;


/*
** ISP_ASSOCIATE_DCB and relative drive number function:
**
** This service is generally called by a TSD. Note that the TSD is
** responsible for determining the logical drives associated with a physical.
** This service associates a logical drive letter with a logical DCB or an
** int13 drive number with a physical. In the case of logical association
** this service calls IFSMGR and sends out a WM_DEVICECHANGE
** DBT_DEVICEARRIVAL broadcast to notify the system of a volume arrival.
**
**  Value add drivers also may need to make this call. For instance, a
** compression layer would call this service as and when it determines
** a compressed volume that is associated with a host.
**
*/
typedef struct ISP_dcb_associate { /* */

    ISP		ISP_d_a_hdr;	/* Standard header */
    ULONG	ISP_d_a_dcb;	/* 32-bit offset of dcb */
    UCHAR	ISP_d_a_drive;	/* relative drive number*/
    UCHAR	ISP_d_a_flags;	/* flags */
    UCHAR	ISP_pad5[2];	/* Pad to DWORD boundary*/

} ISP_dcb_associate, *PISP_dcb_associate;

#define	ISP_D_A_FL_NOSHELLMSG	0x01	/* Suppress the shell message */


/*
** define the ios service packet for the get dcb function
*/
typedef struct ISP_dcb_get { /* */

    ISP		ISP_g_d_hdr;	/* Standard header */
    ULONG	ISP_g_d_dcb;	/* 32-bit offset of dcb */
    ULONG	ISP_g_d_drive;	/* relative drive number */
				    /* note: IORF_PHYS_CMD set for phys */
				    /* device requested */

} ISP_dcb_get, *PISP_get_dcb;


/*
** ISP_GET_FRST_NXT_DCB: the get first/next DCB function
**
** ISP_gfnd_dcb_offset is 0 if the first DCB is desired. else it is
** equal to what was returned in ISP_gfnd_found_dcb from a previous call
** to this service.
**
** ISP_gfnd_dcb_type specifies a particular device type. Device types are
** defined in DCB.H. If this is 0xff the search is not restricted to a
** a specific device type.
*/
typedef struct ISP_GET_FRST_NXT_DCB { /* */

    ISP		ISP_gfnd_hdr;		/* Standard header */
    ULONG	ISP_gfnd_dcb_offset;	/* 32-bit offset of DCB from which */
					    /* to get next or zero to get first.*/
    ULONG	ISP_gfnd_found_dcb; /* if result = zero: dcb found by */
					    /* IOS. undefined if result not zero*/
    BYTE	ISP_gfnd_dcb_type;  /* acceptable device type or 0ffh */
					    /* for any type. see DCB.H for */
					    /* valid device type codes. */
    UCHAR	ISP_pad7[3];		/* Pad to DWORD boundary */

} ISP_GET_FRST_NXT_DCB, *PISP_GET_FRST_NXT_DCB;


/*
** define the ios service packet for the deallocate ddb ISP
**
** This is typically called by a port driver when the controller associated
** with the DDB is removed. IOS sends out an AEP_UNINITIALIZE broadcast
** at this time and typically port drivers use this AEP call to dealloc any
** resources that they may have allocated to handle i/o to the controller
** represented by that DDB. For instance, IRQ can be freed, port trapping
** could be freed etc.
*/
typedef struct ISP_DDB_DEALLOC { /* */

    ISP		ISP_ddb_d_hdr;	    /* Standard header */
    ULONG	ISP_ddb_d_ddb;	    /* 32-bit offset of DDB to dealloc */
    ULONG	ISP_ddb_d_dvt;	    /* 32-bit offset of DVT */

} ISP_DDB_DEALLOC, *PISP_DDB_DEALLOC;


/*
** define the ios service packet for the destroy dcb ISP:
**
** This service is called when a device is removed. This service can be
** only be called at Appy Time. The foll. sequence of events are executed
** in this service:
**
**	. Broadcast an AEP_PEND_UNCONFIG_DCB to all the layer drivers
**	  Subsequently the drivers typically stop generating new requests for
**	  this device
**	. Block untill all pending i/o to this device is completed.
**	. Error any further i/o to this DCB.
**	. For all logicals associated with this physical the IFSMGR is notified
**	  of the volume removal. Also a WM_DEVICECHANGE
**	  DBT_DEVICEREMOVECOMPLETE shell broadcast is made.
**	. Broadcast an AEP_UNCONFIG_DCB to all the layer drivers.
**	  The drivers do any necessary processing at this point.
**	. All VRPs and calldown nodes associated with the DCB are freed.
**	. Finally the DCB itself is freed.
*/
typedef struct ISP_DCB_DESTROY { /* */

    ISP		ISP_ddb_d_hdr;	    /* Standard header */
    ULONG	ISP_dcb_d_dcb;	    /* 32-bit offset of DCB to destroy */

} ISP_DCB_DESTROY, *PISP_DCB_DESTROY;


/*
** ISP_QUERY_MATCH - query drivers for logical-to-physical DCB associations.
**
**  This service is intended for use by the TSD to find any relationships
**  it is unable to deduce for itself. For instance in order to associate
** compressed volumes to a particular physical DCB the DiskTSD makes this
** call out qurying all the layer drivers. This call results in an
** AEP_ASSOCIATE_DCB call to all the layer drivers. The TSD then associates
** the additional logical volumes to the physical iff the real mode mapper
** has not already claimed that logical volume. Note that RMM may claim
** a logical volume if there is an unsafe device driver hooking the real
** mode driver chain for that volume.
**
*/
typedef struct ISP_QUERY_MATCH { /* */

    ISP		ISP_q_match_hdr;	/* Standard header */
    PVOID   ISP_q_match_pdcb;	    /* Physical DCB pointer */
    ULONG   ISP_q_match_drives;	/* Bitmap of associated logical DCBs*/

} ISP_QUERY_MATCH, *PISP_QUERY_MATCH;


/*
** define the ios service packet for the query remove function:
**
**  This service issues a DBT_DEVICEQUERYREMOVE shell broadcast and also
** queries the IFSMGR by calling the IFSMGR service QueryVolumeRemoval.
** This call can be failed by any application or the IFSMGR if they believe
** that this volume cannot be removed.
**
** NOTE: This is an Appy time only service!
*/
typedef struct ISP_QUERY_REMOVE { /* */

    ISP	    ISP_q_remove_hdr;	    /* standard header */
    PVOID ISP_q_remove_pdcb;	/* physical DCB pointer */
    ULONG ISP_q_remove_flags;	/* flags as defined below */

} ISP_QUERY_REMOVE, *PISP_QUERY_REMOVE;

/*
** flags for ISP_QUERY_REMOVE function
*/

#define ISP_Q_REMOVE_FL_MEDIA_ONLY 1  /* indicates only the medium is going away */


/*
** define the ios service packet for the device removed function.
**
**  This service issues a DBT_DEVICEREMOVECOMPLETE shell broadcast for all the
** logical volumes associated with the physical DCB. This is typically
** called if the media is ejected from the drive in which case
** ISP_DEV_REMOVED_FL_MEDIA_ONLY should be set.
**
** NOTE: This is an Appy time only service!
*/
typedef struct ISP_DEV_REMOVED { /* */

    ISP	    ISP_d_removed_hdr;	    /* standard header */
    PVOID ISP_dev_removed_pdcb;	/* physical DCB pointer */
    ULONG ISP_dev_removed_flags;    /* flags as defined below */

} ISP_DEV_REMOVED, *PISP_DEV_REMOVED;

#define ISP_DEV_REMOVED_FL_MEDIA_ONLY 1  /* indicates only the medium is going away */


/*
** define the ios service packet for the device arrived function.
**
**  This service issues a DBT_DEVICEARRIVAL shell broadcast for all the
** logical volumes associated with the physical DCB. This is typically
** called if the media is inserted in the drive in which case
** ISP_DEV_REMOVED_FL_MEDIA_ONLY should be set.
**
** NOTE: This is an Appy time only service!
*/
typedef struct ISP_DEV_ARRIVED { /* */

    ISP	    ISP_d_arrived_hdr;	    /* standard header */
    PVOID ISP_dev_arrived_pdcb;	/* physical DCB pointer */
    ULONG ISP_dev_arrived_flags;    /* flags as defined below */

} ISP_DEV_ARRIVED, *PISP_DEV_ARRIVED;

#define ISP_DEV_ARR_FL_MEDIA_ONLY 1  /* indicates only the medium is going away */


/*
** define the ios service packet for the disassociate DCB ISP
**
** This service disassociates the logical drive number from the DCB that is
** currently associated to it. The IFSMGR is also Notified of a Volume
** Removal. This is typically called by a value add driver when the volumes
** that it associated via ISP_QUERY_MATCH are going away.
*/
typedef struct ISP_DISASSOC_DCB { /* */

    ISP	    ISP_dis_dcb_hdr;	    /* Standard header */
    ULONG ISP_dis_dcb_drive;	/* Drive of associated logical DCB */

} ISP_DISASSOC_DCB, *PISP_DISASSOC_DCB;


/*
** define the ios service packet for the pick drive letter ISP
**
** This service is called to provide a drive letter for a potential logical
** volume. This volume cannot be a volume that is visible in real mode, as
** the protect mode drive letter for that volume will match the real mode
** drive.
**
** The criteria for the search are specified in ISP_p_d_l_flags
**
** . If nothing is specifed, the UserDriveLetterAssignment Value in the
**   registry for this DCB is read and a drive letter that falls in this
**    range is provided. If there is no registry entry the next available
**   drive letter is picked.
**
**  . If the ISP_PDL_FL_USE_RANGE flag is set, the caller provides a search
**    range in ISP_p_d_l_letter[0] = start letter & ISP_p_d_l_letter[1] =
**    end letter. The registry is read and a drive letter in the range that
**    is an intersection of the registry range and the user range is picked.
**    If all the letters in this range are used the next available drive
**    letter is picked unless the ISP_PDL_FL_USER_ONLY flag is set in which
**    case the call is failed.
**
**  In the case where a registry range is specified, when scanning for
** drive letters in this range if a drive letter is already used the
**  foll. two flags provide additional directions to the service:
**
**  . ISP_PDL_FL_OK_RM_CD. This specifies that it is OK to pick the already
**    used drive letter if it was a real mode CD drive.
**
**  . ISP_PDL_FL_OK_INVALID_RM. This specifies that it is OK to pick the
**    already used drive letter if it is an invalid drive in real mode.
**
*/
typedef struct ISP_pick_drive_letter { /* */

    ISP	    ISP_p_d_l_hdr;		/* standard header */
    PVOID ISP_p_d_l_pdcb;	    /* physical DCB pointer */
    UCHAR ISP_p_d_l_letter[2];	  /* drive letter if no error */
    UCHAR ISP_p_d_l_flags;	    /* flags */
    UCHAR ISP_p_d_l_pad[1];	    /* dword alignment */

} ISP_pick_drive_letter, *PISP_pick_drive_letter;

/*
** flags for ISP_DRIVE_LETTER_PICK function
*/

#define ISP_PDL_FL_USER_ONLY	    0x01    /* return only user specified */
						    /* drive letter */
#define ISP_PDL_FL_OK_INVALID_RM 0x02	/* ok to pick user specified drive */
						    /* letter for real mode letter if */
						    /* invalid partition */

#define ISP_PDL_FL_OK_RM_CD	0x04	/* ok to pick user specified drive */
						    /* letter for real mode letter if */
						    /* CDROM */
#define	ISP_PDL_FL_USE_RANGE	    0x08    /* use caller-specified drive range */
#define	ISP_PDL_FL_RELEASE	0x10	/* release a picked drive letter */


/*
** define the ios service packet for the read registry ISP.
**
**  This service reads registry info corresponding with the physical DCB
**  provided.  Note that the parameter definitions are identical to those of
**  VMM service _RegQueryValueEX.
**
**  The foll. IOS specific values can be read for the DCB in question:
**
**	REGSTR_VAL_SCSITID	- SCSI target ID ASCIZ string
**	REGSTR_VAL_SCSILUN	- SCSI LUN ID ASCIZ string
**	REGSTR_VAL_REVLEVEL	    - Revision Level ASCIZ string
**	REGSTR_VAL_PRODUCTID	- product ID from inquiry data ASCIZ string
**	REGSTR_VAL_MANUFACTURER	- vendor ID from inquiry data.
**	REGSTR_VAL_DEVTYPE	- binary value. Device types are defined in dcb.h
**	REGSTR_VAL_REMOVABLE	- boolean. 1 if removable 0 if not.
**	REGSTR_VAL_CURDRVLET	- ASCIZ string. For eg. CDF - implies that C, D
**					  and F are currently assigned to this phys DCB.
**	REGSTR_VAL_USRDRVLET	- user drive range. ASCIZ string. For eg. DG -
**					  implies that the user specifed a drive range
**					  starting from D: and ending at G:. See
**					  ISP_pick_drive_letter for more info.
**	REGSTR_VAL_SYNCDATAXFER	- Boolean. SCSI only. 1 if syncdata enabled 0
**					  if not. Default is ON for Disks and OFF for
**					  CDROM.
**	REGSTR_VAL_AUTOINSNOTE	- Boolean. SCSI CD only. 1 if enabled 0 if not.
**					  Default is ON.
**	REGSTR_VAL_DISCONNECT	- Boolean. SCSI only. 1 if enabled 0 if not.
**					  Default is ON.
**	REGSTR_VAL_INT13	    - Boolean. 1 if int 13 unit 0 if not.
**
**
**  Note that other common registry values for devnodes like class name etc.
**  can also be read using this service for that DCB.
**
*/

typedef struct ISP_read_registry { /* */

    ISP	    ISP_r_r_hdr;		/* standard header */
    PVOID ISP_r_r_dcb;		    /* physical DCB pointer */
    PVOID ISP_r_r_string;	    /* pointer to null-terminated key name */
    PVOID ISP_r_r_data_type;	    /* type of data stored in buffer below */
    PVOID ISP_r_r_data_size;	    /* size of buffer below */
    PVOID ISP_r_r_buffer;	    /* pointer to buffer for result */

} ISP_read_registry, *PISP_read_registry;


/*
** define the ios service packet for the Find LDM Entry ISP
** and the Delete LDM Entry ISP
**
** This packet is used for the service that finds the LDM entry
** for the specified drive and for the service that deletes the
** LDM entry for the specified drive.  Deletion is accomplished
** by compacting the LDM table to copy over the deleted entry.
** Therefore, clients should not rely on a pointer to an LDM entry
** after yielding.
*/
typedef struct ISP_FIND_LDM { /* */

    ISP ISP_fldm_hdr;		    /* Standard header */
    ULONG ISP_fldm_drive;	    /* 0-based drive number */
    PVOID ISP_fldm_pldm;	    /* returned pointer to LDM entry */

} ISP_FIND_LDM, *PISP_FIND_LDM;

/*
** define the ios service packet for the Broadcast AEP ISP
**
** This packet is used for the service that broadcasts the given AEP to the
** entire driver architecture.  
*/
typedef struct ISP_AEP_BROADCAST { /* */

    ISP ISP_baep_hdr;		    /* Standard header */
    PVOID ISP_baep_paep;	    /* pointer to AEP to broadcast */

} ISP_AEP_BROADCAST, *PISP_AEP_BROADCAST;

