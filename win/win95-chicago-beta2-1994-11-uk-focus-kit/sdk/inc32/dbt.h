/*****************************************************************************
 *
 *  (C) Copyright MICROSOFT Corp., 1993-1995
 *
 *  Title:      DBT.H - Equates for WM_DEVICECHANGE and BroadcastSystemMessage
 *
 *  Version:    4.00
 *
 *  Date:       24-May-1993
 *
 *  Author:     rjc
 *
 *----------------------------------------------------------------------------
 *
 *  Change log:
 *
 *     DATE     REV                 DESCRIPTION
 *  ----------- --- ----------------------------------------------------------
 *  24-May-1993 rjc Original
 *
 *****************************************************************************/

#ifndef _DBT_H
#define	_DBT_H

/*
 * BroadcastSpecialMessage constants.
 */
#define WM_DEVICECHANGE		0x0219

/* XLATOFF */
#ifdef	IS_32
#define	DBTFAR
#else
#define	DBTFAR	far
#endif
/* XLATON */

/*
 * Broadcast message and receipient flags.
 *
 * Note that there is a third "flag". If the message has:
 *
 * bit 15 on:	lparam is a pointer and bit 14 is meaningfull.
 * bit 15 off:	lparam is just a UNLONG data type.
 *
 * bit 14 on:	lparam is a pointer to an ASCIIZ string.
 * bit 14 off:	lparam is a pointer to a binary struture starting with
 *		a dword describing the length of the structure.
 */
#define BSF_QUERY		0x00000001
#define BSF_IGNORECURRENTTASK	0x00000002	/* Meaningless for VxDs */
#define BSF_FLUSHDISK		0x00000004	/* Shouldn't be used by VxDs */
#define BSF_NOHANG              0x00000008
#define BSF_POSTMESSAGE		0x00000010
#define BSF_FORCEIFHUNG         0x00000020
#define BSF_NOTIMEOUTIFNOTHUNG  0x00000040
#define	BSF_MSGSRV32ISOK	0x80000000	/* Called synchronously from PM API */
#define	BSF_MSGSRV32ISOK_BIT	31		/* Called synchronously from PM API */

#define BSM_ALLCOMPONENTS	0x00000000
#define BSM_VXDS                0x00000001
#define BSM_NETDRIVER           0x00000002
#define BSM_INSTALLABLEDRIVERS  0x00000004
#define BSM_APPLICATIONS        0x00000008

/*
 * Message = WM_DEVICECHANGE
 * wParam  = DBT_APPYBEGIN
 * lParam  = (not used)
 *
 *	'Appy-time is now available.  This message is itself sent
 *	at 'Appy-time.
 *
 * Message = WM_DEVICECHANGE
 * wParam  = DBT_APPYEND
 * lParam  = (not used)
 *
 *	'Appy-time is no longer available.  This message is *NOT* sent
 *	at 'Appy-time.  (It cannot be, because 'Appy-time is gone.)
 *
 * NOTE!  It is possible for DBT_APPYBEGIN and DBT_APPYEND to be sent
 * multiple times during a single Windows session.  Each appearance of
 * 'Appy-time is bracketed by these two messages, but 'Appy-time may
 * momentarily become unavailable during otherwise normal Windows
 * processing.  The current status of 'Appy-time availability can always
 * be obtained from a call to _SHELL_QueryAppyTimeAvailable.
 */
#define DBT_APPYBEGIN			0x0000
#define DBT_APPYEND			0x0001

/*
 * Message = WM_DEVICECHANGE
 * wParam  = DBT_DEVNODES_CHANGED
 * lParam  = 0
 *
 *	send when configmg finished a process tree batch. Some devnodes
 *	may have been added or removed. This is used by ring3 people which
 *	need to be refreshed whenever any devnode changed occur (like
 *	device manager). People specific to certain devices should use
 *	DBT_DEVICE* instead.
 */

#define DBT_DEVNODES_CHANGED            0x0007

/*
 * Message = WM_DEVICECHANGE
 * wParam  = DBT_QUERYCHANGECONFIG
 * lParam  = 0
 *
 *      sent to ask if a config change is allowed
 */

#define DBT_QUERYCHANGECONFIG           0x0017

