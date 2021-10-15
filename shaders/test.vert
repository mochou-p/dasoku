// zzz

#version 450

#ifdef GL_ES
precision mediump float;
#endif

layout (location = 0) in vec2 position;
layout (location = 1) out vec2 pos;

void main()
{
    gl_Position = vec4(position, 0.0, 1.0);
    pos = position;
}
