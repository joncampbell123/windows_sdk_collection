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
// filename: vcrprop.h
//
// IAMExtDevice Property Page for VCR Control Filter
//

#ifndef __VCRPROP__
#define __VCRPROP__

class CVcrProperties : public CBasePropertyPage
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

    // work-around to make list boxes work
	void    OnCapListNotification(void);

	// you can't always change the communications port
	void	EnablePortSelection(BOOL);
	
	int		FindStringID(long Value);
	void	ShowCapabilityValue(int index, HWND hwndValue);
	void	SetDirty();

    BOOL	OnInitDialog( void );
    BOOL	OnCommand( int iButton, int iNotify );
	void	UpdateControls(void);
    
	CVcrProperties(LPUNKNOWN lpunk, HRESULT *phr);
    
	HWND	m_hwndCapList;		// Handle of capability listbox
	HWND	m_hwndCapValue;		// Handle of capability value control
	HWND	m_hwndVidInList;	// Handle of video input listbox
	HWND	m_hwndAudInList;	// Handle of audio input listbox
	HWND	m_hPortButton1;		// Handle of COM1 radio button
	HWND	m_hPortButton2;		// Handle of COM2 radio button
	HWND	m_hPortButton3;		// Handle of COM3 radio button
	HWND	m_hPortButton4;		// Handle of COM4 radio button
	HWND	m_hPortButton5;		// Handle of SIM radio button

	long	m_CurCapSel;		// current capabilites selection
	long	m_CurVidInSel;		// current video input selection
	long	m_CurAudInSel;		// current audio input selection
	long	m_CurDevPort;		// how we talk to device
	long	m_CurPowerMode;		// on or what?
	long 	m_bLink;			// should VCR run follow graph?

	IBaseFilter		*m_pFilter;			// pointer to IBaseFilter on our object
	IAMExtDevice	*m_pExtDevice;		// pointers to the IAMExtDevice 
										//  and IAMExtTransport 
	IAMExtTransport *m_pExtTransport;	//  interfaces
    
	BOOL m_bIsInitialized;			// Will be false while we set init values in Dlg
	BOOL m_bDevPortEnabled;			// Selection only allowed in STOP
};

#endif // __VCRPROP__
// eof vcrprop.h
