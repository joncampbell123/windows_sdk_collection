/**[f******************************************************************
* $sfpfm.c -
*
* Copyright (C) 1990,1991 Hewlett-Packard Company.
*     All rights reserved.
*     Company confidential.
*  2 feb 92 rk Changed $ include files to _
*
**f]*****************************************************************/
/*
 * $Header: 
 */

/*
 * $Log:
 */
  
//#define DEBUG
  
#include "windows.h"
#include <stdio.h>
#include <string.h>
#include <math.h>              /* for trig functions */
#include "utils.h"
#include "..\bullet\cgconfig.h"          //Sets compile flags for Bullet code
#include "..\bullet\cgifwin.h"           /* specific to this program             */
#include "..\bullet\segments.h"
#include "_sfpfm.h"
#include "_sfpfm2.h"
#include "debug.h"
  
  
/****************************************************************************\
* Debug Definitions
\****************************************************************************/
  
  
#ifdef DEBUG
    #define DBGbptm(msg)           /*DBMSG(msg)*/
    #define DBGmvfname(msg)        /*DBMSG(msg)*/
#else
    #define DBGbptm(msg)           /*null*/
    #define DBGmvfname(msg)        /*null*/
#endif
  
  
/****************************************************************************\
*                          Global Variables
\****************************************************************************/
  
/* FIXME...need to fix the shared header file mess !! */
LPSTR FAR PASCAL lstrncpy(LPSTR, LPSTR, int);
  
  
/****************************************************************************\
*                          Local Definitions
\****************************************************************************/
  
  
  
/****************************************************************************\
*                          Local Variables
\****************************************************************************/
  
WORD            cc_buf_wnhnd;
WORD            cache_wnhnd;
BYTE far        *cache_mem;
WORD            cache_cghnd;
WORD            buffer_wnhnd;
BYTE far        *buffer_mem;
WORD            buffer_cghnd;
  
long                buffer_size;
long            cache_size;
FONTCONTEXT         fc;
  
  
IFCONFIG             cfg;
char                 lbuf[128];     /* may contain pathname too */
//char                 symmap[768];
  
BOOL     symbol_pass;
UWORD    ah_size, th_size, dh_size, cr_size, fa_size, chk_size;
WORD     ah_h, th_h, dh_h, cr_h, fa_h;
FACE_ATT far * ah;
typefaceHeaderType far * th;
DISPLAY  far * dh;
char     far * cr;
LPSTR          fa, fa_tbl;      // font alias segment & table
descriptorSetType far * set;
  
//PTMHEADER       ptm_h;
PFMHEADER       pfm_h;
PFMEXTENSION    pfm_ext;
EXTTEXTMETRIC   pfm_etm;
DRIVERINFO      pfm_di;
WORD            extent_tbl[256];
KERNPAIRS       krn_prs;
MFM             fm;     /* general 'more font metrics' info */
BYTE
face_name[FACESIZE];
BYTE            device_name[DEVSIZE];
BYTE         esc_str[ESCSIZE];
  
short zero_word = 0;
  
BYTE    tf_name[128]; /* becomes PFM name */
BYTE    supp_dir[128];
int     pfm_file;
OFSTRUCT    of_buf;
  
#ifdef PFMSONLY
WORD    pass_num = 0;   /* special pass counter for creating PFMSONLY file */
#endif
  
  
WORD        mp_h;   /* handle to master pairs memory */
LPCGPAIR    mp;     /* pointer "    "     "      "   */
short       mp_num; /* number of CG master pairs     */
  
WORD        mt_h;   /* handle to symbol mapping table memory */
LPMAPENTRY  mt;     /* pointer "    "     "      "      "    */
short       mt_num; /* number of char codes in our symbol set */
  
WORD            wCGIFcallerID;
  
  
LPSTR FAR PASCAL lmemcpy(LPSTR, LPSTR, WORD);
LPSTR FAR PASCAL lmemset(LPSTR, BYTE, WORD);
  
  
/* Local proc's */
  
BOOL FAR PASCAL           init_bullet(HANDLE, BOOL);
void FAR PASCAL           exit_bullet(HANDLE, BOOL);
BOOL FAR PASCAL           config(PIFCONFIG);
int  FAR PASCAL           get_symbol_info(DWORD, LPPFMHEADER,
FPFONTCONTEXT, BOOL, WORD);
int  FAR PASCAL           get_typeface_info(HANDLE, DWORD);
int  FAR PASCAL           set_font(HANDLE, DWORD, WORD, short, short, short);
  
/** Added on 11-12-90 (dtk) from IFW code **/
BOOL FAR PASCAL    MoveFamilyName(LPSTR, LPSTR, WORD);
/****/
  
static int             generate_metrics(HANDLE, WORD, LPSTR, int FAR *);
static int             write_pfm (int, LPPFMHEADER, WORD far *, LPPFMEXTENSION,
BYTE far *, BYTE far *, LPEXTTEXTMETRIC,
KERNPAIRS far *, LPDRIVERINFO, BYTE far *);
static BOOL            get_master_pairs (PSTR);
static BOOL            parse_line (char *, UWORD far *, UWORD far *);
static BOOL            get_symbol_map (UWORD, PSTR);
static BOOL            generate_krn_pairs (HANDLE);
static BOOL            map_CG_nums (LPCGPAIR, KERN_PAIR far *);
static void            release_krn_tbl (void);
  
/** (dtk) 12-3-90 from TD code **/
static WORD            get_weight (int);
static BYTE            get_face(int, int);
  
/** Added on 11-12-90 (dtk) from IFW code **/
static void    NEAR PASCAL    remove_string(LPSTR, LPSTR);
static int     NEAR PASCAL    lstrncmp(LPSTR, LPSTR, WORD);
static BOOL    NEAR PASCAL    myislower(BYTE);
static BOOL    NEAR PASCAL    myisupper(BYTE);
static LPSTR   NEAR PASCAL    mystrstr(LPSTR, LPSTR);
  
typedef struct
{
    LONG id;                /* typeface id */
    LONG complement;            /* character complement */
    LONG space_req;         /* space required for the library */
    BYTE typefaceName[51];              /* null terminated string */
    BYTE nameOrDir[MAXNAME];
    BYTE pad;     /* To make the if.dsc agree with the dos built version */
} TYPEINFO, FAR * LPTYPEINFO;
  
  
  
/****************************************************************************
  
FUNCTION:   build_ptm
  
PURPOSE:    Builds a ptm (printer typeface metrics) file for the specified
typeface (library) file.  The idea behind ptm's is similar to
pcm's where multiple pfm's are combined into a single file,
one pfm for each font.  Even though our fonts are scalable,
we still have a separate pfm for each font (as the typeface
is bound to any given symbol set).  For now, we only
generate a pfm for the Windows ANSI (WN) character set.
Two exceptions to this are for Zapf Dingbats and the Windows
SYMBOL character set.  Zapf Dingbats will only be available
with the Postscript ITC Zapf Dingbats (DS) character set, and
we have chosen to tie the Windows SYMBOL character set to the
CG Times typeface.  (Windows' SYMBOL character set is the same
as CG's Postcript Math (MS) charecter set and the Postscript
Symbol character set).  If a need for sans serif math symbols
arises, we may choose to offer it with Univers as well.
MESSAGES:
  
COMMENTS:   Returns TRUE if the call was successful, otherwise
FALSE indicates that we could not generate the PFM.
If tf == NULL on entry, this indicates that no more typefaces
need to be processed; we will release our internal resources
if this is the case.
  
****************************************************************************/
  
