DEBUG = FALSE

GCC = nspire-gcc
AS  = nspire-as
GXX = nspire-g++
LD  = nspire-ld
GENZEHN = genzehn



GCCFLAGS = -Wall -W -marm -ffreestanding -nostdlib -lgcc -Os -fPIE -g
LDFLAGS = -nostdlib -lgcc -static-pie -T ldscript.ld
ZEHNFLAGS = --name "osext"  --author "nspiredev500" --version "20" --240x320-support true --uses-lcd-blit false

OBJS = $(patsubst %.c, %.o, $(shell find . -depth -name \*.c))
OBJS += $(patsubst %.cpp, %.o, $(shell find . -depth -name \*.cpp))
OBJS += $(patsubst %.S, %.o, $(shell find . -depth -name \*.S))
DISTDIR = .

HEADERS = $(patsubst %.c, %.o, $(shell find . -depth -name \*.h))




all: osext.tns debug.elf
	

HEADERS: ;

.PHONY: clean cleanbuild remake




remake: clean
	$(MAKE) all

osext.tns: osext.elf
	cp osext.elf osext.elf.tmp && \
	arm-none-eabi-strip --strip-debug osext.elf.tmp && \
	$(GENZEHN) $(ZEHNFLAGS) --input osext.elf.tmp --output osext.tns && \
	rm -f osext.elf.tmp


osext.elf: $(OBJS) 
	mkdir -p $(DISTDIR) && \
	$(LD) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJS) osext.elf debug.elf osext.tns

cleanbuild:
	rm -f $(OBJS) osext.elf debug.elf


debug.elf: osext.elf
	arm-none-eabi-objcopy --only-keep-debug osext.elf debug.elf


%.o: %.c $(HEADERS)
	$(GCC) $(GCCFLAGS) -c $< -o $@


%.o: %.cpp $(HEADERS)
	$(GXX) $(GCCFLAGS) -c $< -o $@


%.o: %.S 
	$(AS) -c $< -o $@

















