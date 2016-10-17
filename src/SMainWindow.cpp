#include "includes/SMainWindow.hpp"
#include "includes/SActions.hpp"

#include <QMessageBox>
#include <QFile>

const unsigned int THEME_V0 = 1;
QString SMainWindow::dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/SieloNavigateurV3/";
QSettings * SMainWindow::SSettings = new QSettings(SMainWindow::dataPath + "snsettings.ini", QSettings::IniFormat);
QVector<SHistoryItem> SMainWindow::curSessionHistory = QVector<SHistoryItem>{};
QVector<SDownloadItem*> SMainWindow::dlItems = QVector<SDownloadItem*>{};

SMainWindow::SMainWindow(QWidget* parent) :
    QMainWindow(parent),
    m_actions(QSharedPointer<SActions>(new SActions))
{
	setWindowIcon(QIcon(SMainWindow::dataPath + "Images/icon.ico"));
	setWindowTitle("[S]ielo[N]avigateur V3");
    resize(1024, 768);
	//SWebView* webView{ new SWebView(m_tabs, QUrl("http://google.com")) };
	//m_tabs->createWebTab(tr("Google"), webView);
	//m_tabs->createWebTab(tr("Feldrise"), QUrl("http://feldrise.com"));

//	m_urlArea->hide();
//	m_searchArea->hide();
	m_urlArea->setMinimumWidth(400);
    m_searchArea = new SSearchArea(QIcon(m_actions->themePath + "search-lineedit.png"), this);
	m_searchArea->setMaximumWidth(200);
	m_searchArea->setPlaceholderText("Recherche Google");

    // TEST AREA

    // END OF TEST AREA

    if(SMainWindow::SSettings->value("preferences/saveTabs", false).toBool())
        restoreTabs();
    else
        m_tabs->createDefaultWebTab();

	loadMenus();
    loadToolBar(m_actions->themePath + "/toolBar.txt");
	setCentralWidget(m_tabs);

	setAttribute(Qt::WA_DeleteOnClose);
}


SMainWindow::~SMainWindow()
{
}

void SMainWindow::loadMenus()
{
	m_menus.push_back(new SMenu(this, "&Fichier", SMenuType::File));
	m_menus.push_back(new SMenu(this, "&Affichage", SMenuType::Show));
	m_menus.push_back(new SMenu(this, "&Navigation", SMenuType::Brows));
    m_menus.push_back(new SMenu(this, "&Téléchargement", SMenuType::Dl));
	m_menus.push_back(new SMenu(this, "Fa&voris", SMenuType::Fav));
	m_menus.push_back(new SMenu(this, "&Edition", SMenuType::Edit));
	m_menus.push_back(new SMenu(this, "&?", SMenuType::About));
	
	menuBar()->addMenu(m_menus[0]);
	menuBar()->addMenu(m_menus[1]);
	menuBar()->addMenu(m_menus[2]);
	menuBar()->addMenu(m_menus[3]);
	menuBar()->addMenu(m_menus[4]);
	menuBar()->addMenu(m_menus[5]);
    menuBar()->addMenu(m_menus[6]);
}

