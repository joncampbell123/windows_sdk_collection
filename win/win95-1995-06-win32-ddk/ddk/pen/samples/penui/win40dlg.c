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

/* WIN40DLG.c: 

   Contains the dialog message handling routine.<nl>

   Routines contained in module:<nl>
      <l ConfigDlgProc.ConfigDlgProc><nl>
*/

#define WINVER 0x0400

#define USECOMM
#include <windows.h>
#include "..\inc\penui.h"

#include <winerror.h>


// constant strings
char gpVpenDKey[]="System\\CurrentControlSet\\Services\\VxD\\VPEND";
char gpModelStr[]="Model";
char gpHardwareInfo[]="HardwareInfo";
char gpFriendlyName[]="FriendlyName";

char gpPressure[]="Pressure";

static char gpNoTabletHardware[cbSzRcMax];

static char szComErr[cbSzRcMax];
static char szModelErr[cbSzRcMax];
static char szConfigDialog[cbSzRcMax];
static char szReboot[cbSzRcMax];

#ifdef DEBUG_MSGS
#define MSGBOX(s1,s2) MessageBox(GetFocus(),s1,s2,MB_OK)
#else
#define MSGBOX(s1,s2)
#endif

// Direct copy from pendrv.h!!!!!

typedef struct DRV_CalbStruct {
    DWORD dwOffsetX;
    DWORD dwOffsetY;
    DWORD dwDistinctWidth;
    DWORD dwDistinctHeight;
} DRV_CALBSTRUCT;

typedef struct tag_HARDWAREINFO /*_hardwareinfo*/ {
    DWORD ddCom_Port;
    DWORD com_base;
    DWORD ddIRQ_Number;
    DWORD ddPauseTime;
    DWORD ddReserved;

    DWORD ddOrientation;
    DRV_CALBSTRUCT calibrate;
    DWORD ddBufferZoneX;
    DWORD ddBufferZoneY;

    DWORD dwHardwarePacketSize;
    DWORD dwComPortSettings;   // contains divisor (baud rate), parity, data bits, stop bits
    DWORD dwHwFlags;
    DWORD ddHardwareType;

    // System information that the OEM will not need to modify.
    DWORD ddTimerTickRate;
    DWORD ddDelayTime;

    DWORD VpenD_IRQ_Handle;
    DWORD SystemVMHandle;

} _HARDWAREINFO, *P_HARDWAREINFO;

#define HW_SERIAL      0x00100000

// should be used by all main dialogs in DLL
extern HWND       ghMainDlg;         // exports.c
extern HINSTANCE  ghInst;            // exports.c

HWND   ghComboBox;
int    giIdcCom = -1;

//   Prototypes
DWORD Walk_And_Read_Registry(LPSTR lpdefault);   //LPREG_STRS lpRegMem);
DWORD Record_results(void);
DWORD Set_Com_Port_Defaults(LPSTR lpKey);
DWORD Get_Starting_Model(LPSTR lpStart,DWORD dwSize);
void  EnableComPorts(BOOL b, int rs);
BOOL  InitStrings(void);

void DisplayFriendlyName( LPSTR lpKey);
void EnablePressure(HKEY hKey,DWORD dwHwFlags,DWORD dwType);


