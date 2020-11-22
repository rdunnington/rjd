#pragma once

#define RJD_GFX_METAL_H 1

#if !RJD_GFX_H
	#error "This header should only be included by rjd_gfx.h"
#endif

#if !RJD_IMPL
	#error "This should have only been included when RJD_IMPL is on"
#endif

#if !RJD_PLATFORM_OSX
	#error "Metal is only supported on OSX"
#endif

////////////////////////////////////////////////////////////////////////////////
// includes/libs
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

////////////////////////////////////////////////////////////////////////////////
// data

struct rjd_gfx_texture_metal
{
	id <MTLTexture> texture;
};

struct rjd_gfx_shader_metal
{
	id<MTLLibrary> library;
	id<MTLFunction> function;
};

struct rjd_gfx_pipeline_state_metal
{
	id<MTLRenderPipelineState> state_render;
	id<MTLDepthStencilState> state_depthstencil;
	enum rjd_gfx_cull cull_mode;
	enum rjd_gfx_winding_order winding_order;
};

struct rjd_gfx_mesh_vertex_buffer_metal
{
	id<MTLBuffer> buffer;
	enum rjd_gfx_mesh_buffer_type type;
	enum rjd_gfx_mesh_buffer_usage_flags usage_flags;
	uint32_t buffer_index;
	uint32_t offset;
};

struct rjd_gfx_mesh_index_buffer_metal
{
	id<MTLBuffer> buffer;
	enum rjd_gfx_index_type type;
	uint32_t count_indices;
};

struct rjd_gfx_mesh_metal
{
	struct rjd_gfx_mesh_vertex_buffer_metal* vertex_buffers;
	struct rjd_gfx_mesh_index_buffer_metal* index_buffers;
	uint32_t count_vertices; // 0 if using index_buffers
	uint32_t instance_count; // 0 if it's not instanced
	enum rjd_gfx_primitive_type primitive;
};

struct rjd_gfx_command_buffer_metal
{
	id <MTLCommandBuffer> buffer;
	id<MTLRenderCommandEncoder> encoder;
};

struct rjd_gfx_context_metal
{
	struct rjd_gfx_texture_metal* slotmap_textures;
	struct rjd_gfx_shader_metal* slotmap_shaders;
	struct rjd_gfx_pipeline_state_metal* slotmap_pipeline_states;
	struct rjd_gfx_mesh_metal* slotmap_meshes;
	struct rjd_gfx_command_buffer_metal* slotmap_command_buffers;

	MTKView* view;
	id <MTLDevice> device;
	id <MTLCommandQueue> command_queue;
	MTKMeshBufferAllocator* loader_mesh;

	struct rjd_mem_allocator* allocator;

	dispatch_semaphore_t wait_for_present_counter;
};
RJD_STATIC_ASSERT(sizeof(struct rjd_gfx_context_metal) <= sizeof(struct rjd_gfx_context));

////////////////////////////////////////////////////////////////////////////////
// private helpers

static MTLPixelFormat rjd_gfx_format_color_to_metal(enum rjd_gfx_format format);
static MTLClearColor rjd_gfx_format_value_to_clearcolor(struct rjd_gfx_format_value value);
static double rjd_gfx_format_value_to_depth(struct rjd_gfx_format_value value);
static uint32_t rjd_gfx_format_value_to_stencil(struct rjd_gfx_format_value value);
static bool rjd_gfx_format_isbackbuffercompatible(enum rjd_gfx_format format);

static bool rjd_gfx_mtlformat_is_depth(MTLPixelFormat format);
static bool rjd_gfx_mtlformat_is_stencil(MTLPixelFormat format);

static MTLVertexFormat rjd_gfx_vertex_format_type_to_metal(enum rjd_gfx_vertex_format_type type);
static MTLVertexStepFunction rjd_gfx_vertex_format_step_to_metal(enum rjd_gfx_vertex_format_step step);
static MTLIndexType rjd_gfx_index_type_to_metal(enum rjd_gfx_index_type type);
static MTLPrimitiveType rjd_gfx_primitive_type_to_metal(enum rjd_gfx_primitive_type type);
static MTLCompareFunction rjd_gfx_depth_compare_to_metal(enum rjd_gfx_depth_compare func);
static MTLWinding rjd_gfx_winding_to_metal(enum rjd_gfx_winding_order winding_order);
static MTLCullMode rjd_gfx_cull_to_metal(enum rjd_gfx_cull cull_mode);

static bool rjd_gfx_texture_isbackbuffer(struct rjd_gfx_texture texture);

static inline void rjd_gfx_texture_destroy_metal(struct rjd_gfx_context_metal* context, struct rjd_slot slot);
static inline void rjd_gfx_shader_destroy_metal(struct rjd_gfx_context_metal* context, struct rjd_slot slot);
static inline void rjd_gfx_pipeline_state_destroy_metal(struct rjd_gfx_context_metal* context, struct rjd_slot slot);
static inline void rjd_gfx_mesh_destroy_metal(struct rjd_gfx_context_metal* context, struct rjd_slot slot);
static inline void rjd_gfx_command_buffer_destroy_metal(struct rjd_gfx_context_metal* context, struct rjd_slot slot);

////////////////////////////////
// render control

struct rjd_result rjd_gfx_context_create(struct rjd_gfx_context* out, struct rjd_gfx_context_desc desc)
{
	RJD_ASSERT(out);

	MTKView* view = desc.osx.view;
    if(!view.device) {
        return RJD_RESULT("You must pass a view with an already-initialized Metal device");
    }

	if (!rjd_gfx_format_isbackbuffercompatible(desc.backbuffer_color_format)) {
		return RJD_RESULT("Unsupported backbuffer color format. Supported values are:\n"
						  "\tRJD_GFX_FORMAT_COLOR_U8_BGRA_NORM\n"
                          "\tRJD_GFX_FORMAT_COLOR_U8_BGRA_NORM_SRGB\n");
	}

	if (!rjd_gfx_format_isdepthstencil(desc.backbuffer_depth_format)) {
		return RJD_RESULT("backbuffer_depth_format must be a depth format. See rjd_gfx_format_isdepthstencil().");
	}

	{
		const MTLPixelFormat mtl_color_format = rjd_gfx_format_color_to_metal(desc.backbuffer_color_format);
		const MTLPixelFormat mtl_depth_format = rjd_gfx_format_color_to_metal(desc.backbuffer_depth_format);

        view.colorPixelFormat = mtl_color_format;
        view.depthStencilPixelFormat = mtl_depth_format;
		view.sampleCount = 1; // users can set this higher later with rjd_gfx_set_msaa_count()
	}

