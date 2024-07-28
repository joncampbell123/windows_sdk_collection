/*
 * RAINBOW -- Example Dialog Editor custom control
 *
 */

#include <windows.h>
#include <custcntl.h>
#include "rainbow.h"

/* global static variables */
HANDLE		 hLibData;
HANDLE		 hLibInstance;
LPFNSTRTOID      lpfnVerId;
LPFNIDTOSTR      lpfnIdStr;

/* string for property lists */
#define    IDFNLO                    "lpfnIdFnLo"
#define    IDFNHI                    "lpfnIdFnHi"

#define    RAINBOWCLASS            "Rainbow"

/* general rainbow definitions */
#define    ID			     GetWindowWord( hWnd, GWW_ID )
#define    PARENT                    GetWindowWord( hWnd, GWW_HWNDPARENT )
#define    INSTANCE		     GetWindowWord( hWnd, GWW_HINSTANCE )

/* rainbow specific definitions */
#define    RAINBOW_EXTRA            12                

#define    RANGE		     GetWindowWord( hWnd, 0 )
#define    TABLE		     GetWindowWord( hWnd, 2 )
#define    WIDTH		     GetWindowWord( hWnd, 4 )
#define    HEIGHT                    GetWindowWord( hWnd, 6 )
#define    CHOICE                    GetWindowWord( hWnd, 8 )
#define    CAPTURE		     GetWindowWord( hWnd, 10 )

#define    SET_RANGE(x)            SetWindowWord( hWnd, 0, x )
#define    SET_TABLE(x)            SetWindowWord( hWnd, 2, x )
#define    SET_WIDTH(x)            SetWindowWord( hWnd, 4, x )
#define    SET_HEIGHT(x)	   SetWindowWord( hWnd, 6, x )
#define    SET_CHOICE(x)	   SetWindowWord( hWnd, 8, x )
#define    SET_CAPTURE(x)	   SetWindowWord( hWnd, 10, x )

/* caret related definitions */
#define    CARET_XPOS		   ((CHOICE*WIDTH)+3)
#define    CARET_YPOS		   (3)
#define    CARET_WIDTH		   (WIDTH-6)
#define    CARET_HEIGHT            (HEIGHT-6)

/* selector related definitions */
#define    SELECTOR_XPOS	  ((CHOICE*WIDTH)+1)
#define    SELECTOR_YPOS	  (1)
#define    SELECTOR_WIDTH	  (WIDTH-2)
#define    SELECTOR_HEIGHT        (HEIGHT-2)

/* internal rainbow function prototypes */
BOOL FAR PASCAL     RainbowDlgFn( HWND, WORD, WORD, LONG );
LONG FAR PASCAL     RainbowWndFn( HWND, UINT, WPARAM, LPARAM );
void static	    DrawSelector( HWND, HDC );

/**/

/*
 * LibMain( hInstance, wDataSegment, wHeapSize, lpszCmdLine ) : WORD
 *
 *    hInstance      library instance handle
 *    wDataSegment   library data segment
 *    wHeapSize      default heap size
 *    lpszCmdLine    command line arguments
 *
 * LibMain is called by LibEntry, which is called by Windows when 
 * the DLL is loaded.  The LibEntry routine is provided 
 * in the LIBENTRY.OBJ in the SDK Link Libraries disk.  (The source 
 * LIBENTRY.ASM is also provided.)  
 *
 * LibEntry initializes the DLL's heap, if a HEAPSIZE value is
 * specified in the DLL's DEF file.  Then LibEntry calls
 * LibMain.  The LibMain function below satisfies that call.
 *
 * LibMain performs all the initialization necessary to use the
 * rainbow user control.  Included in this initialization is the
 * registration of the Rainbow window class.
 *
*/
 
