/**[f******************************************************************
* mypcleo.c -
*
* Copyright (C) 1989,1990,1991 Hewlett-Packard Company.
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

/***************************************************************************/
/*
*   PURPOSE: Generate PLCEO's in the font installer (FINSTALL.DLL)
*
*   HISTORY:
*
*      10-Oct-90 - dtk - Original.
*      12-Oct-90 - dtk - Created WritePcleo, init_intellifont, shut_down_bullet,
*                        and config_bullet by modifying ifwin1.c code from CG.
*      13-Oct-90 - dtk - Added dialog function for symbol set selection.
*      15-Oct-90 - dtk - Added build_pcleo name.
*      06-Nov-90 - dtk - Added dialog function for destination directories.
*
*
*   FUNCTIONS:
*
*      WritePcleo()        - writes a pcleo file for the given font info
*      init_intellifont()  - initilize Intellifont subsystem
*      shut_down_bullet()  - closes the Intellifont subsystem
*      build_pcleo_name()  - builds the Type Director compatible pcleo name
*      config_bullet()     - fill in bullet configuration params
*/
/***************************************************************************/
  
  
//#define DEBUG
  
#include "windows.h"
#include <stdio.h>
#include <string.h>
#include <math.h>          /* for trig functions */
#include "..\bullet\cgconfig.h"      /* configuration defines    */
#include "..\bullet\cgifwin.h"       /* cgif for windows         */
#include "faislist.h"      /* fais file format         */
#include "loaderr.h"       /* loader errors            */
#include "debug.h"
#include "strings.h"
#include "dlgutils.h"
#include "mypcleo.h"       /* specific to this program */
#include "resource.h"       /* specific to this program */
  
  
/****************************************************************************\
*                     Debug Definitions
\****************************************************************************/
  
#ifdef DEBUG
    #define DBGwtpcleo(msg)    /*DBMSG(msg)*/
    #define DBGinit(msg)       /*DBMSG(msg)*/
    #define DBGkill(msg)       /*DBMSG(msg)*/
    #define DBGbldname(msg)    /*DBMSG(msg)*/
    #define DBGconfig(msg)     /*DBMSG(msg)*/
#else
    #define DBGwtpcleo(msg)    /*null*/
    #define DBGinit(msg)       /*null*/
    #define DBGkill(msg)       /*null*/
    #define DBGbldname(msg)    /*null*/
    #define DBGconfig(msg)     /*null*/
#endif
  
  
  
extern BYTE szBulletPath[];  /* where bullet can find if.* files */
  
char code_file[]="fullchar.txt";    // paragraph char file
  
DWORD         pageTop, pageBottom, pageLeft, pageRight;
FONTCONTEXT   fc;
HANDLE        hTextBuff;
BOOL          if_init = 0;
DWORD         pcleo_count;   /* 08-29-90 jfd */
  
  
  
WORD            CallerId;
IFCONFIG        cfg;
BYTE far        *cache_mem;
BYTE far        *buffer_mem;
HANDLE          buffer_cghnd;
HANDLE          cc_buf_wnhnd;
HANDLE          cache_wnhnd;
HANDLE          cache_cghnd;
HANDLE          buffer_wnhnd;
  
long        buffer_size;
long        cache_size;
  
#define MAKESSNUM(a,b) (((WORD)(b) << 8) | (WORD)(a))
  
  
// external
  
extern DWORD  lFace;
extern BYTE   szFaceName[];
  
  
/*** Font Alias Table data structure ***/
typedef struct fontAliasTableType {
    char                        aliasTableName[20];
    unsigned short              NXREF;
    char                        xRefArray[100];
    unsigned long               CGtfnum;
} FONTALIASTABLETYPE;
  
typedef struct fontAliasDataType {
    unsigned short              NFATS;
    struct fontAliasTableType   fontAliasTable[1];
} FONTALIASDATATYPE;
  
typedef FONTALIASTABLETYPE FAR * LPFONTALIASTABLETYPE;
typedef FONTALIASDATATYPE FAR * LPFONTALIASDATATYPE;
  
  
BOOL FAR PASCAL FileExist(LPSTR);
  
  
BYTE far    gPcleo_name[15];
  
/****************************************************************************/
/****************************************************************************
FUNCTION: WritePcleo()
  
PURPOSE:  Main function for PCLEO output
  
MESSAGES:
  
COMMENTS:
****************************************************************************/
  
