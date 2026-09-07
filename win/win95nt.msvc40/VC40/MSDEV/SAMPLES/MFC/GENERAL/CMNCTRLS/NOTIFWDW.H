// NotifySowCase.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNotifySowCase window

#ifndef INC_NOTIFYSHOWCASE_H
#define INC_NOTIFYSHOWCASE_H

typedef struct notif_tag 
{
	UINT	nCode;
	TCHAR	sztCodeName[30];
} notifDescrip;


#define NOTIFCOUNT	23

class CNotifyShowCase : public CEdit
{
// Construction
public:
	CNotifyShowCase();
	static notifDescrip m_rgNotifDescrip[NOTIFCOUNT];
	
// Attributes
public:
	int		m_totalLines;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNotifySowCase)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNotifyShowCase();
	void	SetBufferSize(int nLines) { m_totalLines = nLines;};
	void	AddLine(LPCTSTR lpsz);
	void	ShowNotification(UINT nCode);

	// Generated message map functions
protected:
	//{{AFX_MSG(CNotifySowCase)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////


#endif
