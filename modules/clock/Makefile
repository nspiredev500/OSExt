DEBUG = FALSE


build_dir_raw ?= ../build
build_dir = $(build_dir_raw)/modules/clock
bin_dir ?= ../bin


#GCC = nspire-gcc
#AS  = nspire-as
#GXX = nspire-g++
#LD  = nspire-ld
#OBJCOPY = arm-none-eabi-objcopy
#STRIP = arm-none-eabi-strip
#GCC = arm-none-eabi-gcc
#AS = arm-none-eabi-as
#GXX = arm-none-eabi-g++
#LD = arm-none-eabi-gcc
#OBJCOPY = arm-none-eabi-objcopy
#STRIP = arm-none-eabi-strip
GCC = arm-osext-gcc
AS = arm-osext-as
GXX = arm-osext-g++
LD = arm-osext-gcc
OBJCOPY = arm-osext-objcopy
STRIP = arm-osext-strip


NDLESS-INCLUDE = /mnt/c/Users/usertemp/Desktop/TI/Ndless/ndless-sdk/include


GCCFLAGS = -Wall -W -marm -ffreestanding -nostdlib -lgcc -Os -fPIE -g  -I $(NDLESS-INCLUDE) -mcpu=arm926ej-s -march=armv5tej
# -march=armv5tej-s
# ignore unused things to clean warnings up
GCCFLAGS +=  -Wno-unused-function -Wno-unused-variable -Wno-unused-const-variable -Wno-unused-parameter -Wno-unused-but-set-variable -Wno-implicit-fallthrough



ifeq (DEBUG, TRUE)
	GCCFLAGS += -O0
endif

ifdef (RELEASE_KERNEL)
	GCCFLAGS += -Os
endif


#-Xlinker -m -Xlinker armelf_osext
LDFLAGS = -marm -mcpu=arm926ej-s -march=armv5tej -nostdlib -static-pie -T $(CURDIR)/ldscript.ld -static-libgcc -lgcc

OBJS = $(patsubst %.c, %.o, $(shell find . -depth -name \*.c -printf "$(build_dir)/%f "))
OBJS += $(patsubst %.cpp, %.o, $(shell find . -depth -name \*.cpp -printf "$(build_dir)/%f "))
OBJS += $(patsubst %.S, %.o, $(shell find . -depth -name \*.S -printf "$(build_dir)/%f "))
DISTDIR = .

HEADERS = $(patsubst %.c, %.o, $(shell find . -depth -name \*.h))

#$(info $$CURDIR is [${CURDIR}])


all: 
	


HEADERS: ;

.PHONY: clean




clean:
	-rm -f $(OBJS) 



	







$(build_dir)/%.o: $(CURDIR)/%.c $(HEADERS)
	$(GCC) $(GCCFLAGS) -c $< -o $@


$(build_dir)/%.o: $(CURDIR)/%.cpp $(HEADERS)
	$(GXX) $(GCCFLAGS) -c $< -o $@


$(build_dir)/%.o: $(CURDIR)/%.S 
	$(AS) -c $< -o $@
















