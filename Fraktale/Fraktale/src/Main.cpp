#include "CommonIncludes.h"
#include "DLA.h"

/*********************************************************************************************
*********************************************************************************************/
void MyGLFWErrorCallback(int error, const char* description)
{
	std::cerr << description << std::endl;
}

/*********************************************************************************************
*********************************************************************************************/
int main()
{
	// Set glfw error callback
	glfwSetErrorCallback(MyGLFWErrorCallback);

	// Initialize GLFW
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFM." << std::endl;
		exit(EXIT_FAILURE);
	}

	// Window hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Open a window
	GLFWwindow * pWindow = glfwCreateWindow(400, 400, "Diffusion Limited Aggregation (DLA)", NULL, NULL);
	if (pWindow == NULL)
	{
		std::cerr << "Failed to open GLFM window." << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//set the window as context
	glfwMakeContextCurrent(pWindow);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW." << std::endl;
		exit(EXIT_FAILURE);
	}

	glfwSetInputMode(pWindow, GLFW_STICKY_KEYS, GL_TRUE);

	DLA stDLA(400, 400);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	do
	{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		stDLA.RenderNextElement();

		// Swap buffers
		glfwSwapBuffers(pWindow);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(pWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(pWindow) == 0);
	
	glfwDestroyWindow(pWindow);
	glfwTerminate();
	exit(EXIT_SUCCESS);

	return 0;
}

