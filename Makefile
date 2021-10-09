CC = cc
WCC = x86_64-w64-mingw32-gcc
CFLAGS = `sdl2-config --cflags --libs` -lm
WCFLAGS = -I/usr/include/SDL2 -L./ -D_REENTRANT -lSDL2main -lSDL2
ODIR=obj
WODIR=wobj
DEPS = chip8.h chip8Type.h opcode.h sdlFunctions.h
_OBJ = main.o chip8.o opcode.o sdlFunctions.o
OBJ  = $(patsubst %,$(ODIR)/%,$(_OBJ))
WOBJ = $(patsubst %,$(WODIR)/%,$(_OBJ))
default: chip8

$(OBJ): $(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
$(WOBJ): $(WODIR)/%.o: %.c $(DEPS)
	$(WCC) -c -o $@ $< $(WCFLAGS)
chip8: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
chip8.exe: $(WOBJ)
	$(WCC) -o $@ $^ $(WCFLAGS)
clean:
	rm -rf $(ODIR)/*
	rm ./chip8
wclean:
	rm -rf $(WODIR)/*
	rm ./chip8.exe
