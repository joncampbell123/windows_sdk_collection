
HANDLE FAR  PASCAL OpenJob(LPSTR, LPSTR, HANDLE);
short FAR   PASCAL StartSpoolPage(HANDLE);
short FAR   PASCAL EndSpoolPage(HANDLE);
short FAR   PASCAL WriteSpool(HANDLE, LPSTR, short);
short FAR   PASCAL CloseJob(HANDLE);
short FAR   PASCAL DeleteJob(HANDLE, short);
short FAR   PASCAL WriteDialog(HANDLE, LPSTR, short);
long  FAR   PASCAL QueryJob(HANDLE, short);
short FAR   PASCAL QueryAbort(HANDLE, short);


