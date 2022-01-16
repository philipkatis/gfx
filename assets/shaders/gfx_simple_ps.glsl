#version 330 core

// TODO(philip): Move all inputs to a struct.
// TODO(philip): Either use a more modern version of OpenGL or use the ARB_separate_shader_objects extension
// to specify a binding location for these.
in vec2 v_TextureCoordinate;
in vec3 v_Normal;

uniform vec3 CameraDirection;
uniform sampler2D Texture;

layout (location = 0) out vec4 PixelColor;

void main()
{
    // TODO(philip): Load this from a light.
    vec3 LightDirection = vec3(1.0, -1.0, -1.0);
    vec4 AmbientLightColor = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 DiffuseLightColor = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 SpecularLightColor = vec4(1.0, 1.0, 1.0, 1.0);

    // TODO(philip): Load this from a material.
    float AmbientReflectionConstant = 0.1;
    float DiffuseReflectionConstant = 0.6;
    float SpecularReflectionConstant = 0.3;

    vec3 PixelToLight = normalize(-LightDirection);
    vec3 PixelToCamera = normalize(-CameraDirection);
    vec3 Normal = normalize(v_Normal);
    vec3 ReflectedLightDirection = reflect(LightDirection, Normal);

    float DotLN = clamp(dot(PixelToLight, Normal), 0.0, 1.0);
    float DotRV = clamp(dot(PixelToCamera, ReflectedLightDirection), 0.0, 1.0);

    vec4 AmbientLight = AmbientLightColor * AmbientReflectionConstant;
    vec4 DiffuseLight = DiffuseLightColor * DiffuseReflectionConstant * DotLN;
    vec4 SpecularLight = SpecularLightColor * SpecularReflectionConstant * DotRV;

    PixelColor = AmbientLight + DiffuseLight + SpecularLight;

    PixelColor = texture2D(Texture, v_TextureCoordinate);
}
