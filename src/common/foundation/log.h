#pragma once

#include "foundation/platform.h"
#include "foundation/service.h"

namespace Engine
{
	typedef void						(*PrintCallback)(const char*); // Additional callback for printing.

	struct LogService : public Service
	{
		ENGINE_DECLARE_SERVICE( LogService );

		void							print_format(cstring format, ... );

		void							set_callback( PrintCallback callback );

		PrintCallback					print_callback = nullptr;

		static constexpr cstring		k_name = "raptor_log_service";
	};

#if defined(_MSC_VER)
	#define rprint(format, ...)			Engine::LogService::instance()->print_format(format, __VA_ARGS__);
	#define rprintret(format, ...)		Engine::LogService::instance()->print_format(format, __VA_ARGS__); Engine::LogService::instance()->print_format("\n");
#else
	#define rprint(format, ...)			Engine::LogService::instance()->print_format(format, __VA_ARGS__);
	#define rprintret(format, ...)		Engine::LogService::instance()->print_format(format, __VA_ARGS__); Engine::LogService::instance()->print_format("\n");
#endif

} // namespace Engine
