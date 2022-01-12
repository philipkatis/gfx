// TODO(philip): Remove junk from the Windows header file.
#include <Windows.h>
#include <math.h>
#include <GL/gl.h>

#include "gfx_base.h"
#include "gfx_math.h"
#include "gfx_os.h"
#include "gfx_win32.h"
#include "gfx_gl.h"

#include "gfx_base.cpp"
#include "gfx_math.cpp"

//
// NOTE(philip): Memory
//

function void *
OS_AllocateMemory(u64 Size)
{
    // TODO(philip): Change to using the Win32 API.
    void *Memory = calloc(1, Size);
    return Memory;
}

function void
OS_FreeMemory(void *Memory)
{
    if (Memory)
    {
        // TODO(philip): Change to using the Win32 API.
        free(Memory);
    }
}

//
// NOTE(philip): File IO
//

function b32
OS_ReadEntireFile(char *Path, buffer *Buffer)
{
    Assert(Buffer);

    b32 Result = false;

    HANDLE File = CreateFileA(Path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (File != INVALID_HANDLE_VALUE)
    {
        if (GetFileSizeEx(File, (LARGE_INTEGER *)&Buffer->Size))
        {
            Buffer->Data = OS_AllocateMemory(Buffer->Size);

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

function void
OS_FreeFileMemory(buffer *Buffer)
{
    Assert(Buffer);

    OS_FreeMemory(Buffer->Data);

    Buffer->Data = 0;
    Buffer->Size = 0;
}

function LRESULT
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
function void
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

#define Load(Type, Name) Name = (Type *)wglGetProcAddress(#Name)

function void
Win32LoadGLFunctions(void)
{
    // NOTE(philip): OpenGL 1.5

    Load(gl_gen_buffers,                    glGenBuffers);
    Load(gl_bind_buffer,                    glBindBuffer);
    Load(gl_buffer_data,                    glBufferData);
    Load(gl_delete_buffers,                 glDeleteBuffers);

    // NOTE(philip): OpenGL 2.0

    Load(gl_create_shader,                  glCreateShader);
    Load(gl_shader_source,                  glShaderSource);
    Load(gl_compile_shader,                 glCompileShader);
    Load(gl_get_shader_iv,                  glGetShaderiv);
    Load(gl_get_shader_info_log,            glGetShaderInfoLog);
    Load(gl_delete_shader,                  glDeleteShader);
    Load(gl_create_program,                 glCreateProgram);
    Load(gl_attach_shader,                  glAttachShader);
    Load(gl_link_program,                   glLinkProgram);
    Load(gl_validate_program,               glValidateProgram);
    Load(gl_get_program_iv,                 glGetProgramiv);
    Load(gl_get_program_info_log,           glGetProgramInfoLog);
    Load(gl_get_uniform_location,           glGetUniformLocation);
    Load(gl_use_program,                    glUseProgram);
    Load(gl_uniform_matrix_4fv,             glUniformMatrix4fv);
    Load(gl_delete_program,                 glDeleteProgram);
    Load(gl_enable_vertex_attrib_array,     glEnableVertexAttribArray);
    Load(gl_vertex_attrib_pointer,          glVertexAttribPointer);

    // NOTE(philip): OpenGL 3.0

    Load(gl_gen_vertex_arrays,              glGenVertexArrays);
    Load(gl_bind_vertex_array,              glBindVertexArray);
    Load(gl_delete_vertex_arrays,           glDeleteVertexArrays);
}

#undef Load

// TODO(philip): Move this.
struct index_set
{
    u64 Position;
    u64 TextureCoordinate;
    u64 Normal;
};

// TODO(philip): Move this.
struct vertex
{
    v3 Position;
    v2 TextureCoordinate;
    v3 Normal;
};

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

                                Assert(Length < 4096);

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

                                Assert(Length < 4096);

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

                            Assert(Length < 4096);

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

                            Assert(Length < 4096);

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

                        vertex *MeshVertices = 0;
                        u64 MeshVertexCount = 0;

                        u32 *MeshIndices = 0;
                        u64 MeshIndexCount = 0;

                        // TODO(philip): Move to separate file and function.
                        // TODO(philip): Documentation.
                        {
                            buffer FileData;
                            if (OS_ReadEntireFile("assets\\meshes\\woman1.obj", &FileData))
                            {
                                u64 PositionCount = 0;
                                u64 TextureCoordinateCount = 0;
                                u64 NormalCount = 0;
                                u64 TriangleCount = 0;

                                {
                                    char *Pointer = (char *)FileData.Data;
                                    char *End = (char *)FileData.Data + FileData.Size;

                                    while (Pointer != End)
                                    {
                                        switch (*Pointer)
                                        {
                                            case 'v':
                                            {
                                                switch (*(++Pointer))
                                                {
                                                    case 't':
                                                    {
                                                        ++TextureCoordinateCount;
                                                    } break;

                                                    case 'n':
                                                    {
                                                        ++NormalCount;
                                                    } break;

                                                    default:
                                                    {
                                                        ++PositionCount;
                                                    } break;
                                                }
                                            } break;

                                            case 'f':
                                            {
                                                ++TriangleCount;
                                            } break;
                                        }

                                        Pointer = SkipLine(Pointer);
                                        ++Pointer;
                                    }
                                }

                                v3 *Positions = (v3 *)OS_AllocateMemory(PositionCount * sizeof(v3));
                                v2 *TextureCoordinates = (v2 *)OS_AllocateMemory(TextureCoordinateCount * sizeof(v2));
                                v3 *Normals = (v3 *)OS_AllocateMemory(NormalCount * sizeof(v3));

                                // TODO(philip): Change this into a hash table.
                                index_set *UniqueIndexSets = (index_set *)OS_AllocateMemory(TriangleCount * 3 * sizeof(index_set));
                                u64 UniqueIndexSetCount = 0;

                                MeshIndices = (u32 *)OS_AllocateMemory(TriangleCount * 3 * sizeof(u32));

                                u64 PositionIndex = 0;
                                u64 TextureCoordinateIndex = 0;
                                u64 NormalIndex = 0;

                                char *Pointer = (char *)FileData.Data;
                                char *End = (char *)FileData.Data + FileData.Size;

                                while (Pointer != End)
                                {
                                    switch (*Pointer)
                                    {
                                        case 'v':
                                        {
                                            switch (*(++Pointer))
                                            {
                                                case 't':
                                                {
                                                    // TODO(philip): Pull this out to a function.
                                                    // TODO(philip): Do this in a loop.

                                                    Pointer = SkipWhitespace(++Pointer);
                                                    char *XString = Pointer;

                                                    Pointer = SkipUntilWhitespace(Pointer);
                                                    *Pointer = 0;

                                                    Pointer = SkipWhitespace(++Pointer);
                                                    char *YString = Pointer;

                                                    Pointer = SkipUntilWhitespace(Pointer);
                                                    *Pointer = 0;

                                                    v2 *TextureCoordinate = TextureCoordinates +
                                                        TextureCoordinateIndex++;

                                                    TextureCoordinate->X = atof(XString);
                                                    TextureCoordinate->Y = atof(YString);
                                                } break;

                                                case 'n':
                                                {
                                                    // TODO(philip): Pull this out to a function.
                                                    // TODO(philip): Do this in a loop.

                                                    Pointer = SkipWhitespace(++Pointer);
                                                    char *XString = Pointer;

                                                    Pointer = SkipUntilWhitespace(Pointer);
                                                    *Pointer = 0;

                                                    Pointer = SkipWhitespace(++Pointer);
                                                    char *YString = Pointer;

                                                    Pointer = SkipUntilWhitespace(Pointer);
                                                    *Pointer = 0;

                                                    Pointer = SkipWhitespace(++Pointer);
                                                    char *ZString = Pointer;

                                                    Pointer = SkipUntilWhitespace(Pointer);
                                                    *Pointer = 0;

                                                    v3 *Normal = Normals + NormalIndex++;

                                                    Normal->X = atof(XString);
                                                    Normal->Y = atof(YString);
                                                    Normal->Z = atof(ZString);
                                                } break;

                                                default:
                                                {
                                                    // TODO(philip): Pull this out to a function.
                                                    // TODO(philip): Do this in a loop.

                                                    Pointer = SkipWhitespace(Pointer);
                                                    char *XString = Pointer;

                                                    Pointer = SkipUntilWhitespace(Pointer);
                                                    *Pointer = 0;

                                                    Pointer = SkipWhitespace(++Pointer);
                                                    char *YString = Pointer;

                                                    Pointer = SkipUntilWhitespace(Pointer);
                                                    *Pointer = 0;

                                                    Pointer = SkipWhitespace(++Pointer);
                                                    char *ZString = Pointer;

                                                    Pointer = SkipUntilWhitespace(Pointer);
                                                    *Pointer = 0;

                                                    v3 *Position = Positions + PositionIndex++;

                                                    Position->X = atof(XString);
                                                    Position->Y = atof(YString);
                                                    Position->Z = atof(ZString);
                                                } break;
                                            }
                                        } break;

                                        case 'f':
                                        {
                                            for (u32 SetIndex = 0;
                                                 SetIndex < 3;
                                                 ++SetIndex)
                                            {
                                                // TODO(philip): Maybe turn this into a loop.

                                                Pointer = SkipWhitespace(++Pointer);
                                                char *PositionString = Pointer;

                                                Pointer = SkipUntil(Pointer, '/');
                                                *Pointer = 0;

                                                char *TextureCoordinateString = ++Pointer;

                                                Pointer = SkipUntil(Pointer, '/');
                                                *Pointer = 0;

                                                char *NormalString = ++Pointer;

                                                Pointer = SkipUntilWhitespace(Pointer);
                                                *Pointer = 0;

                                                u64 Position = atoi(PositionString);
                                                u64 TextureCoordinate = atoi(TextureCoordinateString);
                                                u64 Normal = atoi(NormalString);

                                                b32 Found = false;
                                                u64 SetID = 0;

                                                // TODO(philip): Replace this with a search into the hash table.
                                                for (u64 Index = 0;
                                                     Index < UniqueIndexSetCount;
                                                     ++Index)
                                                {
                                                    index_set *Set = UniqueIndexSets + Index;
                                                    if (Set->Position == Position &&
                                                        Set->TextureCoordinate == TextureCoordinate &&
                                                        Set->Normal == Normal)
                                                    {
                                                        Found = true;
                                                        SetID = Index;

                                                        break;
                                                    }
                                                }

                                                if (!Found)
                                                {
                                                    SetID = UniqueIndexSetCount;

                                                    index_set *Set = UniqueIndexSets + UniqueIndexSetCount++;
                                                    Set->Position = Position;
                                                    Set->TextureCoordinate = TextureCoordinate;
                                                    Set->Normal = Normal;
                                                }

                                                MeshIndices[MeshIndexCount++] = SetID;
                                            }
                                        } break;

                                        default:
                                        {
                                            Pointer = SkipLine(Pointer);
                                        } break;
                                    }

                                    ++Pointer;
                                }

                                MeshVertexCount = UniqueIndexSetCount;
                                MeshVertices = (vertex *)OS_AllocateMemory(MeshVertexCount * sizeof(vertex));

                                for (u64 Index = 0;
                                     Index < UniqueIndexSetCount;
                                     ++Index)
                                {
                                    index_set *Set = UniqueIndexSets + Index;
                                    vertex *Vertex = MeshVertices + Index;

                                    Vertex->Position = Positions[Set->Position - 1];
                                    Vertex->TextureCoordinate = TextureCoordinates[Set->TextureCoordinate - 1];
                                    Vertex->Normal = Normals[Set->Normal - 1];
                                }

                                OS_FreeMemory(UniqueIndexSets);
                                OS_FreeMemory(Normals);
                                OS_FreeMemory(TextureCoordinates);
                                OS_FreeMemory(Positions);

                                OS_FreeFileMemory(&FileData);
                            }
                        }

                        GLuint VertexBuffer;
                        glGenBuffers(1, &VertexBuffer);
                        glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
                        glBufferData(GL_ARRAY_BUFFER, MeshVertexCount * sizeof(vertex), MeshVertices,
                                     GL_STATIC_DRAW);

                        glEnableVertexAttribArray(0);
                        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), 0);

                        glEnableVertexAttribArray(1);
                        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)(sizeof(v3)));

                        glEnableVertexAttribArray(2);
                        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)(sizeof(v3) + sizeof(v2)));

                        GLuint IndexBuffer;
                        glGenBuffers(1, &IndexBuffer);
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
                        glBufferData(GL_ELEMENT_ARRAY_BUFFER, MeshIndexCount * sizeof(u32), MeshIndices,
                                     GL_STATIC_DRAW);

                        OS_FreeMemory(MeshVertices);
                        OS_FreeMemory(MeshIndices);

                        GLint ViewProjectionUniformLocation = glGetUniformLocation(Program, "ViewProjection");
                        GLint TransformUniformLocation = glGetUniformLocation(Program, "Transform");

                        glUseProgram(Program);

                        // TODO(philip): Use the actual render area size.
                        m4 Projection = Perspective((16.0f / 9.0f), ToRadians(45.0f), 0.01f, 10000.0f);

                        ShowWindow(Window, SW_SHOW);

                        f32 CameraMovementSpeed = 0.025f;
                        v3 CameraPosition = V3(0.0f, 0.0f, 20.0f);

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

                            m4 Transform = Scale(V3(0.05f, 0.05f, 0.05f)) *
                                ToM4(AxisAngleRotate(V3(0.0f, 0.0f, 1.0f), ToRadians(0.0f))) *
                                Translate(V3(0.0f, 0.0f, 0.0f));

                            // TODO(philip): Overload the negative operator for v3.
                            m4 View = Translate(V3(-CameraPosition.X, -CameraPosition.Y, -CameraPosition.Z));

                            // TODO(philip): Why does this work?
                            m4 ViewProjection = View * Projection;

                            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                            glUniformMatrix4fv(ViewProjectionUniformLocation, 1, GL_FALSE,
                                               (GLfloat *)&ViewProjection);
                            glUniformMatrix4fv(TransformUniformLocation, 1, GL_FALSE, (GLfloat *)&Transform);

                            glDrawElements(GL_TRIANGLES, MeshIndexCount, GL_UNSIGNED_INT, 0);

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
