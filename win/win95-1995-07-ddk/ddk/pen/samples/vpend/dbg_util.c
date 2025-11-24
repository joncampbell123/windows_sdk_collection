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

/* DBG_UTIL.c: 

   This file contains debug routines.<nl>

   Routines contained in module:<nl>
*/

#include "defines.h"

#include <basedef.h>
#include <vmm.h>
#include "PenDrv.h"
#include "protos.h"
#ifdef DEBUG
#include <debug.h>
#include "sysutil.h"

#pragma VxD_LOCKED_DATA_SEG

typedef struct tag_MSGS {
   DWORD dwMsg;
   char *pStr;
   } MSGS, *PMSGS;

MSGS msgs[] = {
   {VPEND_LOAD,              "VPEND_LOAD"},
   {VPEND_FREE,              "VPEND_FREE"},
   {VPEND_ENABLE,            "VPEND_ENABLE"},
   {VPEND_DISABLE,           "VPEND_DISABLE"},
   {VPEND_SETSAMPLINGRATE,   "VPEND_SETSAMPLINGRATE"},
   {VPEND_GETSAMPLINGRATE,   "VPEND_GETSAMPLINGRATE"},
   {VPEND_SETSAMPLINGDIST,   "VPEND_SETSAMPLINGDIST"},
   {VPEND_GETSAMPLINGDIST,   "VPEND_GETSAMPLINGDIST"},
   {VPEND_PENPLAYSTART,      "VPEND_PENPLAYSTART"},
   {VPEND_PENPLAYBACK,       "VPEND_PENPLAYBACK"},
   {VPEND_PENPLAYSTOP,       "VPEND_PENPLAYSTOP"},
   {VPEND_GETCALIBRATION,    "VPEND_GETCALIBRATION"},
   {VPEND_SETCALIBRATION,    "VPEND_SETCALIBRATION"},
   {VPEND_GETPENINFO,        "VPEND_GETPENINFO"},
   {0L,NULL}
} ;

char *gpMsgStr;

#ifdef OLDCODE
char pExpectNullP1[]="lParam1: Expecting NULL parameter!!\n\r";
char pExpectNullP2[]="lParam2: Expecting NULL parameter!!\n\r";

// Format strings
char pFormatTrace[] = "t  VpenD %s %s\n\r";
char pFormatMisc[] =  "t  VpenD %s : %lu : %s\n\r";
char pFormatMiscHex[] = "t  VpenD %s : %lx : %s\n\r";
char pFormatWarning[] = "wn VpenD %s : %lu : %s\n\r";
char pFormatReturn[] = "t  VpenD %s returning %lu\n\r";
char pFormatError[] =  "er VpenD %s : %lu : %s\n\r";
#endif

DWORD dword_to_string(DWORD dwNum,char *pBuffer,LONG dwBase);
DWORD dword_to_hex_string(DWORD dwNum,char *pBuffer);
DWORD characters_to_string(char *pstr, char *pbuffer);

#pragma VxD_LOCKED_CODE_SEG


/*
-------------------------------------------------------------------------------
 Function:   Dbg_Output
 Purpose:    Displays output messages on the debug terminal.
 Params:
   dwType      - type of message
   dwCondition - message dependent
   pStr        - A pointer to a message that will be displayed.
 Returns:
 Globals:
   gpMsgStr   - A pointer to a string that defines what messages are
                being processed.
 Comments:  Want output to look something like:

   t  VpenD VpenD_LOAD Entering
   wn VpenD VpenD_SETSAMPLINGRATE 300 rate out of range!
   er VpenD VpenD_LOAD 0 invalid register structure pointer!

   * Must compile DEBUG to get this routine.*
-------------------------------------------------------------------------------
*/
void Dbg_Output(DWORD dwType,DWORD dwValue,char *pStr)
{
   char szBuffer[120];
   char *pBuffer=szBuffer;
   char *pMsgStr=gpMsgStr;

   _asm pusha
   // copy title string
   while(*pMsgStr)
      *pBuffer++=*pMsgStr++;

   *pBuffer++=' ';

   switch(dwType)
   {
      case DBGM_STRING:
#ifdef MAXDEBUG
         // insert value here...
         pBuffer+=characters_to_string((char *)dwValue,pBuffer);
#endif
         break;
      case DBGM_TRACE:
         break;
      case DBGM_MISC_HEX:
#ifdef MAXDEBUG
         // insert value here...
         pBuffer+=dword_to_string(dwValue,pBuffer,16);
         //pBuffer+=dword_to_hex_string(dwValue,pBuffer);
#endif
         break;
      case DBGM_WARNING:
      case DBGM_RETURN:
      case DBGM_MISC:
      case DBGM_ERROR:
#ifdef MAXDEBUG
         // insert value here...
         pBuffer+=dword_to_string(dwValue,pBuffer,10);
#endif
         break;
   }

   *pBuffer++=' ';

   // Copy message string.
   while(*pStr)
      *pBuffer++=*pStr++;

   // Need to append carriage return and line feed.
   *pBuffer++=(char)'\n';
   *pBuffer++=(char)'\r';
   *pBuffer=(char)NULL;

   Out_Debug_String(szBuffer);
   _asm popa
}

