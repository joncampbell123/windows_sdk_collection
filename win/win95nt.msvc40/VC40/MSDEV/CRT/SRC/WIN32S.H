/***
*win32s.h -
*
*       Copyright (c) 1994-1995, Microsoft Corporation. All rights reserved.
*
*Purpose:
*
*       [Internal]
*
****/

#ifndef _INC_WIN32S
#define _INC_WIN32S

#ifndef _CRTBLD
/*
 * This is an internal C runtime header file. It is used when building
 * the C runtimes only. It is not to be used as a public header file.
 */
#error ERROR: Use of C runtime library internal header file.
#endif  /* _CRTBLD */

#if defined (DLL_FOR_WIN32S) && defined (CRTDLL)

/*
 * Header files containing structure, type and constant definitions
 * used in the struct definition below.
 */
#include <heap.h>
#include <internal.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <dbgint.h>
#include <sys\timeb.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/*
 * Other definitions and declarations NOT obtainable from the headers
 */

#define NUM_CHARS   257 /* from mbstring\mbctype.c */

#ifdef _WIN32

#define NUM_ULINFO 6 /* multibyte full-width-latin upper/lower info */

#else  /* _WIN32 */

#define NUM_ULINFO 12 /* multibyte full-width-latin upper/lower info */

#endif  /* _WIN32 */

#define INT_CHAR_CNT    4   /* from misc\inithelp.c */

#define MAX_PATH    260 /* from sdk\inc\windef.h */
typedef unsigned int    UINT;   /* from sdk\inc\windef.h */
typedef unsigned long   DWORD;  /* from sdk\inc\windef.h */
typedef unsigned short  WORD;   /* from sdk\inc\windef.h */

typedef DWORD       LCID;   /* from sdk\inc\winnt.h */
typedef void *      HANDLE; /* from sdk\inc\winnt.h */


#ifndef _TAGLC_ID_DEFINED
typedef struct tagLC_ID {   /* from h\setlocal.h */
   WORD wLanguage;
   WORD wCountry;
   WORD wCodePage;
} LC_ID, *LPLC_ID;
#define _TAGLC_ID_DEFINED
#endif  /* _TAGLC_ID_DEFINED */

#define _MAX_LC_LEN 131 /* must equal MAX_LC_LEN in h\setlocal.h */


struct argnode {        /* from startup\wild.c */
    _TSCHAR *argptr;
    struct argnode *nextnode;
};


/*
 * Structure containing all the per-process data for the C Runtime DLL
 * (MSVCRTx0.DLL).
 */
struct _CRTDLLPPD {

        /*
         * __lc_handle (global)
         *
         * references:
         *  misc\nlsdata2.c, aw_str.c, initctyp.c, initmon.c, initnum.c,
         *       inittime.c, aw_str.c
         *  convert\iswctype.c, mbstowcs.c, mbtowc.c, tolower.c,
         *      towlower.c, toupper.c, towupper.c, wcstombs.c,
         *      wctomb.c
         *  h\setlocal.h, win32s.h
         *  string\strcoll.c, stricoll.c, strlwr.c, strnicol.c, strupr.c,
         *         strxfrm.c, wcscoll.c, wcsicoll.c, wcslwr.c,
         *         wcsnicol.c, wcsupr.c, wcsxfrm.c
         */

        /*
         * IMPORTANT NOTE:
         *      stricmp.asm, strnicmp.asm, and memicmp.asm hard-code the offset
         *      of the _lc_handle[2] field within the _CRTDLLPPD structure.
         *
         *      This field MUST be first in the _CRTDLLPPD structure (win32s.h).
         *
         *      Otherwise a debug assertion at Win32s DLL startup will be triggered (crtlib.c).
         */

        LCID        _ppd___lc_handle[LC_MAX-LC_MIN+1];

        /*
         *
         *
         * references:
         *
         */

        /*
         * CONVERT
         */

        /*
         * HEAP
         */


#ifndef WINHEAP

        /*
         * _amblksiz (USER VISIBLE)
         *
         * references:
         *  heap\heapinit.c, heapdump.c, heapgrow.c, heapprm.c
         *  dllstuff\crtlib.c
         *  exec\spawnvpe.c
         *  h\heap.h, malloc.h, win32s.h
         */
        unsigned int    _ppd__amblksiz;

        /*
         * _heap_desc
         *
         * references:
         *  heap\heapinit.c, findaddr.c, free.c, heapadd.c, heapchk.c,
         *       heapdump.c, heapmin.c, heapsrch.c, heapused.c,
         *       heapwalk.c, malloc.c, realloc.c
         *  dllstuff\crtlib.c
         *  h\heap.h, win32s.h
         */
        struct _heap_desc_ _ppd__heap_desc;

        /*
         * _heap_descpages (common)
         *
         * references:
         *  heap\heapinit.c, heapused.c
         *  h\heap.h, win32s.h
         */
        void **     _ppd__heap_descpages;

