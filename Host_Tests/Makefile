# Host_Tests/Makefile.test

CC = gcc
CFLAGS = -Wall -Wextra -DTEST_HOST
TARGET = test_runner

SRC_C = ../Core/Src/util.c
GEN_HEADER = ../Core/Inc/util.h
HEADER_GEN_SCRIPT = HelperScripts/gen_util_h.py

SRCS = test_util.c $(SRC_C)

$(TARGET): $(SRCS) $(GEN_HEADER)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)
	@echo "✅ Build complete. Run 'make tests' to execute."


# Pass file paths as args to the script
$(GEN_HEADER): $(HEADER_GEN_SCRIPT) $(SRC_C)
	python3 $(HEADER_GEN_SCRIPT) $(SRC_C) $(GEN_HEADER)

tests: $(TARGET)
	./$(TARGET)

all: tests

clean:
	rm -f $(TARGET)
