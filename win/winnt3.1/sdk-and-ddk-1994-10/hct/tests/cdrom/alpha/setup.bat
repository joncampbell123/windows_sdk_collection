@echo Insert a blank floppy diskette in the a:\ drive

pause

copy setupldr. a:\
copy setupapp.exe a:\
copy arctest.inf a:\

echo Done!
echo to run, from ARC firmware prompt, choose run an app, 
     and select a:\setupldr
