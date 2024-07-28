//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;
//
//  acmthunk.c
//
//  Description:
//      This is a thunk layer to the Audio Compression Manager. It's 
//      purpose is to allow an application to use the ACM only if it is
//      available (like under Win 3.1 and NT where the ACM may or may not
//      be installed).
//
//      There are two requirements for using this module:
//
//      1.  Compile and link with this module before linking to MMSYSTEM.LIB.
//          Do *NOT* link with MSACM.LIB.
//
//      2.  Before calling any other functions in ACM function set, call
//          acmThunkInitialize(). This will cause all dyna-linking to occur.
//
//      3.  Before exiting your application, call acmThunkTerminate().
//          This will unlink to the ACM and free allocated resources.
//
//      NOTE! this could be written more efficiently in Assembly by doing
//      a jump to the correct API in the ACM, but this would not be
//      portable (easily) on NT. So the arguments will be repushed on the
//      stack.. such is life.
//
//==========================================================================;

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <msacm.h>

//
//
//
#ifdef WIN32
    #define BCODE
#else
    #define BCODE           _based(_segname("_CODE"))
#endif


//==========================================================================;
//
//  Prototypes and String Defines for Dyna-Linking to the ACM
//
//
//==========================================================================;

#define ACMINST_NOT_PRESENT     NULL
#define ACMINST_TRY_LINKING     (HINSTANCE)(UINT)-1

static HINSTANCE    ghinstAcm   = ACMINST_TRY_LINKING;

#ifdef WIN32
TCHAR BCODE gszAcmModuleName[]  = TEXT("MSACM32.DLL");
#else
char BCODE  gszAcmModuleName[]  = "MSACM.DLL";
#endif

FARPROC    *gpafnAcmFunctions;

PSTR BCODE  gapszAcmFunctions[] =
{
    "acmGetVersion",
    "acmMetrics",
    "acmDriverEnum",
    "acmDriverDetails",
    "acmDriverAdd",
    "acmDriverRemove",
    "acmDriverOpen",
    "acmDriverClose",
    "acmDriverMessage",
    "acmDriverID",
    "acmDriverPriority",
    "acmFormatTagDetails",
    "acmFormatTagEnum",
    "acmFormatChoose",
    "acmFormatDetails",
    "acmFormatEnum",
    "acmFormatSuggest",
    "acmFilterTagDetails",
    "acmFilterTagEnum",
    "acmFilterChoose",
    "acmFilterDetails",
    "acmFilterEnum",
    "acmStreamOpen",
    "acmStreamClose",
    "acmStreamSize",
    "acmStreamConvert",
    "acmStreamReset",
    "acmStreamPrepareHeader",
    "acmStreamUnprepareHeader"
};

#define ACMTHUNK_MAX_FUNCTIONS      (sizeof(gapszAcmFunctions)/sizeof(gapszAcmFunctions[0]))
#define ACMTHUNK_SIZE_TABLE_BYTES   (ACMTHUNK_MAX_FUNCTIONS * sizeof(FARPROC))

#define ACMTHUNK_GETVERSION             0
#define ACMTHUNK_METRICS                1
#define ACMTHUNK_DRIVERENUM             2
#define ACMTHUNK_DRIVERDETAILS          3
#define ACMTHUNK_DRIVERADD              4
#define ACMTHUNK_DRIVERREMOVE           5
#define ACMTHUNK_DRIVEROPEN             6
#define ACMTHUNK_DRIVERCLOSE            7
#define ACMTHUNK_DRIVERMESSAGE          8
#define ACMTHUNK_DRIVERID               9
#define ACMTHUNK_DRIVERPRIORITY         10
#define ACMTHUNK_FORMATTAGDETAILS       11
#define ACMTHUNK_FORMATTAGENUM          12
#define ACMTHUNK_FORMATCHOOSE           13
#define ACMTHUNK_FORMATDETAILS          14
#define ACMTHUNK_FORMATENUM             15
#define ACMTHUNK_FORMATSUGGEST          16
#define ACMTHUNK_FILTERTAGDETAILS       17
#define ACMTHUNK_FILTERTAGENUM          18
#define ACMTHUNK_FILTERCHOOSE           19
#define ACMTHUNK_FILTERDETAILS          20
#define ACMTHUNK_FILTERENUM             21
#define ACMTHUNK_STREAMOPEN             22
#define ACMTHUNK_STREAMCLOSE            23
#define ACMTHUNK_STREAMSIZE             24
#define ACMTHUNK_STREAMCONVERT          25
#define ACMTHUNK_STREAMRESET            26
#define ACMTHUNK_STREAMPREPAREHEADER    27
#define ACMTHUNK_STREAMUNPREPAREHEADER  28


