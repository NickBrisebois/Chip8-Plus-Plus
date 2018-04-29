CC = g++
CC_FLAGS = -std=c++11
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

FILES = src/*.cpp
OUT_EXE = chip8

build: $(FILES)
	$(CC) $(CC_FLAGS) -o $(OUT_EXE) $(FILES) $(LDFLAGS)

clean:
	rm -f *.o core *.exe *~ *.out *.stackdump

rebuild: clean build
