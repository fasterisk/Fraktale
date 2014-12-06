#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "CommonIncludes.h"
#include "DLA.h"
#include <qwidget.h>
#include <qspinbox.h>

class DLAWindow : public QWidget
{
	Q_OBJECT

public:
/// CONSTRUCTION / DESTRUCTION

	/// Constructor
	DLAWindow(QWidget * pParent = NULL);

	/// Destructor
	~DLAWindow();

private slots:
/// SLOTS
	void ItlGoalRegionComboBoxChanged(int iComboBoxIndex);
	void ItlStartRegionComboBoxChanged(int iComboBoxIndex);
	void ItlMaxNumStepsChanged(int iNewMaxNumSteps);
	void ItlPathShouldBeDisplayedCheckBoxToggled(bool bChecked);
	void ItlPauseButtonPressed();
	void ItlResetButtonPressed();
	void ItlLikelihoodLeftChanged(double dLikelihood);
	void ItlLikelihoodRightChanged(double dLikelihood);
	void ItlLikelihoodUpChanged(double dLikelihood);
	void ItlLikelihoodDownChanged(double dLikelihood);

private:

	void ItlUpdateLikelihoodSpinValues();

	/// OBJECT MEMBERS

	DLA *	m_pDLA;

	QDoubleSpinBox * m_pLikelihoodLeftSpinBox;
	QDoubleSpinBox * m_pLikelihoodRightSpinBox;
	QDoubleSpinBox * m_pLikelihoodUpSpinBox;
	QDoubleSpinBox * m_pLikelihoodDownSpinBox;
};

#endif _WINDOW_H_