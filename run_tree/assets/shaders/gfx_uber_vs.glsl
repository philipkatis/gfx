#version 330 core

layout (location = 0) in vec3 ModelSpacePosition;
layout (location = 1) in vec2 TextureCoordinate;
layout (location = 2) in vec3 ModelSpaceNormal;

uniform mat4 ViewProjection;
uniform mat4 Transform;

out vec4 v_WorldSpacePosition;
out vec2 v_TextureCoordinate;
out vec3 v_WorldSpaceNormal;

void
main(void)
{
    v_WorldSpacePosition = Transform * vec4(ModelSpacePosition, 1.0);
    v_TextureCoordinate = TextureCoordinate;
    v_WorldSpaceNormal = mat3(Transform) * ModelSpaceNormal;

    gl_Position = ViewProjection * v_WorldSpacePosition;
}
