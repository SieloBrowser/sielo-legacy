#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QTextCodec>
#include <QtWidgets>
#include <QtWebEngineWidgets>
#include <QStyle>

#include "includes/SWidgets/SWebView.hpp"
#include "includes/SWidgets/STabWidget.hpp"
#include "includes/SMainWindow.hpp"
#include "includes/SStarter.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	QString locale = QLocale::system().name().section('_', 0, 0);
	QTranslator translator;
	translator.load(QString("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	app.installTranslator(&translator);

    SStarter *starter{ new SStarter };
    delete starter;

    return app.exec();
}
