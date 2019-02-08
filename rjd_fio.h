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

#if RJD_IMPL

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

struct rjd_result rjd_fio_delete(const char* path)
{
	if (remove(path)) {
		return RJD_RESULT("Failed to delete file");
	}

	return RJD_RESULT_OK();
}

#endif // RJD_IMPL