	NSUInteger count_msaa = 1;
	for (uint32_t i = 0; desc.optional_desired_msaa_samples && i < desc.count_desired_msaa_samples; ++i) {
		NSUInteger count = desc.optional_desired_msaa_samples[i];
		if ([view.device supportsTextureSampleCount:count]) {
			count_msaa = count;
			break;
		}
	}
	view.sampleCount = count_msaa;

	struct rjd_gfx_context_metal* context_metal = (struct rjd_gfx_context_metal*)out;
    memset(out, 0, sizeof(*out));

	context_metal->slotmap_textures			= rjd_slotmap_alloc(struct rjd_gfx_texture_metal, 64, desc.allocator);
	context_metal->slotmap_shaders			= rjd_slotmap_alloc(struct rjd_gfx_shader_metal, 64, desc.allocator);
	context_metal->slotmap_pipeline_states	= rjd_slotmap_alloc(struct rjd_gfx_pipeline_state_metal, 64, desc.allocator);
	context_metal->slotmap_meshes			= rjd_slotmap_alloc(struct rjd_gfx_mesh_metal, 64, desc.allocator);
	context_metal->slotmap_command_buffers	= rjd_slotmap_alloc(struct rjd_gfx_command_buffer_metal, 16, desc.allocator);

	context_metal->view = view;
	context_metal->device = view.device;
	context_metal->command_queue = [context_metal->device newCommandQueue];
	context_metal->loader_mesh = [[MTKMeshBufferAllocator alloc] initWithDevice:context_metal->device];
    context_metal->allocator = desc.allocator;
	context_metal->wait_for_present_counter = dispatch_semaphore_create(1);

	return RJD_RESULT_OK();
}

struct rjd_result rjd_gfx_wait_for_frame_begin(struct rjd_gfx_context* context)
{
	struct rjd_gfx_context_metal* context_metal = (struct rjd_gfx_context_metal*)context;
	dispatch_semaphore_wait(context_metal->wait_for_present_counter, DISPATCH_TIME_FOREVER);

	return RJD_RESULT_OK();
}

struct rjd_result rjd_gfx_present(struct rjd_gfx_context* context)
{
	// NOTE: this is a no-op for metal since presenting is handled via presentDrawable()
	// which is invoked after the command buffer has finished executing all commands

    RJD_UNUSED_PARAM(context);
	return RJD_RESULT_OK();
}

void rjd_gfx_context_destroy(struct rjd_gfx_context* context)
{
	RJD_ASSERT(context);

	struct rjd_gfx_context_metal* context_metal = (struct rjd_gfx_context_metal*)context;
	
	struct rjd_gfx_texture_metal* textures = context_metal->slotmap_textures;
	struct rjd_gfx_shader_metal* shaders = context_metal->slotmap_shaders;
	struct rjd_gfx_pipeline_state_metal* states = context_metal->slotmap_pipeline_states;
	struct rjd_gfx_mesh_metal* meshes = context_metal->slotmap_meshes;
	struct rjd_gfx_command_buffer_metal* commands = context_metal->slotmap_command_buffers;

	for (struct rjd_slot s = rjd_slotmap_next(textures, NULL); !rjd_slot_isvalid(s); s = rjd_slotmap_next(textures, &s)) {
		rjd_gfx_texture_destroy_metal(context_metal, s);
	}

	for (struct rjd_slot s = rjd_slotmap_next(shaders, NULL); !rjd_slot_isvalid(s); s = rjd_slotmap_next(shaders, &s)) {
		rjd_gfx_shader_destroy_metal(context_metal, s);
	}

	for (struct rjd_slot s = rjd_slotmap_next(states, NULL); !rjd_slot_isvalid(s); s = rjd_slotmap_next(states, &s)) {
		rjd_gfx_pipeline_state_destroy_metal(context_metal, s);
	}

	for (struct rjd_slot s = rjd_slotmap_next(meshes, NULL); !rjd_slot_isvalid(s); s = rjd_slotmap_next(meshes, &s)) {
		rjd_gfx_mesh_destroy_metal(context_metal, s);
	}

	for (struct rjd_slot s = rjd_slotmap_next(commands, NULL); !rjd_slot_isvalid(s); s = rjd_slotmap_next(commands, &s)) {
		rjd_gfx_command_buffer_destroy_metal(context_metal, s);
	}

	rjd_slotmap_free(textures);
	rjd_slotmap_free(shaders);
	rjd_slotmap_free(states);
	rjd_slotmap_free(meshes);
	rjd_slotmap_free(commands);

	context_metal->slotmap_textures			= NULL;
	context_metal->slotmap_shaders			= NULL;
	context_metal->slotmap_pipeline_states	= NULL;
	context_metal->slotmap_meshes			= NULL;
	context_metal->slotmap_command_buffers	= NULL;
}

bool rjd_gfx_msaa_is_count_supported(const struct rjd_gfx_context* context, uint32_t count)
{
	RJD_ASSERT(context);

	struct rjd_gfx_context_metal* context_metal = (struct rjd_gfx_context_metal*)context;
    return [context_metal->device supportsTextureSampleCount:(NSUInteger)count];
}

void rjd_gfx_msaa_set_count(struct rjd_gfx_context* context, uint32_t count)
{
	RJD_ASSERT(context);

	struct rjd_gfx_context_metal* context_metal = (struct rjd_gfx_context_metal*)context;
	context_metal->view.sampleCount = count;
}

struct rjd_result rjd_gfx_vsync_set(struct rjd_gfx_context* context, enum RJD_GFX_VSYNC_MODE mode)
{
	RJD_UNUSED_PARAM(context);
	RJD_UNUSED_PARAM(mode);
	return RJD_RESULT("unimplemented");
}

struct rjd_result rjd_gfx_command_buffer_create(struct rjd_gfx_context* context, struct rjd_gfx_command_buffer* out)
{
	RJD_ASSERT(context);
	RJD_ASSERT(out);

	struct rjd_gfx_context_metal* context_metal = (struct rjd_gfx_context_metal*)context;
	struct rjd_gfx_command_buffer_metal buffer = {
		.buffer = [context_metal->command_queue commandBuffer],
	};

	rjd_slotmap_insert(context_metal->slotmap_command_buffers, buffer, &out->handle);

	return RJD_RESULT_OK();
}

struct rjd_result rjd_gfx_command_pass_begin(struct rjd_gfx_context* context, struct rjd_gfx_command_buffer* cmd_buffer, const struct rjd_gfx_pass_begin_desc* command)
{
	RJD_ASSERT(rjd_slot_isvalid(cmd_buffer->handle));
	RJD_ASSERT(command);
	RJD_ASSERT(context);

