/**[f******************************************************************
* version.h -
*
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1990,1991 Hewlett-Packard Company.
*     All rights reserved.
*     Company confidential.
*
**f]*****************************************************************/
  
/*
 * $Header: 
 */

/*
 * $Log:
 */

/* Be sure to change version number in \ink\finstall.rcv also (4 places) */
  
// this is included into FINSTALL.RC
// Version 1.00 was Original Released version  
// Version 1.01 was internal version  
// Version 1.02 was for Windows 3.1 beta 2   
// Version 1.03 was to upload Bullet and Help files  
// Version 1.04 was for following bugs: 501,502,503,507,523,525,531  
// Version 1.05 was for Windows 3.1 beta 2 fork (and bug 533) 
// Version 1.06 was for the following bugs: 547, 282, 557
// Version 1.07 was sent to Microsoft August 13, 1991 
// Version 1.08 was to fix sporatic GP fault with gModNm and gPortNm 
// Version 1.09 was to send to Microsoft August 16, 1991 for 3.1 Beta 2
// Version 1.10 was to fix Bug 570, and 566. Change gModNm in $readlib.c
// and sfutils.c and sfadd.c.
// Version 1.11 was to send to Microsoft August 22, 1991 for 3.1 Beta 3
// Version 1.12 was to fix bug 631 OpenFile problem with Win3.1
//              but this version will be obsoleted by 1.13 and these changes
//              will be rolled back-in in version 1.15
// Version 1.13 was to upload fix Title for Triton Support Sept 6, 1991
// Version 1.14 was to Freeze version for 3.0 to send to Microsoft Sept 6, 1991
// Version 1.15 was to merge bug 631 ith Triton Support Sept 6, 1991
// Version 1.16 was to Freeze version for 3.1 to send to Microsoft Sept 6, 1991
// Version 1.17 was to upload files to fix bug 685 UAEs in Font Installer
//              dlgutils.c, face.c, and version.h
// Version 1.18 was to Freeze version for 3.1 to send to Microsoft Sept 6, 1991
// Version 1.19 was to upload fix for Bug 681 - Not install upper 128 chars.
// Version 1.20 was to Freeze version for 3.0 to send to Microsoft Oct 14, 1991
// Version 1.20.1 was to make additional changes for localization Oct 25, 1991  
//              Added RCS_VERSION. Bugs 718, 720, XI(housekeeping).
//              $SFADD.c modified to read symbol set names from finstall.rc
//              and use first two characters as symbol set.
// Version 1.20.2 was to change $SFADD.c not to use first two characters
//              as symbol set  
// Version 1.20.3 was to add 1992 copyright year to FINSTALL.RC
// Version 1.20.4 was to increase buf in SFINSTAL.C from 31 to 64 for 
//              SF_NOPORT and SF_YESPORT
// Version 1.25 was to fix bug 726 so San Diego could use AutoFont fonts.
// Version 1.26 was to fix bug for localization so gScalable would be larger.
//              and to fix bug 730 for Microsoft-sf_yn.h file for localization.
// Version 1.27 was to fix bug for localization so gTmpFile would be larger.
// Version 1.28 was to fix bug for localization so SF_TKUNKDRV, SF_TKDUPDRV, 
//              and SF_TKEXPLSTR would be larger. Fixed bug 748 for MS so
//              pfm created for DeskJet would show lastchar as 255.
/* Font Installer version that appears in the About box */
#define VERS    "Version 1.28"
                               
/* Version in RCS under which files will be checked in  */
#define RCS_VERSION   1.28

#define VERCOPY "Copyright 1992"
// VERCOPY assigned to SF_COPYRIGHT in finstall.rc but never used
