#ifndef STRING_FAST
#define STRING_FAST

#include <stdarg.h>

extern inline long wcscpy_count (WCHAR *d, WCHAR const *s);
extern inline long WriteChars(WCHAR* string, int nData);
extern inline long WriteChars(WCHAR* str, const WCHAR *text);
extern inline long WriteChars(WCHAR* str, const WCHAR *str1, const WCHAR *str2, const WCHAR *str3);
extern inline long WriteChars(WCHAR* str, const WCHAR *name, const WCHAR *data, short nDay, short nYear);
extern inline long WriteChars(WCHAR* str, const WCHAR *name, const WCHAR *data, int slen, bool bQuotes);
extern inline long WriteChars(WCHAR* string, const WCHAR *name, int nData);
extern inline WCHAR * wcscpy_fast (WCHAR *d, WCHAR const *s);
extern inline WCHAR * __cdecl wcsncpy_fast (WCHAR * dest, const WCHAR * source, size_t count);
extern inline int wcsncmp_fast (const WCHAR * first, const WCHAR * last, size_t count);
extern inline int __cdecl wcscmp_fast (const WCHAR * src, const WCHAR * dst);

#endif /* STRING_FAST */
