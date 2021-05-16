#include "AClicker.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	AClicker w;

	w.setWindowIcon(QIcon(":/Icon/Assets/AClickerIcon.ico"));

	w.show();
	return a.exec();
}
