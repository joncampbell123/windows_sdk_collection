/*---------------------------------------------------------------------------*\
| WINDOWS DIALOG INTERFACE MODULE                                             |
|   This module contains the routines necessary for handling the modeless     |
|   dialog box which acts as the interface for the application.  All routines |
|   which the interface (DialogBox) uses, are contained in this module.       |
|                                                                             |
| DATE   : July 01, 1989                                                      |
| Copyright 1989-1992 by Microsoft Corporation                                |
\*---------------------------------------------------------------------------*/

#include <windows.h>
#include "PrntTest.h"

#define LARGE_BUFFER_SIZE   512
#define STRING_BUFFER_SIZE  180

/*---------------------------------------------------------------------------*\
| UPDATE SELECTION CHANGE                                                     |
|   This routine updates the edit control boxes to reflect the current string |
|   selection in the LIST ListBox.                                            |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   HWND hwndDialog - The dialog window handle.                                     |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   TRUE if all went successful.                                              |
\*---------------------------------------------------------------------------*/
BOOL UpdateSelectionChange(HWND hwndDialog)
{
  unsigned  uSize, uIndex;
  HANDLE    hBuffer;
  PSTR      pProfile, pName, pDriver, pPort;

  uIndex = (unsigned) SendDlgItemMessage(hwndDialog, IDD_INTRFACE_LIST,
                                         LB_GETCURSEL, NULL, 0L);

  uSize = (unsigned) SendDlgItemMessage(hwndDialog, IDD_INTRFACE_LIST,
                                        LB_GETTEXTLEN, uIndex, 0L);
 
  if (!(hBuffer = LocalAlloc(LHND, ++uSize)) ||
      !(pProfile = LocalLock(hBuffer)))
  {
    if    (hBuffer)
      LocalFree(hBuffer);

    return FALSE;
  }

  SendDlgItemMessage(hwndDialog, IDD_INTRFACE_LIST, LB_GETTEXT, uIndex,
                    (LONG) (LPSTR) pProfile);

  for (pName = pProfile; *pName; pName++)
    if (*pName == ':')
    {
      *pName++ = '\0';
      break;
    }

  for   (pDriver = pName; *pDriver; pDriver++)
    if  (*pDriver == ',')
    {
      *pDriver++ = '\0';
      break;
    }

  for   (pPort = pDriver; *pPort; pPort++)
    if  (*pPort == ',')
    {
      *pPort++ = '\0';
      break;
    }

  SetDlgItemText(hwndDialog, IDD_INTRFACE_PROF, pProfile);
  SetDlgItemText(hwndDialog, IDD_INTRFACE_NAME, pName);
  SetDlgItemText(hwndDialog, IDD_INTRFACE_DRIV, pDriver);
  SetDlgItemText(hwndDialog, IDD_INTRFACE_PORT, pPort);
  SendDlgItemMessage(hwndDialog, IDD_INTRFACE_TEST, LB_SELECTSTRING, -1,
                    (LONG) lstrcat(pProfile,":"));

  SetFocus(GetDlgItem(hwndDialog, IDD_INTRFACE_LIST));

  LocalUnlock(hBuffer);
  LocalFree(hBuffer);

  return    TRUE;
}

