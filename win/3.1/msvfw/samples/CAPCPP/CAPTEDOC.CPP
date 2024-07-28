/****************************************************************************
 *
 *   captedoc.cpp: implementation of the CCaptestDoc class
 * 
 *   Microsoft Video for Windows Capture Class Sample Program
 *
 ***************************************************************************/
/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/

#include "stdafx.h"
#include "captest.h"
#include "captedoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCaptestDoc

IMPLEMENT_DYNCREATE(CCaptestDoc, CDocument)

BEGIN_MESSAGE_MAP(CCaptestDoc, CDocument)
         //{{AFX_MSG_MAP(CCaptestDoc)
                  // NOTE - the ClassWizard will add and remove mapping macros here.
                  //    DO NOT EDIT what you see in these blocks of generated code !
         //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCaptestDoc construction/destruction

CCaptestDoc::CCaptestDoc()
{
         // TODO: add one-time construction code here
         
}

CCaptestDoc::~CCaptestDoc()
{
}

BOOL CCaptestDoc::OnNewDocument()
{
         if (!CDocument::OnNewDocument())
                  return FALSE;
         // TODO: add reinitialization code here
         // (SDI documents will reuse this document)
         return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCaptestDoc serialization

void CCaptestDoc::Serialize(CArchive& ar)
{
         if (ar.IsStoring())
         {
                  // TODO: add storing code here
         }
         else
         {
                  // TODO: add loading code here
         }
}


/////////////////////////////////////////////////////////////////////////////
// CCaptestDoc diagnostics

#ifdef _DEBUG
void CCaptestDoc::AssertValid() const
{
         CDocument::AssertValid();
}

void CCaptestDoc::Dump(CDumpContext& dc) const
{
         CDocument::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCaptestDoc commands