BOOL FAR PASCAL build_ptm(hPcleo, font_id, ssnum, pfm_name, font_name, BoldItal, checkTyp)
HANDLE          hPcleo;
DWORD           font_id;
WORD         ssnum;     /* Requested symbol set */
LPSTR           pfm_name;
LPSTR           font_name;
int FAR *BoldItal;
BOOL            checkTyp;
{
  
    BOOL            done;
    unsigned short  status;
    unsigned short  len, i;
    char            family_name[sizeof(th->familyName)+1];
    BOOL            got_mpairs;
    WORD            my_extent_tbl[256];
    BOOL            bullet_ready = FALSE;
    FARPROC         lpFunc;
  
  
    DBGbptm(("ENTER:  build_ptm\n"));
  
    *BoldItal = 0;
  
    if (!bullet_ready)
    {
        if(!init_bullet(hPcleo, FALSE))
        {
            DBGbptm(("build_ptm:  config failed\n"));
            exit_bullet(hPcleo, bullet_ready);  /* release any resources we did obtain */
            DBGbptm(("Exiting build_ptm:  couldn't initialize.\n"));
            return FALSE;
        }
        bullet_ready = TRUE;
    }
  
  
    if (checkTyp)
    {
        lpFunc = GetProcAddress(hPcleo, "CGIFsegments");
        if (lpFunc != (FARPROC) NULL)
        {
            if (status = (*lpFunc)((long)font_id, (UWORD)FONT_ALIAS_KEY, (UWORD FAR *)&chk_size, (LPSTR)NULL))
            {
                DBGbptm(("CGIFsegment 'getsize' status = %d\n", status ));
                exit_bullet(hPcleo, bullet_ready);  /* release any resources we did obtain */
                return FALSE;
            }
            else
            {
                if (chk_size == 0)
                {
                    exit_bullet(hPcleo, bullet_ready);  /* release any resources we did obtain */
                    return FALSE;
                }
  
                exit_bullet(hPcleo, bullet_ready);  /* release any resources we did obtain */
                return TRUE;
            }
  
        }
  
        else
        {
            exit_bullet(hPcleo, bullet_ready);  /* release any resources we did obtain */
            return FALSE;
        }
  
    }
  
  
  
  
//  /* make copy of support files dir */
//  lstrcpy((LPSTR)supp_dir, bullet_dir);
//  len = lstrlen((LPSTR)supp_dir);
//  if ((supp_dir[len-1] != '\\') &&  /* add a trailing '\' */
//      (supp_dir[len-1] != '/'))     /* (check if root dir) */
//          lstrcat((LPSTR)supp_dir, "\\");
  
//
// ???? don't have kerning info
//      got_mpairs = get_master_pairs(supp_dir);  /* only needed at beginning */
  
#ifdef PFMSONLY     /* quick and dirty way of creating our PFMSONLY file */
    pass_num++;
    if (pass_num == 1)
    {
        /* create PFMSONLY pathname */
        lstrcpy((LPSTR)tf_name, supp_dir);
        lstrcat((LPSTR)tf_name, (LPSTR)"PFMSONLY.NEW");
        /* This file must be renamed to 'PFMSONLY.PTM' for driver to find it. */
#else
//    /* create metrics file name */
//    lstrcpy((LPSTR)tf_name, tf->nameOrDir);
//    len = lstrlen((LPSTR)tf_name);
//    for (i=len-1; i>=0; i--)
//    {
//        if (tf_name[i] == '.')   /* if extension exists, remove it */
//        {
//                tf_name[i] = '\0';
//                break;
//        }
//        if ((tf_name[i] == '\\') ||  /* no extension to remove, just add one */
//            (tf_name[i] == '/'))
//                break;
//    }
//    lstrcat((LPSTR)tf_name, (LPSTR)".PTM");
//    lstrcat((LPSTR)tf_name, (LPSTR)".PFM");
//    lstrcpy((LPSTR)pfm_name, (LPSTR)tf_name);
//
//    len = i - 5;
//
//    tf_name[len++] = fa_tbl[0];
//    tf_name[len++] = fa_tbl[1];
//    tf_name[len] = fa_tbl[3];
//    len += 3;
//    tf_name[len++] = (BYTE)ssnum;
//    tf_name[len] = (BYTE)(ssnum >> 8);
//    lstrcat((LPSTR)tf_name, (LPSTR)"O.PFM");
//
//    DBGbptm(("build_ptm:  tf_name = %ls\n", (LPSTR)tf_name));
//    lstrcpy((LPSTR)pfm_name, (LPSTR)tf_name);
//
//
  
#endif
  
//
//    /* create actual file */
//    if ((pfm_file = OpenFile ((LPSTR)tf_name, (LPOFSTRUCT) &of_buf, OF_CREATE)) == -1)
//        return FALSE;           /* couldn't create it */
//    /* insert PTM header first; we'll update total size & family name later */
//    ptm_h.ptmMagic = PTM_MAGIC;     /* so as not to confuse w/ PCM's */
//    ptm_h.ptmVersion = PTM_VERSION; /* first version for now */
//    ptm_h.ptmSize = sizeof(ptm_h);  /* init size; we'll set real value later */
//    ptm_h.ptmNumPFMs = 0;           /* init for # of PFM's */
//    ptm_h.ptmPFMList = ptm_h.ptmSize;
//    ptm_h.ptmReserved = 0;          /* must be NULL for now */
//
//    if (_lwrite (pfm_file, (LPSTR)&ptm_h, sizeof(ptm_h)) != sizeof(ptm_h))
//    {
//        (void) OpenFile ((LPSTR)tf_name, (LPOFSTRUCT) &of_buf, OF_DELETE);
//        return FALSE;
//    }
  
#ifdef PFMSONLY
    }
#endif
  
    symbol_pass = FALSE;        /* if CG Times, do WN before Symbol char set */
    done = FALSE;
    status = 0;
  
    while (!done)
    {
        /* initialize our internal tables, etc. */
        lmemset((LPSTR)&pfm_h, 0, sizeof(pfm_h));
        lmemset((LPSTR)extent_tbl, 0, sizeof(extent_tbl));
        lmemset((LPSTR)&pfm_ext, 0, sizeof(pfm_ext));
        lmemset((LPSTR)face_name, 0, sizeof(face_name));
        lmemset((LPSTR)device_name, 0, sizeof(device_name));
        lmemset((LPSTR)&pfm_etm, 0, sizeof(pfm_etm));
        lmemset((LPSTR)&pfm_di, 0, sizeof(pfm_di));
        lmemset((LPSTR)esc_str, 0, sizeof(esc_str));
  
  
  
  
  
  
        /* tell Bullet what font we're talking about -- was below call
        to get_symbol_info
        */
        if (status = set_font(hPcleo, font_id, ssnum, 250,(short)HDPI,(short)VDPI))
            break;
  
  
  
  
  
        /* get the symbol-set specific info that we'll need */
        if (status = get_symbol_info(font_id, (LPPFMHEADER)&pfm_h,
            (FONTCONTEXT far *)&fc, symbol_pass, ssnum))
            break;
  
  
  
  
  
  
  
  
  
        /* fill in our info tables with font and typeface data */
        if (status = get_typeface_info(hPcleo, font_id))
        {
//            release_typeface_info();    /* return anything we did get */
            break;
        }
  
        /*** Scary stuff -- if we loop, we die ***/
        DBGbptm(("build_ptm:  Point 1\n"));
  
        /* create metrics file name */
//      lstrcpy((LPSTR)tf_name, tf->nameOrDir);
        lstrcpy((LPSTR)tf_name, pfm_name);
        lstrcat((LPSTR)tf_name, (LPSTR)"\\");
        DBGbptm(("pfm_name1 in $sfpfm.c = %ls\n", (LPSTR)tf_name));
  
        len = lstrlen((LPSTR)tf_name);
  
//        for (i=len-1; i>=0; i--)
//        {
//            if (tf_name[i] == '.')   /* if extension exists, remove it */
//            {
//                    tf_name[i] = '\0';
//                    break;
//            }
//            if ((tf_name[i] == '\\') ||  /* no extension to remove, just add one */
//                (tf_name[i] == '/'))
//                    break;
//        }
        DBGbptm(("build_ptm:  Point 2\n"));
  
//        len = i - 5;
  
        DBGbptm(("build_ptm:  Point 3\n"));
  
        fa_tbl = (LPSTR)((LPFONTALIASDATATYPE)fa)->fontAliasTable[0].xRefArray;
        tf_name[len++] = fa_tbl[0];
        tf_name[len++] = fa_tbl[1];
        tf_name[len++] = fa_tbl[3];
        tf_name[len++] = '0';
        tf_name[len++] = '0';
  
        DBGbptm(("build_ptm:  Point 4\n"));
  
        tf_name[len++] = (BYTE)ssnum;
        tf_name[len++] = (BYTE)(ssnum >> 8);
        tf_name[len] = '\0';
  
        DBGbptm(("build_ptm:  Point 5\n"));
  
        lstrcat((LPSTR)tf_name, (LPSTR)"O.PFM");
  
        DBGbptm(("build_ptm:  tf_name = %ls\n", (LPSTR)tf_name));
        lstrcpy((LPSTR)pfm_name, (LPSTR)tf_name);
        DBGbptm(("pfm_name2 in $sfpfm.c = %ls\n", (LPSTR)pfm_name));
  
        /* create actual file */
        if ((pfm_file = OpenFile ((LPSTR)tf_name, (LPOFSTRUCT) &of_buf, OF_CREATE)) == -1)
        {
            exit_bullet(hPcleo, bullet_ready);
            return FALSE;           /* couldn't create it */
        }
        /*** End of scary stuff ***/
  
        krn_prs.npairs = 0;     /* initialize in case there are none */
//        if ((!TSTFIXEDPITCH()) &&  /* gen kern prs for variable pitch only */
//            got_mpairs &&
//            get_symbol_map(fc.ssnum, supp_dir))
//                (void) generate_krn_pairs();
  
        /* calculate & format the metrics we need */
        status = generate_metrics(hPcleo, ssnum, font_name, BoldItal);
  
//        release_typeface_info();    /* all through with typeface segments */
  
        if (status)                 /* if couldn't create metrics, bail out */
            break;
  
        status = write_pfm(pfm_file, &pfm_h, extent_tbl, &pfm_ext, face_name,
        device_name, &pfm_etm, &krn_prs, &pfm_di, esc_str);
  
        release_krn_tbl();  /* return mem for this font's kern pair table */
//      ptm_h.ptmNumPFMs++;         /* bump up # of PFM's */
  
        if (status)         /* if couldn't write data, bail out */
            break;
  
//        if (((tf->id != CGTIMESUID) &&
//             (tf->id != CGTIMESIID) &&
//             (tf->id != CGTIMESBID) &&
//             (tf->id != CGTIMESBIID)) ||
//            symbol_pass)        /* just finished symbol pass ? */
//                done = TRUE;
//        else
//                symbol_pass = TRUE;     /* need 2 passes for CG Times */
  
        done = TRUE;   /* Just a PFM -- Never ever loop ever on earth */
    }
  
    /*
    Since we're adding each typeface to the if.fnt index file
    one at a time, and Bullet caches it in memory after it starts up,
    we're going to have to exit Bullet EACH time in order to force
    it to re-read if.fnt and get the most recent data.
    */
  
    /* No more PFM's so go back and fill in total file size in PTM */
  
#ifdef PFMSONLY
    if (pass_num == 9)    /* (3 base typefaces) * (Italic, Bold, Bold-Italic) */
    {                     /* currently contains 12 PFM's (9 WN's, 3 MS's) */
        pass_num = 0;                               /* reset pass count */
#endif
  
//    temp_size = ptm_h.ptmSize;                  /* save real size */
//    temp_num = ptm_h.ptmNumPFMs;                /* save # of PFM's */
//    if ((status) ||        /* if couldn't create a PFM, delete the file */
//        (_llseek (pfm_file, 0L, 0) == -1) ||    /* go to start of file */
//        (_lread (pfm_file, (LPSTR)&ptm_h, sizeof(ptm_h)) != sizeof(ptm_h)) ||
//        (_llseek (pfm_file, 0L, 0) == -1) ||    /* go to start of file */
//        (!(ptm_h.ptmSize = temp_size)) ||       /* update total length */
//        (!(ptm_h.ptmNumPFMs = temp_num)) ||     /* update total # of PFM's */
//        (_lwrite (pfm_file, (LPSTR)&ptm_h, sizeof(ptm_h)) != sizeof(ptm_h)))
  
        if (status)        /* if couldn't create a PFM, delete the file */
        {
            (void) OpenFile ((LPSTR)tf_name, (LPOFSTRUCT) &of_buf, OF_DELETE);
            return FALSE;
        }
  
        _lclose (pfm_file);     /* everything was OK */
#ifdef PFMSONLY
    }
    if (status)     /* if couldn't create a part of PFMSONLY, return error */
    {
        return FALSE;
    }
#endif
  
    /* shut down bullet before we return
    */
    exit_bullet(hPcleo, bullet_ready);
    return TRUE;
  
}
  
  
/****************************************************************************
  
FUNCTION:   init_bullet
  
PURPOSE:    Initializes the bullet sub-system for use during pfm
generation.
  
MESSAGES:
  
COMMENTS:
  
  
****************************************************************************/
  
