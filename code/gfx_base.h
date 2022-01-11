#ifndef GFX_BASE_H
#define GFX_BASE_H

//
// NOTE(philip): Base Keywords
//

#define true                   1
#define false                  0

// TODO(philip): Redefine static.

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

// TODO(philip): Floating point.
// TODO(philip): Ensure proper size for base types during compile time.

// TODO(philip): Assert macro.

#endif
