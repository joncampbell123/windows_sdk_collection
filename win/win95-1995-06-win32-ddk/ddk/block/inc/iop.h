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
* IOP (I/O Request Packet) Data Structure
*
\****************************************************************************/

/*
** call back table entry
*/

#define IOP_CALLBACK_TABLE_DEPTH 6

typedef struct IOP_callback_entry { /* */

	ULONG	IOP_CB_address; 	/* call back address		        */
	ULONG	IOP_CB_ref_data;	/* pointer to callback ref data */

} IOP_callback_entry;

/*
** i/o request packet - offset 0
*/
typedef struct _IOP { /* */

	ULONG	IOP_physical;		/* physical address of IOP.	    */
	ULONG	IOP_physical_dcb;	/* pointer to physical dcb.	    */
	ULONG	IOP_original_dcb;	/* ptr to dcb designated by ior.    */
/*
** offset 20 (hex)
*/
	USHORT	IOP_timer;		/* rcb current timeout value.	    */
	USHORT	IOP_timer_orig;		/* rcb original timeout value.	    */

	ULONG	IOP_calldown_ptr;	/* ptr to next calldown routine.    */
	ULONG	IOP_callback_ptr;	/* ptr to current callback address. */
/*
** offset 30 (hex)
*/
	ULONG	IOP_voltrk_private; /* for use by volume tracking.  BUGBUG BGP- I */
									  /* only need one byte of this, but to maintain */
                             /* alignment it's a dword.							*/

#ifdef	WIN31COMPAT

	ULONG	IOP_VM_Handle;			/* contains the handle of the VM in whose    */
										/* context this IO originated. This is inited*/
										/* in IOS_SendCommand.								*/

#else

	ULONG	IOP_Thread_Handle;	/* contains the handle of the thread in whose*/
										/* context this IO originated. This is inited*/
										/* in IOS_SendCommand.								*/
#endif

	ULONG	IOP_srb;				   /* used by SCSI'izers to pass SRB pointer to */
										/* next layer											*/

	ULONG	IOP_reserved[2];		/* reserved for future use - must be zero */

/*
** offset 44 (hex)
*/
	IOP_callback_entry IOP_callback_table[IOP_CALLBACK_TABLE_DEPTH];	       // RCB_CallBackTable
/*
** offset 74 (hex)
*/

	 BYTE   IOP_format_head;	/* fields for low level format */ 
	 BYTE   IOP_format_xfer_rate;	
	 USHORT IOP_format_track;
	 ULONG  IOP_format_num_sectors;

	IOR	IOP_ior;			/* i/o request descriptor	    */

} IOP, *pIOP;