BOOL FAR PASCAL _loadds ConfigDlgProc(
   HWND     hDlg,
   WORD     message,
   WPARAM   wParam,
   DWORD    lParam)
   {
   switch (message)
      {
      // need to load different models...
   case WM_INITDIALOG:
      {
      char szStartModel[256];
      int i=0;

      // Main dialog window handle
      ghMainDlg=hDlg;

      InitStrings();

      if( !(ghComboBox=GetDlgItem(ghMainDlg,IDC_COMBOBOX)) )
         {
         EndDialog( ghMainDlg, (int)DRV_COMBOBOXPROBLEM );
         return FALSE;
         }   

      Get_Starting_Model((LPSTR)szStartModel,256);

      // walk the registry
      Walk_And_Read_Registry((LPSTR)szStartModel);

      if ((lstrlen((LPSTR)szStartModel) != 0) && 
            lstrcmpi((LPSTR)gpNoTabletHardware, (LPSTR)szStartModel) != 0)
         {
         // enable and disable controls based on HW info in registry
         Set_Com_Port_Defaults((LPSTR)szStartModel);
         }
      else
         {
         EnableWindow(GetDlgItem(ghMainDlg,IDC_PRESSURE),FALSE);
         EnableComPorts(FALSE, rsUnknownGroup);
         CheckRadioButton(ghMainDlg,IDC_COM1,IDC_COM4,0);
         }

      return TRUE;
      }
      break;

   case WM_COMMAND:
      {
      WORD wNotifyCode=HIWORD(lParam);

      switch (wParam)
            {
            case IDC_COMBOBOX:
               {
               switch(wNotifyCode)
                  {
               case CBN_SELCHANGE:
                  {
                  char szResult[256];

                  GetWindowText(ghComboBox,(LPSTR)&szResult,256);

                  if(lstrcmpi((LPSTR)gpNoTabletHardware,(LPSTR)szResult) != 0)
                     {
                     DisplayFriendlyName((LPSTR)&szResult);
                     Set_Com_Port_Defaults((LPSTR)&szResult);
                     }
                  else
                     {
                     SetDlgItemText(ghMainDlg,IDC_FRIENDLYNAME,"");
                     EnableComPorts(FALSE, rsUnknownGroup);
                     EnableWindow(GetDlgItem(ghMainDlg,IDC_PRESSURE),FALSE);
                     CheckDlgButton(ghMainDlg,IDC_PRESSURE,FALSE);
                     }
                  }
               break;
                  }
               }
               break;

            case IDOK:
               {
               int iMB;

               // set registry with model string
               Record_results();

               iMB=MessageBox(ghMainDlg,(LPSTR)szReboot,
                  (LPSTR)szConfigDialog,MB_OK|MB_ICONEXCLAMATION);
               EndDialog( ghMainDlg, 0 );
               }
               break;

            case IDCANCEL:
               EndDialog( ghMainDlg, 0 );
               break;

            default:
               return FALSE;
            }
         }
      default:
         break;
      }
   return FALSE;
   }


DWORD Set_Com_Port_Defaults(LPSTR lpKey)
   {
   HKEY hkResult;
   DWORD dwRet=TRUE;

   if(RegOpenKey(HKEY_LOCAL_MACHINE,(LPSTR)gpVpenDKey,&hkResult) == ERROR_SUCCESS)
      {
      HKEY hkHwResult;
      if(RegOpenKey(hkResult,lpKey,&hkHwResult) == ERROR_SUCCESS)
         {
         DWORD dwHwType;
         _HARDWAREINFO HwInfo;
         DWORD dwfSize=sizeof(_HARDWAREINFO);

         if( RegQueryValueEx(hkHwResult,gpHardwareInfo,
              0,&dwHwType,(LPBYTE)&HwInfo,
              (LPDWORD)&dwfSize) == ERROR_SUCCESS)
            {

            /*
            If running on a machine that supports pressure, enable the pressure setting. 
            Any machine that does not support pressure should have the pressure check box 
            disabled.
            */
            EnablePressure(hkResult,HwInfo.dwHwFlags,HwInfo.ddHardwareType);

            EnableComPorts((HwInfo.dwHwFlags & HW_SERIAL) ? TRUE : FALSE, 0);

            if (giIdcCom == -1)   // unchanged?
               {
               switch(HwInfo.ddCom_Port)
                  {
                  case 1:
                     giIdcCom = IDC_COM1;
                     break;
                  case 2:
                     giIdcCom = IDC_COM2;
                     break;
                  case 3:
                     giIdcCom = IDC_COM3;
                     break;
                  case 4:
                     giIdcCom = IDC_COM4;
                     break;
                  default:
                     giIdcCom = 0;
                     break;
                  }
               CheckRadioButton(ghMainDlg, IDC_COM1, IDC_COM4, giIdcCom);
               }

            MSGBOX("Com == 1",(LPSTR)szConfigDialog);
            }
         else
            {
            dwRet=FALSE;
            MSGBOX("Can't read Hardware info structure",(LPSTR)szConfigDialog);
            }

         RegCloseKey(hkHwResult);
         }
      else
         {
         dwRet=FALSE;
         MSGBOX("Can't open registry key",(LPSTR)szConfigDialog);
         }

      RegCloseKey(hkResult);
      }
   else
      {
      dwRet=FALSE;
      MSGBOX("Can't open key",(LPSTR)szConfigDialog);
      }
   return dwRet;
   }


void EnablePressure(HKEY hk, DWORD dwHwFlags,DWORD dwType)
   {
   //Get previous state
   DWORD dwHwType;
   DWORD dwPressure=0;
   DWORD dwSize=sizeof(DWORD);

   RegQueryValueEx(hk,gpPressure,0,&dwHwType,(LPBYTE)&dwPressure,
      (LPDWORD)&dwSize);

     // enable any machine that supports pressure...
   if (dwHwFlags & PHW_PRESSURE)
      {
      if (dwPressure)
         CheckDlgButton(ghMainDlg,IDC_PRESSURE,TRUE);
      else
         CheckDlgButton(ghMainDlg,IDC_PRESSURE,FALSE);
      EnableWindow(GetDlgItem(ghMainDlg,IDC_PRESSURE),TRUE);
      }
   else
      {
      CheckDlgButton(ghMainDlg,IDC_PRESSURE,FALSE);
      EnableWindow(GetDlgItem(ghMainDlg,IDC_PRESSURE),FALSE);
      }
   }


