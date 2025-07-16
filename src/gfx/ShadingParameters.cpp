#include "ShadingParameters.hpp"

#include "engine/container/Array.hxx"
// FIXME: ideally Gfx should not have dependency over Engine.

using namespace Gfx;

#ifndef _WIN32

// Type instantiation, to force the compiler to put methods in this
// compilation unit; clang++ is stricter on this kind of stuff than
// vc++ it seems.

template class Container::Array<Gfx::Uniform>;

#endif

#if GFX_ENABLE_COMPUTE_SHADERS
ComputeParameters::ComputeParameters() :
	uniforms(GFX_MAX_UNIFORMS)
{
}

ComputeParameters::ComputeParameters(const ComputeParameters& src)
{
	*this = src;
}

ComputeParameters& ComputeParameters::operator = (const ComputeParameters& src)
{
	uniforms.copyFrom(src.uniforms);
	return *this;
}
#endif // GFX_ENABLE_COMPUTE_SHADERS

ShadingParameters::ShadingParameters():
	blendingMode(BlendingMode::Opaque),
	numberOfInstances(1),
	polygonMode(PolygonMode::Filled),
	shader(ShaderID::InvalidID),
	uniforms(GFX_MAX_UNIFORMS)
{
}

ShadingParameters::ShadingParameters(const ShadingParameters& src)
{
	*this = src;
}

ShadingParameters& ShadingParameters::operator = (const ShadingParameters& src)
{
	blendingMode = src.blendingMode;
	numberOfInstances = src.numberOfInstances;
	polygonMode = src.polygonMode;
	shader = src.shader;
	uniforms.copyFrom(src.uniforms);

	return *this;
}
