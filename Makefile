DAYS := $(addprefix day, 1 2 3 4 5 6 7)
OBJS := $(addprefix bin/, $(DAYS))

CFLAGS := -g -Wall -Werror
LDFLAGS := -lpthread

.PHONY: all dir clean

all: dir $(OBJS)

dir:
	mkdir -p bin

all: $(OBJS)

$(OBJS):
	gcc $(CFLAGS) -o $@ $(addsuffix /main.c, $(notdir $@)) $(LDFLAGS)


clean:
	rm -r bin
