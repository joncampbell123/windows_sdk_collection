//+--------------------------------------------------------
// File:        App.cxx
//
// Classes:     CController
//
// Functions:   WinMain
//              WndProc
//              MakeWindowClass
//              AboutDlgProc
//
// History:     22-Jan-1993     asmusf  created
//----------------------------------------------------------

#include <windows.h>
#include <windowsx.h>
#include "app.hxx"
//+--------------------------------------------------------
// Class:       CController
//
// Purpose:     Controlling the application, UI handler
//
// History:     22-Jan-1993     asmusf  created
//----------------------------------------------------------

void CController::Create(HWND hwnd, LONG lParam )
{
    HANDLE handle = ((LPCREATESTRUCT) lParam)->hInstance;
    _hInst = (HINSTANCE)handle;
    _funAbout = (DLGPROC) MakeProcInstance((FARPROC)AboutDlgProc, _hInst);

    // Set up the Main View
    _pView = new CScrollableView(((LPCREATESTRUCT) lParam)->cx,
                                ((LPCREATESTRUCT) lParam)->cy);
    _pView->SetScale(100);

    SetScrollRange(hwnd, SB_VERT, 0, PAGEHEIGHT, FALSE);
    SetScrollRange(hwnd, SB_HORZ, 0, PAGEWIDTH, FALSE);

    SetScrollPos(hwnd, SB_HORZ, 0, TRUE );
    SetScrollPos(hwnd, SB_VERT, 0, TRUE );

    // Set up the Model
    _pModel= new CModel(_hInst, hwnd, HEXADECIMAL, USEDONLY);

    // Set up the initial state of the Menu
    Page(hwnd, IDM_FIRSTPAGE);

    // initially no pop-up character box
    _pBox = 0;
}

void CController::Destroy()
{

    FreeProcInstance((FARPROC)AboutDlgProc);

    delete _pView;
    delete _pModel;
    PostQuitMessage(0);
}

void CController::Size ( LONG lParam )
{
   _pView->SetSize ( LOWORD(lParam), HIWORD(lParam) );
}

// Message Box with text from resource file
void CController::AlertBox( HWND hwnd, UINT idsText, UINT fuStyle)
{
    TCHAR szText[100];
    TCHAR szCaption[100];

    LoadString(_hInst, idsText, (LPTSTR)szText, 100);
    LoadString(_hInst, IDS_MSGCAPTION, (LPTSTR)szCaption, 100);

    ::MessageBox (hwnd, (LPTSTR)szText, (LPTSTR)szCaption, fuStyle) ;
};

// Menu commands processing

void CController::Command ( HWND hwnd, WPARAM wID )
{
    switch ( wID )
    {
      // File Menu
      case IDM_PRINT:
           {
                CPrintRequest job(hwnd, 1, _pModel->GetMaxPage());

                if(!job.Cancelled() && !job.Error())
                {
                    CPrintCanvas canvas (job);
                    job.Print (_hInst, canvas, _pModel);
                    
                    if(job.Error())
                    {
                        AlertBox (hwnd, IDS_PRINTERR, MB_OK | MB_ICONEXCLAMATION) ;
                    }
                }
           }
           break;
      case IDM_EXIT:
           SendMessage ( hwnd, WM_CLOSE, 0L, 0L );
           break;

      // View Menu
      case IDM_NEXTPAGE:
           Page(hwnd, IDM_NEXTPAGE);
           break;
      case IDM_PREVPAGE:
           Page(hwnd, IDM_PREVPAGE);
           break;
      case IDM_NEXTSECTION:
           Page(hwnd, IDM_NEXTSECTION);
           break;
      case IDM_PREVSECTION:
           Page(hwnd, IDM_PREVSECTION);
           break;
      case IDM_ZOOMIN:
           {
               UINT iScale;
               _pView->GetScale(iScale);
               iScale -= (iScale > 50 ? 25 : 0);
               _pView->SetScale(iScale);
               _pView->Invalidate(hwnd);
           }
           break;
      case IDM_ZOOMOUT:
           {
               UINT iScale;
               _pView->GetScale(iScale);
               iScale += (iScale < 200 ? 25 : 0);
               _pView->SetScale(iScale);
               _pView->Invalidate(hwnd);
           }
           break;

      // Options Menu
      case IDM_FONT:
           _pModel->ChooseFont(hwnd);
           _pView->Invalidate(hwnd);
           break;
     case IDM_DECIMAL:
           {
               UINT fuFormat;
               _pModel->GetFormat(fuFormat);
               fuFormat ^= DECIMAL;
               CheckMenuItem (GetMenu(hwnd), IDM_DECIMAL, 
                        fuFormat & DECIMAL ? MF_CHECKED : MF_UNCHECKED ) ;
              _pModel->SetFormat(fuFormat);
           }
           _pView->Invalidate(hwnd);
           break;
      case IDM_ALLPAGES:
           {
               UINT fPageMode;
               _pModel->GetPageMode(fPageMode);
               fPageMode ^= ALLPAGES;
               CheckMenuItem (GetMenu(hwnd), IDM_ALLPAGES, 
                        fPageMode & ALLPAGES ? MF_CHECKED : MF_UNCHECKED ) ;
               _pModel->SetPageMode(fPageMode);
           }
           _pView->Invalidate(hwnd);
           break;

      // Help Menu
      case IDM_HELP:
           AlertBox ( hwnd, IDS_NOTIMPLEM, MB_ICONINFORMATION | MB_OK);
           break;
      case IDM_ABOUT:
           DialogBox ( _hInst, TEXT("AboutBox"), hwnd, _funAbout );
           break;
     }
}

