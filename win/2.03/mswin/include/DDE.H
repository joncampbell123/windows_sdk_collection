
/* DDE window messages */

#define WM_DDE_FIRST	    0x03E0
#define WM_DDE_INITIATE     (WM_DDE_FIRST)
#define WM_DDE_TERMINATE    (WM_DDE_FIRST+1)
#define WM_DDE_ADVISE	    (WM_DDE_FIRST+2)
#define WM_DDE_UNADVISE     (WM_DDE_FIRST+3)
#define WM_DDE_ACK	    (WM_DDE_FIRST+4)
#define WM_DDE_DATA	    (WM_DDE_FIRST+5)
#define WM_DDE_REQUEST	    (WM_DDE_FIRST+6)
#define WM_DDE_POKE	    (WM_DDE_FIRST+7)
#define WM_DDE_EXECUTE	    (WM_DDE_FIRST+8)
#define WM_DDE_LAST	    (WM_DDE_FIRST+8)

/*----------------------------------------------------------------------------
|       DDELN structure
|
|	WM_DDE_ADVISE parameter structure
|
|       Fields:
|               fAckReq
|               fDeferUpd
|		cfFormat
----------------------------------------------------------------------------*/

typedef struct {
        unsigned unused:13,
                 fRelease:1,
                 fDeferUpd:1,
		 fAckReq:1;
	int	 cfFormat;
} DDELN;


/*----------------------------------------------------------------------------
|       DDEUP structure
|
|       WM_DDE_DATA parameter structure.  The actual size of this
|       structure depends on the size of the rgb array.
|
|       Fields:
|               fAckReq
|               fRelease
|		cfFormat
|               rgb
----------------------------------------------------------------------------*/

typedef struct {
	unsigned unused:12,
                 fAck:1,
                 fRelease:1,
                 fReserved:1,
                 fAckReq:1;
	int	 cfFormat;
	BYTE	 rgb[1];
} DDEUP;


/*----------------------------------------------------------------------------
|	DDEPOKE structure
|
|	WM_DDE_POKE parameter structure.  The actual size of this
|       structure depends on the size of the rgb array.
|
|       Fields:
|               fAckReq
|               fRelease
|		cfFormat
|               rgb
----------------------------------------------------------------------------*/

typedef struct {
	unsigned unused:12,
		 fRelease:1,
		 fReserved:2;
	int	 cfFormat;
	BYTE	 rgb[1];
} DDEPOKE;
