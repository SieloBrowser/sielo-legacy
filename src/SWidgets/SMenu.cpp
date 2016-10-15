#include "includes/SWidgets/SMenu.hpp"
#include "includes/SWindows/SBookmarks.hpp"
#include "includes/SWindows/SHistory.hpp"
#include "includes/SWindows/SHtmlSrcViewver.hpp"
#include "includes/SWindows/SPreferences.hpp"
#include "includes/SMainWindow.hpp"
#include "includes/SActions.hpp"

#include <QFileDialog>
#include <QInputDialog>
#include <QFileInfo>

SMenu::SMenu(SMainWindow * parent, const QString & name, SMenuType menuType) :
    QMenu(name, parent),
    m_parent(parent),
    m_actions(parent->getActions())
{
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
    m_actions->newTab->setParent(this);
    m_actions->newWindow->setParent(this);
    m_actions->openFile->setParent(this);
    m_actions->exit->setParent(this);

    connect(m_actions->newTab, &QAction::triggered, m_parent->getTabs(), &STabWidget::createDefaultWebTab);
    connect(m_actions->newWindow, &QAction::triggered, this, &SMenu::createNewWindows);
    connect(m_actions->openFile, &QAction::triggered, this, &SMenu::openFile);
    connect(m_actions->exit, &QAction::triggered, m_parent, &SMainWindow::close);

    addAction(m_actions->newTab);
    addAction(m_actions->newWindow);
    addAction(m_actions->openFile);
    addSeparator();
    addAction(m_actions->exit);

    SThemeManager *manager{ new SThemeManager(this) };
//    manager->compressTheme("/home/feldrise/Documents/C++/Projects/Qt/Sielo-NavigateurV3-WebEngine/Themes/SIcons", "/home/feldrise/testeTheme.stm");
    manager->decompressTheme("/home/feldrise/SIcones.stm");

}

void SMenu::createShowMenu()
{
    m_actions->showFullScreen->setParent(this);
    m_actions->findInPage->setParent(this);
    m_actions->findNext->setParent(this);
    m_actions->findPrevious->setParent(this);
    m_actions->zoomMore->setParent(this);
    m_actions->zoomLess->setParent(this);

    connect(m_actions->showFullScreen, &QAction::triggered, m_parent, &SMainWindow::fullScreen);
    connect(m_actions->findInPage, &QAction::triggered, this, &SMenu::findInPage);
    connect(m_actions->findNext, &QAction::triggered, this, &SMenu::findInPage);
    connect(m_actions->findPrevious, &QAction::triggered, this, &SMenu::findInPage);
    connect(m_actions->zoomMore, &QAction::triggered, this, &SMenu::zoom);
    connect(m_actions->zoomLess, &QAction::triggered, this, &SMenu::zoom);

    addAction(m_actions->showFullScreen);
    addSeparator();
    addAction(m_actions->findInPage);
    addAction(m_actions->findNext);
    addAction(m_actions->findPrevious);
    addSeparator();
    addAction(m_actions->zoomMore);
    addAction(m_actions->zoomLess);
}

void SMenu::createBrowsMenu()
{
    m_actions->back->setParent(this);
    m_actions->next->setParent(this);
    m_actions->home->setParent(this);
    m_actions->refreshOrStop->setParent(this);
    m_actions->history->setParent(this);
    m_actions->privateBrowsing->setParent(this);
    m_actions->downloads->setParent(this);
    m_actions->viewPageCodeSource->setParent(this);

    connect(m_actions->back, &QAction::triggered, m_parent, &SMainWindow::back);
    connect(m_actions->next, &QAction::triggered, m_parent, &SMainWindow::next);
    connect(m_actions->home, &QAction::triggered, m_parent, &SMainWindow::home);
    connect(m_actions->refreshOrStop, &QAction::triggered, m_parent, &SMainWindow::refresh);
    connect(m_actions->history, &QAction::triggered, this, &SMenu::showHistory);
    connect(m_actions->privateBrowsing, &QAction::triggered, this, &SMenu::privateBrowsing);
    connect(m_actions->viewPageCodeSource, &QAction::triggered, this, &SMenu::showPageSrc);

    connect(m_actions->shearch, &QAction::triggered, m_parent->getSearchArea(), &SSearchArea::loadSearch);
    connect(m_actions->go, &QAction::triggered, m_parent->getUrlArea(), &SUrlArea::loadUrl);

    addAction(m_actions->back);
    addAction(m_actions->next);
    addAction(m_actions->home);
    addAction(m_actions->refreshOrStop);
    addAction(m_actions->history);
    addAction(m_actions->privateBrowsing);
    addAction(m_actions->downloads);
    addAction(m_actions->viewPageCodeSource);
}

void SMenu::createDlMenu()
{

}

void SMenu::createBookmarksMenu()
{
    m_bView = new SBookmarksView(this);
    m_bView->setAttribute(Qt::WA_DeleteOnClose);

    connect(m_actions->addBookmarks, &QAction::triggered, this, &SMenu::addBookmark);
    connect(m_actions->bookmarksManager, &QAction::triggered, this, &SMenu::openBookmarksManager);

    addAction(m_actions->addBookmarks);
    addAction(m_actions->bookmarksManager);
    addSeparator();

    for(int i{ 0 }; i < m_bView->getModel()->rowCount(); ++i)
        createBookmarksItem(m_bView->getModel()->item(i), this);


    m_bView->close();
    m_bView = nullptr;
}

