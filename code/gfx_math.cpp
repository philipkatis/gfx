//
// NOTE(philip): Base Functions
//

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

function f32
ToRadians(f32 Value)
{
    f32 Result = (Value * (PI / 180.0f));
    return Result;
}

//
// NOTE(philip): 3-Component Vector
//

function v3
V3(float X, float Y, float Z)
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

//
// NOTE(philip): Quaternion
//

function quat
AxisAngleRotate(v3 Axis, f32 Angle)
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
