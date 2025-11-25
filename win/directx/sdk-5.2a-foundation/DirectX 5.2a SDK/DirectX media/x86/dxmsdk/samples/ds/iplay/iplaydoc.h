// This code and information is provided "as is" without warranty of
// any kind, either expressed or implied, including but not limited to
// the implied warranties of merchantability and/or fitness for a
// particular purpose.

// Copyright (C) 1996 -1997 Intel corporation.	All rights reserved.

// IPlaydoc.h : interface of the CIPlayDoc class
//
/////////////////////////////////////////////////////////////////////////////
class CIPlayDoc : public CDocument
{
protected: 
	CIPlayDoc();
	DECLARE_DYNCREATE(CIPlayDoc)
	void DeleteContents();

	// Attributes
protected:
	HANDLE       m_hGraphEvent;
	long		 m_lWinWidth;   // original window size
	long		 m_lWinHeight;
	long         m_lWidth;      // encoded video size
	long         m_lHeight;
	BOOL         m_bLoop;
	BOOL         m_bZoom;
	void         *m_pIndeo;	    // pointer to Indeo interface

public:
	IFilterGraph *m_pGraph;
	CHAR   		  m_lpstrPath[MAX_PATH];

    HANDLE GetGraphEventHandle( ) { return m_hGraphEvent; };
	void OnGraphNotify();

    enum {Uninitialized, Stopped, Paused, Playing } m_State;
    BOOL CanPlay(){ return m_State==Stopped || m_State==Paused; };
    BOOL CanStop(){ return m_State==Playing || m_State==Paused; };
    BOOL CanPause(){return m_State==Playing; };
    BOOL IsInitialized(){ return m_State!=Uninitialized; };
	ULONG VideoWidth();
	ULONG VideoHeight();

// Operations
public:
	BOOL IsIndeo();
	void GetBCS(int&, int&, int& );
	void GetTransFillRGB(int&, int&, int&);
	void GetDecodeTime(DWORD& );
	void GetDecodeRect(DWORD&, DWORD&, DWORD&, DWORD& );
	void GetViewRect(DWORD&, DWORD&, DWORD&, DWORD& );
	void GetSequenceOptions(BOOL&, BOOL&, BOOL&, BOOL&, BOOL&, DWORD& );
	void SetBCS(int, int, int);
	void SetTransFillRGB(DWORD);
	void SetDecodeTime(DWORD);
	void SetDecodeRect(DWORD, DWORD, DWORD, DWORD);
	void SetViewRect(BOOL, DWORD, DWORD, DWORD, DWORD);
	void SetSequenceOptions(BOOL, LONG, BOOL, BOOL, DWORD);
	void GetFrameDefaults(int&, int&, int&, DWORD&,
					 DWORD&, DWORD&, DWORD&, DWORD&, 
					 DWORD&, DWORD&, DWORD&, DWORD& );
	void GetSeqDefaults(BOOL&, BOOL&, BOOL&, BOOL&, BOOL&, DWORD&);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIPlayDoc)
	public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CIPlayDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CIPlayDoc)
	afx_msg void OnMediaLoop();
	afx_msg void OnMediaPause();
	afx_msg void OnMediaZoomx2();
	afx_msg void OnUpdateMediaPause(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMediaPlay(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMediaStop(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMediaZoomx2(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMediaLoop(CCmdUI* pCmdUI);
	afx_msg void OnMediaPlay();
	afx_msg void OnMediaStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void SetWindow(BSTR);
    BOOL CreateFilterGraph( void );

};
