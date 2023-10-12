#include <foundation/memory.h>
#include <foundation/memory_utils.h>
#include <foundation/assert.h>
#include <assert.h>

#include <tlsf.h>

#include <stdlib.h>
#include <memory.h>

#if defined ENGINE_IMGUI
#include <imgui/imgui.h>
#endif

// Define this and add StackWalker to heavy memory profile
//#define ENGINE_MEMORY_STACK

//
#define HEAP_ALLOCATOR_STATS

#if defined (ENGINE_MEMORY_STACK)
#include <stackwalker/StackWalker.h>
#endif // RAPTOR_MEMORY_STACK

namespace Engine
{
	//#define RAPTOR_MEMORY_DEBUGS
#if defined (ENGINE_MEMORY_STACK)
#define hy_mem_assert(cond) hy_asssert(cond)
#else
#define hy_mem_assert(cond)
#endif // ENGINE_MEMORY_DEBUGS

	// Memory Service //////////////////////////////////////////////////////////
	static MemoryService	s_memory_service;

	// Locals
	static size_t s_size = rmega(32) + tlsf_size() + 8;

	//
	// Walker methods.
	static void exit_walker(void* ptr, size_t size, int used, void* user);
	static void imgui_walker(void* ptr, size_t size, int used, void* user);

	MemoryService* MemoryService::instance()
	{
		return &s_memory_service;
	}

	void MemoryService::init(void* configuration)
	{
		rprint("Memory Service Init\n");
		MemoryServiceConfiguration* memory_configuration = static_cast<MemoryServiceConfiguration*>(configuration);
		system_allocator.init(memory_configuration ? memory_configuration->maximum_dynamic_size : s_size);
	}

	void MemoryService::shutdown()
	{
		system_allocator.shutdown();

		rprint("Memory Service Shutdown\n ");
	}

	void exit_walker(void* ptr, size_t size, int used, void* user)
	{
		MemoryStatictics* stats = (MemoryStatictics*)user;
		stats->add(used ? size : 0);

		if (used)
		{
			rprint("Found active allocation %p, %llu\n", ptr, size);
		}
	}

#if defined ENGINE_IMGUI
	void imgui_walker(void* ptr, size_t size, int used, void* user)
	{
		u32 memory_size = (u32)size;
		cstring memory_unit = "b";
		if (memory_size > 1024 * 1024)
		{
			memory_size /= 1024 * 1024;
			memory_unit = "kb";
		}
		else if (memory_size > 1024)
		{
			memory_size /= 1024;
			memory_unit = "kb";
		}

		ImGui::Text("\t%p %s size: %4llu %s\n", ptr, used ? "used" : "free", memory_size, memory_unit);

		MemoryStatictics* stats = (MemoryStatictics*)user;
		stats->add(used ? size : 0);
	}

	void MemoryService::imgui_draw()
	{
		if( ImGui::Begin( "Memory Service") )
		{
			system_allocator.debug_ui();
		}

		ImGui::End();
	}

#endif	// ENGINE_IMGUI

	// Memory Methods ////////////////////////////////////////////////////////
	void memory_copy( void* destination, void* source, sizet size )
	{
		memcpy( destination, source, size ); 
	}

	sizet memory_align( sizet size, sizet alignment )
	{
		const sizet alignment_mask = alignment - 1;
		return ( size + alignment_mask ) & ~alignment_mask;		
	}


	// StackAllocator ////////////////////////////////////////////////////////

	void StackAllocator::init(sizet size)
	{
		memory = (u8*)malloc( size );
		allocated_size = 0;
		total_size = size;
	}

	void StackAllocator::shutdown()
	{
		free( memory );
	}

	void* StackAllocator::allocate(sizet size, sizet alignment)
	{
		RASSERT( size > 0 );

		const sizet new_start = memory_align( allocated_size, alignment );
		RASSERT( new_start < total_size );
		const sizet new_allocated_size = new_start + size;
		if( new_allocated_size > total_size )
		{
			hy_mem_assert( false && 'Overflow' );
			return nullptr;
		}

		allocated_size = new_allocated_size;
		return memory + new_start;
	}

	void* StackAllocator::allocate(sizet size, sizet alignment, cstring file, i32 line)
	{
		return allocate( size, alignment );
	}

	void StackAllocator::deallocate(void* pointer)
	{
		RASSERT( pointer >= memory );
		RASSERTM(pointer < memory + total_size, "Out of bound free on linear allocator (outside bounds). Tempting to free %p, %llu after beginning of buffer (memory %p size %llu, allocated %llu)", (u8*)pointer, (u8*)pointer - memory, memory, total_size, allocated_size);
		RASSERTM(pointer < memory + allocated_size, "Out of bound free on linear allocator (inside bounds, after allocated). Tempting to free %p, %llu after beginning of buffer (memory %p size %llu, allocated %llu)", (u8*)pointer, (u8*)pointer - memory, memory, total_size, allocated_size);
	
		const sizet	size_at_pointer = ( u8* )pointer - memory;

		allocated_size = size_at_pointer;
	}

	size_t StackAllocator::get_marker()
	{
		return allocated_size;
	}

