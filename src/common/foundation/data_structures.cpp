#include "foundation/data_structures.h"

#include <string.h>

namespace Engine
{
	static const u32				k_invalid_index = 0xffffffff;

	// Resource Pool /////////////////////////////////////////////////////////////////////

	void ResourcePool::init(Allocator* allocator_, u32 pool_size_, u32 resource_size_)
	{
		allocator = allocator_;
		pool_size = pool_size_;
		resource_size = resource_size_;

		// Group allocate ( resource size + u32 )
		sizet allocation_size = pool_size * ( resource_size + sizeof( u32 ) );
		memory = (u8*)allocator->allocate( allocation_size, 1 );
		memset( memory, 0, allocation_size );

		// Allocate and add free  indices.
		free_indices = ( u32* )( memory + pool_size * resource_size );
		free_indices_head = 0;

		for (u32 i = 0; i < pool_size; ++i)
		{
			free_indices[i] = i;
		}

	}
	
	void ResourcePool::shutdown()
	{
		if (free_indices_head != 0)
			rprint("Resource pool has unfreed resources. \n");

		for (u32 i = 0; i < free_indices_head; ++i)
		{
			rprint( "\tResource %u\n", free_indices[i] );
		}

		RASSERT( used_indices == 0);

		allocator->deallocate( memory );
	}


	void ResourcePool::free_all_resource()
	{
		free_indices_head = 0;
		used_indices = 0;

		for (uint32_t i = 0; i < pool_size; ++i)
		{
			free_indices[i] = i;
		}
	}

	u32 ResourcePool::obtain_resource()
	{
		if ( free_indices_head < pool_size )
		{
			const u32 free_index = free_indices[ free_indices_head++ ];
			++used_indices;
			return free_index;
		}
		
		// Error: no more resources left.
		RASSERT( false );
		return k_invalid_index;
	}

	void ResourcePool::release_resource( u32 index )
	{
		// TODO: add bits for checking if resource is alive and use bitmasks.
		free_indices[ --free_indices_head ] = index;
		--used_indices;
	}

	void* ResourcePool::access_resource( u32 index )
	{
		if (index != k_invalid_index)
		{
			return &memory[index * resource_size];
		}

		return nullptr;
	}

	const void* ResourcePool::access_resource( u32 index ) const
	{
		if (index != k_invalid_index)
		{
			return &memory[ index * resource_size ];
		}

		return nullptr;
	}

} // namespace Engine