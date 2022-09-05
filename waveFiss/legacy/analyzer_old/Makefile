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

CXXFLAGS += -w -Wconversion -Wformat -Wall -Wuninitialized -I$(PWD)/include
LDFLAGS  += -lz -pthread #-mt

SRCDIR = $(PWD)/src
OBJDIR = $(PWD)/obj
BINDIR = $(PWD)/bin
LIBDIR = $(PWD)/lib

SRCS  := $(shell find $(SRCDIR) -type f -name *.$(SrcSuf))
SRCS  := $(filter-out $(SRCDIR)/fefix.$(SrcSuf), $(SRCS))
BINS  := $(patsubst $(SRCDIR)/%.$(SrcSuf), $(BINDIR)/%$(ExeSuf), $(SRCS))
OBJS  := $(patsubst $(SRCDIR)/%.$(SrcSuf), $(OBJDIR)/%.$(ObjSuf), $(SRCS))

############# RULES ###############
$(OBJDIR)/%.$(ObjSuf): $(SRCDIR)/%.$(SrcSuf)
	$(CXX) $(CXXFLAGS) -c $< -o $@

############# TARGETS #############
.SUFFIXES: .$(SrcSuf) .$(ObjSuf) $(ExeSuf) .$(DllSuf)

all: $(BINS)

### for some reason, zlib is picky about where link flags are placed
$(BINDIR)/analyzer: $(OBJDIR)/analyzer.o | $(BINDIR)
	$(LD) $^ $(GLIBS) $(OutPutOpt)$@ $(LDFLAGS)
	@echo "\n$@ done"

$(BINDIR)/angle_calibrations: $(OBJDIR)/angle_calibrations.o | $(BINDIR)
	$(LD) $(LDFLAGS) $^ $(GLIBS) $(OutPutOpt)$@
	@echo "\n$@ done"

$(BINDIR)/eloss_corrections: $(OBJDIR)/eloss_corrections.o | $(BINDIR)
	$(LD) $(LDFLAGS) $^ $(GLIBS) $(OutPutOpt)$@
	@echo "\n$@ done"

$(BINDIR)/eventbuild: $(OBJDIR)/eventbuild.o | $(BINDIR)
	$(LD) $(LDFLAGS) $^ $(GLIBS) $(OutPutOpt)$@
	@echo "\n$@ done"

$(BINDIR)/eventbuild_gammatest: $(OBJDIR)/eventbuild_gammatest.o | $(BINDIR)
	$(LD) $(LDFLAGS) $^ $(GLIBS) $(OutPutOpt)$@
	@echo "\n$@ done"

$(BINDIR)/iterate: $(OBJDIR)/iterate.o | $(BINDIR)
	$(LD) $(LDFLAGS) $^ $(GLIBS) $(OutPutOpt)$@
	@echo "\n$@ done"

$(BINDIR)/singlesprocessing: $(OBJDIR)/singlesprocessing.o | $(BINDIR)
	$(LD) $(LDFLAGS) $^ $(GLIBS) $(OutPutOpt)$@
	@echo "\n$@ done"

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(BINDIR):
	mkdir $(BINDIR)

clean:
	@rm -f $(OBJS) $(BINS)
