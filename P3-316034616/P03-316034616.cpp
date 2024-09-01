//práctica 3: Modelado Geométrico y Cámara Sintética.
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <glew.h>
#include <glfw3.h>
//glm
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
#include <gtc\random.hpp>
//clases para dar orden y limpieza al còdigo
#include "Mesh.h"
#include "Shader.h"
#include "Sphere.h"
#include "Window.h"
#include "Camera.h"
//tecla E: Rotar sobre el eje X
//tecla R: Rotar sobre el eje Y
//tecla T: Rotar sobre el eje Z

using std::vector;

//Dimensiones de la ventana
const float toRadians = 3.14159265f/180.0; //grados a radianes
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader>shaderList;
//Vertex Shader, solo se utiliza uno pero se declaran dos
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";
Sphere sp = Sphere(1.0, 20, 20); //recibe radio, slices, stacks

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		// front
		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		// back
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}

// Pirámide triangular regular
// se pueden declarar como primitivas las piramides por tener pocos vertices, pero no lo son
void CrearPiramideTriangular()
{
	unsigned int indices_piramide_triangular[] = {
			0,1,2,
			1,3,2,
			3,0,2,
			1,0,3

	};
	GLfloat vertices_piramide_triangular[] = {
		-0.5f, -0.5f,0.0f,	//0 esq izq
		0.5f,-0.5f,0.0f,	//1 esq der
		0.0f,0.5f, -0.25f,	//2 punta
		0.0f,-0.5f,-0.87f,	//3 esq tras, se modifica el valor para hacer la piramide equilateral

	};
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(obj1);

}

//Crear cilindro, cono y esferas con arreglos dinámicos vector creados en el Semestre 2023 - 1 : por Sánchez Pérez Omar Alejandro
void CrearCilindro(int res, float R) {
	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//ciclo for para crear los vértices de las paredes del cilindro
	for (n = 0; n <= (res); n++) {
		if (n != res) {
			x = R * cos((n)*dt);
			z = R * sin((n)*dt);
		}
		//caso para terminar el círculo
		else {
			x = R * cos((0)*dt);
			z = R * sin((0)*dt);
		}
		for (i = 0; i < 6; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			case 3:
				vertices.push_back(x);
				break;
			case 4:
				vertices.push_back(0.5); //origen esta a la mitad y mide 1 de alto
				break;
			case 5:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia inferior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(-0.5f); //origen esta a la mitad y mide 1 de alto
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia superior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(0.5);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//Se generan los indices de los vértices
	for (i = 0; i < vertices.size(); i++) indices.push_back(i);

	//se genera el mesh del cilindro
	Mesh *cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
}

//función para crear un cono
void CrearCono(int res,float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;
	
	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//caso inicial para crear el cono
	//se crea vertice de la punta, la altura esta definida a 1 por default
	vertices.push_back(0.0);
	vertices.push_back(0.5);
	vertices.push_back(0.0);
	
	//ciclo for para crear los vértices de la circunferencia del cono
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}
	vertices.push_back(R * cos(0) * dt);
	vertices.push_back(-0.5);
	vertices.push_back(R * sin(0) * dt);

	for (i = 0; i < res+2; i++) indices.push_back(i);

	//se genera el mesh del cono
	Mesh *cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, vertices.size(), res + 2);
	meshList.push_back(cono);
}

//función para crear pirámide cuadrangular unitaria
void CrearPiramideCuadrangular()
{
	vector<unsigned int> piramidecuadrangular_indices = {
		0,3,4,
		3,2,4,
		2,1,4,
		1,0,4,
		0,1,2,
		0,2,4

	};
	vector<GLfloat> piramidecuadrangular_vertices = {
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,
		0.0f,0.5f,0.0f,
	};
	Mesh *piramide = new Mesh();
	piramide->CreateMeshGeometry(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramide);
}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	Shader* shader2 = new Shader();
	shader2->CreateFromFiles(vShaderColor, fShader);
	shaderList.push_back(*shader2);
}

