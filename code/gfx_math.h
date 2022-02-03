#ifndef GFX_MATH_H
#define GFX_MATH_H

//
// NOTE(philip): Constants
//

#define PI (3.1415926535897932f)

//
// NOTE(philip): 2-Component Integer Vector
//

union iv2
{
    struct
    {
        s32 X;
        s32 Y;
    };

    s32 Data[2];
};

//
// NOTE(philip): 2-Component Floating Point Vector
//

union v2
{
    struct
    {
        f32 X;
        f32 Y;
    };

    f32 Data[2];
};

//
// NOTE(philip): 3-Component Floating Point Vector
//

union v3
{
    struct
    {
        f32 X;
        f32 Y;
        f32 Z;
    };

    f32 Data[3];
};

//
// NOTE(philip): 4-Component Floating Point Vector
//

union v4
{
    struct
    {
        f32 X;
        f32 Y;
        f32 Z;
        f32 W;
    };

    f32 Data[4];
};

//
// NOTE(philip): Quaternion
//

union quat
{
    struct
    {
        f32 X;
        f32 Y;
        f32 Z;
        f32 W;
    };

    f32 Data[4];
};

//
// NOTE(philip): 4x4 Matrix
//

union m4
{
    struct
    {
        v4 Rows[4];
    };

    f32 Data[16];
};

#endif
