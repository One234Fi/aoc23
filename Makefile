#TODO: I should stop being lazy and automate this (or be lazier?)

OBJS := day1, day2

.PHONY: all dir clean

all: dir bin/day1 bin/day2 bin/day3 bin/day4 bin/day5 bin/day6

dir:
	mkdir -p bin

bin/day1: day1/main.c
	gcc -g -o $@ $^

bin/day2: day2/main.c
	gcc -g -o $@ $^

bin/day3: day3/main.c
	gcc -g -o $@ $^

bin/day4: day4/main.c
	gcc -g -Wall -Werror -o $@ $^

bin/day5: day5/main.c
	gcc -g -Wall -Werror -o $@ $^ -lpthread

bin/day6: day6/main.c
	gcc -g -Wall -Werror -o $@ $^ 

clean:
	rm -r bin