int FAR PASCAL LibMain(
   HANDLE      hInstance,
   WORD        wDataSegment,
   WORD        wHeapSize,
   LPSTR       lpszCmdLine )
{
    HANDLE            hClassStruct;
    LPWNDCLASS        lpClassStruct;

    /* register rainbow window if necessary */
    if ( hLibInstance == NULL ) {

        /* allocate memory for class structure */
        hClassStruct = GlobalAlloc( GHND, (DWORD)sizeof(WNDCLASS) );
        if ( hClassStruct ) {

            /* lock it down */
            lpClassStruct = (LPWNDCLASS)GlobalLock( hClassStruct );
            if ( lpClassStruct ) {
    
                /* define class attributes */
                lpClassStruct->lpszClassName =     (LPSTR)RAINBOWCLASS;
                lpClassStruct->hCursor =            LoadCursor( NULL, IDC_ARROW );
                lpClassStruct->lpszMenuName =        (LPSTR)NULL;
                lpClassStruct->style =                CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS|CS_GLOBALCLASS;
                lpClassStruct->lpfnWndProc =        RainbowWndFn;
                lpClassStruct->hInstance =            hInstance;
                lpClassStruct->hIcon =                NULL;
                lpClassStruct->cbWndExtra =        RAINBOW_EXTRA;
                lpClassStruct->hbrBackground =    (HBRUSH)(COLOR_WINDOW + 1 );
    
                /* register rainbow window class */
                hLibInstance = ( RegisterClass(lpClassStruct) ) ? hInstance : NULL;

                /* unlock structure */
                GlobalUnlock( hClassStruct );

            }

            /* release class structure */
            GlobalFree( hClassStruct );

        }

    }

    /* return result 1 = success; 0 = fail */
    return( hLibInstance? 1:0 );
}

/**/


/****************************************************************************
    FUNCTION:  WEP(int)

    PURPOSE:  Performs cleanup tasks when the DLL is unloaded.  WEP() is
              called automatically by Windows when the DLL is unloaded
              (no remaining tasks still have the DLL loaded).  It is
              strongly recommended that a DLL have a WEP() function,
              even if it does nothing but return, as in this example.

*******************************************************************************/
VOID FAR PASCAL WEP (bSystemExit)
int  bSystemExit;
{
    return;
}


/*
 * RainbowInfo() : HANDLE
 *
 * This function returns a handle to a global block of memory that
 * contains various information about the kinds of controls the library
 * is capable of supporting.  This data block can, for example, be used
 * by the dialog editor when determining the capabilities of a particular
 * control library.
 *
 * Note that this handle becomes the property of the caller once this
 * function returns.  This implies that the caller must call GlobalFree
 * once it is finished with the data.
 *
 */

HANDLE FAR PASCAL RainbowInfo()
{
    HANDLE        hCtlInfo;
    LPCTLINFO    lpCtlInfo;

    /* allocate space for information structure */
    hCtlInfo = GlobalAlloc( GHND, (DWORD)sizeof(CTLINFO) );
    if ( hCtlInfo ) {

        /* attempt to lock it down */
        lpCtlInfo = (LPCTLINFO)GlobalLock( hCtlInfo );
        if ( lpCtlInfo ) {

            /* define the fixed portion of the structure */
            lpCtlInfo->wVersion = 100;
            lpCtlInfo->wCtlTypes = 1;
            lstrcpy( lpCtlInfo->szClass, RAINBOWCLASS );
            lstrcpy( lpCtlInfo->szTitle, "Sample User Control" );

            /* define the variable portion of the structure */
            lpCtlInfo->Type[0].wWidth = 33;
            lpCtlInfo->Type[0].wHeight = 20;
            lpCtlInfo->Type[0].dwStyle = WS_CHILD;
            lstrcpy( lpCtlInfo->Type[0].szDescr, "Rainbow" );

            /* unlock it */
            GlobalUnlock( hCtlInfo );
        
        } else {
            GlobalFree( hCtlInfo );
            hCtlInfo = NULL;
        }
    
    }

    /* return result */
    return( hCtlInfo );

}
 
/**/

/*
 * RainbowStyle( hWnd, hCtlStyle, lpfnVeriyId, lpfnGetIdStr ) : BOOL;
 *
 *    hWnd           handle to parent window
 *    hCtlStyle      handle to control style
 *    lpfnVerifyId   pointer to the VerifyId function from Dialog editor
 *    lpfnGetIdStr   pointer to the GetIdStr functionn from Dialog editor
 *
 * This function enables the user to edit the style of a particular
 * control provided.  The current control style information is passed
 * in using a handle to a control style data structure.
 *
 * This function returns this same handle (referencing updated
 * information) if the dialog box is normally closed.  A value of
 * NULL is returned if the user cancelled the operation.
 *
 */

