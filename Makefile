build_dir ?= ../build
build_dir_raw = $(build_dir)
bin_dir ?= ../bin




all: installer kernel uninstaller loader modules



.PHONY: installer kernel remake charset uninstaller cleanbuild loader release modules
	


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


release: charset installer uninstaller loader
	$(MAKE) -C kernel release
	cp bin/osext.tns bin/installer/boot/osext.tns


kernel: charset
	$(MAKE) -C kernel
	cp bin/osext.tns bin/installer/boot/osext.tns



clean:
	$(MAKE) -C installer clean && \
	$(MAKE) -C kernel clean && \
	$(MAKE) -C pngtoascii clean && \
	$(MAKE) -C loader clean && \
	$(MAKE) -C modules clean && \
	$(MAKE) -C uninstaller clean
	-rm bin/installer/boot/osext.tns bin/osext.tns bin/osextinstaller.tns bin/osextloader.tns bin/uninstall_osext.tns

cleanbuild:


remake:
	$(MAKE) clean && \
	$(MAKE) all
