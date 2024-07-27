extern far pascal Lib1Proc( int );

far pascal Lib2Proc( n )
int n;
{
    if (n)
        Lib1Proc( --n );
}
