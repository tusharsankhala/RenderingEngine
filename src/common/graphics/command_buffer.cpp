#include "graphics/command_buffer.h"
#include "graphics/gpu_device.h"


namespace Engine
{
	void CommandBuffer::reset()
	{
		is_recording = false;
		current_render_pass = nullptr;
		current_pipeline = nullptr;
		current_command = 0;
	}

	void CommandBuffer::init(QueueType::Enum type, u32 buffer_size, u32 submit_size, bool baked)
	{
		this->type = type;
		this->buffer_size = buffer_size;
		this->baked = baked;
	}

	void CommandBuffer::terminate()
	{
		is_recording = false;
	}

	//
	// Commands Interface
	//

	void CommandBuffer::bind_pass(RenderPassHandle handle)
	{
		is_recording = true;

		RenderPass* render_pass = device->access_render_pass( handle );

		// Begin/End render pass are valid only for graphics render passes.
		if( current_render_pass && ( current_render_pass->type != RenderPassType::Compute ) && ( render_pass != current_render_pass ) )
		{
			vkCmdEndRenderPass( vk_command_buffer );
		}

		if ( render_pass != current_render_pass && ( render_pass->type != RenderPassType::Compute ) )
		{
			VkRenderPassBeginInfo render_pass_begin{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
			render_pass_begin.framebuffer = render_pass->type == RenderPassType::Swapchain ? device->vulkan_swapchain_framebuffers[ device->vulkan_image_index ] : render_pass->vk_frame_buffer;
			render_pass_begin.renderPass = render_pass->vk_render_pass;

			render_pass_begin.renderArea.offset = { 0, 0 };
			render_pass_begin.renderArea.extent = { render_pass->width, render_pass->height };

			// TODO: this breaks.
			render_pass_begin.clearValueCount = 2;
			render_pass_begin.pClearValues = clears;

			vkCmdBeginRenderPass( vk_command_buffer, &render_pass_begin, VK_SUBPASS_CONTENTS_INLINE );
		}

		// Cache render pass.
		current_render_pass	 = render_pass;
	}

	void CommandBuffer::bind_pipeline(PipelineHandle handle)
	{
		Pipeline* pipeline = device->access_pipeline( handle );
		vkCmdBindPipeline( vk_command_buffer, pipeline->vk_bind_point, pipeline->vk_pipeline );

		// Cache Pipeline.
		current_pipeline = pipeline;
	}

	void CommandBuffer::bind_vertex_buffer(BufferHandle handle, u32 binding, u32 offset)
	{
		Buffer* buffer = device->access_buffer( handle );
		VkDeviceSize offsets[] = { offset };

		VkBuffer vk_buffer = buffer->vk_buffer;
		// TODO: Add global vertex buffer ?
		if ( buffer->parent_buffer.index != k_invalid_index )
		{
			Buffer* parent_buffer = device->access_buffer( buffer->parent_buffer );
			vk_buffer = parent_buffer->vk_buffer;
			offsets[ 0 ] = buffer->global_offset;
		}

		vkCmdBindVertexBuffers( vk_command_buffer, binding, 1, &vk_buffer, offsets );
	}

	void CommandBuffer::bind_index_buffer(BufferHandle handle, u32 offset_, VkIndexType index_type)
	{
		Buffer* buffer = device->access_buffer(handle);
		
		VkBuffer vk_buffer = buffer->vk_buffer;
		VkDeviceSize offset = offset_;

		if ( buffer->parent_buffer.index != k_invalid_index )
		{
			Buffer* parent_buffer = device->access_buffer( buffer->parent_buffer );
			vk_buffer = parent_buffer->vk_buffer;
			offset = buffer->global_offset;
		}

		vkCmdBindIndexBuffer( vk_command_buffer, vk_buffer, offset, index_type );
	}

	void CommandBuffer::bind_descriptor_set(DescriptorSetHandle* handles, u32 num_lists, u32* offset, u32 num_offsets )
	{
		// TODO:
		u32 offsets_cache[ 8 ];
		num_offsets = 0;

		for( u32 l = 0; l < num_lists; ++l )
		{
			DescriptorSet* descriptor_set = device->access_descriptor_set( handles[l] );
			vk_descriptor_sets[l] = descriptor_set->vk_descriptor_set;

			// Search for dynamic buffers.
			const DescriptorSetLayout* descriptor_set_layout = descriptor_set->layout;
			for( u32 i = 0; i < descriptor_set_layout->num_bindings; ++i )
			{
				const DescriptorBinding& rb = descriptor_set_layout->bindings[ i ] ;

				if( rb.type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER )
				{
					// Search for the actual buffer offset.
					const u32 resource_index = descriptor_set->bindings[ i ];
					ResourceHandle buffer_handle = descriptor_set->resources[ resource_index ];
					Buffer* buffer = device->access_buffer( { buffer_handle } );

					offsets_cache[ num_offsets++ ] = buffer->global_offset;
				}
			}
		}

		const u32 k_first_set = 0;
		vkCmdBindDescriptorSets( vk_command_buffer, current_pipeline->vk_bind_point, current_pipeline->vk_pipeline_layout, k_first_set, num_lists,
			vk_descriptor_sets, num_offsets, offsets_cache);
	}

	void	CommandBuffer::set_viewport(const Viewport* viewport)
	{
		VkViewport vk_viewport;

		if( viewport )
		{
			vk_viewport.x			= viewport->rect.x * 1.f;
			vk_viewport.width		= viewport->rect.width * 1.f;
			// Invert Y with negative height and proper offset - Vulkan has unique Clipping Y.
			vk_viewport.y			= viewport->rect.height * 1.f - viewport->rect.y;
			vk_viewport.height		= -viewport->rect.height * 1.f;
			vk_viewport.minDepth	= viewport->min_depth;
			vk_viewport.maxDepth	= viewport->max_depth;
		}
		else
		{
			vk_viewport.x = 0.f;

			if( current_render_pass )
			{
				vk_viewport.width	= current_render_pass->width * 1.f;
				// Invert Y with negative height and proper offset - Vulkan has unique Clipping Y.
				vk_viewport.y		= current_render_pass->height * 1.f;
				vk_viewport.height	= -current_render_pass->height * 1.f;
			}
			else
			{
				vk_viewport.width = device->swapchain_width * 1.f;
				// Invert Y with negative height and proper offset - Vulkan has unique Clipping Y.
				vk_viewport.y = device->swapchain_height * 1.f;
				vk_viewport.height = -device->swapchain_height * 1.f;
			}

			vk_viewport.minDepth = 0.0f;
			vk_viewport.maxDepth = 1.0f;
		}

		vkCmdSetViewport( vk_command_buffer, 0, 1, &vk_viewport );
	}


	void	CommandBuffer::set_scissor(const Rect2DInt* rect)
	{
		VkRect2D vk_scissor;

		if( rect )
		{
			vk_scissor.offset.x			= rect->x;
			vk_scissor.offset.y			= rect->y;
			vk_scissor.extent.width		= rect->width;
			vk_scissor.extent.height	= rect->height;
		}
		else
		{
			vk_scissor.offset.x = 0;
			vk_scissor.offset.y = 0;
			vk_scissor.extent.width = device->swapchain_width;
			vk_scissor.extent.height = device->swapchain_height;
		}

		vkCmdSetScissor( vk_command_buffer, 0, 1, &vk_scissor );
	}

	void CommandBuffer::clear(f32 red, f32 green, f32 blue, f32 alpha)
	{
		clears[0].color = { red, green, blue, alpha };
	}

	void CommandBuffer::clear_depth_stencil(f32 depth, u8 stencil)
	{
		clears[1].depthStencil.depth = depth;
		clears[1].depthStencil.stencil = stencil;
	}

	void CommandBuffer::draw_indexed(TopologyType::Enum topology, u32 index_count, u32 instance_count, u32 first_index, i32 vertex_offset, u32 first_instance)
	{
		vkCmdDrawIndexed( vk_command_buffer, index_count, instance_count, first_index, vertex_offset, first_instance );
	}

	void CommandBuffer::fill_buffer(BufferHandle buffer, u32 offset, u32 size, u32 data)
	{
		Buffer* vk_buffer = device->access_buffer( buffer );
		vkCmdFillBuffer( vk_command_buffer, vk_buffer->vk_buffer, VkDeviceSize( offset ), size ? VkDeviceSize( size ) : VkDeviceSize( vk_buffer->size), data );
	}

	void CommandBuffer::push_marker(const char* name)
	{
		device->push_gpu_timestamp( this, name );

		if ( !device->debug_utils_extension_present )
			return;

		device->push_marker( vk_command_buffer, name );
	}

	void CommandBuffer::pop_marker()
	{
		device->pop_gpu_timestamp(this);

		if ( !device->debug_utils_extension_present )
			return;

		device->pop_marker(vk_command_buffer);
	}

} // namespace Engine