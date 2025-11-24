// database.cpp : Defines the behaviors for the CDataBase class.
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

#include "database.h"
#include <string.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

const char szUntitled[] = "Untitled";

//////////////////////////////////////////////////////////////////////////
//  CDataBase
//

//////////////////////////////////////////////////
//  CDataBase::New
//  Initializes the database.
//
BOOL CDataBase::New()
{
	ASSERT_VALID( this );
	
	// Clean up any old data.
	Terminate();

	m_pDataList = new CPersonList;
	
	return ( m_pDataList != NULL );
}

//////////////////////////////////////////////////
//  CDataBase::Terminate
//  Cleans up the database.
//
void CDataBase::Terminate()
{
	ASSERT_VALID( this );
	
	if ( m_pDataList != NULL )
		m_pDataList->DeleteAll();

	delete m_pDataList;
	delete m_pFindList;

	m_pDataList = NULL;
	m_pFindList = NULL;
	
	m_szFileName = szUntitled;
	m_szFileTitle = szUntitled;
}

//////////////////////////////////////////////////
//  CDataBase::AddPerson
//  Inserts a person in the appropriate position (alphabetically by last
//  name) in the database.
//
void CDataBase::AddPerson( CPerson* pNewPerson )
{
	ASSERT_VALID( this );
	ASSERT_VALID( pNewPerson );
	ASSERT( pNewPerson != NULL );
	ASSERT( m_pDataList != NULL );

	POSITION pos = m_pDataList->GetHeadPosition();
	while ( pos != NULL &&
			_stricmp( ((CPerson*)m_pDataList->GetAt(pos))->GetLastName(),
					pNewPerson->GetLastName() ) <= 0 )
		m_pDataList->GetNext( pos );
			
	if ( pos == NULL )
		m_pDataList->AddTail( pNewPerson );
	else
		m_pDataList->InsertBefore( pos, pNewPerson );

	m_pDataList->SetDirty( TRUE );
}

//////////////////////////////////////////////////
//  CDataBase::GetPerson
//  Look up someone by index.
//
CPerson* CDataBase::GetPerson( int nIndex )
{
	ASSERT_VALID( this );
	ASSERT( m_pDataList != NULL );
	
	if ( m_pFindList != NULL )
		return (CPerson*)m_pFindList->GetAt( m_pFindList->FindIndex( nIndex ) );
	else
		return (CPerson*)m_pDataList->GetAt( m_pDataList->FindIndex( nIndex ) );
}

//////////////////////////////////////////////////
//  CDatabase::DeletePerson
//  Removes record of person from database.
//
void CDataBase::DeletePerson( int nIndex )
{
	ASSERT_VALID( this );
	ASSERT( m_pDataList != NULL );
	
	POSITION el = m_pDataList->FindIndex( nIndex );
	delete m_pDataList->GetAt( el );
	m_pDataList->RemoveAt( el );
	m_pDataList->SetDirty( TRUE );
}

//////////////////////////////////////////////////
//  CDatabase::ReplacePerson
//  Replaces an object in the list with the new object.
//
void CDataBase::ReplacePerson( CPerson* pOldPerson, const CPerson& rNewPerson )
{
	ASSERT_VALID( this );
	
	ASSERT( pOldPerson != NULL );
	ASSERT( m_pDataList != NULL );

	// Using the overloaded operator= for CPerson
	*pOldPerson = rNewPerson;
	m_pDataList->SetDirty( TRUE );
}

//////////////////////////////////////////////////
//  CDataBase::DoFind
//  Does a FindPerson call, or clears the find data.
//
BOOL CDataBase::DoFind( const char* pszLastName /* = NULL */ )
{
	ASSERT_VALID( this );
	ASSERT( m_pDataList != NULL );

	if ( pszLastName == NULL )
	{
		delete m_pFindList;
		m_pFindList = NULL;
		return FALSE;
	}

	// The interface should not allow a second find to occur while
	// we already have one.
	ASSERT( m_pFindList == NULL );
	return ( ( m_pFindList = m_pDataList->FindPerson( pszLastName ) ) != NULL );
}

