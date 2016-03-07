CC=gcc
CFLAGES = -I.
DEPS = queue.h sequence.h file.h jwHash.h math.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

WAM_bin : main.o jwHash.o
	$(CC) -o WAM_bin main.o jwHash.o -I.
