/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

/* INITUTIL.c: 

   This file contains the initialization code for the virtual pen driver.
   This code is called from the control dispatch routine. It is called once
   and is then discarded. This file also contains all the initialization
   strings that the virtual driver uses.<nl>

   Routines contained in module:<nl>
      <l cVpenD_Device_Load.cVpenD_Device_Load><nl>
*/

#define WANTVXDWRAPS

#include "defines.h"
#include <basedef.h>
#include <VMM.H>
#include <vxdwraps.h>
#include <vmmreg.h>
#include <vpicd.h>
#include "pendrv.h"
#include "protos.h"

extern DWORD _VpenD_GlobalizePort(DWORD);
extern DWORD ddMiscFlags;

extern P_HARDWAREINFO pVMHwInfo;
extern LPDRV_PENINFO pVMPenInfo;
extern PCOMMANDS gpCommandStrs;
extern PRATETABLE gpRateTable;
extern P_VpenD_Register ActiveDevice;
extern P_VpenD_Register NullDevice;
extern void VpenD_Hw_Int(void);
extern void VpenD_EOI(void);

#ifdef DEBUG
extern char *gpMsgStr;
#endif

//initutil.c
char gpVpenDKey[]="System\\CurrentControlSet\\Services\\VxD\\VPEND";
char gpBootStatus[]="BootStatus";
char gpModelStr[]="Model";

char gpPenInfo[]="PenInfo";
char gpHardwareInfo[]="HardwareInfo";
char gpCommandSet[]="CommandSet";
char gpRateCommands[]="RateCommands";
char gpRateValues[]="RateValues";

char gpForce[]="Force";
extern DWORD ddForce;
char gpInductive[]="Inductive";
extern DWORD ddInductive;
PVOID gpGlobalMemory = 0;

//#define BUILD_REGISTRY

#ifdef BUILD_REGISTRY
#include "tmp.h"
#endif

// prototypes
DWORD ReadRegStruct(VMMHKEY hkModel,PVOID pDataDest,PCHAR pRegEntry,
          DWORD dwSize);
DWORD lstrcpy(PCHAR pDest,PCHAR pSource);

DWORD ParseRateTable(VMMHKEY hkModel,PRATETABLE gpRateTable,
          PCHAR pRateCommands,PCHAR pRateValues);
DWORD ParseCommands(VMMHKEY hkModel,PCOMMANDS gpCommandStrs,
         PCHAR pCommandSet);
DWORD ReadFromRegistry(VMMHKEY hkVpenD);
void AddExtraSettings(void);
DWORD ClaimResources(void);

#ifdef DEBUG
void DisplayInformation(void);
#endif

#define LOADING_SUCCESS            0x00000001
#define LOADING_NO_MODEL_ENTRY     0x00000002
#define LOADING_NO_MODEL_VALUE     0x00000004
#define LOADING_NO_MEMORY          0x00000008
#define LOADING_NO_PENINFO         0x00000010
#define LOADING_NO_HARDWAREINFO    0x00000020
#define LOADING_NO_RATETABLE       0x00000040
#define LOADING_NO_COMMANDSET      0x00000080
#define LOADING_NO_GLOBALIZE_PORT  0x00000100
#define LOADING_NO_HW_PROC         0x00000200
#define LOADING_NO_EOI_PROC        0x00000400
#define LOADING_NO_IRQ_HANDLE      0x00000800
#define LOADING_NO_ACQUIRE_PORT    0x00001000

#define MODEL_SIZE 256

#pragma VxD_LOCKED_DATA_SEG

// variables get locked into memory...
// #pragma VxD_LOCKED_DATA_SEG

VID VpenD_IRQ_Desc = {0,
        VPICD_OPT_READ_HW_IRR | VPICD_OPT_CAN_SHARE,
        (DWORD)&VpenD_Hw_Int,
        0,
        (DWORD)&VpenD_EOI,
        0,0,500,0};