/*---------------------------------------------------------------------------*\
| INITIALIZE INTERFACE BOXES (variables)                                      |
|   This routine looks reads in the information contained in the PRNTTEST.INI |
|   file to update the LIST and TEST listboxes.                               |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   HWND hwndDialog - The dialog window handle.                                     |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   TRUE if all went successful.                                              |
\*---------------------------------------------------------------------------*/
BOOL InitializeInterface(HWND hwndDialog)
{
  HANDLE    hBuffer = NULL, hProfiles = NULL, hString = NULL;
  LPSTR     lpBuffer = NULL, lpProfiles = NULL, lpString = NULL, lpProfile;
  int       nCount, idx;
  static struct
    {
      char  szListName[15];
      WORD  wListID;
    }       sListBoxes[] = {{"ProfilesList", IDD_INTRFACE_LIST},
                            {"ProfilesTest", IDD_INTRFACE_TEST}};


  /*-----------------------------------------*\
  | Allocate buffers needed for initializing  |
  | the dialog box.                           |
  \*-----------------------------------------*/
  if (!(hProfiles = LocalAlloc(LHND, LARGE_BUFFER_SIZE)) ||
      !(hBuffer = LocalAlloc(LHND, STRING_BUFFER_SIZE)) ||
      !(hString = LocalAlloc(LHND, STRING_BUFFER_SIZE)) ||
      !(lpProfiles = (LPSTR) LocalLock(hProfiles)) ||
      !(lpBuffer = (LPSTR) LocalLock(hBuffer)) ||
      !(lpString = (LPSTR)LocalLock(hString)))
  {
    if (lpProfiles)
      LocalUnlock(hProfiles);

    if (hProfiles)
      LocalFree(hProfiles);

    if (lpBuffer)
      LocalUnlock(hBuffer);

    if (hBuffer)
      LocalFree(hBuffer);

    if (hString)
      LocalFree(hString);

    return FALSE;
  }

  /*-----------------------------------------*\
  | Get ALL strings from the file sections,   |
  | and add to appropriate listboxes.         |
  \*-----------------------------------------*/
  for   (idx = 0; idx < sizeof(sListBoxes) / sizeof(sListBoxes[0]); idx++)
  {
    nCount = GetPrivateProfileString(sListBoxes[idx].szListName, NULL, "",
                                     lpProfiles, LARGE_BUFFER_SIZE,
                                     strApplicationProfile);
    if (nCount <= 0)
    {
      LocalUnlock(hProfiles);
      LocalFree(hProfiles);
      LocalUnlock(hBuffer);
      LocalFree(hBuffer);
      LocalUnlock(hString);
      LocalFree(hString);
      return    FALSE;
    }

    for (lpProfile = lpProfiles;
         *lpProfile;
         lpProfile += 1 + lstrlen(lpProfile))
    {
      GetPrivateProfileString(sListBoxes[idx].szListName, lpProfile, "",
                              lpString, STRING_BUFFER_SIZE,
                              strApplicationProfile);

      SendDlgItemMessage(hwndDialog,sListBoxes[idx].wListID, LB_ADDSTRING,
                         NULL, (LONG) lstrcat(lstrcat(lstrcpy(lpBuffer,
                                                              lpProfile),
                                              ":"), lpString));
    }
  }

  /*-----------------------------------------*\
  | Free up buffers.                          |
  \*-----------------------------------------*/
  LocalUnlock(hProfiles);
  LocalFree(hProfiles);
  LocalUnlock(hBuffer);
  LocalFree(hBuffer);
  LocalUnlock(hString);
  LocalFree(hString);

  /*-----------------------------------------*\
  | Update editboxes to reflect current sel.  |
  \*-----------------------------------------*/
  SendDlgItemMessage(hwndDialog, IDD_INTRFACE_LIST, LB_SETCURSEL, 0, 0l);
  UpdateSelectionChange(hwndDialog);

  return    TRUE;
}

/******************************************************************************

    Private Function:   Complain

    Purpose:            Reports errors

    Change History:

    02-16-1991  Use MessageBox, for now.

******************************************************************************/

void Complain(PSTR pstrComplaint)
{
  MessageBox(NULL, pstrComplaint, NULL, MB_ICONSTOP | MB_OK);
}

/******************************************************************************

    Private Function:   BuildProfileString

    Purpose:            Builds a string from the edit controls in the
                        internal format used in this app.

    Change History:

    02-16-1991  Time for some quality code

******************************************************************************/

