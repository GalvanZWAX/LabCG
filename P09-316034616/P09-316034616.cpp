/*
Semestre 2024-1
Animación:
Sesión 1:
Simple o básica:Por banderas y condicionales (más de 1 transforomación geométrica se ve modificada
Sesión 2
Compleja: Por medio de funciones y algoritmos.
Adicional.- ,Textura Animada
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>
#include <random>

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

//variables para animación
int counter = 0;
int framesAnim = 0;
int framesMov = 0;
int estAct = 0;
int estAnt = 0;
int rTime;
//float rotaLetrero;
//float movLetrero;
float rotaDadoX;
float rotaDadoY;
float rotaDadoZ;
float dirDadoX;
float dirDadoY;
float dirDadoZ;
float posDadoY;
float posScooterX;
float posScooterZ;
float dirScooter;

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
Texture AmTexture;
Texture AzTexture;
Texture RoTexture;
Texture VeTexture;
Texture AzGrTexture;

Model Kitt_M;
Model Llanta_M;
Model Blackhawk_M;
Model Arco_M;
Model Letrero_M;
Model Scooter_M;
Model LlantaD_M;
Model LlantaT_M;

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

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

void animacionCaida()
{
	posDadoY = posDadoY - 0.025f;
	//printf("[Frame %d] Pos dado: %f\n", framesAnim, posDadoY);
}

void animacionGiro(float rotaX, float rotaY, float rotaZ)
{
	dirDadoX = dirDadoX + rotaX;
	dirDadoY = dirDadoY + rotaY;
	dirDadoZ = dirDadoZ + rotaZ;
	//printf("[Frame %d] Dir dado: %f, %f, %f\n", framesAnim, dirDadoX, dirDadoY, dirDadoZ);
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
	AmTexture = Texture("Textures/amarillo.png");
	AmTexture.LoadTextureA();
	RoTexture = Texture("Textures/rojo.png");
	RoTexture.LoadTextureA();
	AzTexture = Texture("Textures/azul.png");
	AzTexture.LoadTextureA();
	VeTexture = Texture("Textures/verde.png");
	VeTexture.LoadTextureA();
	AzGrTexture = Texture("Textures/azulgris.png");
	AzGrTexture.LoadTextureA();

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	Letrero_M = Model();
	Letrero_M.LoadModel("Models/Letrero.obj");
	Arco_M = Model();
	Arco_M.LoadModel("Models/Arco.obj");
	Scooter_M = Model();
	Scooter_M.LoadModel("Models/Scooter.obj");
	LlantaD_M = Model();
	LlantaD_M.LoadModel("Models/llantaD.obj");
	LlantaT_M = Model();
	LlantaT_M.LoadModel("Models/llantaT.obj");

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
		0.6f, 0.6f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		-6.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
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

	//se crean mas luces puntuales y spotlight 

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	//movLetrero = 0.0f;
	//rotaLetrero = 0.0f;
	rotaDadoX = 0.0f;
	rotaDadoY = 0.0f;
	rotaDadoZ = 0.0f;
	posDadoY = 6.35f;
	posScooterX = 200.0f;
	posScooterZ = 15.0f;
	dirScooter = 0.0f;

	////Loop mientras no se cierra la ventana
	glfwSetTime(0.0);
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		rTime = static_cast<int>(std::round(lastTime));

		if (rTime % 2 == 0) //si es un segundo par
		{
			estAct = 1; //estado actual se actualiza
		}
		else
		{
			estAct = 0;
		}
		if (estAct > estAnt) //si estado actual diferente a anterior
		{
			counter++; //actualiza contador, cada 2 segundos
		}
		estAnt = estAct; //estado anterior se actualiza
		if (counter >= 4) //si se cumplio el ciclo de cuatro estados
		{
			counter = 0; //se reinicia el ciclo
		}

		if (mainWindow.getTiroDado())
		{
			mainWindow.resetTiroDado();
			int num = mainWindow.getNumDado();
			printf("Numero obtenido: %d\n", num);

			switch (num)
			{
				case 1:
					rotaDadoX = -0.2375f; rotaDadoY = -0.72f; rotaDadoZ = 0.0f;
					framesAnim = 1;
					break;

				case 2:
					rotaDadoX = -1.145f; rotaDadoY = -0.18f; rotaDadoZ = 0.0f;
					framesAnim = 1;
					break;

				case 3:
					rotaDadoX = -0.225f; rotaDadoY = -0.36f; rotaDadoZ = 0.0f;
					framesAnim = 1;
					break;

				case 4:
					rotaDadoX = -1.125f; rotaDadoY = -0.54f; rotaDadoZ = 0.0f;
					framesAnim = 1;
					break;

				case 5:
					rotaDadoX = -0.215f; rotaDadoY = 0.0f; rotaDadoZ = 0.0f;
					framesAnim = 1;
					break;

				case 6:
					rotaDadoX = -1.125f; rotaDadoY = -0.91f; rotaDadoZ = 0.0f;
					framesAnim = 1;
					break;

				case 7:
					rotaDadoX = -0.225f; rotaDadoY = 0.36f; rotaDadoZ = 0.0f;
					framesAnim = 1;
					break;

				case 8:
					rotaDadoX = -1.125f; rotaDadoY = 0.54f; rotaDadoZ = 0.0f;
					framesAnim = 1;
					break;
				
				case 9:
					rotaDadoX = -0.2375f; rotaDadoY = 0.72f; rotaDadoZ = 0.0f;
					framesAnim = 1;
					break;

				case 10:
					rotaDadoX = -1.145f; rotaDadoY = 0.18f; rotaDadoZ = 0.0f;
					framesAnim = 1;
					break;

				default:
					break;
			}
		}

		if (framesAnim == 1)
		{
			printf("Animacion de caida comenzada\n");
			posDadoY = 6.35f;
			dirDadoX = 0.0f;
			dirDadoY = 0.0f;
			dirDadoZ = 0.0f;
			framesAnim = 2;
		}
		else if (framesAnim >= 2 && framesAnim < 200)
		{
			animacionCaida();
			framesAnim++;
		}
		else if(framesAnim >= 200 && framesAnim < 400)
		{
			animacionGiro(rotaDadoX, rotaDadoY, rotaDadoZ);
			framesAnim++;
		}
		else if (framesAnim == 400)
		{
			printf("Animacion de giro terminada\n\n\n");
			framesAnim = 0;
		}

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
		//shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		//instancia de decaedro
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, posDadoY, 0.0f));
		model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));
		model = glm::rotate(model, dirDadoX * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, dirDadoY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, dirDadoZ * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		decaTexture.UseTexture();
		int idx = meshList.size() - 1;
		meshList[idx]->RenderMesh();

		//instancia de scooter
		AzGrTexture.UseTexture();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.45f, 0.45f, 0.45f));
		model = glm::translate(model, glm::vec3(posScooterX, 0.3f, posScooterZ));
		model = glm::rotate(model, dirScooter * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Scooter_M.RenderModel();
		model = glm::translate(model, glm::vec3(-7.5f, 1.6f, 0.0f));
		modelaux = model;
		model = glm::rotate(model, framesMov * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaD_M.RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(12.5f, 0.0f, 0.0f));
		model = glm::rotate(model, framesMov * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaT_M.RenderModel();

		if (framesMov >= 0 && framesMov < 2200)
		{
			posScooterX = posScooterX - 0.1f;
			framesMov++;
		}
		else if (framesMov == 2200)
		{
			dirScooter = -90.0f;
			framesMov = 2201;
		}
		else if (framesMov > 2200 && framesMov < 5000)
		{
			posScooterZ = posScooterZ - 0.1f;
			framesMov++;
		}
		else if (framesMov == 5000)
		{
			dirScooter = -180.0f;
			framesMov = 5001;
		}
		else if (framesMov >= 5001 && framesMov < 7400)
		{
			posScooterX = posScooterX + 0.1f;
			framesMov++;
		}
		else if (framesMov == 7400)
		{
			dirScooter = 90.0f;
			framesMov = 7401;
		}
		else if (framesMov >= 7401 && framesMov < 10200)
		{
			posScooterZ = posScooterZ + 0.1f;
			framesMov++;
		}
		else if (framesMov == 10200)
		{
			posScooterX = 200.0f;
			posScooterZ = 15.0f;
			dirScooter = 0.0f;
			framesMov = 0;
		}

		//printf("framesMov %d\n", framesMov);

		RoTexture.UseTexture();

		model = glm::mat4(1.0); //casilla 1
		model = glm::translate(model, glm::vec3(0.0f, 0.2f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 4
		model = glm::translate(model, glm::vec3(30.3f, 0.2f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 7
		model = glm::translate(model, glm::vec3(60.6f, 0.2f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 10
		model = glm::translate(model, glm::vec3(90.9f, 0.2f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 13
		model = glm::translate(model, glm::vec3(90.9f, 0.2f, -30.3f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 19
		model = glm::translate(model, glm::vec3(90.9f, 0.2f, -90.9f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 24
		model = glm::translate(model, glm::vec3(60.6f, 0.2f, -111.1f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 28
		model = glm::translate(model, glm::vec3(20.2f, 0.2f, -111.1f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 34
		model = glm::translate(model, glm::vec3(0.0f, 0.2f, -70.7f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 38
		model = glm::translate(model, glm::vec3(0.0f, 0.2f, -30.3f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		AmTexture.UseTexture();

		model = glm::mat4(1.0); //casilla 2
		model = glm::translate(model, glm::vec3(10.1f, 0.2f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 6
		model = glm::translate(model, glm::vec3(50.5f, 0.2f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 9
		model = glm::translate(model, glm::vec3(80.8f, 0.2f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 12
		model = glm::translate(model, glm::vec3(90.9f, 0.2f, -20.2f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 14
		model = glm::translate(model, glm::vec3(90.9f, 0.2f, -40.4f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 17
		model = glm::translate(model, glm::vec3(90.9f, 0.2f, -70.7f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 20
		model = glm::translate(model, glm::vec3(90.9f, 0.2f, -101.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 25
		model = glm::translate(model, glm::vec3(50.5f, 0.2f, -111.1f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 30
		model = glm::translate(model, glm::vec3(0.0f, 0.2f, -111.1f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 32
		model = glm::translate(model, glm::vec3(0.0f, 0.2f, -90.9f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 37
		model = glm::translate(model, glm::vec3(0.0f, 0.2f, -40.4f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		AzTexture.UseTexture();

		model = glm::mat4(1.0); //casilla 3
		model = glm::translate(model, glm::vec3(20.2f, 0.2f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 8
		model = glm::translate(model, glm::vec3(70.7f, 0.2f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 11
		model = glm::translate(model, glm::vec3(90.9f, 0.2f, -10.1f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 15
		model = glm::translate(model, glm::vec3(90.9f, 0.2f, -50.5f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 18
		model = glm::translate(model, glm::vec3(90.9f, 0.2f, -80.8f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 22
		model = glm::translate(model, glm::vec3(80.8f, 0.2f, -111.1f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 26
		model = glm::translate(model, glm::vec3(40.4f, 0.2f, -111.1f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 31
		model = glm::translate(model, glm::vec3(0.0f, 0.2f, -101.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 36
		model = glm::translate(model, glm::vec3(0.0f, 0.2f, -50.5f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 39
		model = glm::translate(model, glm::vec3(0.0f, 0.2f, -10.1f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		VeTexture.UseTexture();

		model = glm::mat4(1.0); //casilla 5
		model = glm::translate(model, glm::vec3(40.4f, 0.2f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 16
		model = glm::translate(model, glm::vec3(90.9f, 0.2f, -60.6f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 21
		model = glm::translate(model, glm::vec3(90.9f, 0.2f, -111.1f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 23
		model = glm::translate(model, glm::vec3(70.7f, 0.2f, -111.1f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 27
		model = glm::translate(model, glm::vec3(30.3f, 0.2f, -111.1f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 29
		model = glm::translate(model, glm::vec3(10.1f, 0.2f, -111.1f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 33
		model = glm::translate(model, glm::vec3(0.0f, 0.2f, -80.8f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 35
		model = glm::translate(model, glm::vec3(0.0f, 0.2f, -60.6f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0); //casilla 39
		model = glm::translate(model, glm::vec3(0.0f, 0.2f, -20.2f));
		model = glm::scale(model, glm::vec3(0.5f, 0.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		

		glUseProgram(0);

		mainWindow.swapBuffers();
	}
	return 0;
}