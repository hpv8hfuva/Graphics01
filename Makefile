.PHONEY: build, run

build: program

run: program
	./program $(file)

program: main.cpp
	clang++ -O3 -I. main.cpp lodepng.cpp -o program