#pragma once

#include "foundation/platform.h"

#if !defined(_MSC_VER)


#endif

namespace Engine
{
	namespace ColorWriteEnabled
	{
		enum Enum
		{
			Red, Green, Blue, Alpha, All, Count
		};

		enum Mask
		{
			Red_mask = 1 << 0, Green_mask = 1 << 1, Blue_mask = 1 << 2, Alpha_mask = 1 << 3, All_mask = Red_mask | Green_mask | Blue_mask | Alpha_mask
		};

		static const char* s_values_names[] =
		{
			"Red", "Green", "Blue", "Alpha", "All", "Count"
		};

		static const char* ToString(Enum e)
		{
			return ((u32)e < Enum::Count ? s_values_names[(int)e] : "unsupported");
		};

	} // namespace ColorWriteFormat

	namespace FillMode
	{
		enum Enum
		{
			Wireframe, Solid, Point, Count
		};

		enum Mask
		{
			Wireframe_mask = 1 << 0, Solid_mask = 1 << 1, Point_mask = 1 << 2, Count_mask = 1 << 3
		};

		static const char* s_values_names[] =
		{
			"Wireframe", "Solid", "Point", "Count"
		};

		static const char* ToString(Enum e)
		{
			return ((u32)e < Enum::Count ? s_values_names[(int)e] : "unsupported");
		};

	} // namespace FillMode

	namespace TopologyType
	{
		enum Enum
		{
			Unknown, Point, Line, Triangle, Patch, Count
		};

		enum Mask
		{
			Unknown_mask = 1 << 0, Point_mask = 1 << 1, Line_mask = 1 << 2, Triangle_mask = 1 << 3, Patch_mask = 1 << 4, Count_mask = 1 << 5
		};

		static const char* s_values_names[] =
		{
			"Unknown", "Point", "Line", "Triangle", "Patch", "Count"
		};

		static const char* ToString(Enum e)
		{
			return ((u32)e < Enum::Count ? s_values_names[(int)e] : "unsupported");
		};

	} // namespace TopologyType

	namespace ResourceUsageType
	{
		enum Enum
		{
			Immutable, Dynamic, Stream, Count
		};

		enum Mask
		{
			Immutable_mask = 1 << 0, Dynamic_mask = 1 << 1, Stream_mask = 1 << 2, Count_mask = 1 << 3
		};

		static const char* s_values_names[] = 
		{
			"Immutable", "Dynamic", "Stream", "Count"
		};

		static const char* ToString( Enum e )
		{
			return ((u32)e < Enum::Count ? s_values_names[(int)e] : "unsupported" );
		};

	} // namespace ResourceUsageType

	namespace IndexType
	{
		enum Enum
		{
			Uint16, Uint32, Count
		};

		enum Mask
		{
			Uint16_mask = 1 << 0, Uint32_mask = 1 << 1, Count_mask = 1 << 2
		};

		static const char* s_values_names[] =
		{
			"Uint16", "Uint32", "Count"
		};

		static const char* ToString(Enum e)
		{
			return ((u32)e < Enum::Count ? s_values_names[(int)e] : "unsupported");
		};

	} // namespace IndexType
	
	namespace TextureType
	{
		enum Enum
		{
			Texture1D, Texture2D, Texture3D, Texture_1D_Array, Texture_2D_Array, Texture_Cube_Array, Count
		};

		enum Mask
		{
			Texture1D_mask = 1 << 0, Texture2D_mask = 1 << 1, Texture3D_mask = 1 << 2, Texture_1D_Array_mask = 1 << 3, Texture_2D_Array_mask = 1 << 4, Texture_Cube_Array_mask = 1 << 5, Count_mask = 1 << 6
		};

		static const char* s_values_names[] =
		{
			"Texture1D", "Texture2D", "Texture3D", "Texture_1D_Array", "Texture_2D_Array", "Texture_Cube_Array", "Count"
		};

		static const char* ToString(Enum e)
		{
			return ((u32)e < Enum::Count ? s_values_names[(int)e] : "unsupported");
		};

	} // namespace TextureType


	namespace VertexComponentFormat
	{
		enum Enum
		{
			Float, Float2, Float3, Float4, Mat4, Byte, Byte4N, UByte, UByte4N, Short2, Short2N, Short4, Short4N, Uint, Uint2, Uint4, Count
		};

