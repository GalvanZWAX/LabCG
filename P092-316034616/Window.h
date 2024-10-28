#pragma once
#include<stdio.h>
#include<random>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getmuevex() { return muevex; }
	GLfloat getmuevez() { return muevez; }
	GLfloat getRotaCofre() { return rotaCofre; }
	int getNumDado() { return numDado; }
	bool getTiroDado() { return tiroDado; }
	void resetTiroDado() { tiroDado = false; }
	bool getBanderaLuz() { return banderaLuz; };
	bool getBanderaBoton() { return banderaBoton; };
	bool getBanderaFaroD() { return banderaFaroD; };
	bool getBanderaFaroT() { return banderaFaroT; };
	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
	
	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat muevex;
	GLfloat muevez;
	GLfloat rotaCofre;
	bool banderaLuz;
	bool banderaBoton;
	bool banderaFaroD;
	bool banderaFaroT;
	bool mouseFirstMoved;
	bool tiroDado;
	int numDado;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);
};

