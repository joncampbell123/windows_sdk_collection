#include <assert.h>
#include <windows.h>
#include <imm.h>
#include "fullime.h"

static DWORD CompColor[ 4 ] = { RGB( 255, 0, 0 ),
                                RGB( 255, 0 , 255 ),
                                RGB( 0, 0, 255 ),
                                RGB( 0, 255, 0 ) };

//**********************************************************************
//
// void ImeUIStartComposition()
//
// This handles WM_IME_STARTCOMPOSITION message. 
//
//**********************************************************************

void ImeUIStartComposition( HWND hwnd )
{
        
    //
    // Change caption title to DBCS composition mode.
    //

    SetWindowText( hwnd, (LPSTR)szSteCompTitle );    

    //
    // Reset global variables.
    // 

    gImeUIData.uCompLen = 0;             // length of composition string.

    gImeUIData.ImeState |= IME_IN_COMPOSITION;
}

//**********************************************************************
//
// void ImeUIComposition()
//
// This handles WM_IME_COMPOSITION message. It here just handles 
// composition string and result string. For normal case, it should 
// examine all posibile flags indicated by CompFlag, then do some
// actitions to reflect what kinds of composition info. IME conversion
// engine informs.
//
//**********************************************************************

void ImeUIComposition( HWND hwnd, WPARAM wParam, LPARAM CompFlag )
{

    if ( CompFlag & GCS_RESULTSTR )
         GetResultStr( hwnd );
    else
    if ( CompFlag & GCS_COMPSTR )
         GetCompositionStr( hwnd, CompFlag );

}

//**********************************************************************
//
// void GetCompositionStr()
//
// This handles WM_IME_COMPOSITION message with GCS_COMPSTR flag on.
//
//**********************************************************************

void GetCompositionStr( HWND hwnd, LPARAM CompFlag )
{
     DWORD      dwBufLen;               // Stogare for len. of composition str
     LPSTR      lpCompStr;              // Pointer to composition str.
     HIMC       hIMC;                   // Input context handle.
     HLOCAL     hMem;                   // Memory handle.
     LPSTR      lpCompStrAttr;          // Pointer to composition str array.
     HLOCAL     hMemAttr;               // Memory handle for comp. str. array.
     DWORD      dwBufLenAttr;
    
     //
     // If fail to get input context handle then do nothing.
     // Applications should call ImmGetContext API to get
     // input context handle.
     //

     if ( !( hIMC = ImmGetContext( hwnd ) ) )
         return;

     //
     // Determines how much memory space to store the composition string.
     // Applications should call ImmGetCompositionString with
     // GCS_COMPSTR flag on, buffer length zero, to get the bullfer
     // length.
     //

     if ( ( dwBufLen = ImmGetCompositionString( hIMC, GCS_COMPSTR, 
                                         (void FAR*)NULL, 0l ) ) < 0 )
         goto exit2;

     //
     // Allocates memory with dwBufLen+1 bytes to store the composition
     // string. Here we allocale on more byte to put null character.
     //

     if ( !( hMem = LocalAlloc( LPTR, (int)dwBufLen + 1 ) ) )
         goto exit2;

     if ( !( lpCompStr = (LPSTR) LocalLock( hMem ) ) )
         goto exit1;

     //
     // Reads in the composition string.
     //

     ImmGetCompositionString( hIMC, GCS_COMPSTR, lpCompStr, dwBufLen );

     //
     // Null terminated.
     //

     lpCompStr[ dwBufLen ] = 0;

     //
     // If GCS_COMPATTR flag is on, then we need to take care of it.
     //

     if ( CompFlag & GCS_COMPATTR )
     {

         if ( ( dwBufLenAttr = ImmGetCompositionString( hIMC, GCS_COMPATTR,
                               ( void FAR *)NULL, 0l ) ) < 0 )
             goto nothing;

         //
         // Allocate memory to store attributes of composition strings.
         //

         if ( !( hMemAttr = LocalAlloc( LPTR, (int)dwBufLenAttr + 1 ) ) )
             goto nothing;

         if ( !( lpCompStrAttr = (LPSTR) LocalLock( hMemAttr ) ) )
         {
             LocalFree( hMemAttr );
             goto nothing;
         }

         //
         // Reads in the attribute array.
         //

         ImmGetCompositionString( hIMC, GCS_COMPATTR, lpCompStrAttr,
                                  dwBufLenAttr );

         lpCompStrAttr[ dwBufLenAttr ] = 0;

     } else
     {

nothing:
         lpCompStrAttr = NULL;
     }


     //
     // Display new composition chars.
     //

     DisplayCompString( hwnd, lpCompStr, lpCompStrAttr );

     //
     // Keep the length of the composition string for using later.
     //

     gImeUIData.uCompLen = (UINT)dwBufLen;

     LocalUnlock( hMem );

     if ( lpCompStrAttr )
     {
         LocalUnlock( hMemAttr );
         LocalFree( hMemAttr );
     }

exit1:

     LocalFree( hMem );

exit2:

     ImmReleaseContext( hwnd, hIMC );

}