        /*
         * _heap_maxregsize (global)
         *
         * references:
         *  heap\heapinit.c, heapgrow.c
         *  h\heap.h, win32s.h
         */
        unsigned int    _ppd__heap_maxregsize;

        /*
         * _heap_regions (common)
         *
         * references:
         *  heap\heapinit.c, heapdump.c, heapgrow.c, heapmin.c
         *       heapused.c, realloc.c
         *  h\heap.h, win32s.h
         */
        struct _heap_region_ _ppd__heap_regions[_HEAP_REGIONMAX];

        /*
         * _heap_regionsize (global)
         *
         * references:
         *  heap\heapinit.c, heapgrow.c
         *  h\heap.h, win32s.h
         */
        unsigned int    _ppd__heap_regionsize;

        /*
         * _heap_resetsize (global)
         *
         * references:
         *      heap\heapinit.c, free.c, heapadd.c, heapprm.c
         *      dllstuff\crtlib.c
         *      h\heap.h, win32s.h
         */
        unsigned int    _ppd__heap_resetsize;

        /*
         * _newmode (global, link-time option)
         *
         * references:
         *      heap\_newmode.c, new_mode.cxx, malloc.c
         *      dllstuff\crtexe.c, crtlib.c
         *      h\internal.h, win32s.h
         *      linkopts\newmode.c
         */
        int _ppd__newmode;

        /*
         * _pnhHeap (global)
         *
         * references:
         *      heap\malloc.c, handler.cxx
         *      h\win32s.h
         */
        _PNH            _ppd__pnhHeap;

#endif  /* WINHEAP */

        /*
         * LOWIO
         */

        /*
         *  _fmode (USER VISIBLE, also link-time option)
         *
         * references:
         *      lowio\txtmode.c, pipe.c, open.c
         *      dllstuff\crtexe.c, crtlib.c
         *      h\stdlib.h, win32s.h
         *      linkopts\binmode.c
         *
         */
        int             _ppd__fmode;

        /*
         * _nhandle (global)
         *
         * references:
         *      exec\dospawn.c,
         *      h\internal.h, win32s.h
         *      lowio\chsize.c, close.c, commit.c, dup.c, dup2.c, eof.c,
         *            fstat.c, flength.c, fleni64.c, ioinit.c, isatty.c,
         *            locking.c, lseek.c, lseeki64.c, osfinfo.c, read.c,
         *            setmode.c, write.c
         *      stdio\_freebuf.c, fdopen.c
         */
        int             _ppd__nhandle;

        /*
         * __pioinfo (global)
         *
         * references:
         *      exec\dospawn.c
         *      h\internal.h, win32s.h
         *      lowio\close.c, commit.c, dup.c, dup2.c, fstat.c, ioinit.c,
         *            isatty.c, lseek.c, lseeki64.c, open.c, osinfo.c,
         *            pipe.c, read.c, setmode.c, write.c
         *      misc\wrt2err.c
         *      startup\
         *      stdio\_filbuf.c, _file.c, _flsbuf.c, clearerr.c, fgetwc.c,
         *            fputwc.c, ftell.c, ftelli64.c, popen.c, rewind.c,
         *            ungetwc.c
         */
        ioinfo *        _ppd___pioinfo[IOINFO_ARRAYS];

        /*
         * MATH
         */

        /*
         * pusermatherr (static)
         *
         * references
         *  fpw32\tran\matherr.c
         *  crtw32\h\win32s.h
         */
        int (__cdecl * _ppd_pusermatherr)(struct _exception *);

        /*
         * MBSTRING
         */

        /*
         * __mbcodepage (global)
         *
         * references:
         *  mbstring\mbctype.c, ismbalnm.c, ismbbtype.c, ismbgrph.c,
         *       ismbknj.c, ismbpunc.c, tojisjms.c, tombbmbc.c,
         *       mbtohira.c, mbtokata.c, mbclevel.c
         *  h\mbctype.h, win32s.h
         */
        int     _ppd___mbcodepage;

        /*
         * _mbctype (USER VISIBLE)
         *
         * references:
         *  mbstring\mbctype.c, ismbbtype.c
         *  h\mbctype.h, win32s.h
         */
        unsigned char   _ppd__mbctype[NUM_CHARS];

        /*
         * __mblcid (global)
         *
         * references:
         *  mbstring\mbctype.c, ismbalnum.c, ismbalph.c, ismbdgt.c,
         *       ismbgrph.c, ismblwr.c, ismbprn.c, ismbpunc.c,
         *       ismbspc.c, ismbupr.c, mbscmp.c, mbsicmp.c, mbslwr.c,
         *       mbsnbcmp.c, mbsnbicm.c, mbsupr.c, mbtolwr.c,
         *       mbtoupr.c
         *  h\mbdata.h, win32s.h
         */
        unsigned int    _ppd___mblcid;

