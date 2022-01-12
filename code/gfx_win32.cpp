// TODO(philip): Remove junk from the Windows header file.
#include <Windows.h>
#include <math.h>
#include <GL/gl.h>

#include "gfx_base.h"
#include "gfx_math.h"
#include "gfx_os.h"
#include "gfx_win32.h"
#include "gfx_gl.h"

#include "gfx_math.cpp"

static b32
OS_ReadEntireFile(char *Path, buffer *Buffer)
{
    // TODO(philip): Assert that the buffer is valid.

    b32 Result = false;

    HANDLE File = CreateFileA(Path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (File != INVALID_HANDLE_VALUE)
    {
        if (GetFileSizeEx(File, (LARGE_INTEGER *)&Buffer->Size))
        {
            // TODO(philip): Change to using the Win32 API.
            Buffer->Data = calloc(1, Buffer->Size);

            DWORD BytesRead;
            if (ReadFile(File, Buffer->Data, Buffer->Size, &BytesRead, 0) && (BytesRead == Buffer->Size))
            {
                Result = true;
            }
            else
            {
                OS_FreeFileMemory(Buffer);
            }
        }

        CloseHandle(File);
    }

    return Result;
}

static void
OS_FreeFileMemory(buffer *Buffer)
{
    // TODO(philip): Assert that the buffer is valid.

    if (Buffer->Data)
    {
        // TODO(philip): Change to using the Win32 API.
        free(Buffer->Data);
    }

    Buffer->Data = 0;
    Buffer->Size = 0;
}

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

    WNDCLASSA WindowClass = { };
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

            PIXELFORMATDESCRIPTOR DesiredPixelFormat = { };
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
    // TODO(philip): Maybe make a macro to load these more easily.

    // NOTE(philip): OpenGL 1.5

    glGenBuffers = (gl_gen_buffers *)wglGetProcAddress("glGenBuffers");
    glBindBuffer = (gl_bind_buffer *)wglGetProcAddress("glBindBuffer");
    glBufferData = (gl_buffer_data *)wglGetProcAddress("glBufferData");
    glDeleteBuffers = (gl_delete_buffers *)wglGetProcAddress("glDeleteBuffers");

    // NOTE(philip): OpenGL 2.0

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
    glGetUniformLocation = (gl_get_uniform_location *)wglGetProcAddress("glGetUniformLocation");
    glUseProgram = (gl_use_program *)wglGetProcAddress("glUseProgram");
    glUniformMatrix4fv = (gl_uniform_matrix_4fv *)wglGetProcAddress("glUniformMatrix4fv");
    glDeleteProgram = (gl_delete_program *)wglGetProcAddress("glDeleteProgram");
    glEnableVertexAttribArray = (gl_enable_vertex_attrib_array *)wglGetProcAddress("glEnableVertexAttribArray");
    glVertexAttribPointer = (gl_vertex_attrib_pointer *)wglGetProcAddress("glVertexAttribPointer");

    // NOTE(philip): OpenGL 3.0

    glGenVertexArrays = (gl_gen_vertex_arrays *)wglGetProcAddress("glGenVertexArrays");
    glBindVertexArray = (gl_bind_vertex_array *)wglGetProcAddress("glBindVertexArray");
    glDeleteVertexArrays = (gl_delete_vertex_arrays *)wglGetProcAddress("glDeleteVertexArrays");
}

