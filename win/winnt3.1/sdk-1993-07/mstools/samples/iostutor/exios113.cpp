// exios113.cpp
// Binary example 3
#include <fstream.h>
#include <fcntl.h>
#include <io.h>
int iarray[2] = { 99, 10 };
void main()
{
    filedesc fd = _open( "test.dat", _O_BINARY | _O_CREAT | _O_WRONLY 
);
    ofstream ofs( fd );
    ofs.write( (char*)iarray, 4 ); // Exactly 4 bytes written
}
