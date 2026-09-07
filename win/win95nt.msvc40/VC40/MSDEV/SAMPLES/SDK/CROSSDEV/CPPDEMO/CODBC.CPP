/*--------------------------------------------------------------------------
  CODBC.CPP - Low-level ODBC object classes implementation

  This code is furnished on an as-is basis as part of the ODBC SDK and is
  intended for example purposes only.

--------------------------------------------------------------------------*/

#include "headers.h"

#ifdef _AFX
IMPLEMENT_DYNAMIC(CODBCException, CException);
IMPLEMENT_DYNAMIC(CENV,  CODBC);
IMPLEMENT_DYNAMIC(CDBC,  CODBC);
IMPLEMENT_DYNAMIC(CSTMT, CODBC);
#endif
