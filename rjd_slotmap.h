#pragma once

#define RJD_SLOTMAP_H 1

struct rjd_slot
{
	uint16_t index;
	uint16_t salt;
};

#define rjd_slotmap_alloc(type, count, allocator)	(rjd_slotmap_alloc_impl(sizeof(type), count, allocator))
#define rjd_slotmap_insert(map, data, out_slot)		((map) = rjd_slotmap_insert_impl((map), (out_slot)), \
													 (map)[(out_slot)->index] = data)
#define rjd_slotmap_get(map, slot)					((map)[rjd_slotmap_get_impl((map), (slot))])
#define rjd_slotmap_count(map)						(rjd_slotmap_count_impl(map))
#define rjd_slotmap_erase(map, slot)				(rjd_slotmap_erase_impl((map), slot))
#define rjd_slotmap_free(map)						(rjd_slotmap_free_impl(map))

void* rjd_slotmap_alloc_impl(size_t sizeof_type, uint32_t count, struct rjd_mem_allocator* allocator);
void* rjd_slotmap_insert_impl(void* map, struct rjd_slot* out_slot);
uint32_t rjd_slotmap_get_impl(const void* map, struct rjd_slot slot);
uint32_t rjd_slotmap_count_impl(const void* map);
void rjd_slotmap_erase_impl(void* map, struct rjd_slot slot);
void rjd_slotmap_free_impl(void* map);

#if RJD_IMPL

////////////////////////////////////////////////////////////////////////////////
// private interface

struct rjd_slotmap_header
{
	struct rjd_mem_allocator* allocator;
	uint32_t sizeof_type;
	uint32_t count;
	void* data;
	uint16_t* salts;
	uint32_t* freelist;
	uint32_t debug_sentinel;
};

enum {
	RJD_SLOTMAP_DEBUG_SENTINEL = 0x5A5A5A5A,
};

static struct rjd_slotmap_header* rjd_slotmap_getheader(const void* map);
static void* rjd_slotmap_grow(void* oldmap, size_t sizeof_type, uint32_t count, struct rjd_mem_allocator* allocator);

////////////////////////////////////////////////////////////////////////////////
// public implementation

void* rjd_slotmap_alloc_impl(size_t sizeof_type, uint32_t count, struct rjd_mem_allocator* allocator)
{
	return rjd_slotmap_grow(NULL, sizeof_type, count, allocator);
}

void* rjd_slotmap_insert_impl(void* map, struct rjd_slot* out_slot)
{
	RJD_ASSERT(map);
	RJD_ASSERT(out_slot);

	struct rjd_slotmap_header* header = rjd_slotmap_getheader(map);

	if (rjd_array_count(header->freelist) == 0) {
		map = rjd_slotmap_grow(map, header->sizeof_type, header->count * 2, header->allocator);
		header = rjd_slotmap_getheader(map);
		RJD_ASSERT(rjd_array_count(header->freelist) > 0);
	}

	uint32_t index = rjd_array_pop(header->freelist);

	uint16_t* salt = header->salts + index;
	*salt += 1;

	out_slot->index = rjd_math_truncate_u32_to_u16(index);
	out_slot->salt = *salt;

	return map;
}

uint32_t rjd_slotmap_get_impl(const void* map, struct rjd_slot slot)
{
	RJD_ASSERT(map);

	const struct rjd_slotmap_header* header = rjd_slotmap_getheader(map);
	RJD_ASSERT(slot.index < header->count);
	uint32_t index = slot.index;
	RJD_ASSERTMSG(!rjd_array_contains(header->freelist, index), "This slot is unallocated.");
	return slot.index;
}

uint32_t rjd_slotmap_count_impl(const void* map)
{
	RJD_ASSERT(map);

	const struct rjd_slotmap_header* header = rjd_slotmap_getheader(map);
	return header->count;
}

void rjd_slotmap_erase_impl(void* map, struct rjd_slot slot)
{
	RJD_ASSERT(map);
	struct rjd_slotmap_header* header = rjd_slotmap_getheader(map);
	RJD_ASSERT(slot.index < header->count);
	
	rjd_array_push(header->freelist, slot.index);
}

void rjd_slotmap_free_impl(void* map)
{
	RJD_ASSERT(map);
	struct rjd_slotmap_header* header = rjd_slotmap_getheader(map);

	rjd_array_free(header->freelist);
	rjd_mem_free(header);
}

////////////////////////////////////////////////////////////////////////////////
// private implementation

struct rjd_slotmap_header* rjd_slotmap_getheader(const void* map)
{
	char* raw = (char*)map;
	char* rawheader = (raw - sizeof(struct rjd_slotmap_header));
	struct rjd_slotmap_header* header = (struct rjd_slotmap_header*)rawheader;
	RJD_ASSERTMSG(header->debug_sentinel == RJD_SLOTMAP_DEBUG_SENTINEL, 
		"Debug sentinel does not match. Address %p does not point to a slotmap or there was a buffer underrun corruption.", map);
	return header;
}

void* rjd_slotmap_grow(void* oldmap, size_t sizeof_type, uint32_t count, struct rjd_mem_allocator* allocator)
{
	struct rjd_slotmap_header* oldheader = oldmap ? rjd_slotmap_getheader(oldmap) : NULL;

	uint32_t oldcount = oldheader ? oldheader->count : 0;
	if (count <= oldcount) {
		return oldmap;
	}

	size_t total_mem_size = sizeof(struct rjd_slotmap_header) + sizeof_type * count + sizeof(uint16_t) * count;
	char* mem = rjd_mem_alloc_array(char, total_mem_size, allocator);

	struct rjd_slotmap_header* header = (struct rjd_slotmap_header*)mem;
	header->allocator = allocator;
	header->sizeof_type = (uint32_t)sizeof_type;
	header->count = count;
	header->data = (void*)(mem + sizeof(struct rjd_slotmap_header));
	header->salts = (uint16_t*)((char*)header->data + sizeof_type * count);
	header->freelist = rjd_array_alloc(uint32_t, count, allocator);
	header->debug_sentinel = RJD_SLOTMAP_DEBUG_SENTINEL;

	memset(header->salts + oldcount, 0, (header->count - oldcount) * sizeof(*header->salts));

	for (uint32_t i = oldcount; i < count; ++i) {
		rjd_array_push(header->freelist, i);
	}

	// copy existing data
	if (oldheader) {
		memcpy(header->data, oldheader->data, sizeof_type * oldheader->count);
		memcpy(header->salts, oldheader->salts, sizeof(uint16_t) * oldheader->count);
		rjd_slotmap_free(oldmap);
	}

	return header->data;
}

#endif

