#include "..\includes\SMainWindow.hpp"
#include "../includes/Actions.hpp"

#include <QMessageBox>
#include <QFile>

const unsigned int THEME_V0 = 1;
QSettings * SMainWindow::SSettings = new QSettings("data/sdata.ini", QSettings::IniFormat);

SMainWindow::SMainWindow(QWidget* parent) :
	QMainWindow(parent),
	m_actions(new Actions)
{
	setWindowTitle("[S]ielo[N]avigateur V3");
	//SWebView* webView{ new SWebView(m_tabs, QUrl("http://google.com")) };
	//m_tabs->createWebTab(tr("Google"), webView);
	//m_tabs->createWebTab(tr("Feldrise"), QUrl("http://feldrise.com"));

	m_tabs->createDefaultWebTab();

//	m_urlArea->hide();
//	m_searchArea->hide();
	m_urlArea->setMinimumWidth(400);
	m_searchArea->setMaximumWidth(200);
	m_searchArea->setPlaceholderText("Recherche Google");

	loadMenus();
	loadToolBar("Themes/SIcons/toolBar.txt");
	setCentralWidget(m_tabs);
}


SMainWindow::~SMainWindow()
{
}

void SMainWindow::loadMenus()
{
	m_menus.push_back(new SMenu(this, "&Fichier", SMenuType::File));
	m_menus.push_back(new SMenu(this, "&Affichage", SMenuType::Show));
	m_menus.push_back(new SMenu(this, "&Navigation", SMenuType::Brows));
	m_menus.push_back(new SMenu(this, "&Fa&voris", SMenuType::Fav));
	m_menus.push_back(new SMenu(this, "&Edition", SMenuType::Edit));
	m_menus.push_back(new SMenu(this, "&?", SMenuType::About));
	
	menuBar()->addMenu(m_menus[0]);
	menuBar()->addMenu(m_menus[1]);
	menuBar()->addMenu(m_menus[2]);
	menuBar()->addMenu(m_menus[3]);
	menuBar()->addMenu(m_menus[4]);
	menuBar()->addMenu(m_menus[5]);
}

bool SMainWindow::loadToolBar(const QString & filePath)
{
	QFile file{ filePath };

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::critical(this, "Error", "Impossible d'ouvrir le theme de la bare d'outile");
		return false;
	}

	QTextStream in{ &file };

	unsigned version{ 0 };
	in >> version;

	unsigned nbreToolBar{ 0 };
	switch (version)
	{
	case THEME_V0:
		in >> nbreToolBar;

		for (size_t i{ 0 }; i < nbreToolBar; ++i) {
			m_toolsBars.append(new SToolBar("toolBar" + QString::number(i + 1), this));
			m_toolsBars[i]->loadToolBarV0(in);
			addToolBar(m_toolsBars[i]);
		}
		break;
	default:
		QMessageBox::critical(this, "Error", "The version " + QString::number(version) + " is unknonw");
		//throw std::runtime_error("The version of current theme is unknown.");
		return false;
		break;
	}

	return true;

}

SWebView * SMainWindow::currentPage()
{
	return m_tabs->currentWidget()->findChild<SWebView *>();
}

//-- PUBLIC SLOTS

void SMainWindow::changeTitle(const QString& newTitle)
{
	SWebView* view{ static_cast<SWebView*>(sender()) };

	if (!view) {
		QMessageBox::critical(this, "Error", "Failed to know the sender of new title signal");
		return;
	}

	if (view == currentPage()) {
		QString shorTitle{ newTitle };

		if (newTitle.size() > 40)
			shorTitle = newTitle.left(40) + "...";

		setWindowTitle(shorTitle + " - [S]ielo [N]avigateur V3");
		m_tabs->setTabText(m_tabs->currentIndex(), shorTitle);
	}
}

void SMainWindow::changeTabTitle(const QString& newTitle)
{
	QString shorTitle{ newTitle };

	if (newTitle.size() > 40)
		shorTitle = newTitle.left(40) + "...";

	setWindowTitle(shorTitle + " - [S]ielo [N]avigateur V3");
}


void SMainWindow::changeUrl(const QUrl& newUrl)
{
	SWebView* view{ static_cast<SWebView*>(sender()) };

	if (!view) {
		QMessageBox::critical(this, "Error", "Failed to know the sender of new title signal");
		return;
	}

	if (view == currentPage()) {
		if (newUrl.toString() != tr("html/page_blanche"))
			m_urlArea->setText(newUrl.toString());
	}
}

void SMainWindow::fullScreen()
{
	if (isFullScreen()) {
		showNormal();
		m_actions->showFullScreen->setText("Afficher en pleine écran");
	}
	else {
		showFullScreen();
		m_actions->showFullScreen->setText("Enlever le pleine écran");
	}
}

void SMainWindow::back()
{
	currentPage()->back();
}

void SMainWindow::next()
{
	currentPage()->forward();
}

void SMainWindow::home()
{
	currentPage()->load(SMainWindow::SSettings->value("preferences/homePage", "http://google.com").toUrl());
}

void SMainWindow::refresh()
{
	currentPage()->reload();
}

void SMainWindow::stop()
{
	currentPage()->stop();
}

void SMainWindow::changeTabUrl(const QUrl& newUrl)
{
	if (newUrl.toString() != tr("html/page_blanche"))
		m_urlArea->setText(newUrl.toString());

}
