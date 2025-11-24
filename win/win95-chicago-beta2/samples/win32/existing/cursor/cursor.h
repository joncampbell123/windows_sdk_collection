#define IDM_ABOUT 100

typedef POINTS MPOINT;
#define MPOINT2POINT(mpt,pt)  ((pt).x = (mpt).x, (pt).y = (mpt).y)
#define POINT2MPOINT(pt, mpt) ((mpt).x = (SHORT)(pt).x, (mpt).y = (SHORT)(pt).y)

BOOL InitApplication(HANDLE);
BOOL InitInstance(HANDLE, INT);
LONG APIENTRY MainWndProc(HWND, UINT, UINT, LONG);
BOOL APIENTRY About(HWND, UINT, UINT, LONG);
INT sieve(VOID);
