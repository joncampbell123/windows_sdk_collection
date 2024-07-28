@echo off
:DISK1
cls
echo Insert disk #1 in %1
..\..\windows\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto DISK2
..\..\windows\tools\imdump disk1.img %1
..\..\windows\tools\imdump disk1.img %1 /v

:DISK2
echo 
echo Insert disk #2 in %1
..\..\windows\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto DISK3
..\..\windows\tools\imdump disk2.img %1
..\..\windows\tools\imdump disk2.img %1 /v

:DISK3
echo 
echo Insert disk #3 in %1
..\..\windows\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto DISK4
..\..\windows\tools\imdump disk3.img %1
..\..\windows\tools\imdump disk3.img %1 /v

:DISK4
echo 
echo Insert disk #4 in %1
..\..\windows\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto DISK5
..\..\windows\tools\imdump disk4.img %1
..\..\windows\tools\imdump disk4.img %1 /v

:DISK5
echo 
echo Insert disk #5 in %1
..\..\windows\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto DISK6
..\..\windows\tools\imdump disk5.img %1
..\..\windows\tools\imdump disk5.img %1 /v

:DISK6
echo 
echo Insert disk #6 in %1
..\..\windows\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto DISK7
..\..\windows\tools\imdump disk6.img %1
..\..\windows\tools\imdump disk6.img %1 /v

:DISK7
echo 
echo Insert disk #7 in %1
..\..\windows\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto DISK8
..\..\windows\tools\imdump disk7.img %1
..\..\windows\tools\imdump disk7.img %1 /v

:DISK8
echo 
echo Insert disk #8 in %1
..\..\windows\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto DISK9
..\..\windows\tools\imdump disk8.img %1
..\..\windows\tools\imdump disk8.img %1 /v

:DISK9
echo 
echo Insert disk #9 in %1
..\..\windows\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto DISK10
..\..\windows\tools\imdump disk9.img %1
..\..\windows\tools\imdump disk9.img %1 /v

:DISK10
echo 
echo Insert disk #10 in %1
..\..\windows\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto DISK11
..\..\windows\tools\imdump disk10.img %1
..\..\windows\tools\imdump disk10.img %1 /v

:DISK11
echo 
echo Insert disk #11 in %1
..\..\windows\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto DISK12
..\..\windows\tools\imdump disk11.img %1
..\..\windows\tools\imdump disk11.img %1 /v

:DISK12
echo 
echo Insert disk #12 in %1
..\..\windows\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto DISK13
..\..\windows\tools\imdump disk12.img %1
..\..\windows\tools\imdump disk12.img %1 /v

:DISK13
goto EGRESS
echo 
echo Insert disk #13 in %1
..\..\windows\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto DISK14
..\..\windows\tools\imdump diskd.img %1
..\..\windows\tools\imdump diskd.img %1 /v

:DISK14
echo 
echo Insert disk #14 in %1
..\..\windows\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto DISK15
..\..\windows\tools\imdump diske.img %1
..\..\windows\tools\imdump diske.img %1 /v

:DISK15
echo 
echo Insert disk #15 in %1
..\..\windows\tools\ync /c msx "Make, Skip, eXit"
if ERRORLEVEL 2 goto EGRESS
if ERRORLEVEL 1 goto EGRESS
..\..\windows\tools\imdump diskf.img %1
..\..\windows\tools\imdump diskf.img %1 /v

echo All done:
:EGRESS
echo on



