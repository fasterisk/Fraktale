#include "Mandelbrot.h"
#include "MJWindow.h"
#include <qevent.h>
#include <time.h>
#include <algorithm>

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

#define SUPERSAMPLING 2

/*********************************************************************************************
*********************************************************************************************/
Mandelbrot::Mandelbrot(int iResolutionX, int iResolutionY, unsigned int nMaxIterations, MJWindow * pParent)
	: QGLWidget(pParent)
{
	assert(iResolutionX > 0);
	assert(iResolutionY > 0);

	m_iResolutionX = iResolutionX;
	m_iResolutionY = iResolutionY;
	m_nMaxIterations = nMaxIterations;

	m_fOffsetX = -0.5f;
	m_fOffsetY = 0.0f;
	m_fScale = 1.2f;

	m_pTransferFunction = new TransferFunctionHSVToRGB();

	m_pParentWindow = pParent;

	m_bLeftMousePressed = false;
	m_bMiddleMousePressed = false;

	m_bUp2Date = false;
	m_bRenderPreview = true;
	m_bGaussianEnabled = true;
	m_bKeyPressed = false;

	m_fTFPar1 = 200;
	m_fTFPar2 = 0.0f;

	setFocusPolicy(Qt::StrongFocus);
}

/*********************************************************************************************
*********************************************************************************************/
Mandelbrot::~Mandelbrot()
{
	delete m_pTransferFunction;
}

/*********************************************************************************************
*********************************************************************************************/
void	Mandelbrot::GetResolution(int & riResolutionX, int & riResolutionY) const
{
	riResolutionX = m_iResolutionX;
	riResolutionY = m_iResolutionY;
}

/*********************************************************************************************
*********************************************************************************************/
void	Mandelbrot::SetResolution(int iResolutionX, int iResolutionY)
{
	if (m_iResolutionX != iResolutionX || m_iResolutionY != iResolutionY)
	{
		m_iResolutionX = iResolutionX;
		m_iResolutionY = iResolutionY;
		Reset();

		m_bUp2Date = false;
		m_bRenderPreview = true;
	}
}

/*********************************************************************************************
*********************************************************************************************/
void	Mandelbrot::SetMaxNumIterations(int iNumIterations)
{
	assert(iNumIterations > 0);
	if (m_nMaxIterations != iNumIterations)
	{
		m_nMaxIterations = iNumIterations;

		m_bUp2Date = false;
		m_bRenderPreview = true;
	}
}

/*********************************************************************************************
*********************************************************************************************/
void	Mandelbrot::SetOffset(float fXOffset, float fYOffset)
{
	if (fXOffset != m_fOffsetX || fYOffset != m_fOffsetY)
	{
		m_fOffsetX = fXOffset;
		m_fOffsetY = fYOffset;

		m_bUp2Date = false;
		m_bRenderPreview = true;
	}
}

/*********************************************************************************************
*********************************************************************************************/
void	Mandelbrot::SetScale(float fScale)
{
	if (fScale != m_fScale)
	{
		m_fScale = fScale;

		m_bUp2Date = false;
		m_bRenderPreview = true;
	}
}

/*********************************************************************************************
*********************************************************************************************/
void Mandelbrot::Render()
{
	if (!m_bUp2Date)
	{
		int iSuperSampling = SUPERSAMPLING;
		unsigned int nIterations = m_nMaxIterations;

		if (m_bRenderPreview)
		{
			iSuperSampling = 1;
			nIterations = 5000;
		}

		///////////////////////////////////////////////////////////////////////////////////
		//Render mandelbrot
		///////////////////////////////////////////////////////////////////////////////////
		ItlRenderMandelbrotSet(m_iResolutionX * iSuperSampling, m_iResolutionY * iSuperSampling, nIterations, 0);

		///////////////////////////////////////////////////////////////////////////////////
		//Apply gaussian blur
		///////////////////////////////////////////////////////////////////////////////////
		if (m_bGaussianEnabled && !m_bRenderPreview)
		{
			ItlRenderGaussian(m_iResolutionX * iSuperSampling, m_iResolutionY * iSuperSampling, 1);
			m_iCurrentScreenTexture = 1;
		}
		else
		{
			m_iCurrentScreenTexture = 0;
		}


		if (m_bRenderPreview)
			m_bRenderPreview = false;
		else
			m_bUp2Date = true;
	}
	
	///////////////////////////////////////////////////////////////////////////////////
	// Render to screen
	///////////////////////////////////////////////////////////////////////////////////
	ItlRenderTextureToScreen(m_iCurrentScreenTexture);
	
}

/*********************************************************************************************
*********************************************************************************************/
void	Mandelbrot::Reset()
{
}

/*********************************************************************************************
*********************************************************************************************/
QSize	Mandelbrot::minimumSizeHint() const
{
	return QSize(m_iResolutionX, m_iResolutionY);
}

/*********************************************************************************************
*********************************************************************************************/
QSize	Mandelbrot::sizeHint() const
{
	return QSize(m_iResolutionX, m_iResolutionY);
}

