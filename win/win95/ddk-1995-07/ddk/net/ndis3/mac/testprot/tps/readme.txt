FINAL PRODUCT SUPPLEMENT
------------------------


This document has been created for the purpose of describing how to compare log files
based on the filter modes supported by the card and the new tpctl test tool enhancements

1  SUPPORTED MODE TEST SUPPLEMENT

1.0 Environment Variables

    Very Important NOTE

    The tool TPDIFF MUST be run in the command window in which the environment variables for
supported modes has been set/unset. The 5 modes are:

        PROMISCUOUS
        SOURCEROUTING
        MACFRAME
        ALLFUNCTIONAL
        ALLMULTICAST

Should your driver not suppport one or more of the modes, please unset the variables prior to running
TPDIFF. This can be achieved by

    a. Editing the set_vars.bat file and changing set VARIABLE=1 to set VARIABLE
    b. By manually within the command window unsetting the variable


1.1 Tester Control

    Commands

    Four new commands

           DISABLE
           ENABLE
           RECORDINGENABLE [ScriptName]
           RECORDINGDISABLE
    
    have been added to the TPCTL tool.


DISABLE and ENABLE

The purpose of these commands is to disable/enable the tool based on the setting of the environment
variables in section 1.0. These commands are present in the two machine tests since they
synchronization control is dependent on the filter support offered by the card.

As an example, if you card does not support PROMISCUOUS and ALLMULTICAST modes of operation,
you will unset these variables and then from within the same command window invoke the TPCTL
tool. On encountering the command

            DISABLE PROMISCUOUS

the tool will detect(from the enviroment) that this mode is not supported and will skip over
following commands till it encounters the command

            ENABLE

More help is available on-line by typing in HELP at the TPCTL prompt.


RECORDINGENABLE and RECORDINGDISABLE

These commands are responsible for recording an interactive session with the tool to a script
file.

More help is available on-line by typing in HELP at the TPCTL prompt.


2   TPDIFF.EXE Keywords

2.0 Operational characteristcs of the comparator

    The current line keywords being supported are

    MAY_DIFFER
    EQUAL_LAST
    BETWEEN_VALUES
    SKIP_LINE

    MAY_DIFFER

    The lines under comparison may differ

    EQUAL_LAST

    The current extracted value or values from the right hand
    side of an "=" or ":" operator must equal the last
    extracted values.

    BETWEEN_VALUES X, Y

    The current extracted values from the current line under examination
    must be between values X and Y


    SKIP_LINE

    The tool is instructed to skip to the next line in the file


    The current tool control keywords being supported are

    -SECTION_START-( <Section_Id> )
    -SECTION_END-( <Section_Id> )
    -TOKEN_MATCH-( <X>, <Y> )
    -OPTIONALS-( [ENV_VAR_1[<,> ENV_VAR_2[<,>...ENV_VAR_N]]..] )
    -SECTION_DESC-( <">[Informational String]<"> )

    where parameters within <> are required and [] are optional.

    -SECTION_START-[ \t]<(>[ \t]<Section_Id>[ \t]<)>
        where Section_Id is a numeric value


    -SECTION_END-[ \t]<(>[ \t]<Section_Id>[ \t]<)>
        where Section_Id is a numeric value

    -TOKEN_MATCH-[ \t]<(><X><,>[ \t]<Y>[ \t]<)>
        where X and Y mark the starting and ending lines


    -OPTIONALS-[ \t]<(>[ENV_VAR_1[<,> ENV_VAR_2[<,>...ENV_VAR_N]]..][ \t]<)>
        where ENV_VAR_N marks environment variables which may or maynot
        be set

    where [ \t] indicates white space(blanks, tabs)

    The tool uses the above control keywords to break up files into sections
    and compare sections. It permits nesting of sections. Each section is
    marked at its beginning with the keyword -SECTION_START- followed by
    a numberical value indicating the section number and ended by the keyword
    -SECTION_END- followed by the same section identifier. The two must match
    else the ending will be treated as a mismatch and be compared as a regular
    line. -TOKEN-MATCH- within a section treats lines as tokens where the
    comparison starts at section offset X and ends at section offset Y. Since
    this is a token comparison, it removes any order dependencies. -OPTIONALS-
    controls the tool in that if ALL environment variables specified within
    the control have been set, the tool compares that section else it simply
    skips over that section.

    For section nesting, the section ID of the internal nest must be greater
    that the external nest

    Example: Please see the comparison comments


          FILE A(COMPARED)             FILE B(GOLDEN)

