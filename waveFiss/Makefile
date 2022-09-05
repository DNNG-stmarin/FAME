############# MACROS ##############
RC     := root-config
ifeq ($(shell which $(RC) 2>&1 | sed -ne "s@.*/$(RC)@$(RC)@p"),$(RC))
MKARCH := $(wildcard $(shell $(RC) --etcdir)/Makefile.arch)
endif
ifneq ($(MKARCH),)
include $(MKARCH)
else
include $(ROOTSYS)/etc/Makefile.arch
endif

SRCDIR = $(PWD)/src
OBJDIR = $(PWD)/obj
BINDIR = $(PWD)/bin
LIBDIR = $(PWD)/lib
INCDIR = $(PWD)/include
CINT   = $(PWD)/cint

CXXFLAGS += -w -Wconversion -Wformat -Wall -Wuninitialized -I$(INCDIR)
LDFLAGS  += -lz -pthread #-mt
GLIBS    += -lz

DICT      := $(CINT)/WaveformProcessingDict.$(SrcSuf)
#DICTH     := $(CINT)/$(DICT:.$(SrcSuf)=.h)
# DICTO     := $(OBJDIR)/$(notdir $(DICT:.$(SrcSuf)=.$(ObjSuf)))
DICTO     := $(OBJDIR)/WaveformProcessingDict.$(ObjSuf)

SRCS  := $(shell find $(SRCDIR) -type f -name *.$(SrcSuf))
SRCS  := $(filter-out $(SRCDIR)/WaveformProcessing.$(SrcSuf), $(SRCS))
BINS  := $(patsubst $(SRCDIR)/%.$(SrcSuf), $(BINDIR)/%$(ExeSuf), $(SRCS))
SRCS  += $(SRCDIR)/WaveformProcessing.$(SrcSuf)
OBJS  := $(patsubst $(SRCDIR)/%.$(SrcSuf), $(OBJDIR)/%.$(ObjSuf), $(SRCS))
OBJS  += $(DICTO)

HDRS      := $(shell find $(INCDIR) -type f -name *.h) cint/LinkDef.h
HDRS      := $(filter-out $(DICTH),$(HDRS))

TARGETLIB   := lib/libWaveformProcessing.$(DllSuf)
ROOTMAPFILE := $(patsubst %.so,%.rootmap,$(TARGETLIB))

############# RULES ###############
$(OBJDIR)/%.$(ObjSuf): $(SRCDIR)/%.$(SrcSuf)
	$(CXX) $(CXXFLAGS) -c $< -o $@

############# TARGETS #############
.SUFFIXES: .$(SrcSuf) .$(ObjSuf) $(ExeSuf) .$(DllSuf)

all: $(TARGETLIB) $(BINS)

$(DICTO): $(DICT)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(DICT): $(HDRS) | $(LIBDIR)
	@echo "Generating dictionary $@..."
	$(ROOTCLING) -f $@ -s $(TARGETLIB) -rml $(TARGETLIB) -rmf $(ROOTMAPFILE) -c $(INCLUDE) -p $^

$(TARGETLIB): $(DICTO) $(OBJDIR)/WaveformProcessing.o | $(LIBDIR)
ifeq ($(PLATFORM),macosx)
	$(LD) $(SOFLAGS)$@ $(LDFLAGS) $^ $(OutPutOpt) $@ $(GLIBS)
else
	$(LD) $(SOFLAGS) $(LDFLAGS) $^ $(GLIBS) $(OutPutOpt)$@
endif

$(BINDIR)/unpacker: $(OBJDIR)/unpacker.o | $(BINDIR)
	$(LD) $(LDFLAGS) $^ $(GLIBS) $(OutPutOpt)$@
	@echo "\n$@ done"

$(BINDIR)/angle_calibrations: $(OBJDIR)/angle_calibrations.o $(TARGETLIB) $(DICTO) | $(BINDIR)
	$(LD) $(LDFLAGS) $^ $(GLIBS) -L$(LIBDIR) -lWaveformProcessing $(OutPutOpt)$@
	@echo "\n$@ done"

$(BINDIR)/eloss_corrections: $(OBJDIR)/eloss_corrections.o $(TARGETLIB) $(DICTO) | $(BINDIR)
	$(LD) $(LDFLAGS) $^ $(GLIBS) -L$(LIBDIR) -lWaveformProcessing $(OutPutOpt)$@
	@echo "\n$@ done"

$(BINDIR)/eventbuild: $(OBJDIR)/eventbuild.o $(TARGETLIB) $(DICTO) | $(BINDIR)
	$(LD) $(LDFLAGS) $^ $(GLIBS) -L$(LIBDIR) -lWaveformProcessing $(OutPutOpt)$@
	@echo "\n$@ done"

# $(BINDIR)/eventbuild_gammatest: $(OBJDIR)/eventbuild_gammatest.o $(TARGETLIB) $(DICTO) | $(BINDIR)
# 	$(LD) $(LDFLAGS) $^ $(GLIBS) $(OutPutOpt)$@
# 	@echo "\n$@ done"

$(BINDIR)/iterate: $(OBJDIR)/iterate.o $(TARGETLIB) $(DICTO) | $(BINDIR)
	$(LD) $(LDFLAGS) $^ $(GLIBS) -L$(LIBDIR) -lWaveformProcessing $(OutPutOpt)$@
	@echo "\n$@ done"

$(BINDIR)/singlesprocessing: $(OBJDIR)/singlesprocessing.o $(TARGETLIB) $(DICTO) | $(BINDIR)
	$(LD) $(LDFLAGS) $^ $(GLIBS) -L$(LIBDIR) -lWaveformProcessing $(OutPutOpt)$@
	@echo "\n$@ done"

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(BINDIR):
	mkdir $(BINDIR)

$(LIBDIR):
	mkdir $(LIBDIR)

.PHONY: all

clean:
	@rm -f $(OBJS) $(BINS) $(DICT) $(DICTO) $(TARGETLIB) $(ROOTMAPFILE) $(LIBDIR)/*.pcm
