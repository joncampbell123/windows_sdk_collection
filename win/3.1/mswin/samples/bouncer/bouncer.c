/*  BOUNCER.C - ScreenSaverProc(), RegisterDialogClasses(), 
 *   ScreenSaverConfigureDialog() and other support code for 
 *   BOUNCER.
 *
 *   BOUNCER is a sample screen saver application. It bounces a
 *   bitmap across the display and produces a sound when the
 *   bitmap image is at the bottom of the screen.
 *
 *    (C) Copyright Microsoft Corp. 1991.  All rights reserved.
 *
 *    You have a royalty-free right to use, modify, reproduce and 
 *    distribute the Sample Files (and/or any modified version) in 
 *    any way you find useful, provided that you agree that 
 *    Microsoft has no warranty obligations or liability for any 
 *    Sample Application Files which are modified. 
 */

#include <windows.h> 
#include <mmsystem.h>

#include "bouncer.h"


/* Global used by SCRNSAVE.LIB. Required for all screen savers.
 */
char szAppName[40];


/* Globals specific to BOUNCER.
 */
char szDIBName[] = "BounceDIB";
char szSpeedName[] = "Speed";
char szXPosName[] = "xPosition";
char szYPosName[] = "yPosition";
char szXVelocityName[] = "xVelocity";
char szGravityName[] = "Gravity";
char szSoundName[] = "Sound";
char szDIBNumName[] = "DIBNum";
char szPauseName[]= "Pause at bottom";
char szName[]="Bounce a bitmap";

/* Externals defined in SCRNSAVE.LIB. Required for all screen savers.
 */

HINSTANCE _cdecl hMainInstance;
HWND _cdecl hMainWindow;
char _cdecl szName[TITLEBARNAMELEN];
char _cdecl szIsPassword[22];
char _cdecl szIniFile[MAXFILELEN];
char _cdecl szScreenSaver[22];
char _cdecl szPassword[16];
char _cdecl szDifferentPW[BUFFLEN];
char _cdecl szChangePW[30];
char _cdecl szBadOldPW[BUFFLEN];
char _cdecl szHelpFile[MAXFILELEN];
char _cdecl szNoHelpMemory[BUFFLEN];
UINT _cdecl MyHelpMessage;
HOOKPROC _cdecl fpMessageFilter;

HBITMAP hbmImage;                   // image handle
WORD wElapse;                       // speed parameter
WORD wTimer;                        // timer id
BOOL bBottom;                       // TRUE if frog is at bottom of screen
int xPos;                           // current x position
int yPos;                           // current y position
int xPosInit;                       // initial x position
int yPosInit;                       // initial y position
int xVelocInit;                     // x initial velocity
int nGravity;                       // acceleration factor
BOOL bSound;                        // sound on/off flag
BOOL bPause;                        // stick at bottom of screen?
BOOL bPassword;                     // password protected?
HANDLE hresWave;                    // handle to sound resource
LPSTR lpWave;                       // pointer to wave resource


/* ScreenSaverProc - Main entry point for screen saver messages.
 *  This function is required for all screen savers.
 *
 * Params:  Standard window message handler parameters.
 *
 * Return:  The return value depends on the message.
 *
 *  Note that all messages go to the DefScreenSaverProc(), except
 *  for ones we process.
 */
