//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1997  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;

// This class implements the property page for the contrast filter

class CContrastProperties : public CBasePropertyPage
{

public:

    static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);

private:

    BOOL OnReceiveMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
    HRESULT OnConnect(IUnknown *pUnknown);
    HRESULT OnDisconnect();
    HRESULT OnDeactivate();
    HRESULT OnApplyChanges();

    void SetDirty();

    HWND	CreateSlider(HWND hwndParent);
    void	OnSliderNotification(WPARAM wParam);
    HWND	m_hwndSlider;

    CContrastProperties(LPUNKNOWN lpunk, HRESULT *phr);
    signed char m_cContrastOnExit; // Remember contrast level for CANCEL
    signed char m_cContrastLevel;  // And likewise for next activate

    IContrast *m_pContrast;

    IContrast *pIContrast() {
        ASSERT(m_pContrast);
        return m_pContrast;
    };

}; // CContrastProperties

