#include "..\includes\SMainWindow.hpp"

#include <QMessageBox>
#include <QFile>

const unsigned int THEME_V0 = 1;
QSettings * SMainWindow::SSettings = new QSettings("data/sdata.ini", QSettings::IniFormat);

SMainWindow::SMainWindow(QWidget* parent) :
	QMainWindow(parent)
{
	setWindowTitle("[S]ielo[N]avigateur V3");
	SWebView* webView{ new SWebView(m_tabs, QUrl("http://google.com")) };
	m_tabs->createWebTab("Google", webView);

//	m_urlArea->hide();
//	m_searchArea->hide();
	m_searchArea->setPlaceholderText("Recherche Google");

	loadToolBar("Themes/SIcons/toolBar.txt");
	setCentralWidget(m_tabs);
}


SMainWindow::~SMainWindow()
{
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

		for (int i{ 0 }; i < nbreToolBar; ++i) {
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

//-- PUBLIC SLOTS

void SMainWindow::changeTitle(const QString& newTitle)
{
	QString shorTitle{ newTitle };

	if (newTitle.size() > 40)
		shorTitle = newTitle.left(40) + "...";

	setWindowTitle(shorTitle + " - [S]ielo [N]avigateur V3");
	m_tabs->setTabText(m_tabs->currentIndex(), shorTitle);
}

void SMainWindow::changeUrl(const QUrl& newUrl)
{

}
