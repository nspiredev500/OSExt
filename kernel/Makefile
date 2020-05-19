DEBUG = FALSE


build_dir_raw ?= ../build
build_dir = $(build_dir_raw)/kernel
bin_dir ?= ../bin

ifdef (RELEASE_KERNEL)
	build_dir = $(build_dir_raw)/release_kernel
endif

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
GENZEHN = genzehn


NDLESS-INCLUDE = /mnt/c/Users/usertemp/Desktop/TI/Ndless/ndless-sdk/include


GCCFLAGS = -Wall -W -marm -ffreestanding -nostdlib -lgcc -Os -fPIE -g  -I $(NDLESS-INCLUDE) -mcpu=arm926ej-s -march=armv5tej $(RELEASE_KERNEL)
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
LDFLAGS = -marm -mcpu=arm926ej-s -march=armv5tej -nostdlib -static-pie -T $(CURDIR)/ldscript.ld -static-libgcc -lgcc -z max-page-size=4096
ZEHNFLAGS = --name "osext"  --author "nspiredev500" --version "30" --240x320-support true --uses-lcd-blit false

OBJS = $(patsubst %.c, %.o, $(shell find . -depth -name \*.c -printf "$(build_dir)/%f "))
OBJS += $(patsubst %.cpp, %.o, $(shell find . -depth -name \*.cpp -printf "$(build_dir)/%f "))
OBJS += $(patsubst %.S, %.o, $(shell find . -depth -name \*.S -printf "$(build_dir)/%f "))
DISTDIR = .

HEADERS = $(patsubst %.c, %.o, $(shell find . -depth -name \*.h))

#$(info $$CURDIR is [${CURDIR}])


all: $(build_dir)/osext.tns  $(build_dir)/debug.elf
	


HEADERS: ;

.PHONY: clean cleanbuild remake release release_kernel cleanrelease




remake: clean
	$(MAKE) all


release: 
	$(MAKE) "RELEASE_KERNEL=-D RELEASE"


cleanrelease: 
	$(MAKE) "RELEASE_KERNEL=-D RELEASE" clean



$(build_dir)/osext.tns: $(build_dir)/osext.elf
	cd $(build_dir) && \
	cp osext.elf osext-nodebug.elf && \
	$(STRIP) --strip-debug osext-nodebug.elf && \
	$(GENZEHN) $(ZEHNFLAGS) --input osext-nodebug.elf --output osext.tns && \
	rm osext-nodebug.elf
	cp $(build_dir)/osext.tns $(bin_dir)/osext.tns



$(build_dir)/osext.elf: $(OBJS) 
	mkdir -p $(build_dir)/$(DISTDIR) && \
	$(LD) $^ -o $@ $(LDFLAGS) && \
	cp $(build_dir)/osext.elf $(bin_dir)/osext.elf.tns


clean:
	-rm -f $(OBJS) $(build_dir)/osext.elf $(build_dir)/debug.elf $(build_dir)/osext.tns


cleanbuild: clean
	


$(build_dir)/debug.elf: $(build_dir)/osext.elf
	cd $(build_dir) && \
	$(OBJCOPY) --only-keep-debug osext.elf debug.elf




$(build_dir)/%.o: $(CURDIR)/libk/%.c $(HEADERS)
	$(GCC) $(GCCFLAGS) -c $< -o $@


$(build_dir)/%.o: $(CURDIR)/drivers/%.c $(HEADERS)
	$(GCC) $(GCCFLAGS) -c $< -o $@


$(build_dir)/%.o: $(CURDIR)/charset/%.c $(HEADERS)
	$(GCC) $(GCCFLAGS) -c $< -o $@


$(build_dir)/%.o: $(CURDIR)/exceptions/%.c $(HEADERS)
	$(GCC) $(GCCFLAGS) -c $< -o $@



$(build_dir)/%.o: $(CURDIR)/syscalls/svc/%.c $(HEADERS)
	$(GCC) $(GCCFLAGS) -c $< -o $@



$(build_dir)/%.o: $(CURDIR)/syscalls/usr/%.c $(HEADERS)
	$(GCC) $(GCCFLAGS) -c $< -o $@



$(build_dir)/%.o: $(CURDIR)/scheduling/%.c $(HEADERS)
	$(GCC) $(GCCFLAGS) -c $< -o $@


$(build_dir)/%.o: $(CURDIR)/memory/%.c $(HEADERS)
	$(GCC) $(GCCFLAGS) -c $< -o $@


$(build_dir)/%.o: $(CURDIR)/gui/%.c $(HEADERS)
	$(GCC) $(GCCFLAGS) -c $< -o $@


$(build_dir)/%.o: $(CURDIR)/elf/%.c $(HEADERS)
	$(GCC) $(GCCFLAGS) -c $< -o $@















$(build_dir)/%.o: $(CURDIR)/%.c $(HEADERS)
	$(GCC) $(GCCFLAGS) -c $< -o $@


$(build_dir)/%.o: $(CURDIR)/%.cpp $(HEADERS)
	$(GXX) $(GCCFLAGS) -c $< -o $@


$(build_dir)/%.o: $(CURDIR)/%.S 
	$(AS) -c $< -o $@

















