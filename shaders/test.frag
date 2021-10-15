// zzz

#version 450

#ifdef GL_ES
precision mediump float;
#endif

layout (location = 0) out vec4 outColor;
layout (location = 1) in vec2 pos;


void main()
{
    outColor = vec4(
        abs(mod(pos.x*3.0*(pos.x*2.0+pos.y*3.0), pos.y))*2.0,
        abs(mod(abs(pos.x*3.0)*(pos.x*2.0+pos.y*3.0), pos.y))*2.0,
        abs(mod(pos.x*3.0*(pos.x*2.0+pos.y*3.0), abs(pos.y)))*2.0,
        // 0.0, // abs((abs(pos.x)*pos.y)/(abs(pos.x)-pos.y)),
        // 0.0, // abs((pos.x*abs(pos.y))/(pos.x+abs(pos.y))),
        1.0
    );
}
