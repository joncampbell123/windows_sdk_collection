/**[f******************************************************************
* lclstr.h -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.  Company confidential.
*
**f]*****************************************************************/
  
/***************************************************************************/
/******************************   lclstr.h    ******************************/
/*
*  Common, multiple use strings.
*
*  Fri 13-Jan-1989  09:30:12    -by-   Jim Mathews   [jimmat]
*
*  Initial version.  Created to reduce the number of redundant strings
*  in the local data segment.  Now there will only be one copy of
*  the version # string, the module name string, etc. whereas before
*  there were multiple copies.
*
*   1-25-89    jimmat  Removed FINSTALL specific strings.
*   1-30-89    jimmat  Added FontInstaller string.
*   2-20-89    jimmat  Added ModuleNameStr.
*/
  
extern char VNumStr[];          /* driver version # */
extern char NullStr[];          /* ""      */
extern char PclStr[];           /* "PCL"   */
extern char ModuleNameStr[];        /* "HPPCL" */
extern char FontInstallerStr[];     /* name of Font Installer DLL */
