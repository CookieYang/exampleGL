#pragma once

#include "Tool.h"
#include "Object.h"
#include "Camera.h"
namespace baseClass
{

	class application
	{
	private:


	public:
		application() {}
		virtual ~application() {}

		virtual int run(baseClass::application* the_app)
		{
			glfwInit();

			window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
			if (window == NULL) {
				std::cout << "Failed to create GLFW window" << std::endl;
				return -1;
			}

			glfwMakeContextCurrent(window);
		//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glewInit();
			glViewport(0, 0, 800, 600);
			start();
			glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
			glfwSetCursorPosCallback(window, Camera::processMousePosition);
			glfwSetKeyCallback(window, Camera::processKeyboardInput);
			glfwSetScrollCallback(window, Camera::processScroll);
			while (!glfwWindowShouldClose(window)) {
				processInput(window);
				render(glfwGetTime());
				glfwSwapBuffers(window);
				glfwPollEvents();
			}
			shutdown();
			glfwTerminate();
			return 0;
		}

		virtual void start()
		{

		}

		virtual void render(double currentTime)
		{

		}

		virtual void shutdown()
		{

		}

		virtual	void processInput(GLFWwindow* window) {
			if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
				glfwSetWindowShouldClose(window, true);
			}
		}

	protected:
		static  baseClass::application * app;
		GLFWwindow* window;

		static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
			glViewport(0, 0, width, height);
		}
	};

};


#define DECLARE_MAIN(a)                             \
int main(int argc, const char ** argv)              \
{                                                   \
    a *app = new a;                                 \
    app->run(app);                                  \
    delete app;                                     \
    return 0;                                       \
}



