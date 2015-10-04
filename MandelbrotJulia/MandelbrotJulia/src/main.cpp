#include "CommonIncludes.h"
#include "MJWindow.h"
#include <qapplication.h>

/*********************************************************************************************
*********************************************************************************************/
void MyGLFWErrorCallback(int error, const char* description)
{
	std::cerr << description << std::endl;
}

/*********************************************************************************************
*********************************************************************************************/
int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	MJWindow stWindow;
	stWindow.resize(stWindow.sizeHint());

	stWindow.show();

	return app.exec();
}

