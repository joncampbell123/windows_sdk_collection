/*
 * CLIENT.CPP
 * Patron Chapter 2
 *
 * Implementation of the CPatronClient class that just makes sure
 * we get a CPatronDoc on doc creation and that we initialize fully.
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
 * CPatronClient::CPatronClient
 * CPatronClient::~CPatronClient
 *
 * Constructor Parameters:
 *  hInst           HINSTANCE of the application.
 */

CPatronClient::CPatronClient(HINSTANCE hInst)
    : CClient(hInst)
    {
    return;
    }


CPatronClient::~CPatronClient(void)
    {
    return;
    }





/*
 * CPatronClient::CreateCDocument
 *
 * Purpose:
 *  Constructs a new document specific to the application.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  PCDocument      Pointer to the new document object.
 */

PCDocument CPatronClient::CreateCDocument(void)
    {
    return (PCDocument)(new CPatronDoc(m_hInst, m_pFR));
    }
