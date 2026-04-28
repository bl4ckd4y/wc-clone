CC      = gcc
CFLAGS  = -Wall -Wextra -Werror -std=c11 -O2
TARGET  = wc
SRC     = src/wc.c

.PHONY: all clean test asan test-asan

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

test: $(TARGET)
	@bash tests/run_tests.sh

clean:
	rm -f $(TARGET)

asan: CFLAGS = -Wall -Wextra -Werror -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer
asan: clean $(TARGET)

test-asan: asan
	@bash tests/run_tests.sh