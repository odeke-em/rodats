CC := gcc
THREAD_LIB := -pthread

pseudoRandGen:	hashlist.o  pseudoRandGen.c
	    $(CC) hashlist.o pseudoRandGen.c -o pRand

Scrooge:    hlru.o Scrooge.c Scrooge.h hashlist.o mutexcondpair.o
	    $(CC) $(THREAD_LIB) hlru.o Scrooge.c mutexcondpair.o hashlist.o -o Scrooge
mutexcondpair.o:    MutexCondPair.h MutexCondPair.c
	    $(CC) $(THREAD_LIB) -c MutexCondPair.c -o mutexcondpair.o

hlru.o:	    hashlist.o HLRU.c HLRU.h
	    $(CC) -c HLRU.c hashlist.o -o hlru.o

testHash:     hashlist.o  testHash.c
	    $(CC) testHash.c hashlist.o -o testHash

wordTransition.o:   hashlist.o wordTransition.c wordTransition.h
	    $(CC) -c wordTransition.c -o wordTransition.o

hashlist.o:   hashList.c hashList.h
	    $(CC) -c hashList.c -o hashlist.o

clean:	  
	    rm -f *.o
