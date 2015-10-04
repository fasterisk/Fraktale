#include "MJWindow.h"
#include <qgridlayout.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <string>
#include <sstream>

/*********************************************************************************************
*********************************************************************************************/
MJWindow::MJWindow(QWidget * pParent)
	: QWidget(pParent)
{

	QGridLayout * pGrid = new QGridLayout;
	pGrid->setSpacing(20);

	//Create the Mandelbrot GL Widget
	m_pMandelbrot = new Mandelbrot(512, 512, 5000, this);
	m_pMandelbrot->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	//Create the Julia GL Widget
	m_pJulia = new Julia(512, 512, 5000, this);
	m_pJulia->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	OnComplexChanged(0.0f, 0.0f);

	//Add it to the grid
	pGrid->addWidget(m_pMandelbrot, 0, 0, 1, 1);
	pGrid->addWidget(m_pJulia, 0, 1, 1, 1);


	//Set the layout and the window title
	setLayout(pGrid);
}

/*********************************************************************************************
*********************************************************************************************/
MJWindow::~MJWindow()
{
	delete m_pMandelbrot;
}

/*********************************************************************************************
*********************************************************************************************/
void	MJWindow::OnComplexChanged(float fReal, float fImag)
{
	if (m_pJulia != NULL)
		m_pJulia->SetComplex(fReal, fImag);

	std::ostringstream ss;
	ss.precision(3);
	ss << "Mandelbrot | Julia | C = ";
	ss << fReal;
	if (fImag >= 0)
		ss << " + ";
	else
		ss << " ";
	ss << fImag;
	ss << "i";

	setWindowTitle(ss.str().c_str());
}

/*********************************************************************************************
*********************************************************************************************/
void	MJWindow::ItlMaxNumIterationsChanged(int iNewMaxNumIterations)
{
	m_pMandelbrot->SetMaxNumIterations(iNewMaxNumIterations);
}

