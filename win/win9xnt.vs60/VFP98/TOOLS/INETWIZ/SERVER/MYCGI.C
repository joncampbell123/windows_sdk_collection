#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winbase.h>
#include <time.h>
#include <direct.h>
#include <sys/timeb.h>

#define TEMP_PREFIX_LENGTH 8		// Length of temporary file prefix excluding null terminator
#define DATA_BUFFER_LENGTH 1000		// Length of data buffer.

#define MAX_FILE_ATTEMPTS		10  // Number of times to attempt creating temp file.
#define ACK_RETRY_MILLISECONDS 250  // Number of milliseconds for each retry attempt to read ACK file.
#define ACK_TIMEOUT_SECONDS     60  // Number of seconds until attempts to read ACK file time out.

#define SERVER_INACCESSIBLE_TITLE_LOC	"Server Inaccessible"
#define SERVER_INACCESSIBLE_TEXT_LOC	"Communication with the server is not possible at this time."
#define SERVER_TIMEOUT_TITLE_LOC	"Server Timeout" 
#define SERVER_TIMEOUT_TEXT_LOC		"The server timed out. It is probably too busy to fulfill your request."
#define SERVER_NODATA_TITLE_LOC	"Server Error"
#define SERVER_NODATA_TEXT_LOC	"A server error occurred.  No return data was sent."

void errorform( char *errorstring, char *errortitle );

void main( int argc, char *argv[] ) 
{

	char tmpPrefix[TEMP_PREFIX_LENGTH+1];
	char tmpFolder[] = "\\temp";
	char tmpName[_MAX_PATH];

	char *t1		 = NULL;
	char *tempbuffer = NULL;

	char datFileName[_MAX_PATH];
	char ackFileName[_MAX_PATH];
	char atnFileName[_MAX_PATH];

	FILE *datFile    = NULL;
	FILE *atnFile    = NULL;
	FILE *tempFile   = NULL;
	struct _timeb timebuffer;

	
	char buffer[DATA_BUFFER_LENGTH];
	int cont_len, bytesread, fileattempts;
	time_t begintout, chktout;
	char **envptr;
	fpos_t pos;

	// Turn stdin buffering off
	setvbuf( stdin, NULL, _IONBF, 0 );

	// Create server.app compatible temporary folder.
	// Note:  Server.app is looking for files in <current drive>:\temp,
	// not in the actual system temporary folder.
	_mkdir( tmpFolder );

	// Seed random number generator and zero fileattempts counter.
	_ftime( &timebuffer );
	srand( ((timebuffer.time & 0xFF)<<10) | timebuffer.millitm );
	fileattempts = 0;

	for (;;) 
	{

		fileattempts++;

		// If we've tried MAX_FILE_ATTEMPTS times, give error message.
		if ( fileattempts > MAX_FILE_ATTEMPTS ) 
		{
			errorform( SERVER_INACCESSIBLE_TEXT_LOC, SERVER_INACCESSIBLE_TITLE_LOC );
			goto CleanupAndExit;
		}

		// Create random file name. Format is "VF" plus unique 4 digit hex number
		// representing the processid, plus 2 digit random number (in case multiple
		// machines are running the CGI scripts).
		sprintf( tmpPrefix, "VF%04X%02X", (DWORD) GetCurrentProcessId(), rand() & 0xFF);

		// Build temporary file name.
		sprintf( tmpName, "%s\\%s", tmpFolder, tmpPrefix );

		// Create the names for the three files.
		sprintf( datFileName, "%s.dat", tmpName );
		sprintf( ackFileName, "%s.ack", tmpName );
		sprintf( atnFileName, "%s.atn", tmpName );

		// Create data file.
		datFile = fopen( datFileName, "a" );

		// Make sure data file didn't already exist.
		if ( NULL != datFile ) 
		{
			// Seek to the end of the file.
			fseek( datFile, 0, SEEK_END );
			if( 0 == fgetpos( datFile, &pos ) ) 
			{
				// We know fgetpos was successful, now check position.
				// If pos is zero, we know we have a new file and can break out of loop.
				if ( 0 == pos ) break;
			}
			fclose( datFile );
		}
		
	}

	// We could check REQUEST_METHOD here, but what would be the point?
	// If CONTENT_LENGTH is NULL, we won't read the stdin, otherwise we will.
	// This is entirely for POST method requests.
	t1 = getenv( "CONTENT_LENGTH" );
	if ( NULL != t1 ) 
	{
		cont_len = atoi( t1 );

		tempbuffer = (char *) malloc( sizeof(char) * cont_len );
		if ( NULL == tempbuffer )
		{
			goto CleanupAndExit;
		}

		bytesread = fread( tempbuffer, sizeof(char), cont_len, stdin );
		if ( 0 != bytesread )
			fwrite( tempbuffer, sizeof(char), bytesread, datFile );

		free( tempbuffer );
	}

	// This part reads all of the environment variables and sends
	// them along in the DAT file as well. All ampersand delimited.
	envptr = environ;
	while ( NULL != *envptr )
	{
		fputs( "&", datFile );
		fputs( *envptr, datFile );
		envptr++;
	}
	fclose( datFile );

	// Create ATN File
	atnFile = fopen( atnFileName,"w" );
	if ( NULL == atnFile ) 
	{
		errorform( SERVER_INACCESSIBLE_TEXT_LOC , SERVER_INACCESSIBLE_TITLE_LOC );
		goto CleanupAndExit;
	}

	fclose( atnFile );

	_flushall();

	// Wait for Server to ACK
	time( &begintout );
	do
	{
		tempFile = fopen( ackFileName, "r" );
		if ( NULL != tempFile ) break;	// Break before sleep if successful.
		time( &chktout );
		Sleep( ACK_RETRY_MILLISECONDS );
	//if _access() can't find the file, then the query is currently being processed.  
	//Keep looping for another ACK_TIMEOUT_SECONDS.
	} while ( ( NULL == tempFile ) && (( difftime( chktout, begintout ) < ACK_TIMEOUT_SECONDS ) 
		|| ((_access(atnFileName,0) == -1) && ( difftime( chktout, begintout ) < (2*ACK_TIMEOUT_SECONDS) ))));

	if ( NULL != tempFile )
	{
		fclose( tempFile );
	}
	else
	{
		errorform( SERVER_TIMEOUT_TEXT_LOC , SERVER_TIMEOUT_TITLE_LOC);
		goto CleanupAndExit;
	}

	// Send the Data file to stdout.
	datFile = fopen( datFileName, "r" );
	if ( NULL == datFile ) 
	{
		errorform( SERVER_NODATA_TEXT_LOC , SERVER_NODATA_TITLE_LOC);
		goto CleanupAndExit;
	}
	else 
	{
		// Return results to web server.
		while ( !feof( datFile ) )
		{
			if ( NULL != fgets( buffer, DATA_BUFFER_LENGTH, datFile ) )
				printf( "%s", buffer );
		} 
		fclose( datFile );
	}

CleanupAndExit:

	// Close all file handles.
	_fcloseall();

	// Remove temporary files.
	remove( ackFileName );
	remove( datFileName );
	remove( atnFileName );

	return;

}

void errorform( char *errorstring, char *errortitle )
{
	printf( "Content-Type: text/html\n\n" );
	printf( "<head><title>%s</title></head>\n", errortitle );
	printf( "<body>\n<h1>%s</h1>\n", errortitle );
	printf( "%s<hr>\n</body>", errorstring );
}