DWORD characters_to_string(char *pstr, char *pBuffer)
{
   DWORD dwi=0;

   while(*pstr)
   {
      // if valid character
      if( (*pstr>=' ') && (*pstr <='~') )
      {
         // copy it
         *pBuffer++=*pstr;
         dwi++;
      }
      else
      {
         switch(*pstr)
         {
            case '\x0A': //lf
               *pBuffer++='\\';
               *pBuffer++='x';
               *pBuffer++='0';
               *pBuffer++='A';
               dwi+=4;
               break;
            case '\x0D': //cr
               *pBuffer++='\\';
               *pBuffer++='x';
               *pBuffer++='0';
               *pBuffer++='D';
               dwi+=4;
               break;
            case '\x1B': //esc
               *pBuffer++='\\';
               *pBuffer++='x';
               *pBuffer++='1';
               *pBuffer++='B';
               dwi+=4;
               break;
            default:    // make it a period
               *pBuffer++='.';
               dwi++;
               break;
         }
      }

      // advance pointers
      pstr++;
   }
   return dwi;
}


DWORD dword_to_string(DWORD dwNum,char *pBuffer,LONG lBase)
{
   char szTmp[80];
   int k;
   DWORD dwRemainder=0;
   DWORD i=0;
   BOOL bNeg=FALSE;

   if(dwNum==0)
   {
      *pBuffer++='0';
      *pBuffer=(char)NULL;
      return 1;
   }
   else
   {
      if(lBase == 10 )
      {
         // If the high bit is set, it's negative.
         if(dwNum & 0x80000000)
         {
            (LONG)dwNum=(LONG)0-(LONG)dwNum;   // make positive
            bNeg=TRUE;
         }
      }

      while(dwNum)
      {
         dwRemainder=dwNum%lBase;
         dwNum=dwNum/lBase;

         switch(dwRemainder)
         {
            case 0: szTmp[i++]='0'; break;
            case 1: szTmp[i++]='1'; break;
            case 2: szTmp[i++]='2'; break;
            case 3: szTmp[i++]='3'; break;
            case 4: szTmp[i++]='4'; break;
            case 5: szTmp[i++]='5'; break;
            case 6: szTmp[i++]='6'; break;
            case 7: szTmp[i++]='7'; break;
            case 8: szTmp[i++]='8'; break;
            case 9: szTmp[i++]='9'; break;
            case 10: szTmp[i++]='A'; break;
            case 11: szTmp[i++]='B'; break;
            case 12: szTmp[i++]='C'; break;
            case 13: szTmp[i++]='D'; break;
            case 14: szTmp[i++]='E'; break;
            case 15: szTmp[i++]='F'; break;
         }
      }

      if(i)
      {
         if(bNeg)
         {
            szTmp[i++]='-';
         }

         // copy backwards buffer
         for(k=i-1;k>=0;k--)
            *pBuffer++=szTmp[k];

         *pBuffer=(char)NULL;
      }
   }
   return i;
}

/*
-------------------------------------------------------------------------------
 Function:  Dbg_RecordMessage
 
-------------------------------------------------------------------------------
*/
void Dbg_RecordMessage(DWORD dwMsg)
{
   PMSGS pMessages=&msgs[0];
   dwMsg&=~(VpenD_lParam1_ptr | VpenD_lParam2_ptr | VPEND_OEM);

   while(pMessages->pStr)
   {
      if(pMessages->dwMsg==dwMsg)
      {
         gpMsgStr=pMessages->pStr;
         break;
      }
      else
         pMessages++;
   }
   if(!gpMsgStr)
      gpMsgStr="Unknown Message";
}


