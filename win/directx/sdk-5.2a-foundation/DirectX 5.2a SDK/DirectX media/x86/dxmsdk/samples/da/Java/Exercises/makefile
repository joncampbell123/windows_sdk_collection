!CMDSWITCHES +S
 
DIRS = Apple Hello HelloBvr HelloCycle HelloRBvr Movie Image Import Switch Geometry Sound\  

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
