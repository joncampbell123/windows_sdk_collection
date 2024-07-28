/**[f******************************************************************
* extescs.h -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1989-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved. Company confidential.
*
**f]*****************************************************************/
  
#define SETCOPYCOUNT        17
#define SELECTPAPERSOURCE   18
#define DEVICEDATA      19
  
#define GETTECHNOLOGY       20
#define SETLINECAP      21
#define SETLINEJOIN     22
#define SETMITERLIMIT       23
#define BANDINFO        24
#define DRAWPATTERNRECT     25
  
#define ENABLEDUPLEX        28
#ifndef GETSETPAPERBINS
#define GETSETPAPERBINS     29
#endif
#ifndef GETSETPRINTORIENT
#define GETSETPRINTORIENT   30
#endif
#define ENUMPAPERBINS       31
  
#define GETEXTENDEDTEXTMETRICS  256
#define GETEXTENTTABLE      257
#define GETPAIRKERNTABLE    258
#define GETTRACKKERNTABLE   259
  
#define SETALLJUSTVALUES    771
#define SETCHARSET      772
  
  
/*  Extended data structure for extended text escapes
*/
typedef struct {
    short nSize;
    LPSTR lpInData;
    LPFONTINFO lpFont;
    LPTEXTXFORM lpXForm;
    LPDRAWMODE lpDrawMode;
} EXTTEXTDATA;
typedef EXTTEXTDATA FAR * LPEXTTEXTDATA;
  
  
