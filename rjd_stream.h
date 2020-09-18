#define RJD_STREAM_H 1

// rjd_istream API design based on Fabien Giesen's Buffer-Centric IO:
// https://fgiesen.wordpress.com/2011/11/21/buffer-centric-io/

struct rjd_istream;

typedef struct rjd_result rjd_istream_refill_func(struct rjd_istream* stream);
typedef void rjd_istream_close_func(struct rjd_istream* stream);

struct rjd_istream
{
	const uint8_t* start;
	const uint8_t* end;
	const uint8_t* cursor;
	void* userdata;
	struct rjd_result result;
	rjd_istream_refill_func* refill;
	rjd_istream_close_func* close;
};

// Pass this as buffer_size in rjd_istream_from_file to read the entire file into memory
enum
{
	RJD_ISTREAM_FILE_BUFFER_SIZE_ALL = 0,
};

enum rjd_ostream_type
{
	RJD_OSTREAM_TYPE_MEMORY,
	RJD_OSTREAM_TYPE_FILE,
};

struct rjd_ostream_memory
{
	uint8_t* buffer;
	uint32_t size;
	uint64_t cursor;
};

struct rjd_ostream_file
{
	FILE* file;
};

struct rjd_ostream
{
	enum rjd_ostream_type type;
	union {
		struct rjd_ostream_memory memory;
		struct rjd_ostream_file file;
	} state;
};

enum rjd_ostream_mode
{
	RJD_OSTREAM_MODE_REPLACE,
	RJD_OSTREAM_MODE_APPEND,
};

struct rjd_mem_allocator;

struct rjd_istream rjd_istream_from_zeroes(void);
struct rjd_istream rjd_istream_from_memory(const void* buffer, size_t size);
struct rjd_istream rjd_istream_from_file(const char* filepath, struct rjd_mem_allocator* allocator, size_t buffer_size);
struct rjd_result rjd_istream_read(struct rjd_istream* stream, void* buffer, size_t size); 
void rjd_istream_close(struct rjd_istream* stream);

struct rjd_ostream rjd_ostream_from_memory(void* buffer, size_t size);
struct rjd_ostream rjd_ostream_from_file(const char* filepath, enum rjd_ostream_mode);
struct rjd_result rjd_ostream_write(struct rjd_ostream* stream, const void* buffer, size_t size);
void rjd_ostream_close(struct rjd_ostream* stream);

#if RJD_IMPL

static struct rjd_result rjd_istream_fail(struct rjd_istream* stream, const char* reason);
static struct rjd_result rjd_istream_refill_zeroes(struct rjd_istream* stream);
static struct rjd_result rjd_istream_refill_memory(struct rjd_istream* stream);
static struct rjd_result rjd_istream_refill_file(struct rjd_istream* stream);
static void rjd_istream_close_file(struct rjd_istream* stream);

struct rjd_istream rjd_istream_from_zeroes()
{
	struct rjd_istream stream = 
	{
		.start = NULL,
		.end = NULL,
		.cursor = NULL,
		.userdata = NULL,
		.result = RJD_RESULT_OK(),
		.refill = rjd_istream_refill_zeroes,
		.close = NULL,
	};
	stream.refill(&stream);
	return stream;
}

struct rjd_istream rjd_istream_from_memory(const void* buffer, size_t size)
{
	const uint8_t* cbuffer = buffer;

	struct rjd_istream stream =
	{
		.start = cbuffer,
		.end = cbuffer + size,
		.cursor = cbuffer,
		.userdata = NULL,
		.result = RJD_RESULT_OK(),
		.refill = rjd_istream_refill_memory,
		.close = NULL,
	};
	return stream;
}

struct rjd_istream rjd_istream_from_file(const char* filepath, struct rjd_mem_allocator* allocator, size_t buffer_size)
{
	RJD_ASSERT(filepath);
	RJD_ASSERT(allocator);

	uint8_t* buffer = NULL;
	struct rjd_result result = RJD_RESULT_OK();
	FILE* file = fopen(filepath, "rb");

	if (file) {
		if (buffer_size == RJD_ISTREAM_FILE_BUFFER_SIZE_ALL) {
			result = RJD_RESULT("Failed to get file length");
			if (fseek(file, 0, SEEK_END) == 0) {
				long int length = ftell(file);
				if (length >= 0) {
					if (fseek(file, 0, SEEK_SET) == 0) {
						buffer_size = (size_t)length;
						result = RJD_RESULT_OK();
					}
				}
				printf("\trjd_istream_from_file (%s): file length: %d", filepath, (int)length);
			}
		}
	} else {
		result = RJD_RESULT("Failed to open file");
	}

	if (rjd_result_isok(result)) {
		buffer = rjd_mem_alloc_array(uint8_t, buffer_size, allocator);
	}

	struct rjd_istream stream =
	{
		.start = buffer,
		.end = buffer + buffer_size,
		.cursor = buffer,
		.userdata = file,
		.refill = rjd_istream_refill_file,
		.close = rjd_istream_close_file,
	};
	stream.result = stream.refill(&stream);
	return stream;
}

struct rjd_result rjd_istream_read(struct rjd_istream* stream, void* buffer, size_t size)
{
	RJD_ASSERT(stream);
	RJD_ASSERT(buffer);
	RJD_ASSERT(size > 0);

