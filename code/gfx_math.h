#ifndef GFX_MATH_H
#define GFX_MATH_H

// TODO(philip): If we use this for more things, change into a union.
struct v3
{
    f32 X;
    f32 Y;
    f32 Z;
};

struct m4
{
    f32 Elements[16];
};

#endif