PSTR BuildProfileString(HWND hwndDialog)
{
  static unsigned   uStringLength = 0;
  static HANDLE     hBuffer;
  static struct
    {
      WORD  wDlgID;
      PSTR  pstrDescription;
      PSTR  pstrSuffix;
    }               asItems[] = {{IDD_INTRFACE_PROF, "Profile Name", ":"},
                               {IDD_INTRFACE_NAME, "Printer Name", ","},
                               {IDD_INTRFACE_DRIV, "Driver Name", ","},
                               {IDD_INTRFACE_PORT, "Port or File Name", ""}};

  PSTR              pstrBuild, pstrChunk;
  unsigned          uItemSize, uMaximumSize = 0, uSizeNeeded = 0, uItem;
  HANDLE            hChunk;

  /*
    Determine the maximum item size, and the total string size.
  */

  for (uItem = 0; uItem < sizeof(asItems) / sizeof(asItems[0]); uItem++)
  {
    uItemSize = (unsigned) SendDlgItemMessage(hwndDialog,
                                              asItems[uItem].wDlgID,
                                              EM_LINELENGTH, 0, 0L);
    if (!uItemSize++)
    {
      /*
        No entry for item.  complain!
      */

      char  acBuffer[80];

      wsprintf(acBuffer, "No entry was made for %s!",
              (LPSTR) asItems[uItem].pstrDescription);
      Complain(acBuffer);
    }

    uMaximumSize = max(uMaximumSize, uItemSize);
    uSizeNeeded += uItemSize;
  }

  if    (uSizeNeeded > uStringLength)
  {
    if    (uStringLength)
    {
      LocalUnlock(hBuffer);
      LocalFree(hBuffer);
    }

    if    (!(hBuffer = LocalAlloc(LHND, uSizeNeeded)))
    {
      /*
        Couldn't allocate buffer- we're dead!
      */

      Complain("Couldn't allocate profile string buffer (Intrface.C)");

      uStringLength = 0;
      return    NULL;

    }

    uStringLength = uSizeNeeded;
  }

  if (!(hChunk = LocalAlloc(LHND, uMaximumSize)) ||
      !(pstrChunk = (PSTR) LocalLock(hChunk)))
  {
    /*
      Couldn't allocate helper buffer.
    */

    Complain("Couldn't allocate helper buffer (Intrface.C)");

    if (hChunk)
      LocalFree(hChunk);

    return NULL;

  }

  if (!(pstrBuild = LocalLock(hBuffer)))
  {
    /*
      Couldn't lock build string into place.
    */

    Complain("Couldn't lock build string (Intrface.C)");

    LocalUnlock(hChunk);
    LocalFree(hChunk);

    return NULL;

  }

  /*
    OK, everything's set up!  Build the string, lickety split!
  */

  for (uItem = 0, *pstrBuild = '\0';
       uItem < sizeof(asItems) / sizeof(asItems[0]);
       uItem++)
  {
    GetDlgItemText(hwndDialog, asItems[uItem].wDlgID, pstrChunk,
          uMaximumSize);

    if    (asItems[uItem].wDlgID == IDD_INTRFACE_DRIV)
    {
      PSTR  pstr;

      for   (pstr = pstrChunk; *pstr; *pstr++)
        if  (*pstr == '.')
        {
          *pstr = '\0';
          break;
        }

      AnsiUpper(pstrChunk);
      SetDlgItemText(hwndDialog, IDD_INTRFACE_DRIV, pstrChunk);
    }

    lstrcat(lstrcat(pstrBuild, pstrChunk), asItems[uItem].pstrSuffix);
  }

  LocalUnlock(hChunk);
  LocalFree(hChunk);

  return    pstrBuild;
}

