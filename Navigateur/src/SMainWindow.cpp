#include "includes/SMainWindow.hpp"
#include "includes/SActions.hpp"

#include <QMessageBox>
#include <QFile>
#include <QCoreApplication>

const unsigned int THEME_V0 = 1;
const unsigned int THEME_V1 = 2;

QString SMainWindow::dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/SieloNavigateurV3/";
QSettings * SMainWindow::SSettings = new QSettings(SMainWindow::dataPath + "snsettings.ini", QSettings::IniFormat);
QVector<SHistoryItem> SMainWindow::curSessionHistory = QVector<SHistoryItem>{};
QVector<SDownloadItem*> SMainWindow::dlItems = QVector<SDownloadItem*>{};

SMainWindow::SMainWindow(QWidget* parent, SWebView *view, bool isPrivateBrowsing) :
	QMainWindow(parent),
	privateBrowsing(isPrivateBrowsing),
	m_actions(QSharedPointer<SActions>(new SActions))
{
	// Set window attributes
	setWindowIcon(QIcon(SMainWindow::dataPath + "Images/icon.ico"));
	setWindowTitle(tr("Sielo Navigateur"));
    setAttribute(Qt::WA_DeleteOnClose);
    resize(SMainWindow::SSettings->value("windowSave/attributes/size", QSize(1024, 768)).toSize());
    if (!QString::number(SMainWindow::SSettings->value("windowSave/attributes/pos", QPoint()).toInt()).isEmpty())
        move(SMainWindow::SSettings->value("windowSave/attributes/pos").toPoint());

	// Set widgets attributes
	m_urlArea->setMinimumWidth(500);
	m_searchArea = new SSearchArea(QIcon(m_actions->themePath + "search-lineedit.png"), this);
	m_searchArea->setMaximumWidth(200);
	m_searchArea->setPlaceholderText(tr("Recherche Google"));

	// TEST AREA

	// END OF TEST AREA

	if(SMainWindow::SSettings->value("preferences/saveTabs", false).toBool() && !view)
		restoreTabs();
    else if(SMainWindow::SSettings->value("preferences/themes/changed", false).toBool()) {
        restoreTabs();
        SMainWindow::SSettings->setValue("preferences/themes/changed", false);
    }
	else if (view) {
		m_tabs->createWebTab(tr("Nouvel onglet"), view);
	}
	else
		m_tabs->createDefaultWebTab();

	// Load menus and tool bar
	loadMenus();
	loadToolBar(m_actions->themePath + "toolBar.txt");
	setCentralWidget(m_tabs);

}


SMainWindow::~SMainWindow()
{
}

void SMainWindow::loadMenus()
{
	// Append menus list
	m_menus.push_back(new SMenu(this, tr("&Fichier"), SMenuType::File));
	m_menus.push_back(new SMenu(this, tr("&Affichage"), SMenuType::Show));
	m_menus.push_back(new SMenu(this, tr("&Navigation"), SMenuType::Brows));
	m_menus.push_back(new SMenu(this, tr("&Téléchargements"), SMenuType::Dl));
    m_menus.push_back(new SMenu(this, tr("&Thèmes"), SMenuType::Theme));
    m_menus.push_back(new SMenu(this, tr("Fa&voris"), SMenuType::Fav));
	m_menus.push_back(new SMenu(this, tr("&Edition"), SMenuType::Edit));
	m_menus.push_back(new SMenu(this, tr("&?"), SMenuType::About));
	
	// Add menus
	menuBar()->addMenu(m_menus[0]);
	menuBar()->addMenu(m_menus[1]);
	menuBar()->addMenu(m_menus[2]);
	menuBar()->addMenu(m_menus[3]);
	menuBar()->addMenu(m_menus[4]);
	menuBar()->addMenu(m_menus[5]);
	menuBar()->addMenu(m_menus[6]);
    menuBar()->addMenu(m_menus[7]);
}

