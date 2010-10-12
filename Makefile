fastahack: Fasta.h Fasta.cpp FastaHack.cpp split.o disorder.o
	g++ Fasta.cpp FastaHack.cpp split.o disorder.o -o fastahack

split.o: split.h split.cpp
	g++ -c split.cpp

disorder.o: disorder.c disorder.h
	g++ -c disorder.c

clean:
	rm -f fastahack *.o

.PHONY: clean
