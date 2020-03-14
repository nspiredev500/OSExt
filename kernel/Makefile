DEBUG = FALSE

#GCC = nspire-gcc
#AS  = nspire-as
#GXX = nspire-g++
#LD  = nspire-ld
GCC = arm-none-eabi-gcc
AS = arm-none-eabi-as
GXX = arm-none-eabi-g++
LD = arm-none-eabi-gcc
GENZEHN = genzehn


NDLESS-INCLUDE = /mnt/c/Users/usertemp/Desktop/TI/Ndless/ndless-sdk/include


GCCFLAGS = -Wall -W -marm -ffreestanding -nostdlib -lgcc -Os -fPIE -g  -I $(NDLESS-INCLUDE) -mcpu=arm926ej-s -march=armv5tej
# -march=armv5tej-s
# ignore unused things to clean warnings up
GCCFLAGS +=  -Wno-unused-function -Wno-unused-variable -Wno-unused-const-variable

ifeq (DEBUG, TRUE)
	GCCFLAGS += -O0
endif

LDFLAGS = -nostdlib -static-pie -T ldscript.ld -static-libgcc -lgcc
ZEHNFLAGS = --name "osext"  --author "nspiredev500" --version "30" --240x320-support true --uses-lcd-blit false

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
	cp osext.elf osext-nodebug.elf && \
	arm-none-eabi-strip --strip-debug osext-nodebug.elf && \
	$(GENZEHN) $(ZEHNFLAGS) --input osext-nodebug.elf --output osext.tns && \
	rm osext-nodebug.elf


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

















