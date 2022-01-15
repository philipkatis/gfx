// TODO(philip): Remove unwanted includes.
// TODO(philip): Remove junk from the Windows header file.
#include <Windows.h>
#include <stdio.h>
#include <math.h>
#include <GL/gl.h>

#include "gfx_base.h"
#include "gfx_math.h"
#include "gfx_asset.h"
#include "gfx_os.h"
#include "gfx_win32.h"
#include "gfx_gl.h"

#include "gfx_base.cpp"
#include "gfx_math.cpp"
#include "gfx_asset.cpp"
#include "gfx_gl.cpp"

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

// TODO(philip): Is this a good plan?
struct win32_state
{
    b32 IsCursorHidden;
    iv2 CursorPositionToRestore;

    b32 UseRawInput;
    iv2 RawCursorPosition;
};

// TODO(philip): Should this be global?
global win32_state Win32State = { };

function void
Win32InitializeState(void)
{
    // TODO(philip): If raw input is enabled by default, actually enable it.
    Win32State.UseRawInput = false;
    Win32State.IsCursorHidden = false;
}

function iv2
Win32GetWindowSize(HWND Window)
{
    RECT Dimensions;
    GetClientRect(Window, &Dimensions);

    iv2 Size = IV2((Dimensions.right - Dimensions.left), (Dimensions.bottom - Dimensions.top));
    return Size;
}

function iv2
Win32GetCursorPosition(HWND Window)
{
    iv2 Position;

    if (Win32State.UseRawInput)
    {
        Position = Win32State.RawCursorPosition;
    }
    else
    {
        POINT ScreenSpacePosition;
        GetCursorPos(&ScreenSpacePosition);

        POINT ClientSpacePosition = ScreenSpacePosition;
        ScreenToClient(Window, &ClientSpacePosition);

        Position = IV2(ClientSpacePosition.x, ClientSpacePosition.y);
    }

    return Position;
}

function void
Win32SetCursorPosition(HWND Window, iv2 Position)
{
    POINT ScreenSpacePosition;
    ScreenSpacePosition.x = Position.X;
    ScreenSpacePosition.y = Position.Y;
    ClientToScreen(Window, &ScreenSpacePosition);

    SetCursorPos(ScreenSpacePosition.x, ScreenSpacePosition.y);
}

function void
Win32HideCursor(HWND Window)
{
    if (!Win32State.IsCursorHidden)
    {
        iv2 WindowSize = Win32GetWindowSize(Window);
        iv2 WindowCenter = IV2((WindowSize.X / 2), (WindowSize.Y / 2));

        // NOTE(philip): We store the position of the cursor before we disable it. This way we can restore back to it,
        // whenever we re-enable it.
        Win32State.CursorPositionToRestore = Win32GetCursorPosition(Window);

        // NOTE(philip): Hide the windows cursor.
        ShowCursor(false);

        // NOTE(philip): Center the cursor on the window.
        Win32SetCursorPosition(Window, WindowCenter);

        // NOTE(philip): Restrict the cursor within the bounds of the client area.
        {
            RECT ClientSpaceClientRect = { };
            ClientSpaceClientRect.right = WindowSize.X;
            ClientSpaceClientRect.bottom = WindowSize.Y;

            // NOTE(philip): Since a RECT is essectialy two points, we convert them to screen space separately.
            RECT ScreenSpaceClientRect = ClientSpaceClientRect;
            ClientToScreen(Window, (POINT *)&ScreenSpaceClientRect.left);
            ClientToScreen(Window, (POINT *)&ScreenSpaceClientRect.right);

            ClipCursor(&ScreenSpaceClientRect);
        }

        Win32State.IsCursorHidden = true;
    }
}

function void
Win32RestoreCursor(HWND Window)
{
    if (Win32State.IsCursorHidden)
    {
        ClipCursor(0);
        Win32SetCursorPosition(Window, Win32State.CursorPositionToRestore);
        ShowCursor(true);

        Win32State.IsCursorHidden = false;
    }
}

function void
Win32EnableRawInput(HWND Window)
{
    Win32HideCursor(Window);

    // NOTE(philip): Enable raw input from the mouse.
    // TODO(philip): Probably we want to use raw input for the keyboard and mouse buttons as well.
    // TODO(philip): Ignore legacy messages.
    RAWINPUTDEVICE MouseDevice = { };
    MouseDevice.usUsagePage = 0x01; // NOTE(philip): HID_USAGE_PAGE_GENERIC
    MouseDevice.usUsage = 0x02; // NOTE(philip): HID_USAGE_GENERIC_MOUSE
    MouseDevice.hwndTarget = Window;

    if (!RegisterRawInputDevices(&MouseDevice, 1, sizeof(RAWINPUTDEVICE)))
    {
        // TODO(philip): Error message.
    }
}

