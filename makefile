FLAGS=-std=c99 -Wall -g -c
LIBS=-lvulkan -lSDL3

all: $(patsubst %.c,%.o,$(wildcard *.c))
	clang -o vulkookbook $^ $(LIBS)

main.o: main.c
	clang $(FLAGS) $<

%.o: %.c %.h
	clang $(FLAGS) $<

clean:
	rm -rf *.o vulkookbook
