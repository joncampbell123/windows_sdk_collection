#
# Win32 SDK makefile. Builds all MAPI samples.
#

DIRLIST =   CHECKERS.FRM	\
            CHSFLD.CLI		\
            CMC.CLI			\
            COMMAND.EXT		\
			DOCFILE.MS		\
            EVENTS.EXT		\
            FLATFILE.AB		\
            MANAGER.SH		\
            PEER.XP			\
            PROFILE.CLI		\
            PROPSH.EXT		\
            ROUTE.CLI		\
			REMOTE.SRV		\
			REMOTE.XP		\
            SENDRTF.CLI		\
            SIMPLE.CLI		\
            SIMPLE.FRM		\


all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** mapi\$@ *** >>$(MSTOOLS)\samples\mapi.tmp
        @nmake -a -i -nologo >>$(MSTOOLS)\samples\mapi.tmp
        cd ..

