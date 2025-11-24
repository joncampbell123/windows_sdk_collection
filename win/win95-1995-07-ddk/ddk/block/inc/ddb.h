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
* DDB (Driver Data Block) Data Structure
*
\****************************************************************************/

typedef struct DDB { /* */

	ULONG	DDB_phys_addr;		/* Physical address of ddb	    */
	ULONG	DDB_Next_DDB;		/* next ddb on dvt_ddb chain	    */
	ULONG	DDB_Next_DDB_init;	/* next ddb on dvt_ddb_init chain   */
	ULONG	DDB_dcb_ptr;		/* pointer to first DCB owned by this DDB */
	UCHAR	DDB_number_buses; /* number of buses supported by the adapter */
	UCHAR DDB_ios_flags;    /* IOS private flags - read only for non-IOS */
	USHORT DDB_sig;			/* dword padding (also DDB signature for asserting) */
	PVOID	DDB_dvt;				/* pointer to this DDB's DVT */
	PVOID	DDB_devnode_ptr;  /* pointer to devnode for device */
	PVOID	DDB_reserved;  	/* reserved- MBZ */
} DDB , *pDDB;

/*
** flags definitions for ios_flags
*/

#define DDB_IOS_FL_PENDING_REMOVAL 1
#define DDB_IOS_FL_PAGE_ALLOCED 2
#define DDB_IOS_FL_FIRST_ENUM_DONE_BIT 2
#define DDB_IOS_FL_PLEASE_DESTROY 4
