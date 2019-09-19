#pragma once

#define RJD_RESOURCE_H 1

// TODO update all the RJD_LOGs to a resource channel
struct rjd_result;
struct rjd_resource_type;
struct rjd_resource;
struct rjd_resource_loader;
struct rjd_resource_load_begin_params;
struct rjd_resource_load_end_params;
struct rjd_resource_unload_params;
struct rjd_resource_load_dependency_params;
struct rjd_resource_get_dependency_params;

enum rjd_resource_status
{
	RJD_RESOURCE_STATUS_INVALID,
	RJD_RESOURCE_STATUS_LOAD_BEGIN,
	RJD_RESOURCE_STATUS_LOAD_RESOLVE,
	RJD_RESOURCE_STATUS_LOAD_END,
	RJD_RESOURCE_STATUS_FAILED,
	RJD_RESOURCE_STATUS_READY,
};

struct rjd_resource_dependency
{
    enum rjd_resource_status status;
    void* typed_resource_data;
};

// Loading takes place in 2 passes, where resources get a chance to declare any dependencies they want loaded before
// their load function gets called.
typedef struct rjd_result rjd_resource_load_begin_func(struct rjd_resource_load_begin_params* params);
typedef struct rjd_result rjd_resource_load_end_func(struct rjd_resource_load_end_params* params);
typedef void rjd_resource_unload_func(struct rjd_resource_unload_params* params);
typedef struct rjd_result rjd_resource_add_dependency_func(struct rjd_resource_load_dependency_params* params, struct rjd_resource_id dependency, struct rjd_resource_handle* child);
typedef struct rjd_resource_dependency rjd_resource_get_dependency_func(struct rjd_resource_get_dependency_params* params, struct rjd_resource_handle handle);

struct rjd_resource_load_begin_params
{
	struct rjd_istream stream_data;
	uint32_t filesize;
	void* typed_resource_data;
	void* userdata;
	struct rjd_mem_allocator* allocator;
	struct rjd_mem_allocator* scratch_allocator;

	// for loading dependent resources
	rjd_resource_add_dependency_func* load_dependency_func;
	struct rjd_resource_load_dependency_params* dependency_params;
};

struct rjd_resource_load_end_params
{
	void* typed_resource_data;
	void* userdata;
	struct rjd_mem_allocator* allocator;
	struct rjd_mem_allocator* scratch_allocator;

	struct rjd_resource_get_dependency_params* get_dependency_params;
	rjd_resource_get_dependency_func* get_dependency_func;
};

struct rjd_resource_unload_params
{
	void* typed_resource_data;
	void* userdata;

	struct rjd_resource_get_dependency_params* get_dependency_params;
	rjd_resource_get_dependency_func* get_dependency_func;
};

struct rjd_resource_type
{
	struct rjd_resource_type_id id;
	struct rjd_mem_allocator* optional_allocator;
	void* userdata;
	rjd_resource_load_begin_func* load_begin_func;
	rjd_resource_load_end_func* optional_load_end_func;
	rjd_resource_unload_func* optional_unload_func;
	uint32_t in_memory_size;
};

struct rjd_resource_lib_desc
{
	struct rjd_mem_allocator* allocator;
	struct rjd_resource_loader* loader;

	struct rjd_resource_type* types;
	uint32_t count_types;
	uint32_t initial_capacity_types;
	uint32_t initial_capacity_resources;
	uint32_t initial_capacity_buffers;
};

struct rjd_resource_lib
{
	struct rjd_mem_allocator* allocator;
	struct rjd_resource_loader* loader;
	struct rjd_resource_type* registered_types; // should only be accessed from one thread, TODO enforce
	struct rjd_resource* resources; // TODO make threadsafe

	// TODO make these threadsafe queues
	struct {
		struct rjd_resource_handle* begin;
		struct rjd_resource_handle* resolving_dependencies;
		struct rjd_resource_handle* end;
	} load_stage_queues;

	struct rjd_resource_handle* unload_queue;
};

struct rjd_resource_id rjd_resource_id_from_path(const char* path);

void rjd_resource_lib_create(struct rjd_resource_lib* lib, struct rjd_resource_lib_desc desc);
struct rjd_result rjd_resource_lib_destroy(struct rjd_resource_lib* lib);
struct rjd_result rjd_resource_lib_register_type(struct rjd_resource_lib* lib, struct rjd_resource_type type);
// TODO have the system return fallback handles for resources that failed to load
//struct rjd_result rjd_resource_lib_register_fallback(struct rjd_resource_lib* lib, struct rjd_resource_type type, struct rjd_resource_handle fallback);
void rjd_resource_lib_pump(struct rjd_resource_lib* lib, struct rjd_mem_allocator* scratch_allocator);
void rjd_resource_lib_wait(struct rjd_resource_lib* lib, struct rjd_resource_handle* resources, size_t count);
void rjd_resource_lib_waitall(struct rjd_resource_lib* lib);

