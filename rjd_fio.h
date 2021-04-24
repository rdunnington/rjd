#pragma once

#define RJD_FILEIO_H 1

enum rjd_fio_writemode
{
	RJD_FIO_WRITEMODE_REPLACE,
	RJD_FIO_WRITEMODE_APPEND,
};

enum rjd_fio_attributes
{
	RJD_FIO_ATTRIBUTES_READONLY		= 0x1,
	RJD_FIO_ATTRIBUTES_DIRECTORY	= 0x2,
};

// use rjd_array_free() to free *buffer after use
struct rjd_result rjd_fio_read(const char* path, char** buffer, struct rjd_mem_allocator* allocator);
struct rjd_result rjd_fio_write(const char* path, const char* data, size_t length, enum rjd_fio_writemode mode);
struct rjd_result rjd_fio_size(const char* path, size_t* out_size);
struct rjd_result rjd_fio_attributes_get(const char* path, enum rjd_fio_attributes* attribute_flags);
struct rjd_result rjd_fio_attributes_set_readonly(const char* path, bool readonly);
struct rjd_result rjd_fio_delete(const char* path);
struct rjd_result rjd_fio_mkdir(const char* path);
bool rjd_fio_exists(const char* path);

#if RJD_IMPL

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
			size_t length = end - next;
            RJD_ASSERT(length < (ptrdiff_t)sizeof(stackbuffer));
			memcpy(stackbuffer, next, length);
			stackbuffer[length] = '\0';
			if (stackbuffer[length - 1] == '/') {
				stackbuffer[length - 1] = '\0';
			}
            subpath = stackbuffer;
		} else {
			subpath = next;
		}

		if (rjd_fio_exists(subpath)) {
			result = RJD_RESULT("Path already exists");
		} else {
			result = rjd_fio_mkdir_platform(subpath);
			if (!rjd_result_isok(result)) {
				break;
			}
		}
    } while (end != NULL);
    
    return result;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#if RJD_COMPILER_MSVC

#include <shellapi.h>

static struct rjd_result rjd_fio_delete_folder_recursive(const wchar_t* directory_path);

#define RJD_FIO_UTF8_TO_UTF16(utf8, out_utf16_name)											\
	const size_t length_utf16 = mbstowcs(NULL, utf8, INT_MAX);								\
	wchar_t* out_utf16_name = rjd_mem_alloc_stack_array_noclear(wchar_t, length_utf16 + 1);	\
	mbstowcs(out_utf16_name, utf8, INT_MAX);

struct rjd_result rjd_fio_attributes_get(const char* path, enum rjd_fio_attributes* attribute_flags)
{
	*attribute_flags = 0;

	RJD_FIO_UTF8_TO_UTF16(path, path_wide);
	DWORD attributes = GetFileAttributes(path_wide);
	if (attributes == INVALID_FILE_ATTRIBUTES) {
		return RJD_RESULT("Failed to get file attributes");
	}

	*attribute_flags |= (attributes & FILE_ATTRIBUTE_DIRECTORY)	? RJD_FIO_ATTRIBUTES_DIRECTORY : 0;
	*attribute_flags |= (attributes & FILE_ATTRIBUTE_READONLY)	? RJD_FIO_ATTRIBUTES_READONLY : 0;

	return RJD_RESULT_OK();
}

struct rjd_result rjd_fio_attributes_set_readonly(const char* path, bool readonly)
{
	RJD_FIO_UTF8_TO_UTF16(path, path_wide);
	DWORD attributes = GetFileAttributes(path_wide);
	if (attributes == INVALID_FILE_ATTRIBUTES) {
		return RJD_RESULT("Failed to get file attributes");
	}

	attributes &= ~FILE_ATTRIBUTE_READONLY;

	if (SetFileAttributes(path_wide, attributes)) {
		return RJD_RESULT_OK("Failed to set file attributes");
	}

	return RJD_RESULT_OK();
}

struct rjd_result rjd_fio_delete(const char* path)
{
	RJD_ASSERT(path && *path);
	RJD_FIO_UTF8_TO_UTF16(path, path_wide);

	DWORD attributes = GetFileAttributesW(path_wide);
	if (attributes == INVALID_FILE_ATTRIBUTES)
	{
		return RJD_RESULT("Failed getting path attributes. Check permissions and verify the path exists.");
	}

	if (attributes & FILE_ATTRIBUTE_READONLY) {
		return RJD_RESULT("Path is read-only");
	}

	if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
		return rjd_fio_delete_folder_recursive(path_wide);
	}

	if (!DeleteFileW(path_wide)) {
		return RJD_RESULT("Failed to delete file. Check GetLastError() for more info.");
	}

	return RJD_RESULT_OK();
}

