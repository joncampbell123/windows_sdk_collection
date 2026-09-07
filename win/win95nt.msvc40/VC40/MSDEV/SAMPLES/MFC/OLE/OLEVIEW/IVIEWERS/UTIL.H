// util.h

#ifndef _UTIL_H_
#define _UTIL_H_
LPCTSTR AFXAPI _GetScodeString(SCODE sc);
void ErrorMessage( const CString& str, HRESULT hr ) ;
LPTSTR HRtoString( HRESULT hr ) ;
LONG WINAPI ParseOffNumber( LPTSTR FAR *lplp, LPINT lpConv ) ;
CString TYPEDESCtoString( ITypeInfo* pti, TYPEDESC* ptdesc );
CString  VTtoString( VARTYPE vt ) ;

#endif 

