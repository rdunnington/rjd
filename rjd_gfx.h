#pragma once

#include <stdint.h>

#define RJD_GFX_H 1

// interface dependencies:
// * rjd_result

// impl dependencies:
// * rjd_mem
// * rjd_math

// Supported RJG_GFX_BACKEND_* values:
// RJD_GFX_BACKEND_METAL (osx only)

#ifndef RJD_GFX_BACKEND_METAL
	#define RJD_GFX_BACKEND_METAL 0
#endif

#if !RJD_GFX_BACKEND_METAL
	#error	"You must #define one of the RJD_GFX_BACKEND_* macros to 1 before including this file. "
			"See the above comment for a list of supported values."
#endif

#if RJD_GFX_BACKEND_METAL
	#if RJD_PLATFORM_OSX
	    // Workaround .c files not being able to read objective-c forward declarations
	    #if RJD_LANG_OBJC
	        @class MTKView;
	        RJD_STATIC_ASSERT(sizeof(void*) == sizeof(MTKView*));
	    #else
	        typedef void MTKView;
	    #endif
	#else
		#error "Metal is only supported on OSX"
	#endif
#endif

struct rjd_mem_allocator;

struct rjd_gfx_viewport // TODO figure out if this should have a start x,y pair
{
	uint32_t width;
	uint32_t height;
};

////////////////////////////////////////////////////////////////////////////////
// render data

enum rjd_gfx_camera_mode
{
	RJD_GFX_CAMERA_MODE_ORTHOGRAPHIC,
	//RJD_GFX_CAMERA_MODE_PERSPECTIVE, // TODO
	RJD_GFX_CAMERA_MODE_COUNT,
};

struct rjd_gfx_camera
{
	enum rjd_gfx_camera_mode mode;
	rjd_math_float3 pos;
};

struct rjd_gfx_quad_uv
{
	union
	{
		struct
		{
			struct {
				rjd_math_float2 righttop;
				rjd_math_float2 lefttop;
				rjd_math_float2 leftbot;
			} tri1;

			struct {
				rjd_math_float2 leftbot;
				rjd_math_float2 rightbot;
				rjd_math_float2 righttop;
			} tri2;
		};
		rjd_math_float2 v[6];
	};
};

////////////////////////////////////////////////////////////////////////////////
// render configuration

enum rjd_gfx_stencilmode
{
	RJD_GFX_STENCILMODE_DISABLED,
	RJD_GFX_STENCILMODE_TEST,
	RJD_GFX_STENCILMODE_WRITE,
	RJD_GFX_STENCILMODE_COUNT,
};

//struct rjd_gfx_render_config
//{
//	struct rjd_gfx_viewport window_size; // TODO upgrade this into a render view (render target w/ size & offset)?
//	enum rjd_gfx_stencilmode stencil_mode;
//	int use_buffer_color: 1;
//	int use_buffer_depth: 1;
//};
//
//struct rjd_gfx_render_geo2d
//{
//	struct rjd_math_float16* transforms;
//	union rjd_gfx_quad_uv* uvs;
//	rjd_math_float4* tints;
//	uint32_t count;
//};

////////////////////////////////////////////////////////////////////////////////
// resources

enum rjd_gfx_format
{
	RJD_GFX_FORMAT_COLOR_U8_RGBA,
	RJD_GFX_FORMAT_COLOR_U8_BGRA_NORM,
	RJD_GFX_FORMAT_COLOR_U8_BGRA_NORM_SRGB,
    RJD_GFX_FORMAT_DEPTHSTENCIL_F32_D32,
	RJD_GFX_FORMAT_DEPTHSTENCIL_U32_D24_S8,
	RJD_GFX_FORMAT_COUNT,
};

// all values correspond to the entry in the enum rjd_gfx_format
struct rjd_gfx_format_value
{
	enum rjd_gfx_format type;
	union
	{
		uint8_t color_u8_rgba[4];
		union { 
			uint32_t value; 
			struct { 
				uint32_t depth:24; 
				uint32_t stencil:8; 
			} parts; 
		} depthstencil_u32_d24_s8;
        float depthstencil_f32_d32;
	};
};

