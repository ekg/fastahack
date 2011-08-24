fastahack: Fasta.o FastaHack.cpp split.o disorder.o
	g++ Fasta.o FastaHack.cpp split.o disorder.o -o fastahack

Fasta.o: Fasta.h Fasta.cpp
	g++ -c Fasta.cpp

split.o: split.h split.cpp
	g++ -c split.cpp

disorder.o: disorder.c disorder.h
	g++ -c disorder.c

clean:
	rm -f fastahack *.o

.PHONY: clean
