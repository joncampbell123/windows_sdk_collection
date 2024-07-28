/**[f******************************************************************
* nocrap.h -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1989-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.  Company confidential.
*
**f]*****************************************************************/
  
#define NOATOM      /* Atom Manager routines */
#define NOBITMAP    /* typedef HBITMAP and associated routines */
#define NOBRUSH     /* typedef HBRUSH and associated routines */
#define NOCLIPBOARD /* clipboard routines */
#define NOCOLOR     /* CTLCOLOR_*, COLOR_* */
#define NOCOMM      /* COMM driver routines */
#define NOCREATESTRUCT  /* typedef CREATESTRUCT */
#define NOCTLMGR    /* control and dialog routines */
#define NODRAWTEXT  /* DrawText() and DT_* */
#define NOFONT      /* typedef FONT and associated routines */
#define NOGDI       /* StretchBlt modes and gdi logical objects */
#define NOGDICAPMASKS   /* CC_*, LC_*, PC_*, CP_*, TC_*, RC_ */
#define NOHDC       /* typedef HDC and associated routines */
#define NOICON      /* IDI_* */
#define NOKANJI     /* Kanji support stuff. */
#define NOKEYBOARDINFO  /* keyboard information functions */
#define NOKEYSTATE  /* MK_* */
#define NOLANGUAGE  /* 3.0 language-dependent text functions */
#define NOMB        /* MB_* and MessageBox() */
#define NOMEMMGR    /* GMEM_*, LMEM_*, GHND, LHND, associated routines */
#define NOMENUS     /* MF_*, HMENU and associated routines */
#define NOMETAFILE  /* typedef METAFILEPICT */
#define NOMINMAX    /* Macros min(a,b) and max(a,b) */
#define NOMSG       /* typedef MSG and associated routines */
#define NONCMESSAGES    /* WM_NC* and HT* */
#define NOOPENFILE  /* OpenFile(), OemToAnsi, AnsiToOem, and OF_* */
#define NOPEN       /* typedef HPEN and associated routines */
#define NORASTEROPS /* binary and tertiary raster ops */
#define NOREGION    /* typedef HRGN and associated routines */
#define NORESOURCE  /* Predefined resource types:  RT_* */
#define NOSCROLL    /* SB_* and scrolling routines */
#define NOSHOWWINDOW    /* SHOW_* and HIDE_* */
#define NOSOUND     /* Sound driver routines */
#define NOSYSCOMMANDS   /* SC_* */
#define NOSYSMETRICS    /* SM_*, GetSystemMetrics */
#define NOTEXTMETRIC    /* typedef TEXTMETRIC and associated routines */
#define NOVIRTUALKEYCODES   /* VK_* */
#define NOWH        /* SetWindowsHook and WH_* */
#define NOWINMESSAGES   /* WM_* */
#define NOWINOFFSETS    /* GWL_*, GCL_*, associated routines */
#define NOWINSTYLES /* WS_*, CS_*, ES_*, LBS_* */
#define NOWNDCLASS  /* typedef WNDCLASS and associated routines */
