include ../Makefile.inc

DIRS = ../lib

TARGET	= unpack2root

SRCCPP	= unpack2root.cpp  

OBJECTS	= $(SRCCPP:.cpp=.o) 

OBJLIBS	= unpack2root.o

CFLAGS += -I../lib -I.

LFLAGS += -L../lib

.PHONY: all $(DIRS)
all:: $(DIRS)
$(DIRS):
	$(MAKE) -C $@

.cpp.o:
	$(CXX) -c $(CFLAGS) $< $(ROOTCFLAGS) -o $@

all:: $(TARGET)
$(TARGET):  $(OBJECTS) unpack2root.o ../lib/libDSP.a
	$(CXX) $(OBJECTS) \
	$(LFLAGS) $(ROOTLIBS) $(ROOTGLIBS) -lDSP -o $@

.PHONY: clean distclean
clean:
	echo cleaning up in .
	-rm -f *.o
	-for d in $(DIRS); do $(MAKE) -C $$d clean; done

distclean: clean
	echo cleaning up in .
	-rm -f $(TARGET)
	-for d in $(DIRS); do $(MAKE) -C $$d distclean; done
