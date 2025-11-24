extern	VOID BuildLists(VOID);
extern	VOID Install(VOID);
extern	VOID BuildCopyList(VOID);
extern	VOID BuildBillboardList(VOID);
extern	VOID AskQuit(VOID);

#define	cchMax	255

extern	int		installType;
extern	char	szInstallPath[cchMax];
extern	char	szSrcDir[cchMax];

#define itComplete           1
#define itCustom             2
#define itMinimum            3


// these are usually defined as used in CUI.DLL
#define	 DLG_WELCOME				 100
#define  HELP_APPLICATION			 900
#define  DLG_INSTALLPATH			 300
#define  HELP_INSTALLPATH			 900
#define  DLG_INVALIDPATH			6400
#define  DLG_DESTISSRC				6400
#define  DLG_PATHTOOLONG			6400
#define  DLG_BLANK					6400

#define  DLG_CREATEDIR				 100
#define  HELP_CREATEDIR				 900

#define  DLG_INSTALLTYPE			 800
#define  HELP_INSTALLTYPE			 900

#define  DLG_BOARD1					5000
#define  DLG_BOARD2					5000
#define  DLG_BOARD3					5000
#define  DLG_BOARD4					5000
#define  DLG_REGINI					5000

#define  DLG_QUIT					 200
#define  HELP_ASKQUIT				 900
#define  DLG_ERROR					 400

#define  IDC_CONTINUE				((WORD)('C'+'O'))
#define  IDC_CANCEL					((WORD)('C'+'A'))
#define  IDC_PATHTOOLONGEXIT		((WORD)('E'+'X'))
#define  IDC_ERROREXIT				((WORD)('C'+'O'))
#define  IDC_QUITEXIT				((WORD)('E'+'X'))
#define  IDC_QUITCONTINUE			((WORD)('C'+'O'))
#define  IDC_TYPEEXIT				((WORD)('E'+'X'))
#define  IDC_CREATEEXIT				((WORD)('E'+'X'))
#define  IDC_CREATERETRY			((WORD)('R'+'E'))
#define  IDC_CREATECONTINUE			((WORD)('C'+'O'))
#define  IDC_BLANKEXIT				((WORD)('E'+'X'))
#define  IDC_BLANKCONTINUE			((WORD)('C'+'O'))
#define  IDC_DESTISSRCEXIT			((WORD)('E'+'X'))
#define  IDC_DESTISSRCCONTINUE		((WORD)('C'+'O'))
#define  IDC_INVALIDEXIT			((WORD)('E'+'X'))
#define  IDC_INVALIDCONTINUE		((WORD)('C'+'O'))
#define  IDC_PATHEXIT				((WORD)('E'+'X'))
#define  IDC_PATHCANCEL				((WORD)('C'+'A'))
#define  IDC_BACK					((WORD)('B'+'A'))
