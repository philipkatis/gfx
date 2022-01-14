#ifndef GFX_MATH_H
#define GFX_MATH_H

#define PI (3.1415926535897932f)

union iv2
{
    struct
    {
        s32 X;
        s32 Y;
    };

    s32 Data[2];
};

union v2
{
    struct
    {
        f32 X;
        f32 Y;
    };

    f32 Data[2];
};

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

struct quat
{
    f32 X;
    f32 Y;
    f32 Z;
    f32 W;
};

struct m4
{
    f32 Elements[16];
};

#endif
