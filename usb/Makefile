default: usb

usb.o: usb.c
	gcc -c usb.c -o usb.o -lm

usb: usb.o
	gcc usb.o -o usb -lm

clean:
	-rm -f usb.o
	-rm -f usb
