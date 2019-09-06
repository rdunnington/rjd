#pragma once

#define RJD_RESOURCE_LOADER_H 1

struct rjd_resource_loader;

typedef void rjd_resource_loader_destroy_func(struct rjd_resource_loader* loader);
typedef struct rjd_result rjd_resource_loader_get_type_func(struct rjd_resource_loader* loader, struct rjd_resource_id id, struct rjd_resource_type_id* out);
typedef struct rjd_result rjd_resource_loader_load_func(struct rjd_resource_loader* loader, struct rjd_resource_id id, struct rjd_mem_allocator* allocator, int8_t** out);

struct rjd_resource_loader
{
	void* impl;
	rjd_resource_loader_destroy_func* destroy_func;
	rjd_resource_loader_get_type_func* get_type_func;
	rjd_resource_loader_load_func* load_func;
};

enum rjd_resource_loader_type
{
	RJD_RESOURCE_LOADER_TYPE_FILESYSTEM, // recursively crawls a root directory to find all resource files
	//RJD_RESOURCE_LOADER_TYPE_REMOTE, // requests manifest from a given http endpoint
	//RJD_RESOURCE_LOADER_TYPE_PACK, // reads manifest embedded inside given pack files
};

struct rjd_resource_extension_to_type_id
{
	struct rjd_resource_type_id type;
	const char* extension;
};

struct rjd_resource_loader_desc
{
	enum rjd_resource_loader_type type;
	struct rjd_mem_allocator* allocator;
	union
	{
		struct {
			const char* root;
			const struct rjd_resource_extension_to_type_id* type_mappings;
			uint32_t type_mappings_count;
			uint32_t manifest_capacity;
		} filesystem;
	};
};

struct rjd_result rjd_resource_loader_create(struct rjd_resource_loader* out, struct rjd_resource_loader_desc desc);
static inline void rjd_resource_loader_destroy(struct rjd_resource_loader* loader);
static inline struct rjd_result rjd_resource_loader_get_type(struct rjd_resource_loader* loader, struct rjd_resource_id id, struct rjd_resource_type_id* out);
static inline struct rjd_result rjd_resource_loader_load(struct rjd_resource_loader* loader, struct rjd_resource_id id, struct rjd_mem_allocator* allocator, int8_t** out);
// TODO update load() to return an rjd_stream instead of loading the entire file at once?

////////////////////////////////////////////////////////////////////////////////
// Inline implementation

static inline void rjd_resource_loader_destroy(struct rjd_resource_loader* loader)
{
	RJD_ASSERT(loader);
	RJD_ASSERT(loader->destroy_func);

	loader->destroy_func(loader);
}

static inline struct rjd_result rjd_resource_loader_get_type(struct rjd_resource_loader* loader, struct rjd_resource_id id, struct rjd_resource_type_id* out)
{
	RJD_ASSERT(loader);
	RJD_ASSERT(loader->get_type_func);

	return loader->get_type_func(loader, id, out);
}

static inline struct rjd_result rjd_resource_loader_load(struct rjd_resource_loader* loader, struct rjd_resource_id id, struct rjd_mem_allocator* allocator, int8_t** out)
{
	RJD_ASSERT(loader);
	RJD_ASSERT(loader->load_func);

	return loader->load_func(loader, id, allocator, out);
}

#if RJD_IMPL

struct rjd_resource_manifest_entry_filesystem
{
	struct rjd_resource_id id;
	struct rjd_resource_type_id type;
	struct rjd_strref* path;
};

// private types
struct rjd_resource_loader_filesystem
{
	enum rjd_resource_loader_type debug_sentinel;
	struct rjd_strpool strpool;
	struct rjd_resource_extension_to_type_id* type_mappings;
	struct rjd_strref* root;
	struct rjd_resource_manifest_entry_filesystem* manifest_entries; // TODO could be sorted by resource id
};

// static helpers

static struct rjd_resource_loader_filesystem* rjd_resource_loader_to_filesystem_loader(struct rjd_resource_loader* loader);
static void rjd_resource_loader_filesystem_destroy(struct rjd_resource_loader* loader);
static struct rjd_result rjd_resource_loader_filesystem_get_type(struct rjd_resource_loader* loader, struct rjd_resource_id id, struct rjd_resource_type_id* out);
static struct rjd_result rjd_resource_loader_filesystem_load(struct rjd_resource_loader* loader, struct rjd_resource_id id, struct rjd_mem_allocator* allocator, int8_t** out);

// public implementation

struct rjd_result rjd_resource_loader_create(struct rjd_resource_loader* out, struct rjd_resource_loader_desc desc)
{
	void* impl_any = NULL;

