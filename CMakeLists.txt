add_library(fastahck
	FastaHack.cpp
	Fasta.cpp
	split.cpp
	disorder.c
)

add_executable(fastahack
	FastaHack.cpp
	disorder.c
)

target_link_libraries(fastahack
	fastahck
)
