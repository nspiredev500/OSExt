build_dir ?= ../build
build_dir_raw = $(build_dir)
bin_dir ?= ../bin




all: installer kernel uninstaller loader modules bin/installer/boot/osext.tns



.PHONY: installer kernel remake charset uninstaller cleanbuild loader release modules clean_charset configure
	

config/config: config/config.c
	cd config && \
	$(CC) config.c -o config


configure: config/config
	cd config && \
	./config

config.h: config/config
	cd config && \
	./config


modules:
	$(MAKE) -C modules


installer:
	$(MAKE) -C installer

loader:
	$(MAKE) -C loader

uninstaller:
	$(MAKE) -C uninstaller


charset:
	$(MAKE) -C pngtoascii

clean_charset:
	$(MAKE) -C pngtoascii clean


bin/installer/boot/osext.tns: bin/osext.tns
	cp bin/osext.tns bin/installer/boot/osext.tns


release:  installer uninstaller loader
	$(MAKE) -C kernel release


kernel: config.h
	$(MAKE) -C kernel



clean:
	$(MAKE) -C installer clean && \
	$(MAKE) -C kernel clean && \
	$(MAKE) -C loader clean && \
	$(MAKE) -C modules clean && \
	$(MAKE) -C uninstaller clean
	-rm bin/installer/boot/osext.tns bin/osext.tns bin/osextinstaller.tns bin/osextloader.tns bin/uninstall_osext.tns

cleanbuild:


remake:
	$(MAKE) clean && \
	$(MAKE) all
