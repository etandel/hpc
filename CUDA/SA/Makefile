IDIR =.

H_CC=gcc
H_DEBUG=
H_CFLAGS=-I$(IDIR) -Wall -O0

D_CC=nvcc
D_DEBUG=
D_CFLAGS=-DCUDA

ODIR=obj

LDIR=
LIBS=-lm

_DEPS = town.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ_SEQ = seq_main.o seq_town.o seq_tour.o seq_pools.o
OBJ_SEQ = $(patsubst %,$(ODIR)/%,$(_OBJ_SEQ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean all configure

all:
	make sequential
	make parallel

sequential: $(OBJ_SEQ)
	$(H_CC) -o $@ $^ $(H_CFLAGS) $(H_DEBUG) $(LDIR) $(LIBS)

parallel:
	$(D_CC) -o $@ parallel.cu $(D_CFLAGS) $(D_DEBUG) $(LDIR) $(LIBS)

clean:
	rm -f $(ODIR)/*.o *~ sequential parallel $(IDIR)/*~ 

config:
	chmod +x benchmark.sh
	[ -d $(ODIR) ] || mkdir $(ODIR)
