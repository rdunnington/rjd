#pragma once

#define RJD_PATH_H 1

#ifndef RJD_PATH_BUFFER_LENGTH
	#define RJD_PATH_BUFFER_LENGTH 256
#endif

struct rjd_path
{
	uint32_t length;
	char str[RJD_PATH_BUFFER_LENGTH];
};

struct rjd_path_enumerator_state
{
	char impl[32];
};

struct rjd_path rjd_path_create(void);
struct rjd_path rjd_path_create_with(const char* path);
void rjd_path_append(struct rjd_path* path, const char* str);
void rjd_path_join(struct rjd_path* path1, const struct rjd_path* path2);
const char* rjd_path_get(struct rjd_path* path);
void rjd_path_clear(struct rjd_path* path);
const char* rjd_path_extension(const struct rjd_path* path);
const char* rjd_path_extension_str(const char* path);

struct rjd_path_enumerator_state rjd_path_enumerate_create(const char* path, struct rjd_mem_allocator* string_allocator);
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

struct rjd_path rjd_path_create()
{
	struct rjd_path path = {0};
	return path;
}

struct rjd_path rjd_path_create_with(const char* initial_contents)
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
	return rjd_path_extension_str(path->str);
}

const char* rjd_path_extension_str(const char* path)
{
    if (!path) {
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

struct rjd_path_enumerator_state_win32
{
	struct rjd_mem_allocator* allocator;
	const char* nextpath;
	const char* root;
	HANDLE handle;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_path_enumerator_state_win32) <= sizeof(struct rjd_path_enumerator_state));

static const char* rjd_path_enumerate_copystring(const char* path, struct rjd_mem_allocator* allocator);

struct rjd_path_enumerator_state rjd_path_enumerate_create(const char* path, struct rjd_mem_allocator* string_allocator)
{
	RJD_ASSERT(path);
	RJD_ASSERT(string_allocator);

	struct rjd_path_enumerator_state state = {0};
	struct rjd_path_enumerator_state_win32* state_win32 = (struct rjd_path_enumerator_state_win32*)&state;
	state_win32->allocator = string_allocator;
	state_win32->nextpath = NULL;
	state_win32->root = rjd_path_enumerate_copystring(path, string_allocator);
	state_win32->handle = INVALID_HANDLE_VALUE;

	return state;
}

const char* rjd_path_enumerate_next(struct rjd_path_enumerator_state* state)
{
	RJD_ASSERT(state);

	struct rjd_path_enumerator_state_win32* state_win32 = (struct rjd_path_enumerator_state_win32*)&state;

	if (state_win32->nextpath) {
		rjd_mem_free(state_win32->nextpath);
		state_win32->nextpath = NULL;
	}

	WIN32_FIND_DATAA find_data = {0};

	bool success = true;
	if (state_win32->handle == INVALID_HANDLE_VALUE) {
		state_win32->handle = FindFirstFileA(state_win32->root, &find_data);
		success = state_win32->handle != INVALID_HANDLE_VALUE;
		state_win32->root = NULL;
	} else {
		success = FindNextFileA(state_win32->handle, &find_data);
	}

	if (success)
	{
		state_win32->nextpath = rjd_path_enumerate_copystring(find_data.cFileName, state_win32->allocator);
	}

	return state_win32->nextpath;
}

void rjd_path_enumerate_destroy(struct rjd_path_enumerator_state* state)
{
	RJD_ASSERT(state);

	struct rjd_path_enumerator_state_win32* state_win32 = (struct rjd_path_enumerator_state_win32*)&state;

	rjd_mem_free(state_win32->root);
	if (state_win32->nextpath) {
		rjd_mem_free(state_win32->nextpath);
	}

	FindClose(state_win32->handle);
}

static const char* rjd_path_enumerate_copystring(const char* path, struct rjd_mem_allocator* allocator)
{
	uint32_t length = (uint32_t)strlen(path) + 1;
	char* copy = rjd_mem_alloc_array_noclear(char, length, allocator);
	memcpy(copy, path, length);
	return copy;
}

#elif RJD_PLATFORM_OSX

#if !RJD_LANG_OBJC
	#error "rjd_path implementation on OSX uses Objective-C interfaces. You must #include this file in a .m file."
#endif

#import <Foundation/Foundation.h>

struct rjd_path_enumerator_state_osx
{
	NSDirectoryEnumerator<NSString*>* enumerator;
	NSString* next;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_path_enumerator_state_osx) <= sizeof(rjd_path_enumerator_state));

struct rjd_path_enumerator_state rjd_path_enumerate_create(const char* path)
{
	RJD_ASSERT(path);

	NSFileManager* manager = [NSFileManager defaultManager];
	NSString* startingPath = [NSString stringWithUTF8String:path];

	// NSFileManager:enumeratorAtPath is threadsafe
	NSDirectoryEnumerator<NSString*>* enumerator = [manager enumeratorAtPath:startingPath];
	[enumerator skipDescendants];

	struct rjd_path_enumerator_state state = {0};
	struct rjd_path_enumerator_state_osx* state_osx = (struct rjd_path_enumerator_state_osx*)&state;
	state_osx->enumerator = enumerator;
	state_osx->next = nil;

	return state;
}

const char* rjd_path_enumerate_next(struct rjd_path_enumerator_state* state)
{
	RJD_ASSERT(state);

	struct rjd_path_enumerator_state_osx* state_osx = (struct rjd_path_enumerator_state_osx*)state;
	state_osx->next = (NSString*) [state_osx->enumerator nextObject];
	
	return state_osx->next.UTF8String;
}

void rjd_path_enumerate_destroy(struct rjd_path_enumerator_state* state)
{
	RJD_ASSERT(state);

	struct rjd_path_enumerator_state_osx* state_osx = (struct rjd_path_enumerator_state_osx*)state;
	state_osx->enumerator = nil;
	state_osx->next = nil;
}

#endif // RJD_PLATFORM_OSX && RJD_LANG_OBJC
#endif // RJD_IMPL
