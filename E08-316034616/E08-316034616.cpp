/*
Práctica 7: Iluminación 1
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include "Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture decaTexture;

Model Coche_M;
Model Llanta_M;
Model Parabrisas_M;
Model Parrilla_M;

Model Lampara_M;

Model Blackhawk_M;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;

//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

void crearDecaedro()
{
	unsigned int deca_indices[] =
	{
		// piramide pentagonal superior
		0, 1, 2,
		3, 4, 5,
		6, 7, 8,
		9, 10, 11,
		12, 13, 14,

		// piramide pentagonal inferior
		15, 16, 17,
		18, 19, 20,
		21, 22, 23,
		24, 25, 26,
		27, 28, 29,
	};

	GLfloat deca_vertices[] =
	{
		//cara 1 sup : I
		0.0f,  0.09f,  0.0f,		0.501f,  0.666f,		0.0f,	-1.0f,	1.0f,   //punta superior 0
		0.0f,  0.0f,  -0.100f,		0.324f,  0.414f,		0.0f,	-1.0f,	1.0f,   //1
		0.100f, 0.0f, -0.025f,      0.668f,  0.414f,		0.0f,	-1.0f,	1.0f,   //2
		//cara 2 sup : III
		0.0f,  0.09f,  0.0f,		0.519f,  0.683f,		-1.0f,	-1.0f,	0.0f,   //0
		0.100f, 0.0f, -0.025f,      0.850f,  0.840f,		-1.0f,	-1.0f,	0.0f,	//2
		0.065f,  0.0f,  0.095f,     0.923f,  0.611f,		-1.0f,	-1.0f,	0.0f,	//3
		//cara 3 sup : V
		0.0f,  0.09f,  0.0f,		0.496f,  0.689f,		0.0f,	-1.0f,	-1.0f,	//0
		0.065f,  0.0f,  0.095f,     0.197f,  0.884f,		0.0f,	-1.0f,	-1.0f,	//3
		-0.065f, 0.0f, 0.095f,      0.519f,  0.968f,		0.0f,	-1.0f,	-1.0f,	//4
		//cara 4 sup : VII
		0.0f,  0.09f,  0.0f,		0.488f,  0.671f,		1.0f,	-1.0f,	0.0f,	//0
		-0.065f, 0.0f, 0.095f,      0.310f,  0.420f,		1.0f,	-1.0f,	0.0f,	//4
		-0.100f, 0.0f, -0.025f,     0.080f,  0.601f,		1.0f,	-1.0f,	0.0f,	//5
		//cara 5 sup : IX
		0.0f,  0.09f,  0.0f,		0.689f,  0.086f,		0.0f,	-1.0f,	1.0f,	//0
		0.0f,  0.0f,  -0.100f,		0.869f,  0.341f,		0.0f,	-1.0f,	1.0f,   //1
		-0.100f, 0.0f, -0.025f,     0.509f,  0.341f,		0.0f,	-1.0f,	1.0f,	//5

		//cara 1 inf : X
		0.0f,  -0.09f,  0.0f,		0.501f,  0.328f,		0.0f,	1.0f,	0.5f,   //punta inferior 6
		0.0f,  0.0f,  -0.100f,		0.326f,  0.078f,		0.0f,	1.0f,	0.5f,   //1
		0.100f, 0.0f, -0.025f,      0.675f,  0.078f,		0.0f,	1.0f,	0.5f,   //2
		//cara 2 inf : VIII
		0.0f,  -0.09f,  0.0f,		0.312f,  0.089f,		-0.5f,	1.0f,	0.0f,   //6
		0.100f, 0.0f, -0.025f,      0.488f,  0.341f,		-0.5f,	1.0f,	0.0f,	//2
		0.065f,  0.0f,  0.095f,     0.134f,  0.341f,		-0.5f,	1.0f,	0.0f,	//3
		//cara 3 inf : VI
		0.0f,  -0.09f,  0.0f,		0.480f,  0.681f,		0.0f,	1.0f,	-0.5f,	//6
		0.065f,  0.0f,  0.095f,     0.144f,  0.843f,		0.0f,	1.0f,	-0.5f,	//3 
		-0.065f, 0.0f, 0.095f,      0.076f,  0.611f,		0.0f,	1.0f,	-0.5f,	//4
		//cara 4 inf : IV
		0.0f,  -0.09f,  0.0f,		0.511f,  0.699f,		0.5f,	1.0f,	0.0f,	//6
		-0.065f, 0.0f, 0.095f,      0.847f,  0.853f,		0.5f,	1.0f,	0.0f,	//4
		-0.100f, 0.0f, -0.025f,     0.537f,  0.966f,		0.5f,	1.0f,	0.0f,	//5 
		//cara 5 inf : II
		0.0f,  -0.09f,  0.0f,		0.519f,  0.667f,		0.0f,	1.0f,	0.5f,	//6
		0.0f,  0.0f,  -0.100f,		0.917f,  0.601f,		0.0f,	1.0f,	0.5f,   //1
		-0.100f, 0.0f, -0.025f,     0.691f,  0.421f,		0.0f,	1.0f,	0.5f,	//5
	};

	Mesh* decaedro = new Mesh();
	decaedro->CreateMesh(deca_vertices, deca_indices, 240, 30);
	meshList.push_back(decaedro);
}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	crearDecaedro();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	decaTexture = Texture("Textures/dado_Deca.tga");
	decaTexture.LoadTextureA();

	Coche_M = Model();
	Coche_M.LoadModel("Models/coche.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_propia.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	Parabrisas_M = Model();
	Parabrisas_M.LoadModel("Models/parabrisas.obj");
	Parrilla_M = Model();
	Parrilla_M.LoadModel("Models/parrilla.obj");

	Lampara_M.LoadModel("Models/lampara.obj");

	bool banderaLuz = true;

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.5f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.5f, 0.1f,
		-6.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	//luz de lampara
	pointLights[1] = PointLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.9f,
		13.0f, 3.65f, 2.8f, //pos
		0.3f, 0.2f, 0.1f); //con lin exp
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	//luz de helicoptero
	spotLights[2] = SpotLight(1.0f, 1.0f, 0.0f, //rgb
		1.0f, 2.0f, //aIntens dIntens
		0.0f, 0.0f, 0.0f, //pos
		0.0f, -1.0f, 0.0f, // dir
		1.0f, 0.0f, 0.0f, //con lin exp
		15.0f); //edg
	spotLightCount++;

	//se crean mas luces puntuales y spotlight 

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación

		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		if (mainWindow.getBanderaLuz())
		{
			shaderList[0].SetPointLights(pointLights, pointLightCount);
		}
		else
		{
			shaderList[0].SetPointLights(pointLights, pointLightCount - 1);
		}
			
		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.85f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		//renderizado de decaedro
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, 3.0f));
		model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		decaTexture.UseTexture();
		int idx = meshList.size() - 1;
		meshList[idx]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevex(), 5.0f, 6.0));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Blackhawk_M.RenderModel();
		// luz de helicptero amarilla
		model = translate(model, glm::vec3(-0.45f, 2.5f, -0.75f)); //mover donde se colocará la luz
		glm::vec3 faroPos = glm::vec3(model[3][0], model[3][1], model[3][2]); //implementacion a partir del modelo
		glm::vec3 faroDir = glm::vec3(model[2][0] - 0.5f, model[2][1] - 1.3f, model[2][2]);
		//spotLights[2].SetFlash(faroPos, faroDir);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(13.0f, -0.8f, 1.5f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara_M.RenderModel();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}