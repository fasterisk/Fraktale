#include "Julia.h"
#include <qevent.h>
#include <time.h>
#include <algorithm>

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

/*********************************************************************************************
*********************************************************************************************/
Julia::Julia(int iResolutionX, int iResolutionY, unsigned int nMaxIterations, QWidget * pParent)
	: QGLWidget(pParent)
{
	assert(iResolutionX > 0);
	assert(iResolutionY > 0);

	m_iResolutionX = iResolutionX;
	m_iResolutionY = iResolutionY;
	m_nMaxIterations = nMaxIterations;

	m_fOffsetX = 0.0f;
	m_fOffsetY = 0.0f;
	m_fScale = 1.0f;

	m_pTransferFunction = new TransferFunctionCustom2();

	m_fComplexReal = 0.0f;
	m_fComplexImag = 0.0f;
}

/*********************************************************************************************
*********************************************************************************************/
Julia::~Julia()
{
	delete m_pTransferFunction;
}

/*********************************************************************************************
*********************************************************************************************/
void	Julia::GetResolution(int & riResolutionX, int & riResolutionY) const
{
	riResolutionX = m_iResolutionX;
	riResolutionY = m_iResolutionY;
}

/*********************************************************************************************
*********************************************************************************************/
void	Julia::SetResolution(int iResolutionX, int iResolutionY)
{
	if (m_iResolutionX != iResolutionX || m_iResolutionY != iResolutionY)
	{
		m_iResolutionX = iResolutionX;
		m_iResolutionY = iResolutionY;
		Reset();
	}
}

/*********************************************************************************************
*********************************************************************************************/
void	Julia::SetMaxNumIterations(int iNumIterations)
{
	assert(iNumIterations > 0);
	if (m_nMaxIterations != iNumIterations)
	{
		m_nMaxIterations = iNumIterations;
	}
}

/*********************************************************************************************
*********************************************************************************************/
void	Julia::SetOffset(float fXOffset, float fYOffset)
{
	m_fOffsetX = fXOffset;
	m_fOffsetY = fYOffset;
}

/*********************************************************************************************
*********************************************************************************************/
void	Julia::SetScale(float fScale)
{
	m_fScale = fScale;
}

/*********************************************************************************************
*********************************************************************************************/
void Julia::Render()
{
	glUseProgram(m_glnShader);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_glnVertexBuffer);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
		);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_1D, m_glnTransferFunctionTexture);
	glUniform1d(m_gliLookupTableLoc, GL_TEXTURE0);
	glUniform1f(m_gliMaxIterationsLoc, (float)m_nMaxIterations);
	glUniform1f(m_gliScaleLoc, m_fScale);
	glUniform2f(m_gliOffsetLoc, m_fOffsetX, m_fOffsetY);
	glUniform2f(m_gliComplexLoc, m_fComplexReal, m_fComplexImag);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(0);
}

/*********************************************************************************************
*********************************************************************************************/
void	Julia::Reset()
{
}

/*********************************************************************************************
*********************************************************************************************/
void	Julia::SetComplex(float fReal, float fImag)
{
	m_fComplexReal = fReal;
	m_fComplexImag = fImag;
}

/*********************************************************************************************
*********************************************************************************************/
QSize	Julia::minimumSizeHint() const
{
	return QSize(m_iResolutionX, m_iResolutionY);
}

/*********************************************************************************************
*********************************************************************************************/
QSize	Julia::sizeHint() const
{
	return QSize(m_iResolutionX, m_iResolutionY);
}

/*********************************************************************************************
*********************************************************************************************/
void	Julia::initializeGL()
{
	//Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW." << std::endl;
	}

	ItlInitializeShaderAndTextures();
}

/*********************************************************************************************
*********************************************************************************************/
void	Julia::paintGL()
{
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Render();

	update();
}

/*********************************************************************************************
*********************************************************************************************/
void	Julia::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
}

/*********************************************************************************************
*********************************************************************************************/
void	Julia::mousePressEvent(QMouseEvent * event)
{
	m_iMouseXBefore = event->x();
	m_iMouseYBefore = event->y();
}

/*********************************************************************************************
*********************************************************************************************/
void	Julia::mouseMoveEvent(QMouseEvent * event)
{
	int iDiffX = event->x() - m_iMouseXBefore;
	int iDiffY = event->y() - m_iMouseYBefore;

	m_fOffsetX -= (iDiffX / (float)m_iResolutionX) * m_fScale;
	m_fOffsetY += (iDiffY / (float)m_iResolutionY) * m_fScale;

	m_iMouseXBefore = event->x();
	m_iMouseYBefore = event->y();
}

/*********************************************************************************************
*********************************************************************************************/
void	Julia::mouseReleaseEvent(QMouseEvent * event)
{

}

/*********************************************************************************************
*********************************************************************************************/
void	Julia::wheelEvent(QWheelEvent * event)
{
	int iNumDegrees = event->delta() / 8;
	int iNumSteps = iNumDegrees / 15;
	iNumDegrees *= 20.0f;

	m_fScale = std::max(0.0f, m_fScale * (1.0f - iNumSteps / 100.0f));
}