//***********************************************************************
//
// void GetResultStr()
//
// This handles WM_IME_COMPOSITION with GCS_RESULTSTR flag on.
//
//***********************************************************************

void GetResultStr( HWND hwnd )
{
    DWORD       dwBufLen;               // Storage for length of result str.
    LPSTR       lpResultStr;            // Pointer to result string.
    HIMC        hIMC;                   // Input context handle.
    HLOCAL      hMem;                   // Memory handle.

    //
    // If fail to get input context handle then do nothing.
    //

    if ( !( hIMC = ImmGetContext( hwnd ) ) )
        return;

    //
    // Determines how much memory space to store the result string.
    // Applications should call ImmGetCompositionString with
    // GCS_RESULTSTR flag on, buffer length zero, to get the bullfer
    // length.
    //

    if ( ( dwBufLen = ImmGetCompositionString( hIMC, GCS_RESULTSTR,
                                  (void FAR *)NULL, (DWORD) 0 ) ) <= 0 )
        goto exit2;

    //
    // Allocates memory with dwBufLen+1 bytes to store the result
    // string. Here we allocale on more byte to put null character.
    //

    if ( !( hMem = LocalAlloc( LPTR, (int)dwBufLen + 1 ) ) )
        goto exit2;

    if ( !( lpResultStr = (LPSTR) LocalLock( hMem ) ) )
        goto exit1;

    //
    // Reads in the result string.
    //

    ImmGetCompositionString( hIMC, GCS_RESULTSTR, lpResultStr, dwBufLen );

    //
    // Displays the result string.
    //

    DisplayResultString( hwnd, lpResultStr );

    LocalUnlock( hMem );

exit1:
    
    LocalFree( hMem );

exit2:

    ImmReleaseContext( hwnd, hIMC );

}


//**********************************************************************
//
// void ImeUIEndComposition
//
// This handles WM_IME_ENDCOMPOSITION message.
//
//**********************************************************************

void ImeUIEndComposition( HWND hwnd )
{
        
    RECT       rect;

    //
    // Change caption title to normal
    //

    SetWindowText( hwnd, (LPSTR)szSteTitle );

    //
    // Update client area.
    //

    GetClientRect( hwnd, (LPRECT)&rect );

    InvalidateRect( hwnd, (LPRECT)&rect, FALSE );

    //
    // Reset the length of composition string to zero.
    //

    gImeUIData.uCompLen = 0;

    gImeUIData.ImeState &= ~IME_IN_COMPOSITION;

}

//**********************************************************************
//
// BOOL ImeUINotify()
//
// This handles WM_IME_NOTIFY message.
//
//**********************************************************************

