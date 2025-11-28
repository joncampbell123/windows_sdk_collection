/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1993 - 1997  Microsoft Corporation.  All Rights Reserved.
 *
 **************************************************************************/

//
// DVDSampl.h: DVDGraphBuilder sample app header file
//

//
// App menu ids
//
#define IDM_SELECT           101
#define IDM_ABOUT            102
#define IDM_EXIT             103

#define IDM_FLAGBASE         110
#define IDM_HWMAX            111
#define IDM_HWONLY           112
#define IDM_SWMAX            113
#define IDM_SWONLY           114
#define IDM_NOVPE            115
#define IDM_BUILDGRAPH       121
#define IDM_PLAY             122
#define IDM_PAUSE            123
#define IDM_STOP             124
#define IDM_SLOWPLAY         125
#define IDM_FASTFWD          126
#define IDM_VERYFASTFWD      127
#define IDM_FASTRWND         128
#define IDM_VERYFASTRWND     129
#define IDM_MENU             131
#define IDM_CC               132
#define IDM_LANG             133
#define IDM_VIEWLEVEL        134
#define IDM_FULLSCRN         135

// A custom menu message by which the app will be informed of the current
// menu state.
#define IDM_USER_MENUSTATUS   200

//
// Special consts for the app
//
#define SPSTREAM_NOCHANGE    32
#define SPSTREAM_NONE        32
#define MAX_SPSTREAMID       32


//
// Version info related constant ids
//
#define DLG_VERFIRST        400
#define IDC_COMPANY         DLG_VERFIRST
#define IDC_FILEDESC        DLG_VERFIRST+1
#define IDC_PRODVER         DLG_VERFIRST+2
#define IDC_COPYRIGHT       DLG_VERFIRST+3
#define IDC_OSVERSION       DLG_VERFIRST+4
#define IDC_TRADEMARK       DLG_VERFIRST+5
#define DLG_VERLAST         DLG_VERFIRST+5

#define IDC_LABEL           DLG_VERLAST+1


//
// App string resource ids
//
#define IDS_APP_TITLE       500
#define IDS_WINDOW_TITLE    501
#define IDS_VER_INFO_LANG   502
#define IDS_VERSION_ERROR   503

//
// Playback Settings dialog ids
//
#define IDD_LANGUAGE                600
#define IDD_LANG_AUDIOLANG          601
#define IDD_LANG_SHOWSUBPIC         602
#define IDD_LANG_SPLANG             603
#define IDD_LANG_SPLANGTEXT         604
#define IDD_LANG_AUDIOLANGTEXT      605

#define IDD_VIEWLEVEL               700
#define IDD_VIEW_LEVELTEXT          701
#define IDD_VIEW_LEVELLIST          702


//
// Special user message used by the app for event notification
//
#define WM_DVDPLAY_EVENT    (WM_USER+100)


//
// Some enumerated type definitions...
//

// Cursor movement directions
typedef enum {
    Cursor_Up = 0, Cursor_Down, Cursor_Left, Cursor_Right
} CURSOR_DIR ;

// Player state
typedef enum {
    Unknown = 0, Stopped, Paused, Playing, Scanning
} PLAYER_STATE ;


//
// A class to wrap the viewing level number and names
//
class CViewLevels {
public:
    CViewLevels() ;

    int     GetCount(void)   { return m_iCount ; } ;
    LPCTSTR GetName(int i)   { return m_alpszNames[i] ; } ;
    int     GetValue(int i)  { return m_aiValues[i] ; } ;

private:
    LPCTSTR   m_alpszNames[6] ;
    int       m_aiValues[6] ;
    int       m_iCount ;
} ;


//
// A class to wrap a few language names and the corresponding 2-char codes
//
// We have only 10 languages from ISO 639 as a sample. It can be extended to
// include any other language in ISO 639.
//
class CDVDLanguages {
public:
    CDVDLanguages() ;

    BOOL GetLangString(LPTSTR lpszCode, LPTSTR lpszLang) ;

private:
    LPCTSTR  m_alpszCodes[10] ;
    LPCTSTR  m_alpszLangNames[10] ;
} ;

//
// An easy way of getting the lang code from the DVD_Subpicture/AudioATR data
//
typedef struct _LangInfo {
    WORD  wRes1 ;  // don't care, just skip 2 bytes
    WORD  wLang ;  // lang code as a WORD value
    WORD  wRes2 ;  // don't care, another 2 bytes
} LANGINFO, *PLANGINFO ;


//
// Sample DVD Playback class
//
class CSampleDVDPlay {
public:   // public methods for Windows structure to call
    CSampleDVDPlay(void) ;
    ~CSampleDVDPlay(void) ;

    void    SetAppValues(HINSTANCE hInst, LPTSTR szAppName, 
                         int iAppTitleResId) ;
    BOOL    InitInstance(int nCmdShow) ;
    BOOL    InitApplication(void) ;
    LPTSTR  GetAppName() { return m_szAppName ; } ;
    HINSTANCE GetInstance() { return m_hInstance ; } ;
    HWND    GetWindow() { return m_hWnd ; } ;

