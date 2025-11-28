// Document.h: interface for the CDocument class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOCUMENT_H__26A093E9_DD1C_11D0_8FFE_00C04FD9189D__INCLUDED_)
#define AFX_DOCUMENT_H__26A093E9_DD1C_11D0_8FFE_00C04FD9189D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CClip
{
public:
    CClip() : m_stStart(0), m_stEnd(0) {};
    CComBSTR    m_FileName;
    STREAM_TIME m_stStart;
    STREAM_TIME m_stEnd;
    BOOL WriteToStream(IStream *pStream);
    BOOL ReadFromStream(IStream *pStream);
    BOOL DoSettingsDialog(HWND hwndParent);
    void InitDialog(HWND);
    bool CleanUpDialog(HWND, bool);
};


class CClipList
{
public:
    BOOL Initialize(HWND hwndParent);
    BOOL WriteToStream(IStream *pStream);
    BOOL ReadFromStream(IStream *pStream);
    void ResetContents(void);
    int NumClips();
    int CurSelClipIndex();
    CClip * GetClip(int i);
    BOOL AddClip(int i, CClip **ppClip);
    void DeleteClip(int i);
    void UpdateClipView(int i);
    void SetSize(int, int);

private:
    void InsertCol(int iColNum, TCHAR * pszColHeader, int Width);

public:
    HWND m_hLV;
};



class CDocument  
{
public:
        CDocument() : m_bDirty(false), m_Height(240), m_Width(320), m_PixelDepth(0) {};
        BOOL Initialize(HWND hDocWindow);
        void MarkDirty() { m_bDirty = true; };
	bool m_bModified;
	BOOL WriteToStream(IStream *pStream);
	BOOL ReadFromStream(IStream *pStream);
        void ResetContents(void);
	virtual ~CDocument();
        BOOL OpenFile();
        BOOL SaveAsFile(bool bShowDialog);
        BOOL NewClip();
        BOOL DeleteClip();
        BOOL EditClip();

public:
        HWND    m_hWnd;
        CClipList   m_ClipList;
	long m_PixelDepth;
	long m_Width;
	long m_Height;
        CComBSTR    m_DocumentFileName;
        CComBSTR    m_TargetFileName;
        CComBSTR    m_VideoCodecDisplayName;
        CComBSTR    m_AudioCodecDisplayName;
        bool        m_bDirty;
};

#endif // !defined(AFX_DOCUMENT_H__26A093E9_DD1C_11D0_8FFE_00C04FD9189D__INCLUDED_)