BOOL ImeUINotify( HWND hwnd, WPARAM wParam, LPARAM lParam )
{
    switch (wParam )
    {
        case IMN_OPENCANDIDATE:

            ImeUIOpenCandidate( hwnd, lParam );
            break;

        case IMN_CLOSECANDIDATE:

            ImeUICloseCandidate( hwnd, lParam );
            break;

        case IMN_CHANGECANDIDATE:

            ImeUIChangeCandidate( hwnd, lParam );
            break;

        case IMN_SETOPENSTATUS:

            ImeUISetOpenStatus( hwnd );
            break;

	default:
	    return FALSE;

    }
    
    return TRUE;
}

//**********************************************************************
//
// void ImeUIOpenCandidate()
//
// This handles WM_IME_NOTIFY message with wParam = IMN_OPENCANDIDATE.
//
//**********************************************************************

void ImeUIOpenCandidate( HWND hwnd, LPARAM CandList )
{
    HIMC        hIMC;                   // Input context handle.
    LPCANDIDATELIST lpCandList;         // Storage for LP to candidate list.
    DWORD       dwBufLen;               // Storage for candidate strings.
    LPSTR       lpStr;                  // Storage for LP to a string.
    DWORD       dwIndex;                // Storage for index of ListCand array
    DWORD       i;                      // Loop count.
    int         width = 0;              // Storage for width of listCand
    int         CurNumCandList = 0;     // Storage for number of cand. lists.
    DWORD       dwPreferNumPerPage;     // Storage for PreferNumPerPage
    POINT       point;                  // Storage for caret position.

    //
    // If fail to get input context handle, do nothing.
    //

    if ( ! (hIMC = ImmGetContext( hwnd ) ) )
        return;

    //
    // Change caption title to DBCS candidate mode.
    //

    SetWindowText( hwnd, (LPSTR)szSteCandTitle );

    //
    // Find out how many candidate windows have already been opened.
    //

    for( i = 0; i < MAX_LISTCAND; i++ )
    {
        if ( gImeUIData.hListCand[ i ] )
            CurNumCandList++;
    }

    //
    // Check which candidate lists should be displayed by loopping
    // through all possible candidate lists.
    //

    for( dwIndex = 0; dwIndex < MAX_LISTCAND ; dwIndex ++ )
    {

        if ( CandList & ( 1 << dwIndex ) )
        {   
            //
            // The dwIndex-th candidate list contains candidate strings.
            // So here we want to display them.
            //

            //
            // Determines how musch memory space should be allocated to
            // read in the corresponding candidate list .
            //

            if ( ! ( dwBufLen = ImmGetCandidateList( hIMC, dwIndex, 
                       lpCandList, 0 ) ) )                      
                goto exit2;
                       
            
            //
            // Allocate memory space.
            //

            if( !( gImeUIData.hListCandMem[ dwIndex ]  = 
                   GlobalAlloc( LPTR, (int)dwBufLen ) ) )
                goto exit2;

            if( !( lpStr =
                 (LPSTR)GlobalLock( gImeUIData.hListCandMem[ dwIndex ] ) ) )
            {   
                GlobalFree( gImeUIData.hListCandMem[ dwIndex ] );
                gImeUIData.hListCandMem[ dwIndex ] = NULL;
                goto exit2;
            }
                    
            lpCandList = (LPCANDIDATELIST) lpStr;

            //
            // Reads in the corresponding candidate list.
            //

            ImmGetCandidateList( hIMC, dwIndex, lpCandList, dwBufLen );

            //
            // Get current caret position.
            //

            GetCaretPos( (POINT FAR*)&point );
            ClientToScreen( hwnd, (LPPOINT)&point );

            //
            // Determines how many candidate strings per page.
            //

            dwPreferNumPerPage = ( !lpCandList->dwPageSize ) ?
                                 DEFAULT_CAND_NUM_PER_PAGE :
                                 lpCandList->dwPageSize;
            //
            // Determining maximum character length the list box
            // will display by loopping through all candidate strings.
            //

            for( i = 0; i < lpCandList->dwCount; i++ )
            {
                //
                // Get the pointer to i-th candidate string.
                //

                lpStr = (LPSTR)lpCandList + 
                        lpCandList->dwOffset[ i ];

                width = ( width < lstrlen( lpStr ) ) ? lstrlen( lpStr ) :
                          width;

            }

            //
            // Create a candidate window for the candidate list.
            //


            gImeUIData.hListCand[ dwIndex ] = CreateWindow(          
                                 (LPCTSTR)szCandClass,
                                 NULL,
                                 WS_BORDER | WS_POPUP | WS_DISABLED,
                                 CurNumCandList * X_INDENT + point.x,
                                 CurNumCandList * Y_INDENT + 
                                 point.y + cyMetrics,
                                 ( width ) * cxMetrics + 10,
                                 (int)(dwPreferNumPerPage) * cyMetrics + 5,,
                                 hwnd,
                                 (HMENU)NULL,
                                 (HANDLE)GetWindowLong( hwnd, GWL_HINSTANCE ),
                                 (LPVOID)NULL
                                 );
            //
            // If fail to create the candidate window then do nothing.
            //

            if ( gImeUIData.hListCand[ dwIndex ] < 0 )
            {
                GlobalUnlock( gImeUIData.hListCandMem[ dwIndex ] );
                GlobalFree( gImeUIData.hListCandMem[ dwIndex ] );
                goto exit2;
            }

 
            //
            // Show candidate window.
            //

            ShowWindow( gImeUIData.hListCand[ dwIndex ], SW_SHOWNOACTIVATE );

            //
            // Display candidate strings.
            //

            DisplayCandStrings( gImeUIData.hListCand[ dwIndex ], lpCandList );

            GlobalUnlock( gImeUIData.hListCandMem[ dwIndex ] );

            CurNumCandList++;
        }
    }

    //
    // Reset IME state.
    //

    gImeUIData.ImeState |= IME_IN_CHOSECAND;

exit2:

    ImmReleaseContext( hwnd, hIMC );

}

