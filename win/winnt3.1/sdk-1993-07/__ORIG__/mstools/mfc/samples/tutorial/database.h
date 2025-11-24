// database.h : Declares the interfaces for the CDataBase class.
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "person.h"

/////////////////////////////////////////////////////////////////////////////

// string const for untitiled database
extern const char szUntitled[];
//////////////////////////////////////////////////
//  CDataBase
//  The database object is intended to encapsulate everything
//  needed to work the the CPersonList into one interface.
//  Exception handling, data manipulations, and searching
//  are handled on this level freeing the view program, whether it
//  is character or windows, to deal with displaying the data.
//
class CDataBase: public CObject
{
public:
	// constructor
	CDataBase()
		{
			m_pDataList = NULL;
			m_pFindList = NULL;
			m_szFileName = "";
			m_szFileTitle = "";
		}

	// Create/Destory CPersonLists
	BOOL New();
	void Terminate();

	// File handling
	BOOL DoOpen( const char* pszFileName );
	BOOL DoSave( const char* pszFileName = NULL );
	BOOL DoFind( const char* pszLastName = NULL );

	// Person Handling
	void AddPerson( CPerson* pNewPerson );
	void ReplacePerson( CPerson* pOldPerson, const CPerson& rNewPerson );
	void DeletePerson( int nIndex );
	CPerson* GetPerson( int nIndex );

	// Database Attributes
	int GetCount()
		{
			ASSERT_VALID( this );
			if ( m_pFindList != NULL )
				return m_pFindList->GetCount();
			if ( m_pDataList != NULL )
				return m_pDataList->GetCount();
			return 0;
		}
			
	BOOL IsDirty()
		{   ASSERT_VALID( this );
			return ( m_pDataList != NULL ) ? m_pDataList->GetDirty() : FALSE; }
			
	BOOL IsNamed()
		{   ASSERT_VALID( this );
			return m_szFileName != szUntitled; }

	const char* GetName()
		{   ASSERT_VALID( this );
			return m_szFileName; }
		
	CString GetTitle()
		{   ASSERT_VALID( this );
			return  "Phone Book - " + m_szFileTitle; }
	void SetTitle( const char* pszTitle )
		{   ASSERT_VALID( this );
			m_szFileTitle = pszTitle; }

	BOOL IsPresent()
		{   ASSERT_VALID( this );
			return m_pDataList != NULL; }
		
protected:
	CPersonList* m_pDataList;
	CPersonList* m_pFindList;
	CString m_szFileName;
	CString m_szFileTitle;

private:
	CPersonList* ReadDataBase( CFile* pFile );
	BOOL WriteDataBase( CFile* pFile );
	
#ifdef _DEBUG
public:
	void AssertValid() const;
#endif
};

/////////////////////////////////////////////////////////////////////////////

#endif // __DATABASE_H__