        /*
         * __mbulinfo (global)
         *
         * references:
         *  mbstring\mbctype.c, mbsicmp.c, mbsnicmp.c, mbsnbicm.c
         *  h\mbdata.h, win32s.h
         */
        unsigned short  _ppd___mbulinfo[NUM_ULINFO];

        /*
         * MISC
         */

        /*
         * cachid (static)
         *
         * references:
         *  misc\setlocal.c
         *  h\win32s.h
         */
        LC_ID       _ppd_cacheid;

        /*
         * cachecp (static)
         *
         * references:
         *  misc\setlocal.c
         *  h\win32s.h
         */
        UINT        _ppd_cachecp;

        /*
         * cachein (static)
         *
         * references:
         *  misc\setlocal.c
         *  h\win32s.h
         */
        char        _ppd_cachein[_MAX_LC_LEN];

        /*
         * cacheout (static)
         *
         * references:
         *  misc\setlocal.c
         *  h\win32s.h
         */
        char        _ppd_cacheout[_MAX_LC_LEN];

        /*
         * ctype1 (static)
         *
         * references:
         *  misc\initctyp.c
         *  h\win32s.h
         */
         unsigned short * _ppd_ctype1;

        /*
         * wctype1 (static)
         *
         * references:
         *  misc\initctyp.c
         *  h\win32s.h
         */
         unsigned short * _ppd_wctype1;

        /*
         * __decimal_point (global)
         *
         * references:
         *  misc\nlsdata1.c, initnum.c
         *  h\nlsint.h, win32s.h
         *  stdio\input.c
         */
        char *      _ppd___decimal_point;

        /*
         * __decimal_point_length (global)
         *
         * references:
         *  misc\nlsdata1.c, initnum.c
         *  h\nlsint.h, win32s.h
         */
        size_t      _ppd___decimal_point_length;

        /*
         * dec_pnt (static)
         *
         * references:
         *  misc\initnum.c
         *  h\win32s.h
         */
        char *      _ppd_dec_pnt;

        /*
         * grping (static)
         *
         * references:
         *  misc\initnum.c
         *  h\win32s.h
         */
        char *      _ppd_grping;

        /*
         * __lc_handle (global) is the first field in structure !!
         */


        /*
         * __lc_category (static)
         *
         * references:
         *  misc\setlocal.c
         *  h\win32s.h
         */
        struct {
            const char * catname;
            char * locale;
            int (* init)(void);
        } _ppd___lc_category[LC_MAX-LC_MIN+1];

        /*
         * __lc_codepage (global)
         *
         * references:
         *  misc\nlsdata2.c, aw_cmp.c, aw_loc.c, aw_map.c, aw_str.c,
         *       initctyp.c, setlocal.c
         *  convert\mbstowcs.c, mbtowc.c, _mbslen.c, mblen.c, wcstombs.c,
         *      wctomb.c
         *  h\setlocal.h, win32s.h
         *  stdio\fgetwc.c, fputwc.c, ungetwc.c
         *  string\strxfrm.c
         */
        UINT        _ppd___lc_codepage;

        /*
         * __lc_id (global)
         *
         * references:
         *  misc\nlsdata3.c, initctyp.c, inithelp.c, initmon.c,
         *       initnum.c, inittime.c, setlocal.c
         *  h\setlocal.h, win32s.h
         */
        LC_ID           _ppd___lc_id[LC_MAX-LC_MIN+1];

        /*
         * __lc_time_intl (global, though it shouldn't be)
         *
         * references:
         *  misc\inittime.c
         *  h\win32s.h
         */
        struct __lc_time_data * _ppd___lc_time_intl;

        /*
         * __lconv (global)
         *
         * references:
         *  misc\lconv.c, initmon.c, initnum.c, lcnvinit.c
         *  h\setlocal.h, win32s.h
         */
        struct lconv *  _ppd___lconv;

        /*
         * __lconv_c (global)
         *
         * references:
         *  misc\lconv.c, initmon.c, lcnvinit.c
         *  h\setlocal.h, win32s.h
         */
        struct lconv    _ppd___lconv_c;

        /*
         * __lconv_intl (static)
         *
         * references:
         *  misc\initmon.c
         *  h\win32s.h
         */
        struct lconv *  _ppd___lconv_intl;

        /*
         * __mb_cur_max (USER VISIBLE, aka MB_CUR_MAX)
         *
         * references:
         *  misc\nlsdata1.c, initctyp.c
         *  convert\mblen.c, _mbslen.c, mbstowcs.c, mbtowc.c, wcstombs.c,
         *      wctomb.c
         *  dllstuff\crtlib.c
         *  h\ctype.h, stdlib.h, win32s.h
         *  stdio\fputwc.c, input.c, output.c, ungetwc.c
         */
        int     _ppd___mb_cur_max;


