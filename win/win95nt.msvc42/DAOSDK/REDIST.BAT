
REM Creates a directory of DAO redist components
REM (assumed running from directory containing full
REM  dao installation, i.e. disk1, disk2, ...
REM ============================================

mkdir REDIST
copy disk1\nosdk.pkg redist\setup.pkg
copy disk1\daocore.1 redist\*.*
copy disk1\_SETUP.lib redist\*.*
copy disk1\NOSDK.iss redist\setup.iss
copy disk1\Disk1.ID redist\*.*
copy disk1\SETUP.INS redist\*.*
copy disk1\setup.exe redist\*.*
copy disk1\_setup.dll redist\*.*
copy disk1\UNINST.EXE redist\*.*
copy disk2\daocore.2 redist\*.*
copy disk2\jetisam.z redist\*.*


