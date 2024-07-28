rem
rem makeapp2 srcfile dstfile Class TYPENAME AppPrefix appname ptrtype
rem             %1      %2      %3      %4     %5      %6       %7
copy %1 %2 >nul
rep MakeWc %3   %2
rep MAKEWC %4   %2
rep MakeApp_ %5 %2
rep makeapp %6  %2
rep pmwc %7     %2
