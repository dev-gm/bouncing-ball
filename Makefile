IDIR=include
CC=gcc
CFLAGS=-I$(IDIR) -Wall -Wextra -Werror -pedantic -pedantic-errors -g -fopenmp

ODIR=obj
SDIR=src

LIBS=-lSDL2 -lSDL2_gfx -lSDL2_ttf

_OBJ=sim.o main.o 
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

EXE=out

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXE): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean run test bear

clean:
	rm -f $(ODIR)/*.o $(EXE)

run:
	./$(EXE)

test:
	make
	make run

bear:
	make clean
	bear -- make
