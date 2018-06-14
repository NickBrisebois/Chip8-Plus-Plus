CC = g++
CC_FLAGS = -std=c++11 -pedantic
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lGL -lsfml-audio

FILES = src/*.cpp libs/imgui/*.cpp libs/imgui-sfml/*.cpp
OUT_EXE = chip8

build: $(FILES)
	$(CC) $(CC_FLAGS) -o $(OUT_EXE) $(FILES) $(LDFLAGS)

clean:
	rm -f *.o core *.exe *~ *.out *.stackdump

rebuild: clean build
