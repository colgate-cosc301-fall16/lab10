CC=clang
CFLAGS=-g -Wall -D_GNU_SOURCE
LDFLAGS=-g -Wall

diskperf: diskperf.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o diskperf