	void StackAllocator::free_marker(size_t marker)
	{
		const sizet difference = marker - allocated_size;
		if( difference > 0 )
		{
			allocated_size = marker;
		}
	}

	void StackAllocator::clear()
	{
		allocated_size = 0;
	}

	// HeapAllocator ////////////////////////////////////////////////////////
	HeapAllocator::~HeapAllocator()
	{}

	void HeapAllocator::init(sizet size)
	{
		// Allocate.
		memory = malloc( size );
		max_size = size;
		allocated_size = 0;

		tlsf_handle = tlsf_create_with_pool( memory, size );

		rprint( "HeapAllocator of size %llu crerated\n", size);
	}

	void HeapAllocator::shutdown()
	{
		// Check memory at the application exit.
		MemoryStatictics stats{ 0, max_size };
		pool_t pool = tlsf_get_pool( tlsf_handle );
		tlsf_walk_pool( pool, exit_walker, ( void* )&stats );

		if( stats.allocated_bytes )
		{
			rprint( "HeapAllocator Shutdown.\n==============\nFAILURE! Allocated memory detected, allocated %llu, total %llu\n============\n\n", stats.allocated_bytes, stats.total_bytes );
		}
		else
		{
			rprint( "HeapAllocator Shutdown - all memory free!\n" );
		}

		RASSERTM( stats.allocated_bytes == 0, "Allocations still present. Check your code!" );
		tlsf_destroy( tlsf_handle );

		free( memory );
	}

#if defined ENGINE_IMGUI
	void HeapAllocator::debug_ui()
	{
		ImGui::Separator();
		ImGui::Text( "Heap Allocator" );
		ImGui::Separator();
		MemoryStatictics stats{ 0, max_size };
		pool_t pool = tlsf_get_pool( tlsf_handle );
		tlsf_walk_pool( pool, imgui_walker, ( void* )&stats );

		ImGui::Separator();
		ImGui::Text( "\tAllocation Count %d", stats.allocation_count );
		ImGui::Text( "\tAllocation %llu K, free %llu Mb, total %llu Mb", stats.allocated_bytes / (1024 * 1024), ( max_size - stats.allocated_bytes ) / ( 1024 * 1024 ), max_size / ( 1024 * 1024 ) );
	}
#endif // ENGINE_IMGUI

#if defined ( ENGINE_MEMORY_STACK )
	class EngineStackWalker : public StackWalker
	{
	public:
		EngineStackWalker() : StackWalker() {}
	protected:
		virtual void OnOutput(LPCSTR szText)
		{
			rprint( "\nStack: \n%s\n", szText );
			StackWalker::OnOutput( szText );
		}
	}; // class EngineStackWalker

	void* HeapAllocator::allocate( sizet size, sizet alignment )
	{
		void* mem = tlsf_malloc( tlsf_handle, size );
		rprint( "Mem: %p, size %llu \n", mem, size );
		return mem;
	}

#else
	void* HeapAllocator::allocate(sizet size, sizet alignment)
	{
#if defined ( HEAP_ALLOCATOR_STATS )
		void* allocated_memory = alignment == 1 ? tlsf_malloc( tlsf_handle, size ) : tlsf_memalign( tlsf_handle, alignment, size );
		sizet actual_size = tlsf_block_size( allocated_memory );
		allocated_size += actual_size;


		return allocated_memory;
#else
		tlsf_free(tlsf_handle, pointer);

#endif // HEAP_ALLOCATOR_STATS
	}
#endif // ENGINE_MEMORY_STACK

	void* HeapAllocator::allocate(sizet size, sizet alignment, cstring file, i32 line)
	{
		return allocate( size, alignment );
	}

	void HeapAllocator::deallocate(void* pointer)
	{
#if defined ( HEAP_ALLOCATOR_STATS )
		sizet actual_size = tlsf_block_size( pointer );
		allocated_size -= actual_size;

		tlsf_free( tlsf_handle, pointer );
#else
		tlsf_free( tlsf_handle, pointer );
#endif
	}

	// LinearAllocator //////////////////////////////////////////////////////
	LinearAllocator::~LinearAllocator()
	{}

	void LinearAllocator::init( sizet size )
	{
		memory = ( u8* )malloc( size );
		total_size = size;
		allocated_size = 0;
	}

	void LinearAllocator::shutdown()
	{
		clear();
		free( memory );
	}

	void* LinearAllocator::allocate( sizet size, sizet alignment )
	{
		RASSERT( size > 0 );

		const sizet new_start = memory_align( allocated_size, alignment );
		RASSERT( new_start < total_size );
		const sizet new_allocated_size = new_start + size;
		if( new_allocated_size > total_size )
		{
			hy_mem_assert( false && "Overflow" );
			return nullptr;
		}

		allocated_size = new_allocated_size;
		return memory + new_start;
	}

	void* LinearAllocator::allocate( sizet size, sizet alignment, cstring file, i32 line )
	{
		return allocate( size, alignment );
	}

	void LinearAllocator::deallocate( void* pointer )
	{
		// This allocator does not allocate on a per-pointer base!
	}

	void LinearAllocator::clear()
	{
		allocated_size = 0;
	}
}