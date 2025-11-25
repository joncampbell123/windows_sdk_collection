/*===========================================================================*\
|
|  File:        cgdlist.cpp
|
|  Description: 
|       
|-----------------------------------------------------------------------------
|
|  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
|
|  Written by Moss Bay Engineering, Inc. under contract to Microsoft Corporation
|
\*===========================================================================*/

/**************************************************************************

    (C) Copyright 1995 Microsoft Corp.  All rights reserved.

    You have a royalty-free right to use, modify, reproduce and 
    distribute the Sample Files (and/or any modified version) in 
    any way you find useful, provided that you agree that 
    Microsoft has no warranty obligations or liability for any 
    Sample Application Files which are modified. 

    we do not recomend you base your game on IKlowns, start with one of
    the other simpler sample apps in the GDK

 **************************************************************************/

#include <windows.h>
#include "cgrsrce.h"
#include "cgexcpt.h"
#include "strrec.h"
#include "cgdebug.h"
#include "cgchar.h"
#include "cgimage.h"
#include "cgload.h"
#include "cgdlist.h"

#define CGG_CHARACTER 0
#define CGG_TILEDIMAGE 1
#define CGG_SKYIMAGE 2
char* graphicTypes[] = 
{
    "Character",    // type 0
    "TiledImage",   // type 1
    "Sky",      // type 2
    NULL
};

/*---------------------------------------------------------------------------*\
|
|       Class CGameDisplayList
|
|  DESCRIPTION:
|       
|
|
\*---------------------------------------------------------------------------*/
extern void dbgprintf(char *, ...);
extern CLoadingScreen *gLoadingScreen;

CGameDisplayList::CGameDisplayList(
    char* pFileName,
    char* pLevelName,
    CGameLevel* pLevel
    ) : mpHead( NULL )
{
    char nameBuf[256];
    char dataBuf[256];

    // load the graphic objects
    GetPrivateProfileString(
        pLevelName,
        NULL,
        "",     // no default
        nameBuf,
        sizeof( nameBuf ),
        pFileName
        );


    for (char *pChar = nameBuf; *pChar; pChar++)
    {
        CGameGraphic* pGraphic;

        if (gLoadingScreen != NULL)
            gLoadingScreen->Update();

        GetPrivateProfileString(
            pLevelName,
            pChar,
            "",
            dataBuf,
            sizeof( dataBuf ),
            pFileName
            );

        // parse the data string into fields
        CStringRecord fields( dataBuf, "," );

        for (int i=0; graphicTypes[i]; i++)
        {
            if (lstrcmpi(fields[0], graphicTypes[i]) == 0)
            {
                break;  // we found it
            }
        }

        switch (i)
        {
            case CGG_CHARACTER:
            
                if (atoi(fields[6]) == 1)
                {
                    char *pRemoteName=NULL;
                    if (fields.GetNumFields() > 7)
                        pRemoteName = fields[7];

                    pGraphic = new CGameCharacter(
                                    pFileName, 
                                    pChar,
                                    pLevelName,
                                    pLevel, 
                                    atoi(fields[1]), 
                                    atoi(fields[2]),
                                    atoi(fields[3]),
                                    atoi(fields[4]),
                                    NULL,
                                    pRemoteName);
                    if (stricmp(pChar, "Klown") == 0)
                    {
                    pLevel->mMainKlown = (CGameCharacter *) pGraphic;
                    }

                }
                else {
                    pChar += lstrlen( pChar );  // move beyond terminator
                    continue;
                }

                break;

            case CGG_TILEDIMAGE:
            {
                pGraphic = new CGameTiledImage(pChar, atoi(fields[1]), atoi(fields[2]), atoi(fields[3]));
                break;
            }


            case CGG_SKYIMAGE:
                pGraphic = new CGameSkyImage(pChar);
                break;

            default:
                pGraphic = NULL;
                break;
        }

        if (pGraphic)
        {
            Insert( pGraphic );
        }

        pChar += lstrlen( pChar );  // move beyond terminator

#if 1
        // NOTE: if we don't grab messages, we see a white flash while we're
        //      loading (!?!?).  We can't allow switching away until we've
        //      finished loading, however, because we'll lose the surfaces
        //      we've already created, so we just drop the messages on the floor.
        MSG     msg;
        if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE | PM_NOYIELD ) ) {
        }
#endif
    }
}   

