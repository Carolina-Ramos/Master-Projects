//
// AVT: Phong Shading and Text rendered with FreeType library
// The text rendering was based on https://learnopengl.com/In-Practice/Text-Rendering
// This demo was built for learning purposes only.
// Some code could be severely optimised, but I tried to
// keep as simple and clear as possible.
//
// The code comes with no warranties, use it at your own risk.
// You may use it, or parts of it, wherever you want.
// 
// Author: Jo„o Madeiras Pereira
//
//	Carolina Ramos ist193694
//  Duarte Boto    ist192457
//  Marta Vicente  ist196894

#include <math.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <chrono>

// include GLEW to access OpenGL 3.3 functions
#include <GL/glew.h>


// GLUT is the toolkit to interface with the OS
#include <GL/freeglut.h>

#include <IL/il.h>

// assimp include files. These three are usually needed.
#include "assimp/Importer.hpp"	//OO version Header!
#include "assimp/scene.h"


// Use Very Simple Libs
#include "VSShaderlib.h"
#include "AVTmathLib.h"
#include "VertexAttrDef.h"
#include "geometry.h"
#include "Texture_Loader.h"

#include "avtFreeType.h"

#include "Rover.h"
#include "Rocks.h"
#include "StaticRocks.h"
#include "Camera.h"
#include "Light.h"
#include "Beacon.h"
#include "ObjectTransp.h"
#include "Box.h"
#include "LensFlare.h"
#include "meshFromAssimp.h"
#include "Billboard.h"
#include "MirrorSphere.h"

using namespace std;

#define CAPTION "Lives"

#define frand()			((float)rand()/RAND_MAX)
#define M_PI			3.14159265
#define MAX_PARTICULAS  600

int WindowHandle = 0;
int WinX = 1024, WinY = 768;

unsigned int FrameCount = 0;

//shaders
VSShaderLib shader;  //geometry
VSShaderLib shaderText;  //render bitmap text
VSShaderLib shaderFlare;  //render bitmap text

//File with the font
const string font_name = "fonts/arial.ttf";

//Vector with meshes
vector<struct MyMesh> myMeshes;
vector<struct MyMesh> myMeshesAssimp;

//External array storage defined in AVTmathLib.cpp

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

GLint pvm_uniformId;
GLint vm_uniformId;
GLint model_uniformId;
GLint view_uniformId;
GLint normal_uniformId;
GLint lPos_uniformId;
GLint normalMap_loc;
GLint specularMap_loc;
GLint diffMapCount_loc;

// texture Variables
GLint tex_loc, tex_loc1, tex_loc2, tex_loc3, tex_loc4, tex_loc5, tex_cube_loc, tex_loc_sky, texUnitNormalMap_loc;
GLint texMode_uniformId;
GLuint TextureArray[9];

GLint shadowMode_uniformId;

GLint flare_pvm_uniformId;
GLint flare_vm_uniformId;
GLint flare_normal_uniformId;
GLint flare_tex_loc;

// Camera Position
float camX, camY, camZ;

// Mouse Tracking Variables
int startX, startY, tracking = 0;

// Camera Spherical Coordinates
float alpha = -90.0f, beta = 30.0f;
float r = 10.0f;
float currentAlpha = alpha;
float currentBeta = beta;
float currentR = r;

// Frame counting and FPS computation
long myTime, timebase = 0, frame = 0;
char s[32];
float lightPos[4] = { 4.0f, 6.0f, 2.0f, 1.0f };

float _time = 30.0f;
float deltaTime = 0.010f;
std::chrono::time_point<std::chrono::system_clock> lastTime;

int rocksNumber = 4;
int staticRocksNum = 4;
int lives = 5;
int points = 0;

bool fogEnabled = true;
bool flareEnabled = true;
bool paused = false;
bool gameOver = false;

#define NUM_POINT_LIGHTS 6
#define NUM_SPOT_LIGHTS 2

Rover rover;
Rocks rocks[30];
StaticRocks staticRocks[10];
Camera camera[4];
Beacon beacons[NUM_POINT_LIGHTS];
ObjectTransp dome;
int activeCamera = 2;
Billboard billboard;
MirrorSphere environmentSphere;

PointLight pointLights[NUM_POINT_LIGHTS];
SpotLight spotLights[NUM_SPOT_LIGHTS];
DirLight dirLight;

std::vector<MinMaxBox> _positions;

LensFlare lensFlare;

//particles

float angle = 0.0, deltaAngle = 0.0, ratio;
float x = 0.0f, y = 1.75f, z = 10.0f;
float lx = 0.0f, ly = 0.0f, lz = -1.0f;

int deltaMove = 0, deltaUp = 0, type = 0;
int fireworks = 0;

typedef struct {
	float	life;		// vida
	float	fade;		// fade
	float	r, g, b;    // color
	GLfloat x, y, z;    // posição
	GLfloat vx, vy, vz; // velocidade 
	GLfloat ax, ay, az; // aceleração
} Particle;

Particle particula[MAX_PARTICULAS];
int dead_num_particles = 0;

/* IMPORTANT: Use the next data to make this Assimp demo to work*/
// Created an instance of the Importer class in the meshFromAssimp.cpp file
extern Assimp::Importer importer;
// the global Assimp scene object
extern const aiScene* scene; 
char model_dir[50] = "alien";  //initialized by the user input at the console
bool normalMapKey = TRUE; // by default if there is a normal map then bump effect is implemented. press key "b" to enable/disable normal mapping
extern float scaleFactor; //HERE
int windowWidth, windowHeight;
int firstRender = 1;

void timer(int value)
{
	std::ostringstream oss;
	oss << CAPTION << ": " << lives << " Points " << points;
	std::string s = oss.str();
	glutSetWindow(WindowHandle);
	glutSetWindowTitle(s.c_str());
	FrameCount = 0;
	glutTimerFunc(1000, timer, 0);
}

// ------------------------------------------------------------
//
// Reshape Callback Function
//