    uint8_t* offset_buffer = buffer;
	size_t bytes_remaining = size;
	while (bytes_remaining > 0) {
		if (stream->cursor == stream->end) {
			stream->result = stream->refill(stream);
		}
        RJD_ASSERT(stream->end >= stream->cursor)
		ptrdiff_t buffersize = stream->end - stream->cursor;
		size_t readsize = (size_t)rjd_math_minu64((size_t)buffersize, bytes_remaining);

		memcpy(offset_buffer, stream->cursor, readsize);
        
        offset_buffer += readsize;
		bytes_remaining -= readsize;
		stream->cursor += readsize;
	}

	return stream->result;
}

void rjd_istream_close(struct rjd_istream* stream)
{
	RJD_ASSERT(stream);

	if (stream->close) {
		stream->close(stream);
	}
}

struct rjd_ostream rjd_ostream_from_memory(void* buffer, size_t size)
{
	struct rjd_ostream stream = 
	{
		.type = RJD_OSTREAM_TYPE_MEMORY,
		.state = {
			.memory = { buffer, (uint32_t)size, 0 },
		},
	};
	return stream;
}

struct rjd_ostream rjd_ostream_from_file(const char* filepath, enum rjd_ostream_mode mode)
{
	const char* writemode = NULL;
	switch (mode)
	{
		case RJD_OSTREAM_MODE_REPLACE: writemode = "wb"; break;
		case RJD_OSTREAM_MODE_APPEND:  writemode = "ab"; break;
	}
	RJD_ASSERT(writemode);
	FILE* file = fopen(filepath, writemode);

	struct rjd_ostream stream =
	{
		.type = RJD_OSTREAM_TYPE_FILE,
		.state = {
			.file = { file },
		},
	};
	return stream;
}

struct rjd_result rjd_ostream_write(struct rjd_ostream* stream, const void* buffer, size_t size)
{
	if (stream->type == RJD_OSTREAM_TYPE_MEMORY) {
		struct rjd_ostream_memory* state = &stream->state.memory;
		RJD_ASSERT(state->size >= state->cursor);

		const size_t bytes_remaining = state->size - state->cursor;
		if (size > bytes_remaining) {
			return RJD_RESULT("attempted to write more data than the buffer can hold");
		}

		const uint8_t* cbuffer = buffer;
		RJD_ASSERTMSG(!(cbuffer >= state->buffer && cbuffer < state->buffer + state->size), 
					"source and destination buffers must not overlap");

		memcpy(state->buffer + state->cursor, buffer, size);
		state->cursor += size;
	} else if (stream->type == RJD_OSTREAM_TYPE_FILE) {
		struct rjd_ostream_file* state = &stream->state.file;
		if (state->file == NULL) {
			return RJD_RESULT("failed to open file for writing");
		}
		
		size_t bytes_written = fwrite(buffer, 1, size, state->file);
		if (bytes_written != size) {
			return RJD_RESULT("failed to write all data to file");
		}
	}

	return RJD_RESULT_OK();
}

void rjd_ostream_close(struct rjd_ostream* stream)
{
	if (stream->type == RJD_OSTREAM_TYPE_MEMORY) {
		stream->state.memory.buffer = NULL;
	} else if (stream->type == RJD_OSTREAM_TYPE_FILE) {
		fclose(stream->state.file.file);
	}
}

////////////////////////////////////////////////////////////////////////////////
// static helpers

static struct rjd_result rjd_istream_fail(struct rjd_istream* stream, const char* reason)
{
	stream->result = RJD_RESULT(reason);
	stream->refill = rjd_istream_refill_zeroes;
	return stream->refill(stream);
}

static struct rjd_result rjd_istream_refill_zeroes(struct rjd_istream* stream)
{
	static uint8_t zeroes[128] = {0};

	stream->start = zeroes;
	stream->end = zeroes + sizeof(zeroes);
	stream->cursor = stream->start;
	return stream->result;
}

static struct rjd_result rjd_istream_refill_memory(struct rjd_istream* stream)
{
	return rjd_istream_fail(stream, "reached end of memory buffer");
}

static struct rjd_result rjd_istream_refill_file(struct rjd_istream* stream)
{
	FILE* file = (FILE*)stream->userdata;

	 // cast to non-const since we know the file buffer is ok to write to
	void* writable_buffer = (void*)stream->start;
	ptrdiff_t bytes_wanted = stream->end - stream->start;
	int32_t bytes_read = (int32_t)fread(writable_buffer, 1, bytes_wanted, file);

    RJD_ASSERT(bytes_read <= bytes_wanted);
    
	if (bytes_read < bytes_wanted) {
		if (feof(file)) {
			if (bytes_read == 0) {
				rjd_istream_close_file(stream);
				rjd_istream_fail(stream, "end of file reached, no more data available");
			} else {
				stream->end = stream->start + bytes_read;
                stream->cursor = stream->start;
                stream->result = RJD_RESULT_OK();
			}
		} else {
			rjd_istream_close_file(stream);
			rjd_istream_fail(stream, "error reading file contents into memory");
		}
	} else {
        if (feof(file)) {
            rjd_istream_fail(stream, "end of file reached, no more data available");
        } else {
            stream->cursor = stream->start;
        }
    }

	return stream->result;
}

static void rjd_istream_close_file(struct rjd_istream* stream)
{
	rjd_mem_free(stream->start);
	FILE* file = (FILE*)stream->userdata;
	if (file) {
		fclose(file);
	}

	stream->userdata = NULL;
	stream->refill = rjd_istream_refill_zeroes;
	stream->close = NULL;

	stream->refill(stream);
}

#endif

