#include <QApplication>
#include <QtWidgets>
#include <QtWebEngineWidgets>

#include "SWebView.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);

	QWidget fen;
	SWebView web(&fen, QUrl("http://youtube.com"));

	QVBoxLayout* layout{ new QVBoxLayout };
	layout->addWidget(&web);

	fen.setLayout(layout);
	fen.show();

    return app.exec();
}
