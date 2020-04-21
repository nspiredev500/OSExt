DEBUG = FALSE


build_dir_raw ?= ../build
build_dir = $(build_dir_raw)/libc
bin_dir ?= ../bin

sysroot = /mnt/d/toolchain/debian/sysroot







GCC = arm-osext-gcc
AS = arm-osext-as
GXX = arm-osext-g++
LD = arm-osext-gcc
OBJCOPY = arm-osext-objcopy
STRIP = arm-osext-strip



GCCFLAGS = -nostartfiles -nodefaultlibs -Wall -W -marm -Os -g -mcpu=arm926ej-s -march=armv5tej -nostdinc -I include
# -march=armv5tej-s
# ignore unused things to clean warnings up
#GCCFLAGS +=  -Wno-unused-function -Wno-unused-variable -Wno-unused-const-variable -Wno-unused-parameter -Wno-unused-but-set-variable -Wno-implicit-fallthrough



LDFLAGS = -nostartfiles -nodefaultlibs -march=armv5tej -static-libgcc

OBJS = $(patsubst %.c, %.o, $(shell find . -depth -name \*.c -printf "$(build_dir)/%f "))
OBJS += $(patsubst %.cpp, %.o, $(shell find . -depth -name \*.cpp -printf "$(build_dir)/%f "))
DISTDIR = .

HEADERS = $(patsubst %.c, %.o, $(shell find . -depth -name \*.h))
KERNEL_HEADERS += $(patsubst %.c, %.o, $(shell find ../kernel/include -depth -name \*.h))



all: $(build_dir)/libc.a $(build_dir)/debug-libc.a crt




.PHONY: clean install-headers crt


install-headers: kernel_headers
	cp -r include $(sysroot)/usr





kernel_headers: $(KERNEL_HEADERS)
	cp -r ../kernel/include/sys include




install: all
	cp $(build_dir)/libc.a $(sysroot)/usr/lib
	cp $(build_dir)/debug-libc.a $(sysroot)/usr/lib
	cp $(build_dir)/crt0.o $(sysroot)/usr/lib
	cp $(build_dir)/crti.o $(sysroot)/usr/lib
	cp $(build_dir)/crtn.o $(sysroot)/usr/lib







crt: $(build_dir)/crt0.o $(build_dir)/crti.o $(build_dir)/crtn.o
	


$(build_dir)/libc.a: $(OBJS) 
	arm-osext-ar rcs $(build_dir)/libc.a $(OBJS) 


clean:
	-rm -f $(build_dir)/libc.a $(build_dir)/debug-libc.a $(build_dir)/*.o



$(build_dir)/debug-libc.a: $(build_dir)/libc.a
	cd $(build_dir) && \
	$(OBJCOPY) --only-keep-debug libc.a debug-libc.a && \
	arm-osext-strip --strip-debug libc.a




$(build_dir)/%.o: $(CURDIR)/stdio/%.c $(HEADERS)
	$(GCC) $(GCCFLAGS) -c $< -o $@


$(build_dir)/%.o: $(CURDIR)/stdlib/%.c $(HEADERS)
	$(GCC) $(GCCFLAGS) -c $< -o $@

$(build_dir)/%.o: $(CURDIR)/sys/%.c $(HEADERS)
	$(GCC) $(GCCFLAGS) -c $< -o $@











$(build_dir)/%.o: $(CURDIR)/%.c $(HEADERS)
	$(GCC) $(GCCFLAGS) -c $< -o $@


$(build_dir)/%.o: $(CURDIR)/%.cpp $(HEADERS)
	$(GXX) $(GCCFLAGS) -c $< -o $@


$(build_dir)/%.o: $(CURDIR)/%.S $(HEADERS)
	$(AS) -c $< -o $@


$(build_dir)/%.o: $(CURDIR)/%.s
	$(AS) -c $< -o $@










