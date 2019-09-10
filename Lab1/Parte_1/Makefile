CC := gcc
CFLAGS := -Wall -Wextra -Wshadow -Wunreachable-code -g -pedantic -save-temps

ls: ls.c
	echo "Compilo ls_propio"
	${CC} ${CFLAGS} ls.c -o builds/ls_propio

cp: cp.c
	echo "Compilo cp_propio"
	${CC} ${CFLAGS} cp.c -o builds/cp_propio
	
clean:
	rm  *.i *.o *.s builds/*
