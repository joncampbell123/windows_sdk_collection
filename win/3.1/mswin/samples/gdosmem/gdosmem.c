/****************************************************************************

    PROGRAM: Gdosmem.c

    PURPOSE: Demo for GlobalDosAlloc

    FUNCTIONS:

        WinMain() - calls initialization function, processes message loop
        InitApplication() - initializes window data and registers window
        InitInstance() - saves instance handle and creates main window
        MainWndProc() - processes messages
        About() - processes messages for "About" dialog box

    COMMENTS:

        This program communicates with the GTSR program to demonstrate
        a technique for passing data between a Windows application and
        a DOS TSR. This program allocates data using GlobalDosAlloc,
        and then issues INT60h to request the TSR to increment a word
        in the allocated buffer.


****************************************************************************/

#include "windows.h"                /* required for all Windows applications */
#include "gdosmem.h"                /* specific to this program              */


HANDLE hInst;                       /* current instance                      */
int   nHandler_Installed = 0;
WORD  wSelector = 0;
WORD  wSegment;
WORD FAR *pPointer;
char  szName[] = "GDOSMem";
DWORD dWinFlags;

/****************************************************************************

    FUNCTION: WinMain(HANDLE, HANDLE, LPSTR, int)

    PURPOSE: calls initialization function, processes message loop

    COMMENTS:

        Windows recognizes this function by name as the initial entry point 
        for the program.  This function calls the application initialization 
        routine, if no other instance of the program is running, and always 
        calls the instance initialization routine.  It then executes a message 
        retrieval and dispatch loop that is the top-level control structure 
        for the remainder of execution.  The loop is terminated when a WM_QUIT 
        message is received, at which time this function exits the application 
        instance by returning the value passed by PostQuitMessage(). 

        If this function must abort before entering the message loop, it 
        returns the conventional value NULL.  

****************************************************************************/

int PASCAL WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HANDLE hInstance;                            /* current instance             */
HANDLE hPrevInstance;                        /* previous instance            */
LPSTR lpCmdLine;                             /* command line                 */
int nCmdShow;                                /* show-window type (open/icon) */
{
    MSG msg;                                 /* message                      */

    if (!hPrevInstance)                  /* Other instances of app running? */
        if (!InitApplication(hInstance)) /* Initialize shared things */
            return (FALSE);              /* Exits if unable to initialize     */

    /* Perform initializations that apply to a specific instance */

    if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

    /* Acquire and dispatch messages until a WM_QUIT message is received. */

    while (GetMessage(&msg,        /* message structure                      */
            NULL,                  /* handle of window receiving the message */
            NULL,                  /* lowest message to examine              */
            NULL))                 /* highest message to examine             */
        {
        TranslateMessage(&msg);    /* Translates virtual key codes           */
        DispatchMessage(&msg);     /* Dispatches message to window           */
    }
    return (msg.wParam);           /* Returns the value from PostQuitMessage */
}


/****************************************************************************

    FUNCTION: InitApplication(HANDLE)

    PURPOSE: Initializes window data and registers window class

    COMMENTS:

        This function is called at initialization time only if no other 
        instances of the application are running.  This function performs 
        initialization tasks that can be done once for any number of running 
        instances.  

        In this case, we initialize a window class by filling out a data 
        structure of type WNDCLASS and calling the Windows RegisterClass() 
        function.  Since all instances of this application use the same window 
        class, we only need to do this when the first instance is initialized.  


****************************************************************************/

BOOL InitApplication(hInstance)
HANDLE hInstance;                              /* current instance           */
{
    WNDCLASS  wc;

    /* Fill in window class structure with parameters that describe the       */
    /* main window.                                                           */

    wc.style = NULL;                    /* Class style(s).                    */
    wc.lpfnWndProc = MainWndProc;       /* Function to retrieve messages for  */
                                        /* windows of this class.             */
    wc.cbClsExtra = 0;                  /* No per-class extra data.           */
    wc.cbWndExtra = 0;                  /* No per-window extra data.          */
    wc.hInstance = hInstance;           /* Application that owns the class.   */
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = COLOR_WINDOW+1;
    wc.lpszMenuName =  "GdosmemMenu";   /* Name of menu resource in .RC file. */
    wc.lpszClassName = "GdosmemWClass"; /* Name used in call to CreateWindow. */

    /* Register the window class and return success/failure code. */

    return (RegisterClass(&wc));

}


/****************************************************************************

    FUNCTION:  InitInstance(HANDLE, int)

    PURPOSE:  Saves instance handle and creates main window

    COMMENTS:

        This function is called at initialization time for every instance of 
        this application.  This function performs initialization tasks that 
        cannot be shared by multiple instances.  

        In this case, we save the instance handle in a static variable and 
        create and display the main program window.  
        
****************************************************************************/

