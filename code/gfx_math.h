#ifndef GFX_MATH_H
#define GFX_MATH_H

struct iv2
{
    s32 X;
    s32 Y;
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

struct quat
{
    f32 X;
    f32 Y;
    f32 Z;
    f32 W;
};

struct m4
{
    f32 Data[16];
};

#endif