	struct rjd_gfx_context_metal* context_metal = (struct rjd_gfx_context_metal*)context;
    struct rjd_gfx_command_buffer_metal* command_buffer_metal = rjd_slotmap_get(context_metal->slotmap_command_buffers, cmd_buffer->handle);

	if (command_buffer_metal->encoder != nil) {
    	[command_buffer_metal->encoder endEncoding];
		command_buffer_metal->encoder = nil;
	}

	MTLRenderPassDescriptor* render_pass = NULL;
	if (rjd_gfx_texture_isbackbuffer(command->render_target))
	{
		if (context_metal->view.currentDrawable) {
	        render_pass = context_metal->view.currentRenderPassDescriptor;
		} else {
			return RJD_RESULT("All backbuffers are in use. The app is likely GPU-bound.");
		}
	}
	else
	{
		struct rjd_gfx_texture_metal* texture_metal = rjd_slotmap_get(context_metal->slotmap_textures, command->render_target.handle);
		id<MTLTexture> texture = texture_metal->texture;
		if (texture.usage != MTLTextureUsageRenderTarget) {
			return RJD_RESULT("Specified texture is not a render target");
		}

		render_pass = [MTLRenderPassDescriptor renderPassDescriptor];
	}

    if (rjd_gfx_format_iscolor(command->clear_color.type)) {
        const MTLClearColor color = rjd_gfx_format_value_to_clearcolor(command->clear_color);
        render_pass.colorAttachments[0].clearColor = color;
    }

	if (rjd_gfx_format_isdepthstencil(command->clear_depthstencil.type)) {
        if (render_pass.depthAttachment) {
            const double depth = rjd_gfx_format_value_to_depth(command->clear_depthstencil);
            render_pass.depthAttachment.clearDepth = depth;
        }

        if (render_pass.stencilAttachment) {
            const uint32_t stencil = rjd_gfx_format_value_to_stencil(command->clear_depthstencil);
            render_pass.stencilAttachment.clearStencil = stencil;
        }
    }

	id<MTLRenderCommandEncoder> encoder = [command_buffer_metal->buffer renderCommandEncoderWithDescriptor:render_pass];
	const char* label = command->debug_label ? command->debug_label : "rjd_gfx_command_pass_begin";
	encoder.label = [NSString stringWithUTF8String:label];

	command_buffer_metal->encoder = encoder;
    
    return RJD_RESULT_OK();
}

struct rjd_result rjd_gfx_command_pass_draw(struct rjd_gfx_context* context, struct rjd_gfx_command_buffer* cmd_buffer, const struct rjd_gfx_pass_draw_desc* command)
{
	RJD_ASSERT(context);
	RJD_ASSERT(cmd_buffer);
	RJD_ASSERT(command);
    RJD_ASSERT(command->viewport);
    RJD_ASSERT(command->pipeline_state);
    RJD_ASSERT(command->meshes);
    RJD_ASSERT(command->textures);
    RJD_ASSERT(command->texture_indices);
	RJD_ASSERT(rjd_slot_isvalid(cmd_buffer->handle));
	RJD_ASSERT(rjd_slot_isvalid(command->pipeline_state->handle));

	struct rjd_gfx_context_metal* context_metal = (struct rjd_gfx_context_metal*)context;
    struct rjd_gfx_command_buffer_metal* command_buffer_metal = rjd_slotmap_get(context_metal->slotmap_command_buffers, cmd_buffer->handle);
	const struct rjd_gfx_pipeline_state_metal* pipeline_metal = rjd_slotmap_get(context_metal->slotmap_pipeline_states, command->pipeline_state->handle);

	RJD_ASSERTMSG(command_buffer_metal->encoder != nil, "You must call rjd_gfx_command_pass_begin before issuing draw commands.");
	id<MTLRenderCommandEncoder> encoder = command_buffer_metal->encoder;

    [encoder pushDebugGroup:[NSString stringWithUTF8String:command->debug_label]];

	const MTLWinding winding_order_metal = rjd_gfx_winding_to_metal(pipeline_metal->winding_order);
	const MTLCullMode cull_mode_metal = rjd_gfx_cull_to_metal(pipeline_metal->cull_mode);

    [encoder setRenderPipelineState:pipeline_metal->state_render];
    [encoder setDepthStencilState:pipeline_metal->state_depthstencil];
    [encoder setFrontFacingWinding:winding_order_metal];
    [encoder setCullMode:cull_mode_metal];

	for (uint32_t i = 0; i < command->count_textures; ++i) {
		const struct rjd_gfx_texture* texture = command->textures + i;
        RJD_ASSERT(rjd_slot_isvalid(texture->handle));
		struct rjd_gfx_texture_metal* texture_metal = rjd_slotmap_get(context_metal->slotmap_textures, texture->handle);

		const uint32_t* index = command->texture_indices + i;
		[encoder setFragmentTexture:texture_metal->texture atIndex:*index];
	}

	// TODO add functionality to draw portions of an uploaded buffer. For simplicity, currently you must draw the entire buffer.
	for (uint32_t i = 0; i < command->count_meshes; ++i) {
        const struct rjd_gfx_mesh* mesh = command->meshes + i;
        RJD_ASSERT(rjd_slot_isvalid(mesh->handle));
		const struct rjd_gfx_mesh_metal* mesh_metal = rjd_slotmap_get(context_metal->slotmap_meshes, mesh->handle);

		// set all shader inputs before attempting to draw anything
		for (uint32_t i = 0; i < rjd_array_count(mesh_metal->vertex_buffers); ++i) {
			struct rjd_gfx_mesh_vertex_buffer_metal* buffer = mesh_metal->vertex_buffers + i;

			if (buffer->usage_flags & RJD_GFX_MESH_BUFFER_USAGE_VERTEX) {
				[encoder setVertexBuffer:buffer->buffer offset:buffer->offset atIndex:buffer->buffer_index];
			}

			if (buffer->usage_flags & RJD_GFX_MESH_BUFFER_USAGE_FRAGMENT) {
				[encoder setFragmentBuffer:buffer->buffer offset:buffer->offset atIndex:buffer->buffer_index];
			}
		}

		const MTLPrimitiveType primitive = rjd_gfx_primitive_type_to_metal(mesh_metal->primitive);

		if (mesh_metal->count_vertices > 0) {
			if (mesh_metal->instance_count) {
				[encoder drawPrimitives:primitive vertexStart:0 vertexCount:mesh_metal->count_vertices instanceCount:mesh_metal->instance_count];
			} else {
				[encoder drawPrimitives:primitive vertexStart:0 vertexCount:mesh_metal->count_vertices];
			}
		}

		if (mesh_metal->index_buffers) {
			for (uint32_t i = 0; i < rjd_array_count(mesh_metal->index_buffers); ++i) {
				struct rjd_gfx_mesh_index_buffer_metal* indices = mesh_metal->index_buffers + i;
				const MTLIndexType type = rjd_gfx_index_type_to_metal(indices->type);

				if (mesh_metal->instance_count) {
					[encoder drawIndexedPrimitives:primitive 
							 indexCount:indices->count_indices 
							 indexType:type
                             indexBuffer:indices->buffer
							 indexBufferOffset:0 
							 instanceCount:mesh_metal->instance_count];
				} else {
					[encoder drawIndexedPrimitives:primitive 
							 indexCount:indices->count_indices 
							 indexType:type 
							 indexBuffer:indices->buffer
							 indexBufferOffset:0];
				}
			}
		}
	}

