OBJECTS=scan.o vdu.o window.o
CFLAGS=-I/usr/X11R6/include
LDFLAGS=-L/usr/X11R6/lib
vdu: $(OBJECTS)
	cc $(LDFLAGS) -lX11 -Wall -o $@ $^
clean:
	rm -f vdu $(OBJECTS)



