/***
*crtlib.c - CRT DLL initialization and termination routine (Win32, Dosx32)
*
*       Copyright (c) 1991-1995, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This module contains initialization entry point for the CRT DLL
*       in the Win32 environment. It also contains some of the supporting
*       initialization and termination code.
*
*******************************************************************************/

#ifdef CRTDLL

#include <cruntime.h>
#include <oscalls.h>
#include <dos.h>
#include <internal.h>
#include <malloc.h>
#include <mbctype.h>
#include <mtdll.h>
#include <process.h>
#include <rterr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <awint.h>
#include <tchar.h>
#include <time.h>
#include <dbgint.h>

#ifdef DLL_FOR_WIN32S

#include <setlocal.h>
#include <dbgint.h>

/*
 * Initial value for _ppd___lc_time_curr. Defined in time\strftime.c
 */
extern struct __lc_time_data __lc_time_c;

#define proc_attached   _GetPPD()->__proc_attached

static unsigned long ppdindex = 0xffffffff;

/*
 * Procedures to create and release the per-process data structure in
 * Win32S.
 */
static int __cdecl AllocPerProcessDataStruct(void);
static void __cdecl FreePerProcessDataStruct(void);

#else  /* DLL_FOR_WIN32S */

/*
 * flag set iff _CRTDLL_INIT was called with DLL_PROCESS_ATTACH
 */
static int proc_attached = 0;

/*
 * command line, environment, and a few other globals
 */
wchar_t *_wcmdln = NULL;           /* points to wide command line */
char *_acmdln = NULL;              /* points to command line */

char *_aenvptr = NULL;      /* points to environment block */
wchar_t *_wenvptr = NULL;   /* points to wide environment block */

void (__cdecl * _aexit_rtn)(int) = _exit;   /* RT message return procedure */

extern int _newmode;    /* declared in <internal.h> */

int __error_mode = _OUT_TO_DEFAULT;

#endif  /* DLL_FOR_WIN32S */

int __app_type = _UNKNOWN_APP;

static void __cdecl inherit(void);  /* local function */

/***
*void __[w]getmainargs - get values for args to main()
*
*Purpose:
*       This function invokes the command line parsing and copies the args
*       to main back through the passsed pointers. The reason for doing
*       this here, rather than having _CRTDLL_INIT do the work and exporting
*       the __argc and __argv, is to support the linked-in option to have
*       wildcard characters in filename arguments expanded.
*
*Entry:
*       int *pargc              - pointer to argc
*       _TCHAR ***pargv         - pointer to argv
*       _TCHAR ***penvp         - pointer to envp
*       int dowildcard          - flag (true means expand wildcards in cmd line)
*       _startupinfo * startinfo- other info to be passed to CRT DLL
*
*Exit:
*       No return value. Values for the arguments to main() are copied through
*       the passed pointers.
*
*******************************************************************************/


_CRTIMP void __cdecl __wgetmainargs (
        int *pargc,
        wchar_t ***pargv,
        wchar_t ***penvp,
        int dowildcard,
        _startupinfo * startinfo)
{
#ifdef _MT
        /* set per-thread new handler for main thread */
        _ptiddata ptd = _getptd();
#endif  /* _MT */

        /* set global new mode flag */
        _newmode = startinfo->newmode;

        if ( dowildcard )
            __wsetargv();   /* do wildcard expansion after parsing args */
        else
            _wsetargv();    /* NO wildcard expansion; just parse args */

        *pargc = __argc;
        *pargv = __wargv;

        /*
         * if wide environment does not already exist,
         * create it from multibyte environment
         */
        if (!_wenviron)
            __mbtow_environ();

        *penvp = _wenviron;
}



_CRTIMP void __cdecl __getmainargs (
        int *pargc,
        char ***pargv,
        char ***penvp,
        int dowildcard
        ,
        _startupinfo * startinfo
        )
{
#ifdef _MT
        /* set per-thread new handler for main thread */
        _ptiddata ptd = _getptd();
#endif  /* _MT */

        /* set global new mode flag */
        _newmode = startinfo->newmode;

        if ( dowildcard )
            __setargv();    /* do wildcard expansion after parsing args */
        else
            _setargv();     /* NO wildcard expansion; just parse args */

        *pargc = __argc;
        *pargv = __argv;
        *penvp = _environ;
}


