#ifndef GFX_WIN32_H
#define GFX_WIN32_H

//
// NOTE(philip): WGL Extensions
//

/*

  NOTE(philip): In Windows, OpenGL interfaces with the windowing system through WGL. WGL is responsible for
  managing the OpenGL context, swapping the front and back buffers, etc. Since Windows only ever shipped with
  OpenGL 1.1, modern OpenGL contexts are not supported natively, but through extensions. These extensions are
  part of the OpenGL driver provided by the GPU vendor. They have been around for a while and they are supported
  by most GPUs, so it's safe to use them. References to the extension documentation have been provided below.

*/

// NOTE(philip): https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_pixel_format.txt

#define WGL_DRAW_TO_WINDOW_ARB            0x2001
#define WGL_SUPPORT_OPENGL_ARB            0x2010
#define WGL_DOUBLE_BUFFER_ARB             0x2011
#define WGL_PIXEL_TYPE_ARB                0x2013
#define WGL_COLOR_BITS_ARB                0x2014
#define WGL_DEPTH_BITS_ARB                0x2022
#define WGL_STENCIL_BITS_ARB              0x2023

#define WGL_TYPE_RGBA_ARB                 0x202B

typedef BOOL wgl_choose_pixel_format_arb(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList,
                                         UINT nMaxFormats, int *piFormats, UINT *nNumFormats);

static wgl_choose_pixel_format_arb *wglChoosePixelFormatARB = 0;

// NOTE(philip): https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_create_context.txt

#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_FLAGS_ARB             0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126

#define WGL_CONTEXT_DEBUG_BIT_ARB         0x00000001

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001

typedef HGLRC wgl_create_context_attribs_arb(HDC hDC, HGLRC hShareContext, const int *attribList);

static wgl_create_context_attribs_arb *wglCreateContextAttribsARB = 0;

// NOTE(philip): https://www.khronos.org/registry/OpenGL/extensions/EXT/WGL_EXT_swap_control.txt

typedef BOOL wgl_swap_interval_ext(int interval);

static wgl_swap_interval_ext *wglSwapIntervalEXT = 0;

#endif
