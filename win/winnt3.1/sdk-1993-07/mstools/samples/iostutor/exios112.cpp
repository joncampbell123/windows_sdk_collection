// exios112.cpp
// Binary example 2
#include <fstream.h>
int iarray[2] = { 99, 10 };
void main()
{
    ofstream ofs ( "test.dat" );
    ofs.setmode( filebuf::binary );
    ofs.write( (char*)iarray, 4 ); 
// Exactly 4 bytes written
}