/*---------------------------------------------------------------------------*\
| MODIFY PROFILE SELECTION                                                    |
|   This routine performs the modification of the currently selected profile  |
|   in the LIST ListBox.  If the profile indicated in the Edit box already    |
|   exist in the LIST, then no modification is performed.  If the modification|
|   is allowed, then both the LIST and TEST Listboxes are updated.            |
|                                                                             |
|   ALGORITHM                                                                 |
|     1. Get string from Profile EditBox.                                     |
|     2. Search LIST ListBox for Match                                        |
|        if(Match)                                                            |
|             MessageBox - Can't perform update, since it already exist.      |
|        else                                                                 |
|             Update the LIST and TEST listboxes with changes.                |
|     3. Set Selection to new modification string.                            |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   HWND hwndDialog - The dialog window handle.                                     |
|                                                                             |
| GLOBAL VARIABLES                                                           |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   TRUE if all went successful.                                              |
\*---------------------------------------------------------------------------*/
BOOL ModifyProfiles(HWND hwndDialog)
{
  HANDLE    hProfile;
  LPSTR     lpBuffer, lpProfile;
  PSTR      pstrBuffer;
  unsigned  uProfileLength, uIndex;

  /*-----------------------------------------*\
  | Retrieve the profile from the Profile     |
  | EditBox.  Use buffer in local heap.       |
  \*-----------------------------------------*/
  uProfileLength  = 2 + (unsigned) SendDlgItemMessage(hwndDialog,
                          IDD_INTRFACE_PROF, EM_LINELENGTH, 0, 0l);

  if (!(hProfile  = LocalAlloc(LHND, uProfileLength)) ||
      !(lpProfile = (LPSTR) LocalLock(hProfile)))
  {
    if    (hProfile)
      LocalFree(hProfile);

    return    FALSE;
  }
  GetDlgItemText(hwndDialog, IDD_INTRFACE_PROF, lpProfile, uProfileLength);
  lstrcat(lpProfile, ":");

  if ((uIndex = (unsigned) SendDlgItemMessage(hwndDialog, IDD_INTRFACE_LIST,
                                              LB_FINDSTRING, -1,
                                              (LONG) lpProfile)) == LB_ERR)
  {
    Complain("The profile you wish to modify does not exist!");
    LocalUnlock(hProfile);
    LocalFree(hProfile);
    return    TRUE;
  }

  /*-----------------------------------------*\
  | Create new string with modified strings   |
  | retrieved from edit boxes.                |
  \*-----------------------------------------*/
  pstrBuffer = BuildProfileString(hwndDialog);
  if (!pstrBuffer)
  {
    LocalUnlock(hProfile);
    LocalFree(hProfile);
    return    FALSE;
  }

  lpBuffer = (LPSTR) pstrBuffer;

  SendDlgItemMessage(hwndDialog, IDD_INTRFACE_LIST, LB_DELETESTRING, uIndex,
                     NULL);

  SendDlgItemMessage(hwndDialog, IDD_INTRFACE_LIST, LB_INSERTSTRING, uIndex,
                    (LONG) lpBuffer);

  lpProfile[lstrlen(lpProfile) - 1] = '\0';

  WritePrivateProfileString("ProfilesList", lpProfile,
                            lpBuffer + lstrlen(lpProfile) + 1,
                            strApplicationProfile);

  WritePrivateProfileString("Windows", "Device",
                            lpBuffer + lstrlen(lpProfile) + 1, lpProfile);

  /*
    Fix up the test box, too- if you have to!
  */

  if ((uIndex = (unsigned) SendDlgItemMessage(hwndDialog, IDD_INTRFACE_TEST,
                                              LB_FINDSTRING, -1,
                                              (LONG) lpProfile)) != LB_ERR)
  {

    SendDlgItemMessage(hwndDialog, IDD_INTRFACE_TEST, LB_DELETESTRING,
                       uIndex, NULL);

    SendDlgItemMessage(hwndDialog, IDD_INTRFACE_TEST, LB_INSERTSTRING,
                       uIndex, (LONG) lpBuffer);

    WritePrivateProfileString("ProfilesTest", lpProfile,
                              lpBuffer + lstrlen(lpProfile) + 1,
                              strApplicationProfile);
  }

  SendDlgItemMessage(hwndDialog, IDD_INTRFACE_LIST, LB_SETCURSEL, uIndex, 0L);
  UpdateSelectionChange(hwndDialog);
  LocalUnlock(hProfile);
  LocalFree(hProfile);

  return    TRUE;
}

