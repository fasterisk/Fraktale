#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "CommonIncludes.h"
#include "DLA.h"
#include <qwidget.h>

class DLAWindow : public QWidget
{
	Q_OBJECT

public:
	/// CONSTRUCTION / DESTRUCTION

	/// Constructor
	DLAWindow();

	/// Destructor
	~DLAWindow();

private:
	/// OBJECT MEMBERS

	DLA *	m_pDLA;


};

#endif _WINDOW_H_