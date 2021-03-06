#include "DLA.h"
#include <qevent.h>
#include <time.h>
#include <algorithm>

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

/*********************************************************************************************
*********************************************************************************************/
DLA::DLA(int iResolutionX, int iResolutionY, unsigned int nMaxSteps, QWidget * pParent)
	: QGLWidget(pParent)
{
	assert(iResolutionX > 0);
	assert(iResolutionY > 0);

	m_iResolutionX = iResolutionX;
	m_iResolutionY = iResolutionY;
	m_eStartRegion = START_REGION_SQUARE;
	m_nMaxSteps = nMaxSteps;
	m_nNumDrawnPixels = 0;
	m_nNumPixels = iResolutionX * iResolutionY;
	m_nCurrentPathLength = 0;

	m_eGoalRegion = GOAL_REGION_POINT;

	m_iv2StartingPoint = glm::ivec2(m_iResolutionX / 2, m_iResolutionY / 2);

	m_iv2LinePoint1 = glm::ivec2(m_iResolutionX * 1 / 3, m_iResolutionY / 2);
	m_iv2LinePoint2 = glm::ivec2(m_iResolutionX * 2 / 3, m_iResolutionY / 2);

	m_iv2CurDrawnMin = m_iv2StartingPoint;
	m_iv2CurDrawnMax = m_iv2StartingPoint;

	m_pfRaster = NULL;
	m_pfStartRegion = NULL;

	m_pfCurrentPathRaster = NULL;

	m_iStartingRegionRadius = 10;

	m_bPaused = false;

	m_bShowPath = false;

	m_fLikelihoodLeft = 0.25f;
	m_fLikelihoodRight = 0.25f;
	m_fLikelihoodUp = 0.25f;
	m_fLikelihoodDown = 0.25f;

	m_pTransferFunction = new TransferFunctionCustom2();

	srand(time(NULL));

	ItlUpdateStartRegion(m_iv2StartingPoint);

	ItlInitializeRaster();
}

/*********************************************************************************************
*********************************************************************************************/
DLA::~DLA()
{
	if (m_pfRaster != NULL)
	{
		delete[] m_pfRaster;
		m_pfRaster = NULL;
	}

	if (m_pfStartRegion != NULL)
	{
		delete[] m_pfStartRegion;
		m_pfStartRegion = NULL;
	}

	if (m_pfCurrentPathRaster != NULL)
	{
		delete[] m_pfCurrentPathRaster;
		m_pfCurrentPathRaster = NULL;
	}

	delete m_pTransferFunction;
}

/*********************************************************************************************
*********************************************************************************************/
void	DLA::GetResolution(int & riResolutionX, int & riResolutionY) const
{
	riResolutionX = m_iResolutionX;
	riResolutionY = m_iResolutionY;
}

/*********************************************************************************************
*********************************************************************************************/
void	DLA::SetStartRegionType(TStartRegion eStartRegion)
{
	if (eStartRegion != m_eStartRegion)
	{
		m_eStartRegion = eStartRegion;
		Reset();
	}
}

/*********************************************************************************************
*********************************************************************************************/
void	DLA::SetGoalRegionType(TGoalRegion eGoalRegion)
{
	if (eGoalRegion != m_eGoalRegion)
	{
		m_eGoalRegion = eGoalRegion;
		Reset();
	}
}

/*********************************************************************************************
*********************************************************************************************/
void	DLA::SetResolution(int iResolutionX, int iResolutionY)
{
	if (m_iResolutionX != iResolutionX || m_iResolutionY != iResolutionY)
	{
		m_iResolutionX = iResolutionX;
		m_iResolutionY = iResolutionY;
		//setFixedSize(m_iResolutionX, m_iResolutionY);
		Reset();
	}
}

/*********************************************************************************************
*********************************************************************************************/
void	DLA::SetShowPath(bool bShow)
{
	m_bShowPath = bShow;
}

/*********************************************************************************************
*********************************************************************************************/
void	DLA::SetMaxNumSteps(int iNumSteps)
{
	assert(iNumSteps > 0);
	if (m_nMaxSteps != iNumSteps)
	{
		m_nMaxSteps = iNumSteps;
	}
}

/*********************************************************************************************
*********************************************************************************************/
void	DLA::CalculateNextElement()
{
	//Reset the path
	ItlResetPath();

	//Check if we need to stop
	if ((m_iv2StartRegionSize.x == m_iResolutionX - 1) &&
		(m_iv2StartRegionSize.y == m_iResolutionY - 1) &&
		((m_iv2CurDrawnMin.x == 0) || (m_iv2CurDrawnMin.y == 0) || (m_iv2CurDrawnMax.x == m_iResolutionX - 1) || m_iv2CurDrawnMax.y == m_iResolutionY - 1))
		return;

	//Create a point on the border of the region randomly
	glm::ivec2 iv2Random;
	bool bOk = ItlGetRandomPoint(iv2Random);

	if (bOk)
	{
		bool bDraw = false;
		unsigned int nSteps = 0;

		while (!bDraw && nSteps < m_nMaxSteps)
		{
			bDraw = ItlShouldBeDrawn(iv2Random.x, iv2Random.y);

			if (!bDraw)
				ItlMovePoint(iv2Random);

			if (m_bShowPath)
				ItlSetValueInPathRaster(iv2Random.x, iv2Random.y);

			nSteps++;
		}

		//DRAW
		if (bDraw)
			ItlSetRasterValue(iv2Random.x, iv2Random.y, (float)m_nNumDrawnPixels);
	}
}

