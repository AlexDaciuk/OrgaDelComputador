CC := gcc
CFLAGS := -g -O2 -std=c99 -Wall -Wextra -pedantic -Wformat=2

ls: ls.c
	@echo " "
	@echo "Compilo ls57"
	${CC} ${CFLAGS} ls.c -o builds/ls57
	@echo " "

cp: cp.c
	@echo " "
	@echo "Compilo cp57"
	${CC} ${CFLAGS} cp.c -o builds/cp57
	@echo " "

clean:
	rm  *.i *.o *.s builds/*