/*
   Read the registry for
   HKEY_LOCAL_MACHINE\System\CurrentControlSet\Services\VxD\VPEND
   and look for the "Model" string.  This string will be a key to the type of
   hardware the virtual driver will be interacting with.  For instance, "Wacom\SD-510C".  
   Under this location read the _HARDWARE structure and the PENINFO structure.

   Regardless of the outcome, the virtual driver should update the
   "BootStatus" entry with the appropriate information for the pen driver.
   For example, "Loaded OK", "Registry Error", "Unknown Model", etc.

   Need to modify this routine so that it loads different elements from the
   registry in modular components.  For instance, modify this code so it can be called to
   load the hardware info structure only, or called to load the peninfo structure only, or 
   called to load other values from the registry.

   For instance,

   Allocate memory and assign memory to global variables : INIT code

   Update from registry:
      Open registry all the way up to VPEND section
      Read user preferences
      Open registry all the way up to the hardware information section
      Read HardwareInfo
      Read PenInfo
      Read all other
      Close registry - Hardware section
      Close registry - VPEND section

   Handle rotation
   Take resources, IRQ, ports
   Display information
*/

#pragma VxD_LOCKED_CODE_SEG


void cVpenD_Device_Load(void)
{
   DWORD dwMemSize;
   VMMHKEY hkResult;

#ifdef DEBUG
   gpMsgStr="VPEND Load";
#endif

   DBG_TRACE("------------------------------------------------------");
   DBG_TRACE("Attempting to load the version 2.0 virtual pen driver.");

   // This variable is used to indicate successful loading.
   ddMiscFlags=LOADING_SUCCESS;

   // Determine how much memory to allocate and allocate it.
   dwMemSize=((sizeof(_VpenD_Register)*2) +
      sizeof(_HARDWAREINFO) +
      sizeof(DRV_PENINFO) +
      (sizeof(COMMANDS)*API_NUMBER) +
      (sizeof(RATETABLE)*12) );

   // gpGlobalMemory points to a block of memory.
   if( (gpGlobalMemory=_HeapAllocate(dwMemSize,HEAPZEROINIT)) != 0)
   {
      //Have the memory for the structures, so set up pointers.
      pVMHwInfo=(P_HARDWAREINFO)gpGlobalMemory;
      pVMPenInfo=(LPDRV_PENINFO)((DWORD)pVMHwInfo+sizeof(_HARDWAREINFO));
      gpRateTable=(PRATETABLE)((DWORD)pVMPenInfo+sizeof(DRV_PENINFO));
      gpCommandStrs=(PCOMMANDS)((DWORD)gpRateTable+(sizeof(RATETABLE)*12));
      ActiveDevice=(P_VpenD_Register)((DWORD)gpCommandStrs+sizeof(COMMANDS)*API_NUMBER);
      NullDevice=(P_VpenD_Register)((DWORD)ActiveDevice+sizeof(_VpenD_Register));
   }
   else
   {
      ddMiscFlags|=LOADING_NO_MEMORY;
   }
#ifdef DEBUG

   DBG_MISC_HEX((DWORD)ActiveDevice,"The address of ActiveDevice");

   ActiveDevice->pDataType=0xFFFFFFFF;
   ActiveDevice->piNumberOfPenPackets=0x11111111;
   ActiveDevice->piThis_Offset=0x22222222;
   ActiveDevice->piThis=0x33333333;
   ActiveDevice->pPenPacketBuffer=(LPDRV_PENPACKET)0x44444444;
   ActiveDevice->pPenInfo=(LPDRV_PENINFO)0x55555555;
   ActiveDevice->pOEMBuffer=0x66666666;
   ActiveDevice->pfnEntryPoint=0x77777777;

#endif

   // check for loading
   if(_RegOpenKey(HKEY_LOCAL_MACHINE,&gpVpenDKey,&hkResult)==ERROR_SUCCESS)
   {
      // If memory is available, then read and fill it.
      if(!(ddMiscFlags&LOADING_NO_MEMORY))
      {
         ddMiscFlags|=ReadFromRegistry(hkResult);

         AddExtraSettings();

         if (ddMiscFlags == LOADING_SUCCESS) // fixes RIP on first boot (limbo case)
            ddMiscFlags|=ClaimResources();
      }

      _RegSetValueEx(hkResult,&gpBootStatus,0,
         REG_BINARY,&ddMiscFlags,sizeof(DWORD));

      // need to close "HKLM\...\VPEND"
      _RegCloseKey(hkResult);
   }
#ifdef DEBUG
   DisplayInformation();
#endif

   DBG_TRACE("Version 2.0 virtual pen driver successfully loaded.     ");
   DBG_TRACE("--------------------------------------------------------");

   // Always return success.
   _asm clc
}