BOOL FAR PASCAL   init_bullet(hPcleo, max_cache)
HANDLE      hPcleo;
BOOL        max_cache;
{
  
    unsigned short status;
    short          len, i;
    unsigned long   ulCacheSize;
    unsigned long  ulBufferSize;
    FARPROC        lpFunc;
  
    DBGbptm(("ENTER:  init_bullet\n"));
  
    if(!config(&cfg))
    {
        DBGbptm(("init_bullet:  config failed\n"));
        return FALSE;
    }
  
    if((cc_buf_wnhnd = GlobalAlloc(GMEM_MOVEABLE,(long)cfg.cc_buf_size)) == NULL)
    {
        DBGbptm(("init_bullet:  Global Alloc failed - char buffer\n"));
        return FALSE;
    }
  
    if((cfg.cc_buf_ptr = GlobalLock(cc_buf_wnhnd)) == (LPSTR)NULL)
    {
        DBGbptm(("init_bullet:  GlobalLock failed - char buffer\n"));
        return FALSE;
    }
  
  
    lpFunc = GetProcAddress(hPcleo, "CGIFinit");
    if (lpFunc != (FARPROC) NULL)
    {
        if(status = (*lpFunc)())
        {
            return FALSE;
            DBGbptm(("init_bullet:  CGIFinit failed,  status = %d\n", status));
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
            DBGbptm(("init_bullet:  CGIFconfig failed,  status = %d\n", status));
        }
    }
    else
        return FALSE;
  
  
    if((buffer_wnhnd = GlobalAlloc(GMEM_MOVEABLE, (long)buffer_size)) == NULL)
    {
        DBGbptm(("init_bullet:  GlobalAlloc failed,  - buffer mem\n"));
        return FALSE;
    }
  
    if((buffer_mem = GlobalLock(buffer_wnhnd)) == (LPSTR)NULL)
    {
        DBGbptm(("init_bullet:  GlobalLock failed,  - buffer mem\n"));
        return FALSE;
    }
  
  
    lpFunc = GetProcAddress(hPcleo, "CGIFfund");
    if (lpFunc != (FARPROC) NULL)
    {
        if(status = (*lpFunc)((UWORD)BUFFER_POOL, (LPSTR)buffer_mem, (LONG)buffer_size, (FPWORD) &buffer_cghnd))
        {
            return FALSE;
            DBGbptm(("init_bullet:  CGIFfund failed,  status = %d\n", status));
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
            DBGbptm(("init_bullet:  CGIFenter failed,  status = %d\n", status));
        }
    }
    else
        return FALSE;
  
    return (TRUE);
}
  
  
/****************************************************************************
  
FUNCTION:   exit_bullet
  
PURPOSE:    Shuts down the bullet sub-system and releases the appropriate
resources back to Windows.
  
MESSAGES:
  
COMMENTS:
  
  
****************************************************************************/
  
void FAR PASCAL   exit_bullet(hPcleo, bullet_ready)
HANDLE hPcleo;
BOOL  bullet_ready;
{
  
    FARPROC      lpFunc;
  
    DBGbptm(("ENTER:  exit_bullet\n"));
  
    if (bullet_ready)     /* call only if we have gone through CGIFenter */
    {
        lpFunc = GetProcAddress(hPcleo, "CGIFexit");
        if (lpFunc != (FARPROC) NULL)
            (*lpFunc)();
    }
  
  
    if(cfg.cc_buf_ptr)
    {
        GlobalUnlock(cc_buf_wnhnd);
        cfg.cc_buf_ptr = 0;
        DBGbptm(("exit_bullet:  GlobalUnlock failed - char buffer\n"));
    }
  
    if(cc_buf_wnhnd)
    {
        GlobalFree(cc_buf_wnhnd);
        cc_buf_wnhnd = 0;
        DBGbptm(("exit_bullet:  GlobalFree failed - char buffer\n"));
    }
  
  
    if(buffer_mem)
    {
        GlobalUnlock(buffer_wnhnd);
        buffer_mem = 0;
        DBGbptm(("exit_bullet:  GlobalUnlock failed - buffer memory\n"));
    }
  
    if(buffer_wnhnd)
    {
        GlobalFree(buffer_wnhnd);
        buffer_wnhnd = 0;
        DBGbptm(("exit_bullet:  GlobalFree failed - buffer memory\n"));
    }
  
  
    if(ah)
    {
        GlobalUnlock(ah_h);
        DBGbptm(("exit_bullet:  GlobalUnlock failed - buffer memory\n"));
        ah=0;
    }
  
    if(ah_h)
    {
        GlobalFree(ah_h);
        ah_h = 0;
        DBGbptm(("exit_bullet:  GlobalFree failed - buffer memory\n"));
    }
  
  
    if(th)
    {
        GlobalUnlock(th_h);
        DBGbptm(("exit_bullet:  GlobalUnlock failed - buffer memory\n"));
        th=0;
    }
  
    if(th_h)
    {
        GlobalFree(th_h);
        th_h = 0;
        DBGbptm(("exit_bullet:  GlobalFree failed - buffer memory\n"));
    }
  
  
    if(dh)
    {
        GlobalUnlock(dh_h);
        DBGbptm(("exit_bullet:  GlobalUnlock failed - buffer memory\n"));
        dh=0;
    }
  
    if(dh_h)
    {
        GlobalFree(dh_h);
        dh_h = 0;
        DBGbptm(("exit_bullet:  GlobalFree failed - buffer memory\n"));
    }
  
  
    if(fa)
    {
        GlobalUnlock(fa_h);
        DBGbptm(("exit_bullet:  GlobalUnlock failed - buffer memory\n"));
        fa=0;
    }
  
    if(fa_h)
    {
        GlobalFree(fa_h);
        fa_h = 0;
        DBGbptm(("exit_bullet:  GlobalFree failed - buffer memory\n"));
    }
  
  
    if(cr)
    {
        GlobalUnlock(cr_h);
        DBGbptm(("exit_bullet:  GlobalUnlock failed - buffer memory\n"));
        cr=0;
    }
  
    if(cr_h)
    {
        GlobalFree(cr_h);
        cr_h = 0;
        DBGbptm(("exit_bullet:  GlobalFree failed - buffer memory\n"));
    }
  
  
    if(mp)
    {
        GlobalUnlock(mp_h);
        DBGbptm(("exit_bullet:  GlobalUnlock failed - buffer memory\n"));
        mp=0;
    }
  
    if(mp_h)
    {
        GlobalFree(mp_h);
        mp_h = 0;
        DBGbptm(("exit_bullet:  GlobalFree failed - buffer memory\n"));
    }
  
    bullet_ready = FALSE;
  
}
  
  
/****************************************************************************\
  
FUNCTION: config()
  
PURPOSE: read configuration paramaters:
  
MESSAGES:
  
COMMENTS:
  
\****************************************************************************/
  
