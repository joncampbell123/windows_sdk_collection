/*
 *  MATH64.C
 *  
 *  64-bit integer arithmetic
 *
 *  Copyright 1993-1995 Microsoft Corporation. All Rights Reserved.
 */

#pragma warning(disable:4001)   /* single line comments */
#pragma warning(disable:4054)   /* cast function pointer to data pointer */
#pragma warning(disable:4100)   /* unreferenced formal parameter */
#pragma warning(disable:4127)   /* conditional expression is constant */
#pragma warning(disable:4201)   /* nameless struct/union */
#pragma warning(disable:4204)   /* non-constant aggregate initializer */
#pragma warning(disable:4209)   /* benign typedef redefinition */
#pragma warning(disable:4214)   /* bit field types other than int */
#pragma warning(disable:4505)   /* unreferenced local function removed */
#pragma warning(disable:4514)   /* unreferenced inline function removed */
#pragma warning(disable:4702)   /* unreachable code */
#pragma warning(disable:4704)   /* inline assembler turns off global optimizer */
#pragma warning(disable:4705)   /* statement has no effect */
#pragma warning(disable:4706)   /* assignment within conditional expression */
#pragma warning(disable:4710)   /* function not expanded */
#pragma warning(disable:4115)   /* named type def in parens */

#ifdef WIN32
#define INC_OLE2 /* Get the OLE2 stuff */
#define INC_RPC  /* harmless on Daytona; Chicago needs it */
#endif
#include <windows.h>
#pragma warning(disable:4001)   /* single line comments */
#include <windowsx.h>
#include <mapiwin.h>
#include <mapidbg.h>
#ifdef WIN32
#include <objerror.h>
#include <objbase.h>
#endif
#if defined (WIN16) || defined (DOS)
#include <compobj.h>
#endif
#include <mapiutil.h>
#include <mapiperf.h>

#include <_mapiwin.h>

#pragma SEGMENT(MAPI_Util)

/*  Unsigned 64-bit add */
STDAPI_(FILETIME) 
FtAddFt(FILETIME ft1, FILETIME ft2)
{
    FILETIME    ft;

    ft.dwLowDateTime = ft1.dwLowDateTime + ft2.dwLowDateTime;
    ft.dwHighDateTime = ft1.dwHighDateTime + ft2.dwHighDateTime +
        ((ft.dwLowDateTime < ft1.dwLowDateTime ||
            ft.dwLowDateTime < ft2.dwLowDateTime) ?
                1L : 0L);

    return ft;
}

STDAPI_(FILETIME) 
FtAdcFt(FILETIME ft1, FILETIME ft2, WORD FAR *pwCarry)
{
    FILETIME    ft;
    WORD        wCarry;

    wCarry = (WORD) (pwCarry ? *pwCarry : 0);

    ft.dwLowDateTime = ft1.dwLowDateTime + ft2.dwLowDateTime + wCarry;
    wCarry = (WORD) (((ft.dwLowDateTime < ft1.dwLowDateTime ||
        ft.dwLowDateTime < ft2.dwLowDateTime)) ?
            1 : 0);

    ft.dwHighDateTime = ft1.dwHighDateTime + ft2.dwHighDateTime + wCarry;
    if (pwCarry)
        *pwCarry = (WORD) (((ft.dwHighDateTime < ft1.dwHighDateTime ||
            ft.dwHighDateTime < ft2.dwHighDateTime)) ?
                1 : 0);

    return ft;
}


/*  Unsigned 64-bit subtract */
STDAPI_(FILETIME)
FtSubFt(FILETIME ftMinuend, FILETIME ftSubtrahend)
{
    FILETIME ft;

    ft = FtNegFt(ftSubtrahend);
    return FtAddFt(ftMinuend, ft);
}

/*  Unsigned 32 by 64-bit multiply */
STDAPI_(FILETIME)
FtMulDw(DWORD dw, FILETIME ft)
{
    FILETIME    ftProd = { 0, 0 };

    while (dw)
    {
        if (dw & 1)
            ftProd = FtAddFt(ftProd, ft);
        ft.dwHighDateTime <<= 1;
        if (ft.dwLowDateTime & 0x80000000)
            ft.dwHighDateTime |= 1;
        ft.dwLowDateTime <<= 1;
        dw >>= 1;
    }
    return ftProd;
}

/*  Unsigned 32 by 32-bit multiply */
STDAPI_(FILETIME)
FtMulDwDw(DWORD dw1, DWORD dw2)
{
    FILETIME ft;

    ft.dwLowDateTime = dw2;
    ft.dwHighDateTime = 0L;

    return FtMulDw(dw1, ft);
}


/*  64-bit two's complement */
STDAPI_(FILETIME)
FtNegFt(FILETIME ft)
{

    ft.dwLowDateTime = ~(ft.dwLowDateTime) + 1;
    ft.dwHighDateTime = ~(ft.dwHighDateTime) +
        (ft.dwLowDateTime ? 0 : 1);

    return ft;
}

/*  ftMagicDivisor is the reciprocal of the thing we want to divide by. */
/*  So this works by multiplying, then shifting down. */

STDAPI_(FILETIME)
FtDivFtBogus(FILETIME Dividend, FILETIME MagicDivisor, CHAR ShiftCount)
{
    FILETIME    ft1;
    FILETIME    ftAcc;
    WORD        wCarry1 = 0;
    WORD        wCarry2 = 0;

    Assert(ShiftCount <= 31);

    ft1 = FtMulDwDw(MagicDivisor.dwLowDateTime, Dividend.dwLowDateTime);
    /*  Low dword of result falls off */
    ftAcc.dwLowDateTime = ft1.dwHighDateTime;
    ftAcc.dwHighDateTime = 0;

    ft1 = FtMulDwDw(MagicDivisor.dwHighDateTime, Dividend.dwLowDateTime);
    ftAcc = FtAdcFt(ftAcc, ft1, &wCarry1);

    ft1 = FtMulDwDw(MagicDivisor.dwLowDateTime, Dividend.dwHighDateTime);
    ftAcc = FtAdcFt(ftAcc, ft1, &wCarry2);

    ft1 = FtMulDwDw(MagicDivisor.dwHighDateTime, Dividend.dwHighDateTime);
    ftAcc.dwLowDateTime = ftAcc.dwHighDateTime;
    ftAcc.dwHighDateTime = wCarry2 + wCarry1;
    ftAcc = FtAddFt(ftAcc, ft1);

    /*  At this point, ftAcc.dwLowDateTime has the low 32 bits of the product, */
    /*  and ft2 has the next 64 bits. Take the high 64 bits, shifted */
    /*  right by the ShiftCount. */

    ftAcc.dwLowDateTime = (ftAcc.dwLowDateTime >> ShiftCount) |
        (ftAcc.dwHighDateTime << (32 - ShiftCount));
    ftAcc.dwHighDateTime >>= ShiftCount;

    return ftAcc;
}

