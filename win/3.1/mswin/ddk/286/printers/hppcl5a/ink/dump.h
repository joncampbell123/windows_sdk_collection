/**[f******************************************************************
* dump.h -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.  Company confidential.
*
**f]*****************************************************************/
  
// 01 dec 89    peterbe lp_*() for VisualEdge/LaserPort in #ifdef now.
  
int FAR PASCAL dump(LPDEVICE);
  
#ifdef VISUALEDGE
int FAR PASCAL lp_ff(LPDEVICE);
int FAR PASCAL lp_enbl(void);
int FAR PASCAL lp_disable(void);
int FAR PASCAL lp_DeviceData(LPDEVICE,LPSTR,int);
int FAR PASCAL lp_Reset(LPDEVICE);
#endif
