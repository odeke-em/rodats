FLAGS := -Wall -Wextra
RM := rm -f

main: main.c array.o

%.o:	%.c
	$(CC) -c $^ -o $@

%:	%.o %.c
	$(CC) $^ -o $@

clean:
	$(RM) *.o
