CC =  /opt/csw/gcc3/bin/gcc
LDFLAGS =
CFLAGS =
CPPFLAGS =
MAKE = make

all: p2 

lib/csim.a:
	(cd lib && $(MAKE))

p2: p2.c lib/csim.a
	$(CC) -Ilib $(LDFLAGS) $(CFLAGS) $(CPPFLAGS) -o p2 p2.c lib/csim.a -lm
clean:
	rm -f p2 
