#include <stdio.h>

#define LOCAL_USEAGE
#include "pscript.h"

#define GLOBALSTUFF
#define GPAPERSIZES
#define NULLPRINTERCAPS
#define PRINTERCAPS
#define PRINTERLIES
#define GPAPERBINS
/*#define BINTOFEEDMAP
 */
#include "printers.h"

/*#define DEBUG*/
#ifdef DEBUG
#define DBMSG(msg) printf msg
#else
#define DBMSG(msg)
#endif

int CloseFile(FILE*,char*);
int WriteFile(char*,int,int,FILE*,char*);
FILE * OpenFile(char*,char*);
int Data2File(char*, char*, int, int);

/*
POINT gPaperSizes[NUMPAPERS]
PAPER rgPaper[NUMPAPERS]
PAPER pmPaper[NUMPAPERS]
PRINTER gNullPrCaps
PRINTER gPrCaps[NUMPRINTERS]
char gPaperBins[NUMBINS][BINSTRLEN]
char gBinToFeedMap[NUMBINS]
*/

main()
{
	Data2File("papersiz.dta",(char*)gPaperSizes,sizeof(POINT),NUMPAPERS);
	Data2File("nomargin.dta",(char*)rgPaper,sizeof(PAPER),NUMPAPERS);
	Data2File("ngmargin.dta",(char*)pmPaper,sizeof(PAPER),NUMPAPERS);
	Data2File("paperbin.dta",(char*)gPaperBins,sizeof(char),
		(int)(NUMBINS*BINSTRLEN));
/*	Data2File("bin2feed.dta",(char*)gBinToFeedMap,sizeof(char),NUMBINS);
 */

	/* now covert the printer capabilities structures */

	Data2File("apple1.dta",(char*)&gPrCaps[APPLE],sizeof(PRINTER),1);
	Data2File("apple2.dta",(char*)&gPrCaps[APPLEPLUS],sizeof(PRINTER),1);
	Data2File("dp2665.dta",(char*)&gPrCaps[DPLZR2665],sizeof(PRINTER),1);
	Data2File("decln03r.dta",(char*)&gPrCaps[LN03R],sizeof(PRINTER),1);
	Data2File("dec40.dta",(char*)&gPrCaps[PRINTSERVER40],sizeof(PRINTER),1);
	Data2File("ibm1.dta",(char*)&gPrCaps[IBM1],sizeof(PRINTER),1);
	Data2File("lino.dta",(char*)&gPrCaps[LINOTYPE],sizeof(PRINTER),1);
	Data2File("qmsps800.dta",(char*)&gPrCaps[QMSPS800],sizeof(PRINTER),1);
	Data2File("qmsps80p.dta",(char*)&gPrCaps[QMSPS80P],sizeof(PRINTER),1);
	Data2File("ti08.dta",(char*)&gPrCaps[OMNILASER2108],sizeof(PRINTER),1);
	Data2File("ti.dta",(char*)&gPrCaps[OMNILASER2115],sizeof(PRINTER),1);
	Data2File("wang15.dta",(char*)&gPrCaps[WANGLCS15],sizeof(PRINTER),1);
	Data2File("wang15fp.dta",(char*)&gPrCaps[WANGLCS15FP],sizeof(PRINTER),1);
	Data2File("nullpr.dta",(char*)&gNullPrCaps,sizeof(PRINTER),1);
}

int Data2File(pFileName,pData,iDataSize,iDataCount)
	char *pFileName;
	char *pData;
	int	iDataSize;
	int iDataCount;
{
	FILE *pF,*OpenFile();
	char *pMode;
	int fWriteOK=1;
	int rc=1;

	printf("Converting data into binary file...%s\n",pFileName);
	if(pF=OpenFile(pFileName,"wb")){
		fWriteOK=WriteFile(pData,iDataSize,iDataCount,pF,pFileName);
		if(!fWriteOK || !CloseFile(pF,pFileName)) rc=0;
	}else rc=0;

	DBMSG(("Data2File rc=%d\n",rc));
	return(rc);
}

FILE * OpenFile(pFileName,pModeString)
	char *pFileName;
	char *pModeString;
{
	FILE *pF,*fopen();

	if( !(pF=fopen(pFileName,pModeString)) ){
		printf("*** OPEN ERROR on \"%s\"\n",pFileName);
		pF=0;
	}
	DBMSG(("OpenFile pF=%d\n",pF));
	return(pF);
}

int CloseFile(pFile,pFileName)
	FILE *pFile;
	char *pFileName;
{
	int rc=1;

	if(fclose(pFile)==EOF){
		printf("*** CLOSE ERROR on \"%s\"\n",pFileName);
		rc=0;
	}
	DBMSG(("CloseFile rc=%d\n",rc));
	return(rc);
}

int WriteFile(pData,iDataSize,iDataCount,pF,pFileName)
	char *pData;
	int iDataSize;
	int iDataCount;
	FILE *pF;
	char *pFileName;
{
	int iCount;
	int rc=1;

	if((iCount=fwrite(pData,iDataSize,iDataCount,pF))!=iDataCount){
		printf("*** WRITE ERROR on \"%s\"\n",pFileName);
		rc=0;
	}
	DBMSG(("WriteFile rc=%d, iCount=%d\n",rc,iCount));
	return(rc);
}