void changeSize(int w, int h) {
	windowWidth = w;
	windowHeight = h;
	float ratio;
	// Prevent a divide by zero, when window is too short
	if (h == 0)
		h = 1;
	// set the viewport to be the entire window
	glViewport(0, 0, w, h);
	// set the projection matrix
	loadIdentity(PROJECTION);

	//STARTING STENCIL
	// load identity matrices for Model-View
	loadIdentity(VIEW);
	loadIdentity(MODEL);

	glUseProgram(shader.getProgramIndex());
	//n„o vai ser preciso enviar o material pois o cubo n„o È desenhado
	scale(MODEL, 0.01, 0.01, 1);
	translate(MODEL, 85.0f, -85, -0.5f);
	// send matrices to OGL
	computeDerivedMatrix(PROJ_VIEW_MODEL);
	glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
	computeNormalMatrix3x3();
	glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

	glClear(GL_STENCIL_BUFFER_BIT);

	int m_viewport[4] = { 0, 0, w, h };
	camera[activeCamera].camProjection(m_viewport);
}

void reset() {
	rover.reset();

	if (!gameOver) {
		for (int i = 0; i < rocksNumber; i++) {
			rocks[i].reset();
		}
	}
	for (int d = 0; d < staticRocksNum; d++) {
		staticRocks[d].reset();
	}
	staticRocks[0].createStaticRocks(30, -8);
	staticRocks[1].createStaticRocks(25, -45);
	staticRocks[2].createStaticRocks(-15, 1);
	staticRocks[3].createStaticRocks(-10, -20);

	if (!gameOver) {
		lives = 5;
		points = 0;
	}
	fogEnabled = true;
	if (!dirLight.getEnabled()) dirLight.lightSwitch();
	for (int i = 0; i < NUM_POINT_LIGHTS; i++) if (!pointLights[i].getEnabled()) pointLights[i].lightSwitch();
	for (int i = 0; i < NUM_SPOT_LIGHTS; i++) if (!spotLights[i].getEnabled()) spotLights[i].lightSwitch();
	lastTime = std::chrono::system_clock::now();
}

void updateParticles()
{
	int i;
	float h;

	/* MÈtodo de Euler de integraÁ„o de eq. diferenciais ordin·rias
	h representa o step de tempo; dv/dt = a; dx/dt = v; e conhecem-se os valores iniciais de x e v */

	h = 0.033;
	if (fireworks) {
		for (i = 0; i < MAX_PARTICULAS; i++)
		{
			particula[i].x += (h * particula[i].vx);
			particula[i].y += (h * particula[i].vy);
			particula[i].z += (h * particula[i].vz);
			particula[i].vx += (h * particula[i].ax);
			particula[i].vy += (h * particula[i].ay);
			particula[i].vz += (h * particula[i].az);
			particula[i].life -= particula[i].fade;
			if (particula[i].y < 0) particula[i].life = 0;
		}
	}
}

void initParticles(Rocks rock) {
	GLfloat v, theta, phi;
	int i;

	for (i = 0; i < MAX_PARTICULAS; i++)
	{
		v = 0.8 * frand() + 0.2;
		phi = frand() * M_PI;
		theta = 2.0 * frand() * M_PI;

		particula[i].x = rock.getPositionX();
		particula[i].y = 1.0f;
		particula[i].z = rock.getPositionZ();
		particula[i].vx = v * cos(theta) * sin(phi);
		particula[i].vy = v * cos(phi);
		particula[i].vz = v * sin(theta) * sin(phi);
		particula[i].ax = -0.1f; /* simular um pouco de vento */
		particula[i].ay = 0.15f; /* simular a aceleraÁ„o da gravidade */
		particula[i].az = 0.1f;

		/* tom amarelado que vai ser multiplicado pela textura que varia entre branco e preto */
		particula[i].r = 0.210f;
		particula[i].g = 0.180f;
		particula[i].b = 0.140f;

		particula[i].life = 1.0f;		/* vida inicial */
		particula[i].fade = 0.0025f;	    /* step de decrÈscimo da vida para cada iteraÁ„o */
	}
}

// ------------------------------------------------------------
//
// Update function
//

void updateScene(int value) {
	std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
	float realDelta = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime).count() / 1000.f;
	lastTime = now;
	GLint loc;
	float particle_color[4];
	
	if (!paused) {
		if (!gameOver) {

			// Update rocks
			for (int i = 0; i < rocksNumber; i++) {
				rocks[i].update(realDelta);
			}
			for (int i = 0; i < staticRocksNum; i++) {
				staticRocks[i].update(realDelta);
			}

			// Set up collision boxes
			_positions.clear();
			for (int i = 0; i < rocksNumber; i++) {
				_positions.push_back(rocks[i].getMinMax());
			}
			for (int i = 0; i < staticRocksNum; i++) {
				_positions.push_back(staticRocks[i].getMinMax());
			}
			for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
				_positions.push_back(beacons[i].getMinMax());
			}
			_positions.push_back(dome.getMinMax());

			// Update and collide rover
			Collision collision = rover.update(realDelta, _positions);
			if (collision.id >= 0 && collision.id < rocksNumber) {
				lives -= 1;
				initParticles(rocks[collision.id]);
				fireworks = 1;
				rocks[collision.id].init();
			}
			else if (collision.id - rocksNumber >= 0 && collision.id - rocksNumber < staticRocksNum) {
				staticRocks[collision.id - rocksNumber].pushFromPos(rover.getPos(), collision.speed);
			}

			// Update spotlight position
			RoverLights roverLights = rover.getRoverLights();
			spotLights[0].moveLight(roverLights.pos1, roverLights.dir);
			spotLights[1].moveLight(roverLights.pos2, roverLights.dir);

			_time -= realDelta;
			if (_time < 0) {
				points += 1;
				_time = 30.0f;
			}

			if (lives < 1) {
				gameOver = true;
			}
		}
	}

		glutTimerFunc(deltaTime * 1000, updateScene, 0);
}

// ------------------------------------------------------------
//
// Render stufff
//

