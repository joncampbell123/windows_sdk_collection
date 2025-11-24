/*
 * CLIENT.CPP
 * Freeloader Chapter 6
 *
 * Implementation of the CFreeloaderClient class that just makes
 * sure we get a CFreeloaderDoc on doc creation and that we
 * initialize fully.
 *
 * Copyright (c)1993-1994 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */



#include "freeload.h"


/*
 * CFreeloaderClient::CFreeloaderClient
 * CFreeloaderClient::~CFreeloaderClient
 *
 * Constructor Parameters:
 *  hInst           HINSTANCE of the application.
 */

CFreeloaderClient::CFreeloaderClient(HINSTANCE hInst)
    : CClient(hInst)
    {
    return;
    }


CFreeloaderClient::~CFreeloaderClient(void)
    {
    return;
    }



/*
 * CFreeloaderClient::CreateCDocument
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

PCDocument CFreeloaderClient::CreateCDocument(void)
    {
    return (PCDocument)(new CFreeloaderDoc(m_hInst, m_pFR));
    }