/***
*BOOL _CRTDLL_INIT(hDllHandle, dwReason, lpreserved) - C DLL initialization.
*
*Purpose:
*       This routine does the C runtime initialization.
*
*Entry:
*
*Exit:
*
*******************************************************************************/

BOOL WINAPI _CRTDLL_INIT(
        HANDLE  hDllHandle,
        DWORD   dwReason,
        LPVOID  lpreserved
        )
{
        unsigned tmposver;

#ifdef _M_IX86
        HANDLE hmod;
#endif  /* _M_IX86 */

        if ( dwReason == DLL_PROCESS_ATTACH ) {

            /*
             * Get the full Win32 version. Stash it in a local variable
             * temporarily.
             */
            tmposver = GetVersion();

#ifdef _M_IX86

#ifdef DLL_FOR_WIN32S
            /*
             * Make sure we are running on Win32s
             */
            if ( ((tmposver & 0x00ff) >= 4) || !((tmposver >> 31) & 1) ) {
                __crtMessageBoxA("MSVCRT40.DLL for Win32s\n\nError: This version of MSVCRT40.DLL is only intended\nfor Win32s.",
                         "Microsoft Visual C++ Runtime Library",
                         MB_OK|MB_ICONHAND|MB_SETFOREGROUND|MB_TASKMODAL);
                return FALSE;
            }

            /*
             * For Win32S, must get and initialize the per-process data
             * structure before anything else!
             */
            if ( !AllocPerProcessDataStruct() )
                return FALSE;

#ifdef _DEBUG
            {
                int oldmode;

                /*
                 * IMPORTANT NOTE:
                 *      stricmp.asm, strnicmp.asm, and memicmp.asm hard-code the offset
                 *      of the _lc_handle[2] field within the _CRTDLLPPD structure.
                 *
                 *      This field MUST be first in the _CRTDLLPPD structure (win32s.h).
                 *
                 *      Otherwise a debug assertion at Win32s DLL startup will be triggered (crtlib.c).
                 */
                oldmode = _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_WNDW);

                _ASSERTE((((char *)&(_GetPPD()->_ppd___lc_handle[2])) - ((char *)_GetPPD())) == 8);

                _CrtSetReportMode(_CRT_WARN, oldmode);
            }
#endif  /* _DEBUG */

            /*
             * Check if we are running on Phar Lap TNT. If we are, then
             * the client process must be a console app. Otherwise, we
             * are really on Win32S and it must be a GUI app.
             */
            if ( __app_type == _UNKNOWN_APP ) {
                if ( (hmod = GetModuleHandleA( "kernel32.dll" ))
                     != NULL )
                {
                    if ( GetProcAddress( hmod, "IsTNT" )
                         != NULL )
                        __set_app_type(_CONSOLE_APP);
                    else
                        __set_app_type(_GUI_APP);
                }
            }


#else  /* DLL_FOR_WIN32S */

            /*
             * Make sure we are NOT running on Win32s
             */
            if ( ((tmposver & 0x00ff) == 3) && ((tmposver >> 31) & 1) ) {
                __crtMessageBoxA("MSVCRT40.DLL for Win32\n\nError: This version of MSVCRT40.DLL is not compatible\nwith Win32s.",
                         "Microsoft Visual C++ Runtime Library",
                         MB_OK|MB_ICONHAND|MB_SETFOREGROUND|MB_TASKMODAL);
                return FALSE;
            }

#endif  /* DLL_FOR_WIN32S */

            /*
             * Check if we are running on Phar Lap TNT. If we are, then
             * the client process must be a console app.
             */
            if ( (hmod = GetModuleHandleA( "kernel32.dll" )) != NULL ) {
                if ( GetProcAddress( hmod, "IsTNT" ) != NULL )
                    __set_app_type(_CONSOLE_APP);
            }

#endif  /* _M_IX86 */

            /*
             * Increment flag indicating process attach notification
             * has been received.
             */
            proc_attached++;


            _heap_init();           /* initialize heap */

            /*
             * Store OS version
             */
            _osver = tmposver;

            _winminor = (_osver >> 8) & 0x00FF ;
            _winmajor = _osver & 0x00FF ;
            _winver = (_winmajor << 8) + _winminor;
            _osver = (_osver >> 16) & 0x00FFFF ;

#ifdef _MT
            if(!_mtinit())          /* initialize multi-thread */
            {
                /*
                 * If the DLL load is going to fail, we must clean up
                 * all resources that have already been allocated.
                 */
                _heap_term();       /* heap is now invalid! */

                return FALSE;       /* fail DLL load on failure */
            }
#endif  /* _MT */

            _ioinit();          /* inherit file info */

            _aenvptr = (char *)__crtGetEnvironmentStringsA();

            _acmdln = (char *)GetCommandLineA();
            _wcmdln = (wchar_t *)__crtGetCommandLineW();

#if defined (_MBCS)
            /*
             * Initialize multibyte ctype table. Always done since it is
             * needed for processing the environment strings.
             */
            __initmbctable();
#endif  /* defined (_MBCS) */

            /*
             * For CRT DLL, since we don't know the type (wide or multibyte)
             * of the program, we create only the multibyte type since that
             * is by far the most likely case. Wide environment will be created
             * on demand as usual.
             */

            _setenvp();     /* get environ info */

            _cinit();           /* do C data initialize */

        }
        else if ( dwReason == DLL_PROCESS_DETACH ) {
                /*
                 * if a client process is detaching, make sure minimal
                 * runtime termination is performed and clean up our
                 * 'locks' (i.e., delete critical sections).
                 */
            if ( proc_attached > 0 ) {
                proc_attached--;

                /*
                 * Any basic clean-up done here may also need
                 * to be done below if Process Attach is partly
                 * processed and then a failure is encountered.
                 */

                if ( _C_Termination_Done == FALSE )
                    _c_exit();

#ifdef _DEBUG
                /* Dump all memory leaks */
                if (_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) & _CRTDBG_LEAK_CHECK_DF)
                {
                    _CrtSetDumpClient(NULL);
                    _CrtDumpMemoryLeaks();
                }
#endif  /* _DEBUG */

                /* Shut down lowio */
                _ioterm();
#ifdef _MT
                /* free TLS index, call _mtdeletelocks() */
                _mtterm();
#endif  /* _MT */

                /* This should be the last thing the C run-time does */
                _heap_term();   /* heap is now invalid! */

#ifdef DLL_FOR_WIN32S
                FreePerProcessDataStruct();
#endif  /* DLL_FOR_WIN32S */

            }
            else
                /* no prior process attach, just return */
                return FALSE;

        }
        else if ( dwReason == DLL_THREAD_DETACH )
        {
            _freeptd(NULL);     /* free up per-thread CRT data */
        }

        return TRUE;
}

