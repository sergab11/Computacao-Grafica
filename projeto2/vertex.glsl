#version 410

layout (location=0) in vec4 pos;
layout (location=1) in vec3 normal;
layout (std140) uniform Material {
	vec4 amb;
	vec4 dif;
	vec4 spe;
	float shi;
} mat;
layout (std140) uniform Lights {
	vec4 pos;
	vec3 dir;
	float abert;
} spot;

const float Ka = 0.2f;

uniform vec4 aleye;	//ambient light pos in eye space
uniform mat4 mv;
uniform mat4 nm;
uniform mat4 mvp;

uniform vec4 scale;	
uniform vec4 translate;

out mydata{
	vec3 neye;
	vec3 light;
	vec3 veye;
} v;

void main (void)
{
	v.veye = vec3(mv*pos);
	vec4 pos_final = pos;

	pos_final.x += translate.x;	pos_final.x *= scale.x;
	pos_final.y += translate.y;	pos_final.y *= scale.y;
	pos_final.z += translate.z;	pos_final.z *= scale.z;
	
	if(aleye.w == 0)
		v.light = normalize(vec3(aleye));
	else
		v.light = normalize(vec3(aleye)-v.veye);
	v.neye = normalize(vec3(nm*vec4(normal, 0.0f)));
	
  gl_Position = mvp*pos_final;
}
