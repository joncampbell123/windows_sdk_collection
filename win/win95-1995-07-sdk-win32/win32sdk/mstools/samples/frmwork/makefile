DIRLIST =       analog          \
                barmdi          \
                barsdi          \
                blandmdi        \
                cliptext        \
                commondg        \
                dialog          \
                dllskel         \
                editsdi         \
                filesys         \
                gdidib          \
                gdiinput        \
                gdimeta         \
                gdiout          \
                gdipal          \
                gdiprint        \
                gen32           \
                generic         \
                hooks           \
                iathread        \
                initree         \
                input           \
                listctrl        \
                menu95          \
                minihelp        \
                mixtree         \
                multipad        \
                ole2            \
                output          \
                propshet        \
                pview95         \
                rtfedit         \
                simple          \
                subcls          \
                tabctrl         \
                threads         \
                thunks95        \
                writepad


$(DIRLIST):
	cd $@
	@echo *** frmwork\$@ *** >>$(MSTOOLS)\samples\frmwork.tmp
	@nmake -a -i -nologo >>$(MSTOOLS)\samples\frmwork.tmp
	cd ..
