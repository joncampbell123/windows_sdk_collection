/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1994 - 1995  Microsoft Corporation.  All Rights Reserved.
 * 
 *  File:       othervxd.c
 *  Content:	handle loading of alternate VxD for handling joystick
 */

#define WANTVXDWRAPS
#include <basedef.h>
#include <vmm.h>
#include <vmmreg.h>
#include <vxdldr.h>
#include <vxdwraps.h>
#include <debug.h>
#include <regstr.h>
#include "getpos.h"

#undef CURSEG               
#define CURSEG()                   PCODE

#pragma VxD_PAGEABLE_CODE_SEG
#pragma VxD_PAGEABLE_DATA_SEG


#ifdef DEBUG
#define DPF(x)	Out_Debug_String( x )
extern void DPF_DW( DWORD );
extern void DPF_INT( DWORD );
#else
#define DPF(x)
#define DPF_DW(x)
#define DPF_INT(x)
#endif

LPVXDINFO		pVxDInfo[MAX_JOYSTICKS+1] =
    { NULL, NULL, NULL, NULL,NULL, NULL, NULL, NULL,
      NULL, NULL, NULL, NULL,NULL, NULL, NULL, NULL, NULL };

DWORD			dwOEMJoyId = 0;

static const char	szCfgKey[] = REGSTR_PATH_JOYCONFIG "\\";
static const char	szCurrCfgKey[] = "\\" REGSTR_KEY_JOYCURR;
static char		*regCfgKey = NULL;
static char		*regCurrCfgKey = NULL;

int			iNumVxDs = 0;
VXDUSAGE		VxDUsage[MAX_JOYSTICKS+1] = {
    { NULL, 0 },
    { NULL, 0 },
    { NULL, 0 },
    { NULL, 0 },
    { NULL, 0 },
    { NULL, 0 },
    { NULL, 0 },
    { NULL, 0 },
    { NULL, 0 },
    { NULL, 0 },
    { NULL, 0 },
    { NULL, 0 },
    { NULL, 0 },
    { NULL, 0 },
    { NULL, 0 },
    { NULL, 0 },
    { NULL, 0 }
};

/*
 * UnloadVxD - unload a loaded VxD
 */
void UnloadVxD( int idx )
{
    DPF( "Unloading VxD " );
    DPF( VxDUsage[idx].pvi->pszVxDModName );
    DPF( ", idx = " );
    DPF_INT( idx );
    DPF( "\r\n" );

    VXDLDR_UnloadDevice( UNDEFINED_DEVICE_ID, VxDUsage[idx].pvi->pszVxDModName );
    _HeapFree( VxDUsage[idx].pvi->pszVxDModName, 0 );
    _HeapFree( VxDUsage[idx].pvi->pszVxDName, 0 );
    _HeapFree( VxDUsage[idx].pvi, 0 );
    VxDUsage[idx].pvi = NULL;

} /* UnloadVxD */

#define LOWER( c ) (c>='A' && c<='Z') ? (c-'A'+'a'):c

/*
 * strEqual - see if two strings are equal
 */
int strEqual( char *s1, char *s2 )
{
    char	c1,c2;

    while( *s1 && *s2 ) {
	c1 = LOWER( *s1 );
	c2 = LOWER( *s2 );
	if( c1 != c2 ) {
	    return FALSE;
	}
	s1++;
	s2++;
    }
    if( *s1 || *s2 ) {
	return FALSE;
    }
    return TRUE;

} /* strEqual */

/*
 * LoadOtherVxDs - open another VxD, based on the name in the registry
 */