BOOL FAR PASCAL RainbowStyle(
    HWND        hWnd,
    HANDLE      hCtlStyle,
    LPFNSTRTOID    lpfnVerifyId,
    LPFNIDTOSTR    lpfnGetIdStr )
{
    FARPROC	  lpDlgFn;
    HANDLE        hNewCtlStyle;

    /* initialization */
    hLibData = hCtlStyle;
    lpfnVerId = lpfnVerifyId;
    lpfnIdStr = lpfnGetIdStr;

    /* display dialog box */
    lpDlgFn = MakeProcInstance( (FARPROC)RainbowDlgFn, hLibInstance );
    hNewCtlStyle = ( DialogBox(hLibInstance,"RainbowStyle",hWnd,lpDlgFn) ) ? hLibData : NULL;
    FreeProcInstance( lpDlgFn );

    /* return updated data block */
    return( hNewCtlStyle );

}

/**/

/*
 * RainbowFlags( wFlags, lpszString, wMaxString ) : WORD;
 *
 *    wFlags         class style flags
 *    lpszString     class style string
 *    wMaxString     maximum size of class style string
 *
 * This function translates the class style flags provided into a
 * corresponding text string for output to an RC file.  The general
 * windows flags (contained in the low byte) are not interpreted,
 * only those in the high byte.
 *
 * The value returned by this function is the library instance
 * handle when sucessful, and NULL otherwise.
 *
 */

WORD FAR PASCAL RainbowFlags(
    WORD        wFlags,
    LPSTR       lpszString,
    WORD        wMaxString )
{
    lpszString[0] = NULL;
    return( 0 );
}

/**/

/*
 * RainbowWndFn( hWnd, wMsg, wParam, lParam ) : LONG
 *
 *        hWnd                    handle to rainbow window
 *        wMsg                    message number
 *        wParam                single word parameter
 *        lParam                double word parameter
 *
 * This function is responsible for processing all the messages
 * which relate to the rainbow control window.  Note how the
 * code is written to avoid potential problems when re-entrancy
 * ocurrs - this involves the use of extra bytes associated with
 * the window data structure.
 *
 * The LONG value returned by this function is either a value
 * returned by the internal handling of the message or by the
 * default window procedure.
 *
 */

