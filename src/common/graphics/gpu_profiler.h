#pragma once

#include "foundation/memory.h"
#include "graphics/gpu_device.h"

namespace Engine
{
	// GPUProfiler //////////////////////////////////////////////////////////

	struct GPUProfiler
	{
		void					init( Allocator* allocator, u32 max_frames );
		void					shutdown();

		void					update( GpuDevice& gpu );

		void					imgui_draw();

		Allocator*				allocator;
		GPUTimestamp*			timestamps;
		u16*					per_frame_active;

		u32						max_frames;
		u32						current_frames;

		f32						max_time;
		f32						min_time;
		f32						average_time;

		f32						max_duration;
		bool					paused;

	}; // struct GPUProfiler

} // namespace Engine