        /*
         * outwlocale (static)
         *
         * references:
         *  misc\wsetloca.c
         *  h\win32s.h
         */
        wchar_t *   _ppd_outwlocale;

        /*
         * _pctype (USER VISIBLE)
         *
         * references:
         *  misc\ctype.c
         *  h\ctype.h, wchar.h, win32s.h
         */
        unsigned short *_ppd__pctype;

        /*
         * _pwctype (USER VISIBLE)
         *
         * references:
         *  misc\ctype.c
         *  h\ctype.h, wchar.h, win32s.h
         */
        unsigned short *_ppd__pwctype;

        /*
         * thous_sep (static)
         *
         * references:
         *  misc\initnum.c
         *  h\win32s.h
         */
        char *      _ppd_thous_sep;

        /*
         * wcbuffer (static)
         *
         * references:
         *  misc\inithelp.c
         *  h\win32s.h
         *
         */
        wchar_t     _ppd_wcbuffer[INT_CHAR_CNT];


#ifdef _DEBUG

        /*
         * _crtDbgFlag (USER VISIBLE)
         */
        int _ppd__crtDbgFlag;

        /*
         * _lRequestCurr (static)
         */
        long _ppd__lRequestCurr;

        /*
         * _crtBreakAlloc (USER VISIBLE)
         */
        long _ppd__crtBreakAlloc;

        /*
         * _lTotalAlloc (static)
         */
        unsigned long _ppd__lTotalAlloc;

        /*
         * _lCurAlloc (static)
         */
        unsigned long _ppd__lCurAlloc;

        /*
         * _lMaxAlloc (static)
         */
        unsigned long _ppd__lMaxAlloc;

        /*
         * bNoMansLandFill (static)
         */
        unsigned char _ppd__bNoMansLandFill;

        /*
         * bDeadLandFill (static)
         */
        unsigned char _ppd__bDeadLandFill;

        /*
         * bCleanLandFill (static)
         */
        unsigned char _ppd__bCleanLandFill;

        /*
         * pFirstBlock (static)
         */
        _CrtMemBlockHeader * _ppd__pFirstBlock;

        /*
         * pLastBlock (static)
         */
        _CrtMemBlockHeader * _ppd__pLastBlock;

        /*
         * _pfnDumpClient (global)
         */
        _CRT_DUMP_CLIENT _ppd__pfnDumpClient;

        /*
         * _pfnAllocHook (global)
         */
        _CRT_ALLOC_HOOK _ppd__pfnAllocHook;

        /*
         * _pfnReportHook (global)
         */
        _CRT_REPORT_HOOK _ppd__pfnReportHook;

        /*
         * _crtAssertBusy (USER VISIBLE)
         */
        long _ppd__crtAssertBusy;

        /*
         * _CrtDbgMode (global)
         */
        int _ppd__CrtDbgMode[_CRT_ERRCNT];

        /*
         * _CrtDbgFile (global)
         */
        _HFILE _ppd__CrtDbgFile[_CRT_ERRCNT];

#endif  /* _DEBUG */

        /*
         * STARTUP
         */

        /*
         * _acmdln (USED IN CLIENT STARTUP)
         *
         * references:
         *  dllstuff\crtlib.c, crtexe.c
         *  h\internal.h, win32s.h
         *  startup\crt0.c, dllcrt0.c, stdargv.c
         */
        char *      _ppd__acmdln;

        /*
         * _wcmdln (USED IN CLIENT STARTUP)
         *
         * references:
         *  dllstuff\crtlib.c, crtexe.c
         *  h\internal.h, win32s.h
         *  startup\crt0.c, dllcrt0.c, stdargv.c
         */
        wchar_t *   _ppd__wcmdln;

        /*
         * _aenvptr (global)
         *
         * references:
         *  dllstuff\crtlib.c
         *  exec\cenvarg.c
         *  h\internal.h, win32s.h
         *  startup\crt0.c, dllcrt0.c, stdenvp.c
         */
        char *      _ppd__aenvptr;

        /*
         * _wenvptr (global)
         *
         * references:
         *  dllstuff\crtlib.c
         *  exec\cenvarg.c
         *  h\internal.h, win32s.h
         *  startup\crt0.c, dllcrt0.c, stdenvp.c
         */
        wchar_t *   _ppd__wenvptr;

        /*
         * _aexit_rtn (global)
         *
         * references:
         *  dllstuff\crtlib.c
         *  h\internal.h, win32s.h
         *  startup\crt0.c, dllcrt0.c
         */
        void (__cdecl * _ppd__aexit_rtn)(int);

        /*
         * __argc (USER VISIBLE)
         *
         * references:
         *  startup\crt0dat.c, crt0.c, stdargv.c, wild.c
         *  dllstuff\crtlib.c
         *  h\stdlib.h, win32s.h
         */
        int     _ppd___argc;