LONG FAR PASCAL ScreenSaverProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    RECT rc;
    static WORD wBottomCount;
    switch (msg)
    {
        case WM_CREATE:                             // BOUNCER-specific
        {
            HANDLE hResInfo;

            /* Load the strings from the STRINGTABLE
             */
            GetIniEntries();

            /* Load the initial bounce settings.
             */
            GetIniSettings();

            /* Load the DIB image we want to use.
             */
            hbmImage = LoadBitmap(hMainInstance, szDIBName);

            /* Load and lock the sound resource
             */
            if( hResInfo = FindResource(hMainInstance, "Sound", "WAVE") )
            {
                if( hresWave = LoadResource(hMainInstance, hResInfo) )
                {
                    lpWave = LockResource(hresWave);
                }
            }

            /* Create a timer to move the image
             */
            wTimer = SetTimer(hWnd, ID_TIMER, wElapse, NULL);

            xPos = xPosInit;
            yPos = yPosInit;

            break;
        }

        case WM_TIMER:                              // BOUNCER-specific
            if(bPause && bBottom)
            {
                if(++wBottomCount == 10)
                {
                    wBottomCount = 0;
                    bBottom = FALSE;
                }
                break;
            }

            /* Move the image around a bit
             */
            MoveImage(hWnd);

            break;

        case WM_DESTROY:                            // BOUNCER-specific

            /* Destroy any objects we created
             */
            if( hbmImage ) DeleteObject(hbmImage);
            if( wTimer )   KillTimer(hWnd, ID_TIMER);
            sndPlaySound(NULL, 0);
            if( lpWave )   UnlockResource(hresWave);
            if( hresWave ) FreeResource(hresWave);

            break;

        case WM_ERASEBKGND:
            GetClientRect(hWnd,&rc);
            FillRect((HDC)wParam,&rc,(HBRUSH)GetStockObject(BLACK_BRUSH));
            return 0L;

        default:
            break;
        }

    return DefScreenSaverProc(hWnd, msg, wParam, lParam);
}

/* RegisterDialogClasses -- Entry point for registering window
 * classes required by configuration dialog box.
 *
 * Params:  hWnd -- Handle to window
 *
 * Return:  None
 */
BOOL RegisterDialogClasses(HINSTANCE hInst)
{
    return TRUE;
}


/* ScreenSaverConfigureDialog -- Dialog box function for configuration
 * dialog.
 *
 * Params:  hWnd -- Handle to window
 *
 * Return:  None
 */
BOOL FAR PASCAL ScreenSaverConfigureDialog(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static HWND hIDOK;
  static HWND hSetPassword;

    switch (msg)
    {
        case WM_INITDIALOG:                         // BOUNCER-specific
            GetIniEntries();
            GetIniSettings();
            SetDlgItemInt(hDlg, ID_SPEED,    wElapse,    FALSE);
            SetDlgItemInt(hDlg, ID_XPOS,     xPosInit,   TRUE);
            SetDlgItemInt(hDlg, ID_YPOS,     yPosInit,   TRUE);
            SetDlgItemInt(hDlg, ID_VELOCITY, xVelocInit, TRUE);
            SetDlgItemInt(hDlg, ID_GRAVITY,  nGravity,   TRUE);
            SendDlgItemMessage(hDlg, ID_SOUND, BM_SETCHECK, bSound, NULL);
            SendDlgItemMessage(hDlg, ID_PAUSE, BM_SETCHECK, bPause, NULL);
            SendDlgItemMessage(hDlg, ID_PASSWORDPROTECTED, BM_SETCHECK,
                bPassword, NULL);
            hSetPassword=GetDlgItem(hDlg, ID_SETPASSWORD);
            EnableWindow(hSetPassword, bPassword);
            hIDOK=GetDlgItem(hDlg, IDOK);
            return TRUE;

        case WM_COMMAND:                            // BOUNCER-specific
            switch (wParam)
            {
                case IDOK:
                    wElapse    = GetDlgItemInt(hDlg, ID_SPEED,    NULL, FALSE);
                    xPosInit   = GetDlgItemInt(hDlg, ID_XPOS,     NULL, TRUE);
                    yPosInit   = GetDlgItemInt(hDlg, ID_YPOS,     NULL, TRUE);
                    xVelocInit = GetDlgItemInt(hDlg, ID_VELOCITY, NULL, TRUE);
                    nGravity   = GetDlgItemInt(hDlg, ID_GRAVITY,  NULL, TRUE);
                    bSound     = IsDlgButtonChecked(hDlg, ID_SOUND);
                    bPause     = IsDlgButtonChecked(hDlg, ID_PAUSE);
                    bPassword  = IsDlgButtonChecked(hDlg, ID_PASSWORDPROTECTED);

                    WriteProfileInt(szAppName, szSpeedName, wElapse);
                    WriteProfileInt(szAppName, szXPosName, xPosInit);
                    WriteProfileInt(szAppName, szYPosName, yPosInit);
                    WriteProfileInt(szAppName, szXVelocityName, xVelocInit);
                    WriteProfileInt(szAppName, szGravityName, nGravity);
                    WriteProfileInt(szAppName, szSoundName, bSound);
                    WriteProfileInt(szAppName, szPauseName, bPause);
                    WriteProfileInt(szAppName, szIsPassword, bPassword);

                    EndDialog(hDlg, TRUE);
                    return TRUE;

                case IDCANCEL:
                    EndDialog(hDlg, FALSE);
                    return TRUE;

                case ID_SETPASSWORD:
                {
                    FARPROC fpDialog;

                    if((fpDialog = MakeProcInstance(DlgChangePassword,hMainInstance)) == NULL)
                        return FALSE;
                    DialogBox(hMainInstance, MAKEINTRESOURCE(DLG_CHANGEPASSWORD), 
                              hDlg, fpDialog);
                    FreeProcInstance(fpDialog);
                    SendMessage(hDlg, WM_NEXTDLGCTL, hIDOK, 1l);
                    break;
                }

                case ID_PASSWORDPROTECTED:
                    bPassword ^= 1;
                    CheckDlgButton(hDlg, wParam, bPassword);
                    EnableWindow(hSetPassword, bPassword);
                    break;

                case ID_HELP:
DoHelp:
#if 0
                    bHelpActive=WinHelp(hDlg, szHelpFile, HELP_CONTEXT, IDH_DLG_BOUNCER);
                    if (!bHelpActive)
                        MessageBox(hDlg, szNoHelpMemory, szName, MB_OK);
#else
                    MessageBox(hDlg, "Insert your call to WinHelp() here.",
                        szName, MB_OK);
#endif
                    break;
            }
            break;
        default:
            if (msg==MyHelpMessage)     // Context sensitive help msg.
                goto DoHelp;
    }
    return FALSE;
}


