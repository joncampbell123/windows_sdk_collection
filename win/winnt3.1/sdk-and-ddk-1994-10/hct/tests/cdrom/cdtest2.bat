echo >>cdtest.tmp Starting Pass %3
xcopy %1\i386\n*.* %2\cd-test 2>>cdtest.tmp
comp %1\i386\n*.* %2\cd-test\*.* <n.dat	>>cdtest.tmp
