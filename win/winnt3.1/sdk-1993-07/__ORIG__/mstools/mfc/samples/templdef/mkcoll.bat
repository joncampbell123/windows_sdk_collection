echo "Building all collection shapes"

copy afxcoll.hTT newcoll.h

REM ////////////////////////////////////////////////////////////////////////
REM -- Arrays

templdef "CArray<BYTE,BYTE,1,0> CByteArray" array.ctt temp.h array_b.cpp
copy newcoll.h+temp.h newcoll.h

templdef "CArray<WORD,WORD,1,0> CWordArray" array.ctt temp.h array_w.cpp
copy newcoll.h+temp.h newcoll.h

templdef "CArray<DWORD,DWORD,1,0> CDWordArray" array.ctt temp.h array_d.cpp
copy newcoll.h+temp.h newcoll.h

templdef "CArray<void*,void*,0,0> CPtrArray" array.ctt temp.h array_p.cpp
copy newcoll.h+temp.h newcoll.h

templdef "CArray<CObject*,CObject*,1,0> CObArray" array.ctt temp.h array_o.cpp
copy newcoll.h+temp.h newcoll.h

REM -- String Arrays require special support
templdef "CArray<CString,const char*,1,1> CStringArray" array.ctt temp.h array_s.cpp
copy newcoll.h+temp.h newcoll.h

REM ////////////////////////////////////////////////////////////////////////
REM -- Lists
templdef "CList<void*,void*,0,0> CPtrList" list.ctt temp.h list_p.cpp
copy newcoll.h+temp.h newcoll.h

templdef "CList<CObject*,CObject*,1,0> CObList" list.ctt temp.h list_o.cpp
copy newcoll.h+temp.h newcoll.h

REM -- String Lists require special support
templdef "CList<CString,const char*,1,1> CStringList" list.ctt temp.h list_s.cpp
copy newcoll.h+temp.h newcoll.h

REM ////////////////////////////////////////////////////////////////////////
REM -- Maps

templdef "CMap<WORD,WORD,void*,void*,0,0> CMapWordToPtr" map.ctt temp.h map_wp.cpp
copy newcoll.h+temp.h newcoll.h

templdef "CMap<void*,void*,WORD,WORD,0,0> CMapPtrToWord" map.ctt temp.h map_pw.cpp
copy newcoll.h+temp.h newcoll.h

templdef "CMap<void*,void*,void*,void*,0,0> CMapPtrToPtr" map.ctt temp.h map_pp.cpp
copy newcoll.h+temp.h newcoll.h

templdef "CMap<WORD,WORD,CObject*,CObject*,1,0> CMapWordToOb" map.ctt temp.h map_wo.cpp
copy newcoll.h+temp.h newcoll.h

REM ////////////////////////////////////////////////////////////////////////
REM -- Maps from Strings
templdef "CMapStringTo<void*,void*,0,0> CMapStringToPtr" map_s.ctt temp.h map_sp.cpp
copy newcoll.h+temp.h newcoll.h

templdef "CMapStringTo<CObject*,CObject*,1,0> CMapStringToOb" map_s.ctt temp.h map_so.cpp
copy newcoll.h+temp.h newcoll.h

templdef "CMapStringTo<CString,const char*,1,1> CMapStringToString" map_s.ctt temp.h map_ss.cpp
copy newcoll.h+temp.h newcoll.h

REM ////////////////////////////////////////////////////////////////////////
REM - Wrap up/Clean up

:skip_map

erase temp.h
echo #undef THIS_FILE >> newcoll.h
echo #define THIS_FILE __FILE__ >> newcoll.h
echo #endif //!__AFXCOLL_H__ >> newcoll.h
