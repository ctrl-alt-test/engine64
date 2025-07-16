//
// A minimal example showing how to set up a window and its OpenGL
// render buffer using the Ctrl-Alt-Test size-coding demo engine, and
// evaluate how big the resulting binary is.
//

#include "engine/debug/Debug.hpp"
#include "gfx/OpenGL/OpenGLLayer.hpp"
#include "platform/Platform.hpp"

//
// Example of a class that could implement the demo.
//
class ExampleDemoClass
{
public:
	explicit ExampleDemoClass(Gfx::IGraphicLayer* gfxLayer): m_gfxLayer(gfxLayer) {}

	//
	// The Draw function is called each frame, with the time elapsed since
	// the beginning of the main loop.
	//
	void Draw(long currentTime)
	{
		const float red = (0.2f + 0.4f * ((currentTime % 3000) / 2999.f));
		const float green = (0.2f + 0.4f * (((currentTime + 1000) % 3000) / 2999.f));
		const float blue = (0.2f + 0.4f * (((currentTime + 2000) % 3000) / 2999.f));

		// In this example, nothing is drawn, but the render buffer is
		// cleared with different colours to demonstrate the use of
		// currentTime.
		m_gfxLayer->ClearFrameBuffer(Gfx::FrameBufferID::InvalidID, red, green, blue, true);
	}

private:
	Gfx::IGraphicLayer* m_gfxLayer;
};

//
// The run function is the actual top level function of the demo
// project. It sets up what it needs, before entering the loop.
//
inline void run()
{
	// Initialise the window.
	// Here, a 1024x768 (non fullscreen) window titled "Example 01".
	// The screen is just assumed to be 1920x1080, to center the window.
	platform::Platform platform("Example 01", 1024, 768, 0, 0, 1920, 1080, false);

	// Initialise the OpenGL abstraction layer.
	Gfx::OpenGLLayer gfxLayer;
	if (!gfxLayer.CreateRenderingContext()) {
#if _HAS_EXCEPTIONS
		Debug::TerminateOnFatalError("Could not load graphics API.");
#endif
		return;
	}

	// Initialise the demo example.
	ExampleDemoClass exampleDemo(&gfxLayer);

	// Time is started after everything is initialised, right before
	// the main loop.
	const long startTime = platform.GetTime();

	// Loop until an exit message is received.
	while (platform.HandleMessages()) {
		const long currentTime = platform.GetTime() - startTime;

		// The Draw function of the demo is called once per frame.
		// Various other update functions could be called as well, but
		// this example is minimal.
		exampleDemo.Draw(currentTime);

		gfxLayer.EndFrame();
		platform.SwapBuffers();
	}

	gfxLayer.DestroyRenderingContext();
}

//
// The entry point immediately calls the run function and does nothing
// else part from a start and end log.
//
int __cdecl main()
{
	LOG_INFO("Starting Example01");

#if _HAS_EXCEPTIONS
	try {
		run();
	} catch (std::exception* e) {
		LOG_FATAL(e->what());
		return 1;
	}
#else  // !_HAS_EXCEPTIONS
	run();
#endif // !_HAS_EXCEPTIONS

	LOG_INFO("Ending Example 01");
	return 0;
}
