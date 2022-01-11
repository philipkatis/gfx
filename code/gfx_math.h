#ifndef GFX_MATH_H
#define GFX_MATH_H

#define PI (3.1415926535897932f)

// TODO(philip): If we use this for more things, change into a union.
struct v3
{
    f32 X;
    f32 Y;
    f32 Z;
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