//==========================================================================;
//
//
//
//==========================================================================;

//--------------------------------------------------------------------------;
//  
//  BOOL acmThunkInitialize
//  
//  Description:
//  
//  
//  Arguments:
//      None.
//  
//  Return (BOOL):
//  
//--------------------------------------------------------------------------;

BOOL FAR PASCAL acmThunkInitialize
(
    void
)
{
    DWORD (ACMAPI *pfnAcmGetVersion)
    (
        void
    );

    UINT            fuErrorMode;
    DWORD           dwVersion;
    UINT            u;

    //
    //  if we have already linked to the API's, then just succeed...
    //
    if (NULL != gpafnAcmFunctions)
    {
        //
        //  someone isn't satisfied with calling this API only once?
        //
        return (TRUE);
    }


    //
    //  if we have already tried to link to the ACM, then fail this
    //  call--it isn't present.
    //
    if (ACMINST_TRY_LINKING != ghinstAcm)
        return (FALSE);


    //
    //  try to get a handle on the ACM--if we cannot do this, then fail
    //
    fuErrorMode = SetErrorMode(SEM_NOOPENFILEERRORBOX);
    ghinstAcm = LoadLibrary(gszAcmModuleName);
    SetErrorMode(fuErrorMode);
    if (ghinstAcm < HINSTANCE_ERROR)
    {
        ghinstAcm = ACMINST_NOT_PRESENT;
        return (FALSE);
    }

    (FARPROC)pfnAcmGetVersion = GetProcAddress(ghinstAcm, gapszAcmFunctions[ACMTHUNK_GETVERSION]);
    if (NULL == pfnAcmGetVersion)
    {
        FreeLibrary(ghinstAcm);
        ghinstAcm = ACMINST_NOT_PRESENT;

        return (FALSE);
    }


    //
    //  allocate our array of function pointers to the ACM... note that
    //  this is dynamically allocated so if the ACM is _not_ present,
    //  then this code and data takes up very little space.
    //
    gpafnAcmFunctions = (FARPROC *)LocalAlloc(LPTR, ACMTHUNK_SIZE_TABLE_BYTES);
    if (NULL == gpafnAcmFunctions)
    {
        FreeLibrary(ghinstAcm);
        ghinstAcm = ACMINST_NOT_PRESENT;

        return (FALSE);
    }

    gpafnAcmFunctions[ACMTHUNK_GETVERSION] = (FARPROC)pfnAcmGetVersion;

    //
    //  if the version of the ACM is *NOT* V2.00 or greater, then
    //  all other API's are unavailable--so don't waste time trying
    //  to link to them.
    //
    dwVersion = (* pfnAcmGetVersion)();
    if (0x0200 > HIWORD(dwVersion))
    {
        return (TRUE);
    }


    //
    //  yipee! the ACM V2.00 or greater appears to be installed and
    //  happy with us--so link to the rest of the nifty cool API's.
    //
    //  start at index 1 since we already linked to acmGetVersion above
    //
    for (u = 1; u < ACMTHUNK_MAX_FUNCTIONS; u++)
    {
        gpafnAcmFunctions[u] = GetProcAddress(ghinstAcm, gapszAcmFunctions[u]);
    }


    //
    //  finally, return success
    //
    return (TRUE);
} // acmThunkInitialize()


//--------------------------------------------------------------------------;
//  
//  BOOL acmThunkTerminate
//  
//  Description:
//  
//  
//  Arguments:
//      None.
//  
//  Return (BOOL):
//  
//--------------------------------------------------------------------------;

BOOL FAR PASCAL acmThunkTerminate
(
    void
)
{
    //
    //
    //
    if (NULL != gpafnAcmFunctions)
    {
        LocalFree((HLOCAL)gpafnAcmFunctions);

        gpafnAcmFunctions = NULL;

        FreeLibrary(ghinstAcm);
        ghinstAcm = ACMINST_TRY_LINKING;
    }

    return (TRUE);
} // acmThunkTerminate()


//==========================================================================;
//
//  General Information API's
//
//
//==========================================================================;

DWORD ACMAPI acmGetVersion
(
    void
)
{
    DWORD (ACMAPI *pfnAcmGetVersion)
    (
        void
    );

    DWORD           dwVersion;

    if (NULL == gpafnAcmFunctions)
        return (0L);

    (FARPROC)pfnAcmGetVersion = gpafnAcmFunctions[ACMTHUNK_GETVERSION];
    if (NULL == pfnAcmGetVersion)
        return (0L);

    dwVersion = (* pfnAcmGetVersion)();

    return (dwVersion);
} // acmGetVersion()


//--------------------------------------------------------------------------;
//--------------------------------------------------------------------------;

