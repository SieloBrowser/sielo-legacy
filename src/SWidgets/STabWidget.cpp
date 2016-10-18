#include "includes/SWidgets/STabWidget.hpp"
#include "includes/SWidgets/SWebView.hpp"
#include "includes/SWidgets/SUrlArea.hpp"
#include "includes/SMainWindow.hpp"

STabWidget::STabWidget(SMainWindow * parent) : 
	QTabWidget(parent),
	m_parent(parent)
{
	setTabsClosable(true);
	setMovable(true);
	connect(this, &STabWidget::currentChanged, this, &STabWidget::tabChanged);
	connect(this, &STabWidget::tabCloseRequested, this, &STabWidget::tabClosed);
}

STabWidget::~STabWidget()
{
	// Empty
}

void STabWidget::createWebTab(QString title, SWebView * view)
{
	QWidget* tabPage{ new QWidget(this) };
	QVBoxLayout* pageLayout{ new QVBoxLayout(tabPage) };
	view->changeParent(tabPage, this);

	pageLayout->addWidget(view);

    connect(view, &SWebView::titleChanged, m_parent, &SMainWindow::changeTitle);
    connect(view, &SWebView::urlChanged, m_parent, &SMainWindow::changeUrl);
	connect(view, &SWebView::loadStarted, m_parent->getUrlArea(), &SUrlArea::loadStarted);
	connect(view, &SWebView::loadProgress, m_parent->getUrlArea(), &SUrlArea::loadInProgress);
    connect(view, &SWebView::loadFinished, m_parent->getUrlArea(), &SUrlArea::loadFinished);
    connect(view->page()->profile(), &QWebEngineProfile::downloadRequested, m_parent, &SMainWindow::addDownload);

	addTab(tabPage, title);

	tabPage->setAttribute(Qt::WA_DeleteOnClose);
	if (!SMainWindow::SSettings->value("preferences/enableCookies", true).toBool())
		view->page()->profile()->cookieStore()->deleteAllCookies();
}

void STabWidget::createWebTab(QString title, QUrl url)
{
	QWidget* tabPage{ new QWidget(this) };
	SWebView* view{ new SWebView(tabPage, this, url) };
	QVBoxLayout* pageLayout{ new QVBoxLayout(tabPage) };

	pageLayout->addWidget(view);

	connect(view, &SWebView::titleChanged, m_parent, &SMainWindow::changeTitle);
	connect(view, &SWebView::urlChanged, m_parent, &SMainWindow::changeUrl);
	connect(view, &SWebView::loadStarted, m_parent->getUrlArea(), &SUrlArea::loadStarted);
	connect(view, &SWebView::loadProgress, m_parent->getUrlArea(), &SUrlArea::loadInProgress);
    connect(view, &SWebView::loadFinished, m_parent->getUrlArea(), &SUrlArea::loadFinished);
    connect(view->page()->profile(), &QWebEngineProfile::downloadRequested, m_parent, &SMainWindow::addDownload);

	addTab(tabPage, title);

	tabPage->setAttribute(Qt::WA_DeleteOnClose);
	if (!SMainWindow::SSettings->value("preferences/enableCookies", true).toBool())
		view->page()->profile()->cookieStore()->deleteAllCookies();
}

void STabWidget::createDefaultWebTab()
{
	createWebTab(tr("Home page"), SMainWindow::SSettings->value("preferences/homePage", "http://google.com").toUrl());
	createPlusTab();
	if (tabBar()->tabButton(count() - 3, QTabBar::RightSide))
		removeTab(count() - 3);
}

void STabWidget::createPlusTab()
{
	createWebTab(tr("+"), QUrl("html/page_blanche"));
	tabBar()->tabButton(count() - 1, QTabBar::RightSide)->resize(0, 0);
}

void STabWidget::tabClosed(int index)
{
	if (index == -1)
		index = currentIndex();

	if (count() > 2) {
		if (index == count() - 2) {
			setCurrentIndex(index - 1);
		}
		widget(index)->deleteLater();
		removeTab(index);
	}
	else
		m_parent->close();
}

void STabWidget::tabChanged(int index)
{
	if (index == count() - 1 && count() > 1) 
		createDefaultWebTab();
	else {
		m_parent->changeTabTitle(m_parent->currentPage()->title());
		m_parent->changeTabUrl(m_parent->currentPage()->url());
	}
}

