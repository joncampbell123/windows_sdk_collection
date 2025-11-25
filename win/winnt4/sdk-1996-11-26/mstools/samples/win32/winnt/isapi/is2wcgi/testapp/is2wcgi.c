// is2wcgi.c - a quick test app
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

LPSTR glpProfile;

void GetCgiProfileKey (LPSTR lpszSection, LPSTR lpszKey, LPSTR lpszBuf, DWORD dwSize);
void WriteString (HANDLE hFile, LPSTR lpsz);
void HtmlWriteText (HANDLE hFile, LPSTR lpsz);

int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
    {
    char szOutputFileName[MAX_PATH];
    HANDLE hOutput;
    HANDLE hProfile;
    BYTE byBuf[1025];
    DWORD dwLen;
    int i;
    BOOL bQuote = FALSE;

    // Profile name is passed on command line
    glpProfile = lpCmdLine;
    for (i = 0 ; glpProfile[i] && (glpProfile[i] != ' ' || bQuote) ; i++)
        {
        if (glpProfile[i] == '\"')
            bQuote = !bQuote;
        }
    glpProfile[i] = 0;

    // Get output file name
    GetCgiProfileKey ("System", "Output File", szOutputFileName, MAX_PATH);

    hOutput = CreateFile (szOutputFileName,
                          GENERIC_WRITE,
                          0,
                          NULL,
                          CREATE_ALWAYS,
                          FILE_ATTRIBUTE_NORMAL,
                          NULL);

    if (hOutput == INVALID_HANDLE_VALUE)
        {
        MessageBox (NULL, "IS2WCGI Test App - Invalid output file", NULL, MB_OK);
        return (0);
        }

    WriteString (hOutput, "Content Type: text/html\r\n\r\n");
    WriteString (hOutput, "<HTML><HEAD>\r\n");
    WriteString (hOutput, "<TITLE>Output of IS2WCGI.EXE</TITLE>\r\n");
    WriteString (hOutput, "</HEAD><BODY>\r\n");
    WriteString (hOutput, "<H1>Profile Sent to Windows CGI App</H1>\r\n");
    WriteString (hOutput, "<PRE>\r\n");

    hProfile = CreateFile (glpProfile,
                          GENERIC_READ,
                          0,
                          NULL,
                          OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL,
                          NULL);

    if (hProfile == INVALID_HANDLE_VALUE)
        HtmlWriteText (hOutput, "Unable to open profile\r\n");
    else
        {
        do  {
            if (!ReadFile (hProfile, byBuf, 1024, &dwLen, NULL))
                {
                HtmlWriteText (hOutput, "Error reading from profile\r\n");
                break ;
                }

            byBuf[dwLen] = 0;
            if (dwLen)
                HtmlWriteText (hOutput, byBuf);
            } while (dwLen == 1024);
        }


    WriteString (hOutput, "</PRE></BODY></HTML>\r\n");
    CloseHandle (hOutput);
    CloseHandle (hProfile);

    return (0);
    }

void GetCgiProfileKey (LPSTR lpszSection, LPSTR lpszKey, LPSTR lpszBuf, DWORD dwSize)
    {
    GetPrivateProfileString (lpszSection, lpszKey, "", lpszBuf, dwSize, glpProfile);
    }

void WriteString (HANDLE hFile, LPSTR lpsz)
    {
    DWORD dwRead;
    if (!WriteFile (hFile ,lpsz, lstrlen (lpsz), &dwRead, NULL))
        MessageBox (NULL, "IS2WCGI Test App - Cannot write to output file", NULL, MB_OK);
    }

void HtmlWriteText (HANDLE hFile, LPSTR lpsz)
    {
    char szBuf[2048];
    int nLen;
    int i;

    //
    // Build up enough data to make call to WriteString
    // worthwhile; convert special chars too.
    //
    nLen = 0;
    szBuf[0] = 0;
    for (i = 0 ; lpsz[i] ; i++)
        {
        if (lpsz[i] == '<')
            lstrcpy (&szBuf[nLen], "&lt;");
        else if (lpsz[i] == '>')
            lstrcpy (&szBuf[nLen], "&gt;");
        else if (lpsz[i] == '&')
            lstrcpy (&szBuf[nLen], "&amp;");
        else if (lpsz[i] == '\"')
            lstrcpy (&szBuf[nLen], "&quot;");
        else
            {
            szBuf[nLen] = lpsz[i];
            szBuf[nLen + 1] = 0;
            }

        nLen += lstrlen (&szBuf[nLen]);

        if (nLen >= 1024)
            {
            WriteString (hFile, szBuf);
            nLen = 0;
            }
        }

    if (nLen)
        WriteString (hFile, szBuf);
    }