        /*
         * arghead (static)
         *
         * references:
         *  startup\wild.c
         *  h\win32s.h
         */
        struct argnode *    _ppd_arghead;

        /*
         * argend (static)
         *
         * references:
         *  startup\wild.c
         *  h\win32s.h
         */
        struct argnode *    _ppd_argend;


        /*
         * __argv (USER VISIBLE)
         *
         * references:
         *  startup\crt0dat.c, crt0.c, stdargv.c, wild.c
         *  dllstuff\crtlib.c
         *  h\stdlib.h, win32s.h
         */
        char **     _ppd___argv;

        /*
         * __wargv (USER VISIBLE)
         *
         * references:
         *  startup\crt0dat.c, crt0.c, stdargv.c, wild.c
         *  dllstuff\crtlib.c
         *  h\stdlib.h, win32s.h
         */
        wchar_t **  _ppd___wargv;

        /*
         * _C_Termination_Done (global)
         *
         * references:
         *  startup\crt0dat.c, dllcrt0.c
         *  dllstuff\crtlib.c
         *  h\internal.h, win32s.h
         */
        int     _ppd__C_Termination_Done;

        /*
         * _environ (USER VISIBLE)
         *
         * references:
         *  startup\crt0dat.c, crt0.c, stdenvp.c
         *  dllstuff\crtlib.c
         *  h\stdlib.h, tchar.h, win32s.h
         *  misc\getenv.c, mbtowenv.c, putenv.c, setenv.c
         */
        char **     _ppd__environ;

        /*
         * _wenviron (USER VISIBLE)
         *
         * references:
         *  startup\crt0dat.c, crt0.c, stdenvp.c
         *  dllstuff\crtlib.c
         *  h\stdlib.h, tchar.h, win32s.h
         *  misc\getenv.c, putenv.c, setenv.c, mbtowenv.c
         *
         */
        wchar_t **  _ppd__wenviron;

        /*
         * __error_mode (global)
         *
         * references:
         *  startup\crt0.c, crt0msg.c
         *  dllstuff\crtexe.c, crtlib.c
         *  h\internal.h, stdlib.h, win32s.h
         *  misc\assert.c
         */
        int     _ppd___error_mode;

        /*
         * _exitflag (global)
         *
         * references:
         *  startup\crt0dat.c
         *  stdio\fflush.c
         *  h\internal.h, win32s.h
         */
        char        _ppd__exitflag;

        /*
         * findbuf (static)
         *
         * references:
         *  startup\wild.c
         *  h\win32s.h
         */
        void *      _ppd_findbuf;

        /*
         * __initenv (USED IN CLIENT STARTUP)
         *
         * references:
         *  startup\crt0dat.c, crt0.c
         *  dllstuff\crtexe.c
         *  h\internal.h, win32s.h
         *  misc\setenv.c
         */
        char **     _ppd___initenv;

        /*
         * __winitenv (USED IN CLIENT STARTUP)
         *
         * references:
         *  startup\crt0dat.c, crt0.c
         *  dllstuff\crtexe.c
         *  h\internal.h, win32s.h
         *  misc\setenv.c
         */
        wchar_t **  _ppd___winitenv;

        /*
         * __onexitbegin (global, note msvcrt*.dll and client both have their
         * instance of this global)
         *
         *  reference:
         *  startup\crt0dat.c,
         *  dllstuff\atonexit.c, crtdll.c, crtexe.c
         *  h\win32s.h
         *  misc\onexit.c
         */
        _PVFV *     _ppd___onexitbegin;

        /*
         * __onexitend (global, note msvcrt*.dll and client both have their
         * instance of this global)
         *
         *  reference:
         *  startup\crt0dat.c,
         *  dllstuff\atonexit.c, crtdll.c, crtexe.c
         *  h\win32s.h
         *  misc\onexit.c
         */
        _PVFV *     _ppd___onexitend;

        /*
         * _osver (USER VISIBLE)
         *
         * references:
         *  startup\crt0dat.c, crt0.c, dllcrt0.c
         *  dllstuff\crtlib.c
         *  heap\heapinit.c
         *  h\stdlib.h, win32s.h
         *
         */
        unsigned int    _ppd__osver;

        /*
         * _pgmname (static)
         *
         *  reference:
         *  startup\stdargv.c
         *  h\win32s.h
         */
        _TSCHAR     _ppd__pgmname[ MAX_PATH ];

        /*
         * __proc_attached (static)
         *
         *  reference:
         *  dllstuff\crtlib.c
         */
        int     __proc_attached;

        /*
         * _pgmptr (USER VISIBLE)
         *
         * references:
         *  startup\crt0dat.c, stdargv.c
         *  h\dos.h, stdlib.h, win32s.h
         */
        char *      _ppd__pgmptr;

