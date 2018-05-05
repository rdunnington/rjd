all:
	gcc --std=c11 -pedantic -Wall -Wextra -g -march=native tests.c 

clean:
	rm *.exe
	rm *.ilk
	rm *.obj
	rm *.pdb
	rm -r Debug
	rm *.stackdump

