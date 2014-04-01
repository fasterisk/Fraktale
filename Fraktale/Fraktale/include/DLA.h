#include "CommonIncludes.h"

class DLA
{
public:
	/// Constructor
	DLA(int iWindowWidth, int iWindowHeight);

	/// Destructor
	~DLA();

	/// Renders next element
	void	RenderNextElement();

private:
	/// Initializes the DLA
	void	ItlInitialize();

	/// Checks if the given position is in the raster, or a neighboring point is in the raster
	bool	ItlShouldBeDrawn(int iX, int iY);

	/// Loads the needed shaders
	void	ItlLoadShaders();

	/// Sets a value in the raster
	void	ItlSetRasterValue(int iX, int iY, float fValue);

	/// Member variables
	int m_iWindowWidth;
	int m_iWindowHeight;

	float		* m_pfRaster;

	GLuint	m_glnVertexArray;
	GLuint	m_glnVertexBuffer;

	GLuint	m_glnTextureID;

	GLuint	m_glnShader;

	GLuint	m_glnTextureLocation;
};