BOOL FAR PASCAL WritePcleo(hPcleo, PcleoDir, lpSymSet, font_id, pcleo_pathname)
HANDLE hPcleo;
LPSTR  PcleoDir;
LPSTR  lpSymSet;
DWORD  font_id;
LPSTR  pcleo_pathname;
  
{
    BYTE   *p;
    WORD   status;
  
    ULONG  pcount;
    BOOL   intellifont_ready = FALSE;
    FARPROC  lpFunc;
    WORD            extent_tbl[256];
  
    DBGwtpcleo(("Inside BeginPCLDemo\n"));
  
    /*
    * Init subsystem
    */
  
  
    // check if already initialized
  
    if (!intellifont_ready)
    {
        if(!init_intellifont(hPcleo))
        {
            intellifont_ready=FALSE;
            shut_down_bullet(hPcleo, intellifont_ready);
            DBGwtpcleo(("failed init_intellifont in pcleo\n"));
            return (FALSE);
        }
        intellifont_ready = TRUE;
    }
  
    DBGwtpcleo(("after call to init_intellifont\n"));
  
    /* Set fontcontext structure typeface number and symbol set */
  
    DBGwtpcleo(("Font_is = %d\n", font_id));
  
    /* Store some default values into the font context */
  
    fc.font_id    = font_id;
    fc.point_size = 96;
    fc.set_size   = 96;
    fc.shear.x    = 0;
    fc.shear.y    = 65536;
    fc.rotate.x   = 65536;
    fc.rotate.y   = 0;
    fc.xres       = 3780;
    fc.yres       = 3780;
    fc.xspot      = 65536;
    fc.yspot      = 65536;
    fc.xbold      = 0;
    fc.ybold      = 0;
    fc.format     = 0;
    fc.ssnum = MAKESSNUM(lpSymSet[0], lpSymSet[1]);
  
  
    lpFunc = GetProcAddress(hPcleo, "CGIFfont");
    if (lpFunc != (FARPROC) NULL)
    {
        if(status = (*lpFunc)((FPFONTCONTEXT)&fc))
        {
            DBGwtpcleo(("CGIFfont error - %d\n",status));
            shut_down_bullet(hPcleo, intellifont_ready);
            return FALSE;
        }
    }
    else
    {
        shut_down_bullet(hPcleo, intellifont_ready);
        return FALSE;
    }
  
  
    lmemset((LPSTR)gPcleo_name, 0, sizeof(gPcleo_name));
  
    if (!(build_pcleo_name(hPcleo, font_id, lpSymSet)))
    {
        shut_down_bullet(hPcleo, intellifont_ready);
        return FALSE;
    }
  
    lstrcpy((LPSTR)pcleo_pathname, (LPSTR)PcleoDir);
  
    lstrcat((LPSTR)pcleo_pathname, (LPSTR)"\\");
    lstrcat((LPSTR)pcleo_pathname, (LPSTR)gPcleo_name);
  
    DBGwtpcleo(("pcleo_path = %ls\n", (LPSTR)pcleo_pathname));
  
  
    if (FileExist((LPSTR)pcleo_pathname))
    {
        shut_down_bullet(hPcleo, intellifont_ready);
        return(TRUE);
    }
  
  
    lpFunc = GetProcAddress(hPcleo, "CGIFpcleo");
    if (lpFunc != (FARPROC) NULL)
    {
        if(status = (*lpFunc)((LPSTR)pcleo_pathname, (LPLONG)&pcount))
        {
            DBGwtpcleo(("CGIFpcleo error - %d\n",status));
            shut_down_bullet(hPcleo, intellifont_ready);
            return FALSE;
        }
    }
    else
    {
        shut_down_bullet(hPcleo, intellifont_ready);
        return FALSE;
    }
  
    shut_down_bullet(hPcleo, intellifont_ready);
    return TRUE;
}
  
  
/****************************************************************************
  
FUNCTION: init_intellifont
  
PURPOSE:  Init subsystem
  
MESSAGES:
  
COMMENTS:
  
  
****************************************************************************/
  