/*********************************************************************************************
*********************************************************************************************/
void	Julia::ItlInitializeShaderAndTextures()
{
	static const GLfloat glfVertexBufferData[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f
	};

	///VAO / VBO
	glGenVertexArrays(1, &m_glnVertexArray);
	glBindVertexArray(m_glnVertexArray);

	glGenBuffers(1, &m_glnVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_glnVertexBuffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(glfVertexBufferData), glfVertexBufferData, GL_STATIC_DRAW);

	/// Ping-pong textures

	//Generate frame buffers and textures
	/*glGenFramebuffers(2, m_glnPingPongFramebuffers);
	glGenTextures(2, m_glnPingPongTextures);

	//Ping pong 1
	glBindFramebuffer(GL_FRAMEBUFFER, m_glnPingPongFramebuffers[0]);
	glBindTexture(GL_TEXTURE_2D, m_glnPingPongTextures[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_iResolutionX, m_iResolutionY, 0, GL_RGBA, GL_FLOAT, NULL);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_glnPingPongTextures[0], 0);

	//GLenum gleDrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	//glDrawBuffers(1, gleDrawBuffers);

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	//Ping pong 2
	glBindFramebuffer(GL_FRAMEBUFFER, m_glnPingPongFramebuffers[1]);
	glBindTexture(GL_TEXTURE_2D, m_glnPingPongTextures[1]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_iResolutionX, m_iResolutionY, 0, GL_RGBA, GL_FLOAT, NULL);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_glnPingPongTextures[1], 0);

	//glDrawBuffers(1, gleDrawBuffers);

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);*/

	//Transfer function texture
	glEnable(GL_TEXTURE_1D);
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &m_glnTransferFunctionTexture);
	glBindTexture(GL_TEXTURE_1D, m_glnTransferFunctionTexture);

	float * pTransferFunctionTexture = new float[1024 * 3];
	for (unsigned int n = 0; n < 1024; ++n)
	{
		//pTransferFunctionTexture[n * 3 + 0] = 0.0f;
		//pTransferFunctionTexture[n * 3 + 1] = 0.0f;
		//pTransferFunctionTexture[n * 3 + 2] = (1024.0f - n) / 1024.0f;

		m_pTransferFunction->GetColor(n / (float)1024, pTransferFunctionTexture[3 * n], pTransferFunctionTexture[3 * n + 1], pTransferFunctionTexture[3 * n + 2]);
	}

	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 1024, 0, GL_RGB, GL_FLOAT, pTransferFunctionTexture);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	/*m_glnPass1Shader = ItlCreateShader("shader/VSRenderPass1.vert", "shader/FSRenderPass1.frag");
	m_glnPass2Shader = ItlCreateShader("shader/VSRenderPass2.vert", "shader/FSRenderPass2.frag");
	m_glnPass3Shader = ItlCreateShader("shader/VSRenderPass3.vert", "shader/FSRenderPass3.frag");


	m_gliPass2InputTexLoc = glGetUniformLocation(m_glnPass2Shader, "tInputTexture");
	m_gliPass2CurIterationLoc = glGetUniformLocation(m_glnPass2Shader, "fCurIteration");

	m_gliPass3InputTexLoc = glGetUniformLocation(m_glnPass3Shader, "tInputTexture");
	m_gliPass3InsideColorLoc = glGetUniformLocation(m_glnPass3Shader, "v4InsideColor");
	m_gliPass3ColorTableLoc = glGetUniformLocation(m_glnPass3Shader, "tColorTable");
	m_gliPass3MaxIterationsLoc = glGetUniformLocation(m_glnPass3Shader, "fMaxIterations");*/

	m_glnShader = ItlCreateShader("shader/Julia.vert", "shader/Julia.frag");

	m_gliMaxIterationsLoc = glGetUniformLocation(m_glnShader, "fMaxIterations");
	m_gliLookupTableLoc = glGetUniformLocation(m_glnShader, "tLookupTable");
	m_gliScaleLoc = glGetUniformLocation(m_glnShader, "fScale");
	m_gliOffsetLoc = glGetUniformLocation(m_glnShader, "v2Offset");
	m_gliComplexLoc = glGetUniformLocation(m_glnShader, "v2Complex");
}

/*********************************************************************************************
*********************************************************************************************/
GLuint	Julia::ItlLoadShader(std::string sPath, int iShaderType)
{
	GLuint glnShader = glCreateShader(iShaderType);

	std::string sShaderCode;
	std::ifstream sShaderStream(sPath, std::ios::in);
	if (sShaderStream.is_open())
	{
		std::string sLine = "";
		while (getline(sShaderStream, sLine))
			sShaderCode += "\n" + sLine;
		sShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Shader
	std::cout << "Compiling shader: " << sPath << std::endl;
	char const * pSourcePointer = sShaderCode.c_str();
	glShaderSource(glnShader, 1, &pSourcePointer, NULL);
	glCompileShader(glnShader);

	// Check Shader
	glGetShaderiv(glnShader, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(glnShader, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> vShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(glnShader, InfoLogLength, NULL, &vShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &vShaderErrorMessage[0]);

	return glnShader;
}

/*********************************************************************************************
*********************************************************************************************/
GLuint	Julia::ItlCreateShader(std::string sVertexShaderPath, std::string sFragmentShaderPath)
{
	GLuint glnVertexShader = ItlLoadShader(sVertexShaderPath, GL_VERTEX_SHADER);
	GLuint glnFragmentShader = ItlLoadShader(sFragmentShaderPath, GL_FRAGMENT_SHADER);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Link the program
	fprintf(stdout, "Linking program\n");
	GLuint glnShader = glCreateProgram();
	glAttachShader(glnShader, glnVertexShader);
	glAttachShader(glnShader, glnFragmentShader);
	glLinkProgram(glnShader);

	// Check the program
	glGetProgramiv(glnShader, GL_LINK_STATUS, &Result);
	glGetProgramiv(glnShader, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> vProgramErrorMessage(std::max(InfoLogLength, int(1)));
	glGetProgramInfoLog(glnShader, InfoLogLength, NULL, &vProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &vProgramErrorMessage[0]);

	glDeleteShader(glnVertexShader);
	glDeleteShader(glnFragmentShader);

	return glnShader;
}