/***************************************************************************
 * devinstl.c -
 *
 * Copyright (C) 1991 Hewlett-Packard Company.
 * All rights reserved.  Company confidential.
 *
 * Contains code for the DDI DevInstall()
 *
 * History:
 *  05 nov 91  SD:  BUG #592:  Old TD incompatibility.  Somehow this part
 *                  of the fix didn't get archived!
 *  13 seo 91  SD:  BUG #655:  increased size of message for localization.
 *  13 aug 91  SD:  Moved warning message string to the resource file.
 *  27 jun 91  SD:  Created.
 *
 ****************************************************************************/
#include "build.h"
#include "windows.h"
#include "lclstr.h"

#if defined(WIN31)
#include "strings.h"

extern void FAR PASCAL MakeAppName(LPSTR, LPSTR, LPSTR, int);
extern HANDLE hLibInst;

#define LOCAL static
#define NAME_LEN 25

/* Forward references */
LOCAL void MoveWinIniEntries(LPSTR,LPSTR);
LOCAL void SendFontWarning(HWND,LPSTR);

/***************************************************************************
 * short FAR PASCAL DevInstall(HWND hWnd, LPSTR lpModelName,
 *                             LPSTR lpOldPort, LPSTR lpNewPort)
 *
 * Function:  DDI used to install and remove printers.
 *
 *         If OldPort == NULL and NewPort == NULL, return.
 *
 *         If OldPort and NewPort are both non-NULL, change the port in
 *         [ModelName,OldPort] to NewPort. Check the [driver,OldPort]
 *         section and if there are fonts in it, warn the user.
 *
 *         If OldPort is NULL, install the new printer model.  If there is
 *         a [driver,NewPort] section and the printer index matches the
 *         ModelName, move all non-font related information from [driver,
 *         NewPort] to [ModelName,NewPort]. The [driver,NewPort] section
 *         will then contain only the font information.  If there is no
 *         [driver, NewPort] section, do nothing.
 *
 *         If NewPort is NULL, remove the old printer model.  Remove the
 *         [ModelName,OldPort] section and all settings.
 *
 *         
 *
 * Parameters:
 *      hWnd:           Handle to the parent window.
 *      lpModelName:    Pointer to string containing name of the printer model.
 *      lpOldPort:      Pointer to name of old port.
 *      lpNewPort:      Pointer to name of new port.
 *
 * Returns:
 *      1 if successful;
 *      0 if not supported;
 *      -1 if failed;
 *
 ****************************************************************************/
short FAR PASCAL DevInstall(hWnd, lpModelName, lpOldPort, lpNewPort)
HWND hWnd;
LPSTR lpModelName;
LPSTR lpOldPort;
LPSTR lpNewPort;
{
    char oldAppName[25];
    char newAppName[25];
    char keyName[16];
    char datastr[NAME_LEN];
    short prtIndex;

    if (!lpOldPort && !lpNewPort)
        return(-1);
    if (!lpOldPort) /* Install new printer model */
    {
        /*  Scan win.ini for [<driver name>,lpNewPort] and read the printer
         *  index.  Compare the ModelName with the printer name in the
         *  printer index information string.
         */
        MakeAppName(ModuleNameStr, lpNewPort, oldAppName, sizeof(oldAppName));
        keyName[0] = '\0';
        LoadString(hLibInst,WININI_PRTINDEX,(LPSTR)keyName,sizeof(keyName));
        prtIndex = GetProfileInt(oldAppName, keyName, -1);
        if (LoadString(hLibInst, DEVNAME_BASE + prtIndex,
                   (LPSTR)datastr, NAME_LEN)) 
        {
            /*  Parse printer name from printer information string */
            char sepchar;
            LPSTR bufptr;
            char devname[NAME_LEN];
            LPSTR lpDevname;
            int i;

            lpDevname = (LPSTR)devname;
            bufptr = (LPSTR)datastr;
            sepchar = *bufptr++;   /* First char in string is separator character */
            for (i = 0; *bufptr && (*bufptr != sepchar); i++)
            {
                if (i < NAME_LEN - 1)
                *lpDevname++ = *bufptr++;
            }
            *lpDevname = '\0';

            /* Compare name from printer index string to ModelName */
            if (!lstrcmpi(lpModelName, (LPSTR)devname))
            {
                /* ModelName matches printer index. Create new section
                * [ModelName,NewPort] and move all the non-font info
                * into it.
                */
                MakeAppName(lpModelName, lpNewPort, newAppName, sizeof(newAppName));
                MoveWinIniEntries(oldAppName,newAppName);            
            }
         } /* end if LoadString() */
    } /* end if !lpOldPort */
    else if (!lpNewPort)  /* Remove old printer model section*/
    {
        MakeAppName(lpModelName, lpOldPort, oldAppName, sizeof(oldAppName));
        WriteProfileString(oldAppName,NULL,NULL);
    }
    else /* Change port in [ModelName,OldPort] to NewPort*/
    {
        MakeAppName(lpModelName,lpNewPort,newAppName,sizeof(newAppName));
        MakeAppName(lpModelName,lpOldPort,oldAppName,sizeof(oldAppName));
        MoveWinIniEntries(oldAppName,newAppName);
        WriteProfileString(oldAppName,NULL,NULL);
        SendFontWarning(hWnd,lpOldPort);
    } 
    return(1);
}