BOOL InitInstance(hInstance, nCmdShow)
    HANDLE          hInstance;          /* Current instance identifier.       */
    int             nCmdShow;           /* Param for first ShowWindow() call. */
{
    HWND            hWnd;               /* Main window handle.                */
    short   xClient, yClient;

    /* Save the instance handle in static variable, which will be used in  */
    /* many subsequence calls from this application to Windows.            */

    hInst = hInstance;

    SizeWindow (&xClient, &yClient);

    /* Create a main window for this application instance.  */

    hWnd = CreateWindow(
        "GdosmemWClass",                /* See RegisterClass() call.          */
        "GlobalDosAlloc Demo",          /* Text for window title bar.         */
        WS_OVERLAPPEDWINDOW,            /* Window style.                      */
        CW_USEDEFAULT,                  /* Default horizontal position.       */
        CW_USEDEFAULT,                  /* Default vertical position.         */
        xClient, yClient,
        NULL,                           /* Overlapped windows have no parent. */
        NULL,                           /* Use the window class menu.         */
        hInstance,                      /* This instance owns this window.    */
        NULL                            /* Pointer not needed.                */
    );

    /* If window could not be created, return "failure" */

    if (!hWnd)
        return (FALSE);

    /* Make the window visible; update its client area; and return "success" */

    ShowWindow(hWnd, nCmdShow);  /* Show the window                        */
    UpdateWindow(hWnd);          /* Sends WM_PAINT message                 */
    return (TRUE);               /* Returns the value from PostQuitMessage */

}

void SizeWindow (short *pxClient, short *pyClient)
   {
   HDC hdc;
   TEXTMETRIC tm;
   short xSize = 35;
   short ySize = 20;

   hdc = CreateIC ("DISPLAY", NULL, NULL, NULL);
   GetTextMetrics (hdc, &tm);
   DeleteDC (hdc);

   *pxClient = 2* GetSystemMetrics (SM_CXBORDER) + xSize*tm.tmAveCharWidth;
   *pyClient = 2* GetSystemMetrics (SM_CXBORDER) +
                               ySize*(tm.tmHeight+tm.tmExternalLeading);

   }

/****************************************************************************

    FUNCTION: MainWndProc(HWND, UINT, WPARAM, LPARAM)

    PURPOSE:  Processes messages

****************************************************************************/

