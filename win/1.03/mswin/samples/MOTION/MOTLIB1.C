extern far pascal Lib2Proc( int );

far pascal Lib1Proc( n )
int n;
{
    if (n)
        Lib2Proc( --n );
}
