#pragma once

#include <stdint.h>

#define RJD_GFX_H 1

// interface dependencies:
// * rjd_result

// impl dependencies:
// * rjd_mem
// * rjd_math

////////////////////////////////////////////////////////////////////////////////
// platform configuration

// Supported RJG_GFX_BACKEND_* values:
// RJD_GFX_BACKEND_METAL (osx only)
// RJD_GFX_BACKEND_D3D11 (windows only)

#ifndef RJD_GFX_BACKEND_METAL
	#define RJD_GFX_BACKEND_METAL 0
#endif
#ifndef RJD_GFX_BACKEND_D3D11
	#define RJD_GFX_BACKEND_D3D11 0
#endif

#if !RJD_GFX_BACKEND_METAL && !RJD_GFX_BACKEND_D3D11
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

#if RJD_GFX_PLATFORM_D3D11
	#if RJD_PLATFORM_WINDOWS
		typedef void* HWND;
	#else
		#error "DirectX is only supported on Windows"
	#endif
#endif

////////////////////////////////////////////////////////////////////////////////
// render configuration

struct rjd_mem_allocator;

struct rjd_gfx_viewport // TODO figure out if this should have a start x,y pair
{
	uint32_t width;
	uint32_t height;
};

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
	RJD_GFX_TEXTURE_ACCESS_CPU_WRITE_GPU_READWRITE,
	RJD_GFX_TEXTURE_ACCESS_CPU_NONE_GPU_READWRITE,
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
	const char* debug_label;
	void* data;
    uint32_t data_length;
	uint32_t pixels_width;
	uint32_t pixels_height;
	enum rjd_gfx_format format;
	enum rjd_gfx_texture_access access;
	enum rjd_gfx_texture_usage usage;
};

struct rjd_gfx_texture
{
	struct rjd_slot handle;
};

// TODO determine if this is a good idea or not
//struct rjd_gfx_shader_input_slot
//{
//	const char* name;
//	uint32_t index;
//	enum rjd_gfx_shader_input_buffer_type_flags type_flags;
//};

enum rjd_gfx_shader_type
{
	RJD_GFX_SHADER_TYPE_VERTEX,
	RJD_GFX_SHADER_TYPE_PIXEL,
};

// TODO provide a precompiled path as well, preferably with a shader_precompiled_desc?
struct rjd_gfx_shader_desc
{
	const char* source_name;
	const char* function_name;
	const void* data;
	uint32_t count_data;
	enum rjd_gfx_shader_type type;

	//struct rjd_gfx_shader_input_slot* slots;
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

enum rjd_gfx_vertex_semantic
{
	RJD_GFX_VERTEX_SEMANTIC_POSITION,
	RJD_GFX_VERTEX_SEMANTIC_COLOR,
	RJD_GFX_VERTEX_SEMANTIC_NORMAL,
	RJD_GFX_VERTEX_SEMANTIC_TEXCOORD,
	RJD_GFX_VERTEX_SEMANTIC_BINORMAL,
	RJD_GFX_VERTEX_SEMANTIC_TANGENT,
	RJD_GFX_VERTEX_SEMANTIC_BLENDINDEX,
	RJD_GFX_VERTEX_SEMANTIC_BLENDWEIGHT,
	RJD_GFX_VERTEX_SEMANTIC_COUNT,
};

struct rjd_gfx_vertex_format_attribute
{
	enum rjd_gfx_vertex_format_type type;
	enum rjd_gfx_vertex_format_step step;
	enum rjd_gfx_vertex_semantic semantic; // only used for d3d11
	uint32_t attribute_index;
	uint32_t shader_slot_metal;
	uint32_t shader_slot_d3d11;
	uint32_t stride;
    uint32_t step_rate;
	uint32_t offset;
};

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
	struct rjd_gfx_shader shader_vertex;
	struct rjd_gfx_shader shader_pixel;
	struct rjd_gfx_texture render_target; // specify RJD_GFX_TEXTURE_BACKBUFFER to use the backbuffer
	struct rjd_gfx_texture depthstencil_target; // specify RJD_GFX_TEXTURE_BACKBUFFER to use the backbuffer
	struct rjd_gfx_vertex_format_attribute* vertex_attributes;
	uint32_t count_vertex_attributes;
	enum rjd_gfx_depth_compare depth_compare;
	enum rjd_gfx_cull cull_mode;
	enum rjd_gfx_winding_order winding_order;
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

enum rjd_gfx_mesh_buffer_usage_flags
{
	RJD_GFX_MESH_BUFFER_USAGE_VERTEX = 1 << 0,
	RJD_GFX_MESH_BUFFER_USAGE_VERTEX_CONSTANT = 1 << 1,
	RJD_GFX_MESH_BUFFER_USAGE_PIXEL_CONSTANT = 1 << 2,
};

union rjd_gfx_mesh_buffer_common_desc
{
	struct {
		uint32_t capacity;
	} constant;