long FAR PASCAL MainWndProc(hWnd, message, wParam, lParam)
HWND hWnd;                                /* window handle                   */
UINT message;			      /* type of message		 */
WPARAM wParam;				    /* additional information	       */
LPARAM lParam;				    /* additional information	       */
{
    FARPROC lpProcAbout;                  /* pointer to the "About" function */
    DWORD dMem;

    short y;
    static short cxChar, cyChar;
    HDC     hdc;
    PAINTSTRUCT ps;
    TEXTMETRIC tm;
    char    szBuffer [132];

    switch (message) {

        /*------------------------ C R E A T E -----------------------*/
        case WM_CREATE:
            hdc = GetDC (hWnd);
            GetTextMetrics (hdc, &tm);
            cxChar = tm.tmAveCharWidth;
            cyChar = tm.tmHeight + tm.tmExternalLeading;
            y = 0;
            ReleaseDC (hWnd, hdc);
            
            dWinFlags = GetWinFlags();
            break;

        /*------------------------ P A I N T -------------------------*/
        case WM_PAINT:
            hdc = BeginPaint (hWnd, &ps);
            y = 0;

            if (! (dWinFlags & WF_PMODE))
                {
                TextOut (hdc, cxChar, cyChar*y++, szBuffer,
                  wsprintf (szBuffer, "Windows in Real mode."));
                TextOut (hdc, cxChar, cyChar*y++, szBuffer,
                  wsprintf (szBuffer, "Just use addresses directly."));
                break;
                }

            TSR_Check();

            if (0==nHandler_Installed)
                TextOut (hdc, cxChar, cyChar*y++, szBuffer,
                  wsprintf (szBuffer, "TSR not installed."));
            else
                TextOut (hdc, cxChar, cyChar*y++, szBuffer,
                  wsprintf (szBuffer, "TSR installed."));

            if (0==wSelector)
                TextOut (hdc, cxChar, cyChar*y++, szBuffer,
                  wsprintf (szBuffer, "Buffer not allocated."));
            else
                {
                TextOut (hdc, cxChar, cyChar*y++, szBuffer,
                  wsprintf (szBuffer, "Buffer allocated:"));

                TextOut (hdc, cxChar, cyChar*y++, szBuffer,
                  wsprintf (szBuffer, "  Phys addr=%.4X:%.4X",wSegment,0));
                
                TextOut (hdc, cxChar, cyChar*y++, szBuffer,
                  wsprintf (szBuffer, "  Prot addr=%.4X:%.4X",wSelector,0));

                y++;
                pPointer = (WORD FAR *) ( (DWORD)wSelector << 16);
                TextOut (hdc, cxChar, cyChar*y++, szBuffer,
                  wsprintf (szBuffer, "Contents = %.4X",*pPointer));
                }

            EndPaint (hWnd, &ps);
            break;


        /*------------------------ C O M M A N D ---------------------*/
        case WM_COMMAND:           /* message: command from application menu */

         if (wParam == IDM_ABOUT)
            {
            lpProcAbout = MakeProcInstance(About, hInst);

            DialogBox(hInst,                 /* current instance         */
                    "AboutBox",                  /* resource to use          */
                    hWnd,                        /* parent handle            */
                    lpProcAbout);                /* About() instance address */

            FreeProcInstance(lpProcAbout);
            break;
            }

         else if (wParam == IDM_ALLOC) {
            if (0!=wSelector)
               MessageBox (hWnd, "Memory Already Allocated", szName, MB_OK);
            else
               {
               dMem = GlobalDosAlloc(2);        /* allocate a word- will */
                                                /* actually allocate more, */
                                                /* but your mileage may vary */
               wSelector = LOWORD (dMem);
               wSegment = HIWORD (dMem);
               InvalidateRect (hWnd, NULL, TRUE);
               }
            break;
            }
         
         else if (wParam == IDM_INT) {
            if (0==nHandler_Installed)
               MessageBox (hWnd, "GDOS TSR is not installed", szName, MB_OK);
            else if (0==wSelector)
               MessageBox (hWnd, "Memory Not Allocated", szName, MB_OK);
            else
                {
                TSR_Request();
                InvalidateRect (hWnd, NULL, TRUE);
                }
            break;
            }
         
         else if (wParam == IDM_FREE) {
            if (0==wSelector)
               MessageBox (hWnd, "Memory Not Allocated", szName, MB_OK);
            else
               {
               wSelector = GlobalDosFree (wSelector);
               if (0!=wSelector)
                MessageBox (hWnd, "GlobalDosFree error", szName, MB_OK);
               InvalidateRect (hWnd, NULL, TRUE);
               }
            break;
            }
         
         else
                return (DefWindowProc(hWnd, message, wParam, lParam));


        /*------------------------ D E S T R O Y ---------------------*/
        case WM_DESTROY:                  /* message: window being destroyed */
            if (0!=wSelector) {
               wSelector = GlobalDosFree (wSelector);
               }
            PostQuitMessage(0);
            break;

        default:                          /* Passes it on if unproccessed    */
            return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (NULL);
}


/****************************************************************************

    FUNCTION: About(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages for "About" dialog box

    MESSAGES:

        WM_INITDIALOG - initialize dialog box
        WM_COMMAND    - Input received

    COMMENTS:

        No initialization is needed for this particular dialog box, but TRUE
        must be returned to Windows.

        Wait for user to click on "Ok" button, then close the dialog box.

****************************************************************************/

BOOL FAR PASCAL About(hDlg, message, wParam, lParam)
HWND hDlg;                                /* window handle of the dialog box */
unsigned message;                         /* type of message                 */
WORD wParam;                              /* message-specific information    */
LONG lParam;
{
    switch (message) {
        case WM_INITDIALOG:                /* message: initialize dialog box */
            return (TRUE);

        case WM_COMMAND:                      /* message: received a command */
            if (wParam == IDOK                /* "OK" box selected?          */
                || wParam == IDCANCEL) {      /* System menu close command? */
                EndDialog(hDlg, TRUE);        /* Exits the dialog box        */
                return (TRUE);
            }
            break;
    }
    return (FALSE);                           /* Didn't process a message    */
}

/*************************************************************************

    FUNCTION: TSR_Check()

        This function checks to see it GTSR is installed.

*************************************************************************/
void TSR_Check()
{
    _asm{

        mov     ax, 0200h       ; get real mode interrupt vector
        mov     bl, 60h         ; our handler
        int     31h             ; DPMI Call

        or      cx, dx          ; anything there?
        jz      short notsr
        mov     ax, 899ah       ; GTSR Signature
        mov     bx, 0           ; install check
        int     60h

        cmp     bx, 899ah       ; did it do it?
        jnz     short notsr
        mov     nHandler_Installed, -1  ; show that it's there
notsr:
        }
}

/*************************************************************************

    FUNCTION: TSR_Request()

        This function requests the TSR to increment the WORD in our
        allocated buffer.

*************************************************************************/

void TSR_Request()
{
    _asm{
        mov     ax, 899ah               ; Signature
        mov     bx, 1                   ; issue request
        mov     cx, wSegment            ; pass in segment address
        xor     dx, dx                  ; offset zero
        int     60h
        }
}

