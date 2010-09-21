fastahack: Fasta.h Fasta.cpp FastaHack.cpp Split.o
	g++ Fasta.cpp FastaHack.cpp Split.o -o fastahack

Split.o: Split.h Split.cpp
	g++ -c Split.cpp

clean:
	rm fastahack *.o

.PHONY: clean