    [encoder popDebugGroup];

	return RJD_RESULT_OK();
}

struct rjd_result rjd_gfx_command_buffer_commit(struct rjd_gfx_context* context, struct rjd_gfx_command_buffer* cmd_buffer)
{
	RJD_ASSERT(cmd_buffer);
	RJD_ASSERT(context);
	RJD_ASSERT(rjd_slot_isvalid(cmd_buffer->handle));

	struct rjd_gfx_context_metal* context_metal = (struct rjd_gfx_context_metal*)context;
	struct rjd_gfx_command_buffer_metal* command_buffer_metal = rjd_slotmap_get(context_metal->slotmap_command_buffers, cmd_buffer->handle);

	RJD_ASSERTMSG(command_buffer_metal->encoder != nil, "You must call rjd_gfx_command_pass_begin before issuing draw commands.");
    [command_buffer_metal->encoder endEncoding];

	[command_buffer_metal->buffer presentDrawable:context_metal->view.currentDrawable];
	
    __block dispatch_semaphore_t wait_counter = context_metal->wait_for_present_counter;
    [command_buffer_metal->buffer addCompletedHandler:^(id<MTLCommandBuffer> buffer)
    {
		RJD_UNUSED_PARAM(buffer);
    	dispatch_semaphore_signal(wait_counter);
    }];

	[command_buffer_metal->buffer commit];
	rjd_gfx_command_buffer_destroy_metal(context_metal, cmd_buffer->handle);
    rjd_slot_invalidate(&cmd_buffer->handle);

	return RJD_RESULT_OK();
}

////////////////////////////////
// resources

struct rjd_result rjd_gfx_texture_create(struct rjd_gfx_context* context, struct rjd_gfx_texture* out, struct rjd_gfx_texture_desc desc)
{
	RJD_ASSERT(out);
	RJD_ASSERT(context);

	RJD_RESULT_CHECK(desc.data != NULL, "Texture data must not be NULL");
	RJD_RESULT_CHECK(desc.data_length != 0, "Texture data length must not be 0");
	RJD_RESULT_CHECK(desc.pixels_width != 0, "Texture width must not be 0");
	RJD_RESULT_CHECK(desc.pixels_height != 0, "Texture height must not be 0");

	struct rjd_gfx_context_metal* context_metal = (struct rjd_gfx_context_metal*)context;

    id<MTLTexture> texture = nil;
	{
		MTLTextureDescriptor* descriptor = [[MTLTextureDescriptor alloc] init];
		descriptor.textureType = MTLTextureType2D;
		descriptor.pixelFormat = rjd_gfx_format_color_to_metal(desc.format);
		descriptor.width = desc.pixels_width;
		descriptor.height = desc.pixels_height;
		descriptor.depth = 1; // Always 1 for 2D textures
		descriptor.mipmapLevelCount = 1; // TODO support mipmaps
		descriptor.sampleCount = 1; // Always 1 unless this is a MTLTextureType2DMultisample
		descriptor.arrayLength = 1;

        MTLResourceOptions buffer_options;
		switch (desc.access)
		{
			case RJD_GFX_TEXTURE_ACCESS_CPU_NONE_GPU_READWRITE:
				descriptor.cpuCacheMode = MTLCPUCacheModeDefaultCache;
				descriptor.storageMode = MTLStorageModePrivate;
				descriptor.allowGPUOptimizedContents = YES;
                buffer_options = MTLResourceStorageModePrivate;
				break;
			case RJD_GFX_TEXTURE_ACCESS_CPU_WRITE_GPU_READWRITE:
				descriptor.cpuCacheMode = MTLCPUCacheModeWriteCombined;
				descriptor.allowGPUOptimizedContents = NO;
                buffer_options = MTLResourceStorageModeManaged | MTLResourceCPUCacheModeWriteCombined;
				break;
            case RJD_GFX_TEXTURE_ACCESS_COUNT:
                RJD_ASSERTFAIL("RJD_GFX_TEXTURE_ACCESS_COUNT is not a valid access.")
                break;
		}

		switch (desc.usage)
		{
			case RJD_GFX_TEXTURE_USAGE_DEFAULT: 
				descriptor.usage = MTLTextureUsageShaderRead;
				break;
			case RJD_GFX_TEXTURE_USAGE_RENDERTARGET: 
				descriptor.usage = MTLTextureUsageRenderTarget;
				break;
			case RJD_GFX_TEXTURE_USAGE_COUNT:
				RJD_ASSERTFAIL("Unhandled case %d", desc.usage);
				break;
		}

        id<MTLBuffer> buffer = [context_metal->device newBufferWithBytes:desc.data length:desc.data_length options:buffer_options];
        
        const NSUInteger bytes_per_row = rjd_gfx_format_bytesize(desc.format) * desc.pixels_width;
        texture = [buffer newTextureWithDescriptor:descriptor offset:0 bytesPerRow:bytes_per_row];
        MTLRegion region = {
            .origin = {0,0,0},
            .size = { desc.pixels_width, desc.pixels_height, 1 },
        };
        [texture replaceRegion:region mipmapLevel:0 withBytes:desc.data bytesPerRow:bytes_per_row];

        const char* label = desc.debug_label ? desc.debug_label : "anonymous_texture";
        texture.label = [NSString stringWithUTF8String:label];
	}

	struct rjd_gfx_texture_metal texture_metal = {
		.texture = texture,
	};

	rjd_slotmap_insert(context_metal->slotmap_textures, texture_metal, &out->handle);

	return RJD_RESULT_OK();
}

void rjd_gfx_texture_destroy(struct rjd_gfx_context* context, struct rjd_gfx_texture* texture)
{
	RJD_ASSERT(texture);
	RJD_ASSERT(context);
	RJD_ASSERT(rjd_slot_isvalid(texture->handle));

	struct rjd_gfx_context_metal* context_metal = (struct rjd_gfx_context_metal*)context;

	rjd_gfx_texture_destroy_metal(context_metal, texture->handle);
}

