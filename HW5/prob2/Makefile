CC =  /opt/csw/gcc3/bin/gcc
LDFLAGS =
CFLAGS =
CPPFLAGS =
MAKE = make

all: p1_reads p1_writes

lib/csim.a:
	(cd lib && $(MAKE))


p1_reads: p1_reads.c lib/csim.a
	$(CC) -Ilib $(LDFLAGS) $(CFLAGS) $(CPPFLAGS) -o p1_reads p1_reads.c lib/csim.a -lm

p1_writes: p1_writes.c lib/csim.a
	$(CC) -Ilib $(LDFLAGS) $(CFLAGS) $(CPPFLAGS) -o p1_writes p1_writes.c lib/csim.a -lm
clean:
	rm -f p1 