MMRESULT ACMAPI acmMetrics
(
    HACMOBJ                 hao,
    UINT                    uMetric,
    LPVOID                  pMetric
)
{
    MMRESULT (ACMAPI *pfnAcmMetrics)
    (
        HACMOBJ                 hao,
        UINT                    uMetric,
        LPVOID                  pMetric
    );

    MMRESULT        mmr;

    if (NULL == gpafnAcmFunctions)
        return (MMSYSERR_ERROR);

    (FARPROC)pfnAcmMetrics = gpafnAcmFunctions[ACMTHUNK_METRICS];
    if (NULL == pfnAcmMetrics)
        return (MMSYSERR_ERROR);

    mmr = (* pfnAcmMetrics)(hao, uMetric, pMetric);

    return (mmr);
} // acmMetrics()


//==========================================================================;
//
//  ACM Driver Management API's
//
//
//==========================================================================;

//--------------------------------------------------------------------------;
//--------------------------------------------------------------------------;

MMRESULT ACMAPI acmDriverEnum
(
    ACMDRIVERENUMCB         fnCallback,
    DWORD                   dwInstance,
    DWORD                   fdwEnum
)
{
    MMRESULT (ACMAPI *pfnAcmDriverEnum)
    (
        ACMDRIVERENUMCB         fnCallback,
        DWORD                   dwInstance,
        DWORD                   fdwEnum
    );

    MMRESULT        mmr;

    if (NULL == gpafnAcmFunctions)
        return (MMSYSERR_ERROR);

    (FARPROC)pfnAcmDriverEnum = gpafnAcmFunctions[ACMTHUNK_DRIVERENUM];
    if (NULL == pfnAcmDriverEnum)
        return (MMSYSERR_ERROR);

    mmr = (* pfnAcmDriverEnum)(fnCallback, dwInstance, fdwEnum);

    return (mmr);
}


//--------------------------------------------------------------------------;
//--------------------------------------------------------------------------;

MMRESULT ACMAPI acmDriverDetails
(
    HACMDRIVERID            hadid,
    LPACMDRIVERDETAILS      padd,
    DWORD                   fdwDetails
)
{
    MMRESULT (ACMAPI *pfnAcmDriverDetails)
    (
        HACMDRIVERID            hadid,
        LPACMDRIVERDETAILS      padd,
        DWORD                   fdwDetails
    );

    MMRESULT        mmr;

    if (NULL == gpafnAcmFunctions)
        return (MMSYSERR_ERROR);

    (FARPROC)pfnAcmDriverDetails = gpafnAcmFunctions[ACMTHUNK_DRIVERDETAILS];
    if (NULL == pfnAcmDriverDetails)
        return (MMSYSERR_ERROR);

    mmr = (* pfnAcmDriverDetails)(hadid, padd, fdwDetails);

    return (mmr);
}


//--------------------------------------------------------------------------;
//--------------------------------------------------------------------------;

MMRESULT ACMAPI acmDriverAdd
(
    LPHACMDRIVERID          phadid,
    HINSTANCE               hinstModule,
    LPARAM                  lParam, 
    DWORD                   dwPriority,
    DWORD                   fdwAdd
)
{
    MMRESULT (ACMAPI *pfnAcmDriverAdd)
    (
        LPHACMDRIVERID          phadid,
        HINSTANCE               hinstModule,
        LPARAM                  lParam, 
        DWORD                   dwPriority,
        DWORD                   fdwAdd
    );

    MMRESULT        mmr;

    if (NULL == gpafnAcmFunctions)
        return (MMSYSERR_ERROR);

    (FARPROC)pfnAcmDriverAdd = gpafnAcmFunctions[ACMTHUNK_DRIVERADD];
    if (NULL == pfnAcmDriverAdd)
        return (MMSYSERR_ERROR);

    mmr = (* pfnAcmDriverAdd)(phadid, hinstModule, lParam, dwPriority, fdwAdd);

    return (mmr);
}


//--------------------------------------------------------------------------;
//--------------------------------------------------------------------------;

MMRESULT ACMAPI acmDriverRemove
(
    HACMDRIVERID            hadid,
    DWORD                   fdwRemove
)
{
    MMRESULT (ACMAPI *pfnAcmDriverRemove)
    (
        HACMDRIVERID            hadid,
        DWORD                   fdwRemove
    );

    MMRESULT        mmr;

    if (NULL == gpafnAcmFunctions)
        return (MMSYSERR_ERROR);

    (FARPROC)pfnAcmDriverRemove = gpafnAcmFunctions[ACMTHUNK_DRIVERREMOVE];
    if (NULL == pfnAcmDriverRemove)
        return (MMSYSERR_ERROR);

    mmr = (* pfnAcmDriverRemove)(hadid, fdwRemove);

    return (mmr);
}