struct rjd_result rjd_gfx_shader_create(struct rjd_gfx_context* context, struct rjd_gfx_shader* out, struct rjd_gfx_shader_desc desc)
{
	RJD_ASSERT(out);
	RJD_ASSERT(context);
	RJD_ASSERTMSG(((char*)desc.data)[desc.count_data] == '\0', "Expected a null-terminated string for the data.");
    
    struct rjd_gfx_context_metal* context_metal = (struct rjd_gfx_context_metal*)context;

	NSString* source = [NSString stringWithUTF8String:desc.data];
	MTLCompileOptions* options = [[MTLCompileOptions alloc] init];
	NSError* error = NULL;
	id<MTLLibrary> library = [context_metal->device newLibraryWithSource:source options:options error:&error];

	if (error) {
		RJD_GFX_LOG_ERROR("Error compiling shader: %s", error.localizedDescription.UTF8String);
		return RJD_RESULT("Error compiling shaders");
	}

	NSString* function_name = [NSString stringWithUTF8String:desc.function_name];
	id<MTLFunction> function = [library newFunctionWithName:function_name];

	if (function == nil) {
		return RJD_RESULT("The shader source did not have the specified function name.");
	}

	struct rjd_gfx_shader_metal shader_metal = {
		.library = library,
		.function = function,
	};
    
	rjd_slotmap_insert(context_metal->slotmap_shaders, shader_metal, &out->handle);

	return RJD_RESULT_OK();
}

void rjd_gfx_shader_destroy(struct rjd_gfx_context* context, struct rjd_gfx_shader* shader)
{
	RJD_ASSERT(shader);
	RJD_ASSERT(context);
	RJD_ASSERT(rjd_slot_isvalid(shader->handle));

	struct rjd_gfx_context_metal* context_metal = (struct rjd_gfx_context_metal*)context;
	rjd_gfx_shader_destroy_metal(context_metal, shader->handle);
}

struct rjd_result rjd_gfx_pipeline_state_create(struct rjd_gfx_context* context, struct rjd_gfx_pipeline_state* out, struct rjd_gfx_pipeline_state_desc desc)
{
	RJD_ASSERT(out);
	RJD_ASSERT(context);
    
    struct rjd_gfx_context_metal* context_metal = (struct rjd_gfx_context_metal*)context;

    id <MTLRenderPipelineState> pipeline_state = nil;
	{
    	MTLRenderPipelineDescriptor *desc_metal = [[MTLRenderPipelineDescriptor alloc] init];
		if (desc.debug_name != NULL) {
    		desc_metal.label = [NSString stringWithUTF8String:desc.debug_name];
		}

		if (rjd_slot_isvalid(desc.render_target.handle)) {
			struct rjd_gfx_texture_metal* texture_metal = rjd_slotmap_get(context_metal->slotmap_textures, desc.render_target.handle);
			id<MTLTexture> target = texture_metal->texture;
    		desc_metal.sampleCount = target.sampleCount;
    		desc_metal.colorAttachments[0].pixelFormat = target.pixelFormat;
		} else {
			MTKView* view = context_metal->view;
    		desc_metal.sampleCount = view.sampleCount;
    		desc_metal.colorAttachments[0].pixelFormat = view.colorPixelFormat;
		}

        {
            MTLPixelFormat source_format = MTLPixelFormatInvalid;
            if (rjd_gfx_texture_isbackbuffer(desc.depthstencil_target)) {
                MTKView* view = context_metal->view;
                source_format = view.depthStencilPixelFormat;
            } else if (rjd_slot_isvalid(desc.depthstencil_target.handle)) {
                struct rjd_gfx_texture_metal* texture_metal = rjd_slotmap_get(context_metal->slotmap_textures, desc.render_target.handle);
                id<MTLTexture> target = texture_metal->texture;
                source_format = target.pixelFormat;
            }
            
            MTLPixelFormat depth_format = MTLPixelFormatInvalid;
            MTLPixelFormat stencil_format = MTLPixelFormatInvalid;
            if (source_format != MTLPixelFormatInvalid) {
                if (rjd_gfx_mtlformat_is_depth(source_format)) {
                    depth_format = source_format;
                }
                if (rjd_gfx_mtlformat_is_stencil(source_format)) {
                    stencil_format = source_format;
                }
            }

            desc_metal.depthAttachmentPixelFormat = depth_format;
            desc_metal.stencilAttachmentPixelFormat = stencil_format;
        }

		if (rjd_slot_isvalid(desc.shader_vertex.handle)) {
			struct rjd_gfx_shader_metal* shader = rjd_slotmap_get(context_metal->slotmap_shaders, desc.shader_vertex.handle);
    		desc_metal.vertexFunction = shader->function;
		}

		if (rjd_slot_isvalid(desc.shader_pixel.handle)) {
			struct rjd_gfx_shader_metal* shader = rjd_slotmap_get(context_metal->slotmap_shaders, desc.shader_pixel.handle);
    		desc_metal.fragmentFunction = shader->function;
		}

		// vertex format
        {
            MTLVertexDescriptor* vertex_descriptor = [[MTLVertexDescriptor alloc] init];

			// shader struct input definitions
            for (size_t i = 0; i < desc.count_vertex_attributes; ++i) {
				const struct rjd_gfx_vertex_format_attribute* attribute = desc.vertex_attributes + i;

				uint32_t attribute_index = attribute->attribute_index;
				uint32_t buffer_index = attribute->buffer_index;
				MTLVertexFormat format = rjd_gfx_vertex_format_type_to_metal(attribute->type);
				MTLVertexStepFunction step_function = rjd_gfx_vertex_format_step_to_metal(attribute->step);

				vertex_descriptor.attributes[attribute_index].format = format;
				vertex_descriptor.attributes[attribute_index].offset = attribute->offset;
				vertex_descriptor.attributes[attribute_index].bufferIndex = attribute->buffer_index;

				vertex_descriptor.layouts[buffer_index].stride = attribute->stride;
				vertex_descriptor.layouts[buffer_index].stepRate = attribute->step_rate;
				vertex_descriptor.layouts[buffer_index].stepFunction = step_function;
            }

            desc_metal.vertexDescriptor = vertex_descriptor;
        }

    	NSError *error = NULL;
    	pipeline_state = [context_metal->device newRenderPipelineStateWithDescriptor:desc_metal error:&error];
    	if (pipeline_state == nil) {
    	    RJD_GFX_LOG_ERROR("Failed to create pipeline state: %s", error.localizedDescription.UTF8String);
			return RJD_RESULT("Failed to create pipeline state");
		}
	}

