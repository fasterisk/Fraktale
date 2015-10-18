#include "Julia.h"
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

	m_pTransferFunction = new TransferFunctionHSVToRGB();

	m_fComplexReal = 0.0f;
	m_fComplexImag = 0.0f;

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

		m_bUp2Date = false;
		m_bRenderPreview = true;
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

		m_bUp2Date = false;
		m_bRenderPreview = true;
	}
}

/*********************************************************************************************
*********************************************************************************************/
void	Julia::SetOffset(float fXOffset, float fYOffset)
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
void	Julia::SetScale(float fScale)
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
void Julia::Render()
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
		//Render julia set
		///////////////////////////////////////////////////////////////////////////////////
		ItlRenderJuliaSet(m_iResolutionX * iSuperSampling, m_iResolutionY * iSuperSampling, nIterations, 0);

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
void	Julia::Reset()
{
}

/*********************************************************************************************
*********************************************************************************************/
void	Julia::SetComplex(float fReal, float fImag)
{
	m_fComplexReal = fReal;
	m_fComplexImag = fImag;

	m_bUp2Date = false;
	m_bRenderPreview = true;
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


	SetOffset(m_fOffsetX - (iDiffX / (float)m_iResolutionX) * m_fScale,
			  m_fOffsetY + (iDiffY / (float)m_iResolutionY) * m_fScale);


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
	iNumSteps *= 10.0f;

	SetScale(std::max(0.0f, m_fScale * (1.0f - iNumSteps / 100.0f)));
}

/*********************************************************************************************
*********************************************************************************************/
void	Julia::keyPressEvent(QKeyEvent * event)
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
void	Julia::keyReleaseEvent(QKeyEvent *event)
{
	m_bKeyPressed = false;
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
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	m_glnJuliaShader = ItlCreateShader("shader/Julia.vert", "shader/Julia.frag");
	m_JuliaShaderLocs.gliMaxIterationsLoc = glGetUniformLocation(m_glnJuliaShader, "fMaxIterations");
	m_JuliaShaderLocs.gliLookupTableLoc = glGetUniformLocation(m_glnJuliaShader, "tLookupTable");
	m_JuliaShaderLocs.gliScaleLoc = glGetUniformLocation(m_glnJuliaShader, "fScale");
	m_JuliaShaderLocs.gliOffsetLoc = glGetUniformLocation(m_glnJuliaShader, "v2Offset");
	m_JuliaShaderLocs.gliComplexLoc = glGetUniformLocation(m_glnJuliaShader, "v2Complex");
	m_JuliaShaderLocs.gliTFPar1Loc = glGetUniformLocation(m_glnJuliaShader, "fTFPar1");
	m_JuliaShaderLocs.gliTFPar2Loc = glGetUniformLocation(m_glnJuliaShader, "fTFPar2");

	m_glnTextureToScreenShader = ItlCreateShader("shader/TextureToScreen.vert", "shader/TextureToScreen.frag");
	m_TextureToScreenShaderLocs.gliInputTextureLoc = glGetUniformLocation(m_glnTextureToScreenShader, "tInputTexture");

	m_glnGaussianShader = ItlCreateShader("shader/GaussianSmoothing.vert", "shader/GaussianSmoothing.frag");
	m_GaussianShaderLocs.gliInputTextureLoc = glGetUniformLocation(m_glnGaussianShader, "tInputTexture");
	m_GaussianShaderLocs.gliScaleLoc = glGetUniformLocation(m_glnGaussianShader, "v2Scale");
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

/*********************************************************************************************
*********************************************************************************************/
void	Julia::ItlRenderJuliaSet(int iResolutionX, int iResolutionY, unsigned int nIterations, int iPingPongTextureIndex)
{
	//Resize texture
	glBindTexture(GL_TEXTURE_2D, m_glnRenderTextures[iPingPongTextureIndex]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iResolutionX, iResolutionY, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glViewport(0, 0, iResolutionX, iResolutionY);
	glUseProgram(m_glnJuliaShader);

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
	glUniform1d(m_JuliaShaderLocs.gliLookupTableLoc, GL_TEXTURE0);
	glUniform1f(m_JuliaShaderLocs.gliMaxIterationsLoc, (float)nIterations);
	glUniform1f(m_JuliaShaderLocs.gliScaleLoc, m_fScale);
	glUniform2f(m_JuliaShaderLocs.gliOffsetLoc, m_fOffsetX, m_fOffsetY);
	glUniform2f(m_JuliaShaderLocs.gliComplexLoc, m_fComplexReal, m_fComplexImag);
	glUniform1f(m_JuliaShaderLocs.gliTFPar1Loc, m_fTFPar1);
	glUniform1f(m_JuliaShaderLocs.gliTFPar2Loc, m_fTFPar2);

	// Draw the triangles !
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/*********************************************************************************************
*********************************************************************************************/
void	Julia::ItlRenderGaussian(int iResolutionX, int iResolutionY, int iPingPongTextureIndex)
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
void	Julia::ItlRenderTextureToScreen(int iPingPongTextureIndex)
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