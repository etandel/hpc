IDIR =.

H_CC=gcc
H_DEBUG=
H_CFLAGS=-I$(IDIR) -Wall -O0

D_CC=nvcc
D_DEBUG=
D_CFLAGS=-DCUDA

H_ODIR=hobj
D_ODIR=dobj

LDIR=
LIBS=-lm

_DEPS = town.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ_SEQ = seq_main.o seq_town.o seq_tour.o seq_pools.o
OBJ_SEQ = $(patsubst %,$(H_ODIR)/%,$(_OBJ_SEQ))

_OBJ_PAR = par_main.o par_town.o par_tour.o par_pools.o
OBJ_PAR = $(patsubst %,$(D_ODIR)/%,$(_OBJ_PAR))

$(H_ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(D_ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean all configure

all:
	make sequential
	make parallel

sequential: $(OBJ_SEQ)
	$(H_CC) -o $@ $^ $(H_CFLAGS) $(H_DEBUG) $(LDIR) $(LIBS)

parallel: $(OBJ_PAR)
	$(D_CC) -o $@ $^ $(D_CFLAGS) $(D_DEBUG) $(LDIR) $(LIBS)

clean:
	rm -f $(H_ODIR)/*.o $(D_ODIR)/*.o *~ sequential parallel $(IDIR)/*~ 

config:
	chmod +x benchmark.sh
	[ -d $(H_ODIR) ] || mkdir $(H_ODIR)
	[ -d $(D_ODIR) ] || mkdir $(D_ODIR)
