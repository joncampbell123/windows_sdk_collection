/****************************************************************************
 *
 *   mcipionr.h
 *
 *   Copyright (c) 1991-1992 Microsoft Corporation.  All Rights Reserved.
 *
 ***************************************************************************/

#define IDS_PRODUCTNAME                 1
#define IDS_COMMANDS                    2
#define MCIERR_PIONEER_ILLEGAL_FOR_CLV  (MCIERR_CUSTOM_DRIVER_BASE)
#define MCIERR_PIONEER_NOT_SPINNING     (MCIERR_CUSTOM_DRIVER_BASE + 1)
#define MCIERR_PIONEER_NO_CHAPTERS      (MCIERR_CUSTOM_DRIVER_BASE + 2)
#define MCIERR_PIONEER_NO_TIMERS        (MCIERR_CUSTOM_DRIVER_BASE + 3)

/* custom command support */
#define VDISC_FLAG_ON       0x00000100L
#define VDISC_FLAG_OFF      0x00000200L

/* must use literals to satisfy the RC compiler  */
#define VDISC_INDEX         1000
#define VDISC_KEYLOCK       1002

#define VDISC_FIRST         VDISC_INDEX
#define VDISC_LAST          VDISC_KEYLOCK

extern HINSTANCE hInstance;

extern WORD FAR PASCAL pionGetComport(LPSTR lpstrBuf);
extern DWORD FAR PASCAL mciDriverEntry(WORD wDeviceID, UINT message,
                                        LPARAM lParam1, LPARAM lParam2);

/****************************************************************************

    Debug support

 ***************************************************************************/

#ifdef DEBUG
   #define DOUT(sz)  (wDebugLevel != 0 ? OutputDebugStr("\r\n"), OutputDebugStr(sz), 0 : 0 )
   #define DOUTX(sz) (wDebugLevel != 0 ? OutputDebugStr(sz), 0 : 0 )
   #define DBREAK()  DebugBreak()

#else
   #define DOUT(sz)  0
   #define DOUTX(sz) 0
   #define DBREAK()  0
#endif
