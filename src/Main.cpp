#include <QApplication>
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

//	QMessageBox::information(nullptr, "DEBUG", SMainWindow::dataPath);
    SStarter *starter{ new SStarter };
    delete starter;

    SMainWindow* fen{ new SMainWindow() };

	QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);
    QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::PluginsEnabled, SMainWindow::SSettings->value("preferences/enablePlugins", true).toBool());
    QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::JavascriptEnabled, SMainWindow::SSettings->value("preferences/enableJavascript", true).toBool());

    fen->show();

    return app.exec();
}
