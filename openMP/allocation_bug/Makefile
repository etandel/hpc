IDIR =.
CC=gcc
DEBUG=-ggdb
CFLAGS=-I$(IDIR) -Wall -O0

ODIR=obj
LDIR=../lib

LIBS=-lm

_DEPS = town.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ_SEQ = main.o town.o tour.o pools.o
OBJ_SEQ = $(patsubst %,$(ODIR)/%,$(_OBJ_SEQ))

_OBJ_PAR = main.o par_town.o par_tour.o par_pools.o
OBJ_PAR = $(patsubst %,$(ODIR)/%,$(_OBJ_PAR))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean all

sequential: $(OBJ_SEQ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

parallel: $(OBJ_PAR)
	$(CC) -o $@ $^ -fopenmp $(CFLAGS) $(LIBS)

all:
	make sequential
	make parallel

clean:
	rm -f $(ODIR)/*.o *~ sequential parallel $(INCDIR)/*~ 