//**********************************************************************
//
// void ImeUICloseCandidate()
//
// This handles WM_IME_NOTIFY message with
// wParam = IMN_CLOSECANDIDATE.
//
//**********************************************************************

void ImeUICloseCandidate( HWND hwnd, LPARAM CandList )
{
    int         index;

    //
    // Change window's caption title to normal.
    //

    SetWindowText( hwnd, (LPSTR)szSteCompTitle );

    //
    // If the i-th bit on of CandList that means the i-th
    // candidate list should be closed.
    //

    for( index = 0; index < MAX_LISTCAND; index ++ )
    {
        if (( CandList & ( 1 << index ) ) && gImeUIData.hListCand[ index ])
        {

            //
            // Destroy the candidate window.
            //

            DestroyWindow( gImeUIData.hListCand[ index ] );

            gImeUIData.hListCand[ index ] = NULL;

            //
            // Free memory.
            //

            GlobalFree( gImeUIData.hListCandMem[ index ] );

            gImeUIData.hListCandMem[ index ] = NULL;

        }
    }

    gImeUIData.ImeState &= ~IME_IN_CHOSECAND;

}

//**********************************************************************
//
// void ImeUIChangeCandidate()
//
// This handles WM_IME_NOTIFY message with wParam = IMN_CHANGECANDIDATE.
//
//**********************************************************************

