#pragma once

#define RJD_STRBUF 1

#ifndef RJD_STRBUF_STATIC_SIZE
	#define RJD_STRBUF_STATIC_SIZE 512
#endif

RJD_STATIC_ASSERT(RJD_STRBUF_STATIC_SIZE > 0);

struct rjd_alloc_context;

struct rjd_strbuf
{
	struct rjd_alloc_context* allocator;
	size_t length;
	char* heap;
	char stack[RJD_STRBUF_STATIC_SIZE];
};

struct rjd_strbuf rjd_strbuf_init(struct rjd_alloc_context* allocator);
const char* rjd_strbuf_str(const struct rjd_strbuf* buf);
void rjd_strbuf_append(struct rjd_strbuf* buf, const char* format, ...);
void rjd_strbuf_vappend(struct rjd_strbuf* buf, const char* format, const va_list args);
void rjd_strbuf_free(struct rjd_strbuf* buf);

#if RJD_ENABLE_SHORTNAMES
	#define strbuf_init		rjd_strbuf_init
	#define strbuf_str		rjd_strbuf_str
	#define strbuf_append	rjd_strbuf_append
	#define strbuf_free		rjd_strbuf_free
#endif

#if RJD_IMPL

static void rjd_strbuf_grow(struct rjd_strbuf* buf, uint32_t format_length);

struct rjd_strbuf rjd_strbuf_init(struct rjd_alloc_context* allocator)
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
	return buf->heap ? buf->heap : buf->stack;
}

void rjd_strbuf_append(struct rjd_strbuf* buf, const char* format, ...)
{
	RJD_ASSERT(buf);

	if (!format || *format == '\0') {
		return;
	}

	va_list args;
	va_start(args, format);
		rjd_strbuf_vappend(buf, format, args);
	va_end(args);
}

void rjd_strbuf_vappend(struct rjd_strbuf* buf, const char* format, const va_list args)
{
	uint32_t capacity = buf->heap ? rjd_array_capacity(buf->heap) : RJD_STRBUF_STATIC_SIZE;
	uint32_t remaining = capacity - buf->length;
	uint32_t format_length = strlen(format);

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

void rjd_strbuf_free(struct rjd_strbuf* buf)
{
	rjd_array_free(buf->heap);
	buf->length = 0;
	buf->heap = 0;
	buf->stack[0] = '\0';
}

static void rjd_strbuf_grow(struct rjd_strbuf* buf, uint32_t format_length)
{
	RJD_ASSERT(buf && buf->allocator);

	uint32_t current = buf->heap ? rjd_array_capacity(buf->heap) : RJD_STRBUF_STATIC_SIZE;
	uint32_t min = current + format_length;
	uint32_t next = rjd_math_next_pow2(min);

	if (!buf->heap) {
		buf->heap = rjd_array_alloc(char, next, buf->allocator);

		strcpy(buf->heap, buf->stack);
	}
	rjd_array_resize(buf->heap, next);
}

#endif // RJD_IMPL

