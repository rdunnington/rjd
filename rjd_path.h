#pragma once

#define RJD_PATH_H 1

#ifndef RJD_PATH_BUFFER_LENGTH
	#define RJD_PATH_BUFFER_LENGTH 256
#endif

struct rjd_path
{
	int32_t length;
	char str[RJD_PATH_BUFFER_LENGTH];
};

struct rjd_path_enumerator_state
{
	char impl[40];
};

enum RJD_PATH_ENUMERATE_MODE
{
	RJD_PATH_ENUMERATE_MODE_RECURSIVE,
	RJD_PATH_ENUMERATE_MODE_FLAT,
};

struct rjd_path rjd_path_init(void);
struct rjd_path rjd_path_init_with(const char* path);
void rjd_path_append(struct rjd_path* path, const char* str);
void rjd_path_pop(struct rjd_path* path);
void rjd_path_join(struct rjd_path* path1, const struct rjd_path* path2);
const char* rjd_path_get(struct rjd_path* path);
void rjd_path_clear(struct rjd_path* path);

const char* rjd_path_extension(const struct rjd_path* path);
bool rjd_path_endswith(const struct rjd_path* path, const char* str);

const char* rjd_path_str_extension(const char* path);
bool rjd_path_str_endswith(const char* path, const char* str);

struct rjd_path_enumerator_state rjd_path_enumerate_create(const char* path, struct rjd_mem_allocator* allocator, enum RJD_PATH_ENUMERATE_MODE mode);
const char* rjd_path_enumerate_next(struct rjd_path_enumerator_state* state);
void rjd_path_enumerate_destroy(struct rjd_path_enumerator_state* state);

#if RJD_IMPL

#if RJD_PLATFORM_WINDOWS
	#define RJD_PATH_SLASH ('/')
#elif RJD_PLATFORM_OSX
	#define RJD_PATH_SLASH ('/')
#else
	#error "Unknown platform"
#endif

static uint32_t rjd_path_normalize_slashes(char* path, uint32_t length);

struct rjd_path rjd_path_init()
{
	struct rjd_path path = {0};
	return path;
}

struct rjd_path rjd_path_init_with(const char* initial_contents)
{
	struct rjd_path path;
	path.length = (uint32_t)strlen(initial_contents);
	RJD_ASSERTMSG(path.length < RJD_PATH_BUFFER_LENGTH, 
				"The static size of RJD_PATH_BUFFER_LENGTH (%u) is smaller than the passed string (%u).",
				RJD_PATH_BUFFER_LENGTH, path.length);

	strncpy(path.str, initial_contents, path.length);
	path.str[path.length] = 0;
	path.length = rjd_path_normalize_slashes(path.str, path.length);
	return path;
}

void rjd_path_append(struct rjd_path* path, const char* str)
{
    const char slash = RJD_PATH_SLASH;
    
	uint32_t start = path->length;
	uint32_t max_length = RJD_PATH_BUFFER_LENGTH - 1;
	if (start > 0 && start < max_length && path->str[start - 1] != slash && str[0] != slash) {
		path->str[start] = slash;
		++start;
		++path->length;
	}

	size_t append_length = strlen(str);
	size_t new_length = append_length + path->length;
	RJD_ASSERTMSG(new_length < RJD_PATH_BUFFER_LENGTH, 
				"The static size of RJD_PATH_BUFFER_LENGTH (%u) is smaller than the concatenated length (%u).",
				RJD_PATH_BUFFER_LENGTH, new_length);
	strncpy(path->str + path->length, str, append_length);
	path->str[new_length] = 0;
	path->length = rjd_path_normalize_slashes(path->str, (uint32_t)new_length);
}

void rjd_path_pop(struct rjd_path* path)
{
	RJD_ASSERT(path);

	for (int32_t i = (int32_t)path->length - 1; i >= 0; --i) {
		if (path->str[i] == RJD_PATH_SLASH && i < path->length - 1) {
					
			path->length = i;
			path->str[i] = 0;
			break;
		}
	}
}

void rjd_path_join(struct rjd_path* path1, const struct rjd_path* path2)
{
	rjd_path_append(path1, path2->str);
}

const char* rjd_path_get(struct rjd_path* path)
{
	return path->str;
}

const char* rjd_path_extension(const struct rjd_path* path)
{
	return rjd_path_str_extension(path->str);
}

bool rjd_path_endswith(const struct rjd_path* path, const char* str)
{
	return rjd_path_str_endswith(path->str, str);
}

const char* rjd_path_str_extension(const char* path)
{
    if (!path || !*path) {
        return NULL;
    }

	const char* extension = strrchr(path, '.');
	if (!extension) {
		return NULL;
	}

	// If a path ends with a period, it doesn't have an extension
	if (extension[1] == '\0') {
		return NULL;
	}

	return extension;
}

bool rjd_path_str_endswith(const char* path, const char* str)
{
	if (path == NULL || str == NULL) {
		return false;
	}

	if (*str == 0) {
		return true;
	}

	int length_string = strlen(path);
	int length_end = strlen(str);

	if (length_end > length_string) {
		return false;
	}

	const char* path_end = path + (length_string - length_end);
	return !strcmp(path_end, str);
}