enum rjd_gfx_texture_access
{
	RJD_GFX_TEXTURE_ACCESS_CPU_NONE_GPU_READWRITE,
	RJD_GFX_TEXTURE_ACCESS_CPU_WRITE_GPU_READWRITE,
	RJD_GFX_TEXTURE_ACCESS_COUNT,
};

enum rjd_gfx_texture_usage
{
	RJD_GFX_TEXTURE_USAGE_DEFAULT,
	RJD_GFX_TEXTURE_USAGE_RENDERTARGET,
    RJD_GFX_TEXTURE_USAGE_COUNT,
};

struct rjd_gfx_texture_desc
{
	void* data;
    uint32_t data_length;
	uint32_t pixels_width;
	uint32_t pixels_height;
	enum rjd_gfx_format format;
	enum rjd_gfx_texture_access access;
	enum rjd_gfx_texture_usage usage;
	const char* debug_label;
};

struct rjd_gfx_texture
{
	struct rjd_slot handle;
};

//enum rjd_gfx_shader_input_buffer_type_flags
//{
//	RJD_GFX_SHADER_INPUT_USAGE_VERTEX = 0x1,
//	RJD_GFX_SHADER_INPUT_USAGE_FRAGMENT = 0x2,
//};

// TODO determine if this is a good idea or not
//struct rjd_gfx_shader_input_slot
//{
//	const char* name;
//	uint32_t index;
//	enum rjd_gfx_shader_input_buffer_type_flags type_flags;
//};

// TODO provide a precompiled path as well, e.g. bool is_compiled
struct rjd_gfx_shader_desc
{
	const void* data;
	//struct rjd_gfx_shader_input_slot* slots;

	uint32_t count_data;
	//uint32_t count_slots;
};

struct rjd_gfx_shader
{
	struct rjd_slot handle;
};

enum rjd_gfx_vertex_format_type
{
	RJD_GFX_VERTEX_FORMAT_TYPE_FLOAT1,
	RJD_GFX_VERTEX_FORMAT_TYPE_FLOAT2,
	RJD_GFX_VERTEX_FORMAT_TYPE_FLOAT3,
	RJD_GFX_VERTEX_FORMAT_TYPE_FLOAT4,
	RJD_GFX_VERTEX_FORMAT_TYPE_COUNT,
};

enum rjd_gfx_vertex_format_step
{
	RJD_GFX_VERTEX_FORMAT_STEP_VERTEX,
	RJD_GFX_VERTEX_FORMAT_STEP_INSTANCE,
	RJD_GFX_VERTEX_FORMAT_STEP_CONSTANT,
};

struct rjd_gfx_vertex_format_attribute
{
	enum rjd_gfx_vertex_format_type type;
	enum rjd_gfx_vertex_format_step step;
	uint32_t attribute_index;
	uint32_t buffer_index;
	uint32_t stride;
    uint32_t step_rate;
	uint32_t offset;
};

enum rjd_gfx_depth_compare
{
	RJD_GFX_DEPTH_COMPARE_DISABLED,
	RJD_GFX_DEPTH_COMPARE_ALWAYS_FAIL,
	RJD_GFX_DEPTH_COMPARE_ALWAYS_SUCCEED,
	RJD_GFX_DEPTH_COMPARE_LESS,
	RJD_GFX_DEPTH_COMPARE_LESSEQUAL,
	RJD_GFX_DEPTH_COMPARE_GREATER,
	RJD_GFX_DEPTH_COMPARE_GREATEREQUAL,
	RJD_GFX_DEPTH_COMPARE_EQUAL,
	RJD_GFX_DEPTH_COMPARE_NOTEQUAL,
};

struct rjd_gfx_pipeline_state_desc
{
	const char* debug_name;
	struct rjd_gfx_shader shader;
	struct rjd_gfx_texture render_target; // specify RJD_GFX_TEXTURE_BACKBUFFER to use the backbuffer
	struct rjd_gfx_texture depthstencil_target; // specify RJD_GFX_TEXTURE_BACKBUFFER to use the backbuffer
	struct rjd_gfx_vertex_format_attribute* vertex_attributes;
	uint32_t count_vertex_attributes;
	enum rjd_gfx_depth_compare depth_compare;
	// TODO stencil config
};

