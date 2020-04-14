build_dir_raw ?= ../build
build_dir = $(build_dir_raw)/pngtoascii




all: $(CURDIR)/../kernel/charset/ascii.c
	

$(CURDIR)/../kernel/charset/ascii.c: $(build_dir)/pngtoascii.class
	cp kernel_font.png $(build_dir)/kernel_font.png && \
	(cd $(build_dir) && java pngtoascii kernel_font.png) && \
	cp $(build_dir)/ascii.c ../kernel/charset/ascii.c


$(build_dir)/pngtoascii.class:
	javac -d $(build_dir) pngtoascii.java



.PHONY: clean cleanbuild
	


clean:
	-rm $(build_dir)/pngtoascii.class $(build_dir)/ascii.c $(build_dir)/kernel_font.png


cleanbuild: clean
	






