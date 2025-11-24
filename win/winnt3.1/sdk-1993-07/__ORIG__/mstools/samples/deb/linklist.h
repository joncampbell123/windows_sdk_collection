
/******************************************************************************\
*       This is a part of the Microsoft Source Code Samples. 
*       Copyright (C) 1993 Microsoft Corporation.
*       All rights reserved. 
*       This source code is only intended as a supplement to 
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the 
*       Microsoft samples programs.
\******************************************************************************/

#ifndef LINKLIST_H

 #define LINKLIST_H

 // public typedefs and defines
 // -----------------------------------------------------------------------
 #define LIST_NO_ERROR                 1
 #define LIST_ERROR_NO_NODE           -1
 #define LIST_ERROR_NO_MATCH          -2
 #define LIST_ERROR_NO_FREE_MEM       -3
 #define LIST_ERROR_DEREFERENCE_NULL -99

 #define LIST_LEFT_OF   -1
 #define LIST_MATCH      0
 #define LIST_RIGHT_OF   1

 #ifndef TRUE
   #define TRUE  1
   #define FALSE 0
   typedef int   BOOL;
 #endif

 typedef void* PVOID;

 //-- definition of a node
 typedef struct NODE_STRUCT* PNODE;
 typedef struct NODE_STRUCT {
           PVOID pNodeData;
           PNODE pLeftLink;
           PNODE pRightLink;
         } NODE;

 //-- definition of a list
 typedef struct LIST_STRUCT* PLIST;
 typedef struct LIST_STRUCT {
           PVOID pListData;
           PNODE pFirstNode;
           PNODE pCurrentNode;
           PNODE pLastNode;
           int   (*OrderFunction)(PNODE, PNODE);
           int   ListError;
         } LIST;

 // public function prototypes
 // -----------------------------------------------------------------------
 BOOL CreateList( PLIST* ppList, int (*OrderFunction)( PNODE pNodeOne, PNODE pNodeTwo ) );
 BOOL CreateNode( PNODE* ppNewNode );
 BOOL InsertNode( PLIST pList, PNODE pNewNode );
 BOOL SetCurrentNode( PLIST pList, PNODE pKeyNode, int (*MatchFunction)( PNODE pNodeOne, PNODE pNodeTwo ) );
 BOOL SetCurrentNodeEx( PLIST pList, PNODE pKeyNode, int (*MatchFunction)( PNODE pNodeOne, PNODE pNodeTwo ), int Occurence );
 BOOL GetCurrentNode( PLIST pList, PNODE* ppNode );
 BOOL GetFirstNode( PLIST pList, PNODE* ppNode );
 BOOL GetLastNode( PLIST pList, PNODE* ppNode );
 BOOL GetNextNode( PLIST pList, PNODE* ppNode );
 BOOL GetPrevNode( PLIST pList, PNODE* ppNode );
 BOOL DeleteCurrentNode( PLIST pList );
 BOOL DestroyNode( PNODE pNode );
 BOOL DestroyList( PLIST pList );
 int  GetListError( PLIST pList );

#endif // LINKLIST_H
