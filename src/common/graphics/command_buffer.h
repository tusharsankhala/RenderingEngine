#pragma once

#include "graphics/gpu_device.h"

namespace Engine
{
	//
	//
	struct CommandBuffer
	{
		void					init( QueueType:: Enum type, u32 buffer_size, u32 submit_size, bool baked );
		void					terminate();

		//
		// Commands Interface
		//

		void					bind_pass( RenderPassHandle handle );
		void					bind_pipeline( PipelineHandle handle );
		void					bind_vertex_buffer( BufferHandle handle, u32 binding, u32 offset );
		void					bind_index_buffer( BufferHandle handle, u32 offset, VkIndexType index_type );
		void					bind_descriptor_set( DescriptorSetHandle* handles, u32 num_lists, u32* offset, u32 num_offsets );

		void					set_viewport( const Viewport* viewport );
		void					set_scissor( const Rect2DInt* rect );

		void					clear( f32 red, f32 green, f32 blue, f32 alpha );
		void					clear_depth_stencil( f32 depth, u8 stencil );

		void                    draw_indexed( TopologyType::Enum topology, u32 index_count, u32 instance_count, u32 first_index, i32 vertex_offset, u32 first_instance );


		void					fill_buffer( BufferHandle buffer, u32 offset, u32 size, u32 data );
		void					push_marker( const char* name );
		void					pop_marker();
		void					reset();

		VkCommandBuffer			vk_command_buffer;

		GpuDevice*				device;

		VkDescriptorSet			vk_descriptor_sets[16];

		RenderPass*				current_render_pass;
		Pipeline*				current_pipeline;
		VkClearValue			clears[2];						// 0 = color, 1 = depth_stencil
		bool					is_recording;

		u32						handle;

		u32						current_command;
		ResourceHandle			resourceHandle;
		QueueType::Enum         type				= QueueType::Graphics;
		u32						buffer_size			= 0;

		bool					baked				= false;	// If baked reset will affect onl;y the read of the commands.
	};
}