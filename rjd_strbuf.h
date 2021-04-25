#pragma once

#define RJD_STRBUF_H 1

#ifndef RJD_STRBUF_STATIC_SIZE
	#define RJD_STRBUF_STATIC_SIZE 512
#endif

RJD_STATIC_ASSERT(RJD_STRBUF_STATIC_SIZE > 0);

struct rjd_mem_allocator;

struct rjd_strbuf
{
	struct rjd_mem_allocator* allocator;
	uint32_t length;
	char* heap;
	char stack[RJD_STRBUF_STATIC_SIZE];
};

struct rjd_strbuf rjd_strbuf_init(struct rjd_mem_allocator* allocator);
uint32_t rjd_strbuf_length(const struct rjd_strbuf* buf);
const char* rjd_strbuf_str(const struct rjd_strbuf* buf);
void rjd_strbuf_clear(struct rjd_strbuf* buf);
void rjd_strbuf_append(struct rjd_strbuf* buf, const char* format, ...);
void rjd_strbuf_appendv(struct rjd_strbuf* buf, const char* format, va_list args);
void rjd_strbuf_appendl(struct rjd_strbuf* buf, const char* str, uint32_t length);
void rjd_strbuf_free(struct rjd_strbuf* buf);

#define RJD_STRBUF_SCOPED(buffername, allocator, scope)				\
	{																\
		struct rjd_strbuf buffername = rjd_strbuf_init(allocator);	\
		{scope}														\
		rjd_strbuf_free(&buffername);								\
	}

#if RJD_IMPL

static void rjd_strbuf_grow(struct rjd_strbuf* buf, uint32_t format_length);

struct rjd_strbuf rjd_strbuf_init(struct rjd_mem_allocator* allocator)
{
	struct rjd_strbuf buf;
	buf.length = 0;
	buf.heap = 0;
	buf.stack[0] = 0;
	buf.allocator = allocator;

	return buf;
}

const char* rjd_strbuf_str(const struct rjd_strbuf* buf)
{
	RJD_ASSERT(buf);
	return buf->heap ? buf->heap : buf->stack;
}

void rjd_strbuf_clear(struct rjd_strbuf* buf)
{
	RJD_ASSERT(buf);
	buf->length = 0;
}

void rjd_strbuf_append(struct rjd_strbuf* buf, const char* format, ...)
{
	RJD_ASSERT(buf);

	if (!format || *format == '\0') {
		return;
	}

	va_list args;
	va_start(args, format);
		rjd_strbuf_appendv(buf, format, args);
	va_end(args);
}

void rjd_strbuf_appendv(struct rjd_strbuf* buf, const char* format, va_list args)
{
	RJD_ASSERT(buf);

	if (!format || *format == '\0') {
		return;
	}

	uint32_t capacity = buf->heap ? rjd_array_capacity(buf->heap) : RJD_STRBUF_STATIC_SIZE;
	uint32_t remaining = capacity - buf->length;
	uint32_t format_length = (uint32_t)strlen(format);

	if (remaining < format_length + 1) {
		rjd_strbuf_grow(buf, format_length);
		remaining = rjd_array_capacity(buf->heap) - buf->length;
	}

	char* str = buf->heap ? buf->heap : buf->stack;
	int written = vsnprintf(str + buf->length, remaining, format, args);
	while (written < 0) {
		rjd_strbuf_grow(buf, 1);
		str = buf->heap;
		written = vsnprintf(str + buf->length, remaining, format, args);
	}

	buf->length += written;
}

void rjd_strbuf_appendl(struct rjd_strbuf* buf, const char* format, uint32_t length)
{
	RJD_ASSERT(buf);
	RJD_ASSERT(format + length <= format + strlen(format));

	if (format == NULL || *format == '\0') {
		return;
	}

	uint32_t capacity = buf->heap ? rjd_array_capacity(buf->heap) : RJD_STRBUF_STATIC_SIZE;
	uint32_t remaining = capacity - buf->length;

	if (remaining < length + 1) {
		rjd_strbuf_grow(buf, length);
	}

	char* str = buf->heap ? buf->heap : buf->stack;
	memcpy(str + buf->length, format, length);
	buf->length += length;
	str[buf->length] = '\0';
}

void rjd_strbuf_free(struct rjd_strbuf* buf)
{
	RJD_ASSERT(buf);

	rjd_array_free(buf->heap);
	buf->length = 0;
	buf->heap = 0;
	buf->stack[0] = '\0';
}

static void rjd_strbuf_grow(struct rjd_strbuf* buf, uint32_t format_length)
{
	RJD_ASSERT(buf);
	RJD_ASSERT(buf->allocator);

	uint32_t current = buf->heap ? rjd_array_capacity(buf->heap) : RJD_STRBUF_STATIC_SIZE;
	uint32_t min = current + format_length + 1;
	uint32_t next = rjd_math_next_pow2_u32(min);

	if (!buf->heap) {
		buf->heap = rjd_array_alloc(char, next, buf->allocator);
		rjd_array_resize(buf->heap, next);

		strcpy(buf->heap, buf->stack);
	} else {
		rjd_array_resize(buf->heap, next);
	}
}

#endif // RJD_IMPL

