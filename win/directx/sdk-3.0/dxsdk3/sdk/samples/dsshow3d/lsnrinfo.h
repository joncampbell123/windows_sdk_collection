#ifndef __LISTENERINFO_H__
#define __LISTENERINFO_H__

#define LISTENER_SLIDER_MAX (50)
#define LISTENER_SLIDER_MIN (-LISTENER_SLIDER_MAX)
#define LISTENER_SLIDER_FACTOR  (1)
#define LISTENER_SLIDER_SHIFT   (LISTENER_SLIDER_MAX)

class ListenerInfo
{
friend BOOL CALLBACK ListenerInfoDlgProc( HWND, UINT, WPARAM, LPARAM );

public:
    ListenerInfo();
    ~ListenerInfo();

    void UpdateUI( void );

// Useful protected member functions
protected:
    BOOL OnInitDialog( HWND, WPARAM );
    BOOL OnCommand( WPARAM, LPARAM );
    BOOL OnHScroll( WORD, LONG, HWND );
    void OnDestroy( void );

    void HandleXSliderScroll( WORD, LONG );
    void HandleYSliderScroll( WORD, LONG );
    void HandleZSliderScroll( WORD, LONG );

    void UpdateXSliderUI( LONG, BOOL );
    void UpdateYSliderUI( LONG, BOOL );
    void UpdateZSliderUI( LONG, BOOL );

    void SetSliders( void );

protected:
    HWND    m_hDistanceEdit, m_hDopplerEdit, m_hRolloffEdit;
    HWND    m_hXSlider, m_hYSlider, m_hZSlider;
    HWND    m_hXText, m_hYText, m_hZText;

    D3DVECTOR   m_vPos;
};


typedef ListenerInfo *  PLISTENERINFO;

BOOL CALLBACK ListenerInfoDlgProc( HWND, UINT, WPARAM, LPARAM );

#endif