struct rjd_gfx_pipeline_state
{
	struct rjd_slot handle;
};

enum rjd_gfx_primitive_type
{
	RJD_GFX_PRIMITIVE_TYPE_TRIANGLES,
};

enum rjd_gfx_index_type
{
	RJD_GFX_INDEX_TYPE_UINT32,
	RJD_GFX_INDEX_TYPE_UINT16,
};

enum rjd_gfx_mesh_buffer_type
{
	RJD_GFX_MESH_BUFFER_TYPE_UNIFORMS,
	RJD_GFX_MESH_BUFFER_TYPE_VERTEX,
};

enum rjd_gfx_mesh_buffer_usage_flags
{
	RJD_GFX_MESH_BUFFER_USAGE_VERTEX = 0x1,
	RJD_GFX_MESH_BUFFER_USAGE_FRAGMENT = 0x2,
};

union rjd_gfx_mesh_buffer_common_desc
{
	struct {
		uint32_t capacity;
	} uniforms;

	struct {
		const void* data;
		uint32_t length;
	} vertex;
};

// TODO vertex_buffer isn't a great name, since it can also be inputs to fragment shaders. Maybe just mesh_shader_buffer?
struct rjd_gfx_mesh_vertex_buffer_desc
{
	enum rjd_gfx_mesh_buffer_type type;
	union rjd_gfx_mesh_buffer_common_desc common;
	enum rjd_gfx_mesh_buffer_usage_flags usage_flags;
	uint32_t buffer_index; // TODO maybe this should be name and index? buffer_slot? shader_input_slot?
};

//struct rjd_gfx_mesh_index_buffer_desc
//{
//	union rjd_gfx_mesh_buffer_desc buffer_desc;
//	enum rjd_gfx_index_type type;
//};
// TODO implement the other 2 descs later
struct rjd_gfx_mesh_vertexed_desc
{
	enum rjd_gfx_primitive_type primitive;
	struct rjd_gfx_mesh_vertex_buffer_desc* buffers;
	uint32_t count_buffers;
	uint32_t count_vertices;
};

//struct rjd_gfx_mesh_indexed_desc
//{
//	enum rjd_gfx_mesh_type type;
//	enum rjd_gfx_primitive_type primitive;
//	struct rjd_gfx_mesh_vertex_buffer_desc* buffers;
//	union rjd_gfx_mesh_index_buffer_desc* buffers;
//	uint32_t count_vertex_buffers;
//	uint32_t count_index_buffers;
//};

//struct rjd_gfx_mesh_instanced_desc
//{
//	enum rjd_gfx_mesh_type type;
//	enum rjd_gfx_primitive_type primitive;
//	struct rjd_gfx_mesh_vertex_buffer_desc* buffers;
//	uint32_t count_vertex_buffers;
//	uint32_t instance_count;
//};

struct rjd_gfx_mesh
{
	struct rjd_slot handle;
};

////////////////////////////////////////////////////////////////////////////////
// render commands

enum rjd_gfx_winding_order
{
	RJD_GFX_WINDING_ORDER_CLOCKWISE,
	RJD_GFX_WINDING_ORDER_COUNTERCLOCKWISE,
};

enum rjd_gfx_cull
{
	RJD_GFX_CULL_NONE,
	RJD_GFX_CULL_BACK,
	RJD_GFX_CULL_FRONT,
};

struct rjd_gfx_pass_begin_desc
{
	struct rjd_gfx_texture render_target; // specify RJD_GFX_TEXTURE_BACKBUFFER to use the backbuffer
	struct rjd_gfx_format_value clear_color;
	struct rjd_gfx_format_value clear_depthstencil;
	const char* debug_label;
};