        /*
         * _wpgmptr (USER VISIBLE)
         *
         * references:
         *  startup\crt0dat.c, stdargv.c
         *  h\dos.h, stdlib.h, win32s.h
         */
        wchar_t *   _ppd__wpgmptr;

        /*
         * _tlsindex (global)
         *
         * references:
         *  startup\tidtable.c, thread.c, threadex.c
         *  h\mtdll.h, win32s.h
         */
        unsigned long   _ppd___tlsindex;

        /*
         * _umaskval (global)
         *
         * references:
         *  startup\crt0dat.c
         *  lowio\open.c
         *  misc\umask.c
         */
        int     _ppd__umaskval;

        /*
         * _WildFindHandle (static)
         *
         * references:
         *  startup\wild.c
         *  h\win32s.h
         *
         */
        HANDLE      _ppd__WildFindHandle;

        /*
         * _winmajor (USER VISIBLE)
         *
         * references:
         *  startup\crt0dat.c, crt0.c, dllcrt0.c
         *  dllstuff\crtlib.c
         *  heap\heapinit.c
         *  h\stdlib.h, win32s.h
         *
         */
         unsigned int   _ppd__winmajor;

        /*
         * _winminor (USER VISIBLE)
         *
         * references:
         *  startup\crt0dat.c, crt0.c, dllcrt0.c
         *  dllstuff\crtlib.c
         *  h\stdlib.h, win32s.h
         *
         */
         unsigned int   _ppd__winminor;

        /*
         * _winver (USER VISIBLE)
         *
         * references:
         *  startup\crt0dat.c, crt0.c, dllcrt0.c
         *  dllstuff\crtlib.c
         *  h\stdlib.h, win32s.h
         *
         */
         unsigned int   _ppd__winver;

        /*
         * STDIO
         */

        /*
         * _commode (global, also a link-time option)
         *
         * references:
         *  stdio\ncommode.c, fdopen.c, _open.c, tmpfile.c
         *  dllstuff\crtexe.c, crtlib.c
         *  h\internal.h, win32s.h
         *  linkopts\commode.c
         *
         */
        int     _ppd__commode;

        /*
         * _iob (USER VISIBLE)
         *
         * references:
         *  stdio\_file.c, closeall.c
         *  dllstuff\crtlib.c
         *  h\stdio.h
         *
         */
        FILE        _ppd__iob[_NSTREAM_];

        /*
         * namebuf0 (static)
         *
         *  reference:
         *  stdio\tmpfile.c
         *  h\win32s.h
         */
        _TSCHAR     _ppd_namebuf0[L_tmpnam];

        /*
         * namebuf0 (static)
         *
         *  reference:
         *  stdio\tmpfile.c
         *  h\win32s.h
         */
        _TSCHAR     _ppd_namebuf1[L_tmpnam];

        /*
         * _nstream (global)
         *
         * references:
         *  h\internal.h
         *  stdio\closeall.c, _file.c, rmtmp.c, setmax.c, stream.c
         */
        int     _ppd__nstream;

        /*
         * _old_pfxlen (global)
         *
         * references:
         *  stdio\rmtmp.c, tempnam.c
         *  h\internal.h, win32s.h
         */
        unsigned    _ppd__old_pfxlen;

        /*
         * __piob (global)
         *
         * references:
         *  h\internal.h
         *  startup\mlock.c
         *  stdio\closeall.c, _file.c, rmtmp.c, setmax.c, stream.c
         */
        void **     _ppd___piob;

        /*
         * _tempoff  (global)
         *
         * references:
         *  stdio\rmtmp.c, tempnam.c
         *  h\internal.h, win32s.h
         */
        unsigned    _ppd__tempoff;

        /*
         * TIME
         */

        /*
         * _alternate_form (global, though it shouldn't be)
         *
         * references:
         *  time\strftime.c
         *  h\win32s.h
         */
        unsigned    _ppd__alternate_form;

        /*
         * _daylight (USER VISIBLE)
         *
         * references:
         *  time\timeset.c, dtoxtime.c, localtim.c, tzset.c
         *  h\time.h, win32s.h
         */
        int     _ppd__daylight;

        /*
         * first_time (static)
         *
         * references:
         *  time\tzset.c
         *  h\win32s.h
         */
        int     _ppd_tzset_first_time;

        /*
         * __itimeb (global, though it shouldn't be)
         *
         * references:
         *  time\clock.c
         *  h\win32s.h
         */
        struct _timeb   _ppd___itimeb;

        /*
         * lastTZ (static)
         *
         * references:
         *  time\tzset.c
         *  h\win32s.h
         */
        char *      _ppd_lastTZ;

        /*
         *_lc_time_curr (global)
         *
         * references:
         *  time\strftime.c
         *  dllstuff\crtlib.c
         *  misc\inittime.c
         *  h\win32s.h
         */
        struct __lc_time_data * _ppd___lc_time_curr;

