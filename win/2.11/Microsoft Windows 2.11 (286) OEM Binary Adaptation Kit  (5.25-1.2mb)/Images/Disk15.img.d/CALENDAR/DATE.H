/* Typedefs and constants for use with date.c */

typedef struct {
    BYTE dayofweek;
    BYTE day;
    BYTE month;
    WORD year;
} DOSDATE;
typedef DOSDATE *PDOSDATE;

#define GDS_SHORT       1
#define GDS_LONG        2
#define GDS_DAYOFWEEK   4
#define GDS_NODAY       8

typedef struct {
    BYTE hundredths;
    BYTE seconds;
    BYTE minutes;
    BYTE hour;
} DOSTIME;
typedef DOSTIME *PDOSTIME;

#define GTS_DEFAULT      0
#define GTS_SECONDS      1
#define GTS_HUNDREDTHS   2
#define GTS_LEADINGZEROS 4
#define GTS_LEADINGSPACE 8
#define GTS_12HOUR       16
#define GTS_24HOUR       32

#define IDS_DATESTRINGS 32736
#define IDS_MONTHS      IDS_DATESTRINGS
#define IDS_DAYSOFWEEK  IDS_MONTHS+12
#define IDS_DAYABBREVS  IDS_DAYSOFWEEK+7
#define IDS_SEPSTRINGS  IDS_DAYABBREVS+7

#define PD_ERRFORMAT    -1
#define PD_ERRSUBRANGE  -2
#define PD_ERRRANGE     -3


BOOL FAR PASCAL InitTimeDate(HANDLE, unsigned);
BOOL FAR PASCAL InitLongTimeDate(unsigned);
void FAR PASCAL GetDosTime(PDOSTIME);
void FAR PASCAL GetDosDate(PDOSDATE);
BOOL FAR PASCAL ValidateDosDate(PDOSDATE);
int FAR PASCAL GetTimeString(PDOSTIME, char *, unsigned);
int FAR PASCAL GetDateString(PDOSDATE, char *, unsigned);
int FAR PASCAL GetLongDateString(PDOSDATE, char *, unsigned);
int FAR PASCAL ParseTimeString(PDOSTIME, char *);
int FAR PASCAL ParseDateString(PDOSDATE, char *);
