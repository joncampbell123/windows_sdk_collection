// person.h : Defines the class interfaces for CPerson, CPersonList.
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

#ifndef __PERSON_H__
#define __PERSON_H__
#ifdef _DOS
	#include <afx.h>
#else
#ifdef _NTWIN
	#include <afx.h>
#else
	#include <afxwin.h>
#endif
#endif
#include <afxcoll.h>


/////////////////////////////////////////////////////////////////////////////
// class CPerson:
// Represents one person in the phone database.  This class is derived from
// CObject (mostly to get access to the serialization protocol).

class CPerson : public CObject
{
	DECLARE_SERIAL( CPerson );

public:
//Construction
	// For serializable classes, declare a constructor with no arguments.
	CPerson()
		{ m_modTime = CTime::GetCurrentTime(); }
		
	CPerson( const CPerson& a );

	// For our convenience, also declare a constructor with arguments.
	CPerson( const char* pszLastName,
		const char* pszFirstName,
		const char* pszPhoneNum );

//Attributes
	// Member functions to modify the protected member variables.
	void SetLastName( const char* pszName )
		{   ASSERT_VALID( this );
			ASSERT( pszName != NULL);
			m_LastName = pszName;
			m_modTime = CTime::GetCurrentTime(); }
			
	const CString& GetLastName() const
		{   ASSERT_VALID( this );
			return m_LastName; }

	void SetFirstName( const char* pszName )
		{   ASSERT_VALID( this );
			ASSERT( pszName != NULL );
			m_FirstName = pszName;
			m_modTime = CTime::GetCurrentTime(); }
			
	const CString& GetFirstName() const
		{   ASSERT_VALID( this );
			return m_FirstName; }

	void SetPhoneNumber( const char* pszNumber )
		{   ASSERT_VALID( this );
			ASSERT( pszNumber != NULL );
			m_PhoneNumber = pszNumber;
			m_modTime = CTime::GetCurrentTime(); }
			
	const CString& GetPhoneNumber() const
		{   ASSERT_VALID( this );
			return m_PhoneNumber; }

	const CTime GetModTime() const
		{   ASSERT_VALID( this );
			return m_modTime; }

//Operations
	CPerson& operator=( const CPerson& b );

//Implementation
protected:
	// Member variables that hold data for person
	CString        m_LastName;
	CString        m_FirstName;
	CString        m_PhoneNumber;
	CTime          m_modTime;

public:
	// Override the Serialize function
	virtual void Serialize( CArchive& archive );

#ifdef _DEBUG
	// Override Dump for debugging support
	virtual void Dump( CDumpContext& dc ) const;
	virtual void AssertValid() const;
#endif
};

/////////////////////////////////////////////////////////////////////////////
// class CPersonList:
// This represents a list of all persons in a phone database.  This class is
// derived from CObList, a list of pointers to CObject-type objects.

class CPersonList : public CObList
{
	DECLARE_SERIAL( CPersonList )
	
public:
//Construction

	CPersonList()
		{ m_bIsDirty = FALSE; }

	// Add new functions
	CPersonList* FindPerson( const char * szTarget );
	
	// SetDirty/GetDirty
	// Mark the person list as "dirty" (meaning "modified").  This flag can be
	// checked later to see if the database needs to be saved.
	//
	void SetDirty( BOOL bDirty )
		{   ASSERT_VALID( this );
			m_bIsDirty = bDirty; }
	
	BOOL GetDirty()
		{   ASSERT_VALID( this );
			return m_bIsDirty; }

	// Delete All will delete the Person objects as well as the pointers.
	void DeleteAll();
	
protected:
	BOOL  m_bIsDirty;
};

/////////////////////////////////////////////////////////////////////////////

#endif // __PERSON_H__

