/*
 -  S T R T B L . H
 -
 *  Purpose:
 *      Header file for the Named String Table utility.
 *
 */


#ifndef ULONG
typedef unsigned long ULONG;
#endif

/*
 *  ENTRY - Contains the ID and string of a single STRTBL entry.
 */

typedef struct entry
{
    LONG    ulID;
    LPSTR   lpszValue;
} ENTRY, *LPENTRY;


/*
 *  STRTBL - Defines a string table by Name, Count of Entries and Entry List
 */

typedef struct strtbl
{
    LPSTR   lpszName;
    ULONG   cEntrys;
    LPENTRY lpEntry;
} STRTBL;


/* Function Prototypes for the Named String Table stuff */

#ifdef __cplusplus
extern "C" {
#endif

LPSTR CDECL GetString(LPSTR, LONG, LPSTR);
BOOL  CDECL GetID(LPSTR, LPSTR, LONG *);
LPSTR CDECL GetRowString(LPSTR,ULONG,LPSTR);
ULONG CDECL GetRowID(LPSTR,ULONG);
ULONG CDECL GetRowCount(LPSTR);

#ifdef __cplusplus
}
#endif