struct rjd_result rjd_resource_load(struct rjd_resource_lib* lib, struct rjd_resource_id id, struct rjd_resource_handle* out);
enum rjd_resource_status rjd_resource_lib_status(struct rjd_resource_lib* lib, struct rjd_resource_handle handle);
bool rjd_resource_is_loading(struct rjd_resource_lib* lib, struct rjd_resource_handle handle);
void* rjd_resource_get(struct rjd_resource_lib* lib, struct rjd_resource_handle id);
void rjd_resource_unload(struct rjd_resource_lib* lib, struct rjd_resource_handle handle);

#if RJD_IMPL

// NOTE: We explicitly don't have refcounting in this system. Clients can build their own refcount on top of this if they want.
struct rjd_resource
{
	struct rjd_resource_id id;
	void* typed_resource_data;
	struct rjd_resource_handle* dependencies;
	enum rjd_resource_status status;
	uint32_t refcount; // TODO atomic
	uint32_t registry_index;
};

struct rjd_resource_load_dependency_params
{
	struct rjd_resource_lib* lib;
	struct rjd_resource_handle parent;
};

static struct rjd_result rjd_resource_add_dependency(struct rjd_resource_load_dependency_params* params, struct rjd_resource_id id, struct rjd_resource_handle* child);
static struct rjd_resource_dependency rjd_resource_get_dependency(struct rjd_resource_get_dependency_params* params, struct rjd_resource_handle handle);

void rjd_resource_lib_create(struct rjd_resource_lib* lib, struct rjd_resource_lib_desc desc)
{
	if (desc.initial_capacity_types == 0) {
		desc.initial_capacity_types = 32;
	}

	if (desc.initial_capacity_resources == 0) {
		desc.initial_capacity_resources = 256;
	}

	if (desc.initial_capacity_buffers == 0) {
		desc.initial_capacity_buffers = 64;
	}

	RJD_ASSERTMSG(desc.loader, "You must create a loader with rjd_resource_loader_create() and set it in rjd_resource_lib_desc");

	lib->allocator = desc.allocator;
	lib->loader = desc.loader;
	lib->registered_types = rjd_array_alloc(struct rjd_resource_type, desc.initial_capacity_types, desc.allocator);
	lib->resources = rjd_slotmap_alloc(struct rjd_resource, desc.initial_capacity_resources, desc.allocator);
	lib->load_stage_queues.begin = rjd_array_alloc(struct rjd_resource_handle, desc.initial_capacity_buffers, desc.allocator);
	lib->load_stage_queues.resolving_dependencies = rjd_array_alloc(struct rjd_resource_handle, desc.initial_capacity_buffers, desc.allocator);
	lib->load_stage_queues.end = rjd_array_alloc(struct rjd_resource_handle, desc.initial_capacity_buffers, desc.allocator);
	lib->unload_queue = rjd_array_alloc(struct rjd_resource_handle, desc.initial_capacity_buffers, desc.allocator);
}

struct rjd_result rjd_resource_lib_destroy(struct rjd_resource_lib* lib)
{
	RJD_ASSERT(lib);

	if (rjd_slotmap_count(lib->resources) > 0) {
		return RJD_RESULT("You must unload all resources before destroying the lib");
	}

	rjd_array_free(lib->unload_queue);
	rjd_array_free(lib->load_stage_queues.end);
	rjd_array_free(lib->load_stage_queues.resolving_dependencies);
	rjd_array_free(lib->load_stage_queues.begin);
	rjd_slotmap_free(lib->resources);
	rjd_array_free(lib->registered_types);
	rjd_resource_loader_destroy(lib->loader);

	return RJD_RESULT_OK();
}

struct rjd_result rjd_resource_lib_register_type(struct rjd_resource_lib* lib, struct rjd_resource_type type)
{
	RJD_ASSERT(lib);
	RJD_ASSERT(type.load_begin_func);
	RJD_ASSERT(type.in_memory_size > 0);

	for (uint32_t i = 0; i < rjd_array_count(lib->registered_types); ++i)
	{
		if (rjd_strhash_isequal(lib->registered_types[i].id.hash, type.id.hash))
		{
			return RJD_RESULT("Resource type has already been registered.");
		}
	}