	struct {
		const void* data;
		uint32_t length;
		uint32_t stride;
	} vertex;
};

struct rjd_gfx_mesh_buffer_desc
{
	union rjd_gfx_mesh_buffer_common_desc common;
	enum rjd_gfx_mesh_buffer_usage_flags usage_flags;
	uint32_t shader_slot_metal; // metal shader slots are shared between all buffer types
	uint32_t shader_slot_d3d11; // d3d11 vertex/constant buffers have their own lists of slots
};

// TODO implement the other 2 descs later
struct rjd_gfx_mesh_vertexed_desc
{
	enum rjd_gfx_primitive_type primitive;
	struct rjd_gfx_mesh_buffer_desc* buffers;
	uint32_t count_buffers;
	uint32_t count_vertices;
};

//struct rjd_gfx_mesh_indexed_desc
//{
//	enum rjd_gfx_mesh_type type;
//	enum rjd_gfx_primitive_type primitive;
//	struct rjd_gfx_mesh_buffer_desc* buffers;
//	union rjd_gfx_mesh_index_buffer_desc* buffers;
//	uint32_t count_vertex_buffers;
//	uint32_t count_index_buffers;
//};

//struct rjd_gfx_mesh_instanced_desc
//{
//	enum rjd_gfx_mesh_type type;
//	enum rjd_gfx_primitive_type primitive;
//	struct rjd_gfx_mesh_buffer_desc* buffers;
//	uint32_t count_vertex_buffers;
//	uint32_t instance_count;
//};

struct rjd_gfx_mesh
{
	struct rjd_slot handle;
};

////////////////////////////////////////////////////////////////////////////////
// render command

struct rjd_gfx_pass_begin_desc
{
	const char* debug_label;
	struct rjd_gfx_texture render_target; // specify RJD_GFX_TEXTURE_BACKBUFFER to use the backbuffer
	struct rjd_gfx_format_value clear_color;
	struct rjd_gfx_format_value clear_depthstencil;
};

struct rjd_gfx_pass_draw_buffer_offset_desc
{
	uint32_t mesh_index;
	uint32_t buffer_index;
	uint32_t offset_bytes;
	uint32_t range_bytes;
};

struct rjd_gfx_pass_draw_desc
{
	const char* debug_label;
	const struct rjd_gfx_viewport* viewport;
	const struct rjd_gfx_pipeline_state* pipeline_state;
	const struct rjd_gfx_mesh* meshes;
	const struct rjd_gfx_pass_draw_buffer_offset_desc* buffer_offset_descs;
	const struct rjd_gfx_texture* textures;
	const uint32_t* texture_indices; // parallel array with textures
	uint32_t count_meshes;
	uint32_t count_constant_descs;
	uint32_t count_textures;
};

struct rjd_gfx_command_buffer
{
	struct rjd_slot handle;
};

////////////////////////////////////////////////////////////////////////////////
// gfx context

enum rjd_gfx_num_backbuffers
{
	RJD_GFX_NUM_BACKBUFFERS_TRIPLE,
	RJD_GFX_NUM_BACKBUFFERS_DOUBLE,
};

enum RJD_GFX_VSYNC_MODE
{
	RJD_GFX_VSYNC_MODE_ON,
	RJD_GFX_VSYNC_MODE_OFF,
};

struct rjd_gfx_context_desc
{
	struct rjd_mem_allocator* allocator;
	enum rjd_gfx_format backbuffer_color_format;
	enum rjd_gfx_format backbuffer_depth_format;
	enum rjd_gfx_num_backbuffers num_backbuffers;
	uint32_t* optional_desired_msaa_samples; // desired samples and fallbacks if unavailable. 1 is the default.
	uint32_t count_desired_msaa_samples;