		static const char* s_values_names[] =
		{
			"Float", "Float2", "Float3", "Float4", "Mat4", "Byte", "Byte4N", "UByte", "UByte4N", "Short2", "Short2N", "Short4", "Short4N", "Uint", "Uint2", "Uint4", "Count"
		};

		static const char* ToString(Enum e)
		{
			return ((u32)e < Enum::Count ? s_values_names[(int)e] : "unsupported");
		};

	}; // namespace VertexComponentFormat

	namespace VertexInputRate
	{
		enum Enum
		{
			PerVertex, PerInstance, Count
		};

		enum Mask
		{
			PerVertex_mask = 1 << 0, PerInstance_mask = 1 << 1, Count_mask = 1 << 2
		};

		static const char* s_values_names[] =
		{
			"PerVertex", "PerInstance", "Count"
		};

		static const char* ToString(Enum e)
		{
			return ((u32)e < Enum::Count ? s_values_names[(int)e] : "unsupported");
		};

	}; // namespace VertexInputRate

	namespace QueueType
	{
		enum Enum
		{
			Graphics, Compute, CopyTransfer, Count
		};
	}

	// Manually typed enums /////////////////////////////////////////////////////////
	
	enum DeviceExtensions
	{
		DeviceExtensions_DebugCallback			= 1 << 0,
	};

	namespace TextureFlags
	{
		enum Enum
		{
			Default, RenderTarget, Compute, Count
		};
		
		enum Mask
		{
			Default_mask = 1 << 0, RenderTarget_mask = 1 << 1, Compute_mask = 1 << 2
		};

	}; // namespace TextureFlags

	namespace PipelineStage
	{
		enum Enum
		{
			DrawIndirect = 0, VertexInput = 1, VertexShader = 2, FragmentShader = 3, RenderTarget = 4, ComputeShader = 5, Transfer = 6
		};

		enum Mask
		{
			DrawIndirect_mask = 1 << 0, VertexInput_mask = 1 << 1, VertexShader_mask = 1 << 2, FragmentShader_mask = 1 << 3, RenderTarget_mask = 1 << 4, ComputeShader_mask = 1 << 5, Transfer_mask = 1 << 6
		};

	}; // namespace PipelineStage

	namespace RenderPassType
	{
		enum Enum
		{
			Geometry, Swapchain, Compute
		};

	}; // namespace RenderPassType

	namespace ResourceDeletionType
	{
		enum Enum
		{
			Buffer, Texture, Pipeline, Sampler, DescriptorSetLayout,DescriptorSet, RenderPass, ShaderState, Count
		};

	}; // namespace ResourceDeletionType

	namespace PresentMode
	{
		enum Enum
		{
			Immediate, VSync, VSyncFast, VSyncRelaxed, Count
		}; // enum Enum

	}; // namespace PresentMode

	namespace RenderPassOperation
	{
		enum Enum
		{
			DontCare, Load, Clear, Count
		}; // enum Enum

	}; // namespace RenderPassOperation

	// TODO: taken from the Forge
	typedef enum ResourceState
	{
		RESOURCE_STATE_UNDEFINED = 0,
		RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER = 0x1,
		RESOURCE_STATE_INDEX_BUFFER = 0x2,
		RESOURCE_STATE_RENDER_TARGET = 0x4,
		RESOURCE_STATE_UNORDERED_ACCESS = 0x8,
		RESOURCE_STATE_DEPTH_WRITE = 0x10,
		RESOURCE_STATE_DEPTH_READ = 0x20,
		RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE = 0x40,
		RESOURCE_STATE_PIXEL_SHADER_RESOURCE = 0x80,
		RESOURCE_STATE_SHADER_RESOURCE = 0x40 | 0x80,
		RESOURCE_STATE_STREAM_OUT = 0x100,
		RESOURCE_STATE_INDIRECT_ARGUMENT = 0x200,
		RESOURCE_STATE_COPY_DEST = 0x400,
		RESOURCE_STATE_COPY_SOURCE = 0x800,
		RESOURCE_STATE_GENERIC_READ = (((((0x1 | 0x2) | 0x40) | 0x80) | 0x200) | 0x800),
		RESOURCE_STATE_PRESENT = 0x1000,
		RESOURCE_STATE_COMMON = 0x2000,
		RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE = 0x4000,
		RESOURCE_STATE_SHADING_RATE_SOURCE = 0x8000,
	} ResourceState;

} // namespace Engine