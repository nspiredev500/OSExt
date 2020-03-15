



all: installer kernel uninstaller
	cp kernel/osext.tns installer/installer/boot/osext.tns
	

.PHONY: installer kernel remake charset
	

installer:
	$(MAKE) -C installer


uninstaller:
	$(MAKE) -C uninstaller


charset:
	$(MAKE) -C pngtoascii



kernel: charset
	$(MAKE) -C kernel



clean:
	$(MAKE) -C installer clean && \
	$(MAKE) -C kernel clean && \
	$(MAKE) -C pngtoascii clean && \
	$(MAKE) -C uninstaller clean && \
	rm installer/installer/boot/osext.tns


remake:
	$(MAKE) clean && \
	$(MAKE) all