void CController::Page(HWND hwnd, WPARAM wParam)
{
    UINT iPage = _pModel->GetPage();
    switch (wParam)
    {
    case IDM_PREVPAGE: 
        _pModel->PrevPage();
        break;  
    case IDM_NEXTPAGE:
        _pModel->NextPage();
        break;  
    case IDM_NEXTSECTION:
        _pModel->NextSection();
        break;
    case IDM_PREVSECTION:
        _pModel->PrevSection();
        break;
    case IDM_FIRSTPAGE:
        _pModel->SetPage( 0 );
        break;  
    case IDM_LASTPAGE:
        _pModel->SetPage( _pModel->GetMaxPage() -1 );
        break;  
    }
    if (iPage != _pModel->GetPage())
    {   
        _pView->Invalidate(hwnd);
    }
    HMENU hmenu = GetMenu(hwnd);

    EnableMenuItem (hmenu, IDM_PREVPAGE, _pModel->CanPrevPage()  ?
                      MF_ENABLED : MF_DISABLED | MF_GRAYED ) ;
    EnableMenuItem (hmenu, IDM_NEXTPAGE, _pModel->CanNextPage() ?
                      MF_ENABLED : MF_DISABLED | MF_GRAYED ) ;
    EnableMenuItem (hmenu, IDM_NEXTSECTION, _pModel->CanNextSection() ?
                      MF_ENABLED : MF_DISABLED | MF_GRAYED ) ;
    EnableMenuItem (hmenu, IDM_PREVSECTION,_pModel->CanPrevSection() ?
                      MF_ENABLED : MF_DISABLED | MF_GRAYED ) ;
}

void CController::ButtonDown(HWND hwnd, LONG lParam )
{
    static RECT rc;
    SetCapture (hwnd);

    if( _pBox )
    {
        delete _pBox;
        _pBox = 0;
		_pView->Invalidate(hwnd, &rc);
        return;
    }
    
    SIZE size = { 4*(INCH1-INCH8)/5, INCH1 /*-INCH8*/ };
    CBoxFormat bxf(size);

    CScreenCanvas canvas(hwnd);
    
    POINT pt = {LOWORD(lParam), HIWORD(lParam)};

    UINT iChar = _pView->Hittest(canvas, pt, _pModel);
    
    if( iChar == 0xFFFF )
    {
        return;
    }
    HFONT hfont = _pModel->GetFont();
    _pBox = new CBox(bxf, iChar, hfont); 

    canvas.DPtoLP(&pt);
    pt.x -= size.cx/2;
    pt.y -= size.cy/2;
#ifdef UNICODE
    pt.x -= size.cx; // accommodate wider popup
#endif

    GetClientRect(hwnd, &rc);

    _pBox->Paint(canvas, pt, rc);
    size = _pBox->GetSize();
    
	POINT ptSize;
	ptSize.x = size.cx;
	ptSize.y = size.cy;
	canvas.LPtoDP(&ptSize);
	pt.x+=ptSize.x;
	pt.y+=ptSize.y;
}

void CController::ButtonUp(HWND hwnd, LONG lParam )
{
    ReleaseCapture();
}

void CController::KeyDown(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    switch( wParam )
    {
    case VK_PRIOR:
         Page(hwnd, IDM_PREVPAGE);
         break;
    case VK_NEXT:
         Page(hwnd, IDM_NEXTPAGE);
         break;
    case VK_HOME:
         Page(hwnd, IDM_FIRSTPAGE);
         break;
    case VK_END:
         Page(hwnd, IDM_LASTPAGE);
         break;
    case VK_UP:
         _pView->SetVScrollPos(hwnd, SB_LINEUP, lParam, _pModel);       
         break;
    case VK_DOWN:
         _pView->SetVScrollPos(hwnd, SB_LINEDOWN, lParam, _pModel);     
         break;
    case VK_LEFT:
         _pView->SetHScrollPos(hwnd, SB_LINEUP, lParam, _pModel);       
         break;
    case VK_RIGHT:
         _pView->SetHScrollPos(hwnd, SB_LINEDOWN, lParam, _pModel);     
         break;
    }
}

void CController::KeyUp(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
}

void CController::HScroll(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	if (_pBox)
	{
		// Remove box by simulating second mouse click
		ButtonDown(hwnd, 0); 
		UpdateWindow(hwnd);
	}
    _pView->SetHScrollPos(hwnd, wParam, lParam, _pModel);       
}

