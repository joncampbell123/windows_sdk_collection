#define MAX_NUM_RANGES 100

typedef struct {
    int		cRanges;
    BOOL	fPrune;
    int		Above[MAX_NUM_RANGES];
    int		TextColor[MAX_NUM_RANGES];
    DWORD	BackColor[MAX_NUM_RANGES];
} RANGESTRUCT;
int DoRangesDlg(CWnd * pWnd, RANGESTRUCT * rs);