/*
 * Message = WM_DEVICECHANGE
 * wParam  = DBT_CONFIGCHANGED
 * lParam  = new config number in low word, old config number in high word
 *	     (convert to %04X to get subkey in HKLM)
 *
 *      sent when a config has changed
 */

#define DBT_CONFIGCHANGED               0x0018

/*
 * Message = WM_DEVICECHANGE
 * wParam  = DBT_CONFIGCHANGECANCELED
 * lParam  = 0
 *
 *      someone cancelled the config change
 */

#define DBT_CONFIGCHANGECANCELED        0x0019

/*
 * Message = WM_DEVICECHANGE
 * wParam  = DBT_MONITORCHANGE
 * lParam  = new resolution to use (LOWORD=x, HIWORD=y)
 *           if 0, use the default res for current config
 *
 *      this message is sent when the display monitor has changed
 *      and the system should change the display mode to match it.
 */

#define DBT_MONITORCHANGE               0x001B

/*
 * Message = WM_DEVICECHANGE
 * wParam  = DBT_SHELLLOGGEDON
 * lParam  = 0
 *
 *	The shell has finished login on: VxD can now do Shell_EXEC.
 */

#define	DBT_SHELLLOGGEDON		0x0020

/*
 * Message = WM_DEVICECHANGE
 * wParam  = DBT_CONFIGMGAPI
 * lParam  = CONFIGMG API Packet
 *
 *	CONFIGMG ring 3 call.
 */
#define	DBT_CONFIGMGAPI32		0x0022

/*
 * Message = WM_DEVICECHANGE
 * wParam  = DBT_APMPOWERSTATUSCHANGE
 * lParam  = power status packet
 *
 *	Messages issued by VPOWERD.
 */
#define DBT_APMBATTERYLOW               0x0039
#define DBT_APMPOWERSTATUSCHANGE	0x003A
#define DBT_APMUPDATETIME               0x003B
#define	DBT_POWERSETSYSTEMSTATE		0x0040

/*
 * Message = WM_DEVICECHANGE
 * wParam  = DBT_VOLLOCK*
 * lParam  = pointer to VolLockBroadcast structure described below
 *
 *	Messages issued by IFSMGR for volume locking purposes on WM_DEVICECHANGE.
 *	All these messages pass a pointer to a struct which has no pointers.
 */

#define DBT_VOLLOCKQUERYLOCK	0x8041
#define DBT_VOLLOCKLOCKTAKEN	0x8042
#define DBT_VOLLOCKLOCKFAILED	0x8043
#define DBT_VOLLOCKQUERYUNLOCK	0x8044
#define DBT_VOLLOCKLOCKRELEASED	0x8045
#define DBT_VOLLOCKUNLOCKFAILED	0x8046

/*
 * Device broadcast header
 */
 
struct _DEV_BROADCAST_HDR {	/* */
    ULONG	dbch_size;
    ULONG	dbch_devicetype;
    ULONG       dbch_reserved;
};

typedef struct 	_DEV_BROADCAST_HDR	DEV_BROADCAST_HDR;
typedef		DEV_BROADCAST_HDR	DBTFAR *PDEV_BROADCAST_HDR;

/*
 * Structure for volume lock broadcast
 */
 
typedef struct VolLockBroadcast VolLockBroadcast;
typedef VolLockBroadcast *pVolLockBroadcast;
struct VolLockBroadcast {
	struct	_DEV_BROADCAST_HDR vlb_dbh;
	ULONG	vlb_owner;		// thread on which lock request is being issued
	BYTE	vlb_perms;		// lock permission flags defined below
	BYTE	vlb_lockType;	// type of lock
	BYTE	vlb_drive;		// drive on which lock is issued
	BYTE	vlb_flags;		// miscellaneous flags
};

/*
 * Values for vlb_perms
 */
#define LOCKP_ALLOW_WRITES		0x01	// Bit 0 set - allow writes
#define LOCKP_FAIL_WRITES		0x00	// Bit 0 clear - fail writes
#define LOCKP_FAIL_MEM_MAPPING	0x02	// Bit 1 set - fail memory mappings
#define LOCKP_ALLOW_MEM_MAPPING	0x00	// Bit 1 clear - allow memory mappings
#define LOCKP_USER_MASK			0x03	// Mask for user lock flags
#define LOCKP_LOCK_FOR_FORMAT	0x04	// Level 0 lock for format

