#pragma once

#define RJD_DCC_H 1

struct rjd_dcc_obj_object
{
	
};

struct rjd_dcc_obj_group
{
	struct rjd_strhash name;
};

struct rjd_dcc_obj
{
	struct rjd_strpool stringpool;
};

struct rjd_result rjd_dcc_obj_create(struct rjd_dcc_obj* out, const char* stream, size_t length, struct rjd_mem_allocator* allocator);
void rjd_dcc_obj_destroy(struct rjd_dcc_obj* obj);

#if RJD_IMPL

/////////////////////////////////////////////////////////////////
// 

enum token_type
{
	TOKEN_TYPE_INVALID,
	TOKEN_TYPE_MATERIAL_LIB,
	TOKEN_TYPE_MATERIAL_USE,
	TOKEN_TYPE_GROUP_NAME,
	TOKEN_TYPE_GROUP_SMOOTH,
	TOKEN_TYPE_VERTEX,
	TOKEN_TYPE_TEXCOORD,
	TOKEN_TYPE_NORMAL,
	TOKEN_TYPE_FACE,
	TOKEN_TYPE_COUNT,
};

struct token_material_lib
{
	struct rjd_strref* name;
};

struct token_material_use
{
	struct rjd_strref* name;
};

struct token_group_name
{
	struct rjd_strref* name;
};

struct token_group_smooth
{
	uint32_t id;
};

struct token_vertex
{
	float x;
	float y;
	float z;
	float w;
};

struct token_texcoord
{
	float u;
	float v;
	float w;
};

struct token_normal
{
	float i;
	float j;
	float k;
};

struct face_indices
{
	int32_t vertex; // if -1, no data
	int32_t texcoord; // optional, can be -1 if no data
	int32_t normal; // optional, can be -1 if no data
};

struct token_face
{
	struct face_indices indices[4]; // TODO dynamic allocation?
};

struct token
{
	enum token_type type;
	union
	{
		struct token_material_lib material_lib;
		struct token_material_use material_use;
		struct token_group_name group_name;
		struct token_group_smooth group_smooth;
		struct token_vertex vertex;
		struct token_normal normal;
		struct token_texcoord texcoord;
		struct token_face face;
	};
};

const char* rjd_gfx_dcc_obj_find_value_end(const char* stream, size_t length) {
	for (size_t i = 0; i < length && stream && *stream; ++i) {
		if (isspace(stream[i])) {
			return stream + i;
		}
	}

	return stream + length;
}

const char* rjd_gfx_dcc_obj_find_next_token(const char* stream, size_t length) {
	bool looking_for_end = true;
	for (size_t i = 0; i < length && stream && *stream; ++i) {
		if (looking_for_end) {
			if (stream[i] == '\n') {
				looking_for_end = false;
			}
		} else {
			if (!isspace(stream[i])) {
				return stream + i;
			}
		}
	}

	return stream + length;
}

const char* rjd_gfx_dcc_obj_find_next_value(const char* stream, size_t length)
{
	bool looking_for_space = true;
	for (size_t i = 0; i < length; ++i) {
		if (stream[i] == '\n') {
			return stream + i;
		}

		if (looking_for_space) {
			if (isspace(stream[i])) {
				looking_for_space = false;
			}
		} else {
			if (!isspace(stream[i])) {
				return stream + i;
			}
		}

	}

	return stream + length;
}

uint32_t rjd_gfx_dcc_obj_read_uint(const char* stream, size_t length, const char** next) // base 10 only
{
	const char* end_stream = stream + length;
	const char* end_number = stream;
	for (; *end_number && end_number < end_stream; ++end_number) {
		if (*end_number != '-' && (*end_number < '0' || *end_number > '9')) {
			break;
		}
	}

	*next = end_number;

	if (end_number == end_stream) {
		return 0;
	}

	char copy[32];
	memcpy(copy, stream, end_number - stream);
	copy[end_number - stream] = '\0';
	return strtol(copy, NULL, 10);
}