/************************************************************************
 *  LOCAL void MoveWinIniEntries(lpOldAppName,lpNewAppName)
 *
 *  Function:   Moves non-fsvers entries from OldAppName section to NewAppName
 *              section in the win.ini file.
 *
 *  Parameters:
 *      lpOldAppName -  pointer to string containing section header to
 *                      to be copied from.
 *      lpNewAppName -  pointer to string containing section header to
 *                      be copied to.
 *
 *************************************************************************/
LOCAL void MoveWinIniEntries(lpOldAppName,lpNewAppName)
LPSTR lpOldAppName;
LPSTR lpNewAppName;
{
    short ind;
    short strlen;
    char keyName[16];
    char datastr[16];


    for (ind = WININI_BASE; ind < WININI_LAST; ++ind)
    {
       /*  Load key name of item from resources and get its
        *  corresponding info field from win.ini.  Copy it
        *  into new section and then remove it from the old section.
        *  font info is the exception;  It stays in [driver,port]
        */
        switch (ind)
        {
            case WININI_FSVERS:
            case WININI_CARTINDEX:
            case WININI_CARTINDEX1:
            case WININI_CARTINDEX2:
            case WININI_CARTINDEX3:
            case WININI_CARTINDEX4:
            case WININI_CARTINDEX5:
            case WININI_CARTINDEX6:
            case WININI_CARTINDEX7:
                break;
            case WININI_PRTCAPS2:
                /* BUG # 592: Copy prtcaps2 into new section, but do not 
                 * remove it from the old one for backwards TypeDirector
                 * compatibility.
                 */
                keyName[0] = '\0';
                if (LoadString(hLibInst, ind, (LPSTR)keyName, sizeof(keyName)))
                {
                    /*  Get data from .INI, use -1 to indicate failure.
                     */
                    strlen = GetProfileString(lpOldAppName,keyName,"",datastr,
                                              sizeof(datastr));
                    if (strlen == 0)
                        continue;
                    WriteProfileString(lpNewAppName,keyName,datastr);
                }
                break;
            default:
                keyName[0] = '\0';
                if (LoadString(hLibInst, ind, (LPSTR)keyName, sizeof(keyName)))
                {
                    /*  Get data from .INI, use -1 to indicate failure.
                     */
                    strlen = GetProfileString(lpOldAppName,keyName,"",datastr,
                                              sizeof(datastr));
                    if (strlen == 0)
                        continue;
                    WriteProfileString(lpNewAppName,keyName,datastr);
                    WriteProfileString(lpOldAppName,keyName,NULL);
                }
                break;
        } /* end switch */
    } /* end for */
} /* end of MoveWinIniEntries() */

/************************************************************************
 *  LOCAL void SendFontWarning(hWnd, lpOldPort);
 *
 *  Function:  Checks to see of fonts were installed on OldPort, and if so,
 *             write a warning message to the user.          
 *
 *  Parameters:
 *      hWnd -          handle to parent window.
 *      lpOldPort -     pointer to string containing old port name.
 *
 *************************************************************************/
LOCAL void SendFontWarning(hWnd,lpOldPort)
HWND    hWnd;
LPSTR   lpOldPort;
{
    char message[276];                          /* BUG #655 */
    char tmpstring[NAME_LEN];
    short data;

    MakeAppName(ModuleNameStr, lpOldPort, tmpstring, sizeof(tmpstring));
    data = GetProfileInt(tmpstring,"SoftFonts",-1);
    if (data > 0)
    {
        LoadString(hLibInst, IDS_WARNING, (LPSTR)tmpstring, NAME_LEN);
        LoadString(hLibInst, DEVINSTL_WARNING, (LPSTR)message, sizeof(message));
        MessageBox(hWnd,(LPSTR)message, (LPSTR)tmpstring, MB_OK | MB_ICONEXCLAMATION);

     }
}
#else /* end of WIN31, start of Windows 3.0 */

 /* Stub for Windows 3.0 */

short FAR PASCAL DevInstall(hWnd, lpModelName, lpOldPort, lpNewPort)
HWND hWnd;
LPSTR lpModelName;
LPSTR lpOldPort;
LPSTR lpNewPort;
{
    return(0);
}

#endif
