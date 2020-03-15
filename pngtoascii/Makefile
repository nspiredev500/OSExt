




all: ascii.c
	

ascii.c: pngtoascii.class
	java pngtoascii kernel_font.png && \
	cp ascii.c ../kernel/ascii.c


pngtoascii.class:
	javac pngtoascii.java



.PHONY: clean
	


clean:
	rm pngtoascii.class ascii.c










