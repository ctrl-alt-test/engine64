#include "engine/container/Utils.hpp"
#include "gfx/OpenGL/OpenGLLayer.hpp"
#include "gfx/ShadingParameters.hpp"
#include "platform/Platform.hpp"

#if DEBUG
#include <iostream>
#include <sstream>
#endif

/// <summary>
/// Prototype of a test function.
/// It receives a gfxLayer ready to use, and returns true if the test
/// is successful, false otherwise.
/// </summary>
typedef bool (*FunctionalTest)(Gfx::IGraphicLayer* gfxLayer);

bool dummyTest(Gfx::IGraphicLayer* gfxLayer)
{
	return true;
}

bool dummyBrokenTest(Gfx::IGraphicLayer* gfxLayer)
{
	return false;
}

bool StorageBufferTest(Gfx::IGraphicLayer* gfxLayer)
{
#if GFX_ENABLE_STORAGE_BUFFER_OBJECT
	// Test data.
	const size_t testDataSize = 1024;
	std::uint32_t testData[testDataSize];
	for (size_t i = 0; i < testDataSize; ++i)
	{
		testData[i] = 42 + 2 * i;
	}

	// Create Storage Buffer.
	Gfx::StorageBufferID storageBuffer = gfxLayer->CreateStorageBuffer();
	if (storageBuffer == Gfx::StorageBufferID::InvalidID)
	{
		return false;
	}

	// Load data into Storage Buffer.
	gfxLayer->LoadStorageBuffer(storageBuffer, sizeof(testData), testData);

	// Read data from Storage Buffer.
	std::uint32_t readData[testDataSize];
	memset(readData, 0, sizeof(readData));
	gfxLayer->ReadStorageBuffer(storageBuffer, sizeof(readData), readData);

	// Validate that the data read matches the data loaded.
	if (memcmp(testData, readData, sizeof(testData)) != 0)
	{
		return false;
	}

	gfxLayer->DestroyStorageBuffer(storageBuffer);
#endif // GFX_ENABLE_STORAGE_BUFFER_OBJECT

	return true;
}

bool ComputeShaderTest(Gfx::IGraphicLayer* gfxLayer)
{
#if GFX_ENABLE_COMPUTE_SHADERS && GFX_ENABLE_STORAGE_BUFFER_OBJECT
	// Test data.
	const size_t testDataSize = 1024;
	std::uint32_t inputData[testDataSize];
	for (size_t i = 0; i < testDataSize; ++i)
	{
		inputData[i] = 42 + 2 * i;
	}

	// Create shader.
	Gfx::ShaderID computeShader = gfxLayer->CreateShader();
	if (computeShader == Gfx::ShaderID::InvalidID)
	{
		return false;
	}

	const char* computeShaderSource = R"(
        #version 450
        layout(local_size_x = 1) in;
        layout(std430) buffer ShaderData {
            int data[];
        };
        void main() {
            data[gl_GlobalInvocationID.x] *= 2;
        }
    )";
	const Gfx::ShaderStage shaderStage = { Gfx::ShaderType::ComputeShader, computeShaderSource, __FILE__ };
	gfxLayer->LoadShader(computeShader, &shaderStage, 1);

	// Prepare Storage Buffer and compute parameters.
	Gfx::StorageBufferID storageBuffer = gfxLayer->CreateStorageBuffer();
	gfxLayer->LoadStorageBuffer(storageBuffer, sizeof(inputData), inputData);
	Gfx::Uniform storageBufferUniform = { "ShaderData", 1, Gfx::UniformType::StorageBufferInput, {} };
	storageBufferUniform.storageBufferId = storageBuffer;

	Gfx::ComputeParameters computeParameters = Gfx::ComputeParameters();
	computeParameters.uniforms.add(storageBufferUniform);

	// Dispatch compute shader.
	gfxLayer->Compute(computeShader, computeParameters, testDataSize);

	// Read back and validate that the data matches the expected result.
	std::uint32_t outputData[testDataSize];
	memset(outputData, 0, sizeof(outputData));
	gfxLayer->ReadStorageBuffer(storageBuffer, sizeof(outputData), outputData);

	for (size_t i = 0; i < testDataSize; ++i)
	{
		if (outputData[i] != inputData[i] * 2)
		{
			return false;
		}
	}

	gfxLayer->DestroyStorageBuffer(storageBuffer);
	gfxLayer->DestroyShader(computeShader);
#endif // GFX_ENABLE_COMPUTE_SHADERS && GFX_ENABLE_STORAGE_BUFFER_OBJECT

	return true;
}

