cmake_minimum_required(VERSION 2.8)

add_library(fastahack
	FastaHack.cpp
	Fasta.cpp
	split.cpp
	disorder.c
)

add_executable(fastahack-bin
	FastaHack.cpp
	disorder.c
)

set_target_properties(fastahack-bin
	PROPERTIES OUTPUT_NAME fastahack
)

target_link_libraries(fastahack-bin
	fastahack
)
