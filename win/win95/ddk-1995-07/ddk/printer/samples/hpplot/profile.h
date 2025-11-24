/* (c) Copyright 1987 MICROGRAFX, Inc., All Rights Reserved.  Permission to
   use this work for any purpose must be obtained in writing from MICROGRAFX,
   1820 N. Greenville Ave., Richardson, Tx.  75081.

*******************************************************************************

				   profile

*******************************************************************************

*/

void FAR PASCAL setup_profile (HANDLE,LPSTR,LPSTR);
int FAR PASCAL get_profile (LPSTR,LPENVIRONMENT,LPSTR,LPSTR);
// void FAR PASCAL save_profile (LPDIALOGINFO);
void FAR PASCAL get_defaults (LPENVIRONMENT,BOOL);
short FAR PASCAL MakeAppName(LPSTR,LPSTR,LPSTR,short);
void SaveOneProfileString(LPSTR,LPSTR,LPSTR,LPSTR);
int GetOneProfileInt(LPSTR,LPSTR,int,LPSTR);
void GetOneProfileString(LPSTR,LPSTR,LPSTR,LPSTR,int,LPSTR);