bool UniformBufferTest(Gfx::IGraphicLayer* gfxLayer)
{
#if GFX_ENABLE_UNIFORM_BUFFER_OBJECT && GFX_ENABLE_COMPUTE_SHADERS && GFX_ENABLE_STORAGE_BUFFER_OBJECT
	// Test data.
	const size_t testDataSize = 1024;
	std::uint32_t inputData[testDataSize];
	for (size_t i = 0; i < testDataSize; ++i)
	{
		inputData[i] = 42 + 2 * i;
	}

	// Create shader.
	Gfx::ShaderID computeShader = gfxLayer->CreateShader();
	if (computeShader == Gfx::ShaderID::InvalidID)
	{
		return false;
	}

	const char* computeShaderSource = R"(
        #version 450
        layout(local_size_x = 1) in;
        layout(std140) uniform ShaderParams {
            int offset;
            int multiplier;
        };
        layout(std430) buffer ShaderData {
            int data[];
        };
        void main() {
            data[gl_GlobalInvocationID.x] += offset;
            data[gl_GlobalInvocationID.x] *= multiplier;
        }
    )";
	const Gfx::ShaderStage shaderStage = { Gfx::ShaderType::ComputeShader, computeShaderSource, __FILE__ };
	gfxLayer->LoadShader(computeShader, &shaderStage, 1);

	// Prepare the Uniform Buffer, Storage Buffer and compute parameters.
	struct ShaderUniformParams {
		int offset;
		int multiplier;
	};
	ShaderUniformParams shaderParams = { 3, 2 };

	Gfx::UniformBufferID uniformBuffer = gfxLayer->CreateUniformBuffer();
	gfxLayer->LoadUniformBuffer(uniformBuffer, sizeof(ShaderUniformParams), &shaderParams);
	Gfx::Uniform uniformBufferUniform = { "ShaderParams", 1, Gfx::UniformType::UniformBuffer, {} };
	uniformBufferUniform.uniformBufferId = uniformBuffer;

	Gfx::StorageBufferID storageBuffer = gfxLayer->CreateStorageBuffer();
	gfxLayer->LoadStorageBuffer(storageBuffer, sizeof(inputData), inputData);
	Gfx::Uniform storageBufferUniform = { "ShaderData", 1, Gfx::UniformType::StorageBufferInput, {} };
	storageBufferUniform.storageBufferId = storageBuffer;

	Gfx::ComputeParameters computeParameters = Gfx::ComputeParameters();
	computeParameters.uniforms.add(uniformBufferUniform);
	computeParameters.uniforms.add(storageBufferUniform);

	// Dispatch compute shader.
	gfxLayer->Compute(computeShader, computeParameters, testDataSize);

	// Read back and validate that the data matches the expected result.
	std::uint32_t outputData[testDataSize];
	memset(outputData, 0, sizeof(outputData));
	gfxLayer->ReadStorageBuffer(storageBuffer, sizeof(outputData), outputData);

	for (size_t i = 0; i < testDataSize; ++i)
	{
		if (outputData[i] != (inputData[i] + shaderParams.offset) * shaderParams.multiplier)
		{
			return false;
		}
	}

	gfxLayer->DestroyStorageBuffer(storageBuffer);
	gfxLayer->DestroyShader(computeShader);
#endif // GFX_ENABLE_UNIFORM_BUFFER_OBJECT && GFX_ENABLE_COMPUTE_SHADERS && GFX_ENABLE_STORAGE_BUFFER_OBJECT

	return true;
}

FunctionalTest tests[] = {
	//dummyTest,
	//dummyBrokenTest,
	StorageBufferTest,
	ComputeShaderTest,
	UniformBufferTest,
};

/// <summary>
/// Run all functional tests.
/// The return value indicates how the tests went:
/// - A negative value is returned if the setup failed outside of the tests.
/// - Zero is returned if all tests pass.
/// - A positive value indicates the number of tests failing.
/// </summary>
int RunAllTests()
{
	platform::Platform platform("Functional Tests",
								1024, 768, 0, 0, 1920, 1080, false);

	Gfx::IGraphicLayer* gfxLayer = new Gfx::OpenGLLayer();
	if (!gfxLayer->CreateRenderingContext())
	{
#if _HAS_EXCEPTIONS
		Debug::TerminateOnFatalError("Could not load graphics API.");
#endif
		return -1;
	}

	int failureCount = 0;
	for (size_t i = 0; i < ARRAY_LEN(tests); ++i)
	{
		FunctionalTest test = tests[i];
		if (!test(gfxLayer))
		{
			++failureCount;
		}
	}

	gfxLayer->DestroyRenderingContext();
	return failureCount;
}

int __cdecl main()
{
	LOG_INFO("Starting FunctionalTests");
	int result = 0;

#if _HAS_EXCEPTIONS
	try
	{
		result = RunAllTests();
	}
	catch (std::exception* e)
	{
		LOG_FATAL(e->what());
		result = -1;
	}
#else // !_HAS_EXCEPTIONS
	result = RunAllTests();
#endif // !_HAS_EXCEPTIONS

	if (result == 0)
	{
		LOG_INFO("All %d test(s) passed.", ARRAY_LEN(tests));
	}
	else if (result < 0)
	{
		LOG_INFO("Could not run tests.");
	}
	else
	{
		LOG_INFO("%d test(s) failed out of %d.", result, ARRAY_LEN(tests));
	}

	LOG_INFO("End of FunctionalTests");
	return result;
}