struct rjd_gfx_pass_draw_desc
{
	const struct rjd_gfx_viewport* viewport;
	const struct rjd_gfx_pipeline_state* pipeline_state;
	const struct rjd_gfx_mesh* meshes;
	const struct rjd_gfx_texture* textures;
	const uint32_t* texture_indices; // parallel array with textures
	uint32_t count_meshes;
	uint32_t count_textures;
	enum rjd_gfx_cull cull_mode;
	enum rjd_gfx_winding_order winding_order;

	const char* debug_label;
};

struct rjd_gfx_command_buffer
{
	struct rjd_slot handle;
};

////////////////////////////////////////////////////////////////////////////////
// gfx context

struct rjd_gfx_context_desc
{
	enum rjd_gfx_format backbuffer_color_format;
	enum rjd_gfx_format backbuffer_depth_format;
	struct rjd_mem_allocator* allocator;
	uint32_t msaa_samples;

	// TODO forward declare HWND somehow
	#if RJD_PLATFORM_WINDOWS
		struct {
			HWND window_handle;
		} win32;
	#elif RJD_PLATFORM_OSX
		struct {
			MTKView* view;
		} osx;
	#endif
};

struct rjd_gfx_context
{
	char pimpl[128];
};

////////////////////////////////////////////////////////////////////////////////
// interface

// backend
static inline int32_t rjd_gfx_backend_ismetal(void);

// camera
struct rjd_gfx_camera rjd_gfx_camera_init(enum rjd_gfx_camera_mode mode);
rjd_math_mat4 rjd_gfx_camera_lookat_ortho_righthanded(const struct rjd_gfx_camera* camera);
//rjd_math_mat4 rjd_gfx_camera_lookat_ortho_lefthanded(const struct rjd_gfx_camera* camera); // TODO

// context
// NOTE: all functions that deal with a context are not threadsafe for simplicity. If you are making a multithreaded
// renderer, you must have a strategy for synchronizing resource creation and drawing with the context.
struct rjd_result rjd_gfx_context_create(struct rjd_gfx_context* out, struct rjd_gfx_context_desc desc);
void rjd_gfx_context_destroy(struct rjd_gfx_context* context);

bool rjd_gfx_msaa_is_count_supported(const struct rjd_gfx_context* context, uint32_t count); // count is usually: 1,2,4, or 8
void rjd_gfx_msaa_set_count(struct rjd_gfx_context* context, uint32_t count);
bool rjd_gfx_vsync_try_enable(struct rjd_gfx_context* context);
struct rjd_result rjd_gfx_wait_for_frame_begin(struct rjd_gfx_context* context);
struct rjd_result rjd_gfx_present(struct rjd_gfx_context* context);

// commands
struct rjd_result rjd_gfx_command_buffer_create(struct rjd_gfx_context* context, struct rjd_gfx_command_buffer* out);
struct rjd_result rjd_gfx_command_pass_begin(struct rjd_gfx_context* context, struct rjd_gfx_command_buffer* cmd_buffer, const struct rjd_gfx_pass_begin_desc* command);
struct rjd_result rjd_gfx_command_pass_draw(struct rjd_gfx_context* context, struct rjd_gfx_command_buffer* cmd_buffer, const struct rjd_gfx_pass_draw_desc* command);
struct rjd_result rjd_gfx_command_buffer_commit(struct rjd_gfx_context* context, struct rjd_gfx_command_buffer* cmd_buffer);

