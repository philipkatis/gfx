function f32
ToRadians(f32 Degrees)
{
    f32 Result = (M_PI * (Degrees / 180.0f));
    return Result;
}

function f32
Clamp(f32 Value, f32 Min, f32 Max)
{
    f32 Result = Value;

    if (Value < Min)
    {
        Result = Min;
    }
    else if (Value > Max)
    {
        Result = Max;
    }

    return Result;
}

function iv2
operator-(iv2 Left, iv2 Right)
{
    iv2 Result = { (Left.X - Right.X), (Left.Y - Right.Y) };
    return Result;
}

function iv2 &
operator+=(iv2 &Left, iv2 Right)
{
    Left.X += Right.X;
    Left.Y += Right.Y;

    return Left;
}

function iv2
IV2(s32 X, s32 Y)
{
    iv2 Result = { X, Y };
    return Result;
}

function v3
operator-(v3 Vector)
{
    v3 Result = { -Vector.X, -Vector.Y, -Vector.Z };
    return Result;
}

function v3
operator*(v3 Vector, f32 Scalar)
{
    v3 Result = { (Vector.X * Scalar), (Vector.Y * Scalar), (Vector.Z * Scalar) };
    return Result;
}

function v3 &
operator+=(v3 &Left, v3 Right)
{
    Left.X += Right.X;
    Left.Y += Right.Y;
    Left.Z += Right.Z;

    return Left;
}

function v3 &
operator-=(v3 &Left, v3 Right)
{
    Left.X -= Right.X;
    Left.Y -= Right.Y;
    Left.Z -= Right.Z;

    return Left;
}

function v3
V3(f32 X, f32 Y, f32 Z)
{
    v3 Result = { X, Y, Z };
    return Result;
}

function f32
Length(v3 Vector)
{
    f32 Result = sqrtf((Vector.X * Vector.X) + (Vector.Y * Vector.Y) + (Vector.Z * Vector.Z));
    return Result;
}

function v3
Normalize(v3 Vector)
{
    f32 Len = Length(Vector);

    v3 Result = { (Vector.X / Len), (Vector.Y / Len), (Vector.Z / Len) };
    return Result;
}

function v4
V4(f32 X, f32 Y, f32 Z, f32 W)
{
    v4 Result = { X, Y, Z, W };
    return Result;
}

function quat
Conjugate(quat Quaternion)
{
    quat Result = { -Quaternion.X, -Quaternion.Y, -Quaternion.Z, Quaternion.W };
    return Result;
}

function f32
Length(quat Quaternion)
{
    f32 Result = sqrtf((Quaternion.X * Quaternion.X) + (Quaternion.Y * Quaternion.Y) +
                       (Quaternion.Z * Quaternion.Z) + (Quaternion.W * Quaternion.W));
    return Result;
}

function quat
Normalize(quat Quaternion)
{
    f32 Len = Length(Quaternion);

    quat Result = { (Quaternion.X / Len), (Quaternion.Y / Len), (Quaternion.Z / Len), (Quaternion.W / Len) };
    return Result;
}

function quat
operator*(quat Left, quat Right)
{
    quat Result =
    {
        ((Left.W * Right.X) + (Left.X * Right.W) + (Left.Y * Right.Z) - (Left.Z * Right.Y)),
        ((Left.W * Right.Y) - (Left.X * Right.Z) + (Left.Y * Right.W) + (Left.Z * Right.X)),
        ((Left.W * Right.Z) + (Left.X * Right.Y) - (Left.Y * Right.X) + (Left.Z * Right.W)),
        ((Left.W * Right.W) - (Left.X * Right.X) - (Left.Y * Right.Y) - (Left.Z * Right.Z))
    };

    Result = Normalize(Result);

    return Result;
}

function quat
Rotate(v3 Axis, f32 Angle)
{
    Axis = Normalize(Axis);

    f32 HalfAngle = (Angle / 2.0f);
    f32 HalfAngleSin = sinf(HalfAngle);
    f32 HalfAngleCos = cosf(HalfAngle);

    quat Result = { (Axis.X * HalfAngleSin), (Axis.Y * HalfAngleSin), (Axis.Z * HalfAngleSin), HalfAngleCos };
    return Result;
}

