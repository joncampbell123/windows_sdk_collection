@echo off

if %1# == /not# goto UNINSTALL
if %1# == /NOT# goto UNINSTALL
if %1# == /Not# goto UNINSTALL
if %1# == not# goto UNINSTALL
if %1# == NOT# goto UNINSTALL
if %1# == Not# goto UNINSTALL

if not %1#==# goto USAGE

if exist c:\ntdetect.000 goto ALREADY

attrib -h -s -r c:\ntdetect.com
copy c:\ntdetect.com c:\ntdetect.000
copy ntdetect.com c:\ntdetect.com
attrib +h +s +r c:\ntdetect.com
echo Debug NTDETECT installed successfully
echo To un install, type INSTALLD /NOT
goto END

:ALREADY
echo Debug NTDETECT already installed
echo To un install, type INSTALLD /NOT
goto END

:UNINSTALL
if not exist c:\ntdetect.000 goto NOT_INST
attrib -h -s -r c:\ntdetect.com
copy c:\ntdetect.000 c:\ntdetect.com
attrib +h +s +r c:\ntdetect.com
echo Debug NTDETECT has been removed.
erase c:\ntdetect.000
goto END

:NOT_INST
echo Can't remove.  Debug NTDETECT not installed!
goto END

:USAGE
echo USAGE:
echo INSTALLD [ /NOT ]

:END