LONG FAR PASCAL RainbowWndFn( hWnd, wMsg, wParam, lParam )
    HWND            hWnd;
    UINT	    wMsg;
    WPARAM	      wParam;
    LPARAM	      lParam;
{
    /* local variables */
    LONG            lResult;                    /* temporary result variable */
    
    /* initialization */
    lResult = TRUE;
    
    /* process message */
    switch( wMsg )
        {
    case WM_GETDLGCODE : /* capture all key strokes */
	 lResult=DLGC_WANTARROWS;
        break;
    case WM_CREATE : /* create pallette window */
        
        {
            /* temporary variables */
            HANDLE        hrgbList;                /* handle to rgb list */
            LONG FAR *    lprgbEntry;                /* pointer to rgb list */
            
            /* allocate space for rgb color list */
            hrgbList = GlobalAlloc( GMEM_MOVEABLE, sizeof(LONG)*16L );
            if ( hrgbList ) {
            
               /*
                 * Define initial rgb color & value list - note that
                 * eight default colors are selected with the values
                 * matching each of the colors.
                 */

                lprgbEntry = (LONG FAR *)GlobalLock( hrgbList );
                lprgbEntry[0] = RGB( 0x00, 0x00, 0x00 );
                lprgbEntry[1] = RGB( 0x00, 0x00, 0xFF );
                lprgbEntry[2] = RGB( 0x00, 0xFF, 0x00 );
                lprgbEntry[3] = RGB( 0xFF, 0x00, 0x00 );
                lprgbEntry[4] = RGB( 0x00, 0xFF, 0xFF );
                lprgbEntry[5] = RGB( 0xFF, 0xFF, 0x00 );
                lprgbEntry[6] = RGB( 0xFF, 0x00, 0xFF );
                lprgbEntry[7] = RGB( 0xFF, 0xFF, 0xFF );
                lprgbEntry[8] = RGB( 0x00, 0x00, 0x00 );
                lprgbEntry[9] = RGB( 0x00, 0x00, 0xFF );
                lprgbEntry[10] = RGB( 0x00, 0xFF, 0x00 );
                lprgbEntry[11] = RGB( 0xFF, 0x00, 0x00 );
                lprgbEntry[12] = RGB( 0x00, 0xFF, 0xFF );
                lprgbEntry[13] = RGB( 0xFF, 0xFF, 0x00 );
                lprgbEntry[14] = RGB( 0xFF, 0x00, 0xFF );
                lprgbEntry[15] = RGB( 0xFF, 0xFF, 0xFF );
                GlobalUnlock( hrgbList );
            
                /* define instance variables */
                SET_RANGE( 8 );
                SET_TABLE( hrgbList );
                SET_WIDTH( ((LPCREATESTRUCT)lParam)->cx / 8 );
                SET_HEIGHT( ((LPCREATESTRUCT)lParam)->cy );
                SET_CHOICE( 0 );
                SET_CAPTURE( FALSE );
    
            } else
                DestroyWindow( hWnd );
                
        }
        
        break;
    case WM_SIZE : /* window being resized */
    
        /* redefine width & height instance variables */
        SET_WIDTH( LOWORD(lParam) / 8 );
        SET_HEIGHT( HIWORD(lParam) );

        break;
    case WM_PAINT : /* paint control window */
        
        {
            PAINTSTRUCT        Ps;                    /* paint structure */
            WORD                wEntry;                /* current color entry */
            HANDLE            hBrush;                /* handle to new brush */
            HANDLE            hOldBrush;            /* handle to old brush */
            LONG FAR *        lprgbEntry;            /* pointer to rgb list */
                    
            /* start paint operation */
            BeginPaint( hWnd, (LPPAINTSTRUCT)&Ps );
            
            /* iteratively paint each color patch */
            lprgbEntry = (LONG FAR *)GlobalLock( TABLE );
            for ( wEntry=0; wEntry<RANGE; wEntry++ ) {

                /* create solid brush for patch & select */
                hBrush = CreateSolidBrush( lprgbEntry[wEntry] );
                hOldBrush = SelectObject( Ps.hdc, hBrush );
    
                /* draw rectangle with brush fill */
                Rectangle(
                    Ps.hdc,
                    wEntry*WIDTH,
                    0,
                    (wEntry*WIDTH)+WIDTH,
                    HEIGHT
                );

                /* unselect brush and delete */
                SelectObject( Ps.hdc, hOldBrush );
                DeleteObject( hBrush );
    
            }
            GlobalUnlock( TABLE );
                    
            /* End paint operation */
            EndPaint( hWnd, (LPPAINTSTRUCT)&Ps );
        }
        
        break;
    case WM_KEYDOWN : /* key being pressed */
        
        {
            /* local variables */
            HDC                hDC;                    /* display context handle */
        
            /* retrieve display context & unmark current selection */
            hDC = GetDC( hWnd );
            DrawSelector( hWnd, hDC );            

            /* process virtual key codes */
            switch( wParam )
                {
            case VK_HOME : /* home key */
                SET_CHOICE( 0 );
                break;
            case VK_UP : /* up cursor key */
            case VK_LEFT : /* left cursor key */
                SET_CHOICE( (CHOICE > 0) ? CHOICE-1 : RANGE-1 );
                break;
            case VK_DOWN : /* down cursor key */
            case VK_RIGHT : /* right cursor key */
            case VK_SPACE : /* space bar - move right */
                SET_CHOICE( (CHOICE < RANGE-1) ? CHOICE+1 : 0 );
                break;
            case VK_END : /* end key */
                SET_CHOICE( RANGE-1 );
                break;
            default : /* some other key */
                lResult = FALSE;
                break;
            }

            /* mark new selection & release display context */
            DrawSelector( hWnd, hDC );
            ReleaseDC( hWnd, hDC );

            /* move caret to new position */
            SetCaretPos( CARET_XPOS, CARET_YPOS );

            /* notify parent of new selection */
            SendMessage(PARENT,WM_COMMAND, ID, MAKELONG (hWnd, CHOICE));
                    
        }
        
        break;
    case WM_SETFOCUS : /* get focus - display caret */
        /* create caret & display */
        CreateCaret( hWnd, NULL, CARET_WIDTH, CARET_HEIGHT );
        SetCaretPos( CARET_XPOS, CARET_YPOS );
        ShowCaret( hWnd );
        break;

    case WM_LBUTTONDOWN : /* left button depressed - fall through */

        {
            /* local variables */
            HDC                hDC;                    /* display context handle */

            /* retrieve display context */
            hDC = GetDC ( hWnd );

            /* unmark old selection & mark new one */
            DrawSelector( hWnd, hDC );            
            SET_CHOICE( LOWORD(lParam)/WIDTH );
            DrawSelector( hWnd, hDC );
        
            /* release display context & move caret */
            ReleaseDC( hWnd, hDC );

            /* capture focus & move caret */
            SetFocus(hWnd);
            SetCaretPos( CARET_XPOS, CARET_YPOS );

            /* notify parent of new selection */
            SendMessage(PARENT,WM_COMMAND,ID, MAKELONG (hWnd, CHOICE));
            
            /* activate capture */
            SetCapture( hWnd );
            SET_CAPTURE( TRUE );            

        }
        
        break;
    case WM_MOUSEMOVE : /* mouse being moved */

        /* track mouse only if capture on */
        if ( CAPTURE ) {
        
            /* local variables */
            HDC                hDC;                    /* display context handle */
            WORD                wNewChoice;            /* new mouse selection */
        
            /* calculate new selection */
            wNewChoice = ( LOWORD(lParam) <= 0 ) ?
                0 :
                ( LOWORD(lParam)/WIDTH >= RANGE ) ?
                    RANGE - 1 :
                    LOWORD(lParam) / WIDTH;
            
            /* update display if different */
            if ( wNewChoice != CHOICE ) {

                /* retrieve display context */
                hDC = GetDC ( hWnd );

                /* unmark old selection & mark new one */
                DrawSelector( hWnd, hDC );            
                SET_CHOICE( wNewChoice );
                DrawSelector( hWnd, hDC );
        
                /* release display context & move caret */
                ReleaseDC( hWnd, hDC );
                SetCaretPos( CARET_XPOS, CARET_YPOS );
            
                /* notify parent of new selection */
                SendMessage(PARENT,WM_COMMAND, ID, MAKELONG (hWnd, CHOICE));
            
            }
            
        }
        
        break;
    case WM_LBUTTONUP : /* left button released */

        /* release capture if active */
        if ( CAPTURE ) {
            SET_CAPTURE( FALSE );
            ReleaseCapture();
        }            

        break;
    case WM_KILLFOCUS : /* kill focus - hide caret */
        DestroyCaret();
        break;
    case RM_SETSEL : /* select a color entry */
        {
    
            /* update selection & redraw rainbow */
            SET_CHOICE( (wParam >= RANGE) ? 0 : wParam );
            InvalidateRect( hWnd, NULL, TRUE );
        
            /* notify parent of change in color */
            SendMessage( PARENT, WM_COMMAND, ID, MAKELONG (hWnd, CHOICE) );

            /* return new choice */
            lResult = CHOICE;

        }
        break;
    case RM_GETSEL : /* retrieve selected color */
        {
            LONG FAR *        lprgbEntry;            
    
            /* return selected entry */
            lResult = CHOICE;

            /* define selected color */
            lprgbEntry = (LONG FAR *)GlobalLock( TABLE );
            *(LONG FAR *)lParam = lprgbEntry[RANGE+CHOICE];
            GlobalUnlock( TABLE );

        }
        break;
    case RM_SETCOLORS : /* define rainbow color table */
        {
            WORD            wEntry;
            HANDLE        hrgbList;
            RECT            rectClient;
            LONG FAR *    lprgbEntry;
    
            /* release previous table from memory */
            GlobalFree( TABLE );
    
            hrgbList = GlobalAlloc(    GMEM_MOVEABLE,    sizeof(LONG)*wParam*2L);        
            if ( hrgbList ) {
            
                /* define initial rgb colors & values */
                lprgbEntry = (LONG FAR *)GlobalLock( hrgbList );        
                for ( wEntry=0; wEntry < wParam; wEntry++ ) {
                    lprgbEntry[wEntry] = ((LONG FAR*)lParam)[wEntry];        
                    lprgbEntry[wParam+wEntry] = ((LONG FAR*)lParam)[wParam+wEntry];
                }    
                GlobalUnlock( hrgbList );
            
                /* retrieve current window dimensions */
                GetClientRect( hWnd, &rectClient );

                /* re-define instance variables */
                SET_RANGE( wParam );
                SET_TABLE( hrgbList );
                SET_WIDTH( (rectClient.right-rectClient.left)/wParam );
                SET_HEIGHT( rectClient.bottom-rectClient.top );
                SET_CHOICE( 0 );
    
                /* update window & notify parent of new selection */
                InvalidateRect( hWnd, NULL, TRUE );
                SendMessage( PARENT, WM_COMMAND, ID, MAKELONG (hWnd, CHOICE) );

                /* normal return */
                lResult = wParam;
        
            } else
                lResult = -1L;

        }
        break;
    case RM_GETCOLORS : /* retrieve rainbow color table */
        {
            WORD            wEntry;      
            LONG FAR *    lprgbEntry;

            /* retrieve number of colors */
            lResult = RANGE;
    
            /* retrieve rgb color list */
            lprgbEntry = (LONG FAR *)GlobalLock( TABLE );        
            for ( wEntry=0; (wEntry < RANGE)&&(wEntry < wParam); wEntry++ ) {
                ((LONG FAR *)lParam)[wEntry] = lprgbEntry[wEntry];        
                ((LONG FAR *)lParam)[RANGE+wEntry] = lprgbEntry[RANGE+wEntry];
            }
            GlobalUnlock( TABLE );
    
        }
        break;
    case WM_DESTROY : /* window being destroyed */
        GlobalFree( TABLE );        
        break;
    default : /* default window message processing */
        lResult = DefWindowProc( hWnd, wMsg, wParam, lParam );
        break;
    }
    
    /* return final result */
    return( lResult );

}

