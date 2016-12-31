#include "includes/SWidgets/SMenu.hpp"
#include "includes/SWindows/SBookmarks.hpp"
#include "includes/SWindows/SHistory.hpp"
#include "includes/SWindows/SHtmlSrcViewver.hpp"
#include "includes/SWindows/SPreferences.hpp"
#include "includes/SMainWindow.hpp"
#include "includes/SActions.hpp"
#include "includes/SApplication.hpp"

#include <QFileDialog>
#include <QInputDialog>
#include <QFileInfo>
#include <QPixmap>
#include <QEventLoop>

AboutSN::AboutSN(QWidget * parent) :
	QDialog(parent)
{
	setModal(true);

	// The HTML text for the label
	QString htmlTxt{ 
        tr("<h2>Sielo <img src=\"") + SMainWindow::dataPath + tr("Images/icon.ico\" width=\"64\" height=\"64\"/> Navigateur (0.4.1)</h2>"
		"<p>"
			"Qu'est-ce que c'est ? <br/>"
			"Sielo est un navigateur web léger, performant, très personnalisable et<br/>"
			"<a href=\"https://github.com/Feldrise/Sielo-NavigateurV3-WebEngine\">open source</a>.<br/>"
			"Il supporte plusieurs fonctionnalités de base :"
			"<ul>"
				"<li>La navigation par onglet,</li>"
				"<li>Vous pouvez créer plusieurs éspaces d'onglets dans la même fenêtre</li>"
				"<li>Une très grande personnalisation grâce aux thèmes,</li>"
				"<li>L'enregistrement de marque-pages/favoris,</li>"
				"<li>L'enregistrement de l'historique et la navigation privée,</li>"
				"<li>La possibilité de voir le code source d'une page web,</li>"
				"<li>La possibilité de paramétrer Sielo (Page d'accueil, Cookies, Plugins...),</li>"
				"<li>Et plein d'autres fonctionnalités encore.</li>"
			"</ul>"
		"</p>"
		"<p>"
			"Ce navigateur est développé par <a href=\"https://feldrise.com\">Feldrise</a>, et est en évolution constante.<br/>"
			"Il est disponible sous <a href=\"https://opensource.org/licenses/MIT\">licence MIT</a>, et fait avec <a href=\"https://www.qt.io/\">Qt 5.7</a> en C++ 14. N'oubliez<br/>"
			"pas de faire régulièrement les mises à jour pour profiter de nos dernières<br/>"
			"fonctionnalités et corrections ! Vous pouvez retrouver les auteurs des icones<br/>"
			"dans le fichier à la racine du thème."
		"</p>" )
	};

	// Set text and icons
	m_txt->setText(htmlTxt);
	m_SNIcon->setPixmap(QPixmap(SMainWindow::dataPath + "Images/icon.ico"));

	// Fill layouts
	m_txtLayout->addWidget(m_SNIcon, 0, 0);
	m_txtLayout->addItem(m_spacer, 1, 0);
	m_txtLayout->addWidget(m_txt, 0, 1, 1, 2);

	m_layout->addLayout(m_txtLayout);
	m_layout->addWidget(m_boxBtn);

	// Connections
	connect(m_boxBtn, &QDialogButtonBox::accepted, this, &AboutSN::accept);
}

AboutSN::~AboutSN()
{
	// Empty
}

SMenu::SMenu(SMainWindow * parent, const QString & name, SMenuType menuType) :
	QMenu(name, parent),
	m_parent(parent),
	m_actions(parent->getActions())
{
	// Choose the menu to build
	switch (menuType)
	{
	case SMenuType::File:
		createFileMenu();
		break;
	case SMenuType::Show:
		createShowMenu();
		break;
	case SMenuType::Brows:
		createBrowsMenu();
		break;
	case SMenuType::Fav:
		createBookmarksMenu();
		break;
    case SMenuType::Theme:
        createThemeMenu();
        break;
	case SMenuType::Edit:
		createEditMenu();
		break;
	case SMenuType::About:
		createAboutMenu();
		break;
	default:
		break;
	}

}

SMenu::~SMenu()
{
}

