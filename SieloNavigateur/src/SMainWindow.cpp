#include "includes/SMainWindow.hpp"
#include "includes/SActions.hpp"

#include <QMessageBox>
#include <QFile>
#include <QCoreApplication>
#include <QXmlStreamReader>

const unsigned int THEME_V0 = 1;
const unsigned int THEME_V1 = 2;
const unsigned int THEME_V2 = 3;

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
	m_urlArea->move(-50, -50);
	
	m_searchArea = new SSearchArea(QIcon(m_actions->themePath + "search-lineedit.png"), this);
	m_searchArea->setMaximumWidth(200);
	m_searchArea->setPlaceholderText(tr("Recherche Google"));
	m_searchArea->move(-50, -50);

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
	loadToolBar(m_actions->themePath + "toolBar");

    m_splitter->addWidget(m_tabs);
    setCentralWidget(m_splitter);
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

	if (SMainWindow::SSettings->value("preferences/showMenuBar", false).toBool())
		menuBar()->setVisible(true);
	else 
		menuBar()->setVisible(false);
}

bool SMainWindow::loadToolBar(const QString & filePath)
{
	QFileInfo check_file(filePath + ".fielo");
	QFile file{};
	bool isFieloFile{ false };
	if (check_file.exists() && check_file.isFile()) {
		isFieloFile = true;
		file.setFileName(filePath + ".fielo");
	}
	else
		file.setFileName(filePath + ".txt");

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::critical(this, tr("Erreur"), tr("Impossible d'ouvrir le thème de la barre d'outils"));
		return false;
	}

	if (!isFieloFile) {
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
	}
	else {
		QXmlStreamReader fielo{};
		fielo.setDevice(&file);
		unsigned i{ 0 };

		if (fielo.readNextStartElement()) {
			if (fielo.name() == "fielo" && fielo.attributes().value("version") == "1.0") {
				while (fielo.readNextStartElement()) {
					if (fielo.name() == "toolbar") {
						m_toolsBars.append(new SToolBar("toolBar" + QString::number(i + 1), this));
						m_toolsBars[i]->loadToolBarV2(&fielo);
						++i;
					}
					else
						fielo.skipCurrentElement();
				}
			}
			else
				fielo.raiseError(tr("Le thème n'est pas un thème valide"));
		}
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

	if (currentPage() && view == currentPage()) {
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

QColor SMainWindow::getWindowFrameColor()
{
#ifdef Q_OS_WIN
	DWORD ColorizationColor;
	DWORD ColorizationColorBalance;
	DWORD size = sizeof(DWORD);

	RegGetValue(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM", L"ColorizationColor", RRF_RT_REG_DWORD, 0, &ColorizationColor, &size);
	RegGetValue(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM", L"ColorizationColorBalance", RRF_RT_REG_DWORD, 0, &ColorizationColorBalance, &size);

	BYTE ALPHA = 255 * ColorizationColorBalance / 100; // Convert from 0-100 to 0-255
	BYTE RED = (ColorizationColor >> 16) & 0xFF;
	BYTE GREEN = (ColorizationColor >> 8) & 0xFF;
	BYTE BLUE = ColorizationColor & 0xFF;

	return QColor(RED, GREEN, BLUE, ALPHA);
#endif

	return QColor("#F0F0F0");
}

void SMainWindow::changeUrl(const QUrl& newUrl)
{
	SWebView* view{ static_cast<SWebView*>(sender()) };

	if (!view) {
		QMessageBox::critical(this, tr("Erreur"), tr("Impossible de connaitre l'origine de ce signal"));
		return;
	}

	if (currentPage() && view == currentPage()) {
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

void SMainWindow::separateVideo()
{
	QRegExp youtubeRegex{ R"regex(^.*(youtu\.be\/|v\/|u\/\w\/|embed\/|watch\?v=|\&v=)([^#\&\?]*).*)regex" };
	QPushButton* separateButton{ static_cast<QPushButton*>(sender()) };
	QWidget *videoWidget{ static_cast<QWidget*>(separateButton->parent()) };
	SWebView *videoView{ videoWidget->findChild<SWebView*>() };
	bool isYouTubeVideo{ videoView->url().toString().contains(youtubeRegex) };

	videoView->page()->runJavaScript("document.getElementById(\"movie_player\").click()", [](const QVariant &result) {});

	videoView->load(QUrl("https://www.youtube.com/embed/" + youtubeRegex.cap(2) + "?autoplay=1"));
	videoWidget->setParent(nullptr);
	videoWidget->resize(this->width() / 3, this->height() / 3);
	videoWidget->move(this->pos().x() + (this->width() / 3 * 2) - 20, this->pos().y() + (this->height() / 3 * 2) - 10);
	videoWidget->setWindowFlags(videoWidget->windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	videoWidget->show();

	separateButton->setText(tr("Attacher la vidéo"));
	separateButton->resize(separateButton->width(), 10);
	disconnect(separateButton, &QPushButton::clicked, this, &SMainWindow::separateVideo);
	connect(separateButton, &QPushButton::clicked, this, &SMainWindow::attachVideo);
}

void SMainWindow::attachVideo()
{
	QRegExp youtubeRegex{ R"regex(^.*(youtu\.be\/|v\/|u\/\w\/|embed\/|watch\?v=|\&v=)([^#\&\?]*).*)regex" };
	QPushButton* separateButton{ static_cast<QPushButton*>(sender()) };
	QWidget *videoWidget{ static_cast<QWidget*>(separateButton->parent()) };
	SWebView *videoView{ videoWidget->findChild<SWebView*>() };
	bool isYouTubeVideo{ videoView->url().toString().contains(youtubeRegex) };

	videoView->page()->runJavaScript("document.getElementById(\"movie_player\").click()", [](const QVariant &result) {});

	videoView->load(QUrl("https://youtu.be/" + youtubeRegex.cap(2)));
	m_tabs->addTab(videoWidget, videoView->title());
	m_tabs->setCurrentWidget(videoWidget);

	separateButton->setText(tr("Détacher la vidéo"));
	disconnect(separateButton, &QPushButton::clicked, this, &SMainWindow::attachVideo);
	connect(separateButton, &QPushButton::clicked, this, &SMainWindow::separateVideo);
}

void SMainWindow::back()
{
	if (currentPage())
		currentPage()->back();
}

void SMainWindow::next()
{
	if (currentPage())
		currentPage()->forward();
}

void SMainWindow::home()
{
	if (currentPage())
		currentPage()->load(SMainWindow::SSettings->value("preferences/homePage", "http://google.com").toUrl());
}

void SMainWindow::refresh()
{
	if (currentPage())
		currentPage()->reload();
}

void SMainWindow::stop()
{
	if (currentPage()) {
		currentPage()->stop();
		m_actions->refreshOrStop->setIcon(QIcon(m_actions->themePath + "refresh.png"));
		m_actions->refreshOrStop->setText(tr("Rafraîchir la page"));
		m_actions->refreshOrStop->setShortcuts(QKeySequence::Refresh);
		disconnect(m_actions->refreshOrStop, &QAction::triggered, this, &SMainWindow::stop);
		connect(m_actions->refreshOrStop, &QAction::triggered, this, &SMainWindow::refresh);
	}
}

void SMainWindow::restoreTabs()
{
	SMainWindow::SSettings->beginGroup("windowSave/tabs");
    for(int i{ 0 }; i < SMainWindow::SSettings->value("count", 1).toInt(); ++i) {
        m_tabs = new STabWidget(this);
        for(int j{ 0 }; j < SMainWindow::SSettings->value(QString::number(i) + "/count", 1).toInt(); ++j) {
			if (SMainWindow::SSettings->value(QString::number(i) + "/" + QString::number(j) + "/name", "Google").toString() != "sielonullptr") {
				m_tabs->createWebTab(SMainWindow::SSettings->value(QString::number(i) + "/" + QString::number(j) + "/name", "Google").toString(),
									 SMainWindow::SSettings->value(QString::number(i) + "/" + QString::number(j) + "/url", "http://feldrise.com").toUrl());
			}
        }

        m_tabs->setCurrentIndex(SMainWindow::SSettings->value(QString::number(i) + "/focused", 0).toInt());
        m_splitter->addWidget(m_tabs);
    }
	SMainWindow::SSettings->endGroup();
}

void SMainWindow::saveTabs()
{
    if (!privateBrowsing) {
        SMainWindow::SSettings->beginGroup("windowSave/tabs");
        SMainWindow::SSettings->remove("");
        SMainWindow::SSettings->setValue("count", m_splitter->count());

        for (int i{ 0 }; i < m_splitter->count(); ++i) {
            m_tabs = static_cast<STabWidget*>(m_splitter->widget(i));

            for(int j{ 0 }; j < m_tabs->count(); ++j) {
                SMainWindow::SSettings->setValue(QString::number(i) + "/focused", m_tabs->currentIndex());
                SMainWindow::SSettings->setValue(QString::number(i) + "/count", m_tabs->count());
                m_tabs->setCurrentIndex(j);

				if (currentPage()) {
					SMainWindow::SSettings->setValue(QString::number(i) + "/" + QString::number(j) + "/name", currentPage()->title());
					SMainWindow::SSettings->setValue(QString::number(i) + "/" + QString::number(j) + "/url", currentPage()->url());
					SMainWindow::SSettings->endGroup();

					if (!SMainWindow::SSettings->value("preferences/enableCookies", true).toBool())
						currentPage()->page()->profile()->cookieStore()->deleteAllCookies();

					SMainWindow::SSettings->beginGroup("windowSave/tabs");
				}
				else {
					SMainWindow::SSettings->setValue(QString::number(i) + "/" + QString::number(j) + "/name", "sielonullptr");
					SMainWindow::SSettings->setValue(QString::number(i) + "/" + QString::number(j) + "/url", "sielonullptr");
				}
            }
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

void SMainWindow::createTab(QWidget * widget, const QString & title)
{
	m_tabs->addTab(widget, title);
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