/**/

/*
 * RainbowDlgFn( hDlg, wMessage, wParam, lParam ) : BOOL;
 *
 *    hDlg           dialog box handle
 *    wMessage       current window message
 *    wParam         word parameter
 *    lParam         long parameter
 *
 * This function is responsible for processing all the messages that
 * relate to the style dialog box.  This function transfers data 
 * between itself and the RainbowStyle using a global data handle.
 *
 * If the user presses the OK button, this data handle is used to pass
 * back a new style data block.  It is left to the calling routine to
 * delete this new block.
 *
 */

BOOL FAR PASCAL RainbowDlgFn(
    HWND        hDlg,
    WORD        wMessage,
    WORD        wParam,
    LONG        lParam )
{
    BOOL            bResult;

    /* initialization */
    bResult = TRUE;

    /* process message */
    switch( wMessage )
        {
    case WM_INITDIALOG :
        {
            HANDLE        hCtlStyle;
            LPCTLSTYLE    lpCtlStyle;
            char            szId[  20 ];    /* temp. string to hold id */

            /* disable Ok button & save dialog data handle */
            hCtlStyle = hLibData;
            EnableWindow( GetDlgItem(hDlg,IDOK), FALSE );

            /* retrieve & display style parameters */
            if ( hCtlStyle ) {

                /* add handle to property list */
                SetProp( hDlg, MAKEINTRESOURCE(1), hCtlStyle );
                
                /* update dialog box fields */
                lpCtlStyle = (LPCTLSTYLE)GlobalLock( hCtlStyle );
                SetDlgItemText( hDlg, IDTEXT, lpCtlStyle->szTitle );

                /* Kanhom Ng 2/7/89 
                 * Set the id value string correctly.
                 * Save the pointer to the verify id function from dialog editor
                 */
                if ( lpfnIdStr )
                {
                    (*lpfnIdStr)(lpCtlStyle->wId, (LPSTR)szId, sizeof( szId ) );
                    SetDlgItemText( hDlg, IDVALUE, szId );
                }
                else
                {
                    EnableWindow( GetDlgItem( hDlg, IDVALUE ), FALSE );
                }
                lstrcpy( lpCtlStyle->szClass, RAINBOWCLASS );
                SetProp( hDlg, IDFNHI, HIWORD( (DWORD)lpfnVerId ) );
                SetProp( hDlg, IDFNLO, LOWORD( (DWORD)lpfnVerId ) );

                GlobalUnlock( hCtlStyle );
                    
            } else
                EndDialog( hDlg, FALSE );
            
        }
        break;
    case WM_COMMAND :

        /* process sub-message */
        switch( wParam )
            {
        case IDCANCEL : /* cancel dialog box */
            RemoveProp( hDlg, MAKEINTRESOURCE(1) );
            RemoveProp( hDlg, IDFNLO );
            RemoveProp( hDlg, IDFNHI );
            EndDialog( hDlg, FALSE );
            break;
        case IDOK : /* save dialog changes */
            {
                HANDLE        hCtlStyle;
                LPCTLSTYLE    lpCtlStyle;
                LPFNSTRTOID      lpfnId;          /* pointer to the verify id function from dialog editor */
                char            szId[ 20 ];        /* temp. string to hold the id */

                hCtlStyle = GetProp( hDlg, MAKEINTRESOURCE(1) );

                /* update structure contents */
                lpCtlStyle = (LPCTLSTYLE)GlobalLock( hCtlStyle );

                szId[ 0 ] = NULL;
                GetDlgItemText( hDlg, IDVALUE, szId, sizeof(szId) );
                lpfnId = (LPFNSTRTOID)MAKELONG( GetProp( hDlg, IDFNLO ), GetProp( hDlg, IDFNHI ) );
                if ( lpfnId )
                {
                    DWORD        dwResult; /* result ofthe verifyId function */

                    dwResult = (*lpfnId)( (LPSTR)szId );
                    if ( !(BOOL)dwResult )
                    {
                        /* Wrong id */
                        GlobalUnlock( hCtlStyle );
                        break;        
                    }

                    lpCtlStyle->wId = HIWORD( dwResult );
                }
                        
                GetDlgItemText( hDlg, IDTEXT, lpCtlStyle->szTitle, sizeof(lpCtlStyle->szTitle) );
                GlobalUnlock( hCtlStyle );

                RemoveProp( hDlg, MAKEINTRESOURCE(1) );
                RemoveProp( hDlg, IDFNLO );
                RemoveProp( hDlg, IDFNHI );
            
                /* end dialog box */
                hLibData = hCtlStyle;
                EndDialog( hDlg, TRUE );

            }
            break;
        case IDTEXT : /* control text */
        case IDVALUE : /* control id */

            /* enable or disable Ok button */
            if ( HIWORD(lParam) == EN_CHANGE )
                EnableWindow(
                    GetDlgItem(hDlg,IDOK),
                    (SendMessage(GetDlgItem(hDlg,IDTEXT),WM_GETTEXTLENGTH,0,0L) &&
                     SendMessage(GetDlgItem(hDlg,IDVALUE),WM_GETTEXTLENGTH,0,0L)) ? TRUE : FALSE
                );

            break;
        default : /* something else */
            bResult = FALSE;
            break;
        }

        break;
    default :
        bResult = FALSE;
        break;
    }

    /* return final result */
    return( bResult );

}

