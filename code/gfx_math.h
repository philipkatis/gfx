#ifndef GFX_MATH_H
#define GFX_MATH_H

#define PI (3.1415926535897932f)

// TODO(philip): Change these to unions. Store as array as well.
struct v2
{
    f32 X;
    f32 Y;
};

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
