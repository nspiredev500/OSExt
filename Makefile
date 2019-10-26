DEBUG = FALSE

GCC = nspire-gcc
AS  = nspire-as
GXX = nspire-g++
LD  = nspire-ld
GENZEHN = genzehn

GCCFLAGS = -Wall -W -marm
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
HEADERS = modules/desktop/desktop.h modules/shell/shell.h modules/windowoverlay/windowmanager.h modules/clock/miniclock.h apis/digits.h apis/rawdraw.h apis/nclockfragments.h apis/graphics.h apis/usbtest.h modules/disablenavnet/disablenavnet.h
EXE = osext
DISTDIR = .
vpath %.tns $(DISTDIR)
vpath %.elf $(DISTDIR)

all: $(EXE).tns

$(HEADERS):

%.o: %.c $(HEADERS)
	$(GCC) $(GCCFLAGS) -c $< -o $@

%.o: %.cpp
	$(GXX) $(GCCFLAGS) -c $< -o $@
	
%.o: %.S
	$(AS) -c $< -o $@

$(EXE).elf: $(OBJS)
	mkdir -p $(DISTDIR)
	$(LD) $^ -o $@ $(LDFLAGS)

$(EXE).tns: $(EXE).elf
	$(GENZEHN) --input $^ --output $@.zehn $(ZEHNFLAGS)
	make-prg $@.zehn $@
	rm $@.zehn

clean:
	rm -f $(OBJS) $(DISTDIR)/$(EXE).tns $(DISTDIR)/$(EXE).elf $(DISTDIR)/$(EXE).zehn
