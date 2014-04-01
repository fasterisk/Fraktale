#include "DLA.h"

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

/*********************************************************************************************
*********************************************************************************************/
DLA::DLA(int iWindowWidth, int iWindowHeight)
{
	m_iWindowWidth = iWindowHeight;
	m_iWindowHeight = iWindowHeight;

	ItlInitialize();
}

/*********************************************************************************************
*********************************************************************************************/
DLA::~DLA()
{
}

/*********************************************************************************************
*********************************************************************************************/
void DLA::RenderNextElement()
{
	//Create a point on the border of the region randomly
	glm::ivec2 v2Random;

	v2Random.x = rand() % m_iWindowWidth;
	v2Random.y = rand() % m_iWindowHeight;

	bool bDraw = false;

	while (!bDraw)
	{
		bDraw = ItlShouldBeDrawn(v2Random.x, v2Random.y);

		if (!bDraw)
		{
			int iRandom = rand() % 4;

			switch (iRandom)
			{
			case UP:
				if (v2Random.y < m_iWindowHeight - 1)
					v2Random.y++;
				break;

			case RIGHT:
				if (v2Random.x < m_iWindowWidth - 1)
					v2Random.x++;
				break;

			case DOWN:
				if (v2Random.y > 0)
					v2Random.y--;
				break;

			case LEFT:
				if (v2Random.x > 0)
					v2Random.x--;
				break;

			default:
				return;
			}
		}
	}

	//DRAW
	float fValue = 1.0f;
	ItlSetRasterValue(v2Random.x, v2Random.y, fValue);

	glUseProgram(m_glnShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_glnTextureID);
	
	glTexSubImage2D(GL_TEXTURE_2D, 0, v2Random.x, v2Random.y, 1, 1, GL_RED, GL_FLOAT, &fValue);

	glUniform1i(m_glnTextureID, 0);

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
}

/*********************************************************************************************
*********************************************************************************************/
void	DLA::ItlInitialize()
{
	m_pfRaster = new float[m_iWindowWidth * m_iWindowHeight];
	for (int i = 0; i < m_iWindowWidth * m_iWindowHeight; ++i)
		m_pfRaster[i] = 0.0f;

	int iX = m_iWindowWidth / 2;
	int iY = m_iWindowHeight / 2;


	for (int i = 0; i < m_iWindowWidth; ++i)
		ItlSetRasterValue(i, iY, 1.0f);

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

	glGenTextures(1, &m_glnTextureID);
	glBindTexture(GL_TEXTURE_2D, m_glnTextureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_iWindowWidth, m_iWindowHeight, 0, GL_RED, GL_FLOAT, m_pfRaster);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	ItlLoadShaders();
}

/*********************************************************************************************
*********************************************************************************************/
bool	DLA::ItlShouldBeDrawn(int iX, int iY)
{
	bool bShouldBeDrawn = m_pfRaster[iY * m_iWindowWidth + iX] > 0;

	if (!bShouldBeDrawn && iX > 0)
		bShouldBeDrawn = m_pfRaster[iY * m_iWindowWidth + (iX - 1)] > 0;

	if (!bShouldBeDrawn && iX < m_iWindowWidth - 1)
		bShouldBeDrawn = m_pfRaster[iY * m_iWindowWidth + (iX + 1)] > 0;

	if (!bShouldBeDrawn && iY > 0)
		bShouldBeDrawn = m_pfRaster[(iY - 1) * m_iWindowWidth + iX] > 0;

	if (!bShouldBeDrawn && iY < m_iWindowHeight - 1)
		bShouldBeDrawn = m_pfRaster[(iY + 1) * m_iWindowWidth + iX] > 0;

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

	m_glnTextureLocation = glGetUniformLocation(m_glnShader, "tTexture");
}

/*********************************************************************************************
*********************************************************************************************/
void	DLA::ItlSetRasterValue(int iX, int iY, float fValue)
{
	assert(m_pfRaster != NULL);
	assert(iX >= 0);
	assert(iX < m_iWindowWidth);
	assert(iY >= 0);
	assert(iY < m_iWindowHeight);

	m_pfRaster[iY * m_iWindowWidth + iX] = fValue;
}