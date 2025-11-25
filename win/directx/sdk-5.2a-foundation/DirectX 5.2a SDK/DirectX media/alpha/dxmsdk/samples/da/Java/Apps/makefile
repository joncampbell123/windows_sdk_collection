!CMDSWITCHES +S

DIRS = Chess

MAKE = nmake

default: fresh

all: fresh

fresh:
	for %i in ($(DIRS)) do nmake recursive DIR=%%i HOW=fresh

clean:
	for %i in ($(DIRS)) do nmake recursive DIR=%%i HOW=clean

recursive:
	cd $(DIR)
        $(MAKE) $(HOW)
	cd ..