/*---------------------------------------------------------------------------*\
| REMOVE PROFILE FROM LISTBOXES.                                              |
|   This routine removes the Selected profile from the test or list depending |
|   on whether the profile is included in the appropriate listbox.            |
|                                                                             |
| ALGORITHM                                                                   |
|   Get Profile to remove.                                                    |
|   Look in TEST Box to remove.                                               |
|     Exist?                                                                  |
|       Remove from TEST Box and profile.                                     |
|     Not Exist?                                                              |
|       Look in LIST Box to remove.                                           |
|         Exist?                                                              |
|           Remove from LIST Box and profile.                                 |
|         Not Exist?                                                          |
|           Prompt the profile doesn't exist.                                 |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   HWND hwndDialog - The dialog window handle.                                     |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   TRUE if all went successful.                                              |
\*---------------------------------------------------------------------------*/
BOOL RemoveProfiles(HWND hwndDialog)
{
  HANDLE    hProfile;
  LPSTR     lpProfile;
  int       nProfile;
  unsigned  uIndex;
  OFSTRUCT  of;

  /*-----------------------------------------*\
  | Allocate buffers for use.                 |
  \*-----------------------------------------*/
  nProfile = 2 + (int) SendDlgItemMessage(hwndDialog, IDD_INTRFACE_PROF,
        EM_LINELENGTH, 0, 0L);
  if (!(hProfile = LocalAlloc(LHND, nProfile)) ||
      !(lpProfile = (LPSTR)LocalLock(hProfile)))
  {
    if (hProfile)
      LocalFree(hProfile);

    return FALSE;
  }

  GetDlgItemText(hwndDialog, IDD_INTRFACE_PROF, lpProfile, nProfile);
  lstrcat(lpProfile, ":");

  /*-----------------------------------------*\
  | Get the profile to remove.  Then look for |
  | it in the TEST Listbox.                   |
  \*-----------------------------------------*/
  if ((uIndex = (unsigned) SendDlgItemMessage(hwndDialog, IDD_INTRFACE_TEST,
                                              LB_FINDSTRING, -1,
                                              (LONG) lpProfile)) != LB_ERR)
  {
    SendDlgItemMessage(hwndDialog, IDD_INTRFACE_TEST, LB_DELETESTRING,
                       uIndex, 0L);

    lpProfile[lstrlen(lpProfile) - 1] = '\0';

    WritePrivateProfileString("ProfilesTest", lpProfile, NULL,
                              strApplicationProfile);
    LocalUnlock(hProfile);
    LocalFree(hProfile);

    return    TRUE;
  }

  /*-----------------------------------------*\
  | Look for profile in the LIST listbox.     |
  | This is done to delete it from the tests  |
  | altogether.                               |
  \*-----------------------------------------*/
  if ((uIndex = (unsigned) SendDlgItemMessage(hwndDialog, IDD_INTRFACE_LIST,
                                              LB_FINDSTRING, -1,
                                              (LONG) lpProfile)) != LB_ERR)
  {
    if (MessageBox(GetParent(hwndDialog),
                   "Do you wish to remove from List of profiles",
                   "Warning!  About to remove profile!",
                   MB_ICONQUESTION | MB_YESNO) == IDYES)
    {
      SendDlgItemMessage(hwndDialog, IDD_INTRFACE_LIST, LB_DELETESTRING,
                         uIndex, 0L);

      lpProfile[lstrlen(lpProfile) - 1] = '\0';

      WritePrivateProfileString("ProfilesList", lpProfile, NULL,
                                strApplicationProfile);

      SendDlgItemMessage(hwndDialog, IDD_INTRFACE_LIST, LB_SETCURSEL, 0, 0L);
      UpdateSelectionChange(hwndDialog);

      OpenFile(lpProfile, &of, OF_DELETE);
    }
    SetFocus(GetDlgItem(hwndDialog, IDD_INTRFACE_LIST));
    LocalUnlock(hProfile);
    LocalFree(hProfile);
    return    TRUE;
  }

  /*-----------------------------------------*\
  | If the previous two checks didn't find the|
  | the profile, then profile doesn't exist.  |
  \*-----------------------------------------*/
  Complain("Profile Does not exist");
  LocalUnlock(hProfile);
  LocalFree(hProfile);

  return    TRUE;
}