BOOL FAR PASCAL config(cfg)
PIFCONFIG       cfg;
{
    short   len, i;
  
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
  
FUNCTION: get_symbol_info()
  
PURPOSE: Gets all the metrics that are bound to a given symbol set.
e.g.  first and last chars, etc.  It fills in the appropriate
fields in the pfm header and font context structures.
  
MESSAGES:
  
COMMENTS:   Since we currently use only the Windows (WN) character set
for all typefaces (with the exception of Zapf Dingbats),
some of the information is hard coded in this routine.
If we later allow the user to generate multiple character
sets or even define their own, then we should parse the
'if.ss' file to get this info.
Returns a 0 to indicate good status.
  
\****************************************************************************/
  
int FAR PASCAL get_symbol_info(font_id, pfm_h, fc, symbol_pass, ssnum)
DWORD   font_id;        /* typeface # */
LPPFMHEADER pfm_h;      /* ptr to pfm header */
FPFONTCONTEXT fc;       /* ptr to font context structure to use */
BOOL       symbol_pass; /* special Symbol charset flag for CG Times */
WORD       ssnum;    /* Requested symbol set */
{
  
//    if (font_id == CGDINGBATSID)        /* ITC Zapf Dingbats typeface */
//    {
//            pfm_h->dfFirstChar = 32;
//            pfm_h->dfLastChar = 255;
//* FIXME...are the next 2 values reasonable ? */
//            pfm_h->dfBreakChar = (BYTE)' ' - pfm_h->dfFirstChar;
// black box
//            fc->ssnum = ('S' << 8) | 'D';    /* DS = Postscript Dingbats */
//    }
  
//    /* CG TIMES - gets both WN and MS char sets */
//    else if (((font_id == CGTIMESUID) ||
//              (font_id == CGTIMESIID) ||
//              (font_id == CGTIMESBID) ||
//              (font_id == CGTIMESBIID)) &&
//             (symbol_pass))    /* special case for Windows SYMBOL char set */
//    {
//                pfm_h->dfFirstChar = 32;
//                pfm_h->dfLastChar = 254;
//                pfm_h->dfBreakChar = (BYTE)' ' - pfm_h->dfFirstChar;
// '_'
//                fc->ssnum = ('S' << 8) | 'M';    /* MS = Windows SYMBOL  */
//    }                           /* (same mapping as CG postscript math) */
  
//    else                            /* WN = Windows regular char set */
//    {
//            pfm_h->dfFirstChar = 32; /* using current Windows char set */
//            pfm_h->dfLastChar = 255;
//            pfm_h->dfBreakChar = (BYTE)' ' - pfm_h->dfFirstChar;
//            pfm_h->dfDefaultChar = HP_DF_CH - pfm_h->dfFirstChar;
//            fc->ssnum = ('N' << 8) | 'W';
//    }
  
    pfm_h->dfFirstChar = 0;
//  pfm_h->dfFirstChar = 32;
    pfm_h->dfLastChar = 255;
    pfm_h->dfBreakChar = (BYTE)' ' - pfm_h->dfFirstChar;
    pfm_h->dfDefaultChar = HP_DF_CH;
//    fc->ssnum = ssnum;
  
    DBGbptm(("ssnum = %c%c\n", (BYTE)ssnum, (ssnum >> 8)));
  
    switch (ssnum)
    {
        case ZAPF100:
        case ZAPF200:
        case ZAPF300:
//      pfm_h->dfDefaultChar = (BYTE)124 - pfm_h->dfFirstChar;
            break;
        case ZAPFPS:
//      pfm_h->dfDefaultChar = (BYTE)110 - pfm_h->dfFirstChar;
            break;
        case ASCII:
        case PIFONT:
        case LEGAL:
            pfm_h->dfLastChar = 127;
            break;
        case PSMATH:
//             pfm_h->dfDefaultChar = (BYTE)183 - pfm_h->dfFirstChar;
            pfm_h->dfLastChar = 254;
            break;
        case MSPUB:
//          pfm_h->dfDefaultChar = (BYTE)185 - pfm_h->dfFirstChar;
            pfm_h->dfLastChar = 248;
            break;
        case PC8DN:
        case PC8:
        case PC850:
//      pfm_h->dfDefaultChar = (BYTE)177 - pfm_h->dfFirstChar;
            pfm_h->dfFirstChar = 1;
            /* Note: no break; */
        case DESKTOP:
        case MATH8:
        case ROMAN8:
            pfm_h->dfLastChar = 254;
            break;
        case PSTEXT:
//      pfm_h->dfDefaultChar = (BYTE)183 - pfm_h->dfFirstChar;
            pfm_h->dfLastChar = 251;
            break;
        case VENTMATH:
//      pfm_h->dfDefaultChar = (BYTE)203 - pfm_h->dfFirstChar;
            break;
        case VENTUS:
//      pfm_h->dfDefaultChar = (BYTE)252 - pfm_h->dfFirstChar;
            break;
        case ECMA94:
        case VENTINT:
        case WINSET:
            break;
//  default:
//      fc->ssnum = WINSET; /* Unknown symset -- makeit WN */
    }
  
    DBGbptm(("pfm_h->dfFirstChar = %d (%c)\n", pfm_h->dfFirstChar,
    pfm_h->dfFirstChar));
    DBGbptm(("pfm_h->dfLastChar = %d (%c)\n", pfm_h->dfLastChar,
    pfm_h->dfLastChar));
    DBGbptm(("pfm_h->dfBreakChar = %d (%c)\n", pfm_h->dfBreakChar,
    pfm_h->dfBreakChar));
    DBGbptm(("pfm_h->dfDefaultChar = %d (%c)\n", pfm_h->dfDefaultChar,
    pfm_h->dfDefaultChar));
//       DBGbptm(("fc->ssnum = %c%c\n", (BYTE)fc->ssnum, (fc->ssnum >> 8)));
  
    return FALSE;      /* always return good status for now */
}
  
  
/****************************************************************************\
  
FUNCTION: set_font()
  
PURPOSE:  Sets the current CG font context.
  
MESSAGES:
  
COMMENTS:   We assume on entry that the 'ssnum' field already contains
the necessary 2-char symbol set code.
Returns non-zero value if an error occurred, otherwise
returns 0.
  
\****************************************************************************/
  
int FAR PASCAL set_font (hPcleo, font_id, ssnum, point_size, xRes, yRes)
HANDLE          hPcleo;
DWORD           font_id;    /* typeface # */
WORD            ssnum;      /* ss # */
short           point_size;
short xRes, yRes;       //Reslution in DPM
{
    unsigned short  status;
    FARPROC         lpFunc;
  
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
    fc.ssnum      = ssnum;
  
    DBGbptm(("Open font file\n"));
  
  
    lpFunc = GetProcAddress(hPcleo, "CGIFfont");
    if (lpFunc != (FARPROC) NULL)
    {
        if(status = (*lpFunc)((FPFONTCONTEXT)&fc))
        {
            DBGbptm(("CGIFfont failed,  status = %d\n", status));
            return TRUE;
        }
    }
    else
        return TRUE;
  
  
    return FALSE;       /* everything went OK */
  
}
  
  
/****************************************************************************\
  
FUNCTION: get_typface_info()
  
PURPOSE:  Gets most of the necessary typeface info in preparation
for building the PTM/PFM file.  This data is not
font (symbol set) specific.
  
MESSAGES:
  
COMMENTS:
  
\****************************************************************************/
  
int FAR PASCAL get_typeface_info (hPcleo, font_id)
HANDLE         hPcleo;
DWORD          font_id;    /* typeface # */
{
    unsigned short  status;
    FARPROC         lpFunc;
  
    /*
    Sizes for each of the segments could be different in various
    typeface files so we should probably get the buffers each time.
    */
  
  
    lpFunc = GetProcAddress(hPcleo, "CGIFsegments");
    if (lpFunc != (FARPROC) NULL)
    {
        if ((status = (*lpFunc)((long)font_id, (UWORD)TF_HEADER_KEY, (UWORD FAR *)&th_size, (LPSTR)NULL)) ||
            (status = (*lpFunc)((long)font_id, (UWORD)ATTRIBUTE_KEY, (UWORD FAR *)&ah_size, (LPSTR)NULL)) ||
            (status = (*lpFunc)((long)font_id, (UWORD)DISPLAY_KEY, (UWORD FAR *)&dh_size, (LPSTR)NULL)) ||
            (status = (*lpFunc)((long)font_id, (UWORD)FONT_ALIAS_KEY, (UWORD FAR *)&fa_size, (LPSTR)NULL)) ||
            (status = (*lpFunc)((long)font_id, (UWORD)COPYRIGHT_KEY, (UWORD FAR *)&cr_size, (LPSTR)NULL)))
        {
            DBGbptm(("CGIFsegment 'getsize' status = %d\n", status ));
            return TRUE;
        }
    }
    else
        return TRUE;
  
  
    /* allocate and lock the segment buffers */
  
    if ( ((th_h = GlobalAlloc(GMEM_MOVEABLE, (long)th_size)) == NULL) ||
        ((ah_h = GlobalAlloc(GMEM_MOVEABLE, (long)ah_size)) == NULL) ||
        ((dh_h = GlobalAlloc(GMEM_MOVEABLE, (long)dh_size)) == NULL) ||
        ((fa_h = GlobalAlloc(GMEM_MOVEABLE, (long)fa_size)) == NULL) ||
        ((cr_h = GlobalAlloc(GMEM_MOVEABLE, (long)cr_size)) == NULL))
    {
        DBGbptm(("Global Alloc Failed inside get_typeface_info\n"));
        return TRUE;
    }
  
    if ( ((th = (typefaceHeaderType far *) GlobalLock(th_h)) == (LPSTR)NULL) ||
        ((ah = (FACE_ATT far *)           GlobalLock(ah_h)) == (LPSTR)NULL) ||
        ((dh = (DISPLAY far *)            GlobalLock(dh_h)) == (LPSTR)NULL) ||
        ((fa = (LPSTR)                    GlobalLock(fa_h)) == (LPSTR)NULL) ||
        ((cr = (char far *)               GlobalLock(cr_h)) == (LPSTR)NULL))
    {
        DBGbptm(("Gobal Lock Failed inside get_typeface_info\n"));
        return TRUE;
    }
  
    /* get pointers to the necessary segments for this typeface */
    if ((status = (*lpFunc)((long)font_id, (UWORD)TF_HEADER_KEY, (UWORD FAR *)&th_size, (FPBYTE)th)) ||
        (status = (*lpFunc)((long)font_id, (UWORD)ATTRIBUTE_KEY, (UWORD FAR *)&ah_size, (FPBYTE)ah)) ||
        (status = (*lpFunc)((long)font_id, (UWORD)DISPLAY_KEY, (UWORD FAR *)&dh_size, (FPBYTE)dh)) ||
        (status = (*lpFunc)((long)font_id, (UWORD)FONT_ALIAS_KEY, (UWORD FAR *)&fa_size, (FPBYTE)fa)) ||
        (status = (*lpFunc)((long)font_id, (UWORD)COPYRIGHT_KEY, (UWORD FAR *)&cr_size, (FPBYTE)cr)))
    {
        DBGbptm(("CGIFsegment 'getdata' status = %d\n", status ));
        return TRUE;
    }
  
    /* FIXME...shouldn't we search for the id just in case ? */
    set = (descriptorSetType far *)&th->typeFaceSet[1] ;
  
    return FALSE;       /* everything went OK */
}
  
  
/****************************************************************************\
  
FUNCTION: generate_metrics()
  
PURPOSE:  Fills in the necessary structures with the correct metrics.
  
MESSAGES:
  
COMMENTS:   We assume on entry that the CG font context has already
been established (necessary for width info, etc. - things
which are bound to single font and not the typeface in
general).  The typeface segments must also be present in
memory.
Returns non-zero value if an error occurred, otherwise
returns 0.
  
\****************************************************************************/
  
static int generate_metrics (hPcleo, ssnum, font_name, BoldItal)
HANDLE         hPcleo;
WORD           ssnum;                  /* requested symbol set */
LPSTR  font_name;           /* font name to return */
int FAR * BoldItal;         /* bold and/or italic designator */
{
    unsigned short  status;
    unsigned short  max, num_chars, i;
    long            sum;
    int             style;
    int             pclweight;
    int             pclstyle;
    int             posture;
    BYTE            fa_face_name[20];
    BYTE            fa_posture[2];
    BYTE            fa_stroke_wt[3];
    BYTE            fa_serif_style[3];
    BYTE            tempBuffer[20];
    BYTE            SSstr[3];
    FARPROC         lpFunc;
  
  
  
    /* get extent tbl (design units) */
    /* char codes outside ss will have 0 widths */
  
    lpFunc = GetProcAddress(hPcleo, "CGIFwidth");
    if (lpFunc != (FARPROC) NULL)
    {
        if(status = (*lpFunc)((UWORD)0, (UWORD)255, (UWORD)512, (FPWORD)extent_tbl))
        {
            DBGbptm(("CGIFwidth failed,  status = %d\n", status));
            return TRUE;
        }
    }
    else
        return TRUE;
  
  
    /* Fix the extent table so it does not contain */
    /* zero widths. Like Type Director, we will give the */
    /* width of the space char for undefined chars. */
  
    for (i = 32; i <= pfm_h.dfLastChar; i++)
    {
        if (extent_tbl[i] == 0)
            extent_tbl[i] = extent_tbl[32];
    }
  
  
    /* generate metrics which are used in other calculations */
  
    fm.ptsize = dh->masterPointSize / 8 * PTRES;
    fm.setsize = dh->masterPointSize / 8 * STRES;
    fm.spacesize = ah->spaceBand;     /* size of space          */
    if (fm.spacesize == 0)
        fm.spacesize = ah->fixedSpaceRelWidths[0];  /* 1/3 EM space     */
    fm.emsize = ah->scaleFactor;     /* size of em              */
  
    /* The following 'FIXEM' values were also hardcoded in type director... */
    /* They are retained right now for consistency */
    fm.baseline = BASELINE;        /* Baseline Dist (design)        */
    fm.cellwidth = CELLWIDTH;      /* Cell Width (design)       */
    fm.cellheight = CELLHEIGHT;    /* Cell height (design)  */
    fm.extleading = LEADING;       /* optimal leading (design)      */
    fm.intleading = 0;
    fm.charspacing = ah->fixedSpaceRelWidths[1];    /* EN space     */
    /* optimal character spacing (design)   */
  
    /* pfm header */
    pfm_h.dfVersion = 0x100;
    lstrncpy (pfm_h.dfCopyright, cr, sizeof(pfm_h.dfCopyright));
    pfm_h.dfType = 0x80;        /* see SDK for definition */
    pfm_h.dfPoints = (dh->masterPointSize + 4) / 8;  /* dh has 1/8 pt. units */
    pfm_h.dfVertRes = VDPI;     /* vertical res in dpi      */
    pfm_h.dfHorizRes = HDPI;    /* horizontal res in dpi    */
  
    /* the following formula was 'baseline * (PTPIN/HPPTPIN)' */
//  pfm_h.dfAscent = (short) ldiv(lmul((long)fm.baseline, 7231L)+3600L, 7200L);
    pfm_h.dfAscent = BASELINE;
    pfm_h.dfInternalLeading = fm.intleading;
    pfm_h.dfExternalLeading = fm.extleading;
  
    lmemset(fa_posture, 0, sizeof(fa_posture));
    lmemcpy(fa_posture, fa_tbl+POSTURE_POS, POSTURE_LEN);
    fa_posture[sizeof(fa_posture)-1] = '\0';
  
    DBGbptm(("posture string = %ls\n", (LPSTR)fa_posture));
  
    posture = atoi(fa_posture);
  
    DBGbptm(("posture int = %d\n", posture));
  
    if((posture == 1) || (posture == 2))
    {
        pfm_h.dfItalic = 1;
  
        *BoldItal |= DESC_ITAL;
    }
    else
        pfm_h.dfItalic = 0;
  
    DBGbptm(("posture int (pfm) = %d\n", pfm_h.dfWeight));
  
//  if (TSTITALIC()) /* tstitalic() does not work correctly - dtk */
//      pfm_h.dfItalic = 1;
//  else
//      pfm_h.dfItalic = 0;
  
  
  
    pfm_h.dfUnderline = 0;          /* no underline fonts       */
    pfm_h.dfStrikeOut = 0;          /* no strikeout fonts       */
  
    lmemset(fa_stroke_wt, 0, sizeof(fa_stroke_wt));
    lmemcpy(fa_stroke_wt, fa_tbl+STROKEWT_POS, STROKEWT_LEN);
    fa_stroke_wt[sizeof(fa_stroke_wt)-1] = '\0';
  
    DBGbptm(("fa_tbl    string = %ls\n", (LPSTR)fa_tbl));
    DBGbptm(("stroke_wt string = %ls\n", (LPSTR)fa_stroke_wt));
  
    pclweight = atoi(fa_stroke_wt);
  
    DBGbptm(("stroke_wt int = %d\n", pclweight));
  
    if ((pfm_h.dfWeight = get_weight((pclweight - 8))) > FW_NORMAL)
        *BoldItal |= DESC_BOLD;
  
    DBGbptm(("stroke_wt int (pfm) = %d\n", pfm_h.dfWeight));
  
    if (symbol_pass)
        pfm_h.dfCharSet = 2;     /* needed for font matching / realization */
    else
        pfm_h.dfCharSet = 0;
  
    pfm_h.dfPixWidth = 0;           /* all scalable fonts - use extent tbl */
    pfm_h.dfPixHeight = 0;
  
    lmemset(fa_serif_style, 0, sizeof(fa_serif_style));
    lmemcpy(fa_serif_style, fa_tbl+SERIF_POS, SERIF_LEN);
    fa_serif_style[sizeof(fa_serif_style)-1] = '\0';
  
    DBGbptm(("serif_style string = %ls\n", (LPSTR)fa_serif_style));
  
    pclstyle = atoi(fa_serif_style);
  
    DBGbptm(("serif_style int = %d\n", pclstyle));
  
    pfm_h.dfPitchAndFamily = get_face(pclstyle, ah->isFixedPitch);
  
    DBGbptm(("pitch and family (pfm) = %d\n", pfm_h.dfPitchAndFamily));
  
  
    if (TSTFIXEDPITCH())
    {
        pfm_h.dfAvgWidth =
        pfm_h.dfPixWidth =
        pfm_h.dfMaxWidth = fm.spacesize;
    }
    else
    {
        num_chars = 0;
//      sum = 0;
        max = 0;  /* find the max char width */
        for (i = pfm_h.dfFirstChar; i <= pfm_h.dfLastChar; i++)
            if (extent_tbl[i] != 0)  /* Is it in the char set ? */
            {
                num_chars++;
//              sum += extent_tbl[i];
                if (extent_tbl[i] > max)
                    max = extent_tbl[i];
            }
//        pfm_h.dfAvgWidth = (WORD) ldiv(sum, (long)num_chars); /* no rounding */
  
        /* avg width should be the width of [X] or 2 * [space] */
  
        if (extent_tbl[88] == 0)
            pfm_h.dfAvgWidth = (2 * (extent_tbl[32]));
        else
            pfm_h.dfAvgWidth = extent_tbl[88];
  
        pfm_h.dfMaxWidth = max;
    }
  
    pfm_h.dfWidthBytes = 0;     /* ignored for scalable fonts */
  
    /* offset is size of pfm header + extent tbl + extension */
  
    pfm_h.dfFace =  sizeof(pfm_h) - sizeof(pfm_h.dfCharOffset) +
    sizeof(pfm_ext);
  
    if (!TSTFIXEDPITCH())
        pfm_h.dfFace += ((pfm_h.dfLastChar - pfm_h.dfFirstChar + 2) * 2);
  
    pfm_h.dfDevice = pfm_h.dfFace + FACESIZE;
    pfm_h.dfBitsPointer = NULL;     /* ignored */
    pfm_h.dfBitsOffset = NULL;      /* ignored */
    pfm_h.dfSize = 0;               /* length of this PFM (changed later) */
  
    // We will use the abbreviated typeface name found in the font alias seg.
    // This is what IFW uses (we will expand / strip it later in the same way)
  
    lmemset(fa_face_name, 0, sizeof(fa_face_name));
    lmemcpy(fa_face_name, fa_tbl+FACENAME_POS, FACENAME_LEN);
    fa_face_name[sizeof(fa_face_name)-1] = '\0';
  
    lstrcpy((LPSTR)tempBuffer, (LPSTR)fa_face_name);
  
    DBGbptm(("face name (before movefamname) = %ls\n", (LPSTR)tempBuffer));
  
    /*** Call MoveFamilyName to strip the face name so that it ***
    *** agrees with the Type Director face name           ***/
  
    MoveFamilyName((LPSTR)face_name, (LPSTR)tempBuffer, sizeof(face_name));
  
    lstrcat((LPSTR)face_name, " (");
  
    SSstr[0] = LOBYTE((WORD)ssnum);
    SSstr[1] = (BYTE)(HIBYTE((WORD)ssnum));
    SSstr[2] = NULL;
  
    lstrcat((LPSTR)face_name, (LPSTR)SSstr);
    lstrcat((LPSTR)face_name, ")");
  
    /* copy face to return to calling procedures */
    lstrcpy(font_name, (LPSTR)face_name);
  
    DBGbptm(("face name (after movefamname) = %ls\n", (LPSTR)face_name));
  
  
    lstrncpy(device_name, "PCL5 / HP LaserJet III", sizeof(device_name));
  
    /* PFM Extension */
    pfm_ext.dfSizeFields = sizeof (pfm_ext);
    pfm_ext.dfExtMetricsOffset = pfm_h.dfDevice + DEVSIZE;
    if(TSTFIXEDPITCH())
        pfm_ext.dfExtentTable = 0;
    else
        pfm_ext.dfExtentTable = sizeof(pfm_h)-sizeof(pfm_h.dfCharOffset);
  
    pfm_ext.dfOriginTable = NULL;
    pfm_ext.dfTrackKernTable = NULL;
    pfm_ext.dfPairKernTable = pfm_ext.dfExtMetricsOffset + sizeof (pfm_etm);
    pfm_ext.dfDriverInfo = pfm_ext.dfPairKernTable +
    (krn_prs.npairs * sizeof(KERNPAIR));
    if (krn_prs.npairs == 0)
        pfm_ext.dfPairKernTable = NULL;
    pfm_ext.dfReserved = NULL;
  
    /* we now have the total pfm length to put into the header */
    pfm_h.dfSize = pfm_ext.dfDriverInfo + sizeof (pfm_di);
  
//  ptm_h.ptmSize += pfm_h.dfSize;  /* update total file length */
  
    /* Extended Text Metrics......*/
    pfm_etm.emSize = sizeof (pfm_etm);
    pfm_etm.emPointSize = pfm_h.dfPoints * 20;    /* twentieths of a point*/
    pfm_etm.emOrientation = 0;   /* all fonts are in both orientations */
  
    pfm_etm.emMasterHeight = YPT2DOT(pfm_h.dfPoints, 1);
    pfm_etm.emMinScale = YPT2DOT((dh->minimumPointSize+4)/8, 1);
    pfm_etm.emMaxScale = YPT2DOT((dh->maximumPointSize+4)/8, 1);
    pfm_etm.emMasterUnits = fm.emsize;
  
    pfm_etm.emCapHeight = ah->capHeight;
    pfm_etm.emXHeight = ah->xHeight;
  
    pfm_etm.emLowerCaseAscent = pfm_etm.emXHeight;  /* it's what TD uses ! */
    pfm_etm.emLowerCaseDescent = pfm_etm.emXHeight; /* it's what TD uses ! */
  
    if (dh->italicAngle <= 0)      /* Windows expects only slant to right */
        pfm_etm.emSlant = 0;
    else                          /* convert 1/100's to 1/10's with roundoff */
        pfm_etm.emSlant = (dh->italicAngle + 5) / 10;
  
    pfm_etm.emSuperScript = pfm_etm.emCapHeight - pfm_etm.emXHeight;
    pfm_etm.emSubScript = -pfm_etm.emLowerCaseDescent;
    pfm_etm.emSuperScriptSize = pfm_etm.emMasterUnits;
    pfm_etm.emSubScriptSize = pfm_etm.emMasterUnits;
    pfm_etm.emUnderlineOffset = ah->uscoreDepth;
    pfm_etm.emUnderlineWidth = ah->uscoreThickness;
    pfm_etm.emDoubleUpperUnderlineOffset =  pfm_etm.emUnderlineOffset;
    pfm_etm.emDoubleUpperUnderlineWidth = pfm_etm.emUnderlineWidth;
    pfm_etm.emDoubleLowerUnderlineWidth = pfm_etm.emUnderlineWidth;
    pfm_etm.emDoubleLowerUnderlineOffset =
    pfm_etm.emDoubleUpperUnderlineOffset
    + 2 * pfm_etm.emDoubleUpperUnderlineWidth;
  
    pfm_etm.emStrikeOutOffset = pfm_etm.emXHeight/2;
    pfm_etm.emStrikeOutWidth = pfm_etm.emXHeight/4;
    pfm_etm.emKernPairs = krn_prs.npairs;
    pfm_etm.emKernTracks = 0;
  
    /* Driver info section */
    pfm_di.epSize = sizeof (pfm_di);     /* size of this data structure */
    pfm_di.epVersion = DRIVERINFO_VERSION;  /* version of struct */
//    pfm_di.epSsid = fc.ssnum;            /* symbol set translation info */
//    pfm_di.epFontClass = 0;              /* other fonts types later ? */
    pfm_di.epMemUsage = 0L;
    pfm_di.epEscape = pfm_ext.dfDriverInfo + sizeof(DRIVERINFO);
    switch (ssnum)
    {
//  case ECMA94:
//      pfm_di.xtbl.symbolSet = epsymECMA94;
//      break;
        case MATH8:
            pfm_di.xtbl.symbolSet = epsymMath8;
            break;
//  case USASCII:
//      pfm_di.xtbl.symbolSet = epsymUSASCII;
//      break;
        case PSMATH:
            pfm_di.xtbl.symbolSet = epsymMathSymbols;
            break;
        case LEGAL:
            pfm_di.xtbl.symbolSet = epsymUSLegal;
            break;
        case PIFONT:
            pfm_di.xtbl.symbolSet = epsymGENERIC7;
            break;
        default:
            pfm_di.xtbl.symbolSet = epsymGENERIC8;
    }
    pfm_di.xtbl.offset = 0L;
    pfm_di.xtbl.len = 0;
    pfm_di.xtbl.firstchar = 0;
    pfm_di.xtbl.lastchar = 0;
  
//    /* note that the 'names' in the typeface header are NOT null-terminated */
//    lmemcpy((LPSTR)pfm_di.epFamilyName, th->familyName,
//            MIN(sizeof(pfm_di.epFamilyName), sizeof(th->familyName)));
//    if (pfm_di.epFamilyName[sizeof(pfm_di.epFamilyName)-1] != '\0')
//        pfm_di.epFamilyName[sizeof(pfm_di.epFamilyName)-1] = '\0';
  
    /***************** Fill out escapes for select string ****************/
  
    esc_str[0] = '\x1B';
    esc_str[1] = '(';
  
    /* Set WN symset */
    switch (ssnum)
    {
        case ZAPF100:
            lstrcat((LPSTR)esc_str, (LPSTR)"11L\x1B(s");
            break;
        case ZAPF200:
            lstrcat((LPSTR)esc_str, (LPSTR)"12L\x1B(s");
            break;
        case ZAPF300:
            lstrcat((LPSTR)esc_str, (LPSTR)"13L\x1B(s");
            break;
        case ZAPFPS:
            lstrcat((LPSTR)esc_str, (LPSTR)"10L\x1B(s");
            break;
        case ASCII:
            lstrcat((LPSTR)esc_str, (LPSTR)"0U\x1B(s");
            break;
        case PIFONT:
            lstrcat((LPSTR)esc_str, (LPSTR)"15U\x1B(s");
            break;
        case LEGAL:
            lstrcat((LPSTR)esc_str, (LPSTR)"1U\x1B(s");
            break;
        case PSMATH:
            lstrcat((LPSTR)esc_str, (LPSTR)"5M\x1B(s");
            break;
        case MSPUB:
            lstrcat((LPSTR)esc_str, (LPSTR)"6J\x1B(s");
            break;
        case PC8DN:
            lstrcat((LPSTR)esc_str, (LPSTR)"11U\x1B(s");
            break;
        case PC8:
            lstrcat((LPSTR)esc_str, (LPSTR)"10U\x1B(s");
            break;
        case PC850:
            lstrcat((LPSTR)esc_str, (LPSTR)"12U\x1B(s");
            break;
        case DESKTOP:
            lstrcat((LPSTR)esc_str, (LPSTR)"7J\x1B(s");
            break;
        case MATH8:
            lstrcat((LPSTR)esc_str, (LPSTR)"8M\x1B(s");
            break;
        case ROMAN8:
            lstrcat((LPSTR)esc_str, (LPSTR)"8U\x1B(s");
            break;
        case PSTEXT:
            lstrcat((LPSTR)esc_str, (LPSTR)"10J\x1B(s");
            break;
        case VENTMATH:
            lstrcat((LPSTR)esc_str, (LPSTR)"6M\x1B(s");
            break;
        case VENTUS:
            lstrcat((LPSTR)esc_str, (LPSTR)"14J\x1B(s");
            break;
        case ECMA94:
            lstrcat((LPSTR)esc_str, (LPSTR)"0N\x1B(s");
            break;
        case VENTINT:
            lstrcat((LPSTR)esc_str, (LPSTR)"13J\x1B(s");
            break;
        case WINSET:
        default:
            lstrcat((LPSTR)esc_str, (LPSTR)"9U\x1B(s");
            break;
    }
  
    /* Determine spacing (fixed/prop) */
    if (ah->isFixedPitch)
        lstrcat((LPSTR)esc_str, (LPSTR)"0p");
    else
        lstrcat((LPSTR)esc_str, (LPSTR)"1p");
  
    /* Determine style word */
    style = 10 * (fa_tbl[12] - '0') + (fa_tbl[13] - '0'); /* Add PCL Structure */
    style *= 32;
    style |= ((WORD)(fa_tbl[15] - '0') << 2);       /* Add PCL Width */
    style |= (fa_tbl[17] - '0');            /* Add PCL Posture */
    itoa((int)style, (LPSTR)&esc_str[lstrlen(esc_str)]);
  
    lstrcat((LPSTR)esc_str, (LPSTR)"s");
  
    /* Determine stroke weight */
    style = 10 * (fa_tbl[19] - '0') + (fa_tbl[20] - '0') - 8;
    itoa((int)style, (LPSTR)&esc_str[lstrlen(esc_str)]);
  
    lstrcat((LPSTR)esc_str, (LPSTR)"b");
    lmemcpy((LPSTR)&esc_str[lstrlen(esc_str)],
    (LPSTR)&fa_tbl[22], 5); /* Add PCL Typeface */
  
    /* Don't need to null terminate since we lmemset esc_str with zeros */
  
    /* Add pitch/height field */
    if (ah->isFixedPitch)
        lstrcat((LPSTR)esc_str, (LPSTR)"T\x1B(s#PITCHH");
    else
        lstrcat((LPSTR)esc_str, (LPSTR)"T\x1B(s#HEIGHTV");
  
    DBGbptm(("esc_str: %ls\n", (LPSTR)esc_str));
  
    return FALSE;       /* everything went OK */
}
  
  
/****************************************************************************\
  
FUNCTION: write_pfm()
  
PURPOSE:  Writes the necessary printer font metric info to disk.
  
MESSAGES:
  
COMMENTS:
  
\****************************************************************************/
static int write_pfm (fh, pfm_h, extent_tbl, pfm_ext, facename, devicename,
pfm_etm, krn_prs, pfm_di, escstr)
  
int fh;
LPPFMHEADER pfm_h;
WORD far *extent_tbl;
LPPFMEXTENSION pfm_ext;
BYTE far *facename;
BYTE far *devicename;
LPEXTTEXTMETRIC pfm_etm;
KERNPAIRS far *krn_prs;
LPDRIVERINFO pfm_di;
BYTE far *escstr;
  
{
    int i, j, len;
  
    if (pfm_h)
    {
        len = sizeof(*pfm_h)-sizeof(*pfm_h->dfCharOffset);
        if (_lwrite (fh, (LPSTR)pfm_h, len) != len)
            return (TRUE);        /* error during write */
    }
  
    if ((extent_tbl) && (!TSTFIXEDPITCH()))   /* Note !! this is not a width tbl (device units) */
        /* Driver finds this through the extension struct */
    {
        len = ((pfm_h->dfLastChar - pfm_h->dfFirstChar + 1) * 2);
        if (_lwrite (fh, (LPSTR)&extent_tbl[pfm_h->dfFirstChar], len) != len)
            return (TRUE);
        if (_lwrite (fh, (LPSTR)&zero_word, 2) != 2)
            return (TRUE);
    }
  
    if (pfm_ext)
    {
        len = sizeof(*pfm_ext);
        if (_lwrite (fh, (LPSTR)pfm_ext, len) != len)
            return (TRUE);        /* error during write */
    }
  
    if (facename)
    {
        len = FACESIZE;
        if (_lwrite (fh, (LPSTR)facename, len) != len)
            return (TRUE);        /* error during write */
    }
  
    if (devicename)
    {
        len = DEVSIZE;
        if (_lwrite (fh, (LPSTR)devicename, len) != len)
            return (TRUE);        /* error during write */
    }
  
    if (pfm_etm)
    {
        len = sizeof(*pfm_etm);
        if (_lwrite (fh, (LPSTR)pfm_etm, len) != len)
            return (TRUE);        /* error during write */
    }
  
    if (krn_prs && krn_prs->npairs)     /* only if at least one pair */
    {
        len = sizeof(*krn_prs->kpptr) * krn_prs->npairs;
        if (_lwrite (fh, (LPSTR)krn_prs->kpptr, len) != len)
            return (TRUE);        /* error during write */
    }
  
    if (pfm_di)
    {
        len = sizeof(*pfm_di);
        if (_lwrite (fh, (LPSTR)pfm_di, len) != len)
            return (TRUE);        /* error during write */
    }
  
    if (escstr)
    {
        len = ESCSIZE;
        if (_lwrite (fh, (LPSTR)escstr, len) != len)
            return (TRUE);        /* error during write */
    }
  
    return (FALSE);
}
  
  
// /****************************************************************************\
//
//     FUNCTION: get_master_pairs()
//
//     PURPOSE:  Reads in the master kerning pair file which defines all the
//               possible character combinations for which we might need to
//               generate kerning info for.  Since this list contains the
//               pair combinations for all CG character codes, it will be
//               used later to perform a reverse mapping into the desired
//               symbol set before we can calculate the kerning space.
//
//     MESSAGES:
//
//     COMMENTS:
//
// \****************************************************************************/
// static BOOL get_master_pairs (supp_dir)
// PSTR    supp_dir;
// {
//
//     FILE *  fh;
//     short   num_pairs = 0;  /* # of master pairs */
//     short   dummy;          /* dummy variable for holding char nums */
//     short   i;
//
//    lstrcpy(lbuf, supp_dir);     /* already has trailing '\' */
//    lstrcat(lbuf, PAIRS_FILE);   /* pathname of master pairs file */
//     if ((fh = fopen(lbuf, "r")) == NULL)
//         return FALSE;                   /* couldn't get file */
//
//     while ( fgets(lbuf, sizeof(lbuf), fh) != NULL)
//     {
//         if (parse_line(lbuf, &dummy, &dummy))
//             num_pairs++;
//     }
//     /* now allocate enough global memory for the master list */
//
//     if (((mp_h = GlobalAlloc(GMEM_MOVEABLE, (long)(num_pairs * sizeof(CGPAIR)))) == NULL) ||
//         ((mp = (LPCGPAIR) GlobalLock(mp_h)) == (LPSTR)NULL))
//     {
//         DBGbptm(("get_master_pairs:  Global Alloc or Lock failed,  - mp mem\n"));
//         if (mp_h)
//             GlobalFree(mp_h);
//         fclose(fh);
//         return FALSE;
//     }
//
//     rewind(fh);
//     i=0;
//     while ( (fgets(lbuf, sizeof(lbuf), fh) != NULL) &&
//             (i < num_pairs))
//     {
//         if (parse_line(lbuf, &mp[i].lnum, &mp[i].rnum))
//             i++;
//     }
//
//     mp_num = i;     /* # of CG master pairs */
//     fclose(fh);
//     GlobalUnlock(mp_h);
//     GlobalFree(mp_h);
//     return TRUE;
// }
//
//
// /****************************************************************************\
//
//     FUNCTION: parse_line()
//
//     PURPOSE:  Performs limited parsing on a line from the master kerning
//               pairs file.  If it finds a valid line, it fills in the two
//               CG char code variables and returns TRUE; otherwise it returns
//               FALSE.
//
//     MESSAGES:
//
//     COMMENTS:
//
// \****************************************************************************/
// static BOOL parse_line (line, lnum, rnum)
// char      * line;
// UWORD far * lnum;
// UWORD far * rnum;
// {
//     char *p;
//
//     /* get first token (ignore whitespace) */
//     if (!(p = strtok(line, " \t")) ||
//         !(*lnum = atoi(p)))             /* all chars are non-zero anyway */
//             return FALSE;
//     if (!(p = strtok(NULL, " \t")) ||   /* get next token */
//         !(*rnum = atoi(p)))
//             return FALSE;
//     return TRUE;
//
// }
//
//
/****************************************************************************\
  
FUNCTION: get_symbol_map()
  
PURPOSE:  Searches the CG symbol set file (if.ss) for the symbol set
defined by the current font.  It then reads it into a table
which will allow us to map CG char numbers into the char
codes defined by that symbol set.
  
MESSAGES:
  
COMMENTS: If the symbol set was found and we were able to load it into
a table,  TRUE is returned; otherwise we return FALSE.
  
\****************************************************************************/
static BOOL get_symbol_map (ssnum, supp_dir)
UWORD   ssnum;
PSTR    supp_dir;
{
    int  fh;
    UWORD   num_sets;
    SS_DIRECTORY ssdir;
    SS_ENTRY ssentry;
    short   i;
  
    lstrcpy(lbuf, supp_dir);     /* already has trailing '\' */
    lstrcat(lbuf, SYM_FILE);   /* pathname of symbol sets file */
    if ((fh = OpenFile ((LPSTR)lbuf, (LPOFSTRUCT) &of_buf, OF_READ)) == -1)
        return FALSE;           /* couldn't open it */
  
    if (_lread(fh, (LPSTR)&num_sets, sizeof(num_sets)) != sizeof(num_sets))
    {
        _lclose(fh);
        return FALSE;           /* bad read */
    }
  
    for (i=0; i < num_sets; i++)
    {
        if (_lread(fh, (LPSTR)&ssdir, sizeof(ssdir)) != sizeof(ssdir))
        {
            _lclose(fh);
            return FALSE;           /* bad read */
        }
        if (ssdir.ss_code == ssnum)
            break;                  /* found our symbol set ! */
    }
    if (ssdir.ss_code != ssnum)     /* did we find it ? */
        return FALSE;               /* undefined symbol set ! */
  
    mt_num = ssdir.num_codes;       /* number of char codes in our set */
  
    /* now allocate enough memory for the mapping table */
    if (((mt_h = GlobalAlloc(GMEM_MOVEABLE, (long)(mt_num * sizeof(MAPENTRY)))) == NULL) ||
        ((mt = (LPMAPENTRY) GlobalLock(mt_h)) == NULL))
    {
        DBGbptm(("get_symbol_map:  Global Alloc or Lock failed,  - mt mem\n"));
        if (mt_h)
            GlobalFree(mt_h);
        _lclose(fh);
        return FALSE;
    }
  
    if (_llseek(fh, (LONG)ssdir.offset, 0) == -1)
    {                           /* bad seek */
        _lclose(fh);
        GlobalUnlock(mt_h);
        GlobalFree(mt_h);
        return FALSE;
    }
  
    for (i=0; i < mt_num; i++)  /* fill in mapping table */
    {
        mt[i].ss_char = (BYTE) i + ssdir.first_code;
        if (_lread(fh, (LPSTR)&ssentry, sizeof(ssentry)) != sizeof(ssentry))
        {
            _lclose(fh);
            GlobalUnlock(mt_h);
            GlobalFree(mt_h);
            return FALSE;           /* bad read */
        }
        mt[i].cgnum = ssentry.cgnum;
    }
  
    _lclose(fh);
    GlobalUnlock(mt_h);
    GlobalFree(mt_h);
    return TRUE;
}
  
  
/****************************************************************************\
  
FUNCTION: generate_krn_pairs()
  
PURPOSE:  Performs a reverse mapping on the CG master kerning pairs
into the current symbol set.  When both CG char codes in
a pair are represented by chars in the current symbol set,
a kerning pair entry is created in the table which will
be written later to the PTM file.
  
MESSAGES:
  
COMMENTS: Returns TRUE if all went well; otherwise FALSE.
  
\****************************************************************************/
static BOOL generate_krn_pairs (hPcleo)
HANDLE         hPcleo;
{
  
    short   i, j;
    HANDLE  cgkrn_h;
    KERN_PAIR FAR * cgkrn_ptr;  /* CG's kerning pair struct is different */
    UWORD   status;
    FARPROC    lpFunc;
  
    /* allocate memory for the maximum # of pairs */
    /* (for both the Windows' kern tbl and the CG kern tbl) */
  
    if (((krn_prs.kph = GlobalAlloc(GMEM_MOVEABLE,
        (long)(mp_num * sizeof(KERNPAIR)))) == NULL) ||
        ((krn_prs.kpptr = (LPKERNPAIR) GlobalLock(krn_prs.kph))
        == (LPSTR)NULL) ||
        ((cgkrn_h =     GlobalAlloc(GMEM_MOVEABLE,
        (long)(mp_num * sizeof(KERN_PAIR)))) == NULL) ||
        ((cgkrn_ptr =     (KERN_PAIR FAR *) GlobalLock(cgkrn_h))
        == (LPSTR)NULL))
    {
        DBGbptm(("generate_krn_pairs:  Global Alloc or Lock failed,  - krn tbl mem\n"));
        if (krn_prs.kpptr)
        {
            GlobalUnlock(krn_prs.kph);
            krn_prs.kpptr = NULL;
        }
        if (krn_prs.kph)
            krn_prs.kph = GlobalFree(krn_prs.kph);
        if (cgkrn_ptr)
        {
            GlobalUnlock(cgkrn_h);
            cgkrn_ptr = NULL;
        }
        if (cgkrn_h)
            cgkrn_h = GlobalFree(cgkrn_h);
  
        return FALSE;
    }
  
    /* loop through the master pairs table now */
    for (i=0, j=0; i < mp_num; i++)
        if (map_CG_nums(&mp[i], &cgkrn_ptr[j]))
        {
            krn_prs.kpptr[j].kpPair.each[0] = (BYTE) cgkrn_ptr[j].chId0;
            krn_prs.kpptr[j].kpPair.each[1] = (BYTE) cgkrn_ptr[j].chId1;
            j++;
        }
    krn_prs.npairs = j;     /* set actual number */
  
    /* get the kerning space for all our pairs now */
  
  
    lpFunc = GetProcAddress(hPcleo, "CGIFkern");
    if (lpFunc != (FARPROC) NULL)
    {
        if(status = (*lpFunc)((UWORD)TEXT_KERN, (UWORD)krn_prs.npairs, cgkrn_ptr))
        {
            DBGbptm(("CGIFkern failed,  status = %d\n", status));
            return(status);
        }
    }
    else
        return FALSE;
  
    /* move the values over to our Windows kerning tbl */
    for (i=0; i < krn_prs.npairs; i++)
        krn_prs.kpptr[i].kpKernAmount = -(cgkrn_ptr[i].adj);
  
    /* don't need CG kern tbl any longer; save Windows tbl though */
    if(GlobalUnlock(cgkrn_h))
        DBGbptm(("generate_krn_pairs:  Global Unlock failed,  - krn tbl mem\n"));
  
    if(cgkrn_h = GlobalFree(cgkrn_h))
        DBGbptm(("generate_krn_pairs:  Global Free failed,  - krn tbl mem\n"));
  
    return (status == 0);   /* TRUE if everything was OK */
}
  
  
/****************************************************************************\
  
FUNCTION: map_CG_nums()
  
PURPOSE:  Checks to see if both the given CG char codes are in the
symbol set defined by 'mt'.  If they are, the corresponding
char codes in the symbol set are placed into the given
KERN_PAIR structure.
  
MESSAGES:
  
COMMENTS: Return TRUE if both CG char codes were in the symbol set;
otherwise FALSE.
  
\****************************************************************************/
static BOOL map_CG_nums (cg_pr, krn_pr)
LPCGPAIR        cg_pr;
KERN_PAIR FAR * krn_pr;     /* CG's kern struct (w/ sym set char codes) */
{
    BOOL    matched_left, matched_right;
    short   i;
  
    /* Search through the symbol set mapping table for a match.             */
    /* FIXME...this should be a search on sorted table for performance      */
    /* (However, since it only takes a couple of seconds to do it this      */
    /*  way and the rest of the typeface installation takes about a minute  */
    /*  per typeface, this is low priority.)                                */
    matched_left = FALSE;       /* init before search */
    matched_right = FALSE;
    for (i=0; i < mt_num; i++)  /* linear search for now...brute force */
    {
        if (cg_pr->lnum == mt[i].cgnum)
        {
            krn_pr->chId0 = mt[i].ss_char;
            matched_left = TRUE;
        }
        if (cg_pr->rnum == mt[i].cgnum)
        {
            krn_pr->chId1 = mt[i].ss_char;
            matched_right = TRUE;
        }
        if (matched_left && matched_right)  /* don't look any further */
            break;
    }
  
    return (matched_left && matched_right);
}
  
  
/****************************************************************************\
  
FUNCTION: release_krn_tbl()
  
PURPOSE:  Returns the kerning tbl resources associated with the current
font.
  
MESSAGES:
  
COMMENTS:
  
\****************************************************************************/
static void release_krn_tbl ()
{
  
    if(GlobalUnlock(mt_h))
        DBGbptm(("release_krn_tbl:  GlobalUnlock failed - mt memory\n"));
  
    if(mt_h = GlobalFree(mt_h))
        DBGbptm(("release_krn_tbl:  GlobalFree failed - mt memory\n"));
  
    if(GlobalUnlock(krn_prs.kph))
        DBGbptm(("release_krn_tbl:  GlobalUnlock failed - krn_prs memory\n"));
  
    if(krn_prs.kph = GlobalFree(krn_prs.kph))
        DBGbptm(("release_krn_tbl:  GlobalFree failed - krn_prs memory\n"));
  
}
/****************************************************************************
  
Routine Title: get_face
  
****************************************************************************/
  
BYTE get_face(int serifstyle, int fixpitch)
  
{
    /* This does not take in to account the FF_DECORATIVE case */
    /* because this info can not be found in a TFM file   */
  
    /* Set low bit to one for everything but MODERN */
  
    if (fixpitch)                                   /* Modern (fixed pitch) */
        return(FF_MODERN);
    else if ((serifstyle >= 0 && (serifstyle < 2)) ||
        (serifstyle == 8))                          /* San Serif */
        return((FF_SWISS)|1);
    else if (serifstyle < 8)                             /* Serif   */
        return((FF_ROMAN)|1);
    else if (serifstyle < 12)                           /*  Script */
        return((FF_SCRIPT)|1);
    else
        return((FF_DONTCARE)|1);                              /* Don't Care */
  
}
  
  
/******************************************************************
Function: get_weight
Modified: December 3, 1990 (DTK)
Summary: Get weight of font
Inputs: pclweight => fa.strokeweight - 8
Outputs: Weight
******************************************************************/
WORD get_weight (pclweight)
  
int pclweight;
{
    switch (pclweight)
    {
        case -7: return (FW_THIN-50);
        case -6: return (FW_THIN);
        case -5: return (FW_EXTRALIGHT-50);
        case -4: return (FW_EXTRALIGHT);
        case -3: return (FW_LIGHT-50);
        case -2: return (FW_LIGHT);
        case -1: return (FW_LIGHT+50);
        case  0: return (FW_NORMAL);
        case  1: return (FW_MEDIUM);
        case  2: return (FW_SEMIBOLD);
        case  3: return (FW_BOLD);
        case  4: return (FW_BOLD+50);
        case  5: return (FW_EXTRABOLD);
        case  6: return (FW_EXTRABOLD+50);
        case  7: return (FW_HEAVY);
        default: return (FW_DONTCARE);
    }
}
/**[r******************************************************************
*  MoveFamilyName
*
*  Taken from fontbld.c in the IFW code. (dtk)
*  Leveraged from Type Director, function build_family,
*  file fntmaker.c.  Modified for far pointers.
*
**[r******************************************************************/
  
BOOL FAR PASCAL MoveFamilyName(lpszFamilyName, lpSourceName, wLength)
LPSTR lpszFamilyName;
LPSTR lpSourceName;
WORD  wLength;
{
    BYTE temp[64];
    int n = 64;
    LPSTR lpTemp = (LPSTR)temp;
    LPSTR lpSrc = lpSourceName;
    static BYTE FAR *famspecs[] = FAMSPECS;
    static BYTE FAR *cfamspecs[] = CFAMSPECS;
    int i;
  
    --wLength;
  
    DBGmvfname(("MoveFamilyName: lpSourceName= %ls\n", lpSourceName));
  
    /*** Guarantee termination of source name at maximum length ***/
    lpSrc[FAMILYNAMLEN - 1] = NULL;
  
    /*** Zero init the temp name buffer ***/
    lmemset(lpTemp, 0, sizeof(temp));
  
    if (lstrncmp (lpSrc, (LPSTR)CGFOUND, sizeof(CGFOUND) - 2) == 0) /* includes space   */
    {
        lstrcpy (lpTemp, (LPSTR)CGFOUND);
        lpTemp += sizeof(CGFOUND) - 1;
        n -= sizeof(CGFOUND) - 1;
        lpSrc += sizeof(CGFOUND) - 2;
        DBGmvfname(("MoveFamilyName: CGFOUND\n"));
    }
    else if (lstrncmp (lpSrc, (LPSTR)VGCFOUND, sizeof(VGCFOUND) - 2) == 0) /* incl space   */
    {
        lstrcpy (lpTemp, (LPSTR)VGCFOUND);
        lpTemp += sizeof(VGCFOUND) - 1;
        n -= sizeof(VGCFOUND) - 1;
        lpSrc += sizeof(VGCFOUND) - 2;
        DBGmvfname(("MoveFamilyName: VGCFOUND\n"));
    }
    else if (lstrncmp (lpSrc, (LPSTR)ITCFOUND, sizeof(ITCFOUND) - 2) == 0)/* incl space    */
    {
        lstrcpy (lpTemp, (LPSTR)ITCFOUND);
        lpTemp += sizeof(ITCFOUND) - 1;
        n -= sizeof(ITCFOUND) - 1;
        lpSrc += sizeof(ITCFOUND) - 2;
        DBGmvfname(("MoveFamilyName: ITCFOUND\n"));
        if (lstrncmp (lpSrc, (LPSTR)ITCSUB1, sizeof(ITCSUB1) - 2) == 0)
        {
            lstrcpy (lpTemp, (LPSTR)ITCSUB1);
            lpTemp += sizeof(ITCSUB1) - 1;
            n -= sizeof(ITCSUB1) - 1;
            lpSrc += sizeof(ITCSUB1) - 2;
            DBGmvfname(("MoveFamilyName: ITCSUB1\n"));
            if (myislower (*lpSrc))
            {
                *lpTemp++ = 'C';
                --n;
            }
        }
        else if (lstrncmp (lpSrc, (LPSTR)ITCSUB2, sizeof(ITCSUB2) - 2) == 0)
        {
            lstrcpy (lpTemp, (LPSTR)ITCSUB2);
            lpTemp += sizeof(ITCSUB2) - 1;
            n -= sizeof(ITCSUB2) - 1;
            lpSrc += sizeof(ITCSUB2) - 2;
            DBGmvfname(("MoveFamilyName: ITCSUB2\n"));
            if (myislower (*lpSrc))
            {
                *lpTemp++ = 'C';
                --n;
            }
        }
    }
  
  
    for (i = 0; ((i < n) && (*lpSrc)); ++i)
    {
  
        DBGmvfname(("MoveFamilyName: i = %d, n = %d, s = %ls\n", i, n, lpSrc));
  
  
        if ((*lpSrc != ' ')  ||  (*(lpSrc + 1) != ' '))
        {
            if ((myisupper (*lpSrc)) && (myislower (*(lpTemp - 1))))    /* check char for uc    */
            {
                *lpTemp++ = ' ';            /* add a space          */
                ++i;
            }
            *lpTemp++ = *lpSrc++;           /* then copy over character */
        }
        else
            ++lpSrc;                /* eliminate extra blanks   */
    }
  
    *lpTemp = NULL;             /* guarantee an NULL        */
  
  
    DBGmvfname(("MoveFamilyName: temp = %ls\n", (LPSTR)temp));
  
    /* remove treatments from face name */
    for (i = 0; i < (sizeof(famspecs) / sizeof(famspecs[0])); ++i)
        remove_string ((LPSTR)temp, famspecs[i]);
  
    for (i = 0; i < (sizeof(cfamspecs) / sizeof(cfamspecs[0])); ++i)
        remove_string ((LPSTR)temp, cfamspecs[i]);
  
    lpSrc = (LPSTR)&temp[lstrlen(temp)-1];
  
    while ((*lpSrc == ' ')  &&  (lpSrc >= (LPSTR)temp))
        *lpSrc-- = NULL;    /* remove trailing blnks */
  
    lpSrc = (LPSTR)temp;
    lpTemp = lpszFamilyName;
  
    DBGmvfname(("MoveFamilyName: lpSrc = %ls\n", lpSrc));
  
    for (i = 0; i < wLength; ++i)
        if ((*lpSrc != ' ')  ||  (*(lpSrc+1) != ' '))
            *lpTemp++ = *lpSrc++;           /* then copy over character */
        else
        {
            ++lpSrc;                /* eliminate extra blanks   */
            --i;
        }
  
    *lpTemp = NULL;             /* guarantee an NULL        */
  
    DBGmvfname(("MoveFamilyName: lpszFamilyName = %ls\n", lpszFamilyName));
  
    return (TRUE);
}
  
/**[r******************************************************************
*  remove_string
*
* Taken from fontbld.c in the IFW code. (dtk)
*  Leveraged from Type Director, function remove_string,
*  file fntmaker.c.  Modified for far pointers.
*
**[r******************************************************************/
static void NEAR PASCAL remove_string (lpDest, lpSrc)
LPSTR lpDest;
LPSTR lpSrc;
{
    LPSTR lpString;
    int i;
  
    i = lstrlen(lpSrc);
  
    DBGmvfname(("remove_string: lpDest= %ls, lpSrc= %ls, i= %d\n", lpDest, lpSrc, i));
  
    if (((lpString = mystrstr(lpDest, lpSrc)) != NULL) && (!myislower(lpString[i]) || lpString[i] == NULL))
    {
        DBGmvfname(("remove_string: lpDest= %ls, lpSrc= %ls, lpString= %ls, i= %d\n",
        lpDest, lpSrc, lpString, i));
        lstrcpy (lpString, lpString+i);
    }
  
}
  
  
/**[r******************************************************************
*  lstrncmp
*
**[r******************************************************************/
int NEAR PASCAL lstrncmp(s, t, n)
LPSTR s;
LPSTR t;
WORD n;
{
    int diff;
  
    while ((n > 0) && (*s) && (*t) && ((diff = *s - *t) == 0))
    {
        s++;
        t++;
        n--;
    }
  
    return (diff);
}
  
/**[r******************************************************************
*  mystrstr
*
**[r******************************************************************/
LPSTR NEAR PASCAL mystrstr(lpMain, lpSearch)
LPSTR lpMain;
LPSTR lpSearch;
{
    int n;
    int i;
    WORD wSrchLen = lstrlen(lpSearch);
    BOOL found = FALSE;
    LPSTR lpStr = (LPSTR)NULL;
  
    DBGmvfname(("mystrstr: lpMain= %ls, lpSearch= %ls, wSrchLen= %d\n",
    lpMain, lpSearch, wSrchLen));
  
    /*** If both strings are non-empty, then set the maximum index for
    comparison, else return null ***/
    if ((*lpMain) && (*lpSearch))
        n = lstrlen(lpMain) - wSrchLen + 1;
    else
        return ((LPSTR)NULL);
  
    /*** Scan thru lpMain to find lpSearch ***/
    for (i = 0; !found && (i < n); i++)
    {
        DBGmvfname(("mystrstr: current lpMain position= %ls\n",
        (LPSTR)&lpMain[i]));
  
        if (lstrncmp((LPSTR)&lpMain[i], lpSearch, wSrchLen) == 0)
        {
            DBGmvfname(("mystrstr: Found lpSearch at i= %d\n", i));
  
            /*** We found it! ***/
            found = TRUE;
  
            /*** Save position where we found it ***/
            lpStr = (LPSTR)&lpMain[i];
        }
    }
  
    return (lpStr);
}
  
  
/**[r******************************************************************
*  myislower
*
**[r******************************************************************/
BOOL NEAR PASCAL myislower(ch)
BYTE ch;
{
  
    if ((ch > (BYTE)'a') && (ch < (BYTE)'z'))
        return (TRUE);
    else
        return (FALSE);
}
  
  
/**[r******************************************************************
*  myisupper
*
**[r******************************************************************/
BOOL NEAR PASCAL myisupper(ch)
BYTE ch;
{
  
    if ((ch > (BYTE)'A') && (ch < (BYTE)'Z'))
        return (TRUE);
    else
        return (FALSE);
}
  
