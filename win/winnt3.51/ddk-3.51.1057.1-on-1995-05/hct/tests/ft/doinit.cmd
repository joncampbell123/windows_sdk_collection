@rem ************************************************************************
@rem Copyright (c) 1991 Microsoft Corporation
@rem
@rem Module Name:
@rem ------------
@rem   doinit.cmd
@rem
@rem Abstract:
@rem ---------
@rem   Script to perform the initialization for all of the do scripts,
@rem   except do.0.  It will set the env variables that all of them use.
@rem
@rem History:
@rem --------
@rem   Steveko 2-4-93
@rem
@rem *************************************************************************

@rem
@rem Run the cmd files that set relevant env variables
@rem

call getdat.cmd label fileSystem sizeMB driveLetter1 driveLetter2 _options1

if "%label%"=="" then goto error_set_env

call scendat.cmd logfile dummy ft_version ft_forever ft_qformat

call intldat.cmd intlYes intlNo intlCompFilesOk

@rem
@rem Set the _fill_percent to how full fsetmap will fill the disk.
@rem if _write_while_regen is set, the below value will not be used.
@rem 

set _fill_percent=85

@rem
@rem Set the _formatoptions to however you'ld like them.
@rem

@rem if fileSystem == ntfs set _formatoptions=/q
@rem if logfile == mirror _formatoptions=/q
@rem if logfile == cpbtmr _formatoptions=/q
@rem if logfile == strwp  _formatoptions=/q

goto done

:error_set_env
@echo Unexpected error setting environment variables
fttest -mUnexpected error setting environment variables

:done