/*---------------------------------------------------------------------------*\
| ADD PROFILE TO LISTBOXES                                                    |
|   This routine adds the selected profile from the edit box to the list or   |
|   test boxes, depending on the existence of the profile.                    |
|                                                                             |
| ALGORITHM                                                                   |
|   Get the profile to add.                                                   |
|     Search for profile in the TEST listbox.                                 |
|       Found?                                                                |
|         - prompt messagebox indicating it's been added.                     |
|       Not Found?                                                            |
|         Search for profile in the LIST listbox.                             |
|           Found?                                                            |
|             - Add it to TEST listbox.                                       |
|           Not Found?                                                        |
|             - Add it to LIST listbox.                                       |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   HWND hwndDialog - The dialog window handle.                                     |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   TRUE if all went successful.                                              |
\*---------------------------------------------------------------------------*/
BOOL AddProfiles(HWND hwndDialog)
{
  HANDLE    hProfile;
  PSTR      pstrBuffer;
  LPSTR     lpBuffer, lpProfile;
  int       nProfile;

  pstrBuffer = BuildProfileString(hwndDialog);
  if (!pstrBuffer)
    return  FALSE;

  lpBuffer = (LPSTR) pstrBuffer;

  /*-----------------------------------------*\
  | Allocate buffer for profile string.  Use  |
  | the local heap for storage.               |
  \*-----------------------------------------*/
  nProfile = 2 + (int)SendDlgItemMessage(hwndDialog, IDD_INTRFACE_PROF,
                                         EM_LINELENGTH, 0, 0L);
  if (!(hProfile = LocalAlloc(LHND,(WORD) nProfile)))
    return  FALSE;

  /*-----------------------------------------*\
  | Lock buffers for use.                     |
  \*-----------------------------------------*/
  if (!(lpProfile = (LPSTR)LocalLock(hProfile)))
  {
    LocalFree(hProfile);
    return    FALSE;
  }

  /*-----------------------------------------*\
  | Look for profile in the TEST listbox. If  |
  | it exists, then prompt user with Message. |
  | If the profile is not found, then drop    |
  | down for next test.                       |
  \*-----------------------------------------*/
  GetDlgItemText(hwndDialog, IDD_INTRFACE_PROF, lpProfile, nProfile);
  lstrcat(lpProfile, ":");

  if (SendDlgItemMessage(hwndDialog, IDD_INTRFACE_TEST, LB_FINDSTRING, -1,
                        (LONG) lpProfile) != LB_ERR)
  {
    Complain("Test already added");
    SetFocus(GetDlgItem(hwndDialog, IDD_INTRFACE_LIST));
    LocalUnlock(hProfile);
    LocalFree(hProfile);
    return    TRUE;
  }

  /*-----------------------------------------*\
  | Look for profile in the LIST listbox.  If |
  | it exists, then add the profile and other |
  | strings to the TEST listbox.  If it does  |
  | not exist, then drop down for next test.  |
  \*-----------------------------------------*/
  if (SendDlgItemMessage(hwndDialog, IDD_INTRFACE_LIST, LB_FINDSTRING, -1,
                        (LONG) lpProfile) != LB_ERR)
  {

    SendDlgItemMessage(hwndDialog, IDD_INTRFACE_TEST, LB_ADDSTRING, NULL,
                      (LONG) lpBuffer);
    lpProfile[lstrlen(lpProfile) - 1] = '\0';
    WritePrivateProfileString("ProfilesTest", lpProfile,
                              lpBuffer + lstrlen(lpProfile) + 1,
                              strApplicationProfile);
    SetFocus(GetDlgItem(hwndDialog, IDD_INTRFACE_LIST));
    LocalUnlock(hProfile);
    LocalFree(hProfile);
    UpdateSelectionChange(hwndDialog);
    return    TRUE;

  }

  /*-----------------------------------------*\
  | If the previous searches failed, then the |
  | string has not been added to the tests.   |
  | First we will add the strings to the test.|
  | Then we will drop down for the existence  |
  | of the profile.                           |
  |                                           |
  | Add the string to the listbox, then add   |
  | to the prnttest.ini profile.              |
  \*-----------------------------------------*/
  SendDlgItemMessage(hwndDialog, IDD_INTRFACE_LIST, LB_ADDSTRING, NULL,
                    (LONG) lpBuffer);

  SendDlgItemMessage(hwndDialog, IDD_INTRFACE_LIST, LB_SELECTSTRING, NULL,
                    (LONG) lpBuffer);

  lpProfile[lstrlen(lpProfile) - 1] = '\0';

  WritePrivateProfileString((LPSTR)"ProfilesList", lpProfile,
                            lpBuffer + lstrlen(lpProfile) + 1,
                            strApplicationProfile);

  WritePrivateProfileString("Windows", "Device",
                            lpBuffer + lstrlen(lpProfile) + 1, lpProfile);

  /*-----------------------------------------*\
  | Free up the buffers used for building the |
  | device string.  THAT BE IT!!!             |
  \*-----------------------------------------*/
  LocalUnlock(hProfile);
  LocalFree(hProfile);

  UpdateSelectionChange(hwndDialog);

  return    TRUE;
}

