



all: installer kernel 
	cp kernel/osext.tns installer/installer/boot/osext.tns
	

.PHONY: installer kernel remake charset
	

installer:
	$(MAKE) -C installer




charset:
	$(MAKE) -C pngtoascii



kernel: charset
	$(MAKE) -C kernel



clean:
	$(MAKE) -C installer clean && \
	$(MAKE) -C kernel clean && \
	$(MAKE) -C pngtoascii clean && \
	rm installer/installer/boot/osext.tns


remake:
	$(MAKE) clean && \
	$(MAKE) all