// resources
struct rjd_result rjd_gfx_texture_create(struct rjd_gfx_context* context, struct rjd_gfx_texture* out, struct rjd_gfx_texture_desc desc);
void rjd_gfx_texture_destroy(struct rjd_gfx_context* context, struct rjd_gfx_texture* texture);
struct rjd_result rjd_gfx_shader_create(struct rjd_gfx_context* context, struct rjd_gfx_shader* out, struct rjd_gfx_shader_desc desc);
void rjd_gfx_shader_destroy(struct rjd_gfx_context* context, struct rjd_gfx_shader* shader);
struct rjd_result rjd_gfx_pipeline_state_create(struct rjd_gfx_context* context, struct rjd_gfx_pipeline_state* out, struct rjd_gfx_pipeline_state_desc desc);
void rjd_gfx_pipeline_state_destroy(struct rjd_gfx_context* context, struct rjd_gfx_pipeline_state* pipeline_state);
struct rjd_result rjd_gfx_mesh_create_vertexed(struct rjd_gfx_context* context, struct rjd_gfx_mesh* out, struct rjd_gfx_mesh_vertexed_desc desc, struct rjd_mem_allocator* allocator);
//struct rjd_result rjd_gfx_mesh_create_indexed(struct rjd_gfx_context* context, struct rjd_gfx_mesh* out, struct rjd_gfx_mesh_indexed_desc desc);
struct rjd_result rjd_gfx_mesh_modify(struct rjd_gfx_context* context, struct rjd_gfx_mesh* mesh, uint32_t buffer_index, uint32_t offset, void* data, uint32_t length);
void rjd_gfx_mesh_destroy(struct rjd_gfx_context* context, struct rjd_gfx_mesh* mesh);

// format
struct rjd_gfx_format_value rjd_gfx_format_value_from_u32(enum rjd_gfx_format, uint32_t value);
uint32_t rjd_gfx_format_bytesize(enum rjd_gfx_format format);
bool rjd_gfx_format_iscolor(enum rjd_gfx_format format);
bool rjd_gfx_format_isdepthstencil(enum rjd_gfx_format format);
bool rjd_gfx_format_isdepth(enum rjd_gfx_format format);
bool rjd_gfx_format_isstencil(enum rjd_gfx_format format);