/* THE REST OF THIS FILE IS SPECIFIC TO BOUNCER.
 *
 * Replace it with your own screen saver code.
 */


/* GetIniSettings -- Get initial bounce settings from WIN.INI
 *
 * Params:  hWnd -- Handle to window
 *
 * Return:  None
 */
static void GetIniSettings()
{
    wElapse =    GetPrivateProfileInt(szAppName, szSpeedName, DEF_SPEED, szIniFile);
    xPosInit =   GetPrivateProfileInt(szAppName, szXPosName, DEF_INIT_XPOS, szIniFile);
    yPosInit =   GetPrivateProfileInt(szAppName, szYPosName, DEF_INIT_YPOS, szIniFile);
    xVelocInit = GetPrivateProfileInt(szAppName, szXVelocityName, DEF_INIT_VELOC, szIniFile);
    nGravity =   GetPrivateProfileInt(szAppName, szGravityName, DEF_INIT_GRAVITY, szIniFile);
    bSound =     GetPrivateProfileInt(szAppName, szSoundName, DEF_SOUND, szIniFile);
    bPause =     GetPrivateProfileInt(szAppName, szPauseName, DEF_PAUSE, szIniFile);
    bPassword =  GetPrivateProfileInt(szAppName, szIsPassword, FALSE, szIniFile);
}

/* MoveImage -- Move image around the screen
 *
 * Params:  hWnd -- Handle to window
 *
 * Return:  None
 */
