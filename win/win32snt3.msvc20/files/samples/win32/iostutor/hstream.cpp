// hstream.cpp  - HP LaserJet output stream
#include "hstream.h"

#define REG 0x01   // Regular font code
#define UND 0x02   // Underline font code
#define CR 0x0d    // Carriage return character
#define NL 0x0a    // Newline character
#define FF 0x0c    // Formfeed character
#define TAB 0x09   // Tab character

#define LPP 57     // Lines per Page
#define TABW 5     // Tab width

// Prolog defines printer initialization (font, orientation, etc.
char prolog[] =
{ 0x1B, 0x45,                                // Reset printer
  0x1B, 0x28, 0x31, 0x30, 0x55,			// IBM PC char set
  0x1B, 0x26, 0x6C, 0x31, 0x4F,			// Landscape
  0x1B, 0x26, 0x6C, 0x38, 0x44,              // 8 lines-per-inch
  0x1B, 0x26, 0x6B, 0x32, 0x53};	          // Lineprinter font

// Epilog prints the final page and terminates the output
char epilog[] = { 0x0C, 0x1B, 0x45 };   // Formfeed, reset

char uon[] = { 0x1B, 0x26, 0x64, 0x44, 0 }; // Underline on
char uoff[] = { 0x1B, 0x26, 0x64, 0x40, 0 };//  Underline off

hstreambuf::hstreambuf( int filed ) : filebuf( filed )
{
     column = line = page = 0;
     int size = sizeof( prolog );
     setp( prolog, prolog + size );
     pbump( size );   // Puts the prolog in the put area
     filebuf::sync(); // Sends the prolog to the output file
     buffer = new char[1024]; // Allocates destination buffer
}

hstreambuf::~hstreambuf()
{
     sync(); // Makes sure the current buffer is empty
     delete buffer; // Free the memory
     int size = sizeof( epilog );
     setp( epilog, epilog + size );
     pbump( size );   // Puts the epilog in the put area
     filebuf::sync(); // Sends the epilog to the output file
}

int hstreambuf::sync()
{
     long count = out_waiting();
	if ( count ) {
		convert( count );
	}
	return filebuf::sync();
}

int hstreambuf::overflow( int ch )
{
     long count = out_waiting();
	if ( count ) {
		convert( count );
	}
	return filebuf::overflow( ch );
}
//*** The following code is specific to the HP LaserJet printer ***

// Converts a buffer to HP, then writes it
void hstreambuf::convert( long cnt )
{
    char *bufs, *bufd; // Source, destination pointers
    int j = 0;

    bufs = pbase();
    bufd = buffer;
    if( page == 0 ) {
        newline( bufd, j );
    }
    for( int i = 0; i < cnt; i++ ) {
        char c = *( bufs++ );  // Gets character from source buffer
        if( c >= ' ' ) {     // Character is printable
            * ( bufd++ ) = c;
            j++;
            column++;
        }

        else if( c == NL ) { // Moves down one line
            *( bufd++ ) = c;   // Passes character through
            j++;
            line++;
            newline( bufd, j ); // Checks for page break, etc.
        }
        else if( c == FF ) {    // Ejects paper on formfeed
            line = line - line % LPP + LPP;
            newline( bufd, j ); // Checks for page break, etc.
        }
        else if( c == TAB ) {   // Expands tabs
            do {
                *( bufd++ ) = ' ';
                j++;
                column++;
            } while ( column % TABW );
        }
        else if( c == UND ) { // Responds to 'und' manipulator
            pstring( uon, bufd, j );
        }
        else if( c == REG ) { // Responds to 'reg' manipulator
            pstring( uoff, bufd, j );  //
        }
    }
    setp( buffer, buffer + 1024 ); // Sets new put area
    pbump( j ); // Indicates the number of characters in the dest buffer
}

// simple manipulators - apply to all ostream classes
ostream& und( ostream& os ) // Turns on underscore mode
{
    os << (char) UND; return os;
}

ostream& reg( ostream& os ) // Turns off underscore mode
{
    os << (char) REG; return os;
}

void hstreambuf::newline( char*& pd, int& jj )
// Called for each newline character
{
	column = 0;
	if ( ( line % ( LPP*2 ) ) == 0 ) // Even page
		{
		page++;
		pstring( "\033&a+0L", pd, jj );  //  Set left margin to zero
		heading( pd, jj );		/*  print heading  */
		pstring( "\033*p0x77Y", pd, jj ); // Cursor to (0,77) dots  
		}

	if ( ( ( line % LPP ) == 0 ) && ( line % ( LPP*2 ) ) != 0 ) 
// Odd page
		{ //  prepare to move to right column
		page++;
		pstring( "\033*p0x77Y", pd, jj ); // Cursor to (0,77) dots  
		pstring( "\033&a+88L", pd, jj ); // Left margin to 88th column
		}
	}
void hstreambuf::heading( char*& pd, int& jj ) // Prints page heading
{
		char hdg[20];
		int i;

		if( page > 1 ) {
			*( pd++ ) = FF;
			jj++;
		}
		pstring( "\033*p0x0Y", pd, jj ); // Top of page
		pstring( uon, pd, jj ); // Underline on
		sprintf( hdg, "Page %-3d", page );
		pstring( hdg, pd, jj );
		for( i=0; i < 80; i++ ) { // Pads with blanks
			*( pd++ ) = ' ';
			jj++;
		}
		sprintf( hdg, "Page %-3d", page+1 ) ;
		pstring( hdg, pd, jj );
		for( i=0; i < 80; i++ ) { // Pads with blanks
			*( pd++ ) = ' ';
			jj++;
		}
		pstring( uoff, pd, jj ); // Underline off
}
// Outputs a string to the buffer
void hstreambuf::pstring( char* ph, char*& pd, int& jj )
{
    int len = strlen( ph );
    strncpy( pd, ph, len );
    pd += len;
    jj += len;
}
