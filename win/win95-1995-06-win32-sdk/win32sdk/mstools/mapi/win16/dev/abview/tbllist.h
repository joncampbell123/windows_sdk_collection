/*
 -  T B L L I S T . H
 -  Copyright (C) 1995 Microsoft Corporation
 *
 */

#ifndef __TBLLIST_H__
#define __TBLLIST_H__

#define INDENT   10
#define BMHEIGHT 16
#define BMWIDTH  20
#define BMCOUNT  5

#define fRoot                   ((ULONG) 0x00000001)
#define fHierarchy              ((ULONG) 0x00000002)
#define fContents               ((ULONG) 0x00000004)

/*
 *  Static properties
 */
static const SizedSPropTagArray(6, ABHN_PropTagArray) =
{
    6,
    {
        PR_DISPLAY_NAME,
        PR_ENTRYID,
        PR_OBJECT_TYPE,
        PR_DISPLAY_TYPE,
        PR_DEPTH,
        PR_CONTAINER_FLAGS
    }
};


static const SizedSPropTagArray(6, ABHW_PropTagArray) =
{
    6,
    {
        PR_DISPLAY_NAME_W,
        PR_ENTRYID,
        PR_OBJECT_TYPE,
        PR_DISPLAY_TYPE,
        PR_DEPTH,
        PR_CONTAINER_FLAGS
    }
};


/*
 -  CTblList
 -
 *  Purpose:
 *      Defines the ListBox for Open Directory
 *
 *
 */


class CTblList : public CListBox
{
protected:
    int             m_nPageSize;
    CBitmap*        m_lpbBitmap;         

private:
    afx_msg void    OnVScroll( UINT, UINT, CScrollBar * );
    afx_msg void    OnKeyDown( UINT, UINT, UINT );
    
            void    VScroll( short );
            int     GetPageSize();
    
public:
                    CTblList( UINT, LPMAPITABLE, ULONG );
                    ~CTblList();
            void    InitListBox();
    
    UINT            m_nIDTemplate;
    LPMAPITABLE     m_lpMAPITable;
    ULONG           m_ulCurrentTableType;
    ULONG           m_ulCount;

            int     DeleteString(UINT);

// Implementation
    virtual void    MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
    virtual void    DrawItem(LPDRAWITEMSTRUCT lpDIS);
    virtual int     CompareItem(LPCOMPAREITEMSTRUCT lpCIS);
    virtual void    DeleteItem(LPDELETEITEMSTRUCT);

    DECLARE_MESSAGE_MAP();
    
};

#endif  /* __TBLLIST_H__ */
