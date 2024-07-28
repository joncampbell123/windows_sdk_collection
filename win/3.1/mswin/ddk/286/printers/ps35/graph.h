/*
 * graph.h	defs of for graph.c
 *
 */


void	FAR PASCAL SelectBrush(LPDV, LPBR, LPDRAWMODE);
void	FAR PASCAL SelectPen(LPDV, LPPEN);

void	FAR PASCAL FillDraw(LPDV lpdv, BOOL Fill, BOOL Draw, int style);

int FAR PASCAL GetRComponent(LPDV lpdv, DWORD dwRGB);
int FAR PASCAL GetGComponent(LPDV lpdv, DWORD dwRGB);
int FAR PASCAL GetBComponent(LPDV lpdv, DWORD dwRGB);
