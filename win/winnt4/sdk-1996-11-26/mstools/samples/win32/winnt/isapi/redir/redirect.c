// redir.dll
//this isapi app is used to wisk the client away to a new
//location and log the transaction.  redir.dll can be used
//in one of two ways
//
//1.    <A HREF="redir.dll?target=http://www.volcanocoffee.com/">
//      <IMG SRC="Volcano.gif"></A>
//
//
//2. <FORM ACTION="redir.dll">
//    <SELECT NAME="Target">  <!-- Name must be target -->
//      <OPTION VALUE="http://www.volcanocoffee.com/">Volcano Coffee
//      <OPTION VALUE="/some/local/path.htm">Local Path
//   </SELECT>
//   <INPUT TYPE="SUBMIT" VALUE="Go!">
//  </FORM>
//
//local paths must being with '/'.  all other paths generate
//an http 302 event.  redir.dll logs both the referring page
//and the target page.  this allows the server admin to parse
//the log file and give 'credit' to any pages containing 
//advertisements.

#include <windows.h>
#include <stdio.h>
#include <httpext.h>

//Function Prototypes
INT GetValueFromHex (CHAR);
DWORD Redirect(CHAR *, EXTENSION_CONTROL_BLOCK *);


BOOL WINAPI GetExtensionVersion (HSE_VERSION_INFO  *version)
{
    version->dwExtensionVersion = MAKELONG( HSE_VERSION_MINOR,
                                            HSE_VERSION_MAJOR );
    strcpy( version->lpszExtensionDesc,
            "Redirect to another page" );

    return TRUE;
}

//
//
//

DWORD WINAPI HttpExtensionProc (LPEXTENSION_CONTROL_BLOCK lpEcb)
{
      DWORD     dwRetCode;              //holds this request's status
      CHAR      szBuff[2048];           //holds either the target var
                                        //or the returning HTML data.
      BOOL      done = FALSE;           //flag used in query string parsing
      INT       i, j;                   //counters used in query string parsing
      DWORD     dwLen;                  //used to hold length for server calls
      CHAR      szReferer[MAX_PATH];    //used to hold the HTTP_REFERER variable
      CHAR      szLogLine[MAX_PATH];    //used to hold the data we write to the log file
  
    //check request method, as we only support the GET method
    if (stricmp (lpEcb->lpszMethod, "GET") == 0)
    {    
    	//look for the target variable in the query string
        if (strnicmp (lpEcb->lpszQueryString, "target=", 7) == 0)
    	{
    		i = 7;
    		j = 0;

    		//parse query string to extract the value of the target
            //variable.  this loop expands the %XX escapes via the
            //GetValueFromHex function.  we parse the query string
            //until we see a '&' char, indicating the end of the
            //target variable, or until we see the end of the query
            //string.
            while ((done == FALSE) && (lpEcb->lpszQueryString[i] != '\0'))
    		{
    			if (lpEcb->lpszQueryString[i] == '%')
    			{
    				szBuff[j++] = (GetValueFromHex (lpEcb->lpszQueryString[i+1]) * 16) +
    												GetValueFromHex (lpEcb->lpszQueryString[i+2]);
    				//skip past %XX escape
                    i += 3;
    			}
    			else if (lpEcb->lpszQueryString[i] == '&')
    			{
    				done = TRUE;
    			}
    			else
    			{
    				szBuff[j++] = lpEcb->lpszQueryString[i++];
    			}
    		}

    		szBuff[j] = '\0';
		
            //get the HTTP_REFERER from the server.  if the
            //client did not set the HTTP_REFERER variable,
            //then set it to '-'.
            dwLen = MAX_PATH;
            if (!lpEcb->GetServerVariable(  lpEcb->ConnID, 
                                            "HTTP_REFERER", 
                                            szReferer, 
                                            &dwLen))                                          
            {
                strcpy (szReferer, "-");
            }
            
            //build the string that we want to add to the log
            //file.  it has the form " rdrRef=URL rdrTgt=URL"

            strcpy (szLogLine, " rdrRef=");
            strcat (szLogLine, szReferer);
            strcat (szLogLine, " rdrTgt=");
            strcat (szLogLine, szBuff);

            //as an ISAPI Server Extension, we have only HSE_LOG_BUFFER_LEN
            //chars (80 chars as of 7 feb 1996) to play with, so we copy
            //HSE_LOG_BUFFER_LEN chars into the lpEcb->lpszLogData buffer.

            strncpy (lpEcb->lpszLogData, szLogLine, HSE_LOG_BUFFER_LEN -1);
            lpEcb->lpszLogData[HSE_LOG_BUFFER_LEN -1] = '\0';
            
            //call the function that will do the redirection
            dwRetCode = Redirect (szBuff, lpEcb);
    	}
    	else
    	{
    		//we could not find the target variable in the query string
            //so we send the client a nicely formatted error message.
            //we send the header "200 OK" to ensure that the client
            //will display the text we are sending.

            wsprintf (szBuff,   "Content-Type: text/html\r\n"
                                "\r\n"
                                "<HTML>\n<HEAD>\n<TITLE>Unknown error.</TITLE>\n</HEAD>\n"
                                "<BODY>\n<H1>Unknown error.</H1>\n"
                                "Page parameter not found."
                                "</BODY></HTML>");

    		lpEcb->ServerSupportFunction (  lpEcb->ConnID,
                                            HSE_REQ_SEND_RESPONSE_HEADER,
                                            "200 OK",
                                            NULL,
                                            (LPDWORD) szBuff);
            //we set the status to HSE_STATUS_ERROR tell the server
            //that something went wrong.

            dwRetCode = HSE_STATUS_ERROR;
    	}



    }
    else 
    {
    	//request method not GET, so we send back a nicely formatted
        //error message to tell the client what request method
        //we support.
            wsprintf (szBuff,   "Content-Type: text/html\r\n"
                                "\r\n"
                                "<HTML>\n<HEAD>\n<TITLE>Request Method Not Supported</TITLE>\n</HEAD>\n"
                                "<BODY>\n<H1>Request Method Not Supported</H1>\n"
                                "This ISAPI extension only supports the GET method."
                                "</BODY></HTML>");

    		lpEcb->ServerSupportFunction (  lpEcb->ConnID,
                                            HSE_REQ_SEND_RESPONSE_HEADER,
                                            "200 OK",
                                            NULL,
                                            (LPDWORD) szBuff);
            dwRetCode = HSE_STATUS_ERROR;

    }    
    return dwRetCode;
}
   // end HttpExtensionProc


