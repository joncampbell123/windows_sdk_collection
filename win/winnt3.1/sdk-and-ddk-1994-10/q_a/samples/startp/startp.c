
/******************************************************************************\
*       This is a part of the Microsoft Source Code Samples. 
*       Copyright (C) 1993 Microsoft Corporation.
*       All rights reserved. 
*       This source code is only intended as a supplement to 
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the 
*       Microsoft samples programs.
\******************************************************************************/

/********************************************************************
* This program demonstrates various CreateProcess parameters,       *
* including starting processes in a given priority class. This is   *
* very similar to the "start" command but with added functionality. *
*                                                                   *
* This program demonstrates the use of the following Win32 APIs:    *
*   CreateProcess, TerminateProcess.                                *
*                                                                   *
* This program also uses the following Win32 APIs:                  *
*   WaitForSingleObject GetLastError.                               *
*                                                                   *
* Execution instructions:                                           *
*   see the help() function or run the program without any          *
*   parameters to see detailed execution info.                      *
*                                                                   *
* Possible enhancements:                                            *
*   Handle cmd.exe internal commands                                *
********************************************************************/

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Standard error macro for reporting API errors */
#define PERR(bSuccess, api) {if (!(bSuccess)) printf("%s: Error %d from %s \
    on line %d\n", __FILE__, GetLastError(), api, __LINE__);}

void help()
{
  puts("Starts a specified program, batch, or command file.");
  puts("STARTP [/Ttitle] [/Dpath] [/l] [/h] [/r] [/min] [/max] [/w]");
  puts("       [/c] [/b] [program] [parameters]");
  puts("\n    title       Title to display in window title bar. Quote the");
  puts("                entire paramter to include spaces in the title,");
  puts("                i.e. startp \"/Ttest job\"");
  puts("    path        Starting directory");
  puts("    l           Set default to low priority");
  puts("    h           Set default to high priority");
  puts("    r           Set default to realtime priority");
  puts("    min         Start window minimized");
  puts("    max         Start window maximized");
  puts("    w           Wait for started process to end before returning");
  puts("                control to the command processor. This option starts");
  puts("                the process synchronously");
  puts("    c           Use current console instead of creating a new console");
  puts("    b           Start detached with no console at all");
  puts("    program     A batch file or program to run as either a GUI");
  puts("                application or a console application");
  puts("    parameters  These are the parameters passed to the program");
  puts("\nNote that the priority parameters may have no effect if the program");
  puts("changes its own priority.");
  exit(1);
}

