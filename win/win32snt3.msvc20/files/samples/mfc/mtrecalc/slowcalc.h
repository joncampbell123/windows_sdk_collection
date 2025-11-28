////////////////////////////////////////////////////////////////////////////////////
// slowcalc.h   Simulation of a calculation that takes a long time

BOOL SlowAdd(int nInt1, int nInt2, int& nResult, CRecalcThreadInfo* pInfo, int nSeconds,
	HWND hwndNotifyProgress);
