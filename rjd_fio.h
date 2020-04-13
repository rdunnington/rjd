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

#if RJD_COMPILER_GCC || RJD_COMPILER_CLANG
#include <sys/stat.h>
#include <ftw.h>
#endif

static inline struct rjd_result rjd_fio_mkdir_platform(const char* foldername);

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

struct rjd_result rjd_fio_mkdir(const char* path)
{
    RJD_ASSERT(path);
    RJD_ASSERT(*path);
    
    struct rjd_result result = RJD_RESULT_OK();
    
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
            RJD_ASSERT(end - next < (ptrdiff_t)sizeof(stackbuffer));
			memcpy(stackbuffer, next, end - next);
			stackbuffer[end - next] = '\0';
            subpath = stackbuffer;
		} else {
			subpath = next;
		}

		result = rjd_fio_mkdir_platform(subpath);
		if (!rjd_result_isok(result)) {
			break;
		}
    } while (end != NULL);
    
    return result;
}

#if RJD_COMPILER_MSVC

//struct rjd_result rjd_fio_delete_recursive(const char* path)
//{
//}
//
//struct rjd_result rjd_fio_delete(const char* path)
//{
//	return RJD_RESULT("not implmented");
//}

static inline struct rjd_result rjd_fio_mkdir_platform(const char* foldername)
{
	struct rjd_result result = RJD_RESULT_OK();
	if (CreateDirectoryA(foldername)) {
		const int error = GetLastError();
		RJD_ASSERTMSG(error != ERROR_PATH_NOT_FOUND, "The rjd_fio_mkdir() code should handle this case."); break;
		switch (GetLastError())
		{
			case ERROR_ALREADY_EXISTS: result = RJD_RESULT("Folder already exists"); break;
			default: result = RJD_RESULT("Unknown error creating subfolder"); break;
		}
	}

	return result;
}


bool rjd_fio_exists(const char* path)
{
	SECURITY_ATTRIBUTES security = { .nlength = sizeof(SECURITY_ATTRIBUTES) };
	HANDLE file = CreateFileA(path,
							GENERIC_READ,
							FILE_SHARE_READ,
							&security,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL);

	const bool exists = file != INVALID_HANDLE_VALUE;

	CloseHandle(file);

	return exists;
}

#else

static int rjd_delete_nftw_func(const char *path, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
	RJD_UNUSED_PARAM(sb);
	RJD_UNUSED_PARAM(typeflag);
	RJD_UNUSED_PARAM(ftwbuf);
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

static inline struct rjd_result rjd_fio_mkdir_platform(const char* foldername)
{
	const mode_t mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
	const int error = mkdir(foldername, mode);

	struct rjd_result result = RJD_RESULT_OK();
	switch (error)
	{
		case EACCES: result = RJD_RESULT("The parent directory does not allow write permission to the process, or one of the directories in pathname did not allow search permission."); break;
		case EDQUOT: result = RJD_RESULT("The user's quota of disk blocks or inodes on the file system has been exhausted."); break;
		case EEXIST: result = RJD_RESULT("pathname already exists (not necessarily as a directory). This includes the case where pathname is a symbolic link, dangling or not."); break;
		case EFAULT: result = RJD_RESULT("pathname points outside your accessible address space."); break;
		case ELOOP: result = RJD_RESULT("Too many symbolic links were encountered in resolving pathname."); break;
		case EMLINK: result = RJD_RESULT("The number of links to the parent directory would exceed LINK_MAX."); break;
		case ENAMETOOLONG: result = RJD_RESULT("pathname was too long."); break;
		case ENOENT: result = RJD_RESULT("A directory component in pathname does not exist or is a dangling symbolic link."); break;
		case ENOMEM: result = RJD_RESULT("Insufficient kernel memory was available."); break;
		case ENOSPC: result = RJD_RESULT("The device has no room for the new directory (user's disk quota may be exhausted)."); break;
		case ENOTDIR: result = RJD_RESULT("A component used as a directory in pathname is not, in fact, a directory."); break;
		case EPERM: result = RJD_RESULT("The file system containing pathname does not support the creation of directories."); break;
		case EROFS: result = RJD_RESULT("pathname refers to a file on a read-only file system."); break; 
		default: result = RJD_RESULT("Unknown error creating subpath"); break;
	}

	return result;
}

#endif

#endif // RJD_IMPL

