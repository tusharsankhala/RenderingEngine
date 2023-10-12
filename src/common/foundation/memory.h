#pragma once

#include "foundation/platform.h"
#include "foundation/service.h"

#define ENGINE_IMGUI

namespace Engine
{
	// Memory Methods /////////////////////////////////////////////////////
	void		memory_copy(void* destination, void* source, sizet size);

	//
	// Calculate aligned memory size.
	sizet		memory_align( sizet size, sizet alignment );

	// Memory Structs /////////////////////////////////////////////////////
	//
	//
	struct MemoryStatictics
	{
		sizet						allocated_bytes;
		sizet						total_bytes;

		u32							allocation_count;

		void add(sizet a) {
			if (a) {
				allocated_bytes += a;
				++allocation_count;
			}
		}
	};	// struct MemoryStatictics
	
	//
	//
	struct Allocator {
		virtual ~Allocator() {}
		virtual void*						allocate( sizet size, sizet alignment ) = 0;
		virtual void*						allocate( sizet size, sizet alignment, cstring file, i32 line ) = 0;

		virtual void						deallocate( void* pointer ) = 0;
	}; // struct Allocator


	//
	//
	struct StackAllocator : public Allocator {

		void								init(sizet size);
		void								shutdown();

		void* allocate(sizet size, sizet alignment) override;
		void* allocate(sizet size, sizet alignment, cstring file, i32 line) override;

		void								deallocate(void* pointer) override;

		size_t								get_marker();
		void								free_marker(size_t marker);

		void								clear();

		u8* memory = nullptr;
		size_t								total_size = 0;
		size_t								allocated_size = 0;

	}; // struct HeapAllocator

	//
	//
	struct HeapAllocator : public Allocator
	{
		~HeapAllocator() override;

		void								init( sizet size );
		void								shutdown();

#if defined ENGINE_IMGUI
		void								debug_ui();
#endif // ENGINE_IMGUI

		void*								allocate( sizet size, sizet alignment ) override;
		void*								allocate( sizet size, sizet alignment, cstring file, i32 line) override;
		
		void								deallocate( void* pointer ) override;

		void*								tlsf_handle;
		void*								memory;
		size_t								allocated_size = 0;
		size_t								max_size = 0;

	}; // struct HeapAllocator

	//
	// Allocator that can only be reset.
	//
	struct LinearAllocator : public Allocator
	{
		~LinearAllocator();

		void								init( sizet size );
		void								shutdown();

		void*								allocate(sizet size, sizet alignment) override;
		void*								allocate(sizet size, sizet alignment, cstring file, i32 line) override;

		void								deallocate(void* pointer) override;

		void								clear();

		u8*									memory			= nullptr;
		size_t								total_size		= 0;
		size_t								allocated_size	= 0;

	}; // struct LinearAllocator

	// Memory Service /////////////////////////////////////////////////////
	// 
	//
	struct MemoryServiceConfiguration
	{
		sizet								maximum_dynamic_size = 32 * 1024 * 1024;	// Defaults to max 32 MB of dynamic memory.

	}; // struct MemoryServiceConfiguration
	//
	//
	struct MemoryService : public Service
	{
		ENGINE_DECLARE_SERVICE(MemoryService)

		virtual void						init(void* configuration);
		virtual void						shutdown();

#if defined ENGINE_IMGUI
		void								imgui_draw();
#endif // RAPTOR_IMGUI

		// Frame allocator.
		LinearAllocator						scratch_allocator;
		HeapAllocator						system_allocator;

		//
		// Test allocators.
		void								test();

		static constexpr cstring			k_name = "raptor_memory_service";

	}; // struct MemoryService

	// Macro helpers /////////////////////////////////////////////////////////////
	#define ralloca(size, allocator) ((allocator)->allocate( size, 1, __FILE__, __LINE__ ))
	#define rallocam(size, allocator) ((u8*)(allocator)->allocate( size, 1, __FILE__, __LINE__ ))
	#define rallocat(type, allocator) ((type*)(allocator)->allocate( sizeof(type), 1, __FILE__, __LINE__ ))

	#define rallocaa(size, allocator, alignment) ((allocator)->allocate( size, alignment, __FILE__, __LINE__ ))

	#define rfree(pointer, allocator) (allocator)->deallocate( pointer )

	#define rkilo(size)						(size * 1024)
	#define rmega(size)						(size * 1024 * 1024)
	#define rgiga(size)						(size * 1024 * 1024 * 1024)

} // namespace Engine
