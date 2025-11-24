// exios202.cpp
// A custom manipulator with a char* parameter
#include <iostream.h>

#include <iomanip.h>
#include <string.h>

typedef char* charp;
IOMANIPdeclare( charp );

class money {
private:
    long value;
    static char *szCurrentPic;
public:
    money( long val ) { value = val; }
    friend ostream& operator << ( ostream& os, money m ) {
        // A more complete function would merge the picture
        // with the value rather than simply appending it
        os << m.value << '[' << money::szCurrentPic << ']';
        return os;
    }
    friend ostream& setpic( ostream& os, char* szPic ) {
        money::szCurrentPic = new char[strlen( szPic ) + 1];
        strcpy( money::szCurrentPic, szPic );
        return os;
    }
};
char *money::szCurrentPic;  // Static pointer to picture

OMANIP(charp) setpic(charp c)
{
    return OMANIP(charp) (setpic, c);
}

void main()
{
    money amt = 35235.22;
    cout << setiosflags( ios::fixed );
    cout << setpic( "###,###,###.##" ) << "amount = " << amt << endl;
}
