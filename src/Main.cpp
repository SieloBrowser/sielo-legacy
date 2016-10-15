#include <QApplication>
#include <QTextCodec>
#include <QtWidgets>
#include <QtWebEngineWidgets>
#include <QStyle>

#include "includes/SWidgets/SWebView.hpp"
#include "includes/SWidgets/STabWidget.hpp"
#include "includes/SMainWindow.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

//	QApplication::setStyle(QStyleFactory::create("Fusion"));

    QCoreApplication::setOrganizationName("Feldrise");
    QCoreApplication::setOrganizationDomain("feldrise.com");
    QCoreApplication::setApplicationName("Sielo NAvigateur V3");

	QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);

    SMainWindow* fen{ new SMainWindow };

    QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::PluginsEnabled, SMainWindow::SSettings->value("preferences/enablePlugins", true).toBool());
    QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::JavascriptEnabled, SMainWindow::SSettings->value("preferences/enableJavascript", true).toBool());
/*
	STabWidget tab;
	SWebView webView(&tab, QUrl("http://feldrise.com"));

	tab.createWebTab("Teste", &webView);

	QVBoxLayout* mainLayout{ new QVBoxLayout };

	mainLayout->addWidget(&tab);
	fen.setLayout(mainLayout); 
*/

    fen->show();

    return app.exec();
}
