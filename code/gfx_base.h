#ifndef GFX_BASE_H
#define GFX_BASE_H

//
// NOTE(philip): Base Keywords
//

#define function static
#define global   static

//
// NOTE(philip): Base Types
//

typedef unsigned char          u8;
typedef unsigned short         u16;
typedef unsigned int           u32;
typedef unsigned long long     u64;

typedef signed char            s8;
typedef signed short           s16;
typedef signed int             s32;
typedef signed long long       s64;

typedef float                  f32;
typedef double                 f64;

typedef s32                    b32;

//
// NOTE(philip): Base Structures
//

struct buffer
{
    u8 *Data;
    u64 Size;
};

#endif
