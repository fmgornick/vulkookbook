all: *.o
	clang $^ -o vulkookbook

%.o: %.c
	clang -I/usr/local/include/vulkan -Wall -g -c $^

clean:
	rm -f *.o vulkookbook
