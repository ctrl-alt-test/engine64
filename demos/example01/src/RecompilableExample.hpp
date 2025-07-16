#ifndef RECOMPILABLE_EXAMPLE_HPP
#define RECOMPILABLE_EXAMPLE_HPP

#include "engine/runtime/RuntimeCompiler.hpp"

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/IObject.h"
struct IExample : public IObject
{
	virtual void Build(int* result) = 0;
};
#endif // ENABLE_RUNTIME_COMPILATION

#ifdef ENABLE_RUNTIME_COMPILATION
struct ExampleA : public TInterface<runtime::IID_IMeshBuilder, IExample>
{
	void			Build(int* result);

	static ObjectId	objectId;
	static ExampleA*	instance;
	static void		BuildExample(int* result);
};
#else // !ENABLE_RUNTIME_COMPILATION
void				ExampleA(int* result);
#endif // !ENABLE_RUNTIME_COMPILATION

#ifdef ENABLE_RUNTIME_COMPILATION
struct ExampleB : public TInterface<runtime::IID_IMeshBuilder, IExample>
{
	void			Build(int* result);

	static ObjectId	objectId;
	static ExampleB*	instance;
	static void		BuildExample(int* result);
};
#else // !ENABLE_RUNTIME_COMPILATION
void				ExampleB(int* result);
#endif // !ENABLE_RUNTIME_COMPILATION

#endif // RECOMPILABLE_EXAMPLE_HPP