/*********************************************************************************************
*********************************************************************************************/
void DLA::Render()
{
	float * pTexture = new float[m_iResolutionX * m_iResolutionY * 3];

	float * pCurrentTexture = pTexture;
	float * pCurrentRasterPointer = m_pfRaster;
	float * pCurrentStartRegionPointer = m_pfStartRegion;
	float * pCurrentPathPointer = m_pfCurrentPathRaster;


	for (int i = 0; i < m_iResolutionX * m_iResolutionY; ++i)
	{

		if ((*pCurrentRasterPointer) > 0.0f)
		{
			float fRed, fGreen, fBlue;
			m_pTransferFunction->GetColor((*pCurrentRasterPointer) / (float)m_nNumDrawnPixels, fRed, fGreen, fBlue);
			(*pCurrentTexture) = fRed;
			++pCurrentTexture;
			(*pCurrentTexture) = fGreen;
			++pCurrentTexture;
			(*pCurrentTexture) = fBlue;
			++pCurrentTexture;
		}
		else if ((*pCurrentPathPointer) > 0.0f)
		{
			(*pCurrentTexture) = (*pCurrentPathPointer) / (float)m_nCurrentPathLength;
			++pCurrentTexture;
			(*pCurrentTexture) = (*pCurrentPathPointer) / (float)m_nCurrentPathLength;
			++pCurrentTexture;
			(*pCurrentTexture) = 1.0f;
			++pCurrentTexture;
		}
		else if ((*pCurrentStartRegionPointer) > 0.0f)
		{
			(*pCurrentTexture) = (*pCurrentStartRegionPointer);
			++pCurrentTexture;
			(*pCurrentTexture) = (*pCurrentStartRegionPointer);
			++pCurrentTexture;
			(*pCurrentTexture) = (*pCurrentStartRegionPointer);
			++pCurrentTexture;
		}
		else
		{
			(*pCurrentTexture) = 0.0f;
			++pCurrentTexture;
			(*pCurrentTexture) = 0.0f;
			++pCurrentTexture;
			(*pCurrentTexture) = 0.0f;
			++pCurrentTexture;
		}


		++pCurrentRasterPointer;
		++pCurrentStartRegionPointer;
		++pCurrentPathPointer;

	}

	glUseProgram(m_glnShader);

	//Raster texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_glnTexID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_iResolutionX, m_iResolutionY, 0, GL_RGB, GL_FLOAT, pTexture);

	glUniform1i(m_glnTexLoc, 0);

	//Starting region texture
	/*glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_glnStartingRegionTexID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_iResolutionX, m_iResolutionY, 0, GL_RED, GL_FLOAT, m_pfStartRegion);

	glUniform1i(m_glnStartingRegionTexID, 1);

	//Path texture
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_glnPathTexID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_iResolutionX, m_iResolutionY, 0, GL_RED, GL_FLOAT, m_pfCurrentPathRaster);

	glUniform1i(m_glnPathTexID, 2);*/


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

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);

	delete[] pTexture;
}

/*********************************************************************************************
*********************************************************************************************/
void	DLA::Reset()
{
	if (m_eStartRegion == START_REGION_LINE)
	{
		m_iv2StartingPoint = glm::vec2(m_iResolutionX / 2, 0);
		m_iv2LinePoint1 = glm::vec2(0, 0);
		m_iv2LinePoint2 = glm::vec2(m_iResolutionX, 0);
	}
	else
	{
		m_iv2StartingPoint = glm::vec2(m_iResolutionX / 2, m_iResolutionY / 2);
		m_iv2LinePoint1 = glm::vec2(m_iResolutionX * 1/3,  m_iResolutionY / 2);
		m_iv2LinePoint2 = glm::vec2(m_iResolutionX * 2/3, m_iResolutionY / 2);
	}

	ItlClampPointInsideArea(m_iv2StartingPoint);
	ItlClampPointInsideArea(m_iv2LinePoint1);
	ItlClampPointInsideArea(m_iv2LinePoint2);

	m_nNumDrawnPixels = 0;
	m_iv2CurDrawnMin = m_iv2StartingPoint;
	m_iv2CurDrawnMax = m_iv2StartingPoint;

	if (m_pfRaster != NULL)
	{
		delete[] m_pfRaster;
		m_pfRaster = NULL;
	}

	if (m_pfStartRegion != NULL)
	{
		delete[] m_pfStartRegion;
		m_pfStartRegion = NULL;
	}

	if (m_pfCurrentPathRaster != NULL)
	{
		delete[] m_pfCurrentPathRaster;
		m_pfCurrentPathRaster = NULL;
	}

	m_iStartingRegionRadius = 10;

	if (m_eGoalRegion == GOAL_REGION_POINT)
		ItlUpdateStartRegion(m_iv2StartingPoint);
	else
	{
		ItlUpdateStartRegion(m_iv2LinePoint1);
		ItlUpdateStartRegion(m_iv2LinePoint2);
	}

	ItlInitializeRaster();
}


