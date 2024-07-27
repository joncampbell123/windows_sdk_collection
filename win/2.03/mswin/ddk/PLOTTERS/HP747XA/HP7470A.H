/* Declarations specific to the hp7470A driver. */

#define NUMPENCOLORS	    3

#include    "hp747xA.h"

#define XAPHYSICALUNITS     10300
#define YAPHYSICALUNITS     7650

#define XA4PHYSICALUNITS    10900
#define YA4PHYSICALUNITS    YAPHYSICALUNITS

/* The code assumes these pen groups are based at n*10 with n starting at 2. */
#define LBLACKPEN	    20
#define LREDPEN 	    21
#define LGREENPEN	    22
#define LBLUEPEN	    23
#define LYELLOWPEN	    24
#define LVIOLETPEN	    25
#define LTURQUOISEPEN	    26
#define LORANGEPEN	    27
#define LBROWNPEN	    28

#define RBLACKPEN	    30
#define RREDPEN 	    31
#define RGREENPEN	    32
#define RBLUEPEN	    33
#define RYELLOWPEN	    34
#define RVIOLETPEN	    35
#define RTURQUOISEPEN	    36
#define RORANGEPEN	    37
#define RBROWNPEN	    38


typedef struct {
	char	deviceName[10];
	int	PaperType;
	int	PenSpeed;
	int	Orientation;	/* Landscape vs portrait. */
#ifdef SPOOLING
	BOOL	Spooling;	/* Signals spooling vs direct output. */
#endif
	DWORD	PenColors[2];
	int	PenIndexes[2];
	} MODEBLOCK;
typedef MODEBLOCK   *PMODEBLOCK;
typedef MODEBLOCK   FAR *LPMODEBLOCK;