/*********************************************************************************************
*********************************************************************************************/
void	Mandelbrot::initializeGL()
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
void	Mandelbrot::paintGL()
{
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Render();

	update();
}

/*********************************************************************************************
*********************************************************************************************/
void	Mandelbrot::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
}

/*********************************************************************************************
*********************************************************************************************/
void	Mandelbrot::mousePressEvent(QMouseEvent * event)
{
	m_iMouseXBefore = event->x();
	m_iMouseYBefore = event->y();

	if (event->button() == Qt::MouseButton::LeftButton)
	{
		m_bLeftMousePressed = true;

		int iMouseX = event->x();
		int iMouseY = event->y();

		float fRelMouseX = iMouseX / (float)m_iResolutionX;
		float fRelMouseY = iMouseY / (float)m_iResolutionY;

		float fReal = fRelMouseX * 2.0f - 1.0f;
		float fImag = fRelMouseY * 2.0f - 1.0f;

		fReal = fReal * m_fScale + m_fOffsetX;
		fImag = -1.0f * (fImag * m_fScale - m_fOffsetY);

		m_pParentWindow->OnComplexChanged(fReal, fImag);
	}
	else if (event->button() == Qt::MouseButton::MiddleButton)
	{
		m_bMiddleMousePressed = true;
	}
}

/*********************************************************************************************
*********************************************************************************************/
void	Mandelbrot::mouseMoveEvent(QMouseEvent * event)
{
	if (m_bMiddleMousePressed)
	{
		int iDiffX = event->x() - m_iMouseXBefore;
		int iDiffY = event->y() - m_iMouseYBefore;


		SetOffset(m_fOffsetX - (iDiffX / (float)m_iResolutionX) * m_fScale,
				  m_fOffsetY + (iDiffY / (float)m_iResolutionY) * m_fScale);

		m_iMouseXBefore = event->x();
		m_iMouseYBefore = event->y();
	}
	else if (m_bLeftMousePressed)
	{
		int iMouseX = event->x();
		int iMouseY = event->y();

		float fRelMouseX = iMouseX / (float)m_iResolutionX;
		float fRelMouseY = iMouseY / (float)m_iResolutionY;

		float fReal = fRelMouseX * 2.0f - 1.0f;
		float fImag = fRelMouseY * 2.0f - 1.0f;

		fReal = fReal * m_fScale + m_fOffsetX;
		fImag = -1.0f * (fImag * m_fScale - m_fOffsetY);

		m_pParentWindow->OnComplexChanged(fReal, fImag);
	}
}

/*********************************************************************************************
*********************************************************************************************/
void	Mandelbrot::mouseReleaseEvent(QMouseEvent * event)
{
	if (event->button() == Qt::MouseButton::LeftButton)
	{
		m_bLeftMousePressed = false;
	}
	else if (event->button() == Qt::MouseButton::MiddleButton)
	{
		m_bMiddleMousePressed = false;
	}
}

/*********************************************************************************************
*********************************************************************************************/
void	Mandelbrot::wheelEvent(QWheelEvent * event)
{
	int iNumDegrees = event->delta() / 8;
	int iNumSteps = iNumDegrees / 15;
	iNumSteps *= 10.0f;

	SetScale(std::max(0.0f, m_fScale * (1.0f - iNumSteps / 100.0f)));
}

/*********************************************************************************************
*********************************************************************************************/
void	Mandelbrot::keyPressEvent(QKeyEvent * event)
{
	if (!m_bKeyPressed)
	{
		if (event->key() == Qt::Key_G)
		{
			m_bGaussianEnabled = !m_bGaussianEnabled;
			m_bUp2Date = false;
		}
	}

	if (event->key() == Qt::Key_1)
	{
		m_fTFPar1 += 100;
		m_bUp2Date = false;
		m_bRenderPreview = true;
	}
	else if (event->key() == Qt::Key_2)
	{
		m_fTFPar1 -= 100;
		m_bUp2Date = false;
		m_bRenderPreview = true;
	}
	else if (event->key() == Qt::Key_3)
	{
		m_fTFPar2 += 0.05f;
		m_bUp2Date = false;
		m_bRenderPreview = true;
	}
	else if (event->key() == Qt::Key_4)
	{
		m_fTFPar2 -= 0.05f;
		m_bUp2Date = false;
		m_bRenderPreview = true;
	}


	m_bKeyPressed = true;
}

/*********************************************************************************************
*********************************************************************************************/
void	Mandelbrot::keyReleaseEvent(QKeyEvent *event)
{
	m_bKeyPressed = false;
}

