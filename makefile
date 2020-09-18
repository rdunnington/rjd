ifeq ($(OS), Windows_NT)
	PLATFORM_FLAGS := -D WINVER=_WIN32_WINNT_WIN10 -D _WIN32_WINNT=_WIN32_WINNT_WIN10 -D WDK_NTDDI_VERSION=NTDDI_WIN10_RS1
	PLATFORM_FILES := tests_rjd.c
	OUTPUT_FILE := tests.exe
else
	#SHELL_NAME := $(shell uname -s)
	#ifeq($(SHELL_NAME), Darwin) #OSX
	#endif
	#ifeq($(SHELL_NAME), Linux) #Linux
	#endif

	PLATFORM_FLAGS := -fsanitize=undefined -fsanitize=address  
	PLATFORM_FILES := tests_rjd.m -framework Foundation -framework AppKit -framework Metal -framework MetalKit
	OUTPUT_FILE := tests
endif

all:
	@# -Wno-unused-local-typedefs to suppress locally defined typedefs coming from RJD_STATIC_ASSERT
	gcc --std=c11 -pedantic -Wall -Wextra -g -march=native -Wno-unused-local-typedefs -Wno-missing-braces $(PLATFORM_FLAGS) tests.c $(PLATFORM_FILES) -o $(OUTPUT_FILE)

tags:
	ctags -f tags *

clean:
	rm *.exe
	rm *.ilk
	rm *.obj
	rm *.pdb
	rm -r Debug
	rm *.stackdump
	rm tests
	rm -r tests.dSYM

test:
	./$(OUTPUT_FILE)