Line#1  ...                          ...
Line#2  ...                          ...
Line#3  ...                          ...
Line#4  ...                          ...
Line#5  -SECTION_START-( 1.0 )       ...
Line#6  Section 1.0 test line ok     ...
Line#7  Section 1.0 test nok         -SECTION_START-(  1.0   )
Line#8  ...                          Section 1.0 test line ok
Line#9  ...                          Section 1.0 test line nok  <- Mismatched
Line#10 -SECTION_START-( 1.01 )      ...
Line#11 ...                          -SECTION_START-( 1.01 ) -OPTIONALS-()
Line#12 -SECTION_END-( 1.01 )        This section is optional...
Line#13 Back in section 1.0          ...
Line#14 More lines cause Sync Errs             -SECTION_END-( 1.01 )
Line#15 -SECTION_START-( 1.03 )      Back in section 1.0
Line#16 ...                          -SECTION_START-(1.03) -OPTIONALS-( ENV1,ENV2 )
Line#17 -SECTION_END-( 1.03 )        This section is not optional if ENV1 And ENV2 are defined
Line#18 -SECTION_START-(1.05)                  -SECTION_END-( 1.03 )
Line#19 ...                          -SECTION_START-(1.05) -TOKEN_MATCH-( 4,0 )
Line#20 ...                          Starting from line 4 offset within this section
Line#21 ...                          everything will be compared as a token and is not
Line#22 Token5                       order dependent
Line#23 Token4                       Token1
Line#24 Token3                       Token2
Line#25 Token1                       Token3
Line#26 Token2                       Token4
Line#27 -SECTION_END-( 1.05 )        Token5  MAY_DIFFER
Line#28 Abc = 123,456,789            -SECTION_END-( 1.05 )
Line#29 Efg = 123,123,123            Abc = 123,456,789
Line#30 ...                          Efg = 123,456,789 EQUAL_LAST
Line#31 ...                          In the last comparison,index 2 and 3 values did not equal last
Line#32 Skip this line  SKIP_LINE    Skip this line     SKIP_LINE
Line#33 Not this one                 Skip this one also SKIP_LINE
Line#34 Abc=123,345,1000             Not this one               <-Comparison will be OK
Line#35 ...                          Abc=123,345,999    BETWEEN_VALUES 0,999
Line#36 May differ line              Last line index 3 would have been a mismatch
Line#37 -SECTION_END-( 1.0 )         Line may differ    MAY_DIFFER
Line#38                              -SECTION_END-( 1.0 )


3. Test Driver Debug Information (Checked build *.DBG only)

The test driver checked build has a variable "TpDebug" which is responsible for
controling the information being sent to the debug screen output. The are
various levels and type of debug information which can be controled by this
variable.

TpDebug is a ULONG(32 or 64 bit variable depending on the definition of a ULONG).

Current control settings are:

TP_DEBUG_NDIS_CALLS  0x00000001    // print Ndis Status returns
TP_DEBUG_NDIS_ERROR  0x00000002    // print Ndis Error returns
TP_DEBUG_STATISTICS  0x00000004    // print stress statistics
TP_DEBUG_DATA        0x00000008    // print Data Corruption msgs

TP_DEBUG_DISPATCH    0x00000010    // TpDispatch routines
TP_DEBUG_IOCTL_ARGS  0x00000020    // print args from the ioctl

TP_DEBUG_NT_STATUS   0x00000100    // print !success NT Status returns
TP_DEBUG_DPC         0x00000200    // print DPC problem info
TP_DEBUG_INITIALIZE  0x00000400    // print init error info
TP_DEBUG_RESOURCES   0x00000800    // print resource allocation errors

TP_DEBUG_BREAKPOINT  0x00001000    // enable and disable DbgBreakPoints

TP_DEBUG_INFOLEVEL_1 0x00010000    // print information. Level 1
TP_DEBUG_INFOLEVEL_2 0x00020000    // through 4 represent different
TP_DEBUG_INFOLEVEL_3 0x00040000    // types of information where
TP_DEBUG_INFOLEVEL_4 0x00080000    // Level 1 is purely informational
                                   // Level 2 is corrective action information
                                   // Level 3 is sequential action information
                                   // Level 4 Reserved. Currently undefined.

TP_DEBUG_ALL         0xFFFFFFFF    // turns on all flags

You can edit this variable and control the information being printed to the
debug screen by

  a. Breaking into the kernel debugger
  b. Editing the variable:
     Examples:
      kd > ed TPDRVR!TpDebug 00000001       This will cause only NdisStatus information to appear
      or
      kd > ed TPDRVR!TpDebug 00000020       This will cause only args from the ioctl to be printed
      or
      kd > ed TPDRVR!TpDebug ffffffff       This will cause ALL tpdrvr debug information to be printed
