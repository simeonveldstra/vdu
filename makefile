OBJECTS=scan.o vdu.o window.o
CFLAGS=-I/usr/X11R6/include -Wall
LDFLAGS=-L/usr/X11R6/lib -lm -lX11

vdu: $(OBJECTS)
	cc $(LDFLAGS) -o $@ $^

clean:
	rm -f vdu $(OBJECTS)

test:
	sh ./test.sh


