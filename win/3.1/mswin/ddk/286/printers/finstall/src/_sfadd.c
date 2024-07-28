/**[f******************************************************************
* $sfadd.c -
*
* Copyright (C) 1990,1991 Hewlett-Packard Company.
*     All rights reserved.
*     Company confidential.
*  2 feb 92 rk Changed $ include files to _ include files
*
**f]*****************************************************************/

/*
 * $Header: 
 */

/*
 * $Log:
 */
  
/*
BOOL FAR PASCAL SSBoxDlgFn(hDB, wMsg, wParam, lParam)
 * 30 Oct 91 rk(HP) Modified SSBoxDlgFn to read FINSTALL.RC for strings to
 *                  load into SSBOX dialog.
 * 21 Aug 91 rk(HP) Changed WriteSelections to call LoadString for
 *                  SF_INSTALLFONT,SF_BUILDFONT,SF_NOSCALABLE,SF_TYPEFILEERR
 *                   SF_NOFONTINSTALL. Used SFINSTAL_NM as title for message
 *                   boxes. Increased err_buf from 80 to 128.
 * 29 Jul 91 rk(HP) Changed text to "DS: PS ITC Zaph Dingbats".
 */

//#define DEBUG
  
#include "nocrap.h"
#undef NOOPENFILE
#undef NOMSG
#undef NOCTLMGR
#undef NOWINMESSAGES
#undef NOSCROLL
#undef NOMEMMGR
#undef NOMB
  
  
#include "_windows.h"
#include "debug.h"
#include "_kludge.h"
#include "_cgifwin.h"
#include "_tmu.h"
#include "_sflib.h"
#include "_readlib.h"
#include "_sfpfm.h"
  
#include "strings.h"
#include "resource.h"
#include "sfdir.h"
#include "sflb.h"
#include "sfutils.h"
#include "mypcleo.h"
#include "utils.h"
#include "sfadd.h"
#include "dlgutils.h"
#include "_sfpfm2.h"
  
#include "sfedit.h"
#include "dosutils.h"
#include "_sfadd.h"
  
/****************************************************************************\
*  DEBUG definitions
\****************************************************************************/
  
#ifdef DEBUG
    #define DBGrdsrc(msg)     /*DBMSG(msg)*/
    #define DBGwrite(msg)     /*DBMSG(msg)*/
    #define DBGdlgfn(msg)     /*DBMSG(msg)*/
    #define DBGutils(msg)     /*DBMSG(msg)*/
#else
    #define DBGrdsrc(msg)     /*null*/
    #define DBGwrite(msg)     /*null*/
    #define DBGdlgfn(msg)     /*null*/
    #define DBGutils(msg)     /*null*/
#endif
  
  
/****************************************************************************\
*  LOACL Definitions
\****************************************************************************/
  
#define LOCAL           static
#define VK_SHIFT          0x10
#define VK_CONTROL     0x11
#define SFLB_CAPTIONWID 29
#define MAXTYPS    250
  
/****************************************************************************\
*  Structure definitions
\****************************************************************************/
  
/****************************************************************************\
* Forward References
\***************************************************************************/
  
LOCAL BOOL GetCWD(LPSTR, int);
BOOL FAR PASCAL QueryCopyFile(LPSTR, LPSTR, HWND, LPSTR);
LPSTR FAR PASCAL LastPartOfFilename(LPSTR);
BOOL FAR PASCAL unlink(LPSTR);
LPSTR FAR PASCAL lmemcpy(LPSTR, LPSTR, WORD);
  
/****************************************************************************\
*  Some Global and Local Variables
\****************************************************************************/
  
  
// dtk 10-19-90
  
LPSFDIRFILE lpSFfile;
  
/*  Structure used by AddFont().
*/
typedef struct {
    char buf[1024];     /* General buffer */
    char path[80];      /* Destination directory path */
    char pfmFile[80];       /* PFM file name */
    char dlFile[80];        /* Download file name */
    char appName[64];       /* Application name for win.ini */
    char sfstr[32];     /* SoftFontn= line from win.ini */
    char cartstr[32];       /* cartridgen= line from win.ini */
    char adding[32];        /* "Adding: " for status line */
    char bldpfm[32];        /* "Building PFM: " for status line */
    char point[32];     /* For description */
    char bold[32];      /* For description */
    char italic[32];        /* For description */
    char fntAppNm[32];      /* App name for fonts section in win.ini */
    int canReplace;     /* ==1 if can replace existing files */
    BOOL duWrned;       /* TRUE if user wrned re: low disk space */
    int fontCount;      /* Number of fonts in dst listbox */
    EDREC edrec;        /* Edit record for editing PFM face */
    OFSTRUCT ofstruct;      /* Open file struct */
    DIRDATA dirdata;        /* Directory data for _opend */
    PFMHEADER pfmHead;      /* PFM file header */
    WORD widths[256];       /* PFM file width table */
    PFMEXTENSION pfmExt;    /* PFM file extension (comes after widths) */
    char face[80];      /* PFM file font face */
    char device[80];        /* PFM file printer name */
    EXTTEXTMETRIC extText;  /* PFM file extended text metrics */
    DRIVERINFO drvInfo; /* PFM file driver info struct */
} ADDREC;
typedef ADDREC FAR *LPADDREC;
  
/*  Structure used by SearchDisk().
*/
typedef struct {
    SFDIRFILE SFfile;
    char more_s[128];
    char file[128];
    PFMHEADER pfmHead;
    EXTTEXTMETRIC extText;
    OFSTRUCT ofstruct;
    WORD state;
    char point[32];
    char bold[32];
    char italic[32];
    char scan[32];
    char buf[512];
} SRCHREC;
typedef SRCHREC FAR *LPSRCHREC;
  
// end dtk
  
