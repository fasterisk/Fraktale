#ifndef _MJ_WINDOW_H_
#define _MJ_WINDOW_H_

#include "CommonIncludes.h"
#include "Mandelbrot.h"
#include "Julia.h"
#include <qwidget.h>
#include <qspinbox.h>

class MJWindow : public QWidget,
				 public Mandelbrot::IListener
{
	Q_OBJECT

public:
/// CONSTRUCTION / DESTRUCTION

	/// Constructor
	MJWindow(QWidget * pParent = NULL);

	/// Destructor
	~MJWindow();

/// IMPLEMENTED OPERATIONS OF THE Mandelbrot::IListener interface
	virtual void	OnComplexChanged(float fReal, float fImag);

private slots:
/// SLOTS
	void ItlMaxNumIterationsChanged(int iNewMaxNumIterations);

private:

	/// OBJECT MEMBERS

	Mandelbrot *	m_pMandelbrot;
	Julia *			m_pJulia;

	QDoubleSpinBox * m_pLikelihoodLeftSpinBox;
	QDoubleSpinBox * m_pLikelihoodRightSpinBox;
	QDoubleSpinBox * m_pLikelihoodUpSpinBox;
	QDoubleSpinBox * m_pLikelihoodDownSpinBox;
};

#endif //_MJ_WINDOW_H_