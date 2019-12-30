# 0 (default): no debugging flags; build for optimization
# 1: build with debugging info
# 2: build with debugging info, undefined behavior sanitizer and TONS of extra warnings
DEBUG := 0

ifeq ($(DEBUG), 2)
CFLAGS := -ggdb -Wall -Wextra -fsanitize=undefined -Wno-idiomatic-parentheses -Wno-pointer-sign -Wno-dangling-else
else ifeq ($(DEBUG), 1)
CFLAGS := -ggdb
else
CFLAGS := -O3 -flto -march=native -mtune=native
endif

all: ddump gbafhash

clean:
	rm -rf ddump
	rm -rf gbafhash

ddump: datadump/*.c
	$(CC) $(CFLAGS) $^ -o $@

gbafhash: funchash/*.c
	$(CC) $(CFLAGS) $^ -o $@
