!CMDSWITCHES +S
 
DIRS = Album3D Americana Cityscape Coffee Encarta FifteenPuzzle Lighthouse GeoImage Magnify MsCubes Pick3 Solar Tile Union\

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
