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

    return app.exec();
}
