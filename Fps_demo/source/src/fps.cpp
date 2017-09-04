#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>


void error_callback(int error, const char * descr) 
{
	std::cerr << "GLFW ERROR: " << descr << std::endl;
}


int main() 
{
	glfwSetErrorCallback(error_callback);
	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow * window = glfwCreateWindow(640, 480, "Fps Demo", NULL, NULL);

	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);


	glewExperimental = GL_TRUE; // allow to use all of opengl core profile
	
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failedto initialize GLEW" << std::endl;
		return -1;
	}
	
	int vport_width, vport_height;
	glfwGetFramebufferSize(window, &vport_width, &vport_height);

	glViewport(0, 0, vport_width, vport_height);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glfwSwapBuffers(window);

	}
	glfwTerminate();
	return 0;
}