/***
*_amsg_exit(rterrnum) - Fast exit fatal errors
*
*Purpose:
*       Exit the program with error code of 255 and appropriate error
*       message.
*
*Entry:
*       int rterrnum - error message number (amsg_exit only).
*
*Exit:
*       Calls exit() (for integer divide-by-0) or _exit() indirectly
*       through _aexit_rtn [amsg_exit].
*       For multi-thread: calls _exit() function
*
*Exceptions:
*
*******************************************************************************/

void __cdecl _amsg_exit (
        int rterrnum
        )
{

        if ( (__error_mode == _OUT_TO_STDERR) || ((__error_mode ==
               _OUT_TO_DEFAULT) && (__app_type == _CONSOLE_APP)) )
            _FF_MSGBANNER();    /* write run-time error banner */

        _NMSG_WRITE(rterrnum);      /* write message */
        _aexit_rtn(255);        /* normally _exit(255) */
}

#ifdef DLL_FOR_WIN32S

/*
 * ppdindexRefCount - usage count for the TLS index "ppdindex"
 */

static int ppdindexRefCount;


/***
*AllocPerProcessDataStruct() - allocate per-process data
*
*Purpose:
*       Allocates and initializes a _CRTDLLPPD structure, and stores a pointer
*       to it in a thread local storage slot. If necessary, the TLS slot is
*       also allocated.
*
*Entry:
*       No arguments.
*
*Exit:
*       Returns 1 for success, 0 for failure.
*
*Exceptions:
*
*******************************************************************************/
static int __cdecl
AllocPerProcessDataStruct (
        void
        )
{
        void *p;

        /*
         * Get the memory to hold the _CRTDLLPPD structure
         */
        if ( (p = VirtualAlloc( NULL,
                    sizeof(struct _CRTDLLPPD),
                    MEM_COMMIT,
                    PAGE_READWRITE )) == NULL )
            return 0;

        /*
         * If this is the first process to connect, call TlsAlloc() to get
         * a slot for per-process data.
         */
        if ( (ppdindex == 0xffffffff) &&
             ((ppdindex = TlsAlloc()) == 0xffffffff) )
        {
            VirtualFree(p, 0, MEM_RELEASE);
            return 0;
        }

        ++ ppdindexRefCount ;   /* increment reference count */

        TlsSetValue(ppdindex, p);

        /*
         * Zero out the whole structure, then initialize any component
         * requiring a non-zero (non-NULL) initial value.
         */
        memset(p, 0, sizeof(struct _CRTDLLPPD));

        /*
         * IMPORTANT NOTE:
         *      stricmp.asm, strnicmp.asm, and memicmp.asm hard-code the offset
         *      of the _lc_handle field.
         *
         *      This field MUST be first in the _CRTDLLPPD structure (win32s.h).
         *
         *      Otherwise a debug assertion at Win32s DLL startup will be triggered (crtlib.c).
         */
        ((struct _CRTDLLPPD *)p)->_ppd___lc_handle[0] = _CLOCALEHANDLE;
        ((struct _CRTDLLPPD *)p)->_ppd___lc_handle[1] = _CLOCALEHANDLE;
        ((struct _CRTDLLPPD *)p)->_ppd___lc_handle[2] = _CLOCALEHANDLE;
        ((struct _CRTDLLPPD *)p)->_ppd___lc_handle[3] = _CLOCALEHANDLE;
        ((struct _CRTDLLPPD *)p)->_ppd___lc_handle[4] = _CLOCALEHANDLE;
        ((struct _CRTDLLPPD *)p)->_ppd___lc_handle[5] = _CLOCALEHANDLE;

        /*
         * Heap
         */


#ifdef WINHEAP

        ((struct _CRTDLLPPD *)p)->_ppd__amblksiz = _PAGESIZE_;

#else  /* WINHEAP */

        ((struct _CRTDLLPPD *)p)->_ppd__amblksiz = _HEAP_GROWSIZE;

        ((struct _CRTDLLPPD *)p)->_ppd__heap_desc.pfirstdesc =
            &(((struct _CRTDLLPPD *)p)->_ppd__heap_desc.sentinel);
        ((struct _CRTDLLPPD *)p)->_ppd__heap_desc.proverdesc =
            &(((struct _CRTDLLPPD *)p)->_ppd__heap_desc.sentinel);
        ((struct _CRTDLLPPD *)p)->_ppd__heap_desc.emptylist =
            NULL;
        ((struct _CRTDLLPPD *)p)->_ppd__heap_desc.sentinel.pnextdesc =
            NULL;
        ((struct _CRTDLLPPD *)p)->_ppd__heap_desc.sentinel.pblock =
            NULL;

        ((struct _CRTDLLPPD *)p)->_ppd__heap_maxregsize = _HEAP_MAXREGSIZE_L;
        ((struct _CRTDLLPPD *)p)->_ppd__heap_regionsize = _HEAP_MAXREGSIZE_L;
        ((struct _CRTDLLPPD *)p)->_ppd__heap_resetsize = 0xffffffff;

#endif  /* WINHEAP */

        /*
         * Misc
         */

        ((struct _CRTDLLPPD *)p)->_ppd_cachein[0] = 'C';
        ((struct _CRTDLLPPD *)p)->_ppd_cachein[1] = '\0';
        ((struct _CRTDLLPPD *)p)->_ppd_cacheout[0] = 'C';
        ((struct _CRTDLLPPD *)p)->_ppd_cacheout[1] = '\0';

        ((struct _CRTDLLPPD *)p)->_ppd___decimal_point = ".";

        ((struct _CRTDLLPPD *)p)->_ppd___decimal_point_length = 1;

        ((struct _CRTDLLPPD *)p)->_ppd___lc_codepage = _CLOCALECP;

        ((struct _CRTDLLPPD *)p)->_ppd___lc_category[0].catname =
            "LC_ALL";
        ((struct _CRTDLLPPD *)p)->_ppd___lc_category[0].locale =
            NULL;
        ((struct _CRTDLLPPD *)p)->_ppd___lc_category[0].init =
            __init_dummy;

        ((struct _CRTDLLPPD *)p)->_ppd___lc_category[1].catname =
            "LC_COLLATE";
        ((struct _CRTDLLPPD *)p)->_ppd___lc_category[1].locale =
            __clocalestr;
        ((struct _CRTDLLPPD *)p)->_ppd___lc_category[1].init =
            __init_collate;

        ((struct _CRTDLLPPD *)p)->_ppd___lc_category[2].catname =
            "LC_CTYPE";
        ((struct _CRTDLLPPD *)p)->_ppd___lc_category[2].locale =
            __clocalestr;
        ((struct _CRTDLLPPD *)p)->_ppd___lc_category[2].init =
            __init_ctype;

        ((struct _CRTDLLPPD *)p)->_ppd___lc_category[3].catname =
            "LC_MONETARY";
        ((struct _CRTDLLPPD *)p)->_ppd___lc_category[3].locale =
            __clocalestr;
        ((struct _CRTDLLPPD *)p)->_ppd___lc_category[3].init =
            __init_monetary;

        ((struct _CRTDLLPPD *)p)->_ppd___lc_category[4].catname =
            "LC_NUMERIC";
        ((struct _CRTDLLPPD *)p)->_ppd___lc_category[4].locale =
            __clocalestr;
        ((struct _CRTDLLPPD *)p)->_ppd___lc_category[4].init =
            __init_numeric;

        ((struct _CRTDLLPPD *)p)->_ppd___lc_category[5].catname =
            "LC_TIME";
        ((struct _CRTDLLPPD *)p)->_ppd___lc_category[5].locale =
            __clocalestr;
        ((struct _CRTDLLPPD *)p)->_ppd___lc_category[5].init =
            __init_time;

        ((struct _CRTDLLPPD *)p)->_ppd___lconv =
            &((struct _CRTDLLPPD *)p)->_ppd___lconv_c;

        ((struct _CRTDLLPPD *)p)->_ppd___lconv_c.decimal_point =
            __lconv_static_decimal;
        ((struct _CRTDLLPPD *)p)->_ppd___lconv_c.thousands_sep =
            __lconv_static_null;
        ((struct _CRTDLLPPD *)p)->_ppd___lconv_c.grouping =
            __lconv_static_null;
        ((struct _CRTDLLPPD *)p)->_ppd___lconv_c.int_curr_symbol =
            __lconv_static_null;
        ((struct _CRTDLLPPD *)p)->_ppd___lconv_c.currency_symbol =
            __lconv_static_null;
        ((struct _CRTDLLPPD *)p)->_ppd___lconv_c.mon_decimal_point =
            __lconv_static_null;
        ((struct _CRTDLLPPD *)p)->_ppd___lconv_c.mon_thousands_sep =
            __lconv_static_null;
        ((struct _CRTDLLPPD *)p)->_ppd___lconv_c.mon_grouping =
            __lconv_static_null;
        ((struct _CRTDLLPPD *)p)->_ppd___lconv_c.positive_sign =
            __lconv_static_null;
        ((struct _CRTDLLPPD *)p)->_ppd___lconv_c.negative_sign =
            __lconv_static_null;
        ((struct _CRTDLLPPD *)p)->_ppd___lconv_c.int_frac_digits =
            CHAR_MAX;
        ((struct _CRTDLLPPD *)p)->_ppd___lconv_c.frac_digits =
            CHAR_MAX;
        ((struct _CRTDLLPPD *)p)->_ppd___lconv_c.p_cs_precedes =
            CHAR_MAX;
        ((struct _CRTDLLPPD *)p)->_ppd___lconv_c.p_sep_by_space =
            CHAR_MAX;
        ((struct _CRTDLLPPD *)p)->_ppd___lconv_c.n_cs_precedes =
            CHAR_MAX;
        ((struct _CRTDLLPPD *)p)->_ppd___lconv_c.n_sep_by_space =
            CHAR_MAX;
        ((struct _CRTDLLPPD *)p)->_ppd___lconv_c.p_sign_posn =
            CHAR_MAX;
        ((struct _CRTDLLPPD *)p)->_ppd___lconv_c.n_sign_posn =
            CHAR_MAX;

        ((struct _CRTDLLPPD *)p)->_ppd___mb_cur_max = 1;

        ((struct _CRTDLLPPD *)p)->_ppd__pctype = _ctype + 1;

        ((struct _CRTDLLPPD *)p)->_ppd__pwctype = _ctype + 1;

#ifdef _DEBUG

        ((struct _CRTDLLPPD *)p)->_ppd__crtDbgFlag = _CRTDBG_ALLOC_MEM_DF;

        ((struct _CRTDLLPPD *)p)->_ppd__lRequestCurr = 1;

        ((struct _CRTDLLPPD *)p)->_ppd__crtBreakAlloc = -1L;

        ((struct _CRTDLLPPD *)p)->_ppd__bNoMansLandFill = 0xFD;

        ((struct _CRTDLLPPD *)p)->_ppd__bDeadLandFill   = 0xDD;

        ((struct _CRTDLLPPD *)p)->_ppd__bCleanLandFill  = 0xCD;

        ((struct _CRTDLLPPD *)p)->_ppd__crtAssertBusy = -1;

        ((struct _CRTDLLPPD *)p)->_ppd__CrtDbgMode[0] = _CRTDBG_MODE_DEBUG;
        ((struct _CRTDLLPPD *)p)->_ppd__CrtDbgMode[1] =
            ((struct _CRTDLLPPD *)p)->_ppd__CrtDbgMode[2] = _CRTDBG_MODE_WNDW;

        ((struct _CRTDLLPPD *)p)->_ppd__CrtDbgFile[0] =
            ((struct _CRTDLLPPD *)p)->_ppd__CrtDbgFile[1] =
            ((struct _CRTDLLPPD *)p)->_ppd__CrtDbgFile[2] = _CRTDBG_INVALID_HFILE;

        ((struct _CRTDLLPPD *)p)->_ppd__pfnAllocHook = _CrtDefaultAllocHook;

#endif  /* _DEBUG */

        /*
         * Startup
         */
        ((struct _CRTDLLPPD *)p)->_ppd__aexit_rtn = _exit;

        ((struct _CRTDLLPPD *)p)->_ppd___tlsindex = 0xffffffff;

        /*
         * Stdio
         */
        ((struct _CRTDLLPPD *)p)->_ppd__tempoff = 1;

        ((struct _CRTDLLPPD *)p)->_ppd__iob[0]._file =
            ((struct _CRTDLLPPD *)p)->_ppd__iob[1]._file =
            ((struct _CRTDLLPPD *)p)->_ppd__iob[2]._file = -1;
        ((struct _CRTDLLPPD *)p)->_ppd__iob[0]._flag =
            ((struct _CRTDLLPPD *)p)->_ppd__iob[1]._flag =
            ((struct _CRTDLLPPD *)p)->_ppd__iob[2]._flag = _IOREAD | _IOWRT;

        ((struct _CRTDLLPPD *)p)->_ppd__nstream = _NSTREAM_;

        /*
         * Time
         */

        ((struct _CRTDLLPPD *)p)->_ppd__daylight = 1;

        ((struct _CRTDLLPPD *)p)->_ppd__dstbias = -3600L;

        ((struct _CRTDLLPPD *)p)->_ppd__timezone = 8*3600L;

        ((struct _CRTDLLPPD *)p)->_ppd__tzname[0] =
            &(((struct _CRTDLLPPD *)p)->_ppd_tzstd[0]);
        ((struct _CRTDLLPPD *)p)->_ppd__tzname[1] =
            &(((struct _CRTDLLPPD *)p)->_ppd_tzdst[0]);

        ((struct _CRTDLLPPD *)p)->_ppd_tzstd[0] =
            ((struct _CRTDLLPPD *)p)->_ppd_tzdst[0] = 'P';

        ((struct _CRTDLLPPD *)p)->_ppd_tzstd[1] = 'S';
        ((struct _CRTDLLPPD *)p)->_ppd_tzdst[1] = 'D';

        ((struct _CRTDLLPPD *)p)->_ppd_tzstd[2] =
            ((struct _CRTDLLPPD *)p)->_ppd_tzdst[2] = 'T';

        ((struct _CRTDLLPPD *)p)->_ppd_tzstd[3] =
            ((struct _CRTDLLPPD *)p)->_ppd_tzdst[3] = '\0';;

        ((struct _CRTDLLPPD *)p)->_ppd___lc_time_curr = &__lc_time_c;

        return 1;
}

