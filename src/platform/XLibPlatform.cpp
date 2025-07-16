// Compatibility fix
// See https://stackoverflow.com/a/13405205
#define GL_GLEXT_PROTOTYPES

#include <ctime>
#include <cstdio>
#include <unistd.h>
#include <stdexcept>
#include "engine/core/Assert.hpp"
#include "engine/core/Debug.hpp"
#include "XLibPlatform.hpp"

using namespace platform;

class XLibPlatform* platform::g_platform = nullptr;

namespace
{
	void handleGLFWError(int, const char* msg)
	{
		throw std::runtime_error(msg);
	}

	void initVAOCompatibility()
	{
		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
	}

#ifdef ENABLE_KEYBOARD_AND_MOUSE
#define MAX_INPUT_HANDLERS	8

	CharacterHandler s_characterHandlers[MAX_INPUT_HANDLERS] = { 0 };
	KeyHandler s_keyHandlers[MAX_INPUT_HANDLERS] = { 0 };
	MouseHandler s_mouseHandlers[MAX_INPUT_HANDLERS] = { 0 };
	void* s_handlerContexts[MAX_INPUT_HANDLERS] = { 0 };
	int s_nextHandler = 0;
	bool s_mouseL = false;
	bool s_mouseR = false;
	bool s_mouseM = false;
	int s_mouseX = 0;
	int s_mouseY = 0;

	::platform::KeyCode::Enum GetKeyCode(int ks)
	{
		if (ks >= GLFW_KEY_F1 && ks <= GLFW_KEY_F12)
		{
			return (::platform::KeyCode::Enum)(::platform::KeyCode::keyF1 + (ks - GLFW_KEY_F1));
		}

		if (ks >= GLFW_KEY_KP_0 && ks <= GLFW_KEY_KP_9)
		{
			LOG_INFO("Keypad digit");
			return (::platform::KeyCode::Enum)(::platform::KeyCode::keyNumpad0 + (ks - GLFW_KEY_KP_0));
		}

		if (ks >= GLFW_KEY_0 && ks <= GLFW_KEY_9)
		{
			return (::platform::KeyCode::Enum)(::platform::KeyCode::key0 + (ks - GLFW_KEY_0));
		}

		if (ks >= GLFW_KEY_A && ks <= GLFW_KEY_Z)
		{
			return (::platform::KeyCode::Enum)(::platform::KeyCode::keyA + (ks - GLFW_KEY_A));
		}

		switch (ks)
		{
		case GLFW_KEY_LEFT:			 return ::platform::KeyCode::keyArrowLeft;
		case GLFW_KEY_RIGHT:		 return ::platform::KeyCode::keyArrowRight;
		case GLFW_KEY_UP:			 return ::platform::KeyCode::keyArrowUp;
		case GLFW_KEY_DOWN:			 return ::platform::KeyCode::keyArrowDown;
		case GLFW_KEY_PAGE_UP:		 return ::platform::KeyCode::keyPageUp;
		case GLFW_KEY_PAGE_DOWN:	 return ::platform::KeyCode::keyPageDown;

		case GLFW_KEY_LEFT_ALT:		 return ::platform::KeyCode::keyLeftAlt;
		case GLFW_KEY_RIGHT_ALT:	 return ::platform::KeyCode::keyRightAlt;
		case GLFW_KEY_LEFT_CONTROL:	 return ::platform::KeyCode::keyLeftControl;
		case GLFW_KEY_RIGHT_CONTROL: return ::platform::KeyCode::keyRightControl;
		case GLFW_KEY_LEFT_SHIFT:	 return ::platform::KeyCode::keyLeftShift;
		case GLFW_KEY_RIGHT_SHIFT:	 return ::platform::KeyCode::keyRightShift;

		case GLFW_KEY_ENTER:		 return ::platform::KeyCode::keyEnter;
		case GLFW_KEY_SPACE:		 return ::platform::KeyCode::keySpace;
		case GLFW_KEY_TAB:			 return ::platform::KeyCode::keyTab;
		case GLFW_KEY_ESCAPE:		 return ::platform::KeyCode::keyEscape;

		case GLFW_KEY_PRINT_SCREEN:  return ::platform::KeyCode::keyPrintScreen;
		case GLFW_KEY_SCROLL_LOCK:	 return ::platform::KeyCode::keyScrollLock;
		case GLFW_KEY_PAUSE:		 return ::platform::KeyCode::keyPause;

		case GLFW_KEY_KP_MULTIPLY:	 return ::platform::KeyCode::keyNumpadMultiply;
		case GLFW_KEY_KP_ADD:		 return ::platform::KeyCode::keyNumpadPlus;
		case GLFW_KEY_KP_SUBTRACT:	 return ::platform::KeyCode::keyNumpadMinus;
		case GLFW_KEY_KP_DECIMAL:	 return ::platform::KeyCode::keyNumpadDecimal;
		case GLFW_KEY_KP_DIVIDE:	 return ::platform::KeyCode::keyNumpadDivide;;
		}

		return ::platform::KeyCode::numberOfKeys;
	}