/*---------------------------------------------------------------------------*\
| SETUP PRINTER                                                               |
|   This routine calls the printer drivers EXTDEVICEMODE routine to display   |
|   the Setup DialogBox and set the printer profiles devmode section.         |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   HWND hwndDialog - Handle to the Modeless Dialogbox interface.                   |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   TRUE if all went successful.                                              |
\*---------------------------------------------------------------------------*/
BOOL SetupPrinter(HWND hwndDialog)
{
  LPFNDEVMODE   lpfnExtDeviceMode;
  char          szModule[25],szName[64],szDriver[25],szPort[25],szProfile[25];
  HANDLE        hLibrary;

  /*-----------------------------------------*\
  | Retrieve the text from the selected edit  |
  | boxes.                                    |
  \*-----------------------------------------*/
  GetDlgItemText(hwndDialog,IDD_INTRFACE_DRIV,(LPSTR)szDriver,sizeof(szDriver));
  GetDlgItemText(hwndDialog,IDD_INTRFACE_PORT,(LPSTR)szPort,sizeof(szPort));
  GetDlgItemText(hwndDialog,IDD_INTRFACE_NAME,(LPSTR)szName,sizeof(szName));
  GetDlgItemText(hwndDialog,IDD_INTRFACE_PROF,(LPSTR)szProfile,sizeof(szProfile));
  lstrcpy((LPSTR)szModule,szDriver);
  lstrcat((LPSTR)szModule,(LPSTR)".DRV");

  /*-----------------------------------------*\
  | Load the printer library, and retreive    |
  | the xxxDeviceMode from the library.       |
  \*-----------------------------------------*/
  if ((hLibrary = LoadLibrary(szModule)) < 32)
    return  FALSE;

  if (!(lpfnExtDeviceMode = GetProcAddress(hLibrary, "ExtDeviceMode")))
  {
    int (FAR PASCAL *lpfnDeviceMode)(HWND, HANDLE, LPSTR, LPSTR);

    if (!(lpfnDeviceMode = GetProcAddress(hLibrary, "DeviceMode")))
    {
      FreeLibrary(hLibrary);
      return    FALSE;
    }

    (*lpfnDeviceMode)(hwndDialog, hLibrary, szName, szPort);

  }
  else

    /*-----------------------------------------*\
    | Call ExtDeviceMode to do the honors.  For |
    | this operation, we do not need a DEVMODE. |
    \*-----------------------------------------*/

    (*lpfnExtDeviceMode)(hwndDialog, hLibrary, NULL, szName, szPort, NULL,
                         szProfile, DM_IN_PROMPT | DM_OUT_DEFAULT);

  FreeLibrary(hLibrary);
  return    TRUE;
}