void ImeUIChangeCandidate( HWND hwnd, LPARAM CandList )
{
    HIMC            hIMC;
    LPCANDIDATELIST lpCandList = NULL;         
    DWORD           dwIndex;
    DWORD 	    dwBufLen;
    LPSTR	    lpStr;
    DWORD	    i;
    RECT	    rect;
    int		    width = 0;
    DWORD	    dwPreferNumPerPage;


    //
    // If fail to get input context, do nothing.
    //

    if ( !( hIMC = ImmGetContext( hwnd ) ) )
        return;

    //
    // Determine which candidate list should be updated.
    //

    for ( dwIndex = 0; dwIndex < MAX_LISTCAND; dwIndex++ )
        if ( CandList & ( 1 << dwIndex ) )
            break;

    //
    // If dwIndex == MAX_LISTCAND, then something wrong, do nothing.
    //

    if ( dwIndex == MAX_LISTCAND )
        return;

    //
    // Determines how much memory space should be allocated to read in the
    // corresponding candidate list.
    //

    if ( !( dwBufLen = ImmGetCandidateList( hIMC, dwIndex, lpCandList, 0 ) ) )
	goto exit2;

    //
    // Relocate memory space.
    //

    if ( !( gImeUIData.hListCandMem[ dwIndex ] = GlobalReAlloc(
	    gImeUIData.hListCandMem[ dwIndex ], (int)dwBufLen, LPTR ) ) )
	goto exit2;

    if ( !( lpStr = 
	(LPSTR)GlobalLock( gImeUIData.hListCandMem[ dwIndex ] ) ) )
    {
	GlobalFree( gImeUIData.hListCandMem[ dwIndex ] );
        gImeUIData.hListCandMem[ dwIndex ] = NULL;
	goto exit2;
    }

    lpCandList = (LPCANDIDATELIST) lpStr;

    //
    // Reads in the corresponding candidate list.
    //

    ImmGetCandidateList( hIMC, dwIndex, lpCandList, dwBufLen );

    //
    // Determines how many candidate strings per page.
    //

    dwPreferNumPerPage = ( !lpCandList->dwPageSize ) ?
                         DEFAULT_CAND_NUM_PER_PAGE :
                         lpCandList->dwPageSize;
    // 
    // Determining maximum character length  the list box
    // will display by loopping through all candidate strings.
    //

    for( i = 0; i < lpCandList->dwCount; i++ )
    {
        //
        // Get the pointer to i-th candidate string.
        //

        lpStr = (LPSTR)lpCandList + 
                lpCandList->dwOffset[ i ];

        width = ( width < lstrlen( lpStr ) ) ? lstrlen( lpStr ) :
                  width;

    }

    GetWindowRect( gImeUIData.hListCand[ dwIndex ] , (LPRECT) &rect);

    SetWindowPos( gImeUIData.hListCand[ dwIndex ],
		  hwnd,
		  rect.left,
		  rect.top,
		  ( width ) * cxMetrics + 10,
		  (int)(dwPreferNumPerPage) * cyMetrics + 5,
		  SWP_NOZORDER | SWP_NOACTIVATE );
		  

    DisplayCandStrings( gImeUIData.hListCand[ dwIndex ], lpCandList );

    GlobalUnlock( gImeUIData.hListCandMem[ dwIndex ] );

exit2:

    return;

}


//***********************************************************************
//
// void ImeUISetOpenStatus()
//
// This handles WM_IME_REPORT message with wParam = IR_NOTIFY &
// lParam = IMC_SETOPENSTATUS.
//
//**********************************************************************

void ImeUISetOpenStatus( HWND hwnd )
{
    int         i;       // Lopp counter
    HIMC        hIMC;    // Storage for input context handle.

    //
    // If fail to get input context handle then do nothing
    //

    if ( !( hIMC = ImmGetContext( hwnd ) ) )
        return;

    if ( ImmGetOpenStatus( hIMC ) )
    {

        //
        // If the IME conversion engine is open, here we change
        // window's caption title to DBCS composition mode.
        // 

        SetWindowText( hwnd, (LPSTR)szSteCompTitle );

    } else
    {
        RECT    rect;        
        
        //
        // If the IME conversion engine is closed, here we
        // erase all already displayed composition chars if any,
        // change the window's caption title to normal.
        //

        GetClientRect( hwnd, (LPRECT)&rect );
        InvalidateRect( hwnd, (LPRECT)&rect, FALSE );

        SetWindowText( hwnd, (LPSTR)szSteTitle );

        //
        // Here we close and destroy all of candidate windows
        // if IME conversion engine is closed.
        //

        for( i = 0; i <= MAX_LISTCAND; i++ )
        {
            if ( gImeUIData.hListCand[ i ] ) 
            {
                DestroyWindow( gImeUIData.hListCand[ i ] );
                gImeUIData.hListCand[ i ] = NULL;
                GlobalFree( gImeUIData.hListCandMem[ i ] );
                gImeUIData.hListCandMem[ i ] = NULL;
            }
        }

        //
        // Reset IMEUI's global data. 
        //

        gImeUIData.uCompLen = 0;
        gImeUIData.ImeState = 0;

        ResetCaret( hwnd );

    }

    ImmReleaseContext( hwnd, hIMC );
}

