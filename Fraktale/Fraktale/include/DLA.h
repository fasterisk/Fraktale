#include "CommonIncludes.h"

class DLA
{
public:
	enum TStartRegion
	{
		START_REGION_SQUARE,
		START_REGION_CIRCLE
	};

	enum TGoalRegion
	{
		GOAL_REGION_POINT,
		GOAL_REGION_LINE
	};

	/// Constructor
	DLA(int iWindowWidth, int iWindowHeight, TStartRegion eStartRegion, unsigned int nMaxSteps, glm::ivec2 iv2StartingPoint);

	/// Constructor 2
	DLA(int iWindowWidth, int iWindowHeight, TStartRegion eStartRegion, unsigned int nMaxSteps, glm::ivec2 iv2LinePoint1, glm::ivec2 iv2LinePoint2);

	/// Destructor
	~DLA();

	/// Calculates next element
	void	CalculateNextElement();

	/// Renders the textures
	void	Render();

	/// Resets the algorithm
	void	Reset();

private:
	/// Initializes the raster
	void	ItlInitializeRaster();

	/// Initializes the shader and textures
	void	ItlInitializeShaderAndTextures();

	/// Checks if the given position is in the raster, or a neighboring point is in the raster
	bool	ItlShouldBeDrawn(int iX, int iY);

	/// Loads the needed shaders
	void	ItlLoadShaders();

	/// Sets a value in the raster
	void	ItlSetRasterValue(int iX, int iY, float fValue);

	/// Returns a random point inside the start region
	bool	ItlGetRandomPoint(glm::ivec2 & riv2RandomPoint);

	/// Moves the given point inside the starting region
	void	ItlMovePoint(glm::ivec2 &rPoint);

	/// Updates the size of the start region
	void	ItlUpdateStartRegion(glm::ivec2 iv2LastDrawnPoint);

	/// Clamps the given point inside the drawing area
	void	ItlClampPointInsideArea(glm::ivec2 & riv2Point);
	

	/// Member variables
	int m_iWindowWidth;
	int m_iWindowHeight;

	unsigned int m_nMaxSteps;

	unsigned int m_nNumDrawnPixels;
	unsigned int m_nNumPixels;

	float		* m_pfRaster;
	float		* m_pfStartRegion;

	GLuint	m_glnVertexArray;
	GLuint	m_glnVertexBuffer;

	GLuint	m_glnRasterTexID;
	GLuint	m_glnStartingRegionTexID;

	GLuint	m_glnShader;

	GLuint	m_glnRasterTexLoc;
	GLuint	m_glnStartingRegionTexLoc;

	TStartRegion m_eStartRegion;
	TGoalRegion m_eGoalRegion;

	glm::ivec2 m_iv2StartingPoint;

	glm::ivec2 m_iv2LinePoint1;
	glm::ivec2 m_iv2LinePoint2;

	// Variables for a square starting region
	glm::ivec2 m_iv2StartRegionMin;
	glm::ivec2 m_iv2StartRegionMax;
	glm::ivec2 m_iv2StartRegionSize;

	glm::ivec2 m_iv2CurDrawnMin;
	glm::ivec2 m_iv2CurDrawnMax;

	// Variables for a circle starting region
	int m_iStartingRegionRadius;


	
};