	id<MTLDepthStencilState> depth_stencil_state = nil;
    {
        MTLDepthStencilDescriptor *depth_state_desc = [[MTLDepthStencilDescriptor alloc] init];
        depth_state_desc.depthWriteEnabled = desc.depth_compare != RJD_GFX_DEPTH_COMPARE_DISABLED;
        if (depth_state_desc.depthWriteEnabled) {
            depth_state_desc.depthCompareFunction = rjd_gfx_depth_compare_to_metal(desc.depth_compare);
        }
        depth_stencil_state = [context_metal->device newDepthStencilStateWithDescriptor:depth_state_desc];

        if (depth_stencil_state == nil) {
               RJD_GFX_LOG_ERROR("Failed to create depth stencil state");
            return RJD_RESULT("Failed to create depth stencil state");
        }
    }

	struct rjd_gfx_pipeline_state_metal state = {
		.state_render = pipeline_state,
		.state_depthstencil = depth_stencil_state,
		.cull_mode = desc.cull_mode,
		.winding_order = desc.winding_order,
	};

	rjd_slotmap_insert(context_metal->slotmap_pipeline_states, state, &out->handle);

	return RJD_RESULT_OK();
}

void rjd_gfx_pipeline_state_destroy(struct rjd_gfx_context* context, struct rjd_gfx_pipeline_state* pipeline_state)
{
	RJD_ASSERT(pipeline_state);
	RJD_ASSERT(context);
	RJD_ASSERT(rjd_slot_isvalid(pipeline_state->handle));

	struct rjd_gfx_context_metal* context_metal = (struct rjd_gfx_context_metal*)context;
	rjd_gfx_pipeline_state_destroy_metal(context_metal, pipeline_state->handle);
}

struct rjd_result rjd_gfx_mesh_create_vertexed(struct rjd_gfx_context* context, struct rjd_gfx_mesh* out, struct rjd_gfx_mesh_vertexed_desc desc)
{
	RJD_ASSERT(out);
	RJD_ASSERT(context);

	if (desc.count_buffers == 0) {
		return RJD_RESULT("Must have more than 0 vertex buffers to create a mesh");
	}

	struct rjd_result result = RJD_RESULT_OK();

	struct rjd_gfx_context_metal* context_metal = (struct rjd_gfx_context_metal*)context;
	
	struct rjd_gfx_mesh_vertex_buffer_metal* mesh_buffers = rjd_array_alloc(struct rjd_gfx_mesh_vertex_buffer_metal, desc.count_buffers, context_metal->allocator);
    rjd_array_resize(mesh_buffers, desc.count_buffers);

	for (uint32_t i = 0; i < desc.count_buffers; ++i) {
		id<MTLBuffer> buffer = nil;
		struct rjd_gfx_mesh_vertex_buffer_desc* desc_buffer = desc.buffers + i;

        if (desc_buffer->type == RJD_GFX_MESH_BUFFER_TYPE_UNIFORMS) {
			NSUInteger length = desc_buffer->common.uniforms.capacity;
			buffer = [context_metal->device newBufferWithLength:length options:MTLResourceStorageModeShared];
		} else if (desc_buffer->type == RJD_GFX_MESH_BUFFER_TYPE_VERTEX) {
			const void* bytes = desc_buffer->common.vertex.data;
			NSUInteger length = desc_buffer->common.vertex.length;
			buffer = [context_metal->device newBufferWithBytes:bytes length:length options:MTLResourceStorageModeManaged];
            NSRange range = NSMakeRange(0, length);
            [buffer didModifyRange:range];
		} else {
			RJD_ASSERTFAIL("Unknown mesh buffer type %d", desc_buffer->type);
		}

	    if (rjd_result_isok(result) && buffer == nil) {
            for (uint32_t k = 0; k < i; ++k) {
                mesh_buffers[k].buffer = nil;
            }
	        result = RJD_RESULT("Not enough memory available to create buffers");
            break;
	    }
        
        struct rjd_gfx_mesh_vertex_buffer_metal mesh_buffer = {
            .buffer = buffer,
			.type = desc.buffers[i].type,
            .usage_flags = desc.buffers[i].usage_flags,
            .buffer_index = desc.buffers[i].buffer_index,
            .offset = 0,
        };
        
        mesh_buffers[i] = mesh_buffer;
	}

    if (rjd_result_isok(result)) {
        struct rjd_gfx_mesh_metal mesh_metal = {
            .vertex_buffers = mesh_buffers,
            .count_vertices = desc.count_vertices,
            .primitive = desc.primitive,
        };

        rjd_slotmap_insert(context_metal->slotmap_meshes, mesh_metal, &out->handle);
    }

	return result;
}

struct rjd_result rjd_gfx_mesh_modify(struct rjd_gfx_context* context, struct rjd_gfx_command_buffer* cmd_buffer, struct rjd_gfx_mesh* mesh, uint32_t buffer_index, uint32_t offset, void* data, uint32_t length)
{
	RJD_ASSERT(context);
	RJD_ASSERT(mesh);
	RJD_ASSERT(rjd_slot_isvalid(mesh->handle));

	RJD_UNUSED_PARAM(cmd_buffer);

	struct rjd_gfx_context_metal* context_metal = (struct rjd_gfx_context_metal*)context;
	struct rjd_gfx_mesh_metal* mesh_metal = rjd_slotmap_get(context_metal->slotmap_meshes, mesh->handle);

	struct rjd_gfx_mesh_vertex_buffer_metal* buffer = NULL;
	for (uint32_t i = 0; i < rjd_array_count(mesh_metal->vertex_buffers); ++i) {
		if (mesh_metal->vertex_buffers[i].buffer_index == buffer_index) {
			buffer = mesh_metal->vertex_buffers + i;
		}
	}

	if (buffer == NULL) {
		return RJD_RESULT("Mesh buffer with specified buffer not found in mesh.");
	}

	if (buffer->buffer.length < offset + length) {
		return RJD_RESULT("Not enough storage available. You must increase the allocated size when the mesh is created.");
	}

	if (buffer->buffer.contents == NULL) {
		return RJD_RESULT("This is an immutable buffer. Modifying its contents is not allowed.");
	}

	memcpy((uint8_t*)buffer->buffer.contents + offset, data, length);
    if (buffer->type == RJD_GFX_MESH_BUFFER_TYPE_VERTEX) {
        NSRange range = NSMakeRange(offset, length);
        [buffer->buffer didModifyRange:range];
    }

	// TODO determine if we should push this responsibility up to the caller when they do draw calls
	buffer->offset = offset;

	return RJD_RESULT_OK();
}

void rjd_gfx_mesh_destroy(struct rjd_gfx_context* context, struct rjd_gfx_mesh* mesh)
{
	RJD_ASSERT(mesh);
	RJD_ASSERT(context);
	RJD_ASSERT(rjd_slot_isvalid(mesh->handle));

	struct rjd_gfx_context_metal* context_metal = (struct rjd_gfx_context_metal*)context;
	rjd_gfx_mesh_destroy_metal(context_metal, mesh->handle);
}