//*********************************************************************
//
// void DisplayCompString()
//
// This displays composition string.
//
// This function supports only fixed pitch font.
//
//*********************************************************************

void DisplayCompString( HWND hwnd, LPSTR lpStr, LPSTR lpStrAttr )
{
    HDC         hdc;                            
    int         StrLen = lstrlen( lpStr );
    RECT        rect;
    DWORD       dwColor;
    int         i;


    hdc = GetDC( hwnd );

    HideCaret( hwnd );

    //
    // Determine OPAQUE rect.
    //

    rect.left = xPos * cxMetrics;
    rect.top  = yPos * cyMetrics;
    rect.bottom = rect.top + cyMetrics;
    rect.right = ( (int)gImeUIData.uCompLen > StrLen ) ?
                 ( xPos + gImeUIData.uCompLen ) * cxMetrics:
                 ( xPos + StrLen ) * cxMetrics;
   
    //
    // This example we use red to display composition chars
    // with attribute 000, pink for attribute 001,
    // blue for attribute 010, green for attribute 011.
    //

    //
    // Each composition character has different attribute.
    // We here use different kinds of color to represent attributes.
    // Red, pink, blue and green are for attribute 000, 001, 010 and 011,
    // respectively.
    //
  

    dwColor = GetTextColor( hdc );

    if ( !lpStrAttr )
    {
        //
        // If there are not attribute array, here we use default color, RED,
        // to display all of composition characters.
        //

        SetTextColor( hdc, CompColor[ 0 ] ); // default color

        ExtTextOut( hdc, xPos * cxMetrics, yPos * cyMetrics,
                ETO_OPAQUE, &rect, lpStr, StrLen, 0 );

    } else
    {

        int  ColorIndex;

        ExtTextOut( hdc, xPos * cxMetrics, yPos * cyMetrics, ETO_OPAQUE,
                    &rect, NULL, 0, 0 );

        for( i = 0; *lpStr;)
        {
            int cnt = IsDBCSLeadByte(*lpStr) ? 2 : 1;

            ColorIndex = ( ((int)*lpStrAttr) < 0 ) ? 0 : (int)*lpStrAttr;
            ColorIndex = ( ColorIndex > 3 ) ? 3 : ColorIndex;

            SetTextColor( hdc, CompColor[ ColorIndex ] );
            
            TextOut( hdc, ( i + xPos ) * cxMetrics, yPos * cyMetrics,
                     lpStr, cnt );
            lpStr += cnt;
            lpStrAttr += cnt;
            i += cnt;
        }
    }


    SetTextColor( hdc, dwColor );

    SetCaretPos( ( xPos + StrLen ) * cxMetrics, yPos * cyMetrics );

    ShowCaret( hwnd );

    ReleaseDC( hwnd, hdc );

}

//*********************************************************************
//
// void DisplayResultString()
//
// This displays result string.
//
// This function supports only fixed pitch font.
//
//*********************************************************************

