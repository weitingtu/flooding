#include "flooding.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	srand(0);

	QApplication a(argc, argv);
	flooding w;
	w.show();
	return a.exec();
}