/*
 * Values for vlb_flags
 */
#define LOCKF_LOGICAL_LOCK		0x00	// Bit 0 clear - logical lock
#define LOCKF_PHYSICAL_LOCK		0x01	// Bit 0 set - physical lock


#define DBT_CONFIGMGPRIVATE	0x7FFF

/*
 * The following messages are for WM_DEVICECHANGE. The immediate list
 * is for the wParam. ALL THESE MESSAGES PASS A POINTER TO A STRUCT
 * STARTING WITH A DWORD SIZE AND HAVING NO POINTER IN THE STRUCT.
 *
 */
#define DBT_DEVICEARRIVAL		0x8000	// system detected a new device
#define DBT_DEVICEQUERYREMOVE		0x8001	// wants to remove, may fail
#define DBT_DEVICEQUERYREMOVEFAILED	0x8002  // removal aborted         
#define DBT_DEVICEREMOVEPENDING		0x8003  // about to remove, still avail.
#define DBT_DEVICEREMOVECOMPLETE	0x8004  // device is gone
#define DBT_DEVICETYPESPECIFIC		0x8005  // type specific event

#define DBT_DEVTYP_OEM			0x00000000  // oem-defined device type
#define DBT_DEVTYP_DEVNODE		0x00000001  // devnode number
#define DBT_DEVTYP_VOLUME		0x00000002  // logical volume
#define DBT_DEVTYP_PORT			0x00000003  // serial, parallel
#define DBT_DEVTYP_NET			0x00000004  // network resource

struct _DEV_BROADCAST_OEM {	/* */
    ULONG	dbco_size;
    ULONG	dbco_devicetype;
    ULONG       dbco_reserved;
    ULONG	dbco_identifier;
    ULONG	dbco_suppfunc;
};

typedef struct 	_DEV_BROADCAST_OEM	DEV_BROADCAST_OEM;
typedef		DEV_BROADCAST_OEM	DBTFAR *PDEV_BROADCAST_OEM;

struct _DEV_BROADCAST_DEVNODE { /* */
    ULONG	dbcd_size;
    ULONG	dbcd_devicetype;
    ULONG       dbcd_reserved;
    ULONG	dbcd_devnode;
};

typedef struct 	_DEV_BROADCAST_DEVNODE	DEV_BROADCAST_DEVNODE;
typedef		DEV_BROADCAST_DEVNODE	DBTFAR *PDEV_BROADCAST_DEVNODE;

struct _DEV_BROADCAST_VOLUME { /* */
    ULONG	dbcv_size;
    ULONG	dbcv_devicetype;
    ULONG       dbcv_reserved;
    ULONG	dbcv_unitmask;
    USHORT	dbcv_flags;
};

typedef struct 	_DEV_BROADCAST_VOLUME	DEV_BROADCAST_VOLUME;
typedef		DEV_BROADCAST_VOLUME	DBTFAR *PDEV_BROADCAST_VOLUME;

#define DBTF_MEDIA	0x0001		// media comings and goings
#define DBTF_NET	0x0002		// network volume

struct _DEV_BROADCAST_PORT { /* */
    ULONG	dbcp_size;
    ULONG	dbcp_devicetype;
    ULONG       dbcp_reserved;
    char	dbcp_name[1];
};

typedef struct 	_DEV_BROADCAST_PORT	DEV_BROADCAST_PORT;
typedef		DEV_BROADCAST_PORT	DBTFAR *PDEV_BROADCAST_PORT;

struct _DEV_BROADCAST_NET { /* */
    ULONG	dbcn_size;
    ULONG	dbcn_devicetype;
    ULONG       dbcn_reserved;
    ULONG	dbcn_resource;
    ULONG	dbcn_flags;
};

typedef struct 	_DEV_BROADCAST_NET	DEV_BROADCAST_NET;
typedef		DEV_BROADCAST_NET	DBTFAR *PDEV_BROADCAST_NET;

#define DBTF_RESOURCE	0x00000001	// network resource
#define DBTF_XPORT	0x00000002	// new transport coming or going
					// (dbcn_resource undefined for now)

#define	DBT_VPOWERDAPI	0x8100		// VPOWERD API for Chicago

#endif	// _DBT_H