void Dbg_MiscPtrs(P_HARDWAREINFO phw,
        LPDRV_PENINFO ppi,
        PRATETABLE prt,
        PCOMMANDS pcs,
        P_VpenD_Register pad,
        P_VpenD_Register pnd)
{
    DBG_MISC_HEX((DWORD)phw,"P_HARDWAREINFO");
    DBG_MISC_HEX((DWORD)ppi,"LPDRV_PENINFO");
    DBG_MISC_HEX((DWORD)prt,"PRATETABLE");
    DBG_MISC_HEX((DWORD)pcs,"PCOMMANDS");
    DBG_MISC_HEX((DWORD)pad,"P_VpenD_Register");
    DBG_MISC_HEX((DWORD)pnd,"P_VpenD_Register");
    DBG_TRACE("");
}

void Dbg_Hardware(P_HARDWAREINFO pHwInfo)
{
   // HARDWARE structure
   DBG_MISC(pHwInfo->ddHardwareType, "hw.ddHardwareType");
   DBG_MISC(pHwInfo->ddCom_Port, "hw.ddCom_Port");
   DBG_MISC_HEX(pHwInfo->com_base, "hw.com_base");
   DBG_MISC(pHwInfo->ddIRQ_Number, "hw.ddIRQ_Number");
   DBG_MISC_HEX(pHwInfo->ddPauseTime, "hw.ddPauseTime");
   DBG_MISC_HEX(pHwInfo->ddReserved, "hw.ddReserved");
   DBG_MISC_HEX(pHwInfo->SystemVMHandle, "hw.SystemVMHandle");
   DBG_MISC_HEX(pHwInfo->VpenD_IRQ_Handle, "hw.VpenD_IRQ_Handle");
   DBG_MISC(pHwInfo->ddOrientation, "hw.ddOrientation");
   DBG_MISC(pHwInfo->calibrate.dwOffsetX, "hw.calibrate.dwOffsetX");
   DBG_MISC(pHwInfo->calibrate.dwOffsetY, "hw.calibrate.dwOffsetY");
   DBG_MISC(pHwInfo->calibrate.dwDistinctWidth, "hw.calibrate.dwDistinctWidth");
   DBG_MISC(pHwInfo->calibrate.dwDistinctHeight, "hw.calibrate.dwDistinctHeight");
   DBG_MISC(pHwInfo->ddBufferZoneX, "hw.ddBufferZoneX");
   DBG_MISC(pHwInfo->ddBufferZoneY, "hw.ddBufferZoneY");
   DBG_MISC(pHwInfo->dwHardwarePacketSize, "hw.dwHardwarePacketSize");
   DBG_MISC_HEX(pHwInfo->dwComPortSettings, "hw.dwComPortSettings");
   DBG_MISC_HEX(pHwInfo->dwHwFlags, "hw.dwHwFlags");
   if(pHwInfo->dwHwFlags & HW_CALIBRATE)
      DBG_TRACE("    HW_CALIBRATE");
   if(pHwInfo->dwHwFlags & HW_BUFFERZONE)
      DBG_TRACE("    HW_BUFFERZONE");
   if(pHwInfo->dwHwFlags & HW_TIMERMODEL)
      DBG_TRACE("    HW_TIMERMODEL");
   if(pHwInfo->dwHwFlags & HW_PUSHMODEL)
      DBG_TRACE("    HW_PUSHMODEL");
   if(pHwInfo->dwHwFlags & HW_SERIAL)
      DBG_TRACE("    HW_SERIAL");
   if(pHwInfo->dwHwFlags & HW_BIOS)
      DBG_TRACE("    HW_BIOS");

   if(pHwInfo->dwHwFlags & HW_DEBUG)
      DBG_TRACE("    HW_DEBUG");
   if(pHwInfo->dwHwFlags & HW_SEQUENTIAL)
      DBG_TRACE("    HW_SEQUENTIAL");
   if(pHwInfo->dwHwFlags & HW_PROFILE)
      DBG_TRACE("    HW_PROFILE");

   if(pHwInfo->dwHwFlags & PHW_PRESSURE)
      DBG_TRACE("    PHW_PRESSURE");
   if(pHwInfo->dwHwFlags & PHW_HEIGHT)
      DBG_TRACE("    PHW_HEIGHT");
   if(pHwInfo->dwHwFlags & PHW_ANGLEXY)
      DBG_TRACE("    PHW_ANGLEXY");
   if(pHwInfo->dwHwFlags & PHW_ANGLEZ)
      DBG_TRACE("    PHW_ANGLEZ");
   if(pHwInfo->dwHwFlags & PHW_BARRELROTATION)
      DBG_TRACE("    PHW_BARRELROTATION");

   DBG_MISC(pHwInfo->ddTimerTickRate,   "==ddTimerTickRate");
   DBG_MISC(pHwInfo->ddDelayTime,      "==ddDelayTime");

   DBG_TRACE("");
}

