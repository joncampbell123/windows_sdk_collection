/* Declarations specific to the hp7475A driver. */

#define NUMPENCOLORS	    7

#include    "hp747xA.h"

#define XAPHYSICALUNITS     10365
#define YAPHYSICALUNITS     7962

#define XBPHYSICALUNITS     16640
#define YBPHYSICALUNITS     10365

#define XA3PHYSICALUNITS    16158
#define YA3PHYSICALUNITS    11040

#define XA4PHYSICALUNITS    11040
#define YA4PHYSICALUNITS    7721

#define XFANFOLDPHYSICALUNITS	 10900
#define YFANFOLDPHYSICALUNITS	 YA4PHYSICALUNITS

#define BLACKPEN	    20
#define REDPEN		    21
#define GREENPEN	    22
#define BLUEPEN 	    23
#define YELLOWPEN	    24
#define VIOLETPEN	    25
#define TURQUOISEPEN	    26
#define ORANGEPEN	    27
#define BROWNPEN	    28

#define PEN1COLOR	    30
#define PEN2COLOR	    31
#define PEN3COLOR	    32
#define PEN4COLOR	    33
#define PEN5COLOR	    34
#define PEN6COLOR	    35

#define PEN1BUTTON	    40
#define PEN2BUTTON	    41
#define PEN3BUTTON	    42
#define PEN4BUTTON	    43
#define PEN5BUTTON	    44
#define PEN6BUTTON	    45


typedef struct {
	char	deviceName[10];
	int	PaperType;
	int	PenSpeed;
	int	Orientation;	/* Landscape vs portrait. */
#ifdef SPOOLING
	BOOL	Spooling;	/* Signals spooling vs direct output. */
#endif
	DWORD	PenColors[6];
	int	PenIndexes[6];
	int	CurPenID;
	int	CurColorID;
	} MODEBLOCK;
typedef MODEBLOCK   *PMODEBLOCK;
typedef MODEBLOCK   FAR *LPMODEBLOCK;