	rjd_array_push(lib->registered_types, type);

	return RJD_RESULT_OK();
}

void rjd_resource_lib_pump(struct rjd_resource_lib* lib, struct rjd_mem_allocator* scratch_allocator)
{
	RJD_ASSERT(lib);

	if (!rjd_array_empty(lib->load_stage_queues.end)) {
		struct rjd_resource_handle handle = rjd_array_pop(lib->load_stage_queues.end);
		struct rjd_resource* resource = rjd_slotmap_get(lib->resources, handle.slot);
		RJD_ASSERT(resource);
		RJD_ASSERT(resource->status == RJD_RESOURCE_STATUS_LOAD_END);
	
		struct rjd_result result = RJD_RESULT_OK();
		const struct rjd_resource_type* type = lib->registered_types + resource->registry_index;
		if (type->optional_load_end_func) {
			struct rjd_resource_load_end_params params = {
				.typed_resource_data = resource->typed_resource_data,
				.userdata = type->userdata,
				.allocator = lib->allocator,
				.scratch_allocator = scratch_allocator,
				.get_dependency_params = (struct rjd_resource_get_dependency_params*)lib,
				.get_dependency_func = rjd_resource_get_dependency,
			};
			result = type->optional_load_end_func(&params);
		}
	
		if (rjd_result_isok(result)) {
			resource->status = RJD_RESOURCE_STATUS_READY;
			//RJD_LOG("Loaded resource '%s'", resource->filepath);
		} else {
			resource->status = RJD_RESOURCE_STATUS_FAILED;
			//RJD_LOG("Failed ending load for resource '%s'", resource->filepath);
		}
	}

	if (!rjd_array_empty(lib->load_stage_queues.resolving_dependencies)) {
		struct rjd_resource_handle handle = rjd_array_last(lib->load_stage_queues.resolving_dependencies);
		struct rjd_resource* res = rjd_slotmap_get(lib->resources, handle.slot);
		RJD_ASSERT(res);

		bool all_loaded = true;
		for (uint32_t i = 0; i < rjd_array_count(res->dependencies); ++i)
		{
			struct rjd_resource_handle handle = res->dependencies[i];
			struct rjd_resource* dependency = rjd_slotmap_get(lib->resources, handle.slot);
			RJD_ASSERT(dependency);
            // failures to load dependencies don't automatically fail resources; we leave it up to
            // the resource to decide what to do in the end stage
            // TODO use the "backup resource" as a replacement?
			if (dependency->status != RJD_RESOURCE_STATUS_READY && dependency->status != RJD_RESOURCE_STATUS_FAILED) {
				all_loaded = false;
				break;
			}
		}

		if (all_loaded) {
			struct rjd_resource_handle handle = rjd_array_pop(lib->load_stage_queues.resolving_dependencies);
            res->status = RJD_RESOURCE_STATUS_LOAD_END;
			rjd_array_push(lib->load_stage_queues.end, handle);
		}
	}

	if (!rjd_array_empty(lib->load_stage_queues.begin)) {
		struct rjd_resource_handle handle = rjd_array_pop(lib->load_stage_queues.begin);
		struct rjd_resource* resource = rjd_slotmap_get(lib->resources, handle.slot);
		RJD_ASSERT(resource);
		RJD_ASSERT(resource->status == RJD_RESOURCE_STATUS_LOAD_BEGIN);
	
		struct rjd_istream stream = {0};
		struct rjd_result result = rjd_resource_loader_load(lib->loader, resource->id, scratch_allocator, &stream);
		if (rjd_result_isok(result))
		{
	        struct rjd_resource_type* type = rjd_array_get(lib->registered_types, resource->registry_index);
	        struct rjd_mem_allocator* resource_allocator = type->optional_allocator ? type->optional_allocator : lib->allocator;
	        
	        resource->typed_resource_data = rjd_mem_alloc_array(uint8_t, type->in_memory_size, resource_allocator);
	        memset(resource->typed_resource_data, 0, type->in_memory_size);
	        
	        struct rjd_result result = RJD_RESULT_OK();
			if (type->load_begin_func)
			{
				struct rjd_resource_load_dependency_params dependency_params = {
					.lib = lib,
					.parent = handle,
				};
	
				struct rjd_resource_load_begin_params load_params = {
					.stream_data = stream,
					.typed_resource_data = resource->typed_resource_data,
					.userdata = type->userdata,
					.allocator = resource_allocator,
					.scratch_allocator = scratch_allocator,
					.load_dependency_func = &rjd_resource_add_dependency,
					.dependency_params = &dependency_params,
				};
	
				result = type->load_begin_func(&load_params);
			}
	
	        if (rjd_result_isok(result)) {
	            resource->status = RJD_RESOURCE_STATUS_LOAD_RESOLVE;
	            rjd_array_push(lib->load_stage_queues.resolving_dependencies, handle);
	        } else {
	            //RJD_LOG("Failed beginning load for resource '%s': %s", resource->filepath, result.error);
	            resource->status = RJD_RESOURCE_STATUS_FAILED;
	        }
	
			rjd_istream_close(&stream);
		}
		else
		{
			//RJD_LOG("Failed loading resource '%s': %s", resource_id.hash.debug_string, result.error);
			resource->status = RJD_RESOURCE_STATUS_FAILED;
		}
	}

	if (!rjd_array_empty(lib->unload_queue)) {
		struct rjd_resource_handle handle = rjd_array_pop(lib->unload_queue);
		struct rjd_resource* resource = rjd_slotmap_get(lib->resources, handle.slot);

		// The resource could have been already freed if it was inserted into the unload queue more than once
		if (resource)
		{
			struct rjd_resource_type* type = rjd_array_get(lib->registered_types, resource->registry_index);
			if (type->optional_unload_func) {
				struct rjd_resource_unload_params params = {
					.typed_resource_data = resource->typed_resource_data,
					.userdata = type->userdata,
					.get_dependency_params = (struct rjd_resource_get_dependency_params*)lib,
					.get_dependency_func = rjd_resource_get_dependency,
				};
				type->optional_unload_func(&params);
			}
            rjd_mem_free(resource->typed_resource_data);
            rjd_array_free(resource->dependencies);
			rjd_slotmap_erase(lib->resources, handle.slot);
		}
	}

	// TODO get change notifications from loader to insert resources for reload
	// NOTE make sure to preserve the original resource until the reload is complete!
}