/***
*_GetPPD() - Get per-process data
*
*Purpose:
*       Fetches a pointer to the per-process data structure (struct
*       _CRTDLLPPD).
*
*Entry:
*
*Exit:
*       Returns the fetched pointer.
*
*Exceptions:
*
*******************************************************************************/
struct _CRTDLLPPD * __cdecl
_GetPPD (
        void
        )
{
        return (struct _CRTDLLPPD *)TlsGetValue(ppdindex);
}

/***
*FreePerProcessDataStruct() - Free the per-process data.
*
*Purpose:
*       Frees up the memory allocation which held the per-process data
*       structure.
*
*Entry:
*       No arguments.
*
*Exit:
*       No return value.
*
*Exceptions:
*
*******************************************************************************/
static void __cdecl
FreePerProcessDataStruct (
        void
        )
{
        void *p;

        if ( (p = TlsGetValue(ppdindex)) != NULL ) {
            VirtualFree(p, 0, MEM_RELEASE);
            TlsSetValue(ppdindex, NULL);
        }

        /*
         * Free the TLS index "ppdindex" when the last process disconnects
         */

        if ( -- ppdindexRefCount == 0 ) {   /* decrement reference count */
            if ( ppdindex != 0xffffffff ) {
                TlsFree( ppdindex );
                ppdindex = 0xffffffff;
            }
        }
}

