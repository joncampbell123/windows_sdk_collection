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


/*****************************************************************************
 *
 *   Title:     IOS.H - Include file for Dragon I/O Supervisor
 *
 *   Version:   1.00
 *
 */

/**/
/* Define the IOS VxD device ID bugbug - this must be finalized */
/**/

// #define IOS_Device_ID  0x88
// shk BUGBUG need to assert that this is same as that for BLOCKDEV for
// compatibility!
#define IOS_Device_ID	0x00010

/**/
/* Define the IOS VxD version # */
/**/

#define IOS_Major_Ver 0x3
#define IOS_Minor_Ver 0xA

/**/
/* Define the IOS service table.  The first 7 services are supersets of */
/* BlockDev services																		*/
/**/

/* ASM

Begin_Service_Table IOS

;   PROCEDURE: IOS_Get_Version
;
;   DESCRIPTION:
;       ios service to return version
;
;   ENTRY:
;       none
;
;   EXIT:
;      eax = version
;      carry clear

IOS_Service IOS_Get_Version, LOCAL

;   PROCEDURE: IOS_BD_Register_Device
;
;   DESCRIPTION:
;      ios service to register a BlockDev device
;
;   ENTRY:
;      edi => BDD (BlockDev Device Descriptor)
;
;   EXIT:
;      CLC is successful, STC if not
;      

IOS_Service IOS_BD_Register_Device

;   PROCEDURE: IOS_Find_Int13_Drive
;
;   DESCRIPTION:
;      ios service to return BDD for int 13 drive
;
;   ENTRY:
;      al = int 13 unit #
;
;   EXIT:
;      edi => BDD if successful
;      CLC is successful, STC if not
;      

IOS_Service IOS_Find_Int13_Drive

;   PROCEDURE: IOS_Get_Device_List
;
;   DESCRIPTION:
;      returns pointer to first BDD
;
;   ENTRY:
;      none
;
;   EXIT:
;      edi => BDD or zero if none
;      

IOS_Service IOS_Get_Device_List

;   PROCEDURE: IOS_SendCommand
;
;   DESCRIPTION:
;      service to accept BlockDev Command Blocks, or I/O Requests (version 2)
;
;   ENTRY:
;      if IORF_VERSION_002 is set:
;           esi points to IOR
;	 			 edi points to DCB_bdd
;
;      otherwise: (for backward BlockDev client compatibility)
;           esi points to BlockDev_Command_Block
;           edi points to BlockDev_Device_Descriptor
;
;       EXIT:
;			    none
;      

IOS_Service IOS_SendCommand

;   PROCEDURE: IOS_BD_Command_Complete
;
;   DESCRIPTION:
;      service to indicate a command has completed
;
;   ENTRY:
;		  esi => IOR
;
;   EXIT:
;      none
;    

IOS_Service IOS_BD_Command_Complete

;   PROCEDURE: IOS_Synchronous_Command
;
;   DESCRIPTION:
;      not currently supported under Chicago
;
;   ENTRY:
;
;   EXIT:
;    

IOS_Service IOS_Synchronous_Command

;   PROCEDURE: IOS_Register
;
;   DESCRIPTION:
;   		service to register a driver with IOS
;
;   ENTRY:
;		   [esp] => Driver Registration Packet (DRP)
;
;   EXIT:
;    	DRP_result set appropriately
;

IOS_Service IOS_Register

;   PROCEDURE: IOS_Requestor_Service
;
;   DESCRIPTION:
;   		service for access to some IOS services
;
;   ENTRY:
;		   [esp] => IOS Requestor Service (IRS) packet
;
;   EXIT:
;    	IRS_result set appropriately
;

IOS_Service IOS_Requestor_Service

;   PROCEDURE: IOS_Exclusive_Access
;
;   DESCRIPTION:
;			locks a given drive for exclusive access from one VM
;
;   ENTRY: 
;				eax - drive to lock
;	       	    - ah - set to IOSEA_LOCK_THREAD or clear, indicating a thread
;		             or VM based lock (lock only)
;	         ebx - VM handle to lock on / release
;	         ecx - nz - obtain exclusive access
;	             - 0 - release exclusive access
;
;   EXIT: 
;				carry set if unsuccessful
;

IOS_Service IOS_Exclusive_Access

;   PROCEDURE: IOS_Send_Next_Command
;
;   DESCRIPTION:
;   		service to send the next command down to a device
;
;   ENTRY:
;			edi => Device Descriptor Block (DCB) for device
;
;   EXIT:
;    	none
;

IOS_Service IOS_Send_Next_Command

;   PROCEDURE: IOS_Set_Async_Timeout
;
;   DESCRIPTION:
;   		identical to VMMCall Set_Async_Timeoout
;
;   ENTRY:
;       EAX = Number of milliseconds to wait until time-out
;       EDX = Reference data to return to procedure
;       ESI = Address of procedure to call when time-out occurs
;
;   EXIT:
;       If time-out was NOT scheduled then
;           ESI = 0 (This is useful since 0 = NO TIME-OUT SCHEDULED)
;       else
;           ESI = Time-out handle (used to cancel time-out)
;    	none
;

IOS_Service IOS_Set_Async_Time_Out

;   PROCEDURE: IOS_Signal_Semaphore_No_Switch
;
;   DESCRIPTION:
;   		identical to VMMCall Signal_Semaphore_No_Switch
;
;   ENTRY:
;			eax = semaphore handle
;
;   EXIT:
;    	none
;

IOS_Service IOS_Signal_Semaphore_No_Switch

;   PROCEDURE: IOSIdleStatus
;
;   DESCRIPTION:
;			This routine determines whether IOS is idle by
;			walking the IOS heap looking for allocated IOPs.
; 		Therefore this should not be called after
;			allocating an IOP.
;   		
;
;   ENTRY:
;			none
;
;   EXIT:
;    	eax = zero if idle, non-zero if not
;

IOS_Service IOSIdleStatus

;   PROCEDURE: IOSMapIORSToI24
;
;   DESCRIPTION:
;			service maps an IOR error code to its int 24 equivalent.
;   		
;
;   ENTRY:
;			eax = IOR error code
;			ecx = IOR function
;
;   EXIT:
;			eax = int 23 error code

IOS_Service	IOSMapIORSToI24

;   PROCEDURE: IOSMapIORSToI24
;
;   DESCRIPTION:
;			service maps an IOR error code to its int 21 equivalent.
;   		
;
;   ENTRY:
;			ecx = IOR error code
;
;   EXIT:
;			eax = int 21 error code

IOS_Service	IOSMapIORSToI21

ifdef INITLOG

;   PROCEDURE: PrintLog
;
;   DESCRIPTION:
;   		
;   ENTRY:
;
;   EXIT:
;			

IOS_Service	PrintLog

endif

End_Service_Table IOS
*/

/**/
/** Definitions for VFBACKUP exclusive access services */
/**/

#define IOSEA_LOCK_THREAD (1 << 8)	// indicates a DCB lock is thread based




