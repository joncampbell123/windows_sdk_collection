//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
//
//-------------------------------------------------------------------------;
//
//  codec.h
//
//  Description:
//
//
//
//==========================================================================;


//
//  CODEC version:
//
//  the version is a 32 bit number that is broken into three parts as 
//  follows:
//
//      bits 24 - 31:   8 bit _major_ version number
//      bits 16 - 23:   8 bit _minor_ version number
//      bits  0 - 15:   16 bit build number
//
//  this is then displayed as follows:
//
//      bMajor = (BYTE)(dwVersion >> 24)
//      bMinor = (BYTE)(dwVersion >> 16) & 
//      wBuild = LOWORD(dwVersion)
//
#define VERSION_CODEC_MAJOR     2
#define VERSION_CODEC_MINOR     1
#define VERSION_CODEC_BUILD     0

#define VERSION_CODEC       MAKE_ACM_VERSION(VERSION_CODEC_MAJOR,   \
                                             VERSION_CODEC_MINOR,   \
                                             VERSION_CODEC_BUILD)

//
//  version of the ACM that this codec was designed for (requires)..
//
#define VERSION_MSACM       MAKE_ACM_VERSION(2, 1, 0)



//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ; 
// 
//  Win 16/32 portability stuff...
// 
// 
// 
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ; 

#ifndef WIN32
    #ifndef FNLOCAL
        #define FNLOCAL     NEAR PASCAL
        #define FNGLOBAL    FAR PASCAL
        #define FNEXPORT    FAR PASCAL _loadds
    #endif

    //
    //
    //
    //
    #ifndef FIELD_OFFSET
    #define FIELD_OFFSET(type, field)    ((LONG)&(((type *)0)->field))
    #endif

    //
    //  based code makes since only in win 16 (to try and keep stuff out of
    //  our fixed data segment...
    //
    #define BCODE           _based(_segname("_CODE"))

    //
    //  stuff for unicode in Win 32--make it a noop in Win 16
    //
    #ifndef TCHAR
        #define TCHAR       char
    #endif
    #ifndef LPTSTR
        #define LPTSTR      LPSTR
    #endif
    #define TEXT(a)         a
    #define SIZEOF(x)       sizeof(x)
#else
    #ifndef FNLOCAL
        #define FNLOCAL     _stdcall
        #define FNGLOBAL    _stdcall
        #define FNEXPORT    _stdcall
    #endif

    //
    //  we compile with UNICODE--we are a mm driver...
    //
    #ifdef UNICODE
        #define SIZEOF(x)   (sizeof(x)/sizeof(WCHAR))
    #else
        #define SIZEOF(x)   sizeof(x)
    #endif

    #define BCODE

    //
    //  win32 doesn't have to worry about 'huge' data
    //
    void *memcpy(void *dest, void *src, size_t count);
    #define hmemcpy     memcpy
#endif


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ; 
//
//  misc defines for misc sizes and things...
//
//
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ; 

//
//  bilingual. this allows the same identifier to be used in resource files
//  and code without having to decorate the id in your code.
//
#ifdef RC_INVOKED
    #define RCID(id)    id
#else
    #define RCID(id)    MAKEINTRESOURCE(id)
#endif


//
//  macros to compute block alignment and convert between samples and bytes
//  of PCM data. note that these macros assume:
//
//      wBitsPerSample  =  8 or 16
//      nChannels       =  1 or 2
//
//  the pwf argument is a pointer to a WAVEFORMATEX structure.
//
#define PCM_BLOCKALIGNMENT(pwfx)        (UINT)(((pwfx)->wBitsPerSample >> 3) << ((pwfx)->nChannels >> 1))
#define PCM_AVGBYTESPERSEC(pwfx)        (DWORD)((pwfx)->nSamplesPerSec * (pwfx)->nBlockAlign)
#define PCM_BYTESTOSAMPLES(pwfx, cb)    (DWORD)(cb / PCM_BLOCKALIGNMENT(pwfx))
#define PCM_SAMPLESTOBYTES(pwfx, dw)    (DWORD)(dw * PCM_BLOCKALIGNMENT(pwfx))


//
//  we use this dwId to determine when we where opened as an audio codec
//  by the ACM or from the control panel, etc. see codecConverterOpen for more
//  information.
//
#define BOGUS_DRIVER_ID     1L


//
//
//
//
typedef struct tCODECINST
{
    //
    //  although not required, it is suggested that the first two members
    //  of this structure remain as fccType and DriverProc _in this order_.
    //  the reason for this is that the codec will be easier to combine
    //  with other types of codecs (defined by AVI) in the future.
    //
    FOURCC          fccType;        // type of codec: 'audc'
    DRIVERPROC      DriverProc;     // driver proc for the instance

    //
    //  the remaining members of this structure are entirely open to what
    //  your codec requires.
    //
    HDRVR           hdrvr;          // driver handle we were opened with
    DWORD           vdwACM;         // current version of ACM opening you
    DWORD           dwFlags;        // flags from open description

} CODECINST, *PCODECINST, FAR *LPCODECINST;



//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ; 
//
//  typedefs
//
//
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ; 

typedef DWORD (FNGLOBAL *CONVERTPROC)(LPWAVEFORMATEX, LPBYTE, LPWAVEFORMATEX, LPBYTE, DWORD);

typedef LRESULT (FNGLOBAL *NEWCONVERTPROC)(LPACMDRVSTREAMHEADER, DWORD);




//
//  resource id's
//
//
#define ICON_CODEC                  RCID(10)

#define IDS_CODEC_SHORTNAME         (1)     // ACMDRIVERDETAILS.szShortName
#define IDS_CODEC_LONGNAME          (2)     // ACMDRIVERDETAILS.szLongName
#define IDS_CODEC_COPYRIGHT         (3)     // ACMDRIVERDETAILS.szCopyright
#define IDS_CODEC_LICENSING         (4)     // ACMDRIVERDETAILS.szLicensing
#define IDS_CODEC_FEATURES          (5)     // ACMDRIVERDETAILS.szFeatures

#define IDS_CODEC_NAME              (10)


//
//  global variables, etc...
//
extern HINSTANCE    ghinst;         // DLL instance handle (module in Win 16)
extern UINT         guCodecFlags;   // flags for various things
