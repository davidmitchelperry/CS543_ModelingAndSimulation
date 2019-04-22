CC =  /opt/csw/gcc3/bin/gcc
LDFLAGS =
CFLAGS =
CPPFLAGS =
MAKE = make

all: p3 

lib/csim.a:
	(cd lib && $(MAKE))

p3: p3.c lib/csim.a
	$(CC) -Ilib $(LDFLAGS) $(CFLAGS) $(CPPFLAGS) -o p3 p3.c lib/csim.a -lm
clean:
	rm -f p3 
