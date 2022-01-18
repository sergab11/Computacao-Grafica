# version 410
layout (location = 0) in vec3 rpos;

uniform mat4 mvp;

out vec3 dir;

void main(){
	vec4 pos = vec4(rpos, 1.0f);
	dir = vec3(pos);
	gl_Position = mvp * pos;
}
