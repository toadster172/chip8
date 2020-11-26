chip8: main.c chip8.c opcode.c sdlFunctions.c
	clang -o chip8 main.c chip8.c opcode.c sdlFunctions.c -I `sdl2-config --cflags --libs`
clean:
	rm ./chip8
