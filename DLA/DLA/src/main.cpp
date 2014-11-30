#include "CommonIncludes.h"
#include "DLAWindow.h"
#include <qapplication.h>

/*********************************************************************************************
*********************************************************************************************/
void MyGLFWErrorCallback(int error, const char* description)
{
	std::cerr << description << std::endl;
}

/*********************************************************************************************
*********************************************************************************************/
int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	DLAWindow stDLAWindow;
	stDLAWindow.resize(stDLAWindow.sizeHint());

	stDLAWindow.show();

	return app.exec();

	// Set glfw error callback
	/*glfwSetErrorCallback(MyGLFWErrorCallback);

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
	GLFWwindow * pWindow = glfwCreateWindow(800, 800, "Diffusion Limited Aggregation (DLA)", NULL, NULL);
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

	//DLA stDLA(100, 100, DLA::START_REGION_CIRCLE, 200000000, glm::ivec2(50, 50));
	DLA stDLA(800, 800, DLA::START_REGION_CIRCLE, 2000000, glm::ivec2(400, 400), glm::ivec2(500, 500));
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	bool bPaused = false;
	bool bCanBePaused = true;

	do
	{
	if (glfwGetKey(pWindow, GLFW_KEY_R))
	stDLA.Reset();

	int iSpaceKeyState = glfwGetKey(pWindow, GLFW_KEY_SPACE);

	if (iSpaceKeyState == GLFW_RELEASE)
	{
	bCanBePaused = true;
	}
	else if (iSpaceKeyState == GLFW_PRESS)
	{
	if (bCanBePaused == true)
	{
	bPaused = !bPaused;
	bCanBePaused = false;

	if (!bPaused)
	glfwSetWindowTitle(pWindow, "Diffusion Limited Aggregation (DLA)");
	else
	glfwSetWindowTitle(pWindow, "Diffusion Limited Aggregation (DLA) (paused)");
	}
	}


	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!bPaused)
	stDLA.CalculateNextElement();

	stDLA.Render();

	// Swap buffers
	glfwSwapBuffers(pWindow);
	glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(pWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(pWindow) == 0);

	glfwDestroyWindow(pWindow);
	glfwTerminate();
	exit(EXIT_SUCCESS);
	*/
	return app.exec();
}