void SMenu::createFileMenu()
{
	// Set parents for actions
	m_actions->newTab->setParent(this);
	m_actions->newWindow->setParent(this);
	m_actions->openFile->setParent(this);
	m_actions->exit->setParent(this);

	// Connect actions to their respective slot
	connect(m_actions->newTab, &QAction::triggered, m_parent->getTabs(), &STabWidget::createDefaultWebTab);
	connect(m_actions->newWindow, &QAction::triggered, this, &SMenu::createNewWindows);
	connect(m_actions->openFile, &QAction::triggered, this, &SMenu::openFile);
	connect(m_actions->exit, &QAction::triggered, m_parent, &SMainWindow::close);

	// Add action to the menu
	addAction(m_actions->newTab);
	addAction(m_actions->newWindow);
	addAction(m_actions->openFile);
	addSeparator();
	addAction(m_actions->exit);

	// Set shortcut for all actions from this menu
	m_actions->newTab->setShortcut(QKeySequence::AddTab);
	m_actions->newWindow->setShortcut(QKeySequence("Ctrl+N"));
	m_actions->openFile->setShortcut(QKeySequence::Open);
	m_actions->exit->setShortcut(QKeySequence::Quit);
}

void SMenu::createShowMenu()
{
	// Set parents for actions
	m_actions->showFullScreen->setParent(this);
    m_actions->split->setParent(this);
	m_actions->showMenuBar->setParent(nullptr);
	m_actions->findInPage->setParent(this);
	m_actions->findNext->setParent(this);
	m_actions->findPrevious->setParent(this);
	m_actions->zoomMore->setParent(this);
	m_actions->zoomLess->setParent(this);

	// Connect actions to their respective slot
	connect(m_actions->showFullScreen, &QAction::triggered, m_parent, &SMainWindow::fullScreen);
    connect(m_actions->split, &QAction::triggered, this, &SMenu::split);
	connect(m_actions->showMenuBar, &QAction::triggered, this, &SMenu::showMenuBar);
	connect(m_actions->findInPage, &QAction::triggered, this, &SMenu::findInPage);
	connect(m_actions->findNext, &QAction::triggered, this, &SMenu::findInPage);
	connect(m_actions->findPrevious, &QAction::triggered, this, &SMenu::findInPage);
	connect(m_actions->zoomMore, &QAction::triggered, this, &SMenu::zoom);
	connect(m_actions->zoomLess, &QAction::triggered, this, &SMenu::zoom);

	// Add actions to the menu
	addAction(m_actions->showFullScreen);
	addAction(m_actions->showMenuBar);
	addSeparator();
    addAction(m_actions->split);
    addSeparator();
	addAction(m_actions->findInPage);
	addAction(m_actions->findNext);
	addAction(m_actions->findPrevious);
	addSeparator();
	addAction(m_actions->zoomMore);
	addAction(m_actions->zoomLess);

	// Set shortcut for all actions from this menu
	m_actions->showFullScreen->setShortcut(QKeySequence("F11"));
	m_actions->showMenuBar->setShortcut(QKeySequence("Alt+B"));
	m_actions->findInPage->setShortcut(QKeySequence::Find);
	m_actions->findNext->setShortcut(QKeySequence::FindNext);
	m_actions->findPrevious->setShortcut(QKeySequence::FindPrevious);
	m_actions->zoomMore->setShortcut(QKeySequence::ZoomIn);
	m_actions->zoomLess->setShortcut(QKeySequence::ZoomOut);
}