/*********************************************************************************************
*********************************************************************************************/
void	DLA::SetLikelihoodLeft(float fLikelihood)
{
	float fRestBefore = 1.0f - m_fLikelihoodLeft;
	float fRestNow = 1.0f - fLikelihood;

	m_fLikelihoodLeft = fLikelihood;

	if (fRestBefore > 0.0f && fRestNow > 0.0f)
	{
		m_fLikelihoodDown = (m_fLikelihoodDown / fRestBefore) * fRestNow;
		m_fLikelihoodUp = (m_fLikelihoodUp / fRestBefore) * fRestNow;
		m_fLikelihoodRight = fRestNow - m_fLikelihoodDown - m_fLikelihoodUp;
	}
	else if (fRestBefore == 0.0f)
	{
		m_fLikelihoodDown = fRestNow / 3.0f;
		m_fLikelihoodUp = fRestNow / 3.0f;
		m_fLikelihoodRight = fRestNow - m_fLikelihoodDown - m_fLikelihoodUp;
	}
	else // fRestNow == 0.0f
	{
		m_fLikelihoodDown = 0.0f;
		m_fLikelihoodUp = 0.0f;
		m_fLikelihoodRight = 0.0f;
	}

}

/*********************************************************************************************
*********************************************************************************************/
void	DLA::SetLikelihoodRight(float fLikelihood)
{
	float fRestBefore = 1.0f - m_fLikelihoodRight;
	float fRestNow = 1.0f - fLikelihood;

	m_fLikelihoodRight = fLikelihood;

	if (fRestBefore > 0.0f && fRestNow > 0.0f)
	{
		m_fLikelihoodDown = (m_fLikelihoodDown / fRestBefore) * fRestNow;
		m_fLikelihoodUp = (m_fLikelihoodUp / fRestBefore) * fRestNow;
		m_fLikelihoodLeft = fRestNow - m_fLikelihoodDown - m_fLikelihoodUp;
	}
	else if (fRestBefore == 0.0f)
	{
		m_fLikelihoodDown = fRestNow / 3.0f;
		m_fLikelihoodUp = fRestNow / 3.0f;
		m_fLikelihoodLeft = fRestNow - m_fLikelihoodDown - m_fLikelihoodUp;
	}
	else // fRestNow == 0.0f
	{
		m_fLikelihoodDown = 0.0f;
		m_fLikelihoodUp = 0.0f;
		m_fLikelihoodLeft = 0.0f;
	}
}

/*********************************************************************************************
*********************************************************************************************/
void	DLA::SetLikelihoodUp(float fLikelihood)
{
	float fRestBefore = 1.0f - m_fLikelihoodUp;
	float fRestNow = 1.0f - fLikelihood;

	m_fLikelihoodUp = fLikelihood;

	if (fRestBefore > 0.0f && fRestNow > 0.0f)
	{
		m_fLikelihoodDown = (m_fLikelihoodDown / fRestBefore) * fRestNow;
		m_fLikelihoodLeft = (m_fLikelihoodLeft / fRestBefore) * fRestNow;
		m_fLikelihoodRight = fRestNow - m_fLikelihoodDown - m_fLikelihoodLeft;
	}
	else if (fRestBefore == 0.0f)
	{
		m_fLikelihoodDown = fRestNow / 3.0f;
		m_fLikelihoodLeft = fRestNow / 3.0f;
		m_fLikelihoodRight = fRestNow - m_fLikelihoodDown - m_fLikelihoodLeft;
	}
	else // fRestNow == 0.0f
	{
		m_fLikelihoodDown = 0.0f;
		m_fLikelihoodLeft = 0.0f;
		m_fLikelihoodRight = 0.0f;
	}
}

/*********************************************************************************************
*********************************************************************************************/
void	DLA::SetLikelihoodDown(float fLikelihood)
{
	float fRestBefore = 1.0f - m_fLikelihoodDown;
	float fRestNow = 1.0f - fLikelihood;

	m_fLikelihoodDown = fLikelihood;

	if (fRestBefore > 0.0f && fRestNow > 0.0f)
	{
		m_fLikelihoodLeft = (m_fLikelihoodLeft / fRestBefore) * fRestNow;
		m_fLikelihoodUp = (m_fLikelihoodUp / fRestBefore) * fRestNow;
		m_fLikelihoodRight = fRestNow - m_fLikelihoodLeft - m_fLikelihoodUp;
	}
	else if (fRestBefore == 0.0f)
	{
		m_fLikelihoodLeft = fRestNow / 3.0f;
		m_fLikelihoodUp = fRestNow / 3.0f;
		m_fLikelihoodRight = fRestNow - m_fLikelihoodLeft - m_fLikelihoodUp;
	}
	else // fRestNow == 0.0f
	{
		m_fLikelihoodLeft = 0.0f;
		m_fLikelihoodUp = 0.0f;
		m_fLikelihoodRight = 0.0f;
	}
}

