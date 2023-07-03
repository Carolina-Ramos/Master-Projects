#version 430

uniform	sampler2D texUnitDiff;
uniform	sampler2D texUnitDiff1;
uniform	sampler2D texUnitSpec;
uniform	sampler2D texUnitNormalMap;
uniform bool normalMap;  //for normal mapping
uniform bool specularMap;
uniform uint diffMapCount;

uniform sampler2D texmap;
uniform sampler2D texmap1;
uniform sampler2D texmap2;
uniform sampler2D texmap3;
uniform sampler2D texmap4;
uniform sampler2D texmap5;
uniform sampler2D texmapsky;
uniform samplerCube cubeMap;

uniform int texMode;
uniform bool shadowMode;

out vec4 colorOut;

uniform mat4 m_View;

struct Materials {
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	vec4 emissive;
	float shininess;
	int texCount;
};

struct PointLight {
	vec4 position;
	float range;
	bool enabled;
};

struct SpotLight {
	vec4 position;
	vec4 direction;
	float range;
	float angle;
	bool enabled;
};

struct DirLight {
	vec4 direction;
	bool enabled;
};

uniform Materials mat;

#define NUM_POINT_LIGHTS 6
#define NUM_SPOT_LIGHTS 2

uniform PointLight pointLight[NUM_POINT_LIGHTS];
uniform SpotLight spotLight[NUM_SPOT_LIGHTS];
uniform DirLight dirLight;

uniform bool fogEnabled;
uniform vec3 fogCenter;

in Data {
	vec3 normal;
	vec3 eye;
	vec3 lightDir;
	vec2 tex_coord;
	vec3 skyboxTexCoord;
} DataIn;

vec4 calcPointLight(PointLight light, vec3 n, vec3 e, vec3 pos, vec4 texel, vec4 texel1) {
	if (!light.enabled) return vec4(0.0);
	vec3 l = normalize(vec3(light.position) - pos);

	float dist = length(vec3(light.position) - pos);
	float linearAtt = 0.3 / light.range;
	float quadAtt = 5.0 / (light.range * light.range);
	float attenuation = 1.0 / (1.0 + (linearAtt * dist) + (quadAtt * dist * dist));

	vec4 spec = vec4(0.0);
	float intensity = max(dot(n,l), 0.0);
	if (intensity > 0.0) {
		vec3 h = normalize(l + e);
		float intSpec = max(dot(h,n), 0.0);
		spec = mat.specular * pow(intSpec, mat.shininess);
	}

	return attenuation * max(intensity * mat.diffuse * texel * texel1 + spec, mat.ambient * texel * texel1);
}

vec4 calcSpotLight(SpotLight light, vec3 n, vec3 e, vec3 pos, vec4 texel, vec4 texel1) {
	if (!light.enabled) return vec4(0.0);
	vec3 l = normalize(vec3(light.position) - pos);
	vec3 sd = normalize(vec3(-light.direction));

	float spotCos = dot(l, sd);
	float lightCos = cos(light.angle);
	if (spotCos > lightCos) {
		float dist = length(vec3(light.position) - pos);
		float linearAtt = 0.05 / light.range;
		float quadAtt = 0.9 / (light.range * light.range);
		float attenuation = 1.0 / (1.0 + (linearAtt * dist) + (quadAtt * dist * dist));
		float angleAtt = min((spotCos -  lightCos) * 3.0, 1.0);

		vec4 spec = vec4(0.0);
		float intensity = max(dot(n,l), 0.0);
		if (intensity > 0.0) {
			vec3 h = normalize(l + e);
			float intSpec = max(dot(h,n), 0.0);
			spec = mat.specular * pow(intSpec, mat.shininess);
		}

		return attenuation * angleAtt * max(intensity * mat.diffuse * texel * texel1 + spec, mat.ambient * texel * texel1);
	}

	return vec4(0.0);
}

vec4 calcDirLight(DirLight light, vec3 n, vec3 e, vec4 texel, vec4 texel1) {

	if (!light.enabled) return vec4(0.0); 
	vec3 l = normalize(-vec3(dirLight.direction));

	vec4 spec = vec4(0.0);
	float intensity = max(dot(n,l), 0.0);

	if (intensity > 0.0) {

		vec3 h = normalize(l + e);
		float intSpec = max(dot(h,n), 0.0);
		spec = mat.specular * pow(intSpec, mat.shininess);
	}
	
	return max(intensity * mat.diffuse * texel * texel1 + spec, mat.ambient * texel * texel1);
}

