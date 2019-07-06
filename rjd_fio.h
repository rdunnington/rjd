#pragma once

#define RJD_FILEIO_H 1

enum rjd_fio_writemode
{
	RJD_FIO_WRITEMODE_REPLACE,
	RJD_FIO_WRITEMODE_APPEND,
};

// use rjd_array_free() to free *buffer after use
struct rjd_result rjd_fio_read(const char* path, char** buffer, struct rjd_mem_allocator* allocator);
struct rjd_result rjd_fio_write(const char* path, const char* data, size_t length, enum rjd_fio_writemode mode);
struct rjd_result rjd_fio_size(const char* path, size_t* out_size);
struct rjd_result rjd_fio_delete(const char* path);
struct rjd_result rjd_fio_mkdir(const char* path);
bool rjd_fio_exists(const char* path);

#if RJD_IMPL

#if RJD_PLATFORM_OSX
#include <sys/stat.h>
#include <ftw.h>
#endif

struct rjd_result rjd_fio_read(const char* path, char** buffer, struct rjd_mem_allocator* allocator)
{
	FILE* file = fopen(path, "rb");
	if (!file) {
		return RJD_RESULT("Failed to open the path for reading");
	}

	if (fseek(file, 0, SEEK_END)) {
		fclose(file);
		return RJD_RESULT("Failed to seek to the end of the file");
	}

	long int length = ftell(file);
	if (length < 0) {
		length = 0;
	}

	rewind(file);

	*buffer = rjd_array_alloc(char, (uint32_t)length, allocator);
	if (!*buffer) {
		return RJD_RESULT("Not enough memory in the allocator");
	}
	rjd_array_resize(*buffer, (uint32_t)length);

	size_t read_length = fread(*buffer, 1, length, file);
	fclose(file);

	if (read_length < (size_t)length) {
		rjd_array_free(*buffer);
		*buffer = NULL;
		return RJD_RESULT("Failed to read the entire file into memory");
	}

	return RJD_RESULT_OK();
}

struct rjd_result rjd_fio_write(const char* path, const char* data, size_t length, enum rjd_fio_writemode mode)
{
	const char* m = (mode == RJD_FIO_WRITEMODE_REPLACE) ? "wb" : "ab";
	FILE* file = fopen(path, m);
	if (!file) {
		return RJD_RESULT("Failed to open a writable handle");
	}

	const size_t written = fwrite(data, 1, length, file);

	fclose(file);

	if (written == 0) {
		return RJD_RESULT("Failed to write any bytes");
	} else if (written < length) {
		return RJD_RESULT("Failed to write the entire file");
	} else {
		return RJD_RESULT_OK();
	}
}

struct rjd_result rjd_fio_size(const char* path, size_t* out_size)
{
	FILE* file = fopen(path, "rb");
	if (!file) {
		return RJD_RESULT("Failed to open the path for reading");
	}

	if (fseek(file, 0, SEEK_END)) {
		fclose(file);
		return RJD_RESULT("Failed to seek to the end of the file");
	}

	long int length = ftell(file);
	fclose(file);

	*out_size = (size_t) length;
	return RJD_RESULT_OK();
}

#if RJD_PLATFORM_WINDOWS
struct rjd_result rjd_fio_delete(const char* path)
{
	return RJD_RESULT("not implmented");
}

struct rjd_result rjd_fio_mkdir(const char* path)
{
	return RJD_RESULT("not implmented");
}

bool rjd_fio_exists(const char* path)
{
	return false;
}

#elif RJD_PLATFORM_OSX
static int rjd_delete_nftw_func(const char *path, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
	return remove(path);
}

struct rjd_result rjd_fio_delete(const char* path)
{
	if (remove(path)) {

    	if (nftw(path, rjd_delete_nftw_func, 64, FTW_DEPTH | FTW_PHYS)) {
			return RJD_RESULT("delete failed");
		}
	}

	return RJD_RESULT_OK();
}

bool rjd_fio_exists(const char* path)
{
    struct stat unused;
    return !stat(path, &unused);
}

struct rjd_result rjd_fio_mkdir(const char* path)
{
    RJD_ASSERT(path);
    RJD_ASSERT(*path);
    
    bool created_directory = false;
    
    const char* next = path;
    const char* end = next;
	do
    {
		end = strstr(end, "/");
        // skip directory separator
        if (end) {
            ++end;
        }
        
        char stackbuffer[256];
        const char* subpath = NULL;

		if (end) {
            RJD_ASSERT(end - next < sizeof(stackbuffer));
			memcpy(stackbuffer, next, end - next);
			stackbuffer[end - next] = '\0';
            subpath = stackbuffer;
		} else {
			subpath = next;
		}

		const mode_t mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
		if (mkdir(subpath, mode) == 0) {
            created_directory = true;
        } else if (created_directory == true) {
            // there was an error creating a nested folder
            created_directory = false;
            break;
        }
    } while (end != NULL);
    
    return created_directory ? RJD_RESULT_OK() : RJD_RESULT("error creating directory");
}
#endif

#endif // RJD_IMPL

