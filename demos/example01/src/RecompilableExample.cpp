#include "RecompilableExample.hpp"

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(ExampleA);
ObjectId ExampleA::objectId;
ExampleA* ExampleA::instance = nullptr;
void ExampleA::BuildExample(int* result)
{
	return instance->Build(result);
}

REGISTERCLASS(ExampleB);
ObjectId ExampleB::objectId;
ExampleB* ExampleB::instance = nullptr;
void ExampleB::BuildExample(int* result)
{
	return instance->Build(result);
}
#endif

#ifdef ENABLE_RUNTIME_COMPILATION
void ExampleA::Build(int* result)
#else // !ENABLE_RUNTIME_COMPILATION
void ExampleA(int* result)
#endif // !ENABLE_RUNTIME_COMPILATION
{
	*result = 42;
}

#ifdef ENABLE_RUNTIME_COMPILATION
void ExampleB::Build(int* result)
#else // !ENABLE_RUNTIME_COMPILATION
void ExampleB(int* result)
#endif // !ENABLE_RUNTIME_COMPILATION
{
	*result = 51;
}