void Dbg_PenInfo(LPDRV_PENINFO pPenInfo)
{
   int i;

   DBG_MISC(pPenInfo->cxRawWidth,      "pi.cxRawWidth");
   DBG_MISC(pPenInfo->cyRawHeight,      "pi.cyRawHeight");
   DBG_MISC(pPenInfo->wDistinctWidth,   "pi.wDistinctWidth");
   DBG_MISC(pPenInfo->wDistinctHeight,   "pi.wDistinctHeight");
   DBG_MISC(pPenInfo->nSamplingRate,   "pi.nSamplingRate");
   DBG_MISC(pPenInfo->nSamplingDist,   "pi.nSamplingDist");
   DBG_MISC_HEX(pPenInfo->lPdc,      "pi.lPdc");
   DBG_MISC(pPenInfo->cPens,         "pi.cPens");

   DBG_MISC(pPenInfo->cbOemData,      "pi.cbOemData");

   for(i=0;i<(pPenInfo->cbOemData/2);i++)
   {
      DBG_MISC(pPenInfo->rgoempeninfo[i].wPdt,      "pi.rgoempeninfo[x].wPdt");
      DBG_MISC(pPenInfo->rgoempeninfo[i].wValueMax,   "pi.rgoempeninfo[x].wValueMax");
      DBG_MISC(pPenInfo->rgoempeninfo[i].wDistinct,   "pi.rgoempeninfo[x].wDistinct");
   }

   for(i=0;i<8;i++)
   {
      if(pPenInfo->rgdwReserved[i])
         DBG_MISC(pPenInfo->rgdwReserved[i],"pi.rgdwReserved[x]");
   }

   DBG_MISC_HEX(pPenInfo->fuOEM,         "pi.fuOEM");

   // Standard flags
   if(pPenInfo->fuOEM & PHW_PRESSURE)
      DBG_TRACE("    PHW_PRESSURE");
   if(pPenInfo->fuOEM & PHW_HEIGHT)
      DBG_TRACE("    PHW_HEIGHT");
   if(pPenInfo->fuOEM & PHW_ANGLEXY)
      DBG_TRACE("    PHW_ANGLEXY");
   if(pPenInfo->fuOEM & PHW_ANGLEZ)
      DBG_TRACE("    PHW_ANGLEZ");
   if(pPenInfo->fuOEM & PHW_BARRELROTATION)
      DBG_TRACE("    PHW_BARRELROTATION");
   if(pPenInfo->fuOEM & PHW_OEMSPECIFIC)
      DBG_TRACE("    PHW_OEMSPECIFIC");

   DBG_MISC(pPenInfo->dwSamplingRateMin,   "pi.dwSamplingRateMin");
   DBG_MISC(pPenInfo->dwSamplingRateMax,   "pi.dwSamplingRateMax");
   DBG_TRACE("");

}

void Dbg_Commands(PCOMMANDS pComStrs)
{
   DWORD i;
   char szBuffer[120];
   char *pBuffer;
   char *pMsgStr;
   char *pCommand;

   for(i=API_RESET;i<API_NUMBER;i++)
   {
      pMsgStr=gpMsgStr;
      pBuffer=szBuffer;
      pCommand=&pComStrs[i].CommandStr[0];

      // copy title string
      while(*pMsgStr)
         *pBuffer++=*pMsgStr++;
      *pBuffer++=' ';

      // copy Command string
      while(*pCommand)
         *pBuffer++=*pCommand++;
      *pBuffer++=' ';

      // terminate the string
      *pBuffer++=(char)'\n';
      *pBuffer++=(char)'\r';
      *pBuffer=(char)NULL;

      Out_Debug_String(szBuffer);
   }
   Out_Debug_String("\n\r");
}

