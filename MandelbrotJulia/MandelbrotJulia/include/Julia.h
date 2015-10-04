#ifndef _JULIA_H_
#define _JULIA_H_


#include "CommonIncludes.h"
#include "TransferFunctions.h"
#include <qgl.h>

class Julia : public QGLWidget
{
public:

	/// TYPES

	/// CONSTRUCTION / DESTRUCTION
	/// Constructor
	Julia(int iResolutionX, int iResolutionY, unsigned int nMaxIterations, QWidget * pParent = NULL);

	/// Destructor
	~Julia();

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

	/// Sets the complex number defining the julia set
	void	SetComplex(float fReal, float fImag);


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

	float m_fComplexReal;
	float m_fComplexImag;
	
	int m_iMouseXBefore;
	int m_iMouseYBefore;

	unsigned int m_nMaxIterations;

	GLuint	m_glnVertexArray;
	GLuint	m_glnVertexBuffer;

	GLuint	m_glnFBO;
	GLuint	m_glnRenderTexture;
	GLuint	m_glnTransferFunctionTexture;

	GLuint	m_glnJuliaShader;
	GLuint	m_glnTextureToScreenShader;

	GLint	m_gliJMaxIterationsLoc;
	GLint	m_gliJLookupTableLoc;
	GLint	m_gliJScaleLoc;
	GLint	m_gliJOffsetLoc;
	GLint	m_gliJComplexLoc;

	GLint	m_gliTTSInputTextureLoc;

	ITransferFunction * m_pTransferFunction;
};

#endif //_JULIA_H_