static void MoveImage(HWND hWnd) 
{
//    static int xPos = 10000;            // Current x value (force a reset)
//    static int yPos = 0;                // Current y value
    static int yVeloc = 0;              // Current y velocity

    int xPosOld, yPosOld;

    HDC hDC;                            // Handle to our window DC
    HDC hMemDC;                         // Handle to a memory DC
    BITMAP bm;                          // Bitmap info
    HBITMAP hbmOld;
    RECT rcWnd, rcFill;

    /* Get window size
     */
    GetClientRect(hWnd, &rcWnd);

    /* Get bitmap size
     */
    GetObject(hbmImage, sizeof(bm), (LPSTR)&bm);

    /* Update x- and y-position values
     */
    xPosOld = xPos;
    yPosOld = yPos;

    yVeloc += nGravity;

    xPos += xVelocInit;
    yPos += yVeloc;

    /* If we're at the bottom, BOUNCE!
     */
    if((yPos + bm.bmHeight > rcWnd.bottom) && (yVeloc > 0))
    {
        yVeloc = -yVeloc;                   // Reverse directions

        if (bSound && lpWave)               // Boing!!! 
        {                
            // do the multimedia bit
            sndPlaySound(lpWave, SND_ASYNC | SND_MEMORY | SND_NODEFAULT);
        }
        bBottom = TRUE;
    }

    /* If we're off the right of the screen, or off the bottom of the
     * screen, start over at beginning position and zero y-velocity.
     */
    if((xPos > rcWnd.right) || (yPos > rcWnd.bottom))
    {
        yVeloc = 0;
        xPos   = xPosInit;
        yPos   = yPosInit;
    }

    /* Get a DC to our window.  Create a compatible memory
     * DC and select our image bitmap into it so we can blit
     * it to the main window DC
     */
    hDC = GetDC(hWnd);
    hMemDC = CreateCompatibleDC(hDC);
    hbmOld = SelectObject(hMemDC, hbmImage);

    if(hbmOld)
    {
        /* Blit the image in the new position
         */
        BitBlt( hDC,                        // dest DC
                xPos,yPos,                  // dest origin
                bm.bmWidth,bm.bmHeight,     // dest extents
                hMemDC,                     // src DC
                0,0,                        // src origin
                SRCCOPY );                  // ROP code

        SelectObject(hMemDC, hbmOld);

        /* Tidy up where the old image was by filling exposed bits with
         * the background color
         *
         * This code assumes the image always moves to the right
         */
        rcFill.left   = xPosOld;                        // Left bits
        rcFill.top    = yPosOld;
        rcFill.right  = xPos;
        rcFill.bottom = rcFill.top + bm.bmHeight;

        FillRect(hDC, &rcFill, GetStockObject(BLACK_BRUSH));

        rcFill.right = rcFill.left + bm.bmWidth;        // Top or bottom bits
        if( yPos > yPosOld )
        {
            rcFill.bottom = yPos;                       // Top bits
        }
        else
        {
            rcFill.top = yPos + bm.bmHeight;            // Bottom bits
            rcFill.bottom = yPosOld + bm.bmHeight;
        }
        FillRect(hDC, &rcFill, GetStockObject(BLACK_BRUSH));
    }
    DeleteDC(hMemDC);
    ReleaseDC(hWnd, hDC);
}




/* WriteProfileInt - Write an unsigned integer value to CONTROL.INI.
 *
 * Params:  name - szSection - [section] name in .INI file
 *                 szKey     - key= in .INI file
 *                 i         - value for key above
 *
 * Return:  None
 */
static void WriteProfileInt(LPSTR szSection, LPSTR szKey, int i) 
{
    char achBuf[40];

    /* write out as unsigned because GetPrivateProfileInt() can't
     * cope with signed values!
     */
    wsprintf(achBuf, "%u", i);
    WritePrivateProfileString(szSection, szKey, achBuf, szIniFile);
}

void GetIniEntries(void)
{
  //Load Common Strings from stringtable...
  LoadString(hMainInstance, idsIsPassword, szIsPassword, 22);
  LoadString(hMainInstance, idsIniFile, szIniFile, MAXFILELEN);
  LoadString(hMainInstance, idsScreenSaver, szScreenSaver, 22);
  LoadString(hMainInstance, idsPassword, szPassword, 16);
  LoadString(hMainInstance, idsDifferentPW, szDifferentPW, BUFFLEN);
  LoadString(hMainInstance, idsChangePW, szChangePW, 30);
  LoadString(hMainInstance, idsBadOldPW, szBadOldPW, 255);
  LoadString(hMainInstance, idsHelpFile, szHelpFile, MAXFILELEN);
  LoadString(hMainInstance, idsNoHelpMemory, szNoHelpMemory, BUFFLEN);
}
