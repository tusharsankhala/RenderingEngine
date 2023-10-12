#include "resource_manager.h"

namespace Engine
{
	void ResourceManager::init(Allocator* allocator_, ResourceFilenameResolver* resolver_)
	{
		this->allocator = allocator_;
		this->filename_resolver = resolver_;

		loaders.init( allocator, 8 );
		compilers.init( allocator, 8 );	
	}

	void ResourceManager::shutdown()
	{
		loaders.shutdown();
		compilers.shutdown();
	}

	void ResourceManager::set_loader(cstring resource_type, ResourceLoader* loader)
	{
		const u64 hashed_name = hash_calculate( resource_type );
		loaders.insert( hashed_name, loader );
	}

	void ResourceManager::set_compiler(cstring resource_type, ResourceCompiler* compiler)
	{
		const u64 hashed_name = hash_calculate( resource_type );
		compilers.insert( hashed_name, compiler );
	}
}