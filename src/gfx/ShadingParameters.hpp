#pragma once

#include "BlendingMode.hpp"
#include "GraphicLayerConfig.hpp"
#include "PolygonMode.hpp"
#include "ResourceID.hpp"
#include "Uniform.hpp"
#include "engine/container/Array.hpp"
// FIXME: ideally Gfx should not have dependency over Engine.

namespace Gfx
{
	struct ShaderType
	{
		enum Enum {
			FragmentShader = GL_FRAGMENT_SHADER,
			GeometryShader = GL_GEOMETRY_SHADER,
			VertexShader = GL_VERTEX_SHADER,
			TesselationControlShader = GL_TESS_CONTROL_SHADER,
			TesselationEvaluationShader = GL_TESS_EVALUATION_SHADER,
#if GFX_ENABLE_COMPUTE_SHADERS
			ComputeShader = GL_COMPUTE_SHADER,
#endif // GFX_ENABLE_COMPUTE_SHADERS
		};
	};

	struct ShaderStage
	{
		ShaderType::Enum			shaderType;
		const char*					source;
		const char*					sourceInfo;
	};

#if GFX_ENABLE_COMPUTE_SHADERS
	/// <summary>
	/// Shading parameters for a compute. Consists of a list of uniforms.
	/// </summary>
	struct ComputeParameters
	{
		Container::Array<Uniform>	uniforms; // For data input/output and shader parameters.

		ComputeParameters();
		ComputeParameters(const ComputeParameters& src);
		ComputeParameters& operator = (const ComputeParameters& src);
	};
#endif // GFX_ENABLE_COMPUTE_SHADERS

	/// <summary>
	/// Shading parameters for a drawcall. Consists in a blending mode,
	/// a set of textures, a shader and its set of parameters.
	/// </summary>
	struct ShadingParameters
	{
		BlendingMode				blendingMode;
		int							numberOfInstances;
		PolygonMode::Enum			polygonMode;
		ShaderID					shader;
		Container::Array<Uniform>	uniforms;

		ShadingParameters();
		ShadingParameters(const ShadingParameters& src);
		ShadingParameters& operator = (const ShadingParameters& src);
	};
}
