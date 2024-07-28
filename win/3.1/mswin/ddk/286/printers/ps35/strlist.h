HANDLE FAR PASCAL CreateStringList(void);
void FAR PASCAL DeleteStringList(HANDLE hStrList);
void FAR PASCAL EmptyStringList(HANDLE hStrList);
BOOL FAR PASCAL AddString(HANDLE hStrList, LPSTR lpStr, DWORD dwData);
BOOL FAR PASCAL DeleteString(HANDLE hStrList, LPSTR lpStr);
BOOL FAR PASCAL EnumStrings(HANDLE hStrList, LPSTR lpStr, int nMaxLen);
BOOL FAR PASCAL IsStringInList(HANDLE hStrList, LPSTR lpStr);
DWORD FAR PASCAL GetStringData(HANDLE hStrList, LPSTR lpStr);
DWORD FAR PASCAL SetStringData(HANDLE hStrList, LPSTR lpStr, DWORD dwNewData);
 
