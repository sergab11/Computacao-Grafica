#version 410

in mydata{
	vec3 neye;
	vec3 light;
	vec3 veye;
} f;

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

uniform vec4 aleye;	//light pos in eye space
uniform mat4 mv;
uniform mat4 nm;
uniform mat4 mvp;

void main (void)
{
	vec3 N = normalize(f.neye);
	vec3 V = normalize(f.veye);
	vec3 L = normalize(f.light);

	vec3 S;
	float intensity = 128.0f;
	float k = 0.5f;

	if(spot.pos.w == 0.0)
		S = normalize(vec3(spot.pos));
	else
		S = normalize(vec3(spot.pos)-f.veye);

	vec4 color;
	float ndotl = dot(N, L);
	float ndots = dot(N, S);
	//k = pow(max(ndots, 0), intensity);	
	
	if(ndotl>0){
		vec3 refl = normalize(reflect(-L, N));
		color = mat.amb*Ka + k*(max(0, ndots)) + mat.dif*max(0, ndotl) + mat.spe*pow(max(0, dot(refl, normalize(-V))), mat.shi);
	}
	else{
		color = mat.amb*Ka + k*(max(0, ndots)) + mat.dif*max(0, ndotl);
	}
	gl_FragColor = color;
}
