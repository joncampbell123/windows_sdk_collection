
$(ROOT)\sdk\lib\strmbasd.lib: $(ROOT)\lib$(BUILD_ALT_DIR)\$(TARGET_DIRECTORY)\strmbase.lib
    copy /v $** $@

$(ROOT)\sdk\lib\strmbase.lib: $(ROOT)\lib$(BUILD_ALT_DIR)\$(TARGET_DIRECTORY)\$(@F)
    copy /v $** $@
