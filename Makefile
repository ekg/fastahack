
# Use ?= to allow overriding from the env or command-line
CXX ?=		g++
CFLAGS ?=	-O3
PREFIX ?=	./stage

# Required flags that we shouldn't override
CFLAGS +=	-D_FILE_OFFSET_BITS=64

OBJS =	Fasta.o FastaHack.o split.o disorder.o

all:	fastahack

fastahack: $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o fastahack

FastaHack.o: Fasta.h FastaHack.cpp
	$(CXX) $(CFLAGS) -c FastaHack.cpp

Fasta.o: Fasta.h Fasta.cpp
	$(CXX) $(CFLAGS) -c Fasta.cpp

split.o: split.h split.cpp
	$(CXX) $(CFLAGS) -c split.cpp

disorder.o: disorder.c disorder.h
	$(CXX) $(CFLAGS) -c disorder.c

install: fastahack
	install -d ${DESTDIR}${PREFIX}/bin
	install -c fastahack ${DESTDIR}${PREFIX}/bin

clean:
	rm -rf fastahack *.o stage

.PHONY: clean
