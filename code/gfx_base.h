#ifndef GFX_BASE_H
#define GFX_BASE_H

// TODO(philip): Platform and compiler detection.
// TODO(philip): Configuration setup.

//
// NOTE(philip): Base Keywords
//

#define function               static
#define global                 static
#define local_persistant       static

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

typedef s32                    b32;

typedef float                  f32;
typedef double                 f64;

// TODO(philip): Ensure proper size for base types during compile time.

//
// NOTE(philip): Assert Macro
//

// TODO(philip): Only enable this on debug builds.
#define Assert(Condition) if (!(Condition)) { *(u64 *)0 = 0; }

// TODO(philip): Array count macro.

struct buffer
{
    void *Data;
    u64 Size;
};

#endif
