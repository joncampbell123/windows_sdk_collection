/*
 -  S T R T B L . C
 -  Copyright (C) 1994 Microsoft Corporation
 -
 *  Purpose:
 *      Contains the GetString() function which allows us to
 *      implement named string tables.  This function uses
 *      strings that where pre-processed from the MAPITEST.STR
 *      string table file in this directory.  The pre-processor
 *      used is called SC.EXE and can be found in the \preproc
 *      sub-directory below this one.
 *
 */

#include <windows.h>
#include <strtbl.h>
#include <strdef.h>


/*
 -  GetString
 *
 *  Purpose:
 *      Retrieves a string associated with the ID from the
 *      named string table.
 *
 *  Parameters:
 *      lpszName        - The name of the string table to use
 *      ulID            - The ID of the string you wish to retrieve
 *      lpsz            - Pointer to the buffer to copy the string into
 *
 *  Returns:
 *      lpsz            - The pointer to the destination buffer passed in
 *
 */

LPSTR CDECL GetString(LPSTR lpszName, LONG ulID, LPSTR lpsz)
{
    ULONG   idxTbl = 0;
    ULONG   idxStr = 0;

    while(idxTbl < NUM_TABLES)
    {
        if(!lstrcmpi(lpszName, st[idxTbl].lpszName))
            break;

        idxTbl++;
    }

    if(idxTbl == NUM_TABLES)
        return NULL;

    while(idxStr < st[idxTbl].cEntrys)
    {
        if(st[idxTbl].lpEntry[idxStr].ulID == ulID)
            break;

        idxStr++;
    }

    if(idxStr == st[idxTbl].cEntrys)
    {
        if(lpsz)
            wsprintf(lpsz,"0x%08X",ulID);       // id not found, return textized HEX value
        return NULL;                        // to '(null)'; return NULL
    }

    if(lpsz)
    {
        lstrcpy(lpsz, st[idxTbl].lpEntry[idxStr].lpszValue);
        return lpsz;
    }
    else
        return(st[idxTbl].lpEntry[idxStr].lpszValue);
}


/*
 -  GetRowString
 *
 *  Purpose:
 *      Retrieves a string associated with the Row of the Table 
 *
 *  Parameters:
 *      lpszName        - The name of the string table to use
 *      ulRow           - The Row in the table of the string you wish to retrieve
 *      lpsz            - Pointer to the buffer to copy the string into
 *
 *  Returns:
 *      lpsz            - The pointer to the destination buffer passed in
 *
 */

LPSTR CDECL GetRowString(LPSTR lpszName, ULONG ulRow, LPSTR lpsz)
{
    ULONG   idxTbl = 0; 

    /* find which table we are looking for */
    while(idxTbl < NUM_TABLES)
    {
        if(!lstrcmpi(lpszName, st[idxTbl].lpszName))
            break;

        idxTbl++;
    }

    if(idxTbl == NUM_TABLES)
        return NULL;

    /* check to see the row is in range */
    
    if( ulRow >= st[idxTbl].cEntrys)
    {
        wsprintf(lpsz,"Row %lu Not Found",ulRow);     
        return NULL;                    
    }

    lstrcpy(lpsz, st[idxTbl].lpEntry[ulRow].lpszValue);

    return lpsz;
}



/*
 -  GetRowID
 *
 *  Purpose:
 *      Retrieves a ID associated with the Row of the Table 
 *
 *  Parameters:
 *      lpszName        - The name of the string table to use
 *      ulRow           - The Row in the table of the string you wish to retrieve
 *
 *  Returns:
 *      ULONG           - ID of row in table
 *
 */


ULONG CDECL GetRowID(LPSTR lpszName,ULONG ulRow)
{
    ULONG   idxTbl = 0; 

    /* find which table we are looking for */
    while(idxTbl < NUM_TABLES)
    {
        if(!lstrcmpi(lpszName, st[idxTbl].lpszName))
            break;

        idxTbl++;
    }

    if(idxTbl == NUM_TABLES)
        return 0;

    /* check to see the row is in range */
    
    if( ulRow >= st[idxTbl].cEntrys)
        return 0;                    

    return( st[idxTbl].lpEntry[ulRow].ulID);
}

/*
 -  GetRowCount
 *
 *  Purpose:
 *      Retrieves the number of items/rows in the table
 *
 *  Parameters:
 *      lpszName        - The name of the string table to get rowcount of
 *
 *  Returns:
 *      ulCount         - Number of rows in table
 *
 */

ULONG CDECL GetRowCount(LPSTR lpszName)
{
    ULONG   idxStr = 0;
    ULONG   idxTbl = 0; 

    /* find which table we are looking for */
    while(idxTbl < NUM_TABLES)
    {
        if(!lstrcmpi(lpszName, st[idxTbl].lpszName))
            break;

        idxTbl++;
    }

    if(idxTbl == NUM_TABLES)
        return 0;

    /* check to see the row is in range */
    
    return(st[idxTbl].cEntrys);
}




/*
 -  GetID
 *
 *  Purpose:
 *      Retrieves an ID associated with the string from the
 *      named string table.
 *
 *  Parameters:
 *      lpszName        - The name of the string table to use
 *      lpsz            - Pointer to the string to search for
 *      lpulID          - The ID of the string you were searching for
 *
 *  Returns:
 *      fFound          - Boolean indicating if the ID was found
 *
 */

BOOL CDECL GetID(LPSTR lpszName, LPSTR lpsz, LONG *lpulID)
{
    ULONG   idxTbl = 0;
    ULONG   idxStr = 0;

    while(idxTbl < NUM_TABLES)
    {
        if(!lstrcmpi(lpszName, st[idxTbl].lpszName))
            break;

        idxTbl++;
    }

    if(idxTbl == NUM_TABLES)
        return FALSE;

    while(idxStr < st[idxTbl].cEntrys)
    {
        if(!lstrcmp(st[idxTbl].lpEntry[idxStr].lpszValue, lpsz))
            break;

        idxStr++;
    }

    if(idxStr == st[idxTbl].cEntrys)
        return FALSE;

    *lpulID = st[idxTbl].lpEntry[idxStr].ulID;

    return TRUE;
}
