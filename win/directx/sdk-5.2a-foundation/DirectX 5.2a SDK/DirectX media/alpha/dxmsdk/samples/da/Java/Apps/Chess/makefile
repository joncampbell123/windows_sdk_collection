!CMDSWITCHES +S
.SUFFIXES: .class .java

.java.class:
        $(JC) $(JCFLAGS) $<

default: fresh

fresh: clean all

all: 
    jvc /d classes *.java

clean:
    del *.class

