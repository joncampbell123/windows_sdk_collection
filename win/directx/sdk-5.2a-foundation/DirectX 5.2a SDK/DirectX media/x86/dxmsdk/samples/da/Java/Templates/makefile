!CMDSWITCHES +S

DIRS = Async_Load BasicApplet ButtonPick ExtendedApplet GeoApplet1 GeoApplet2 GeometryDrag IE4_Windowless ImageDrag JumpURL Splines Using_AFCMenus Using_AFCScrollbar Using_VBScript VCR

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