void DisplayResultString( HWND hwnd, LPSTR lpStr )
{

    int         StrLen;         // Storage for string length.
    int         i;              // Loop counter.
    HDC         hdc;            // Display context handle.

    StrLen = lstrlen( lpStr );

    //
    // If there is no room for compsoition string, discard it
    //

    if ( xPos == ( LASTCOL - StrLen - 1 ) )
        return;

    //
    // if insert mode or during composition session,
    // move rest of line to the right by StrLen bytes.
    //

    if ( fInsertMode )
    {

        for( i = LASTCOL; i > xPos; i-- )
            textbuf[ yPos ][ i ] = textbuf[ yPos ][ i - StrLen ];

        //
        // If the row ends on a lead byte, blank it out,
        // To do this we must first traverse the string starting
        // from a know character boundry until we reach the last column.
        // If the last column is a character boundry then the last 
        // character is either a string byte or a lead byte.
        //

        for( i = xPos + StrLen; i < LASTCOL; )
        {
            if ( IsDBCSLeadByte( textbuf[ yPos ][ i ] ) )
                i++;
            i++;
        }

        if ( i == LASTCOL && IsDBCSLeadByte( textbuf[ yPos ][ i ] ) )
            textbuf[ yPos ][ LASTCOL ] = ' ';

    } else
    {
        //
        // overtype mode
        //

        if ( !IsDBCSLeadByte( textbuf[ yPos ][ xPos + StrLen - 2 ] ) )
        {
            //
            // Overtyping the current byte, plus the following byte
            // which could be a lead byte.
            //

            if ( IsDBCSLeadByte( textbuf[ yPos ][ xPos + StrLen - 1 ] ) )
                textbuf[ yPos ][ xPos + StrLen ] = ' ';
        }
    }

    //
    // Store input character at current caret position.
    //

    for( i = 0; i <= LASTCOL && *lpStr; i++ )
        textbuf[ yPos ][ xPos + i ] = *lpStr++;

    //
    // Display input character
    //


    hdc = GetDC( hwnd );

    HideCaret( hwnd );

    //
    // Displays result string with normal color.
    //

    TextOut( hdc, xPos *cxMetrics, yPos * cyMetrics,
             &( textbuf[ yPos][xPos] ), MAXCOL - xPos );


    ShowCaret( hwnd );

    ReleaseDC( hwnd, hdc );

    //
    // Reset Caret position
    //

    xPos += StrLen;

    if ( xPos > LASTCOL )
        xPos = LASTCOL;

    ResetCaret( hwnd );

    gImeUIData.uCompLen = 0;

}

//**********************************************************************
//
// void RestoreImeUI()
//
// This repaints all displayed composition string if need.
// Main window procedure will call this upon receiving
// WM_PAINT message.
//
//**********************************************************************

void RestoreImeUI( HWND hwnd )
{
    HIMC        hIMC;           // Storage for input context handle.
    DWORD       dwBufLen;       // 

    
    //
    // If fail to get input context handle then do nothing.
    //

    if ( !( hIMC = ImmGetContext( hwnd ) ) )
        return;

    //
    // If IME conversion engine is open and there are any composition
    // string in the context then we redisplay them.
    //

    if ( ImmGetOpenStatus( hIMC ) && gImeUIData.ImeState &&
         ( dwBufLen = ImmGetCompositionString( hIMC, GCS_COMPSTR,
                      (void FAR*)NULL, 0l ) ) > 0 )
    {
        LPSTR       lpCompStr;     // Pointer to composition string
        HLOCAL      hMem;          // Storage for memory handle.
        LPSTR       lpCompStrAttr; // Pointer to composition string's attribute
        DWORD       dwBufLenAttr;  // 
        HLOCAL      hMemAttr;      // Memory handle for composition string's
                                   // attributes.

        //
        // If fail to allocate and lock memory space for reading in
        // the composition string then do nothing.
        //

        if ( !( hMem = LocalAlloc( LPTR, (int)dwBufLen + 1 ) ) )
            goto exit2;

        if( !( lpCompStr = (LPSTR) LocalLock( hMem ) ) )
        {
            LocalFree( hMem );
            goto exit2;
        }

        //
        // Get composition string and redisplay them.
        //

        if ( ImmGetCompositionString( hIMC, GCS_COMPSTR, lpCompStr,
                                      dwBufLen ) > 0 )
        {

            //
            // MAke sure whether we need to handle composition string's
            // attributes.
            //

            if ( ( dwBufLenAttr = ( ImmGetCompositionString( hIMC,
                       GCS_COMPATTR, (void FAR*)NULL, 0l ) ) ) > 0 )
            {
                //
                // If fail to allocate and lock memory space for reading in
                // the composition string's attribute then we assume
                // no attribute array.
                //

                if ( !( hMemAttr = LocalAlloc(LPTR, (int)dwBufLenAttr + 1 )))
                    goto nothing;

                if ( !( lpCompStrAttr = (LPSTR) LocalLock( hMemAttr ) ) )
                {
                    LocalFree( hMemAttr );
                    goto nothing;
                }

                ImmGetCompositionString( hIMC, GCS_COMPATTR, lpCompStrAttr,
                                         dwBufLenAttr );

                lpCompStrAttr[ dwBufLenAttr ] = 0;
            } else
            {
nothing:
                lpCompStrAttr = NULL;
            }


            lpCompStr[ dwBufLen ] = 0;

            DisplayCompString( hwnd, lpCompStr, lpCompStrAttr );
     
        }

        LocalUnlock( hMem );
        LocalFree( hMem );
        
        if ( lpCompStrAttr )
        {
            LocalUnlock( hMemAttr );
            LocalFree( hMemAttr );
        }


    }

exit2:

    ImmReleaseContext( hwnd, hIMC );

}