        /*
         * _no_lead_zeros (global, though it shouldn't be)
         *
         * references:
         *  time\strftime.c
         *  h\win32s.h
         */
        unsigned    _ppd__no_lead_zeros;

        /*
         * _timezone (USER VISIBLE)
         *
         * references:
         *  time\timeset.c, dtoxtime.c, ftime.c, localtim.c, mktime.c,
         *       tzset.c
         *  h\time.h, win32s.h
         */
        long        _ppd__timezone;

        /*
         * _tzname (USER VISIBLE)
         *
         * references:
         *  time\timeset.c, strftime.c, tzset.c
         *  h\time.h, win32s.h
         */
        char *      _ppd__tzname[2];

        /*
         * tzstd (static)
         *
         * references:
         *  time\timeset.c
         *  h\win32s.h
         */
        char        _ppd_tzstd[11];

        /*
         * tzdst (static)
         *
         * references:
         *  time\timeset.c
         *  h\win32s.h
         */
        char        _ppd_tzdst[11];

#ifdef WINHEAP
        /*
         * WINHEAP
         */

        /*
         * _amblksiz (USER VISIBLE)
         *
         * references:
         *  winheap\heapinit.c
         *  dllstuff\crtlib.c
         *  exec\spawnvpe.c
         *  h\heap.h, malloc.h, win32s.h
         */
        unsigned int    _ppd__amblksiz;

        /*
         * _crtheap
         *
         * references:
         *  winheap\calloc.c, expand.c, heapchk.c, heapinit.c, heapmin.c,
         *      heapwalk.c, malloc.c, msize.c, realloc.c
         *  h\winheap.h, win32s.h
         */
        HANDLE      _ppd__crtheap;

        /*
         * _newmode (global, link-time option)
         *
         * references:
         *  winheap\calloc.c, _newmode.c, new_mode.cxx, malloc.c,
         *      realloc.c
         *  dllstuff\crtexe.c, crtlib.c
         *  h\internal.h, win32s.h
         *  linkopts\newmode.c
         */
        int _ppd__newmode;

        /*
         * _pnhHeap (global)
         *
         * references:
         *  winheap\calloc.c, malloc.c, handler.cxx, new.cxx, realloc.c
         *  h\win32s.h
         */
        _PNH        _ppd__pnhHeap;

#endif  /* WINHEAP */
};


struct _CRTDLLPPD * __cdecl _GetPPD(void);



/* HEAP */


#ifndef WINHEAP

#define _amblksiz           (_GetPPD()->_ppd__amblksiz)
#define _heap_desc          (_GetPPD()->_ppd__heap_desc)
#define _heap_descpages     (_GetPPD()->_ppd__heap_descpages)
#define _heap_growsize      (_GetPPD()->_ppd__amblksiz)
#define _heap_maxregsize    (_GetPPD()->_ppd__heap_maxregsize)
#define _heap_regions       (_GetPPD()->_ppd__heap_regions)
#define _heap_regionsize    (_GetPPD()->_ppd__heap_regionsize)
#define _heap_resetsize     (_GetPPD()->_ppd__heap_resetsize)
#define _newmode            (_GetPPD()->_ppd__newmode)
#define _pnhHeap            (_GetPPD()->_ppd__pnhHeap)

#endif  /* WINHEAP */

/* LOWIO */

#define _fmode              (_GetPPD()->_ppd__fmode)
#define _nhandle            (_GetPPD()->_ppd__nhandle)
#define __pioinfo           (_GetPPD()->_ppd___pioinfo)

/* MATH */

#define pusermatherr        (_GetPPD()->_ppd_pusermatherr)

/* MBSTRING */

#define __mbcodepage        (_GetPPD()->_ppd___mbcodepage)
#define _mbctype            (_GetPPD()->_ppd__mbctype)
#define __mblcid            (_GetPPD()->_ppd___mblcid)
#define __mbulinfo          (_GetPPD()->_ppd___mbulinfo)

/* MISC */

#define __decimal_point     (_GetPPD()->_ppd___decimal_point)
#define __decimal_point_length  (_GetPPD()->_ppd___decimal_point_length)
#define __lc_codepage       (_GetPPD()->_ppd___lc_codepage)
#define __lc_handle         (_GetPPD()->_ppd___lc_handle)
#define __lc_id             (_GetPPD()->_ppd___lc_id)
#define __lconv             (_GetPPD()->_ppd___lconv)
#define __lconv_c           (_GetPPD()->_ppd___lconv_c)
#define __lc_time_intl      (_GetPPD()->_ppd___lc_time_intl)
#define __mb_cur_max        (_GetPPD()->_ppd___mb_cur_max)
#define MB_CUR_MAX          (_GetPPD()->_ppd___mb_cur_max)
#define _pctype             (_GetPPD()->_ppd__pctype)
#define _pwctype            (_GetPPD()->_ppd__pwctype)

#ifdef _DEBUG

