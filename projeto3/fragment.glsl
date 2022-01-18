#version 410

const float Ka = 0.5f;
const vec4 ma = vec4(0.8f, 0.8f, 0.8f, 1.0f);
const vec4 md = vec4(0.7f, 0.7f, 0.7f, 1.0f);
const vec4 ms = vec4(0.5f, 0.5f, 0.5f, 1.0f);
const float shi = 64.0f;

const float f_bias = 0.2;
const float f_scale = 2.0;
const float f_power = 1.5;

in mydata{
	vec3 ltan;	//light vector in tangent space
	vec3 vtan;	//viewer vector in tangent space
	vec2 texcoord;

	vec3 viewer;
	vec3 normal;
	vec3 light;
} f;

uniform sampler2D golfball;
uniform sampler2D normals;
uniform samplerCube sky;

out vec4 color;

void main (void)
{
	vec3 vnorm = normalize(f.vtan);
	vec3 nnorm = 2*vec3(texture(normals, f.texcoord))-1;
	vec3 lnorm = normalize(f.ltan);

	float ndotl = dot(nnorm, lnorm);
	color = (ma*Ka + md*max(0, ndotl))*texture(golfball, f.texcoord);
	
	vec3 viewer = normalize(f.viewer);
	vec3 normal = normalize(f.normal);
	vec3 light = normalize (f.light);
	float dotr = dot(viewer, normal);
	float reflCoef = max(0, min(1, f_bias + f_scale*pow((1-dotr), f_power)));
	float refrCoef = 1.0f-reflCoef;
	vec3 reflDir = normalize(reflect(viewer, normal)*vec3(1 ,1 , -1));
	vec3 refrDir = normalize(refract(viewer, normal, 1.1)*vec3(1 ,1 , -1));
	vec4 texcolor = reflCoef*texture(sky, reflDir) + refrCoef*texture(sky, refrDir);	
	color = (ma*Ka + (md*max(0, ndotl))*texcolor);
	ndotl = dot(normal,light);
	if(ndotl>0){
		vec3 refl = normalize(reflect(-light, normal));
		color += ms*pow(max(0, dot(refl, viewer)), shi);
	}
}
