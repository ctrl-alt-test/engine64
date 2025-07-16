// Binding of OpenGL functions.
//
// The following code is heavily based on Inigo Quilez (iq/RGBA)'s
// 64k framework.
// https://www.iquilezles.org/code/framework64k/framework64k.htm

#pragma once

#ifdef _WIN32
#include <windows.h>
#elif LINUX
#include <GL/glx.h>
// glx.h includes X11/X.h, which defines things like None and Always
// This conflicts with our code.
#undef None
#undef Always
#endif // _WIN32

#if DEBUG
#define NUM_DEBUG_FUNCTIONS 1
#else // !DEBUG
#define NUM_DEBUG_FUNCTIONS 0
#endif // !DEBUG

#define NUM_FUNCTIONS (8+7+5+16+12+12+5+5+NUM_DEBUG_FUNCTIONS)

namespace Gfx
{
	extern void*	opengl_functions[NUM_FUNCTIONS];

	/// <summary>
	/// Loads the OpenGL extension functions.
	/// </summary>
	///
	/// <returns>True if all functions are successfully bound, false
	/// otherwise.</returns>
	bool			InitializeOpenGLExtensions();
}

// vbo-ibo (8)
#define glBindBuffer                  ((PFNGLBINDBUFFERPROC)              ::Gfx::opengl_functions[0])
#define glBindBufferBase              ((PFNGLBINDBUFFERBASEPROC)          ::Gfx::opengl_functions[1])
#define glBufferData                  ((PFNGLBUFFERDATAPROC)              ::Gfx::opengl_functions[2])
#define glBufferSubData               ((PFNGLBUFFERSUBDATAPROC)           ::Gfx::opengl_functions[3])
#define glDeleteBuffers               ((PFNGLDELETEBUFFERSPROC)           ::Gfx::opengl_functions[4])
#define glGenBuffers                  ((PFNGLGENBUFFERSPROC)              ::Gfx::opengl_functions[5])
#define glDrawArraysInstanced         ((PFNGLDRAWARRAYSINSTANCEDPROC)     ::Gfx::opengl_functions[6])
#define glDrawElementsInstanced       ((PFNGLDRAWELEMENTSINSTANCEDPROC)   ::Gfx::opengl_functions[7])

// Vertex attributes (7)
#define glBindAttribLocation          ((PFNGLBINDATTRIBLOCATIONPROC)      ::Gfx::opengl_functions[8])
#define glDisableVertexAttribArray    ((PFNGLDISABLEVERTEXATTRIBARRAYPROC)::Gfx::opengl_functions[9])
#define glEnableVertexAttribArray     ((PFNGLENABLEVERTEXATTRIBARRAYPROC) ::Gfx::opengl_functions[10])
#define glVertexAttrib1f              ((PFNGLVERTEXATTRIB1FPROC)          ::Gfx::opengl_functions[11])
#define glVertexAttrib3f              ((PFNGLVERTEXATTRIB3FPROC)          ::Gfx::opengl_functions[12])
#define glVertexAttrib4f              ((PFNGLVERTEXATTRIB4FPROC)          ::Gfx::opengl_functions[13])
#define glVertexAttribPointer         ((PFNGLVERTEXATTRIBPOINTERPROC)     ::Gfx::opengl_functions[14])

// Textures (5)
#define glActiveTexture               ((PFNGLACTIVETEXTUREPROC)           ::Gfx::opengl_functions[15])
#define glClientActiveTexture         ((PFNGLCLIENTACTIVETEXTUREPROC)     ::Gfx::opengl_functions[16])
#define glGenerateMipmap              ((PFNGLGENERATEMIPMAPPROC)          ::Gfx::opengl_functions[17])
#define glMultiTexCoord4fv            ((PFNGLMULTITEXCOORD4FVPROC)        ::Gfx::opengl_functions[18])
#define glTexImage3D                  ((PFNGLTEXIMAGE3DPROC)              ::Gfx::opengl_functions[19])

// Shaders (16)
#define glAttachShader                ((PFNGLATTACHSHADERPROC)            ::Gfx::opengl_functions[20])
#define glCompileShader               ((PFNGLCOMPILESHADERPROC)           ::Gfx::opengl_functions[21])
#define glCreateProgram               ((PFNGLCREATEPROGRAMPROC)           ::Gfx::opengl_functions[22])
#define glCreateShader                ((PFNGLCREATESHADERPROC)            ::Gfx::opengl_functions[23])
#define glDeleteProgram               ((PFNGLDELETEPROGRAMPROC)           ::Gfx::opengl_functions[24])
#define glDeleteShader                ((PFNGLDELETESHADERPROC)            ::Gfx::opengl_functions[25])
#define glDetachShader                ((PFNGLDETACHSHADERPROC)            ::Gfx::opengl_functions[26])
#define glDispatchCompute             ((PFNGLDISPATCHCOMPUTEPROC)         ::Gfx::opengl_functions[27])
#define glGetProgramInfoLog           ((PFNGLGETPROGRAMINFOLOGPROC)       ::Gfx::opengl_functions[28])
#define glGetProgramiv                ((PFNGLGETPROGRAMIVPROC)            ::Gfx::opengl_functions[29])
#define glGetShaderInfoLog            ((PFNGLGETSHADERINFOLOGPROC)        ::Gfx::opengl_functions[30])
#define glGetShaderiv                 ((PFNGLGETSHADERIVPROC)             ::Gfx::opengl_functions[31])
#define glIsShader                    ((PFNGLISSHADERPROC)                ::Gfx::opengl_functions[32])
#define glLinkProgram                 ((PFNGLLINKPROGRAMPROC)             ::Gfx::opengl_functions[33])
#define glShaderSource                ((PFNGLSHADERSOURCEPROC)            ::Gfx::opengl_functions[34])
#define glUseProgram                  ((PFNGLUSEPROGRAMPROC)              ::Gfx::opengl_functions[35])

