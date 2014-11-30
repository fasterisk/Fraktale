#include "DLAWindow.h"
#include "Qt\QtWidgets\qboxlayout.h"

/*********************************************************************************************
*********************************************************************************************/
DLAWindow::DLAWindow()
{
	m_pDLA = new DLA(100, 100, DLA::START_REGION_SQUARE, 200000000, glm::ivec2(50, 50));

	QHBoxLayout * pLayout = new QHBoxLayout;
	pLayout->addWidget(m_pDLA);

	
	setLayout(pLayout);
	setWindowTitle("DLA");
}

/*********************************************************************************************
*********************************************************************************************/
DLAWindow::~DLAWindow()
{
	delete m_pDLA;
}