DEBUG = FALSE

GCC = nspire-gcc
AS  = nspire-as
GXX = nspire-g++
LD  = nspire-ld
GENZEHN = genzehn

GCCFLAGS = -Wall -W -marm
GCCFLAGSSAVE = -Wall -W -marm
LDFLAGS =
ZEHNFLAGS = --name "osext"

ifeq ($(DEBUG),FALSE)
	GCCFLAGS += -Os
else
	GCCFLAGS += -O0 -g
endif

OBJS = $(patsubst %.c, %.o, $(shell find . -depth -name \*.c))
OBJS += $(patsubst %.cpp, %.o, $(shell find . -depth -name \*.cpp))
OBJS += $(patsubst %.S, %.o, $(shell find . -depth -name \*.S))
HEADERS = $(patsubst %.c, %.o, $(shell find . -depth -name \*.h))
EXE = osext
DISTDIR = .
vpath %.tns $(DISTDIR)
vpath %.elf $(DISTDIR)

.PHONY: release cleanrelease cleanwoexe osext

release:
	env GCCFLAGS="-Wall -W -marm -DMODULES_CHOSEN_BY_MAKE -DMODULE_CLOCK" EXE="OSExt_clockonly" make -e all;\
	env GCCFLAGS="-Wall -W -marm -DMODULES_CHOSEN_BY_MAKE -DMODULE_CLOCK -DMODULE_SETTINGS -DMODULE_DESKTOP" EXE="OSExt_clock+desktop" make -e all;\
	env GCCFLAGS="-Wall -W -marm -DMODULES_CHOSEN_BY_MAKE -DMODULE_CLOCK -DMODULE_SETTINGS -DMODULE_DESKTOP -DMODULE_DISABLENAVNET" EXE="OSExt_clock+desktop+disablenavnet" make -e all;\
	env GCCFLAGS="-Wall -W -marm -DMODULES_CHOSEN_BY_MAKE -DMODULE_CLOCK -DMODULE_SETTINGS -DMODULE_DESKTOP -DMODULE_DISABLENAVNET -DMODULE_SECURITY" EXE="OSExt_full" make -e all;\
	env GCCFLAGS="-Wall -W -marm -DMODULES_CHOSEN_BY_MAKE -DMODULE_CLOCK" EXE="OSExt_clockonly" make -e cleanwoexe;\
	env GCCFLAGS="-Wall -W -marm -DMODULES_CHOSEN_BY_MAKE -DMODULE_CLOCK -DMODULE_SETTINGS -DMODULE_DESKTOP" EXE="OSExt_clock+desktop" make -e cleanwoexe;\
	env GCCFLAGS="-Wall -W -marm -DMODULES_CHOSEN_BY_MAKE -DMODULE_CLOCK -DMODULE_SETTINGS -DMODULE_DESKTOP -DMODULE_DISABLENAVNET" EXE="OSExt_clock+desktop+disablenavnet" make -e cleanwoexe;\
	env GCCFLAGS="-Wall -W -marm -DMODULES_CHOSEN_BY_MAKE -DMODULE_CLOCK -DMODULE_SETTINGS -DMODULE_DESKTOP -DMODULE_DISABLENAVNET -DMODULE_SECURITY" EXE="OSExt_full" make -e cleanwoexe;

cleanrelease:
	env GCCFLAGS="-Wall -W -marm -DMODULES_CHOSEN_BY_MAKE -DMODULE_CLOCK" EXE="OSExt_clockonly" make -e clean;\
	env GCCFLAGS="-Wall -W -marm -DMODULES_CHOSEN_BY_MAKE -DMODULE_CLOCK -DMODULE_SETTINGS -DMODULE_DESKTOP" EXE="OSExt_clock+desktop" make -e clean;\
	env GCCFLAGS="-Wall -W -marm -DMODULES_CHOSEN_BY_MAKE -DMODULE_CLOCK -DMODULE_SETTINGS -DMODULE_DESKTOP -DMODULE_DISABLENAVNET" EXE="OSExt_clock+desktop+disablenavnet" make -e clean;\
	env GCCFLAGS="-Wall -W -marm -DMODULES_CHOSEN_BY_MAKE -DMODULE_CLOCK -DMODULE_SETTINGS -DMODULE_DESKTOP -DMODULE_DISABLENAVNET -DMODULE_SECURITY" EXE="OSExt_full" make -e clean;

osext: $(EXE).tns

all: $(EXE).tns

$(HEADERS):

%.o: %.c $(HEADERS)
	$(GCC) $(GCCFLAGS) -c $< -o $(EXE)$@

%.o: %.cpp
	$(GXX) $(GCCFLAGS) -c $< -o $(EXE)$@
	
%.o: %.S
	$(AS) -c $< -o $(EXE)$@

$(EXE).elf: $(OBJS)
	mkdir -p $(DISTDIR)
	$(LD) $(EXE)$^ -o $(EXE)$@ $(LDFLAGS)

$(EXE).tns: $(EXE).elf
	$(GENZEHN) --input $(EXE)$^ --output $(EXE)$@.zehn $(ZEHNFLAGS)
	make-prg $(EXE)$@.zehn $@
	rm $(EXE)$@.zehn

clean:
	rm -f $(OBJS) ./$(EXE)osext.o $(DISTDIR)/$(EXE).tns $(DISTDIR)/$(EXE)$(EXE).elf $(DISTDIR)/$(EXE).zehn

cleanwoexe:
	rm -f $(OBJS) ./$(EXE)osext.o $(DISTDIR)/$(EXE)$(EXE).elf $(DISTDIR)/$(EXE).zehn