bool rjd_fio_exists(const char* path)
{
	RJD_ASSERT(path);
	RJD_FIO_UTF8_TO_UTF16(path, path_wide);

	DWORD attributes = GetFileAttributesW(path_wide);
	if (attributes == INVALID_FILE_ATTRIBUTES) {
		DWORD err = GetLastError();
		return err != ERROR_FILE_NOT_FOUND && err != ERROR_PATH_NOT_FOUND;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////

static inline struct rjd_result rjd_fio_mkdir_platform(const char* foldername)
{
	RJD_FIO_UTF8_TO_UTF16(foldername, foldername_wide);

	struct rjd_result result = RJD_RESULT_OK();
	SECURITY_ATTRIBUTES security = { .nLength = sizeof(SECURITY_ATTRIBUTES) };
	if (!CreateDirectoryW(foldername_wide, &security)) {
		const int error = GetLastError();
		RJD_ASSERTMSG(error != ERROR_PATH_NOT_FOUND, "The rjd_fio_mkdir() code should handle this case.");
		switch (GetLastError())
		{
			case ERROR_ALREADY_EXISTS: result = RJD_RESULT("Folder already exists"); break;
			default: result = RJD_RESULT("Unknown error creating subfolder"); break;
		}
	}

	return result;
}

struct rjd_result rjd_fio_delete_folder_recursive(const wchar_t* directory_path)
{
	wchar_t* path_with_search_spec = NULL;
	const size_t path_length = wcslen(directory_path);
	{
		const wchar_t search_spec[] = L"/*";
		path_with_search_spec = rjd_mem_alloc_stack_array_noclear(wchar_t, path_length + rjd_countof(search_spec) + 1);
		wcscpy(path_with_search_spec, directory_path);
		wcscpy(path_with_search_spec + path_length, search_spec);
	}

	WIN32_FIND_DATAW find_data = {0};
	const HANDLE find_handle = FindFirstFileW(path_with_search_spec, &find_data);
	if (find_handle == INVALID_HANDLE_VALUE) {
		const DWORD err = GetLastError();
		if (err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND) {
			return RJD_RESULT("Directory not found");
		}
		return RJD_RESULT("Failed while enumerating directory contents. Check GetLastError() for more info");
	}

	do
	{
		if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (wcscmp(find_data.cFileName, L".") != 0 &&
				wcscmp(find_data.cFileName, L"..") != 0) {

				size_t nested_path_length = wcslen(find_data.cFileName);
				wchar_t* nested_path = _alloca((path_length + nested_path_length + 2) * sizeof(wchar_t)); // +2 for null and path separator
				wcscpy(nested_path, directory_path);
				wcscpy(nested_path + path_length, L"/");
				wcscpy(nested_path + path_length + 1, find_data.cFileName);

				struct rjd_result result = rjd_fio_delete_folder_recursive(nested_path);
				if (!rjd_result_isok(result)) {
					return result;
				}
			}
		} else {
			if (!DeleteFileW(find_data.cFileName)) {
				FindClose(find_handle);
				return RJD_RESULT("Failed to delete file. Check GetLastError() for more info.");
			}
		}
	} while (FindNextFileW(find_handle, &find_data));

	FindClose(find_handle);

	if (GetLastError() != ERROR_NO_MORE_FILES) {
		return RJD_RESULT("Failed while enumerating directory contents. Check GetLastError() for more info");
	}

	if (!RemoveDirectoryW(directory_path)) {
		return RJD_RESULT("Failed to delete the directory. Check GetLastError() for more info");
	}

	return RJD_RESULT_OK();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#elif RJD_COMPILER_GCC || RJD_COMPILER_CLANG

#include <sys/stat.h>
#include <ftw.h>
#include <errno.h>

static int rjd_delete_nftw_func(const char *path, const struct stat *sb, int typeflag, struct FTW *ftwbuf);

struct rjd_result rjd_fio_attributes_get(const char* path, enum rjd_fio_attributes* attribute_flags)
{
	*attribute_flags = 0;

	struct stat s = {0};

	if (stat(path, &s) == 0) {
		const mode_t write_bits = S_IWUSR | S_IWGRP | S_IWOTH;

		*attribute_flags |= S_ISDIR(s.st_mode) ? RJD_FIO_ATTRIBUTES_DIRECTORY : 0;
		*attribute_flags |= (s.st_mode & write_bits) ? 0 : RJD_FIO_ATTRIBUTES_READONLY;
		return RJD_RESULT_OK();
	}
	return RJD_RESULT("Failed to get attributes for path");
}

struct rjd_result rjd_fio_attributes_set_readonly(const char* path, bool readonly)
{
	struct stat s = {0};

	if (stat(path, &s) == 0) {
		const mode_t write_bits = S_IWUSR | S_IWGRP | S_IWOTH;
		mode_t mode = s.st_mode;

		if (readonly) {
			mode &= ~write_bits;
		} else {
			mode |= write_bits;
		}

		if (chmod(path, mode) != 0) {
			return RJD_RESULT("Failed to change access modifier");
		}
		return RJD_RESULT_OK();
	}

	return RJD_RESULT("Failed to get existing access modifiers");
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

////////////////////////////////////////////////////////////////////////////////

static int rjd_delete_nftw_func(const char *path, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
	RJD_UNUSED_PARAM(sb);
	RJD_UNUSED_PARAM(typeflag);
	RJD_UNUSED_PARAM(ftwbuf);
	return remove(path);
}

static inline struct rjd_result rjd_fio_mkdir_platform(const char* foldername)
{
	const mode_t mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
	const int error = mkdir(foldername, mode);

	struct rjd_result result = RJD_RESULT_OK();
	switch (error)
	{
		case 0: break;
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

