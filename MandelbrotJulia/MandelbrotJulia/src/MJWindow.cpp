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

	unsigned int nRow = 0;

	//Create a spin box for the resolution
	/*QSpinBox * pResolutionSpinBox = new QSpinBox;
	pResolutionSpinBox->setMinimum(64);
	pResolutionSpinBox->setMaximum(4096);
	pResolutionSpinBox->setValue(512);
	//Add it to the grid
	pGrid->addWidget(new QLabel("Resolution:"), nRow, 1);
	pGrid->addWidget(pResolutionSpinBox, nRow, 2);

	nRow++;

	//Create a combo box for the global region type
	QComboBox * pGoalRegionTypeComboBox = new QComboBox();
	pGoalRegionTypeComboBox->insertItem(0, "Point");
	pGoalRegionTypeComboBox->insertItem(1, "Line");
	//Add it to the grid
	pGrid->addWidget(new QLabel("Goal region type:"), nRow, 1, 1, 1);
	pGrid->addWidget(pGoalRegionTypeComboBox, nRow, 2, 1, 1);

	nRow++;

	//Create a combo box for the start region type
	QComboBox * pStartRegionTypeComboBox = new QComboBox();
	pStartRegionTypeComboBox->insertItem(0, "Square");
	pStartRegionTypeComboBox->insertItem(1, "Circle");
	pStartRegionTypeComboBox->insertItem(2, "Line");
	//Add it to the grid
	pGrid->addWidget(new QLabel("Start region type:"), nRow, 1, 1, 1);
	pGrid->addWidget(pStartRegionTypeComboBox, nRow, 2, 1, 1);

	nRow++;

	//Create a spin box for the maximal number of steps
	QSpinBox * pMaxNumberStepsSpinBox = new QSpinBox;
	pMaxNumberStepsSpinBox->setMinimum(1);
	pMaxNumberStepsSpinBox->setMaximum(999999999);
	pMaxNumberStepsSpinBox->setValue(100000);
	//Add it to the grid
	pGrid->addWidget(new QLabel("Maximal number of steps:"), nRow, 1);
	pGrid->addWidget(pMaxNumberStepsSpinBox, nRow, 2);

	nRow++;

	//Create 4 spin boxes for the likelihood
	m_pLikelihoodLeftSpinBox = new QDoubleSpinBox;
	m_pLikelihoodLeftSpinBox->setMinimum(0.0);
	m_pLikelihoodLeftSpinBox->setMaximum(1.0);
	m_pLikelihoodLeftSpinBox->setValue(0.25);
	m_pLikelihoodLeftSpinBox->setSingleStep(0.01);
	//Add it to the grid
	pGrid->addWidget(new QLabel("Likelihood left:"), nRow, 1);
	pGrid->addWidget(m_pLikelihoodLeftSpinBox, nRow, 2);
	nRow++;

	m_pLikelihoodRightSpinBox = new QDoubleSpinBox;
	m_pLikelihoodRightSpinBox->setMinimum(0.0);
	m_pLikelihoodRightSpinBox->setMaximum(1.0);
	m_pLikelihoodRightSpinBox->setValue(0.25);
	m_pLikelihoodRightSpinBox->setSingleStep(0.01);
	//Add it to the grid
	pGrid->addWidget(new QLabel("Likelihood right:"), nRow, 1);
	pGrid->addWidget(m_pLikelihoodRightSpinBox, nRow, 2);
	nRow++;

	m_pLikelihoodUpSpinBox = new QDoubleSpinBox;
	m_pLikelihoodUpSpinBox->setMinimum(0.0);
	m_pLikelihoodUpSpinBox->setMaximum(1.0);
	m_pLikelihoodUpSpinBox->setValue(0.25);
	m_pLikelihoodUpSpinBox->setSingleStep(0.01);
	//Add it to the grid
	pGrid->addWidget(new QLabel("Likelihood up:"), nRow, 1);
	pGrid->addWidget(m_pLikelihoodUpSpinBox, nRow, 2);
	nRow++;

	m_pLikelihoodDownSpinBox = new QDoubleSpinBox;
	m_pLikelihoodDownSpinBox->setMinimum(0.0);
	m_pLikelihoodDownSpinBox->setMaximum(1.0);
	m_pLikelihoodDownSpinBox->setValue(0.25);
	m_pLikelihoodDownSpinBox->setSingleStep(0.01);
	//Add it to the grid
	pGrid->addWidget(new QLabel("Likelihood down:"), nRow, 1);
	pGrid->addWidget(m_pLikelihoodDownSpinBox, nRow, 2);
	nRow++;

	//Create a check button if the path should be displayed
	QCheckBox * pPathShouldBeDisplayedCheckBox = new QCheckBox("Display path");
	pPathShouldBeDisplayedCheckBox->setChecked(false);
	//Add it to the grid
	pGrid->addWidget(pPathShouldBeDisplayedCheckBox, nRow, 1, 1, 2);

	nRow++;

	//Create a button for resetting
	QPushButton * pResetButton = new QPushButton("Reset");
	//Add it to the grid
	pGrid->addWidget(pResetButton, nRow, 1, 1, 1);

	//Create a button for pausing
	QPushButton * pPauseButton = new QPushButton("Pause / unpause");
	//Add it to the grid
	pGrid->addWidget(pPauseButton, nRow, 2, 1, 1);

	nRow++;
	*/
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

	//Connect the signals
	/*connect(pResolutionSpinBox, SIGNAL(valueChanged(int)), SLOT(ItlResolutionChanged(int)));
	connect(pMaxNumberStepsSpinBox, SIGNAL(valueChanged(int)), SLOT(ItlMaxNumStepsChanged(int)));
	connect(pGoalRegionTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(ItlGoalRegionComboBoxChanged(int)));
	connect(pStartRegionTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(ItlStartRegionComboBoxChanged(int)));
	connect(pPathShouldBeDisplayedCheckBox, SIGNAL(toggled(bool)), this, SLOT(ItlPathShouldBeDisplayedCheckBoxToggled(bool)));
	connect(pPauseButton, SIGNAL(pressed()), this, SLOT(ItlPauseButtonPressed()));
	connect(pResetButton, SIGNAL(pressed()), this, SLOT(ItlResetButtonPressed()));
	connect(m_pLikelihoodLeftSpinBox, SIGNAL(valueChanged(double)), this, SLOT(ItlLikelihoodLeftChanged(double)));
	connect(m_pLikelihoodRightSpinBox, SIGNAL(valueChanged(double)), this, SLOT(ItlLikelihoodRightChanged(double)));
	connect(m_pLikelihoodUpSpinBox, SIGNAL(valueChanged(double)), this, SLOT(ItlLikelihoodUpChanged(double)));
	connect(m_pLikelihoodDownSpinBox, SIGNAL(valueChanged(double)), this, SLOT(ItlLikelihoodDownChanged(double)));*/

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