/*********************************************************************************************
*********************************************************************************************/
QSize	DLA::minimumSizeHint() const
{
	return QSize(m_iResolutionX, m_iResolutionY);
}

/*********************************************************************************************
*********************************************************************************************/
QSize	DLA::sizeHint() const
{
	return QSize(m_iResolutionX, m_iResolutionY);
}

/*********************************************************************************************
*********************************************************************************************/
void	DLA::initializeGL()
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
void	DLA::paintGL()
{
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!m_bPaused)
		CalculateNextElement();

	Render();

	update();
}

/*********************************************************************************************
*********************************************************************************************/
void	DLA::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
}

/*********************************************************************************************
*********************************************************************************************/
void	DLA::mousePressEvent(QMouseEvent * event)
{

}

/*********************************************************************************************
*********************************************************************************************/
void	DLA::mouseMoveEvent(QMouseEvent * event)
{

}

/*********************************************************************************************
*********************************************************************************************/
void	DLA::ItlInitializeRaster()
{
	if (m_pfRaster != NULL)
	{
		delete[] m_pfRaster;
		m_pfRaster = NULL;
	}

	m_pfRaster = new float[m_iResolutionX * m_iResolutionY];
	float * pCurrentRasterPointer = m_pfRaster;
	for (int i = 0; i < m_iResolutionX * m_iResolutionY; ++i)
	{
		(*pCurrentRasterPointer) = 0.0f;
		++pCurrentRasterPointer;
	}

	if (m_pfCurrentPathRaster != NULL)
	{
		delete[] m_pfCurrentPathRaster;
		m_pfCurrentPathRaster = NULL;
	}

	m_pfCurrentPathRaster = new float[m_iResolutionX * m_iResolutionY];
	pCurrentRasterPointer = m_pfCurrentPathRaster;
	for (int i = 0; i < m_iResolutionX * m_iResolutionY; ++i)
	{
		(*pCurrentRasterPointer) = 0.0f;
		++pCurrentRasterPointer;
	}

	if (m_eGoalRegion == GOAL_REGION_POINT)
	{
		m_nNumDrawnPixels = 1;
		ItlSetRasterValue(m_iv2StartingPoint.x, m_iv2StartingPoint.y, (float)m_nNumDrawnPixels);
	}
	else
	{
		m_nNumDrawnPixels = 1;

		if (m_iv2LinePoint1.x > m_iv2LinePoint2.x)
		{
			glm::ivec2 iv2Temp = m_iv2LinePoint2;
			m_iv2LinePoint2 = m_iv2LinePoint1;
			m_iv2LinePoint1 = iv2Temp;
		}

		glm::ivec2 iv2Delta = m_iv2LinePoint2 - m_iv2LinePoint1;

		if (m_iv2LinePoint1.x == m_iv2LinePoint2.x)
		{
			int iStart = std::min(m_iv2LinePoint1.y, m_iv2LinePoint2.y);
			int iEnd = std::max(m_iv2LinePoint1.y, m_iv2LinePoint2.y);


			for (int i = iStart; i <= iEnd; ++i)
			{
				ItlSetRasterValue(m_iv2LinePoint1.x, i, 1.0f);
			}
		}
		else
		{
			//LINE ALGORITHMUS aus Wikipedia / Bresenham Algorithmus
			int x0 = m_iv2LinePoint1.x;
			int y0 = m_iv2LinePoint1.y;
			int x1 = m_iv2LinePoint2.x;
			int y1 = m_iv2LinePoint2.y;

			int dx = std::abs(x1 - x0);
			int sx = x0 < x1 ? 1 : -1;
			int dy = -std::abs(y1 - y0);
			int sy = y0 < y1 ? 1 : -1;
			int err = dx + dy;
			int e2;

			for (;;)
			{
				ItlSetRasterValue(x0, y0, 1.0f);
				if (x0 == x1 && y0 == y1)
					break;

				e2 = 2 * err;
				if (e2 > dy)
				{
					err += dy;
					x0 += sx;
				}

				if (e2 < dx)
				{
					err += dx;
					y0 += sy;
				}
			}
		}


	}

}

