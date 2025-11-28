// htream.h - HP LaserJet output stream header
#include <fstream.h> // Accesses 'filebuf' class
#include <string.h>
#include <stdio.h> // for sprintf

class hstreambuf : public filebuf
{
public:
    hstreambuf( int filed );
    virtual int sync();
    virtual int overflow( int ch );
    ~hstreambuf();
private:
    int column, line, page;
    char* buffer;
    void convert( long cnt );
    void newline( char*& pd, int& jj );
    void heading( char*& pd, int& jj );
    void pstring( char* ph, char*& pd, int& jj );
};
ostream& und( ostream& os );
ostream& reg( ostream& os );
