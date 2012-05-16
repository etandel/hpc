IDIR =.
CC=gcc
CFLAGS=-I$(IDIR) -O0 -pg -lgomp

ODIR=obj
LDIR=../lib

LIBS=-lm

_DEPS = pools.h population.h config.h town.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ_SEQ = seq_main.o seq_pools.o seq_population.o seq_town.o 
OBJ_SEQ = $(patsubst %,$(ODIR)/%,$(_OBJ_SEQ))

_OBJ_PAR = par_main.o par_pools.o par_population.o par_town.o 
OBJ_PAR = $(patsubst %,$(ODIR)/%,$(_OBJ_PAR))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean all profile

all:
	make sequential
	make parallel

sequential: $(OBJ_SEQ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

parallel: $(OBJ_PAR)
	$(CC) -o $@ $^ -fopenmp $(CFLAGS) $(LIBS)

clean:
	rm -f $(ODIR)/*.o *~ sequential parallel gmon.out $(INCDIR)/*~ 

config:
	mkdir $(ODIR)