////////////////////////////////////////////////////////////////////////////////
// private interface

static MTLPixelFormat rjd_gfx_format_color_to_metal(enum rjd_gfx_format format)
{
	switch (format)
	{
		case RJD_GFX_FORMAT_COLOR_U8_RGBA: return MTLPixelFormatRGBA8Uint;
		case RJD_GFX_FORMAT_COLOR_U8_BGRA_NORM: return MTLPixelFormatBGRA8Unorm;
		case RJD_GFX_FORMAT_COLOR_U8_BGRA_NORM_SRGB: return MTLPixelFormatBGRA8Unorm_sRGB;
		case RJD_GFX_FORMAT_DEPTHSTENCIL_F32_D32: return MTLPixelFormatDepth32Float;
		case RJD_GFX_FORMAT_DEPTHSTENCIL_U32_D24_S8: return MTLPixelFormatDepth24Unorm_Stencil8;
		case RJD_GFX_FORMAT_COUNT: break;
	}

	RJD_ASSERTFAIL("Unhandled format %d.", format);
	return MTLPixelFormatInvalid;
}

static MTLClearColor rjd_gfx_format_value_to_clearcolor(struct rjd_gfx_format_value value)
{
	double red = 0;
	double green = 0;
	double blue = 0;
	double alpha = 1;

	switch (value.type)
	{
		case RJD_GFX_FORMAT_COLOR_U8_RGBA:
			red		= value.color_u8_rgba[0] / (double)UCHAR_MAX;
			green	= value.color_u8_rgba[1] / (double)UCHAR_MAX;
			blue	= value.color_u8_rgba[2] / (double)UCHAR_MAX;
			alpha	= value.color_u8_rgba[3] / (double)UCHAR_MAX;
			break;
        case RJD_GFX_FORMAT_COLOR_U8_BGRA_NORM:
        case RJD_GFX_FORMAT_COLOR_U8_BGRA_NORM_SRGB:
			blue	= value.color_u8_rgba[2] / (double)UCHAR_MAX;
			green	= value.color_u8_rgba[1] / (double)UCHAR_MAX;
			red		= value.color_u8_rgba[0] / (double)UCHAR_MAX;
			alpha	= value.color_u8_rgba[3] / (double)UCHAR_MAX;
            break;
		case RJD_GFX_FORMAT_DEPTHSTENCIL_F32_D32:
		case RJD_GFX_FORMAT_DEPTHSTENCIL_U32_D24_S8:
			RJD_ASSERTFAIL("depth stencil values shouldn't be passed to this function");
			break;
		default: 
			RJD_ASSERTFAIL("Unhandled format %d.", value.type);
			break;
	}
	return MTLClearColorMake(red, green, blue, alpha);
}

static double rjd_gfx_format_value_to_depth(struct rjd_gfx_format_value value)
{
	switch (value.type)
	{
		case RJD_GFX_FORMAT_COLOR_U8_RGBA:
        case RJD_GFX_FORMAT_COLOR_U8_BGRA_NORM:
        case RJD_GFX_FORMAT_COLOR_U8_BGRA_NORM_SRGB:
			RJD_ASSERTFAIL("color values shouldn't be passed to this function");
			break;

		case RJD_GFX_FORMAT_DEPTHSTENCIL_F32_D32: return value.depthstencil_f32_d32;
		case RJD_GFX_FORMAT_DEPTHSTENCIL_U32_D24_S8: return value.depthstencil_u32_d24_s8.parts.depth;
		case RJD_GFX_FORMAT_COUNT: break;
	}

	RJD_ASSERTFAIL("Unhandled format %d.", value.type);
	return MTLPixelFormatInvalid;
}

static uint32_t rjd_gfx_format_value_to_stencil(struct rjd_gfx_format_value value)
{
	switch (value.type)
	{
		case RJD_GFX_FORMAT_COLOR_U8_RGBA:
        case RJD_GFX_FORMAT_COLOR_U8_BGRA_NORM:
        case RJD_GFX_FORMAT_COLOR_U8_BGRA_NORM_SRGB:
			RJD_ASSERTFAIL("color values shouldn't be passed to this function");
			break;

        case RJD_GFX_FORMAT_DEPTHSTENCIL_F32_D32: return 0.0f;
		case RJD_GFX_FORMAT_DEPTHSTENCIL_U32_D24_S8: return value.depthstencil_u32_d24_s8.parts.stencil;
        case RJD_GFX_FORMAT_COUNT: break;
    }

	RJD_ASSERTFAIL("Unhandled format %d.", value.type);
	return MTLPixelFormatInvalid;
}

static bool rjd_gfx_format_isbackbuffercompatible(enum rjd_gfx_format format)
{
	switch (format)
	{
		case RJD_GFX_FORMAT_COLOR_U8_BGRA_NORM: return true;
		case RJD_GFX_FORMAT_COLOR_U8_BGRA_NORM_SRGB: return true;
		default: return false;
	}
}

static MTLVertexFormat rjd_gfx_vertex_format_type_to_metal(enum rjd_gfx_vertex_format_type type)
{
	switch (type)
	{
		case RJD_GFX_VERTEX_FORMAT_TYPE_FLOAT1: return MTLVertexFormatFloat;
		case RJD_GFX_VERTEX_FORMAT_TYPE_FLOAT2: return MTLVertexFormatFloat2;
		case RJD_GFX_VERTEX_FORMAT_TYPE_FLOAT3: return MTLVertexFormatFloat3;
		case RJD_GFX_VERTEX_FORMAT_TYPE_FLOAT4: return MTLVertexFormatFloat4;
        case RJD_GFX_VERTEX_FORMAT_TYPE_COUNT: break;
	}

	RJD_ASSERTFAIL("unhandled type: %d", type);
	return MTLVertexFormatInvalid;
}

static MTLVertexStepFunction rjd_gfx_vertex_format_step_to_metal(enum rjd_gfx_vertex_format_step step)
{
	switch (step)
	{
		case RJD_GFX_VERTEX_FORMAT_STEP_VERTEX: return MTLVertexStepFunctionPerVertex;
		case RJD_GFX_VERTEX_FORMAT_STEP_INSTANCE: return MTLVertexStepFunctionPerInstance;
		case RJD_GFX_VERTEX_FORMAT_STEP_CONSTANT: return MTLVertexStepFunctionConstant;
	}

	RJD_ASSERTFAIL("unhandled type: %d", step);
	return MTLVertexStepFunctionPerVertex;
}