DWORD ReadFromRegistry(VMMHKEY hkVpenD)
{
   DWORD dwStatus=LOADING_SUCCESS;
   DWORD dwcbData;
   DWORD dwType=REG_SZ;
   DWORD dwNext;
   char szModelBuf[MODEL_SIZE];
   char pModel[]="Model";
   char pPressure[]="Pressure";
   char pHeight[]="Height";
   char pAngleXY[]="AngleXY";
   char pAngleZ[]="AngleZ";
   BYTE bPressure;
   BYTE bHeight;
   BYTE bAngleXY;
   BYTE bAngleZ;
   VMMHKEY hkModel;

#ifdef BUILD_REGISTRY
   // This code only needs to be executed if you do not use PENWIN.INF and
   // the Windows 95 setup program to build registry values.
   SetupVpenD(hkVpenD);
#endif

   dwcbData=MODEL_SIZE;

   // get hardware model the virtual pen driver will be interacting with...
   if(_RegQueryValueEx(hkVpenD,&pModel,NULL,&dwType,&szModelBuf,&dwcbData) == ERROR_SUCCESS)
   {
      if(_RegOpenKey(hkVpenD,&szModelBuf,&hkModel)==ERROR_SUCCESS)
      {
         if(ReadRegStruct(hkModel,(PVOID)pVMPenInfo,
            gpPenInfo,sizeof(DRV_PENINFO)) == NULL)
         dwStatus|=LOADING_NO_PENINFO;

         if(ReadRegStruct(hkModel,(PVOID)pVMHwInfo,
            gpHardwareInfo,sizeof(_HARDWAREINFO)) == NULL)
         dwStatus|=LOADING_NO_HARDWAREINFO;

         // only look for rate table on serial devices
         if(pVMHwInfo->dwHwFlags & HW_SERIAL )
         {
            if( ParseRateTable(hkModel,gpRateTable,
               gpRateCommands,gpRateValues) == NULL)
            dwStatus|=LOADING_NO_RATETABLE;

            if( ParseCommands(hkModel,gpCommandStrs,
               gpCommandSet) == NULL)
            dwStatus|=LOADING_NO_RATETABLE;
         }

         dwcbData=sizeof(DWORD);
         _RegQueryValueEx(hkModel,&gpForce,NULL,&dwType,&ddForce,&dwcbData);

         dwcbData=sizeof(DWORD);
         _RegQueryValueEx(hkModel,&gpInductive,NULL,&dwType,&ddInductive,&dwcbData);


         // need to close "Model" key
         _RegCloseKey(hkModel);

      }
      else
      {
         dwStatus|=LOADING_NO_MODEL_VALUE;
      }
   }
   else
   {
      dwStatus|=LOADING_NO_MODEL_ENTRY;
   }

   // Get the friendly values: Pressure, Height, AngleXY, AngleZ.
   dwcbData=1;
   _RegQueryValueEx(hkVpenD,&pPressure,NULL,&dwType,&bPressure,&dwcbData);
   dwcbData=1;
   _RegQueryValueEx(hkVpenD,&pHeight,NULL,&dwType,&bHeight,&dwcbData);
   dwcbData=1;
   _RegQueryValueEx(hkVpenD,&pAngleXY,NULL,&dwType,&bAngleXY,&dwcbData);
   dwcbData=1;
   _RegQueryValueEx(hkVpenD,&pAngleZ,NULL,&dwType,&bAngleZ,&dwcbData);

   // Need to construct PenInfo to fit what the user asked for.
   // This process basically means that the pen info structure needs to be
   // collapsed and anything that is not needed is removed.
   
   dwcbData=0;  // This will be the cbOemData field when done.
   dwType=0;    // This will be the fuOEM field when done.
   dwNext=0;

   if(pVMHwInfo->dwHwFlags & PHW_PRESSURE)  // hardware supports pressure
   {
      if(bPressure)                         // user wants pressure
      {
         // Added fuOEM flag and add information to structure.  In this
         // case pressure info will be in the first position so there is
         // nothing to do.
         dwType|=PHW_PRESSURE;  // add the fuOEM flag
         dwcbData++;            // add the number of words
      }
       else // User doesn't want pressure so remove it.
      {
         pVMPenInfo->rgoempeninfo[dwcbData].wPdt=0;
         pVMPenInfo->rgoempeninfo[dwcbData].wValueMax=0;
         pVMPenInfo->rgoempeninfo[dwcbData].wDistinct=0;
      }
      dwNext++;
   }

   // same for Height...
   if(pVMHwInfo->dwHwFlags & PHW_HEIGHT) // hardware supports Height
   {
      if(bHeight)                        // user wants Height
      {
         // Set the flag, copy the structure, and advance the pointer.
         dwType|=PHW_HEIGHT;
         pVMPenInfo->rgoempeninfo[dwcbData]=pVMPenInfo->rgoempeninfo[dwNext];
         dwcbData++;
      }
      else
      {
         pVMPenInfo->rgoempeninfo[dwcbData].wPdt=0;
         pVMPenInfo->rgoempeninfo[dwcbData].wValueMax=0;
         pVMPenInfo->rgoempeninfo[dwcbData].wDistinct=0;
      }
      dwNext++;
   }

   // same for AngleXY

   if(pVMHwInfo->dwHwFlags & PHW_ANGLEXY) // hardware supports AngleXY
   {
      if(bAngleXY)                        // user wants AngleXY
      {
         // Set the flag, copy the structure, and advance the pointer.
         dwType|=PHW_ANGLEXY;
         pVMPenInfo->rgoempeninfo[dwcbData]=pVMPenInfo->rgoempeninfo[dwNext];
         dwcbData++;
      }
      else
      {
         pVMPenInfo->rgoempeninfo[dwcbData].wPdt=0;
         pVMPenInfo->rgoempeninfo[dwcbData].wValueMax=0;
         pVMPenInfo->rgoempeninfo[dwcbData].wDistinct=0;
      }
      dwNext++;
   }

   // same for AngleZ
   if(pVMHwInfo->dwHwFlags & PHW_ANGLEZ) // hardware supports AngleZ
   {
      if(bAngleZ)                        // user wants AngleZ
      {
         // Set the flag, copy the structure, and advance the pointer.
         dwType|=PHW_ANGLEZ;
         pVMPenInfo->rgoempeninfo[dwcbData]=pVMPenInfo->rgoempeninfo[dwNext];
         dwcbData++;
      }
      else
      {
         pVMPenInfo->rgoempeninfo[dwcbData].wPdt=0;
         pVMPenInfo->rgoempeninfo[dwcbData].wValueMax=0;
         pVMPenInfo->rgoempeninfo[dwcbData].wDistinct=0;
      }
      dwNext++;
   }

   pVMPenInfo->fuOEM=dwType;
   pVMPenInfo->cbOemData=dwcbData*2;

return dwStatus;
}