void SMenu::createBrowsMenu()
{
	// Set parents for actions
	m_actions->back->setParent(this);
	m_actions->next->setParent(this);
	m_actions->home->setParent(this);
	m_actions->refreshOrStop->setParent(this);
	m_actions->history->setParent(this);
	m_actions->privateBrowsing->setParent(this);
	m_actions->viewPageCodeSource->setParent(this);

	// Connect actions to their respective slot
	connect(m_actions->back, &QAction::triggered, m_parent, &SMainWindow::back);
	connect(m_actions->next, &QAction::triggered, m_parent, &SMainWindow::next);
	connect(m_actions->home, &QAction::triggered, m_parent, &SMainWindow::home);
	connect(m_actions->refreshOrStop, &QAction::triggered, m_parent, &SMainWindow::refresh);
	connect(m_actions->history, &QAction::triggered, this, &SMenu::showHistory);
	connect(m_actions->privateBrowsing, &QAction::triggered, this, &SMenu::privateBrowsing);
	connect(m_actions->viewPageCodeSource, &QAction::triggered, this, &SMenu::showPageSrc);

	connect(m_actions->shearch, &QAction::triggered, m_parent->getSearchArea(), &SSearchArea::loadSearch);
	connect(m_actions->go, &QAction::triggered, m_parent->getUrlArea(), &SUrlArea::loadUrl);

	// Add actions to the menu
	addAction(m_actions->back);
	addAction(m_actions->next);
	addAction(m_actions->home);
	addAction(m_actions->refreshOrStop);
	addAction(m_actions->history);
	addAction(m_actions->privateBrowsing);
	addAction(m_actions->viewPageCodeSource);

	// Set shortcut for all actions from this menu
	m_actions->back->setShortcut(QKeySequence::Back);
	m_actions->next->setShortcut(QKeySequence::Forward);
	m_actions->history->setShortcut(QKeySequence("Ctrl+H"));
	m_actions->privateBrowsing->setShortcut(QKeySequence("Ctrl+Shift+P"));
	m_actions->viewPageCodeSource->setShortcut(QKeySequence("Ctrl+U"));
}

void SMenu::createDlMenu()
{
	// Empty
}

void SMenu::createBookmarksMenu()
{
	// Create the view for know bookmarks
	m_bView = new SBookmarksView(this);
	m_bView->setAttribute(Qt::WA_DeleteOnClose);

	// Connect actions to their respective slot
	connect(m_actions->addBookmarks, &QAction::triggered, this, &SMenu::addBookmark);
	connect(m_actions->bookmarksManager, &QAction::triggered, this, &SMenu::openBookmarksManager);

	// Add actions in the menu
	addAction(m_actions->addBookmarks);
	addAction(m_actions->bookmarksManager);
	addSeparator();

	// create the bookmarks menu
	for(int i{ 0 }; i < m_bView->getModel()->rowCount(); ++i)
		createBookmarksItem(m_bView->getModel()->item(i), this);

	m_bView->close();
	m_bView = nullptr;
 
	// Set shortcut for all actions from this menu
	m_actions->addBookmarks->setShortcut(QKeySequence("Ctrl+D"));
	m_actions->bookmarksManager->setShortcut(QKeySequence("Ctrl+B"));
}

void SMenu::createThemeMenu()
{
    m_themesMenu = new QMenu(tr("Thèmes"));

    connect(m_themesMenu, &QMenu::triggered, this, &SMenu::changeTheme);
    connect(m_actions->addTheme, &QAction::triggered, this, &SMenu::addTheme);
    connect(m_actions->openThemeEditor, &QAction::triggered, this, &SMenu::openThmEditor);

    for(int i{ 0 }; i < SMainWindow::SSettings->value("preferences/themes/nbre", 1).toInt(); ++i) {
        QAction *theme{ new QAction(SMainWindow::SSettings->value("preferences/themes/" + QString::number(i) + "/name").toString()) };
        theme->setData(i);
        if(SMainWindow::SSettings->value("preferences/themes/" + QString::number(i) + "/current", false).toBool())
            theme->setEnabled(false);
        m_themesMenu->addAction(theme);
    }

    addMenu(m_themesMenu);
    addAction(m_actions->addTheme);
    addSeparator();
    addAction(m_actions->openThemeEditor);


}

void SMenu::createEditMenu()
{
	// Set parents for the actions
	m_actions->preferences->setParent(this);

	// Connect actions to their respective slots
	connect(m_actions->preferences, &QAction::triggered, this, &SMenu::openPreferencesDialog);

	// Add actions to the menu
	addAction(m_actions->preferences);

	// Set shortcut for all actionss from this menu
	m_actions->preferences->setShortcut(QKeySequence("Ctrl+P"));
}