s32
WinMain(HINSTANCE Instance, HINSTANCE PreviousInstance, LPSTR Arguments, s32 ShowCMD)
{
    Win32LoadWLGExtensions(Instance);

    LPCSTR WindowClassName = "gfx_win32_window_class";

    WNDCLASSA WindowClass = { };
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
                        // TODO(philip): Pull shader loading into it's own function.

                        GLint Status;

                        GLuint VertexShaderModule = glCreateShader(GL_VERTEX_SHADER);

                        buffer VertexShaderModuleSource;
                        if (OS_ReadEntireFile("assets\\shaders\\gfx_simple_vs.glsl", &VertexShaderModuleSource))
                        {
                            glShaderSource(VertexShaderModule, 1, (GLchar **)&VertexShaderModuleSource.Data,
                                           (GLint *)&VertexShaderModuleSource.Size);

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

                            OS_FreeFileMemory(&VertexShaderModuleSource);
                        }

                        GLuint PixelShaderModule = glCreateShader(GL_FRAGMENT_SHADER);

                        buffer PixelShaderModuleSource;
                        if (OS_ReadEntireFile("assets\\shaders\\gfx_simple_ps.glsl", &PixelShaderModuleSource))
                        {
                            glShaderSource(PixelShaderModule, 1, (GLchar **)&PixelShaderModuleSource.Data,
                                           (GLint *)&PixelShaderModuleSource.Size);

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

                            OS_FreeFileMemory(&PixelShaderModuleSource);
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
                            0.5f, 0.5f, 0.0f,
                            -0.5f, 0.5f, 0.0f
                        };

                        GLuint VertexBuffer;
                        glGenBuffers(1, &VertexBuffer);
                        glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
                        glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), Vertices, GL_STATIC_DRAW);

                        glEnableVertexAttribArray(0);
                        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

                        GLuint Indices[] =
                        {
                            0, 1, 2,
                            2, 3, 0
                        };

                        GLuint IndexBuffer;
                        glGenBuffers(1, &IndexBuffer);
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
                        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), Indices, GL_STATIC_DRAW);

                        GLint ViewProjectionUniformLocation = glGetUniformLocation(Program, "ViewProjection");
                        GLint TransformUniformLocation = glGetUniformLocation(Program, "Transform");

                        glUseProgram(Program);

                        // TODO(philip): Use the actual render area size.
                        m4 Projection = Perspective((16.0f / 9.0f), ToRadians(45.0f), 0.01f, 10000.0f);

                        ShowWindow(Window, SW_SHOW);

                        f32 CameraMovementSpeed = 0.025f;
                        v3 CameraPosition = V3(0.0f, 0.0f, 2.0f);

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

                            // TODO(philip): Should the forward and right camera movement vectors be the same
                            // as the rotation ones, or the world ones.

                            if (GetKeyState(0x57) & 0x8000)
                            {
                                // NOTE(philip): W key is pressed.

                                // TODO(philip): Integrate time.
                                CameraPosition.Z -= CameraMovementSpeed;
                            }

                            if (GetKeyState(0x53) & 0x8000)
                            {
                                // NOTE(philip): S key is pressed.

                                // TODO(philip): Integrate time.
                                CameraPosition.Z += CameraMovementSpeed;
                            }

                            if (GetKeyState(0x41) & 0x8000)
                            {
                                // NOTE(philip): A key is pressed.

                                // TODO(philip): Integrate time.
                                CameraPosition.X -= CameraMovementSpeed;
                            }

                            if (GetKeyState(0x44) & 0x8000)
                            {
                                // NOTE(philip): D key is pressed.

                                // TODO(philip): Integrate time.
                                CameraPosition.X += CameraMovementSpeed;
                            }

                            m4 Transform = Scale(V3(0.5f, 0.5f, 0.5f)) *
                                ToM4(AxisAngleRotate(V3(0.0f, 0.0f, 1.0f), ToRadians(45.0f))) *
                                Translate(V3(0.0f, 0.0f, 0.0f));

                            // TODO(philip): Overload the negative operator for v3.
                            m4 View = Translate(V3(-CameraPosition.X, -CameraPosition.Y, -CameraPosition.Z));

                            // TODO(philip): Why does this work?
                            m4 ViewProjection = View * Projection;

                            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                            glUniformMatrix4fv(ViewProjectionUniformLocation, 1, GL_FALSE,
                                               (GLfloat *)&ViewProjection);
                            glUniformMatrix4fv(TransformUniformLocation, 1, GL_FALSE, (GLfloat *)&Transform);

                            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

                            SwapBuffers(DeviceContext);
                        }

                        glDeleteBuffers(1, &IndexBuffer);
                        glDeleteBuffers(1, &VertexBuffer);
                        glDeleteVertexArrays(1, &VertexArray);
                        glDeleteProgram(Program);
                        glDeleteShader(PixelShaderModule);
                        glDeleteShader(VertexShaderModule);
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
