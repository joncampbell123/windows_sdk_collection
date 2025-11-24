
/******************************************************************************\
*       This is a part of the Microsoft Source Code Samples. 
*       Copyright (C) 1993-1995 Microsoft Corporation.
*       All rights reserved. 
*       This source code is only intended as a supplement to 
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the 
*       Microsoft samples programs.
\******************************************************************************/

/*++

Module Name:

    Remote.c

Abstract:

    This module contains the main() entry point for Remote.
    Calls the Server or the Client depending on the first parameter.


Author:

    Rajivendra Nath (rajnath) 2-Jan-1993

Environment:

    Console App. User mode.

Revision History:

--*/


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "Remote.h"

char   HostName[HOSTNAMELEN];
char*  ChildCmd;
char*  PipeName;
char*  ServerName;
HANDLE MyOutHandle;

BOOL   IsAdvertise=TRUE;
DWORD  ClientToServerFlag;

char* ColorList[]={"black" ,"blue" ,"green" ,"cyan" ,"red" ,"purple" ,"yellow" ,"white",
                   "lblack","lblue","lgreen","lcyan","lred","lpurple","lyellow","lwhite"};

WORD
GetColorNum(
    char* color
    );

VOID
SetColor(
    WORD attr
    );

BOOL
GetNextConnectInfo(
    char** SrvName,
    char** PipeName
    );



CONSOLE_SCREEN_BUFFER_INFO csbiOriginal;

