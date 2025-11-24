/*
 *  M A P I W Z . H
 *
 *  Definitions for the Profile Wizard.  Includes all prototypes
 *  and constants required by the provider-wizard code consumers.
 *
 *  Copyright 1994-1995 Microsoft Corporation. All Rights Reserved.
 */

#ifndef _MAPIWZ_H
#define _MAPIWZ_H

#define WIZ_QUERYNUMPAGES   (WM_USER +10)
#define WIZ_NEXT            (WM_USER +11)
#define WIZ_PREV            (WM_USER +12)
/*
 *  NOTE: Provider-Wizards should not use ids ranging between
 *  (WM_USER + 1) and (WM_USER +20) as these have been reserved for 
 *  future releases.
 */

/*  For pre-TR3 compatibility */

#define ID_NEXT             2
#define ID_PREV             3

/*  Flags for LaunchWizard API */

#define MAPI_PW_FIRST_PROFILE           0x00000001
#define MAPI_PW_LAUNCHED_BY_CONFIG      0x00000002
#define MAPI_PW_ADD_SERVICE_ONLY        0x00000004
#define MAPI_PW_PROVIDER_UI_ONLY        0x00000008
#define MAPI_PW_HIDE_SERVICES_LIST      0x00000010

typedef HRESULT (STDAPICALLTYPE LAUNCHWIZARDENTRY)
(
    HWND            hParentWnd,
    ULONG           ulFlags,
    LPCTSTR FAR *   lppszServiceNameToAdd,
    ULONG           cbBufferMax,
    LPTSTR          lpszNewProfileName
);

typedef ULONG (STDAPICALLTYPE SERVICEWIZARDENTRY)
(
    HINSTANCE       hProviderDLLInstance,
    LPSTR           lpcsResourceName,
    FARPROC FAR *   lpDlgProc,
    LPMAPIPROP      lpMapiProp
);

typedef BOOL (STDAPICALLTYPE SERVICEWIZARDDLGPROC)
(
    HWND            hDlg,
    UINT            wMsgID,
    WPARAM          wParam,
    LPARAM          lParam
);

typedef ULONG (STDAPICALLTYPE NEWSERVICEWIZARDENTRY)
(
    HINSTANCE       hProviderDLLInstance,
    LPSTR           lpcsResourceName,
    FARPROC FAR *   lpDlgProc,
    LPMAPIPROP      lpMapiProp,
    LPVOID          lpReserved
);

typedef LAUNCHWIZARDENTRY FAR * LPLAUNCHWIZARDENTRY;
typedef SERVICEWIZARDENTRY FAR * LPSERVICEWIZARDENTRY;
typedef SERVICEWIZARDDLGPROC FAR * LPSERVICEWIZARDDLGPROC;

typedef NEWSERVICEWIZARDENTRY FAR * LPNEWSERVICEWIZARDENTRY;

#define LAUNCHWIZARDENTRYNAME           "LAUNCHWIZARD"

/*  For pre-TR3 compatibility */

typedef LPSERVICEWIZARDDLGPROC  ProviderDlgProc;
typedef LPSERVICEWIZARDENTRY    PFNPROVIDER;

#endif  /* _MAPIWZ_H */