void rjd_path_clear(struct rjd_path* path)
{
	RJD_ASSERT(path);
	path->str[0] = '\0';
}

// local helpers
static uint32_t rjd_path_normalize_slashes(char* path, uint32_t length)
{
    if (length == 0) {
        return length;
    }

    for (int32_t i = length - 1; i > 0 && path[i] == RJD_PATH_SLASH; --i) {
		path[i] = '\0';
        --length;
	}

	char* end = path + length;
	for (char* start = end - 1; start > path; --start)
	{
		while (*start != RJD_PATH_SLASH && start > path) {
			--start;
		}

		char* slot = start;
        while (slot > path && *(slot - 1) == RJD_PATH_SLASH) {
            --slot;
        }

        if (start - slot > 0) {
            uint32_t sublength = (uint32_t)(end - start);
			memmove(slot, start, sublength + 1);
            
            uint32_t trimmed_size = (uint32_t)(start - slot);
            end -= trimmed_size;
		}
		start = slot;
	}

    size_t newlength = end - path;
    RJD_ASSERT(newlength == strlen(path));
    return (uint32_t)(end - path);
}

#if RJD_PLATFORM_WINDOWS

#include <wchar.h>
#include <shellapi.h>
#include <limits.h> // INT_MAX

struct rjd_path_enumerator_state_win32
{
	struct rjd_mem_allocator* allocator;
	char* nextpath;
	wchar_t** root_dirs;
	HANDLE handle;
	bool is_recursive;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_path_enumerator_state_win32) <= sizeof(struct rjd_path_enumerator_state));

struct rjd_path_enumerator_state rjd_path_enumerate_create(const char* path, struct rjd_mem_allocator* allocator, enum RJD_PATH_ENUMERATE_MODE mode)
{
	RJD_ASSERT(path);
	RJD_ASSERT(allocator);

	struct rjd_path_enumerator_state state = {0};
	struct rjd_path_enumerator_state_win32* state_win32 = (struct rjd_path_enumerator_state_win32*)&state;
	state_win32->allocator = allocator;
	state_win32->nextpath = NULL;
	state_win32->root_dirs = rjd_array_alloc(wchar_t*, 16, allocator);
	state_win32->handle = INVALID_HANDLE_VALUE;
	state_win32->is_recursive = (RJD_PATH_ENUMERATE_MODE_RECURSIVE == mode);

	wchar_t* path_wide = NULL;
	{
		const size_t path_length = mbstowcs(NULL, path, INT_MAX);
		path_wide = rjd_mem_alloc_array_noclear(wchar_t, path_length + 1, allocator);
		mbstowcs(path_wide, path, INT_MAX);
	}
	rjd_array_push(state_win32->root_dirs, path_wide);

	return state;
}

static wchar_t* rjd_path_enumerate_concat_paths(const wchar_t* a, const wchar_t* b, struct rjd_mem_allocator* allocator)
{
	const wchar_t path_separator[] = L"/";

	size_t length_a = wcslen(a);
	size_t length_b = wcslen(b);
	size_t length_separator = wcslen(path_separator);

	size_t length_total = length_a + length_separator + length_b;

	wchar_t* concat = rjd_mem_alloc_array_noclear(wchar_t, length_total + 1, allocator);
	wcscpy(concat, a);
	wcscpy(concat + length_a, path_separator);
	wcscpy(concat + length_a + length_separator, b);
	return concat;
}

const char* rjd_path_enumerate_next(struct rjd_path_enumerator_state* state)
{
	RJD_ASSERT(state);

	struct rjd_path_enumerator_state_win32* state_win32 = (struct rjd_path_enumerator_state_win32*)state;

	if (state_win32->nextpath) {
		rjd_mem_free(state_win32->nextpath);
		state_win32->nextpath = NULL;
	}

	WIN32_FIND_DATAW find_data = {0};
	while (!*find_data.cFileName && rjd_array_count(state_win32->root_dirs) > 0) {

		if (state_win32->handle == INVALID_HANDLE_VALUE) {
			wchar_t* root_with_search_spec = rjd_path_enumerate_concat_paths(state_win32->root_dirs[0], L"*", state_win32->allocator);
			state_win32->handle = FindFirstFileW(root_with_search_spec, &find_data);
			rjd_mem_free(root_with_search_spec);

			if (state_win32->handle == INVALID_HANDLE_VALUE) {
				find_data.cFileName[0] = '\0';
				rjd_mem_free(state_win32->root_dirs[0]);
				rjd_array_erase_unordered(state_win32->root_dirs, 0);
			}
		}

		if (state_win32->handle != INVALID_HANDLE_VALUE) {
			bool success = true;
			if (!*find_data.cFileName) {
				success = FindNextFileW(state_win32->handle, &find_data);
			}

			while (success && (!wcscmp(find_data.cFileName, L".") || !wcscmp(find_data.cFileName, L".."))) {
				success = FindNextFileW(state_win32->handle, &find_data);
			}

			if (!success) {
				find_data.cFileName[0] = '\0';
				state_win32->handle = INVALID_HANDLE_VALUE;
				rjd_mem_free(state_win32->root_dirs[0]);
				rjd_array_erase_unordered(state_win32->root_dirs, 0);
			}
		}

		if (*find_data.cFileName && state_win32->is_recursive) {
			if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				wchar_t* new_root = rjd_path_enumerate_concat_paths(state_win32->root_dirs[0], find_data.cFileName, state_win32->allocator);
				rjd_array_push(state_win32->root_dirs, new_root);
			}
		}
	}

	if (*find_data.cFileName)
	{
		wchar_t* path = rjd_path_enumerate_concat_paths(state_win32->root_dirs[0], find_data.cFileName, state_win32->allocator);

		const size_t path_length = wcstombs(NULL, path, INT_MAX);
		state_win32->nextpath = rjd_mem_alloc_array_noclear(char, path_length + 1, state_win32->allocator);
		wcstombs(state_win32->nextpath, path, INT_MAX);

		rjd_mem_free(path);
	}

	return state_win32->nextpath;
}