#endif  /* DLL_FOR_WIN32S */

#if defined (_M_IX86)

/*
 * Functions to access user-visible, per-process variables
 */

/*
 * Macro to construct the name of the access function from the variable
 * name.
 */
#define AFNAME(var) __p_ ## var

/*
 * Conditionally defined macro to construct the access function's return
 * value from the variable name.
 */
#ifdef DLL_FOR_WIN32S
#define AFRET(var)  &(((struct _CRTDLLPPD *)TlsGetValue(ppdindex))->_ppd_ ## var ## )
#else  /* DLL_FOR_WIN32S */
#define AFRET(var)  &var
#endif  /* DLL_FOR_WIN32S */

/*
 ***
 ***  Template
 ***

_CRTIMP __cdecl
AFNAME() (void)
{
        return AFRET();
}

 ***
 ***
 ***
 */

#ifdef _DEBUG

_CRTIMP long *
AFNAME(_crtAssertBusy) (void)
{
        return AFRET(_crtAssertBusy);
}

_CRTIMP long *
AFNAME(_crtBreakAlloc) (void)
{
        return AFRET(_crtBreakAlloc);
}

_CRTIMP int *
AFNAME(_crtDbgFlag) (void)
{
        return AFRET(_crtDbgFlag);
}

#endif  /* _DEBUG */