static inline struct rjd_gfx_format_value rjd_gfx_format_make_color_u8_rgba(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
static inline struct rjd_gfx_format_value rjd_gfx_format_make_depthstencil_f32_d32(float depth);

// constants
const extern struct rjd_gfx_texture RJD_GFX_TEXTURE_BACKBUFFER;

////////////////////////////////////////////////////////////////////////////////
// inline implementations

static inline int32_t rjd_gfx_backend_ismetal(void)
{
	return RJD_GFX_BACKEND_METAL;
}

static inline struct rjd_gfx_format_value rjd_gfx_format_make_color_u8_rgba(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
	struct rjd_gfx_format_value value;
	value.type = RJD_GFX_FORMAT_COLOR_U8_RGBA;
	value.color_u8_rgba[0] = red;
	value.color_u8_rgba[1] = green;
	value.color_u8_rgba[2] = blue;
	value.color_u8_rgba[3] = alpha;
    return value;
}

static inline struct rjd_gfx_format_value rjd_gfx_format_make_depthstencil_f32_d32(float depth)
{
	struct rjd_gfx_format_value value = {
		.type = RJD_GFX_FORMAT_DEPTHSTENCIL_F32_D32,
		.depthstencil_f32_d32 = depth,
	};
	return value;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#if RJD_IMPL

const static struct rjd_logchannel logchannel_default = {
	.enabled = true,
	.name = "rjd_gfx default",
};

const static struct rjd_logchannel logchannel_error = {
	.enabled = true,
	.name = "rjd_gfx error",
};

#define RJD_GFX_LOG(...) RJD_LOG_CHANNEL(&logchannel_default, RJD_LOG_VERBOSITY_LOW, __VA_ARGS__)
#define RJD_GFX_LOG_ERROR(...) RJD_LOG_CHANNEL(&logchannel_error, RJD_LOG_VERBOSITY_LOW, __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
// platform-independent camera

struct rjd_gfx_camera rjd_gfx_camera_init(enum rjd_gfx_camera_mode mode)
{
	struct rjd_gfx_camera cam = { .pos = rjd_math_float3_xyz(0,0,0), .mode = mode };
	return cam;
}

rjd_math_mat4 rjd_gfx_camera_lookat_ortho_righthanded(const struct rjd_gfx_camera* camera)
{
	RJD_ASSERT(camera);

	float x = floorf(camera->pos.x);
	float y = floorf(camera->pos.y);
	float z = floorf(camera->pos.z);

	rjd_math_vec3 pos = rjd_math_vec3_xyz(x,y,z);

	const rjd_math_vec3 look = rjd_math_vec3_xyz(x,y,z - 1.0f);
	const rjd_math_vec3 up = rjd_math_vec3_xyz(0.0f, 1.0f, 0.0f);
	return rjd_math_mat4_lookat_righthanded(pos, look, up);
}

////////////////////////////////////////////////////////////////////////////////
// platform-independent format

uint32_t rjd_gfx_format_bytesize(enum rjd_gfx_format format)
{
	switch (format)
	{
		case RJD_GFX_FORMAT_COLOR_U8_RGBA: return sizeof(char[4]);
        case RJD_GFX_FORMAT_COLOR_U8_BGRA_NORM: return sizeof(char[4]);
        case RJD_GFX_FORMAT_COLOR_U8_BGRA_NORM_SRGB: return sizeof(char[4]);
        case RJD_GFX_FORMAT_DEPTHSTENCIL_F32_D32: return sizeof(float);
		case RJD_GFX_FORMAT_DEPTHSTENCIL_U32_D24_S8: return sizeof(uint32_t);
		case RJD_GFX_FORMAT_COUNT: break;
	}

	RJD_ASSERTFAIL("Invalid value %d", format);
	return 0;
}

bool rjd_gfx_format_iscolor(enum rjd_gfx_format format)
{
	switch (format)
	{
		case RJD_GFX_FORMAT_COLOR_U8_RGBA: return true;
        case RJD_GFX_FORMAT_COLOR_U8_BGRA_NORM: return true;
        case RJD_GFX_FORMAT_COLOR_U8_BGRA_NORM_SRGB: return true;
        case RJD_GFX_FORMAT_DEPTHSTENCIL_F32_D32: return false;
		case RJD_GFX_FORMAT_DEPTHSTENCIL_U32_D24_S8: return false;
		case RJD_GFX_FORMAT_COUNT: break;
	}

	RJD_ASSERTFAIL("Invalid value %d", format);
	return false;
}

bool rjd_gfx_format_isdepthstencil(enum rjd_gfx_format format)
{
	bool is_depth = rjd_gfx_format_isdepth(format);
	bool is_stencil = rjd_gfx_format_isstencil(format);
	return is_depth || is_stencil;
}

bool rjd_gfx_format_isdepth(enum rjd_gfx_format format)
{
	switch (format)
	{
		case RJD_GFX_FORMAT_COLOR_U8_RGBA: return false;
        case RJD_GFX_FORMAT_COLOR_U8_BGRA_NORM: return false;
        case RJD_GFX_FORMAT_COLOR_U8_BGRA_NORM_SRGB: return false;
        case RJD_GFX_FORMAT_DEPTHSTENCIL_F32_D32: return true;
		case RJD_GFX_FORMAT_DEPTHSTENCIL_U32_D24_S8: return true;
		case RJD_GFX_FORMAT_COUNT: break;
	}

	RJD_ASSERTFAIL("Invalid value %d", format);
	return false;

}

bool rjd_gfx_format_isstencil(enum rjd_gfx_format format)
{
	switch (format)
	{
		case RJD_GFX_FORMAT_COLOR_U8_RGBA: return false;
        case RJD_GFX_FORMAT_COLOR_U8_BGRA_NORM: return false;
        case RJD_GFX_FORMAT_COLOR_U8_BGRA_NORM_SRGB: return false;
        case RJD_GFX_FORMAT_DEPTHSTENCIL_F32_D32: return false;
		case RJD_GFX_FORMAT_DEPTHSTENCIL_U32_D24_S8: return true;
		case RJD_GFX_FORMAT_COUNT: break;
	}

	RJD_ASSERTFAIL("Invalid value %d", format);
	return false;
}

////////////////////////////////////////////////////////////////////////////////
// gfx implementation is in API-specific headers
// TODO merge these headers into this file

#if RJD_GFX_BACKEND_METAL
	#if RJD_PLATFORM_OSX
		#include "rjd_gfx_metal.h"
	#else
		#error "Metal backend is only supported on OSX."
	#endif
#else
	#error "Unknown RJD_GFX_BACKEND. Ensure you are #defining to a known rjd_gfx_backend value.
#endif

#endif // RJD_IMPL
