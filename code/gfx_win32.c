// TODO(philip): Remove junk from the Windows header file.
#include <Windows.h>
#include <GL/gl.h>

#include "gfx_base.h"

// TODO(philip): Move these to a header file.
// TODO(philip): Documentation.

#define WGL_DRAW_TO_WINDOW_ARB            0x2001
#define WGL_SUPPORT_OPENGL_ARB            0x2010
#define WGL_DOUBLE_BUFFER_ARB             0x2011
#define WGL_PIXEL_TYPE_ARB                0x2013
#define WGL_COLOR_BITS_ARB                0x2014
#define WGL_DEPTH_BITS_ARB                0x2022
#define WGL_STENCIL_BITS_ARB              0x2023

#define WGL_TYPE_RGBA_ARB                 0x202B

#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_FLAGS_ARB             0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126

#define WGL_CONTEXT_DEBUG_BIT_ARB         0x00000001

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001

typedef BOOL wgl_choose_pixel_format_arb(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList,
                                         UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
typedef HGLRC wgl_create_context_attribs_arb(HDC hDC, HGLRC hShareContext, const int *attribList);
typedef BOOL wgl_swap_interval_ext(int interval);

static wgl_choose_pixel_format_arb *wglChoosePixelFormatARB = 0;
static wgl_create_context_attribs_arb *wglCreateContextAttribsARB = 0;
static wgl_swap_interval_ext *wglSwapIntervalEXT = 0;

// TODO(philip): Move these to a header file.
// TODO(philip): Documentation.

typedef char GLchar;
typedef signed long long int GLsizeiptr;

#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893

#define GL_STATIC_DRAW                    0x88E4

#define GL_VERTEX_SHADER                  0x8B31
#define GL_FRAGMENT_SHADER                0x8B30

#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_VALIDATE_STATUS                0x8B83
#define GL_INFO_LOG_LENGTH                0x8B84

typedef void gl_gen_buffers(GLsizei n, GLuint *buffers);
typedef void gl_bind_buffer(GLenum target, GLuint buffer);
typedef void gl_buffer_data(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void gl_enable_vertex_attrib_array(GLuint index);
typedef void gl_vertex_attrib_pointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride,
                                      const void *pointer);
typedef void gl_delete_buffers(GLsizei n, const GLuint *buffers);
typedef GLuint gl_create_shader(GLenum type);
typedef void gl_shader_source(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
typedef void gl_compile_shader(GLuint shader);
typedef void gl_get_shader_iv(GLuint shader, GLenum pname, GLint *params);
typedef void gl_get_shader_info_log(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void gl_delete_shader(GLuint shader);
typedef GLuint gl_create_program(void);
typedef void gl_attach_shader(GLuint program, GLuint shader);
typedef void gl_link_program(GLuint program);
typedef void gl_validate_program(GLuint program);
typedef void gl_get_program_iv(GLuint program, GLenum pname, GLint *params);
typedef void gl_get_program_info_log(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void gl_use_program(GLuint program);
typedef void gl_delete_program(GLuint program);
typedef void gl_gen_vertex_arrays(GLsizei n, GLuint *arrays);
typedef void gl_bind_vertex_array(GLuint array);
typedef void gl_delete_vertex_arrays(GLsizei n, const GLuint *arrays);

static gl_gen_buffers *glGenBuffers = 0;
static gl_bind_buffer *glBindBuffer = 0;
static gl_buffer_data *glBufferData = 0;
static gl_enable_vertex_attrib_array *glEnableVertexAttribArray = 0;
static gl_vertex_attrib_pointer *glVertexAttribPointer = 0;
static gl_delete_buffers *glDeleteBuffers = 0;
static gl_create_shader *glCreateShader = 0;
static gl_shader_source *glShaderSource = 0;
static gl_compile_shader *glCompileShader = 0;
static gl_get_shader_iv *glGetShaderiv = 0;
static gl_get_shader_info_log *glGetShaderInfoLog = 0;
static gl_delete_shader *glDeleteShader = 0;
static gl_create_program *glCreateProgram = 0;
static gl_attach_shader *glAttachShader = 0;
static gl_link_program *glLinkProgram = 0;
static gl_validate_program *glValidateProgram = 0;
static gl_get_program_iv *glGetProgramiv = 0;
static gl_get_program_info_log *glGetProgramInfoLog = 0;
static gl_use_program *glUseProgram = 0;
static gl_delete_program *glDeleteProgram = 0;
static gl_gen_vertex_arrays *glGenVertexArrays = 0;
static gl_bind_vertex_array *glBindVertexArray = 0;
static gl_delete_vertex_arrays *glDeleteVertexArrays = 0;

static LRESULT
Win32WindowProcedure(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;

    switch (Message)
    {
        // TODO(philip): Close the application when we press escape.

        case WM_CLOSE:
        {
            DestroyWindow(Window);
        } break;

        case WM_DESTROY:
        {
            PostQuitMessage(0);
        } break;

        default:
        {
            Result = DefWindowProcA(Window, Message, WParam, LParam);
        } break;
    }

    return Result;
}

// TODO(philip): Documentation.
// TODO(philip): Maybe return success or failure.
static void
Win32LoadWLGExtensions(HINSTANCE Instance)
{
    LPCSTR WindowClassName = "gfx_dummy_win32_window_class";

    WNDCLASSA WindowClass = {0};
    WindowClass.style = CS_OWNDC;
    WindowClass.lpfnWndProc = DefWindowProcA;
    WindowClass.hInstance = Instance;
    WindowClass.lpszClassName = WindowClassName;

    if (RegisterClassA(&WindowClass))
    {
        HWND Window = CreateWindowA(WindowClassName, 0, 0, 0, 0, 0, 0, 0, 0, Instance, 0);
        if (Window)
        {
            HDC DeviceContext = GetDC(Window);

            PIXELFORMATDESCRIPTOR DesiredPixelFormat = {0};
            DesiredPixelFormat.nSize = sizeof(PIXELFORMATDESCRIPTOR);
            DesiredPixelFormat.nVersion = 1;
            DesiredPixelFormat.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
            DesiredPixelFormat.iPixelType = PFD_TYPE_RGBA;
            DesiredPixelFormat.cColorBits = 32;
            DesiredPixelFormat.cDepthBits = 24;
            DesiredPixelFormat.cStencilBits = 8;

            s32 SelectedPixelFormatIndex = ChoosePixelFormat(DeviceContext, &DesiredPixelFormat);
            if (SelectedPixelFormatIndex)
            {
                PIXELFORMATDESCRIPTOR SelectedPixelFormat;
                DescribePixelFormat(DeviceContext, SelectedPixelFormatIndex, sizeof(PIXELFORMATDESCRIPTOR),
                                    &SelectedPixelFormat);

                if (SetPixelFormat(DeviceContext, SelectedPixelFormatIndex, &SelectedPixelFormat))
                {
                    HGLRC OpenGLContext = wglCreateContext(DeviceContext);
                    if (OpenGLContext)
                    {
                        wglMakeCurrent(DeviceContext, OpenGLContext);

                        wglChoosePixelFormatARB = (wgl_choose_pixel_format_arb *)wglGetProcAddress("wglChoosePixelFormatARB");
                        wglCreateContextAttribsARB = (wgl_create_context_attribs_arb *)wglGetProcAddress("wglCreateContextAttribsARB");
                        wglSwapIntervalEXT = (wgl_swap_interval_ext *)wglGetProcAddress("wglSwapIntervalEXT");

                        // TODO(philip): Investigate what we should do if loading one of these fails.
                    }
                    else
                    {
                        // TODO(philip): Error message.
                    }

                    wglDeleteContext(OpenGLContext);
                }
                else
                {
                    // TODO(philip): Error message.
                }
            }
            else
            {
                // TODO(philip): Error message.
            }

            ReleaseDC(Window, DeviceContext);
        }
        else
        {
            // TODO(philip): Error message.
        }

        DestroyWindow(Window);
    }
    else
    {
        // TODO(philip): Error message.
    }

    UnregisterClassA(WindowClassName, Instance);
}

static void
Win32LoadGLFunctions(void)
{
    // TODO(philip): Investigate what we should do if loading one of these fails.
    glGenBuffers = (gl_gen_buffers *)wglGetProcAddress("glGenBuffers");
    glBindBuffer = (gl_bind_buffer *)wglGetProcAddress("glBindBuffer");
    glBufferData = (gl_buffer_data *)wglGetProcAddress("glBufferData");
    glEnableVertexAttribArray = (gl_enable_vertex_attrib_array *)wglGetProcAddress("glEnableVertexAttribArray");
    glVertexAttribPointer = (gl_vertex_attrib_pointer *)wglGetProcAddress("glVertexAttribPointer");
    glDeleteBuffers = (gl_delete_buffers *)wglGetProcAddress("glDeleteBuffers");
    glCreateShader = (gl_create_shader *)wglGetProcAddress("glCreateShader");
    glShaderSource = (gl_shader_source *)wglGetProcAddress("glShaderSource");
    glCompileShader = (gl_compile_shader *)wglGetProcAddress("glCompileShader");
    glGetShaderiv = (gl_get_shader_iv *)wglGetProcAddress("glGetShaderiv");
    glGetShaderInfoLog = (gl_get_shader_info_log *)wglGetProcAddress("glGetShaderInfoLog");
    glDeleteShader = (gl_delete_shader *)wglGetProcAddress("glDeleteShader");
    glCreateProgram = (gl_create_program *)wglGetProcAddress("glCreateProgram");
    glAttachShader = (gl_attach_shader *)wglGetProcAddress("glAttachShader");
    glLinkProgram = (gl_link_program *)wglGetProcAddress("glLinkProgram");
    glValidateProgram = (gl_validate_program *)wglGetProcAddress("glValidateProgram");
    glGetProgramiv = (gl_get_program_iv *)wglGetProcAddress("glGetProgramiv");
    glGetProgramInfoLog = (gl_get_program_info_log *)wglGetProcAddress("glGetShaderInfoLog");
    glUseProgram = (gl_use_program *)wglGetProcAddress("glUseProgram");
    glDeleteProgram = (gl_delete_program *)wglGetProcAddress("glDeleteProgram");
    glGenVertexArrays = (gl_gen_vertex_arrays *)wglGetProcAddress("glGenVertexArrays");
    glBindVertexArray = (gl_bind_vertex_array *)wglGetProcAddress("glBindVertexArray");
    glDeleteVertexArrays = (gl_delete_vertex_arrays *)wglGetProcAddress("glDeleteVertexArrays");
}

s32
WinMain(HINSTANCE Instance, HINSTANCE PreviousInstance, LPSTR Arguments, s32 ShowCMD)
{
    Win32LoadWLGExtensions(Instance);

    LPCSTR WindowClassName = "gfx_win32_window_class";

    WNDCLASSA WindowClass = {0};
    WindowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
    WindowClass.lpfnWndProc = Win32WindowProcedure;
    WindowClass.hInstance = Instance;
    WindowClass.lpszClassName = WindowClassName;

    if (RegisterClassA(&WindowClass))
    {
        // TODO(philip): Create the window based on the client area dimensions.
        // TODO(philip): Center the window on the screen.
        HWND Window = CreateWindowA(WindowClassName, "gfx", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                                    CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, Instance, 0);

        if (Window)
        {
            HDC DeviceContext = GetDC(Window);

            s32 PixelFormatAttributes[] =
            {
                WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
                WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
                WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
                WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
                WGL_COLOR_BITS_ARB, 32,
                WGL_DEPTH_BITS_ARB, 24,
                WGL_STENCIL_BITS_ARB, 8,
                0
            };

            s32 PixelFormatIndex;
            UINT PixelFormatCount;
            wglChoosePixelFormatARB(DeviceContext, PixelFormatAttributes, 0, 1, &PixelFormatIndex,
                                    &PixelFormatCount);

            if (PixelFormatCount == 1)
            {
                PIXELFORMATDESCRIPTOR PixelFormat;
                DescribePixelFormat(DeviceContext, PixelFormatIndex, sizeof(PIXELFORMATDESCRIPTOR), &PixelFormat);

                if (SetPixelFormat(DeviceContext, PixelFormatIndex, &PixelFormat))
                {
                    // TODO(philip): What is the max version we are allowed to support?
                    // TODO(philip): Only set the debug flag on debug builds.
                    s32 OpenGLContextAttributes[] =
                    {
                        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
                        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
                        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
                        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
                        0
                    };

                    HGLRC OpenGLContext = wglCreateContextAttribsARB(DeviceContext, 0, OpenGLContextAttributes);
                    if (OpenGLContext)
                    {
                        wglMakeCurrent(DeviceContext, OpenGLContext);

                        Win32LoadGLFunctions();

                        // TODO(philip): Pull shader module cration into it's own function.
                        // TODO(philip): Load shader module files from disc.
                        // TODO(philip): Pull shader loading into it's own function.

                        GLint Status;

                        GLchar *VertexShaderModuleSource = "#version 330 core\n\nlayout (location = 0) in vec3 Position;\n\nvoid main()\n{\n    gl_Position = vec4(Position, 1.0);\n}\n";

                        GLuint VertexShaderModule = glCreateShader(GL_VERTEX_SHADER);
                        glShaderSource(VertexShaderModule, 1, &VertexShaderModuleSource, 0);

                        glCompileShader(VertexShaderModule);
                        glGetShaderiv(VertexShaderModule, GL_COMPILE_STATUS, &Status);

                        if (Status == GL_FALSE)
                        {
                            // TODO(philip): Maybe remove this.
                            GLint Length;
                            glGetShaderiv(VertexShaderModule, GL_INFO_LOG_LENGTH, &Length);

                            // TODO(philip): Assert that the length is within bounds.

                            GLchar InfoLog[4096];
                            glGetShaderInfoLog(VertexShaderModule, 4096, &Length, InfoLog);

                            // TODO(philip): Replace this with something like a message box?
                            OutputDebugStringA("Vertex shader module compilation failed!\n");
                            OutputDebugStringA(InfoLog);
                            OutputDebugStringA("\n");

                            glDeleteShader(VertexShaderModule);
                            VertexShaderModule = 0;
                        }

                        GLchar *PixelShaderModuleSource = "#version 330 core\n\nlayout (location = 0) out vec4 Color;\n\nvoid main()\n{\n    Color = vec4(1.0, 1.0, 1.0, 1.0);\n}\n";

                        GLuint PixelShaderModule = glCreateShader(GL_FRAGMENT_SHADER);
                        glShaderSource(PixelShaderModule, 1, &PixelShaderModuleSource, 0);

                        glCompileShader(PixelShaderModule);
                        glGetShaderiv(PixelShaderModule, GL_COMPILE_STATUS, &Status);

                        if (Status == GL_FALSE)
                        {
                            // TODO(philip): Maybe remove this.
                            GLint Length;
                            glGetShaderiv(PixelShaderModule, GL_INFO_LOG_LENGTH, &Length);

                            // TODO(philip): Assert that the length is within bounds.

                            GLchar InfoLog[4096];
                            glGetShaderInfoLog(PixelShaderModule, 4096, &Length, InfoLog);

                            // TODO(philip): Replace this with something like a message box?
                            OutputDebugStringA("Pixel shader module compilation failed!\n");
                            OutputDebugStringA(InfoLog);
                            OutputDebugStringA("\n");

                            glDeleteShader(PixelShaderModule);
                            PixelShaderModule = 0;
                        }

                        GLuint Program = glCreateProgram();

                        glAttachShader(Program, VertexShaderModule);
                        glAttachShader(Program, PixelShaderModule);

                        glLinkProgram(Program);
                        glGetProgramiv(Program, GL_LINK_STATUS, &Status);

                        if (Status == GL_FALSE)
                        {
                            // TODO(philip): Maybe remove this.
                            GLint Length;
                            glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &Length);

                            // TODO(philip): Assert that the length is within bounds.

                            GLchar InfoLog[4096];
                            glGetProgramInfoLog(Program, 4096, &Length, InfoLog);

                            // TODO(philip): Replace this with something like a message box?
                            OutputDebugStringA("Shader program linking failed!\n");
                            OutputDebugStringA(InfoLog);
                            OutputDebugStringA("\n");

                            glDeleteProgram(Program);
                            Program = 0;
                        }

                        glValidateProgram(Program);
                        glGetProgramiv(Program, GL_VALIDATE_STATUS, &Status);

                        if (Status == GL_FALSE)
                        {
                            // TODO(philip): Maybe remove this.
                            GLint Length;
                            glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &Length);

                            // TODO(philip): Assert that the length is within bounds.

                            GLchar InfoLog[4096];
                            glGetProgramInfoLog(Program, 4096, &Length, InfoLog);

                            // TODO(philip): Replace this with something like a message box?
                            OutputDebugStringA("Shader program validation failed!\n");
                            OutputDebugStringA(InfoLog);
                            OutputDebugStringA("\n");

                            glDeleteProgram(Program);
                            Program = 0;
                        }

                        // TODO(philip): Pull the mesh upload into a separate function.

                        GLuint VertexArray;
                        glGenVertexArrays(1, &VertexArray);
                        glBindVertexArray(VertexArray);

                        GLfloat Vertices[] =
                        {
                            -0.5f, -0.5f, 0.0f,
                            0.5f, -0.5f, 0.0f,
                            0.0f, 0.5f, 0.0f
                        };

                        GLuint VertexBuffer;
                        glGenBuffers(1, &VertexBuffer);
                        glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
                        glBufferData(GL_ARRAY_BUFFER, 3 * 3 * sizeof(GLfloat), Vertices, GL_STATIC_DRAW);

                        glEnableVertexAttribArray(0);
                        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

                        GLuint Indices[] =
                        {
                            0, 1, 2
                        };

                        GLuint IndexBuffer;
                        glGenBuffers(1, &IndexBuffer);
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
                        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(GLuint), Indices, GL_STATIC_DRAW);

                        glUseProgram(Program);
                        glClearColor(0.2f, 0.5f, 0.2f, 1.0f);

                        ShowWindow(Window, SW_SHOW);

                        for (;;)
                        {
                            b32 IsExitRequested = false;

                            MSG Message;
                            while (PeekMessageA(&Message, 0, 0, 0, PM_REMOVE))
                            {
                                if (Message.message == WM_QUIT)
                                {
                                    IsExitRequested = true;
                                    break;
                                }

                                TranslateMessage(&Message);
                                DispatchMessageA(&Message);
                            }

                            if (IsExitRequested)
                            {
                                break;
                            }

                            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

                            SwapBuffers(DeviceContext);
                        }

                        glDeleteProgram(Program);
                        glDeleteShader(PixelShaderModule);
                        glDeleteShader(VertexShaderModule);
                        glDeleteVertexArrays(1, &VertexArray);
                    }
                    else
                    {
                        // TODO(philip): Error message.
                    }
                }
                else
                {
                    // TODO(philip): Error message.
                }
            }
            else
            {
                // TODO(philip): Error message.
            }
        }
        else
        {
            // TODO(philip): Error message.
        }
    }
    else
    {
        // TODO(philip): Error message.
    }

    return 0;
};
