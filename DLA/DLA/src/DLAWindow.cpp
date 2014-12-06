#include "DLAWindow.h"
#include <qgridlayout.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qpushbutton.h>

/*********************************************************************************************
*********************************************************************************************/
DLAWindow::DLAWindow(QWidget * pParent)
	: QWidget(pParent)
{

	QGridLayout * pGrid = new QGridLayout;
	pGrid->setSpacing(20);

	

	unsigned int nRow = 0;

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

	//Create the DLA GL Widget
	m_pDLA = new DLA(600, 600, 100000, this);
	m_pDLA->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	//Add it to the grid
	pGrid->addWidget(m_pDLA, 0, 0, nRow+10, 1);


	//Set the layout and the window title
	setLayout(pGrid);
	setWindowTitle("DLA");

	//Connect the signals
	connect(pMaxNumberStepsSpinBox, SIGNAL(valueChanged(int)), SLOT(ItlMaxNumStepsChanged(int)));
	connect(pGoalRegionTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(ItlGoalRegionComboBoxChanged(int)));
	connect(pStartRegionTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(ItlStartRegionComboBoxChanged(int)));
	connect(pPathShouldBeDisplayedCheckBox, SIGNAL(toggled(bool)), this, SLOT(ItlPathShouldBeDisplayedCheckBoxToggled(bool)));
	connect(pPauseButton, SIGNAL(pressed()), this, SLOT(ItlPauseButtonPressed()));
	connect(pResetButton, SIGNAL(pressed()), this, SLOT(ItlResetButtonPressed()));
	connect(m_pLikelihoodLeftSpinBox, SIGNAL(valueChanged(double)), this, SLOT(ItlLikelihoodLeftChanged(double)));
	connect(m_pLikelihoodRightSpinBox, SIGNAL(valueChanged(double)), this, SLOT(ItlLikelihoodRightChanged(double)));
	connect(m_pLikelihoodUpSpinBox, SIGNAL(valueChanged(double)), this, SLOT(ItlLikelihoodUpChanged(double)));
	connect(m_pLikelihoodDownSpinBox, SIGNAL(valueChanged(double)), this, SLOT(ItlLikelihoodDownChanged(double)));

}

/*********************************************************************************************
*********************************************************************************************/
DLAWindow::~DLAWindow()
{
	delete m_pDLA;
}

/*********************************************************************************************
*********************************************************************************************/
void	DLAWindow::ItlGoalRegionComboBoxChanged(int iComboBoxIndex)
{
	if (iComboBoxIndex == 0)
		m_pDLA->SetGoalRegionType(DLA::GOAL_REGION_POINT);
	if (iComboBoxIndex == 1)
		m_pDLA->SetGoalRegionType(DLA::GOAL_REGION_LINE);
}

/*********************************************************************************************
*********************************************************************************************/
void	DLAWindow::ItlStartRegionComboBoxChanged(int iComboBoxIndex)
{
	if (iComboBoxIndex == 0)
		m_pDLA->SetStartRegionType(DLA::START_REGION_SQUARE);
	if (iComboBoxIndex == 1)
		m_pDLA->SetStartRegionType(DLA::START_REGION_CIRCLE);
	if (iComboBoxIndex == 2)
		m_pDLA->SetStartRegionType(DLA::START_REGION_LINE);
}

/*********************************************************************************************
*********************************************************************************************/
void	DLAWindow::ItlMaxNumStepsChanged(int iNewMaxNumSteps)
{
	m_pDLA->SetMaxNumSteps(iNewMaxNumSteps);
}

/*********************************************************************************************
*********************************************************************************************/
void	DLAWindow::ItlPathShouldBeDisplayedCheckBoxToggled(bool bChecked)
{
	m_pDLA->SetShowPath(bChecked);
}

/*********************************************************************************************
*********************************************************************************************/
void	DLAWindow::ItlPauseButtonPressed()
{
	m_pDLA->SetPaused(!m_pDLA->IsPaused());

	if (m_pDLA->IsPaused())
		setWindowTitle("DLA (paused)");
	else
		setWindowTitle("DLA");
}

/*********************************************************************************************
*********************************************************************************************/
void	DLAWindow::ItlResetButtonPressed()
{
	m_pDLA->Reset();
}

/*********************************************************************************************
*********************************************************************************************/
void	DLAWindow::ItlLikelihoodLeftChanged(double dLikelihood)
{
	m_pDLA->SetLikelihoodLeft((float)dLikelihood);
	ItlUpdateLikelihoodSpinValues();
}

/*********************************************************************************************
*********************************************************************************************/
void	DLAWindow::ItlLikelihoodRightChanged(double dLikelihood)
{
	m_pDLA->SetLikelihoodRight((float)dLikelihood);
	ItlUpdateLikelihoodSpinValues();
}

/*********************************************************************************************
*********************************************************************************************/
void	DLAWindow::ItlLikelihoodUpChanged(double dLikelihood)
{
	m_pDLA->SetLikelihoodUp((float)dLikelihood);
	ItlUpdateLikelihoodSpinValues();
}

/*********************************************************************************************
*********************************************************************************************/
void	DLAWindow::ItlLikelihoodDownChanged(double dLikelihood)
{
	m_pDLA->SetLikelihoodDown((float)dLikelihood);
	ItlUpdateLikelihoodSpinValues();
}

/*********************************************************************************************
*********************************************************************************************/
void	DLAWindow::ItlUpdateLikelihoodSpinValues()
{
	float fLikelihoodLeft, fLikelihoodRight, fLikelihoodUp, fLikelihoodDown;
	m_pDLA->GetLikelihood(fLikelihoodLeft, fLikelihoodRight, fLikelihoodUp, fLikelihoodDown);

	m_pLikelihoodLeftSpinBox->blockSignals(true);
	m_pLikelihoodLeftSpinBox->setValue((double)fLikelihoodLeft);
	m_pLikelihoodLeftSpinBox->blockSignals(false);

	m_pLikelihoodRightSpinBox->blockSignals(true);
	m_pLikelihoodRightSpinBox->setValue((double)fLikelihoodRight);
	m_pLikelihoodRightSpinBox->blockSignals(false);

	m_pLikelihoodUpSpinBox->blockSignals(true);
	m_pLikelihoodUpSpinBox->setValue((double)fLikelihoodUp);
	m_pLikelihoodUpSpinBox->blockSignals(false);

	m_pLikelihoodDownSpinBox->blockSignals(true);
	m_pLikelihoodDownSpinBox->setValue((double)fLikelihoodDown);
	m_pLikelihoodDownSpinBox->blockSignals(false);
}