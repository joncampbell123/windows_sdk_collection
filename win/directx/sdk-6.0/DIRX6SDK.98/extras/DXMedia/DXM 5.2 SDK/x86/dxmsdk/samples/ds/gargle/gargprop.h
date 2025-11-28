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
//
// GargProp.h
//
// This file is entirely concerned with the implementation of the
// properties page.  It uses the property page base class to minimise
// the implementation effort.

#ifndef __GARGPROP__
#define __GARGPROP__

#ifdef __cplusplus
extern "C" {
#endif


const int MaxGargleRate = 1000;    // 1000Hz max rate
const int MinGargleRate = 1;       // 1Hz min rate
const int DefaultGargleRate = 10;  // 10 Hz default


class CGargleProperties : public CBasePropertyPage
{

public:

    static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);

    // Overrides from CBasePropertyPage
    HRESULT OnConnect(IUnknown * punk);
    HRESULT OnDisconnect(void);

    HRESULT OnDeactivate(void);

    CGargleProperties(LPUNKNOWN lpunk, HRESULT *phr);

private:

    BOOL OnReceiveMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    HWND        CreateSlider(HWND hwndParent);
    void        OnSliderNotification(WPARAM wParam);

    HWND        m_hwndSlider;   // handle of slider


    IGargle   *m_pGargle;       // pointer to the IGargle interface of the
                                // gargle filter.  Set up in OnConnect.

    int        m_iGargleRate;   // Remember gargle rate between
                                // Deactivate / Activate calls.
    int        m_iGargleShape;  // 0 = triangle (default), 1 = square wave.

};  // class CGargleProperties

#ifdef __cplusplus
}
#endif

#endif // __GARGPROP__