void main() {

	vec3 n;

	if (shadowMode)
		colorOut = vec4(0.5, 0.5, 0.5, 1.0);
	else {
		n = normalize(DataIn.normal);
		vec3 l = normalize(DataIn.lightDir);
		vec3 e = normalize(DataIn.eye);

		vec4 texel;
		vec4 texel1;
		float opacity =  mat.diffuse.a;

		if (texMode == 0) {  //Mutli-Texturing
			texel = texture(texmap, DataIn.tex_coord);
			texel1 = texture(texmap3, DataIn.tex_coord);
		}
		else if (texMode == 1) { // Texture of rocks
			texel = texture(texmap1, DataIn.tex_coord);
			texel1 =  vec4(1.0);
		}
		else if (texMode == 2) {  // Texture of rover
			texel = texture(texmap2, DataIn.tex_coord);
			texel1 =  vec4(1.0);
		}
		else if (texMode == 3) { //assimp objs
			if(diffMapCount == 0) {
				texel = vec4(1.0);
				texel1 =  vec4(1.0);
			} else if(diffMapCount == 1) {
				texel = texture(texUnitDiff, DataIn.tex_coord);
				texel1 = vec4(1.0);
			} else {
				texel = texture(texUnitDiff, DataIn.tex_coord);
				texel1 = texture(texUnitDiff1, DataIn.tex_coord);
			}
		}
		else if (texMode == 5) {  // Texture of rocks (billboard)
			texel = texture(texmap4, DataIn.tex_coord);
			texel1 =  vec4(1.0);
			opacity = texel.a;
			if (opacity == 0.0) discard;
		}
		else if (texMode == 8){ // lookup normal from normal map, move from [0,1] to [-1, 1] range, normalize
			n = normalize(2.0 * texture(texUnitNormalMap, DataIn.tex_coord).rgb - 1.0);
			texel = texture(texmap1, DataIn.tex_coord);  // texel from rocks
			texel1 =  vec4(1.0);
			//colorOut = vec4((max(intensity*texel + spec, 0.2*texel)).rgb, 1.0);
		} else if (texMode == 9) {
			vec3 reflected = vec3 (transpose(m_View) * vec4 (vec3(reflect(-e, n)), 0.0)); //reflection vector in world coord
			reflected.x = -reflected.x;
			texel = texture(cubeMap, reflected);
			//texel = vec4(1.0);
			texel1 =  vec4(1.0);
		} else {
			texel = vec4(1.0);
			texel1 =  vec4(1.0);
		}
		
		vec4 finalColor =  vec4(0.0);

		for (int i = 0; i < NUM_POINT_LIGHTS; i++) 
			finalColor += calcPointLight(pointLight[i], n, e, -DataIn.eye, texel, texel1);
		for (int i = 0; i < NUM_SPOT_LIGHTS; i++) 
			finalColor += calcSpotLight(spotLight[i], n, e, -DataIn.eye, texel, texel1);
		finalColor += calcDirLight(dirLight, n, e, texel, texel1);

		colorOut = finalColor;

		if (texMode == 7) { //SkyBox
			texel = texture(cubeMap, DataIn.skyboxTexCoord);

			opacity = 1.f;
			colorOut = vec4(texel.rgb, 1.f);
		}

		if (fogEnabled) {
			// Fog
			float fogMaxDist = 20.0;
			float fogMinDist = 2.0;
			vec4 fogColor = vec4(0.0, 0.0, 0.0, 1.0);

			float fragDist = length(fogCenter + DataIn.eye);
			float fogFactor = 1.0 - ((fogMaxDist - fragDist) / (fogMaxDist - fogMinDist));
			fogFactor = clamp( pow(fogFactor, 1.5), 0.0, 1.0);
		
			colorOut = mix(colorOut, fogColor, fogFactor);
		}

		colorOut = vec4(colorOut.rgb, opacity);

		if (texMode == 6) {  // Texture of particles
			texel = texture(texmap5, DataIn.tex_coord);
			if((texel.a == 0.0)  || (mat.diffuse.a == 0.0) ) discard;
			else
				colorOut = mat.diffuse * texel;
		}
	}
}