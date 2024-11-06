CC=gcc
CFLAGS=-Wall -Wextra -g #-fsanitize=address,undefined

LOAD=load_balancer
SERVER=server
CACHE=lru_cache
UTILS=utils
LAB_IMPLEM=lab_implem

# Add new source file names here:
# EXTRA=<extra source file name>

.PHONY: build clean

build: tema2

tema2: main.o $(LOAD).o $(SERVER).o $(CACHE).o $(UTILS).o $(LAB_IMPLEM).o # $(EXTRA).o
	$(CC) $(CFLAGS) $^ -o $@

main.o: main.c
	$(CC) $(CFLAGS) $^ -c

$(LOAD).o: $(LOAD).c $(LOAD).h
	$(CC) $(CFLAGS) $^ -c

$(SERVER).o: $(SERVER).c $(SERVER).h
	$(CC) $(CFLAGS) $^ -c

$(CACHE).o: $(CACHE).c $(CACHE).h
	$(CC) $(CFLAGS) $^ -c

$(UTILS).o: $(UTILS).c $(UTILS).h
	$(CC) $(CFLAGS) $^ -c

$(LAB_IMPLEM).o: $(LAB_IMPLEM).c $(LAB_IMPLEM).h
	$(CC) $(CFLAGS) $^ -c

# $(EXTRA).o: $(EXTRA).c $(EXTRA).h
# 	$(CC) $(CFLAGS) $^ -c

clean:
	rm -f *.o tema2 *.h.gch
