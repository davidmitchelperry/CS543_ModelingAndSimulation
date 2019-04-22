CC =  /opt/csw/gcc3/bin/gcc
LDFLAGS =
CFLAGS =
CPPFLAGS =
MAKE = make

all: p1 

lib/csim.a:
	(cd lib && $(MAKE))

p1: p1.c lib/csim.a
	$(CC) -Ilib $(LDFLAGS) $(CFLAGS) $(CPPFLAGS) -o p1 p1.c lib/csim.a -lm
clean:
	rm -f p1 