BOOL FAR PASCAL init_intellifont(hPcleo)
HANDLE hPcleo;
{
  
    unsigned short status;
    short          len, i;
    unsigned long   ulCacheSize;
    unsigned long   ulBufferSize;
    FARPROC        lpFunc;
  
  
    DBGinit(("ENTER:  init_bullet\n"));
  
    if(!config_bullet(&cfg))
    {
        DBGinit(("init_bullet:  config failed\n"));
        return FALSE;
    }
  
    if((cc_buf_wnhnd = GlobalAlloc(GMEM_MOVEABLE,(long)cfg.cc_buf_size)) == NULL)
    {
        DBGinit(("init_bullet:  Global Alloc failed - char buffer\n"));
        return FALSE;
    }
  
    if((cfg.cc_buf_ptr = GlobalLock(cc_buf_wnhnd)) == (LPSTR)NULL)
    {
        DBGinit(("init_bullet:  GlobalLock failed - char buffer\n"));
        return FALSE;
    }
  
  
    lpFunc = GetProcAddress(hPcleo, "CGIFinit");
    if (lpFunc != (FARPROC) NULL)
    {
        if(status = (*lpFunc)())
        {
            return FALSE;
            DBGinit(("init_bullet:  CGIFinit failed,  status = %d\n", status));
        }
    }
    else
        return FALSE;
  
  
    lpFunc = GetProcAddress(hPcleo, "CGIFconfig");
    if (lpFunc != (FARPROC) NULL)
    {
        if(status = (*lpFunc)((FPIFCONFIG)&cfg))
        {
            return FALSE;
            DBGinit(("init_bullet:  CGIFconfig failed,  status = %d\n", status));
        }
    }
    else
        return FALSE;
  
  
    if((buffer_wnhnd = GlobalAlloc(GMEM_MOVEABLE, (long)buffer_size)) == NULL)
    {
        DBGinit(("init_bullet:  GlobalAlloc failed,  - buffer mem\n"));
        return FALSE;
    }
  
    if((buffer_mem = GlobalLock(buffer_wnhnd)) == (LPSTR)NULL)
    {
        DBGinit(("init_bullet:  GlobalLock failed,  - buffer mem\n"));
        return FALSE;
    }
  
  
  
    lpFunc = GetProcAddress(hPcleo, "CGIFfund");
    if (lpFunc != (FARPROC) NULL)
    {
        if(status = (*lpFunc)((UWORD)BUFFER_POOL, (LPSTR)buffer_mem, (LONG)buffer_size, (FPWORD) &buffer_cghnd))
        {
            return FALSE;
            DBGinit(("init_bullet:  CGIFfund failed,  status = %d\n", status));
        }
    }
    else
        return FALSE;
  
  
    lpFunc = GetProcAddress(hPcleo, "CGIFenter");
    if (lpFunc != (FARPROC) NULL)
    {
        if(status = (*lpFunc)())
        {
            return FALSE;
            DBGinit(("init_bullet:  CGIFenter failed,  status = %d\n", status));
        }
    }
    else
        return FALSE;
  
  
    DBGinit(("Returning from init_intellifont TRUE -- No Errors\n"));
    return (TRUE);
}
  
/****************************************************************************
  
FUNCTION:   shut_down_bullet
  
PURPOSE:    Shuts down the bullet sub-system and releases the appropriate
resources back to Windows.
  
MESSAGES:
  
COMMENTS:
  
  
****************************************************************************/
  
VOID PASCAL  shut_down_bullet(hPcleo, intellifont_ready)
HANDLE hPcleo;
BOOL intellifont_ready;
{
  
    FARPROC     lpFunc;
  
    DBGkill(("ENTER:  exit_bullet\n"));
  
    if (intellifont_ready)     /* call only if we have gone through CGIFenter */
    {
        lpFunc = GetProcAddress(hPcleo, "CGIFexit");
        if (lpFunc != (FARPROC) NULL)
            (*lpFunc)();
    }
  
    if(cfg.cc_buf_ptr)
    {
        GlobalUnlock(cc_buf_wnhnd);
        cfg.cc_buf_ptr = 0;
        DBGkill(("exit_bullet:  GlobalUnlock failed - char buffer\n"));
    }
  
    if(cc_buf_wnhnd)
    {
        GlobalFree(cc_buf_wnhnd);
        cc_buf_wnhnd = 0;
        DBGkill(("exit_bullet:  GlobalFree failed - char buffer\n"));
    }
  
  
    if(buffer_mem)
    {
        GlobalUnlock(buffer_wnhnd);
        buffer_mem = 0;
        DBGkill(("exit_bullet:  GlobalUnlock failed - buffer memory\n"));
    }
  
    if(buffer_wnhnd)
    {
        GlobalFree(buffer_wnhnd);
        buffer_wnhnd = 0;
        DBGkill(("exit_bullet:  GlobalFree failed - buffer memory\n"));
    }
  
    intellifont_ready = FALSE;
}
  
  
/****************************************************************************\
  
FUNCTION: build_pcleo_name()
  
PURPOSE: Puts together the pcleo name from the info
in the typeface's font alias segment. PCLEO
names are built following the Type Director
standard.
  
MESSAGES:
  
COMMENTS: 10-16-90  dtk
  
\****************************************************************************/
  