/*********************************************************************************************
*********************************************************************************************/
void	Mandelbrot::ItlInitializeShaderAndTextures()
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

	/// Textures
	glGenFramebuffers(2, m_glnFBOs);
	glGenTextures(2, m_glnRenderTextures);

	for (unsigned int n = 0; n < 2; ++n)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_glnFBOs[n]);
		glBindTexture(GL_TEXTURE_2D, m_glnRenderTextures[n]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_iResolutionX, m_iResolutionY, 0, GL_RGBA, GL_FLOAT, NULL);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_glnRenderTextures[n], 0);

		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	


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
	delete[] pTransferFunctionTexture;

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);


	m_glnMandelbrotShader = ItlCreateShader("shader/Mandelbrot.vert", "shader/Mandelbrot.frag");
	m_MandelbrotShaderLocs.gliMaxIterationsLoc = glGetUniformLocation(m_glnMandelbrotShader, "fMaxIterations");
	m_MandelbrotShaderLocs.gliLookupTableLoc = glGetUniformLocation(m_glnMandelbrotShader, "tLookupTable");
	m_MandelbrotShaderLocs.gliScaleLoc = glGetUniformLocation(m_glnMandelbrotShader, "fScale");
	m_MandelbrotShaderLocs.gliOffsetLoc = glGetUniformLocation(m_glnMandelbrotShader, "v2Offset");
	m_MandelbrotShaderLocs.gliTFPar1Loc = glGetUniformLocation(m_glnMandelbrotShader, "fTFPar1");
	m_MandelbrotShaderLocs.gliTFPar2Loc = glGetUniformLocation(m_glnMandelbrotShader, "fTFPar2");

	m_glnTextureToScreenShader = ItlCreateShader("shader/TextureToScreen.vert", "shader/TextureToScreen.frag");
	m_TextureToScreenShaderLocs.gliInputTextureLoc = glGetUniformLocation(m_glnTextureToScreenShader, "tInputTexture");

	m_glnGaussianShader = ItlCreateShader("shader/GaussianSmoothing.vert", "shader/GaussianSmoothing.frag");
	m_GaussianShaderLocs.gliInputTextureLoc = glGetUniformLocation(m_glnGaussianShader, "tInputTexture");
	m_GaussianShaderLocs.gliScaleLoc = glGetUniformLocation(m_glnGaussianShader, "v2Scale");
}

/*********************************************************************************************
*********************************************************************************************/
GLuint	Mandelbrot::ItlLoadShader(std::string sPath, int iShaderType)
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
GLuint	Mandelbrot::ItlCreateShader(std::string sVertexShaderPath, std::string sFragmentShaderPath)
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

/*********************************************************************************************
*********************************************************************************************/
void	Mandelbrot::ItlRenderMandelbrotSet(int iResolutionX, int iResolutionY, unsigned int nIterations, int iPingPongTextureIndex)
{
	//Resize texture
	glBindTexture(GL_TEXTURE_2D, m_glnRenderTextures[iPingPongTextureIndex]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iResolutionX, iResolutionY, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glViewport(0, 0, iResolutionX, iResolutionY);
	glUseProgram(m_glnMandelbrotShader);

	glBindFramebuffer(GL_FRAMEBUFFER, m_glnFBOs[iPingPongTextureIndex]);

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
	glUniform1d(m_MandelbrotShaderLocs.gliLookupTableLoc, GL_TEXTURE0);
	glUniform1f(m_MandelbrotShaderLocs.gliMaxIterationsLoc, (float)nIterations);
	glUniform1f(m_MandelbrotShaderLocs.gliScaleLoc, m_fScale);
	glUniform2f(m_MandelbrotShaderLocs.gliOffsetLoc, m_fOffsetX, m_fOffsetY);
	glUniform1f(m_MandelbrotShaderLocs.gliTFPar1Loc, m_fTFPar1);
	glUniform1f(m_MandelbrotShaderLocs.gliTFPar2Loc, m_fTFPar2);

	// Draw the triangles !
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/*********************************************************************************************
*********************************************************************************************/
void	Mandelbrot::ItlRenderGaussian(int iResolutionX, int iResolutionY, int iPingPongTextureIndex)
{
	//Resize texture
	glBindTexture(GL_TEXTURE_2D, m_glnRenderTextures[iPingPongTextureIndex]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iResolutionX, iResolutionY, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glViewport(0, 0, iResolutionX, iResolutionY);
	glUseProgram(m_glnGaussianShader);

	glBindFramebuffer(GL_FRAMEBUFFER, m_glnFBOs[iPingPongTextureIndex]);

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
	glBindTexture(GL_TEXTURE_2D, m_glnRenderTextures[(iPingPongTextureIndex + 1) % 2]);
	glUniform1d(m_GaussianShaderLocs.gliInputTextureLoc, GL_TEXTURE0);
	glUniform2f(m_GaussianShaderLocs.gliScaleLoc, 1.0f / iResolutionX, 1.0f / iResolutionY);

	// Draw the triangles !
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/*********************************************************************************************
*********************************************************************************************/
void	Mandelbrot::ItlRenderTextureToScreen(int iPingPongTextureIndex)
{
	glViewport(0, 0, m_iResolutionX, m_iResolutionY);

	glUseProgram(m_glnTextureToScreenShader);
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
	glBindTexture(GL_TEXTURE_2D, m_glnRenderTextures[iPingPongTextureIndex]);
	glUniform1d(m_TextureToScreenShaderLocs.gliInputTextureLoc, GL_TEXTURE0);

	// Draw the triangles !
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(0);
}