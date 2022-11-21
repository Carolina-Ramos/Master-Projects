#version 330

uniform sampler2D texmap;

out vec4 colorOut;

struct Materials {
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	vec4 emissive;
	float shininess;
	int texCount;
};
uniform Materials mat;


in Data {
	vec3 normal;
	vec3 eye;
	vec3 lightDir;
	vec2 tex_coord;
} DataIn;

void main() {
	vec4 texel; 

	texel = texture(texmap, DataIn.tex_coord);
	if((texel.a == 0.0)  || (mat.diffuse.a == 0.0) ) discard;
	else
		colorOut = mat.diffuse * texel;
}