#version 410
const float Ka = 0.7f;
const vec4 ma = vec4(0.8f, 0.8f, 0.8f, 1.0f);
const vec4 md = vec4(1.0f, 1.0f, 1.0f, 1.0f);
const vec4 ms = vec4(0.5f, 0.5f, 0.5f, 1.0f);
const float shi = 64.0f;

in data{
	vec3 neye;
	vec3 veye;
	vec3 light;
	vec2 texcoord;
} f;

uniform sampler2D golfball;

out vec4 color;

void main (void){
	vec3 vnorm = normalize(-f.veye);
	vec3 nnorm = normalize(f.neye);
	vec3 lnorm = normalize(f.light);

	float ndotl = dot(nnorm, lnorm);
	
	color = (ma*Ka+md*max(0, ndotl))*texture(golfball, f.texcoord);
	if(ndotl>0){
		vec3 refl = normalize(reflect(-lnorm, nnorm));
		color += ms*pow(max(0, dot(refl, vnorm)), shi);
	}
}
