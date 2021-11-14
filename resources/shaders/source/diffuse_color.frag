// dsk

#version 450

#ifdef GL_ES
precision mediump float;
#endif

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragUV;

layout (location = 0) out vec4 outColor;

layout (push_constant) uniform Push
{
    mat4 modelMatrix;
    mat3 normalMatrix;
    int textureIndex;
} push;

layout(set = 1, binding = 0) uniform texture2D textures[3];
layout(set = 1, binding = 1) uniform sampler samp;

void main()
{
    vec3 texColor = texture(sampler2D(textures[push.textureIndex], samp), fragUV).xyz * (fragColor * vec3(1.5));
    if (push.textureIndex == 0)
    {
        texColor.x += 0.1;
        texColor.y += 0.1;
        texColor.z += 0.1;
    }

    outColor = vec4(texColor * fragColor, 1.0);
}