// Recursive render of the Assimp Scene Graph
void aiRecursive_render(const aiScene* sc, const aiNode* nd)
{
	GLint loc;

	// Get node transformation matrix
	aiMatrix4x4 m = nd->mTransformation;
	// OpenGL matrices are column major
	m.Transpose();

	// save model matrix and apply node transformation
	pushMatrix(MODEL);

	float aux[16];
	memcpy(aux, &m, sizeof(float) * 16);
	multMatrix(MODEL, aux);

	translate(MODEL, -270, 0, 76.5);
	rotate(MODEL, 70, 0, 1, 0);
	scale(MODEL, 2.5, 2.5, 2.5);


	// draw all meshes assigned to this node
	for (unsigned int n = 0; n < nd->mNumMeshes; ++n) {


		// send the material
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
		glUniform4fv(loc, 1, myMeshesAssimp[nd->mMeshes[n]].mat.ambient);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
		glUniform4fv(loc, 1, myMeshesAssimp[nd->mMeshes[n]].mat.diffuse);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
		glUniform4fv(loc, 1, myMeshesAssimp[nd->mMeshes[n]].mat.specular);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.emissive");
		glUniform4fv(loc, 1, myMeshesAssimp[nd->mMeshes[n]].mat.emissive);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
		glUniform1f(loc, myMeshesAssimp[nd->mMeshes[n]].mat.shininess);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.texCount");
		glUniform1i(loc, myMeshesAssimp[nd->mMeshes[n]].mat.texCount);

		unsigned int  diffMapCount = 0;  //read 2 diffuse textures

		//devido ao fragment shader suporta 2 texturas difusas simultaneas, 1 especular e 1 normal map

		glUniform1i(normalMap_loc, false);   //GLSL normalMap variable initialized to 0
		glUniform1i(specularMap_loc, false);
		glUniform1ui(diffMapCount_loc, 0);

		if (myMeshesAssimp[nd->mMeshes[n]].mat.texCount != 0)
			for (unsigned int i = 0; i < myMeshesAssimp[nd->mMeshes[n]].mat.texCount; ++i) {
				glActiveTexture(GL_TEXTURE0 + myMeshesAssimp[nd->mMeshes[n]].texUnits[i]);
				glBindTexture(GL_TEXTURE_2D, myMeshesAssimp[nd->mMeshes[n]].texIds[i]);
				if (myMeshesAssimp[nd->mMeshes[n]].texTypes[i] == DIFFUSE) {
					if (diffMapCount == 0) {
						diffMapCount++;
						loc = glGetUniformLocation(shader.getProgramIndex(), "texUnitDiff");
						glUniform1i(loc, myMeshesAssimp[nd->mMeshes[n]].texUnits[i]);
						glUniform1ui(diffMapCount_loc, diffMapCount);
					}
					else if (diffMapCount == 1) {
						diffMapCount++;
						loc = glGetUniformLocation(shader.getProgramIndex(), "texUnitDiff1");
						glUniform1i(loc, myMeshesAssimp[nd->mMeshes[n]].texUnits[i]);
						glUniform1ui(diffMapCount_loc, diffMapCount);
					}
					else printf("Only supports a Material with a maximum of 2 diffuse textures\n");
				}
				else if (myMeshesAssimp[nd->mMeshes[n]].texTypes[i] == SPECULAR) {
					loc = glGetUniformLocation(shader.getProgramIndex(), "texUnitSpec");
					glUniform1i(loc, myMeshes[nd->mMeshes[n]].texUnits[i]);
					glUniform1i(specularMap_loc, true);
				}
				else if (myMeshesAssimp[nd->mMeshes[n]].texTypes[i] == NORMALS) { //Normal map
					loc = glGetUniformLocation(shader.getProgramIndex(), "texUnitNormalMap");
					if (normalMapKey)
						glUniform1i(normalMap_loc, normalMapKey);
					glUniform1i(loc, myMeshesAssimp[nd->mMeshes[n]].texUnits[i]);

				}
				else printf("Texture Map not supported\n");
			}

		// send matrices to OGL
		computeDerivedMatrix(PROJ_VIEW_MODEL);
		glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
		glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
		computeNormalMatrix3x3();
		glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

		// bind VAO
		glBindVertexArray(myMeshesAssimp[nd->mMeshes[n]].vao);

		if (!shader.isProgramValid()) {
			printf("Program Not Valid!\n");
			exit(1);
		}
		// draw
		glDrawElements(myMeshesAssimp[nd->mMeshes[n]].type, myMeshesAssimp[nd->mMeshes[n]].numIndexes, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	// draw all children
	for (unsigned int n = 0; n < nd->mNumChildren; ++n) {
		aiRecursive_render(sc, nd->mChildren[n]);
	}
	popMatrix(MODEL);
}

void loadObjectTextures() {
	//Associate the Texture Units to Objects Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureArray[0]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TextureArray[1]);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, TextureArray[2]);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, TextureArray[3]);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, TextureArray[4]);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, TextureArray[5]);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, TextureArray[6]);

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, TextureArray[7]);

	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureArray[8]);

	glUniform1i(tex_loc, 0);
	glUniform1i(tex_loc1, 1);
	glUniform1i(tex_loc2, 2);
	glUniform1i(tex_loc3, 3);
	glUniform1i(tex_loc4, 4);
	glUniform1i(tex_loc5, 5);
	glUniform1i(tex_loc_sky, 6);
	glUniform1i(texUnitNormalMap_loc, 7);
	glUniform1i(tex_cube_loc, 8);
}