const char* rjd_gfx_dcc_obj_read_token(const char* stream, const size_t length, struct token* out, struct rjd_strpool* stringpool)
{
	if (length == 0 || !stream || !*stream) {
		return NULL;
	}

	size_t keyword_length = 0;
	{
		const char* end = stream + length;
		const char* token_end = stream;
		while (token_end != end && !isspace(*token_end)) {
			++token_end;
		}
		keyword_length = token_end - stream;
	}

	out->type = TOKEN_TYPE_INVALID;

	if (!strncmp("mtllib", stream, keyword_length)) {
		const char* name = rjd_gfx_dcc_obj_find_next_value(stream, length);
		const char* end = rjd_gfx_dcc_obj_find_value_end(name, length - (name - stream));
		size_t name_length = end - name;

		out->type = TOKEN_TYPE_MATERIAL_LIB;
		out->material_lib.name = rjd_strpool_addl(stringpool, name, name_length);
		RJD_LOG("mtllib %s", rjd_strref_str(out->material_lib.name));
	} else if (!strncmp("g", stream, keyword_length)) {
		const char* name = rjd_gfx_dcc_obj_find_next_value(stream, length);
		const char* end = rjd_gfx_dcc_obj_find_value_end(name, length - (name - stream));
		size_t name_length = end - name;

		out->type = TOKEN_TYPE_GROUP_NAME;
		out->group_name.name = rjd_strpool_addl(stringpool, name, name_length);
		RJD_LOG("g %s", rjd_strref_str(out->material_lib.name));
	} else if (!strncmp("usemtl", stream, keyword_length)) {
		const char* name = rjd_gfx_dcc_obj_find_next_value(stream, length);
		const char* end = rjd_gfx_dcc_obj_find_value_end(name, length - (name - stream));
		size_t name_length = end - name;

		out->type = TOKEN_TYPE_MATERIAL_USE;
		out->material_use.name = rjd_strpool_addl(stringpool, name, name_length);
		RJD_LOG("usemtl %s", rjd_strref_str(out->material_lib.name));
	} else if (!strncmp("s", stream, keyword_length)) {
		const char* value = rjd_gfx_dcc_obj_find_next_value(stream, length);
		const char* end = rjd_gfx_dcc_obj_find_value_end(value, length - (value - stream));

		uint32_t id = 0;
		if (!strncmp("off", value, end - value)) {
			id = 0;
		} else {
			id = strtol(value, NULL, 10);
		}
		
		out->type = TOKEN_TYPE_GROUP_SMOOTH;
		out->group_smooth.id = id;
		RJD_LOG("s %u", out->group_smooth.id);
	} else if (!strncmp("v", stream, keyword_length)) {
		const char* next = rjd_gfx_dcc_obj_find_next_value(stream, length);
		out->type = TOKEN_TYPE_VERTEX;
		out->vertex.x = strtof(next, (char**)&next);
		out->vertex.y = strtof(next, (char**)&next);
		out->vertex.z = strtof(next, (char**)&next);
		if (rjd_gfx_dcc_obj_find_value_end(next, length - (next - stream)) == next) {
			out->vertex.w = 1.0f;
		} else {
			out->vertex.w = strtof(next, (char**)&next);
		}
		RJD_LOG("v %.2f %.2f %.2f %.2f", out->vertex.x, out->vertex.y, out->vertex.z, out->vertex.w);
	} else if (!strncmp("vn", stream, keyword_length)) {
		const char* next = rjd_gfx_dcc_obj_find_next_value(stream, length);
		out->type = TOKEN_TYPE_NORMAL;
		out->normal.i = strtof(next, (char**)&next);
		out->normal.j = strtof(next, (char**)&next);
		out->normal.k = strtof(next, (char**)&next);
		RJD_LOG("vn %.2f %.2f %.2f", out->normal.i, out->normal.j, out->normal.k);
	} else if (!strncmp("vt", stream, keyword_length)) {
		const char* next = rjd_gfx_dcc_obj_find_next_value(stream, length);
		out->type = TOKEN_TYPE_TEXCOORD;
		out->texcoord.u = strtof(next, (char**)&next);
		out->texcoord.v = strtof(next, (char**)&next);
		if (rjd_gfx_dcc_obj_find_value_end(next, length - (next - stream)) == next) {
			out->texcoord.w = 1.0f;
		} else {
			out->texcoord.w = strtof(next, (char**)&next);
		}
		RJD_LOG("vt %.2f %.2f %.2f", out->texcoord.u, out->texcoord.v, out->texcoord.w);
	} else if (!strncmp("f", stream, keyword_length)) {
		// TODO make this more robust... faces have a minimum of 3 vertices, but no maximum. This assumes
		// each face is a quad to make parsing easier for now, but it should really dynamically allocate and
		// detect how many tris there are.
		const char* next = rjd_gfx_dcc_obj_find_next_value(stream, length);

		out->type = TOKEN_TYPE_FACE;
		out->face.indices[0].vertex   = rjd_gfx_dcc_obj_read_uint(next, length - (next - stream), &next); ++next;
		out->face.indices[0].normal   = rjd_gfx_dcc_obj_read_uint(next, length - (next - stream), &next); ++next;
		out->face.indices[0].texcoord = rjd_gfx_dcc_obj_read_uint(next, length - (next - stream), &next); ++next;

		out->face.indices[1].vertex   = rjd_gfx_dcc_obj_read_uint(next, length - (next - stream), &next); ++next;
		out->face.indices[1].normal   = rjd_gfx_dcc_obj_read_uint(next, length - (next - stream), &next); ++next;
		out->face.indices[1].texcoord = rjd_gfx_dcc_obj_read_uint(next, length - (next - stream), &next); ++next;

		out->face.indices[2].vertex   = rjd_gfx_dcc_obj_read_uint(next, length - (next - stream), &next); ++next;
		out->face.indices[2].normal   = rjd_gfx_dcc_obj_read_uint(next, length - (next - stream), &next); ++next;
		out->face.indices[2].texcoord = rjd_gfx_dcc_obj_read_uint(next, length - (next - stream), &next); ++next;

		out->face.indices[3].vertex   = rjd_gfx_dcc_obj_read_uint(next, length - (next - stream), &next); ++next;
		out->face.indices[3].normal   = rjd_gfx_dcc_obj_read_uint(next, length - (next - stream), &next); ++next;
		out->face.indices[3].texcoord = rjd_gfx_dcc_obj_read_uint(next, length - (next - stream), &next); ++next;

		RJD_LOG("f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
			out->face.indices[0].vertex, out->face.indices[0].normal, out->face.indices[0].texcoord,
			out->face.indices[1].vertex, out->face.indices[1].normal, out->face.indices[1].texcoord,
			out->face.indices[2].vertex, out->face.indices[2].normal, out->face.indices[2].texcoord,
			out->face.indices[3].vertex, out->face.indices[3].normal, out->face.indices[3].texcoord);
	} else if (!strncmp("#", stream, keyword_length)) {
		// comment, just skip this line
	} else {
		char keyword[32];
		const size_t copy_length = keyword_length < rjd_countof(keyword) - 1 ? keyword_length : rjd_countof(keyword) - 1;
		memcpy(keyword, stream, copy_length);
		keyword[copy_length] = 0;
		RJD_LOG("Unrecognized token '%s'", keyword);
	}

	return rjd_gfx_dcc_obj_find_next_token(stream, length);
}