main(
    int    argc,
    char** argv
    )
{
    WORD  RunType;              // Server or Client end of Remote
    DWORD len=HOSTNAMELEN-1;
    int   i;

    char  sTitle[100];          // New Title
    char  orgTitle[100];        // Old Title
    BOOL  bSetAttrib=FALSE;     // Change Console Attributes
    BOOL  bPromptForArgs=FALSE; // Is /P option
    WORD  wAttrib;              // Console Attributes

    GetComputerName((LPTSTR)HostName,&len);

    MyOutHandle=GetStdHandle(STD_OUTPUT_HANDLE);

    GetConsoleScreenBufferInfo(MyOutHandle,&csbiOriginal);





    //
    // Parameter Processing
    //
    // For Server:
    // Remote /S <Executable>  <PipeName> [Optional Params]
    //
    // For Client:
    // Remote /C <Server Name> <PipeName> [Optional Params]
    // or
    // Remote /P
    // This will loop continously prompting for different
    // Servers and Pipename


    if ((argc<2)||((argv[1][0]!='/')&&(argv[1][0]!='-')))
    {

        DisplayServerHlp();
        DisplayClientHlp();
        return(1);
    }

    switch(argv[1][1])
    {

    case 'c':
    case 'C':

        //
        // Is Client End of Remote
        //

        if ((argc<4)||((argv[1][0]!='/')&&(argv[1][0]!='-')))
        {

            DisplayServerHlp();
            DisplayClientHlp();
            return(1);
        }

        ServerName=argv[2];
        PipeName=argv[3];

        RunType=REMOTE_CLIENT;
        break;


    case 'p':
    case 'P':

        //
        // Is Client End of Remote
        //

        bPromptForArgs=TRUE;
        RunType=REMOTE_CLIENT;
        break;


    case 's':
    case 'S':
        //
        // Is Server End of Remote
        //
        if ((argc<4)||((argv[1][0]!='/')&&(argv[1][0]!='-')))
        {

            DisplayServerHlp();
            DisplayClientHlp();
            return(1);
        }

        ChildCmd=argv[2];
        PipeName=argv[3];

        RunType=REMOTE_SERVER;
        break;


    default:
        DisplayServerHlp();
        DisplayClientHlp();
        return(1);
    }

    //
    // Save Existing Values
    //

    //
    //Colors /f   <ForeGround> /b <BackGround>
    //

    wAttrib=csbiOriginal.wAttributes;

    //
    //Title  /T Title
    //

    GetConsoleTitle(orgTitle,sizeof(orgTitle));

    if (RunType==REMOTE_SERVER)
    {
    	//
    	// Base Name of Executable
    	// For setting the title
    	//

        char *tcmd=ChildCmd;

        while ((*tcmd!=' ')    &&(*tcmd!=0))   tcmd++;
        while ((tcmd!=ChildCmd)&&(*tcmd!='\\'))tcmd--;

		  sprintf(sTitle,"%-8.8s [Remote /C %s %s]",tcmd,HostName,PipeName);
    }

    //
    //Process Common (Optional) Parameters
    //

    for (i=4;i<argc;i++)
    {

        if ((argv[i][0]!='/')&&(argv[i][0]!='-'))
        {
            WRITEF((VBuff,"Invalid parameter %s:Ignoring\n",argv[i]));
            continue;
        }

        switch(argv[i][1])
        {
        case 'l':    // Only Valid for client End
        case 'L':    // Max Number of Lines to recieve from Server
            i++;
            if (i>=argc)
            {
                WRITEF((VBuff,"Incomplete Param %s..Ignoring\n",argv[i-1]));
                break;
            }
            LinesToSend=(DWORD)atoi(argv[i])+1;
            break;

        case 't':    // Title to be set instead of the default
        case 'T':
            i++;
            if (i>=argc)
            {
                WRITEF((VBuff,"Incomplete Param %s..Ignoring\n",argv[i-1]));
                break;
            }
            sprintf(sTitle,"%s",argv[i]);
            break;

        case 'b':    // Background color
        case 'B':
            i++;
            if (i>=argc)
            {
                WRITEF((VBuff,"Incomplete Param %s..Ignoring\n",argv[i-1]));
                break;
            }
            {
                WORD col=GetColorNum(argv[i]);
                if (col!=0xffff)
                {
                    bSetAttrib=TRUE;
                    wAttrib=col<<4|(wAttrib&0x000f);
                }
                break;
            }

        case 'f':    // Foreground color
        case 'F':
            i++;
            if (i>=argc)
            {
                WRITEF((VBuff,"Incomplete Param %s..Ignoring\n",argv[i-1]));
                break;
            }
            {
                WORD col=GetColorNum(argv[i]);
                if (col!=0xffff)
                {
                    bSetAttrib=TRUE;
                    wAttrib=col|(wAttrib&0x00f0);
                }
                break;
            }

        case 'u':
        case 'U':
            i++;
            if (i>=argc)
            {
                WRITEF((VBuff,"Incomplete Param %s..Aborting\n",argv[i-1]));
                return 1;
            }
            UserName=argv[i];
            break;


        case 'q':
        case 'Q':
            IsAdvertise=FALSE;
            ClientToServerFlag|=0x80000000;
            break;

        default:
            WRITEF((VBuff,"Unknown Parameter=%s %s\n",argv[i-1],argv[i]));
            break;

        }

    }

    //
    //Now Set various Parameters
    //

    //
    //Colors
    //

    SetColor(wAttrib);

    if (RunType==REMOTE_CLIENT)
    {
        BOOL done=FALSE;

        //
        // Set Client end defaults and start client
        //



        while(!done)
        {
            if (!bPromptForArgs ||
                GetNextConnectInfo(&ServerName,&PipeName)
               )
            {
                sprintf(sTitle,"Remote /C %s %s",ServerName,PipeName);
                SetConsoleTitle(sTitle);

                //
                // Start Client (Client.C)
                //
                Client(ServerName,PipeName);
            }
            done=!bPromptForArgs;
        }
    }

    if (RunType==REMOTE_SERVER)
    {
		SetConsoleTitle(sTitle);

        //
        // Start Server (Server.C)
        //
        Server(ChildCmd,PipeName);
    }

    //
    //Reset Colors
    //
    SetColor(csbiOriginal.wAttributes);
    SetConsoleTitle(orgTitle);

    ExitProcess(0);
}
/*************************************************************/
VOID
ErrorExit(
    char* str
    )
{
    WRITEF((VBuff,"Error-%d:%s\n",GetLastError(),str));
    ExitProcess(1);
}
/*************************************************************/
DWORD
ReadFixBytes(
    HANDLE hRead,
    char*  Buffer,
    DWORD  ToRead,
    DWORD  TimeOut   //ignore for timebeing
    )
{
    DWORD xyzBytesRead=0;
    DWORD xyzBytesToRead=ToRead;
    char* xyzbuff=Buffer;

    while(xyzBytesToRead!=0)
    {
        if (!ReadFile(hRead,xyzbuff,xyzBytesToRead,&xyzBytesRead,NULL))
        {
            return(xyzBytesToRead);
        }

        xyzBytesToRead-=xyzBytesRead;
        xyzbuff+=xyzBytesRead;
    }
    return(0);

}
/*************************************************************/
VOID
DisplayClientHlp()
{
    WRITEF((VBuff,"\n   To Start the CLIENT end of REMOTE\n"));
    WRITEF((VBuff,"   ---------------------------------\n"));
    WRITEF((VBuff,"   Syntax : REMOTE /C <ComputerName> <Unique Id> [/L] [/F] [/B]\n"));
    WRITEF((VBuff,"\n"));

    WRITEF((VBuff,"   <ComputerName> The name of computer on which server-end of remote\n"));
    WRITEF((VBuff,"                  was run on.\n"));

    WRITEF((VBuff,"   <Unique Id>    The Unique Id of the remote server running on \n"));
    WRITEF((VBuff,"                  the computer <ComputerName>.\n"));

    WRITEF((VBuff,"   [/L]           Number of Lines to Get.\n"));
    WRITEF((VBuff,"   [/F]           Foreground color eg blue, red, etc.\n"));
    WRITEF((VBuff,"   [/B]           Background color eg blue, lwhite,etc.\n"));
    WRITEF((VBuff,"\n"));
    WRITEF((VBuff,"   Example: REMOTE /C rajnathX86 imbroglio\n"));
    WRITEF((VBuff,"\n"));
    WRITEF((VBuff,"   This would connect to a server session on \\\\rajnathX86\n"));
    WRITEF((VBuff,"   with id \"imbroglio\" if there was a\n"));
    WRITEF((VBuff,"            REMOTE /S <\"Cmd\"> imbroglio\n"));
    WRITEF((VBuff,"   started on the machine \\\\rajnathX86.\n\n"));
    WRITEF((VBuff,"   To Exit: %cQ (Leaves the Remote Server Running)\n",COMMANDCHAR));
    WRITEF((VBuff,"\n"));
}
/*************************************************************/

