/*********************************************************************/
/*
 -  oper.h
 -  Copyright (C) 1995 Microsoft Corporation
 -
 *  Purpose:
 *      Defines id's, function prototypes, classes for operations class.
 *      This class is used to bring up a dialog from which the 
 *      user can select data to be used as parameters to operations.
 *      Not all the dialog controls are used in every case.
 *
 */
/*********************************************************************/

#ifndef __oper_h_       // test defined
#define __oper_h_


#define TEST_INVALID_FLAG       999


/*********************************************************************/
/*
 -  COperation Class
 -
 *  Purpose:
 *      Defines the controls for the Dialogs to do operations
 *
 */
/*********************************************************************/

class COperation : public CModalDialog
{
public:
                                    // sting describing what operation performing
    CString         m_CurrentOperation;
                        
                                    // text describing what params/data goes
                                    // in which Combobox, edit control or flag
                                    // checkbox
    CString         m_CBText1;     
    CString         m_CBText2;
    CString         m_CBText3;
    CString         m_EditText1;
    CString         m_EditText2;
    CString         m_EditText3;
    CString         m_FlagText1;
    CString         m_FlagText2;
    CString         m_FlagText3;
    CString         m_FlagText4;
    CString         m_FlagText5;
    CString         m_FlagText6;
    
    CStringArray    m_CBContents1;  // data in combo boxes
    CStringArray    m_CBContents2;
    CStringArray    m_CBContents3;
    
    CString         m_EditDefault1; // default string in edit controls
    CString         m_EditDefault2;
    CString         m_EditDefault3;
 
                                    // holds string from listboxes and
                                    // checkboxes to be used by application                                       
    char            m_szCB1[80];
    char            m_szCB2[80];    
    char            m_szCB3[80];    
    char            m_szEdit1[80];    
    char            m_szEdit2[80];    
    char            m_szEdit3[80];    

                                    // determines if flag is checked==TRUE
                                    //   or unchecked==FALSE    
    BOOL            m_bFlag1;           
    BOOL            m_bFlag2;
    BOOL            m_bFlag3;
    BOOL            m_bFlag4;
    BOOL            m_bFlag5;
    BOOL            m_bFlag6;

    COperation( CWnd * pParentWnd)
        : CDialog(OPERATIONS, pParentWnd)
        {
            m_bFlag1            = FALSE;
            m_bFlag2            = FALSE;
            m_bFlag3            = FALSE;
            m_bFlag4            = FALSE;                        
            m_bFlag5            = FALSE;
            m_bFlag6            = FALSE;                        

            m_CBText1           = "";
            m_CBText2           = "";
            m_CBText3           = "";
            m_EditText1         = "";
            m_EditText2         = "";
            m_EditText3         = "";
            m_FlagText1         = "";
            m_FlagText2         = "";
            m_FlagText3         = "";
            m_FlagText4         = "";
            m_EditDefault1      = "";
            m_EditDefault2      = "";
            m_EditDefault3      = "";


        }
    
    ~COperation() {  }

    BOOL OnInitDialog();
  
    void OnOK();
    void OnCancel();
    
    void OnFlag1();             // flags are checkboxes
    void OnFlag2();
    void OnFlag3();
    void OnFlag4();
    void OnFlag5();
    void OnFlag6();

    DECLARE_MESSAGE_MAP();

};

#endif



