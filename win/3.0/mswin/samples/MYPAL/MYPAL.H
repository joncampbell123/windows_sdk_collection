#define PALETTESIZE	256    /* Number of entries in the system palette     */

typedef struct tagRGPT {       /* Values for setting Min Max info	      */
    int iInfo[10];
} RGPT;
typedef RGPT FAR *LPRGPT;

extern void ShowColor(HWND, HDC);
