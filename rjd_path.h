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
	char impl[16];
};

struct rjd_path rjd_path_create(void);
struct rjd_path rjd_path_create_with(const char* path);
void rjd_path_append(struct rjd_path* path, const char* str);
void rjd_path_join(struct rjd_path* path1, const struct rjd_path* path2);
const char* rjd_path_get(struct rjd_path* path);
void rjd_path_clear(struct rjd_path* path);
const char* rjd_path_extension(const struct rjd_path* path);
const char* rjd_path_extension_str(const char* path);

struct rjd_path_enumerator_state rjd_path_enumerate_create(const char* path);
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
	char* end = stpncpy(path.str, initial_contents, RJD_PATH_BUFFER_LENGTH - 1);
	*end = '\0';
	path.length = (uint32_t)(end - path.str);
	RJD_ASSERT(path.length <= RJD_PATH_BUFFER_LENGTH);

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

	char* end = stpncpy(path->str + start, str, max_length - start);
    uint32_t length = (uint32_t)(end - path->str);

	path->length = rjd_path_normalize_slashes(path->str, length);
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