function void
Win32DisableRawInput(HWND Window)
{
    // NOTE(philip): Disable raw input from the mouse.
    // TODO(philip): Probably we want to use raw input for the keyboard and mouse buttons as well.
    // TODO(philip): Ignore legacy messages.
    RAWINPUTDEVICE MouseDevice = { };
    MouseDevice.usUsagePage = 0x01; // NOTE(philip): HID_USAGE_PAGE_GENERIC
    MouseDevice.usUsage = 0x02; // NOTE(philip): HID_USAGE_GENERIC_MOUSE
    MouseDevice.dwFlags = RIDEV_REMOVE;
    MouseDevice.hwndTarget = 0;

    if (!RegisterRawInputDevices(&MouseDevice, 1, sizeof(RAWINPUTDEVICE)))
    {
        // TODO(philip): Error message.
    }

    Win32RestoreCursor(Window);
}

function void
Win32SetRawInput(HWND Window, b32 Enabled)
{
    if (Win32State.UseRawInput != Enabled)
    {
        if (Enabled)
        {
            Win32EnableRawInput(Window);
        }
        else
        {
            Win32DisableRawInput(Window);
        }

        Win32State.UseRawInput = Enabled;
    }
}

function LRESULT
Win32WindowProcedure(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;

    switch (Message)
    {
        // TODO(philip): Close the application when we press escape.

        case WM_KILLFOCUS:
        {
            if (Win32State.UseRawInput)
            {
                Win32DisableRawInput(Window);
            }
        } break;

        case WM_SETFOCUS:
        {
            if (Win32State.UseRawInput)
            {
                Win32EnableRawInput(Window);
            }
        } break;

        case WM_INPUT:
        {
            UINT DataSize;
            GetRawInputData((HRAWINPUT)LParam, RID_INPUT, 0, &DataSize, sizeof(RAWINPUTHEADER));

            // TODO(philip): Replace with a memory arena.
            void *Data = OS_AllocateMemory(DataSize);
            GetRawInputData((HRAWINPUT)LParam, RID_INPUT, Data, &DataSize, sizeof(RAWINPUTHEADER));

            RAWINPUT *Input = (RAWINPUT *)Data;
            if (Input->header.dwType == RIM_TYPEMOUSE)
            {
                RAWMOUSE *Mouse = &Input->data.mouse;

                iv2 CursorPositionDelta = IV2(Mouse->lLastX, Mouse->lLastY);
                if (Mouse->usFlags & MOUSE_MOVE_ABSOLUTE)
                {
                    CursorPositionDelta -= Win32State.RawCursorPosition;
                }

                Win32State.RawCursorPosition.X -= CursorPositionDelta.X;
                Win32State.RawCursorPosition.Y += CursorPositionDelta.Y;
            }

            OS_FreeMemory(Data);
        } break;

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
    Load(gl_uniform_3fv,                    glUniform3fv);
    Load(gl_uniform_matrix_4fv,             glUniformMatrix4fv);
    Load(gl_delete_program,                 glDeleteProgram);
    Load(gl_enable_vertex_attrib_array,     glEnableVertexAttribArray);
    Load(gl_vertex_attrib_pointer,          glVertexAttribPointer);

    // NOTE(philip): OpenGL 3.0
    Load(gl_gen_vertex_arrays,              glGenVertexArrays);
    Load(gl_bind_vertex_array,              glBindVertexArray);
    Load(gl_delete_vertex_arrays,           glDeleteVertexArrays);

    // NOTE(philip): OpenGL 3.2
    Load(gl_draw_elements_base_vertex,      glDrawElementsBaseVertex);
}

#undef Load

s32
WinMain(HINSTANCE Instance, HINSTANCE PreviousInstance, LPSTR Arguments, s32 ShowCMD)
{
    // IMPORTANT(philip): Always the first thing that happens.
    Win32InitializeState();

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

                        GLuint VertexShaderModule = GL_LoadShaderModule(GL_VERTEX_SHADER,
                                                                        "assets\\shaders\\gfx_simple_vs.glsl");
                        GLuint PixelShaderModule = GL_LoadShaderModule(GL_FRAGMENT_SHADER,
                                                                       "assets\\shaders\\gfx_simple_ps.glsl");

                        GLint Status;

                        // TODO(philip): Pull shader loading into it's own function.

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

                        mesh_asset MeshAsset = { };
                        LoadOBJ("assets\\meshes\\woman1.obj", &MeshAsset);

                        mesh Mesh = GL_UploadMeshAsset(&MeshAsset);

                        FreeMeshAsset(&MeshAsset);

                        GLint ViewProjectionUniformLocation = glGetUniformLocation(Program, "ViewProjection");
                        GLint TransformUniformLocation = glGetUniformLocation(Program, "Transform");
                        GLint CameraDirectionUniformLocation = glGetUniformLocation(Program, "CameraDirection");

                        glUseProgram(Program);

                        glEnable(GL_DEPTH_TEST);

                        RECT ClientAreaDimensions;
                        GetClientRect(Window, &ClientAreaDimensions);

                        LONG ClientAreaWidth = (ClientAreaDimensions.right - ClientAreaDimensions.left);
                        LONG ClientAreaHeight = (ClientAreaDimensions.bottom - ClientAreaDimensions.top);

                        // TODO(philip): Support resizing.
                        f32 AspectRatio = ((f32)ClientAreaWidth / (f32)ClientAreaHeight);
                        m4 Projection = Perspective(AspectRatio, ToRadians(45.0f), 0.01f, 10000.0f);

                        ShowWindow(Window, SW_SHOW);

                        f32 CameraVerticalSensitivity = 0.1f;
                        f32 CameraHorizontalSensitivity = 0.1f;

                        v3 CameraForward = V3(0.0f, 0.0f, -1.0f);
                        f32 CameraPitch = 0.0f;
                        f32 CameraYaw = -45.0f;

                        f32 CameraMovementSpeed = 0.15f;
                        v3 CameraPosition = V3(-20.0f, 0.0f, 20.0f);

                        b32 ControlCamera = false;
                        iv2 PreviousCursorPosition = { };

                        wglSwapIntervalEXT(1);

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

                            if (GetKeyState(VK_LBUTTON) & 0x8000)
                            {
                                if (!ControlCamera)
                                {
                                    Win32SetRawInput(Window, true);
                                    ControlCamera = true;
                                }
                            }

                            if (GetKeyState(VK_ESCAPE) & 0x8000)
                            {
                                if (ControlCamera)
                                {
                                    Win32SetRawInput(Window, false);
                                    ControlCamera = false;
                                }
                            }

                            if (ControlCamera)
                            {
                                iv2 CursorPosition = Win32GetCursorPosition(Window);
                                iv2 CursorPositionDelta = PreviousCursorPosition - CursorPosition;
                                PreviousCursorPosition = CursorPosition;

                                // TODO(philip): Integrate time.
                                // TODO(philip): Clamp these.
                                CameraPitch += CursorPositionDelta.Y * CameraVerticalSensitivity;
                                CameraYaw -= CursorPositionDelta.X * CameraHorizontalSensitivity;
                            }

                            v3 CameraRight = Normalize(Cross(CameraForward, V3(0.0f, 1.0f, 0.0f)));

                            quat CameraRotation = AxisAngleRotation(CameraRight, ToRadians(CameraPitch)) *
                                AxisAngleRotation(V3(0.0f, 1.0f, 0.0f), ToRadians(CameraYaw));

                            CameraForward = Normalize(RotateV3(V3(0.0f, 0.0f, -1.0f), CameraRotation));

                            if (ControlCamera)
                            {
                                // TODO(philip): Should the forward and right camera movement vectors be the same
                                // as the rotation ones, or the world ones.

                                if (GetKeyState(0x57) & 0x8000)
                                {
                                    // NOTE(philip): W key is pressed.

                                    // TODO(philip): Integrate time.
                                    CameraPosition += CameraForward * CameraMovementSpeed;
                                }

                                if (GetKeyState(0x53) & 0x8000)
                                {
                                    // NOTE(philip): S key is pressed.

                                    // TODO(philip): Integrate time.
                                    CameraPosition -= CameraForward * CameraMovementSpeed;
                                }

                                if (GetKeyState(0x41) & 0x8000)
                                {
                                    // NOTE(philip): A key is pressed.

                                    // TODO(philip): Integrate time.
                                    CameraPosition -= CameraRight * CameraMovementSpeed;
                                }

                                if (GetKeyState(0x44) & 0x8000)
                                {
                                    // NOTE(philip): D key is pressed.

                                    // TODO(philip): Integrate time.
                                    CameraPosition += CameraRight * CameraMovementSpeed;
                                }
                            }

                            // NOTE(philip): Lock the camera on the XZ plane.
                            CameraPosition.Y = 0.0f;

                            // TODO(philip): Overload the negative operator for v3.
                            m4 View = Translate(V3(-CameraPosition.X, -CameraPosition.Y, -CameraPosition.Z)) *
                                ToM4(Conjugate(CameraRotation));

                            m4 ViewProjection = View * Projection;

                            m4 Transform = Scale(V3(0.05f, 0.05f, 0.05f)) *
                                ToM4(AxisAngleRotation(V3(0.0f, 1.0f, 0.0f), ToRadians(-90.0f))) *
                                Translate(V3(0.0f, 0.0f, 0.0f));

                            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                            glUniformMatrix4fv(ViewProjectionUniformLocation, 1, GL_FALSE,
                                               (GLfloat *)&ViewProjection);
                            glUniformMatrix4fv(TransformUniformLocation, 1, GL_FALSE, (GLfloat *)&Transform);
                            glUniform3fv(CameraDirectionUniformLocation, 1, (GLfloat *)&CameraForward);

                            // TODO(philip): Pull mesh rendering into it's own function.
                            glBindVertexArray(Mesh.VertexArray);

                            for (u64 SubmeshIndex = 0;
                                 SubmeshIndex < Mesh.SubmeshCount;
                                 ++SubmeshIndex)
                            {
                                submesh *Submesh = Mesh.Submeshes + SubmeshIndex;
                                glDrawElements(GL_TRIANGLES, Submesh->IndexCount, GL_UNSIGNED_INT,
                                               (GLvoid *)(Submesh->IndexOffset * sizeof(u32)));
                            }

                            SwapBuffers(DeviceContext);
                        }

                        GL_FreeMesh(&Mesh);

                        glDeleteProgram(Program);

                        // TODO(philip): Move shader deletion after program linking.
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