//////////////////////////////////////////////////
//  CDataBase::DoOpen
//  Reads a database from the given filename.
//
BOOL CDataBase::DoOpen( const char* pszFileName )
{
	ASSERT_VALID( this );
	ASSERT( pszFileName != NULL );
	
	CFile file( pszFileName, CFile::modeRead );

	// read the object data from file
	CPersonList* pNewDataBase = ReadDataBase( &file );
	
	file.Close();

	// get rid of current data base if new one is OK
	if ( pNewDataBase != NULL )
	{
		Terminate();
		m_pDataList = pNewDataBase;
		m_pDataList->SetDirty( FALSE );
		
		m_szFileName = pszFileName;
		return TRUE;
	}
	else
		return FALSE;
}

//////////////////////////////////////////////////
//  CDataBase::DoSave
//  Saves the database to the given file.
//
BOOL CDataBase::DoSave( const char* pszFileName /* = NULL */ )
{
	ASSERT_VALID( this );

	// if we were given a name store it in the object.
	if ( pszFileName != NULL )
		m_szFileName = pszFileName;

	CFileStatus status;
	int nAccess = CFile::modeWrite;

	// GetStatus will return TRUE if file exists, or FALSE if it doesn't.
	if ( !CFile::GetStatus( m_szFileName, status ) )
		nAccess |= CFile::modeCreate;
	
	CFile file( m_szFileName, nAccess );

	// write the data base to a file
	// mark it clean if write is successful
	if ( WriteDataBase( &file ) )
	{
		m_pDataList->SetDirty( FALSE );
		file.Close();
		return TRUE;
	}
	else
	{
		file.Close();
		return FALSE;
	}
}

//////////////////////////////////////////////////
//  CDataBase::ReadDataBase
//  Serializes in the database.
//
CPersonList* CDataBase::ReadDataBase( CFile* pFile )
{
	ASSERT_VALID( this );
	CPersonList* pNewDataBase = NULL;

	// Create a archive from pFile for reading.
	CArchive archive( pFile, CArchive::load );

	// Deserialize the new data base from the archive, or catch the
	// exception.
	TRY
	{
		archive >> pNewDataBase;
	}
	CATCH( CArchiveException, e )
	{
#ifdef _DEBUG
		e->Dump( afxDump );
#endif
		archive.Close();
	
		// If we got part of the database, then delete it.
		if ( pNewDataBase != NULL )
		{
			pNewDataBase->DeleteAll();
			delete pNewDataBase;
		}

		// We caught this exception, but we throw it again so our caller can
		// also catch it.
		THROW_LAST();
	}
	END_CATCH

	// Exit here if no errors or exceptions.
	archive.Close();
	return pNewDataBase;
}

//////////////////////////////////////////////////
//  CDataBase::WriteDataBase
//  Serializes out the data into the given file.
//
BOOL CDataBase::WriteDataBase( CFile* pFile )
{
	ASSERT_VALID( this );
	ASSERT( m_pDataList != NULL );
	
	// Create a archive from theFile for writing
	CArchive archive( pFile, CArchive::store );

	// Archive out, or catch the exception.
	TRY
	{
		archive << m_pDataList;
	}
	CATCH( CArchiveException, e )
	{
#ifdef _DEBUG
		e->Dump( afxDump );
#endif
		archive.Close();

		// Throw this exception again for the benefit of our caller.
		THROW_LAST();
	}
	END_CATCH

	// Exit here if no errors or exceptions.
	archive.Close();
	return TRUE;
}

#ifdef _DEBUG
void CDataBase::AssertValid() const
{
	if ( m_pDataList != NULL )
	{
		ASSERT_VALID( m_pDataList );
		if ( m_pFindList != NULL )
			ASSERT_VALID( m_pFindList );
	}
	else
		ASSERT( m_pFindList == NULL );
}
#endif