//--------------------------------------------------------------------------;
//--------------------------------------------------------------------------;

MMRESULT ACMAPI acmDriverOpen
(
    LPHACMDRIVER            phad, 
    HACMDRIVERID            hadid,
    DWORD                   fdwOpen
)
{
    MMRESULT (ACMAPI *pfnAcmDriverOpen)
    (
        LPHACMDRIVER            phad, 
        HACMDRIVERID            hadid,
        DWORD                   fdwOpen
    );

    MMRESULT        mmr;

    if (NULL == gpafnAcmFunctions)
        return (MMSYSERR_ERROR);

    (FARPROC)pfnAcmDriverOpen = gpafnAcmFunctions[ACMTHUNK_DRIVEROPEN];
    if (NULL == pfnAcmDriverOpen)
        return (MMSYSERR_ERROR);

    mmr = (* pfnAcmDriverOpen)(phad, hadid, fdwOpen);

    return (mmr);
}


//--------------------------------------------------------------------------;
//--------------------------------------------------------------------------;

MMRESULT ACMAPI acmDriverClose
(
    HACMDRIVER              had,
    DWORD                   fdwClose
)
{
    MMRESULT (ACMAPI *pfnAcmDriverClose)
    (
        HACMDRIVER              had,
        DWORD                   fdwClose
    );

    MMRESULT        mmr;

    if (NULL == gpafnAcmFunctions)
        return (MMSYSERR_ERROR);

    (FARPROC)pfnAcmDriverClose = gpafnAcmFunctions[ACMTHUNK_DRIVERCLOSE];
    if (NULL == pfnAcmDriverClose)
        return (MMSYSERR_ERROR);

    mmr = (* pfnAcmDriverClose)(had, fdwClose);

    return (mmr);
}


//--------------------------------------------------------------------------;
//--------------------------------------------------------------------------;

LRESULT ACMAPI acmDriverMessage
(
    HACMDRIVER              had,
    UINT                    uMsg, 
    LPARAM                  lParam1,
    LPARAM                  lParam2
)
{
    LRESULT (ACMAPI *pfnAcmDriverMessage)
    (
        HACMDRIVER              had,
        UINT                    uMsg, 
        LPARAM                  lParam1,
        LPARAM                  lParam2
    );

    LRESULT         lr;

    if (NULL == gpafnAcmFunctions)
        return (MMSYSERR_ERROR);

    (FARPROC)pfnAcmDriverMessage = gpafnAcmFunctions[ACMTHUNK_DRIVERMESSAGE];
    if (NULL == pfnAcmDriverMessage)
        return (MMSYSERR_ERROR);

    lr = (* pfnAcmDriverMessage)(had, uMsg, lParam1, lParam2);

    return (lr);
}


//--------------------------------------------------------------------------;
//--------------------------------------------------------------------------;

MMRESULT ACMAPI acmDriverID
(
    HACMOBJ                 hao,
    LPHACMDRIVERID          phadid,
    DWORD                   fdwDriverId
)
{
    MMRESULT (ACMAPI *pfnAcmDriverId)
    (
        HACMOBJ                 hao,
        LPHACMDRIVERID          phadid,
        DWORD                   fdwDriverId
    );

    MMRESULT        mmr;

    if (NULL == gpafnAcmFunctions)
        return (MMSYSERR_ERROR);

    (FARPROC)pfnAcmDriverId = gpafnAcmFunctions[ACMTHUNK_DRIVERID];
    if (NULL == pfnAcmDriverId)
        return (MMSYSERR_ERROR);

    mmr = (* pfnAcmDriverId)(hao, phadid, fdwDriverId);

    return (mmr);
}

//--------------------------------------------------------------------------;
//--------------------------------------------------------------------------;

MMRESULT ACMAPI acmDriverPriority
(
    HACMDRIVERID            hadid,
    DWORD                   dwPriority,
    DWORD                   fdwPriority
)
{
    MMRESULT (ACMAPI *pfnAcmDriverPriority)
    (
        HACMDRIVERID            hadid,
        DWORD                   dwPriority,
        DWORD                   fdwPriority
    );

    MMRESULT        mmr;

    if (NULL == gpafnAcmFunctions)
        return (MMSYSERR_ERROR);

    (FARPROC)pfnAcmDriverPriority = gpafnAcmFunctions[ACMTHUNK_DRIVERPRIORITY];
    if (NULL == pfnAcmDriverPriority)
        return (MMSYSERR_ERROR);

    mmr = (* pfnAcmDriverPriority)(hadid, dwPriority, fdwPriority);

    return (mmr);
}


//==========================================================================;
//
//  Format Tag Information API's
//
//
//==========================================================================;

//--------------------------------------------------------------------------;
//--------------------------------------------------------------------------;

