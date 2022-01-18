#version 410

layout (location=0) in vec4 vertex;
layout (location=1) in vec4 icolor;

out vec4 vcolor;

uniform mat4 mvp;

void main (void)
{
  vcolor = icolor;
  gl_Position = mvp * vertex;
}

