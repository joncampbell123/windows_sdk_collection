//===========================================================================
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
// PURPOSE.
//
// Copyright (c) 1992 - 1997  Microsoft Corporation.  All Rights Reserved.
//
//===========================================================================
//
// filename: trprop.h 
//
//	IAMExtTransport Property Page for External Device (VCR)Filter
//

#ifndef __TRPROP__
#define __TRPROP__

class CExtTransProperties : public CBasePropertyPage
{
public:

    static CUnknown *CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);

protected:

	// CBasePropertyPage overrides
    BOOL OnReceiveMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
    HRESULT OnConnect(IUnknown *pUnknown);
    HRESULT OnDisconnect();
    HRESULT OnActivate () ;
    HRESULT OnApplyChanges();
      
private:

    CExtTransProperties(LPUNKNOWN lpunk, HRESULT *phr);

    void    InitPropertiesDialog(HWND hwndParent);
	int		FindModeIndex(long Value);
	void	SetDirty();

    BOOL	OnInitDialog( void );
    BOOL	OnCommand( int iButton, int iNotify );
	void	UpdateControls(void);
    
	HWND	m_hwndModeList;		// Handle of transport mode value listbox
	long	m_CurMediaState;	// what's the media doing?
	long	m_CurLocalState;	// online or offline?
	long	m_CurModeSel;		// current transport mode listbox selection
	long	m_CurMode;			// current transport mode
	BOOL	m_bIsInitialized;	// Will be false while we set init 
								//  values in Dlg
	BOOL	m_bSetHourGlass;	// TRUE if an EJECT will happen
	HCURSOR	m_hHourGlass;		// Handle of the hourglass cursor
	HCURSOR	m_hCurrentCursor;	// Handle of the non-hourglass cursor

	IAMExtTransport	*m_pExtTransport;	// pointers to the IAMExtTransport and 
	IAMExtDevice	*m_pExtDevice;		// IAMExtDevice interfaces
};

#endif	// __TRPROP__
// eof trprop.h