//**********************************************************************
//
// void ImeUIMove()
//
// Handler routine of WM_MOVE message.
//
//*********************************************************************

void ImeUIMoveCandWin( HWND hwnd )
{

    if ( gImeUIData.ImeState & IME_IN_CHOSECAND )
    {
        POINT           point;          // Storage for caret position.
        int             i;              // loop counter.
        int             NumCandWin;     // Storage for num of cand win.
        RECT            rect;           // Storage for client rect.

        //
        // If current IME state is in chosing candidate, here we
        // move all candidate windows, if any, to the appropriate
        // position based on the parent window's position.
        //

        NumCandWin = 0;

        GetCaretPos( (LPPOINT)&point );
        ClientToScreen( hwnd, (LPPOINT)&point );

        for ( i = 0; i < MAX_LISTCAND ; i++ )
        {
            if ( gImeUIData.hListCand[ i ] )
            {
                GetClientRect( gImeUIData.hListCand[ i ], &rect );                
                
                MoveWindow( gImeUIData.hListCand[ i ], 
                            point.x + X_INDENT * NumCandWin,
                            point.y + Y_INDENT * NumCandWin + cyMetrics,
                            ( rect.right - rect.left + 1 ),
                            ( rect.bottom - rect.top + 1 ), TRUE );

                NumCandWin++;
            }
        }
    }
}

//**********************************************************************
//
// void ImeUIClearData()
//
// Handler routine of WM_IME_SELECT message.
//
//**********************************************************************

void ImeUIClearData( HWND hwnd )
{

    RECT            rect;           
    int             i;
   
    SetWindowText( hwnd, (LPSTR)szSteTitle );

    //
    // If user switches to other IME, here we destroy all candidate
    // windows which has been opened and erase all composition
    // chars if any.
    //

    for( i = 0; i < MAX_LISTCAND; i++ )
    {
        if ( gImeUIData.hListCand[ i ] )
        {
            //
            // The i-th candidate list has already been displayed,
            // destroy it and free memory which stores candidate
            // strings.
            //

            DestroyWindow( gImeUIData.hListCand[ i] );
            GlobalFree( gImeUIData.hListCandMem[ i ] );

            gImeUIData.hListCand[ i ] =
            gImeUIData.hListCandMem[ i ] = NULL;

        }
    }

    //
    // Update client area.
    //

    GetClientRect( hwnd, (LPRECT)&rect );

    InvalidateRect( hwnd, (LPRECT)&rect, FALSE );


    //
    // Reset IMEUI's global data.
    //

    gImeUIData.uCompLen = gImeUIData.ImeState = 0;

    //
    // Reset caret to the original position.
    //

    HideCaret( hwnd );
    ResetCaret( hwnd );
    ShowCaret( hwnd );

}

