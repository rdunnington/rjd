ifeq ($(OS), Windows_NT)
	PLATFORM_FLAGS :=
	PLATFORM_FILES := tests_rjd.c
else
	#SHELL_NAME := $(shell uname -s)
	#ifeq($(SHELL_NAME), Darwin) #OSX
	#endif
	#ifeq($(SHELL_NAME), Linux) #Linux
	#endif

	PLATFORM_FLAGS := -fsanitize=undefined -fsanitize=address  
	PLATFORM_FILES := tests_rjd.m -framework Foundation -framework Metal -framework MetalKit
endif

all:
	# -Wno-unused-local-typedefs to suppress locally defined typedefs coming from RJD_STATIC_ASSERT
	gcc --std=c11 -pedantic -Wall -Wextra -g -march=native -Wno-unused-local-typedefs $(PLATFORM_FLAGS) tests.c $(PLATFORM_FILES) 

tags:
	ctags -f tags *

clean:
	rm *.exe
	rm *.ilk
	rm *.obj
	rm *.pdb
	rm -r Debug
	rm *.stackdump

