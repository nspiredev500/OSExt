build_dir_raw ?= ../build
build_dir = $(build_dir_raw)/pngtoascii
bin_dir = ../bin



all: $(bin_dir)/charset.tns


$(bin_dir)/charset.tns: $(build_dir)/pngtoascii.class
	cp kernel_font.png $(build_dir)/kernel_font.png && \
	cd $(build_dir) && \
	java pngtoascii kernel_font.png && \
	cd ../../pngtoascii && \
	cp $(build_dir)/charmap $(bin_dir)/charset.tns


$(build_dir)/pngtoascii.class: pngtoascii.java
	javac -d $(build_dir) pngtoascii.java



.PHONY: clean cleanbuild
	


clean:
	-rm $(build_dir)/pngtoascii.class $(build_dir)/charset $(build_dir)/kernel_font.png $(bin_dir)/charset.tns


cleanbuild: clean
	