BOOL gfontsfound = TRUE;
extern HANDLE hLibInst;
extern char gAppName[64];
extern DIRECTORY gSourceLibrary;
extern int gNumSourceFonts;
  
BOOL gIsCart = FALSE;
BYTE gSymSet[3];
BYTE gPcleoDir[64];
BYTE gTypDir[64];
BOOL HelpWasCalled = FALSE;
  
  
/****************************************************************************\
*  Local and Global Procedures
\****************************************************************************/
/****************************************************************************\
*
*  name: ReadSourceFonts
*
*  description: Search the current working directory for fonts
*
*  return value:
*
*  globals:
*
*  called by:
*
*  history:
*
\****************************************************************************/
  
HANDLE FAR PASCAL ReadSourceFonts(hDB, lpSourcePath, ListBoxID, lpBuf2, hSFlb, lpCount)
HWND hDB;
LPSTR lpSourcePath;
int ListBoxID;
LPSTR  lpBuf2;
HANDLE hSFlb;
LPWORD lpCount;
{
    UWORD NumFaisFaces = 0; /* Must be zero for first call to FAISload */
    UWORD NumLibFaces = 0;  /* Must be zero for first call to LIBload  */
    WORD status;
    WORD i;
    static HANDLE hTypeList = (HANDLE)0;
    char cValue[16];
    char lpPath[128];
    int l, tmp;
    LPTYPEINFO lpTypeInfo;
    LPTYPEINFO lpTypeList = NULL;
    LPSTR p;
    /* dtk  10-19-90 */
    HANDLE hRet=hSFlb;
    LPSRCHREC lpBuf = (LPSRCHREC)lpBuf2;
    int NumTotal;
  
    DBGrdsrc(("ReadSourceFonts in sfadd %ls\n", lpSourcePath));
  
    lstrcpy((LPSTR)lpPath, lpSourcePath);
  
    p = lpPath + lstrlen(lpPath) - 1;
    if ( (p >= lpPath) && (*p == '\\'))
        *p = '\0';
  
  
    DBGrdsrc(("ReadSourceFonts in sfadd %ls\n", (LPSTR)lpPath));
  
  
    /*---------------------------*\
    | Search disk for FAIS files
    \*---------------------------*/
  
    status = FAISlist(lpPath, (LPUWORD) &NumFaisFaces, lpTypeList);   /* determine mem needed */
    if(status)
    {
        DBGrdsrc(("FAISlist error status %d\n", status));
    }
  
    /*-------------------------------------------------------------*\
    | If no FAIS files were found, search for Library Format files
    \*-------------------------------------------------------------*/
  
    if (status || (NumFaisFaces == 0))
    {
        DBGrdsrc(("We are inside the if status loop\n"));
  
        NumFaisFaces = 0;
        if (status = LIBlist(lpPath, (LPUWORD) &NumLibFaces, lpTypeList, UPDATE_IF_FNT))
        {
            DBGrdsrc(("LIBlist error status %d\n", status));
        }
  
        if (status || (NumLibFaces == 0))
        {
            DBGrdsrc(("No libs or fais files were found\n"));
            gfontsfound = FALSE;
            return(hRet);
        }
    }
  
    /*--------------------------------------------*\
    | Allocate memory to hold the found typefaces
    \*--------------------------------------------*/
  
    if((hTypeList = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,
        (LONG)((NumFaisFaces + NumLibFaces) * sizeof(TYPEINFO))))==NULL)
    {
        DBGrdsrc(("Could not allocate memory\n"));
        return(hRet);
    }
  
    /*-------------*\
    | Lock it down
    \*-------------*/
  
    if ((lpTypeList = (LPTYPEINFO)GlobalLock(hTypeList)) == (LPTYPEINFO)NULL)
    {
        GlobalFree(hTypeList);
        DBGrdsrc(("Could not lockmemory\n"));
        return(hRet);
    }
  
    /*----------------------------*\
    | Get all the typefaces found
    \*----------------------------*/
  
    if (NumFaisFaces)
    {
        status = FAISlist(lpPath, (LPUWORD) &NumFaisFaces, lpTypeList);       /* get face names */
        if(status)
        {
            GlobalUnlock(hTypeList);
            GlobalFree(hTypeList);
            DBGrdsrc(("FACElist error status %d\n", status));
            return(hRet);
        }
    }
    else
    {
        if (status = LIBlist(lpPath, (LPUWORD) &NumLibFaces, lpTypeList, UPDATE_IF_FNT))
        {
            DBGrdsrc(("FACElist error status %d\n", status));
            GlobalUnlock(hTypeList);
            GlobalFree(hTypeList);
            return(hRet);
        }
    }
  
    /*----------------------------------------------------------------*\
    | Install typefaces into Font Installer's local library structure
    \*----------------------------------------------------------------*/
  
    lpTypeInfo = lpTypeList;
    gNumSourceFonts = 0;
    NumTotal = NumFaisFaces+NumLibFaces;
  
    if (NumTotal > MAXTYPS)
        MaxDirFAlert(hDB, hLibInst);
    else
    {
  
        for(i = 0;   i < NumTotal; ++i, ++lpTypeInfo)
        {
            DBGrdsrc(("Found face <%ls>\n", (LPSTR) lpTypeInfo->typefaceName));
  
            if (!SameFilename((LPSTR) lpTypeInfo->nameOrDir, (LPSTR) "HQ3UPDT.TYQ") &&
                !SameFilename((LPSTR) lpTypeInfo->nameOrDir, (LPSTR) "PLUGIN.TYQ" ) &&
                !SameFilename((LPSTR) lpTypeInfo->nameOrDir, (LPSTR) "SCREEN.TYS" ))
            {
  
                DBGrdsrc(("Found face <%ls>\n", (LPSTR) lpTypeInfo->typefaceName));
                // dtk 10-19-90
  
                lpBuf->state = SF_FILE;
                lpSFfile = (LPSFDIRFILE)&lpBuf->SFfile;
  
                lpSFfile->fIsPCM=FALSE;
                lpSFfile->orient=0;
                lpSFfile->indDLpath = -1;
                lpSFfile->offsDLname=0;
  
                lstrcpy(lpSFfile->s, lpTypeInfo->typefaceName);
                tmp = (lstrlen(lpSFfile->s) + sizeof(SFDIRFILE));
  
                lpSFfile->indLOGdrv = -1;
                lpSFfile->indScrnFnt = -1;
                lpSFfile->indPFMpath = -1;
                lpSFfile->offsPFMname = 0;
  
                if ((tmp = addSFdirEntry(0L,(LPSTR)lpBuf,lpBuf->state,tmp)) > -1)
                {
                    hRet=addSFlistbox(hDB,hRet,ListBoxID,-1,tmp,
                    SFLB_FAIS,(LPSTR)lpBuf,sizeof(SRCHREC),0L);
                    ++(*lpCount);
                }
                else
                {
                    GlobalUnlock(hTypeList);
                    GlobalFree(hTypeList);
                    return(hRet);
                }
  
                // end of additions
  
                if (AddDirEntry((LPDIRECTORY) &gSourceLibrary,
                    (LPSTR) lpTypeInfo->nameOrDir,
                    (LPSTR) lpTypeInfo->typefaceName,
                    (LPSTR) NULL,
                    (LPSTR) NULL,
                    LIB_ENTRY_TYPE,
                    ListBoxID,
                    lpTypeInfo,
                    (LPSCRNFNTINFO) NULL))
  
                    ++gNumSourceFonts;
                else
                {
                    GlobalUnlock(hTypeList);
                    GlobalFree(hTypeList);
                    return(hRet);
                }
  
  
  
            }
  
        } /* for */
  
    }/* else */
  
    while ((GlobalFlags(hTypeList) & GMEM_LOCKCOUNT) > 0)
        GlobalUnlock(hTypeList);
  
    if (hTypeList)
    {
        GlobalFree(hTypeList);
        hTypeList = 0;
    }
  
    return(hRet);
}
  
