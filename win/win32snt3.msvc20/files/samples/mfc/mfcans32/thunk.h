//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       Thunk.h
//
//  Contents:   Common Wrapper Interface.
//
//  Classes:    CInterface
//              CAnsiInterface
//              CWideInterface
//              CWrapperAlloc
//
//  History:    19-Jan-93   v-kentc     Created.
//
//---------------------------------------------------------------------------



//+--------------------------------------------------------------------------
//
//  Class:      CInterface
//
//  Synopsis:   Class definition of CInterface
//
//---------------------------------------------------------------------------
class CInterface : public IUnknown
{
public:
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID * ppvObj);
	STDMETHOD_(unsigned long, AddRef)(void);
	STDMETHOD_(unsigned long, Release)(void);

	CInterface(IDINTERFACE, LPUNKNOWN, LPUNKNOWN);

	inline LPUNKNOWN GetWrappee() const
		{ return m_pObj; };

	inline IDINTERFACE GetInterfaceId(VOID)
		{ return m_idInterface & ID_MASK; };

	inline void SetInterfaceId(IDINTERFACE idInterface)
		{ m_idInterface = (m_idInterface & ~ID_MASK) | idInterface; }

	inline BOOL IsANSI(VOID)
		{ return (m_idInterface & ID_ANSIINTERFACE) != 0; };

	inline VOID SetInterfaceData(LPINTERFACEDATA pidata)
		{ m_pInterfaceData = pidata; };

	inline LPINTERFACEDATA GetInterfaceData(VOID)
		{ return m_pInterfaceData; };

	LONG      m_refs;
	LPUNKNOWN m_pUnk;
	LPUNKNOWN m_pObj;

protected:
	LPINTERFACEDATA m_pInterfaceData;  // Delete on release.

private:
	IDINTERFACE m_idInterface;
};


//+--------------------------------------------------------------------------
//
//  Class:      CAnsiInterface
//
//  Synopsis:   Class definition of CAnsiInterface
//
//---------------------------------------------------------------------------
class CAnsiInterface : public CInterface
{
public:
	inline CAnsiInterface(IDINTERFACE idInterface, LPUNKNOWN pUnk, LPUNKNOWN pObj) :
			CInterface(idInterface | ID_ANSIINTERFACE, pUnk, pObj) {};
};


//+--------------------------------------------------------------------------
//
//  Class:      CWideInterface
//
//  Synopsis:   Class definition of CWideInterface
//
//---------------------------------------------------------------------------
class CWideInterface : public CInterface
{
public:
	inline CWideInterface(IDINTERFACE idInterface, LPUNKNOWN pUnk, LPUNKNOWN pObj) :
			CInterface(idInterface, pUnk, pObj) {};
};


//+--------------------------------------------------------------------------
//
//  Class:      CWrapperAlloc
//
//  Synopsis:   Class definition of CWrapperAlloc
//
//---------------------------------------------------------------------------
class CWrapperAlloc
{
public:
	void * operator new (unsigned int size);
	void operator delete(void * ptr);
};
