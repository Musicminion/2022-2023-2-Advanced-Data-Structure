.PHONY: build clean run gdb

BUILD := build
BIN := ./build/executable
TEST_NUM := 3

build: clean
	cmake -DTEST_NUM=$(TEST_NUM) -B $(BUILD)
	cmake --build $(BUILD)

clean:
	rm -rf $(BUILD)

run: build
	$(BIN)

gdb:
	gdb $(BIN)
