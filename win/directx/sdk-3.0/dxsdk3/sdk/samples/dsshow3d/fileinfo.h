#ifndef __FILEINFO_H__
#define __FILEINFO_H__


#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>

#define FREQ_SLIDER_PAGESIZE_HZ     1000    // Move 1000 Hz per page
#define FREQ_SLIDER_MAX         100000
#define FREQ_SLIDER_MIN         100

#define VOL_SLIDER_FACTOR       100     // Scaling factor
#define VOL_SLIDER_SHIFT        10000   // Offset (guarantees >0 range)
#define VOL_SLIDER_PAGE         500
#define VOL_MIN             -10000
#define VOL_MAX             0

#define PAN_SLIDER_FACTOR       100     // Scaling factor
#define PAN_SLIDER_SHIFT        10000   // Offset (guarantees >0 range)
#define PAN_SLIDER_PAGE         500
#define PAN_MIN             -10000
#define PAN_MAX             10000

#define PROGRESS_MIN            0
#define PROGRESS_MAX            10000
#define PROGRESS_TIC            1000

// Internal class state flags
#define FI_INTERNALF_3D         0x00000001
#define FI_INTERNALF_HARDWARE       0x00000002
#define FI_INTERNALF_LOOPED     0x00000004
#define FI_INTERNALF_PLAYING        0x00000008
#define FI_INTERNALF_LOST       0x00000010
#define FI_INTERNALF_LOADED     0x00000020
#define FI_INTERNALF_STATIC     0x00000040
#define FI_INTERNALF_STREAMING      0x00000080
#define FI_INTERNALF_USEGETPOS2     0x00000100
#define FI_INTERNALF_INTERFACE      0x00000200
#define FI_INTERNALF_STICKY     0x00000400
#define FI_INTERNALF_GLOBAL     0x00000800

typedef struct tag_hwtable
{
    HWND    hLoopedCheck;
    HWND    hProgressSlider, hProgressText, hProgressSpin;
    HWND    hFreqText, hFreqSlider;
    HWND    hVolText, hVolSlider;
    HWND    hPanText, hPanSlider;
    HWND    hDataFormatText;
    HWND    hPlayCursorText, hWriteCursorText;
    HWND    hBufferTypeText, hFocusModeText, hGetPosModeText;
    HWND    hPlayButton;

} HWNDTABLE, *PHWNDTABLE;

class FileInfo
{
friend BOOL CALLBACK FileInfoDlgProc( HWND, UINT, WPARAM, LPARAM );

// Useful protected member functions
protected:
    virtual BOOL OnInitDialog( HWND, WPARAM );
    virtual BOOL OnInitMenu( WPARAM, LPARAM );
    virtual BOOL OnCommand( WPARAM, LPARAM );
    virtual BOOL OnHScroll( WORD, LONG, HWND );
    virtual BOOL OnContextMenu( HWND, int, int );
    virtual void OnDestroy();

    virtual BOOL CreateInterface( HWND );
    virtual void UpdateFileName( void );
    void UpdatePlayButton( void );

    virtual void SetSliders( void );

    void UpdateProgressUI( DWORD );
    void UpdateVolUI( LONG, BOOL );
    void UpdatePanUI( LONG, BOOL );
    void UpdateFreqUI( DWORD, BOOL );

    void HandleFreqSliderScroll( WORD, LONG );
    void HandleVolSliderScroll( WORD, LONG );
    void HandlePanSliderScroll( WORD, LONG );
    void HandleProgressSliderScroll( WORD, LONG );
    void HandleProgressSpinScroll( WORD, LONG );

    BOOL HandleFreqContext( WPARAM );
    BOOL HandleVolContext( WPARAM );
    BOOL HandlePanContext( WPARAM );

    inline void SetInternalFlag( BOOL fSet, DWORD dwVal )
                { if( fSet ) m_dwInternalFlags |= dwVal;
                    else m_dwInternalFlags &= ~dwVal; }

public:
    FileInfo( class MainWnd *pmw = NULL );
    virtual ~FileInfo();

    int LoadWave( LPSTR lpszFile, int nIndx );
    virtual int NewDirectSoundBuffer( void );
    void SetFileName( LPSTR lpsz, int nIndx );
    void PlayBuffer( void );
    void StopBuffer( void );

    void Close( void );

    virtual void UpdateUI( void );
    virtual void Duplicate( FileInfo * );

    void CascadeWindow( void );
    void ResetCascade( void );
    void MinimizeWindow( void );
    void RestoreWindow( void );

    void SetOwner( class MainWnd *pmw )   { if( pmw ) m_pmwOwner = pmw; }

    inline void Set3D( BOOL fNew )
            { SetInternalFlag( fNew, FI_INTERNALF_3D ); }

    inline void SetPlaying( BOOL fNew )
            { SetInternalFlag( fNew, FI_INTERNALF_PLAYING ); }

    inline void SetSticky( BOOL fNew )
            { SetInternalFlag( fNew, FI_INTERNALF_STICKY ); }

    inline void SetGlobal( BOOL fNew )
            { SetInternalFlag( fNew, FI_INTERNALF_GLOBAL ); }

    inline void SetLooped( BOOL fNew )
            { SetInternalFlag( fNew, FI_INTERNALF_LOOPED ); }

    inline void SetUseGetPos2( BOOL fNew )
            { SetInternalFlag( fNew, FI_INTERNALF_USEGETPOS2 ); }

    inline BOOL Is3D()
            { return (m_dwInternalFlags & FI_INTERNALF_3D); }

    inline BOOL IsPlaying()
            { return (m_dwInternalFlags & FI_INTERNALF_PLAYING ); }

    inline BOOL IsLooped()
            { return (m_dwInternalFlags & FI_INTERNALF_LOOPED ); }

    inline BOOL IsSticky()
            { return (m_dwInternalFlags & FI_INTERNALF_STICKY ); }

    inline BOOL IsGlobal()
            { return (m_dwInternalFlags & FI_INTERNALF_GLOBAL ); }

    inline BOOL IsHardware()
            { return (m_dwInternalFlags & FI_INTERNALF_HARDWARE ); }

    inline BOOL IsUsingGetPos2()
            { return (m_dwInternalFlags & FI_INTERNALF_USEGETPOS2 ); }

    // Send a request to the owner MainWnd object that this object be
    // destroyed, which may involve more than a simple delete (like removal
    // from a list or something).
    void SendDestroyRequest( void );

// Member data
protected:
    LPBYTE          m_pbData;       // Pointer to actual data of file.
    UINT            m_cbDataSize;   // Size of data.
    LPWAVEFORMATEX  m_pwfx;     // Pointer to waveformatex structure.
    DSBUFFERDESC    m_dsbd;

    DWORD       m_dwFreqSliderFactor;   // Scaling factor

    DWORD       m_dwInternalFlags;  // A bit field of flags

    HWND        m_hwndInterface;
    HWNDTABLE       m_ht;       // A table of all the control HWND's

    BOOL        m_fPlayButtonSaysPlay;

    char            m_szFileName[MAX_PATH];
    int             m_nFileIndex;   // Index to filename, without dir.

    class MainWnd*  m_pmwOwner;

    LPDIRECTSOUNDBUFFER     m_pDSB;     // Pointer to direct sound buffer.

    static int  m_xNextPos, m_yNextPos;
};

typedef FileInfo *  PFILEINFO;

BOOL CALLBACK FileInfoDlgProc( HWND, UINT, WPARAM, LPARAM );


#endif  // __FILEINFO_H__


