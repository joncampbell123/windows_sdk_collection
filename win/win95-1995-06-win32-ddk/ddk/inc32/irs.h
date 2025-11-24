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
* IRS (IOS Requestor Services) Data Structure
*
\****************************************************************************/

#ifndef	_IRS_H
#define	_IRS_H

/*
** define the ios service function code
*/

#define IRS_GET_VRP         	0  /* get volume request parms         */
#define IRS_REPLACE_VRP     	1  /* replace volume request parms     */
#define IRS_GET_DRVINFO     	2  /* get information about drive      */
#define IRS_MEDIA_CHECK_RESET	3  /* check media and reset changeline */
#define IRS_SET_CUR_FLP_UNIT	4  /* set the current floppy unit on   */
										   /* single floppy systems.	       */
#define IRS_REAL_MODE_HANDOFF	5  /* about to take over notification  */
#define IRS_QUERY_VOLUME_LOCK	6  /* notify that vol. is to be locked */
#define IRS_GET_LOGICAL_DRVS	7  /* get log. vols on an int13 unit   */
#define IRS_MOUNT_NOTIFY		8  /* just mounted a drive */
#define IRS_FIRSTNXT_CHAR_DEV	9	/* gets the name of a char device	*/
#define IRS_IS_DRVCOMPRESSED  10	/* rets a host drive if drive is compressed */

typedef struct IRS_header { /* */

        UCHAR   IRS_func;               /* Function Code                    */
        UCHAR   IRS_result;             /* result: zero = no error          */

} IRS, *PIRS;

/* generice IRS result codes */

#define	IRS_SUCCESS		0x00		/* indicates success for irs_result */

/*
** define the ios service packet for the get vrp function
*/

/*
** IRS_GET_VRP definition:
**
**	A dragon client (usually a file system (FSD)) calls this function to allocate
**	a new VRP.  (see vrp.h for VRP definitions) A successful return from this call
**	also sets this VRP as the current VRP for a given device.  The VRP represents
**	a volume on a given device.  In the case of a removable device, such as
**	a floppy diskette, a new VRP is created for every new diskette that is
**	inserted in the drive. Volume tracking uses the VRP to determine if the
**	correct media is in the drive.
**
**	The VRP also maintains information on whether the media specified by the VRP
**	is the current media in the drive.   Each file system should check the
**	VRP_event_flags to check if the media is changed or uncertain, prior to
**	relying on any cache'd data.
**
*/

typedef struct IRS_vrp_get { /* */

	UCHAR	filler1;						/* function to be performed			*/
	UCHAR	filler2;						/* result: zero = no error				*/
	UCHAR	IRS_v_g_dev_type;			/* see equates below 					*/
	ULONG	IRS_v_g_designtr;			/* volume ID or ptr to dev name 		*/
	ULONG	IRS_v_g_handle;			/* Volume handle: ptr to struct defined in vifsd.h */
	ULONG	IRS_v_g_vrp;				/* return value: ptr to VRP			*/
	UCHAR	IRS_alignchar1;

} IRS_vrp_get, *PIRS_vrp_get;


/*
**	define equates for IRS_v_g_dev_type
**
*/

#define	IRS_V_G_CHAR	0x01	/* indicates that this is a get for a char device */
										/* name specifed in v_g_designtr. */

/*
** define the result codes for the get vrp function- note that these are
** numerically equivalent to their IOR counterparts.
*/

#define IRS_V_G_SUCCESS         IRS_SUCCESS	
#define IRS_V_G_NO_DEVICE       0xA0    /* indicates no device present      */
#define IRS_V_G_UNCERTAIN_MEDIA 0xA2    /* indicates uncertain media        */

/*
**	IRS_REPLACE_VRP definition:
**
**	The IOS client (file system) uses this function when mounting or verifying
**	media.  The call allows the file system to control which VRP is current
**
**	IRS_v_r_old_vrp field = 0, and irs_v_r_new_vrp = 0:
**		In this case, IOS will clear the uncertain status on the current VRP for
**		the drive specified in IRS_v_r_designtr, as long as the IRS_V_R_DEF flag
**		is not set.
**
**	IRS_v_r_old_vrp is non zero, and IRS_v_r_new_vrp = 0:
**		In this case, IOS deallocates the VRP pointed to by old_vrp and also
**		notifies all IOS drivers of the destruction of the VRP by issuing a
**		AEP_DESTROY_VRP AEP.
**
**	IRS_v_r_old_vrp is non zero, and IRS_v_r_new_vrp is non zero
**		In this case, IOS deallocates the old vrp and sets the new_vrp field as
**		the current VRP. This should be used when a verify has indicated that
**		the media in the drive is an existing volume.
**
**	IRS_v_r_old_vrp = 0, and IRS_v_r_new_vrp is non zero
**		In this case, IOS is in the middle of mounting a new volume.  The new vrp
**		must be the vrp most recently made current for the given volume, using
**		the IRS_GET_VRP call.  In this case, the uncertain media status is cleared
**		and a AEP_CREAT_VRP broadcast is issued, indicating the arrival of a new
**		VRP in the system
*/

