CC := gcc
CFLAGS := -g -O2 -std=c99 -Wall -Wextra -pedantic -Wformat=2

recode: recode57.c
	@echo " "
	@echo "Compilo recode57"
	${CC} ${CFLAGS} recode57.c -o builds/recode57
	cp builds/recode57 recode_samples/
	@echo " "

clean:
	rm  *.i *.o *.s builds/*