    void    BuildGraph(void) ;
    BOOL    Play(void) ;
    BOOL    Pause(void) ;
    BOOL    Stop(void) ;
    BOOL    FastForward(void) ;
    BOOL    VeryFastForward(void) ;
    BOOL    FastRewind(void) ;
    BOOL    VeryFastRewind(void) ;
    BOOL    SlowPlay(void) ;
    int     MakeSPStreamList(HWND hDlg, int iListID) ;
    ULONG   GetSPStream(void)   { return m_ulSPStream ; } ;
    BOOL    SetSPState(LONG lStream, BOOL bState) ;
    void    GetSPLangCode(DVD_SubpictureATR *pSPATR, LPTSTR lpszCode) ;
    BOOL    GetSPLanguage(ULONG ulStream, BOOL bSPOn, LPTSTR lpszLang, int iMaxLang) ;
    int     MakeAudioStreamList(HWND hDlg, int iListID) ;
    ULONG   GetAudioStream(void)   { return m_ulAudioStream ; } ;
    BOOL    SetAudioState(LONG lStream) ;
    BOOL    GetAudioLanguage(ULONG ulStream, LPTSTR lpszLang, int iMaxLang) ;
    void    GetAudioLangCode(DVD_AudioATR *pAudATR, LPTSTR lpszCode) ;
    int     MakeParentalLevelList(HWND hDlg, int iListID) ;
    ULONG   SetParentalLevel(LONG lLevel) ;
    BOOL    ShowMenu(void) ;
    BOOL    ClosedCaption(void) ;
    BOOL    ShowFullScreen(void) ;
    BOOL    StartFullScreen(void) ;
    BOOL    StopFullScreen(void) ;
    void    CursorMove(CURSOR_DIR Dir) ;
    void    CursorSelect(void) ;
    void    SetRenderFlag(DWORD dwFlag) ;
    DWORD   GetRenderFlag(void)  { return m_dwRenderFlag ; } ;
    BOOL    IsStillOn(void)  { return m_bStillOn ; } ;
    BOOL    IsLangKnown(void) ;
    BOOL    IsInitialized(void)  { return m_eState != Unknown ; } ;
    BOOL    CanChangeSpeed(void)  { return Playing == m_eState || Scanning == m_eState ; } ;
    PLAYER_STATE GetState(void)  { return m_eState ; } ;

    BOOL    FileSelect(void) ;
    DWORD   GetStatusText(AM_DVD_RENDERSTATUS *pStatus, 
                          LPTSTR lpszStatusText,
                          DWORD dwMaxText) ;
    HRESULT OnDVDPlayEvent(WPARAM wParam, LPARAM lParam) ;
    void    DrawStatus(HDC hDC) ;

private:  // private helper methods for the class' own use
    LPTSTR  GetStringRes (int id) ;
    void    ReleaseInterfaces(void) ;
    
private:  // internal state info
    HINSTANCE     m_hInstance ;       // current instance
    HWND          m_hWnd ;            // app window handle
    TCHAR         m_szAppName[50] ;   // name of the app
    TCHAR         m_szTitle[50] ;     // title bar text
    TCHAR         m_achBuffer[100] ;  // app's internal buffer for res strings etc.

    CViewLevels   m_ViewLevels ;      // viewing level data (static)
    CDVDLanguages m_Langs ;           // language list data (static)
    BOOL          m_bSPOn ;           // Is Subpicture On?
    ULONG         m_ulSPStream ;      // SubPicture stream number
    ULONG         m_ulAudioStream ;   // Audio stream number
    BOOL          m_bMenuOn ;         // is DVD menu being shown now?
    BOOL          m_bCCOn ;           // is CC being shown?
    BOOL          m_bFullScrnOn ;     // is showing in full screen mode?
    PLAYER_STATE  m_eState ;          // player state (run/pause/stop/...)
    BOOL          m_bStillOn ;        // is a still image on now?
    BOOL          m_bLangKnown ;      // can ask for audio/subpic lang etc?
    ULONG         m_ulParentCtrlLevel ;// parental control level of viewing
    DWORD         m_dwRenderFlag ;    // flags to use for building graph
    TCHAR         m_achFileName[MAX_PATH] ; // current root file name
    TCHAR         m_achStillText[20] ;   // still status text for display
    TCHAR         m_achTitleText[20] ;   // current title info for display
    TCHAR         m_achChapterText[20] ; // current chapter for display
    TCHAR         m_achTimeText[50] ;    // current time for display

    IDvdGraphBuilder *m_pDvdGB ;      // IDvdGraphBuilder interface
    IVideoWindow     *m_pVW ;         // IVideoWindow interface
    IGraphBuilder    *m_pGraph ;      // IGraphBuilder interface
    IDvdInfo         *m_pDvdI ;       // IDvdInfo interface
    IDvdControl      *m_pDvdC ;       // IDvdControl interface
    IMediaControl    *m_pMC ;         // IMediaControl interface
    IMediaEventEx    *m_pME ;         // IMediaEventEx interface
    IAMLine21Decoder *m_pL21Dec ;     // IAMLine21Decoder interface
} ;