void drawFloor() {
	GLint loc;

	loadObjectTextures();

	// send the material
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
	glUniform4fv(loc, 1, myMeshes[0].mat.ambient);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
	glUniform4fv(loc, 1, myMeshes[0].mat.diffuse);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
	glUniform4fv(loc, 1, myMeshes[0].mat.specular);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
	glUniform1f(loc, myMeshes[0].mat.shininess);
	pushMatrix(MODEL);

	glUniform1i(texMode_uniformId, 0); //texture mars.tga
	rotate(MODEL, 270, 1, 0, 0);

	// send matrices to OGL
	computeDerivedMatrix(PROJ_VIEW_MODEL);
	glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
	glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
	computeNormalMatrix3x3();
	glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

	// Render mesh
	glBindVertexArray(myMeshes[0].vao);

	glDrawElements(myMeshes[0].type, myMeshes[0].numIndexes, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	popMatrix(MODEL);
}

void drawObjects(bool reflect) {
	GLint loc;
	float particle_color[4];

	// use our shader
	glUseProgram(shader.getProgramIndex());

	float res[4];
	multMatrixPoint(VIEW, lightPos, res);   //lightPos definido em World Coord so is converted to eye space
	glUniform4fv(lPos_uniformId, 1, res);

	for (int i = 0; i < NUM_POINT_LIGHTS; i++) pointLights[i].updateView();
	for (int i = 0; i < NUM_SPOT_LIGHTS; i++) spotLights[i].updateView();
	dirLight.updateView();

	//Setup fog
	glUniform1i(glGetUniformLocation(shader.getProgramIndex(), "fogEnabled"), fogEnabled);
	multMatrixPoint(VIEW, rover.getPos().data(), res);
	glUniform3fv(glGetUniformLocation(shader.getProgramIndex(), "fogCenter"), 1, res);

	loadObjectTextures();

	glUniform1i(texMode_uniformId, 2); //texture rover.tga
	rover.render(shader.getProgramIndex(), vm_uniformId, pvm_uniformId, normal_uniformId);

	glUniform1i(texMode_uniformId, 8); //bump mapping: normal comes from normalMap
	for (int i = 0; i < rocksNumber; i++) {
		rocks[i].render(shader.getProgramIndex(), vm_uniformId, pvm_uniformId, normal_uniformId);
	}
	for (int d = 0; d <= staticRocksNum; d++) {
		staticRocks[d].render(shader.getProgramIndex(), vm_uniformId, pvm_uniformId, normal_uniformId);
	}

	glUniform1i(texMode_uniformId, 4); //texture none
	for (int j = 0; j < NUM_POINT_LIGHTS; j++) {
		beacons[j].render(shader.getProgramIndex(), vm_uniformId, pvm_uniformId, normal_uniformId);
	}

	if (rover.getPos()[0] > -25) {
		glUniform1i(texMode_uniformId, 4); //texture none
		dome.render(shader.getProgramIndex(), vm_uniformId, pvm_uniformId, normal_uniformId);

		glUniform1i(texMode_uniformId, 5); //texture rock.tga
		billboard.render(shader.getProgramIndex(), vm_uniformId, pvm_uniformId, normal_uniformId, reflect);
	}
	else {
		glUniform1i(texMode_uniformId, 5); //texture rock.tga
		billboard.render(shader.getProgramIndex(), vm_uniformId, pvm_uniformId, normal_uniformId, reflect);

		glUniform1i(texMode_uniformId, 4); //texture none
		dome.render(shader.getProgramIndex(), vm_uniformId, pvm_uniformId, normal_uniformId);
	}

	glUniform1i(texMode_uniformId, 9); // Environment map
	glUniformMatrix4fv(view_uniformId, 1, GL_FALSE, mMatrix[VIEW]);
	environmentSphere.render(shader.getProgramIndex(), vm_uniformId, pvm_uniformId, normal_uniformId);

	// sets the model matrix to a scale matrix so that the model fits in the window
	pushMatrix(MODEL);
	scale(MODEL, scaleFactor, scaleFactor, scaleFactor); //HERE
	glUniform1i(texMode_uniformId, 3);
	aiRecursive_render(scene, scene->mRootNode); //HERE
	popMatrix(MODEL);

	if (fireworks) {

		updateParticles();

		// draw fireworks particles
		int objId = 1;  //quad for particle

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDepthMask(GL_FALSE);  //Depth Buffer Read Only

		glUniform1i(texMode_uniformId, 6); // draw modulated textured particles 

		for (int i = 0; i < MAX_PARTICULAS; i++)
		{
			if (particula[i].life > 0.0f) /* sÛ desenha as que ainda est„o vivas */
			{

				/* A vida da partÌcula representa o canal alpha da cor. Como o blend est· activo a cor final È a soma da cor rgb do fragmento multiplicada pelo
				alpha com a cor do pixel destino */

				particle_color[0] = particula[i].r;
				particle_color[1] = particula[i].g;
				particle_color[2] = particula[i].b;
				particle_color[3] = particula[i].life;

				// send the material - diffuse color modulated with texture
				loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
				glUniform4fv(loc, 1, particle_color);

				pushMatrix(MODEL);
				translate(MODEL, particula[i].x, particula[i].y, particula[i].z);

				// Preform billboard viewModel manipulation
				computeDerivedMatrix(VIEW_MODEL);
				float viewModel[16];
				memcpy(viewModel, mCompMatrix[VIEW_MODEL], sizeof(float) * 16);
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						if (i == j)
							viewModel[i * 4 + j] = 1.0;
						else
							viewModel[i * 4 + j] = 0.0;
					}
				}
				if (reflect) viewModel[1 * 4 + 1] = -1;
				memcpy(mCompMatrix[VIEW_MODEL], viewModel, sizeof(float) * 16);

				// send matrices to OGL
				computeDerivedMatrix_PVM();
				glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
				glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
				computeNormalMatrix3x3();
				glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

				glBindVertexArray(myMeshes[objId].vao);
				glDrawElements(myMeshes[objId].type, myMeshes[objId].numIndexes, GL_UNSIGNED_INT, 0);
				popMatrix(MODEL);
			}
			else dead_num_particles++;
		}

		glDepthMask(GL_TRUE); //make depth buffer again writeable

		if (dead_num_particles == MAX_PARTICULAS) {
			fireworks = 0;
			dead_num_particles = 0;
			printf("All particles dead\n");
		}
	}
}