MMRESULT ACMAPI acmFormatTagDetails
(
    HACMDRIVER              had,
    LPACMFORMATTAGDETAILS   paftd,
    DWORD                   fdwDetails
)
{
    MMRESULT (ACMAPI *pfnAcmFormatTagDetails)
    (
        HACMDRIVER              had,
        LPACMFORMATTAGDETAILS   paftd,
        DWORD                   fdwDetails
    );

    MMRESULT        mmr;

    if (NULL == gpafnAcmFunctions)
        return (MMSYSERR_ERROR);

    (FARPROC)pfnAcmFormatTagDetails = gpafnAcmFunctions[ACMTHUNK_FORMATTAGDETAILS];
    if (NULL == pfnAcmFormatTagDetails)
        return (MMSYSERR_ERROR);

    mmr = (* pfnAcmFormatTagDetails)(had, paftd, fdwDetails);

    return (mmr);
} // acmFormatTagDetails()


//--------------------------------------------------------------------------;
//--------------------------------------------------------------------------;

MMRESULT ACMAPI acmFormatTagEnum
(
    HACMDRIVER              had,
    LPACMFORMATTAGDETAILS   paftd,
    ACMFORMATTAGENUMCB      fnCallback,
    DWORD                   dwInstance, 
    DWORD                   fdwEnum
)
{
    MMRESULT (ACMAPI *pfnAcmFormatTagEnum)
    (
        HACMDRIVER              had,
        LPACMFORMATTAGDETAILS   paftd,
        ACMFORMATTAGENUMCB      fnCallback,
        DWORD                   dwInstance, 
        DWORD                   fdwEnum
    );

    MMRESULT        mmr;

    if (NULL == gpafnAcmFunctions)
        return (MMSYSERR_ERROR);

    (FARPROC)pfnAcmFormatTagEnum = gpafnAcmFunctions[ACMTHUNK_FORMATTAGENUM];
    if (NULL == pfnAcmFormatTagEnum)
        return (MMSYSERR_ERROR);

    mmr = (* pfnAcmFormatTagEnum)(had, paftd, fnCallback, dwInstance, fdwEnum);

    return (mmr);
} // acmFormatTagEnum()


//==========================================================================;
//
//  Format Information API's
//
//
//==========================================================================;

//--------------------------------------------------------------------------;
//--------------------------------------------------------------------------;

MMRESULT ACMAPI acmFormatChoose
(
    LPACMFORMATCHOOSE       pafmtc
)
{
    MMRESULT (ACMAPI *pfnAcmFormatChoose)
    (
        LPACMFORMATCHOOSE       pafmtc
    );

    MMRESULT        mmr;

    if (NULL == gpafnAcmFunctions)
        return (MMSYSERR_ERROR);

    (FARPROC)pfnAcmFormatChoose = gpafnAcmFunctions[ACMTHUNK_FORMATCHOOSE];
    if (NULL == pfnAcmFormatChoose)
        return (MMSYSERR_ERROR);

    mmr = (* pfnAcmFormatChoose)(pafmtc);

    return (mmr);
}


//--------------------------------------------------------------------------;
//--------------------------------------------------------------------------;

MMRESULT ACMAPI acmFormatDetails
(
    HACMDRIVER              had,
    LPACMFORMATDETAILS      pafd,
    DWORD                   fdwDetails
)
{
    MMRESULT (ACMAPI *pfnAcmFormatDetails)
    (
        HACMDRIVER              had,
        LPACMFORMATDETAILS      pafd,
        DWORD                   fdwDetails
    );

    MMRESULT        mmr;

    if (NULL == gpafnAcmFunctions)
        return (MMSYSERR_ERROR);

    (FARPROC)pfnAcmFormatDetails = gpafnAcmFunctions[ACMTHUNK_FORMATDETAILS];
    if (NULL == pfnAcmFormatDetails)
        return (MMSYSERR_ERROR);

    mmr = (* pfnAcmFormatDetails)(had, pafd, fdwDetails);

    return (mmr);
} // acmFormatDetails()


//--------------------------------------------------------------------------;
//--------------------------------------------------------------------------;

MMRESULT ACMAPI acmFormatEnum
(
    HACMDRIVER              had,
    LPACMFORMATDETAILS      pafd,
    ACMFORMATENUMCB         fnCallback,
    DWORD                   dwInstance, 
    DWORD                   fdwEnum
)
{
    MMRESULT (ACMAPI *pfnAcmFormatEnum)
    (
        HACMDRIVER              had,
        LPACMFORMATDETAILS      pafd,
        ACMFORMATENUMCB         fnCallback,
        DWORD                   dwInstance, 
        DWORD                   fdwEnum
    );

    MMRESULT        mmr;

    if (NULL == gpafnAcmFunctions)
        return (MMSYSERR_ERROR);

    (FARPROC)pfnAcmFormatEnum = gpafnAcmFunctions[ACMTHUNK_FORMATENUM];
    if (NULL == pfnAcmFormatEnum)
        return (MMSYSERR_ERROR);

    mmr = (* pfnAcmFormatEnum)(had, pafd, fnCallback, dwInstance, fdwEnum);

    return (mmr);
}