VOID
DisplayServerHlp()
{
    WRITEF((VBuff,"\n   To Start the SERVER end of REMOTE\n"));
    WRITEF((VBuff,"   ---------------------------------\n"));
    WRITEF((VBuff,"   Syntax : REMOTE /S <\"Cmd\"> <Unique Id> [/U [domainname\\]username] [/F] [/B]\n"));
    WRITEF((VBuff,"\n"));

    WRITEF((VBuff,"   <\"Cmd\">        A Text-Mode program that you want to control\n"));
    WRITEF((VBuff,"                  from another computer.\n"));

    WRITEF((VBuff,"   <Unique Id>    The Id that will uniquely identify this session\n"));
    WRITEF((VBuff,"                  of remote server from other possible sessions\n"));
    WRITEF((VBuff,"                  on this computer.\n"));

    WRITEF((VBuff,"   [/U]           User or Group  who can connect to this session.\n"));
    WRITEF((VBuff,"                  If this is not specified then anyone can connect.\n"));
    WRITEF((VBuff,"   [/F]           Foreground color eg blue, red, etc.\n"));
    WRITEF((VBuff,"   [/B]           Background color eg blue, lwhite, etc.\n"));

    WRITEF((VBuff,"\n"));
    WRITEF((VBuff,"   Examples: REMOTE /S \"cmd\" emerald /U administrators\n"));
    WRITEF((VBuff,"             REMOTE /S \"i386kd -v\" imbroglio\n"));
    WRITEF((VBuff,"\n"));
    WRITEF((VBuff,"   If you have started the server-end of remote on computer \\\\%s\n",HostName));
    WRITEF((VBuff,"   and you want to interact with \"i386kd -v\" from another computer,\n"));
    WRITEF((VBuff,"   start the client-end of remote on the other computer by typing:\n"));
    WRITEF((VBuff,"            REMOTE /C %s imbroglio\n",HostName));
    WRITEF((VBuff,"   To connect to the session \"Cmd\", start:\n"));
    WRITEF((VBuff,"            REMOTE /C %s emerald\n",HostName));
    WRITEF((VBuff,"   Only people with admin access on \\\\%s can connect to this session.\n",HostName));

    WRITEF((VBuff,"\n\n   To Exit: %cK \n",COMMANDCHAR));
    WRITEF((VBuff,"\n"));

}

WORD
GetColorNum(
    char *color
    )
{
    int i;

    strlwr(color);
    for (i=0;i<16;i++)
    {
        if (strcmp(ColorList[i],color)==0)
        {
            return(i);
        }
    }
    return ((WORD)atoi(color));
}

VOID
SetColor(
    WORD attr
    )
{
	COORD  origin={0,0};
    DWORD  dwrite;
    FillConsoleOutputAttribute
    (
    	MyOutHandle,attr,csbiOriginal.dwSize.
    	X*csbiOriginal.dwSize.Y,origin,&dwrite
    );
    SetConsoleTextAttribute(MyOutHandle,attr);
}

BOOL
GetNextConnectInfo(
    char** SrvName,
    char** PipeName
    )
{
    static char szServerName[32];
    static char szPipeName[32];

    try
    {
        ZeroMemory(szServerName,32);
        ZeroMemory(szPipeName,32);
        SetConsoleTitle("Remote - Prompting for next Connection");
        WRITEF((VBuff,"Debugger machine (server): "));
        fflush(stdout);

        if (((*SrvName=gets(szServerName))==NULL)||
             (strlen(szServerName)==0))
        {
            return(FALSE);
        }

        if (szServerName[0] == COMMANDCHAR &&
            (szServerName[1] == 'q' || szServerName[1] == 'Q')
           )
        {
            return(FALSE);
        }

        WRITEF((VBuff,"Debuggee machine : "));
        fflush(stdout);

        if ((*PipeName=gets(szPipeName))==NULL)
        {
            return(FALSE);
        }
        if (szPipeName[0] == COMMANDCHAR &&
            (szPipeName[1] == 'q' || szPipeName[1] == 'Q')
           )
        {
            return(FALSE);
        }
        WRITEF((VBuff,"\n\n"));
    }

    except(EXCEPTION_EXECUTE_HANDLER)
    {
        return(FALSE);  // Ignore exceptions
    }
    return(TRUE);
}


/*************************************************************/
VOID
Errormsg(
    char* str
    )
{
    WRITEF((VBuff,"Error (%d) - %s\n",GetLastError(),str));
}

/*************************************************************/