/**/

/*
 * DrawSelector( hWnd, hDC ) : void;
 *
 *        hWnd            window handle
 *        hDC            handle to display context
 *
 *    This function is responsible for drawing the selector
 * which surrounds the active color patch.  This drawing
 * process involves the use of the R2_NOT operator to
 * simplify the drawing & re-drawing process.
 *
 */
 
void static DrawSelector( hWnd, hDC )
    HWND            hWnd;
    HDC            hDC;
{
    /* local variables */
    HANDLE        hOldPen;                    /* old pen */
    WORD            wOldROP2;                /* old raster op code */
    WORD            wOldBkMode;                /* old background mode */
    
    /* setup display context */
    wOldROP2 = SetROP2( hDC, R2_NOT );
    wOldBkMode = SetBkMode( hDC, TRANSPARENT );

    hOldPen = SelectObject( hDC, CreatePen(0,1,RGB(0,0,0)) );
    
    /* draw selector rectangle */
    Rectangle(
        hDC,
        SELECTOR_XPOS,
        SELECTOR_YPOS,
        SELECTOR_XPOS+SELECTOR_WIDTH,
        SELECTOR_YPOS+SELECTOR_HEIGHT
    );

    DeleteObject( SelectObject(hDC,hOldPen) );

    /* restore display context */
    SetBkMode( hDC, wOldBkMode );
    SetROP2( hDC, wOldROP2 );

}
