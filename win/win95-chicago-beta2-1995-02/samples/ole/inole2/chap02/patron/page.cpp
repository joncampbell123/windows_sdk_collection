/*
 * PAGE.CPP
 * Patron Chapter 2
 *
 * Do-nothing implementation of a page structure managed by CPages.
 *
 * Copyright (c)1993-1994 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#include "patron.h"



/*
 * CPage::CPage
 * CPage::~CPage
 *
 * Constructor Parameters:
 *  dwID            DWORD identifier for this page.
 */

CPage::CPage(DWORD dwID)
    {
    m_dwID=dwID;
    return;
    }

CPage::~CPage(void)
    {
    return;
    }



/*
 * CPage::GetID
 *
 * Return Value:
 *  DWORD           dwID field in this page.
 */

DWORD CPage::GetID(void)
    {
    return m_dwID;
    }
