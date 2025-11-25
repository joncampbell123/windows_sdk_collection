!CMDSWITCHES +S
.SUFFIXES: .class .java

.java.class:
        $(JC) $(JCFLAGS) $<

default: fresh

fresh: clean all

all: 
    jvc /d ..\..\..\..\..\media *.java
	jvc /d ..\..\..\..\..\media encarta_module\*.java


clean:
    del *.class

