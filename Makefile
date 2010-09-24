fastahack: Fasta.h Fasta.cpp FastaHack.cpp Split.o disorder.o
	g++ Fasta.cpp FastaHack.cpp Split.o disorder.o -o fastahack

Split.o: Split.h Split.cpp
	g++ -c Split.cpp

disorder.o: disorder.c disorder.h
	g++ -c disorder.c

clean:
	rm -f fastahack *.o

.PHONY: clean
