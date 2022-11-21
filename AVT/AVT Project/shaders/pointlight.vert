#version 430

uniform mat4 m_pvm;
uniform mat4 m_viewModel;
uniform mat3 m_normal;
uniform mat4 m_Model;   //por causa do cubo para a skybox

uniform int texMode;
uniform vec4 l_pos;

in vec4 position;
in vec4 normal;    //por causa do gerador de geometria
in vec4 texCoord;
in vec4 tangent;

out Data {
	vec3 normal;
	vec3 eye;
	vec3 lightDir;
	vec2 tex_coord;
	vec3 skyboxTexCoord;
} DataOut;

void main () {

	vec3 n, t, b;
	vec3 lightDir, eyeDir;
	vec3 aux;

	DataOut.skyboxTexCoord = vec3(m_Model * position);	//Transformação de modelação do cubo unitário 
	DataOut.skyboxTexCoord.x = - DataOut.skyboxTexCoord.x; //Texturas mapeadas no interior logo negar a coordenada x

	vec4 pos = m_viewModel * position;

	n = normalize(m_normal * normal.xyz);

	// eye and light vectors in eye space
	eyeDir =  vec3(-pos);
	lightDir = vec3(l_pos - pos);

	if(texMode == 8)  {  //convert eye and light vectors to tangent space

		//Calculate components of TBN basis in eye space
		t = normalize(m_normal * tangent.xyz);  
		b = tangent.w * cross(n,t);

		aux.x = dot(lightDir, t);
		aux.y = dot(lightDir, b);
		aux.z = dot(lightDir, n);
		lightDir = normalize(aux);

		aux.x = dot(eyeDir, t);
		aux.y = dot(eyeDir, b);
		aux.z = dot(eyeDir, n);
		eyeDir = normalize(aux);
	}

	DataOut.normal = n;
	DataOut.lightDir = lightDir;
	DataOut.eye = eyeDir;
	DataOut.tex_coord = texCoord.st;

	gl_Position = m_pvm * position;	
}