void SMenu::createAboutMenu()
{
	// Set parents and icons for actions
	m_actions->aboutQt->setParent(this);
	m_actions->aboutQt->setIcon(QIcon(SMainWindow::dataPath + "/Images/QtIcon.png"));
	m_actions->aboutSielo->setParent(this);
	m_actions->aboutSielo->setIcon(QIcon(SMainWindow::dataPath + "/Images/icon.ico"));
	m_actions->checkMaJ->setParent(this);
	m_actions->checkMaJ->setIcon(QIcon(SMainWindow::dataPath + "Images/icon2.png"));

	// Connect actions to their respective slot
	connect(m_actions->aboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
	connect(m_actions->aboutSielo, &QAction::triggered, this, &SMenu::openAboutSielo);
	connect(m_actions->checkMaJ, &QAction::triggered, this, &SMenu::checkMaJ);

	// Add actions to the menu
	addAction(m_actions->aboutQt);
	addAction(m_actions->aboutSielo);
	addSeparator();
	addAction(m_actions->checkMaJ);
}

void SMenu::reset()
{
	clear(); //< Totally clear the menu
}

void SMenu::createNewWindows()
{
    SWebView *view{ new SWebView(nullptr, nullptr, SMainWindow::SSettings->value("preferences/homePage", "http://google.com").toUrl()) };
    SMainWindow* newWindows{ new SMainWindow(nullptr, view, false) };
	newWindows->show();
}

void SMenu::openFile()
{
	// Open a dialog to get the file path
	QString filePath = QFileDialog::getOpenFileName(this, tr("Ouvrir un fichier"), QString(), tr("Web Resources (*.html *.htm *txt *svg *png *gif *svgz)"));
	if (filePath.isEmpty())
		return;

	// Just for get the file name
	QFileInfo fileInfo{ filePath };

	// Create the new tab
	filePath = "file:///" + filePath;
	m_parent->getTabs()->createWebTab(fileInfo.fileName(), QUrl(filePath));
    m_parent->getTabs()->setCurrentIndex(m_parent->getTabs()->count() - 1);
}

void SMenu::split()
{
    STabWidget *newTabs{ new STabWidget(m_parent) };
    newTabs->createDefaultWebTab();
    m_parent->setTabs(newTabs);
    m_parent->getSplitter()->addWidget(newTabs);
}

void SMenu::showMenuBar()
{
	if (!m_parent->menuBar()->isVisible()) {
		m_parent->menuBar()->setVisible(true);
		m_actions->showMenuBar->setText(tr("Afficher la barre de menu"));
		SMainWindow::SSettings->setValue("preferences/showMenuBar", true);
	}
	else {
		m_parent->menuBar()->setVisible(false);
		m_actions->showMenuBar->setText(tr("Cacher la barre de menu"));
		SMainWindow::SSettings->setValue("preferences/showMenuBar", false);
	}
}

void SMenu::findInPage()
{
	// Get back the action witch sent the signal
	QAction* action{ qobject_cast<QAction*>(sender()) };

	if (action == nullptr)
		return;

	// If the user want to find a word
	if (action == m_actions->findInPage) {
		QString word2find{ QInputDialog::getText(this, tr("Mot à trouver"), tr("Quel est le mot que vous recherchez ?"), QLineEdit::Normal, nullptr) };
		if (word2find.isEmpty())
			return;

		m_parent->currentPage()->findText(word2find);
		m_actions->findInPage->setObjectName(word2find);
	} // If the user want to find the next word in the page
	else if (action == m_actions->findNext)
		m_parent->currentPage()->findText(m_actions->findInPage->objectName());
	// If the user want to find the previous word in the page
	else if (action == m_actions->findPrevious)
		m_parent->currentPage()->findText(m_actions->findInPage->objectName(), QWebEnginePage::FindBackward);
	else
		QMessageBox::critical(this, tr("Erreur"), tr("Une erreur c'est produite. Veuillez envoyer un rapport au développeur de ce navigateur"));
}

void SMenu::zoom()
{
	// Get back the action witch sent the signal
	QAction* action{ qobject_cast<QAction*>(sender()) };

	if (action == nullptr)
		return;

	if (action == m_actions->zoomMore)
		m_parent->currentPage()->setZoomFactor(m_parent->currentPage()->zoomFactor() + 0.1);
	else if (action == m_actions->zoomLess)
		m_parent->currentPage()->setZoomFactor(m_parent->currentPage()->zoomFactor() - 0.1);
	else
		QMessageBox::critical(this, tr("Erreur"), tr("Une erreur c'est produite. Veuillez envoyé un rapport au développeur de ce navigateur"));
}

void SMenu::showHistory()
{
	SHistoryWindow* historyWin{ new SHistoryWindow(m_parent) };
	connect(historyWin, &SHistoryWindow::close, historyWin, &SHistoryWindow::deleteLater);
	if (SMainWindow::SSettings->value("preferences/openWidgetInTab", true).toBool()) {
		m_parent->createTab(historyWin, tr("Favoris"));
		m_parent->getTabs()->setCurrentWidget(historyWin);
	}
	else
		historyWin->show();
}

void SMenu::privateBrowsing()
{
	SMainWindow *privateBrowsingWindow{ new SMainWindow(nullptr, nullptr, true) };
	privateBrowsingWindow->show();
}

void SMenu::showPageSrc()
{
	SHtmlSrcViewver* srcViewver{ new SHtmlSrcViewver(m_parent) };
	if (SMainWindow::SSettings->value("preferences/openWidgetInTab", true).toBool()) {
		m_parent->createTab(srcViewver, tr("Source de la page ").arg(m_parent->currentPage()->title()));
		m_parent->getTabs()->setCurrentWidget(srcViewver);
	}
	else
		srcViewver->show();
}

void SMenu::openBookmarksManager()
{
	SBookmarksDialog *bookmarksDialog{ new SBookmarksDialog(m_parent) };
	if (SMainWindow::SSettings->value("preferences/openWidgetInTab", true).toBool()) {
		m_parent->createTab(bookmarksDialog, tr("Favoris"));
		m_parent->getTabs()->setCurrentWidget(bookmarksDialog);
	}
	else
		bookmarksDialog->show();
}

void SMenu::openBookmark()
{
	QAction *bookmarkAction{ static_cast<QAction*>(sender()) };

	m_parent->getTabs()->createWebTab(bookmarkAction->text(), bookmarkAction->data().toUrl());
    m_parent->getTabs()->setCurrentIndex(m_parent->getTabs()->count() - 1);
}

void SMenu::addBookmark()
{
	if (m_parent->currentPage()) {
		SBookmarksAddDialog *addBookmarkWin{ new SBookmarksAddDialog(m_parent) };
		addBookmarkWin->show();
	}
}

void SMenu::createBookmarksItem(QStandardItem *item, SMenu *parent)
{
	// Get the type of item
	QString tagName{ item->data(Qt::UserRole).toString() };

	// If the item is a folder
	if(tagName == "folder") {
		QMenu *folder{ new QMenu(item->text(), this) };
		folder->setIcon(m_bView->getFolderIcon());
		for(int i{ 0 }; i < m_bView->getModel()->rowCount(item->index()); ++i)
			createBookmarksItem(item->child(i), static_cast<SMenu*>(folder));
		parent->addMenu(folder);
	} // If the item is a separator
	else if(tagName == "separator") {
		parent->addSeparator();
	} // If the item is a bookmark
	else if(tagName == "bookmark") {
		QAction *bookmarkAction{ new QAction(item->icon(), item->text(), parent) };
		bookmarkAction->setData(m_bView->getModel()->itemFromIndex(item->index())->parent()->child(item->row(), 1)->text());
		parent->addAction(bookmarkAction);
		connect(bookmarkAction, &QAction::triggered, this, &SMenu::openBookmark);
	}
}

void SMenu::changeTheme(QAction *theme)
{
    if(!SMainWindow::SSettings->value("preferences/themes/" + QString::number(theme->data().toInt()) + "/current", false).toBool()) {
        // Save value for apply theme
        SMainWindow::SSettings->beginGroup("preferences/themes/");
        SMainWindow::SSettings->setValue(SMainWindow::SSettings->value("currentTheme", "0").toString() + "/current", false);
        SMainWindow::SSettings->setValue(QString::number(theme->data().toInt()) + "/current", true);
        SMainWindow::SSettings->setValue("currentTheme", theme->data().toInt());
        SMainWindow::SSettings->setValue("changed", true);
        SMainWindow::SSettings->endGroup();

        // Save windows state for restoration
        m_parent->saveTabs();
        m_parent->saveWinState();

        // Open a new window and close current window
        SMainWindow *windows{ new SMainWindow() };
        windows->show();
        m_parent->close();
    }

}

void SMenu::addTheme()
{
    // Get the path of the theme
    QString path{ QFileDialog::getOpenFileName(m_parent, "Choisir un thème", QString(), "Sielo Navigateur Themes(*.snthm)") };

    // if path is empty, we don't add a new theme
    if(path.isEmpty())
        return;

    // Info about the new theme
    QFileInfo themeInfo{ path };
    int index{ SMainWindow::SSettings->value("preferences/themes/nbre", 1).toInt() };

    // Uncompress the new theme in the theme directory
    QStringList args{};
    args << "decompress" << path << SMainWindow::dataPath + "Themes/" + themeInfo.baseName();
    QProcess::execute(QDir(QCoreApplication::applicationDirPath()).absolutePath() + "/SieloDataSoftware", args);
    QMessageBox::information(m_parent, tr("Info"), tr("Le thème ") + themeInfo.baseName() + tr(" va être ajouté (patientez quelques instants s'il vous plait)"));

	// TODO: Create generic function to test if valu existe in setting
	bool themeExiste{ false };
	for (int i{ 0 }; i < SMainWindow::SSettings->value("preferences/themes/nbre", 1).toInt(); ++i) {
		SMainWindow::SSettings->beginGroup("preferences/themes/" + QString::number(i));
		if (SMainWindow::SSettings->value("name", "").toString() == themeInfo.baseName()) {
			themeExiste = true;
			SMainWindow::SSettings->endGroup();
			// Save windows state for restoration
			m_parent->saveTabs();
			m_parent->saveWinState();

			// Open a new window and close current window
			SMainWindow *windows{ new SMainWindow() };
			windows->show();
			m_parent->close();
			return;
		}
		SMainWindow::SSettings->endGroup();
	}
	if (!themeExiste) {
		// Update setting
		SMainWindow::SSettings->beginGroup("preferences/themes/");
		SMainWindow::SSettings->setValue("nbre", index);
		SMainWindow::SSettings->setValue(QString::number(index) + "/name/", themeInfo.baseName());
		SMainWindow::SSettings->setValue("nbre", index + 1);
		SMainWindow::SSettings->endGroup();
	}

    // Add the new theme in the theme menu
    QAction *newTheme{ new QAction(themeInfo.baseName()) };
    newTheme->setData(index);
    m_themesMenu->addAction(newTheme);
    QMessageBox::information(m_parent, tr("Fin"), tr("Le thème à bien été ajouté"));
}

void SMenu::openThmEditor()
{
    QProcess::execute(QDir(QCoreApplication::applicationDirPath()).absolutePath() + "/SNThemeEditor");
}

void SMenu::openPreferencesDialog()
{
	SPreferencesWindow *preferences{ new SPreferencesWindow(m_parent) };
	if (SMainWindow::SSettings->value("preferences/openWidgetInTab", true).toBool()) {
		m_parent->createTab(preferences, tr("Paramètres de Sielo"));
		m_parent->getTabs()->setCurrentWidget(preferences);
	}
	else
		preferences->show();
}

void SMenu::openAboutSielo()
{
	AboutSN *aboutSN{ new AboutSN(m_parent) };
	if (SMainWindow::SSettings->value("preferences/openWidgetInTab", true).toBool()) {
		m_parent->createTab(aboutSN, tr("A propos de Sielo"));
		m_parent->getTabs()->setCurrentWidget(aboutSN);
	}
	else
		aboutSN->show();
}

void SMenu::checkMaJ()
{
	// Networks items to download the last version
	QNetworkAccessManager manager{};
	QNetworkReply *reply{ manager.get(QNetworkRequest(QUrl("http://feldrise.com/Sielo/version.txt"))) };

	// Loop during the download of the last verson
	QEventLoop loop{};
	connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
	loop.exec();

	// New version
    QString version{ "0.4.1" };
	version = reply->readAll();

    if (version != mApp->currentVersion) {
		MaJDialog *majDialog{ new MaJDialog(m_parent) };
		majDialog->show();
	}
	else {
		QMessageBox::information(m_parent, tr("Aucune mise à jour"), tr("Vous possédez la dernière version officielle de Sielo."));
	}
}
