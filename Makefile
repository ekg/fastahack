CXX=g++
CFLAGS=-O3 -D_FILE_OFFSET_BITS=64

fastahack: Fasta.o FastaHack.cpp split.o disorder.o
	$(CXX) $(CFLAGS) Fasta.o FastaHack.cpp split.o disorder.o -o fastahack

Fasta.o: Fasta.h Fasta.cpp
	$(CXX) $(CFLAGS) -c Fasta.cpp

split.o: split.h split.cpp
	$(CXX) $(CFLAGS) -c split.cpp

disorder.o: disorder.c disorder.h
	$(CXX) $(CFLAGS) -c disorder.c

clean:
	rm -f fastahack *.o

.PHONY: clean