CGameDisplayList::~CGameDisplayList()
{
    CGameGraphic* pNext = NULL;

    for (CGameGraphic* pGraphic = mpHead;
        pGraphic;
        pGraphic = pNext
        )
    {
        pNext = pGraphic->GetNext();
        delete pGraphic;
        pGraphic = NULL;
    }
}

void
CGameDisplayList::Update(CGameLevel* pLevel, CGameUpdateList* pUpdate)
{
    if (mpHead)
    {
        mpHead->Update(pLevel, pUpdate);
        // see if we've collided...
        int testZ;
        CGameCharacter *pCur, *pNext;
        pCur = (CGameCharacter *) mpHead;
        pNext = (CGameCharacter *) mpHead->GetNext();

        while (pCur && pNext)
        {
            RECT temprect;
            LPRECT pMyRect = pCur->GetRect();
            testZ = pCur->GetCurrentZ();

            // look at  all chars in the Z order for conflicts...

            while (pNext && (testZ == pNext->GetCurrentZ()))
            {
                if (IntersectRect(&temprect, pMyRect, pNext->GetRect()))
                {
                    int removeme = pCur->Collided(pNext);
                    int removeother = pNext->Collided(pCur);
                }

                pNext = (CGameCharacter *) pNext->GetNext();
            }

            pCur = (CGameCharacter *) pCur->GetNext();
            pNext = (CGameCharacter *) pCur->GetNext();
        }
    }
}

void
CGameDisplayList::Render(CGameLevel* pLevel, CGameScreen* pScreen, CGameUpdateList* pUpdate)
{
    if (mpHead)
        mpHead->Render(pLevel, pScreen, pUpdate);
}

void
CGameDisplayList::Insert( CGameGraphic* pGraphic )
{
    if (mpHead)
    {
        CGameGraphic* pNode = mpHead;
        int lookZ = pGraphic->GetMinZ();

        if (lookZ < pNode->GetMinZ())
        {
            pGraphic->SetNext(pNode);
            mpHead = pGraphic;          
        }
        else
        {
            while (pNode->GetNext() && (pNode->GetNext()->GetMinZ() < lookZ))
            {
                pNode = pNode->GetNext();
            }

            pNode->Add( pGraphic );     // put this graphic after (or in) this node
        }
    }
    else
    {
        mpHead = pGraphic;
    }
}

void
CGameDisplayList::Remove( CGameGraphic* pGraphic )
{
    
    CGameGraphic* pNode = mpHead;
    CGameGraphic* pNextNode;

    while (pNode)
    {
        pNextNode = pNode->GetNext();
        if (pNode == pGraphic)
        {
            // must be head of list;
            mpHead = pNextNode;
            break;
        }
        else
        {
            if (pNextNode == pGraphic)
            {
                // normal case; 
                pNode->SetNext( pNextNode->GetNext());
                break;
            }
        }
        pNode = pNextNode;
    }
    delete pGraphic;
}

void CGameDisplayList::ReSort()
{
    // traverse the list; resort based on Z order.  
    // Naively assume only one mismatch for now.
    CGameGraphic* pNode = mpHead;
    CGameGraphic* pLastNode = mpHead;
    CGameGraphic* pNextNode;

    // find bad node (if any!)
    while (pNode)
    {
        pNextNode = pNode->GetNext();
        if (pNextNode)
        {
            if (pNode->GetCurrentZ() > pNextNode->GetCurrentZ())
            {
                // found a "bad" one!.  Remove from list
                if (pLastNode == pNode) // head of list
                {
                    mpHead = pNextNode;
                }
                else
                {
                    pLastNode->SetNext(pNextNode);
                }
                // this node *must* go somewhere *after* pNextNode, so start searching from there
                pLastNode = pNextNode;
                while (pNextNode)
                {
                    if (pNode->GetCurrentZ() <= pNextNode->GetCurrentZ())
                    {
                        // insert here...
                        pLastNode->SetNext(pNode);
                        pNode->SetNext(pNextNode);
                        return;
                    }
                    pLastNode = pNextNode;
                    pNextNode = pNextNode->GetNext();
                }

                // if here, pNextNode must be NULL....
                pLastNode->SetNext(pNode);
                pNode->SetNext(NULL);
                return;
            }
        }
        pLastNode = pNode;
        pNode = pNextNode;
    }
}
