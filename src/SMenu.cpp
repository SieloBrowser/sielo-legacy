#include "../includes/SMenu.hpp"
#include "../includes/SMainWindow.hpp"
#include "../includes/Actions.hpp"

#include <QFileDialog>
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

}

void SMenu::createShowMenu()
{
}

void SMenu::createBrowsMenu()
{

}

void SMenu::createEditMenu()
{
}

void SMenu::createAboutMenu()
{
}

void SMenu::reset()
{
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
