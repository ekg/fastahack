fastahack: Fasta.h Fasta.cpp FastaHack.cpp
	g++ Fasta.cpp FastaHack.cpp -o fastahack

clean:
	rm fastahack

.PHONY: clean
