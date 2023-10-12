#pragma once

#include "foundation/platform.h"

namespace Engine
{
	struct Service
	{
		virtual void						init( void* configuration ) { }
		virtual void						shutdown() { }

	}; // struct Service

	#define ENGINE_DECLARE_SERVICE(Type)		static Type* instance();

} // namespace Engine