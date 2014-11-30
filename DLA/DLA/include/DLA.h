#ifndef _DLA_H_
#define _DLA_H_


#include "CommonIncludes.h"
#include <qgl.h>

class DLA : public QGLWidget
{
public:

/// TYPES
	enum TStartRegion
	{
		START_REGION_SQUARE,
		START_REGION_CIRCLE,
		START_REGION_LINE,

		NUM_START_REGIONS
	};

	enum TGoalRegion
	{
		GOAL_REGION_POINT,
		GOAL_REGION_LINE,

		NUM_GOAL_REGIONS
	};

/// CONSTRUCTION / DESTRUCTION
	/// Constructor
	DLA(int iResolutionX, int iResolutionY, unsigned int nMaxSteps, QWidget * pParent = NULL);

	/// Destructor
	~DLA();

/// ACCESS

	/// Returns the start region type
	TStartRegion	GetStartRegionType() const { return m_eStartRegion; }

	/// Returns the goal region type
	TGoalRegion		GetGoalRegionType() const { return m_eGoalRegion; }

	/// Returns the current resolution of the rendering
	void	GetResolution(int & riResolutionX, int & riResolutionY) const;

	/// Returns the current max steps
	unsigned int	GetMaxSteps() const { return m_nMaxSteps; }

	/// Returns the starting point
	glm::ivec2	GetStartingPoint() const { return m_iv2StartingPoint; }

	/// Returns the first starting line point
	glm::ivec2	GetStartingLinePoint1() const { return m_iv2LinePoint1; }

	/// Returns the second starting line point
	glm::ivec2	GetStartingLinePoint2() const { return m_iv2LinePoint2; }

	/// Returns whether DLA is paused or not
	bool	IsPaused() const { return m_bPaused;  }

/// PUBLIC OPERATIONS
	/// Sets the start region type
	void	SetStartRegionType(TStartRegion eStartRegion);

	/// Sets the goal region type
	void	SetGoalRegionType(TGoalRegion eGoalRegion);

	/// Sets the resolution
	void	SetResolution(int iResolutionX, int iResolutionY);

	/// Sets whether the path should be drawn or not
	void	SetShowPath(bool bShow);

	/// Pauses / unpauses DLA
	void	SetPaused(bool bPaused) { m_bPaused = bPaused;  }

	/// Calculates next element
	void	CalculateNextElement();

	/// Renders the textures
	void	Render();

	/// Resets the algorithm
	void	Reset();

/// IMPLEMENTED PUBLIC OPERATIONS OF QGLWidget
	virtual QSize minimumSizeHint() const;
	virtual QSize sizeHint() const;

protected:
/// IMPLEMENTED PROTECTED OPERATIONS OF QGLWidget
	virtual void	initializeGL();
	virtual void	paintGL();
	virtual void	resizeGL(int width, int height);
	virtual void	mousePressEvent(QMouseEvent * event);
	virtual void	mouseMoveEvent(QMouseEvent * event);

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
	void	ItlSetRasterValue(int iX, int iY, float fNewValue);

	/// Sets a value in the path raster
	void	ItlSetValueInPathRaster(int iX, int iY, float fNewValue);

	/// Returns a random point inside the start region
	bool	ItlGetRandomPoint(glm::ivec2 & riv2RandomPoint);

	/// Moves the given point inside the starting region
	void	ItlMovePoint(glm::ivec2 &rPoint);

	/// Updates the size of the start region
	void	ItlUpdateStartRegion(glm::ivec2 iv2LastDrawnPoint);

	/// Clamps the given point inside the drawing area
	void	ItlClampPointInsideArea(glm::ivec2 & riv2Point);

	/// Resets the path raster
	void	ItlResetPath();


	/// Member variables
	int m_iResolutionX;
	int m_iResolutionY;

	unsigned int m_nMaxSteps;

	unsigned int m_nNumDrawnPixels;
	unsigned int m_nNumPixels;

	float		* m_pfRaster;
	float		* m_pfStartRegion;

	float		* m_pfCurrentPathRaster;

	GLuint	m_glnVertexArray;
	GLuint	m_glnVertexBuffer;

	GLuint	m_glnTexID;

	GLuint	m_glnShader;

	GLuint	m_glnTexLoc;

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

	// Variables for pausing
	bool	m_bPaused;

	bool	m_bShowPath;



};

#endif //_DLA_H_