//--------------------------------------------------------------------------;
//--------------------------------------------------------------------------;

MMRESULT ACMAPI acmFormatSuggest
(
    HACMDRIVER          had,
    LPWAVEFORMATEX      pwfxSrc,
    LPWAVEFORMATEX      pwfxDst,
    DWORD               cbwfxDst,
    DWORD               fdwSuggest
)
{
    MMRESULT (ACMAPI *pfnAcmFormatSuggest)
    (
        HACMDRIVER          had,
        LPWAVEFORMATEX      pwfxSrc,
        LPWAVEFORMATEX      pwfxDst,
        DWORD               cbwfxDst,
        DWORD               fdwSuggest
    );

    MMRESULT        mmr;

    if (NULL == gpafnAcmFunctions)
        return (MMSYSERR_ERROR);

    (FARPROC)pfnAcmFormatSuggest = gpafnAcmFunctions[ACMTHUNK_FORMATSUGGEST];
    if (NULL == pfnAcmFormatSuggest)
        return (MMSYSERR_ERROR);

    mmr = (* pfnAcmFormatSuggest)(had, pwfxSrc, pwfxDst, cbwfxDst, fdwSuggest);

    return (mmr);
}


//==========================================================================;
//
//  Filter Tag Information API's
//
//
//==========================================================================;

//--------------------------------------------------------------------------;
//--------------------------------------------------------------------------;

MMRESULT ACMAPI acmFilterTagDetails
(
    HACMDRIVER              had,
    LPACMFILTERTAGDETAILS   paftd,
    DWORD                   fdwDetails
)
{
    MMRESULT (ACMAPI *pfnAcmFilterTagDetails)
    (
        HACMDRIVER              had,
        LPACMFILTERTAGDETAILS   paftd,
        DWORD                   fdwDetails
    );

    MMRESULT        mmr;

    if (NULL == gpafnAcmFunctions)
        return (MMSYSERR_ERROR);

    (FARPROC)pfnAcmFilterTagDetails = gpafnAcmFunctions[ACMTHUNK_FILTERTAGDETAILS];
    if (NULL == pfnAcmFilterTagDetails)
        return (MMSYSERR_ERROR);

    mmr = (* pfnAcmFilterTagDetails)(had, paftd, fdwDetails);

    return (mmr);
} // acmFilterTagDetails()


//--------------------------------------------------------------------------;
//--------------------------------------------------------------------------;

MMRESULT ACMAPI acmFilterTagEnum
(
    HACMDRIVER              had,
    LPACMFILTERTAGDETAILS   paftd,
    ACMFILTERTAGENUMCB      fnCallback,
    DWORD                   dwInstance, 
    DWORD                   fdwEnum
)
{
    MMRESULT (ACMAPI *pfnAcmFilterTagEnum)
    (
        HACMDRIVER              had,
        LPACMFILTERTAGDETAILS   paftd,
        ACMFILTERTAGENUMCB      fnCallback,
        DWORD                   dwInstance, 
        DWORD                   fdwEnum
    );

    MMRESULT        mmr;

    if (NULL == gpafnAcmFunctions)
        return (MMSYSERR_ERROR);

    (FARPROC)pfnAcmFilterTagEnum = gpafnAcmFunctions[ACMTHUNK_FILTERTAGENUM];
    if (NULL == pfnAcmFilterTagEnum)
        return (MMSYSERR_ERROR);

    mmr = (* pfnAcmFilterTagEnum)(had, paftd, fnCallback, dwInstance, fdwEnum);

    return (mmr);
} // acmFilterTagEnum()


//==========================================================================;
//
//  Filter Information API's
//
//
//==========================================================================;

//--------------------------------------------------------------------------;
//--------------------------------------------------------------------------;

MMRESULT ACMAPI acmFilterChoose
(
    LPACMFILTERCHOOSE       pafltrc
)
{
    MMRESULT (ACMAPI *pfnAcmFilterChoose)
    (
        LPACMFILTERCHOOSE       pafltrc
    );

    MMRESULT        mmr;

    if (NULL == gpafnAcmFunctions)
        return (MMSYSERR_ERROR);

    (FARPROC)pfnAcmFilterChoose = gpafnAcmFunctions[ACMTHUNK_FILTERCHOOSE];
    if (NULL == pfnAcmFilterChoose)
        return (MMSYSERR_ERROR);

    mmr = (* pfnAcmFilterChoose)(pafltrc);

    return (mmr);
}