void CController::VScroll(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	if (_pBox)
	{
		// Remove box by simulating second mouse click
		ButtonDown(hwnd, 0); 
		UpdateWindow(hwnd);
	}
    _pView->SetVScrollPos(hwnd, wParam, lParam, _pModel);       
}

// Main application

// NOTE: for October Beta needed to declare WinMain with HANDLE instead
//       of HINSTANCE, which requires the two casts further down
//       (compiler internally faults otherwise)


// WinMain - Main window funtion
//
int PASCAL WinMain
   ( HANDLE hInst, HANDLE hPrevInst, LPSTR cmdParam, int cmdShow )
{
    TCHAR  szAppName [] = TEXT("Grid") ;
    TCHAR  szCaption [] = TEXT("Character Grid") ;

    // Create Window Class
 
    if ( hPrevInst == 0 )
    {
        MakeWindowClass ( WndProc, szAppName, (HINSTANCE) hInst );
    }

    // Create Window
    
    CWindow win ( szCaption, szAppName, (HINSTANCE) hInst );
    
    win.Show ( cmdShow );
    
    MSG  msg;
    
    while ( GetMessage (&msg, NULL, 0, 0 ) )
    {
        TranslateMessage ( &msg );
        DispatchMessage ( &msg );
    }
    
    return msg.wParam;
}

// Make Window Class
//
void MakeWindowClass ( WNDPROC WndProc, LPTSTR szAppName, HINSTANCE hInst )
{
    WNDCLASS cl;
   
    cl.style = CS_HREDRAW | CS_VREDRAW;
    cl.lpfnWndProc = WndProc;
    cl.cbClsExtra = 0;
    cl.cbWndExtra = 0;
    cl.hInstance = hInst;
    cl.hIcon = LoadIcon ( hInst, szAppName );
    cl.hCursor = LoadCursor ( NULL, IDC_ARROW );
    cl.hbrBackground = GetStockBrush ( WHITE_BRUSH );
    cl.lpszMenuName = szAppName;
    cl.lpszClassName = szAppName;
   
    RegisterClass (&cl);
}

// Window Proc
//

LRESULT CALLBACK WndProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    static CController ctrl;

    switch (message)
    {
        case WM_CREATE:
            ctrl.Create(hwnd, lParam);
            return 0;
        case WM_SIZE:
            ctrl.Size(lParam);
            return 0;
        case WM_PAINT:
            ctrl.Paint(hwnd);
            return 0;
        case WM_COMMAND:
            ctrl.Command(hwnd, wParam);
            return 0;
        case WM_LBUTTONUP:
            ctrl.ButtonUp(hwnd, lParam);
            return 0;
        case WM_LBUTTONDOWN:
            ctrl.ButtonDown(hwnd, lParam);
            return 0;
        case WM_KEYUP:
            ctrl.KeyUp(hwnd, wParam, lParam);
            return 0;
        case WM_KEYDOWN:
            ctrl.KeyDown(hwnd, wParam, lParam);
            return 0;
        case WM_VSCROLL:
            ctrl.VScroll(hwnd, wParam, lParam );
            return 0;
        case WM_HSCROLL:
            ctrl.HScroll(hwnd, wParam, lParam );
            return 0;
        case WM_DESTROY:
            ctrl.Destroy();
            return 0;

    }
    return DefWindowProc (hwnd, message, wParam, lParam );
}

// "About" dialog box procedure
// Process messages from dialog box
//

BOOL CALLBACK AboutDlgProc
   ( HWND hwnd, UINT message, WPARAM wParam, LONG lParam )
{

    switch(message)
    {
       case WM_INITDIALOG:
            return TRUE;
       case WM_COMMAND:
            switch (wParam) //Command ID
            {
               case IDOK:
               case IDCANCEL:
                    EndDialog(hwnd, 0);
                    return TRUE;
            }
            break;
    }
    return FALSE;
}

extern CPrintAux PrGlobal;

//======== PrintDlgProc ===========================================

BOOL CALLBACK PrintDlgProc
   ( HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    switch (message)
    {
        case WM_INITDIALOG:
             SetWindowText (hwndDlg, (TCHAR *)lParam) ;
             EnableMenuItem (GetSystemMenu (hwndDlg, FALSE), SC_CLOSE, MF_GRAYED) ;
             return TRUE ;

        case WM_COMMAND:
             PrGlobal._bUserAbort = TRUE ;
             EnableWindow (GetParent (hwndDlg), TRUE) ;
             DestroyWindow (hwndDlg) ;
             PrGlobal._hDlgPrint = 0 ;
             return TRUE ;
    }
    return FALSE ;
}

//======== AbortProc ===========================================

BOOL CALLBACK AbortProc(HDC hdcPrn, short nCode)
{
    MSG   msg ;

    while (!PrGlobal._bUserAbort && PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (!PrGlobal._hDlgPrint 
                || 
            !IsDialogMessage (PrGlobal._hDlgPrint, &msg))
        {
            TranslateMessage (&msg) ;
            DispatchMessage (&msg) ;
        }
    }
    return !PrGlobal._bUserAbort ;
}


