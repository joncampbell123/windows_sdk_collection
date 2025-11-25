@echo off

@rem
@rem Windows/NT HCT
@rem
@rem hctgmpnt.cmd - cmd file to run GUIMan print test
@rem
@rem

	guiman gmprint.scr

        goto end

:usage
        echo.
	echo Usage: hctgmpnt
        echo.
	echo.
	echo	   Prints to active Default Printer in Print Manager
	echo.

:end
