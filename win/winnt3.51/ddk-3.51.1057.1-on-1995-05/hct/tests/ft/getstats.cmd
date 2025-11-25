@rem ----------------------------------------------------------------------
@rem
@rem   getstats.cmd
@rem   ------------
@rem
@rem   Use this script to report the stats after an FT test run.
@rem
@rem   Usage: getstats version_number
@rem
@rem ----------------------------------------------------------------------

@if "%1" == "" goto usage

@rem
@rem print the arch type
@rem
@echo.
@echo Machine type %PROCESSOR_ARCHITECTURE% 

@rem
@rem Print the header
@rem

resft /b:%1 /h

@rem
@rem For each results file that exists, count the stats.
@rem

@if exist frshm.log  resft /b:%1 /hd frshm.log
@if exist cpbtm.log  resft /b:%1 /hd cpbtm.log
@if exist strwp.log  resft /b:%1 /hd strwp.log
@if exist strwop.log resft /b:%1 /hd strwop.log
@if exist volset.log resft /b:%1 /hd volset.log
@if exist extvol.log resft /b:%1 /hd extvol.log
@if exist part.log   resft /b:%1 /hd part.log

@echo.
@echo.

@if not exist frshm.log  @echo warning: results file frshm.log  does not exist
@if not exist cpbtm.log  @echo warning: results file cpbtm.log  does not exist
@if not exist strwp.log  @echo warning: results file strwp.log  does not exist
@if not exist strwop.log @echo warning: results file strwop.log does not exist
@if not exist volset.log @echo warning: results file volset.log does not exist
@if not exist extvol.log @echo warning: results file extvol.log does not exist
@if not exist part.log   @echo warning: results file part.log   does not exist

@goto done

:usage
@echo usage: getstats version_number
@echo        version_number must be an integer

:done