void dbg_penpacket(LPDRV_PENPACKET lppp)
{
   char szBuffer[120];
   char *pBuffer=szBuffer;

#if 0
   *pBuffer++='X';
   *pBuffer++='=';
   pBuffer+=dword_to_string((LONG)(DWORD)lppp->wTabletX,pBuffer,10);

   *pBuffer++=',';
   *pBuffer++=' ';
   *pBuffer++='Y';
   *pBuffer++='=';
   pBuffer+=dword_to_string((LONG)(DWORD)lppp->wTabletY,pBuffer,10);

   *pBuffer++=',';
   *pBuffer++=' ';
   *pBuffer++='P';
   *pBuffer++='D';
   *pBuffer++='K';
   *pBuffer++='=';
   pBuffer+=dword_to_string((LONG)(int)lppp->wPDK,pBuffer,16);

   *pBuffer++=',';
   *pBuffer++=' ';
   *pBuffer++='T';
   *pBuffer++='=';
   pBuffer+=dword_to_string((LONG)lppp->ddTimeStamp,pBuffer,10);

#ifdef DEBUG
   *pBuffer++=',';
   *pBuffer++=' ';

   *pBuffer++='S';
   *pBuffer++='e';
   *pBuffer++='q';
   *pBuffer++='=';
   pBuffer+=dword_to_string((LONG)lppp->ddSequential,pBuffer,10);

//   *pBuffer++=',';
//   *pBuffer++=' ';

//   lppp->ddProfile
//   lppp->Debug
#endif

   // Need to append Carrage return and line feed.
   *pBuffer++=(char)'\n';
   *pBuffer++=(char)'\r';
   *pBuffer=(char)NULL;

   Out_Debug_String(szBuffer);
#endif // 0
}

#ifdef OLDCODE
void Dbg_PenPacket(LPDRV_PENPACKET pPP)
{
   int i;
   DBG_MISC(pPP->wTabletX,      "pPP->wTabletX");
   DBG_MISC(pPP->wTabletY,      "pPP->wTabletY");
   DBG_MISC_HEX(pPP->wPDK,      "pPP->wPDK");

#ifdef FOOBAR
   for(i=0;i<MAXOEMDATAWORDS;i++)
      DBG_MISC(pPP->rgwOemData[i],"pPP->rgwOemData[x]");
#endif

   DBG_MISC(pPP->ddTimeStamp,   "pPP->ddTimeStamp");
   DBG_MISC(pPP->ddSequential,   "pPP->ddSequential");

#ifdef FOOBAR
   DBG_MISC(pPP->ddProfile,   "pPP->ddProfile");
   DBG_MISC(pPP->ddDebug,      "pPP->ddDebug");
#endif

   DBG_TRACE("");
}
#endif

void Dbg_ActiveDevice(P_VpenD_Register pAD)
{
   // Active Device
   DBG_MISC_HEX(pAD->Device_VM_Handle,   "pAD->Device_VM_Handle");
   DBG_MISC(pAD->ibThis,            "pAD->ibThis");
   DBG_MISC(pAD->ibThis_offset,      "pAD->ibThis_offset");
   DBG_MISC(pAD->ibNext,            "pAD->ibNext");
   DBG_MISC(pAD->ibNext_offset,      "pAD->ibNext_offset");
   DBG_MISC(pAD->ibCurrent,         "pAD->ibCurrent");
   DBG_MISC(pAD->iNumberOfPenPackets,   "pAD->iNumberOfPenPackets");
   DBG_MISC_HEX(pAD->wPDKLast,         "pAD->wPDKLast");

   DBG_MISC_HEX(pAD->pDataType,      "pAD->pDataType");
   DBG_MISC_HEX(pAD->piNumberOfPenPackets,"pAD->piNumberOfPenPackets");
   DBG_MISC_HEX(pAD->piThis_Offset,   "pAD->piThis_Offset");
   DBG_MISC_HEX(pAD->piThis,         "pAD->piThis");
   DBG_MISC_HEX((DWORD)pAD->pPenPacketBuffer,   "pAD->pPenPacketBuffer");
   DBG_MISC_HEX((DWORD)pAD->pPenInfo,         "pAD->pPenInfo");
   DBG_MISC_HEX(pAD->pOEMBuffer,      "pAD->pOEMBuffer");
   DBG_MISC_HEX(pAD->pfnEntryPoint,   "pAD->pfnEntryPoint");
   DBG_TRACE("");
}

char pFormatDump[]="t  VpenD: Buffer Bits: %s\n\r";

void Dbg_DumpByteBuffer(P_HARDWAREINFO pHwInfo, PBYTE pBuffer)
{
   char szResults[256];
   DWORD i,k;
   BYTE bBitSet;
   DWORD dwNumOfPackets=pHwInfo->dwHardwarePacketSize;
   DWORD dwPacketSizeBits=8;
   DWORD location=0;

   // for each byte
   for(i=0;i<dwNumOfPackets;i++)
   {
      // check each bit
      bBitSet=0x80;
      for(k=0;k<dwPacketSizeBits;k++)
      {
         if(pBuffer[i] & bBitSet)
            szResults[location++]='1';
         else
            szResults[location++]='0';
         bBitSet=bBitSet/2;
      }
      szResults[location++]=' ';
   }
   szResults[location++]='\0';
   My_Debug_Printf_Service(pFormatDump,&szResults);
}

#endif

//End-Of-File