void AddExtraSettings()
{
   pVMHwInfo->SystemVMHandle=Get_Sys_VM_Handle();

   // check rotation
   if( (pVMHwInfo->ddOrientation==1) || (pVMHwInfo->ddOrientation==3) )
   {
      // wDistinctWidth is the height and height is the width
      DWORD dwHeight,dwWidth;

      dwHeight=pVMPenInfo->wDistinctWidth;
      dwWidth=pVMPenInfo->wDistinctHeight;
      pVMPenInfo->wDistinctWidth=dwWidth;
      pVMPenInfo->wDistinctHeight=dwHeight;

      // also rotate the raw dimensions...
      dwHeight=pVMPenInfo->cxRawWidth;
      dwWidth=pVMPenInfo->cyRawHeight;
      pVMPenInfo->cxRawWidth=dwWidth;
      pVMPenInfo->cyRawHeight=dwHeight;
   }

   if (pVMHwInfo->calibrate.dwDistinctHeight == 0)
   {
      pVMHwInfo->calibrate.dwDistinctHeight = pVMPenInfo->wDistinctHeight;
      pVMHwInfo->calibrate.dwDistinctWidth = pVMPenInfo->wDistinctWidth;
   }

#ifdef DEBUG
   // add extra fuOEM values
   pVMHwInfo->dwHwFlags=pVMHwInfo->dwHwFlags | HW_SEQUENTIAL | HW_PROFILE | HW_DEBUG;
#endif
}


