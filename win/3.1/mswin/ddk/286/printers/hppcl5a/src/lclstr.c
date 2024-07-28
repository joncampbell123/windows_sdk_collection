/**[f******************************************************************
* lclstr.c -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.  Company confidential.
*
**f]*****************************************************************/
  
/***************************************************************************/
/******************************   lclstr.c    ******************************/
/*
*  Common, multiple use strings.
*
*   1-23-90   SD       Changed driver's module name to HPPCL5A
*
*   1-13-89    jimmat  Initial version.  Created to reduce the number of
*          redundant strings in the local data segment.  Now
*          there will only be one copy of the version # string,
*          the module name string, etc. whereas before there
*          were multiple copies.
*   1-25-89    jimmat  Removed need for Module name string.
*   1-30-89    jimmat  Added FontInstaller string.
*   2-20-89    jimmat  Added ModuleNameStr.
*/
  
#include "version.h"
#include "generic.h"
#include "resource.h"
  
  
char VNumStr[] = VNUM;              /* driver version # */
  
char CrLf[] = "\r\n";
char NullStr[] = "";
  
char PclStr[] = "PCL";
  
char ModuleNameStr[] = "HPPCL5A";       /* Driver's module name    */
char FontInstallerStr[] = "FINSTALL.DLL";   /* Font Installer DLL name */
