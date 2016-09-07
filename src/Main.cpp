#include <QApplication>
#include <QtWidgets>
#include <QtWebEngineWidgets>

#include "SWebView.h"
#include "STabWidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);

	QWidget fen;
	STabWidget tab;
	//QWidget tabPage;
	SWebView webView(&tab, QUrl("http://feldrise.com"));

	QVBoxLayout* mainLayout{ new QVBoxLayout };
	QVBoxLayout* tabLayout{ new QVBoxLayout };

	mainLayout->addWidget(&tab);
	tabLayout->addWidget(&webView);

	webView.setLayout(tabLayout);
	fen.setLayout(mainLayout);

	tab.addTab(&webView, "Page web");

	fen.show();

    return app.exec();
}
