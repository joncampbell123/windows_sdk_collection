// capparms.h : header file
//
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
// 

/////////////////////////////////////////////////////////////////////////////
// CCapParms dialog

class CCapParms : public CDialog
{
// Construction
public:
    CCapParms(CWnd* pParent = NULL);    // standard constructor
    
    HWND 	hwndCap;
    void CCapParms::SetWaveFormatText (HWND hwndCap, CStatic *pcStatic);
    
// Dialog Data
    //{{AFX_DATA(CCapParms)
	enum { IDD = IDD_SETUP_DIALOG };
	CStatic	m_AudioFormatText;
	float	m_FrameRate;
	BOOL	m_EnableAudio;
	BOOL	m_DosBuffers;
	int		m_VideoBuffers;
	BOOL	m_DisableSmartDrv;
	//}}AFX_DATA

// Implementation
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    // Generated message map functions
    //{{AFX_MSG(CCapParms)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK();
	afx_msg void OnClickedSetaudioformat();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};
