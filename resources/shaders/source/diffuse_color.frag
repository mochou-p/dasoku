// dsk

#version 450

#ifdef GL_ES
precision mediump float;
#endif

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragUV;

layout (location = 2) in vec3 fragPosWorld;
layout (location = 3) in vec3 fragNormalWorld;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 0) uniform GlobalUbo
{
    mat4 projectionViewMatrix;
    vec4 ambientLightColor;
    vec3 lightPosition;
    vec4 lightColor;
} ubo;

layout (push_constant) uniform Push
{
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

layout(set = 1, binding = 0) uniform texture2D textures[5];
layout(set = 1, binding = 1) uniform sampler samp;

void main()
{
    vec3 directionToLight = ubo.lightPosition - fragPosWorld;
    float attenuation = 1.0 / dot(directionToLight, directionToLight);

    vec3 lightColor = ubo.lightColor.xyz * ubo.lightColor.w * attenuation;
    vec3 ambientLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
    vec3 diffuseLight = lightColor * max(dot(normalize(fragNormalWorld), normalize(directionToLight)), 0);

    vec3 light = (diffuseLight + ambientLight) * fragColor;

    vec4 tex = texture(sampler2D(textures[int(push.normalMatrix[3][0])], samp), fragUV);
    vec3 texColor = tex.xyz * (fragColor * vec3(1.5));

    if (push.normalMatrix[3][1] == 0.0f)
    {
        outColor = vec4(texColor * light, 1.0);
    }
    else
    {
        outColor = vec4(0.0, 1.0, 0.0, 1.0);
    }

    if (tex.w == 0.0f) { discard; }
}
