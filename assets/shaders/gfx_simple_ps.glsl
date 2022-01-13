#version 330 core

// TODO(philip): Move all inputs to a struct.
// TODO(philip): Either use a more modern version of OpenGL or use the ARB_separate_shader_objects extension
// to specify a binding location for these.
in vec2 v_TextureCoordinate;
in vec3 v_Normal;

layout (location = 0) out vec4 Color;

void main()
{
    vec3 Normal = normalize(v_Normal);

    Color = vec4(v_Normal + 0.5, 1.0);
}
