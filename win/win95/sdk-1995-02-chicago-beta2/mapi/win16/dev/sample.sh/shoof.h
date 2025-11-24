/*
 -  S H O O F . H
 -
 *  Purpose:
 *      This is a sample SpoolerMsgHook Provider for generating
 *      Out of OFfice messages on every message received.
 *
 *  Copyright 1992-94 Microsoft Corporation.  All Rights Reserved.
 *
 */

#ifdef __cplusplus
extern "C"
{
#endif

/*
 *  OOF Hook Provider Init Object
 */

#undef  INTERFACE
#define INTERFACE struct _SH

#undef  MAPIMETHOD_
#define MAPIMETHOD_(type, method)   MAPIMETHOD_DECLARE(type, method, SH_)
MAPI_IUNKNOWN_METHODS (IMPL)
MAPI_ISPOOLERHOOK_METHODS (IMPL)

#undef  MAPIMETHOD_
#define MAPIMETHOD_(type, method)   STDMETHOD_(type, method)

DECLARE_MAPI_INTERFACE (SH_)
{
    MAPI_IUNKNOWN_METHODS (IMPL)
    MAPI_ISPOOLERHOOK_METHODS (IMPL)
};

/*
 *  OOF Hook Provider Init Structure
 */

typedef struct _SH SH;
typedef struct _SH FAR *LPSH;

typedef struct _SH
{
    SH_Vtbl FAR *lpVtbl;                /* Jump table of methods            */
    ULONG ulcRef;                       /* Reference Count                  */
    HINSTANCE hInstance;                /* This instance                    */
    LPMAPISESSION lpSession;            /* Given to us at init time         */
    LPALLOCATEBUFFER lpfAllocateBuffer; /* We'll keep the memory allocators */
    LPALLOCATEMORE lpfAllocateMore;     /* here so we don't have to keep    */
    LPFREEBUFFER lpfFreeBuffer;         /* getting them when we need them.  */
    MAPIUID UIDSection;                 /* MUID of Profile Section          */
};

/* Function Prototypes */

STDMETHODIMP HrCreateOOFMessage (LPSH lpsh,
    LPMAPIFOLDER lpFolder,
    LPMESSAGE lpMsgOrig,
    LPMESSAGE FAR * lppMsg);

#ifdef __cplusplus
}
#endif