/*---------------------------------------------------------------------------*\
| INTERFACE DIALOG PROCEDURE                                                  |
|   This is the main interface module for the application.  It provides the   |
|   control for the Modeless DialogBox created and displayed throughout the   |
|   existence of the application.  The interface consists of the following:   |
|                                                                             |
|      2 ListBoxes   - one contains a list of the printer device lines which  |
|                      the user can select to test.  The other contains a list|
|                      of printer device lines which are to be tested.        |
|      4 PushButtons - These boxes allow the user to ADD, REMOVE, MODIFY and  |
|                      setup the printer device lines.                        |
|      4 EditBoxes   - These allow the user to view and change the text       |
|                      content of the printer device lines.                   |
|      1 Static Text - This box displays description text depending upon the  |
|                      currently selected box or button in the dialog box.    |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   HWND     hwndDialog     - The Window Handle.                                    |
|   unsigned iMessage - Message to be processed.                              |
|   WORD     wParam   - Information associated with message.                  |
|   LONG     lParam   - Information associated with message.                  |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   TRUE if successful.                                                       |
\*---------------------------------------------------------------------------*/
BOOL FAR PASCAL PrntTestDlg(HWND     hwndDialog,
                            unsigned iMessage,
                            WORD     wParam,
                            LONG     lParam)
{
  char szBuffer[80];

  switch    (iMessage)
  {
    case  WM_INITDIALOG:
      InitializeInterface(hwndDialog);
      break;

    case  WM_COMMAND:
      switch  (wParam)
      {
        case IDD_INTRFACE_ADD:
          if (!AddProfiles(hwndDialog))
            MessageBox(hwndDialog, "Adding profiles (intrface.c)",
                       "Assertion", MB_OK);
          break;

        case IDD_INTRFACE_REM:
          if (!RemoveProfiles(hwndDialog))
            MessageBox(hwndDialog, "Removing profiles (intrface.c)",
              "Assertion", MB_OK);
          break;

        case IDD_INTRFACE_SET:
          if (!SetupPrinter(hwndDialog))
            MessageBox(hwndDialog, "Setting profiles (intrface.c)",
                       "Assertion", MB_OK);
          break;

        case IDD_INTRFACE_MOD:
          if (!ModifyProfiles(hwndDialog))
            MessageBox(hwndDialog, "Modifying profiles (intrface.c)",
                       "Assertion", MB_OK);
          break;

        /*--------------------------*\
        | Never let the test listbox |
        | show a selection.          |
        \*--------------------------*/
        case IDD_INTRFACE_TEST:
          if (HIWORD(lParam) != LBN_SELCHANGE)
            return  TRUE;

          SendDlgItemMessage(hwndDialog, IDD_INTRFACE_TEST, LB_SETCURSEL,
                             -1, 0l);
          SetFocus(GetDlgItem(hwndDialog, IDD_INTRFACE_LIST));
          UpdateSelectionChange(hwndDialog);
          break;

        /*--------------------------*\
        | If the listbox selection   |
        | changes, then the edit ctrl|
        | boxes should be updated.   |
        \*--------------------------*/
        case    IDD_INTRFACE_LIST:
          if    (HIWORD(lParam) != LBN_SELCHANGE)
            return  TRUE;

          UpdateSelectionChange(hwndDialog);
          break;

        /*--------------------------*\
        | If the edit boxes get the  |
        | input focus, then output   |
        | the text description to    |
        | the status box.            |
        \*--------------------------*/
        case    IDD_INTRFACE_PROF:
        case    IDD_INTRFACE_NAME:
        case    IDD_INTRFACE_DRIV:
        case    IDD_INTRFACE_PORT:

          switch    (HIWORD(lParam))
          {
            case  EN_SETFOCUS:
              LoadString(hInst, IDS_INTRFACE_PROF +
                        (wParam - IDD_INTRFACE_PROF), szBuffer,
                        sizeof(szBuffer));
              SetDlgItemText(hwndDialog, IDD_INTRFACE_TXT, szBuffer);
              break;

            case  EN_KILLFOCUS:
              SetDlgItemText(hwndDialog, IDD_INTRFACE_TXT, "");
              break;
          }
          break;

        default:
          return    FALSE;
      }
      break;

    default:
      return  FALSE;
  }

  return    TRUE;
}