bool SMainWindow::loadToolBar(const QString & filePath)
{
	QFile file{ filePath };

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::critical(this, tr("Erreur"), tr("Impossible d'ouvrir le thème de la barre d'outils"));
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
	case THEME_V1:
		in >> nbreToolBar;

		for (size_t i{ 0 }; i < nbreToolBar; ++i) {
			m_toolsBars.append(new SToolBar("toolBar" + QString::number(i + 1), this));
			m_toolsBars[i]->loadToolBarV1(in);
		}
		break;
	default:
		QMessageBox::critical(this, tr("Erreur"), tr("La version ") + QString::number(version) + tr(" est inconnue"));
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

	if(privateBrowsing)
		setWindowTitle(shorTitle + tr(" - Sielo Navigateur (Navigation privée)"));
	else
		setWindowTitle(shorTitle + tr(" - Sielo Navigateur"));
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
		QMessageBox::critical(this, tr("Erreur"), tr("Impossible de connaitre l'origine de ce signal"));
		return;
	}

	if (view == currentPage()) {
		QString shorTitle{ newTitle };

		if (newTitle.size() > 40)
			shorTitle = newTitle.left(40) + "...";

		if(privateBrowsing)
			setWindowTitle(shorTitle + tr(" - Sielo Navigateur (Navigation privée)"));
		else
			setWindowTitle(shorTitle + tr(" - Sielo Navigateur"));

		m_tabs->setTabText(m_tabs->currentIndex(), shorTitle);
	}
}

void SMainWindow::changeUrl(const QUrl& newUrl)
{
	SWebView* view{ static_cast<SWebView*>(sender()) };

	if (!view) {
		QMessageBox::critical(this, tr("Erreur"), tr("Impossible de connaitre l'origine de ce signal"));
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
		m_actions->showFullScreen->setText(tr("Afficher en plein écran"));
	}
	else {
		showFullScreen();
		m_actions->showFullScreen->setText(tr("Enlever le plein écran"));
	}
}

void SMainWindow::addDownload(QWebEngineDownloadItem *download)
{
	if(dlItems.isEmpty() || download->url() != dlItems[dlItems.size() - 1]->getDownload()->url()) {
		// Convert the download dialg in download action
		SDownloadItem *item{ new SDownloadItem(download, this) };
		QWidgetAction *actionItem{ new QWidgetAction(m_menus[SMenuType::Dl]) };
		actionItem->setDefaultWidget(item);
		m_menus[SMenuType::Dl]->addAction(actionItem);
		dlItems.push_back(item);
//      connect(item, &SDownloadItem::finished, this, &SMainWindow::removeDownload);
	}
}

void SMainWindow::removeDownload()
{
	SDownloadItem *dlItem{ static_cast<SDownloadItem*>(sender()) };
	if(!dlItem)
		return;

	int index{ dlItems.indexOf(dlItem) };

	dlItems.removeOne(dlItem);
	QList<QAction*> downloads{};
	downloads = m_menus[SMenuType::Dl]->actions();
	m_menus[SMenuType::Dl]->removeAction(downloads[index]);
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

	m_tabs->setCurrentIndex(SMainWindow::SSettings->value("focused", 0).toInt());
	SMainWindow::SSettings->endGroup();
}

void SMainWindow::saveTabs()
{
    if (!privateBrowsing) {
        SMainWindow::SSettings->beginGroup("windowSave/tabs");
        SMainWindow::SSettings->remove("");
        SMainWindow::SSettings->setValue("count", m_tabs->count());
        SMainWindow::SSettings->setValue("focused", m_tabs->currentIndex());

        for (int i{ 0 }; i < m_tabs->count(); ++i) {
            m_tabs->setCurrentIndex(i);
            SMainWindow::SSettings->setValue(QString::number(i) + "/name", currentPage()->title());
            SMainWindow::SSettings->setValue(QString::number(i) + "/url", currentPage()->url());
            SMainWindow::SSettings->endGroup();

            if (!SMainWindow::SSettings->value("preferences/enableCookies", true).toBool())
                currentPage()->page()->profile()->cookieStore()->deleteAllCookies();

            SMainWindow::SSettings->beginGroup("windowSave/tabs");
        }
        SMainWindow::SSettings->endGroup();
    }

}

void SMainWindow::saveWinState()
{
    SMainWindow::SSettings->beginGroup("windowSave/attributes");
    SMainWindow::SSettings->setValue("size", size());
    SMainWindow::SSettings->setValue("pos", pos());
    SMainWindow::SSettings->endGroup();
}

void SMainWindow::closeEvent(QCloseEvent * event)
{
    saveTabs();

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

    saveWinState();

	event->accept();
}
