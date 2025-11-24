//---------------------------------------------------------------------------
//
//  Module:   avvxp500.h
//
//  Description:
//     Header file for AVVXP500
//
//---------------------------------------------------------------------------
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1994 - 1995 Microsoft Corporation.	All Rights Reserved.
//
//---------------------------------------------------------------------------

#define AVVXP500_Device_ID						0x4321
#define AVVXP500_VERSION_RQD              0x0100

// VxD PM API interface...

#define AVVXP500_API_Get_Version          0x0000
#define AVVXP500_API_Get_Info             0x0001
#define AVVXP500_API_GetInfoF_DevNode     0x0001

#define AVVXP500_API_Acquire              0x0002
#define AVVXP500_API_Release              0x0003

typedef struct tagHARDWAREINSTANCE
{
   //
   // Supporting VxD entry point
   //

   LPVOID      pVxDEntry ;
   DWORD       cAcquire ;

   // hardware instance information from VxD

   WORD        wHardwareOptions ;            // hardware options
   WORD        wIOAddressVXP500 ;            // base I/O
   BYTE        bIRQ ;                        // IRQ
   BYTE        bReserved ;                   // alignment
   WORD        wSelector ;                   // frame buffer selector
   DWORD       dwMemBase ;                   // frame buffer phys addr
   DWORD       dn ;                          // DevNode

   //
   // other support data
   //

   WORD        cReference ;                  // reference count
   BOOL        fEnabled ;                    // TRUE if instance enabled

   BYTE        bIntVector ;
   BYTE        bOrigIntMask ;
   LPVOID      lpOldISR ;
   WORD        wEOICommands ;

   // link to next instance

   struct tagHARDWAREINSTANCE NEAR *pNext ;

} HARDWAREINSTANCE, NEAR *PHARDWAREINSTANCE ;

typedef struct tagVXP500INFO
{
   DWORD   dwSize ;
   WORD    wHardwareOptions ;
   WORD    wIOAddressVXP500 ;
   BYTE    bIRQ ;
   BYTE    bReserved ;
   WORD    wVersionVxD ;
   WORD    wFlags ;
   WORD    wSelector ;
   DWORD   dwMemBase ;
                                   
   DWORD   dn ;
   DWORD   dwIRQHandle ;
   DWORD   dwVXP500OwnerCur ;
   DWORD   dwVXP500OwnerLast ;
   DWORD   hVXP500Stubs ;

} AVVXP500INFO, FAR *LPAVVXP500INFO ;

//
// globals
//

// in init.c

#ifdef DEBUG                       
extern WORD         wDebugLevel;    // debug level
#endif
extern HANDLE       ghModule;       // our module handle

//
// prototypes
//

// assemu.asm:

BOOL NEAR PASCAL Create_ISR( PHARDWAREINSTANCE ) ;
BOOL NEAR PASCAL SetInterruptMask( BYTE bIRQ, BYTE fMask ) ;
LPVOID NEAR PASCAL SetInterruptVector( BYTE bIRQ, LPVOID lpNewISR ) ;

// drvproc.c:

extern LRESULT FAR PASCAL _loadds DriverProc
(
    DWORD           dwDriverID,
    HDRVR           hDriver,
    UINT            uiMessage,
    LPARAM          lParam1,
    LPARAM          lParam2
) ;

// vxdiface.c:

BOOL NEAR PASCAL GetVxDInfo
(
    LPVOID              pVxDEntry,
    LPDEVICE_INIT       lpDI
) ;

LPVOID NEAR PASCAL GetVxDEntry
(
    UINT            uVxDId
) ;

WORD NEAR PASCAL GetVxDVersion
(
    LPVOID          pVxDEntry
) ;

BOOL FAR PASCAL AcquireVXP500
(
    LPVOID              pVxDEntry,
    LPDEVICE_INIT       lpDI
) ;

BOOL FAR PASCAL ReleaseVXP500
(
    LPVOID              pVxDEntry,
    LPDEVICE_INIT       lpDI
) ;

//	
//  Product Description strings.  For this driver, all descriptions are
//  the same..
//
#define IDS_VXP500PRODUCT   16

//
// Debug output
//       

#ifdef DEBUG
    extern char STR_PROLOGUE[];
    extern char STR_CRLF[];
    extern char STR_SPACE[];
    extern WORD  wDebugLevel;     // debug level
    #define D( x )          { x }
    #define DPF( x, y ) if (x <= wDebugLevel) (OutputDebugStr(STR_PROLOGUE),OutputDebugStr(y),OutputDebugStr(STR_CRLF))
#else
    #define D( x )
    #define DPF( x, y )
#endif

#define QUOTE(x) #x
#define QQUOTE(y) QUOTE(y)
#define REMIND(str) __FILE__ "(" QQUOTE(__LINE__) ") : " str

//
// Assertion macros
//

#ifdef DEBUG
    extern void FAR PASCAL AssertBreak( void );
    #define AssertF(exp) \
        if ((exp) == 0)  \
        { \
            DPF( 1, "AssertF failed (" #exp ")" ); \
            AssertBreak(); \
        }

    #define AssertT(exp) \
        if ((exp) != 0)  \
        { \
            DPF( 1, "AssertT failed (" #exp ")" ); \
            AssertBreak(); \
        }
#else
    #define AssertBreak()
    #define AssertF(exp) 
    #define AssertT(exp) 
#endif

//---------------------------------------------------------------------------
//  End of File: avvxp500.h
//---------------------------------------------------------------------------
