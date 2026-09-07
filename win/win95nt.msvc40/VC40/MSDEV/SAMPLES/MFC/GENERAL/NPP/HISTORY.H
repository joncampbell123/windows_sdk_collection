// history.h : interface of the CHistoryList class
//
/////////////////////////////////////////////////////////////////////////////

class CHistoryList : public CStringList
{ 
// Operations
public:
	void FillCombobox();
	BOOL AddString(CString &s1);

// Implementation
protected:
	void SwapStrings(CWnd* pWnd, CString &s1);
};

