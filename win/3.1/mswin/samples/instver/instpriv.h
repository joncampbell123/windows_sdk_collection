#ifndef DLL
    /*=============== defines for DOS program only   */

    #undef NULL
    #define NULL		0

    #define FALSE		0
    #define TRUE		1
    #define FAR 		far
    #define NEAR		near
    #define VOID		void

    #define PASCAL		pascal
    #define OFSTRUCT    void            /* Not used by the .lib version */
    #define LOWORD(l)		((WORD)(l))
    #define HIWORD(l)		((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
    #define MAKEINTRESOURCE(i)	(LPSTR)((DWORD)((WORD)(i)))
    #define WINAPI		FAR PASCAL
    typedef int 		BOOL;
    typedef unsigned char	BYTE;
    typedef unsigned short	WORD;
    typedef unsigned int	UINT;
    typedef signed long         LONG;
    typedef unsigned long	DWORD;
    typedef char far		*LPSTR;
    typedef const char far*     LPCSTR;
    typedef int                 HFILE;
    typedef WORD far		*LPWORD;
    typedef char near		*PSTR;

    int         _ret;
    unsigned	_error;

    #define FOPEN(sz)		     ((_ret=-1),(_error=_dos_open(sz,O_RDONLY,&_ret)),_ret)
    #define FCREATE(sz) 	     ((_ret=-1),(_error=_dos_creat(sz,_A_NORMAL,&_ret)),_ret)
    #define FCLOSE(fh)		     ((_error=_dos_close(fh)))

    #define FREAD(fh,buf,len)        ((_error=_dos_read(fh,buf,len,&_ret)),_ret)
    #define FWRITE(fh,buf,len)       ((_error=_dos_write(fh,buf,len,&_ret)),_ret)
    #define FERROR()                 _error
    #define FSEEK(fh,off,i)          lseek(fh,(long)(off),i)

    #define ALLOC(n)                 malloc(n)
    #define FREE(p)                  free(p)
    #define SIZE(p)                  _msize(p)
    #define REALLOC(p,n)             realloc(p,n)
    #define FALLOC(n)                _fmalloc(n)
    #define FFREE(n)                 _ffree(n)
    #define MAKEINTRESOURCE(i)	     (LPSTR)((DWORD)((WORD)(i)))

#else

    /*=============== defines for DLL only   */

    #define FALLOC(n)                (VOID FAR *)MAKELONG(0, GlobalAlloc(GPTR, (DWORD)n))
    #define FFREE(n)                 GlobalFree((HANDLE)HIWORD((LONG)n))

    #define FOPEN(sz)		     _lopen(sz,READ)
    #define FCREATE(sz) 	     _lcreat(sz,0)
    #define FCLOSE(fh)		     _lclose(fh)


    #define FREAD(fh,buf,len)        _lread(fh,buf,len)
    #define FWRITE(fh,buf,len)       _lwrite(fh,buf,len)
    #define FSEEK(fh,off,i)          _llseek(fh,(DWORD)off,i)

    #define ALLOC(n)                 (VOID *)LocalAlloc(LPTR,n)
    #define FREE(p)		     LocalFree((HANDLE)p)
    #define SIZE(p)                  LocalSize(p)
    #define REALLOC(p,n)             LocalRealloc(p,n,LMEM_MOVEABLE)

#endif

#define SLASH(c)     ((c) == '/' || (c) == '\\')

/* The default library filenames. This string could also be created by
 * appending the list of filenames together. It is stored in this format to
 * be consistent with the format of the parsed .DAT file buffer, and to
 * simplify the file copying code.
 */
static char  szDefLibFiles[] =
    "ddeml.dll\0ecd.dll\0srvr.dll\0commdlg.dll\0toolhelp.dll\0lzexpand.dll\0shell.dll\0";

/* functions from dos.asm */
int   FAR PASCAL GetCurrentDrive (void);
int   FAR PASCAL SetCurrentDrive (int iDrive);
int   FAR PASCAL DosCwd   (LPSTR szDir);
int   FAR PASCAL DosChDir (LPSTR szDir);
int   FAR PASCAL DosValidDir (LPSTR szDir);
int   FAR PASCAL FileExists (LPSTR szBuf);
int   FAR PASCAL DosDelete (LPSTR szFile);