void rjd_resource_lib_wait(struct rjd_resource_lib* lib, struct rjd_resource_handle* resources, size_t count)
{
	const size_t K_SCRATCH_SIZE = RJD_MB * 4;
	void* scratch_buffer = rjd_mem_alloc_array_noclear(char, K_SCRATCH_SIZE, lib->allocator);
	struct rjd_mem_allocator scratch_allocator = rjd_mem_allocator_init_linear(scratch_buffer, K_SCRATCH_SIZE);

	for (uint32_t i = 0; i < count; ++i)
	{
		while (rjd_resource_is_loading(lib, resources[i])) {
			rjd_resource_lib_pump(lib, &scratch_allocator);
			rjd_mem_allocator_reset(&scratch_allocator);
		}
	}

	rjd_mem_free(scratch_buffer);
}

void rjd_resource_lib_waitall(struct rjd_resource_lib* lib)
{
	RJD_ASSERT(lib);

	const size_t K_SCRATCH_SIZE = RJD_MB * 4;
	void* scratch_buffer = rjd_mem_alloc_array_noclear(char, K_SCRATCH_SIZE, lib->allocator);
	struct rjd_mem_allocator scratch_allocator = rjd_mem_allocator_init_linear(scratch_buffer, K_SCRATCH_SIZE);

	while (!rjd_array_empty(lib->load_stage_queues.begin) ||
			!rjd_array_empty(lib->load_stage_queues.resolving_dependencies) ||
			!rjd_array_empty(lib->load_stage_queues.end) ||
            !rjd_array_empty(lib->unload_queue))
	{
		rjd_resource_lib_pump(lib, &scratch_allocator);
		rjd_mem_allocator_reset(&scratch_allocator);
	}

	rjd_mem_free(scratch_buffer);
}

struct rjd_result rjd_resource_load(struct rjd_resource_lib* lib, struct rjd_resource_id id, struct rjd_resource_handle* out)
{
	RJD_ASSERT(lib);
	RJD_ASSERT(out);

	// exists already
	for (struct rjd_slot s = rjd_slotmap_next(lib->resources, NULL); rjd_slot_isvalid(s); s = rjd_slotmap_next(lib->resources, &s))
	{
		struct rjd_resource* resource = rjd_slotmap_get(lib->resources, s);
		if (rjd_resource_id_equals(resource->id, id)) {
			++resource->refcount;
			out->slot = s;
			return RJD_RESULT_OK();
		}
	}

	// cancel unload
	for (uint32_t i = 0; i < rjd_array_count(lib->unload_queue); ++i)
	{
		struct rjd_resource_handle handle = lib->unload_queue[i];
		struct rjd_resource* resource = rjd_slotmap_get(lib->resources, handle.slot);
		if (rjd_resource_id_equals(resource->id, id)) {
			rjd_array_erase_unordered(lib->resources, i);
			++resource->refcount;
			*out = handle;
			return RJD_RESULT_OK();
		}
	}