BOOL FAR PASCAL build_pcleo_name(hPcleo, font_id, lpSymSet)
HANDLE hPcleo;
DWORD font_id;
LPSTR lpSymSet;
{
  
  
    UWORD fa_size;
    WORD  fa_h;
    LPSTR fa, fa_tbl;
    unsigned short   status;
    FARPROC     lpFunc;
  
  
    lpFunc = GetProcAddress(hPcleo, "CGIFsegments");
    if (lpFunc != (FARPROC) NULL)
    {
        if(status = (*lpFunc)((long)font_id, (UWORD)FONT_ALIAS_KEY, (UWORD FAR *)&fa_size, (LPSTR)NULL))
        {
            DBGbldname(("CGIFsegment 'getsize' status = %d\n", status ));
            return FALSE;
        }
    }
    else
        return FALSE;
  
    if((fa_h = GlobalAlloc(GMEM_MOVEABLE, (long)fa_size)) == NULL)
    {
        DBGbldname(("Gobal Alloc Failed for font alias\n"));
        return FALSE;
    }
  
    if ((fa = (LPSTR) GlobalLock(fa_h)) == (LPSTR)NULL)
    {
        DBGbldname(("Gobal Lock Failed inside get_typeface_info\n"));
        GlobalFree(fa_h);
        return FALSE;
    }
  
    /* get a pointer to the font alais segment for this typeface */
  
    if(status = (*lpFunc)((long)font_id, (UWORD)FONT_ALIAS_KEY, (UWORD FAR *)&fa_size, (LPSTR)fa))
    {
        DBGbldname(("CGIFsegment 'getdata' status = %d\n", status ));
        GlobalUnlock(fa_h);
        GlobalFree(fa_h);
        return FALSE;
    }
  
    fa_tbl = (LPSTR)((LPFONTALIASDATATYPE)fa)->fontAliasTable[0].xRefArray;
    gPcleo_name[0] = fa_tbl[0];
    gPcleo_name[1] = fa_tbl[1];
    gPcleo_name[2] = fa_tbl[3];
    lstrcat(gPcleo_name, (LPSTR)"00");
    lstrcat(gPcleo_name, lpSymSet);
    lstrcat(gPcleo_name, (LPSTR)"O.SFS");
    DBGbldname(("PCLEO name will be = %ls\n", (LPSTR)gPcleo_name));
  
    GlobalUnlock(fa_h);
    GlobalFree(fa_h);
    return TRUE;
  
  
}
  
  
  
/****************************************************************************\
  
FUNCTION: config_bullet()
  
PURPOSE: read configuration paramaters:
  
MESSAGES:
  
COMMENTS:
  
\****************************************************************************/
  
BOOL PASCAL config_bullet(cfg)
PIFCONFIG       cfg;
{
    short   len, i;
  
    DBGconfig(("Inside BeginPCLDemo\n"));
  
    cache_size         = 64000;       /* FIXME...figure out minimum values */
    cfg->cc_buf_size   = 0xa000;
    buffer_size        = 0x8000;
    cfg->max_char_size = 750;
    cfg->bit_map_width = 2;
    cfg->num_files     = 3;
  
    GetProfileString((LPSTR)"Intellifont", (LPSTR)"SupportFiles", (LPSTR)"c:\\TD",
    (LPSTR) cfg->bulletPath , (int) PATHNAMELEN);
    lstrcpy((LPSTR)cfg->typePath, (LPSTR)cfg->bulletPath);
  
    len = lstrlen((LPSTR)cfg->bulletPath);
    if ((cfg->bulletPath[len-1] != '\\') &&  /* must have a trailing '\' */
        (cfg->bulletPath[len-1] != '/'))     /* (already there if root dir) */
        lstrcat((LPSTR)cfg->bulletPath, "\\");
    return TRUE;
}
  
  
  
  
  
/****************************************************************************\
*
*  name: FileExist
*
*  description:
*
*  return value:
*
*  globals:
*
*  called by:
*
*  history:
*
\****************************************************************************/
  
BOOL FAR PASCAL FileExist(name)
LPSTR name;
{
    int hFile;
  
    if ((hFile =_lopen(name, OF_READ)) == -1)
        return(FALSE);
    _lclose(hFile);
    return(TRUE);
}
  
  
  