_CRTIMP char ** __cdecl
AFNAME(_acmdln) (void)
{
        return AFRET(_acmdln);
}

_CRTIMP wchar_t ** __cdecl
AFNAME(_wcmdln) (void)
{
        return AFRET(_wcmdln);
}

_CRTIMP unsigned int * __cdecl
AFNAME(_amblksiz) (void)
{
        return AFRET(_amblksiz);
}

_CRTIMP int * __cdecl
AFNAME(__argc) (void)
{
        return AFRET(__argc);
}

_CRTIMP char *** __cdecl
AFNAME(__argv) (void)
{
        return AFRET(__argv);
}

_CRTIMP wchar_t *** __cdecl
AFNAME(__wargv) (void)
{
        return AFRET(__wargv);
}

_CRTIMP int * __cdecl
AFNAME(_commode) (void)
{
        return AFRET(_commode);
}

_CRTIMP int * __cdecl
AFNAME(_daylight) (void)
{
        return AFRET(_daylight);
}

_CRTIMP long * __cdecl
AFNAME(_dstbias) (void)
{
        return AFRET(_dstbias);
}

_CRTIMP char *** __cdecl
AFNAME(_environ) (void)
{
        return AFRET(_environ);
}

_CRTIMP wchar_t *** __cdecl
AFNAME(_wenviron) (void)
{
        return AFRET(_wenviron);
}

