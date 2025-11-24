/*
 * IENUM0.H
 *
 * Definition of an IEnumRECT interface as an example of the
 * interface notion introduced in OLE 2.0 with the Component Object
 * Model as well as the idea of enumerators.  This include file
 * defines the interface differently for C or C++.
 *
 * Copyright (c)1993-1994 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _IENUM0_H_
#define _IENUM0_H_


//C++ Definition of an interface.
#ifdef __cplusplus


//This is the interface:  a struct of pure virtual functions.
struct IEnumRECT
    {
    virtual DWORD AddRef(void)=0;
    virtual DWORD Release(void)=0;
    virtual BOOL  Next(DWORD, LPRECT, LPDWORD)=0;
    virtual BOOL  Skip(DWORD)=0;
    virtual void  Reset(void)=0;
    };

typedef IEnumRECT *PENUMRECT;

#else   //!__cplusplus

/*
 * A C interface is explicitly a structure containing a long
 * pointer to a virtual function table that we have to
 * initialize explicitly.
 */

typedef struct
    {
    struct IEnumRECTVtbl FAR *lpVtbl;
    } IEnumRECT;

typedef IEnumRECT *PENUMRECT;

//This is just a convenient naming
typedef struct IEnumRECTVtbl IEnumRECTVtbl;


struct IEnumRECTVtbl
    {
    DWORD (* AddRef)(PENUMRECT);
    DWORD (* Release)(PENUMRECT);
    BOOL  (* Next)(PENUMRECT, DWORD, LPRECT, LPDWORD);
    BOOL  (* Skip)(PENUMRECT, DWORD);
    void  (* Reset)(PENUMRECT);
    };

#endif  //!__cplusplus

#endif //_IENUM0_H_
