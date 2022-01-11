//
// NOTE(philip): 3-Component Vector
//

static v3
V3(float X, float Y, float Z)
{
    v3 Vector = { };

    Vector.X = X;
    Vector.Y = Y;
    Vector.Z = Z;

    return Vector;
}

//
// NOTE(philip): 4x4 Matrix
//

static m4
Identity(void)
{
    m4 Matrix = { };

    Matrix.Elements[0 + 0 * 4] = 1.0f;
    Matrix.Elements[1 + 1 * 4] = 1.0f;
    Matrix.Elements[2 + 2 * 4] = 1.0f;
    Matrix.Elements[3 + 3 * 4] = 1.0f;

    return Matrix;
}

static m4
Translate(v3 Translation)
{
    m4 Matrix = Identity();

    Matrix.Elements[0 + 3 * 4] = Translation.X;
    Matrix.Elements[1 + 3 * 4] = Translation.Y;
    Matrix.Elements[2 + 3 * 4] = Translation.Z;

    return Matrix;
}
