CFLAGS := -m32 -g -Og -std=c99 -Wall -Wextra
CFLAGS += -fno-pic -fno-omit-frame-pointer -no-pie
ASFLAGS = $(CFLAGS)

ASMS := $(wildcard *.S)
SRCS := $(wildcard *.c)

PROG := $(ASMS:%.S=%)
PROG += $(ASMS:%.c=%)

all: $(PROG)

clean:
	rm -f $(PROG) *.o core

.PHONY: clean all