void LoadOtherVxDs( void )
{
    VXDLDRRET	ldr_rc;
    PVMMDDB	pddb;
    PDEVICEINFO	pdevinfo;
    char	name[ MAX_JOYSTICKOEMVXDNAME ];
    char	regval[ MAX_JOYSTICKOEMVXDNAME ];
    VMMREGRET	reg_rc;
    VMMHKEY	hkey;
    DWORD	type;
    char	*pname;
    char	*pmodname;
    LPVXDINFO	pvi;
    int		i;
    int		j;
    BOOL	found;
    BOOL	ok;
    DWORD	cb;

    /*
     * reset usage count so that we can eliminate any unused ones
     * at the end...
     */
    for( j=0;j<iNumVxDs;j++ ) {
	VxDUsage[j].iUsage = 0;
    }
    for( dwOEMJoyId=0;dwOEMJoyId<MAX_JOYSTICKS;dwOEMJoyId++ ) {
	RegisterDeviceDriver( NULL, NULL, NULL, NULL );
    }

    /*
     * get drivers for all possible joysticks
     */
    reg_rc = _RegOpenKey( HKEY_LOCAL_MACHINE, regCurrCfgKey, &hkey );
    if( reg_rc == ERROR_SUCCESS ) {
	for( dwOEMJoyId=0;dwOEMJoyId<=MAX_JOYSTICKS;dwOEMJoyId++ ) {
	    /*
	     * see if we are on the "global" callout
	     */
	    if( dwOEMJoyId == MAX_JOYSTICKS ) {
		strcpy( regval, REGSTR_VAL_JOYOEMCALLOUT );
	    } else {
		_Sprintf( regval, REGSTR_VAL_JOYNOEMCALLOUT, dwOEMJoyId+1 );

		/*
		 * mark that this joystick ID is not in use
		 */
		if( pVxDInfo[dwOEMJoyId] != NULL ) {
		    if( pVxDInfo[dwOEMJoyId]->pExternalJoyId != NULL ) {
			pVxDInfo[dwOEMJoyId]->pExternalJoyId( dwOEMJoyId, FALSE );
		    }
		}
	    }
	    pVxDInfo[dwOEMJoyId] = NULL;
	    cb = sizeof( name );
	    reg_rc = _RegQueryValueEx( hkey, regval, NULL, &type, (PBYTE)name, &cb );
	    if( reg_rc == ERROR_SUCCESS ) {
		found = FALSE;
		for( j=0;j<iNumVxDs;j++ ) {

		    if( strEqual( name, VxDUsage[j].pvi->pszVxDName ) ) {
			found = TRUE;
			VxDUsage[j].iUsage++;
			pvi = VxDUsage[j].pvi;
			pVxDInfo[dwOEMJoyId] = pvi;
			RegisterDeviceDriver( pvi->pExternalPoll,
					      pvi->pExternalCfg,
					      pvi->pExternalHWCaps,
					      pvi->pExternalJoyId );
			break;
		    }
		}
		if( !found ) {
		    pvi = _HeapAllocate( sizeof( VXDINFO ), HEAPZEROINIT );
		    if( pvi != NULL ) {
			/*
			 * load the new VxD.
			 * NOTE: the OEM VxD MUST do a RegisterDevice
			 * during its first init.   If it doesn't, we
			 * don't work.
			 */
			pVxDInfo[ dwOEMJoyId ] = pvi;
			ldr_rc = VXDLDR_LoadDevice( &pddb, &pdevinfo, name, TRUE);
			if( ldr_rc == 0 ) {
			    ok = FALSE;
			    pmodname = NULL;
			    pname = NULL;
			    pname = _HeapAllocate( strlen( name )+1, HEAPZEROINIT );
			    if( pname != NULL ) {
				strcpy( pname, name );
				pmodname = _HeapAllocate( strlen( pdevinfo->DI_ModuleName )+1, HEAPZEROINIT );
				if( pmodname != NULL ) {
				    strcpy( pmodname, pdevinfo->DI_ModuleName );
				    ok = TRUE;
				}
			    }
			    if( ok ) {
				VxDUsage[ iNumVxDs ].iUsage = 1;
				VxDUsage[ iNumVxDs ].pvi = pvi;
				pvi->pszVxDName = pname;
				pvi->pszVxDModName = pmodname;
				iNumVxDs++;
			    } else {
				VXDLDR_UnloadDevice( UNDEFINED_DEVICE_ID, pdevinfo->DI_ModuleName );
				pVxDInfo[ dwOEMJoyId ] = NULL;
				if( pvi != NULL ) {
				    _HeapFree( pvi, 0 );
				}
				if( pname != NULL ) {
				    _HeapFree( pname, 0 );
				}
				if( pmodname != NULL ) {
				    _HeapFree( pmodname, 0 );
				}
			    }
			} else {
			    pVxDInfo[ dwOEMJoyId ] = NULL;
			}
		    }
		}
	    }

	    /*
	     * notify OEM VxD of its assigned joystick id
	     */
	    if( pVxDInfo[dwOEMJoyId] != NULL ) {
		if( pVxDInfo[dwOEMJoyId]->pExternalJoyId != NULL ) {
		    pVxDInfo[dwOEMJoyId]->pExternalJoyId( dwOEMJoyId, TRUE );
		}
	    }
	}
    }

    /*
     * unload any unneeded VxDs
     */
    for( i=0;i<iNumVxDs;i++ ) {
	if( VxDUsage[i].iUsage == 0 ) {
	    UnloadVxD( i );
	    for( j=i;j<iNumVxDs-1;j++ ) {
		VxDUsage[j] = VxDUsage[j+1];
	    }
	    iNumVxDs--;
	}
    }

    /*
     * set up the global VxD
     */
    if( pVxDInfo[MAX_JOYSTICKS] != NULL ) {
	pvi = pVxDInfo[MAX_JOYSTICKS];
	for( i=0;i<iNumVxDs;i++ ) {
	    if( VxDUsage[i].pvi == pvi ) {
		for( j=0;j<MAX_JOYSTICKS;j++ ) {
		    if( pVxDInfo[j] == NULL ) {
			pVxDInfo[j] = pvi;
			VxDUsage[i].iUsage++;
			if( pvi->pExternalJoyId != NULL ) {
			    pvi->pExternalJoyId( j, TRUE );
			}
		    } else {
			if( pvi->pExternalJoyId != NULL ) {
			    pvi->pExternalJoyId( j, FALSE );
			}
		    }
		}
		break;
	    }
	}
    }

} /* LoadOtherVxDs */

