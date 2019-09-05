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

static inline bool rjd_resource_id_equals(struct rjd_resource_id a, struct rjd_resource_id b);
static inline bool rjd_resource_id_none(struct rjd_resource_id id);
static inline bool rjd_resource_type_id_equals(struct rjd_resource_type_id a, struct rjd_resource_type_id b);
static inline bool rjd_resource_type_id_none(struct rjd_resource_type_id id);
static inline bool rjd_resource_handle_equals(struct rjd_resource_handle a, struct rjd_resource_handle b);
static inline bool rjd_resource_handle_none(struct rjd_resource_handle handle);

////////////////////////////////////////////////////////////////////////////////
// static inline implementation

static inline bool rjd_resource_id_equals(struct rjd_resource_id a, struct rjd_resource_id b)
{
	return a.hash.hash.value == b.hash.hash.value;
}

static inline bool rjd_resource_id_none(struct rjd_resource_id id)
{
	return id.hash.hash.value == 0;
}

static inline bool rjd_resource_type_id_equals(struct rjd_resource_type_id a, struct rjd_resource_type_id b)
{
	return a.hash.hash.value == b.hash.hash.value;
}

static inline bool rjd_resource_type_id_none(struct rjd_resource_type_id id)
{
	return id.hash.hash.value == 0;
}

static inline bool rjd_resource_handle_equals(struct rjd_resource_handle a, struct rjd_resource_handle b)
{
	return a.slot.index == b.slot.index &&
			a.slot.salt == b.slot.salt;
}

static inline bool rjd_resource_handle_none(struct rjd_resource_handle handle)
{
	return !rjd_slot_isvalid(handle.slot);
}

