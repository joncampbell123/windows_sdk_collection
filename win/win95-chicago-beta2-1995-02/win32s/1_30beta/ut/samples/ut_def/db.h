/*++

Copyright (c) 1985-92, Microsoft Corporation

Module Name:

    db.h

Abstract:

    Win32s sample code of Universal Thunk (UT) -
    this example demonstrates how existing 16bit code can provide services
    to 32bit application thru a thunking layer based on the UT api.

    This header file defines the interface to the data base services.
    The 32-bit interface provided by thunking is identical to the 16-bit
    interface.

--*/


typedef  struct {
     DWORD         len;
     LPSTR         str;
} DB_NAME, FAR * LPDB_NAME;

typedef  struct {
     short int  year;
     short int  month;
     short int  day;
     short int  hour;
     short int  minute;
     short int  seconds;
} DB_TIME, FAR * LPDB_TIME;


/*
 * following services are provided by DB.DLL which is a 16bit dll.
 */


int         DbGetVersion(void);
void        DbSetTime(LPDB_TIME pTime);
short       DbAddUser(LPDB_NAME pName, DWORD Permission, LPDWORD pId);


