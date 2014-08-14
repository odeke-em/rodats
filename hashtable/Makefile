CC := gcc
FLAGS := -Wall -Wextra
THREAD_LIB := -pthread

pseudoRandGen:	hashtable.o  pseudoRandGen.c
	    $(CC) $(FLAGS) hashtable.o pseudoRandGen.c -o pRand

testHash:     hashtable.o  testHash.c
	    $(CC) $(FLAGS) testHash.c hashtable.o -o testHash

wordTransition.o:   hashtable.o wordTransition.c wordTransition.h
	    $(CC) $(FLAGS) -c wordTransition.c -o wordTransition.o

hashtable.o:   hashTable.c hashTable.h
	    $(CC) $(FLAGS) -c hashTable.c -o hashtable.o

clean:	  
	    rm -f *.o