_CRTIMP int * __cdecl
AFNAME(_fmode) (void)
{
        return AFRET(_fmode);
}

_CRTIMP char *** __cdecl
AFNAME(__initenv) (void)
{
        return AFRET(__initenv);
}

_CRTIMP wchar_t *** __cdecl
AFNAME(__winitenv) (void)
{
        return AFRET(__winitenv);
}

_CRTIMP FILE *
AFNAME(_iob) (void)
{
#ifdef DLL_FOR_WIN32S
        return &(((struct _CRTDLLPPD *)TlsGetValue(ppdindex))->_ppd__iob[0]);
#else  /* DLL_FOR_WIN32S */
        return &_iob[0];
#endif  /* DLL_FOR_WIN32S */
}

_CRTIMP unsigned char * __cdecl
AFNAME(_mbctype) (void)
{
#ifdef DLL_FOR_WIN32S
        return &(((struct _CRTDLLPPD *)TlsGetValue(ppdindex))->_ppd__mbctype[0]);
#else  /* DLL_FOR_WIN32S */
        return &_mbctype[0];
#endif  /* DLL_FOR_WIN32S */
}

_CRTIMP int * __cdecl
AFNAME(__mb_cur_max) (void)
{
        return AFRET(__mb_cur_max);
}


_CRTIMP unsigned int * __cdecl
AFNAME(_osver) (void)
{
        return AFRET(_osver);
}

