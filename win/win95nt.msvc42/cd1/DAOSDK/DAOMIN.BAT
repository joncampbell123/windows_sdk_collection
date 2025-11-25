REM Creates a directory of minimum DAO redist components
REM (assumed running from directory containing full
REM  dao installation, i.e. disk1, disk2, ...
REM ============================================

mkdir DAOMIN
copy disk1\daomin.pkg DAOMIN\setup.pkg
copy disk1\daocore.1 DAOMIN\*.*
copy disk2\daocore.2 DAOMIN\*.*
copy disk1\_SETUP.lib DAOMIN\*.*
copy disk1\DAOMIN.iss daomin\setup.iss
copy disk1\Disk1.ID DAOMIN\*.*
copy disk1\DAOmin.INS DAOMIN\setup.ins
copy disk1\setup.exe DAOMIN\*.*
copy disk1\_setup.dll DAOMIN\*.*
copy disk1\UNINST.EXE DAOMIN\*.*


