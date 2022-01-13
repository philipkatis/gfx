#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TextureCoordinate;
layout (location = 2) in vec3 Normal;

uniform mat4 ViewProjection;
uniform mat4 Transform;

// TODO(philip): Move all outputs to a struct.
// TODO(philip): Either use a more modern version of OpenGL or use the ARB_separate_shader_objects extension
// to specify a binding location for these.
out vec2 v_TextureCoordinate;
out vec3 v_Normal;

void main()
{
    v_TextureCoordinate = TextureCoordinate;
    v_Normal = mat3(Transform) * Normal;

    gl_Position = ViewProjection * Transform * vec4(Position, 1.0);
}