/****************************************************************************\
*
*  name: WriteSelections
*
*  description:
*
*  return value:
*
*  globals:
*
*  called by:
*
*  history:
*
\****************************************************************************/
  
HANDLE FAR PASCAL WriteSelections(hDB, lpTypeDir, lpSupportFileDir,
NumInstalled, hdstSFlb, hsrcSFlb)
HWND hDB;
LPSTR lpTypeDir;
LPSTR lpSupportFileDir;
WORD FAR *NumInstalled;
HANDLE hdstSFlb;
HANDLE hsrcSFlb;
{
    int NumFiles, NumFiles2, NumOldFiles=0, NumNewFiles=0;
    LPSTR lpVF = NULL;
    LPSTR lpVF2 = NULL;
    LPSTR lpVF3;
    LPSTR lpFileName;
    LPSFI_FONTLIBENTRY lpLib;
    LPSFI_FONTLIBENTRY lpLib2;
    LPDIRECTORY lpFontLibrary;
  
    HANDLE hAddBuf = 0;
    HANDLE hSrchBuf = 0;
    LPADDREC lpAddBuf = 0L;
    LPSRCHREC lpSrchBuf = 0L;
    LPSFDIRFILE lpSFfile;   /* ptr to SFfile field of lpBuf */
    LPSTR s;
    LPSTR lpFile;       /* temp ptr to file section of buffer */
    BYTE fontname[FACESIZE];    /* font name from build_ptm */
    int tmp,i,y,id;
    int mayReplace = -1;
    int BoldItal;       /* Bold/Italic designator from build_ptm */
  
    int response;
    int status = 0;
    char StatusLine[128];
    //  HCURSOR hCursor;
    char Message[128];
    char DstFile[MAX_FILENAME_SIZE];
    int WasCopied;
    int fsnum, numtyps, ifwnum;
    LONG  font_id;
    BYTE typ_id_buf[10];               /* buffer used when getting font_id */
    BYTE win_str1[32];
    BYTE win_str2[300];
    BYTE pfm_name[128];
    BYTE pcleo_name[128];
    BYTE fs_numstr[5];
    BYTE tp_buf[80];
    BYTE pp_buf[80];
    BYTE pfm_path[80];
    BYTE pcleo_path[80];
//    BYTE err_buf[80];
    BYTE err_buf[128];
    BOOL Makepfm = FALSE;
    BOOL Cancel = FALSE;
    HANDLE hPcleo = 0;
    BOOL goodTyp, newifw;
  
    /* Stuff from sfadd2.c */
    LPSFLB lpsrcSFlb = 0L;      /* list box struct */
    LPSFLBENTRY sflb = 0L;      /* list box entry */
    LPSFLBENTRY j, k;
    WORD prevPos = 0;           /* Position indx for addSFlistbox */
    BYTE desc[128];         /* Font description for LB */
  
    DBGwrite(("WriteSelections in sfadd\n"));
  
  
    *NumInstalled = 0;
    numtyps = 0;
  
    lpFontLibrary = (LPDIRECTORY) &gSourceLibrary;
  
    if (lpFontLibrary->hFiles == NULL)
    {
        DBGwrite(("returnig because of lpfontlibrary\n"));
        return(hdstSFlb);
    }
  
    if ((lpVF3 = lpVF = (LPSTR) GlobalLock(lpFontLibrary->hFiles)) == NULL)
    {
        DBGwrite(("WriteSelection: Unable to lock memory\n"));
        return(hdstSFlb);
    }
  
    /* Get the symbol set selection.
    */
  
    MyDialogBox(hLibInst,SSBOX,hDB,SSBoxDlgFn);
  
    /* If it is a cartridge font only prompt for a
    * typ file directory, otherwise they want a pcleo
    * so also get the pcleo path.
    */
  
    if(gIsCart)
    {
        if (GetTypDir(hDB,hLibInst,tp_buf,sizeof(tp_buf),(LPSTR)gAppName))
            Makepfm = FALSE;
        else
        {
            Cancel = TRUE;
//      GlobalUnlock(lpFontLibrary->hFiles);
//      return(hdstSFlb);
        }
  
    }
    else
    {
        if (GetBothDirs(hDB,hLibInst,tp_buf,pp_buf,sizeof(tp_buf), sizeof(pp_buf),(LPSTR)gAppName))
        {
            lstrcpy(pfm_path, pp_buf);
            lstrcpy(pcleo_path, pp_buf);
            Makepfm = TRUE;
        }
        else
        {
            Cancel = TRUE;
//      GlobalUnlock(lpFontLibrary->hFiles);
//      return(hdstSFlb);
        }
    }
  
    /* This takes time, so set the cursor to an hour glass
    */
    /* hCursor = */ SetCursor(LoadCursor(NULL, IDC_WAIT));
    //  ShowCursor(TRUE);
  
    /* Get the number of IFW fonts
    */
    ifwnum = GetProfileInt((LPSTR)gAppName,(LPSTR)"IfwFonts",0);
  
  
    if (Cancel)
    {
        lpVF2 = lpVF3;
        for (NumFiles2 = 0; NumFiles2<gSourceLibrary.NumFiles; ++NumFiles2)
        {
            lpLib2 = (LPSFI_FONTLIBENTRY)lpVF2;
  
            if ((lpLib2->Type & LIB_ENTRY_PRINTER) &&
                (lpLib2->Selected > 0) &&
                (lpLib2->usage > 0))
                lpLib2->Selected = FALSE;
  
            /* skip to the next font
            */
            lpVF2 += lpLib2->Length;
        } /* for */
    }
    else
    {
        for (NumFiles = 0; NumFiles<gSourceLibrary.NumFiles; ++NumFiles)
        {
            lpLib = (LPSFI_FONTLIBENTRY)lpVF;
            if ((lpLib->Type & LIB_ENTRY_PRINTER) &&
                (lpLib->Selected > 0) &&
                (lpLib->usage > 0))
            {
//                lstrcpy((LPSTR) StatusLine,(LPSTR) "Installing source font: ");
                LoadString(hLibInst, SF_INSTALLFONT, (LPSTR)StatusLine,
                                    sizeof(StatusLine));                                 // rk/HP 08/21/90
                lstrcat(StatusLine, (LPSTR) lpLib+lpLib->OffsetName);
                SetDlgItemText(hDB, SF_STATUS, (LPSTR)StatusLine);
  
                lstrcpy(DstFile, (LPSTR)tp_buf);
                lstrcat(DstFile, (LPSTR) "\\");
                ltoa(lpLib->TypeInfo.id,(LPSTR)typ_id_buf);
                lstrcat(DstFile, typ_id_buf);
                lstrcat(DstFile, ".TYP");
  
                /* Init status (build_ptm sets it to 1) */
                status = 0;
                goodTyp = FALSE;
                newifw = FALSE;
  
                if (lpLib->TypeInfo.complement == 0)
                    status = !QueryCopyFile(lpLib->TypeInfo.nameOrDir, DstFile, hDB, (LPSTR) NULL);
  
                if (!status)
                {
                    status = FACEinstall((LPTYPEINFO) &(lpLib->TypeInfo),
                    (LPSTR)tp_buf, lpSupportFileDir, UPDATE_IF_FNT, Makepfm, gSymSet, pfm_name);
  
                    if ((!status) || (status == ERRface_entry_exists))
                    {
                        font_id = lpLib->TypeInfo.id;    /* Get the font id number */
                        hPcleo = LoadLibrary("PCLEO.DLL");
                        if (hPcleo >= 32)
                        {
                            goodTyp = build_ptm(hPcleo, font_id,
                            ((gSymSet[1] << 8) | gSymSet[0]),
                            (LPSTR)pfm_name, (LPSTR)fontname,
                            (int FAR *)&BoldItal, TRUE);
                        }
                    }
  
                    if (goodTyp)
                    {
                        if (!status)     /* Not a dup */
                        {
                            if (ifwnum >= MAXTYPS)
                            {
                                MaxFontAlert(hDB, hLibInst);
                                break;
                            }
                            ++ifwnum;
                            newifw = TRUE;
  
                            /* Add to destination list box */
                            if (hsrcSFlb && (lpsrcSFlb = (LPSFLB)GlobalLock(hsrcSFlb)))
                            {
                                sflb = (LPSFLBENTRY)&lpsrcSFlb->sflb[lpLib->ListboxEntry];
  
                                hdstSFlb = addSFlistbox(hDB, hdstSFlb,
                                SF_LB_LEFT, -1, sflb->indSFfile, SFLB_FAIS,
                                (LPSTR)desc, sizeof(desc), &prevPos);
  
                                GlobalUnlock(hsrcSFlb);
                            }
                        }   /* if not a dup */
  
                        /* Nuke entrty in source list box */
                        if (hsrcSFlb && (lpsrcSFlb = (LPSFLB)GlobalLock(hsrcSFlb)))
                        {
                            sflb = (LPSFLBENTRY)&lpsrcSFlb->sflb[lpLib->ListboxEntry];
  
                            /*  Remove the entry from the source listbox.
                            */
                            SendDlgItemMessage(hDB, SF_LB_RIGHT,
                            LB_DELETESTRING,
                            (WORD)lpLib->ListboxEntry, 0L);
  
                            /* Note: this is an inefficient fix that
                            *       updates indexes.  This code should
                            *       be reworked.  Really.
                            */
                            lpVF2 = lpVF3;
                            for (NumFiles2 = 0; NumFiles2<gSourceLibrary.NumFiles; ++NumFiles2)
                            {
                                lpLib2 = (LPSFI_FONTLIBENTRY)lpVF2;
  
                                if ((lpLib2->Type & LIB_ENTRY_PRINTER) &&
                                    (lpLib2->Selected > 0) &&
                                    (lpLib2->usage > 0) &&
                                    (lpLib2->ListboxEntry > lpLib->ListboxEntry))
                                    lpLib2->ListboxEntry--;
  
                                /* skip to the next font
                                */
                                lpVF2 += lpLib2->Length;
  
                            } /* for */
  
                            /*  Shuffle the contents of the SFLB struct
                            *  back one item.
                            */
                            for (j=&sflb[0],k=&sflb[1],i=(int)lpLib->ListboxEntry+1;
                                i < lpsrcSFlb->free; ++i, ++j, ++k)
                                *j = *k;
  
                            --lpsrcSFlb->free;
  
                            GlobalUnlock(hsrcSFlb);
                        }
  
                        ++(*NumInstalled);
                        lpLib->Selected = 0;
                        lpLib->usage = 0;
  
                        if (Makepfm)
                        {
  
                            font_id = lpLib->TypeInfo.id;    /* Get the font id number */
  
                            /* copy building pcleo message to the status line.
                            */
//                            lstrcpy((LPSTR) StatusLine,(LPSTR)"Building scalable printer font for: ");
                            LoadString(hLibInst, SF_BUILDFONT, (LPSTR)StatusLine,
                                    sizeof(StatusLine));                                 // rk/HP 08/21/90
                            lstrcat(StatusLine, (LPSTR) lpLib+lpLib->OffsetName);
                            SetDlgItemText(hDB, SF_STATUS, (LPSTR)StatusLine);
  
                            lstrcpy((LPSTR) pfm_name, (LPSTR) pfm_path);
  
  
                            /* make the pfm file
                            */
                            status = build_ptm(hPcleo, font_id,
                            ((gSymSet[1] << 8) | gSymSet[0]),
                            (LPSTR)pfm_name, (LPSTR)fontname,
                            (int FAR *)&BoldItal, FALSE);
  
                            /* call bullet to generate a pcleo.
                            */
                            if ((status==1) && (WritePcleo(hPcleo, pcleo_path,
                                gSymSet, font_id, (LPSTR)pcleo_name)) &&
                                (hAddBuf = GlobalAlloc(GMEM_FIXED,(DWORD)sizeof(ADDREC))) &&
                                (lpAddBuf=(LPADDREC)GlobalLock(hAddBuf)) &&
                                (hSrchBuf = GlobalAlloc(GMEM_FIXED,(DWORD)sizeof(SRCHREC))) &&
                                (lpSrchBuf=(LPSRCHREC)GlobalLock(hSrchBuf)))
                            {
                                lpSFfile = (LPSFDIRFILE)&lpSrchBuf->SFfile;
  
                                lpSrchBuf->state = SF_FILE;
  
                                lpSFfile->fIsPCM = FALSE;
  
                                lstrcpy(lpSFfile->s, (LPSTR)fontname);
  
                                if (BoldItal & DESC_BOLD)
                                {
                                    LoadString(hLibInst, SF_BOLD, lpAddBuf->buf,
                                    sizeof(lpAddBuf->buf));
  
                                    lstrcat(lpSFfile->s, lpAddBuf->buf);
                                }
  
                                if (BoldItal & DESC_ITAL)
                                {
                                    LoadString(hLibInst, SF_ITALIC, lpAddBuf->buf,
                                    sizeof(lpAddBuf->buf));
  
                                    lstrcat(lpSFfile->s, lpAddBuf->buf);
                                }
  
                                tmp = lstrlen((LPSTR)lpSFfile->s) + 1;
  
                                /*  Step backward thru file name stopping at
                                *  the end of the path.
                                */
                                lpFile = lpSrchBuf->file;
                                lstrcpy(lpFile, (LPSTR)pcleo_name);
                                for (s = lpFile + lstrlen(lpFile);
                                    (s>lpFile) && (s[-1]!=':') && (s[-1]!='\\')
                                    && (s[-1]!=' '); --s)
                                    ;
  
                                /*  If there is a path, insert it.
                                */
                                if (s > lpFile)
                                {
                                    /*  Turn the end character into a null.
                                    */
                                    BYTE savec = *s;
                                    *s = '\0';
  
                                    /*  Insert string path name, allow 2 bytes
                                    *  before string for use by the SF
                                    *  directory utils & 1 byte at the end for
                                    *  the null-terminator.
                                    */
                                    lpSFfile->indDLpath =
                                    addSFdirEntry(0L, lpFile-2, SF_PATH,
                                    lstrlen(lpFile)+3);
  
                                    *s = savec;
                                }
                                else
                                    lpSFfile->indDLpath = -1;   /* glue err */
  
                                /*  Copy file name to after face name in
                                *  SFDIRFILE struct (its safe to do a strcpy
                                *  because the 'file' field follows the
                                *  'more_s' field in the SRCHREC, so at worse
                                *  we'll overwrite something we don't need).
                                */
                                lstrcpy((LPSTR)&lpSFfile->s[tmp], s);
                                lpSFfile->offsDLname = tmp;
  
                                /*  Adjust length to reflect size of whole
                                *  structure (including terminating NULL on
                                *  DL file name).
                                */
                                tmp += lstrlen(s) + sizeof(SFDIRFILE);
  
                                lpSFfile->orient = 0;      /* No bitmap softfnts */
                                lpSFfile->indLOGdrv = -1;
                                lpSFfile->indScrnFnt = -1;
                                lpSFfile->indPFMpath = -1;
                                lpSFfile->offsPFMname = 0;
  
                                /* Add SFdir entry and display in list box */
                                if ((tmp = addSFdirEntry(0L, (LPSTR)lpSrchBuf,
                                    lpSrchBuf->state,tmp)) > -1)
                                {
                                    if ((i = dupSFlistbox(hdstSFlb, tmp,
                                        lpAddBuf->buf, sizeof(lpAddBuf->buf))) > -1)
                                    {
                                        if (!CanReplace(hDB, hLibInst, &mayReplace,
                                            lpAddBuf->buf,sizeof(lpAddBuf->buf)))
                                        {
                                            /* skip to the next font
                                            */
                                            GlobalUnlock(hAddBuf);
                                            GlobalUnlock(hSrchBuf);
                                            lpVF += lpLib->Length;
                                            continue;
                                        }
  
                                        hdstSFlb = replaceSFlistbox(hDB,
                                        hdstSFlb, SF_LB_LEFT, i, &id, tmp,
                                        0, (LPSTR)lpAddBuf, sizeof(ADDREC));
                                    }
                                    else
                                    {
                                        id = getUniqueID(hdstSFlb);
  
                                        hdstSFlb = addSFlistbox(hDB, hdstSFlb,
                                        SF_LB_LEFT, id, tmp, 0,
                                        (LPSTR)lpAddBuf, sizeof(ADDREC), 0L);
                                    }
  
                                    /*  Both files successfully copied, change
                                    *  the path names.
                                    */
                                    chngSFdirPath(0L, tmp, TRUE, pfm_path);
                                    chngSFdirPath(0L, tmp, FALSE, pcleo_path);
  
                                    /* get # of softfont entries from win.ini
                                    */
                                    LoadString(hLibInst, SF_SOFTFONT, win_str1,
                                    sizeof(win_str1));
                                    itoa(id, (LPSTR)&win_str1[lstrlen(win_str1)]);
  
                                    /* bld win.ini string for the pcleo & pfm.
                                    */
                                    lstrcpy((LPSTR)win_str2, (LPSTR)pfm_name);
                                    lstrcat((LPSTR)win_str2, (LPSTR)",");
                                    lstrcat((LPSTR)win_str2, (LPSTR)pcleo_name);
  
                                    DBGwrite(("win_str1 = %ls\n", (LPSTR)win_str1));
                                    DBGwrite(("win_str2 = %ls\n", (LPSTR)win_str2));
  
                                    LoadString(hLibInst, SF_SOFTFONTS, lpAddBuf->buf,
                                    sizeof(lpAddBuf->buf));
                                    fsnum = GetProfileInt(gAppName,
                                    lpAddBuf->buf, 0) + 1;
  
                                    /* write the win.ini strings.
                                    */
                                    WriteProfileString((LPSTR)gAppName,
                                    (LPSTR)win_str1, (LPSTR)win_str2);
                                    itoa(fsnum, fs_numstr);
                                    WriteProfileString((LPSTR)gAppName,
                                    lpAddBuf->buf, (LPSTR)fs_numstr);
                                }
  
                                GlobalUnlock(hAddBuf);
                                GlobalUnlock(hSrchBuf);
                            }
  
                            else
//                                MessageBox(hDB, (LPSTR)"Scalable printer font file not built!", NULL, MB_OK | MB_ICONEXCLAMATION);
                                  if( LoadString(hLibInst,SF_NOSCALABLE,
                                                Message,sizeof(Message)) &&                  // rk 7/10/91
                                     (y=lstrlen(Message)+1) &&
                                     (LoadString(hLibInst,SFINSTAL_NM,
                                                &Message[y],sizeof(Message)-y)) )              // rk 7/10/91
                                          MessageBox(hDB,Message,&Message[y],MB_SYSTEMMODAL);
                                   else
                                          MessageBox(hDB, (LPSTR)"Scalable printer font file not built!", 
                                                NULL, MB_OK | MB_ICONEXCLAMATION);
                        }
  
                    }
                    else
                    {
                        lpLib->Selected = 0;
                        FACEdelete((LPTYPEINFO) &lpLib->TypeInfo, lpSupportFileDir, FALSE);

                        if( LoadString(hLibInst,SF_TYPEFILEERR,err_buf,
                                 sizeof(err_buf)) &&                                     // rk 8/21/91
                            lstrcat((LPSTR)err_buf, (LPSTR)typ_id_buf) &&
                            (y=lstrlen(err_buf)) &&  /* trash NULL terminator */
                            (LoadString(hLibInst,SF_NOFONTINSTAL,&err_buf[y],
                                 sizeof(err_buf)-y)) &&
                            (LoadString(hLibInst,SFINSTAL_NM,Message,
                                 sizeof(Message))) )
                         {
                         MessageBox(hDB, (LPSTR)err_buf, (LPSTR)Message, MB_OK | MB_ICONEXCLAMATION);
                         }

                         else
                         {
                         lstrcpy((LPSTR)err_buf, (LPSTR) "Error in typeface file - ");
                         lstrcat((LPSTR)err_buf, (LPSTR)typ_id_buf);
                         lstrcat((LPSTR)err_buf, (LPSTR)".typ.\nFont not installed!");
                         MessageBox(hDB, (LPSTR)err_buf, NULL, MB_OK | MB_ICONEXCLAMATION);
                         }
                    }
                }
  
            }
            /* skip to the next font
            */
            lpVF += lpLib->Length;
  
        } /* for */
    } /* else */
  
    /*  Enable listbox.
    */
    //SendMessage(GetDlgItem(hDB,SF_LB_RIGHT), WM_SETREDRAW, FALSE, 0L);
    //SendMessage(GetDlgItem(hDB,SF_LB_LEFT), WM_SETREDRAW, FALSE, 0L);
  
    EnableWindow(GetDlgItem(hDB,SF_LB_RIGHT), TRUE);
    EnableWindow(GetDlgItem(hDB,SF_LB_LEFT), TRUE);
    EnableWindow(GetDlgItem(hDB, SF_ADD_RIGHT), TRUE);
    SendMessage(GetDlgItem(hDB,SF_LB_RIGHT), WM_VSCROLL, SB_TOP, 0L);
    SendMessage(GetDlgItem(hDB,SF_LB_LEFT), WM_VSCROLL, SB_TOP, 0L);
    SendMessage(GetDlgItem(hDB,SF_LB_RIGHT), WM_SETREDRAW, TRUE, 0L);
    SendMessage(GetDlgItem(hDB,SF_LB_LEFT), WM_SETREDRAW, TRUE, 0L);
    InvalidateRect(GetDlgItem(hDB,SF_LB_RIGHT), (LPRECT)0L, TRUE);
    InvalidateRect(GetDlgItem(hDB,SF_LB_LEFT), (LPRECT)0L, TRUE);
  
    EnableWindow(GetDlgItem(hDB, SF_LB_RIGHT), TRUE);
    InvalidateRect(GetDlgItem(hDB,SF_LB_LEFT), (LPRECT)0L, FALSE);
    SetDlgItemText(hDB, SF_STATUS, (LPSTR)"");
  
    /* Update the number of screen fonts added
    */
    if (newifw)
    {
        itoa(ifwnum, (LPSTR)fs_numstr);
        WriteProfileString((LPSTR)gAppName,(LPSTR)"IfwFonts", (LPSTR)fs_numstr);
    }
  
    if (hPcleo)
        FreeLibrary(hPcleo);
  
  
    /* Set the cursor back to a pointer, clean-up & return
    */
//  ShowCursor(FALSE);
//  SetCursor(hCursor);
    SetCursor(LoadCursor(NULL,IDC_ARROW));
    GlobalUnlock(lpFontLibrary->hFiles);
    return(hdstSFlb);
}
  
