




all: ascii.c
	

ascii.c: pngtoascii.java
	java pngtoascii kernel_font.png && \
	cp ascii.c ../kernel/ascii.c


pngtoascii.java:
	javac pngtoascii.java



.PHONY: clean
	


clean:
	rm pngtoascii.class ascii.c










