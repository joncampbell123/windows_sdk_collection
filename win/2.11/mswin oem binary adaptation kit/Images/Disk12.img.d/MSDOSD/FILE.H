/* Header file for procedures and data types defined by FILE.ASM */

/* Buffer information filled in by FindFile and FindNextFile */

struct findType {
    char     reserved[21];              /* reserved for start up             */
    char     attr;                      /* attribute found                   */
    unsigned time;                      /* time of last modify               */
    unsigned date;                      /* date of last modify               */
    long     size;                      /* file size                         */
    char     name[13];                  /* asciz file name                   */
};

/* Global procedures exported by FILE.ASM */
int     PASCAL RootPath( char *, char * );
int     PASCAL GetCurDirectory( char * );
int     PASCAL SetCurDirectory( char * );
int     PASCAL CreateDirectory( char * );
int     PASCAL DeleteDirectory( char * );
int     PASCAL FindFile( char *, WORD, struct findType * );
int     PASCAL FindNextFile( struct findType * );
int     PASCAL OpenFile( char *, BYTE );
long    PASCAL MoveFilePtr( int, long, int );
int     PASCAL ReadFile( int, LPSTR, WORD );
int     PASCAL WriteFile( int, LPSTR, WORD );
int     PASCAL CloseFile( int );
int     PASCAL CreateFile( char *, BYTE );
int     PASCAL DeleteFile( char * );
int     PASCAL CopyFile( char *, char *, BYTE, LPSTR, WORD );
long    PASCAL GetFileSize( int );
int     PASCAL SetFileSize( int );
long    PASCAL GetFileDateTime( int );
int     PASCAL SetFileDateTime( int, long  );
BYTE    PASCAL GetFileAttr( char * );
int     PASCAL SetFileAttr( char *, BYTE );