void EnableComPorts(BOOL b, int rs)
   {
   WORD i;
   char sz[cbSzRcMax];

   *sz = '\0';   // insurance
   LoadString(ghInst, rs? rs:
      b? rsSerialGroup:   	// com device
      rsBiosGroup,      	// bios device
      (LPSTR)sz, cbSzRcMax);
   SetWindowText(GetDlgItem(ghMainDlg,IDC_SERIALGROUP), (LPSTR)sz);

   for(i=IDC_COM1;i<=IDC_COM4;i++)
      EnableWindow(GetDlgItem(ghMainDlg,i),b);
   }


/*
  Look for the default model.  Place it in lpStart; else place
  "no tablet hardware" in lpStart.
*/
DWORD Get_Starting_Model(LPSTR lpStart,DWORD dwSize)
   {
   HKEY hkResult;
   DWORD dwRet=TRUE;
   DWORD dwType;

   if( RegOpenKey(HKEY_LOCAL_MACHINE,(LPSTR)gpVpenDKey,&hkResult) == ERROR_SUCCESS)
      {
      if( RegQueryValueEx(hkResult,gpModelStr,
         0,&dwType,(LPBYTE)lpStart,
         (LPDWORD)&dwSize) != ERROR_SUCCESS)
         {
          lstrcpy(lpStart,(LPSTR)gpNoTabletHardware);
         }

      MSGBOX((LPSTR)lpStart,(LPSTR)szConfigDialog);
      RegCloseKey(hkResult);
      }
   else
      {
      lstrcpy(lpStart,(LPSTR)gpNoTabletHardware);
      MSGBOX("Can't open key",(LPSTR)szConfigDialog);
       }
   return dwRet;
   }


DWORD Record_results(void)
   {
   HKEY hkResult;
   char szResult[256];
   DWORD dwRet=TRUE;

   GetWindowText(ghComboBox,(LPSTR)&szResult,256);

   if( RegOpenKey(HKEY_LOCAL_MACHINE,(LPSTR)gpVpenDKey,&hkResult) == ERROR_SUCCESS)
      {
      RegSetValueEx(hkResult,(LPSTR)gpModelStr,0,REG_SZ,(LPBYTE)szResult,lstrlen((LPSTR)szResult));

      if(lstrcmpi((LPSTR)gpNoTabletHardware,(LPSTR)szResult) != 0)
         {
         // need to set the user preferences now...
         HKEY hkSub;
         DWORD dwHwType;
         DWORD dwSize;
         DWORD dwPressure;

         // set pressure if need be...
         if(IsDlgButtonChecked(ghMainDlg,IDC_PRESSURE))
            dwPressure=1;
         else
            dwPressure=0;

           RegSetValueEx(hkResult,gpPressure,0,dwHwType,
            (LPBYTE)&dwPressure,sizeof(BYTE));

         if( RegOpenKey(hkResult,(LPSTR)szResult,&hkSub) == ERROR_SUCCESS)
            {
            _HARDWAREINFO HwInfo;

            dwSize=sizeof(_HARDWAREINFO);
            if( RegQueryValueEx(hkSub,gpHardwareInfo,
                  0,&dwHwType,(LPBYTE)&HwInfo,
                  (LPDWORD)&dwSize) == ERROR_SUCCESS)
               {
               if(IsDlgButtonChecked(ghMainDlg,IDC_COM1))
                  {
                  HwInfo.ddCom_Port=1;
                  HwInfo.com_base=0x000003F8;
                  HwInfo.ddIRQ_Number=4;
                  }
               if(IsDlgButtonChecked(ghMainDlg,IDC_COM2))
                  {
                    HwInfo.ddCom_Port=2;
                  HwInfo.com_base=0x000002F8;
                  HwInfo.ddIRQ_Number=3;
                  }
               if(IsDlgButtonChecked(ghMainDlg,IDC_COM3))
                  {
                  HwInfo.ddCom_Port=3;
                  HwInfo.com_base=0x000003E8;
                  HwInfo.ddIRQ_Number=4;
                  }
               if(IsDlgButtonChecked(ghMainDlg,IDC_COM4))
                  {
                    HwInfo.ddCom_Port=4;
                    HwInfo.com_base=0x000002E8;
                    HwInfo.ddIRQ_Number=3;
                  }

               RegSetValueEx(hkSub,gpHardwareInfo,0,dwHwType,
                  (LPBYTE)&HwInfo,dwSize);
               }

            RegCloseKey(hkSub);
            }
         else
            {
            dwRet=FALSE;
            }
         }
      RegCloseKey(hkResult);
      }
   else
      {
        dwRet=FALSE;
        MSGBOX("Can't open key",(LPSTR)szConfigDialog);
      }
   return dwRet;
   }


