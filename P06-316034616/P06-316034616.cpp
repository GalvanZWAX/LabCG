/*
Práctica 6: Texturizado
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
#include "Shader_m.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include "Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture dadoTexture;
Texture logofiTexture;
Texture decaTexture;
Texture rinTexture;
Texture cauchoTexture;
Texture ojosTexture;
Texture parrillaTexture;

Model Coche_M;
Model LlantaDI_M;
Model LlantaDD_M;
Model LlantaTI_M;
Model LlantaTD_M;
Model RinDI_M;
Model RinDD_M;
Model RinTI_M;
Model RinTD_M;
Model Parabrisas_M;
Model Parrilla_M;
Model Dado_M;

Skybox skybox;

//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;


// Vertex Shader
static const char* vShader = "shaders/shader_texture.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_texture.frag";

//cálculo del promedio de las normales para sombreado de Phong
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
	calcAverageNormals(indices, 12, vertices, 32, 8, 5);
	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

}

void CreateShaders()
{
	Shader *shader1 = new Shader();
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
		0.0f,  0.09f,  0.0f,		0.501f,  0.666f,		0.0f,	0.0f,	0.0f,   //punta superior 0
		0.0f,  0.0f,  -0.100f,		0.324f,  0.414f,		0.0f,	0.0f,	0.0f,   //1
		0.100f, 0.0f, -0.025f,      0.668f,  0.414f,		0.0f,	0.0f,	0.0f,   //2
		//cara 2 sup : III
		0.0f,  0.09f,  0.0f,		0.519f,  0.683f,		0.0f,	0.0f,	0.0f,   //0
		0.100f, 0.0f, -0.025f,      0.850f,  0.840f,		0.0f,	0.0f,	0.0f,	//2
		0.065f,  0.0f,  0.095f,     0.923f,  0.611f,		0.0f,	0.0f,	0.0f,	//3
		//cara 3 sup : V
		0.0f,  0.09f,  0.0f,		0.496f,  0.689f,		0.0f,	0.0f,	0.0f,	//0
		0.065f,  0.0f,  0.095f,     0.197f,  0.884f,		0.0f,	0.0f,	0.0f,	//3
		-0.065f, 0.0f, 0.095f,      0.519f,  0.968f,		0.0f,	0.0f,	0.0f,	//4
		//cara 4 sup : VII
		0.0f,  0.09f,  0.0f,		0.488f,  0.671f,		0.0f,	0.0f,	0.0f,	//0
		-0.065f, 0.0f, 0.095f,      0.310f,  0.420f,		0.0f,	0.0f,	0.0f,	//4
		-0.100f, 0.0f, -0.025f,     0.080f,  0.601f,		0.0f,	0.0f,	0.0f,	//5
		//cara 5 sup : IX
		0.0f,  0.09f,  0.0f,		0.689f,  0.086f,		0.0f,	0.0f,	0.0f,	//0
		0.0f,  0.0f,  -0.100f,		0.869f,  0.341f,		0.0f,	0.0f,	0.0f,   //1
		-0.100f, 0.0f, -0.025f,     0.509f,  0.341f,		0.0f,	0.0f,	0.0f,	//5

		//cara 1 inf : X
		0.0f,  -0.09f,  0.0f,		0.501f,  0.328f,		0.0f,	0.0f,	0.0f,   //punta inferior 6
		0.0f,  0.0f,  -0.100f,		0.326f,  0.078f,		0.0f,	0.0f,	0.0f,   //1
		0.100f, 0.0f, -0.025f,      0.675f,  0.078f,		0.0f,	0.0f,	0.0f,   //2
		//cara 2 inf : VIII
		0.0f,  -0.09f,  0.0f,		0.312f,  0.089f,		0.0f,	0.0f,	0.0f,   //6
		0.100f, 0.0f, -0.025f,      0.488f,  0.341f,		0.0f,	0.0f,	0.0f,	//2
		0.065f,  0.0f,  0.095f,     0.134f,  0.341f,		0.0f,	0.0f,	0.0f,	//3
		//cara 3 inf : VI
		0.0f,  -0.09f,  0.0f,		0.480f,  0.681f,		0.0f,	0.0f,	0.0f,	//6
		0.065f,  0.0f,  0.095f,     0.144f,  0.843f,		0.0f,	0.0f,	0.0f,	//3 
		-0.065f, 0.0f, 0.095f,      0.076f,  0.611f,		0.0f,	0.0f,	0.0f,	//4
		//cara 4 inf : IV
		0.0f,  -0.09f,  0.0f,		0.511f,  0.699f,		0.0f,	0.0f,	0.0f,	//6
		-0.065f, 0.0f, 0.095f,      0.847f,  0.853f,		0.0f,	0.0f,	0.0f,	//4
		-0.100f, 0.0f, -0.025f,     0.537f,  0.966f,		0.0f,	0.0f,	0.0f,	//5 
		//cara 5 inf : II
		0.0f,  -0.09f,  0.0f,		0.519f,  0.667f,		0.0f,	0.0f,	0.0f,	//6
		0.0f,  0.0f,  -0.100f,		0.917f,  0.601f,		0.0f,	0.0f,	0.0f,   //1
		-0.100f, 0.0f, -0.025f,     0.691f,  0.421f,		0.0f,	0.0f,	0.0f,	//5
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
	dadoTexture = Texture("Textures/dado_animales_limpio.tga");
	dadoTexture.LoadTextureA();
	logofiTexture = Texture("Textures/escudo_fi_color.tga");
	logofiTexture.LoadTextureA();
	//textura de dado 10 caras
	decaTexture = Texture("Textures/dado_Deca.tga");
	decaTexture.LoadTextureA();
	//texturas para coche
	rinTexture = Texture("Textures/rin.tga");
	rinTexture.LoadTextureA();
	cauchoTexture = Texture("Textures/caucho.tga");
	cauchoTexture.LoadTextureA();
	ojosTexture = Texture("Textures/ojos.tga");
	ojosTexture.LoadTextureA();
	parrillaTexture = Texture("Textures/parrilla.tga");
	parrillaTexture.LoadTextureA();
	
	Coche_M = Model();
	LlantaDI_M = Model();
	LlantaDD_M = Model();
	LlantaTI_M = Model();
	LlantaTD_M = Model();
	RinDI_M = Model();
	RinDD_M = Model();
	RinTI_M = Model();
	RinTD_M = Model();
	Parabrisas_M = Model();
	Parrilla_M = Model();

	Coche_M.LoadModel("Models/coche.obj");
	LlantaDI_M.LoadModel("Models/llantaDI.obj");
	LlantaDD_M.LoadModel("Models/llantaDD.obj");
	LlantaTI_M.LoadModel("Models/llantaTI.obj");
	LlantaTD_M.LoadModel("Models/llantaTD.obj");
	RinDI_M.LoadModel("Models/rinDI.obj");
	RinDD_M.LoadModel("Models/rinDD.obj");
	RinTI_M.LoadModel("Models/rinTI.obj");
	RinTD_M.LoadModel("Models/rinTD.obj");
	Parabrisas_M.LoadModel("Models/parabrisas.obj");
	Parrilla_M.LoadModel("Models/parrilla.obj");

	
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
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
		uniformColor = shaderList[0].getColorLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		color = glm::vec3(1.0f, 1.0f, 1.0f);//color blanco, multiplica a la información de color de la textura

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		meshList[2]->RenderMesh();
		
		//renderizado de decaedro
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 5.0f, 3.0f));
		model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		decaTexture.UseTexture();
		int idx = meshList.size() - 1;
		meshList[idx]->RenderMesh();		

		//renderizar coche
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.65f, -10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Coche_M.RenderModel();

		//renderizar llantas con textura de caucho
		cauchoTexture.UseTexture();
		LlantaDI_M.RenderModel();
		LlantaDD_M.RenderModel();
		LlantaTI_M.RenderModel();
		LlantaTD_M.RenderModel();

		//renderizar rines con su textura
		rinTexture.UseTexture();
		RinDI_M.RenderModel();
		RinDD_M.RenderModel();
		RinTI_M.RenderModel();
		RinTD_M.RenderModel();

		//renderizar el parabrisas con los ojos del personaje como textura
		ojosTexture.UseTexture();
		Parabrisas_M.RenderModel();

		//renderizar parrilla y cofre (están unidos en el modelo) con textura
		parrillaTexture.UseTexture();
		Parrilla_M.RenderModel();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}
	return 0;
}