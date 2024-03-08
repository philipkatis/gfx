#include <Windows.h>
#include <gl/gl.h>

// NOTE(philip): sprintf_s().
#include <stdio.h>

// NOTE(philip): M_PI, sqrtf(), sinf(), cosf(), tanf().
#define _USE_MATH_DEFINES
#include <math.h>

#include "gfx_base.h"
#include "gfx_math.h"
#include "gfx_platform.h"
#include "gfx_asset.h"
#include "gfx_win32.h"
#include "gfx_gl.h"

#include "gfx_base.cpp"
#include "gfx_math.cpp"
#include "gfx_asset.cpp"
#include "gfx_gl.cpp"

function void *
OS_AllocateMemory(u64 Size)
{
    void *Memory = VirtualAlloc(0, Size, (MEM_COMMIT | MEM_RESERVE), PAGE_READWRITE);
    return Memory;
}

function void
OS_FreeMemory(void *Memory)
{
    VirtualFree(Memory, 0, MEM_RELEASE);
}

function b32
OS_ReadEntireFile(char *FilePath, buffer *FileData)
{
    b32 Success = false;

    HANDLE File = CreateFileA(FilePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (File != INVALID_HANDLE_VALUE)
    {
        GetFileSizeEx(File, (LARGE_INTEGER *)&FileData->Size);
        FileData->Data = (u8 *)OS_AllocateMemory(FileData->Size);

        DWORD BytesRead;
        if (ReadFile(File, FileData->Data, FileData->Size, &BytesRead, 0) && (BytesRead == FileData->Size))
        {
            Success = true;
        }
        else
        {
            OS_FreeMemory(FileData->Data);
            *FileData = { };
        }
    }

    CloseHandle(File);

    return Success;
}

function void
OS_FreeFileMemory(buffer FileData)
{
    OS_FreeMemory(FileData.Data);
}

global b32 IsCameraActive = false;
global f32 CameraMovementSpeed = 25.0f;

function void
Win32EnableCursor(void)
{
    ClipCursor(0);
    SetCursorPos(Win32Context.RestoreCursorPosition.x, Win32Context.RestoreCursorPosition.y);
    SetCursor(LoadCursor(0, IDC_ARROW));

    RAWINPUTDEVICE MouseDevice = { };
    MouseDevice.usUsagePage = 0x01;
    MouseDevice.usUsage = 0x02;
    MouseDevice.dwFlags = RIDEV_REMOVE;

    RegisterRawInputDevices(&MouseDevice, 1, sizeof(RAWINPUTDEVICE));
}

function void
Win32DisableCursor(void)
{
    RECT ClientRect;
    GetClientRect(Win32Context.Window, &ClientRect);

    ClientToScreen(Win32Context.Window, (LPPOINT)&ClientRect.left);
    ClientToScreen(Win32Context.Window, (LPPOINT)&ClientRect.right);

    SetCursor(0);
    GetCursorPos(&Win32Context.RestoreCursorPosition);
    ClipCursor(&ClientRect);

    RAWINPUTDEVICE MouseDevice = { };
    MouseDevice.usUsagePage = 0x01;
    MouseDevice.usUsage = 0x02;
    MouseDevice.hwndTarget = Win32Context.Window;

    RegisterRawInputDevices(&MouseDevice, 1, sizeof(RAWINPUTDEVICE));
}

function LRESULT
Win32WindowProcedure(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;

    switch (Message)
    {
        case WM_LBUTTONDOWN:
        {
            if (!IsCameraActive)
            {
                Win32DisableCursor();
                IsCameraActive = true;
            }
        } break;

        case WM_MOUSEWHEEL:
        {
            if (IsCameraActive)
            {
                s64 Delta = GET_WHEEL_DELTA_WPARAM(WParam);

                CameraMovementSpeed += ((f32)Delta / 240.0f);
                CameraMovementSpeed = Clamp(CameraMovementSpeed, 0.1f, 50.0f);
            }
        } break;

        case WM_KEYDOWN:
        {
            switch (WParam)
            {
                case VK_ESCAPE:
                {
                    if (IsCameraActive)
                    {
                        Win32EnableCursor();
                        IsCameraActive = false;
                    }
                    else
                    {
                        PostQuitMessage(0);
                    }
                } break;
            }
        } break;

        case WM_KILLFOCUS:
        {
            if (IsCameraActive)
            {
                Win32EnableCursor();
            }
        } break;

        case WM_SETFOCUS:
        {
            if (IsCameraActive)
            {
                Win32DisableCursor();
            }
        } break;

        case WM_INPUT:
        {
            HRAWINPUT InputHandle = (HRAWINPUT)LParam;

            UINT InputSize;
            GetRawInputData(InputHandle, RID_INPUT, 0, &InputSize, sizeof(RAWINPUTHEADER));

            RAWINPUT *Input = (RAWINPUT *)OS_AllocateMemory(InputSize);
            GetRawInputData(InputHandle, RID_INPUT, Input, &InputSize, sizeof(RAWINPUTHEADER));

            Win32Context.RawCursorPosition += IV2(Input->data.mouse.lLastX, Input->data.mouse.lLastY);

            OS_FreeMemory(Input);
        } break;

        case WM_DESTROY:
        {
            PostQuitMessage(0);
        } break;

        case WM_CLOSE:
        {
            DestroyWindow(Window);
        } break;

        default:
        {
            Result = DefWindowProcA(Window, Message, WParam, LParam);
        } break;
    }

    return Result;
}

int
WinMain(HINSTANCE Instance, HINSTANCE PreviousInstance, LPSTR Arguments, int ShowCMD)
{
    u64 PerformanceCounterFrequency;
    QueryPerformanceFrequency((LARGE_INTEGER *)&PerformanceCounterFrequency);

    char *WindowClassName = "gfx_window_class";

    WNDCLASSA WindowClass = { };
    WindowClass.style = (CS_VREDRAW | CS_HREDRAW | CS_OWNDC);
    WindowClass.lpfnWndProc = Win32WindowProcedure;
    WindowClass.hInstance = Instance;
    WindowClass.lpszClassName = WindowClassName;

    RegisterClassA(&WindowClass);

    HWND Window = CreateWindowA(WindowClassName, 0, 0, 0, 0, 0, 0, 0, 0, Instance, 0);
    HDC DeviceContext = GetDC(Window);

    PIXELFORMATDESCRIPTOR DesiredPixelFormat = { };
    DesiredPixelFormat.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    DesiredPixelFormat.nVersion = 1;
    DesiredPixelFormat.dwFlags = (PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL);
    DesiredPixelFormat.iPixelType = PFD_TYPE_RGBA;
    DesiredPixelFormat.cColorBits = 32;
    DesiredPixelFormat.cDepthBits = 24;
    DesiredPixelFormat.cStencilBits = 8;
    DesiredPixelFormat.iLayerType = PFD_MAIN_PLANE;

    s32 SelectedPixelFormatIndex = ChoosePixelFormat(DeviceContext, &DesiredPixelFormat);

    PIXELFORMATDESCRIPTOR SelectedPixelFormat;
    DescribePixelFormat(DeviceContext, SelectedPixelFormatIndex, sizeof(PIXELFORMATDESCRIPTOR), &SelectedPixelFormat);

    SetPixelFormat(DeviceContext, SelectedPixelFormatIndex, &SelectedPixelFormat);

    HGLRC OpenGLContext = wglCreateContext(DeviceContext);
    wglMakeCurrent(DeviceContext, OpenGLContext);

    wglChoosePixelFormatARB = (wgl_choose_pixel_format_arb *)wglGetProcAddress("wglChoosePixelFormatARB");
    wglCreateContextAttribsARB = (wgl_create_context_attribs_arb *)wglGetProcAddress("wglCreateContextAttribsARB");
    wglSwapIntervalEXT = (wgl_swap_interval_ext *)wglGetProcAddress("wglSwapIntervalEXT");

    wglDeleteContext(OpenGLContext);

    ReleaseDC(Window, DeviceContext);
    DestroyWindow(Window);

    MSG Message;
    while (PeekMessageA(&Message, 0, 0, 0, PM_REMOVE));

    Window = CreateWindowA(WindowClassName, "gfx", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                           CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, Instance, 0);
    DeviceContext = GetDC(Window);

    Win32Context.Window = Window;

    s32 PixelFormatAttributes[] =
    {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
        WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB,     32,
        WGL_DEPTH_BITS_ARB,     24,
        WGL_STENCIL_BITS_ARB,   8,
        0
    };

    s32 PixelFormatIndex;
    UINT PixelFormatCount;
    wglChoosePixelFormatARB(DeviceContext, PixelFormatAttributes, 0, 1, &PixelFormatIndex, &PixelFormatCount);

    PIXELFORMATDESCRIPTOR PixelFormat;
    DescribePixelFormat(DeviceContext, PixelFormatIndex, sizeof(PIXELFORMATDESCRIPTOR), &PixelFormat);

    SetPixelFormat(DeviceContext, PixelFormatIndex, &PixelFormat);

    s32 OpenGLContextAttributes[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_FLAGS_ARB,         WGL_CONTEXT_DEBUG_BIT_ARB,
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    OpenGLContext = wglCreateContextAttribsARB(DeviceContext, 0, OpenGLContextAttributes);
    wglMakeCurrent(DeviceContext, OpenGLContext);

    glBindBuffer = (gl_bind_buffer *)wglGetProcAddress("glBindBuffer");
    glDeleteBuffers = (gl_delete_buffers *)wglGetProcAddress("glDeleteBuffers");
    glGenBuffers = (gl_gen_buffers *)wglGetProcAddress("glGenBuffers");
    glBufferData = (gl_buffer_data *)wglGetProcAddress("glBufferData");

    glAttachShader = (gl_attach_shader *)wglGetProcAddress("glAttachShader");
    glCompileShader = (gl_compile_shader *)wglGetProcAddress("glCompileShader");
    glCreateProgram = (gl_create_program *)wglGetProcAddress("glCreateProgram");
    glCreateShader = (gl_create_shader *)wglGetProcAddress("glCreateShader");
    glDeleteProgram = (gl_delete_program *)wglGetProcAddress("glDeleteProgram");
    glDeleteShader = (gl_delete_shader *)wglGetProcAddress("glDeleteShader");
    glDetachShader = (gl_detach_shader *)wglGetProcAddress("glDetachShader");
    glEnableVertexAttribArray = (gl_enable_vertex_attrib_arrray *)wglGetProcAddress("glEnableVertexAttribArray");
    glGetProgramiv = (gl_get_program_iv *)wglGetProcAddress("glGetProgramiv");
    glGetProgramInfoLog = (gl_get_program_info_log *)wglGetProcAddress("glGetProgramInfoLog");
    glGetShaderiv = (gl_get_shader_iv *)wglGetProcAddress("glGetShaderiv");
    glGetShaderInfoLog = (gl_get_shader_info_log *)wglGetProcAddress("glGetShaderInfoLog");
    glGetUniformLocation = (gl_get_uniform_location *)wglGetProcAddress("glGetUniformLocation");
    glLinkProgram = (gl_link_program *)wglGetProcAddress("glLinkProgram");
    glShaderSource = (gl_shader_source *)wglGetProcAddress("glShaderSource");
    glUseProgram = (gl_use_program *)wglGetProcAddress("glUseProgram");
    glUniform1fv = (gl_uniform_1fv *)wglGetProcAddress("glUniform1fv");
    glUniform3fv = (gl_uniform_3fv *)wglGetProcAddress("glUniform3fv");
    glUniform4fv = (gl_uniform_4fv *)wglGetProcAddress("glUniform4fv");
    glUniformMatrix4fv = (gl_uniform_matrix_4fv *)wglGetProcAddress("glUniformMatrix4fv");
    glValidateProgram = (gl_validate_program *)wglGetProcAddress("glValidateProgram");
    glVertexAttribPointer = (gl_vertex_attrib_pointer *)wglGetProcAddress("glVertexAttribPointer");

    glBindVertexArray = (gl_bind_vertex_array *)wglGetProcAddress("glBindVertexArray");
    glDeleteVertexArrays = (gl_delete_vertex_arrays *)wglGetProcAddress("glDeleteVertexArrays");
    glGenVertexArrays = (gl_gen_vertex_arrays *)wglGetProcAddress("glGenVertexArrays");

    shader UberShader = GL_LoadShader("assets\\shaders\\gfx_uber_vs.glsl", "assets\\shaders\\gfx_uber_ps.glsl");

    f32 CameraHorizontalSensitivity = 0.05f;
    f32 CameraVerticalSensitivity = 0.05f;

    v3 CameraPosition = V3(-20.0f, 0.0f, 20.0f);
    f32 CameraPitch = 0.0f;
    f32 CameraYaw = 45.0f;

    v3 AmbientLightColor = V3(1.0f, 1.0f, 1.0f);
    f32 AmbientLightIntensity = 0.05f;

    v3 LightColor = V3(1.0f, 1.0f, 1.0f);
    f32 LightIntensity = 1.0f;

    material_asset SphereMaterial = { };
    SphereMaterial.Properties = (MaterialProperty_Unlit | MaterialProperty_Wireframe);
    SphereMaterial.BaseColor = V4(1.0f, 1.0f, 1.0f, 1.0f);

    mesh_asset SphereAsset = GenerateUVSphere(24, 24);
    mesh SphereMesh = GL_UploadMesh(SphereAsset, 1, &SphereMaterial);
    FreeMeshAsset(SphereAsset);

    f32 SphereOrbitAngle = 0.0f;
    f32 SphereSpeed = 65.0f;
    f32 SphereOrbitRadius = 20.0f;

    mesh_asset HeadAsset = LoadMeshAsset("assets\\meshes\\woman1.obj");
    mesh HeadMesh = GL_UploadMesh(HeadAsset, HeadAsset.MaterialAssetCount, HeadAsset.MaterialAssets);
    FreeMeshAsset(HeadAsset);

    quat HeadOrientation = Rotate(V3(0.0f, 1.0f, 0.0f), ToRadians(-90.0f));
    m4 HeadTransform = ToMatrix(HeadOrientation) * Scale(V3(0.075f, 0.075f, 0.075f));

    iv2 LastCursorPosition = { };

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    f32 Timestep = 0.0f;

    u64 LastFrameEndTicks;
    QueryPerformanceCounter((LARGE_INTEGER *)&LastFrameEndTicks);

    wglSwapIntervalEXT(1);

    ShowWindow(Window, SW_SHOW);

    for (;;)
    {
        b32 IsExitRequested = false;

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

        if (IsCameraActive)
        {
            iv2 CursorPosition = Win32Context.RawCursorPosition;
            iv2 CursorPositionDelta = CursorPosition - LastCursorPosition;
            LastCursorPosition = CursorPosition;

            CameraYaw += CursorPositionDelta.X * CameraHorizontalSensitivity;
            CameraPitch += CursorPositionDelta.Y * CameraVerticalSensitivity;

            CameraPitch = Clamp(CameraPitch, -89.9f, 89.9f);
        }

        quat CameraOrientation = Rotate(V3(0.0f, 1.0f, 0.0f), ToRadians(-CameraYaw)) *
            Rotate(V3(1.0f, 0.0f, 0.0f), ToRadians(-CameraPitch));

        if (IsCameraActive)
        {
            v3 CameraForward = RotateVector(V3(0.0f, 0.0f, -1.0f), CameraOrientation);
            v3 CameraRight = RotateVector(V3(1.0f, 0.0f, 0.0f), CameraOrientation);

            if (GetKeyState(0x57) & 0x8000)
            {
                // NOTE(philip): W Key
                CameraPosition += CameraForward * CameraMovementSpeed * Timestep;
            }

            if (GetKeyState(0x53) & 0x8000)
            {
                // NOTE(philip): S Key
                CameraPosition -= CameraForward * CameraMovementSpeed * Timestep;
            }

            if (GetKeyState(0x41) & 0x8000)
            {
                // NOTE(philip): A Key
                CameraPosition -= CameraRight * CameraMovementSpeed * Timestep;
            }

            if (GetKeyState(0x44) & 0x8000)
            {
                // NOTE(philip): D Key
                CameraPosition += CameraRight * CameraMovementSpeed * Timestep;
            }

            if (GetKeyState(VK_LSHIFT) & 0x8000)
            {
                CameraPosition += V3(0.0f, 1.0f, 0.0f) * CameraMovementSpeed * Timestep;
            }

            if (GetKeyState(VK_LCONTROL) & 0x8000)
            {
                CameraPosition -= V3(0.0f, 1.0f, 0.0f) * CameraMovementSpeed * Timestep;
            }
        }

        RECT ClientRect;
        GetClientRect(Win32Context.Window, &ClientRect);

        f32 ClientAreaWidth = ((f32)ClientRect.right - (f32)ClientRect.left);
        f32 ClientAreaHeight = ((f32)ClientRect.bottom - (f32)ClientRect.top);

        f32 AspectRatio = ClientAreaWidth / ClientAreaHeight;
        m4 Projection = Perspective(AspectRatio, ToRadians(45.0f), 0.01f, 10000.0f);

        m4 View = ToMatrix(Conjugate(CameraOrientation)) * Translate(-CameraPosition);
        m4 ViewProjection = Projection * View;

        if (GetKeyState(0x48) & 0x8000)
        {
            SphereSpeed -= 50.0f * Timestep;
        }

        if (GetKeyState(0x4A) & 0x8000)
        {
            SphereSpeed += 50.0f * Timestep;
        }

        SphereOrbitAngle += SphereSpeed * Timestep;

        quat SphereOrientation = Rotate(V3(0.0f, 1.0f, 0.0f), ToRadians(SphereOrbitAngle));
        m4 SphereTransform = ToMatrix(SphereOrientation) * Translate(V3(0.0f, 0.0f, SphereOrbitRadius));
        v3 SpherePosition = GetTranslation(SphereTransform);

        glViewport(0, 0, ClientAreaWidth, ClientAreaHeight);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(UberShader.Program);

        // NOTE(philip): Update system uniforms.
        glUniformMatrix4fv(UberShader.ViewProjectionUniform, 1, GL_TRUE, (GLfloat *)&ViewProjection);
        glUniform3fv(UberShader.CameraPositionUniform, 1, (GLfloat *)&CameraPosition);

        // NOTE(philip): Update scene uniforms.
        glUniform3fv(UberShader.AmbientLightColorUniform, 1, (GLfloat *)&AmbientLightColor);
        glUniform1fv(UberShader.AmbientLightIntensityUniform, 1, (GLfloat *)&AmbientLightIntensity);
        glUniform3fv(UberShader.LightPositionUniform, 1, (GLfloat *)&SpherePosition);
        glUniform3fv(UberShader.LightColorUniform, 1, (GLfloat *)&LightColor);
        glUniform1fv(UberShader.LightIntensityUniform, 1, (GLfloat *)&LightIntensity);

        GL_DrawMesh(UberShader, HeadMesh, HeadTransform);
        GL_DrawMesh(UberShader, SphereMesh, SphereTransform);

        SwapBuffers(DeviceContext);

        u64 CurrentTicks;
        QueryPerformanceCounter((LARGE_INTEGER *)&CurrentTicks);

        u64 ElapsedTicks = (CurrentTicks - LastFrameEndTicks);
        LastFrameEndTicks = CurrentTicks;

        u64 ElapsedUS = (ElapsedTicks * 1000000) / PerformanceCounterFrequency;

        Timestep = ((f32)ElapsedUS / 1000000.0f);

        char WindowTitle[1280];
        sprintf_s(WindowTitle, 256, "gfx | Sphere Speed: %.2f deg/s | Frame Time: %.2f ms", SphereSpeed, ((f32)ElapsedUS / 1000.0f));
        SetWindowText(Window, WindowTitle);
    }

    GL_FreeMesh(HeadMesh);
    GL_FreeMesh(SphereMesh);

    GL_FreeShader(UberShader);

    return 0;
}
