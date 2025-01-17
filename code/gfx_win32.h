#ifndef GFX_WIN32_H
#define GFX_WIN32_H

//
// NOTE(philip): WGL Extensions
//

// NOTE(philip): This assignment requires the use of a modern OpenGL context. Windows only ever shipped with OpenGL 1.1
// so the functions used to create modern contexts come with the form of WGL extensions. These extensions are part of
// the OpenGL driver provided by the GPU manufacturer, and thus need to be loaded explictly. Below are all the function
// declarations and constants we require as well as reference. The extensions used are widely supported, so them not
// existing will not be an issue.

// NOTE(philip): https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_pixel_format.txt

#define WGL_DRAW_TO_WINDOW_ARB                   0x2001
#define WGL_SUPPORT_OPENGL_ARB                   0x2010
#define WGL_DOUBLE_BUFFER_ARB                    0x2011
#define WGL_PIXEL_TYPE_ARB                       0x2013
#define WGL_COLOR_BITS_ARB                       0x2014
#define WGL_DEPTH_BITS_ARB                       0x2022
#define WGL_STENCIL_BITS_ARB                     0x2023

#define WGL_TYPE_RGBA_ARB                        0x202B

typedef BOOL wgl_choose_pixel_format_arb(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList,
                                         UINT nMaxFormats, int *piFormats, UINT *nNumFormats);

global wgl_choose_pixel_format_arb *wglChoosePixelFormatARB = 0;

// NOTE(philip): https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_create_context.txt

#define WGL_CONTEXT_MAJOR_VERSION_ARB            0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB            0x2092
#define WGL_CONTEXT_FLAGS_ARB                    0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB             0x9126

#define WGL_CONTEXT_DEBUG_BIT_ARB                0x00000001

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB         0x00000001

typedef HGLRC wgl_create_context_attribs_arb(HDC hDC, HGLRC hShareContext, const int *attribList);

global wgl_create_context_attribs_arb *wglCreateContextAttribsARB = 0;

// NOTE(philip): https://www.khronos.org/registry/OpenGL/extensions/EXT/WGL_EXT_swap_control.txt

typedef BOOL wgl_swap_interval_ext(int interval);

global wgl_swap_interval_ext *wglSwapIntervalEXT = 0;

//
// NOTE(philip): Win32 Context
//

struct win32_context
{
    HWND Window;
    POINT RestoreCursorPosition;
    iv2 RawCursorPosition;
};

global win32_context Win32Context = { };

#endif
