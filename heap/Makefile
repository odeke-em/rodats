CC := gcc

loadq.o:  loadQ.h loadQ.c heap.o
	$(CC) loadQ.c heap.o -o loadq -pthread

pqueue:	pQueue.c pQueue.h heap.o parcel.o Makefile
	$(CC) -c pQueue.c heap.o -o pqueue.o

parcel.o:	parcel.c parcel.h
	$(CC) -c parcel.c -o parcel.o

heap.o:	heap.c heap.h
	$(CC) -c heap.c -o heap.o

clean:	
	rm -f heap *.o
