#ifndef _MANDELBROT_H_
#define _MANDELBROT_H_


#include "CommonIncludes.h"
#include "TransferFunctions.h"
#include <qgl.h>

class MJWindow;

class Mandelbrot : public QGLWidget
{
public:

/// TYPES
	class IListener
	{
	public:
		virtual void	OnComplexChanged(float fReal, float fImag) = 0;
	};

/// CONSTRUCTION / DESTRUCTION
	/// Constructor
	Mandelbrot(int iResolutionX, int iResolutionY, unsigned int nMaxIterations, MJWindow * pParent = NULL);

	/// Destructor
	~Mandelbrot();

/// ACCESS

	/// Returns the current resolution of the rendering
	void	GetResolution(int & riResolutionX, int & riResolutionY) const;

	/// Returns the current max iterations
	unsigned int	GetMaxIterations() const { return m_nMaxIterations; }

/// PUBLIC OPERATIONS

	/// Sets the resolution
	void	SetResolution(int iResolutionX, int iResolutionY);

	/// Sets the maximal number of iterations
	void	SetMaxNumIterations(int iNumIterations);

	/// Sets the x and y offset
	void	SetOffset(float fXOffset, float fYOffset);

	/// Sets the scaling of the fractal
	void	SetScale(float fScale);

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
	virtual void	mouseReleaseEvent(QMouseEvent * event);
	virtual void	wheelEvent(QWheelEvent * event);

private:
/// INITIALIZATION

	/// Initializes the shader and textures
	void	ItlInitializeShaderAndTextures();

	/// Loads a shader
	GLuint	ItlLoadShader(std::string sPath, int iShaderType);

	/// Create shader
	GLuint	ItlCreateShader(std::string sVertexShaderPath, std::string sFragmentShaderPath);


/// MEMBERS
	int m_iResolutionX;
	int m_iResolutionY;

	float m_fScale;
	float m_fOffsetX;
	float m_fOffsetY;

	int m_iMouseXBefore;
	int m_iMouseYBefore;

	bool m_bLeftMousePressed;
	bool m_bMiddleMousePressed;

	unsigned int m_nMaxIterations;

	GLuint	m_glnVertexArray;
	GLuint	m_glnVertexBuffer;

	GLuint	m_glnRenderTexture;
	GLuint	m_glnTransferFunctionTexture;

	GLuint	m_glnShader;

	GLint	m_gliMaxIterationsLoc;
	GLint	m_gliLookupTableLoc;
	GLint	m_gliScaleLoc;
	GLint	m_gliOffsetLoc;

	ITransferFunction * m_pTransferFunction;

	MJWindow * m_pParentWindow;

};

#endif //_MANDELBROT_H_
