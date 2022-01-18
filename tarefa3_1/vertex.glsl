#version 410
layout (location=0) in vec4 pos;
layout (location=1) in vec3 normal;
layout (location=2) in vec3 tangent;
layout (location=3) in vec2 texcoord;

uniform vec4 leye;	//ambient light pos in eye space
uniform mat4 inv;		//inverse of viewer matrix	
uniform mat4 mvp;

out mydata{
	vec3 ltan;	//light vector in tangent space
	vec3 vtan;	//viewer vector in tangent space	
	vec2 texcoord;
} v;

void main (void)
{
	//v.veye = vec3(mv*pos);
	vec3 t = normalize(tangent);
	vec3 n = normalize(normal);
	vec3 b = normalize(cross(normal, tangent));

	mat3 base = transpose(mat3(t, b, n));
	vec4 eye = inv*vec4(0.0f, 0.0f, 0.0f, 1.0f);
	v.vtan = base*normalize(vec3(eye-pos));
	vec4 lobj = inv*leye;
	v.ltan = base*normalize(vec3(lobj-pos));
	v.texcoord = texcoord;
	
  gl_Position = mvp*pos;
}
