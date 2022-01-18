#version 410

layout (location=0) in vec4 pos;
layout (location=1) in vec3 normal;
layout (location=2) in vec4 md;

const float Ka = 0.2f;
const vec4 ma = vec4(1.0f, 1.0f, 1.0f, 1.0f);
const vec4 ms = vec4(1.0f, 1.0f, 1.0f, 1.0f);
const float shi = 128.0f;

uniform vec4 leye;	//light pos in eye space
uniform mat4 mv;
uniform mat4 nm;
uniform mat4 mvp;
uniform vec4 scale;	
uniform vec4 translate;

out vec4 color;

void main (void)
{
	vec3 veye = vec3(mv*pos);
	vec3 light;
	vec4 pos_final = pos;

	pos_final.x += translate.x;	pos_final.x *= scale.x;
	pos_final.y += translate.y;	pos_final.y *= scale.y;
	pos_final.z += translate.z;	pos_final.z *= scale.z;
	
	if(leye.w == 0)
		light = normalize(vec3(leye));
	else
		light = normalize(vec3(leye)-veye);
	vec3 neye = normalize(vec3(nm*vec4(normal, 0.0f)));
	float ndotl = dot(neye, light);
	color = ma*Ka + md*max(0, ndotl);
	if(ndotl>0){
		vec3 refl = normalize(reflect(-light, neye));
		color += ms*pow(max(0, dot(refl, normalize(-veye))), shi);	
	}
  gl_Position = mvp*pos_final;
}