/****************************************************************************
*
*    SSBoxDlgFn: Function for the select sysmbol set dialog box. (export)
*
*****************************************************************************/
  
  
BOOL FAR PASCAL SSBoxDlgFn(hDB, wMsg, wParam, lParam)
HWND hDB;
unsigned wMsg;
WORD wParam;
LONG lParam;
{
    char buff[8];
    char symbol_string[81];
    ULONG  val;
    int  index;
    int  sym_index;
  
    switch (wMsg)
    {
        case WM_INITDIALOG:
  
            DBGdlgfn(("Inside SSBoxDlgFn:WM_INITIALDIALOG\n"));
  
            gIsCart = FALSE;
  
            SendDlgItemMessage(hDB, SYMBOL_SET, WM_SETREDRAW, FALSE, 0L);

            for (sym_index = 0; sym_index < 23; sym_index++)
            {
                 if (LoadString(hLibInst,(SF_SS_OFFSET + sym_index), (LPSTR)symbol_string,
                               sizeof(symbol_string))) 
                    SendDlgItemMessage(hDB, SYMBOL_SET, LB_INSERTSTRING, 
                              (WORD)-1,(LONG)(LPSTR)symbol_string);
            }

//            SendDlgItemMessage(hDB, SYMBOL_SET, LB_INSERTSTRING, (WORD)-1,(LONG)(LPSTR)"Cartridge - Screen fonts only");
//            SendDlgItemMessage(hDB, SYMBOL_SET, LB_INSERTSTRING, (WORD)-1,(LONG)(LPSTR)"WN:  Windows");
//            SendDlgItemMessage(hDB, SYMBOL_SET, LB_INSERTSTRING, (WORD)-1,(LONG)(LPSTR)"D1:  ITC Zapf Dingbats/100");
//            SendDlgItemMessage(hDB, SYMBOL_SET, LB_INSERTSTRING, (WORD)-1,(LONG)(LPSTR)"D2:  ITC Zapf Dingbats/200");
//            SendDlgItemMessage(hDB, SYMBOL_SET, LB_INSERTSTRING, (WORD)-1,(LONG)(LPSTR)"D3:  ITC Zapf Dingbats/300");
//            SendDlgItemMessage(hDB, SYMBOL_SET, LB_INSERTSTRING, (WORD)-1,(LONG)(LPSTR)"DS:  PS ITC Zapf Dingbats");
//            SendDlgItemMessage(hDB, SYMBOL_SET, LB_INSERTSTRING, (WORD)-1,(LONG)(LPSTR)"DV:  Ventura ITC Zapf Dingbats");
//            SendDlgItemMessage(hDB, SYMBOL_SET, LB_INSERTSTRING, (WORD)-1,(LONG)(LPSTR)"DT:  Desk Top");
//            SendDlgItemMessage(hDB, SYMBOL_SET, LB_INSERTSTRING, (WORD)-1,(LONG)(LPSTR)"E1:  ECMA-94 Latin 1");
//            SendDlgItemMessage(hDB, SYMBOL_SET, LB_INSERTSTRING, (WORD)-1,(LONG)(LPSTR)"LG:  Legal");
//            SendDlgItemMessage(hDB, SYMBOL_SET, LB_INSERTSTRING, (WORD)-1,(LONG)(LPSTR)"M8:  Math-8");
//            SendDlgItemMessage(hDB, SYMBOL_SET, LB_INSERTSTRING, (WORD)-1,(LONG)(LPSTR)"MS:  PS Math");
//            SendDlgItemMessage(hDB, SYMBOL_SET, LB_INSERTSTRING, (WORD)-1,(LONG)(LPSTR)"PB:  Microsoft Publishing");
//            SendDlgItemMessage(hDB, SYMBOL_SET, LB_INSERTSTRING, (WORD)-1,(LONG)(LPSTR)"PC:  PC-8");
//            SendDlgItemMessage(hDB, SYMBOL_SET, LB_INSERTSTRING, (WORD)-1,(LONG)(LPSTR)"PD:  PC-8 D/N");
//            SendDlgItemMessage(hDB, SYMBOL_SET, LB_INSERTSTRING, (WORD)-1,(LONG)(LPSTR)"PI:  Pi Font");
//            SendDlgItemMessage(hDB, SYMBOL_SET, LB_INSERTSTRING, (WORD)-1,(LONG)(LPSTR)"PM:  PC-850");
//            SendDlgItemMessage(hDB, SYMBOL_SET, LB_INSERTSTRING, (WORD)-1,(LONG)(LPSTR)"R8:  Roman-8");
//            SendDlgItemMessage(hDB, SYMBOL_SET, LB_INSERTSTRING, (WORD)-1,(LONG)(LPSTR)"TS:  PS Text");
//            SendDlgItemMessage(hDB, SYMBOL_SET, LB_INSERTSTRING, (WORD)-1,(LONG)(LPSTR)"US:  ASCII");
//            SendDlgItemMessage(hDB, SYMBOL_SET, LB_INSERTSTRING, (WORD)-1,(LONG)(LPSTR)"VI:  Ventura International");
//            SendDlgItemMessage(hDB, SYMBOL_SET, LB_INSERTSTRING, (WORD)-1,(LONG)(LPSTR)"VM:  Ventura Math");
//            SendDlgItemMessage(hDB, SYMBOL_SET, LB_INSERTSTRING, (WORD)-1,(LONG)(LPSTR)"VU:  Ventura US");
  
            SendDlgItemMessage(hDB, SYMBOL_SET, LB_SETCURSEL, (WORD)1, 0L);
            SendDlgItemMessage(hDB, SYMBOL_SET, WM_SETREDRAW, TRUE, 0L);
            InvalidateRect(GetDlgItem(hDB,SYMBOL_SET), (LPRECT)0L, FALSE);
  
            CenterDlg(hDB);
            break;
  
        case WM_COMMAND:
            DBGdlgfn(("Inside SSBoxDlgFn:WM_COMMAND\n"));
  
        switch (wParam)
        {
  
            case SF_HELP:    // HELP pushbutton -- run help.
  
                HelpWasCalled = WinHelp(hDB, (LPSTR) "finstall.hlp",
                (WORD) HELP_INDEX,
                (DWORD) 0L);
                break;
  
  
            case IDOK:
  
                DBGdlgfn(("Inside SSBoxDlgFn:IDOK\n"));
  
                index = (int)(SendDlgItemMessage(hDB, SYMBOL_SET, LB_GETCURSEL, 0, 0L));
  
                DBGdlgfn(("Symbol Set index = %ld\n", index));
            switch (index)
            {
                case 1:
                    lstrcpy(gSymSet, (LPSTR)"WN");
                    break;
  
                case 2:
                    lstrcpy(gSymSet, (LPSTR)"D1");
                    break;
  
                case 3:
                    lstrcpy(gSymSet, (LPSTR)"D2");
                    break;
  
                case 4:
                    lstrcpy(gSymSet, (LPSTR)"D3");
                    break;
  
                case 5:
                    lstrcpy(gSymSet, (LPSTR)"DS");
                    break;
  
                case 6:
                    lstrcpy(gSymSet, (LPSTR)"DV");
                    break;
  
                case 7:
                    lstrcpy(gSymSet, (LPSTR)"DT");
                    break;
  
                case 8:
                    lstrcpy(gSymSet, (LPSTR)"E1");
                    break;
  
                case 9:
                    lstrcpy(gSymSet, (LPSTR)"LG");
                    break;
  
                case 10:
                    lstrcpy(gSymSet, (LPSTR)"M8");
                    break;
  
                case 11:
                    lstrcpy(gSymSet, (LPSTR)"MS");
                    break;
  
                case 12:
                    lstrcpy(gSymSet, (LPSTR)"PB");
                    break;
  
                case 13:
                    lstrcpy(gSymSet, (LPSTR)"PC");
                    break;
  
                case 14:
                    lstrcpy(gSymSet, (LPSTR)"PD");
                    break;
  
                case 15:
                    lstrcpy(gSymSet, (LPSTR)"PI");
                    break;
  
                case 16:
                    lstrcpy(gSymSet, (LPSTR)"PM");
                    break;
  
                case 17:
                    lstrcpy(gSymSet, (LPSTR)"R8");
                    break;
  
                case 18:
                    lstrcpy(gSymSet, (LPSTR)"TS");
                    break;
  
                case 19:
                    lstrcpy(gSymSet, (LPSTR)"US");
                    break;
  
                case 20:
                    lstrcpy(gSymSet, (LPSTR)"VI");
                    break;
  
                case 21:
                    lstrcpy(gSymSet, (LPSTR)"VM");
                    break;
  
                case 22:
                    lstrcpy(gSymSet, (LPSTR)"VU");
                    break;
  
                case 0:
                default:
                    lstrcpy(gSymSet, (LPSTR)"00");
                    gIsCart = TRUE;
                    break;
            }
  
                DBGdlgfn(("Symbol Set = %ls\n", (LPSTR)gSymSet));
                EndDialog(hDB, wParam);
                break;
  
        }
  
            break;
  
        default:
            return FALSE;
    }
  
    return TRUE;
  
}   // SSBoxDlgFn
  
  
  
  
/****************************************************************************\
*
*  name:GetCWD
*
*  description:
*
*  return value:
*
*  globals:
*
*  called by:
*
*  history:
*
\****************************************************************************/
  
LOCAL BOOL GetCWD(lpPath, MaxLength)
LPSTR lpPath;
int MaxLength;
{
    int i;
  
    DBGutils(("GetCWD in sfadd\n"));
  
    lstrcpy(lpPath, (LPSTR) "*.*");
    return(TRUE);
}
