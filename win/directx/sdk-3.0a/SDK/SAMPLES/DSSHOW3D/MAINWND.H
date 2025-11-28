#ifndef __MAINWND_H__
#define __MAINWND_H__

// Pickup the ASSERT_HWND() macro
#include "DbList.h"

class MainWnd
{
    friend LRESULT CALLBACK MainWndProc(HWND, unsigned, WPARAM, LPARAM);
public:
    MainWnd();
    ~MainWnd();
    
    BOOL Create();
    BOOL BatchOpenFiles( PSTR *, int, BOOL, BOOL );

    void UpdateStatus( void );
    inline HWND GetHwnd()       { return m_hwnd; }

    void DuplicateBuffer( FileInfo *pfiSource );
    void DestroyFileInfo( FileInfo *pfi );

    // For convenience, we redefine some global window related functions as
    // members of this class since we can automagically validate and pass the
    // HWND for the class object.
    BOOL UpdateWindow()  { ASSERT_HWND( m_hwnd );
                    return ::UpdateWindow( m_hwnd ); }
    BOOL ShowWindow( int cmdShow )  { ASSERT_HWND( m_hwnd );
                    return ::ShowWindow( m_hwnd, cmdShow ); }
    LRESULT SendMessage( UINT m, WPARAM w, LPARAM l )
        { ASSERT_HWND( m_hwnd ); return ::SendMessage( m_hwnd, m, w, l ); }
    LRESULT PostMessage( UINT m, WPARAM w, LPARAM l )
        { ASSERT_HWND( m_hwnd ); return ::PostMessage( m_hwnd, m, w, l ); }
    int MessageBox( LPCSTR, UINT uType = MB_OK | MB_APPLMODAL );
    int MessageBox( UINT uResID, UINT uType = MB_OK | MB_APPLMODAL );

protected:
    void OnDestroy( void );
    BOOL OnCommand( WPARAM, LPARAM );
    BOOL OnTimer( WPARAM, LPARAM );
    BOOL OnPaint( WPARAM, LPARAM );
    BOOL OnDropFiles( WPARAM );
    BOOL OnInitMenu( WPARAM );

    FileInfo *OnFileOpen( LPSTR );

protected:
    HWND    m_hwnd;
    BOOL    m_fCreated;
    int     m_n3DBuffers;
    DSCAPS  m_dscaps;
    
    DbLinkedList<class FileInfo *>    m_dlInfoList;

};
    


#endif


