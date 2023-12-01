
OBJS := day1

.PHONY: all dir clean

all: dir bin/day1

dir:
	mkdir -p bin

bin/day1: day1/main.c
	gcc -o $@ $^

clean:
	rm -r bin
