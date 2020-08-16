#include "flooding.h"
#include "type.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	srand(random_seed);

	QApplication a(argc, argv);
	flooding w;
	w.show();
	return a.exec();
}
