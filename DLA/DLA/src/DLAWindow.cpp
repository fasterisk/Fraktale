#include "DLAWindow.h"
#include <qgridlayout.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qspinbox.h>
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
	pGrid->addWidget(new QLabel("Maximal number of steps:"), nRow, 1);
	QSpinBox * pMaxNumberStepsSpinBox = new QSpinBox;
	pMaxNumberStepsSpinBox->setMinimum(1);
	pMaxNumberStepsSpinBox->setMaximum(999999999);
	pMaxNumberStepsSpinBox->setValue(100000);
	//Add it to the grid
	pGrid->addWidget(pMaxNumberStepsSpinBox, nRow, 2);

	nRow++;

	//Create a check button if the path should be displayed
	QCheckBox * pPathShouldBeDisplayedCheckBox = new QCheckBox("Display path");
	pPathShouldBeDisplayedCheckBox->setChecked(false);
	//Add it to the grid
	pGrid->addWidget(pPathShouldBeDisplayedCheckBox, nRow, 1, 1, 2);

	nRow++;

	//Create a button for pausing
	QPushButton * pPauseButton = new QPushButton("Pause / unpause");
	//Add it to the grid
	pGrid->addWidget(pPauseButton, nRow, 1, 1, 2);

	nRow++;

	//Create the DLA GL Widget
	m_pDLA = new DLA(600, 600, 200000000, this);
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