/*
  Walk the registry starting with the VpenD directory.

*/
DWORD Walk_And_Read_Registry(LPSTR lpDefault)  //LPREG_STRS lpRegMem)
   {
   DWORD dwRet=TRUE;
   HKEY hkResult;

   if( RegOpenKey(HKEY_LOCAL_MACHINE,(LPSTR)gpVpenDKey,&hkResult) == ERROR_SUCCESS)
      {
      char szBuffer[256];
      char szSubBuffer[256];
      DWORD dwBufferSize=256;
      DWORD dwSubBufferSize=256;
      DWORD dwi,dwj;

      WORD wIndex=0;
      WORD wIndexTmp=0;

      // set the string
      SendMessage(ghComboBox,CB_ADDSTRING,0,
            (LPARAM)(LPSTR)gpNoTabletHardware);

      dwi=0;
      while(RegEnumKey(hkResult,dwi,(LPSTR)&szBuffer,dwBufferSize)
                           ==ERROR_SUCCESS)
         {
         HKEY hkSub;

         if( RegOpenKey(hkResult,(LPSTR)szBuffer,&hkSub) == ERROR_SUCCESS)
            {
            dwj=0;
            while(RegEnumKey(hkSub,dwj,(LPSTR)&szSubBuffer,dwSubBufferSize)
                           ==ERROR_SUCCESS)
               {
               char szResult[256];

               lstrcpy((LPSTR)szResult,(LPSTR)szBuffer);
               lstrcat((LPSTR)szResult,(LPSTR)"\\");
               lstrcat((LPSTR)szResult,(LPSTR)szSubBuffer);

               wIndexTmp++;

               // look and see if this is the default
               if(lstrcmpi(lpDefault,(LPSTR)szResult) == 0)
                  wIndex=wIndexTmp;

               // set the string
               SendMessage(ghComboBox,CB_ADDSTRING,0,
                  (LPARAM)(LPSTR)szResult);

               dwj++;
               }
            RegCloseKey(hkSub);
            }
         dwi++;
         }

      SendMessage(ghComboBox,CB_SETCURSEL,wIndex,0L);

      RegCloseKey(hkResult);
      }
   else
      {
      dwRet=FALSE;
      MSGBOX("Can't open key",(LPSTR)szConfigDialog);
      }

   return dwRet;
   }


void DisplayFriendlyName(LPSTR lpKey)
   {
   HKEY hkResult;
   DWORD dwRet=TRUE;

   if(RegOpenKey(HKEY_LOCAL_MACHINE,(LPSTR)gpVpenDKey,&hkResult) == ERROR_SUCCESS)
      {
      HKEY hkFriendly;
      if(RegOpenKey(hkResult,lpKey,&hkFriendly) == ERROR_SUCCESS)
         {
         // The sub key is open. Look for "FriendlyName."

         DWORD dwType;
         char szFriendly[256];
         DWORD dwfSize=256;

         if( RegQueryValueEx(hkFriendly,gpFriendlyName,
                  0,&dwType,(LPBYTE)&szFriendly,
                  (LPDWORD)&dwfSize) == ERROR_SUCCESS)
            {
            SetDlgItemText(ghMainDlg,IDC_FRIENDLYNAME,(LPSTR)szFriendly);
            }

         RegCloseKey(hkFriendly);
         }
      else
         {
         dwRet=FALSE;
         MSGBOX("Can't open registry key",(LPSTR)szConfigDialog);
         }

      RegCloseKey(hkResult);
      }
   else
      {
      dwRet=FALSE;
      MSGBOX("Can't open key",(LPSTR)szConfigDialog);
      }
   }


BOOL InitStrings(void)
   {
   LoadString(ghInst, rsNoTabletHardware, gpNoTabletHardware, cbSzRcMax);
   LoadString(ghInst, rsComErr, szComErr, cbSzRcMax);
   LoadString(ghInst, rsModelErr, szModelErr, cbSzRcMax);
   LoadString(ghInst, rsConfigDialog, szConfigDialog, cbSzRcMax);
   LoadString(ghInst, rsReboot, szReboot, cbSzRcMax);
   return TRUE;
   }

// End-Of_File