static MTLIndexType rjd_gfx_index_type_to_metal(enum rjd_gfx_index_type type)
{
	switch (type)
	{
		case RJD_GFX_INDEX_TYPE_UINT16: return MTLIndexTypeUInt16;
		case RJD_GFX_INDEX_TYPE_UINT32: return MTLIndexTypeUInt32;
	}
	RJD_ASSERTFAIL("Unknown index type %d", type);
	return MTLIndexTypeUInt16;
}

static MTLPrimitiveType rjd_gfx_primitive_type_to_metal(enum rjd_gfx_primitive_type type)
{
    switch (type)
    {
        case RJD_GFX_PRIMITIVE_TYPE_TRIANGLES: return MTLPrimitiveTypeTriangle;
    }
    RJD_ASSERTFAIL("Unknown primitive type %d", type);
    return MTLPrimitiveTypeTriangle;
}

static MTLCompareFunction rjd_gfx_depth_compare_to_metal(enum rjd_gfx_depth_compare func)
{
	switch (func)
	{
		case RJD_GFX_DEPTH_COMPARE_DISABLED: RJD_ASSERTFAIL("unreachable"); break;
		case RJD_GFX_DEPTH_COMPARE_ALWAYS_FAIL: return MTLCompareFunctionNever;
		case RJD_GFX_DEPTH_COMPARE_ALWAYS_SUCCEED: return MTLCompareFunctionAlways;
		case RJD_GFX_DEPTH_COMPARE_LESS: return MTLCompareFunctionLess;
		case RJD_GFX_DEPTH_COMPARE_LESSEQUAL: return MTLCompareFunctionLessEqual;
		case RJD_GFX_DEPTH_COMPARE_GREATER: return MTLCompareFunctionGreater;
		case RJD_GFX_DEPTH_COMPARE_GREATEREQUAL: return MTLCompareFunctionGreaterEqual;
		case RJD_GFX_DEPTH_COMPARE_EQUAL: return MTLCompareFunctionEqual;
		case RJD_GFX_DEPTH_COMPARE_NOTEQUAL: return MTLCompareFunctionNotEqual;
	}
	RJD_ASSERTFAIL("Unknown compare func %d", func);
	return MTLCompareFunctionAlways;
}

static MTLWinding rjd_gfx_winding_to_metal(enum rjd_gfx_winding_order winding_order)
{
	switch (winding_order)
	{
		case RJD_GFX_WINDING_ORDER_CLOCKWISE: return MTLWindingClockwise;
		case RJD_GFX_WINDING_ORDER_COUNTERCLOCKWISE: return MTLWindingCounterClockwise;
	}
	RJD_ASSERTFAIL("Unknown winding order %d", winding_order);
	return MTLWindingClockwise;
}

static MTLCullMode rjd_gfx_cull_to_metal(enum rjd_gfx_cull cull_mode)
{
	switch (cull_mode)
	{
		case RJD_GFX_CULL_NONE: return MTLCullModeNone;
		case RJD_GFX_CULL_BACK: return MTLCullModeBack;
		case RJD_GFX_CULL_FRONT: return MTLCullModeFront;
	}
	RJD_ASSERTFAIL("Unknown cull mode %d", cull_mode);
	return MTLCullModeNone;
}

static bool rjd_gfx_mtlformat_is_depth(MTLPixelFormat format)
{
    switch (format)
    {
        case MTLPixelFormatDepth32Float:
        case MTLPixelFormatDepth24Unorm_Stencil8:
        case MTLPixelFormatDepth16Unorm:
        case MTLPixelFormatDepth32Float_Stencil8:
            return true;
        default:
            break;
    }
    
    return false;
}

static bool rjd_gfx_mtlformat_is_stencil(MTLPixelFormat format)
{
    switch (format)
    {
        case MTLPixelFormatDepth24Unorm_Stencil8:
        case MTLPixelFormatDepth32Float_Stencil8:
            return true;
        default:
            break;
    }
    
    return false;
}


static bool rjd_gfx_texture_isbackbuffer(struct rjd_gfx_texture texture)
{
	return	texture.handle.salt == RJD_GFX_TEXTURE_BACKBUFFER.handle.salt &&
			texture.handle.index == RJD_GFX_TEXTURE_BACKBUFFER.handle.index;
}

static inline void rjd_gfx_texture_destroy_metal(struct rjd_gfx_context_metal* context, struct rjd_slot slot)
{
    if (rjd_slotmap_contains(context->slotmap_textures, slot)) {
        struct rjd_gfx_texture_metal* texture_metal = rjd_slotmap_get(context->slotmap_textures, slot);
        texture_metal->texture = nil;
//        texture_metal->buffer = nil;
        rjd_slotmap_erase(context->slotmap_textures, slot);
    }
}

static inline void rjd_gfx_shader_destroy_metal(struct rjd_gfx_context_metal* context, struct rjd_slot slot)
{
	struct rjd_gfx_shader_metal* shader_metal = rjd_slotmap_get(context->slotmap_shaders, slot);

	shader_metal->function = nil;
	shader_metal->library = nil;

	rjd_slotmap_erase(context->slotmap_shaders, slot);
}

static inline void rjd_gfx_pipeline_state_destroy_metal(struct rjd_gfx_context_metal* context, struct rjd_slot slot)
{
    struct rjd_gfx_pipeline_state_metal* pipeline_state = rjd_slotmap_get(context->slotmap_pipeline_states, slot);
    pipeline_state->state_render = nil;
    pipeline_state->state_depthstencil = nil;
	rjd_slotmap_erase(context->slotmap_pipeline_states, slot);
}

static inline void rjd_gfx_mesh_destroy_metal(struct rjd_gfx_context_metal* context, struct rjd_slot slot)
{
	struct rjd_gfx_mesh_metal* mesh_metal = rjd_slotmap_get(context->slotmap_meshes, slot);
    
	for (uint32_t i = 0; i < rjd_array_count(mesh_metal->vertex_buffers); ++i) {
		mesh_metal->vertex_buffers[i].buffer = nil;
	}
    
    for (uint32_t i = 0; rjd_array_count(mesh_metal->index_buffers); ++i) {
        mesh_metal->index_buffers[i].buffer = nil;
    }
    
	rjd_array_free(mesh_metal->vertex_buffers);
    rjd_array_free(mesh_metal->index_buffers);
	rjd_slotmap_erase(context->slotmap_meshes, slot);
}

static inline void rjd_gfx_command_buffer_destroy_metal(struct rjd_gfx_context_metal* context, struct rjd_slot slot)
{
	struct rjd_gfx_command_buffer_metal* buffer_metal = rjd_slotmap_get(context->slotmap_command_buffers, slot);
	buffer_metal->buffer = nil;
	rjd_slotmap_erase(context->slotmap_command_buffers, slot);
}
