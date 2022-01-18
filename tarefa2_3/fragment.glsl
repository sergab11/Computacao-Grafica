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

const float Ka = 0.2f;

uniform vec4 leye;	//light pos in eye space
uniform mat4 mv;
uniform mat4 nm;
uniform mat4 mvp;

void main (void)
{
	vec3 N = normalize(f.neye);
	vec3 V = normalize(f.veye);
	vec3 L = normalize(f.light);

	vec4 color;
	float ndotl = dot(N, L);
	color = mat.amb*Ka + mat.dif*max(0, ndotl);
	if(ndotl>0){
		vec3 refl = normalize(reflect(-L, N));
		color += mat.spe*pow(max(0, dot(refl, normalize(-V))), mat.shi);
	}
	gl_FragColor = color;
}