function v3
RotateVector(v3 Vector, quat Quaternion)
{
    quat P = { Vector.X, Vector.Y, Vector.Z };
    quat Conj = Conjugate(Quaternion);

    quat RotatedP = ((Quaternion * P) * Conj);

    v3 Result = { RotatedP.X, RotatedP.Y, RotatedP.Z };
    return Result;
}

function m4
operator*(m4 Left, m4 Right)
{
    m4 Result = { };

    for (u32 Row = 0;
         Row < 4;
         ++Row)
    {
        for (u32 Column = 0;
             Column < 4;
             ++Column)
        {
            for (u32 Index = 0;
                 Index < 4;
                 ++Index)
            {
                Result.Data[Column + Row * 4] += Left.Data[Index + Row * 4] *
                    Right.Data[Column + Index * 4];
            }
        }
    }

    return Result;
}

function m4
Perspective(f32 AspectRatio, f32 VerticalFOV, f32 NearPlane, f32 FarPlane)
{
    f32 HalfVerticalFOVTan = tanf((VerticalFOV / 2.0f));

    f32 A = (1.0f / (AspectRatio * HalfVerticalFOVTan));
    f32 B = (1.0f / HalfVerticalFOVTan);
    f32 C = ((NearPlane + FarPlane) / (NearPlane - FarPlane));
    f32 D = ((2.0f * NearPlane * FarPlane) / (NearPlane - FarPlane));

    m4 Result =
    {
        A,    0.0f,  0.0f, 0.0f,
        0.0f, B,     0.0f, 0.0f,
        0.0f, 0.0f,  C,    D,
        0.0f, 0.0f, -1.0f, 0.0f
    };

    return Result;
}

function m4
Translate(v3 Translation)
{
    m4 Result =
    {
        1.0f, 0.0f, 0.0f, Translation.X,
        0.0f, 1.0f, 0.0f, Translation.Y,
        0.0f, 0.0f, 1.0f, Translation.Z,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    return Result;
}

function v3
GetTranslation(m4 Transform)
{
    v3 Result = { Transform.Data[3 + 0 * 4], Transform.Data[3 + 1 * 4], Transform.Data[3 + 2 * 4] };
    return Result;
}

function m4
Scale(v3 Scale)
{
    m4 Result =
    {
        Scale.X, 0.0f,    0.0f,    0.0f,
        0.0f,    Scale.Y, 0.0f,    0.0f,
        0.0f,    0.0f,    Scale.Z, 0.0f,
        0.0f,    0.0f,    0.0f,    1.0f
    };

    return Result;
}

function m4
ToMatrix(quat Quaternion)
{
    f32 XSquared = (Quaternion.X * Quaternion.X);
    f32 YSquared = (Quaternion.Y * Quaternion.Y);
    f32 ZSquared = (Quaternion.Z * Quaternion.Z);

    m4 Result = { };
    Result.Data[0 + 0 * 4] = (1.0f - (2.0f * YSquared) - (2.0f * ZSquared));
    Result.Data[0 + 1 * 4] = ((2.0f * Quaternion.X * Quaternion.Y) + (2.0f * Quaternion.W * Quaternion.Z));
    Result.Data[0 + 2 * 4] = ((2.0f * Quaternion.X * Quaternion.Z) - (2.0f * Quaternion.W * Quaternion.Y));

    Result.Data[1 + 0 * 4] = ((2.0f * Quaternion.X * Quaternion.Y) - (2.0f * Quaternion.W * Quaternion.Z));
    Result.Data[1 + 1 * 4] = (1.0f - (2.0f * XSquared) - (2.0f * ZSquared));
    Result.Data[1 + 2 * 4] = ((2.0f * Quaternion.Y * Quaternion.Z) + (2.0f * Quaternion.W * Quaternion.X));

    Result.Data[2 + 0 * 4] = ((2.0f * Quaternion.X * Quaternion.Z) + (2.0f * Quaternion.W * Quaternion.Y));
    Result.Data[2 + 1 * 4] = ((2.0f * Quaternion.Y * Quaternion.Z) - (2.0f * Quaternion.W * Quaternion.X));
    Result.Data[2 + 2 * 4] = (1.0f - (2.0f * YSquared) - (2.0f * XSquared));

    Result.Data[3 + 3 * 4] = 1.0f;

    return Result;
}