typedef struct IRS_vrp_replace { /* */

	UCHAR	filler3;				/* function to be performed			 	*/
	UCHAR	filler4;				/* result: zero = no error 				*/
	UCHAR	IRS_v_r_designtr;	/* pointer to VRP to deallocate 			*/
	UCHAR	IRS_v_r_flags;		/* flags to indicate VDEF mounting 		*/
	ULONG	IRS_v_r_old_vrp;	/* pointer to VRP to deallocate 			*/
	ULONG	IRS_v_r_new_vrp;	/* pointer to new VRP or zero if none 	*/

} IRS_vrp_replace, *PIRS_vrp_replace;

/*
** IRS_v_r_flags defines
*/

#define IRS_V_R_CHAR		IRS_V_G_CHAR
#define IRS_V_R_DEF		0x02

/*
**	IRS_GET_DRVINFO: definition
**
**	Returns information about the requested "drive letter".
** Any combination of the following flags is possible:
**	IRS_DRV_SINGLE_FLOPPY - indicates a floppy device that performs drive letter
**									toggling.  I.e. the same physical device responds to
**									both drive letters a: and b:
**
**	IRS_DRV_RMM - 	indicates that the requested drive uses the real mode mapper
**						for logical disk access
**
**	IRS_DRV_REMOTE -	indicates a remote (non local) drive
**
**	IRS_DRV_REMOVABLE -	indicates a device that uses removable media
**
**	IRS_DRV_INT13_DRIVE	-	indicates a device that can be accessed using the
**								-	int13 interface specification
** 
** IRS_DRV_PAGEABLE	- indicates that the device drivers controlling this drive
**					 	     have pageable code in them.
**
**	The input drive letter can be a hex number in the 80h - 8fh range for int13
**	devices or an upper or lowercase letter in the range of a-z
**
**	If the drive letter requested is not a valid IOS device, then the call will
**	return with a status of 1
**
*/

typedef struct IRS_drv_get { /* */

	UCHAR	IRS_filler_5;	/* function to be performed 	*/
	UCHAR	IRS_filler_6;	/* result: zero = no error		*/
	UCHAR	IRS_DrvLetter;	/* drive letter 					*/
	ULONG	IRS_DrvFlags;	/* flags							*/
	UCHAR	IRS_alignchar2;

} IRS_drv, *PIRS_drv;

/*
** define the IRS_DrvFlags codes for the get drv info function- note that
** these are numerically equivalent to their DCB counterparts., except for
** IRS_DRV_ON_INT13_DRIVE, which is not a DCB flag (see definitions above)
*/

#define IRS_DRV_SINGLE_FLOPPY		0x00800000
#define IRS_DRV_PAGEABLE			0x00400000
#define IRS_DRV_RMM					0x00002000
#define IRS_DRV_REMOTE		 		0x00000008
#define IRS_DRV_REMOVABLE			0x00000004
#define IRS_DRV_INT13_DRIVE		0x00000001


/*
** IRS_MEDIA_CHECK_RESET: definiton
**
**	In response to this call, IOS issues an IOR_MEDIA_CHECK_RESET function to
**	the device specified by the IRS_m_c_r_designtr field (0 = a:, 1 = b; etc.)
**	This function will check if the media has changed since the last time the
**	VRP was used, and reset the status.
**
** On return, IRS_STATUS will be set as follows:
**		IRS_M_C_R_SUCCESS	   - media has not been changed
**		IRS_M_C_R_UNCERTAIN	- media might have changed.
*/

typedef struct IRS_media_chk_reset { /* */

	UCHAR	filler7;						/* function to be performed         */
	UCHAR	filler8;						/* result: zero = no error          */
	UCHAR	IRS_m_c_r_designtr;		/* volume designator for device     */
	ULONG	IRS_m_c_r_vrp;				/* vrp for device		    */
	UCHAR	IRS_alignchar3;

} IRS_media_chk_reset, *PIRS_media_chk_reset;


/*
** define the result codes for media check reset function- note that these are
** numerically equivalent to their IOR counterparts.
*/

#define IRS_M_C_R_SUCCESS       IRS_SUCCESS
#define IRS_M_C_R_UNCERTAIN_MEDIA 0xA2  /* indicates uncertain media        */


/*
** IRS_SET_CUR_FLP_UNIT definition:
**
**	If the floppy device for drive a: supports a_b toggling, this function will
**	set the current unit number for the device to a or b depending on the
**	IRS_s_c_f_unit field.
**
*/

typedef struct IRS_cur_flp_unit_set { /* */

	UCHAR	filler9;					/* function to be performed	*/
	UCHAR	filler10;				/* result: zero = no error		*/
	UCHAR	IRS_s_c_f_unit;		/* flp unit # 0 = A, 1 = B		*/
	UCHAR	IRS_alignchar4;

} IRS_cur_flp_unit_set, *PIRS_cur_flp_unit_set;


/*
** IRS_REAL_MODE_HANDOFF: definition
**
**	This is an internal IFSMGR call, and should not be used. refer to aep.h for
**	a complete definition.  In response to this call, IOS issues the
**	AEP_REAL_MODE_HANDOFF function.  This function indicates to device driver
**	layers that this is the last opportunity to issue an INT 21 through DOS.
**
*/

