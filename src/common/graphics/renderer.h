#pragma once

#include "graphics/gpu_device.h"
#include "graphics/gpu_resource.h"

#include "foundation/resource_manager.h"

namespace Engine
{
	struct Renderer;

	//
	// Main class responsible for handling all high level resources
	//
	struct BufferResource : public Engine::Resource
	{
		BufferHandle							handle;
		u32										pool_index;
		BufferDescription						desc;

		static constexpr cstring				k_type = "engine_buffer_type";
		static u64								k_type_hash;

	}; // struct BufferResource

	//
	//
	struct TextureResource : public Engine::Resource
	{
		TextureHandle							handle;
		u32										pool_index;
		TextureDescription						desc;

		static constexpr cstring				k_type = "engine_texture_type";
		static u64								k_type_hash;

	}; // struct BufferResource

	//
	//
	struct SamplerResource : public Engine::Resource
	{
		SamplerHandle							handle;
		u32										pool_index;
		SamplerDescription						desc;

		static constexpr cstring				k_type = "engine_sampler_type";
		static u64								k_type_hash;

	}; // struct BufferResource

	// ResourceCache ////////////////////////////////////////////////
	//
	struct ResourceCache
	{
		void									init( Allocator* allocator);
		void									shutdown( Renderer* renderer );

		FlatHashMap<u64, TextureResource*>		textures;
		FlatHashMap<u64, BufferResource*>		buffers;
		FlatHashMap<u64, SamplerResource*>		samplers;

	}; // struct ResourceCache

	//
	// Renderer /////////////////////////////////////////////////////

	struct RendererCreation
	{
		Engine::GpuDevice*						gpu;
		Allocator*								allocator;
	};

	//
	// Main class responsible for handling all high level resources
	//
	struct Renderer : public Service
	{
		ENGINE_DECLARE_SERVICE( Renderer );

		void									init( const RendererCreation& creation );
		void									shutdown();

		void									set_loaders( Engine::ResourceManager* manager );

		void									begin_frame();
		void									end_frame();

		void									resize_swapchain( u32 width, u32 height );

		f32										aspect_ratio() const;

		// Creation/Destruction
		BufferResource*							create_buffer( const BufferCreation& creation );
		BufferResource*							create_buffer( VkBufferUsageFlags type, ResourceUsageType::Enum usage, u32 size, void* data, cstring name );

		TextureResource*						create_texture( const TextureCreation& creation );
		TextureResource*						create_texture( cstring name, cstring filename );

		SamplerResource*						create_sampler( const SamplerCreation& creation );

		void									destroy_buffer( BufferResource* buffer );
		void									destroy_texture( TextureResource* texture );
		void									destroy_sampler( SamplerResource* sampler );

		// Update resources
		void*									map_buffer( BufferResource* buffer, u32 offset = 0, u32 size = 0 );
		void									unmap_buffer( BufferResource* buffer );

		CommandBuffer*							get_command_buffer( QueueType::Enum type, bool begin ) { return gpu->get_command_buffer( type, begin); }
		void									queue_command_buffer( Engine::CommandBuffer* commands ) { gpu->queue_command_buffer( commands ); }

		ResourcePoolTyped<TextureResource>		textures;
		ResourcePoolTyped<BufferResource>		buffers;
		ResourcePoolTyped<SamplerResource>		samplers;

		ResourceCache							resource_cache;
		Engine::GpuDevice*						gpu;

		u16										width;
		u16										height;
	
		static constexpr cstring				k_name = "engine_rendering_service";
	}; // struct Renderer
}