void SMenu::createEditMenu()
{
    m_actions->preferences->setParent(this);

    connect(m_actions->preferences, &QAction::triggered, this, &SMenu::openPreferencesDialog);

    addAction(m_actions->preferences);
}

void SMenu::createAboutMenu()
{
}

void SMenu::reset()
{
    clear();
}

void SMenu::createNewWindows()
{
    SMainWindow* newWindows{ new SMainWindow };
    newWindows->show();
}

void SMenu::openFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Ouvrir un fichier", QString(), "Web Resources (*.html *.htm *txt *svg *png *gif *svgz)");
    if (filePath.isEmpty())
        return;

    QFileInfo fileInfo{ filePath };

    filePath = "file:///" + filePath;
    m_parent->getTabs()->createWebTab(fileInfo.fileName(), QUrl(filePath));
    m_parent->getTabs()->createPlusTab();
    m_parent->getTabs()->removeTab(m_parent->getTabs()->count() - 3);
}

void SMenu::findInPage()
{
    QAction* action{ qobject_cast<QAction*>(sender()) };

    if (action == nullptr)
        return;

    if (action == m_actions->findInPage) {
        QString word2find{ QInputDialog::getText(this, "Mot(s) à trouver", "Quel(s) est(sont) le(s) mot(s) que vous recherchez ?", QLineEdit::Normal, nullptr) };
        if (word2find.isEmpty())
            return;

        m_parent->currentPage()->findText(word2find);
        m_actions->findInPage->setObjectName(word2find);
    }
    else if (action == m_actions->findNext)
        m_parent->currentPage()->findText(m_actions->findInPage->objectName());
    else if (action == m_actions->findPrevious)
        m_parent->currentPage()->findText(m_actions->findInPage->objectName(), QWebEnginePage::FindBackward);
    else
        QMessageBox::critical(this, "Erreur", "Une erreur c'est produite. Veuillez envoyé un rapport au développeur de ce navigateur");
}

void SMenu::zoom()
{
    QAction* action{ qobject_cast<QAction*>(sender()) };

    if (action == nullptr)
        return;

    if (action == m_actions->zoomMore)
        m_parent->currentPage()->setZoomFactor(m_parent->currentPage()->zoomFactor() + 0.1);
    else if (action == m_actions->zoomLess)
        m_parent->currentPage()->setZoomFactor(m_parent->currentPage()->zoomFactor() - 0.1);
    else
        QMessageBox::critical(this, "Erreur", "Une erreur c'est produite. Veuillez envoyé un rapport au développeur de ce navigateur");
}

void SMenu::showHistory()
{
    SHistoryWindow* historyWin{ new SHistoryWindow(m_parent) };
    connect(historyWin, &SHistoryWindow::close, historyWin, &SHistoryWindow::deleteLater);
    historyWin->show();
}

void SMenu::privateBrowsing()
{
    SMainWindow *privateBrowsingWindow{ new SMainWindow{} };
    privateBrowsingWindow->privateBrowsing = true;
    privateBrowsingWindow->show();
}

void SMenu::showPageSrc()
{
    SHtmlSrcViewver* srcViewver{ new SHtmlSrcViewver(m_parent) };
    srcViewver->show();
}

void SMenu::openBookmarksManager()
{
    SBookmarksDialog *bookmarksDialog{ new SBookmarksDialog(m_parent) };
    bookmarksDialog->show();
}

void SMenu::openBookmark()
{
    QAction *bookmarkAction{ static_cast<QAction*>(sender()) };

    m_parent->getTabs()->createWebTab(bookmarkAction->text(), bookmarkAction->data().toUrl());
    m_parent->getTabs()->createPlusTab();
    m_parent->getTabs()->removeTab(m_parent->getTabs()->count() - 3);
}

void SMenu::addBookmark()
{
    SBookmarksAddDialog *addBookmarkWin{ new SBookmarksAddDialog(m_parent) };
    addBookmarkWin->show();
}

void SMenu::createBookmarksItem(QStandardItem *item, SMenu *parent)
{
    QString tagName{ item->data(Qt::UserRole).toString() };

    if(tagName == "folder") {
        QMenu *folder{ new QMenu(item->text(), this) };
        folder->setIcon(m_bView->getFolderIcon());
        for(int i{ 0 }; i < m_bView->getModel()->rowCount(item->index()); ++i)
            createBookmarksItem(item->child(i), static_cast<SMenu*>(folder));
        parent->addMenu(folder);
    }
    else if(tagName == "separator") {
        parent->addSeparator();
    }
    else if(tagName == "bookmark") {
        QAction *bookmarkAction{ new QAction(m_bView->getItemIcon(), item->text(), parent) };
        bookmarkAction->setData(m_bView->getModel()->itemFromIndex(item->index())->parent()->child(item->row(), 1)->text());
        parent->addAction(bookmarkAction);
        connect(bookmarkAction, &QAction::triggered, this, &SMenu::openBookmark);
    }
}

void SMenu::openPreferencesDialog()
{
    QMessageBox::information(this, "DEBUG", "Open preferences");
    SPreferencesWindow *preferences{ new SPreferencesWindow(m_parent) };
    preferences->show();
}
