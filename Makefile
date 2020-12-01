CC = cc
CFLAGS = -I `sdl2-config --cflags --libs`
ODIR=obj
DEPS = chip8.h chip8Type.h opcode.h sdlFunctions.h
_OBJ = main.o chip8.o opcode.o sdlFunctions.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
default: chip8

$(OBJ): $(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
chip8: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
clean:
	rm -rf $(ODIR)/*
	rm ./chip8
