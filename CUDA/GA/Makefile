IDIR =.

CC=gcc
H_CFLAGS=-I$(IDIR) -O0

D_CFLAGS=-I$(IDIR) -DCUDA

ODIR=obj
LDIR=../lib

LIBS=-lm

_DEPS = pools.h population.h config.h town.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ_SEQ = seq_main.o seq_pools.o seq_population.o seq_town.o 
OBJ_SEQ = $(patsubst %,$(ODIR)/%,$(_OBJ_SEQ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(H_CFLAGS)

.PHONY: clean all config

all:
	make sequential
	make parallel

sequential: $(OBJ_SEQ)
	$(CC) -o $@ $^ $(H_CFLAGS) $(LIBS)

parallel: $(OBJ_PAR)
	nvcc -o $@ parallel.cu $(D_CFLAGS) $(LIBS)

clean:
	rm -f $(ODIR)/*.o *~ sequential parallel gmon.out $(INCDIR)/*~ 

config:
	chmod +x benchmark.sh
	[ -d $(ODIR) ] || mkdir $(ODIR)