void generarTriangulo(float R, float G, float B, float tX, float tY, float tZ, float deg, float rX, float rY, float rZ, float sX, float sY, float sZ, bool inv, int cara)
{
	/*
 	se crea esta función que permite generar las piramides necesarias sin necesidad de 
 	copiar y pegar todas las líneas. Se definen las variables uniform y los vectores ya que
   	si no lo hacía, el programa generaba las figuras "pegadas" a la cámara.
    
    	La función recibe como parámetros todos los valores necesarios para crear cualquier pirámide triangular.
     
     	La variable "inv" es una bandera que indica si el triangulo es invertido, entonces se hace la rotacion correspondiente.
      	La variable "cara" es un índice que indica en cual cara se va a colocar la pirámide, esto debido a que se debe hacer una 
        rotación adicional para la cara derecha.
	Las mismas rotaciones se hacen para las pirámides de la cara izquierda, sin embargo, esto simplemente se consigue cambiando el ángulo de
 	rotación de 120 a -120, pues la pirámide es simétrica. Esto se hace manualmente pues no es necesario escribir una función
  	que lo realice.
	*/
	//inicializar variables uniform
	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;
	shaderList[0].useShader();
	uniformModel = shaderList[0].getModelLocation();
	uniformProjection = shaderList[0].getProjectLocation();
	uniformView = shaderList[0].getViewLocation();
	uniformColor = shaderList[0].getColorLocation();
	glm::mat4 model(1.0);//Inicializar matriz de Modelo 4x4
	glm::vec3 color = glm::vec3(R,G,B); //inicializar Color para enviar a variable Uniform; negro de inicio
	//generacion de modelo
	model = glm::translate(model, glm::vec3(tX,tY,tZ)); //traslado
	model = glm::rotate(model, glm::radians(deg), glm::vec3(rX, rY, rZ)); //rotacion: grados, XYZ
	if (inv) //si es triangulo invertido...
		model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f)); //...se hace esa rotacion
	model = glm::scale(model, glm::vec3(sX, sY, sZ)); //escalado
	if (cara == 1) // si la cara es la derecha se hace una rotacion adicional
	{ 
		model = glm::rotate(model, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model)); //manda modelo al shader
	glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //se manda el nuevo color
	meshList[1]->RenderMesh(); //para piramide triangular se usa mesh

}

