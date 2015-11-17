
# Use ?= to allow overriding from the env or command-line
CXX ?=		g++
CXXFLAGS ?=	-O3
PREFIX ?=	./stage
STRIP_CMD ?=	strip
INSTALL ?=	install -c
MKDIR ?=	mkdir -p

# Required flags that we shouldn't override
CXXFLAGS +=	-D_FILE_OFFSET_BITS=64

OBJS =	Fasta.o FastaHack.o split.o disorder.o

all:	fastahack

fastahack: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o fastahack

FastaHack.o: Fasta.h FastaHack.cpp
	$(CXX) $(CXXFLAGS) -c FastaHack.cpp

Fasta.o: Fasta.h Fasta.cpp
	$(CXX) $(CXXFLAGS) -c Fasta.cpp

split.o: split.h split.cpp
	$(CXX) $(CXXFLAGS) -c split.cpp

disorder.o: disorder.c disorder.h
	$(CXX) $(CXXFLAGS) -c disorder.c

install: fastahack
	$(MKDIR) $(DESTDIR)$(PREFIX)/bin
	$(INSTALL) fastahack $(DESTDIR)$(PREFIX)/bin

install-strip: install
	$(STRIP_CMD) $(DESTDIR)$(PREFIX)/bin/fastahack

clean:
	rm -rf fastahack *.o stage

.PHONY: clean
