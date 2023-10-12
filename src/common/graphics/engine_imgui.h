#pragma once

#include <stdint.h>

#include <foundation/service.h>

struct ImDrawData;

namespace Engine
{
	struct GpuDevice;
	struct CommandBuffer;
	struct TextureHandle;

	//
	//
	enum ImGuiStyles
	{
		Default = 0,
		GreenBlue,
		DarkRed,
		DarkGold

	}; // enum ImGuiStyles

	struct ImGuiServiceConfiguration
	{
		GpuDevice* gpu;
		void* window_handle;

	}; // struct ImGuiSeriveConfiguration

	//
	//
	struct ImGuiService : public Engine::Service
	{
		ENGINE_DECLARE_SERVICE( ImGuiService );

		void							init(void* configuration) override;
		void							shutdown() override;

		void							new_frame();
		void							render(CommandBuffer& commands);

		// Removes the texture from the cache and destory the associated Descriptor Set.
		void							remove_cached_texture( TextureHandle& texture );

		void							set_style(ImGuiStyles style);

		GpuDevice* gpu;

		static constexpr cstring		k_name = "engine_imgui_service";

	}; // ImGuiService

	// Application Log //////////////////////////////////////////////////////////

	void								imgui_log_init();
	void								imgui_log_shutdown();

	void								imgui_log_draw();

	// FPS graph ///////////////////////////////////////////////////////////////

	void								imgui_fps_init();
	void								imgui_fps_shutdown();
	void								imgui_fps_add(f32 dt);
	void								imgui_fps_draw();

}; // namespace Engine.
