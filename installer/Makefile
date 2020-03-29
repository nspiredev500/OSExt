build_dir_raw ?= ../build
build_dir = $(build_dir_raw)/installer
bin_dir ?= ../bin

DEBUG = FALSE

GCC = nspire-gcc
AS  = nspire-as
GXX = nspire-g++
LD  = nspire-ld
GENZEHN = genzehn

GCCFLAGS = -Wall -W -marm
LDFLAGS =
ZEHNFLAGS = --name "osextinstaller" --240x320-support true

ifeq ($(DEBUG),FALSE)
	GCCFLAGS += -Os
else
	GCCFLAGS += -O0 -g
endif

OBJS = $(patsubst %.c, %.o, $(shell find . -name \*.c -printf "$(build_dir)/%f "))
OBJS += $(patsubst %.cpp, %.o, $(shell find . -name \*.cpp -printf "$(build_dir)/%f "))
OBJS += $(patsubst %.S, %.o, $(shell find . -name \*.S -printf "$(build_dir)/%f "))
EXE = osextinstaller
DISTDIR = .
vpath %.tns $(DISTDIR)
vpath %.elf $(DISTDIR)

all: $(build_dir)/$(EXE).tns
	


$(build_dir)/%.o: $(CURDIR)/%.c
	$(GCC) $(GCCFLAGS) -c $< -o $@

$(build_dir)/%.o: $(CURDIR)/%.cpp
	$(GXX) $(GCCFLAGS) -c $< -o $@
	
$(build_dir)/%.o: $(CURDIR)/%.S
	$(AS) -c $< -o $@

$(build_dir)/$(EXE).elf: $(OBJS)
	mkdir -p $(build_dir) && \
	$(LD) $^ -o $@ $(LDFLAGS)

$(build_dir)/$(EXE).tns: $(build_dir)/$(EXE).elf
	$(GENZEHN) --input $^ --output $@.zehn $(ZEHNFLAGS) && \
	make-prg $@.zehn $@ && \
	rm $@.zehn && \
	cp $(build_dir)/$(EXE).tns $(bin_dir)/$(EXE).tns

clean:
	-rm -f $(OBJS) $(build_dir)/$(EXE).tns $(build_dir)/$(EXE).elf $(build_dir)/$(EXE).zehn
