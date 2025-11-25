@echo video regression tests
@echo off
rem
rem
rem /t - specify a *.t file
rem /o - specify a *.o file
rem /v - specify an output log file
rem /l - Do logging to output file
rem /c - close after running scripts (do not use if debugging typically)
rem /m - maximize window on startup
rem /d - enable comparisons against memory dc (for scripts)
rem
guijr /tini1.t /oini1.o /vini1.log  /c /l /m /d