struct rjd_dcc_obj rjd_dcc_obj_create(const char* stream, size_t length, struct rjd_mem_allocator* allocator)
{
	struct rjd_strpool stringpool = rjd_strpool_init(allocator, 256);
	// struct token* array_tokens = rjd_array_alloc(struct token, 1024, allocator);

	size_t remaining = length;
	const char* next = stream;
	while (next) {
		struct token token = {0};
		next = rjd_gfx_dcc_obj_read_token(next, remaining, &token, &stringpool);
		if (next) {
			remaining = next - stream;
		} else {
			remaining = 0;
		}

		if (token.type != TOKEN_TYPE_INVALID) {
			// rjd_array_push(array_tokens, token);
		}
	}

	// return (struct rjd_dcc_obj){.array_tokens = array_tokens, .stringpool = stringpool};
	return (struct rjd_dcc_obj){.stringpool = stringpool};
}

void rjd_dcc_obj_destroy(struct rjd_dcc_obj* obj)
{
	// rjd_array_free(obj->array_tokens);
	rjd_strpool_free(&obj->stringpool);
}

// enum rjd_dcc_extract_flags
// {
// 	RJD_DCC_EXTRACT_FLAGS_VERTEX_XYZ,
// 	RJD_DCC_EXTRACT_FLAGS_VERTEX_W,
// 	RJD_DCC_EXTRACT_FLAGS_NORMAL,
// 	RJD_DCC_EXTRACT_FLAGS_TEXCOORD_UV,
// 	RJD_DCC_EXTRACT_FLAGS_TEXCOORD_W,
// 	RJD_DCC_EXTRACT_FLAGS_INDICES,
// };

// size_t rjd_dcc_obj_extract_calc_size(const struct rjd_dcc_obj* obj, struct rjd_hash64 group, enum rjd_dcc_extract_flags flags)
// {
// 	return 0;
// }

// void rjd_dcc_obj_extract(const struct rjd_dcc_obj* obj, enum rjd_dcc_extract_flags flags, void* buffer)
// {
	
// }

// int __cdecl main(void)
// {
// 	struct rjd_mem_allocator allocator = rjd_mem_allocator_init_default();
// 	char* obj_file = NULL;
// 	struct rjd_result result = rjd_fio_read("teapot.obj", &obj_file, &allocator);
// 	if (!rjd_result_isok(result)) {
// 		RJD_LOG("Failed to read file.");
// 		return 1;
// 	}
// 	rjd_array_push(obj_file, '\0');

// 	struct rjd_dcc_obj obj = rjd_dcc_obj_create(obj_file, rjd_array_count(obj_file), &allocator);

// 	// for (size_t i = 0; i < rjd_array_count(obj.array_tokens); ++i) {
// 	// 	RJD_LOG("token type: %d", obj.array_tokens[i].type);
// 	// }

// 	rjd_dcc_obj_destroy(&obj);

// 	// rjd_array_free(array_tokens);

// 	return 0;
// }