DWORD ClaimResources()
{
   DWORD dwStatus=LOADING_SUCCESS;

   // Need to acquire comm port so another app won't start using same comm port.

   if(pVMHwInfo->dwHwFlags & HW_SERIAL)
   {
      if( !VpenD_AcquirePort(pVMHwInfo->ddCom_Port,"vpend.vxd") )
      {
         DBG_MISC(0,"Could not acquire port");
         dwStatus|=LOADING_NO_ACQUIRE_PORT;
         return dwStatus;
      }
   }

   // virtualize IRQ
   VpenD_IRQ_Desc.VID_IRQ_Number=(USHORT)pVMHwInfo->ddIRQ_Number;

   if(VpenD_IRQ_Desc.VID_Hw_Int_Proc)
   {
      if(VpenD_IRQ_Desc.VID_EOI_Proc)
      {
         pVMHwInfo->VpenD_IRQ_Handle=VPICD_Virtualize_IRQ(&VpenD_IRQ_Desc);

         if(!pVMHwInfo->VpenD_IRQ_Handle)
            dwStatus|=LOADING_NO_IRQ_HANDLE;
      }
      else
         dwStatus|=LOADING_NO_EOI_PROC;
   }
   else
      dwStatus|=LOADING_NO_HW_PROC;

   return dwStatus;
}

#ifdef DEBUG
void DisplayInformation()
{
    Dbg_MiscPtrs(pVMHwInfo,
       pVMPenInfo,
       gpRateTable,
       gpCommandStrs,
       ActiveDevice,
       NullDevice);

    // display IRQ descriptor
    DBG_MISC   (VpenD_IRQ_Desc.VID_IRQ_Number,      "VpenD_IRQ_Desc.VID_IRQ_Number");
    DBG_MISC_HEX(VpenD_IRQ_Desc.VID_Options,      "VpenD_IRQ_Desc.VID_Options");
    DBG_MISC_HEX(VpenD_IRQ_Desc.VID_Hw_Int_Proc,   "&VpenD_Hw_Int");
    DBG_MISC_HEX(VpenD_IRQ_Desc.VID_Virt_Int_Proc,   "VpenD_IRQ_Desc.VID_Virt_Int_Proc");
    DBG_MISC_HEX(VpenD_IRQ_Desc.VID_EOI_Proc,      "&VpenD_EOI");
    DBG_MISC_HEX(VpenD_IRQ_Desc.VID_Mask_Change_Proc,"VpenD_IRQ_Desc.VID_Mask_Change_Proc");
    DBG_MISC_HEX(VpenD_IRQ_Desc.VID_IRET_Proc,      "VpenD_IRQ_Desc.VID_IRET_Proc");
    DBG_MISC   (VpenD_IRQ_Desc.VID_IRET_Time_Out,   "VpenD_IRQ_Desc.VID_IRET_Time_Out");
    DBG_MISC_HEX(VpenD_IRQ_Desc.VID_Hw_Int_Ref,      "VpenD_IRQ_Desc.VID_Hw_Int_Ref");
    DBG_TRACE("");

    // Display information that has been gathered.
    Dbg_Hardware(pVMHwInfo);

    // PENINFO structure
    Dbg_PenInfo(pVMPenInfo);

    // display command table
    // Dbg_Commands(gpCommandStrs);
}
#endif