/*********************************************************************************************
*********************************************************************************************/
void	DLA::ItlInitializeShaderAndTextures()
{
	static const GLfloat glfVertexBufferData[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f
	};

	//VAO / VBO
	glGenVertexArrays(1, &m_glnVertexArray);
	glBindVertexArray(m_glnVertexArray);

	glGenBuffers(1, &m_glnVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_glnVertexBuffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(glfVertexBufferData), glfVertexBufferData, GL_STATIC_DRAW);

	// Raster texture
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &m_glnTexID);
	glBindTexture(GL_TEXTURE_2D, m_glnTexID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_iResolutionX, m_iResolutionY, 0, GL_RED, GL_FLOAT, m_pfRaster);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Starting region texture
	/*glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &m_glnStartingRegionTexID);
	glBindTexture(GL_TEXTURE_2D, m_glnStartingRegionTexID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_iResolutionX, m_iResolutionY, 0, GL_RED, GL_FLOAT, m_pfStartRegion);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Path texture
	glActiveTexture(GL_TEXTURE2);
	glGenTextures(1, &m_glnPathTexID);
	glBindTexture(GL_TEXTURE_2D, m_glnPathTexID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_iResolutionX, m_iResolutionY, 0, GL_RED, GL_FLOAT, m_pfCurrentPathRaster);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);*/

	ItlLoadShaders();
}

/*********************************************************************************************
*********************************************************************************************/
bool	DLA::ItlShouldBeDrawn(int iX, int iY)
{
	assert(m_pfRaster != NULL);
	assert(iX >= 0);
	assert(iX < m_iResolutionX);
	assert(iY >= 0);
	assert(iY < m_iResolutionY);

	bool bShouldBeDrawn = m_pfRaster[iY * m_iResolutionX + iX] > 0;

	if (!bShouldBeDrawn && iX > 0)
	{
		bShouldBeDrawn = m_pfRaster[iY * m_iResolutionX + iX - 1] > 0;
	}

	if (!bShouldBeDrawn && iX < m_iResolutionX - 1)
	{
		bShouldBeDrawn = m_pfRaster[iY * m_iResolutionX + iX + 1] > 0;
	}

	if (!bShouldBeDrawn && iY > 0)
	{
		bShouldBeDrawn = m_pfRaster[(iY - 1) * m_iResolutionX + iX] > 0;
	}

	if (!bShouldBeDrawn && iY < m_iResolutionY - 1)
	{
		bShouldBeDrawn = m_pfRaster[(iY + 1) * m_iResolutionX + iX] > 0;
	}

	return bShouldBeDrawn;
}

