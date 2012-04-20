IDIR =.
CC=gcc
DEBUG=
CFLAGS=-I$(IDIR) -Wall -O0 -fopenmp

ODIR=obj
LDIR=../lib

LIBS=-lm

_DEPS = town.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ_SEQ = seq_main.o seq_town.o seq_tour.o seq_pools.o
OBJ_SEQ = $(patsubst %,$(ODIR)/%,$(_OBJ_SEQ))

_OBJ_PAR = par_main.o par_town.o par_tour.o par_pools.o
OBJ_PAR = $(patsubst %,$(ODIR)/%,$(_OBJ_PAR))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean all

all:
	make sequential
	make parallel

sequential: $(OBJ_SEQ)
	$(CC) -o $@ $^ $(CFLAGS) $(DEBUG) $(LIBS)

parallel: $(OBJ_PAR)
	$(CC) -o $@ $^ $(CFLAGS) $(DEBUG) $(LIBS)

clean:
	rm -f $(ODIR)/*.o *~ sequential parallel $(INCDIR)/*~ 
