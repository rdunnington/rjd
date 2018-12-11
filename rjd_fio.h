#pragma once

#define RJD_FILEIO 1

#define RJD_FIO_ERR_ENUM(macro) \
	macro(RJD_FIO_ERR_OK)		\
	macro(RJD_FIO_ERR_IO)		\
	macro(RJD_FIO_ERR_NOMEM)
RJD_ENUM_DECLARE(rjd_fio_err, RJD_FIO_ERR_ENUM);

#define RJD_FIO_WRITEMODE_ENUM(macro)	\
	macro(RJD_FIO_WRITEMODE_REPLACE)	\
	macro(RJD_FIO_WRITEMODE_APPEND)
RJD_ENUM_DECLARE(rjd_fio_writemode, RJD_FIO_WRITEMODE_ENUM);

// use rjd_array_free() to free *buffer after use
enum rjd_fio_err rjd_fio_read(const char* path, char** buffer, struct rjd_mem_allocator* context);
enum rjd_fio_err rjd_fio_write(const char* path, const char* data, size_t length, enum rjd_fio_writemode mode);
enum rjd_fio_err rjd_fio_size(const char* path, size_t* out_size);
enum rjd_fio_err rjd_fio_delete(const char* path);

#if RJD_ENABLE_SHORTNAMES
	#define fio_err					rjd_fio_err
	#define fio_writemode			rjd_fio_writemode

	#define fio_read				rjd_fio_read
	#define	fio_write				rjd_fio_write
	#define fio_size				rjd_fio_size
	#define fio_delete				rjd_fio_delete

	#define FIO_ERR_OK				RJD_FIO_ERR_OK
	#define FIO_ERR_IO				RJD_FIO_ERR_IO
	#define FIO_ERR_NOMEM			RJD_FIO_ERR_NOMEM
	#define FIO_WRITEMODE_REPLACE	RJD_FIO_WRITEMODE_REPLACE
	#define FIO_WRITEMODE_APPEND	RJD_FIO_WRITEMODE_APPEND
#endif

#if RJD_IMPL

RJD_ENUM_DEFINE(rjd_fio_err, RJD_FIO_ERR_ENUM);
RJD_ENUM_DEFINE(rjd_fio_writemode, RJD_FIO_WRITEMODE_ENUM);

enum rjd_fio_err rjd_fio_read(const char* path, char** buffer, struct rjd_mem_allocator* context)
{
	FILE* file = fopen(path, "rb");
	if (!file) {
		return RJD_FIO_ERR_IO;
	}

	if (fseek(file, 0, SEEK_END)) {
		fclose(file);
		return RJD_FIO_ERR_IO;
	}

	long int length = ftell(file);
	if (length < 0) {
		length = 0;
	}

	rewind(file);

	*buffer = rjd_array_alloc(char, (uint32_t)length, context);
	if (!*buffer) {
		return RJD_FIO_ERR_NOMEM;
	}
	rjd_array_resize(*buffer, (uint32_t)length);

	size_t read_length = fread(*buffer, 1, length, file);
	fclose(file);

	if (read_length < (size_t)length) {
		rjd_array_free(*buffer);
		*buffer = NULL;
		return RJD_FIO_ERR_IO;
	}

	return RJD_FIO_ERR_OK;
}

enum rjd_fio_err rjd_fio_write(const char* path, const char* data, size_t length, enum rjd_fio_writemode mode)
{
	const char* m = (mode == RJD_FIO_WRITEMODE_REPLACE) ? "wb" : "ab";
	FILE* file = fopen(path, m);

	size_t written = fwrite(data, 1, length, file);

	fclose(file);

	if (written == 0) {
		return RJD_FIO_ERR_IO;
	} else if (written < length) {
		return RJD_FIO_ERR_IO;
	} else {
		return RJD_FIO_ERR_OK;
	}
}

enum rjd_fio_err rjd_fio_size(const char* path, size_t* out_size)
{
	FILE* file = fopen(path, "rb");
	if (!file) {
		return RJD_FIO_ERR_IO;
	}

	if (fseek(file, 0, SEEK_END)) {
		fclose(file);
		return RJD_FIO_ERR_IO;
	}

	long int length = ftell(file);
	fclose(file);

	*out_size = (size_t) length;
	return RJD_FIO_ERR_OK;
}

enum rjd_fio_err rjd_fio_delete(const char* path)
{
	if (remove(path)) {
		return RJD_FIO_ERR_IO;
	}

	return RJD_FIO_ERR_OK;
}

#endif // RJD_IMPL

