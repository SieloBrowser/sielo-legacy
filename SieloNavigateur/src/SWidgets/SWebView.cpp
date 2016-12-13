#include "includes/SWidgets/SWebView.hpp"
#include "includes/SWidgets/STabWidget.hpp"
#include "includes/SMainWindow.hpp"

#include <QMessageBox>
#include <QVBoxLayout>

SWebView::SWebView(QWidget * parent, STabWidget *parentTab, QUrl url) :
	QWebEngineView(parent),
	m_parent(parent),
	m_parentTab(parentTab)
{
	connect(this->page(), &QWebEnginePage::fullScreenRequested, this, &SWebView::setFullScreen);

	if (parentTab == nullptr)
		connect(this->page(), &QWebEnginePage::windowCloseRequested, this, &SWebView::close);

	load(url);
}

SWebView::~SWebView()
{
	// Empty
}

void SWebView::changeParent(QWidget * parent, STabWidget *parentTab)
{
	setParent(parent);
	m_parent = parent;
	m_parentTab = parentTab;
}

void SWebView::contextMenuEvent(QContextMenuEvent *event)
{
	// Get the action of current context menu
	QMenu *contextMenu{ page()->createStandardContextMenu() };

	// If the user right click on a link, we add the "open in new tab" action 
	if (!page()->contextMenuData().linkUrl().isEmpty()) {
		contextMenu->insertAction(page()->action(QWebEnginePage::OpenLinkInThisWindow), page()->action(QWebEnginePage::OpenLinkInNewTab));
	}

	// Execute the context menu
	contextMenu->exec(event->globalPos());
}

SWebView * SWebView::createWindow(QWebEnginePage::WebWindowType type)
{
	// If we want to create a complet browser window
	if (type == QWebEnginePage::WebBrowserWindow) {
		SWebView *newView{ new SWebView(nullptr) };
		SMainWindow *newWindow{ new SMainWindow(nullptr, newView) };
        newWindow->show();
		return newView;
	} // If we want to create a simple window with juste the web view
	else if (type == QWebEnginePage::WebDialog) {
		SWebView *newView{ new SWebView(nullptr) };
		newView->show();
		return newView;
	} // If we want to create a new tab, but no focused
	else if (type == QWebEnginePage::WebBrowserBackgroundTab) {
		SWebView *view{ new SWebView(nullptr, m_parentTab) };
		m_parentTab->createWebTab(tr("Nouvel onglet"), view);
		return view;
	} // If we want to create a new tab
	else if (type == QWebEnginePage::WebBrowserTab) {
		SWebView *view{ new SWebView(nullptr, m_parentTab) };
		m_parentTab->createWebTab(tr("Nouvel onglet"), view);
        m_parentTab->setCurrentIndex(m_parentTab->count() - 1);
		return view;
	}

	return nullptr;
}

void SWebView::setFullScreen(QWebEngineFullScreenRequest request)
{
	// If it's not in full screen
	if(!m_fullScreen) {
		setParent(nullptr);
		showFullScreen();
		m_parent->layout()->removeWidget(this);
		m_fullScreen = true;
	} // If it's already in full screen
	else {
		setParent(m_parent);
		show();
		m_parent->layout()->addWidget(this);
		m_fullScreen = false;
	}

	request.accept();
}