void drawHUD() {
	int m_viewport[4];
	glGetIntegerv(GL_VIEWPORT, m_viewport);

	// Update flare variables
	float flarePos[3];

	pushMatrix(MODEL);
	loadIdentity(MODEL);
	computeDerivedMatrix(PROJ_VIEW_MODEL);  //pvm to be applied to lightPost. pvm is used in project function
	if (!project(dirLight.calcSrcPos(camera[activeCamera].getPos()).data(), flarePos, m_viewport))
		printf("Error in getting projected light in screen\n");  //Calculate the window Coordinates of the light position: the projected position of light on viewport
	if (flarePos[2] > 1.f) flarePos[0] = m_viewport[0] - 1;
	lensFlare.updateLight({ flarePos[0], flarePos[1] },
		{ m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3] });
	popMatrix(MODEL);

	//viewer at origin looking down at  negative z direction
	pushMatrix(MODEL);
	loadIdentity(MODEL);
	pushMatrix(PROJECTION);
	loadIdentity(PROJECTION);
	pushMatrix(VIEW);
	loadIdentity(VIEW);
	ortho(m_viewport[0], m_viewport[0] + m_viewport[2] - 1, m_viewport[1], m_viewport[1] + m_viewport[3] - 1, -1, 1);

	if (flareEnabled) {
		glUseProgram(shaderFlare.getProgramIndex());
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(flare_tex_loc, 0);
		lensFlare.render(shaderFlare.getProgramIndex(), flare_vm_uniformId, flare_pvm_uniformId, flare_normal_uniformId);
		glUseProgram(shader.getProgramIndex());
	}

	std::stringstream text;
	text << "Lives: " << lives;
	RenderText(shaderText, text.str(), m_viewport[0] + m_viewport[2] - 180.f, m_viewport[1] + m_viewport[3] - 45.f, 1.0f, 9.0f, 0.1f, 0.1f);

	std::stringstream().swap(text);
	text << "Points: " << points;
	RenderText(shaderText, text.str(), 005.0f, m_viewport[1] + m_viewport[3] - 45.f, 1.0f, 0.1f, 0.1f, 0.9f);


	if (paused) {
		std::stringstream().swap(text);
		text << "Game Paused";
		RenderText(shaderText, text.str(), m_viewport[0] + (m_viewport[2] / 2) - 150.f, m_viewport[1] + (m_viewport[3] / 2) + 45.f, 1.0f, 1.0f, 1.0f, 0.9f);
	}

	if (gameOver) {
		std::stringstream().swap(text);
		text << "Game Over";
		RenderText(shaderText, text.str(), m_viewport[0] + (m_viewport[2] / 2) - 150.f, m_viewport[1] + (m_viewport[3] / 2) + 45.f, 1.0f, 255.0f, 0.0f, 0.0f);
		reset();
	}

	popMatrix(PROJECTION);
	popMatrix(VIEW);
	popMatrix(MODEL);
}

