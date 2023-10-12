#pragma once

#include "foundation/log.h"

namespace Engine
{
#define RASSERT( condition )	if (!(condition)) { rprint(ENGINE_FILELINE("FALSE\n")); ENGINE_DEBUG_BREAK }
#if defined(_MSC_VER)
	#define RASSERTM( condition, message, ... ) if (!(condition)) { rprint(ENGINE_FILELINE(ENGINE_CONCAT(message, "\n")), __VA_ARGS__); ENGINE_DEBUG_BREAK }
#else
	#define RASSERTM( condition, message, ... ) if (!(condition)) { rprint(ENGINE_FILELINE(ENGINE_CONCAT(message, "\n")), ## __VA_ARGS__); ENGINE_DEBUG_BREAK }
#endif

} // namespace engine