//--------------------------------------------------------------------------;
//--------------------------------------------------------------------------;

MMRESULT ACMAPI acmFilterDetails
(
    HACMDRIVER              had,
    LPACMFILTERDETAILS      pafd,
    DWORD                   fdwDetails
)
{
    MMRESULT (ACMAPI *pfnAcmFilterDetails)
    (
        HACMDRIVER              had,
        LPACMFILTERDETAILS      pafd,
        DWORD                   fdwDetails
    );

    MMRESULT        mmr;

    if (NULL == gpafnAcmFunctions)
        return (MMSYSERR_ERROR);

    (FARPROC)pfnAcmFilterDetails = gpafnAcmFunctions[ACMTHUNK_FILTERDETAILS];
    if (NULL == pfnAcmFilterDetails)
        return (MMSYSERR_ERROR);

    mmr = (* pfnAcmFilterDetails)(had, pafd, fdwDetails);

    return (mmr);
} // acmFilterDetails()


//--------------------------------------------------------------------------;
//--------------------------------------------------------------------------;

MMRESULT ACMAPI acmFilterEnum
(
    HACMDRIVER              had,
    LPACMFILTERDETAILS      pafd,
    ACMFILTERENUMCB         fnCallback,
    DWORD                   dwInstance, 
    DWORD                   fdwEnum
)
{
    MMRESULT (ACMAPI *pfnAcmFilterEnum)
    (
        HACMDRIVER              had,
        LPACMFILTERDETAILS      pafd,
        ACMFILTERENUMCB         fnCallback,
        DWORD                   dwInstance, 
        DWORD                   fdwEnum
    );

    MMRESULT        mmr;

    if (NULL == gpafnAcmFunctions)
        return (MMSYSERR_ERROR);

    (FARPROC)pfnAcmFilterEnum = gpafnAcmFunctions[ACMTHUNK_FILTERENUM];
    if (NULL == pfnAcmFilterEnum)
        return (MMSYSERR_ERROR);

    mmr = (* pfnAcmFilterEnum)(had, pafd, fnCallback, dwInstance, fdwEnum);

    return (mmr);
}


//==========================================================================;
//
//  ACM Stream Management API's
//
//
//==========================================================================;

//--------------------------------------------------------------------------;
//--------------------------------------------------------------------------;

MMRESULT ACMAPI acmStreamOpen
(
    LPHACMSTREAM            phas,
    HACMDRIVER              had,
    LPWAVEFORMATEX          pwfxSrc,
    LPWAVEFORMATEX          pwfxDst,
    LPWAVEFILTER            pwfltr,
    DWORD                   dwCallback,
    DWORD                   dwInstance,
    DWORD                   fdwOpen
)
{
    MMRESULT (ACMAPI *pfnAcmStreamOpen)
    (
        LPHACMSTREAM            phas,
        HACMDRIVER              had,
        LPWAVEFORMATEX          pwfxSrc,
        LPWAVEFORMATEX          pwfxDst,
        LPWAVEFILTER            pwfltr,
        DWORD                   dwCallback,
        DWORD                   dwInstance,
        DWORD                   fdwOpen
    );

    MMRESULT        mmr;

    if (NULL == gpafnAcmFunctions)
        return (MMSYSERR_ERROR);

    (FARPROC)pfnAcmStreamOpen = gpafnAcmFunctions[ACMTHUNK_STREAMOPEN];
    if (NULL == pfnAcmStreamOpen)
        return (MMSYSERR_ERROR);

    mmr = (* pfnAcmStreamOpen)(phas, had, pwfxSrc, pwfxDst, pwfltr, dwCallback, dwInstance, fdwOpen);

    return (mmr);
}


//--------------------------------------------------------------------------;
//--------------------------------------------------------------------------;

MMRESULT ACMAPI acmStreamClose
(
    HACMSTREAM              has,
    DWORD                   fdwClose
)
{
    MMRESULT (ACMAPI *pfnAcmStreamClose)
    (
        HACMSTREAM              has,
        DWORD                   fdwClose
    );

    MMRESULT        mmr;

    if (NULL == gpafnAcmFunctions)
        return (MMSYSERR_ERROR);

    (FARPROC)pfnAcmStreamClose = gpafnAcmFunctions[ACMTHUNK_STREAMCLOSE];
    if (NULL == pfnAcmStreamClose)
        return (MMSYSERR_ERROR);

    mmr = (* pfnAcmStreamClose)(has, fdwClose);

    return (mmr);
}


//--------------------------------------------------------------------------;
//--------------------------------------------------------------------------;

