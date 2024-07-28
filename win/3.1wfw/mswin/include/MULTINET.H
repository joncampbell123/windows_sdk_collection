
/*
 *      Windows/Network Interface
 *      Copyright (C) Microsoft 1989-1992
 *
 *      Standard MULTINET Header File, spec version 3.10
 *
 * 		General C include file to be included by modules that use   
 *      the multi-network extensions to the WINNET interface.       
 *
 */


#ifndef _INC_MULTINET
#define _INC_MULTINET	

#ifndef RC_INVOKED
#pragma pack(1)         /* Assume byte packing throughout */
#endif  /* RC_INVOKED */

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif	/* __cplusplus */


/*
 * Return status codes from device/resource/file utilities.
 */
#define DN_PT_UNKNOWN       0       /* Unknown or bad device or resource syntax */
#define DN_PT_PRINTER       1       /* Device is a printer */
#define DN_PT_DISK  	    2       /* Device is a disk drive */
#define DN_PT_UNC       	3       /* Resource is a UNC name */
#define DN_PT_ALIAS     	4       /* Resource is an alias name */
#define DN_PT_NETWARE       5       /* Resource follows NetWare convention */
#define DN_PT_FILELISTDEVICE  6     /* Device is first entry in file list */

/* 
 * Masks for individual network information
 */
#define MNM_NET_PRIMARY     0x0001      /* Network is primary network (Windows network) */

/*
 * Function prototypes for multi-net extensions
 */
HANDLE API  MNetGetLastTarget (void );
WORD   API  MNetSetNextTarget (HANDLE hNetwork );
WORD   API  MNetNetworkEnum   (HANDLE FAR *hNetwork );
WORD   API  MNetGetNetInfo    (HANDLE hNetwork, LPWORD lpwNetInfo, LPSTR lpszButton,
							   LPINT lpcbButton, LPHANDLE lphInstance );


#ifndef RC_INVOKED
#pragma pack()          /* Revert to default packing */
#endif  /* RC_INVOKED */

#ifdef __cplusplus
}                       /* End of extern "C" { */
#endif	/* __cplusplus */

#endif  /* _INC_WINDOWS */


#ifndef _multinet_h_
#define _multinet_h_


#endif /* _multinet_h_ */
