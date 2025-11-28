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
// playdoc.h : interface of the CPlayerDoc class
//
/////////////////////////////////////////////////////////////////////////////

class CPlayerDoc : public CDocument
{
protected: // create from serialization only
	CPlayerDoc();
	DECLARE_DYNCREATE(CPlayerDoc)

    BOOL CreateFilterGraph( void );
    void DeleteContents( void );

// Event handles
protected:
    HANDLE m_hGraphNotifyEvent;
public:
    HANDLE GetGraphEventHandle( ) { return m_hGraphNotifyEvent; };
    void OnGraphNotify();

// Attributes
public:
    enum {Uninitialized, Stopped, Paused, Playing } m_State;

    IGraphBuilder *m_pGraph;

    BOOL CanPlay(){ return m_State==Stopped || m_State==Paused; };
    BOOL CanStop(){ return m_State==Playing || m_State==Paused; };
    BOOL CanPause(){return m_State==Playing || m_State==Stopped; };
    BOOL IsInitialized(){ return m_State!=Uninitialized; }

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlayerDoc)
	public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPlayerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:

        // just stop the graph (if aborting). Normal stop will
        // rewind
        void OnAbortStop();

	//{{AFX_MSG(CPlayerDoc)
	afx_msg void OnUpdateMediaPlay(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMediaPause(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMediaStop(CCmdUI* pCmdUI);
	afx_msg void OnMediaPlay();
	afx_msg void OnMediaPause();
	afx_msg void OnMediaStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


};


/////////////////////////////////////////////////////////////////////////////
