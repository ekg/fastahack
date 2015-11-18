
# Use ?= to allow overriding from the env or command-line.
# Many of these variables are automatically set by package managers
# and can easily be controlled by the package maintainer.

CXX ?=		c++
CXXFLAGS ?=	-O3
DESTDIR ?=	stage
PREFIX ?=	/usr/local
STRIP_CMD ?=	strip
INSTALL ?=	install -c
MKDIR ?=	mkdir -p
AR ?=		ar

# Required flags that we shouldn't override
CXXFLAGS +=	-D_FILE_OFFSET_BITS=64

BIN =	fastahack
LIB =	libfastahack.a
OBJS =	Fasta.o split.o disorder.o
MAIN =	FastaHack.o

all:	$(BIN) $(LIB)

$(BIN): $(OBJS) $(MAIN)
	$(CXX) $(CXXFLAGS) $(OBJS) $(MAIN) -o fastahack

$(LIB): $(OBJS)
	$(AR) -rs $(LIB) $(OBJS)

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
	rm -rf $(BIN) $(LIB) $(OBJS) $(DESTDIR)

.PHONY: clean
