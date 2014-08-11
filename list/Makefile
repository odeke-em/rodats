CC := gcc

Scrooge:    LRU.o Scrooge.c list.o

LRU.o:	list.o LRU.c

list.o:	  list.c Makefile

%: %.c %.o
	$(CC) $< -o $@
%.o:	%.c
	$(CC) -c $< -o $@

clean:	  
	rm *.o Scrooge