void drawSkyBox() {
	loadObjectTextures();

	// Render Sky Box
	int objId = 3;
	glUniform1i(texMode_uniformId, 7);

	//it won't write anything to the zbuffer; all subsequently drawn scenery to be in front of the sky box. 
	glDepthMask(GL_FALSE);
	glFrontFace(GL_CW); // set clockwise vertex order to mean the front

	pushMatrix(MODEL);
	pushMatrix(VIEW);  //se quiser anular a translaÁ„o

	//  Fica mais realista se n„o anular a translaÁ„o da c‚mara 
	// Cancel the translation movement of the camera - de acordo com o tutorial do Antons
	mMatrix[VIEW][12] = 0.0f;
	mMatrix[VIEW][13] = 0.0f;
	mMatrix[VIEW][14] = 0.0f;

	scale(MODEL, 100.0f, 100.0f, 100.0f);
	translate(MODEL, -0.5f, -0.5f, -0.5f);

	// send matrices to OGL
	glUniformMatrix4fv(model_uniformId, 1, GL_FALSE, mMatrix[MODEL]); //TransformaÁ„o de modelaÁ„o do cubo unit·rio para o "Big Cube"
	computeDerivedMatrix(PROJ_VIEW_MODEL);
	glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
	glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
	computeNormalMatrix3x3();
	glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

	glBindVertexArray(myMeshes[objId].vao);
	glDrawElements(myMeshes[objId].type, myMeshes[objId].numIndexes, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	popMatrix(MODEL);
	popMatrix(VIEW);

	glFrontFace(GL_CCW); // restore counter clockwise vertex order to mean the front
	glDepthMask(GL_TRUE);
}

void renderScene(void) {
	int prevCamera = activeCamera;
	GLint loc;

	if (firstRender) {
		FrameCount++;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	// load identity matrices
	loadIdentity(VIEW);
	loadIdentity(MODEL);

	// set the camera using a function similar to gluLookAt
	if (!firstRender) { //rear cam
		glEnable(GL_SCISSOR_TEST);
		glScissor(windowWidth / 7 * 6 - 20, 20, windowWidth / 7, windowHeight / 7);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);

		glViewport(windowWidth / 7 * 6 - 20, 20, windowWidth / 7, windowHeight / 7);
		pushMatrix(PROJECTION);
		loadIdentity(PROJECTION);
		perspective(53.13f, windowWidth/windowHeight * 1.0f, 0.1f, 1000.0f);
		std::array<float, 3> roverTarget = rover.getFollowTarget();
		std::array<float, 3> roverPos = rover.getFollowPos(-90.0f, 10.0f, currentR);
		camera[3].updateTarget(roverPos);
		camera[3].updatePos({ roverTarget[0], roverTarget[1] + 2, roverTarget[2] });
		camera[3].camLookAt();
		activeCamera = 3;
	}
	else if (activeCamera == 2) {
		glViewport(0, 0, windowWidth, windowHeight);
		camera[2].updateTarget(rover.getFollowTarget());
		camera[2].updatePos(rover.getFollowPos(currentAlpha, currentBeta, currentR));
		camera[2].camLookAt();
	}
	else {
		glViewport(0, 0, windowWidth, windowHeight);
		glMatrixMode(GL_PROJECTION);                       // Select The Projection Matrix
		glLoadIdentity();
		camera[activeCamera].camLookAt();
	}

	drawSkyBox();

	glStencilFunc(GL_NEVER, 0x1, 0x1);
	glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);
	drawFloor();

	// Reflections
	if (camera[activeCamera].getPos()[1] > 0) {
		glStencilFunc(GL_EQUAL, 0x1, 0x1);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		for (int i = 0; i < NUM_POINT_LIGHTS; i++) pointLights[i].reflect();
		for (int i = 0; i < NUM_SPOT_LIGHTS; i++) spotLights[i].reflect();
		dirLight.reflect();

		pushMatrix(MODEL);
		scale(MODEL, 1.0f, -1.0f, 1.0f);
		glCullFace(GL_FRONT);
		drawObjects(true);
		glCullFace(GL_BACK);
		popMatrix(MODEL);

		for (int i = 0; i < NUM_POINT_LIGHTS; i++) pointLights[i].reflect();
		for (int i = 0; i < NUM_SPOT_LIGHTS; i++) spotLights[i].reflect();
		dirLight.reflect();
	}

	glStencilFunc(GL_ALWAYS, 0x2, 0x2);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	drawFloor();

	// Shadows
	if (dirLight.getEnabled() && camera[activeCamera].getPos()[1] > 0) {
		float mat[16];
		GLfloat floorPlane[4] = { 0.f, 1.f, 0.f, -0.1f };

		glUniform1i(shadowMode_uniformId, 1);  //Render with constant color
		shadow_matrix(mat, floorPlane, dirLight.calcSrcPos({0.f, 0.f, 0.f, 1.f}).data());

		//Dark the color stored in color buffer
		glStencilFunc(GL_EQUAL, 0x2, 0x3);
		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		glDisable(GL_DEPTH_TEST);

		pushMatrix(MODEL);
		multMatrix(MODEL, mat);
		drawObjects(false);
		popMatrix(MODEL);

		glUniform1i(shadowMode_uniformId, 0);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
	}

	glStencilFunc(GL_ALWAYS, 0x1, 0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	drawObjects(false);

	//Render text (bitmap fonts) in screen coordinates. So use ortoghonal projection with viewport coordinates.
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	if (firstRender) 
		drawHUD();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	if (firstRender) {
		firstRender = 0;
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		renderScene();
	}
	else{
		firstRender = 1;
		activeCamera = prevCamera;
		popMatrix(PROJECTION);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glutSwapBuffers();
	}
}

// ------------------------------------------------------------
//
// Events from the Keyboard
//

void processKeys(unsigned char key, int xx, int yy) {
	int m_viewport[4];
	glGetIntegerv(GL_VIEWPORT, m_viewport);

	switch (key) {

	case 27:
		glutLeaveMainLoop();
		break;
	case 49: //camera 1
		activeCamera = 0;
		loadIdentity(PROJECTION);
		camera[activeCamera].camProjection(m_viewport);
		break;
	case 50: //camera 2
		activeCamera = 1;
		loadIdentity(PROJECTION);
		camera[activeCamera].camProjection(m_viewport);
		break;
	case 51: //camera 3
		activeCamera = 2;
		loadIdentity(PROJECTION);
		camera[activeCamera].camProjection(m_viewport);
		break;
	case 37: //left key (move camera) //TO BE FIXED
		break;
	case 'o': //move rover left
	case 'O':
		rover.move(0);
		break;
	case 'p': //move rover right
	case 'P':
		rover.move(1);
		break;
	case 'q': //move rover forwards
	case 'Q':
		rover.move(2);
		break;
	case 'a': //move rover backwards
	case 'A':
		rover.move(3);
		break;
	case 'n': //turn on/off directional light (day/night)
	case 'N':
		dirLight.lightSwitch();
		break;
	case 'c': //turn on/off point lights (solar beacons)
	case 'C':
		for (int i = 0; i < NUM_POINT_LIGHTS; i++) pointLights[i].lightSwitch();
		break;
	case 'h': //turn on/off spotlight (rover)
	case 'H':
		for (int i = 0; i < NUM_SPOT_LIGHTS; i++) spotLights[i].lightSwitch();
		break;
	case 'f': //turn on/off fog
	case 'F':
		if (fogEnabled) fogEnabled = false;
		else fogEnabled = true;
		break;
	case 's': //pause the game
	case 'S':
		if (paused) paused = false;
		else paused = true;
		break;
	case 'r': //start new game
	case 'R':
		gameOver = false;
		fireworks = 0;
		dead_num_particles = 0;
		reset();
		break;
	case 'l': //turn on/off flare
	case 'L':
		if (flareEnabled) flareEnabled = false;
		else flareEnabled = true;
		break;
	}
}


void processUpKeys(unsigned char key, int xx, int yy) {
	switch (key) {
	case 'o': //move rover left
	case 'O':
		rover.move(4);
		break;
	case 'p': //move rover right
	case 'P':
		rover.move(4);
		break;
	case 'q': //move rover forwards
	case 'Q':
		rover.move(5);
		break;
	case 'a': //move rover backwards
	case 'A':
		rover.move(5);
		break;
	}
}


// ------------------------------------------------------------
//
// Mouse Events
//

void processMouseButtons(int button, int state, int xx, int yy)
{
	// start tracking the mouse
	if (state == GLUT_DOWN) {
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;
	}

	//stop tracking the mouse
	else if (state == GLUT_UP) {
		if (tracking == 1) {
			alpha -= (xx - startX);
			beta += (yy - startY);
		}
		else if (tracking == 2) {
			r += (yy - startY) * 0.01f;
			if (r < 0.1f)
				r = 0.1f;
		}
		tracking = 0;
	}
}

// Track mouse motion while buttons are pressed

void processMouseMotion(int xx, int yy)
{

	int deltaX, deltaY;
	float alphaAux, betaAux;
	float rAux;

	deltaX = -xx + startX;
	deltaY = yy - startY;

	// left mouse button: move camera
	if (tracking == 1) {


		alphaAux = alpha + deltaX;
		betaAux = beta + deltaY;

		if (betaAux > 85.0f)
			betaAux = 85.0f;
		else if (betaAux < -85.0f)
			betaAux = -85.0f;
		rAux = r;
	}
	// right mouse button: zoom
	else if (tracking == 2) {

		alphaAux = alpha;
		betaAux = beta;
		rAux = r + (deltaY * 0.01f);
		if (rAux < 0.1f)
			rAux = 0.1f;
	}

	camX = rAux * sin(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
	camZ = rAux * cos(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
	camY = rAux * sin(betaAux * 3.14f / 180.0f);

	currentAlpha = alphaAux; currentBeta = betaAux; currentR = r;
}


void mouseWheel(int wheel, int direction, int x, int y) {

	r += direction * 0.1f;
	if (r < 0.1f)
		r = 0.1f;

	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camY = r * sin(beta * 3.14f / 180.0f);
}

// --------------------------------------------------------
//
// Shader Stuff
//

void checkCompileErrors(GLuint shader, std::string type) {
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
		else {
			std::cout << "Shader linked" << std::endl;
		}
	}
}

GLuint setupShaders() {

	// Shader for models
	shader.init();
	shader.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/pointlight.vert");
	shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/pointlight.frag");

	// set semantics for the shader variables
	glBindFragDataLocation(shader.getProgramIndex(), 0, "colorOut");
	glBindAttribLocation(shader.getProgramIndex(), VERTEX_COORD_ATTRIB, "position");
	glBindAttribLocation(shader.getProgramIndex(), NORMAL_ATTRIB, "normal");
	glBindAttribLocation(shader.getProgramIndex(), TEXTURE_COORD_ATTRIB, "texCoord");

	glLinkProgram(shader.getProgramIndex());
	checkCompileErrors(shader.getProgramIndex(), "PROGRAM");

	texMode_uniformId = glGetUniformLocation(shader.getProgramIndex(), "texMode");
	shadowMode_uniformId = glGetUniformLocation(shader.getProgramIndex(), "shadowMode");
	pvm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_pvm");
	vm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_viewModel");
	model_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_Model");
	view_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_View");
	normal_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_normal");
	lPos_uniformId = glGetUniformLocation(shader.getProgramIndex(), "l_pos");
	tex_loc = glGetUniformLocation(shader.getProgramIndex(), "texmap");
	tex_loc1 = glGetUniformLocation(shader.getProgramIndex(), "texmap1");
	tex_loc2 = glGetUniformLocation(shader.getProgramIndex(), "texmap2");
	tex_loc3 = glGetUniformLocation(shader.getProgramIndex(), "texmap3");
	tex_loc4 = glGetUniformLocation(shader.getProgramIndex(), "texmap4");
	tex_loc5 = glGetUniformLocation(shader.getProgramIndex(), "texmap5");
	tex_loc_sky = glGetUniformLocation(shader.getProgramIndex(), "texmapsky");
	tex_cube_loc = glGetUniformLocation(shader.getProgramIndex(), "cubeMap");
	normalMap_loc = glGetUniformLocation(shader.getProgramIndex(), "normalMap");
	texUnitNormalMap_loc = glGetUniformLocation(shader.getProgramIndex(), "texUnitNormalMap");
	specularMap_loc = glGetUniformLocation(shader.getProgramIndex(), "specularMap");
	diffMapCount_loc = glGetUniformLocation(shader.getProgramIndex(), "diffMapCount");

	printf("InfoLog for Per Fragment Phong Lightning Shader\n%s\n\n", shader.getAllInfoLogs().c_str());

	// Shader for bitmap Text
	shaderText.init();
	shaderText.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/text.vert");
	shaderText.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/text.frag");

	glLinkProgram(shaderText.getProgramIndex());
	checkCompileErrors(shaderText.getProgramIndex(), "PROGRAM");
	printf("InfoLog for Text Rendering Shader\n%s\n\n", shaderText.getAllInfoLogs().c_str());

	// Shader for flare rendering
	shaderFlare.init();
	shaderFlare.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/flare.vert");
	shaderFlare.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/flare.frag");

	glLinkProgram(shaderFlare.getProgramIndex());
	checkCompileErrors(shaderFlare.getProgramIndex(), "PROGRAM");

	flare_pvm_uniformId = glGetUniformLocation(shaderFlare.getProgramIndex(), "m_pvm");
	flare_vm_uniformId = glGetUniformLocation(shaderFlare.getProgramIndex(), "m_viewModel");
	flare_normal_uniformId = glGetUniformLocation(shaderFlare.getProgramIndex(), "m_normal");
	flare_tex_loc = glGetUniformLocation(shaderFlare.getProgramIndex(), "texmap");

	printf("InfoLog for Flare Rendering Shader\n%s\n\n", shaderFlare.getAllInfoLogs().c_str());

	return(shader.isProgramLinked() && shaderText.isProgramLinked() && shaderFlare.isProgramLinked());
}

// ------------------------------------------------------------
//
// Model loading and OpenGL setup
//

void init()
{
	MyMesh amesh;

	/* Initialization of DevIL */
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	{
		printf("wrong DevIL version \n");
		exit(0);
	}
	ilInit();

	/// Initialization of freetype library with font_name file
	freeType_init(font_name);

	// Init the fixed cameras
	float camPos[] = { 0.0f, 80.0f, 0.0f };
	float camTarget[] = { 0.f, 0.f, 0.f };
	camera[0].init(CAM_ORTHO, camPos, camTarget, true);
	camera[1].init(CAM_PERSPECTIVE, camPos, camTarget, true);

	// Init the follow camera position based on spherical coordinates
	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camY = r * sin(beta * 3.14f / 180.0f);
	float camFollowPos[] = { camX, camY, camZ };
	camera[2].init(CAM_PERSPECTIVE, camFollowPos, camTarget);

	float camRearFollow[] = { camX, camY + 2, camZ };
	float camRearTarget[] = { 0, 0, 0 };
	camera[3].init(CAM_PERSPECTIVE, camRearFollow, camRearTarget);

	glGenTextures(9, TextureArray);
	Texture2D_Loader(TextureArray, "sand2.tga", 0);
	Texture2D_Loader(TextureArray, "rocks.tga", 1);
	Texture2D_Loader(TextureArray, "rover.tga", 2);
	Texture2D_Loader(TextureArray, "sand.tga", 3);
	Texture2D_Loader(TextureArray, "rock.tga", 4);
	Texture2D_Loader(TextureArray, "particle.tga", 5);
	Texture2D_Loader(TextureArray, "skybox/stars/starry.png", 6);
	Texture2D_Loader(TextureArray, "bump.jpg", 7);
	//Sky Box Texture Object
	//FINEconst char* filenames[] = { "skybox/desert/posz.png", "skybox/desert/negz.png", "skybox/desert/posy.png", "skybox/desert/negy.png", "skybox/desert/posx.png", "skybox/desert/negx.png" };
	const char* filenames[] = { "skybox/blue_nebula/posz.png", "skybox/blue_nebula/negz.png", "skybox/blue_nebula/posy.png", "skybox/blue_nebula/negy.png", "skybox/blue_nebula/posx.png", "skybox/blue_nebula/negx.png" };
	//const char* filenames[] = { "skybox/stars/starry.png", "skybox/stars/starry.png", "skybox/stars/starry.png", "skybox/stars/starry.png", "skybox/stars/starry.png", "skybox/stars/starry.png" };
	TextureCubeMap_Loader(TextureArray, filenames, 8);


	float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
	float diff[] = { 0.8f, 0.6f, 0.4f, 1.0f };
	float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 100.0f;
	int texcount = 0;

	float diffTransparent[] = { 0.8f, 0.6f, 0.4f, 0.9f };

	// create geometry and VAO of the cube (ground)
	amesh = createQuad(120, 120);
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diffTransparent, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	myMeshes.push_back(amesh);

	amesh = createQuad(2, 2);
	amesh.mat.texCount = texcount;
	myMeshes.push_back(amesh);

	// create geometry and VAO of the quad (mirror)
	amesh = createQuad(20, 20);
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	myMeshes.push_back(amesh);

	// create geometry and VAO of the cube, objId=3;
	amesh = createCube();
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	myMeshes.push_back(amesh);


	billboard.init();
	
	rover.init();

	for (int i = 0; i < rocksNumber; i++) {
		rocks[i].init();
	}
	for (int d = 0; d < staticRocksNum; d++) {
		staticRocks[d].init();
	}
	staticRocks[0].createStaticRocks(30, -8);
	staticRocks[1].createStaticRocks(25, -45);
	staticRocks[2].createStaticRocks(-15, 1);
	staticRocks[3].createStaticRocks(-10, -20);

	for (int i = 0; i < staticRocksNum; i++) {
		_positions.push_back(staticRocks[i].getMinMax());
	}

	for (int j = 0; j < NUM_POINT_LIGHTS; j++) {
		beacons[j].init();
	}
	beacons[0].createBeacon(-30, -30);
	beacons[1].createBeacon(5, 15);
	beacons[2].createBeacon(0, -15);
	beacons[3].createBeacon(-30, 15);
	beacons[4].createBeacon(30, -15);
	beacons[5].createBeacon(25, 20);

	for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
		_positions.push_back(beacons[i].getMinMax());
	}

	//initialize pointLigts for beacons
	pointLights[0].init(shader.getProgramIndex(), "pointLight[0]",
		{ -30.0f, 5.0f, -30.0f, 1.0f }, 18.0f);
	pointLights[1].init(shader.getProgramIndex(), "pointLight[1]",
		{ 5.0f, 5.0f, 15.0f, 1.0f }, 18.0f);
	pointLights[2].init(shader.getProgramIndex(), "pointLight[2]",
		{ 0.0f, 5.0f, -15.0f, 1.0f }, 18.0f);
	pointLights[3].init(shader.getProgramIndex(), "pointLight[3]",
		{ -30.0f, 5.0f, 15.0f, 1.0f }, 18.0f);
	pointLights[4].init(shader.getProgramIndex(), "pointLight[4]",
		{ 30.0f, 5.0f, -15.0f, 1.0f }, 18.0f);
	pointLights[5].init(shader.getProgramIndex(), "pointLight[5]",
		{ 25.0f, 5.0f, 20.0f, 1.0f }, 18.0f);

	spotLights[0].init(shader.getProgramIndex(), "spotLight[0]",
		{ 6.0f, 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, -1.0f, 0.0f },
		50.f,
		40.f
	);
	spotLights[1].init(shader.getProgramIndex(), "spotLight[1]",
		{ -6.0f, 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, -1.0f, 0.0f },
		50.f,
		40.f
	);

	dirLight.init(shader.getProgramIndex(), "dirLight", { 1.0, -1.0, 1.0, 0.0f });

	dome.init();
	dome.createObjectTransp(-25, -10);
	_positions.push_back(dome.getMinMax());

	environmentSphere.init();
	environmentSphere.createMirrorSphere(10.f, 10.f);

	//ASSINP
	std::string filepath;
	std::ostringstream oss;
	oss << "objs/" << model_dir << "/" << model_dir << ".obj";
	filepath = oss.str();   //path of OBJ file in the VS project

	strcat(model_dir, "/");  //directory path in the VS project

	//check if file exists
	ifstream fin(filepath.c_str());
	if (!fin.fail()) {
		fin.close();
	}
	else
		printf("Couldn't open file: %s\n", filepath.c_str());

	//import 3D file into Assimp scene graph
	if (!Import3DFromFile(filepath))
		printf("ERROR\n");

	//creation of Mymesh array with VAO Geometry and Material
	myMeshesAssimp = createMeshFromAssimp(scene);

	// Intialize lens flare
	lensFlare.init();

	lastTime = std::chrono::system_clock::now();
	// some GL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);		 // cull back face
	glFrontFace(GL_CCW); // set counter-clockwise vertex order to mean the front
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_STENCIL_TEST);
	glClearStencil(0x0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


// ------------------------------------------------------------
//
// Main function
//


int main(int argc, char** argv) {

	//  GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL | GLUT_MULTISAMPLE);

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);

	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WinX, WinY);
	WindowHandle = glutCreateWindow(CAPTION);


	//  Callback Registration
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

	glutTimerFunc(0, timer, 0);
	glutTimerFunc(deltaTime * 1000, updateScene, 0);
	glutIdleFunc(renderScene);  // Use it for maximum performance

//	Mouse and Keyboard Callbacks
	glutKeyboardFunc(processKeys);
	glutKeyboardUpFunc(processUpKeys);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);
	glutMouseWheelFunc(mouseWheel);


	//	return from main loop
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	//	Init GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));
	printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	if (!setupShaders())
		return(1);

	init();

	//  GLUT main loop
	glutMainLoop();

	return(0);
}