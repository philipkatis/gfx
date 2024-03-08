#version 330 core

struct point_light
{
    vec3 Position;
    vec3 Color;
    float Intensity;
};

struct material
{
    float UseDiffuseMap;
    vec4 BaseColor;

    float IsLit;
    float Ambient;
    float Diffuse;
    float Specular;
};

in vec4 v_WorldSpacePosition;
in vec2 v_TextureCoordinate;
in vec3 v_WorldSpaceNormal;

uniform vec3 CameraPosition;

uniform vec3 AmbientLightColor;
uniform float AmbientLightIntensity;

uniform point_light Light;
uniform material Material;

uniform sampler2D DiffuseMap;

layout (location = 0) out vec4 PixelColor;

void
main(void)
{
    vec3 Position = vec3(v_WorldSpacePosition);
    vec3 Normal = normalize(v_WorldSpaceNormal);

    vec3 ViewDirection = normalize(CameraPosition - Position);

    vec3 Ambient = Material.Ambient * AmbientLightColor * AmbientLightIntensity;
    vec3 Lighting = Ambient;

    int LightCount = 1;
    for (int Index = 0;
         Index < LightCount;
         ++Index)
    {
        vec3 LightDirection = normalize(Light.Position - Position);

        float NdotL = max(dot(Normal, LightDirection), 0.0);
        float VdotR = max(dot(-ViewDirection, reflect(LightDirection, Normal)), 0.0);

        vec3 Diffuse = NdotL * Material.Diffuse * Light.Color * Light.Intensity;
        vec3 Specular = VdotR * Material.Specular * Light.Color * Light.Intensity;

        Lighting += Diffuse + Specular;
    }

    vec4 BaseColor = ((1.0 - Material.UseDiffuseMap) * Material.BaseColor) + (Material.UseDiffuseMap * texture(DiffuseMap, v_TextureCoordinate));
    vec3 FinalColor = (((1.0 - Material.IsLit) * BaseColor.rgb) + (Material.IsLit * BaseColor.rgb * Lighting));

    PixelColor = vec4(FinalColor, BaseColor.a);
}