typedef struct IRS_rm_handoff { /* */

	struct IRS_header	IRS_r_m_h_hdr;
	UCHAR 				IRS_r_m_h_pad[2];	/* Pad structure to DWORD boundary */
	UCHAR  IRS_alignchar5;

} IRS_rm_handoff, *PIRS_rm_handoff;

/*
** IRS_QUERY_VOLUME_LOCK: definiton
**
** This function returns a bitmap of logical drives that are associated with
** a drive that is about to be locked.
**
** bit 0 (lsb) of the dword corresponds to drive a:
** bit 1 corresponds to drive b: etc.
**
** An example of this function is a drive with a compressed volume file on it.
** If drive d: is a compression based drive with drive c: as the host, then
** a query_vollock on drive d: will return a bit mask of 0h, indicating that 
** no other drives will be changed, while a query_vollock on drive c: will
** return a bit mask of 4h indicating that drive d: will also be locked with
** a lock call on drive c:
**
*/


typedef struct IRS_query_vollock { /* */

	struct IRS_header	IRS_q_v_l_hdr;
	UCHAR		  			IRS_q_v_l_designtr; 	/* volume designator for device     */
	ULONG	 				IRS_q_v_l_drivemap;	/* Bitmap of logical drives 			*/ 
	UCHAR  				IRS_q_v_l_pad;			/* Pad structure to DWORD boundary 	*/

} IRS_query_vollock, *PIRS_query_vollock;


/*
** IRS_GET_LOGICAL_DRVS: definition
**
** This routine returns all logical drive letters associated with the given
** int 13 unit
**
*/

typedef struct IRS_get_logical_drives { /* */

	struct IRS_header	IRS_g_l_d_hdr;
	UCHAR					IRS_g_l_d_designtr; 	/* int 13 unit designator for device*/
	ULONG	 				IRS_g_l_d_drivemap;	/* Bitmap of logical drives 			*/ 
	UCHAR  				IRS_g_l_d_pad;			/* Pad structure to DWORD boundary 	*/

} IRS_get_logical_drives, *PIRS_get_logical_drives;


/*
** IRS_MOUNT_NOTIFY: definiton
**
** This routine issues an AEP_MOUNT_NOTIFY call to all IOS device drivers to
** indicate that the given drive and VRP are being mounted.  The device layers
** will return a bitmap off all child volumes that are affected.  See
** AEP_MOUNT_NOTIFY in aep.h for a more detailed discussion.
**
*/

typedef struct IRS_mnt_notify { /* */

	struct IRS_header	IRS_m_n_hdr;
	USHORT 				IRS_m_n_drive;		/* Drive number (0 = A:, ...) */
	void 				*IRS_m_n_pvrp;		/* VRP pointer 					*/
	ULONG 				IRS_m_n_drivemap;	/* Bitmap of child volumes 	*/
	ULONG				IRS_m_n_effective_drive;
											/* Effective drive number */
	ULONG				IRS_m_n_actual_drive;
											/* Actual drive number */

} IRS_mnt_notify, *PIRS_mnt_notify;

/*
** define the result codes for mount notify
*/

#define IRS_M_N_SUCCESS	IRS_SUCCESS	/* Nothing to do */
#define IRS_M_N_REMOUNT	(IRS_SUCCESS + 1)
					/* Remount the drive */

/*
** IRS_FIRSTNXT_CHAR_DEV: definition
** 
** This functions finds names for character device drivers controlled by IOS.
** IRS_g_f_n_hndl should be set to 0 to find the first character driver, then
** all subsequent calls to find the next driver should use the IRS_g_f_n_hndl
** returned by the previous call, to locate the next driver.
*/

typedef struct IRS_frstnxt_char_dev { /* */

	struct	IRS_header IRS_g_f_n_hdr;
	ULONG		IRS_g_f_n_hndl;		/* handle for get next, 0 for get first */
	ULONG		IRS_g_f_n_ptr;			/* pointer to 12 byte name buffer */
	USHORT	IRS_alignshort1;

} IRS_frstnxt_char_dev, *PIRS_frstnxt_char_dev;


/*
** IRS_IS_DRVCOMPRESSED: definition
**
** Given a logical drive number this function attempts to determine if the
** is compressed. If it is compressed the flag IRS_I_D_C_COMP is set and 
** the IRS_i_d_c_drive field is filled in with the host drive. Note that in
** in some cases IOS may not know that it is a compressed drive.
*/

typedef struct IRS_drv_comp { /* */

	struct	IRS_header IRS_i_d_c_hdr;
	UCHAR 	IRS_i_d_c_drive;		/* Drive number (0 = A:, ...) */
	UCHAR		IRS_i_d_c_flags;		/* see definitions below */

} IRS_drv_comp, *PIRS_drv_comp;

#define IRS_I_D_C_COMP		0x01		/* indicates that drive is compressed */
#define IRS_I_D_C_PM_COMP	0x02		/* indicates that drive is compressed */
												/* via protect mode. */

#endif	// _IRS_H
