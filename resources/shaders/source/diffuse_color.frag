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
    mat4 normalMatrix;
} push;

layout(set = 1, binding = 0) uniform texture2D textures[5];
layout(set = 1, binding = 1) uniform sampler samp;

void main()
{
    vec3 texColor = texture(sampler2D(textures[int(push.normalMatrix[3][0])], samp), fragUV).xyz * (fragColor * vec3(1.5));

    outColor = vec4(texColor * fragColor, 1.0);
}
