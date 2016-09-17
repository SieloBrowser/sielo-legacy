#include <QApplication>
#include <QtWidgets>
#include <QtWebEngineWidgets>

#include "..\includes\SWebView.h"
#include "..\includes\STabWidget.h"
#include "..\includes\SMainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	QCoreApplication::setOrganizationName("Feldrise");
	QCoreApplication::setOrganizationDomain("feldrise.com");
	QCoreApplication::setApplicationName("Sielo NAvigateur V3");

	QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);

	SMainWindow fen;
/*	
	STabWidget tab;
	SWebView webView(&tab, QUrl("http://feldrise.com"));

	tab.createWebTab("Teste", &webView);

	QVBoxLayout* mainLayout{ new QVBoxLayout };

	mainLayout->addWidget(&tab);
	fen.setLayout(mainLayout); 
*/

	fen.show();

    return app.exec();
}