DWORD ReadRegStruct(VMMHKEY hkModel,PVOID pDataDest,PCHAR pRegEntry,
            DWORD dwSize)
{
   DWORD dwType;
   DWORD dwRet=TRUE;

   if(_RegQueryValueEx(hkModel,pRegEntry,NULL,&dwType,pDataDest,&dwSize) != ERROR_SUCCESS)
   {
      dwRet=FALSE;
   }
   return dwRet;
}

DWORD ParseRateTable(VMMHKEY hkModel,PRATETABLE pRateTable,
                  PCHAR pRateCommands,PCHAR pRateValues)
{
   DWORD dwType;
   char szBuf[256];
   DWORD dwSize;
   DWORD dwi,dwj;
   DWORD dwRet=FALSE;

   dwSize=256;
   if(_RegQueryValueEx(hkModel,pRateValues,NULL,&dwType,&szBuf,&dwSize) == ERROR_SUCCESS)
   {
      // The RateValues have been read.
      DWORD dwTimes=dwSize/sizeof(DWORD);
      PDWORD pSource=(PDWORD)szBuf;

      // add values to data structure...
      dwj=0;

      for(dwi=0;dwi<dwTimes;dwi++)
      {
         pRateTable[dwj++].dwRate=*pSource++;
      }

      dwSize=256;
      if(_RegQueryValueEx(hkModel,pRateCommands,NULL,&dwType,&szBuf,&dwSize) == ERROR_SUCCESS)
      {
         // The rate commands are in szBuf and dwSize is the buffer size.
         // This loop walks the strings read and places them into the
         // CommandStr location of the ratetable.
         PCHAR pNext=szBuf;

         for(dwi=0;dwi<dwj;dwi++)
         {
            pNext+=lstrcpy(pRateTable[dwi].CommandStr,pNext);
         }
         dwRet=TRUE;
      }
   }
    return dwRet;
}


DWORD ParseCommands(VMMHKEY hkModel,PCOMMANDS pCommandStrs,
                   PCHAR pCommandSet)
{
   DWORD dwType;
   DWORD dwSize;
   DWORD dwRet=FALSE;
   char szBuf[256];
   DWORD dwi;

   dwSize=256;
   if(_RegQueryValueEx(hkModel,pCommandSet,NULL,&dwType,&szBuf,&dwSize) == ERROR_SUCCESS)
   {
      // The command strings have been read.
      // This loop walks the strings read and places them into the
      // CommandStr location of the COMMANDS table.
      PCHAR pNext=szBuf;

      for(dwi=0;dwi<API_NUMBER;dwi++)
      {
         pNext+=lstrcpy(pCommandStrs[dwi].CommandStr,pNext);
      }
      dwRet=TRUE;
   }
   return dwRet;
}


/*
    Copies all characters and the following NULL character.  It returns the
    number of characters including the NULL.
*/
DWORD lstrcpy(PCHAR pDest,PCHAR pSource)
{
   int i=0;

   while(*pSource)
   {
      *pDest++=*pSource++;
      i++;
   }
   *pDest=(char)NULL;
   return ++i;
}

//End-Of-File