int main(int argc, char *argv[])
{
  char szArgs[512], *p;  /* new process arguments buffer & temp pointer */
  char szPgmName[MAX_PATH];  /* name of the program */
  BOOL fSuccess;  /* API return code */
  STARTUPINFO si;  /* used for CreateProcess */
  PROCESS_INFORMATION pi;  /* used for CreateProcess */
  LPSTR lpszCurDir = NULL;  /* current directory for new process */
  BOOL bMoreParams;  /* flag end of new process parameter processing */
  BOOL bWait = FALSE;  /* wait/no wait for new process to end */
  DWORD dwResult;  /* API return code */
  DWORD dwCreate = CREATE_NEW_CONSOLE;  /* new process creation flags */
  BOOL bExtension;  /* input filename have an explicit extension? */
  int i;
  char *aExt[] = { ".exe", ".com", ".bat", ".cmd" };

  /* Check to make sure we are running on Windows NT */
  if( GetVersion() & 0x80000000 )
    {
    MessageBox(NULL, "Sorry, this application requires Windows NT.\n"
        "This application will now terminate.",
        "Error: Windows NT Required to Run",  MB_OK );
    return(1);
    }
  /* process all command-line parameters */
  if (argc < 2)
    help();
  memset(&si, 0, sizeof(STARTUPINFO));
  bMoreParams = TRUE;
  while(bMoreParams)
    {
    argv++;  /* point to the first parameter */
    if (!*argv) /* if *argv is NULL we're missing the program name! */
      {
      puts("Error: missing program name");
      exit(1);
      }
    if ((*argv)[0] == '/' || (*argv)[0] == '-')
      {
      strlwr(*argv);
      switch ((*argv)[1])  /* letter after the '/' or '-' */
        {
        case 't':
          si.lpTitle = &(*argv)[2];  /*  /tTitle */
          break;
        case 'd':
          lpszCurDir = &(*argv)[2];  /*  /dPath */
          break;
        case 'h':
          dwCreate |= HIGH_PRIORITY_CLASS;  /*  /h */
          break;
        case 'l':
          dwCreate |= IDLE_PRIORITY_CLASS;  /*  /l */
          break;
        case 'r':
          dwCreate |= REALTIME_PRIORITY_CLASS;  /*  /r */
          break;
        case 'm':
          switch ((*argv)[2])
            {
            case 'i':
              si.wShowWindow = SW_MINIMIZE;
              si.dwFlags |= STARTF_USESHOWWINDOW;
              break;
            case 'a':
              si.wShowWindow = SW_SHOWMAXIMIZED;
              si.dwFlags |= STARTF_USESHOWWINDOW;
              break;
            default:
              printf("Error: invalid switch - \"%s\"", *argv);
              help();
            } /* switch */
          break;
        case '?':
          help();  /* help() will terminate app */
        case 'w':
          bWait = TRUE;  /* don't end until new process ends as well */
          break;
        case 'b':
          dwCreate |= DETACHED_PROCESS;  /* start detached */
          /* detached implies no CREATE_NEW_CONSOLE so fall through */
        case 'c':
          dwCreate &= ~CREATE_NEW_CONSOLE;  /* turn off this bit */
          break;
        default:
          printf("Error: invalid switch - \"%s\"", *argv);
          help();
        } /* switch */
      } /* if */
    else  /* not a '-' or '/', must be the program name */
      bMoreParams = FALSE;
    } /* while */
  strcpy(szPgmName, *argv++);  /* copy program name as first param */
  bExtension = (BOOL) strchr(szPgmName, '.');  /* has an extension? */
  if (!bExtension)
    strcat(szPgmName, aExt[0]);  /* first extension to try */
  memset(szArgs, 0, sizeof(szArgs));
  strcpy(szArgs, szPgmName);  /* first arg: program name */
  p = strchr(szArgs, 0);  /* point past program name */
  while (*argv)  /* copy remaining arguments to szArgs separated by spaces */
    {
    strcat(p, " ");
    strcat(p, *argv++);
    }
  si.cb = sizeof(STARTUPINFO);
  i = 1;
  do
    {
    fSuccess = CreateProcess(NULL,  /* image file name */
        szArgs,  /* command line (including program name) */
        NULL,  /* security for process */
        NULL,  /* security for main thread */
        FALSE,  /* new process inherits handles? */
        dwCreate,  /* creation flags */
        NULL,  /* environment */
        lpszCurDir,  /* new current directory */
        &si,  /* STARTUPINFO structure */
        &pi);  /* PROCESSINFORMATION structure */
    if (!fSuccess)
      switch (GetLastError())  /* process common errors */
        {
        case ERROR_FILE_NOT_FOUND:
          if (bExtension || i > 3)
            {
            puts("Error: program or batch file not found");
            exit(1);
            }
          else
            strcpy(strchr(szArgs, '.'), aExt[i++]);
          break;
        case ERROR_DIRECTORY:
          puts("Error: bad starting directory");
          exit(1);
        case ERROR_PATH_NOT_FOUND:
          puts("Error: bad path");
          exit(1);
        default:
          PERR(0, "CreateProcess");
          exit(1);
        }  /* switch */
    } while (!fSuccess);
  /* close pipe handle - child's instance will be closed when terminates */
  CloseHandle(pi.hThread);
  if (bWait)  /* wait /Wtime wait flag specified? */
    {
    dwResult = WaitForSingleObject(pi.hProcess,  /* object to wait for */
        (DWORD) -1);  /* timeout time */
    PERR(dwResult != -1, "WaitForSingleObject");
    }
  CloseHandle(pi.hProcess);  /* close process handle or it won't die */
  return(0);
}
