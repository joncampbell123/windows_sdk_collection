/*------------------------------ enum.c ------------------------------------*/

int	FAR PASCAL RealizeFont (LPDV, LPLOGFONT, LPFONTINFO, LPTEXTXFORM);
int	FAR PASCAL LoadFont(LPDV, LPSTR, LPFONTINFO);
int     FAR PASCAL EnumFonts(LPDV lpdv, LPSTR lszFace, 
                         int (FAR PASCAL *lpfn)(LPLOGFONT,LPTEXTMETRIC,int,LPSTR),
                         LPSTR lpb, WORD wFlags);
void FAR PASCAL FreeTTFontTable(LPDV lpdv);

#define ENUM_INTERNAL      1
#define ENUM_SOFTFONTS     2
#define ENUM_TRUETYPE      4
#define ENUM_SETDEVTTBIT   8     /* set TYPE_TRUETYPE bit of nFontType if
                                  * font is marked as resident TT font.
                                  */





