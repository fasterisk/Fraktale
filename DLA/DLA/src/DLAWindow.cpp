#include "DLAWindow.h"
#include <qgridlayout.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qspinbox.h>

/*********************************************************************************************
*********************************************************************************************/
DLAWindow::DLAWindow()
{

	QGridLayout * pGrid = new QGridLayout;
	pGrid->setSpacing(20);

	

	unsigned int nRow = 0;

	//Create a label for the resolution
	/*pGrid->addWidget(new QLabel("Resolution:"), nRow, 1);
	//Create a spin box for the resolution:
	QSpinBox * pResolutionSpinBox = new QSpinBox;
	pResolutionSpinBox->setMinimum(50);
	pResolutionSpinBox->setMaximum(4096);
	pResolutionSpinBox->setValue(600);
	//Add it to the grid
	pGrid->addWidget(pResolutionSpinBox, nRow, 2);*/

	//nRow++;

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

	//Create the DLA GL Widget
	m_pDLA = new DLA(600, 600, 200000000);
	m_pDLA->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	//Add it to the grid
	pGrid->addWidget(m_pDLA, 0, 0, nRow, 1);


	//Set the layout and the window title
	setLayout(pGrid);
	setWindowTitle("DLA");

	//Connect the signals
	//connect(pResolutionSpinBox, SIGNAL(valueChanged(int)), SLOT(ItlResolutionChanged(int)));
	connect(pGoalRegionTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(ItlGoalRegionComboBoxChanged(int)));
	connect(pStartRegionTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(ItlStartRegionComboBoxChanged(int)));
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
void	DLAWindow::ItlResolutionChanged(int iNewResolution)
{

	m_pDLA->SetResolution(iNewResolution, iNewResolution);
}

