//---------------------------------------------------------------------------
//
//  Module:   cnfgmgr.h
//
//  Description:
//     Header file for cnfgmgr.c
//
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//---------------------------------------------------------------------------
//
//  Copyright (c) 1994 - 1995   Microsoft Corporation.  All Rights Reserved.
//
//---------------------------------------------------------------------------

#define REGSTR_SUBKEY_CONFIG                "Config"
#define REGSTR_VAL_SINGLEMODEDMA            "Single Mode DMA"
#define REGSTR_VAL_ACCEPTCLOSERATES      "Accept Close Sampling Rates"
#define REGSTR_VAL_SBEMULATION       "Sound Blaster Emulation"

#define DEFAULT_SINGLEMODEDMA           FALSE
#define DEFAULT_ACCEPTCLOSERATES        FALSE
#define DEFAULT_DMABUFFERSIZE           24
#define DEFAULT_SBEMULATION             TRUE

#define SSI_FLAG_DISABLEWARNING          0x00000001
#define SSI_FLAG_SINGLEMODEDMA           0x00000004
#define SSI_FLAG_IRQSHARING              0x00000008

#define SSI_FLAG_BUSTYPE_ISAPNP          0x00000100
#define SSI_FLAG_BUSTYPE_PCMCIA          0x00000200
#define SSI_FLAG_BUSTYPE_PNPBIOS         0x00000400

#define SSI_FLAG_HWSB                    0x00001000

#define PCMCIA_ID_WAVJAMMER_REV_E        0x0001     
#define PCMCIA_ID_WAVJAMMER_REV_F        0x0002     
#define PCMCIA_ID_WAVJAMMER_REV_OTHER    0x0003

#define PCMCIA_ID_SOUNDSCSI              0x0100     

#define SBVIRT_FLAG_BUSTYPE_PCMCIA       0x00000001
#define SBVIRT_FLAG_VIRT_DMA             0x00000010
#define SBVIRT_FLAG_VIRT_IRQ             0x00000020

#define QUOTE(x) #x
#define QQUOTE(y) QUOTE(y)
#define REMIND(str) __FILE__ "(" QQUOTE(__LINE__) ") : " str


#if defined DEBUG_RETAIL || defined DEBUG
#define DBG_TRACE(strings) {\
        _Debug_Printf_Service( "MSSNDSYS: "); \
        _Debug_Printf_Service##strings; \
        _Debug_Printf_Service("\n");}
#ifndef DEBUG_RETAIL
#define DBG_ERROR(strings) {\
        _Debug_Printf_Service( "MSSNDSYS ERROR: "); \
        _Debug_Printf_Service##strings; \
        _Debug_Printf_Service("\n");\
        {_asm   int     3}}
#else
#define DBG_ERROR(strings) {\
        _Debug_Printf_Service( "MSSNDSYS ERROR: "); \
        _Debug_Printf_Service##strings; \
        _Debug_Printf_Service("\n");}
#endif
#else
   #define DBG_TRACE(strings)
   #define DBG_ERROR(strings)
#endif

#ifdef DEBUG
#define DPF( strings ) _Debug_Printf_Service##strings;
#else
#define DPF( strings )
#endif


#define DEVIDSTR_WSSSB "VIRTUAL\\WSS-SBEmulation"

//
// local typedefs
//

typedef struct tagIO_RES
{
   IO_DES    IO_Header ;
   IO_RANGE  IO_Data ;

} IO_RES, *PIO_RES ;

// external function prototypes

#ifdef AZTECH
CONFIGRET AZT_Init_Config
(
    WORD            wBaseSB,
    WORD            wBaseAutoSel,
    WORD            wSBIRQ,
    WORD            wSBDMA
) ;
#endif

CONFIGRET MSSNDSYS_Set_Config
(
    DEVNODE         dn,
    WORD            wBaseCODEC,
    WORD            wBaseAutoSel,
    WORD            wBaseAGA,
    WORD            wBaseOPL3,
    WORD            wBaseSB,
    WORD            wIRQ,
    WORD            wPlaybackDMA,
    WORD            wCaptureDMA,
    DWORD           dwBufferSize,
    DWORD           fdwOptions
) ;

VOID MSSNDSYS_Remove_Config
(
    DEVNODE         dn
) ;

BOOL Is_AGA_Valid
(
    WORD            wBaseCODEC,
    WORD            wBaseAGA,
    WORD            wOptions
) ;

BOOL MSSNDSYS_IsOwned
(
    DEVNODE         dn
) ;

BOOL MSSNDSYS_WantSB
(
    DEVNODE         dn,
    PDWORD          pdwFlags
) ;

BOOL MSSNDSYS_Get_DevNode_Flags
(
    DEVNODE         dn,
    PDWORD          pdwFlags
) ;

VOID MSSNDSYS_Suspend
(
    DEVNODE         dn
) ;

VOID MSSNDSYS_Resume
(
    DEVNODE         dn
) ;

#ifdef MSSNDSYS
CONFIGRET SBVirt_Set_Config
(
    DEVNODE         dn,
    WORD            wBaseSB,
    PWORD           pwDMA,
    PWORD           pwIRQ
) ;

VOID SBVirt_Remove_Config
(
    DEVNODE         dn
) ;

BOOL SBVirt_IsOwned
(
    DEVNODE         dn
) ;
#endif

//
// local function prototypes
//

CONFIGRET MSSNDSYS_Config_Handler
(
    CONFIGFUNC      cf,
    SUBCONFIGFUNC   scf,
    DEVNODE         dn,
    DWORD           dwRefData,
    ULONG           ulFlags
) ;

CONFIGRET MSSNDSYS_Enumerator
(
    CONFIGFUNC      cf,
    SUBCONFIGFUNC   scf,
    DEVNODE         dnBus,
    DEVNODE         dnAbout,
    ULONG           ulFlags
)  ;

//
// wrappers for PCMCIA services...
//

WORD VXDINLINE PCCARD_Get_Configuration_Info( GET_CONFIG_INFO_PKT *pcip )
{
   _asm push   ebx
   _asm push   ecx
   _asm push   esi
   _asm mov    eax, F_GET_CONFIGURATION_INFO
   _asm mov    ebx, pcip
   _asm mov    ecx, size GET_CONFIG_INFO_PKT 
   _asm xor    edx, edx
   _asm xor    esi, esi
   VxDCall( PCCARD_Card_Services )
   _asm pop    esi
   _asm pop    ecx
   _asm pop    ebx
}

#define CIP_PRODUCTID_WAVJAMMER 0x05
#define CIP_PRODUCTID_SOUNDSCSI 0x0B

#define CIP_WAVJAMMER_REV_E     0xE000
#define CIP_WAVJAMMER_REV_F     0xF000

//
// inline stuff...
//

DWORD VXDINLINE Open_Boot_Log( VOID )
{
    VMMCall( Open_Boot_Log )
}

VOID VXDINLINE Write_Boot_Log( PCHAR pStr, DWORD cbSize )
{
   _asm push ecx
   _asm mov edx, pStr
   _asm mov ecx, cbSize
   VMMCall( Write_Boot_Log )
   _asm pop ecx
}

VOID VXDINLINE Close_Boot_Log( VOID )
{
    VMMCall( Close_Boot_Log )
}

//---------------------------------------------------------------------------
//  End of File: cnfgmgr.h
//---------------------------------------------------------------------------
