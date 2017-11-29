all:
	gcc --std=c99 -pedantic -Wall -Wextra -Wno-missing-field-initializers -g -march=native tests.c

clean:
	rm *.exe
	rm *.ilk
	rm *.obj
	rm *.pdb
	rm -r Debug

