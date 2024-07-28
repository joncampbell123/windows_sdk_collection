

// Some macros.

#define RECTWIDTH(lpRect)     ((lpRect)->right - (lpRect)->left)
#define RECTHEIGHT(lpRect)    ((lpRect)->bottom - (lpRect)->top)

#define szMDIChild "MyDIBMDI"          // Class name of MDI children.
#define DRAGCURSOR "DragCursor"        // Name of dragging cursor in .RC file.

#define SWAP(x,y)   ((x)^=(y)^=(x)^=(y))


// Global variables.

extern HANDLE hInst;                   // Handle to this instance
extern HWND   hWndMDIClient;           // MDI Client's window handle.



// String Defines

#define IDS_PROGNAME        0
#define IDS_WINBMP          1
#define IDS_PMBMP           2
#define IDS_OPENDLG         3
#define IDS_SAVEDLG         4
#define IDS_FILEOPEN        5
#define IDS_FILESAVE        6
#define IDS_RGB             7
#define IDS_PM              8
#define IDS_RLE4            9
#define IDS_RLE8           10
#define IDS_1              11
#define IDS_4              12
#define IDS_8              13
#define IDS_24             14
#define IDS_PASTE          15
#define IDS_CAPTURE        16       // Window title for captured window.


//  Defines for File Save format combo box

#define ID_FS_BEGIN       IDS_PM
#define ID_FS_END         IDS_24

// Menu Defines for main menu.

#define OPTION_MENU         3          // position of option menu
#define WINDOW_MENU         5          // position of window menu


#define IDM_OPEN            100        // Open a DIB
#define IDM_SAVE            101        // Save a DIB
#define IDM_PRINT           102        // Print a DIB
#define IDM_EXIT            103        // Exit DibView
#define IDM_ABOUT           104        // About Box

#define IDM_COPY            250        // Copy to clipboard
#define IDM_PASTE           251        // Paste from clipboard

#define IDM_WINDOWTILE      300        // Tile MDI windows
#define IDM_WINDOWCASCADE   301        // Cascade MDI windows
#define IDM_WINDOWICONS     302        // Minimize all MDI windows
#define IDM_WINDOWCLOSEALL  303        // Close all MDI windows

#define IDM_OPTIONS         350        // Stretch DIB to Window (or no stretch)

#define IDM_CAPTWINDOW      375        // Capture a Window
#define IDM_CAPTCLIENT      376        // Capture client area
#define IDM_CAPTFULLSCREEN  377        // Capture entire screen
#define IDM_CAPTUREHIDE     379        // "Hide window"

#define IDM_PALDIB          400        // Show DIB's palette
#define IDM_PALSYS          401        // Show the system's palette
#define IDM_PALANIMATE      402        // Animate the DIB's palette
#define IDM_PALRESTORE      403        // Restore the DIB's palette

#define IDM_WINDOWCHILD    4100        // Starting Child Windows here





// Function Prototypes

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
