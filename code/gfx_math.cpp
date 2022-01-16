//
// NOTE(philip): Base Functions
//

function f32
ToRadians(f32 Value)
{
    f32 Result = (Value * (PI / 180.0f));
    return Result;
}

function f32
Sin(f32 Value)
{
    f32 Result = sinf(Value);
    return Result;
}

function f32
Cos(f32 Value)
{
    f32 Result = cosf(Value);
    return Result;
}

function f32
Tan(f32 Value)
{
    f32 Result = tanf(Value);
    return Result;
}

function f32
SquareRoot(f32 Value)
{
    f32 Result = sqrtf(Value);
    return Result;
}

//
// NOTE(philip): 2-Component Integer Vector
//

function iv2
IV2(s32 X, s32 Y)
{
    iv2 Result;
    Result.X = X;
    Result.Y = Y;

    return Result;
}

function iv2
operator-(iv2& Left, iv2& Right)
{
    iv2 Result = { };

    Result.X = Left.X - Right.X;
    Result.Y = Left.Y - Right.Y;

    return Result;
}

function iv2&
operator+=(iv2& Left, iv2& Right)
{
    Left.X += Right.X;
    Left.Y += Right.Y;

    return Left;
}

//
// NOTE(philip): 3-Component Floating-Point Vector
//

function v3
V3(f32 X, f32 Y, f32 Z)
{
    v3 Result = { };

    Result.X = X;
    Result.Y = Y;
    Result.Z = Z;

    return Result;
}

function f32
Length(v3 Vector)
{
    f32 Result = SquareRoot((Vector.X * Vector.X) + (Vector.Y * Vector.Y) + (Vector.Z * Vector.Z));
    return Result;
}

function v3
Normalize(v3 Vector)
{
    v3 Result = { };

    f32 Len = Length(Vector);
    Result.X = (Vector.X / Len);
    Result.Y = (Vector.Y / Len);
    Result.Z = (Vector.Z / Len);

    return Result;
}

function v3
Cross(v3 Left, v3 Right)
{
    v3 Result = { };

    Result.X = ((Left.Y * Right.Z) - (Left.Z * Right.Y));
    Result.Y = ((Left.Z * Right.X) - (Left.X * Right.Z));
    Result.Z = ((Left.X * Right.Y) - (Left.Y * Right.X));

    return Result;
}

function v3
operator*(v3& Vector, f32 Scalar)
{
    v3 Result = { };

    Result.X = Vector.X * Scalar;
    Result.Y = Vector.Y * Scalar;
    Result.Z = Vector.Z * Scalar;

    return Result;
}

function v3&
operator+=(v3& Left, v3& Right)
{
    Left.X += Right.X;
    Left.Y += Right.Y;
    Left.Z += Right.Z;

    return Left;
}

function v3&
operator-=(v3& Left, v3& Right)
{
    Left.X -= Right.X;
    Left.Y -= Right.Y;
    Left.Z -= Right.Z;

    return Left;
}

//
// NOTE(philip): Quaternion
//

function quat
Conjugate(quat Quaternion)
{
    quat Result = { };

    Result.X = -Quaternion.X;
    Result.Y = -Quaternion.Y;
    Result.Z = -Quaternion.Z;
    Result.W = Quaternion.W;

    return Result;
}

function quat
AxisAngleRotation(v3 Axis, f32 Angle)
{
    quat Result = { };

    f32 HalfAngle = (Angle / 2.0f);
    f32 HalfAngleSin = Sin(HalfAngle);
    f32 HalfAngleCos = Cos(HalfAngle);

    Axis = Normalize(Axis);

    Result.X = Axis.X * HalfAngleSin;
    Result.Y = Axis.Y * HalfAngleSin;
    Result.Z = Axis.Z * HalfAngleSin;
    Result.W = HalfAngleCos;

    return Result;
}

function quat
operator*(quat& Left, quat& Right)
{
    quat Result = { };

    Result.X = ((Left.W * Right.X) + (Left.X * Right.W) + (Left.Y * Right.Z) - (Left.Z * Right.Y));
    Result.Y = ((Left.W * Right.Y) - (Left.X * Right.Z) + (Left.Y * Right.W) + (Left.Z * Right.X));
    Result.Z = ((Left.W * Right.Z) + (Left.X * Right.Y) - (Left.Y * Right.X) + (Left.Z * Right.W));
    Result.W = ((Left.W * Right.W) - (Left.X * Right.X) - (Left.Y * Right.Y) - (Left.Z * Right.Z));

    return Result;
}

