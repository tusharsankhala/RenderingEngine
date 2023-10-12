#include "graphics/gpu_profiler.h"

#include "graphics/renderer.h"

#include <imgui/imgui.h>
#include <cmath>

namespace Engine
{
	// GPU task names to color.
	Engine::FlatHashMap<u64, u32>	name_to_color;

	static u32						initial_frames_paused = 3;

	void GPUProfiler::init(Allocator* allocator_, u32 max_frames_ )
	{
		allocator = allocator_;
		max_frames = max_frames_;
		timestamps = (GPUTimestamp*)ralloca( sizeof( GPUTimestamp ) * max_frames * 32, allocator );
		per_frame_active = ( u16* )ralloca( sizeof( u16 ) * max_frames, allocator );

		max_duration = 16.666f;
		current_frames = 0;;
		min_time = max_time = average_time = 0.f;
		paused = false;
		
		memset( per_frame_active, 0, 2 * max_frames );

		name_to_color.init( allocator, 16 );
		name_to_color.set_default_value( u32_max );
	}

	void GPUProfiler::shutdown()
	{
		name_to_color.shutdown();

		rfree( timestamps, allocator );
		rfree( per_frame_active, allocator);
	}

	void GPUProfiler::update(GpuDevice& gpu)
	{
	}

	void GPUProfiler::imgui_draw()
	{
	}

} // namespace Engine