int main()
{
	mainWindow = Window(720, 720); //cuadrada para evitar escalado incorrecto
	mainWindow.Initialise();
	//Cilindro y cono reciben resolución (slices, rebanadas) y Radio de circunferencia de la base y tapa
	//orden influye en la lista, inicializacion de las primitivas
	//solo se tiene una meshlist porque no se utiliza meshcolor que recibe RGB
	CrearCubo();//índice 0 en MeshList
	CrearPiramideTriangular();//índice 1 en MeshList
	CrearCilindro(40, 1.0f);//índice 2 en MeshList
	CrearCono(25, 2.0f);//índice 3 en MeshList
	CrearPiramideCuadrangular();//índice 4 en MeshList
	CreateShaders();
	
	/*Cámara se usa el comando: glm::lookAt(vector de posición, vector de orientación, vector up));
	En la clase Camera se reciben 5 datos:
	glm::vec3 vector de posición,
	glm::vec3 vector up,
	GlFloat yaw rotación para girar hacia la derecha e izquierda
	GlFloat pitch rotación para inclinar hacia arriba y abajo
	GlFloat velocidad de desplazamiento,
	GlFloat velocidad de vuelta o de giro
	Se usa el Mouse y las teclas WASD y su posición inicial está en 0,0,1 y ve hacia 0,0,-1.
	*/

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.3f); //aqui se modifican velocidades

	//inicializar variables uniform
	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;
	//proyeccion en perspectiva, ortogonal ya nel
	glm::mat4 projection = glm::perspective(glm::radians(60.0f)	,mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	//glm::mat4 projection = glm::ortho(-1, 1, -1, 1, 1, 10);
	
	//Loop mientras no se cierra la ventana
	sp.init(); //inicializar esfera
	sp.load();//enviar la esfera al shader

	glm::mat4 model(1.0);//Inicializar matriz de Modelo 4x4
	glm::vec3 color = glm::vec3(0.0f,0.0f,0.0f); //inicializar Color para enviar a variable Uniform; negro de inicio

	while (!mainWindow.getShouldClose())
	{
		
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		//Recibir eventos del usuario
		glfwPollEvents();
		//Cámara
		//recibiendo informacion 
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//Limpiar la ventana
		glClearColor(0.9f, 0.9f, 1.0f, 1.0f); //fondo claro
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Se agrega limpiar el buffer de profundidad
		//las 5 lineas se podrian declarar fuera del while en este caso porque solo hay un shader
		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		model = glm::mat4(1.0);
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));  //al presionar la tecla Y se rota sobre el eje y
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		//la línea de proyección solo se manda una vez a menos que en tiempo de ejecución
		//se programe cambio entre proyección ortogonal y perspectiva
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		//piramide mayor negra
		generarTriangulo(0.0f, 0.0f, 0.1f,		 //RGB 0.0f, 0.0f, 0.1f,
						 0.0f, 0.0f, 0.0f,		 //traslacion X,Y,Z
						 0.0f, 0.0f, 1.0f, 0.0f, //rotacion: grados,X,Y,Z
						 2.0f, 2.0f, 2.0f,		 //escalar X,Y,Z
						 false, 0);				 //si es triangulo invertido

		//triangulos de cara roja
		//triangulo medio en punta
		generarTriangulo(1.0f, 0.1f, 0.1f,		 
						 0.0f, 0.55f, -0.36f,		 
						 -11.0f, 1.0f, 0.0f, 0.0f, 
						 0.55f, 0.55f, 0.05f,		 
						 false, 0);

		//piramide invertida medio medio
		generarTriangulo(1.0f, 0.1f, 0.1f,
			0.0f, -0.07f, -0.195f,
			-11.0f, 1.0f, 0.0f, 0.0f,
			0.55f, 0.55f, 0.05f,
			true, 0);

		//triangulo derecho en medio
		generarTriangulo(1.0f, 0.1f, 0.1f,
			0.33f, -0.07f, -0.195f,
			-11.0f, 1.0f, 0.0f, 0.0f,
			0.55f, 0.55f, 0.05f,
			false, 0);

		//triangulo izquierda en medio 
		generarTriangulo(1.0f, 0.1f, 0.1f,
			-0.33f, -0.07f, -0.195f,
			-11.0f, 1.0f, 0.0f, 0.0f,
			0.55f, 0.55f, 0.05f,
			false, 0);

		//triangulo orilla derecha abajo
		generarTriangulo(1.0f, 0.1f, 0.1f,
			0.64f, -0.67f, -0.05f,
			-11.0f, 1.0f, 0.0f, 0.0f,
			0.55f, 0.55f, 0.05f,
			false, 0);

		//triangulo orilla izquierda abajo
		generarTriangulo(1.0f, 0.1f, 0.1f,
			-0.64f, -0.67f, -0.05f,
			-11.0f, 1.0f, 0.0f, 0.0f,
			0.55f, 0.55f, 0.05f,
			false, 0);

		//triangulo medio abajo
		generarTriangulo(1.0f, 0.1f, 0.1f,
			0.0f, -0.67f, -0.05f,
			-11.0f, 1.0f, 0.0f, 0.0f,
			0.55f, 0.55f, 0.05f,
			false, 0);

		//triangulo invertida derecha abajo
		generarTriangulo(1.0f, 0.1f, 0.1f,
			0.33f, -0.67f, -0.05f,
			-11.0f, 1.0f, 0.0f, 0.0f,
			0.55f, 0.55f, 0.05f,
			true, 0);

		//triangulo invertida izquierda abajo
		generarTriangulo(1.0f, 0.1f, 0.1f,
			-0.33f, -0.67f, -0.05f,
			-11.0f, 1.0f, 0.0f, 0.0f,
			0.55f, 0.55f, 0.05f,
			true, 0);

		//triangulos de cara azul
		//triangulo medio en punta
		generarTriangulo(0.1f, 0.1f, 1.0f,
			0.19f, 0.55f, -0.65f,
			120.0f, 0.0f, 1.0f, 0.0f,
			0.55, 0.55f, 0.55f,
			false, 0);
		

		//triangulo izquierdo medio
		generarTriangulo(0.1f, 0.1f, 1.0f,
			0.56f, -0.07f, -0.46f,
			120.0f, 0.0f, 1.0f, 0.0f,
			0.55, 0.55f, 0.55f,
			false, 0);
		

		//triangulo derecho medio
		generarTriangulo(0.1f, 0.1f, 1.0f,
			0.2f, -0.07f, -1.0f,
			120.0f, 0.0f, 1.0f, 0.0f,
			0.55, 0.55f, 0.55f,
			false, 0);
		
	
		//piramide invertida medio medio
		generarTriangulo(0.1f, 0.1f, 1.0f,
			0.35f, -0.035f, -0.71f,
			120.0f, 0.0f, 1.0f, 0.0f,
			0.55, 0.55f, 0.55f,
			true, 1);

		//triangulo orilla derecha abajo
		generarTriangulo(0.1f, 0.1f, 1.0f,
			0.21f, -0.67f, -1.37f,
			120.0f, 0.0f, 1.0f, 0.0f,
			0.52, 0.55f, 0.52f,
			false, 0);

		//triangulo orilla izquierda abajo
		generarTriangulo(0.1f, 0.1f, 1.0f,
			0.83f, -0.67f, -0.28f,
			120.0f, 0.0f, 1.0f, 0.0f,
			0.52, 0.55f, 0.52f,
			false, 0);

		//triangulo medio abajo
		generarTriangulo(0.1f, 0.1f, 1.0f,
			0.53f, -0.67f, -0.82f,
			120.0f, 0.0f, 1.0f, 0.0f,
			0.55, 0.55f, 0.55f,
			false, 0);

		//triangulo invertida izquierda abajo
		generarTriangulo(0.1f, 0.1f, 1.0f,
			0.71f, -0.63f, -0.55f,
			120.0f, 0.0f, 1.0f, 0.0f,
			0.55, 0.55f, 0.55f,
			true, 1);

		//triangulo invertida derecha abajo
		generarTriangulo(0.1f, 0.1f, 1.0f,
			0.39f, -0.63f, -1.11f,
			120.0f, 0.0f, 1.0f, 0.0f,
			0.55, 0.55f, 0.55f,
			true, 1);

		//triangulos de cara amarilla
		//triangulo medio en punta
		generarTriangulo(1.0f, 1.0f, 0.1f,
			-0.19f, 0.55f, -0.65f,
			-120.0f, 0.0f, 1.0f, 0.0f,
			0.55, 0.55f, 0.55f,
			false, 0);

		
		//triangulo izquierdo medio
		generarTriangulo(1.0f, 1.0f, 0.1f,
			-0.56f, -0.07f, -0.46f,
			-120.0f, 0.0f, 1.0f, 0.0f,
			0.55, 0.55f, 0.55f,
			false, 0);

		
		//triangulo derecho medio
		generarTriangulo(1.0f, 1.0f, 0.1f,
			-0.2f, -0.07f, -1.0f,
			-120.0f, 0.0f, 1.0f, 0.0f,
			0.55, 0.55f, 0.55f,
			false, 0);


		//piramide invertida medio medio
		generarTriangulo(1.0f, 1.0f, 0.1f,
			-0.35f, -0.035f, -0.71f,
			-120.0f, 0.0f, 1.0f, 0.0f,
			0.55, 0.55f, 0.55f,
			true, 1);

		//triangulo orilla derecha abajo
		generarTriangulo(1.0f, 1.0f, 0.1f,
			-0.21f, -0.67f, -1.37f,
			-120.0f, 0.0f, 1.0f, 0.0f,
			0.52, 0.55f, 0.52f,
			false, 0);

		//triangulo orilla izquierda abajo
		generarTriangulo(1.0f, 1.0f, 0.1f,
			-0.83f, -0.67f, -0.28f,
			-120.0f, 0.0f, 1.0f, 0.0f,
			0.52, 0.55f, 0.52f,
			false, 0);

		//triangulo medio abajo
		generarTriangulo(1.0f, 1.0f, 0.1f,
			-0.53f, -0.67f, -0.82f,
			-120.0f, 0.0f, 1.0f, 0.0f,
			0.55, 0.55f, 0.55f,
			false, 0);

		//triangulo invertida izquierda abajo
		generarTriangulo(1.0f, 1.0f, 0.1f,
			-0.71f, -0.63f, -0.55f,
			-120.0f, 0.0f, 1.0f, 0.0f,
			0.55, 0.55f, 0.55f,
			true, 1);

		//triangulo invertida derecha abajo
		generarTriangulo(1.0f, 1.0f, 0.1f,
			-0.39f, -0.63f, -1.11f,
			-120.0f, 0.0f, 1.0f, 0.0f,
			0.55, 0.55f, 0.55f,
			true, 1);

		//triangulos de base/ cara rosa
		//triangulo medio en punta
		generarTriangulo(0.9f, 0.1f, 0.6f,
			0.0f, -0.75f, -1.1f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.55, 0.55f, 0.55f,
			false, 0);


		//triangulo izquierdo medio
		generarTriangulo(0.9f, 0.1f, 0.6f,
			0.30f, -0.75f, -0.55f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.55, 0.55f, 0.55f,
			false, 0);


		//triangulo derecho medio
		generarTriangulo(0.9f, 0.1f, 0.6f,
			-0.30f, -0.75f, -0.55f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.55, 0.55f, 0.55f,
			false, 0);


		//piramide invertida medio medio
		generarTriangulo(0.9f, 0.1f, 0.6f,
			0.0f, -0.75f, -1.05f,
			180.0f, 0.0f, 1.0f, 0.0f,
			0.55, 0.55f, 0.55f,
			false, 0);

		//triangulo orilla derecha abajo
		generarTriangulo(0.9f, 0.1f, 0.6f,
			-0.59f, -0.75f, -0.04f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.55, 0.55f, 0.55f,
			false, 0);


		//triangulo orilla izquierda abajo
		generarTriangulo(0.9f, 0.1f, 0.6f,
			0.59f, -0.75f, -0.04f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.55, 0.55f, 0.55f,
			false, 0);

		//triangulo medio abajo
		generarTriangulo(0.9f, 0.1f, 0.6f,
			0.0f, -0.75f, -0.04f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.55, 0.55f, 0.55f,
			false, 0);

		//triangulo invertida izquierda abajo
		generarTriangulo(0.9f, 0.1f, 0.6f,
			-0.30f, -0.75f, -0.52f,
			180.0f, 0.0f, 1.0f, 0.0f,
			0.55, 0.55f, 0.55f,
			false, 0);


		//triangulo invertida derecha abajo
		generarTriangulo(0.9f, 0.1f, 0.6f,
			0.30f, -0.75f, -0.52f,
			180.0f, 0.0f, 1.0f, 0.0f,
			0.55, 0.55f, 0.55f,
			false, 0);
		
		/*
		color = glm::vec3(1.0f, 0.1f, 0.1f); //rojillo
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -0.035f, -0.23f)); //traslado
		model = glm::rotate(model, glm::radians(-14.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //rotacion: grados, XYZ
		model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f)); //rotacion: grados, XYZ
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.005f)); //escalado
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model)); //manda modelo al shader
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //se manda el nuevo color
		meshList[1]->RenderMesh(); //para piramide triangular
		*/

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}