/*********************************************************************************************
*********************************************************************************************/
void	DLA::ItlLoadShaders()
{
	GLuint glnVertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint glnFragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	std::string sVertexShaderCode;
	std::ifstream sVertexShaderStream("shader/SimpleTexturing.vert", std::ios::in);
	if (sVertexShaderStream.is_open())
	{
		std::string sLine = "";
		while (getline(sVertexShaderStream, sLine))
			sVertexShaderCode += "\n" + sLine;
		sVertexShaderStream.close();
	}

	// Read the Fragment Shader code from the file
	std::string sFragmentShaderCode;
	std::ifstream sFragmentShaderStream("shader/SimpleTexturing.frag", std::ios::in);
	if (sFragmentShaderStream.is_open()){
		std::string sLine = "";
		while (getline(sFragmentShaderStream, sLine))
			sFragmentShaderCode += "\n" + sLine;
		sFragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	std::cout << "Compiling shader: " << "shader/SimpleTexturing.vert" << std::endl;
	char const * pVertexSourcePointer = sVertexShaderCode.c_str();
	glShaderSource(glnVertexShaderID, 1, &pVertexSourcePointer, NULL);
	glCompileShader(glnVertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(glnVertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(glnVertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> vVertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(glnVertexShaderID, InfoLogLength, NULL, &vVertexShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &vVertexShaderErrorMessage[0]);

	// Compile Fragment Shader
	std::cout << "Compiling shader: " << "shader/SimpleTexturing.frag" << std::endl;
	char const * pFragmentSourcePointer = sFragmentShaderCode.c_str();
	glShaderSource(glnFragmentShaderID, 1, &pFragmentSourcePointer, NULL);
	glCompileShader(glnFragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(glnFragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(glnFragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> vFragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(glnFragmentShaderID, InfoLogLength, NULL, &vFragmentShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &vFragmentShaderErrorMessage[0]);

	// Link the program
	fprintf(stdout, "Linking program\n");
	m_glnShader = glCreateProgram();
	glAttachShader(m_glnShader, glnVertexShaderID);
	glAttachShader(m_glnShader, glnFragmentShaderID);
	glLinkProgram(m_glnShader);

	// Check the program
	glGetProgramiv(m_glnShader, GL_LINK_STATUS, &Result);
	glGetProgramiv(m_glnShader, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> vProgramErrorMessage(std::max(InfoLogLength, int(1)));
	glGetProgramInfoLog(m_glnShader, InfoLogLength, NULL, &vProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &vProgramErrorMessage[0]);

	glDeleteShader(glnVertexShaderID);
	glDeleteShader(glnFragmentShaderID);

	m_glnTexLoc = glGetUniformLocation(m_glnShader, "tTexture");
	//m_glnStartingRegionTexLoc = glGetUniformLocation(m_glnShader, "tStartingRegionTexture");
	//m_glnPathTexLoc = glGetUniformLocation(m_glnShader, "tPathTexture");
}

/*********************************************************************************************
*********************************************************************************************/
void	DLA::ItlSetRasterValue(int iX, int iY, float fNewValue)
{
	assert(m_pfRaster != NULL);
	assert(iX >= 0);
	assert(iX < m_iResolutionX);
	assert(iY >= 0);
	assert(iY < m_iResolutionY);

	m_pfRaster[iY * m_iResolutionX + iX] = fNewValue;

	if (iX > m_iv2CurDrawnMax.x)
		m_iv2CurDrawnMax.x = iX;
	if (iX < m_iv2CurDrawnMin.x)
		m_iv2CurDrawnMin.x = iX;
	if (iY > m_iv2CurDrawnMax.y)
		m_iv2CurDrawnMax.y = iY;
	if (iY < m_iv2CurDrawnMin.y)
		m_iv2CurDrawnMin.y = iY;

	ItlUpdateStartRegion(glm::ivec2(iX, iY));

	m_nNumDrawnPixels++;
}

/*********************************************************************************************
*********************************************************************************************/
void	DLA::ItlSetValueInPathRaster(int iX, int iY)
{
	assert(m_pfCurrentPathRaster != NULL);
	assert(iX >= 0);
	assert(iX < m_iResolutionX);
	assert(iY >= 0);
	assert(iY < m_iResolutionY);



	m_pfCurrentPathRaster[iY * m_iResolutionX + iX] = (float)++m_nCurrentPathLength;
}

/*********************************************************************************************
*********************************************************************************************/
bool	DLA::ItlGetRandomPoint(glm::ivec2 & riv2RandomPoint)
{
	bool bOk = true;
	if (m_eStartRegion == START_REGION_SQUARE)
	{
		int iSumStartingRegionSize = 2 * m_iv2StartRegionSize.x + 2 * m_iv2StartRegionSize.y;
		int iRandom = rand() % iSumStartingRegionSize;

		if (iRandom < m_iv2StartRegionSize.x)
		{
			riv2RandomPoint.x = (rand() % m_iv2StartRegionSize.x);
			riv2RandomPoint.y = m_iv2StartRegionSize.y;
		}
		else if (iRandom < m_iv2StartRegionSize.x + m_iv2StartRegionSize.y)
		{
			riv2RandomPoint.x = m_iv2StartRegionSize.x;
			riv2RandomPoint.y = (rand() % m_iv2StartRegionSize.y);
		}
		else if (iRandom < m_iv2StartRegionSize.x + m_iv2StartRegionSize.y + m_iv2StartRegionSize.x)
		{
			riv2RandomPoint.x = (rand() % m_iv2StartRegionSize.x);
			riv2RandomPoint.y = 0;
		}
		else if (iRandom < m_iv2StartRegionSize.x + m_iv2StartRegionSize.y + m_iv2StartRegionSize.x + m_iv2StartRegionSize.y)
		{
			riv2RandomPoint.x = 0;
			riv2RandomPoint.y = (rand() % m_iv2StartRegionSize.y);
		}
		else
		{
			assert(!"should not happen!");
		}

		riv2RandomPoint += m_iv2StartRegionMin;
	}
	else if(m_eStartRegion == START_REGION_CIRCLE)
	{
		bOk = false;
		glm::ivec2 iv2Point(0, 0);
		iv2Point -= m_iv2StartingPoint;
		double dDist = std::sqrt(std::pow(iv2Point.x, 2) + std::pow(iv2Point.y, 2));
		if (dDist >= m_iStartingRegionRadius)
			bOk = true;

		iv2Point.x = m_iResolutionX - 1;
		iv2Point.y = 0;
		iv2Point -= m_iv2StartingPoint;
		dDist = std::sqrt(std::pow(iv2Point.x, 2) + std::pow(iv2Point.y, 2));
		if (dDist >= m_iStartingRegionRadius)
			bOk = true;

		iv2Point.x = m_iResolutionX - 1;
		iv2Point.y = m_iResolutionY - 1;
		iv2Point -= m_iv2StartingPoint;
		dDist = std::sqrt(std::pow(iv2Point.x, 2) + std::pow(iv2Point.y, 2));
		if (dDist >= m_iStartingRegionRadius)
			bOk = true;

		iv2Point.x = 0;
		iv2Point.y = m_iResolutionY - 1;
		iv2Point -= m_iv2StartingPoint;
		dDist = std::sqrt(std::pow(iv2Point.x, 2) + std::pow(iv2Point.y, 2));
		if (dDist >= m_iStartingRegionRadius)
			bOk = true;

		if (bOk)
		{
			do
			{
				float fRandomWinkel = (rand() % 360000) / 1000.0f;
				riv2RandomPoint.x = (int)std::trunc(std::cos(fRandomWinkel) * m_iStartingRegionRadius);
				riv2RandomPoint.y = (int)std::trunc(std::sin(fRandomWinkel) * m_iStartingRegionRadius);

				riv2RandomPoint += m_iv2StartingPoint;
			} while (riv2RandomPoint.x < 0 || riv2RandomPoint.y < 0 || riv2RandomPoint.x >= m_iResolutionX || riv2RandomPoint.y >= m_iResolutionY);
		}
	}
	else if (m_eStartRegion == START_REGION_LINE)
	{
		riv2RandomPoint.x = rand() % m_iResolutionX;
		riv2RandomPoint.y = m_iv2StartRegionMax.y;
	}

	ItlClampPointInsideArea(riv2RandomPoint);

	return bOk;
}

/*********************************************************************************************
*********************************************************************************************/
void	DLA::ItlMovePoint(glm::ivec2 &rPoint)
{
	int iLeft = m_fLikelihoodLeft * 100;
	int iRight = iLeft + m_fLikelihoodRight * 100;
	int iUp = iRight + m_fLikelihoodUp * 100;
	int iDown = 99;

	int iRandom = rand() % 100;

	if (m_eStartRegion == START_REGION_CIRCLE)
	{
		glm::ivec2 v2Point(rPoint);
		v2Point -= m_iv2StartingPoint;

		double dDist = 0.0;

		if (iRandom < iLeft)
		{
			//LEFT
			v2Point.x--;
			dDist = std::sqrt(std::pow(v2Point.x, 2) + std::pow(v2Point.y, 2));

			if (dDist < m_iStartingRegionRadius)
				rPoint.x--;
			
		}
		else if (iRandom < iRight)
		{
			//RIGHT
			v2Point.x++;
			dDist = std::sqrt(std::pow(v2Point.x, 2) + std::pow(v2Point.y, 2));

			if (dDist < m_iStartingRegionRadius)
				rPoint.x++;
		}
		else if (iRandom < iUp)
		{
			//UP
			v2Point.y++;
			dDist = std::sqrt(std::pow(v2Point.x, 2) + std::pow(v2Point.y, 2));

			if (dDist < m_iStartingRegionRadius)
				rPoint.y++;
		}
		else 
		{
			//DOWN
			v2Point.y--;
			dDist = std::sqrt(std::pow(v2Point.x, 2) + std::pow(v2Point.y, 2));

			if (dDist < m_iStartingRegionRadius)
				rPoint.y--;
		}
	}
	else if (m_eStartRegion == START_REGION_SQUARE)
	{
		if (iRandom <= iLeft)
		{
			//LEFT
			if (rPoint.x > m_iv2StartRegionMin.x)
				rPoint.x--;

		}
		else if (iRandom <= iRight)
		{
			//RIGHT
			if (rPoint.x < m_iv2StartRegionMax.x)
				rPoint.x++;
		}
		else if (iRandom <= iUp)
		{
			//UP
			if (rPoint.y < m_iv2StartRegionMax.y)
				rPoint.y++;
		}
		else
		{
			//DOWN
			if (rPoint.y > m_iv2StartRegionMin.y)
				rPoint.y--;
		}
	}
	else if (m_eStartRegion == START_REGION_LINE)
	{
		if (iRandom <= iLeft)
		{
			//LEFT
			if (rPoint.x > 0)
				rPoint.x--;

		}
		else if (iRandom <= iRight)
		{
			//RIGHT
			if (rPoint.x < m_iResolutionX)
				rPoint.x++;
		}
		else if (iRandom <= iUp)
		{
			//UP
			if (rPoint.y < m_iv2StartRegionMax.y)
				rPoint.y++;
		}
		else
		{
			//DOWN
			if (rPoint.y > 0)
				rPoint.y--;
		}

	}

	ItlClampPointInsideArea(rPoint);
}

/*********************************************************************************************
*********************************************************************************************/
void	DLA::ItlUpdateStartRegion(glm::ivec2 iv2LastDrawnPoint)
{
	if (m_eStartRegion == START_REGION_SQUARE)
	{
		m_iv2StartRegionMin = m_iv2CurDrawnMin - 10;
		m_iv2StartRegionMax = m_iv2CurDrawnMax + 10;

		ItlClampPointInsideArea(m_iv2StartRegionMin);
		ItlClampPointInsideArea(m_iv2StartRegionMax);

		m_iv2StartRegionSize = m_iv2StartRegionMax - m_iv2StartRegionMin;

		if (m_pfStartRegion != NULL)
		{
			delete[] m_pfStartRegion;
			m_pfStartRegion = NULL;
		}

		m_pfStartRegion = new float[m_iResolutionX * m_iResolutionY];
		for (int x = 0; x < m_iResolutionX; ++x)
		{
			for (int y = 0; y < m_iResolutionY; ++y)
			{
				m_pfStartRegion[y * m_iResolutionX + x] = 0.0f;
				if (x == m_iv2StartRegionMin.x || x == m_iv2StartRegionMax.x)
				{
					if (y >= m_iv2StartRegionMin.y && y <= m_iv2StartRegionMax.y)
						m_pfStartRegion[y * m_iResolutionX + x] = 1.0f;
				}

				if (y == m_iv2StartRegionMin.y || y == m_iv2StartRegionMax.y)
				{
					if (x >= m_iv2StartRegionMin.x && x <= m_iv2StartRegionMax.x)
						m_pfStartRegion[y * m_iResolutionX + x] = 1.0f;
				}

			}
		}
	}
	else if (m_eStartRegion == START_REGION_CIRCLE)
	{
		glm::ivec2 iv2Point = iv2LastDrawnPoint - m_iv2StartingPoint;
		double dDist = std::sqrt(std::pow(iv2Point.x, 2) + std::pow(iv2Point.y, 2));

		while (dDist > (m_iStartingRegionRadius - 10))
			m_iStartingRegionRadius++;


		m_iv2StartRegionMin = m_iv2StartingPoint - m_iStartingRegionRadius;
		m_iv2StartRegionMax = m_iv2StartingPoint + m_iStartingRegionRadius;

		if (m_pfStartRegion != NULL)
		{
			delete[] m_pfStartRegion;
			m_pfStartRegion = NULL;
		}

		m_pfStartRegion = new float[m_iResolutionX * m_iResolutionY];
		for (int x = m_iv2StartRegionMin.x; x <= m_iv2StartRegionMax.x; ++x)
		{
			for (int y = m_iv2StartRegionMin.y; y <= m_iv2StartRegionMax.y; ++y)
			{
				if (x >= 0 && y >= 0 && x < m_iResolutionX && y < m_iResolutionY)
				{
					m_pfStartRegion[y * m_iResolutionX + x] = 0.0f;

					int iX = x - m_iv2StartingPoint.x;
					int iY = y - m_iv2StartingPoint.y;

					int iRadius = (int)std::trunc(std::sqrt(std::pow(iX, 2) + std::pow(iY, 2)));

					if (iRadius == m_iStartingRegionRadius)
						m_pfStartRegion[y * m_iResolutionX + x] = 1.0f;
				}
			}
		}
	}
	else if (m_eStartRegion == START_REGION_LINE)
	{
		int iMaxY = std::max(m_iv2CurDrawnMin.y, m_iv2CurDrawnMax.y);

		m_iv2StartRegionMin.x = 0;
		m_iv2StartRegionMin.y = iMaxY + 10;
		m_iv2StartRegionMax.x = m_iResolutionX;
		m_iv2StartRegionMax.y = iMaxY + 10;

		ItlClampPointInsideArea(m_iv2StartRegionMin);
		ItlClampPointInsideArea(m_iv2StartRegionMax);

		m_iv2StartRegionSize = m_iv2StartRegionMax - m_iv2StartRegionMin;

		if (m_pfStartRegion != NULL)
		{
			delete[] m_pfStartRegion;
			m_pfStartRegion = NULL;
		}

		m_pfStartRegion = new float[m_iResolutionX * m_iResolutionY];
		for (int x = 0; x < m_iResolutionX; ++x)
		{
			for (int y = 0; y < m_iResolutionY; ++y)
			{
				m_pfStartRegion[y * m_iResolutionX + x] = 0.0f;
				if (x == m_iv2StartRegionMin.x || x == m_iv2StartRegionMax.x)
				{
					if (y >= m_iv2StartRegionMin.y && y <= m_iv2StartRegionMax.y)
						m_pfStartRegion[y * m_iResolutionX + x] = 1.0f;
				}

				if (y == m_iv2StartRegionMin.y || y == m_iv2StartRegionMax.y)
				{
					if (x >= m_iv2StartRegionMin.x && x <= m_iv2StartRegionMax.x)
						m_pfStartRegion[y * m_iResolutionX + x] = 1.0f;
				}

			}
		}
	}
}

/*********************************************************************************************
*********************************************************************************************/
void	DLA::ItlClampPointInsideArea(glm::ivec2 & riv2Point)
{
	if (riv2Point.x < 0)
		riv2Point.x = 0;
	if (riv2Point.x >= m_iResolutionX)
		riv2Point.x = m_iResolutionX - 1;
	if (riv2Point.y < 0)
		riv2Point.y = 0;
	if (riv2Point.y >= m_iResolutionY)
		riv2Point.y = m_iResolutionY - 1;
}

/*********************************************************************************************
*********************************************************************************************/
void	DLA::ItlResetPath()
{
	assert(m_pfCurrentPathRaster != NULL);

	m_nCurrentPathLength = 0;

	float * pfCurrentPointer = m_pfCurrentPathRaster;

	for (int i = 0; i < m_iResolutionX * m_iResolutionY; ++i)
	{
		(*pfCurrentPointer) = 0.0f;
		++pfCurrentPointer;
	}
}