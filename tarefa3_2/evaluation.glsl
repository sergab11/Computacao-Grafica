#version 410
layout (quads) in;
#define pi 3.14159265

const vec4 leye = vec4(0.0f, 0.0f, 0.0f, 1.0f);

uniform mat4 mv;
uniform mat4 nm;
uniform mat4 mvp;

patch in vec3 pos;
patch in float radius;

out data{
	vec3 neye;
	vec3 veye;
	vec3 light;
	vec2 texcoord;
} v;

void main(void){
	float theta = 2*pi*gl_TessCoord.x;
	float phi = pi*gl_TessCoord.y;
	vec4 vpos;

	vpos.x = pos.x + radius*sin(theta)*sin(phi);
	vpos.y = pos.y + radius*cos(phi);
	vpos.z = pos.z + radius*cos(theta)*sin(phi);
	vpos.w = 1.0f;
	v.veye = vec3(mv*vpos);
	
	if(leye.w == 0)
		v.light = normalize(vec3(leye));
	else
		v.light = normalize(vec3(leye)-v.veye);
	v.neye = normalize(vec3(nm*(vpos-vec4(pos, 1.0f))));
	v.texcoord = vec2(gl_TessCoord);
	gl_Position = mvp*vpos;
}
