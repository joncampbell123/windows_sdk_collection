// exios115.cpp
#include <iostream.h>
int n[5], i;
void main()
{
    cout << "Enter 5 values, separated by spaces" << endl;
    for( i = 0; i < 5; i++ ) {
        cin >> n[i];
        if( cin.eof() || cin.bad() ) break; // Tests for end-of-file
                                            // or unrecoverable error
        if( cin.fail() ) { // Tests for format conversion error
            cin.clear(); // Clear stream's fail bit
            n[i] = 0;    // and continue processing
        }
    }
    for( i = 0; i < 5; i++ ) { // Prints the values just read
        cout << n[i];
    }
}
