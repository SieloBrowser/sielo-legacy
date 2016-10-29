#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>

#include "includes/MainWindow.hpp"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QString locale = QLocale::system().name().section('_', 0, 0);
	QTranslator translator;
	translator.load(QString("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	app.installTranslator(&translator);
	
	MainWindow *window{ new MainWindow(nullptr) };
	window->show();

	return app.exec();
}