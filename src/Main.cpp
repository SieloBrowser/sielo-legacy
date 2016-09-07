#include <QApplication>
#include <QtWidgets>
#include <QtWebEngineWidgets>

#include "SWebView.h"
#include "STabWidget.h"

int main(int argc, char *argv[])
{
	// It's a commentaire for Slack integration
    QApplication app(argc, argv);

	QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);

	QWidget fen;
	STabWidget tab;
	SWebView webView(&tab, QUrl("http://feldrise.com"));

	tab.createWebTab("Teste", &webView);

	QVBoxLayout* mainLayout{ new QVBoxLayout };

	mainLayout->addWidget(&tab);
	fen.setLayout(mainLayout);

	fen.show();

    return app.exec();
}