	void keyCallback(GLFWwindow*, int key, int, int action, int)
	{
		const KeyCode::Enum kc = GetKeyCode(key);

		if (kc == KeyCode::keyEscape)
			g_platform->NotifyClose();

		const bool kp = (action == GLFW_PRESS || action == GLFW_REPEAT);
		for (int i = 0; i < MAX_INPUT_HANDLERS; ++i)
		{
			if (s_keyHandlers[i] != nullptr)
			{
				if (s_keyHandlers[i](s_handlerContexts[i], kc, kp))
				{
					break;
				}
			}
		}
	}

	void characterCallback(GLFWwindow*, unsigned int code)
	{
		for (int i = 0; i < MAX_INPUT_HANDLERS; ++i)
		{
			if (s_characterHandlers[i] != nullptr)
			{
				if (s_characterHandlers[i](s_handlerContexts[i], code))
				{
					break;
				}
			}
		}
	}

	void mouseEvent(int wheel)
	{
		for (int i = 0; i < MAX_INPUT_HANDLERS; ++i)
		{
			if (s_mouseHandlers[i] != nullptr)
			{
				if (s_mouseHandlers[i](s_handlerContexts[i],
									   s_mouseX,
									   s_mouseY,
									   wheel,
									   s_mouseL,
									   s_mouseR,
									   s_mouseM,
									   false,
									   false))
				{
					break;
				}
			}
		}
	}

	void mousePosCallback(GLFWwindow*, double x, double y)
	{
		s_mouseX = x;
		s_mouseY = y;
		mouseEvent(0);
	}

	void mouseWheelCallback(GLFWwindow*, double, double w)
	{
		mouseEvent(50 * w);
	}

	void mouseButtonCallback(GLFWwindow*, int button, int action, int)
	{
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:   s_mouseL = GLFW_PRESS == action; break;
		case GLFW_MOUSE_BUTTON_RIGHT:  s_mouseR = GLFW_PRESS == action; break;
		case GLFW_MOUSE_BUTTON_MIDDLE: s_mouseM = GLFW_PRESS == action; break;
		default: break;
		}
		mouseEvent(0);
	}

#endif // ENABLE_KEYBOARD_AND_MOUSE

}

XLibPlatform::XLibPlatform(const char* windowTitle,
						   int width, int height,
						   int /* monitorLeft  */, int /* monitorTop    */,
						   int /* monitorWidth */, int /* monitorHeight */,
						   bool fullscreen)
{
	g_platform = this;

	glfwInit();
	glfwSetErrorCallback(handleGLFWError);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	auto* monitor = fullscreen ? glfwGetPrimaryMonitor() : NULL;
	m_window = glfwCreateWindow(width, height, windowTitle, monitor, NULL);
	glfwMakeContextCurrent(m_window);

	initVAOCompatibility();

#ifdef ENABLE_KEYBOARD_AND_MOUSE
	glfwSetKeyCallback(m_window, keyCallback);
	glfwSetCharCallback(m_window, characterCallback);
	glfwSetScrollCallback(m_window, mouseWheelCallback);
	glfwSetCursorPosCallback(m_window, mousePosCallback);
	glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
#endif // ENABLE_KEYBOARD_AND_MOUSE
}

XLibPlatform::~XLibPlatform()
{
	glfwTerminate();
}

long XLibPlatform::GetTime() const
{
	timespec t;
	clock_gettime(CLOCK_REALTIME, &t);
	return t.tv_sec * 1000 + t.tv_nsec / 1000000;
}

#ifdef ENABLE_PROFILING
long long XLibPlatform::GetPreciseTime() const
{
	timespec t;
#ifdef CLOCK_MONOTONIC_RAW
	clock_gettime(CLOCK_MONOTONIC_RAW, &t);
#else
	clock_gettime(CLOCK_MONOTONIC, &t);
#endif
	return t.tv_sec * 1000000 + t.tv_nsec / 1000;
}
#endif // ENABLE_PROFILING

void XLibPlatform::Sleep(long milliseconds) const
{
	usleep(milliseconds * 1000);
}

void XLibPlatform::SetWindowTitle(const char* title) const
{
	glfwSetWindowTitle(m_window, title);
}

bool XLibPlatform::HandleMessages() const
{
	glfwPollEvents();
	return !glfwWindowShouldClose(m_window);
}

void XLibPlatform::SwapBuffers() const
{
	glfwSwapBuffers(m_window);
}

void XLibPlatform::NotifyClose() const
{
	glfwSetWindowShouldClose(m_window, GLFW_TRUE);
}

void XLibPlatform::GetMonitorRect(int* out_left, int* out_top, int* out_width, int* out_height)
{
	*out_left = 0;
	*out_top = 0;
	*out_width = 0;
	*out_height = 0;
}

#ifdef ENABLE_KEYBOARD_AND_MOUSE

void XLibPlatform::AddInputHandler(CharacterHandler characterHandler,
								   KeyHandler keyHandler,
								   MouseHandler mouseHandler,
								   void* handlerContext)
{
	ASSERT(s_nextHandler < MAX_INPUT_HANDLERS);
	s_characterHandlers[s_nextHandler] = characterHandler;
	s_keyHandlers[s_nextHandler] = keyHandler;
	s_mouseHandlers[s_nextHandler] = mouseHandler;
	s_handlerContexts[s_nextHandler] = handlerContext;
	++s_nextHandler;
}

#endif // ENABLE_KEYBOARD_AND_MOUSE