_CRTIMP unsigned short ** __cdecl
AFNAME(_pctype) (void)
{
        return AFRET(_pctype);
}

_CRTIMP unsigned short ** __cdecl
AFNAME(_pwctype) (void)
{
        return AFRET(_pwctype);
}

_CRTIMP char **  __cdecl
AFNAME(_pgmptr) (void)
{
        return AFRET(_pgmptr);
}

_CRTIMP wchar_t ** __cdecl
AFNAME(_wpgmptr) (void)
{
        return AFRET(_wpgmptr);
}

_CRTIMP long * __cdecl
AFNAME(_timezone) (void)
{
        return AFRET(_timezone);
}

_CRTIMP char ** __cdecl
AFNAME(_tzname) (void)
{
#ifdef DLL_FOR_WIN32S
        return &(((struct _CRTDLLPPD *)TlsGetValue(ppdindex))->_ppd__tzname[0]);
#else  /* DLL_FOR_WIN32S */
        return &_tzname[0];
#endif  /* DLL_FOR_WIN32S */
}

_CRTIMP unsigned int * __cdecl
AFNAME(_winmajor) (void)
{
        return AFRET(_winmajor);
}

_CRTIMP unsigned int * __cdecl
AFNAME(_winminor) (void)
{
        return AFRET(_winminor);
}

_CRTIMP unsigned int * __cdecl
AFNAME(_winver) (void)
{
        return AFRET(_winver);
}

#endif  /* defined (_M_IX86) */

#endif  /* CRTDLL */