	#if RJD_PLATFORM_WINDOWS
		struct {
			void* hwnd; // HWND
		} win32;
	#elif RJD_PLATFORM_OSX
		struct {
			MTKView* view;
		} osx;
	#endif
};

struct rjd_gfx_context
{
	char pimpl[148];
};

////////////////////////////////////////////////////////////////////////////////
// interface

// backend
static inline int32_t rjd_gfx_backend_ismetal(void);
static inline int32_t rjd_gfx_backend_isd3d11(void);

// context
// NOTE: all functions that deal with a context are not threadsafe for simplicity. If you are making a multithreaded
// renderer, you must have a strategy for synchronizing resource creation and drawing with the context.
struct rjd_result rjd_gfx_context_create(struct rjd_gfx_context* out, struct rjd_gfx_context_desc desc);
void rjd_gfx_context_destroy(struct rjd_gfx_context* context);

struct rjd_result rjd_gfx_vsync_set(struct rjd_gfx_context* context, enum RJD_GFX_VSYNC_MODE mode);
struct rjd_result rjd_gfx_wait_for_frame_begin(struct rjd_gfx_context* context);
struct rjd_result rjd_gfx_present(struct rjd_gfx_context* context);
uint32_t rjd_gfx_current_backbuffer_index(struct rjd_gfx_context* context);

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
struct rjd_result rjd_gfx_mesh_create_vertexed(struct rjd_gfx_context* context, struct rjd_gfx_mesh* out, struct rjd_gfx_mesh_vertexed_desc desc);
//struct rjd_result rjd_gfx_mesh_create_indexed(struct rjd_gfx_context* context, struct rjd_gfx_mesh* out, struct rjd_gfx_mesh_indexed_desc desc);
struct rjd_result rjd_gfx_mesh_modify(struct rjd_gfx_context* context, struct rjd_gfx_command_buffer* cmd_buffer, struct rjd_gfx_mesh* mesh, uint32_t buffer_index, uint32_t offset, const void* data, uint32_t length);
void rjd_gfx_mesh_destroy(struct rjd_gfx_context* context, struct rjd_gfx_mesh* mesh);

// constant buffer helpers
static inline uint32_t rjd_gfx_constant_buffer_alignment(void);
static inline uint32_t rjd_gfx_calc_constant_buffer_stride(uint32_t constant_size);

// format
uint32_t rjd_gfx_format_bytesize(enum rjd_gfx_format format);
bool rjd_gfx_format_iscolor(enum rjd_gfx_format format);
bool rjd_gfx_format_isdepthstencil(enum rjd_gfx_format format);
bool rjd_gfx_format_isdepth(enum rjd_gfx_format format);
bool rjd_gfx_format_isstencil(enum rjd_gfx_format format);

static inline struct rjd_gfx_format_value rjd_gfx_format_make_color_u8_rgba(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
static inline struct rjd_gfx_format_value rjd_gfx_format_make_depthstencil_f32_d32(float depth);

// constants
extern const struct rjd_gfx_texture RJD_GFX_TEXTURE_BACKBUFFER;

////////////////////////////////////////////////////////////////////////////////
// inline implementations

static inline int32_t rjd_gfx_backend_ismetal(void)
{
	return RJD_GFX_BACKEND_METAL;
}

static inline int32_t rjd_gfx_backend_isd3d11(void)
{
	return RJD_GFX_BACKEND_D3D11;
}

static inline uint32_t rjd_gfx_constant_buffer_alignment(void)
{
	#if RJD_GFX_BACKEND_METAL || RJD_GFX_BACKEND_D3D11
		return 256;
	#else
		#error Unknown platform.
	#endif
}

static inline uint32_t rjd_gfx_calc_constant_buffer_stride(uint32_t constant_size)
{
	uint32_t alignment = rjd_gfx_constant_buffer_alignment();
	uint32_t aligned_stride = (constant_size % alignment) + constant_size;
	return aligned_stride;
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

static const struct rjd_logchannel logchannel_default = {
	.enabled = true,
	.name = "rjd_gfx default",
};

static const struct rjd_logchannel logchannel_error = {
	.enabled = true,
	.name = "rjd_gfx error",
};

#define RJD_GFX_LOG(...) RJD_LOG_CHANNEL(&logchannel_default, RJD_LOG_VERBOSITY_LOW, __VA_ARGS__)
#define RJD_GFX_LOG_ERROR(...) RJD_LOG_CHANNEL(&logchannel_error, RJD_LOG_VERBOSITY_LOW, __VA_ARGS__)

const struct rjd_gfx_texture RJD_GFX_TEXTURE_BACKBUFFER = {0};

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
#elif RJD_GFX_BACKEND_D3D11
	#if RJD_PLATFORM_WINDOWS
		#include "rjd_gfx_d3d11.h"
	#else
		#error "Metal backend is only supported on OSX."
	#endif
#else
	#error "Unknown RJD_GFX_BACKEND. Ensure you are #defining to a known rjd_gfx_backend value."
#endif

#endif // RJD_IMPL