/*
 * OtherVxDInit - called when we get our PNP_NEW_DEVNODE msg
 */
void __stdcall OtherVxDInit( DWORD devnode )
{
    VMMHKEY	hkey;
    VMMHKEY	hsubkey;
    VMMREGRET	reg_rc;
    int		keyidx;
    char	keyname[REGSTR_MAX_VALUE_LENGTH];
    DWORD	type;
    DWORD	cb;
    DWORD	dnode;
    int		len;

    /*
     * we need to find where our specific registry info is located, so we
     * go running through every subkey of REGSTR_PATH_JOYCONFIG until we find
     * the one that has our devnode
     */
    keyidx = 0;
    reg_rc = _RegOpenKey( HKEY_LOCAL_MACHINE, REGSTR_PATH_JOYCONFIG, &hkey );
    if( reg_rc == ERROR_SUCCESS ) {
	while( 1 ) {
	    reg_rc = _RegEnumKey( hkey, keyidx, keyname, sizeof( keyname ) );
	    if( reg_rc != ERROR_SUCCESS ) {
		break;
	    }
	    reg_rc = _RegOpenKey( hkey, keyname, &hsubkey );
	    if( reg_rc == ERROR_SUCCESS ) {
		cb = sizeof( dnode );
		reg_rc = _RegQueryValueEx( hsubkey, "DevNode", NULL, &type,
			    (PBYTE) &dnode, &cb );
		if( reg_rc == ERROR_SUCCESS ) {
		    if( dnode == devnode ) {
			/*
			 * allocate string that contains path to all joystick
			 * registry info
			 */
			len = strlen( keyname ) + sizeof( szCfgKey );
			if( regCfgKey != NULL ) {
			    _HeapFree( regCfgKey, 0 );
			}
			regCfgKey = _HeapAllocate( len, HEAPZEROINIT );
			if( regCfgKey != NULL ) {
			    strcpy( regCfgKey, szCfgKey );
			    strcpy( &regCfgKey[ sizeof( szCfgKey )-1 ], keyname );
			} else {
			    break;
			}

			/*
			 * allocate string that contains path to current
			 * joystick registry info
			 */
			if( regCurrCfgKey != NULL ) {
			    _HeapFree( regCurrCfgKey, 0 );
			}
			regCurrCfgKey = _HeapAllocate( len +
				    sizeof( szCurrCfgKey )-1, HEAPZEROINIT );
			if( regCurrCfgKey != NULL ) {
			    strcpy( regCurrCfgKey, regCfgKey );
			    strcpy( &regCurrCfgKey[ len-1 ], szCurrCfgKey );
			}
			break;
		    }
		}
	    }
	    keyidx++;
	}
	if( regCurrCfgKey && regCfgKey ) {
	    LoadOtherVxDs();
	}
    }

} /* OtherVxDInit */

/*
 * DeviceExit - called when our VxD is unloaded - ditch other VxD
 */
void __stdcall DeviceExit( void  )
{
    int	i;
    for( i=0;i<iNumVxDs;i++ ) {
	UnloadVxD( i );
    }
    iNumVxDs = 0;
    if( regCfgKey != NULL ) {
	_HeapFree( regCfgKey, 0 );
	regCfgKey = NULL;
    }
    if( regCurrCfgKey != NULL ) {
	_HeapFree( regCurrCfgKey, 0 );
	regCurrCfgKey = NULL;
    }

} /* DeviceExit */

/*
 * DeviceInit - called when our VxD is initialized
 */
void __stdcall DeviceInit( void  )
{
    InitJoyProcessing();

} /* DeviceInit */