	if (desc.type == RJD_RESOURCE_LOADER_TYPE_FILESYSTEM)
	{
		uint32_t manifest_capacity = desc.filesystem.manifest_capacity == 0 ? 1024 : desc.filesystem.manifest_capacity;

		struct rjd_resource_loader_filesystem* impl = rjd_mem_alloc(struct rjd_resource_loader_filesystem, desc.allocator);
		impl->debug_sentinel = RJD_RESOURCE_LOADER_TYPE_FILESYSTEM;
		impl->strpool = rjd_strpool_init(desc.allocator, 64);
		impl->type_mappings = rjd_array_alloc(struct rjd_resource_extension_to_type_id, desc.filesystem.type_mappings_count, desc.allocator);
		impl->root = rjd_strpool_add(&impl->strpool, desc.filesystem.root);
		impl->manifest_entries = rjd_array_alloc(struct rjd_resource_manifest_entry_filesystem, manifest_capacity, desc.allocator);

		for (uint32_t i = 0; i < desc.filesystem.type_mappings_count; ++i)
		{
			rjd_array_push(impl->type_mappings, desc.filesystem.type_mappings[i]);
		}

		struct rjd_path_enumerator_state path_enumerator = rjd_path_enumerate_create(desc.filesystem.root);
		for (const char* path = rjd_path_enumerate_next(&path_enumerator); path != NULL; path = rjd_path_enumerate_next(&path_enumerator))
		{
			const char* extension = rjd_path_extension_str(path);
			if (extension)
			{
				struct rjd_resource_type_id type = {0};
				for (uint32_t i = 0; i < rjd_array_count(impl->type_mappings); ++i)
				{
					if (strcmp(impl->type_mappings[i].extension, extension) == 0) // TODO case-insensitive compare
					{
						type = impl->type_mappings[i].type;
					}
				}

				if (type.hash.hash.value != 0)
				{
					struct rjd_strref* pathref = rjd_strpool_add(&impl->strpool, path);
					struct rjd_resource_manifest_entry_filesystem entry = {
						.id = rjd_strhash_init(path),
						.type = type,
						.path = pathref,
					};
					rjd_array_push(impl->manifest_entries, entry);
				}
			}
		}

		rjd_path_enumerate_destroy(&path_enumerator);

		impl_any = impl;
	}
	else
	{
		return RJD_RESULT("unimplemented support for this type of loader");
	}

	struct rjd_resource_loader loader = {
		.impl = impl_any,
		.destroy_func = rjd_resource_loader_filesystem_destroy,
		.get_type_func = rjd_resource_loader_filesystem_get_type,
		.load_func = rjd_resource_loader_filesystem_load,
	};
	*out = loader;

	return RJD_RESULT_OK();
}

// private implementation

static struct rjd_resource_loader_filesystem* rjd_resource_loader_to_filesystem_loader(struct rjd_resource_loader* loader)
{
	RJD_ASSERT(loader);

	struct rjd_resource_loader_filesystem* impl = (struct rjd_resource_loader_filesystem*)loader->impl;
	RJD_ASSERT(impl->debug_sentinel == RJD_RESOURCE_LOADER_TYPE_FILESYSTEM);
	return impl;
}

static void rjd_resource_loader_filesystem_destroy(struct rjd_resource_loader* loader)
{
	struct rjd_resource_loader_filesystem* impl = rjd_resource_loader_to_filesystem_loader(loader);
	rjd_array_free(impl->manifest_entries);
	rjd_array_free(impl->type_mappings);
	rjd_strpool_free(&impl->strpool);
	rjd_mem_free(impl);
	memset(loader, 0, sizeof(*loader));
}

static struct rjd_result rjd_resource_loader_filesystem_get_type(struct rjd_resource_loader* loader, struct rjd_resource_id id, struct rjd_resource_type_id* out)
{
	struct rjd_resource_loader_filesystem* impl = rjd_resource_loader_to_filesystem_loader(loader);
	for (uint32_t i = 0; i < rjd_array_count(impl->manifest_entries); ++i) {
		if (rjd_resource_id_equals(impl->manifest_entries[i].id, id)) {
			*out = impl->manifest_entries[i].type;
			return RJD_RESULT_OK();
		}
	}

	return RJD_RESULT("Resource id not found in loader manifest.");
}

static struct rjd_result rjd_resource_loader_filesystem_load(struct rjd_resource_loader* loader, struct rjd_resource_id id, struct rjd_mem_allocator* allocator, int8_t** out)
{
	struct rjd_resource_loader_filesystem* impl = rjd_resource_loader_to_filesystem_loader(loader);
	for (uint32_t i = 0; i < rjd_array_count(impl->manifest_entries); ++i) {
		if (rjd_resource_id_equals(impl->manifest_entries[i].id, id)) {
            const char* root = rjd_strref_str(impl->root);
            const char* relative_path = rjd_strref_str(impl->manifest_entries[i].path);
            
            struct rjd_path fullpath = rjd_path_create();
            rjd_path_append(&fullpath, root);
            rjd_path_append(&fullpath, relative_path);
            
			struct rjd_result result = rjd_fio_read(rjd_path_get(&fullpath), (char**)out, allocator);
			return result;
		}
	}

	return RJD_RESULT("Resource id not found in loader manifest.");
}

#endif // RJD_IMPL