	// kickoff the load
	struct rjd_resource_type_id type = {0};
	struct rjd_result result = rjd_resource_loader_get_type(lib->loader, id, &type);
	if (rjd_result_isok(result))
	{
		int32_t registry_index = RJD_ARRAY_NOT_FOUND;
		for (uint32_t i = 0; i < rjd_array_count(lib->registered_types); ++i) {
			if (rjd_resource_type_id_equals(lib->registered_types[i].id, type)) {
				registry_index = (int32_t)i;
				break;
			}
		}

		if (registry_index == RJD_ARRAY_NOT_FOUND) {
			return RJD_RESULT("No resource type was found for the given type id. Did you forget to register it?");
		} else {
			struct rjd_resource resource = {
				.id = id,
				.status = RJD_RESOURCE_STATUS_LOAD_BEGIN,
				.registry_index = registry_index,
				.dependencies = NULL,
				.typed_resource_data = NULL,
				.refcount = 1,
			};

			rjd_slotmap_insert(lib->resources, resource, &out->slot);

			rjd_array_push(lib->load_stage_queues.begin, *out);

			result = RJD_RESULT_OK();
		}
	}

	return result;
}

enum rjd_resource_status rjd_resource_lib_status(struct rjd_resource_lib* lib, struct rjd_resource_handle handle)
{
	RJD_ASSERT(lib);

    if (rjd_slot_isvalid(handle.slot))
    {
        if (rjd_slotmap_contains(lib->resources, handle.slot)) {
            struct rjd_resource* res = rjd_slotmap_get(lib->resources, handle.slot);
            if (res) {
                return res->status;
            }
        }
    }
	return RJD_RESOURCE_STATUS_INVALID;
}

bool rjd_resource_is_loading(struct rjd_resource_lib* lib, struct rjd_resource_handle handle)
{
	enum rjd_resource_status status = rjd_resource_lib_status(lib, handle);
	return status == RJD_RESOURCE_STATUS_LOAD_BEGIN ||
			status == RJD_RESOURCE_STATUS_LOAD_RESOLVE ||
			status == RJD_RESOURCE_STATUS_LOAD_END;
}

void* rjd_resource_get(struct rjd_resource_lib* lib, struct rjd_resource_handle handle)
{
	RJD_ASSERT(lib);

	struct rjd_resource* resource = rjd_slotmap_get(lib->resources, handle.slot);
	if (resource && resource->status == RJD_RESOURCE_STATUS_READY) {
		return resource->typed_resource_data;
	}
	return NULL;
}

void rjd_resource_unload(struct rjd_resource_lib* lib, struct rjd_resource_handle handle)
{
	RJD_ASSERT(lib);

	struct rjd_resource* resource = rjd_slotmap_get(lib->resources, handle.slot);
	if (resource) {
		--resource->refcount;
		if (resource->refcount == 0) {
			rjd_array_push(lib->unload_queue, handle);
			for (uint32_t i = 0; i < rjd_array_count(resource->dependencies); ++i) {
				rjd_resource_unload(lib, resource->dependencies[i]);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// private implementation

static struct rjd_result rjd_resource_add_dependency(struct rjd_resource_load_dependency_params* params, struct rjd_resource_id id, struct rjd_resource_handle* child)
{
	RJD_ASSERT(params);

	struct rjd_resource* parent = rjd_slotmap_get(params->lib->resources, params->parent.slot);
	RJD_ASSERT(parent);

	struct rjd_result result = rjd_resource_load(params->lib, id, child);
	if (rjd_result_isok(result)) {
        if (parent->dependencies == NULL) {
            parent->dependencies = rjd_array_alloc(struct rjd_resource_handle, 8, params->lib->allocator);
        }
		rjd_array_push(parent->dependencies, *child);
	}

	return result;
}

static struct rjd_resource_dependency rjd_resource_get_dependency(struct rjd_resource_get_dependency_params* params, struct rjd_resource_handle handle)
{
	struct rjd_resource_lib* lib = (struct rjd_resource_lib*)params;
    enum rjd_resource_status status = rjd_resource_lib_status(lib, handle);
	void* typed_resource_data = rjd_resource_get(lib, handle);
    
    struct rjd_resource_dependency dependency = {
        .status = status,
        .typed_resource_data = typed_resource_data,
    };
    return dependency;
}

#endif // RJD_IMPL

