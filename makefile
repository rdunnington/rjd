all:
	# -Wno-unused-local-typedefs to suppress locally defined typedefs coming from RJD_STATIC_ASSERT
	gcc --std=c11 -pedantic -Wall -Wextra -g -march=native -Wno-unused-local-typedefs -fsanitize=undefined -fsanitize=address tests.c 

clean:
	rm *.exe
	rm *.ilk
	rm *.obj
	rm *.pdb
	rm -r Debug
	rm *.stackdump

