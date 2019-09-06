#pragma once

#define RJD_RESOURCE_TYPES_H 1

struct rjd_resource_id
{
	struct rjd_strhash hash;
};

struct rjd_resource_type_id
{
	struct rjd_strhash hash;
};

struct rjd_resource_handle
{
	struct rjd_slot slot;
};

static inline struct rjd_resource_id rjd_resource_id_from_str(const char* str);
static inline bool rjd_resource_id_equals(struct rjd_resource_id a, struct rjd_resource_id b);
static inline bool rjd_resource_id_isvalid(struct rjd_resource_id id);

static inline struct rjd_resource_type_id rjd_resource_type_id_from_str(const char* str);
static inline bool rjd_resource_type_id_equals(struct rjd_resource_type_id a, struct rjd_resource_type_id b);
static inline bool rjd_resource_type_id_isvalid(struct rjd_resource_type_id id);

static inline bool rjd_resource_handle_equals(struct rjd_resource_handle a, struct rjd_resource_handle b);
static inline bool rjd_resource_handle_isvalid(struct rjd_resource_handle handle);

////////////////////////////////////////////////////////////////////////////////
// static inline implementation

static inline struct rjd_resource_id rjd_resource_id_from_str(const char* str)
{
	struct rjd_resource_id id = {
		.hash = rjd_strhash_init(str),
	};
	return id;
}

static inline bool rjd_resource_id_equals(struct rjd_resource_id a, struct rjd_resource_id b)
{
	return a.hash.hash.value == b.hash.hash.value;
}

static inline bool rjd_resource_id_isvalid(struct rjd_resource_id id)
{
	return id.hash.hash.value != 0;
}

static inline struct rjd_resource_type_id rjd_resource_type_id_from_str(const char* str)
{
	struct rjd_resource_type_id id = {
		.hash = rjd_strhash_init(str),
	};
	return id;
}

static inline bool rjd_resource_type_id_equals(struct rjd_resource_type_id a, struct rjd_resource_type_id b)
{
	return a.hash.hash.value == b.hash.hash.value;
}

static inline bool rjd_resource_type_id_isvalid(struct rjd_resource_type_id id)
{
	return id.hash.hash.value != 0;
}

static inline bool rjd_resource_handle_equals(struct rjd_resource_handle a, struct rjd_resource_handle b)
{
	return a.slot.index == b.slot.index &&
			a.slot.salt == b.slot.salt;
}

static inline bool rjd_resource_handle_isvalid(struct rjd_resource_handle handle)
{
	return rjd_slot_isvalid(handle.slot);
}