function v3
RotateV3(v3 Vector, quat Rotation)
{
    v3 Result = { };

    quat P = { };
    P.X = Vector.X;
    P.Y = Vector.Y;
    P.Z = Vector.Z;

    quat Conj = Conjugate(Rotation);
    quat RotatedP = ((Rotation * P) * Conj);

    Result.X = RotatedP.X;
    Result.Y = RotatedP.Y;
    Result.Z = RotatedP.Z;

    return Result;
}

//
// NOTE(philip): 4x4 Matrix
//

function m4
M4(f32 Diagonal = 0.0f)
{
    m4 Result = { };

    Result.Elements[0 + 0 * 4] = Diagonal;
    Result.Elements[1 + 1 * 4] = Diagonal;
    Result.Elements[2 + 2 * 4] = Diagonal;
    Result.Elements[3 + 3 * 4] = Diagonal;

    return Result;
}

function m4
Perspective(f32 AspectRatio, f32 VerticalFOV, f32 NearPlane, f32 FarPlane)
{
    m4 Result = { };

    f32 TanHalfVerticalFOV = Tan(VerticalFOV / 2.0f);
    f32 FrustumLength = (FarPlane - NearPlane);

    Result.Elements[0 + 0 * 4] = (1.0f / (AspectRatio * TanHalfVerticalFOV));
    Result.Elements[1 + 1 * 4] = (1.0f / TanHalfVerticalFOV);

    Result.Elements[2 + 2 * 4] = -((NearPlane + FarPlane) / FrustumLength);
    Result.Elements[3 + 2 * 4] = -1.0f;
    Result.Elements[2 + 3 * 4] = -((2.0f * NearPlane * FarPlane) / FrustumLength);

    return Result;
}

function m4
Translate(v3 Translation)
{
    m4 Result = M4(1.0f);

    Result.Elements[0 + 3 * 4] = Translation.X;
    Result.Elements[1 + 3 * 4] = Translation.Y;
    Result.Elements[2 + 3 * 4] = Translation.Z;

    return Result;
}

function m4
Scale(v3 Scale)
{
    m4 Result = M4(1.0f);

    Result.Elements[0 + 0 * 4] = Scale.X;
    Result.Elements[1 + 1 * 4] = Scale.Y;
    Result.Elements[2 + 2 * 4] = Scale.Z;

    return Result;
}

function m4
operator*(m4& Left, m4& Right)
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
                Result.Elements[Column + Row * 4] += Left.Elements[Index + Row * 4] *
                    Right.Elements[Column + Index * 4];
            }
        }
    }

    return Result;
}

function m4
ToM4(quat Quaternion)
{
    m4 Result = { };

    f32 XSquared = (Quaternion.X * Quaternion.X);
    f32 YSquared = (Quaternion.Y * Quaternion.Y);
    f32 ZSquared = (Quaternion.Z * Quaternion.Z);

    Result.Elements[0 + 0 * 4] = (1.0f - (2.0f * YSquared) - (2.0f * ZSquared));
    Result.Elements[1 + 0 * 4] = ((2.0f * Quaternion.X * Quaternion.Y) + (2.0f * Quaternion.W * Quaternion.Z));
    Result.Elements[2 + 0 * 4] = ((2.0f * Quaternion.X * Quaternion.Z) - (2.0f * Quaternion.W * Quaternion.Y));

    Result.Elements[0 + 1 * 4] = ((2.0f * Quaternion.X * Quaternion.Y) - (2.0f * Quaternion.W * Quaternion.Z));
    Result.Elements[1 + 1 * 4] = (1.0f - (2.0f * XSquared) - (2.0f * ZSquared));
    Result.Elements[2 + 1 * 4] = ((2.0f * Quaternion.Y * Quaternion.Z) + (2.0f * Quaternion.W * Quaternion.X));

    Result.Elements[0 + 2 * 4] = ((2.0f * Quaternion.X * Quaternion.Z) + (2.0f * Quaternion.W * Quaternion.Y));
    Result.Elements[1 + 2 * 4] = ((2.0f * Quaternion.Y * Quaternion.Z) - (2.0f * Quaternion.W * Quaternion.X));
    Result.Elements[2 + 2 * 4] = (1.0f - (2.0f * YSquared) - (2.0f * XSquared));

    Result.Elements[3 + 3 * 4] = 1.0f;

    return Result;
}
