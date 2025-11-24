// exios111.cpp
// Binary example 1
#include <fstream.h>
#include <fcntl.h>
#include <io.h>
int iarray[2] = { 99, 10 };
void main()
{
    ofstream ofs( "test.dat", ios::binary );
    ofs.write( (char*)iarray, 4 ); // Exactly 4 bytes written
}
