DIRLIST =       callback        \
                        cluuid          \
                        data            \
                        dict            \
                        doctor          \
                        dynept          \
                        dynout          \
                        handles         \
                        hello           \
                        interop     \
                        mandel          \
                        ns                      \
                        object          \
                        pickle          \
                        pipes           \
                        rpcssm          \
                        rpcsvc          \
                        strout          \
                        wintyp          \
                        whello          \
                        yield


all: $(DIRLIST)




$(DIRLIST):
        cd $@
        echo *** rpc\$@ *** >>$(MSTOOLS)\samples\rpc.tmp
        nmake $(BUILD_OPT) -nologo -a -i >>$(MSTOOLS)\samples\rpc.tmp
        cd ..