bool SMainWindow::loadToolBar(const QString & filePath)
{
	QFile file{ filePath };

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Impossible d'ouvrir le theme de la bare d'outile : " + filePath);
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

void SMainWindow::changeTabTitle(const QString& newTitle)
{
	QString shorTitle{ newTitle };

	if (newTitle.size() > 40)
		shorTitle = newTitle.left(40) + "...";

	setWindowTitle(shorTitle + " - [S]ielo [N]avigateur V3");
}

void SMainWindow::changeTabUrl(const QUrl& newUrl)
{
	if (newUrl.toString() != tr("html/page_blanche"))
		m_urlArea->setText(newUrl.toString());

}

void SMainWindow::addHistoryItem(QString title, QUrl url)
{
	SHistoryItem item{};
	item.title = title;
    item.url = url;
    SMainWindow::curSessionHistory.push_back(item);
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

        if(privateBrowsing)
            setWindowTitle(shorTitle + " - [S]ielo [N]avigateur V3 (Navigation privée)");
        else
            setWindowTitle(shorTitle + " - [S]ielo [N]avigateur V3");

		m_tabs->setTabText(m_tabs->currentIndex(), shorTitle);
	}
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

void SMainWindow::addDownload(QWebEngineDownloadItem *download)
{
    if(dlItems.isEmpty() || download->url() != dlItems[dlItems.size() - 1]->getDownload()->url()) {
        SDownloadItem *item{ new SDownloadItem(download, this) };
        QWidgetAction *actionItem{ new QWidgetAction(m_menus[SMenuType::Dl]) };
        actionItem->setDefaultWidget(item);
        m_menus[SMenuType::Dl]->addAction(actionItem);
        dlItems.push_back(item);
        connect(item, &SDownloadItem::finished, this, &SMainWindow::removeDownload);
    }
}

void SMainWindow::removeDownload()
{
    QMessageBox::information(this, "DEBUG", "Close DL !!");
    SDownloadItem *dlItem{ static_cast<SDownloadItem*>(sender()) };
    if(!dlItem)
        return;

    int index{ dlItems.indexOf(dlItem) };

    dlItems.removeOne(dlItem);
    QList<QAction*> downloads{};
    downloads = m_menus[SMenuType::Dl]->actions();
    m_menus[SMenuType::Dl]->removeAction(downloads[index]);

    QMessageBox::information(this, "DEBUG", QString::number(index));
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

void SMainWindow::restoreTabs()
{
    SMainWindow::SSettings->beginGroup("windowSave/tabs");
    for(int i{ 0 }; i < SMainWindow::SSettings->value("count", 1).toInt(); ++i) {
        m_tabs->createWebTab(SMainWindow::SSettings->value(QString::number(i) + "/name", "Google").toString(), SMainWindow::SSettings->value(QString::number(i) + "/url", "http://google.com").toUrl());
    }

    m_tabs->createPlusTab();
    m_tabs->setCurrentIndex(SMainWindow::SSettings->value("focused", 0).toInt());
    SMainWindow::SSettings->endGroup();
}

void SMainWindow::closeEvent(QCloseEvent * event)
{
    SMainWindow::SSettings->beginGroup("windowSave/tabs");
    SMainWindow::SSettings->remove("");
    SMainWindow::SSettings->setValue("count", m_tabs->count() - 1);
    SMainWindow::SSettings->setValue("focused", m_tabs->currentIndex());

    for (int i{ 0 }; i < m_tabs->count() - 1; ++i) {
        m_tabs->setCurrentIndex(i);
        SMainWindow::SSettings->setValue(QString::number(i) + "/name", currentPage()->title());
        SMainWindow::SSettings->setValue(QString::number(i) + "/url", currentPage()->url());
		SMainWindow::SSettings->endGroup();

		if (!SMainWindow::SSettings->value("preferences/enableCookies", true).toBool()) 
			currentPage()->page()->profile()->cookieStore()->deleteAllCookies();

		SMainWindow::SSettings->beginGroup("windowSave/tabs");
    }
    SMainWindow::SSettings->endGroup();

    QDate date{ QDate::currentDate() };


    SMainWindow::SSettings->beginGroup("History/" + QString::number(date.year()) + "/" + QString::number(date.month()) + "/" + QString::number(date.day()));
    int itemNum{ SMainWindow::SSettings->value("itemNum", 0).toInt() };

    for (int i{ 0 }; i < SMainWindow::curSessionHistory.size(); ++i) {
        SMainWindow::SSettings->setValue(QString::number(itemNum) + "/title", SMainWindow::curSessionHistory[i].title);
        SMainWindow::SSettings->setValue(QString::number(itemNum) + "/url", SMainWindow::curSessionHistory[i].url);
		++itemNum;
	}
	SMainWindow::SSettings->setValue("itemNum", itemNum);
	SMainWindow::SSettings->endGroup();

    SMainWindow::curSessionHistory.clear();

	event->accept();
}