MMRESULT ACMAPI acmStreamSize
(
    HACMSTREAM              has,
    DWORD                   cbInput,
    LPDWORD                 pdwOutputBytes,
    DWORD                   fdwSize
)
{
    MMRESULT (ACMAPI *pfnAcmStreamSize)
    (
        HACMSTREAM              has,
        DWORD                   cbInput,
        LPDWORD                 pdwOutputBytes,
        DWORD                   fdwSize
    );

    MMRESULT        mmr;

    if (NULL == gpafnAcmFunctions)
        return (MMSYSERR_ERROR);

    (FARPROC)pfnAcmStreamSize = gpafnAcmFunctions[ACMTHUNK_STREAMSIZE];
    if (NULL == pfnAcmStreamSize)
        return (MMSYSERR_ERROR);

    mmr = (* pfnAcmStreamSize)(has, cbInput, pdwOutputBytes, fdwSize);

    return (mmr);
}


//--------------------------------------------------------------------------;
//--------------------------------------------------------------------------;

MMRESULT ACMAPI acmStreamConvert
(
    HACMSTREAM              has, 
    LPACMSTREAMHEADER       pash,
    DWORD                   fdwConvert
)
{
    MMRESULT (ACMAPI *pfnAcmStreamConvert)
    (
        HACMSTREAM              has, 
        LPACMSTREAMHEADER       pash,
        DWORD                   fdwConvert
    );

    MMRESULT        mmr;

    if (NULL == gpafnAcmFunctions)
        return (MMSYSERR_ERROR);

    (FARPROC)pfnAcmStreamConvert = gpafnAcmFunctions[ACMTHUNK_STREAMCONVERT];
    if (NULL == pfnAcmStreamConvert)
        return (MMSYSERR_ERROR);

    mmr = (* pfnAcmStreamConvert)(has, pash, fdwConvert);

    return (mmr);
}


//--------------------------------------------------------------------------;
//--------------------------------------------------------------------------;

MMRESULT ACMAPI acmStreamReset
(
    HACMSTREAM              has, 
    DWORD                   fdwReset
)
{
    MMRESULT (ACMAPI *pfnAcmStreamReset)
    (
        HACMSTREAM              has, 
        DWORD                   fdwReset
    );

    MMRESULT        mmr;

    if (NULL == gpafnAcmFunctions)
        return (MMSYSERR_ERROR);

    (FARPROC)pfnAcmStreamReset = gpafnAcmFunctions[ACMTHUNK_STREAMRESET];
    if (NULL == pfnAcmStreamReset)
        return (MMSYSERR_ERROR);

    mmr = (* pfnAcmStreamReset)(has, fdwReset);

    return (mmr);
}


//--------------------------------------------------------------------------;
//--------------------------------------------------------------------------;

MMRESULT ACMAPI acmStreamPrepareHeader
(
    HACMSTREAM              has, 
    LPACMSTREAMHEADER       pash,
    DWORD                   fdwPrepare
)
{
    MMRESULT (ACMAPI *pfnAcmStreamPrepareHeader)
    (
        HACMSTREAM              has, 
        LPACMSTREAMHEADER       pash,
        DWORD                   fdwPrepare
    );

    MMRESULT        mmr;

    if (NULL == gpafnAcmFunctions)
        return (MMSYSERR_ERROR);

    (FARPROC)pfnAcmStreamPrepareHeader = gpafnAcmFunctions[ACMTHUNK_STREAMPREPAREHEADER];
    if (NULL == pfnAcmStreamPrepareHeader)
        return (MMSYSERR_ERROR);

    mmr = (* pfnAcmStreamPrepareHeader)(has, pash, fdwPrepare);

    return (mmr);
}


//--------------------------------------------------------------------------;
//--------------------------------------------------------------------------;

MMRESULT ACMAPI acmStreamUnprepareHeader
(
    HACMSTREAM              has, 
    LPACMSTREAMHEADER       pash,
    DWORD                   fdwUnprepare
)
{
    MMRESULT (ACMAPI *pfnAcmStreamUnprepareHeader)
    (
        HACMSTREAM              has, 
        LPACMSTREAMHEADER       pash,
        DWORD                   fdwUnprepare
    );

    MMRESULT        mmr;

    if (NULL == gpafnAcmFunctions)
        return (MMSYSERR_ERROR);

    (FARPROC)pfnAcmStreamUnprepareHeader = gpafnAcmFunctions[ACMTHUNK_STREAMUNPREPAREHEADER];
    if (NULL == pfnAcmStreamUnprepareHeader)
        return (MMSYSERR_ERROR);

    mmr = (* pfnAcmStreamUnprepareHeader)(has, pash, fdwUnprepare);

    return (mmr);
}