// Uniforms (12)
#define glGetUniformLocation          ((PFNGLGETUNIFORMLOCATIONPROC)      ::Gfx::opengl_functions[36])
#define glUniform1fv                  ((PFNGLUNIFORM1FVPROC)              ::Gfx::opengl_functions[37])
#define glUniform2fv                  ((PFNGLUNIFORM2FVPROC)              ::Gfx::opengl_functions[38])
#define glUniform3fv                  ((PFNGLUNIFORM3FVPROC)              ::Gfx::opengl_functions[39])
#define glUniform4fv                  ((PFNGLUNIFORM4FVPROC)              ::Gfx::opengl_functions[40])
#define glUniform1iv                  ((PFNGLUNIFORM1IVPROC)              ::Gfx::opengl_functions[41])
#define glUniform2iv                  ((PFNGLUNIFORM2IVPROC)              ::Gfx::opengl_functions[42])
#define glUniform3iv                  ((PFNGLUNIFORM3IVPROC)              ::Gfx::opengl_functions[43])
#define glUniform4iv                  ((PFNGLUNIFORM4IVPROC)              ::Gfx::opengl_functions[44])
#define glUniformMatrix4fv            ((PFNGLUNIFORMMATRIX4FVPROC)        ::Gfx::opengl_functions[45])
#define glGetUniformBlockIndex        ((PFNGLGETUNIFORMBLOCKINDEXPROC)    ::Gfx::opengl_functions[46])
#define glUniformBlockBinding         ((PFNGLUNIFORMBLOCKBINDINGPROC)     ::Gfx::opengl_functions[47])

// Render buffers (12)
#define glBindFramebuffer             ((PFNGLBINDFRAMEBUFFERPROC)         ::Gfx::opengl_functions[48])
#define glBindRenderbuffer            ((PFNGLBINDRENDERBUFFERPROC)        ::Gfx::opengl_functions[49])
#define glCheckFramebufferStatus      ((PFNGLCHECKFRAMEBUFFERSTATUSPROC)  ::Gfx::opengl_functions[50])
#define glDeleteFramebuffers          ((PFNGLDELETEFRAMEBUFFERSPROC)      ::Gfx::opengl_functions[51])
#define glDeleteRenderbuffers         ((PFNGLDELETERENDERBUFFERSPROC)     ::Gfx::opengl_functions[52])
#define glDrawBuffers                 ((PFNGLDRAWBUFFERSPROC)             ::Gfx::opengl_functions[53])
#define glFramebufferRenderbuffer     ((PFNGLFRAMEBUFFERRENDERBUFFERPROC) ::Gfx::opengl_functions[54])
#define glFramebufferTexture1D        ((PFNGLFRAMEBUFFERTEXTURE1DPROC)    ::Gfx::opengl_functions[55])
#define glFramebufferTexture2D        ((PFNGLFRAMEBUFFERTEXTURE2DPROC)    ::Gfx::opengl_functions[56])
#define glFramebufferTexture3D        ((PFNGLFRAMEBUFFERTEXTURE3DPROC)    ::Gfx::opengl_functions[57])
#define glGenFramebuffers             ((PFNGLGENFRAMEBUFFERSPROC)         ::Gfx::opengl_functions[58])
#define glRenderbufferStorage         ((PFNGLRENDERBUFFERSTORAGEPROC)     ::Gfx::opengl_functions[59])

// Shader storage buffers (5)
#define glGetProgramResourceIndex     ((PFNGLGETPROGRAMRESOURCEINDEXPROC) ::Gfx::opengl_functions[60])
#define glShaderStorageBlockBinding   ((PFNGLSHADERSTORAGEBLOCKBINDINGPROC)::Gfx::opengl_functions[61])
#define glMemoryBarrier				  ((PFNGLMEMORYBARRIERPROC)           ::Gfx::opengl_functions[62])
#define glMapBufferRange              ((PFNGLMAPBUFFERRANGEPROC)          ::Gfx::opengl_functions[63])
#define glUnmapBuffer                 ((PFNGLUNMAPBUFFERPROC)             ::Gfx::opengl_functions[64])

// Others (5)
#define glLoadTransposeMatrixf        ((PFNGLLOADTRANSPOSEMATRIXFPROC)    ::Gfx::opengl_functions[65])
#define glBlendEquationSeparate       ((PFNGLBLENDEQUATIONSEPARATEPROC)   ::Gfx::opengl_functions[66])
#define glBlendFuncSeparate           ((PFNGLBLENDFUNCSEPARATEPROC)       ::Gfx::opengl_functions[67])
#define glStencilFuncSeparate         ((PFNGLSTENCILFUNCSEPARATEPROC)     ::Gfx::opengl_functions[68])
#define glStencilOpSeparate           ((PFNGLSTENCILOPSEPARATEPROC)       ::Gfx::opengl_functions[69])

#if DEBUG
#define glDebugMessageCallback        ((PFNGLDEBUGMESSAGECALLBACKPROC)    ::Gfx::opengl_functions[70])
#endif // DEBUG