INT GetValueFromHex (CHAR ch)
//
//this function takes a char and returns the integer value
//of the char interpreted as a HEX digit.  this function
//expects the chars '0'-'9', 'a'-'f', or 'A'-'Z'.  if it
//receives any other char, it returns -1.

{
	if ( (ch >= '0') && (ch <= '9') )
		return (ch - '0');
	else if ( (ch >= 'a') && (ch <= 'f') )
		return (ch - 'a' + 10);
	else if ( (ch >= 'A') && (ch <= 'F') )
		return (ch - 'A' + 10);
	
	return (-1);
}

DWORD Redirect(CHAR * szRedir, EXTENSION_CONTROL_BLOCK *pECB) {
    //this function actually does the redirection.  if szRedir begins with a
    //'/' then it is a local URL and we use HSE_REQ_SEND_URL to send the
    //file.  otherwise, we assume that szRedire contains a remote URL and we
    //use HSE_REQ_SEND_URL_REDIRECT_RESP to generate an HTTP 302 redirection
    //message.

    if (szRedir[0] == '/')
    {
        pECB->ServerSupportFunction(    pECB->ConnID,
                                        HSE_REQ_SEND_URL,
                                        szRedir,
                                        (LPDWORD) sizeof(szRedir),
                                        NULL);
    }
    else 
    {
        pECB->ServerSupportFunction(    pECB->ConnID,
                                        HSE_REQ_SEND_URL_REDIRECT_RESP,
                                        szRedir,
                                        (LPDWORD) sizeof(szRedir),
                                        NULL);

    }
    return HSE_STATUS_SUCCESS;
} //end redirect()



