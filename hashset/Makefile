CC := gcc
FLAGS := -Wall -Wextra
LIBS := -pthread

%.o:	%.c
	$(CC) $(FLAGS) $< -c -o $@ $(LIBS)

%:	%.c %.o
	$(CC) $(FLAGS) $< -o $@

useCase:	DMap.o HashMap.o DNode.o RTrie.o MBuf.o
Test:	HashSet.o DMap.o HashMap.o DNode.o RTrie.o
HashMap:	HashMap.c RTrie.o
HashSet.o:	DMap.o HashSet.c
clean:	
	rm -f *.o