void rjd_path_enumerate_destroy(struct rjd_path_enumerator_state* state)
{
	RJD_ASSERT(state);

	struct rjd_path_enumerator_state_win32* state_win32 = (struct rjd_path_enumerator_state_win32*)state;

	for (uint32_t i = 0; i < rjd_array_count(state_win32->root_dirs); ++i) {
		rjd_mem_free(state_win32->root_dirs + i);
	}
	rjd_array_free(state_win32->root_dirs);
	rjd_mem_free(state_win32->nextpath);

	FindClose(state_win32->handle);
}

#elif RJD_PLATFORM_OSX

#if !RJD_LANG_OBJC
	#error "rjd_path implementation on OSX uses Objective-C interfaces. You must #include this file in a .m file."
#endif

#import <Foundation/Foundation.h>

struct rjd_path_enumerator_state_osx
{
	NSDirectoryEnumerator<NSString*>* enumerator;
	struct rjd_mem_allocator* allocator;
    const char* root;
	const char* next;
	bool no_recursion;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_path_enumerator_state_osx) <= sizeof(struct rjd_path_enumerator_state));

struct rjd_path_enumerator_state rjd_path_enumerate_create(const char* path, struct rjd_mem_allocator* allocator, enum RJD_PATH_ENUMERATE_MODE mode)
{
	RJD_ASSERT(path);
	RJD_ASSERT(allocator);

	NSFileManager* manager = [NSFileManager defaultManager];
	NSString* startingPath = [NSString stringWithUTF8String:path];

	char* path_copy = NULL;
	{
		size_t len = strlen(path);
		path_copy = rjd_mem_alloc_array_noclear(char, len + 1, allocator);
		strcpy(path_copy, path);
	}

	// NSFileManager:enumeratorAtPath is threadsafe
	NSDirectoryEnumerator<NSString*>* enumerator = [manager enumeratorAtPath:startingPath];

	struct rjd_path_enumerator_state state = {0};
	struct rjd_path_enumerator_state_osx* state_osx = (struct rjd_path_enumerator_state_osx*)&state;
	state_osx->enumerator = enumerator;
	state_osx->allocator = allocator;
	state_osx->root = path_copy;
	state_osx->next = NULL;
	state_osx->no_recursion = (mode == RJD_PATH_ENUMERATE_MODE_FLAT);

	return state;
}

const char* rjd_path_enumerate_next(struct rjd_path_enumerator_state* state)
{
	RJD_ASSERT(state);
	struct rjd_path_enumerator_state_osx* state_osx = (struct rjd_path_enumerator_state_osx*)state;

	rjd_mem_free(state_osx->next);
	state_osx->next = NULL;

	if (state_osx->no_recursion) {
		[state_osx->enumerator skipDescendants];
	}

	NSString* next = (NSString*) [state_osx->enumerator nextObject];
	if (next) {
		const char* relative = next.UTF8String;
		const char path_separator[] = "/";

		size_t length_root = strlen(state_osx->root);
		size_t length_relative = strlen(relative);
		size_t length_separator = strlen(path_separator);

		size_t length_total = length_root + length_separator + length_relative;

		char* next_path = rjd_mem_alloc_array_noclear(char, length_total + 1, state_osx->allocator);
		strcpy(next_path, state_osx->root);
		strcpy(next_path + length_root, path_separator);
		strcpy(next_path + length_root + length_separator, relative);
        state_osx->next = next_path;
	}

	return state_osx->next;
}

void rjd_path_enumerate_destroy(struct rjd_path_enumerator_state* state)
{
	RJD_ASSERT(state);

	struct rjd_path_enumerator_state_osx* state_osx = (struct rjd_path_enumerator_state_osx*)state;
	state_osx->enumerator = nil;
	
    rjd_mem_free(state_osx->root);
    rjd_mem_free(state_osx->next);
}

#endif // RJD_PLATFORM_OSX && RJD_LANG_OBJC
#endif // RJD_IMPL
