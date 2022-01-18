#version 410

layout (location=0) in vec3 rpos;
uniform mat4 mvp;

out vec3 dir;

void main (void)
{
	dir = rpos;
	vec4 pos = vec4(rpos, 1.0f);
	
  gl_Position = mvp*pos;
}