#define _crtDbgFlag         (_GetPPD()->_ppd__crtDbgFlag)
#define _lRequestCurr       (_GetPPD()->_ppd__lRequestCurr)
#define _crtBreakAlloc      (_GetPPD()->_ppd__crtBreakAlloc)
#define _lRequestCurr       (_GetPPD()->_ppd__lRequestCurr)
#define _lTotalAlloc        (_GetPPD()->_ppd__lTotalAlloc)
#define _lCurAlloc          (_GetPPD()->_ppd__lCurAlloc)
#define _lMaxAlloc          (_GetPPD()->_ppd__lMaxAlloc)
#define _bNoMansLandFill    (_GetPPD()->_ppd__bNoMansLandFill)
#define _bDeadLandFill      (_GetPPD()->_ppd__bDeadLandFill)
#define _bCleanLandFill     (_GetPPD()->_ppd__bCleanLandFill)
#define _pFirstBlock        (_GetPPD()->_ppd__pFirstBlock)
#define _pLastBlock         (_GetPPD()->_ppd__pLastBlock)
#define _pfnDumpClient      (_GetPPD()->_ppd__pfnDumpClient)
#define _pfnAllocHook       (_GetPPD()->_ppd__pfnAllocHook)
#define _pfnReportHook      (_GetPPD()->_ppd__pfnReportHook)
#define _crtAssertBusy      (_GetPPD()->_ppd__crtAssertBusy)
#define _CrtDbgMode         (_GetPPD()->_ppd__CrtDbgMode)
#define _CrtDbgFile         (_GetPPD()->_ppd__CrtDbgFile)

#endif  /* _DEBUG */

/* STARTUP */

#define _acmdln             (_GetPPD()->_ppd__acmdln)
#define _wcmdln             (_GetPPD()->_ppd__wcmdln)
#define _aenvptr            (_GetPPD()->_ppd__aenvptr)
#define _wenvptr            (_GetPPD()->_ppd__wenvptr)
#define _aexit_rtn          (_GetPPD()->_ppd__aexit_rtn)
#define __argc              (_GetPPD()->_ppd___argc)
#define __argv              (_GetPPD()->_ppd___argv)
#define __wargv             (_GetPPD()->_ppd___wargv)
#define _C_Termination_Done (_GetPPD()->_ppd__C_Termination_Done)
#define _environ            (_GetPPD()->_ppd__environ)
#define _wenviron           (_GetPPD()->_ppd__wenviron)
#define __error_mode        (_GetPPD()->_ppd___error_mode)
#define _exitflag           (_GetPPD()->_ppd__exitflag)
#define __initenv           (_GetPPD()->_ppd___initenv)
#define __winitenv          (_GetPPD()->_ppd___winitenv)
#define _osver              (_GetPPD()->_ppd__osver)
#define __onexitbegin       (_GetPPD()->_ppd___onexitbegin)
#define __onexitend         (_GetPPD()->_ppd___onexitend)
#define _pgmptr             (_GetPPD()->_ppd__pgmptr)
#define _wpgmptr            (_GetPPD()->_ppd__wpgmptr)
#define __tlsindex          (_GetPPD()->_ppd___tlsindex)
#define _umaskval           (_GetPPD()->_ppd__umaskval)
#define _winmajor           (_GetPPD()->_ppd__winmajor)
#define _winminor           (_GetPPD()->_ppd__winminor)
#define _winver             (_GetPPD()->_ppd__winver)

/* STDIO */

#define _commode            (_GetPPD()->_ppd__commode)
#define _iob                (_GetPPD()->_ppd__iob)
#define _nstream            (_GetPPD()->_ppd__nstream)
#define _old_pfxlen         (_GetPPD()->_ppd__old_pfxlen)
#define __piob              (_GetPPD()->_ppd___piob)
#define _tempoff            (_GetPPD()->_ppd__tempoff)

/* TIME */

#define _alternate_form     (_GetPPD()->_ppd__alternate_form)
#define _daylight           (_GetPPD()->_ppd__daylight)
#define __itimeb            (_GetPPD()->_ppd___itimeb)
#define __lc_time_curr      (_GetPPD()->_ppd___lc_time_curr)
#define _no_lead_zeros      (_GetPPD()->_ppd__no_lead_zeros)
#define _timezone           (_GetPPD()->_ppd__timezone)
#define _tzname             (_GetPPD()->_ppd__tzname)

/* WINHEAP */

#ifdef WINHEAP

#define _amblksiz           (_GetPPD()->_ppd__amblksiz)
#define _crtheap            (_GetPPD()->_ppd__crtheap)
#define _newmode            (_GetPPD()->_ppd__newmode)
#define _pnhHeap            (_GetPPD()->_ppd__pnhHeap)

#endif  /* WINHEAP */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* defined (DLL_FOR_WIN32S) && defined (CRTDLL) */

#endif  /* _INC_WIN32S */
